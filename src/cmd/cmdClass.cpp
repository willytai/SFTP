#include "cmdClass.h"


/***************************************/
/* register them to the command parser */
/***************************************/
// this is the only function of cmdParser that is implemented here
void cmdParser::regCmd() {
    if ( this->regEachCmd("lls",    3, new llsCmd())              == CMD_REG_FAIL ||
         this->regEachCmd("lcd",    3, new lcdCmd())              == CMD_REG_FAIL ||
         this->regEachCmd("clear",  5, new clearCmd())            == CMD_REG_FAIL ||
         this->regEachCmd("quit",   1, new quitCmd())             == CMD_REG_FAIL ||
         this->regEachCmd("help",   1, new helpCmd())             == CMD_REG_FAIL ||
         this->regEachCmd("remote", 6, new remoteCmd())           == CMD_REG_FAIL ||
         this->regEachCmd("local",  5, new localCmd())            == CMD_REG_FAIL ||
         this->regEachCmd("exit",   4, new exitCmd())             == CMD_REG_FAIL ||
         this->regEachCmd("usage",  5, new usageCmd())            == CMD_REG_FAIL ||
         this->regEachCmd("ls",     2, new lsCmd(&_sftp_sess))    == CMD_REG_FAIL ||
         this->regEachCmd("cd",     2, new cdCmd(&_sftp_sess))    == CMD_REG_FAIL ||
         this->regEachCmd("mkdir",  5, new mkdirCmd(&_sftp_sess)) == CMD_REG_FAIL ||
         this->regEachCmd("chmod",  5, new chmodCmd(&_sftp_sess)) == CMD_REG_FAIL ||
         this->regEachCmd("mput",   2, new mputCmd(&_sftp_sess))  == CMD_REG_FAIL ||
         this->regEachCmd("mget",   2, new mgetCmd(&_sftp_sess))  == CMD_REG_FAIL ||
         this->regEachCmd("put",    1, new putCmd(&_sftp_sess))   == CMD_REG_FAIL ||
         this->regEachCmd("get",    1, new getCmd(&_sftp_sess))   == CMD_REG_FAIL  ) {
        fprintf(stderr, "Command registration failed.\n");
        exit(-1);
    }
}

/*****************************************/
/* register the options for each command */
/*****************************************/
// since this will be called only on the initialization stage, a naive implementation is adopted
cmdStat cmdParser::regEachCmdOption(cmdExec* cmdHandler) {
    if ( dynamic_cast<llsCmd*>(cmdHandler) ) { // options for lls
        cmdHandler->setOptionFlags( "-l" );
        cmdHandler->setOptionFlags( "-a" );
        cmdHandler->setOptionFlags( "-h" );
#ifdef __APPLE__
        cmdHandler->setOptionFlags( "-@" );
#endif
    }
    else if ( dynamic_cast<lsCmd*>(cmdHandler) ) { // options for ls
        cmdHandler->setOptionFlags( "-l" );
        cmdHandler->setOptionFlags( "-a" );
        cmdHandler->setOptionFlags( "-h" );
    }
    else if ( dynamic_cast<putCmd*>(cmdHandler) ) { // options for put
        cmdHandler->setOptionFlags( "-f" );
        cmdHandler->setOptionFlags( "-r" );
        cmdHandler->setOptionFlags( "--force" );
        cmdHandler->setOptionFlags( "--recursive" );
    }
    else if ( dynamic_cast<getCmd*>(cmdHandler) ) { // options for get
        cmdHandler->setOptionFlags( "-f" );
        cmdHandler->setOptionFlags( "-r" );
        cmdHandler->setOptionFlags( "--force" );
        cmdHandler->setOptionFlags( "--recursive" );
    }
    else if ( dynamic_cast<mputCmd*>(cmdHandler) ) { // options for mput
        cmdHandler->setOptionFlags( "-f" );
        cmdHandler->setOptionFlags( "-r" );
        cmdHandler->setOptionFlags( "--force" );
        cmdHandler->setOptionFlags( "--recursive" );
    }
    else if ( dynamic_cast<mgetCmd*>(cmdHandler) ) { // options for mget
        cmdHandler->setOptionFlags( "-f" );
        cmdHandler->setOptionFlags( "-r" );
        cmdHandler->setOptionFlags( "--force" );
        cmdHandler->setOptionFlags( "--recursive" );
    }
    else if ( dynamic_cast<cdCmd*>(cmdHandler) ) { // options for cd
    }
    else if ( dynamic_cast<mkdirCmd*>(cmdHandler) ) { // options for mkdir
    }
    else if ( dynamic_cast<chmodCmd*>(cmdHandler) ) { // options for chmod
    }
    else if ( dynamic_cast<lcdCmd*>(cmdHandler) ) { // options for lcd
    }
    else if ( dynamic_cast<clearCmd*>(cmdHandler) ) { // options for clear
    }
    else if ( dynamic_cast<quitCmd*>(cmdHandler) ) { // options for quit
    }
    else if ( dynamic_cast<helpCmd*>(cmdHandler) ) { // options for help
    }
    else if ( dynamic_cast<remoteCmd*>(cmdHandler) ) { // options for remote
    }
    else if ( dynamic_cast<localCmd*>(cmdHandler) ) { // options for local
    }
    else if ( dynamic_cast<exitCmd*>(cmdHandler) ) { // options for exit
    }
    else if ( dynamic_cast<usageCmd*>(cmdHandler) ) { // options for usage
    }
    else {
        fprintf(stderr, "Caught unrecongnized command handler \'%s\', check %s line %d\n",
                cmdHandler->getCmdStr().c_str(),
                __FILE__,
                __LINE__);
        return CMD_REG_FAIL;
    }
    return CMD_DONE;
}

/**************************************************************************/
/* The commands are implemented in cmdLocal.cpp/cmdServer.cpp/cmdSftp.cpp */
/**************************************************************************/
