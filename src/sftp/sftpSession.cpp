#include <string.h>
#include <errno.h>
#include <termios.h>
#include "cmdParser.h"
#include "sftpSession.h"

extern errorMgr errMgr;

namespace sftp
{

static void set_psswd_termio() {
    struct termios new_settings;
    tcgetattr(0, &new_settings);
    new_settings.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &new_settings);
}

static void reset_termio() {
    struct termios new_settings;
    tcgetattr(0, &new_settings);
    new_settings.c_lflag &= ECHO;
    tcsetattr(0, TCSANOW, &new_settings);
}

sftpSession::sftpSession() {
    _usr = _hostIP = _psswd = _port = NULL;
}

sftpSession::sftpSession(const char* hostIP, const char* user, const char* psswd, const char* port, int v) {
    this->setUsrName(user,   strlen(user)+1 );
    this->setHostIP (hostIP, strlen(hostIP)+1 );
    this->setPsswd  (psswd,  strlen(psswd)+1 );
    this->setPort   (port,   strlen(port)+1 );
    this->setVerbose(v);
    // this->setVerbose(SSH_LOG_PROTOCOL);
}

sftpSession::~sftpSession() {
    ssh_disconnect(_ssh_session);
    ssh_free(_ssh_session);
    if ( _hostIP != NULL ) free(_hostIP);
    if ( _usr    != NULL ) free(_usr);
    if ( _psswd  != NULL ) free(_psswd);
    if ( _port   != NULL ) free(_port);
}

sftpStat sftpSession::start() {
    sftpStat sfstat;
    if ( (sfstat = this->initSSHSession())  != SFTP_OK ) return sfstat;
    if ( (sfstat = this->connectSSH())      != SFTP_OK ) return sfstat;
    if ( (sfstat = this->verifyKnownHost()) != SFTP_OK ) return sfstat;
    if ( (sfstat = this->authenticate())    != SFTP_OK ) return sfstat;
    if ( (sfstat = this->initSFTP())        != SFTP_OK ) return sfstat;
    return SFTP_OK;
}



/*******************/
/* private methods */
/*******************/
sftpStat sftpSession::initSSHSession() {
    _ssh_session = ssh_new();
    if ( _ssh_session == NULL ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_SSH_INIT_FAILED;
    }
    else {
        printf("ssh session created\n");
    }
    ssh_options_set(_ssh_session, SSH_OPTIONS_HOST, _hostIP);
    ssh_options_set(_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &_verbosity);
    ssh_options_set(_ssh_session, SSH_OPTIONS_USER, _usr);
    // ssh_options_set(_ssh_session, SSH_OPTIONS_SSH_DIR, ".\\");
    if (_port != NULL) ssh_options_set(_ssh_session, SSH_OPTIONS_PORT, _port);
    return SFTP_OK;
}

sftpStat sftpSession::connectSSH() {
    int rc = ssh_connect(_ssh_session);
    if ( rc != SSH_OK ) {
        errMgr.setSftpErr( _hostIP, ssh_get_error(_ssh_session) );
        return SFTP_SSH_CONNECTION_DENIED;
    }
    else {
        printf("connection established\n");
        return SFTP_OK;
    }
}

