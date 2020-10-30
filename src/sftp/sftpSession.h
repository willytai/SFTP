#ifndef __SFTPSESSION_H__
#define __SFTPSESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include "def.h"

namespace sftp
{

enum sftpStat
{
    SFTP_OK = 0,
    SFTP_INIT_FAILED = 1,
    SFTP_AUTH_ERROR  = 2,
    SFTP_AUTH_DENIED = 3,
    SFTP_AUTH_ERROR_UNKNOWN = 4
};

class sftpSession
{
#define PSSWD_BUF_MAX 32
public:
    sftpSession();
    sftpSession(const char* hostIP,
                const char* user,
                const char* psswd,
                const char* port = NULL,
                int v = 0);
    ~sftpSession();

    void setUsrName(const char* u, size_t l) { _usr    = (char*)malloc(l*sizeof(char)); strcpy(_usr,    u); }
    void setHostIP (const char* h, size_t l) { _hostIP = (char*)malloc(l*sizeof(char)); strcpy(_hostIP, h); }
    void setPsswd  (const char* p, size_t l) { _psswd  = (char*)malloc(l*sizeof(char)); strcpy(_psswd,  p); }
    void setPort   (const char* p, size_t l) { _port   = (char*)malloc(l*sizeof(char)); strcpy(_port,   p); }
    void setVerbose(int v) { _verbosity = v; }

    sftpStat start();

private:
    sftpStat initSSHSession();
    sftpStat connectSSH();
    sftpStat verifyKnownHost();
    sftpStat authenticate();

    sftpStat initSFTP();

    ssh_session       _ssh_session;
    sftp_session      _sftp_session;
    char*             _hostIP;
    char*             _usr;
    char*             _psswd;
    char*             _port;
    int               _verbosity;
};

}

#endif /* __SFTPSESSION_H__ */
