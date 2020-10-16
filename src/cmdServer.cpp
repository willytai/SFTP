#include "cmdClass.h"


/*********/
/* lsCmd */
/*********/
cmdStat lsCmd::execute(const std::string& option) const {
    return CMD_DONE;
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
cmdStat cdCmd::execute(const std::string& option) const {
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
cmdStat mkdirCmd::execute(const std::string& option) const {
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
cmdStat rmCmd::execute(const std::string& option) const {
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
cmdStat chmodCmd::execute(const std::string& option) const {
    return CMD_DONE;
}

void chmodCmd::usage() const {
    return;
}

void chmodCmd::help() const {
    return;
}
