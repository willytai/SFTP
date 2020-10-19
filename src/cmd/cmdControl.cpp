#include "cmdClass.h"

/***********/
/* quitCmd */
/***********/
cmdStat quitCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void quitCmd::usage() const {
    return;
}

void quitCmd::help() const {
    return;
}
