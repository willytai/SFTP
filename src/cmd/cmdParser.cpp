#include "cmdParser.h"
#include "cmdCharDef.h"
#include "util.h"
#include "dirIO.h"
#include "Usage.h"
#include <cassert>
#include <sstream>
#include <string>
#include <vector>

extern errorMgr errMgr;

cmdParser::cmdParser(const char* prompt) {
    _prompt    = prompt;
    _bufEnd    = _bufPtr = _bufTmpPtr = NULL;
    _hisID     = 0;
    _scope     = LOCAL;
    _sftp_sess = NULL;
    _history.clear();
    _history.reserve(HISTORY_SIZE);
    UTIL::getHomeDir( &_home );
    _hlen = strlen( _home );
    this->regCmd();
}

cmdParser::~cmdParser() {
    if ( _sftp_sess != NULL ) delete _sftp_sess;
    for (auto& pair : _cmdMap) {
        auto& hndl = pair.second;
        delete hndl;
    }
    _history.clear();
}

void cmdParser::parse(int argc, char** argv) {
    argStat astat = this->parseArgs(argc, argv);

    if ( astat == ARG_PARSE_DONE ) {
        sftp::sftpStat sfstat;
        if ( (sfstat = _sftp_sess->start()) != sftp::SFTP_OK ) {
            errMgr.handle(sfstat);
            return;
        }
        while ( this->readCmd() != CMD_EXIT );
    }

    if ( astat == ARG_PARSE_CMD_PARSER_ONLY ) {
        while ( this->readCmd() != CMD_EXIT );
    }

    #ifdef DEV
    if ( astat == ARG_PARSE_FROM_FILE ) {
        this->readFile();
        return;
    }
    #endif

    errMgr.handle( astat );
}

argStat cmdParser::parseArgs(int argc, char** argv) {
    if ( argc < 2 ) {
        #ifdef DEV
        _sftp_sess = new sftp::sftpSession();
        _sftp_sess->setUsrName( "willytai43", 10 );
        _sftp_sess->setHostIP ( "140.112.48.79", 13 );
        _sftp_sess->setPsswd  ( "09855184", 8 );
        // _sftp_sess->setHostIP ( "140.112.48.77", 13 );
        // _sftp_sess->setPsswd  ( "00000000", 8 );
        return ARG_PARSE_DONE;
        #endif
        return ARG_PARSE_ARG_MISSING;
    }
    if ( argc == 2 ) {
        if ( argv[1][0] == '-' ) return ARG_PARSE_CMD_PARSER_ONLY;
        // check username and server address
        std::vector<std::string_view> tokens;
        UTIL::parseTokens( argv[1], tokens, '@');
        _sftp_sess = new sftp::sftpSession();
        _sftp_sess->setUsrName( tokens[0].data(), tokens[0].size() );
        _sftp_sess->setHostIP ( tokens[1].data(), tokens[1].size() );
        return ARG_PARSE_DONE;
    }
    #ifdef DEV
    if ( argc == 3 ) {
        if ( argv[1][0] != '-' ) return ARG_PARSE_OPTION_MISSING;
        if ( strncmp(argv[1] , "-f", 2) != 0 ) return ARG_PARSE_OPTION_UNKNOWN;
        this->setInFileName( argv[2] );
        return ARG_PARSE_FROM_FILE;
    }
    #endif
    return ARG_PARSE_ARG_TOO_MANY;
}

cmdStat cmdParser::readCmd() {
    cmdStat stat = this->readChar(cin);
    if ( stat == CMD_EXECUTE ) {
        stat = this->interpretateAndExecute();
        if ( stat == CMD_CHANGE_REMOTE_SCOPE ) _scope = REMOTE;
        if ( stat == CMD_CHANGE_LOCAL_SCOPE  ) _scope = LOCAL;
    }
    errMgr.handle(stat);
    return stat;
}


/*******************/
/* private methods */
/*******************/

