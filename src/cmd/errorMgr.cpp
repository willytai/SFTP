#include "cmdParser.h"
#include <set>

static std::set<std::string> ErrorOPT;

static std::string ErrorCMD;
static const cmdExec* ErrorHandler;
static const char* ErrorEntry;
static const char* ErrorDir;

void errorMgr::setErrCmd(const std::string& s) const {
    ErrorCMD = s;
}

void errorMgr::setErrOpt(const std::string& s) const {
    if ( ErrorOPT.find(s) == ErrorOPT.end() ) {
        ErrorOPT.insert(s);
    }
}

void errorMgr::setErrOpt(const char& c) const {
    std::string tmp;
    tmp.resize(1);
    tmp.back() = c;
    this->setErrOpt(tmp);
}

void errorMgr::setErrHndlr(const cmdExec* h) const {
    ErrorHandler = h;
}

void errorMgr::setErrEntryAndDir(const char* e, const char* d) const {
    ErrorEntry = e;
    ErrorDir   = d;
}

void errorMgr::handle(const cmdStat& errCode) {
    switch (errCode) {
        case CMD_ERROR:       this->cmdError();      break;
        case CMD_OPT_ILLEGAL: this->cmdOptIllegal(); break;
        case CMD_EXEC_ERROR:  this->cmdExecError();  break;
        default: return;
    }
    ErrorOPT.clear();
}

void errorMgr::cmdError() {
    if ( _pretty ) cerr << BOLD_RED;
    cerr << "Command Error: " << ErrorCMD << endl;
    if ( _pretty ) cerr << COLOR_RESET;
}

void errorMgr::cmdOptIllegal() {
    if ( _pretty ) cerr << BOLD_RED;
    cerr << "Illegal Option for \'" << ErrorHandler->getKeyWord() << ErrorHandler->getOptional() << "\':";
    for (const auto& opt : ErrorOPT) cerr << " -" << opt; cerr << endl;
    if ( _pretty ) cerr << COLOR_RESET;
}

void errorMgr::cmdExecError() {
    std::string msg = std::string(ErrorCMD)+": "+std::string(ErrorEntry)+" in "+std::string(ErrorDir);
    if ( msg.back() != '/' ) msg.push_back('/');
    perror(msg.c_str());
}
