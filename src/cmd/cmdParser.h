#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "def.h"
#include "sftpSession.h"


/******************/
/* command status */
/******************/
enum cmdStat
{
    // command execution status
    CMD_DONE         = 0,
    CMD_ERROR        = 1,
    CMD_EXIT         = 2,
    CMD_EXECUTE      = 3,

    // command option errors
    CMD_OPT_MISSING  = 4,
    CMD_OPT_EXTRA    = 5,
    CMD_OPT_ILLEGAL  = 6,

    // command execution error
    CMD_EXEC_ERROR   = 9,

    CMD_ARG_TOO_MANY = 10,

    // command registration
    CMD_REG_FAIL     = 7,
    CMD_REG_DONE     = 8,

    CMD_DUMMY
};

/*****************/
/* arg pare stat */
/*****************/
enum argStat
{
    ARG_PARSE_DONE,
    ARG_PARSE_CMD_PARSER_ONLY,
    ARG_PARSE_OPTION_MISSING,
    ARG_PARSE_OPTION_UNKNOWN,
    ARG_PARSE_ARG_INVALID,
    ARG_PARSE_ARG_MISSING,

    #ifdef DEV
    ARG_PARSE_FROM_FILE,
    #endif

    ARG_PARSE_ARG_TOO_MANY
};

/*****************/
/* command class */
/*****************/
class cmdExec
{
public:
    cmdExec() : _flags(0) {}
    virtual ~cmdExec() {}

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
    std::string _key;
    std::string _opt;

protected:
    mutable int _flags;
};


/*****************************/
/* local command class macro */
/*****************************/
#define cmdClassLocal(T)                       \
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

/******************************/
/* server command class macro */
/******************************/
#define cmdClassServer(T)                      \
class T : public cmdExec                       \
{                                              \
public:                                        \
    T(sftp::sftpSession** s) { _sftp_sess_ptr = s; }\
    ~T() {}                                    \
                                               \
    cmdStat execute(const std::string&) const; \
    void    usage()   const;                   \
    void    help()    const;                   \
private:                                       \
    sftp::sftpSession**  _sftp_sess_ptr;       \
}

/*****************/
/* Error Handler */
/*****************/
class errorMgr
{
public:
    errorMgr() : _colorful(false) {}
    ~errorMgr() {}

    void handle(const cmdStat&);
    void handle(const argStat&);
    void handle(const sftp::sftpStat&);
    bool colorOutput() const { return _colorful; }

    // for 'ls' and 'lls'
    void handleNonExistDir(const std::string&, const std::vector<std::string>&, bool) const;

    void setColorfulOutput() { _colorful = true; }

    // for command parser
    void setErrCmd        (const std::string&)  const;
    void setErrOpt        (const std::string&)  const;
    void setErrArg        (const std::string&)  const;
    void setErrOpt        (const char&)         const;
    void setErrHndlr      (const cmdExec*)      const;

    // for sftp session
    void setSftpErr       (const char*, const char*) const;
    void setSftpErr       (const char*) const;

private:

    // for command parser
    void cmdError();
    void cmdOptIllegal();
    void cmdExecError();
    void cmdArgTooMany();

    // for sftp session
    void sftpSshConnectError() const;
    void sftpSshAllocError() const;
    void sftpSessAllocError() const;
    void sftpSessInitError() const;
    void sftpAuthError() const;
    void sftpAuthUndefError() const;
    void sftpVrfyPubKeyError() const;
    void sftpVrfyPubHashError() const;
    void sftpVrfyHostOtherError() const;
    void sftpVrfyStdinError() const;
    void sftpVrfyHostConnectDefined() const;
    void sftpVrfyUpdateError() const;
    void sftpVrfyKnownHostError() const;

private:
    // colorful print or not
    bool _colorful;
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
#define MATCH_KEY_OUTPUT_MIN_WIDTH 8

typedef std::unordered_map<std::string, cmdExec*> cmdMAP;
typedef std::pair<std::string, cmdExec*>          cmdKeyHandlerPair;

public:
    cmdParser(const char* prompt);
    ~cmdParser();

    // TOP API
    void parse(int, char**);


#ifndef DEV
private:
#endif

    // parse arguments
    argStat parseArgs(int, char**);

    // read commands
    cmdStat readCmd();

    // register commands
    void    regCmd();

#ifdef DEV
    void    readFile();
    void    setInFileName(const char* f) { _filename = std::string(f); }
    cmdStat readCmdFile(std::ifstream&);
#endif


#ifdef DEV
private:
#endif
    cmdStat readChar(std::istream&);
    cmdStat regEachCmd(std::string, size_t, cmdExec*);
    cmdStat interpretateAndExecute() const;

    cmdExec* getCmdHandler(const std::string&) const;

    void resetBuf();
    void printPrompt();

    void autoComplete();
    void completeCmd(const std::string&);
    void completePath(const std::string&);
    void showMatched(const std::vector<std::pair<std::string, bool> >&, int);
    void rePrintBuf();
    void insertChar(char, int count = 1);
    void insertStr(const char*, size_t);
    void moveBufPtr(const char*);
    void backSpaceChar();
    void deleteChar();
    void newLineCmd();
    void retrieveHistory(short);
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

    // sftp session placeholder
    sftp::sftpSession* _sftp_sess;

#ifdef DEV
    std::string _filename;
#endif
};

#endif /* __CMD_PARSER_H__ */