cmdStat cmdParser::readChar(std::istream& stream) {
    // The buffer will reset in resetBuf()
    // Don't reset the buffer when NEW_LINE_KEY is pressed
    // The content is needed for command execution
    resetBuf();
    while (1) {
        ParseChar pch = getChar(stream);
        if (pch == INPUT_END_KEY) {
            cout << endl;
            return CMD_EXIT;
        }
        switch (pch) {
            case NEW_LINE_KEY:    this->newLineCmd(); return CMD_EXECUTE; // the enter key is the key to trigger commands
            case LINE_BEGIN_KEY:  this->moveBufPtr(_buf); break;
            case LINE_END_KEY:    this->moveBufPtr(_bufEnd); break;
            case HOME_KEY:        this->moveBufPtr(_buf); break;
            case END_KEY:         this->moveBufPtr(_bufEnd); break;
            case BACK_SPACE_KEY:  this->backSpaceChar(); this->makeCopy(); break;
            case DELETE_KEY:      this->deleteChar(); this->makeCopy(); break;
            case TAB_KEY:         this->autoComplete(); break;
            case ARROW_UP_KEY:    this->retrieveHistory(short(_hisID-1)); break;
            case ARROW_DOWN_KEY:  this->retrieveHistory(short(_hisID+1)); break;
            case ARROW_LEFT_KEY:  this->moveBufPtr(_bufPtr-1); break;
            case ARROW_RIGHT_KEY: this->moveBufPtr(_bufPtr+1); break;
            case PG_UP_KEY:       this->retrieveHistory(short(_hisID-PG_OFFSET)); break;
            case PG_DOWN_KEY:     this->retrieveHistory(short(_hisID+PG_OFFSET)); break;
            case UNDEF_KEY:       bell(); break;
            default:              this->insertChar(char(pch)); this->makeCopy(); break;
        }
    }
}

/*******************************/
/* commands are CASE SENSITIVE!*/
/*******************************/
// TODO check if commands contain illegal keywords i.e. non-alphabetical characters
//      string optimized!
cmdStat cmdParser::regEachCmd(const char* cmd, size_t minCmp, cmdExec* cmdHandler) {
    // check for ambiguity
    std::string_view tmp = cmd;
    while (true) {
        cmdExec* cmdCheck;
        if ( (cmdCheck = this->getCmdHandler(tmp)) ) {
            fprintf(stderr, "Ambiguity detected for command \'%s\' and \'%s\'\n",
                    cmd,
                    cmdCheck->getCmdStrData());
            fprintf(stderr, "Check the \'minCmp\' variable in cmdClass.cpp\n");
            return CMD_REG_FAIL;
        }
        if ( tmp.size() == minCmp ) break;
        tmp.remove_suffix(1);
    }

    // set its keyword, optional string, and register cmd
    cmdHandler->setCmdStr( cmd );
    cmdHandler->setKeyWordOptional( minCmp );
    auto check = _cmdMap.emplace( cmdHandler->getKeyWordView(), cmdHandler );

    if ( check.second ) return this->regEachCmdOption(cmdHandler);
    else {
        fprintf(stderr, "Command keyword conflict in command \'%s\', \'%s\'\n",
                cmd,
                check.first->second->getCmdStrData());
        return CMD_REG_FAIL;
    }
}

