#ifndef __DIR_PRINTER_H__
#define __DIR_PRINTER_H__

#include <stdio.h>
#include <libssh/sftp.h>
#include <dirent.h>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <climits>
#include "util.h"
#include "dirIO.h"
#include "def.h"


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


namespace sftp     /* helper functions for 'ls' */
{

typedef std::vector<sftp_attributes> Files;
typedef std::unordered_map<std::string, Files> dirCntMap;

class Printer
{
public:
    Printer(bool c) : _flags(0), _illegal(false), _pDirName(false) { _colorful = c; }
    ~Printer() {}

    bool setFlag         (const char&);
    bool print           (const dirCntMap&) const;
    bool illegal         ()                 const;
    void setColorPrinter () { _colorful = true; };
    void setPrintDirName (bool);

    const char* getErrEntry() const { return _errEntry; }
    const char* getErrDir()   const { return _errDir; }

private:
    bool   longPrint   (const char*, const Files&) const;
    bool   columnPrint (const Files&)              const;
    bool   checkFlag   (const lsFlag&)             const;
    lsFlag getFlag     (const char&)               const;

private:
    int     _flags;
    bool    _illegal;
    bool    _pDirName;
    bool    _colorful;

    // the LATEST entry/dir that wasn't able to be printed (for error handling)
    mutable const char*   _errEntry;
    mutable const char*   _errDir;
};

}

namespace LISTING     /* helper functions for 'lls' */
{

typedef std::vector<dirent*> Files;
typedef std::unordered_map<std::string, Files> dirCntMap;

class Printer
{
public:
    Printer(bool c) : _flags(0), _illegal(false), _pDirName(false) { _colorful = c; }
    ~Printer() {}

    bool setFlag         (const char&);
    bool print           (const dirCntMap&) const;
    bool illegal         ()                 const;
    void setColorPrinter () { _colorful = true; };
    void setPrintDirName (bool);

    const char* getErrEntry() const { return _errEntry; }
    const char* getErrDir()   const { return _errDir; }

private:
    bool   longPrint   (const char*, const Files&) const;
    bool   columnPrint (const Files&)              const;
    bool   checkFlag   (const lsFlag&)             const;
    lsFlag getFlag     (const char&)               const;

private:
    int     _flags;
    bool    _illegal;
    bool    _pDirName;
    bool    _colorful;

    // the LATEST entry/dir that wasn't able to be printed (for error handling)
    mutable const char*   _errEntry;
    mutable const char*   _errDir;
};

}

#endif /* __DIR_PRINTER_H__ */
