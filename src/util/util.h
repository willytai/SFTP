#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include <string>
#include <string_view>
#include "def.h"

namespace UTIL
{
// common helper functions
void parseTokens(const std::string&, std::vector<std::string>&, char delimiter = ' ', size_t maxTokens = 0xffffffffffffffff);
void parseTokens(const std::string&, std::vector<std::string_view>&, char delimiter = ' ', size_t maxTokens = 0xffffffffffffffff);
void substr(const char*, char*, size_t, size_t n = 0xffffffffffffffff, size_t substart = 0);
int  strNcmp(const std::string&, const std::string&, size_t);
int  strNcmp(const char*, const char*, size_t);
int  strNcmp_soft(const std::string&, const std::string&, size_t);

// hard comparison between two strings ( need to be identical )
// TODO write test
// calling these two functions are pretty dangerous, make sure size do not exceede length
int strcmp(const std::string_view&, const char*, size_t);
int strcmp(const char*, const char*, size_t);

// length of string or integers
int    wLength(size_t);
int    wLength(int);
int    wLength(const char*);

// convert size to human readabel format
void toHuman(double*, char*);

// size of the terminal
int getTermWidth();

// home directory
void getHomeDir(const char**);

// split into directory and filename
void splitPathFile(const std::string&, std::string&, std::string&);

// string manipulation
char*  rmEscChar(const char*);
char*  fillEscChar(const char*);

std::string_view find_last(const std::string_view&, char);

}

#endif /* __UTIL_H__ */
