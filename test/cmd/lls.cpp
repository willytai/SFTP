#include <sstream>
#include <streambuf>
#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../outputRedirect.h"
#include "cmdParser.h"

errorMgr errMgr;

TEST_CASE( "Error Message", "[lls]" )
{
    cmdParser* cmdMgr = new cmdParser("err");
    cmdMgr->regCmd();

    // redirect stderr
    std::stringstream cerrBuf;

    SECTION( "not file or directory error" )
    {
        cerrToString(&cerrBuf);
        cmdMgr->readFile("../lls/err1.in");
        cout << "test" << endl;
        cout << cerrBuf.str() << endl;
        cout << "testend" << endl;
        // TODO: compare with golden
        cerrReset(&cerrBuf);
    }
    SECTION( "option error" )
    {
        cerrToString(&cerrBuf);
        cmdMgr->readFile("../lls/err2.in");
        cout << "test" << endl;
        cout << cerrBuf.str() << endl;
        cout << "testend" << endl;
        cerrReset(&cerrBuf);
    }


    delete cmdMgr;
}
