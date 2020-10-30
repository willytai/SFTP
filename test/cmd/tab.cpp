#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../testHelper.h"
#include "cmdParser.h"

errorMgr errMgr;

TEST_CASE( "tab press", "[cmdParser]")
{
    cmdParser* cmdMgr = new cmdParser("sftp");

    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    cerrToString(&cerrBuf);
    coutToString(&coutBuf);
    CHECK( readGolden("./tab/tab.out", goldenBuffer) );
    cmdMgr->setInFileName("./tab/tab.in");
    cmdMgr->readFile();
    UNSCOPED_INFO( "\033[36m-------------------- result start --------------------\033[37m" );
    UNSCOPED_INFO( coutBuf.str().c_str() );
    UNSCOPED_INFO( "\033[36m--------------------  result end  --------------------\033[37m" ); 
    UNSCOPED_INFO( "\033[36m-------------------- golden start --------------------\033[37m" );
    UNSCOPED_INFO( goldenBuffer.c_str() );
    UNSCOPED_INFO( "\033[36m--------------------  golden end  --------------------\033[32m" );
    CHECK( coutBuf.str().size() == goldenBuffer.size() );
    CHECK( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
    cerrReset(&cerrBuf);
    coutReset(&coutBuf);

    delete cmdMgr;
}
