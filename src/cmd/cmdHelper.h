#ifndef __CMD_HELPER__
#define __CMD_HELPER__

#include <dirent.h>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <climits>
#include "def.h"

namespace LIST      /* helper functions for 'lls' and 'ls' */
{

struct EntryStat
{
    char            en_type;        // entry type
    char            en_perm[10];    // entry permission in string
    char            en_xattr;       // extended attributes
    size_t          en_nlink;       // number of hard links
    size_t          en_size;        // size
    char            en_mtime[13];   // last modification time in string
    const char*     en_name;        // entry name
    const char*     en_usrname;     // username
    const char*     en_grname;      // group name
    const char**    en_xattrList;   // the name of the extended attributes, NOT YET IMPLEMENTED
};

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
