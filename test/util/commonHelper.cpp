#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "util.h"
#include <string>
#include <vector>

TEST_CASE("parseTokens", "[UTIL]")
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
}
TEST_CASE("strNcmp", "[UTIL]")
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
}
TEST_CASE("wLength", "[UTIL]")
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
