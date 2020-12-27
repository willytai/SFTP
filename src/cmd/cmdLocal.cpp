#include "cmdClass.h"
#include "dirPrinter.h"
#include "util.h"
#include "dirIO.h"
#include <unistd.h>
#include <unordered_map>
#include <vector>

extern errorMgr errMgr;

/**********/
/* llsCmd */
/**********/
// TODO support single-file listing and wildcard listing
cmdStat llsCmd::execute(const std::string_view& option) const {
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
    LISTING::Printer* dirPrinter = new LISTING::Printer( errMgr.colorOutput() );

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
    LISTING::dirCntMap dirContent;
    size_t curCheck = 0;
    while ( queryDir.size() && curCheck < queryDir.size() ) {
        auto& dir = queryDir[curCheck];
        auto check = dirContent.emplace(dir, LISTING::Files());
        if ( !check.second ) { // ignore duplicate entries
            std::swap(dir, queryDir.back());
            queryDir.pop_back();
        }
        else if ( UTIL::readDir(dir, check.first->second) ) {
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
        errMgr.setErrCmd("lls");
        errMgr.setErrArg( dirPrinter->getErrDir() );
        returnStat = CMD_EXEC_ERROR;
    }
    delete dirPrinter;
    return returnStat;
}

void llsCmd::usage() const {
    return;
}

void llsCmd::help() const {
    return;
}

/**********/
/* lcdCmd */
/**********/
// TODO: support "cd old new"
cmdStat lcdCmd::execute(const std::string_view& option) const {
    vecstr_view tokens;
    UTIL::parseTokens(option, tokens);
    if ( tokens.size() > 1 ) {
        errMgr.setErrCmd("lcd");
        return CMD_ARG_TOO_MANY;
    }

    int stat;
    if ( !tokens.size() ) {
        stat = chdir( _home );
    }
    else {
        std::unique_ptr<char[]> char_dir = std::make_unique<char[]>(tokens[0].size()+1);
        memcpy( char_dir.get(), tokens[0].data(), tokens[0].size() );
        char_dir[tokens[0].size()] = '\0';

        // check for escape characters
        char* nEscDir = UTIL::rmEscChar( char_dir.get() );
        const char* dir = nEscDir == NULL ? char_dir.get() : nEscDir;
        if ( nEscDir ) delete [] nEscDir;

        stat = chdir( dir );
    }

    // error handling
    if ( stat != 0 ) {
        errMgr.setErrCmd("lcd");
        errMgr.setErrArg( tokens[0] );
        return CMD_EXEC_ERROR;
    }
    else return CMD_DONE;
}

void lcdCmd::usage() const {
    return;
}

void lcdCmd::help() const {
    return;
}
