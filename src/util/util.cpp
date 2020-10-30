#include "util.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <cstdio>

namespace UTIL
{

void parseTokens(const std::string& buf, std::vector<std::string>& tokens, char delimiter) {

    // dummy check
    if (buf.empty()) return;

    size_t start = 0, end;

    // get rid of leading spaces and check
    for (; start < buf.length(); ++start) if (buf[start] != delimiter) break;
    if (start == buf.length()) return;

    // get rid of trailing delimiters
    size_t maxCheck = buf.length()-1;
    while (buf[maxCheck] == delimiter) --maxCheck;

    end = start+1;
    while ( end <= maxCheck ) {
        if (buf[end] == delimiter) {
            tokens.push_back(buf.substr(start, end-start));
            start = end+1;
            while ( buf[start] == delimiter ) start++;
            end = start+1;
        }
        else ++end;
    }

    tokens.push_back(buf.substr(start, end-start));
}

int wLength(int val) {
    return wLength((size_t)val);
}

int wLength(size_t val) {
    int ret = 1;
    while ( val / 10 ) {
        ++ret;
        val /= 10;
    }
    return ret;
}

int wLength(const char* str) {
    return (int)strlen(str);
}

int strNcmp(const std::string& s1, const std::string& s2, size_t n) {
    return strncmp(s1.c_str(), s2.c_str(), n);
}

int strNcmp_soft(const std::string& s1, const std::string& s2, size_t n) {
    return strncasecmp(s1.c_str(), s2.c_str(), n);
}

static const char unitDict[] = {'B', 'K', 'M', 'G', 'T', 'P'};
void toHuman(double* val, char* unit) {
    size_t intval = size_t((*val));
    int lengthval = wLength(intval);
    short unitIdx = 0;
    if ( lengthval <= 3 ) {
        (*unit) = 'B';
    }
    else {
        while ( lengthval > 3 ) {
            (*val) /= 1000;
            lengthval -= 3;
            ++unitIdx;
        }

        // max supported unit is Petabyte
        unitIdx = unitIdx > 5 ? 5 : unitIdx;
        (*unit) = unitDict[unitIdx];

        // round to the first decimal place
        if ( lengthval == 1 ) {
            (*val) *= 10;
            (*val)  = std::round((*val));
            (*val) /= 10;
        }
        // otherwise, round to interger
        else {
            (*val) = std::round((*val));
        }

        // check if the rounded value has more than three digits
        if ( unitIdx < 5 && wLength(size_t(*val)) == 4 ) {
            (*val) /= 1000;
            (*unit) = unitDict[++unitIdx];
        }
    }
}

int getTermWidth() {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return ws.ws_col;
}

void getHomeDir(const char** h) {
    *h = getpwuid(getuid())->pw_dir;
}

void splitPathFile(const std::string& full, std::string& dir, std::string& file) {
    size_t end = full.find_last_of('/');
    dir  = (end == std::string::npos) ? "./" : full.substr(0, end);
    file = (end+1 == full.size())     ? ""   : full.substr(end+1); // npos == -1
}

}
