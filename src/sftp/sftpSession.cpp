#include <string.h>
#include <errno.h>
#include <termios.h>
#include "cmdParser.h"
#include "sftpSession.h"
#include "util.h"

extern errorMgr errMgr;

static char fullpath[PATH_BUF_MAX];

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
    _pwd.clear();
}

sftpSession::sftpSession(const char* hostIP, const char* user, const char* psswd, const char* port, int v) {
    this->setUsrName(user,   strlen(user)+1 );
    this->setHostIP (hostIP, strlen(hostIP)+1 );
    this->setPsswd  (psswd,  strlen(psswd)+1 );
    this->setPort   (port,   strlen(port)+1 );
    this->setVerbose(v);
    // this->setVerbose(SSH_LOG_PROTOCOL);
    _pwd.clear();
}

sftpSession::~sftpSession() {
    if ( _sftp_session != NULL ) sftp_free(_sftp_session);
    if ( _ssh_session  != NULL ) ssh_disconnect(_ssh_session);
    if ( _ssh_session  != NULL ) ssh_free(_ssh_session);
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
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_SESS_ALLOCATE_FAILED;
    }
    if ( sftp_init(_sftp_session) != SSH_OK ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        sftp_free(_sftp_session);
        return SFTP_SESS_INIT_FAILED;
    }
    printf("sftp session established\n");

    // store the absolute path of $HOME and set _pwd to _home;
    char* chome = sftp_canonicalize_path(_sftp_session, "./");
    _pwd = _home = std::string(chome);
    ssh_string_free_char(chome);

    return SFTP_OK;
}

// sftp does not have the conecpt of "pwd"
// the working directory is will be stored locally
sftpStat sftpSession::cd(std::string& path) {
    if ( path.size() == 0 ) { // back to $HOME
        _pwd = _home;
    }

    // concatenate path
    snprintf(fullpath, PATH_BUF_MAX, "%s/%s", _pwd.c_str(), path.c_str());

    // if path doesn't exist, set errno properly
    if ( (_dir = sftp_opendir(_sftp_session, fullpath)) == NULL ) {
        this->seterrno( sftp_get_error(_sftp_session) );
        return SFTP_CD_ERROR;
    }
    else {
        sftp_closedir( _dir );
    }

    // make sure the stored path is in canonical form
    // this function gets rid of leading "./" or trailing "/", very powerful
    char* cpath = sftp_canonicalize_path(_sftp_session, fullpath);
    size_t i = 0;
    while ( cpath[i] != '\0' ) {
        if ( _pwd.size() < i+1 ) _pwd.resize(i+1);
        _pwd[i] = cpath[i];
        ++i;
    }
    _pwd.resize(i);
    ssh_string_free_char(cpath);

    return SFTP_OK;
}

/*
   stuct sftp_attributes_struct {
    char *name;
    char *longname; // ls -l output on openssh, not reliable else
    uint32_t flags;
    uint8_t type;   // doesn't know what in the fuck this is
    uint64_t size;
    uint32_t uid;
    uint32_t gid;
    char *owner; // set if openssh and version 4
    char *group; // set if openssh and version 4
    uint32_t permissions; // octal based
    uint64_t atime64;
    uint32_t atime;
    uint32_t atime_nseconds;
    uint64_t createtime;
    uint32_t createtime_nseconds;
    uint64_t mtime64;
    uint32_t mtime;
    uint32_t mtime_nseconds;
    ssh_string acl;
    uint32_t extended_count;
    ssh_string extended_type;
    ssh_string extended_data;
 }*/
sftpStat sftpSession::readDir(const std::string& dir, std::vector<std::pair<std::string, bool> >& container) const {
    std::string fulldir = _pwd+"/"+dir;
    sftp_dir curdir;
    if ( ( curdir = sftp_opendir( _sftp_session, fulldir.c_str() ) ) == NULL ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_READDIR_ERROR;
    }

    sftp_attributes curattr;
    while ( (curattr = sftp_readdir( _sftp_session, curdir )) != NULL ) {
        container.emplace_back( std::string(curattr->name), curattr->longname[0]=='d' );
        sftp_attributes_free( curattr );
    }

    // error handling
    if ( curdir->eof != 1) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_READDIR_ERROR;
    }
    if ( sftp_closedir( curdir ) ) {
        errMgr.setSftpErr( ssh_get_error(_ssh_session) );
        return SFTP_READDIR_ERROR;
    }

    return SFTP_OK;
}

void sftpSession::seterrno(int code) const {
    switch (code) {
        case SSH_FX_PERMISSION_DENIED:   errno = EACCES; return;
        case SSH_FX_NO_SUCH_FILE:        errno = ENOENT; return;
        case SSH_FX_NO_SUCH_PATH:        errno = ENOENT; return;
        case SSH_FX_NO_MEDIA:            errno = ENODEV; return;
        case SSH_FX_WRITE_PROTECT:       errno = EROFS; return;
        case SSH_FX_FILE_ALREADY_EXISTS: errno = EEXIST; return;
        case SSH_FX_OP_UNSUPPORTED:      errno = EOPNOTSUPP; return;
        case SSH_FX_CONNECTION_LOST:     errno = ETIMEDOUT; return;
        case SSH_FX_NO_CONNECTION:       errno = ENOTCONN; return;
        case SSH_FX_INVALID_HANDLE:      errno = EBADF; return;
        case SSH_FX_EOF:                 errno = EBADF; return;
        case SSH_FX_BAD_MESSAGE:         errno = EBADMSG; return;
        case SSH_FX_FAILURE:             errno = EBADRPC; return;
        default: return;
    }
}

static char cwdBuf[CWD_BUF_MAX];
const char* sftpSession::pwd() const {
    if ( UTIL::strNcmp(_pwd, _home, _home.length()) == 0 ) {
        cwdBuf[0] = '~';
        UTIL::substr( _pwd.c_str(), cwdBuf, _home.length(), 0xffffffffffffffff, 1 );
        return cwdBuf;
    }
    else {
        return _pwd.c_str();
    }
}

}
