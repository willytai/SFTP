#include "dirIO.h"
#include <unistd.h>
#include <string.h>

namespace UTIL
{
// static data members initialization
char EntryStat::TYPE_DIR = 'd';
char EntryStat::TYPE_LNK = 'l';
char EntryStat::TYPE_REG = 'r';

// returns false if dir cannot be opened
// container will contain filenames and whether the corresponding entry is a directory
// TODO read dir/file whose names have spaces
bool readDir(const char* dir, std::vector<std::pair<std::string, bool> >& container) { 
    DIR* dirptr = opendir(dir);
    if ( dirptr == NULL ) return false;
    struct dirent* direntry;
    while ( (direntry = readdir(dirptr)) != NULL ) {
        container.emplace_back( direntry->d_name, direntry->d_type==DT_DIR );
    }
    return true;
}

bool readDir(const char* dir, std::vector<dirent*>& container) { // returns the entire entries' structure
    DIR* dirptr = opendir(dir);
    if ( dirptr == NULL ) return false;
    struct dirent* direntry;
    while ( (direntry = readdir(dirptr)) != NULL ) {
        container.push_back(direntry);
    }
    return true;
}

/*
 * struct stat {
 *      dev_t       st_dev;         // ID of device containing file
 *      ino_t       st_ino;         // Inode number
 *      mode_t      st_mode;        // File type and mode
 *      nlink_t     st_nlink;       // Number of hard links
 *      uid_t       st_uid;         // User ID of owner
 *      gid_t       st_gid;         // Group ID of owner
 *      dev_t       st_rdev;        // Device ID (if special file)
 *      off_t       st_size;        // Total size, in bytes
 *      blksize_t   st_blksize;     // Block size for filesystem I/O
 *      blkcnt_t    st_blocks;      // Number of 512B blocks allocated
 *
 *      struct timespec st_atim;    // Time of last access
 *      struct timespec st_mtim;    // Time of last modification
 *      struct timespec st_ctim;    // Time of last status change
 * }
 */
bool getEntryStat(const char* dirName, const char* filename, struct EntryStat* entryStat) {
    std::string fullname = std::string(dirName);
    if ( fullname.back() != '/' ) fullname += "/"+std::string(filename);
    else fullname += std::string(filename);

    struct stat statbuf;
    if ( lstat(fullname.c_str(), &statbuf) != 0 ) return false;
    getTypeChar     (statbuf.st_mode, entryStat->en_type);
    getPermStr      (statbuf.st_mode, entryStat->en_perm);
    getXattrChar    (filename,        entryStat->en_xattr);
    getTimeStampStr (statbuf.st_mtimespec, entryStat->en_mtime);
    getUnameByUid   (statbuf.st_uid, &(entryStat->en_usrname));
    getGnameByGid   (statbuf.st_gid, &(entryStat->en_grname));
    entryStat->en_nlink   = statbuf.st_nlink;
    entryStat->en_size    = statbuf.st_size;
    entryStat->en_name    = filename;
    entryStat->en_unit_h  = 'B';
    return true;
}

void getPermStr(const mode_t& mode, char* modeStr) {
    modeStr[0] = (mode & S_IRUSR) ? 'r' : '-';  // user has read access
    modeStr[1] = (mode & S_IWUSR) ? 'w' : '-';  // user has write access
    modeStr[2] = (mode & S_IXUSR) ? 'x' : '-';  // user has execute access
    modeStr[3] = (mode & S_IRGRP) ? 'r' : '-';  // group has read access
    modeStr[4] = (mode & S_IWGRP) ? 'w' : '-';  // group has write access
    modeStr[5] = (mode & S_IXGRP) ? 'x' : '-';  // group has execute access
    modeStr[6] = (mode & S_IROTH) ? 'r' : '-';  // other has read access
    modeStr[7] = (mode & S_IWOTH) ? 'w' : '-';  // other has write access
    modeStr[8] = (mode & S_IXOTH) ? 'x' : '-';  // other has execute access
    modeStr[9] = '\0';
}

void getTypeChar(const mode_t& mode, char& tchar) {
    switch (mode&S_IFMT) {                       // S_IFMT is the bit mask for the file type bit field
        case S_IFREG:  tchar = '-'; break;       // regular file
        case S_IFDIR:  tchar = 'd'; break;       // directory
        case S_IFCHR:  tchar = 'c'; break;       // character device
        case S_IFBLK:  tchar = 'b'; break;       // block device
        case S_IFLNK:  tchar = 'l'; break;       // symbolic link
        case S_IFIFO:  tchar = 'p'; break;       // named pipe or FIFO
        case S_IFSOCK: tchar = 's'; break;       // local-domain socket
        default:       tchar = ' '; break;       // unknown type (only some filesystems have full support to return file types)
    }
}

void getXattrChar(const char* filename, char& xchar, char* namebuf, size_t size) {
    if ( listxattr(filename, namebuf, size, XATTR_NOFOLLOW) > 0) xchar = '@';
    else if ( acl_get_link_np(filename, ACL_TYPE_EXTENDED) != NULL ) xchar = '+';
    else xchar = '.';
}

void getTimeStampStr(const struct timespec& mtime, char* mtimeStr) {
    memset(mtimeStr, 'x', 13);
    struct tm t;
    tzset();
    localtime_r(&(mtime.tv_sec), &t);
    char month[4];
    switch (t.tm_mon) {
        case 0:  strcpy(month, "Jan"); break;
        case 1:  strcpy(month, "Feb"); break;
        case 2:  strcpy(month, "Mar"); break;
        case 3:  strcpy(month, "Apr"); break;
        case 4:  strcpy(month, "May"); break;
        case 5:  strcpy(month, "Jun"); break;
        case 6:  strcpy(month, "Jul"); break;
        case 7:  strcpy(month, "Aug"); break;
        case 8:  strcpy(month, "Sep"); break;
        case 9:  strcpy(month, "Oct"); break;
        case 10: strcpy(month, "Nov"); break;
        case 11: strcpy(month, "Dec"); break;
    }
    snprintf(mtimeStr, 13, "%s %02d %02d:%02d", month, t.tm_mday, t.tm_hour, t.tm_min);
}

void getUnameByUid(const uid_t& uid, const char** target) {
    *target = getpwuid(uid)->pw_name;
}

void getGnameByGid(const gid_t& gid, const char** target) {
    *target = getgrgid(gid)->gr_name;
}

static char linkbuf[1024];
char* readLink(const char* dir, const char* file) {
    std::string path = std::string(dir) + std::string(file);
    readlink( path.c_str(), linkbuf, 1024 );
    return linkbuf;
}

}
