#ifndef __UTIL_H__
#define __UTIL_H__

#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <vector>
#include <string>
#include "def.h"

namespace UTIL
{
bool   readDir(const char*, std::vector<dirent*>&);
bool   readDir(const char*, std::vector<std::string>&);


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

// Top API for retrieving file status
bool   getEntryStat(const char*, const char*, struct EntryStat*);

// for retrieving file status
void   getPermStr   (const mode_t& mode, char* modeStr);
void   getTypeChar  (const mode_t& mode, char& tchar);
void   getXattrChar (const char* filename, char& xchar, char* namebuf = NULL, size_t size = 0);
void   getTimeStampStr(const struct timespec& mtime, char* mtimeStr);
const char* getUnameByUid(const uid_t& uid);
const char* getGnameByGid(const gid_t& gid);



// common helper functions
void   parseTokens(const std::string&, std::vector<std::string>&, char delimiter = ' ');
size_t strNcmp(const std::string&, const std::string&, size_t);
int wLength(size_t);
int wLength(int);
int wLength(const char*);
}

#endif /* __UTIL_H__ */
