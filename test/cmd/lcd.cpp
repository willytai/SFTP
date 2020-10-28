#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../testHelper.h"
#include "cmdParser.h"

errorMgr errMgr;

TEST_CASE( "Error Message" , "[lcd]" )
{
    cmdParser* cmdMgr = new cmdParser("err");
    cmdMgr->regCmd();

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
        cmdMgr->readFile("./lcd/"#TCASE".in");                                                      \
        REQUIRE( readGolden("./lcd/"#TCASE".out", goldenBuffer) );                                  \
        REQUIRE( goldenBuffer.size() == cerrBuf.str().size() );                                     \
        REQUIRE( strncmp(cerrBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );  \
        cerrReset(&cerrBuf);                                                                        \
        coutReset(&coutBuf);                                                                        \
    }

    TEST_SECTION( err1, no such file or directory error);
    TEST_SECTION( err2, too many arguments error);
    TEST_SECTION( err3, too many arguments error);

    delete cmdMgr;
    #undef TEST_SECTION
}
TEST_CASE( "lcd with lls checking", "[lcd]" )
{
    // redirect stderr and stdout
    std::stringstream cerrBuf;
    std::stringstream coutBuf;
    std::string goldenBuffer;

    // define macro for all test sections
    # define TEST_SECTION(TCASE, ERROR)                                                             \
    SECTION( "["#TCASE"] "#ERROR)                                                                   \
    {                                                                                               \
        cmdParser* cmdMgr = new cmdParser("sftp");                                                  \
        cmdMgr->regCmd();                                                                           \
                                                                                                    \
        cerrToString(&cerrBuf);                                                                     \
        coutToString(&coutBuf);                                                                     \
        REQUIRE( readGolden("./lcd/"#TCASE".out", goldenBuffer) );                                  \
        cmdMgr->readFile("./lcd/"#TCASE".in");                                                      \
        REQUIRE( goldenBuffer.size() == coutBuf.str().size() );                                     \
        REQUIRE( strncmp(coutBuf.str().c_str(), goldenBuffer.c_str(), goldenBuffer.size()) == 0 );  \
        cerrReset(&cerrBuf);                                                                        \
        coutReset(&coutBuf);                                                                        \
        delete cmdMgr;                                                                              \
    }

    TEST_SECTION( lcd2, to current directory);
    TEST_SECTION( lcd3, to previous directory);
    TEST_SECTION( lcd4, to dirforcheck);
    TEST_SECTION( lcd1, to home directory);

    #undef TEST_SECTION
}
// DO NOT implement anymore test cases becuase pwd is already changed
