#include "cmdClass.h"

/***********/
/* quitCmd */
/***********/
cmdStat quitCmd::execute(const std::string& option) const {
    // doesn't matter what follows the command, just return the exit status
    return CMD_EXIT;
}

void quitCmd::usage() const {
    return;
}

void quitCmd::help() const {
    return;
}

/***********/
/* clearCmd */
/***********/
cmdStat clearCmd::execute(const std::string& option) const {
    // doens't matter what follows the command, just clear the screen
    // -- clear from (J) top to (2) bottom
    // -- position cursor at row 1, col 1
    cout << "\033[2J\033[1;1H";
    return CMD_DONE;
}

void clearCmd::usage() const {
    return;
}

void clearCmd::help() const {
    return;
}

/***********/
/* helpCmd */
/***********/
cmdStat helpCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void helpCmd::usage() const {
    return;
}

void helpCmd::help() const {
    return;
}
