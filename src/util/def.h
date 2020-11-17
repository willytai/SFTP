#ifndef __DEF_F__
#define __DEF_F__

#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::left;
using std::right;
using std::setw;
using std::istream;

#define BOLD_BLACK      "\033[1;30m"
#define BOLD_RED        "\033[1;31m"
#define BOLD_GREEN      "\033[1;32m"
#define BOLD_YELLOW     "\033[1;33m"
#define BOLD_BLUE       "\033[1;34m"
#define BOLD_MAGENTA    "\033[1;35m"
#define BOLD_CYAN       "\033[1;36m"
#define BOLD_WHITE      "\033[1;37m"
#define NORMAL_BLACK    "\033[0;30m"
#define NORMAL_RED      "\033[0;31m"
#define NORMAL_GREEN    "\033[0;32m"
#define NORMAL_YELLOW   "\033[0;33m"
#define NORMAL_BLUE     "\033[0;34m"
#define NORMAL_MAGENTA  "\033[0;35m"
#define NORMAL_CYAN     "\033[0;36m"
#define NORMAL_WHITE    "\033[0;37m"
#define COLOR_RESET     "\033[0m"


// for prompt printing and listing
#define CWD_BUF_MAX 256
#define CWD_DEPTH_MAX 3
#define UNAME_BUF_MAX 32
#define GNAME_BUF_MAX UNAME_BUF_MAX

#define ESCAPE_CHAR '\\'
#define SPACE_CHAR ' '
#define WILDCARD_CHAR '*'

#endif /* __DEF_F__ */
