#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include <sstream>
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
    CMD_EXEC_ERROR   = 7,

    CMD_ARG_TOO_MANY = 8,

    // command registration
    CMD_REG_FAIL     = 9,
    CMD_REG_DONE     = 10,

    // change scope
    CMD_CHANGE_REMOTE_SCOPE = 11,
    CMD_CHANGE_LOCAL_SCOPE  = 12,

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

    virtual cmdStat execute(const std::string_view&) const = 0;
    virtual void    usage()                          const = 0;
    virtual void    help()                           const = 0;

    // set the availabel options
    void setOptionFlags(const char* str) { _available_options.emplace_back( str ); }
    auto getAvailOptions() const -> const std::vector<std::string>& { return _available_options; }

    void setCmdStr( const char* s ) { _cmd_string = std::string(s); }
    void setKeyWordOptional( size_t minCmp ) { _key = std::string_view(_cmd_string.data(), minCmp);
                                               _opt = std::string_view((_cmd_string.data()+minCmp), _cmd_string.size()-minCmp); }

    // option flag
    // TODO the coding style here is messed up
    //      don't force execute to const if _flags needs to be modified during token parsing
    void setFlag(int f) const { _flags |= f; }
    void resetFlag() const { _flags = 0; }
    bool checkFlag(int f) const { return (_flags & f) == f; }

    // for error handling and verbosity
    // const char* getOptional() const { return _opt.c_str(); }
    // const char* getKeyWord() const { return _key.c_str(); }
    const std::string_view& getKeyWordView() const { return _key; }
    const std::string_view& getOptionalView() const { return _opt; }
    // const std::string& getOptionalStr() const { return _opt; }
    // const std::string& getKeyWordStr() const { return _key; }
    const std::string& getCmdStr() const { return _cmd_string; }
    const char*        getCmdStrData() const { return _cmd_string.c_str(); }

private:
    std::string_view _key;
    std::string_view _opt;
    std::string _cmd_string;

protected:
    mutable int _flags;
    std::vector<std::string> _available_options;
};


/*****************************/
/* local command class macro */
/*****************************/
#define cmdClassLocal(T)                                     \
class T : public cmdExec                                     \
{                                                            \
public:                                                      \
    T() {}                                                   \
    ~T() {}                                                  \
                                                             \
    cmdStat execute(const std::string_view&) const override; \
    void    usage()   const override;                        \
    void    help()    const override;                        \
}

/******************************/
/* server command class macro */
/******************************/
#define cmdClassServer(T)                                    \
class T : public cmdExec                                     \
{                                                            \
public:                                                      \
    T(sftp::sftpSession** s) { _sftp_sess_ptr = s; }         \
    ~T() {}                                                  \
                                                             \
    cmdStat execute(const std::string_view&) const override; \
    void    usage()   const;                                 \
    void    help()    const;                                 \
private:                                                     \
    sftp::sftpSession**  _sftp_sess_ptr;                     \
}

/*****************/
/* Error Handler */
/*****************/
class errorMgr
{
public:
    errorMgr() : _colorful(false) {}
    ~errorMgr() {}

    // handle returns false if there is actually error to be handled
    bool handle(const cmdStat&);
    bool handle(const argStat&);
    bool handle(const sftp::sftpStat&);
    bool colorOutput() const { return _colorful; }

    // for 'ls' and 'lls'
    void handleNonExistDir(const std::string&, const std::vector<std::string_view>&, bool) const;

    void setColorfulOutput() { _colorful = true; }

    // for command parser
    void setErrCmd        (const std::string&)  const;
    void setErrCmd        (const char*)  const;
    void setErrCmd        (const std::string_view&)  const;
    void setErrOpt        (const std::string&)  const;
    void setErrArg        (const std::string&)  const;
    void setErrArg        (const std::string_view&) const;
    void setErrArg        (const char*) const;
    void setErrOpt        (const char&)         const;
    void setErrOpt        (const std::string_view&)  const;
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
    void sftpReaddirError() const;
    void sftpReadfileError() const;
    void sftpClosefileError() const;
    void sftpGetError() const;

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

// scope
#define REMOTE 1
#define LOCAL  0

// status of autoComplete would be the same as sftpStat
// since errors are only likely to occur during transmission
// between client and server
#define cmpltStat sftp::sftpStat

typedef std::unordered_map<std::string_view, cmdExec*> cmdMAP;
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
    cmdStat regEachCmd(const char*, size_t, cmdExec*);
    cmdStat regEachCmdOption(cmdExec*);
    cmdStat interpretateAndExecute() const;

    cmdExec* getCmdHandler(const std::string_view&) const;

    void resetBuf();
    void printPrompt();

    cmpltStat completePath(const std::string_view&, short scope, bool dirOnly = false);
    void autoComplete();
    void completeCmd(const std::string_view&);
    template<typename T>
    void showMatched(const T&, int);
    void rePrintBuf();

    void insertChar(char, int count = 1);
    void insertStr(const char*, size_t);
    void moveBufPtr(const char*);
    void backSpaceChar();
    void deleteChar();
    void newLineCmd();
    void retrieveHistory(short);
    void makeCopy();

    // prompt helper functions
    void getLocalCWD(std::ostringstream&) const;
    void getRemoteCWD(std::ostringstream&) const;
    void trimPath(const char*) const;

private:
    std::string  _prompt;

    char         _buf[BUF_SIZE];
    char*        _bufEnd;
    char*        _bufPtr;

    // when up/down/pgUp/pgDown keys are pressed
    std::vector<std::string> _history;
    // _bufTmp is a copy _buf
    std::string              _bufTmp;
    char*                    _bufTmpPtr;
    short                    _hisID;

    // the map of each command to its class
    // the map of each keyword to its minCmp
    cmdMAP  _cmdMap;

    // sftp session placeholder
    sftp::sftpSession* _sftp_sess;

    // the home directory
    // store the length to avoid recalculating it every time
    const char* _home;
    size_t      _hlen;

    // the scope to perfoem linux commands
    short       _scope;

#ifdef DEV
    std::string _filename;
#endif
};

#endif /* __CMD_PARSER_H__ */
