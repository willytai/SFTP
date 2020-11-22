#include "cmdClass.h"

extern errorMgr errMgr;

/*********/
/* getCmd */
/*********/
cmdStat getCmd::execute(const std::string& option) const {
    std::vector<std::string> targets, stargets, rtargets;

    // check for single targets and recursive targets
    // TODO this is just an naive implementation
    UTIL::parseTokens( option, targets );
    std::string flags;
    bool r = false;
    for (const auto& t : targets) {
        if ( t[0] == '-' ) {
            r = t.size() > 1 && t[1] == 'r';
            flags += t.substr(1);
        }
        if ( !r ) stargets.push_back(t);
        else      rtargets.push_back(t);
    }

    // option check
    bool valid_opt = true;
    for (const auto& f : flags) {
        if ( f != 'r' ) {
            errMgr.setErrOpt(f);
            valid_opt = false;
        }
    }
    if ( !valid_opt ) {
        errMgr.setErrHndlr(this);
        return CMD_OPT_ILLEGAL;
    }

    (*_sftp_sess_ptr)->get( stargets );
    (*_sftp_sess_ptr)->get_recursive( rtargets );
    return CMD_DONE;
}

void getCmd::usage() const {
    // -r only applies for files after this flag
    return;
}

void getCmd::help() const {
    return;
}

/*********/
/* putCmd */
/*********/
cmdStat putCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void putCmd::usage() const {
    return;
}

void putCmd::help() const {
    return;
}
