#ifndef __CMD_CLASS_H__
#define __CMD_CLASS_H__

#include "cmdParser.h"
#include <unordered_map>

/*************/
/* for local */
/*************/
cmdClass(llsCmd);
cmdClass(lcdCmd);


/**************/
/* for server */
/**************/
cmdClass(cdCmd);
cmdClass(lsCmd);
cmdClass(mkdirCmd);
cmdClass(rmCmd);
cmdClass(chmodCmd);


/***********************/
/* for program control */
/***********************/
cmdClass(quitCmd);


/************/
/* for sftp */
/************/
cmdClass(putCmd);
cmdClass(getCmd);


/*******************************/
/* flags for directory listing */
/* only accept lower case!!!!  */
/* (max left shift = 26)       */
/*******************************/
#include <dirent.h>
#include <iomanip>
namespace LIST
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

enum lsFlag
{
    HUMAN_READABLE = (1 << (int('h')-int('a'))), // -h
    LIST_ALL       = (1 << (int('a')-int('a'))), // -a
    LIST_LONG      = (1 << (int('l')-int('a'))), // -l

    UNDEF_FLAG     = INT_MAX
};

lsFlag getFlag(const char& c);
bool   checkFlag(const lsFlag& f, const int& stat);
void   listPrint  (const dirCntMap& dirContent, bool all, bool human);
void   columnPrint(const dirCntMap& dirContent, bool all, bool human);
}


#endif /* __CMD_CLASS_H__ */
