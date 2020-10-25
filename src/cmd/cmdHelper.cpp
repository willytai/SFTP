#include "cmdHelper.h"
// TODO: -h version
#include "cmdParser.h"

extern errorMgr errMgr;

namespace LIST
{

lsFlag getFlag(const char& c) {
    int code = 1 << (int(c) - int('a'));
    lsFlag f = lsFlag(code);
    if ( f == HUMAN_READABLE ||
         f == LIST_ALL       ||
         f == LIST_LONG) {
        return f;
    }
    else return UNDEF_FLAG;
}

bool checkFlag(const lsFlag& f, const int& stat) {
    return (stat & f) == f;
}

// TODO: -h version
bool listpp(const char* dirName, const LIST::Files& entries, const bool& all, const bool& human, const char* cmd) {
    bool returnStat = true;

    // word length for each property
    int w_nlink = 0, w_usrname = 0, w_grname = 0, w_size = 0;

    std::vector<struct UTIL::EntryStat> entryStats;
    std::vector<bool> valid;
    entryStats.resize(entries.size());
    valid.resize(entries.size(), true);
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& info = entries[i];
        auto& infoStat   = entryStats[i];
        if ( !UTIL::getEntryStat(dirName, info->d_name, &infoStat) ) {
            errMgr.setErrCmd(cmd);
            errMgr.setErrEntryAndDir(info->d_name, dirName);
            valid[i] = false;
            returnStat = false;
            continue;
        }
        else {
            w_nlink   = std::max(w_nlink,   UTIL::wLength(infoStat.en_nlink));
            w_usrname = std::max(w_usrname, UTIL::wLength(infoStat.en_usrname));
            w_grname  = std::max(w_grname,  UTIL::wLength(infoStat.en_grname));
            w_size    = std::max(w_size,    UTIL::wLength(infoStat.en_size));
        }
    }

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& infoStat = entryStats[i];
        const auto& isValid  = valid[i];
        if ( !isValid ) continue;
        if ( !all && infoStat.en_name[0] == '.' ) continue;
        cout << infoStat.en_type
             << infoStat.en_perm
             << infoStat.en_xattr       << ' '
             << right << setw(w_nlink)  << infoStat.en_nlink   << ' '
             << right<< setw(w_usrname) << infoStat.en_usrname << ' '
             << right<< setw(w_grname)  << infoStat.en_grname  << ' '
             << right<< setw(w_size)    << infoStat.en_size    << ' '
             << infoStat.en_mtime       << ' '
             << infoStat.en_name        << endl;
    }
    return returnStat;
}

bool listPrint(const dirCntMap& dirContent, bool all, bool human, bool printDirName, const char* cmd) {
    size_t count = 0;
    bool returnStat = true;
    for (const auto& pair : dirContent) {
        const auto& dirName = pair.first;
        const auto& entries = pair.second;
        if ( printDirName ) cout << dirName << ":" << endl;
        returnStat = returnStat && listpp(dirName.c_str(), entries, all, human, cmd);
        if ( ++count != dirContent.size() ) cout << endl << endl;
    }
    return returnStat;
}

bool listcp(const char* dirName, const LIST::Files& entries, const bool& all, const bool& human, const char* cmd) {
    bool returnStat = true;

    // word length for each property
    int w_usrname = 0;

    // terminal width in char
    int twidth = UTIL::getTermWidth();

    // max length of filename
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& info = entries[i];
        w_usrname = std::max(w_usrname, UTIL::wLength(info->d_name));
    }

    // max number of files in one line
    int nfiles = twidth / (w_usrname+1);

    // print
    int count = 0;
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& info = entries[i];
        if ( !all && info->d_name[0] == '.') continue;
        cout << left << setw(w_usrname) << info->d_name;
        if ( ++count == nfiles ) cout << endl;
        else cout << ' ';
    }
    if ( count != nfiles ) cout << endl;

    return returnStat;
}

bool columnPrint(const dirCntMap& dirContent, bool all, bool human, bool printDirName, const char* cmd) {
    size_t count = 0;
    bool returnStat = true;
    for (const auto& pair : dirContent) {
        const auto& dirName = pair.first;
        const auto& entries = pair.second;
        if ( printDirName ) cout << dirName << ":" << endl;
        returnStat = returnStat && listcp(dirName.c_str(), entries, all, human, cmd);
        if ( ++count != dirContent.size() ) cout << endl << endl;
    }
    return returnStat;
}

}
