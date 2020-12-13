#include "sftpSession.h"
#include "cmdParser.h"
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cmath>

extern errorMgr errMgr;

#define MAX_XFER_BUF_SIZE 32768
#define WAIT_INTERVAL 1000 // in microsecond

static char xferbuf[MAX_XFER_BUF_SIZE];
static char fullpath[PATH_BUF_MAX];

// TODO try using unique pointer

namespace sftp
{

/*************
* GET method *
**************/
// read file to memory in chunks and transmit with sftp_read/sftp_write
// TODO show ETA
sftpStat sftpSession::get(const std::string_view& source, const std::string_view& destination, bool force) const {

    // create c_str for c API
    char* file_src = new char[source.size()+1];
    memcpy( file_src, &source.front(), source.size() );
    file_src[source.size()] = '\0';
    char* file_dst = new char[destination.size()+1];
    memcpy( file_dst, &destination.front(), destination.size() );
    file_dst[destination.size()] = '\0';

    char* nEscFile1 = UTIL::rmEscChar( file_src );
    char* nEscFile2 = UTIL::rmEscChar( file_dst );
    const char* p_file_src = nEscFile1 == NULL ? file_src : nEscFile1;
    const char* p_file_dst = nEscFile2 == NULL ? file_dst : nEscFile2;

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), p_file_src);

    if ( nEscFile1 ) delete [] nEscFile1;

    cout << "Fetching " << fullpath << " to " << p_file_dst << endl;

    const char* __accesstype = force ? "wb" : "wbx";
    FILE* fptr = fopen( file_dst, __accesstype );
    if ( fptr == NULL ) {
        // -- doesn't matter which kind of error is returned,
        //    the cmdExec class will check for SFTP_OK or not and return cmdExecError on any sftp error
        // -- errno wil be set automatically
        errMgr.setErrArg( file_dst );
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile2;
        return SFTP_GET_ERROR;
    }
    sftp_file __file = sftp_open( _sftp_session, fullpath, O_RDONLY, 0);
    if ( __file == NULL ) {
        // remove the local file and set errorno properly
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file_src );
        if ( fptr ) {
            fclose( fptr );
            remove( file_dst );
        }
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile2;
        return SFTP_READFILE_ERROR;
    }
    sftp_file_set_nonblocking( __file );

    // start fetching
    sftp_attributes attr = sftp_stat( _sftp_session, fullpath );
    int count = 0L, nbytes = -1;
    double __elpased_time = 0;
    const size_t& __total_size = attr->size;
    int __term_width = UTIL::getTermWidth();
    int __status_bar_print_width = __term_width - 13;
    {

        Timer t(&__elpased_time);
        double __process = 0.0;
        int    __int_process = 0;

        cout << "[>" << std::right << std::setw(__status_bar_print_width-2) << ']' << std::flush;

        // read in chunks
        // TODO add a timeout function in case of a poor connection causing the application to suspend
        int async_req = sftp_async_read_begin( __file, MAX_XFER_BUF_SIZE );
        if ( async_req >= 0 ) {
            usleep( WAIT_INTERVAL );
            nbytes = sftp_async_read( __file, xferbuf, MAX_XFER_BUF_SIZE, async_req );
        }
        else {
            this->seterrno( sftp_get_error(_sftp_session) );
            errMgr.setErrArg( "sftp_async_read_begin" );
            if ( fptr ) {
                fclose( fptr );
                remove( file_src );
            }
            delete [] file_src;
            delete [] file_dst;
            delete [] nEscFile2;
            return SFTP_GET_ERROR;
        }
        while ( nbytes == SSH_AGAIN || nbytes > 0 ) {
            if ( nbytes > 0 ) {
                fwrite( xferbuf, nbytes, 1, fptr );

                __process += (double)nbytes / (double)__total_size;
                __int_process = (int)std::ceil( (__status_bar_print_width-3) * __process );
                cout << "\r[";
                for (int i = 0; i < __int_process; ++i) cout << '=';
                cout << '>';
                cout << std::right << std::setw(__status_bar_print_width-2-__int_process) << ']' << std::flush;

                async_req = sftp_async_read_begin( __file, MAX_XFER_BUF_SIZE );
            }
            else ++count;
            usleep( WAIT_INTERVAL );
            if ( async_req >= 0 ) {
                nbytes = sftp_async_read( __file, xferbuf, MAX_XFER_BUF_SIZE, async_req );
            }
            else {
                nbytes = -1;
            }
        }

        // remove file if error happens
        if ( nbytes < 0 ) {
            this->seterrno( sftp_get_error(_sftp_session) );
            errMgr.setErrArg( file_src );
            sftp_close( __file );
            if ( fptr ) {
                fclose( fptr );
                remove( file_dst );
            }
            delete [] file_src;
            delete [] file_dst;
            delete [] nEscFile2;
            return SFTP_GET_ERROR;
        }

        // transfer successful, GREEN COLOR!
        cout << NORMAL_GREEN << "\r[";
        for (int i = 0; i < __status_bar_print_width-3; ++i) cout << '=';
        cout << ">]";

    } // fetch complete

    printf("\t%.3f sec.\n%s", __elpased_time/1000, COLOR_RESET);
    // cout << "waited for " << count * WAIT_INTERVAL / 1000 << " ms during transmission" << endl;

    // close the written file
    fclose( fptr );

    // close the file handler
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file_src );
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile2;
        return SFTP_CLOSEFILE_ERROR;
    }

    // last, make sure the fetched file is in correct mode
    chmod( file_dst, (mode_t)attr->permissions );

    delete [] file_src;
    delete [] file_dst;
    delete [] nEscFile2;
    return SFTP_OK;
}

