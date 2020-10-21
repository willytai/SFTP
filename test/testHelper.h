#ifndef __TEST_HELPER_H__
#define __TEST_HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>

using std::cout;
using std::cerr;
using std::endl;


bool readGolden(const char* filename, std::string& buffer) {
    FILE* f = fopen(filename, "rb");
    if ( f == NULL ) return false;
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    char* tmp = (char*)malloc(fsize+1);
    fread(tmp, 1, fsize, f);
    fclose(f);
    buffer.clear();
    buffer.resize(fsize);
    for (int i = 0; i < fsize; ++i) buffer[i] = tmp[i];
    free(tmp);
    return true;
}


/************************************/
/* This part is for output redirect */
/************************************/
static std::streambuf* oldcerr;
static std::streambuf* oldcout;

void coutToString(std::stringstream* buffer) {
    oldcout = std::cout.rdbuf();
    if ( buffer )
        std::cout.rdbuf( buffer->rdbuf() );
    else
        std::cout.rdbuf( NULL );
}

void coutReset(std::stringstream* buffer = NULL) {
    std::cout.rdbuf( oldcout );
    if ( buffer ) buffer->str(std::string());
}

void cerrToString(std::stringstream* buffer) {
    oldcerr = std::cerr.rdbuf();
    if ( buffer )
        std::cerr.rdbuf( buffer->rdbuf() );
    else
        std::cerr.rdbuf( NULL );
}

void cerrReset(std::stringstream* buffer = NULL) {
    std::cerr.rdbuf( oldcerr );
    if ( buffer ) buffer->str(std::string());
}

#endif /* __TEST_HELPER_H__ */
