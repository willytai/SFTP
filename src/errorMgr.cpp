#include "cmdParser.h"

static std::string ErrorCMD;
static std::vector<std::string> ErrorOPT;
static const cmdExec* ErrorHandler;

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

void errorMgr::handle(const cmdStat& errCode) {
    switch (errCode) {
        case CMD_ERROR:       this->cmdError();      break;
        case CMD_OPT_ILLEGAL: this->cmdOptIllegal(); break;
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
