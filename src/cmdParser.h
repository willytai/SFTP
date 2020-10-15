#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include <string.h>
#include <vector>
#include "cmdCharDef.h"

class cmdParser
{
#define BUF_SIZE 65536
#define HISTORY_SIZE 1000
#define TAB_STOP 4
#define PG_OFFSET 10
public:
    cmdParser(std::string prompt) : _prompt(prompt),
                                    _bufEnd(NULL),
                                    _bufPtr(NULL) {
        _history.clear();
        _history.reserve(HISTORY_SIZE);
        _hisID = 0;
    }
    ~cmdParser() {}

    void readCmd();

private:
    void readChar(std::istream&);
    void resetBuf();
    void printPrompt();

    void autoComplete();
    void insertChar(char, int count = 1);
    void moveBufPtr(const char*);
    void backSpaceChar();
    void deleteChar();
    void retrieveHistory(short);
    void newLineCmd();


    std::string  _prompt;

    char         _buf[BUF_SIZE];
    char*        _bufEnd;
    char*        _bufPtr;

    // _bufTmp is a copy _buf
    // when up/down/pgUp/pgDown keys are pressed
    std::vector<std::string> _history;
    std::string              _bufTmp;
    char*                    _bufTmpPtr;
    short                    _hisID;
};

#endif /* __CMD_PARSER_H__ */
