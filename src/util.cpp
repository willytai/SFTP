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

void parseTokens(const std::string& buf, std::vector<std::string>& tokens, char delimiter) {

    // dummy check
    if (buf.empty()) return;

    size_t start = 0, end;

    // get rid of leading spaces and check
    for (; start < buf.length(); ++start) if (buf[start] != delimiter) break;
    if (start == buf.length()) return;

    // get rid of trailing spaces
    size_t maxCheck = buf.length();
    while (buf[maxCheck] == delimiter) --maxCheck;

    end = start+1;
    while ( end < maxCheck ) {
        if (buf[end] == delimiter) {
            tokens.push_back(buf.substr(start, end-start));
            start = end+1;
            while ( buf[start] == delimiter ) start++;
            end = start+1;
        }
        else ++end;
    }

    tokens.push_back(buf.substr(start, end));
}

size_t strNcmp(const std::string& s1, const std::string& s2, size_t n) {
    size_t ret = 0;
    for (size_t i = 0; i < n; ++i) {
        ret += abs(int(s1[i]) - int(s2[i]));
    }
    return ret;
}
}
