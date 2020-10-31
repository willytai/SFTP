#include "cmdClass.h"

extern errorMgr errMgr;


/*********/
/* lsCmd */
/*********/
cmdStat lsCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void lsCmd::usage() const {
    return;
}

void lsCmd::help() const {
    return;
}

/*********/
/* cdCmd */
/*********/
// TODO: support "cd old new"
//       this part is pretty bad, copying strings at most three times
cmdStat cdCmd::execute(const std::string& option) const {
    vecstr tokens;
    UTIL::parseTokens(option, tokens);
    if ( tokens.size() > 1 ) {
        errMgr.setErrCmd("cd");
        return CMD_ARG_TOO_MANY;
    }
    std::string target = tokens.size() == 0 ? "" : tokens[0];
    if ( (*_sftp_sess_ptr)->cd(target) != sftp::SFTP_OK ) {
        errMgr.setErrCmd("cd");
        errMgr.setErrArg( target );
        return CMD_EXEC_ERROR;
    }
    else return CMD_DONE;
    return CMD_DONE;
}

void cdCmd::usage() const {
    return;
}

void cdCmd::help() const {
    return;
}

/************/
/* mkdirCmd */
/************/
cmdStat mkdirCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void mkdirCmd::usage() const {
    return;
}

void mkdirCmd::help() const {
    return;
}

/*********/
/* rmCmd */
/*********/
cmdStat rmCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void rmCmd::usage() const {
    return;
}

void rmCmd::help() const {
    return;
}

/************/
/* chmodCmd */
/************/
cmdStat chmodCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void chmodCmd::usage() const {
    return;
}

void chmodCmd::help() const {
    return;
}
