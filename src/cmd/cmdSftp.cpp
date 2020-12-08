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

    cmdStat returnStat = CMD_DONE;
    if ( (*_sftp_sess_ptr)->get( stargets ) != sftp::SFTP_OK ) {
        // set errorno properly in the get() function of sftpSession
        errMgr.setErrCmd("get");
        returnStat = CMD_EXEC_ERROR;
    }
    if ( (*_sftp_sess_ptr)->get_recursive( rtargets ) != sftp::SFTP_OK ) {
        errMgr.setErrCmd("get");
        returnStat = CMD_EXEC_ERROR;
    }
    return returnStat;
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

    cmdStat returnStat = CMD_DONE;
    if ( (*_sftp_sess_ptr)->put( stargets ) != sftp::SFTP_OK ) {
        // set errorno properly in the put() function of sftpSession
        errMgr.setErrCmd("put");
        returnStat = CMD_EXEC_ERROR;
    }
    if ( (*_sftp_sess_ptr)->put_recursive( rtargets ) != sftp::SFTP_OK ) {
        errMgr.setErrCmd("put");
        returnStat = CMD_EXEC_ERROR;
    }
    return returnStat;
}

void putCmd::usage() const {
    return;
}

void putCmd::help() const {
    return;
}
