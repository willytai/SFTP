#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "def.h"


/******************/
/* command status */
/******************/
enum cmdStat
{
    // command execution status
    CMD_DONE        = 0,
    CMD_ERROR       = 1,
    CMD_EXIT        = 2,
    CMD_EXECUTE     = 3,

    // command option errors
    CMD_OPT_MISSING = 4,
    CMD_OPT_EXTRA   = 5,
    CMD_OPT_ILLEGAL = 6,

    // command execution error
    CMD_EXEC_ERROR  = 9,

    // command registration
    CMD_REG_FAIL    = 7,
    CMD_REG_DONE    = 8,

    CMD_DUMMY
};

/*****************/
/* command class */
/*****************/
class cmdExec
{
public:
    cmdExec() : _flags(0) {}
    ~cmdExec() {}

    virtual cmdStat execute(const std::string&) const = 0;
    virtual void    usage()                     const = 0;
    virtual void    help()                      const = 0;

    void setOptional( const std::string& s) { _opt = s; }
    void setKeyWord( const std::string& s) { _key = s; }
    void resetFlag() const { _flags = 0; }

    // for error handling and verbosity
    const char* getOptional() const { return _opt.c_str(); }
    const char* getKeyWord() const { return _key.c_str(); }
    const std::string& getOptionalStr() const { return _opt; }
    const std::string& getKeyWordStr() const { return _key; }
    std::string        getCmdStr() const { return _key+_opt; }

private:
    std::string _opt;

    // just for error printing, not really needed
    std::string _key;

protected:
    mutable int _flags;
};


/*********/
/* macro */
/*********/
#define cmdClass(T)                            \
class T : public cmdExec                       \
{                                              \
public:                                        \
    T() {}                                     \
    ~T() {}                                    \
                                               \
    cmdStat execute(const std::string&) const; \
    void    usage()   const;                   \
    void    help()    const;                   \
}

/*****************/
/* Error Handler */
/*****************/
class errorMgr
{
public:
    errorMgr() : _pretty(false) {}
    ~errorMgr() {}

    void handle(const cmdStat&);

    // for 'ls' and 'lls'
    void handleNonExistDir(const std::string&, const std::vector<std::string>&, bool) const;

    void setColorfulPrint () { _pretty = true; }
    void setErrCmd        (const std::string&)       const;
    void setErrOpt        (const std::string&)       const;
    void setErrOpt        (const char&)              const;
    void setErrHndlr      (const cmdExec*)           const;
    void setErrEntryAndDir(const char*, const char*) const;

private:
    void cmdError();
    void cmdOptIllegal();
    void cmdExecError();

private:
    // colorful print or not
    bool _pretty;
};


/******************/
/* command parser */
/******************/
class cmdParser
{
#define BUF_SIZE 65536
#define HISTORY_SIZE 1000
#define TAB_STOP 4
#define PG_OFFSET 10

typedef std::unordered_map<std::string, cmdExec*> cmdMAP;
typedef std::pair<std::string, cmdExec*>          cmdKeyHandlerPair;

public:
    cmdParser(const char* prompt) : _prompt(prompt),
                                    _bufEnd(NULL),
                                    _bufPtr(NULL) {
        _history.clear();
        _history.reserve(HISTORY_SIZE);
        _hisID = 0;
    }
    ~cmdParser() {}

    // register commands
    void    regCmd();

    // read commands
    cmdStat readCmd();

#ifdef DEV
    void readFile(const char*);
    cmdStat readCmdFile();
#endif

private:
    cmdStat readChar(std::istream&);
    cmdStat regEachCmd(std::string, size_t, cmdExec*);
    cmdStat interpretateAndExecute() const;

    cmdExec* getCmdHandler(const std::string&) const;

    void resetBuf();
    void printPrompt();

    void autoComplete();
    void insertChar(char, int count = 1);
    void moveBufPtr(const char*);
    void backSpaceChar();
    void deleteChar();
    void retrieveHistory(short);
    void newLineCmd();
    void makeCopy();

private:
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

    // the map of each command to its class
    // the map of each keyword to its minCmp
    cmdMAP  _cmdMap;

#ifdef DEV
    std::ifstream _file;
#endif
};

#endif /* __CMD_PARSER_H__ */
