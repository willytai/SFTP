#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "util.h"
#include <string>
#include <vector>

TEST_CASE( "-- parseTokens --", "[UTIL]" )
{
    SECTION("null string")
    {
        std::string test = "";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container);
        REQUIRE( container.size() == 0 );
    }
    SECTION("single string")
    {
        std::string test = "fukawf";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container);
        REQUIRE( container.size() == 1 );
        REQUIRE( container[0]     == "fukawf" );
    }
    SECTION("normal string")
    {
        std::string test = "what the fuck is this";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container);
        REQUIRE( container.size() == 5 );
        REQUIRE( container[0]     == "what" );
        REQUIRE( container[1]     == "the" );
        REQUIRE( container[2]     == "fuck" );
        REQUIRE( container[3]     == "is" );
        REQUIRE( container[4]     == "this" );
    }
    SECTION("normal string, custom delimiter")
    {
        std::string test = "what\nthe\nfuck\nis\nthis";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, '\n');
        REQUIRE( container.size() == 5 );
        REQUIRE( container[0]     == "what" );
        REQUIRE( container[1]     == "the" );
        REQUIRE( container[2]     == "fuck" );
        REQUIRE( container[3]     == "is" );
        REQUIRE( container[4]     == "this" );
    }
    SECTION("repeadted delimiter")
    {
        std::string test = "    what   the fuck  is this  ";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container);
        REQUIRE( container.size() == 5 );
        REQUIRE( container[0]     == "what" );
        REQUIRE( container[1]     == "the" );
        REQUIRE( container[2]     == "fuck" );
        REQUIRE( container[3]     == "is" );
        REQUIRE( container[4]     == "this" );
    }
    SECTION("weird strings")
    {
        std::string test = "   what   the\nfuck  is this  ";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container);
        REQUIRE( container.size() == 4 );
        REQUIRE( container[0]     == "what" );
        REQUIRE( container[1]     == "the\nfuck" );
        REQUIRE( container[2]     == "is" );
        REQUIRE( container[3]     == "this" );
    }
    SECTION("weird delimiters")
    {
        std::string test = "   what   the fuck  is this  ";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, 't');
        REQUIRE( container.size() == 4 );
        REQUIRE( container[0]     == "   wha" );
        REQUIRE( container[1]     == "   " );
        REQUIRE( container[2]     == "he fuck  is " );
        REQUIRE( container[3]     == "his  " );
    }
    SECTION("string with normal escape characters")
    {
        std::string test = "this is an\\ escape character";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ');
        REQUIRE( container.size() == 4 );
        REQUIRE( container[0]     == "this" );
        REQUIRE( container[1]     == "is" );
        REQUIRE( container[2]     == "an\\ escape" );
        REQUIRE( container[3]     == "character" );
    }
    SECTION("string with leading escape characters 1")
    {
        std::string test = "\\this is an escape character";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ');
        REQUIRE( container.size() == 5 );
        REQUIRE( container[0]     == "\\this" );
        REQUIRE( container[1]     == "is" );
        REQUIRE( container[2]     == "an" );
        REQUIRE( container[3]     == "escape" );
        REQUIRE( container[4]     == "character" );
    }
    SECTION("string with leading escape characters 2")
    {
        std::string test = "\\ this is an escape character";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ');
        REQUIRE( container.size() == 5 );
        REQUIRE( container[0]     == "\\ this" );
        REQUIRE( container[1]     == "is" );
        REQUIRE( container[2]     == "an" );
        REQUIRE( container[3]     == "escape" );
        REQUIRE( container[4]     == "character" );
    }
    SECTION("string with leading escape characters 3")
    {
        std::string test = "\\  this is an escape character";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ');
        REQUIRE( container.size() == 6 );
        REQUIRE( container[0]     == "\\ " );
        REQUIRE( container[1]     == "this" );
        REQUIRE( container[2]     == "is" );
        REQUIRE( container[3]     == "an" );
        REQUIRE( container[4]     == "escape" );
        REQUIRE( container[5]     == "character" );
    }
    SECTION("string with trailing escape characters")
    {
        std::string test = "lcd fuck\\ ";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ');
        REQUIRE( container.size() == 2 );
        REQUIRE( container[0]     == "lcd" );
        REQUIRE( container[1]     == "fuck\\ " );
    }
    SECTION("max number of tokens")
    {
        std::string test = "max number of tokens";
        std::vector<std::string> container;
        UTIL::parseTokens(test, container, ' ', 2);
        REQUIRE( container.size() == 2 );
        REQUIRE( container[0]     == "max" );
        REQUIRE( container[1]     == "number of tokens" );
    }
}
TEST_CASE( "-- strNcmp_soft --", "[UTIL]" )
{
    SECTION("identical strings")
    {
        REQUIRE( UTIL::strNcmp_soft("identical", "identical", 9) == 0 );
        REQUIRE( UTIL::strNcmp_soft("\tidEntical", "\tidentiCal", 10) == 0 );
        REQUIRE( UTIL::strNcmp_soft("';pouhFckf e6 af$\t5|fgs\n?sd!", "';Pouhfckf e6 af$\t5|fgs\n?sd!", 28) == 0 );
    }
    SECTION("different strings")
    {
        REQUIRE( UTIL::strNcmp_soft("qhw;sdfasdf", "823985uhasdf", 11) != 0 );
        REQUIRE( UTIL::strNcmp_soft("qhw;sdfasdf", "82sdf", 5) != 0 );
    }
    SECTION("different N")
    {
        REQUIRE( UTIL::strNcmp_soft("asdfwhathefuck", "asdfshit", 4) == 0 );
        REQUIRE( UTIL::strNcmp_soft("asdfwhathefuck", "asdfshit", 5) != 0 );
    }
}
TEST_CASE( "-- strNcmp --", "[UTIL]" )
{
    SECTION("identical strings")
    {
        REQUIRE( UTIL::strNcmp("identical", "identical", 9) == 0 );
        REQUIRE( UTIL::strNcmp("\tidentical", "\tidentical", 10) == 0 );
        REQUIRE( UTIL::strNcmp("';pouhfckf e6 af$\t5|fgs\n?sd!", "';pouhfckf e6 af$\t5|fgs\n?sd!", 28) == 0 );
    }
    SECTION("different strings")
    {
        REQUIRE( UTIL::strNcmp("qhw;sdfasdf", "823985uhasdf", 11) != 0 );
        REQUIRE( UTIL::strNcmp("qhw;sdfasdf", "82sdf", 5) != 0 );
    }
    SECTION("different N")
    {
        REQUIRE( UTIL::strNcmp("asdfwhathefuck", "asdfshit", 4) == 0 );
        REQUIRE( UTIL::strNcmp("asdfwhathefuck", "asdfshit", 5) != 0 );
    }
    SECTION("not-long-enough strings")
    {
        REQUIRE( UTIL::strNcmp("fuck", "fu", 4) != 0 );
        REQUIRE( UTIL::strNcmp("fuck", "fu", 3) != 0 );
        REQUIRE( UTIL::strNcmp("fuck", "fu", 2) == 0 );
    }
}
TEST_CASE( "-- substr --", "[UTIL]" )
{
    SECTION("normal substrings")
    {
        char sub[64];
        UTIL::substr("this is base string", sub, 0, 4);
        UNSCOPED_INFO( "\033[37mthe substring is: \'" << sub << "\'");
        UNSCOPED_INFO( "should have been: " << "\'this\'" << "\033[32m");
        REQUIRE( UTIL::strNcmp(sub, "this", 4)  == 0 );
        UTIL::substr("this is base string", sub, 5, 2);
        UNSCOPED_INFO( "\033[37mthe substring is: \'" << sub << "\'");
        UNSCOPED_INFO( "should have been: " << "\'is\'" << "\033[32m");
        REQUIRE( UTIL::strNcmp(sub, "is", 2)  == 0 );
        UTIL::substr("this is base string", sub, 8);
        UNSCOPED_INFO( "\033[37mthe substring is: \'" << sub << "\'");
        UNSCOPED_INFO( "should have been: " << "\'base string\'" << "\033[32m");
        REQUIRE( UTIL::strNcmp(sub, "base string", 11)  == 0 );
    }
    SECTION("offset in substrings")
    {
        char sub[64];
        sub[0] = '~';
        UTIL::substr("this is base string", sub, 5, 0xffffffffffffffff, 1);
        UNSCOPED_INFO( "\033[37mthe substring is: \'" << sub << "\'");
        UNSCOPED_INFO( "should have been: " << "\'~is base string\'" << "\033[32m");
        REQUIRE( UTIL::strNcmp(sub, "~is base string", 15)  == 0 );
        UTIL::substr("this is base string", sub, 8, 4, 2);
        UNSCOPED_INFO( "\033[37mthe substring is: \'" << sub << "\'");
        UNSCOPED_INFO( "should have been: " << "\'~ibase\'" << "\033[32m");
        REQUIRE( UTIL::strNcmp(sub, "~ibase", 6)  == 0 );
    }
}
TEST_CASE( "-- wLength --", "[UTIL]" )
{
    SECTION("word length of strings")
    {
        REQUIRE( UTIL::wLength("") == 0 );
        REQUIRE( UTIL::wLength("12345") == 5 );
        REQUIRE( UTIL::wLength("as ff/.,~*&^%$#@") == 16 );
    }
    SECTION("word length of numbers")
    {
        REQUIRE( UTIL::wLength(0) == 1 );
        REQUIRE( UTIL::wLength(5) == 1 );
        REQUIRE( UTIL::wLength(50) == 2 );
        REQUIRE( UTIL::wLength(99) == 2 );
        REQUIRE( UTIL::wLength((1 << 15)) == 5 );
        REQUIRE( UTIL::wLength((1 << 30)) == 10 );
        REQUIRE( UTIL::wLength(0xffffffffffffffff) == 20 );
    }
}
TEST_CASE( "-- toHuman --", "[UTIL]" )
{
    #define prepData(v, u)         \
        val  = v;                  \
        unit = u;                  \
        UTIL::toHuman(&val, &unit);
    SECTION("Bytes")
    {
        double val;
        char   unit;

        prepData(1, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'B' ); 

        prepData(10, 'B');
        REQUIRE( val  == 10 );
        REQUIRE( unit == 'B' ); 

        prepData(999, 'B');
        REQUIRE( val  == 999 );
        REQUIRE( unit == 'B' ); 
    }
    SECTION("KBytes")
    {
        double val;
        char   unit;

        prepData(1000, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'K' ); 

        prepData(1001, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'K' ); 

        prepData(1050, 'B');
        REQUIRE( val  == 1.1 );
        REQUIRE( unit == 'K' ); 

        prepData(1130, 'B');
        REQUIRE( val  == 1.1 );
        REQUIRE( unit == 'K' ); 

        prepData(1150, 'B');
        REQUIRE( val  == 1.2 );
        REQUIRE( unit == 'K' ); 

        prepData(9999, 'B');
        REQUIRE( val  == 10 );
        REQUIRE( unit == 'K' ); 

        prepData(10000, 'B');
        REQUIRE( val  == 10 );
        REQUIRE( unit == 'K' ); 

        prepData(10500, 'B');
        REQUIRE( val  == 11 );
        REQUIRE( unit == 'K' ); 

        prepData(10400, 'B');
        REQUIRE( val  == 10 );
        REQUIRE( unit == 'K' ); 

        prepData(999000, 'B');
        REQUIRE( val  == 999 );
        REQUIRE( unit == 'K' ); 

        prepData(999400, 'B');
        REQUIRE( val  == 999 );
        REQUIRE( unit == 'K' ); 

        prepData(999500, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'M' ); 
    }
    SECTION("GBytes")
    {
        double val;
        char   unit;

        prepData(1000000000, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'G' ); 

        prepData(1001000000, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'G' ); 

        prepData(1050000000, 'B');
        REQUIRE( val  == 1.1 );
        REQUIRE( unit == 'G' ); 

        prepData(1130000000, 'B');
        REQUIRE( val  == 1.1 );
        REQUIRE( unit == 'G' ); 

        prepData(1150000000, 'B');
        REQUIRE( val  == 1.2 );
        REQUIRE( unit == 'G' ); 

        prepData(9999000000, 'B');
        CHECK( val  == 10 );
        CHECK( unit == 'G' ); 

        prepData(99999000000, 'B');
        CHECK( val  == 100 );
        CHECK( unit == 'G' ); 

        prepData(10000000000, 'B');
        CHECK( val  == 10 );
        CHECK( unit == 'G' ); 

        prepData(10500000000, 'B');
        CHECK( val  == 11 );
        CHECK( unit == 'G' ); 

        prepData(10400000000, 'B');
        CHECK( val  == 10 );
        CHECK( unit == 'G' ); 

        prepData(999000000000, 'B');
        CHECK( val  == 999 );
        CHECK( unit == 'G' ); 

        prepData(999400000000, 'B');
        CHECK( val  == 999 );
        CHECK( unit == 'G' ); 

        prepData(999500000000, 'B');
        CHECK( val  == 1 );
        CHECK( unit == 'T' ); 
    }
    SECTION("PBytes")
    {
        double val;
        char   unit;

        prepData(1000000000000000, 'B');
        REQUIRE( val  == 1 );
        REQUIRE( unit == 'P' ); 

        prepData(10000000000000000, 'B');
        REQUIRE( val  == 10 );
        REQUIRE( unit == 'P' ); 

        prepData(999000000000000000, 'B');
        REQUIRE( val  == 999 );
        REQUIRE( unit == 'P' ); 
    }
    #undef prepData
}
TEST_CASE( "-- split path --", "[UTIL]" )
{
    std::string dir, file;
    UTIL::splitPathFile( "", dir, file );
    REQUIRE( dir  == "./" );
    REQUIRE( file == "" );

    UTIL::splitPathFile( "./", dir, file );
    REQUIRE( dir  == "." );
    REQUIRE( file == "" );

    UTIL::splitPathFile( ".", dir, file );
    REQUIRE( dir  == "./" );
    REQUIRE( file == "." );

    UTIL::splitPathFile( "asdf", dir, file );
    REQUIRE( dir  == "./" );
    REQUIRE( file == "asdf" );

    UTIL::splitPathFile( "asdf/what/shit", dir, file );
    REQUIRE( dir  == "asdf/what" );
    REQUIRE( file == "shit" );
}
