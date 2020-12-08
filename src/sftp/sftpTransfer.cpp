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
// TODO -f option ( overwrite )
//      no overwrite ( warn )
//      show percentage
//      create the file in correct mode ( make executable executable )
sftpStat sftpSession::get(const std::string& file) const {

    char* nEscFile = UTIL::rmEscChar( file.c_str() );
    const char* pfile = nEscFile == NULL ? file.c_str() : nEscFile;

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), pfile);

    if ( nEscFile ) free( nEscFile );

    cout << "getting " << fullpath << endl;

    // TODO Error handling for fptr
    //      This is wrong right now
    FILE* fptr = fopen( file.c_str(), "wb" );
    sftp_file __file = sftp_open( _sftp_session, fullpath, O_RDONLY, 0);
    if ( __file == NULL ) {
        // remove the local file and set errorno properly
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file );
        if ( fptr ) {
            fclose( fptr );
            remove( file.c_str() );
        }
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
            remove( file.c_str() );
        }
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
        errMgr.setErrArg( file );
        sftp_close( __file );
        if ( fptr ) {
            fclose( fptr );
            remove( file.c_str() );
        }
        return SFTP_GET_ERROR;
    }

    // close the written file
    fclose( fptr );

    // close the file handler
    if ( sftp_close(__file) != SSH_OK ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        errMgr.setErrArg( file );
        return SFTP_CLOSEFILE_ERROR;
    }
    return SFTP_OK;
}

sftpStat sftpSession::get(const std::vector<std::string>& targets) const {
    sftpStat returnStat = SFTP_OK;
    for (const auto& file : targets) {
        returnStat = this->get( file );
    }
    return returnStat;
}

// TODO
sftpStat sftpSession::get_recursive(const std::string& dir) const {
    return SFTP_OK;
}

sftpStat sftpSession::get_recursive(const std::vector<std::string>& targets) const {
    sftpStat returnStat = SFTP_OK;
    for (const auto& dir : targets) {
        returnStat = this->get_recursive( dir );
    }
    return returnStat;
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
