#ifndef __SFTPSESSION_H__
#define __SFTPSESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <string>
#include <vector>
#include "def.h"
#include "dirIO.h"
#include "Usage.h"

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

    SFTP_CD_ERROR = 16,

    SFTP_READDIR_ERROR,
    SFTP_READFILE_ERROR,
    SFTP_CLOSEFILE_ERROR,
    SFTP_GET_ERROR,
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
                int v = SSH_LOG_PROTOCOL);
    ~sftpSession();

    // str: c_string, l: string length ( strlen(str) will suffice )
    void setUsrName(const char* str, size_t l) { _usr    = new char[l+1]; strcpy(_usr,    str); }
    void setHostIP (const char* str, size_t l) { _hostIP = new char[l+1]; strcpy(_hostIP, str); }
    void setPsswd  (const char* str, size_t l) { _psswd  = new char[l+1]; strcpy(_psswd,  str); }
    void setPort   (const char* str, size_t l) { _port   = new char[l+1]; strcpy(_port,   str); }

    // verbosity
    void setVerbose(int v) { _verbosity = v; }

    sftpStat start();

    const char* pwd() const;
    sftpStat    cd(const std::string&);
    sftpStat    readDir(const std::string&, std::vector<std::pair<std::string, bool> >&) const;
    sftpStat    readDir(const char*, std::vector<sftp_attributes>&) const;
    sftpStat    get(const std::string_view&, const std::string_view&, bool) const;
    sftpStat    get_recursive(const std::string_view&, const std::string_view&, bool) const;
    sftpStat    put(const std::string_view&, const std::string_view&, bool) const;
    sftpStat    put_recursive(const std::string_view&, const std::string_view&, bool) const;

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

    // always points to the current working directory
    sftp_dir          _dir;
};

}

#endif /* __SFTPSESSION_H__ */
