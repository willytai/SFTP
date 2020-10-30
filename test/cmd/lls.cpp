#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../testHelper.h"
#include "cmdParser.h"

errorMgr errMgr;

TEST_CASE( "Error Message", "[lls]" )
{
    cmdParser* cmdMgr = new cmdParser("err");

    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    // define macro for all test sections
    # define TEST_SECTION(TCASE, ERROR)                                                             \
    SECTION( "["#TCASE"] "#ERROR)                                                                   \
    {                                                                                               \
        cerrToString(&cerrBuf);                                                                     \
        coutToString(&coutBuf);                                                                     \
        cmdMgr->setInFileName("./lls/"#TCASE".in");                                                 \
        cmdMgr->readFile();                                                                         \
        REQUIRE( readGolden("./lls/"#TCASE".out", goldenBuffer) );                                  \
        REQUIRE( goldenBuffer.size() == cerrBuf.str().size() );                                     \
        REQUIRE( strncmp(cerrBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );  \
        cerrReset(&cerrBuf);                                                                        \
        coutReset(&coutBuf);                                                                        \
    }

    TEST_SECTION( err1, not file or directory error);
    TEST_SECTION( err2, option error);

    delete cmdMgr;
    #undef TEST_SECTION
}
TEST_CASE( "Listing Output", "[lls]" )
{

    /***********************************
     * the -la option cannot be checked
     * becuase the modification time of
     * "../" folder is always changing
     ***********************************/

    cmdParser* cmdMgr = new cmdParser("sftp");

    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    // define macro for all test sections
    #define TEST_SECTION(T)                                                                         \
    SECTION( "["#T"] single directory" )                                                            \
    {                                                                                               \
        cerrToString(&cerrBuf);                                                                     \
        coutToString(&coutBuf);                                                                     \
        cmdMgr->setInFileName("./lls/"#T".in");                                                     \
        cmdMgr->readFile();                                                                         \
        UNSCOPED_INFO( "can't open golden answer" );                                                \
        CHECK( readGolden("./lls/"#T".out", goldenBuffer) );                                        \
        UNSCOPED_INFO( "\033[36m-------------------- result start --------------------\033[37m" );  \
        UNSCOPED_INFO( coutBuf.str().c_str() );                                                     \
        UNSCOPED_INFO( "\033[36m--------------------  result end  --------------------\033[37m" );  \
        UNSCOPED_INFO( "\033[36m-------------------- golden start --------------------\033[37m" );  \
        UNSCOPED_INFO( goldenBuffer.c_str() );                                                      \
        UNSCOPED_INFO( "\033[36m--------------------  golden end  --------------------\033[32m" );  \
        CHECK( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );    \
        cerrReset(&cerrBuf);                                                                        \
        coutReset(&coutBuf);                                                                        \
    }

    TEST_SECTION(lls1);
    TEST_SECTION(lls2);
    TEST_SECTION(lls3);
    TEST_SECTION(lls4);
    TEST_SECTION(lls5);
    TEST_SECTION(lls6);
    TEST_SECTION(lls7);
    TEST_SECTION(lls8);

    delete cmdMgr;
    #undef TEST_SECTION
}
