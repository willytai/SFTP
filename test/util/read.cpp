#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "dirIO.h"

TEST_CASE( "-- readDir --", "[UTIL]")
{
    SECTION("unsuccessful reading")
    {
        std::vector<std::pair<std::string, bool> > container;
        REQUIRE( UTIL::readDir("no such dir", container)  == false );
        REQUIRE( container.size() == 0 );
    }
    SECTION("return filenames only")
    {
        std::vector<std::pair<std::string, bool> > container;
        REQUIRE( UTIL::readDir("./dirforcheck", container) == true );
        REQUIRE( container.size() == 7 );
        REQUIRE( container[0].first == "." );
        REQUIRE( container[1].first == ".." );
        REQUIRE( container[2].first == "file2.txt" );
        REQUIRE( container[3].first == "file3.txt" );
        REQUIRE( container[4].first == "file1.txt" );
        REQUIRE( container[5].first == "test.txt" );
        REQUIRE( container[6].first == "file1.lnk" );
    }
    SECTION("return entry properties")
    {
        std::vector<dirent*> container;
        REQUIRE( UTIL::readDir("./dirforcheck", container) == true );
        REQUIRE( container.size() == 7 );
        REQUIRE( container[0]->d_type == DT_DIR );
        REQUIRE( container[1]->d_type == DT_DIR );
        REQUIRE( container[3]->d_type == DT_REG );
        REQUIRE( container[6]->d_type == DT_LNK );
    }
}
TEST_CASE( "-- getEntryStat --", "[UTIL]")
{
    SECTION("different ways of passing dir path")
    {
        struct UTIL::EntryStat stat;
        REQUIRE( UTIL::getEntryStat("./dirforcheck", "test.txt", &stat) == true );
        REQUIRE( UTIL::getEntryStat("dirforcheck/", "test.txt", &stat) == true );
        REQUIRE( UTIL::getEntryStat("dirforcheck", "test.txt", &stat) == true );
    }
    SECTION("entry content")
    {
        struct UTIL::EntryStat stat;
        REQUIRE( UTIL::getEntryStat("./dirforcheck/", "test.txt", &stat) == true );

        REQUIRE( stat.en_type == '-' );
        REQUIRE( strncmp(stat.en_perm, "r--r--r--", 10) == 0 );
        #ifdef __APPLE__
        REQUIRE( stat.en_xattr == '.' );
        #endif
        REQUIRE( stat.en_nlink == 1 );
        REQUIRE( stat.en_size == 46 );
        REQUIRE( strncmp(stat.en_mtime, "Oct 24 00:15", 13) == 0 );
        REQUIRE( strncmp(stat.en_name, "test.txt", 9) == 0 );

        // these two may differ when "test.txt" is downloaded to different environment
        // don't wory to much if it fails
        UNSCOPED_INFO("The username/groupname info test may fail due to different systems.");
        UNSCOPED_INFO("No need to worry if these two are the only failed case.");
        CHECK( strncmp(stat.en_usrname, "willytai", 9) == 0 );
        CHECK( strncmp(stat.en_grname, "staff", 6) == 0 );

        #ifdef __APPLE__
        // NOT YET IMPLEMENTED!
        // REQUIRE( strncmp(stat.en_xattrList, "", 0) == 0 );
        #endif
    }
}
TEST_CASE( "-- rmEscChar --", "[UTIL]" )
{
    char* ans;
    REQUIRE( (ans = UTIL::rmEscChar("asdf")) == NULL );
    REQUIRE( std::string(ans = UTIL::rmEscChar("a\\ \\*shit")) == "a *shit" );
    free( ans );
}
TEST_CASE( "-- fillEscChar --", "[UTIL]" )
{
    char* ans;
    REQUIRE( (ans = UTIL::fillEscChar("asdfghj")) == NULL);
    REQUIRE( std::string(ans = UTIL::fillEscChar("a b")) == "a\\ b");
    free( ans );
    REQUIRE( std::string(ans = UTIL::fillEscChar("a*b ")) == "a\\*b\\ ");
    free( ans );
}
