#include <stdio.h>
#include <stdlib.h>
#include "global_var.h"
#include "sftpSession.h"
#include "cmdParser.h"
#include "memUtil.h"

errorMgr errMgr;

int main(int argc, char** argv) {
    // colorful output is disabled for default
    // the entire output status will be referenced from errMgr
    errMgr.setColorfulOutput();
    cmdParser* cmdMgr = new cmdParser("sftp");
    cmdMgr->parse(argc, argv);
    delete cmdMgr;
    memUtil::MemUsage::usage();
    memUtil::MemUsage::leakCheck();
    return 0;
}
