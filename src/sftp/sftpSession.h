#ifndef __SFTPSESSION_H__
#define __SFTPSESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include "def.h"

class sftpSession
{
public:
    sftpSession(const char* hostIP,
                const char* user,
                const char* psswd,
                const char* port = NULL,
                int v = 0);
    ~sftpSession();

private:
    void initSSHSession();
    bool connectSSH();
    bool verifyKnownHost();
    bool authenticate();

    bool initSFTP();

    ssh_session       _ssh_session;
    sftp_session      _sftp_session;
    const char*       _hostIP;
    const char*       _usr;
    const char*       _psswd;
    const char*       _port;
    int               _verbosity;
};


#endif /* __SFTPSESSION_H__ */
