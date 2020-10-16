#include "cmdParser.h"

static std::string ErrorCMD;
static std::string ErrorOPT;

void errorMgr::setErrCmd(const std::string& s) const {
    ErrorCMD = s;
}

void errorMgr::setErrOpt(const std::string& s) const {
    ErrorOPT = s;
}

void errorMgr::handle(const cmdStat& errCode) {
    switch (errCode) {
        case CMD_ERROR: this->cmdError(errCode); break;
        default: break;
    }
}

void errorMgr::cmdError(const cmdStat& errCode) {
    cerr << "Command Error: " << ErrorCMD << endl;
}
