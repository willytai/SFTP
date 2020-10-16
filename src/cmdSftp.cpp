#include "cmdClass.h"


/*********/
/* getCmd */
/*********/
cmdStat getCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void getCmd::usage() const {
    return;
}

void getCmd::help() const {
    return;
}

/*********/
/* putCmd */
/*********/
cmdStat putCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void putCmd::usage() const {
    return;
}

void putCmd::help() const {
    return;
}
