#include "cmdClass.h"
#include "util.h"
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

    // check options
    bool illegalOpt = false;
    this->resetFlag();
    for (const auto& c : flags) {
        LIST::lsFlag f = LIST::getFlag(c);
        if ( f == LIST::UNDEF_FLAG ) {
            illegalOpt = true;
            errMgr.setErrOpt(c);
        }
        else {
            _flags |= f;
        }
    }
    if ( illegalOpt ) {
        errMgr.setErrHndlr(this);
        return CMD_OPT_ILLEGAL;
    }

    // check directories
    // leave queryDir with the directories that doesn't exist
    LIST::dirCntMap dirContent;
    size_t curCheck = 0;
    while ( queryDir.size() && curCheck < queryDir.size() ) {
        auto& dir = queryDir[curCheck];
        auto check = dirContent.emplace(dir, LIST::Files());
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
            cerr << this->getCmdStr() << ": " << dir << ": No such file or directory" << endl;
            if ( dirContent.size() ) cerr << endl << endl;
        }
    }

    // determine the print method
    cmdStat returnStat = CMD_EXEC_ERROR;
    bool human = LIST::checkFlag(LIST::HUMAN_READABLE, _flags);
    bool all   = LIST::checkFlag(LIST::LIST_ALL,       _flags);
    if ( LIST::checkFlag(LIST::LIST_LONG, _flags) ) {
        if ( LIST::listPrint(dirContent, all, human, nonExistDir.size()>0 || dirContent.size()>1, "lls") ) {
            returnStat = CMD_DONE;
        }
    }
    else {
        if ( LIST::columnPrint(dirContent, all, human, nonExistDir.size()>0 || dirContent.size()>1, "lls") ) {
            returnStat = CMD_DONE;
        }
    }

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
cmdStat lcdCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void lcdCmd::usage() const {
    return;
}

void lcdCmd::help() const {
    return;
}
