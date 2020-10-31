#ifndef __SFTPSESSION_H__
#define __SFTPSESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <string>
#include "def.h"

namespace sftp
{

enum sftpStat
{
    SFTP_OK = 0,
    SFTP_SSH_INIT_FAILED = 1,
    SFTP_SSH_CONNECTION_DENIED = 2,
    SFTP_SESS_ALLOCATE_FAILED = 3,
    SFTP_SESS_INIT_FAILED = 4,
    SFTP_AUTH_ERROR  = 5,
    SFTP_AUTH_DENIED = 6,
    SFTP_AUTH_UNDEF_ERROR = 7,
    SFTP_VERIFY_PUBLIC_KEY_ERROR = 8,
    SFTP_VERIFY_PUBLIC_KEYHASH_ERROR = 9,
    SFTP_VERIFY_HOSTS_CHANGED = 10,
    SFTP_VERIFY_HOSTS_OTHER = 11,
    SFTP_VERIFY_STDIN_ERROR = 12,
    SFTP_VERIFY_HOST_CONNECTION_DENIED = 13,
    SFTP_VERIFY_UPDATE_KNOWN_HOST_ERROR = 14,
    SFTP_VERIFY_KNOWN_HOST_ERROR = 15,

    SFTP_CD_ERROR = 16
};

class sftpSession
{
#define PSSWD_BUF_MAX 32
#define PATH_BUF_MAX  256
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

    const char* pwd() const { return _pwd.c_str(); }
    sftpStat    cd(std::string&);

private:
    sftpStat initSSHSession();
    sftpStat connectSSH();
    sftpStat verifyKnownHost();
    sftpStat authenticate();
    sftpStat initSFTP();

    void seterrno(int) const;

private:
    ssh_session       _ssh_session;
    sftp_session      _sftp_session;
    char*             _hostIP;
    char*             _usr;
    char*             _psswd;
    char*             _port;
    int               _verbosity;

    // need to keep track of the current working directory
    // this is always a relative path to the $HOME directory on the remote server
    std::string       _pwd;
    std::string       _home;
    sftp_dir          _dir;
};

}

#endif /* __SFTPSESSION_H__ */