// analyze the command keyword and its option
cmdStat cmdParser::interpretateAndExecute() const {
    size_t buflen = (size_t)(_bufEnd - _buf);

    // get rid of leading white spaces
    const char* buf_start = _buf;
    while ( buflen ) {
        if ( *buf_start != SPACE_CHAR ) break;
        buf_start += 1;
        buflen -= 1;
    }

    // get rid of trailing white spaces
    const char* buf_end = _bufEnd-1;
    while ( buflen ) {
        if ( *buf_end != SPACE_CHAR ) break;
        if ( *buf_end == SPACE_CHAR && *(buf_end-1) == ESCAPE_CHAR ) break;
        buf_end -= 1;
        buflen -= 1;
    }

    if ( !buflen ) return CMD_DONE;

    std::string_view buf_view( buf_start, buflen );
    size_t w_index = buf_view.find_first_of(' ');
    w_index = w_index == std::string::npos ? buf_view.size() : w_index;
    std::string_view cmd_view = buf_view.substr( 0, w_index );
    std::string_view opt_view = buflen < 2 ? EMPTY_STRING
                                           : w_index <= buf_view.size()-2 ? buf_view.substr( w_index+1 )
                                                                          : EMPTY_STRING;
    // get rid of leading white spaces in opt_view
    opt_view = opt_view.size() ? opt_view.substr( opt_view.find_first_not_of(' ') ) : opt_view;

    cmdExec* cmdHandler;
    if ( (cmdHandler = this->getCmdHandler(cmd_view)) == NULL )
        return CMD_ERROR;
    else
        return cmdHandler->execute(opt_view);

    /*
    size_t optEnd = _bufEnd - _buf;

    // dummy check first
    if ( optEnd == 0 ) return CMD_DONE;

    // split _buf into two parts
    std::vector<std::string> tokens;
    size_t buflen = _bufEnd - _buf;
    char* sub = (char*)malloc((buflen+1)*sizeof(char));
    UTIL::substr( _buf, sub, 0, buflen );
    UTIL::parseTokens( sub, tokens, ' ', 2 );
    free(sub);
    if ( tokens.empty() ) return CMD_DONE;
    if ( tokens.size() == 1 ) tokens.emplace_back("");

    // find the corresponding command class and execute
    cmdExec* cmdHandler;
    if ( (cmdHandler = this->getCmdHandler(tokens[0])) == NULL ) return CMD_ERROR;
    else return cmdHandler->execute(tokens[1]);
    */
}

/**************************************
 * -- comparisons are CASE SENSITIVE  *
 *    in command strings              *
 * -- for a command to be recognized, *
 *    the query string has to be at   *
 *    least as long as the keyword    *
 *************************************/
cmdExec* cmdParser::getCmdHandler(const std::string_view& cmd) const {
    for (const auto& pair : _cmdMap) {
        const auto& keyword    = pair.first;
        const auto& cmdHandler = pair.second;
        const auto& cmdstr     = cmdHandler->getCmdStr();

        if ( cmd.length() > cmdstr.length() ) continue;
        if ( cmd.length() < keyword.length() ) continue;
        if ( UTIL::strNcmp(cmd, cmdstr, cmd.size()) == 0 ) {
            return cmdHandler;
        }
    }

    // error command
    errMgr.setErrCmd(cmd);
    return NULL;
}

/**********************************/
/* _buf:    [________________]    */
/*           ⋀         ⋀     ⋀    */
/*           │         │     │    */
/*         _buf        │     │    */
/*                     │  _bufEnd */
/*                  _bufPtr       */
/*             (cursor postion)   */
/**********************************/

void cmdParser::insertChar(char c, int count) {

    // store the "postfix"
    int n = int(_bufEnd - _bufPtr);
    std::vector<char> postfix;
    postfix.reserve(n);
    for (int i = 0; i < n; ++i) {
        postfix[i] = *(_bufPtr+i);
    }

    // insert the characters
    for (int i = 0; i < count; ++i) {
        cout << c;
        *_bufPtr = c;
        ++_bufPtr;
        ++_bufEnd;
    }

    // output and store the "postfix" back
    for (int i = 0; i < n; ++i) {
        cout << postfix[i];
        *(_bufPtr+i) = postfix[i];
    }

    // move the cursor back
    for (int i = 0; i < n; ++i) {
        cout << char(BACK_SPACE_CHAR);
    }

}

void cmdParser::insertStr(const char* str, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        this->insertChar(str[i]);
    }
}

