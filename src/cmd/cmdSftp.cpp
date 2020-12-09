#include "cmdClass.h"
#include "flags.h"
#include <algorithm>

extern errorMgr errMgr;

/***********************************************
* recognized options for commands in the file: *
*   -f/--force                                 *
*   -r/--recursive                             *
***********************************************/
// helper function for option checking
static bool checkOpts(std::vector<std::string_view>& tokens, const cmdExec* cmdPlacehoder) {
    bool returnStat = true;
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        auto& t = *it;
        if ( t.size() < 2 ) continue;
        if ( t.front() != '-' ) continue;
        if ( UTIL::strcmp( t, "-f", 2 ) == 0 ) {
            cmdPlacehoder->setFlag( INT_FLAG_FORCE );
        }
        else if ( UTIL::strcmp( t, "--force", 7 ) == 0 ) {
            cmdPlacehoder->setFlag( INT_FLAG_FORCE );
        }
        else if ( UTIL::strcmp( t, "-r", 2 ) == 0 ) {
            cmdPlacehoder->setFlag( INT_FLAG_RECURSIVE );
        }
        else if ( UTIL::strcmp( t, "--recursive", 11 ) == 0 ) {
            cmdPlacehoder->setFlag( INT_FLAG_RECURSIVE );
        }
        else {
            t.remove_prefix( t.find_first_not_of("-") );
            errMgr.setErrOpt( t );
            returnStat = false;
        }
        // the order cannot be changed, so simply deleting element despite its inefficiency
        tokens.erase( it );
        --it;
    }
    return returnStat;
}

/**********/
/* getCmd */
/**********/
// this command can only accpet signle target
cmdStat getCmd::execute(const std::string& option) const {
    std::vector<std::string_view> tokens;
    UTIL::parseTokens( option, tokens );
    if ( !checkOpts( tokens, this ) ) {
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
        errMgr.setErrCmd("get");
        return CMD_ARG_TOO_MANY;
    }

    // duplicate the name if tartget name not provided
    if ( tokens.size() == 1 ) {
        tokens.push_back( tokens.front() );
    }

    cmdStat returnStat = CMD_DONE;
    if ( this->checkFlag(INT_FLAG_RECURSIVE) ) {
        if ( (*_sftp_sess_ptr)->get_recursive(tokens[0], tokens[1], this->checkFlag(INT_FLAG_FORCE)) != sftp::SFTP_OK ) {
            errMgr.setErrCmd("get");
            returnStat = CMD_EXEC_ERROR;
        }
    }
    else {
        if ( (*_sftp_sess_ptr)->get(tokens[0], tokens[1], this->checkFlag(INT_FLAG_FORCE)) != sftp::SFTP_OK ) {
            // set errorno properly in the get() function of sftpSession
            errMgr.setErrCmd("get");
            returnStat = CMD_EXEC_ERROR;
        }
    }
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


/***********/
/* mputCmd */
/***********/
cmdStat mputCmd::execute(const std::string& option) const {
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
cmdStat mgetCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void mgetCmd::usage() const {
    return;
}

void mgetCmd::help() const {
    return;
}
