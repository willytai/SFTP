#include <vector>
#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "util.h"
#include <string>

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
