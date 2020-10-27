#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include <string>
#include "def.h"

namespace UTIL
{
// common helper functions
void   parseTokens(const std::string&, std::vector<std::string>&, char delimiter = ' ');
size_t strNcmp(const std::string&, const std::string&, size_t);

// length of string or integers
int    wLength(size_t);
int    wLength(int);
int    wLength(const char*);

// convert size to human readabel format
void toHuman(double*, char*);

// size of the terminal
int getTermWidth();
}

#endif /* __UTIL_H__ */