/***********************/
/* move _bufPtr to pos */
/***********************/
void cmdParser::moveBufPtr(const char* pos) {

    // cannot move further left/right
    if ( pos-1 == _bufEnd || pos+1 == _buf) {
        bell();
        return;
    }

    if ( pos < _bufPtr ) {
        while ( _bufPtr != pos ) {
            cout << char(BACK_SPACE_CHAR);
            --_bufPtr;
        }
    }
    else if ( pos > _bufPtr ) {
        while ( _bufPtr != pos ) {
            cout << (*_bufPtr);
            ++_bufPtr;
        }
    }
    else return;
}

void cmdParser::backSpaceChar() {

    // dummy check
    if ( _bufPtr == _buf ) {
        bell();
        return;
    }

    this->moveBufPtr(_bufPtr-1);
    this->deleteChar();
}

/***********************************************/
/* delete the character that _bufPtr points to */
/***********************************************/
void cmdParser::deleteChar() {

    // dummy check
    if ( _bufPtr == _bufEnd ) {
        bell();
        return;
    }

    // output and store the "postfix" excluding the to-be-deleted char
    int n = int(_bufEnd - _bufPtr) - 1;
    for (int i = 0; i < n; ++i) {
        cout << *(_bufPtr+i+1);
        *(_bufPtr+i) = *(_bufPtr+i+1);
    }

    // output white space to cover the last character
    cout << ' ';

    // move the cursor back
    for (int i = 0; i < n+1; ++i) {
        cout << char(BACK_SPACE_CHAR);
    }

    // fix the end pointer
    --_bufEnd;
}

// complete commands or file paths or just tabs
void cmdParser::autoComplete() {
    size_t buflen = (size_t)(_bufPtr - _buf);
    char* buf     = (char*)malloc((buflen+1)*sizeof(char));
    std::vector<std::string_view> tokens;
    UTIL::substr( _buf, buf, 0, buflen );
    UTIL::parseTokens(buf, tokens);
    if ( tokens.empty() ) {
        this->insertChar(' ', TAB_STOP);
    }
    else if ( tokens.size() == 1 && *(_bufPtr-1) != ' ' ) {
        // this is safe because only alphabetical characters are allowed in command keywords
        this->completeCmd( tokens[0] );
    }
    else {
        cmpltStat stat;
        bool dirOnly = ( tokens[0]=="cd" || tokens[0]=="lcd" );
        short scope = ( tokens[0] == "lls" || tokens[0] == "lcd" || tokens[0] == "put") ? LOCAL : REMOTE;
        // buflen is garuanteed to be at least 2, so the statement is safe
        // make sure that space is not preceded by an escape chartacter
        if ( *(_bufPtr-1) == SPACE_CHAR && *(_bufPtr-2) != ESCAPE_CHAR ) {
            stat = this->completePath( "", scope, dirOnly );
        }
        else {
            stat = this->completePath( tokens.back(), scope, dirOnly );
        }
        if ( !errMgr.handle( stat ) ) {
            this->rePrintBuf();
        }
    }
    free(buf);
}

