#include "cmdParser.h"

static std::vector<std::string> ErrorOPT;

static std::string ErrorCMD;
static const cmdExec* ErrorHandler;
static const char* ErrorEntry;
static const char* ErrorDir;

void errorMgr::setErrCmd(const std::string& s) const {
    ErrorCMD = s;
}

void errorMgr::setErrOpt(const std::string& s) const {
    ErrorOPT.push_back(s);
}

void errorMgr::setErrOpt(const char& c) const {
    ErrorOPT.resize(ErrorOPT.size()+1);
    ErrorOPT.back().push_back(c);
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
    cerr << "Command Error: " << ErrorCMD << endl;
}

void errorMgr::cmdOptIllegal() {
    cerr << "Illegal Option for \'" << ErrorHandler->getKeyWord() << ErrorHandler->getOptional() << "\':";
    for (const auto& opt : ErrorOPT) cerr << " -" << opt; cerr << endl;
}

void errorMgr::cmdExecError() {
    std::string msg = std::string(ErrorCMD)+": "+std::string(ErrorEntry)+" in "+std::string(ErrorDir);
    if ( msg.back() != '/' ) msg.push_back('/');
    perror(msg.c_str());
}
