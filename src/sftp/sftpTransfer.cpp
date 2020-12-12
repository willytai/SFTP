#include "sftpSession.h"
#include "cmdParser.h"
#include "util.h"
#include <fcntl.h>
#include <unistd.h>

extern errorMgr errMgr;

#define MAX_XFER_BUF_SIZE 32768
#define WAIT_INTERVAL 10000 // in microsecond

static char xferbuf[MAX_XFER_BUF_SIZE];
static char fullpath[PATH_BUF_MAX];

namespace sftp
{

/*************
* GET method *
**************/
// read file to memory in chunks and transmit with sftp_read/sftp_write
// set errMgr.setErrArg() only and set it properly!!!
// TODO
//      show percentage
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

    if ( nEscFile1 ) free( nEscFile1 );

    cout << "fetching " << fullpath << " to " << p_file_dst << endl;

    const char* __accesstype = force ? "wb" : "wbx";
    FILE* fptr = fopen( file_dst, __accesstype );
    if ( fptr == NULL ) {
        // -- doesn't matter which kind of error is returned,
        //    the cmdExec class will check for SFTP_OK or not and return cmdExecError on any sftp error
        // -- errno wil be set automatically
        errMgr.setErrArg( file_dst );
        delete [] file_src;
        delete [] file_dst;
        free( nEscFile2);
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
        free(nEscFile2);
        return SFTP_READFILE_ERROR;
    }
    sftp_file_set_nonblocking( __file );

    // read in chunks
    // TODO add a timeout function in case of a poor connection causing the application to suspend
    int count = 0L, nbytes = -1, async_req = sftp_async_read_begin( __file, MAX_XFER_BUF_SIZE );
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
        free(nEscFile2);
        return SFTP_GET_ERROR;
    }
    while ( nbytes == SSH_AGAIN || nbytes > 0 ) {
        if ( nbytes > 0 ) {
            fwrite( xferbuf, nbytes, 1, fptr );
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

    cout << "waited for " << count * WAIT_INTERVAL / 1000 << " ms during transmission" << endl;

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
        free(nEscFile2);
        return SFTP_GET_ERROR;
    }

    // close the written file
    fclose( fptr );

    // close the file handler
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file_src );
        delete [] file_src;
        delete [] file_dst;
        free(nEscFile2);
        return SFTP_CLOSEFILE_ERROR;
    }

    // last, make sure the fetched file is in correct mode
    sftp_attributes attr = sftp_lstat( _sftp_session, fullpath );
    chmod( file_dst, (mode_t)attr->permissions );

    delete [] file_src;
    delete [] file_dst;
    free(nEscFile2);
    return SFTP_OK;
}

// TODO
sftpStat sftpSession::get_recursive(const std::string_view& source, const std::string_view& destination, bool force) const {
    return SFTP_OK;
}


/*************
* PUT method *
**************/
// read file to memory in chunks and transmit with sftp_read/sftp_write
// TODO
//      show percentage
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

    if ( nEscFile2 ) free( nEscFile2 );

    cout << "putting " << p_file_src << " to " << fullpath << endl;

    FILE* fptr = fopen( p_file_src, "rb" );
    if ( fptr == NULL ) {
        // -- doesn't matter which kind of error is returned,
        //    the cmdExec class will check for SFTP_OK or not and return cmdExecError on any sftp error
        // -- errno wil be set automatically
        errMgr.setErrArg( file_src );
        delete [] file_src;
        delete [] file_dst;
        free( nEscFile1);
        return SFTP_GET_ERROR;
    }
    struct stat statbuf;
    lstat( p_file_src, &statbuf);
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
        free(nEscFile1);
        return SFTP_READFILE_ERROR;
    }

    // read in chunks and upload
    int nbytes = -1;
    while ( (nbytes = (int)fread( xferbuf, sizeof(char), MAX_XFER_BUF_SIZE, fptr )) > 0 ) {
        cout << nbytes << " read from " << file_src << endl;
        sftp_write( __file, xferbuf, (size_t)nbytes );
    }

    // remove file if error happens
    // THERE ARE NO API TO REMOVE REMOTE FILES
    if ( nbytes < 0 ) {
        errMgr.setErrArg( file_src );
        if ( fptr ) fclose( fptr );
        delete [] file_src;
        delete [] file_dst;
        free(nEscFile1);
        return SFTP_READFILE_ERROR;
    }

    // close the written file
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file_dst );
        delete [] file_src;
        delete [] file_dst;
        free(nEscFile1);
        return SFTP_CLOSEFILE_ERROR;
    }

    // close the file handler 
    fclose( fptr );

    delete [] file_src;
    delete [] file_dst;
    free(nEscFile1);
    return SFTP_OK;
}

// TODO
sftpStat sftpSession::put_recursive(const std::string_view& source, const std::string_view& destination, bool force) const {
    return SFTP_OK;
}

}
