#include "cmdParser.h"
#include <set>
#include <string>

static std::set<std::string> ErrorOPT;

static std::string ErrorCMD;
static std::string ErrorArg;
static const cmdExec* ErrorHandler;

void errorMgr::handle(const cmdStat& errCode) {
    switch (errCode) {
        case CMD_ERROR:       this->cmdError();      break;
        case CMD_OPT_ILLEGAL: this->cmdOptIllegal(); break;
        case CMD_EXEC_ERROR:  this->cmdExecError();  break;
        default: return;
    }
    ErrorOPT.clear();
}

void errorMgr::handleNonExistDir(const std::string& cmd, const std::vector<std::string>& nonExistDir, bool lbreak) const {
    for (const auto& dir : nonExistDir) {
        cerr << cmd << ": " << dir << ": No such file or directory" << endl;
        if ( lbreak ) cerr << endl << endl;
    }
}

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

void errorMgr::setErrArg(const std::string& a) const {
    ErrorArg = a;
}

void errorMgr::cmdError() {
    if ( _pretty ) cerr << BOLD_RED;
    cerr << "Command Error: " << ErrorCMD << endl;
    if ( _pretty ) cerr << COLOR_RESET;
}

void errorMgr::cmdOptIllegal() {
    if ( _pretty ) cerr << BOLD_RED;
    cerr << "Illegal Option for \'" << ErrorHandler->getKeyWord() << ErrorHandler->getOptional() << "\':";
    for (const auto& opt : ErrorOPT) cerr << " -" << opt;
    if ( _pretty ) cerr << COLOR_RESET;
    cerr << endl;
}

void errorMgr::cmdExecError() {
    std::string msg = ErrorCMD+": "+ErrorArg;
    if ( _pretty ) cerr << BOLD_RED;
    perror(msg.c_str());
    if ( _pretty ) cerr << COLOR_RESET;
}
