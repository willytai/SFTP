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
namespace LIST
{
typedef std::vector<std::string> Files;
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