void cmdParser::completeCmd(const std::string_view& prtCmd) {
    int printWidth = MATCH_KEY_OUTPUT_MIN_WIDTH;

    // second entry in each element indicates whether
    // the matched item is a direcotry
    // store the entire command instead of just the keyword
    std::vector<std::pair<std::string_view, bool> > matched;
    for (const auto& pair : _cmdMap) {
        const auto& handler = pair.second;
        const auto& fullkey = handler->getCmdStr();
        if ( prtCmd.length() > fullkey.length() ) continue;
        if ( UTIL::strNcmp( prtCmd, fullkey, prtCmd.length() ) == 0 ) {
            matched.emplace_back( fullkey, false );
            printWidth = std::max(printWidth, (int)fullkey.length());
        }
    }

    if ( matched.size() == 0 ) { // no match, beep
        bell();
    }
    else if ( matched.size() == 1 ) { // found match, complete
        this->insertStr( matched[0].first.substr(prtCmd.size()).data(),
                         matched[0].first.size()-prtCmd.size() );
        this->insertChar(' ');
    }
    else { // more than one match

        // check whether the matched string can be extended
        size_t matchIdEnd = prtCmd.size();
        char   matchChar  = matched[0].first[matchIdEnd];
        bool   terminate  = false;
        while (true) {
            for (size_t i = 1; i < matched.size(); ++i) {
                if ( matched[i].first[matchIdEnd] != matchChar ) {
                    terminate = true;
                    break;
                }
            }
            if (terminate) break;
            else ++matchIdEnd;
        }

        if ( matchIdEnd == prtCmd.size() ) { // show matched options
            this->showMatched( matched, printWidth );
        }
        else { // extend matched string
            this->insertStr( matched[0].first.substr(prtCmd.size(), matchIdEnd-prtCmd.size()).data(),
                             matchIdEnd-prtCmd.size() );
        }
    }
}

// TODO zsh-like tab selection
// TODO when prtPath starts with '/', make sure it reads from absolute path
// TODO place '\' before a space/parathesis in a filename that contains them
cmpltStat cmdParser::completePath(const std::string_view& prtPath, short scope, bool dirOnly) {
    int printWidth = MATCH_KEY_OUTPUT_MIN_WIDTH;

    // the second entry in each element indicates whether
    // the matched item is a direcotry
    // the matched entries need to be stored entirly (string_view NOT OK!)
    std::vector<std::pair<std::string, bool> > matched;

    // make sure to read the correct dir
    std::string_view targetDir, prtFile;
    UTIL::splitPathFile(prtPath, targetDir, prtFile);

    // read from the current scope
    cmpltStat stat = sftp::SFTP_OK;
    if ( scope == LOCAL )  UTIL::readDir(targetDir, matched);
    if ( scope == REMOTE ) stat = this->_sftp_sess->readDir(targetDir, matched);
    if ( stat   != sftp::SFTP_OK ) return stat;

    // don't show hidden files unless query string starts with '.'
    bool ignoreHidden = prtFile.size() ? prtFile[0] != '.' : true;

    // check matched
    for (size_t i = 0; i < matched.size(); ++i) {
        if ( dirOnly && !matched[i].second ) { // filter out not-direcotry entries if necessary
            std::swap( matched[i], matched.back() );
            matched.pop_back();
            --i;
            continue;
        }
        if ( matched[i].first[0] == '.' && ignoreHidden ) { // filter out hidden files if necessary
            std::swap( matched[i], matched.back() );
            matched.pop_back();
            --i;
            continue;
        }
        if ( UTIL::strNcmp_soft( prtFile, matched[i].first, prtFile.length() ) != 0 ||
             prtFile.length() > matched[i].first.length() ) { // mismatch
            std::swap( matched[i], matched.back() );
            matched.pop_back();
            --i;
        }
        else {
            printWidth = std::max(printWidth, (int)matched[i].first.length());
        }
    }

    if ( matched.size() == 0 ) { // no match, beep
        bell();
    }
    else if ( matched.size() == 1 ) { // found match, complete
        // re-insert the matched string to activate case-insensitive comparison
        for (size_t i = 0; i < prtFile.size(); ++i) this->backSpaceChar();
        this->insertStr( matched.front().first.c_str(), matched.front().first.length() );
        // insert a trailing space iff entry is NOT a directory
        if ( !matched[0].second ) this->insertChar(' ');
        else                      this->insertChar('/');
    }
    else { // more than one match

        // check whether the matched string can be extended
        size_t matchIdEnd = prtFile.size();
        bool   terminate  = false;
        while (true) {
            char matchChar;
            if ( matched[0].first.size() == prtFile.size() ) break;
            else matchChar = matched[0].first[matchIdEnd];
            for (size_t i = 1; i < matched.size(); ++i) {
                if ( matched[i].first[matchIdEnd] != matchChar ) {
                    terminate = true;
                    break;
                }
            }
            if (terminate) break;
            else ++matchIdEnd;
        }

        if ( matchIdEnd == prtFile.size() ) { // show matched options
            this->showMatched( matched, printWidth );
        }
        else { // extend matched string
            // -- if the matched strings are all identical after case sensitive comparison,
            //    re-insert the matched string to activate case-insensitive comparison,
            //    otherwise, keep the original prtFile
            bool caseConsistent = true;
            const auto& ref = matched.front().first;
            for (size_t i = 1; i < matched.size(); ++i) {
                if ( UTIL::strNcmp(ref, matched[i].first, matchIdEnd) != 0 ) {
                    caseConsistent = false;
                    break;
                }
            }

            if ( caseConsistent ) {
                for (size_t i = 0; i < prtFile.size(); ++i) this->backSpaceChar();
                this->insertStr( matched.front().first.c_str(), matchIdEnd );
            }
            else {
                this->insertStr( matched.front().first.substr(prtFile.size(), matchIdEnd-prtFile.size()).data(),
                                 matchIdEnd-prtFile.size());
            }
        }
    }
    return stat;
}