sftpStat sftpSession::verifyKnownHost() {
    ssh_known_hosts_e   stat;
    unsigned char*      hash = NULL;
    ssh_key             srv_pubkey = NULL;
    size_t              hlen;
    char                buf[10];
    char*               hexa;

    if ( ssh_get_server_publickey(_ssh_session, &srv_pubkey) < 0 ) {
        return SFTP_VERIFY_PUBLIC_KEY_ERROR;
    }
    if ( ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen) < 0 ) {
        ssh_key_free(srv_pubkey);
        return SFTP_VERIFY_PUBLIC_KEYHASH_ERROR;
    }
    ssh_key_free(srv_pubkey);

    stat = ssh_session_is_known_server(_ssh_session);
    switch (stat) {
        case SSH_KNOWN_HOSTS_OK:
            printf("Host recognized\n");
            break;

        case SSH_KNOWN_HOSTS_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hash(SSH_PUBLICKEY_HASH_SHA1, hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            return SFTP_VERIFY_HOSTS_CHANGED;

        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");

            // -- this is to indicate that falling through is intentional
            //    and mutes the compiler for warnings (c++11/14/17)
            [[gnu::fallthrough]];

        case SSH_KNOWN_HOSTS_UNKNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr, "Public key hash: %s\n", hexa);
            fprintf(stderr, "The server is unknown. Do you trust the host key? (yes/no) ");
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            if ( fgets(buf, sizeof(buf), stdin) == NULL ) {
                return SFTP_VERIFY_STDIN_ERROR;
            }
            if ( strncasecmp(buf, "yes", 3) != 0 ) {
                return SFTP_VERIFY_HOST_CONNECTION_DENIED;
            }
 
            if ( ssh_session_update_known_hosts(_ssh_session) < 0 ) {
                errMgr.setSftpErr( strerror(errno) );
                return SFTP_VERIFY_UPDATE_KNOWN_HOST_ERROR;
            }
            break;

        case SSH_KNOWN_HOSTS_OTHER:
            ssh_clean_pubkey_hash(&hash);
            return SFTP_VERIFY_HOSTS_OTHER;

        case SSH_KNOWN_HOSTS_ERROR:
            errMgr.setSftpErr( ssh_get_error(_ssh_session) );
            ssh_clean_pubkey_hash(&hash);
            return SFTP_VERIFY_KNOWN_HOST_ERROR;

    }
    ssh_clean_pubkey_hash(&hash);
    return SFTP_OK;
}

sftpStat sftpSession::authenticate() {
    if ( _psswd == NULL ) {
        _psswd = (char*)malloc(PSSWD_BUF_MAX*sizeof(char));
        for (int i = 0; i < 3; ++i) {
            printf("%s@%s's password: ", _usr, _hostIP);
            set_psswd_termio();
            scanf("%s", _psswd);
            reset_termio();

            int rc = ssh_userauth_password(_ssh_session, NULL, _psswd);

            if ( rc == SSH_AUTH_SUCCESS ) {
                // -- this is to clear the input buffer, preventing the
                //    command parser to detect the newline character after
                //    entering the password
                while (getchar() != '\n');
                return SFTP_OK;
            }
            else if ( rc == SSH_AUTH_DENIED ) {
                printf("\nPermission denied");
                if ( i < 2 ) {
                    printf(", please try again.\n");
                }
                else {
                    printf(".\n");
                }
            }
            else if ( rc == SSH_AUTH_ERROR ) {
                return SFTP_AUTH_ERROR;
            }
            else {
                return SFTP_AUTH_UNDEF_ERROR;
            }
        }
        return SFTP_AUTH_DENIED;
    }
    else {
        int rc = ssh_userauth_password(_ssh_session, NULL, _psswd);
        if ( rc != SSH_AUTH_SUCCESS ) {
            printf("authentication failed\n");
        }
        return SFTP_OK;
    }
}

sftpStat sftpSession::initSFTP() {
    _sftp_session = sftp_new(_ssh_session);
    if ( _sftp_session == NULL ) {
        errMgr.setSftpErr( ssh_get_error(_sftp_session) );
        return SFTP_SESS_ALLOCATE_FAILED;
    }
    if ( sftp_init(_sftp_session) != SSH_OK ) {
        errMgr.setSftpErr( ssh_get_error(_sftp_session) );
        sftp_free(_sftp_session);
        return SFTP_SESS_INIT_FAILED;
    }
    printf("sftp session established\n");
    sftp_dir dir;
    sftp_attributes file;
    dir = sftp_opendir(_sftp_session, "./Desktop");
    while ( (file = sftp_readdir(_sftp_session, dir)) ) {
        fprintf(stderr, "%30s(%.8o) : %s(%.5d) %s(%.5d) : %.10lu bytes\n",
                file->name,
                file->permissions,
                file->owner,
                file->uid,
                file->group,
                file->gid,
                (size_t)file->size);
        sftp_attributes_free(file);
    }
    sftp_free(_sftp_session);
    return SFTP_OK;
}

}
