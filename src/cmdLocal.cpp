#include "cmdClass.h"
#include "util.h"

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
    std::vector<std::string> tokens;
    UTIL::parseTokens(option, tokens);
    for (const auto& o : tokens) cout << o << endl;
    return CMD_DONE;
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
