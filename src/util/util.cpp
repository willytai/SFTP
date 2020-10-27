#include "util.h"
#include <sys/ioctl.h>
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

size_t strNcmp(const std::string& s1, const std::string& s2, size_t n) {
    size_t ret = 0;
    for (size_t i = 0; i < n; ++i) {
        ret += abs(int(s1[i]) - int(s2[i]));
    }
    return ret;
}

static const char unitDict[] = {'B', 'K', 'M', 'G', 'T', 'P'};
void toHuman(double* val, char* unit) {
    int intval = (int)((*val));
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
    }
}

int getTermWidth() {
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    return ws.ws_col;
}

}
