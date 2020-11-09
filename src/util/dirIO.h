#ifndef __DIR_IO_H__
#define __DIR_IO_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <vector>
#include <string>

namespace UTIL
{

bool   readDir(const char*, std::vector<dirent*>&);
bool   readDir(const char*, std::vector<std::pair<std::string, bool> >&);

struct EntryStat
{
static char TYPE_DIR;
static char TYPE_LNK;
static char TYPE_REG;
    char            en_type;        // entry type
    char            en_perm[10];    // entry permission in string
    char            en_xattr;       // extended attributes
    size_t          en_nlink;       // number of hard links
    size_t          en_size;        // the is the raw size (in bytes)
    double          en_size_h;      // the size after converted into human readable format, round to first decimal
    char            en_unit_h;      // unit of the human-readable size
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
void   getUnameByUid(const uid_t& uid, const char**);
void   getGnameByGid(const gid_t& gid, const char**);
char*  readLink     (const char*, const char*);

}

#endif /* __DIR_IO_H__ */
