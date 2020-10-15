#include "cmdParser.h"
#include <cassert>
#include <vector>

void cmdParser::readCmd() {
    this->readChar(cin);
}



/*******************/
/* private methods */
/*******************/

void cmdParser::readChar(std::istream& stream) {
    resetBuf();
    while (1) {
        ParseChar pch = getChar(stream);
        if (pch == INPUT_END_KEY) {
            cout << endl;
            break;
        }
        switch (pch) {
            case LINE_BEGIN_KEY:  this->moveBufPtr(_buf); break;
            case LINE_END_KEY:    this->moveBufPtr(_bufEnd); break;
            case HOME_KEY:        this->moveBufPtr(_buf); break;
            case END_KEY:         this->moveBufPtr(_bufEnd); break;
            case BACK_SPACE_KEY:  this->backSpaceChar(); break;
            case DELETE_KEY:      this->deleteChar(); break;
            case NEW_LINE_KEY:    this->newLineCmd(); break;
            case TAB_KEY:         this->insertChar(' ', TAB_STOP); break;
            case ARROW_UP_KEY:    this->retrieveHistory(short(_hisID-1)); break;
            case ARROW_DOWN_KEY:  this->retrieveHistory(short(_hisID+1)); break;
            case ARROW_LEFT_KEY:  this->moveBufPtr(_bufPtr-1); break;
            case ARROW_RIGHT_KEY: this->moveBufPtr(_bufPtr+1); break;
            case PG_UP_KEY:       this->retrieveHistory(short(_hisID+PG_OFFSET)); break;
            case PG_DOWN_KEY:     this->retrieveHistory(short(_hisID+PG_OFFSET)); break;
            case UNDEF_KEY:       bell(); break;
            default:              this->insertChar(char(pch)); break;
        }
    }
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

    // make a copy
    // this is for the history to work
    _bufTmp.resize(size_t(_bufEnd-_buf));
    for (size_t i = 0; i < _bufTmp.size(); ++i) {
        _bufTmp[i] = _buf[i];
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

}

// TODO: doesn't work like standard terminal during string matching
void cmdParser::retrieveHistory(short tID) {

    // dummy check
    if ( tID == -1 ) return;
    if ( tID == (short)_history.size()+1 ) return;

    assert(tID >= 0 && tID <= (short)_history.size() );

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

    // end line and print prompt
    cout << endl;
    this->resetBuf();
}

void cmdParser::resetBuf() {
    _bufEnd = _bufPtr = _buf;
    this->printPrompt();
}

void cmdParser::printPrompt() {
    cout << _prompt << "> ";
}
