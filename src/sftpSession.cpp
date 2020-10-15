#include <string.h>
#include <errno.h>
#include "sftpSession.h"

sftpSession::sftpSession(const char* hostIP, const char* user, const char* psswd, const char* port, int v) {
    _hostIP    = hostIP;
    _usr       = user;
    _psswd     = psswd;
    _port      = port;
    _verbosity = v;
    // _verbosity = SSH_LOG_PROTOCOL;

    this->initSSHSession();
    this->connectSSH();
    this->verifyKnownHost();
    this->authenticate();

    this->initSFTP();
}

sftpSession::~sftpSession() {
    ssh_disconnect(_ssh_session);
    ssh_free(_ssh_session);
}



/*******************/
/* private methods */
/*******************/

void sftpSession::initSSHSession() {
    _ssh_session = ssh_new();
    if ( _ssh_session == NULL ) {
        exit(-1);
    }
    else {
        printf("ssh session created\n");
    }
    ssh_options_set(_ssh_session, SSH_OPTIONS_HOST, _hostIP);
    ssh_options_set(_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &_verbosity);
    ssh_options_set(_ssh_session, SSH_OPTIONS_USER, _usr);
    // ssh_options_set(_ssh_session, SSH_OPTIONS_SSH_DIR, ".\\");
    if (_port != NULL) ssh_options_set(_ssh_session, SSH_OPTIONS_PORT, _port);
}

bool sftpSession::connectSSH() {
    int rc = ssh_connect(_ssh_session);
    if ( rc != SSH_OK ) {
        fprintf(stderr, "Error connecting to %s: %s\n", _hostIP, ssh_get_error(_ssh_session));
        return false;
    }
    else {
        printf("connection established\n");
        return true;
    }
}

bool sftpSession::verifyKnownHost() {
    ssh_known_hosts_e   stat;
    unsigned char*      hash = NULL;
    ssh_key             srv_pubkey = NULL;
    size_t              hlen;
    char                buf[10];
    char*               hexa;

    if ( ssh_get_server_publickey(_ssh_session, &srv_pubkey) < 0 ) {
        fprintf(stderr, "unable to get public key from server\n");
        return false;
    }
    if ( ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen) < 0 ) {
        fprintf(stderr, "unable to get key hash from server\n");
        ssh_key_free(srv_pubkey);
        return false;
    }
    ssh_key_free(srv_pubkey);

    stat = ssh_session_is_known_server(_ssh_session);
    switch (stat) {
        case SSH_KNOWN_HOSTS_OK:
            printf("Host recognized\n");
            break;

        case SSH_KNOWN_HOSTS_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            return false;

        case SSH_KNOWN_HOSTS_OTHER:
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            return false;

        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");

            /* Fall through the next behavior */

        case SSH_KNOWN_HOSTS_UNKNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr, "Public key hash: %s\n", hexa);
            fprintf(stderr, "The server is unknown. Do you trust the host key? (yes/no) ");
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            if ( fgets(buf, sizeof(buf), stdin) == NULL ) {
                fprintf(stderr, "stdin error.\n");
                return false;
            }
            if ( strncasecmp(buf, "yes", 3) != 0 ) {
                fprintf(stderr, "Connection denied by user.\n");
                return false;
            }
 
            if ( ssh_session_update_known_hosts(_ssh_session) < 0 ) {
                fprintf(stderr, "Error %s\n", strerror(errno));
                return false;
            }
            break;

        case SSH_KNOWN_HOSTS_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(_ssh_session));
            ssh_clean_pubkey_hash(&hash);
            return false;

    }
    ssh_clean_pubkey_hash(&hash);
    return true;
}

bool sftpSession::authenticate() {
    // TODO: this part should be interactive with the terminal
    int rc = ssh_userauth_password(_ssh_session, NULL, _psswd);
    if ( rc != SSH_OK ) {
        fprintf(stderr, "cannot authenticate server\n");
        return false;
    }
    else {
        printf("authentication complete\n");
        return true;
    }
}

bool sftpSession::initSFTP() {
    _sftp_session = sftp_new(_ssh_session);
    if ( _sftp_session == NULL ) {
        fprintf(stderr, "Could not allocate a sftp session: %s\n", ssh_get_error(_sftp_session));
        return false;
    }
    if ( sftp_init(_sftp_session) != SSH_OK ) {
        fprintf(stderr, "Could not initialize a sftp session: %s\n", ssh_get_error(_sftp_session));
        sftp_free(_sftp_session);
        return false;
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
    return true;
}
