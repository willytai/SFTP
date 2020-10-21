#ifndef __COUT_REDIRECT__
#define __COUT_REDIRECT__

#include <iostream>
#include <sstream>
#include <streambuf>

using std::cout;
using std::cerr;
using std::endl;

static std::streambuf* oldcerr;
static std::streambuf* oldcout;

void coutToString(std::stringstream* buffer) {
    oldcout = std::cout.rdbuf( buffer->rdbuf() );
}

void coutReset(std::stringstream* buffer) {
    std::cout.rdbuf( oldcout );
    buffer->str(std::string());
}

void cerrToString(std::stringstream* buffer) {
    oldcerr = std::cerr.rdbuf( buffer->rdbuf() );
}

void cerrReset(std::stringstream* buffer) {
    std::cerr.rdbuf( oldcerr );
    buffer->str(std::string());
}

#endif /* __COUT_REDIRECT__ */
