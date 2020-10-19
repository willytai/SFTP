#ifndef __CMD_CHAR_DEF_H__
#define __CMD_CHAR_DEF_H__

#include <termios.h>
#include <climits>
#include "def.h"

/***************************************/
/* ASCII control characters    0 - 31  */
/* ASCII printable characters 32 - 127 */
/* extended ASCII codes      128 - 255 */
/***************************************/

enum ParseChar
{
    // range of printable characters
    PRINTABLE_BEGIN = 32,
    PRINTABLE_END   = 255,

    // Control keys
    LINE_BEGIN_KEY  = 1,          // Ctrl-a
    INPUT_END_KEY   = 4,          // Ctrl-d
    LINE_END_KEY    = 5,          // Ctrl-e
    BELL_KEY        = 7,          // makes a beep by outputing this char
    BACK_SPACE_CHAR = 8,          // goes back one space by outputing this char
    TAB_KEY         = int('\t'),
    NEW_LINE_KEY    = int('\n'),
    ESC_KEY         = 27,
    BACK_SPACE_KEY  = 127,

    // Combo keys: arrows
    // shift the arrow keys' codes by 256
    // TODO: this combo is tested on mac, should make a configure file if wish to make platform independent
    // 27 1 [65=up, 66=down, 67=right, 68=left]
    ARROW_KEY_FLAG  = 1 << 8,
    ARROW_KEY_INT   = 91,

    // mark the begin/end
    ARROW_KEY_BEGIN = 65,
    ARROW_KEY_END   = 68,

    ARROW_UP_KEY    = 65 + ARROW_KEY_FLAG,
    ARROW_DOWN_KEY  = 66 + ARROW_KEY_FLAG,
    ARROW_RIGHT_KEY = 67 + ARROW_KEY_FLAG,
    ARROW_LEFT_KEY  = 68 + ARROW_KEY_FLAG,


    // Combo keys: other
    // shift the other keys' codes by 512
    // 27 1 [49=home, 51=delete, 52=end, 53=pgUp, 54=pgDown] 126
    // INSERT_KEY is not supported
    // it is assumed that OTHER_KEY_INT==ARROW_KEY_INT, if not, then these combo keys will not be supported
    OTHER_KEY_FLAG  = 1 << 9,
    OTHER_KEY_INT   = 91,

    // mark the begin/end, end the last ascii
    OTHER_KEY_BEGIN = 49,
    OTHER_KEY_END   = 54,
    OTHER_KEY_DUMMY = 126,

    HOME_KEY        = 49 + OTHER_KEY_FLAG,
    DELETE_KEY      = 51 + OTHER_KEY_FLAG,
    END_KEY         = 52 + OTHER_KEY_FLAG,
    PG_UP_KEY       = 53 + OTHER_KEY_FLAG,
    PG_DOWN_KEY     = 54 + OTHER_KEY_FLAG,



    // Undefined keys
    UNDEF_KEY = INT_MAX,

    // DUMMY END
    PARSE_CHAR_END
};

/* Declaration */
char      getc(istream&);
void      bell();
ParseChar getChar(istream&);

#endif /* __CMD_CHAR_DEF_H__ */
