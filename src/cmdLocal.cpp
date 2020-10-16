#include "cmdClass.h"

/**********/
/* llsCmd */
/**********/
cmdStat llsCmd::execute(const std::string& option) const {
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
