#include "cmdParser.h"
#include "util.h"
#include <cassert>
#include <string>
#include <vector>

cmdStat cmdParser::readCmd() {
    cmdStat stat = this->readChar(cin);
    if ( stat == CMD_EXECUTE ) {
        stat = this->interpretateAndExecute();
    }
    _errMgr.handle(stat);
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
            case PG_UP_KEY:       this->retrieveHistory(short(_hisID+PG_OFFSET)); break;
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
    else {
        cout << "command recongnized: " << cmd << endl;
        return cmdHandler->execute(opt);
    }
}

/*****************************************
 * String comparisons are CASE SENSITIVE
 * This function is mainly used to check for 
 * command ambiguity and does not necessarrily
 * returns the corresponing cmdHandler 
 * EX:
 *    if there are two keys: "ga", "gu" and 
 *    cmd="g", it returns which ever is 
 *    first traversed
 *****************************************/
cmdExec* cmdParser::getCmdHandler(const std::string& cmd) const {
    for (const auto& pair : _cmdMap) {
        const auto& cmdHandler = pair.second;
        const auto& keyword    = pair.first;
        const auto& optional   = cmdHandler->getOptionalStr();

        if ( cmd.length() > keyword.length()+optional.length() ) continue;
        else if ( cmd.length() > keyword.length() ) {
            if ( UTIL::strNcmp(cmd, keyword+optional, cmd.size()) == 0 ) {
                return cmdHandler;
            }
        }
        else {
            if ( UTIL::strNcmp(cmd, keyword, cmd.size()) == 0) {
                return pair.second;
            }
        }
    }

    // error command
    _errMgr.setErrCmd(cmd);
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

// TODO: tab keys
void cmdParser::autoComplete() {
    std::vector<std::string> ans;
    UTIL::readDir("./", ans);
    for (auto s : ans) {
        cout << s << endl;
    }
}

// TODO: doesn't work like standard terminal during string matching
void cmdParser::retrieveHistory(short tID) {

    // dummy check
    if ( tID == -1 ) return;
    if ( tID == (short)_history.size()+1 ) return;

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
