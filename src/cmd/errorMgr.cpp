#include "cmdParser.h"
#include <set>
#include <string>

static std::set<std::string> ErrorOPT;

static std::string ErrorCMD;
static std::string ErrorArg;
static const cmdExec* ErrorHandler;

// for ssh connection error
static const char* ErrorHostIP;
static const char* ErrorSftpMsg;

void errorMgr::handle(const cmdStat& errCode) {
    switch (errCode) {
        case CMD_ERROR:        this->cmdError();      break;
        case CMD_OPT_ILLEGAL:  this->cmdOptIllegal(); break;
        case CMD_EXEC_ERROR:   this->cmdExecError();  break;
        case CMD_ARG_TOO_MANY: this->cmdArgTooMany(); break;
        default: return;
    }
    ErrorOPT.clear();
}

// TODO
void errorMgr::handle(const argStat& errCode) {

}

void errorMgr::handle(const sftp::sftpStat& errCode) {
    switch (errCode) {
        case sftp::SFTP_SSH_CONNECTION_DENIED:          this->sftpSshConnectError();        break;
        case sftp::SFTP_SSH_INIT_FAILED:                this->sftpSshAllocError();          break;
        case sftp::SFTP_SESS_ALLOCATE_FAILED:           this->sftpSessAllocError();         break;
        case sftp::SFTP_SESS_INIT_FAILED:               this->sftpSessInitError();          break;
        case sftp::SFTP_AUTH_ERROR:                     this->sftpAuthError();              break;
        case sftp::SFTP_AUTH_UNDEF_ERROR:               this->sftpAuthUndefError();         break;
        case sftp::SFTP_VERIFY_PUBLIC_KEY_ERROR:        this->sftpVrfyPubKeyError();        break;
        case sftp::SFTP_VERIFY_PUBLIC_KEYHASH_ERROR:    this->sftpVrfyPubHashError();       break;
        case sftp::SFTP_VERIFY_HOSTS_OTHER:             this->sftpVrfyHostOtherError();     break;
        case sftp::SFTP_VERIFY_STDIN_ERROR:             this->sftpVrfyStdinError();         break;
        case sftp::SFTP_VERIFY_HOST_CONNECTION_DENIED:  this->sftpVrfyHostConnectDefined(); break;
        case sftp::SFTP_VERIFY_UPDATE_KNOWN_HOST_ERROR: this->sftpVrfyUpdateError();        break;
        case sftp::SFTP_VERIFY_KNOWN_HOST_ERROR:        this->sftpVrfyKnownHostError();     break;
        default: return;
    }
}

/**********************
 * for command parser *
 *********************/
void errorMgr::handleNonExistDir(const std::string& cmd, const std::vector<std::string>& nonExistDir, bool lbreak) const {
    for (const auto& dir : nonExistDir) {
        cerr << cmd << ": " << dir << ": No such file or directory" << endl;
        if ( lbreak ) cerr << endl << endl;
    }
}

void errorMgr::setErrCmd(const std::string& cmd) const {
    ErrorCMD = cmd;
}

void errorMgr::setErrOpt(const std::string& optstr) const {
    if ( ErrorOPT.find(optstr) == ErrorOPT.end() ) {
        ErrorOPT.insert(optstr);
    }
}

void errorMgr::setErrOpt(const char& optchar) const {
    std::string tmp;
    tmp.resize(1);
    tmp.back() = optchar;
    this->setErrOpt(tmp);
}

void errorMgr::setErrHndlr(const cmdExec* handler) const {
    ErrorHandler = handler;
}

void errorMgr::setErrArg(const std::string& arg) const {
    ErrorArg = arg;
}

void errorMgr::cmdError() {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Command Error: " << ErrorCMD << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::cmdOptIllegal() {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Illegal Option for \'" << ErrorHandler->getKeyWord() << ErrorHandler->getOptional() << "\':";
    for (const auto& opt : ErrorOPT) cerr << " -" << opt;
    if ( _colorful ) cerr << COLOR_RESET;
    cerr << endl;
}

void errorMgr::cmdExecError() {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << ErrorCMD << ": " << ErrorArg << ": " << strerror(errno) << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::cmdArgTooMany() {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << ErrorCMD << ": Too many arguments" << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

/********************
 * for sftp session *
 *******************/
void errorMgr::setSftpErr(const char* hostIP, const char* errmsg) const {
    ErrorHostIP  = hostIP;
    ErrorSftpMsg = errmsg;
}

void errorMgr::setSftpErr(const char* errmsg) const {
    ErrorSftpMsg = errmsg;
}

void errorMgr::sftpSshConnectError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Error connecting to " << ErrorHostIP << ": " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpSshAllocError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Could not allocate ssh session: " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpSessAllocError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Could not allocate a sftp session: " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpSessInitError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Could not initialize a sftp session: " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpAuthError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "A serious error happened. Please try again later." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpAuthUndefError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Caught undefined error. Please try again later or use another method for authentication." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyPubKeyError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Unable to get public key from server." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyPubHashError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Unable to get key hash from server." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyHostOtherError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "The host key for this server was not found but an other type of key exists." << endl;
    cerr << "An attacker might change the default server key to confuse your client into thinking the key does not exist" << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyStdinError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Stdin error. Please type \'yes\' or \'no\'." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyHostConnectDefined() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Connection defined by user." << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyUpdateError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Unable to update known hosts list: " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}

void errorMgr::sftpVrfyKnownHostError() const {
    if ( _colorful ) cerr << BOLD_RED;
    cerr << "Known Host Error: " << ErrorSftpMsg << endl;
    if ( _colorful ) cerr << COLOR_RESET;
}
