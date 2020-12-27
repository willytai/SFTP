#include "cmdClass.h"
#include "flags.h"
#include <algorithm>

extern errorMgr errMgr;

/***********************************************
* recognized options for commands in the file: *
*   -f/--force                                 *
*   -r/--recursive                             *
***********************************************/

/**********/
/* getCmd */
/**********/
// this command can only accpet signle target
// TODO support flags concatenation
cmdStat getCmd::execute(const std::string_view& option) const {
    std::vector<std::string_view> tokens;
    UTIL::parseTokens( option, tokens );
    if ( !flags::checkOpts( tokens, this ) ) {
        errMgr.setErrHndlr(this);
        return CMD_OPT_ILLEGAL;
    }

    // not files specified
    if ( tokens.size() == 0 ) {
        cout << "Specify a file/folder to fetch." << endl;
        this->usage();
        return CMD_DONE;
    }

    // to many arguments
    if ( tokens.size() > 2 ) {
        errMgr.setErrCmd("get");
        return CMD_ARG_TOO_MANY;
    }

    // duplicate the name if tartget name not provided
    // get the last string after '/'
    if ( tokens.size() == 1 ) {
        tokens.push_back( UTIL::find_last( tokens.front(), '/') );
    }

    cmdStat returnStat = CMD_DONE;
    if ( this->checkFlag(flags::intflag::RECURSIVE) ) {
        if ( (*_sftp_sess_ptr)->get_recursive(tokens[0], tokens[1], this->checkFlag(flags::intflag::FORCE)) != sftp::SFTP_OK ) {
            errMgr.setErrCmd("get");
            returnStat = CMD_EXEC_ERROR;
        }
    }
    else {
        if ( (*_sftp_sess_ptr)->get(tokens[0], tokens[1], this->checkFlag(flags::intflag::FORCE)) != sftp::SFTP_OK ) {
            // set errorno properly in the get() function of sftpSession
            errMgr.setErrCmd("get");
            returnStat = CMD_EXEC_ERROR;
        }
    }

    // remember to reset the flags
    this->resetFlag();
    return returnStat;
}

void getCmd::usage() const {
    return;
}

void getCmd::help() const {
    // -r only applies for files after this flag
    return;
}

/**********/
/* putCmd */
/**********/
cmdStat putCmd::execute(const std::string_view& option) const {
    std::vector<std::string_view> tokens;
    UTIL::parseTokens( option, tokens );
    if ( !flags::checkOpts( tokens, this ) ) {
        errMgr.setErrHndlr(this);
        return CMD_OPT_ILLEGAL;
    }

    // not files specified
    if ( tokens.size() == 0 ) {
        cout << "Specify a file/folder to transmit." << endl;
        this->usage();
        return CMD_DONE;
    }

    // to many arguments
    if ( tokens.size() > 2 ) {
        errMgr.setErrCmd("put");
        return CMD_ARG_TOO_MANY;
    }

    // duplicate the name if tartget name not provided
    // get the last string after '/'
    if ( tokens.size() == 1 ) {
        tokens.push_back( UTIL::find_last( tokens.front(), '/') );
    }

    cmdStat returnStat = CMD_DONE;
    if ( this->checkFlag(flags::intflag::RECURSIVE) ) {
        if ( (*_sftp_sess_ptr)->put_recursive(tokens[0], tokens[1], this->checkFlag(flags::intflag::FORCE)) != sftp::SFTP_OK ) {
            errMgr.setErrCmd("put");
            returnStat = CMD_EXEC_ERROR;
        }
    }
    else {
        if ( (*_sftp_sess_ptr)->put(tokens[0], tokens[1], this->checkFlag(flags::intflag::FORCE)) != sftp::SFTP_OK ) {
            // set errorno properly in the put() function of sftpSession
            errMgr.setErrCmd("put");
            returnStat = CMD_EXEC_ERROR;
        }
    }

    // remember to reset the flags
    this->resetFlag();
    return returnStat;
}

void putCmd::usage() const {
    return;
}

void putCmd::help() const {
    return;
}


/***********/
/* mputCmd */
/***********/
cmdStat mputCmd::execute(const std::string_view& option) const {
    return CMD_DONE;
}

void mputCmd::usage() const {
    return;
}

void mputCmd::help() const {
    return;
}


/***********/
/* mgetCmd */
/***********/
cmdStat mgetCmd::execute(const std::string_view& option) const {
    return CMD_DONE;
}

void mgetCmd::usage() const {
    return;
}

void mgetCmd::help() const {
    return;
}
