#ifndef __UTIL_H__
#define __UTIL_H__

#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>

namespace UTIL
{
bool   readDir(const char*, std::vector<std::string>&);
void   parseTokens(const std::string&, size_t, std::vector<std::string>&);
size_t strNcmp(const std::string&, const std::string&, size_t);
}

#endif /* __UTIL_H__ */
