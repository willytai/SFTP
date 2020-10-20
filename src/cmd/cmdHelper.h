#ifndef __CMD_HELPER__
#define __CMD_HELPER__

#include <dirent.h>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <climits>
#include "util.h"
#include "def.h"

namespace LIST      /* helper functions for 'lls' and 'ls' */
{

typedef std::vector<dirent*> Files;
typedef std::unordered_map<std::string, Files> dirCntMap;

/*******************************/
/* flags for directory listing */
/* only accept lower case!!!!  */
/* (max left shift = 26)       */
/*******************************/
enum lsFlag
{
    HUMAN_READABLE = (1 << (int('h')-int('a'))), // -h
    LIST_ALL       = (1 << (int('a')-int('a'))), // -a
    LIST_LONG      = (1 << (int('l')-int('a'))), // -l
    LIST_XATTR     = (1 << 26),                  // -@

    UNDEF_FLAG     = INT_MAX
};

lsFlag getFlag    (const char& c);
bool   checkFlag  (const lsFlag& f, const int& stat);
bool   listPrint  (const dirCntMap& dirContent, bool all, bool human, bool printDirName, const char* cmd);
bool   columnPrint(const dirCntMap& dirContent, bool all, bool human, bool printDirName, const char* cmd);

}

#endif /* __CMD_HELPER__ */
