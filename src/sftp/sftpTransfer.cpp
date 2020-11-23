#include "sftpSession.h"
#include "cmdParser.h"
#include <fcntl.h>
#include <unistd.h>

extern errorMgr errMgr;

#define MAX_XFER_BUF_SIZE 16384
#define WAIT_INTERVAL 1000 // in microsecond
static char xferbuf[MAX_XFER_BUF_SIZE];

namespace sftp
{

// read file to memory in chunks and transmit with sftp_read/sftp_write
// TODO remember to remove the created file if transfer fails
sftpStat sftpSession::get(const std::string& file) const {
    // TODO Error handling for fptr
    FILE* fptr = fopen( file.c_str(), "a" );
    sftp_file __file = sftp_open( _sftp_session, file.c_str(), O_RDONLY, 0);
    if ( __file == NULL ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_READFILE_ERROR;
    }
    sftp_file_set_nonblocking( __file );

    // read in chunks
    // TODO add a timeout function in case of a poor connection causing the application to suspend
    int nbytes = -1, async_req = sftp_async_read_begin( __file, MAX_XFER_BUF_SIZE );
    if ( async_req >= 0 ) {
        usleep( WAIT_INTERVAL );
        nbytes = sftp_async_read( __file, xferbuf, MAX_XFER_BUF_SIZE, async_req );
    }
    while ( nbytes == SSH_AGAIN || nbytes > 0 ) {
        if ( nbytes > 0 ) {
            cout << "writing..." << endl;
            fwrite( xferbuf, (size_t)nbytes, 1, fptr );
            async_req = sftp_async_read_begin( __file, MAX_XFER_BUF_SIZE );
        }
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
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        sftp_close( __file );
        if ( fptr ) {
            fclose( fptr );
            remove( file.c_str() );
        }
        return SFTP_GET_ERROR;
    }

    // close the file handler
    if ( sftp_close(__file) != SSH_OK ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_CLOSEFILE_ERROR;
    }
    return SFTP_OK;
}

sftpStat sftpSession::get(const std::vector<std::string>& targets) const {
    sftpStat returnStat;
    for (const auto& file : targets) {
        returnStat = this->get( file );
    }
    return returnStat;
}

sftpStat sftpSession::get_recursive(const std::string& dir) const {
    return SFTP_OK;
}

sftpStat sftpSession::get_recursive(const std::vector<std::string>& targets) const {
    return SFTP_OK;
}

}
