#include "util.h"
#include <vector>

namespace UTIL
{
// returns false if dir cannot be opened
// container will contain all the names of the stuffs under dir
// including hidden files
bool readDir(const char* dir, std::vector<std::string>& container) {
    DIR* dirptr = opendir(dir);
    if ( dirptr == NULL ) return false;
    struct dirent* direntry;
    while ( (direntry = readdir(dirptr)) != NULL ) {
        container.push_back(direntry->d_name);
    }
    return true;
}

void parseTokens(const std::string&  buf, size_t length, std::vector<std::string>& tokens) {

}

size_t strNcmp(const std::string& s1, const std::string& s2, size_t n) {
    size_t ret = 0;
    for (size_t i = 0; i < n; ++i) {
        ret += abs(int(s1[i]) - int(s2[i]));
    }
    return ret;
}
}
