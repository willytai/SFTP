#include <stdio.h>
#include <stdlib.h>
#include "sftpSession.h"
#include "cmdParser.h"

int main(int argc, char *argv[]) {
    // TODO: commandline reader/parser is next

    cmdParser* parser = new cmdParser("sftp");
    parser->readCmd();
    delete parser;
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
