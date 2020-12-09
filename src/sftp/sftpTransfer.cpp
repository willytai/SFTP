#include "sftpSession.h"
#include "cmdParser.h"
#include "util.h"
#include <fcntl.h>
#include <unistd.h>

extern errorMgr errMgr;

#define MAX_XFER_BUF_SIZE 16384
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
//      create the file in correct mode ( make executable executable )
sftpStat sftpSession::get(const std::string_view& source, const std::string_view& destination, bool force) const {

    // create c_str for c API
    char* file_src = new char[source.size()+1];
    memcpy( file_src, &source.front(), source.size() );
    file_src[source.size()] = '\0';
    char* file_dst = new char[destination.size()+1];
    memcpy( file_dst, &destination.front(), destination.size() );
    file_dst[destination.size()] = '\0';

    char* nEscFile = UTIL::rmEscChar( file_src );
    const char* pfile = nEscFile == NULL ? file_src : nEscFile;

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), pfile);

    if ( nEscFile ) free( nEscFile );

    cout << "getting " << fullpath << endl;

    const char* mode = force ? "wb" : "wbx";
    FILE* fptr = fopen( file_dst, mode );
    if ( fptr == NULL ) {
        // doesn't matter which kind of error is returned, the cmdExec class will check for SFTP_OK or not and return cmdExecError on any sftp error
        // errno wil be set automatically
        delete [] file_src;
        delete [] file_dst;
        errMgr.setErrArg( file_dst );
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
        return SFTP_CLOSEFILE_ERROR;
    }
    delete [] file_src;
    delete [] file_dst;
    return SFTP_OK;
}

// TODO
sftpStat sftpSession::get_recursive(const std::string_view& source, const std::string_view& target, bool force) const {
    return SFTP_OK;
}


/*************
* PUT method *
**************/
// read file to memory in chunks and transmit with sftp_read/sftp_write
// TODO -f option ( overwrite )
//      no overwrite ( warn )
//      show percentage
//      create the file in correct mode ( make executable executable )
sftpStat sftpSession::put(const std::string& file) const {

    char* nEscFile = UTIL::rmEscChar( file.c_str() );
    const char* pfile = nEscFile == NULL ?  file.c_str() : nEscFile;

    // get the last string after '/'
    const char* filename = UTIL::find_last( pfile, '/' );

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), filename);

    if ( nEscFile ) free( nEscFile );

    cout << "putting " << pfile << " to " << fullpath << endl;

    // TODO Error handling for fptr
    //      Control override option here!!!!!!!
    FILE* fptr = fopen( pfile, "rb" );
    struct stat statbuf;
    lstat( pfile, &statbuf);
    int __accesstype = O_WRONLY | O_CREAT;
    __accesstype |= false ? O_EXCL : O_TRUNC;
    sftp_file __file = sftp_open( _sftp_session, fullpath, __accesstype, statbuf.st_mode );
    if ( __file == NULL ) {
        // set errno properly
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file );
        if ( fptr ) fclose( fptr );
        return SFTP_READFILE_ERROR;
    }

    // read in chunks and upload
    int nbytes = -1;
    while ( (nbytes = (int)fread( xferbuf, sizeof(char), MAX_XFER_BUF_SIZE, fptr )) > 0 ) {
        cout << nbytes << " read from " << pfile << endl;
        sftp_write( __file, xferbuf, (size_t)nbytes );
    }

    // remove file if error happens
    // THERE ARE NO API TO REMOVE REMOTE FILES
    if ( nbytes < 0 ) {
        errMgr.setErrArg( file );
        if ( fptr ) fclose( fptr );
        return SFTP_READFILE_ERROR;
    }

    // close the written file
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file );
        return SFTP_CLOSEFILE_ERROR;
    }

    // close the file handler 
    fclose( fptr );

    return SFTP_OK;
}

sftpStat sftpSession::put(const std::vector<std::string>& targets) const {
    sftpStat returnStat = SFTP_OK;
    for (const auto& file : targets) {
        returnStat = this->put( file );
    }
    return returnStat;
}

sftpStat sftpSession::put_recursive(const std::string& dir) const {
    return SFTP_OK;
}

sftpStat sftpSession::put_recursive(const std::vector<std::string>& targets) const {
    sftpStat returnStat = SFTP_OK;
    for (const auto& dir : targets) {
        returnStat = this->put_recursive( dir );
    }
    return returnStat;
}

}
