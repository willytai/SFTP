#include <termios.h>
#include <iomanip>
#include "cmdCharDef.h"

// this is for the original terminal IO settings
static struct termios stored_settings;

static void reset_termio() {
    tcsetattr(0, TCSANOW, &stored_settings);
}

static void set_termio() {
    struct termios new_settings;
    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;  // make a copy
    new_settings.c_lflag &= ~ICANON; // disable input cononical mode
    new_settings.c_lflag &= ~ECHO;   // disable echo mode
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0, &stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings); // apply new settings
}

char getc(istream& stream) {
    char ch;
    set_termio();
    stream.unsetf(std::ios_base::skipws); // DO NOT SKIP WHITE SPACES
    stream >> ch;
    stream.setf(std::ios_base::skipws);
    reset_termio();
    // #ifdef TEST_ASCII
    // cout << std::left << std::setw(6) << int(ch);
    // #endif
    return ch;
}

ParseChar getChar(istream& stream) {
    char ch = getc(stream);
    if ( stream.eof() ) return ParseChar(INPUT_END_KEY);
    switch (ch) {
        case LINE_BEGIN_KEY:  return ParseChar(LINE_BEGIN_KEY);
        case INPUT_END_KEY:   return ParseChar(INPUT_END_KEY);
        case LINE_END_KEY:    return ParseChar(LINE_END_KEY);
        case BELL_KEY:        return ParseChar(BELL_KEY);
        case BACK_SPACE_CHAR: return ParseChar(BACK_SPACE_CHAR);
        case TAB_KEY:         return ParseChar(TAB_KEY);
        case NEW_LINE_KEY:    return ParseChar(NEW_LINE_KEY);
        case BACK_SPACE_KEY:  return ParseChar(BACK_SPACE_KEY);

        // combo keys starts with ESC key
        case ESC_KEY:
        {
            int ch1 = int(getc(stream));

            // ARROW_KEY_INT and OTHER_KEY_INT collides
            if ( ch1 == int(ARROW_KEY_INT) ) {
                int ch2 = int(getc(stream));
                if ( int(ARROW_KEY_BEGIN) <= ch2 && ch2 <= int(ARROW_KEY_END) ) {
                    return ParseChar(ch2+ARROW_KEY_FLAG);
                }
                else if ( int(OTHER_KEY_BEGIN) <= ch2 && ch2 <= int(OTHER_KEY_END) ) {
                    if ( getc(stream) == OTHER_KEY_DUMMY ) return ParseChar(ch2+OTHER_KEY_FLAG);
                    else return ParseChar(UNDEF_KEY);
                }
                else return ParseChar(UNDEF_KEY);
            }
            else {
                bell(); return getChar(stream);
            }
        }

        // printable characters
        default:
            // if ( int(PRINTABLE_BEGIN) <= int(ch) && int(ch) <= int(PRINTABLE_END) ) return ParseChar(ch);
            // Doesn't need to check for PRINTABLE_END because CHAR_MAX is 255
            if ( int(PRINTABLE_BEGIN) <= int(ch) ) return ParseChar(ch);
            else return ParseChar(UNDEF_KEY);
    }
}

void bell() {
    cout << char(BELL_KEY);
}

