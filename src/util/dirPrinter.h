#ifndef __DIR_PRINTER_H__
#define __DIR_PRINTER_H__

#include <dirent.h>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <climits>
#include "util.h"
#include "def.h"

namespace LISTING     /* helper functions for 'lls' and 'ls' */
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

class Printer
{
public:
    Printer() : _flags(0), _illegal(false), _pDirName(false) {}
    ~Printer() {}

    bool setFlag(const char& c);
    bool illegal() const;
    bool print(const dirCntMap&) const;
    void setPrintDirName(bool);

    const char* getErrEntry() const { return _errEntry; }
    const char* getErrDir()   const { return _errDir; }

private:
    lsFlag getFlag (const char& c) const;
    bool   checkFlag  (const lsFlag& f) const;
    bool   longPrintMacro  (const dirCntMap& dirContent) const;
    bool   longPrintDetail (const char*, const Files&) const;
    bool   columnPrintMacro(const dirCntMap& dirContent) const;
    bool   columnPrintDetail(const char*, const Files&) const;

private:
    int     _flags;
    bool    _illegal;
    bool    _pDirName;

    // the LATEST entry/dir that wasn't able to be printed (for error handling)
    mutable const char*   _errEntry;
    mutable const char*   _errDir;
};

}

#endif /* __DIR_PRINTER_H__ */
