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

template <typename T>
void parseTokens(const T& buf, std::vector<std::string>& tokens, char delimiter, size_t maxTokens) {

    // dummy check
    if (buf.empty()) return;

    size_t start = 0, end;

    // get rid of leading delimiters
    // will stop if ESCAPE_CHAR is encountered
    for (; start < buf.length(); ++start) {
        if (buf[start] != delimiter) break;
    }
    if (start == buf.length()) return;

    // get rid of trailing delimiters
    size_t maxCheck = buf.length()-1;
    while (buf[maxCheck] == delimiter) --maxCheck;

    // make sure to deal with escape characters for the first word
    end = buf[start] == ESCAPE_CHAR ? start+2 : start+1;
    while ( end <= maxCheck ) {
        if ( buf[end] == ESCAPE_CHAR ) {
            end += 2;
        }
        else if ( buf[end] == delimiter ) {
            tokens.push_back(buf.substr(start, end-start));
            start = end+1;
            while ( buf[start] == delimiter ) start++;
            end = start+1;

            // fill at most maxTokens tokens
            if ( tokens.size() == maxTokens-1 ) {
                tokens.push_back(buf.substr(start, maxCheck-start+1));
                return;
            }

        }
        else ++end;
    }

    tokens.push_back(buf.substr(start, end-start));
}

// more memory efficient
void parseTokens(const std::string_view& buf, std::vector<std::string_view>& tokens, char delimiter, size_t maxTokens) {

    // dummy check
    if (buf.empty()) return;

    size_t start = 0, end;

    // get rid of leading delimiters
    // will stop if ESCAPE_CHAR is encountered
    for (; start < buf.length(); ++start) {
        if (buf[start] != delimiter) break;
    }
    if (start == buf.length()) return;

    // get rid of trailing delimiters
    size_t maxCheck = buf.length()-1;
    while (buf[maxCheck] == delimiter) --maxCheck;

    // make sure to deal with escape characters for the first word
    end = buf[start] == ESCAPE_CHAR ? start+2 : start+1;
    while ( end <= maxCheck ) {
        if ( buf[end] == ESCAPE_CHAR ) {
            end += 2;
        }
        else if ( buf[end] == delimiter ) {
            // tokens.push_back(buf.substr(start, end-start));
            tokens.emplace_back( &buf[start], end-start );
            start = end+1;
            while ( buf[start] == delimiter ) start++;
            end = start+1;

            // fill at most maxTokens tokens
            if ( tokens.size() == maxTokens-1 ) {
                // tokens.push_back(buf.substr(start, maxCheck-start+1));
                tokens.emplace_back( &buf[start], maxCheck-start+1 );
                return;
            }

        }
        else ++end;
    }

    // tokens.push_back(buf.substr(start, end-start));
    tokens.emplace_back( &buf[start], end-start );
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

// if 'n' not specified, substr to the end of source
// if 'substart' is not specified, start 'sub' from 0
// make sure sub does not overflow
void substr(const char* source, char* sub, size_t pos, size_t n, size_t substart) {
    size_t i = 0;
    for (; i < n; ++i) {
        sub[substart+i] = source[pos+i];
        if ( source[pos+i] == '\0' ) break;
    }
    if ( source[pos+i-1] != '\0' ) sub[substart+i] = '\0';
}

// allows strings that are shorter than n (will simply be detected and returns -1)
int strNcmp(const char* s1, const char* s2, size_t n) {
    size_t i = 0;
    for (; i < n; ++i) {
        if ( s1[i] == '\0' || s2[i] == '\0' ) break;
        if ( s1[i] != s2[i] ) return -1;
    }
    if ( i < n ) return -1;
    else         return 0;
}

int strNcmp(const std::string& s1, const std::string& s2, size_t n) {
    return strNcmp(s1.c_str(), s2.c_str(), n);
}

int strNcmp(const std::string_view& s1, const std::string& s2, size_t n) {
    return strNcmp(s1.data(), s2.c_str(), n);
}

int strNcmp(const std::string_view& s1, const std::string_view& s2, size_t n) {
    return strNcmp(s1.data(), s2.data(), n);
}

int strNcmp_soft(const std::string& s1, const std::string& s2, size_t n) {
    return strncasecmp(s1.c_str(), s2.c_str(), n);
}

int strNcmp_soft(const std::string_view& s1, const std::string_view& s2, size_t n) {
    return strncasecmp(s1.data(), s2.data(), n);
}

int strcmp(const std::string_view& sview, const char* s2, size_t n) {
    if ( sview.size() != n ) return -1;
    return strcmp( &sview.front(), s2, n );
}

// be very careful when calling this function!!!!
// DO NOT pass in s1/s2 by the reference of the 'front' of a string_view ( no null char in string_view )
int strcmp(const char* s1, const char* s2, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if ( s1[i] != s2[i] ) return -1;
    }
    if ( s2[i] != '\0' ) return -1;
    return 0;
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

void splitPathFile(const std::string_view& full, std::string_view& dir, std::string_view& file) {
    size_t end = full.find_last_of('/');
    dir  = (end == std::string::npos) ? "./" : full.substr(0, end);
    file = (end+1 == full.size())     ? ""   : full.substr(end+1); // npos == -1
}

// returns the original string_view if not found
std::string_view find_last(const std::string_view& sview, char delimiter) {
    size_t pos = 0;
    size_t i = 0;
    for (; i < sview.size(); ++i) {
        if ( sview[i] == delimiter && i+1 < sview.size() ) {
            pos = i+1;
        }
    }
    return sview.substr(pos);
}

}
