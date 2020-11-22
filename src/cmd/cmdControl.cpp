#include "cmdClass.h"
#include "Usage.h"
#include <iomanip>

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

/************/
/* clearCmd */
/************/
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

/*************/
/* remoteCmd */
/*************/
cmdStat remoteCmd::execute(const std::string& option) const {
    return CMD_CHANGE_REMOTE_SCOPE;
}

void remoteCmd::usage() const {
    return;
}

void remoteCmd::help() const {
    return;
}

/************/
/* localCmd */
/************/
cmdStat localCmd::execute(const std::string& option) const {
    return CMD_CHANGE_LOCAL_SCOPE;
}

void localCmd::usage() const {
    return;
}

void localCmd::help() const {
    return;
}

/***********/
/* exitCmd */
/***********/
cmdStat exitCmd::execute(const std::string& option) const {
    return CMD_EXIT;
}

void exitCmd::usage() const {
    return;
}

void exitCmd::help() const {
    return;
}

/************/
/* usageCmd */
/************/
cmdStat usageCmd::execute(const std::string& option) const {
    char* buf = (char*)malloc(16*sizeof(char));
    snprintf( buf, 16, "%.2f MB", Usage::getMem() );
    cout << "+-------------+" << endl;
    cout << "| Peak Memory |" << endl;
    cout << "+-------------+" << endl;
    cout << "| " << left << setw(11) << buf << " |"<< endl;
    cout << "+-------------+" << endl;
    free( buf );
    return CMD_DONE;
}

void usageCmd::usage() const {
    return;
}

void usageCmd::help() const {
    return;
}