template<typename T>
void cmdParser::showMatched(const T& matched, int printWidth) {
    printWidth += 3;
    int twidth = UTIL::getTermWidth();
    int nItms  = twidth / printWidth;
    int count  = 0;

    // print matches after newline
    cout << endl;
    for (size_t i = 0; i < matched.size(); ++i) {
        if ( !matched[i].second ) {
            cout << left << setw(printWidth) << matched[i].first;
        }
        else if ( errMgr.colorOutput() ) {
            cout << BOLD_RED;
            cout << matched[i].first << COLOR_RESET << left << setw(printWidth-(int)matched[i].first.size()) << '/';
        }
        else {
            cout << matched[i].first << left << setw(printWidth-(int)matched[i].first.size()) << '/';
        }
        if ( ++count == nItms ) {
            count = 0;
            cout << endl;
        }
        else cout << ' ';
    }
    if ( matched.size()%nItms ) cout << endl;

    // re-print what _buf contains
    this->rePrintBuf();
}

void cmdParser::rePrintBuf() {
    this->printPrompt();
    size_t length = _bufEnd - _buf;
    for (size_t i = 0; i < length; ++i) {
        cout << _buf[i];
    }

    // move the cursor to where it belongs
    length = _bufEnd - _bufPtr;
    for (size_t i = 0; i < length; ++i) {
        cout << char(BACK_SPACE_CHAR);
    }
}

// TODO: doesn't work like standard terminal during string matching
// TODO: history size is not restricted
void cmdParser::retrieveHistory(short tID) {

    // dummy check
    if ( tID <= -1 ) {
        tID = 0;
    }
    if ( tID >= (short)_history.size()+1 ) {
        tID = (short)_history.size();
    }

    const std::string* targetString = tID == (short)_history.size() ? &_bufTmp : &_history[tID];

    // clear buffer
    int n = int(_bufEnd - _buf);
    int i = 0;
    for (; i < n; ++i) cout << char(BACK_SPACE_CHAR);

    // paste history
    _bufPtr = _bufEnd  = _buf + targetString->size();
    for (i = 0; i < (int)targetString->size(); ++i) {
        _buf[i] = targetString->at(i);
        cout << _buf[i];
    }

    // remove unwanted characters
    int blanks = n - i;
    if ( blanks > 0 ) {
        for (int j = 0; j < blanks; ++j) cout << ' ';
        for (int j = 0; j < blanks; ++j) cout << char(BACK_SPACE_CHAR);
    }

    // update _hisID
    _hisID = tID;
}

