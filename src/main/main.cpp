#include <stdio.h>
#include <stdlib.h>
#include "sftpSession.h"
#include "cmdParser.h"

errorMgr errMgr;

int main(int argc, char *argv[]) {
    cmdParser* cmdMgr = new cmdParser("sftp");
    cmdMgr->regCmd();
    if ( argc == 2) {
        cmdMgr->readFile(argv[1]);
    }
    else {
        while ( cmdMgr->readCmd() != CMD_EXIT ) {}
    }
    delete cmdMgr;
    return 0;

    char* hostIP;
    if (argc < 2) {
        perror("option error");
        return 0;
    }
    else {
        hostIP = argv[1];
    }
    sftpSession sess(hostIP, "willytai43", "09855184");
    return 0;
}
