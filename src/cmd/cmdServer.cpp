#include "cmdClass.h"
#include "dirPrinter.h"

extern errorMgr errMgr;


/*********/
/* lsCmd */
/*********/
// TODO support single-file listing and wildcard listing
cmdStat lsCmd::execute(const std::string_view& option) const {
    /*************************************************
     * -- Split with ' '
     * -- Any tokens that doesn't start with '-' will
     *    be treated as the to-be-listed directories
     * -- Characters that follow '-' will be stored
     *    and checked
     **************************************************/

    // parse tokens
    vecstr_view tokens;
    UTIL::parseTokens(option, tokens);

    vecstr_view queryDir;
    std::string flags;
    for (const auto& tok : tokens) {
        if ( tok[0] != '-' ) queryDir.push_back(tok);
        else flags += tok.substr(1);
    }

    // default listing directory
    if ( queryDir.size() == 0 ) queryDir.emplace_back( DEFAULT_DIR );

    // the printer
    sftp::Printer* dirPrinter = new sftp::Printer( errMgr.colorOutput() );

    // check options
    // only support single char flags for now
    for (const auto& c : flags) {
        if ( !dirPrinter->setFlag(c) ) {
            errMgr.setErrOpt(c);
        }
    }
    if ( dirPrinter->illegal() ) {
        errMgr.setErrHndlr(this);
        return CMD_OPT_ILLEGAL;
    }

    // check directories
    // leave queryDir with the directories that doesn't exist
    sftp::dirCntMap dirContent;
    size_t curCheck = 0;
    while ( queryDir.size() && curCheck < queryDir.size() ) {
        auto& dir = queryDir[curCheck];
        auto check = dirContent.emplace(dir, sftp::Files());
        if ( !check.second ) { // ignore duplicate entries
            std::swap(dir, queryDir.back());
            queryDir.pop_back();
        }
        else if ( (*_sftp_sess_ptr)->readDir(dir, check.first->second) == sftp::SFTP_OK ) {
            std::swap(dir, queryDir.back());
            queryDir.pop_back();
        }
        else {
            dirContent.erase(check.first);
            ++curCheck;
        }
    }

    // -- queryDir now contains non-existing dirs
    // -- becuase the error is already handled here, the command execution
    //    returns CMD_DONE unless something wrong happens afterward
    auto& nonExistDir = queryDir;
    errMgr.handleNonExistDir( this->getCmdStr(), nonExistDir, dirContent.size()>0 );

    // whether to print the directory's name or not
    dirPrinter->setPrintDirName( nonExistDir.size()>0 || dirContent.size()>1 );

    // not sure whether this would happen or not
    cmdStat returnStat = CMD_DONE;
    if ( !dirPrinter->print( dirContent ) ) {
        errMgr.setErrCmd("ls");
        errMgr.setErrArg( dirPrinter->getErrDir() );
        returnStat = CMD_EXEC_ERROR;
    }
    else {
        // TODO do this in dirPrinter (NECESSARY)
        for (auto& pair : dirContent) {
            for (auto& attr : pair.second) {
                sftp_attributes_free( attr );
            }
        }
    }
    delete dirPrinter;
    return returnStat;
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
cmdStat cdCmd::execute(const std::string_view& option) const {
    vecstr_view tokens;
    UTIL::parseTokens(option, tokens);
    if ( tokens.size() > 1 ) {
        errMgr.setErrCmd("cd");
        return CMD_ARG_TOO_MANY;
    }
    const std::string_view& target = tokens.size() == 0 ? "" : tokens[0];
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
cmdStat mkdirCmd::execute(const std::string_view& option) const {
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
cmdStat rmCmd::execute(const std::string_view& option) const {
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
cmdStat chmodCmd::execute(const std::string_view& option) const {
    return CMD_DONE;
}

void chmodCmd::usage() const {
    return;
}

void chmodCmd::help() const {
    return;
}
