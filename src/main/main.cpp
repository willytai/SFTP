#include <stdio.h>
#include <stdlib.h>
#include "sftpSession.h"
#include "cmdParser.h"

errorMgr errMgr;

int main(int argc, char** argv) {
    // colorful output is disabled for default
    // the entire output status will be referenced from errMgr
    errMgr.setColorfulOutput();
    cmdParser* cmdMgr = new cmdParser("sftp");
    cmdMgr->parse(argc, argv);
    delete cmdMgr;
    return 0;
}
