#include "cmdClass.h"
#include "dirPrinter.h"
#include "util.h"
#include "dirIO.h"
#include <unordered_map>
#include <vector>

extern errorMgr errMgr;

/**********/
/* llsCmd */
/**********/
cmdStat llsCmd::execute(const std::string& option) const {
    /*************************************************
     * -- Split with ' '
     * -- Any tokens that doesn't start with '-' will
     *    be treated as the to-be-listed directories
     * -- Characters that follow '-' will be stored
     *    and checked
     **************************************************/

    // parse tokens
    std::vector<std::string> tokens;
    UTIL::parseTokens(option, tokens);

    std::vector<std::string> queryDir;
    std::string flags;
    for (const auto& tok : tokens) {
        if ( tok[0] != '-' ) queryDir.push_back(tok);
        else flags += tok.substr(1);
    }

    // default listing directory
    if ( queryDir.size() == 0 ) queryDir.push_back("./");

    // the printer
    LISTING::Printer* dirPrinter = new LISTING::Printer();

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
        else if ( UTIL::readDir(dir.c_str(), check.first->second) ) {
            std::swap(dir, queryDir.back());
            queryDir.pop_back();
        }
        else {
            dirContent.erase(check.first);
            ++curCheck;
        }
    }

    // queryDir now contains non-existing dirs
    // error msg
    auto& nonExistDir = queryDir;
    if ( nonExistDir.size() ) {
        for (const auto& dir : nonExistDir) {
            // TODO let errMgr handle instead of explicitly implementing
            cerr << this->getCmdStr() << ": " << dir << ": No such file or directory" << endl;
            if ( dirContent.size() ) cerr << endl << endl;
        }
    }

    // whether to print the directory's name or not
    dirPrinter->setPrintDirName( nonExistDir.size()>0 || dirContent.size()>1 );

    cmdStat returnStat = CMD_DONE;
    if ( !dirPrinter->print( dirContent ) ) {
        errMgr.setErrCmd("lls");
        errMgr.setErrEntryAndDir( dirPrinter->getErrEntry(), dirPrinter->getErrDir() );
        returnStat = CMD_EXEC_ERROR;
    }
    delete dirPrinter;
    return returnStat;
}

void llsCmd::usage() const {
    // TODO: re-organize files and write test
    return;
}

void llsCmd::help() const {
    return;
}

/**********/
/* lcdCmd */
/**********/
cmdStat lcdCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void lcdCmd::usage() const {
    return;
}

void lcdCmd::help() const {
    return;
}
