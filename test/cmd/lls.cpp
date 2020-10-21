#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../testHelper.h"
#include "cmdParser.h"

// this is to avoid weird charaters from file

errorMgr errMgr;

TEST_CASE( "Error Message", "[lls]" )
{
    cmdParser* cmdMgr = new cmdParser("err");
    cmdMgr->regCmd();

    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    SECTION( "not file or directory error" )
    {
        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/err1.in");
        REQUIRE( readGolden("../lls/err1.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == cerrBuf.str().size() );
        REQUIRE( strncmp(cerrBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);
    }
    SECTION( "option error" )
    {
        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/err2.in");
        REQUIRE( readGolden("../lls/err2.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == cerrBuf.str().size() );
        REQUIRE( strncmp(cerrBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);
    }


    delete cmdMgr;
}
TEST_CASE( "Listing Output", "[lls]" )
{

    /***********************************
     * the -la option cannot be checked
     * becuase the modification time of
     * "../" folder is always changing
     ***********************************/

    cmdParser* cmdMgr = new cmdParser("sftp");
    cmdMgr->regCmd();

    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    SECTION( "listing one dir" )
    {
        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls1.in");
        REQUIRE( readGolden("../lls/lls1.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls2.in");
        REQUIRE( readGolden("../lls/lls2.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls3.in");
        REQUIRE( readGolden("../lls/lls3.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls4.in");
        REQUIRE( readGolden("../lls/lls4.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls5.in");
        REQUIRE( readGolden("../lls/lls5.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls6.in");
        REQUIRE( readGolden("../lls/lls6.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);

        cerrToString(&cerrBuf);
        coutToString(&coutBuf);
        cmdMgr->readFile("../lls/lls7.in");
        REQUIRE( readGolden("../lls/lls7.out", goldenBuffer) );
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );
        cerrReset(&cerrBuf);
        coutReset(&coutBuf);
    }

    delete cmdMgr;
}