// TODO
sftpStat sftpSession::get_recursive(const std::string_view& source, const std::string_view& destination, bool force) const {

    // create c_str for c API
    char* file_src = new char[source.size()+1];
    memcpy( file_src, &source.front(), source.size() );
    file_src[source.size()] = '\0';
    char* file_dst = new char[destination.size()+1];
    memcpy( file_dst, &destination.front(), destination.size() );
    file_dst[destination.size()] = '\0';

    std::vector<sftp_attributes> attrs;
    sftpStat returnStat = this->readDir( file_src, attrs );
    if ( returnStat == SFTP_OK ) { // directory
        auto mkdir_result = mkdir(file_dst, 0755);
        if ( mkdir_result != 0 && errno == EEXIST && !force ) {
            errMgr.setErrArg( file_dst );
            delete [] file_src;
            delete [] file_dst;
            return SFTP_GET_ERROR;
        }
        else if ( mkdir_result != 0 && errno != EEXIST ) {
            errMgr.setErrArg( file_dst );
            delete [] file_src;
            delete [] file_dst;
            return SFTP_GET_ERROR;
        }
        for (auto& attr : attrs) {
            if ( strcmp(attr->name, ".") == 0 ) continue;
            if ( strcmp(attr->name, "..") == 0 ) continue;
            size_t newSrcSize = source.size()+strlen(attr->name)+2;
            size_t newDstSize = destination.size()+strlen(attr->name)+2;
            char* newSrc = new char[newSrcSize];
            char* newDst = new char[newDstSize];
            if ( source.back() == '/' ) {
                snprintf( newSrc, newSrcSize, "%s%s", file_src, attr->name );
                snprintf( newDst, newDstSize, "%s%s", file_dst, attr->name );
            }
            else {
                snprintf( newSrc, newSrcSize, "%s/%s", file_src, attr->name );
                snprintf( newDst, newDstSize, "%s/%s", file_dst, attr->name );
            }
            this->get_recursive( std::string_view(newSrc), std::string_view(newDst), force );
            sftp_attributes_free( attr );
            delete [] newSrc;
            delete [] newDst;
        }
    }
    else { // other files or illegal files, leave error pasring to the next level
        returnStat = this->get( source, destination, force );
    }


    delete [] file_src;
    delete [] file_dst;
    return returnStat;
}


