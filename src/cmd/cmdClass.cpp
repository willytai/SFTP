#include "cmdClass.h"


/***************************************/
/* register them to the command parser */
/***************************************/
// this is the only function of cmdParser that is implemented here
void cmdParser::regCmd() {
    if ( this->regEachCmd("lls",   3, new llsCmd())   == CMD_REG_FAIL ||
         this->regEachCmd("lcd",   3, new lcdCmd())   == CMD_REG_FAIL ||
         this->regEachCmd("ls",    2, new lsCmd())    == CMD_REG_FAIL ||
         this->regEachCmd("cd",    2, new cdCmd())    == CMD_REG_FAIL ||
         this->regEachCmd("mkdir", 5, new mkdirCmd()) == CMD_REG_FAIL ||
         this->regEachCmd("clear", 5, new clearCmd()) == CMD_REG_FAIL ||
         this->regEachCmd("chmod", 5, new chmodCmd()) == CMD_REG_FAIL ||
         this->regEachCmd("quit",  1, new quitCmd())  == CMD_REG_FAIL ||
         this->regEachCmd("help",  1, new helpCmd())  == CMD_REG_FAIL ||
         this->regEachCmd("put",   1, new putCmd())   == CMD_REG_FAIL ||
         this->regEachCmd("get",   1, new getCmd())   == CMD_REG_FAIL  ) {
        fprintf(stderr, "Command registration failed.\n");
        exit(-1);
    }
}

/**************************************************************************/
/* The commands are implemented in cmdLocal.cpp/cmdServer.cpp/cmdSftp.cpp */
/**************************************************************************/
