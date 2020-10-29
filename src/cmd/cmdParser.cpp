#include "cmdParser.h"
#include "cmdCharDef.h"
#include "util.h"
#include "dirIO.h"
#include <cassert>
#include <string>
#include <utility>
#include <vector>

extern errorMgr errMgr;

cmdStat cmdParser::readCmd() {
    cmdStat stat = this->readChar(cin);
    if ( stat == CMD_EXECUTE ) {
        stat = this->interpretateAndExecute();
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
cmdStat cmdParser::regEachCmd(std::string cmd, size_t minCmp, cmdExec* cmdHandler) {
    // check for ambiguity
    std::string tmp = cmd;
    while (true) {
        cmdExec* cmdCheck;
        if ( (cmdCheck = this->getCmdHandler(tmp)) ) {
            fprintf(stderr, "Ambiguity detected for command \'%s\' and \'%s%s\'\n",
                    cmd.c_str(),
                    cmdCheck->getKeyWord(),
                    cmdCheck->getOptional());
            fprintf(stderr, "Check the \'minCmp\' variable in cmdClass.cpp\n");
            return CMD_REG_FAIL;
        }
        if ( tmp.size() == minCmp ) break;
        tmp.resize(tmp.size()-1);
    }

    // set its keyword, optional string, and register cmd
    tmp = cmd.substr(minCmp);
    cmdHandler->setOptional(tmp);
    tmp = cmd.substr(0, minCmp);
    cmdHandler->setKeyWord(tmp);
    auto check = _cmdMap.insert(cmdKeyHandlerPair(tmp, cmdHandler));

    if ( check.second ) return CMD_REG_DONE;
    else {
        fprintf(stderr, "Command keyword conflict in command \'%s\', \'%s%s\'\n",
                cmd.c_str(),
                check.first->first.c_str(),
                check.first->second->getOptional());
        return CMD_REG_FAIL;
    }
}

cmdStat cmdParser::interpretateAndExecute() const {

    /*
     * analyze the command keyword and its option
     * ignore the white spaces in the beginning and end
     */
    size_t optEnd = _bufEnd - _buf;

    // dummy check first
    if ( optEnd == 0 ) return CMD_DONE;
    while ( _buf[optEnd-1] == ' ' ) {
        // command with entire white spaces
        if ( optEnd == 1 ) return CMD_DONE;
        else --optEnd;
    }
    size_t cmdStart = 0;
    for (; cmdStart < optEnd; ++cmdStart) if ( _buf[cmdStart] != ' ' ) break;
    size_t cmdEnd = cmdStart+1;
    for (; cmdEnd < optEnd; ++cmdEnd) if ( _buf[cmdEnd] == ' ' ) break;
    size_t optStart = cmdEnd;
    for (; optStart < optEnd; ++optStart) if ( _buf[optStart] != ' ' ) break;
    std::string cmd, opt;
    cmd.resize(cmdEnd-cmdStart);
    opt.resize(optEnd-optStart);
    for (size_t i = 0; i < cmd.size(); ++i) {
        cmd[i] = _buf[cmdStart+i];
    }
    for (size_t i = 0; i < opt.size(); ++i) {
        opt[i] = _buf[optStart+i];
    }

    // find the corresponding command class and execute
    cmdExec* cmdHandler;
    if ( (cmdHandler = this->getCmdHandler(cmd)) == NULL ) return CMD_ERROR;
    else return cmdHandler->execute(opt);
}

/**************************************
 * -- comparisons are CASE SENSITIVE  *
 *    in command strings              *
 * -- for a command to be recognized, *
 *    the query string has to be at   *
 *    least as long as the keyword    *
 *************************************/
cmdExec* cmdParser::getCmdHandler(const std::string& cmd) const {
    for (const auto& pair : _cmdMap) {
        const auto& cmdHandler = pair.second;
        const auto& keyword    = pair.first;
        const auto& optional   = cmdHandler->getOptionalStr();

        if ( cmd.length() > keyword.length()+optional.length() ) continue;
        if ( cmd.length() < keyword.length() ) continue;
        if ( UTIL::strNcmp(cmd, keyword+optional, cmd.size()) == 0 ) {
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
    std::vector<std::string> tokens;
    std::string buf; buf.resize(_bufPtr-_buf);
    for (size_t i = 0; i < buf.length(); ++i) buf[i] = _buf[i];
    UTIL::parseTokens(buf, tokens);
    if ( tokens.empty() ) {
        this->insertChar(' ', TAB_STOP);
    }
    else if ( tokens.size() == 1 && *(_bufPtr-1) != ' ' ) {
        this->completeCmd( tokens[0] );
    }
    else {
        if ( *(_bufPtr-1) == ' ' ) {
            this->completePath( "" );
        }
        else {
            this->completePath( tokens.back() );
        }
    }
}

void cmdParser::completeCmd(const std::string& prtCmd) {
    int printWidth = MATCH_KEY_OUTPUT_MIN_WIDTH;

    // the second entry in each element indicates whether
    // the matched item is a direcotry or not
    std::vector<std::pair<std::string, bool> > matched;
    for (const auto& pair : _cmdMap) {
        const auto& key = pair.first;
        const auto& hdl = pair.second;
        if ( prtCmd.length() > key.length() ) continue;
        if ( UTIL::strNcmp( prtCmd, key, prtCmd.length() ) == 0 ) {

            // store the entire command instead of just the keyword
            matched.emplace_back( hdl->getCmdStr(), false );
            printWidth = std::max(printWidth, (int)key.length());
        }
    }

    if ( matched.size() == 0 ) { // no match, beep
        bell();
    }
    else if ( matched.size() == 1 ) { // found match, complete
        this->insertStr( matched[0].first.substr(prtCmd.size()).c_str(),
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
            this->insertStr( matched[0].first.substr(prtCmd.size(), matchIdEnd-prtCmd.size()).c_str(),
                             matchIdEnd-prtCmd.size() );
        }
    }
}

// TODO zsh-like tab selection
void cmdParser::completePath(const std::string& prtPath) {
    int printWidth = MATCH_KEY_OUTPUT_MIN_WIDTH;

    // the second entry in each element indicates whether
    // the matched item is a direcotry
    std::vector<std::pair<std::string, bool> > matched;

    // don't show hidden files unless query string starts with '.'
    bool ignoreHidden = prtPath.size() ? prtPath[0] != '.' : true;

    // make sure to read the correct dir
    std::string targetDir, prtFile;
    UTIL::splitPathFile(prtPath, targetDir, prtFile);
    UTIL::readDir(targetDir.c_str(), matched);

    // check matched
    for (size_t i = 0; i < matched.size(); ++i) {
        if ( matched[i].first[0] == '.' && ignoreHidden ) { 
            std::swap( matched[i], matched.back() );
            matched.pop_back();
            --i;
            continue;
        }
        if ( UTIL::strNcmp_soft( prtFile, matched[i].first, prtFile.length() ) != 0 ||
             prtFile.length() > matched[i].first.length() ) {
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
            const std::string& ref = matched.front().first;
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
                this->insertStr( matched.front().first.substr(prtFile.size(), matchIdEnd-prtFile.size()).c_str(),
                                 matchIdEnd-prtFile.size());
            }
        }
    }
}

void cmdParser::showMatched(const std::vector<std::pair<std::string, bool> >& matched, int printWidth) {
    int twidth = UTIL::getTermWidth();
    int nItms  = twidth / (printWidth + 1);
    int count  = 0;

    // print matches after newline
    cout << endl;
    for (size_t i = 0; i < matched.size(); ++i) {
        cout << left << setw(printWidth) << matched[i].first;
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

// TODO: print pwd in prompt
void cmdParser::newLineCmd() {

    // save history
    size_t n = _bufEnd - _buf;
    if (n) {
        std::string tmp;
        tmp.resize(n);
        for (size_t i = 0; i < n; ++i) tmp[i] = _buf[i];
        _history.emplace_back(tmp);
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

void cmdParser::printPrompt() {
    cout << _prompt << "> ";
}

void cmdParser::makeCopy() {

    // TODO: make sure that when the _bufTmpPtr is no 
    //       pointing to the end, history function stils work

    // make a copy
    // this is for the history to work
    _bufTmp.resize(size_t(_bufEnd-_buf));
    for (size_t i = 0; i < _bufTmp.size(); ++i) {
        _bufTmp[i] = _buf[i];
    }
}

#ifdef DEV
void cmdParser::readFile(const char* filename) {
    _file.open(filename);
    if ( !_file.good() ) {
        std::string msg = "readFile: " + std::string(filename);
        perror(msg.c_str());
        exit(-1);
    }

    while ( this->readCmdFile() != CMD_EXIT ) {}
    _file.close();
}

cmdStat cmdParser::readCmdFile() {
    cmdStat stat = this->readChar(_file);
    if ( stat == CMD_EXECUTE ) {
        stat = this->interpretateAndExecute();
    }
    errMgr.handle(stat);
    return stat;
}
#endif