/*************
* PUT method *
**************/
// read file to memory in chunks and transmit with sftp_read/sftp_write
// TODO show ETA
sftpStat sftpSession::put(const std::string_view& source, const std::string_view& destination, bool force) const {

    // create c_str for c API
    char* file_src = new char[source.size()+1];
    memcpy( file_src, &source.front(), source.size() );
    file_src[source.size()] = '\0';
    char* file_dst = new char[destination.size()+1];
    memcpy( file_dst, &destination.front(), destination.size() );
    file_dst[destination.size()] = '\0';

    char* nEscFile1 = UTIL::rmEscChar( file_src );
    char* nEscFile2 = UTIL::rmEscChar( file_dst );
    const char* p_file_src = nEscFile1 == NULL ? file_src : nEscFile1;
    const char* p_file_dst = nEscFile2 == NULL ? file_dst : nEscFile2;

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), p_file_dst);

    if ( nEscFile2 ) delete [] nEscFile2;

    cout << "Uploading " << p_file_src << " to " << fullpath << endl;

    FILE* fptr = fopen( p_file_src, "rb" );
    if ( fptr == NULL ) {
        // -- doesn't matter which kind of error is returned,
        //    the cmdExec class will check for SFTP_OK or not and return cmdExecError on any sftp error
        // -- errno wil be set automatically
        errMgr.setErrArg( file_src );
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile1;
        return SFTP_GET_ERROR;
    }
    struct stat statbuf;
    stat( p_file_src, &statbuf);
    int __accesstype = O_WRONLY | O_CREAT;
    __accesstype |= force ? O_TRUNC : O_EXCL;
    sftp_file __file = sftp_open( _sftp_session, fullpath, __accesstype, statbuf.st_mode );
    if ( __file == NULL ) {
        // set errno properly
        this->seterrno( SSH_FX_FILE_ALREADY_EXISTS );
        errMgr.setErrArg( file_dst );
        if ( fptr ) fclose( fptr );
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile1;
        return SFTP_READFILE_ERROR;
    }

    // start transferring
    int nbytes = -1;
    double __elpased_time = 0;
    const off_t& __total_size = statbuf.st_size;
    int __term_width = UTIL::getTermWidth();
    int __status_bar_print_width = __term_width - 13;
    {

        Timer t(&__elpased_time);
        double __process = 0.0;
        int    __int_process = 0;

        cout << "[>" << std::right << std::setw(__status_bar_print_width-2) << ']' << std::flush;

        // read in chunks and upload
        while ( (nbytes = (int)fread( xferbuf, sizeof(char), MAX_XFER_BUF_SIZE, fptr )) > 0 ) {
            sftp_write( __file, xferbuf, (size_t)nbytes );

            __process += (double)nbytes / (double)__total_size;
            __int_process = (int)std::ceil( (__status_bar_print_width-3) * __process );
            cout << "\r[";
            for (int i = 0; i < __int_process; ++i) cout << '=';
            cout << '>';
            cout << std::right << std::setw(__status_bar_print_width-2-__int_process) << ']' << std::flush;

        }

        // remove file if error happens
        // THERE ARE NO API TO REMOVE REMOTE FILES
        if ( nbytes < 0 ) {
            errMgr.setErrArg( file_src );
            if ( fptr ) fclose( fptr );
            delete [] file_src;
            delete [] file_dst;
            delete [] nEscFile1;
            return SFTP_READFILE_ERROR;
        }

        // transfer successful, GREEN COLOR!
        cout << NORMAL_GREEN << "\r[";
        for (int i = 0; i < __status_bar_print_width-3; ++i) cout << '=';
        cout << ">]";

    } // transfer complete

    printf("\t%.3f sec.\n%s", __elpased_time/1000, COLOR_RESET);

    // close the written file
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file_dst );
        delete [] file_src;
        delete [] file_dst;
        delete [] nEscFile1;
        return SFTP_CLOSEFILE_ERROR;
    }

    // close the file handler 
    fclose( fptr );

    delete [] file_src;
    delete [] file_dst;
    delete [] nEscFile1;
    return SFTP_OK;
}

// TODO
sftpStat sftpSession::put_recursive(const std::string_view& source, const std::string_view& destination, bool force) const {
    return SFTP_OK;
}

}