void cmdParser::newLineCmd() {

    // save history
    size_t n = _bufEnd - _buf;
    if (n) {
        _history.resize( _history.size()+1 );
        _history.back().resize( n );
        for (size_t i = 0; i < n; ++i) _history.back()[i] = _buf[i];
        _hisID = (short)_history.size();
    }

    // clear _buf's copy
    _bufTmp.clear();

    // end line
    cout << endl;
}

void cmdParser::resetBuf() {
    _bufEnd = _bufPtr = _buf;
    this->printPrompt();

}

// TODO: should keep track of whether cd/lcd was triggered
static char cwdBuf[CWD_BUF_MAX];
static char cwdBufabbrv[CWD_BUF_MAX];
void cmdParser::printPrompt() {

    // this part is for testing
    if ( !errMgr.colorOutput() ) {
        cout << _prompt << "> ";
        return;
    }

    std::ostringstream sswd;
    if ( _scope == LOCAL )  this->getLocalCWD( sswd );
    if ( _scope == REMOTE ) this->getRemoteCWD( sswd );

    cout << sswd.str() << " » ";
}

void cmdParser::getLocalCWD(std::ostringstream& ss) const {
    ss << BOLD_YELLOW << '[' << _prompt << ']';
    ss << BOLD_CYAN   << " local ";
    ss << BOLD_RED    << " ➜ ";

    if ( getcwd(cwdBuf, CWD_BUF_MAX) == NULL ) {
        // TODO
        // dynamic memory allocation is not implemented
        // if this happens, program aborts
        exit(-1);
    }

    // replace $HOME with "~/"
    if ( UTIL::strNcmp(cwdBuf, _home, _hlen) == 0 ) {
        cwdBufabbrv[0] = '~';
        UTIL::substr( cwdBuf, cwdBufabbrv, _hlen, 0xffffffffffffffff, 1 );
        std::swap( cwdBuf, cwdBufabbrv );
    }

    // find the last CWD_DEPTH_MAX directories
    this->trimPath( cwdBuf );

    ss << BOLD_GREEN  << cwdBufabbrv << COLOR_RESET;
}

void cmdParser::getRemoteCWD(std::ostringstream& ss) const {
    const char* cwdRemote = this->_sftp_sess->pwd();
    this->trimPath( cwdRemote );
    ss << BOLD_YELLOW << '[' << _prompt << ']';
    ss << BOLD_CYAN   << " remote";
    ss << BOLD_RED    << " ➜ ";
    ss << BOLD_GREEN << cwdBufabbrv << COLOR_RESET;
}

// the trimed path will be saved the cwdBufabbrv
// which is a static char*
void cmdParser::trimPath(const char* fullpath) const {
    int pos = (int)strlen(fullpath)-1;
    int count = 0;
    for (; pos > 0; --pos) {
        if ( fullpath[pos] == '/' ) ++count;
        if ( count == CWD_DEPTH_MAX ) {
            ++pos; // do not print the thrid '/'
            break;
        }
    }
    UTIL::substr(fullpath, cwdBufabbrv, pos, strlen(fullpath)-pos+1);
}

void cmdParser::makeCopy() {

    // TODO: make sure that when the _bufTmpPtr is not
    //       pointing to the end, history function stils work

    // make a copy
    // this is for the history to work
    _bufTmp.resize(size_t(_bufEnd-_buf));
    for (size_t i = 0; i < _bufTmp.size(); ++i) {
        _bufTmp[i] = _buf[i];
    }
}

#ifdef DEV
void cmdParser::readFile() {
    std::ifstream file(_filename);
    if ( !file.good() ) {
        std::string msg = "readFile: " + std::string(_filename);
        perror(msg.c_str());
        exit(-1);
    }

    while ( this->readCmdFile(file) != CMD_EXIT ) {}
    file.close();
}

cmdStat cmdParser::readCmdFile(std::ifstream& file) {
    cmdStat stat = this->readChar(file);
    if ( stat == CMD_EXECUTE ) {
        stat = this->interpretateAndExecute();
    }
    errMgr.handle(stat);
    return stat;
}
#endif
