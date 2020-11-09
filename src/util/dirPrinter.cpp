#include "dirPrinter.h"

namespace LISTING
{

// TODO: print colorful format for different entry types
// TODO: -@ not yet implemented
bool Printer::print(const dirCntMap& dirContent) const {
    size_t count      = 0;
    bool   returnStat = true;
    for (const auto& pair : dirContent) {
        const auto& dirName = pair.first;
        const auto& entries = pair.second;
        if ( _pDirName ) cout << dirName << ":" << endl;

        // decide print method
        if ( this->checkFlag(LIST_LONG) ) {
            returnStat = returnStat && this->longPrint(dirName.c_str(), entries);
        }
        else {
            returnStat = returnStat && this->columnPrint(entries);
        }

        if ( ++count != dirContent.size() ) cout << endl << endl;
    }
    return returnStat;
}


/**************
 * long print *
 *************/
bool Printer::longPrint(const char* dirName, const Files& entries) const {
    bool returnStat = true;
    bool PRINT_ALL  = this->checkFlag(LIST_ALL);
    bool PRINT_HUM  = this->checkFlag(HUMAN_READABLE);

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
            _errEntry  = info->d_name;
            _errDir    = dirName;
            valid[i]   = false;
            returnStat = false;
            continue;
        }
        else {
            w_nlink   = std::max(w_nlink,   UTIL::wLength(infoStat.en_nlink));
            w_usrname = std::max(w_usrname, UTIL::wLength(infoStat.en_usrname));
            w_grname  = std::max(w_grname,  UTIL::wLength(infoStat.en_grname));

            /*******************************************
             * human readable format always has length *
             * of 3 puls 1 more for wider columns      *
             ******************************************/
            if ( PRINT_HUM ) {
                infoStat.en_size_h = (double)infoStat.en_size;
                UTIL::toHuman(&(infoStat.en_size_h), &(infoStat.en_unit_h));
                w_size = 4;
            }
            else {
                w_size = std::max(w_size,    UTIL::wLength(infoStat.en_size));
            }
        }
    }

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& infoStat = entryStats[i];
        const auto& isValid  = valid[i];
        if ( !isValid ) continue;
        if ( !PRINT_ALL && infoStat.en_name[0] == '.' ) continue;
        cout << infoStat.en_type
             << infoStat.en_perm
             << infoStat.en_xattr        << ' '
             << right << setw(w_nlink)   << infoStat.en_nlink   << ' '
             << right << setw(w_usrname) << infoStat.en_usrname << ' '
             << right << setw(w_grname)  << infoStat.en_grname  << ' ';
        if ( PRINT_HUM ) {
            cout << right << setw(w_size) << infoStat.en_size_h << infoStat.en_unit_h << ' ';
        }
        else {
            cout << right << setw(w_size) << infoStat.en_size << ' ';
        }
        cout << infoStat.en_mtime        << ' ';
        if ( _colorful && infoStat.en_type == 'd' ) {
            cout << BOLD_CYAN << infoStat.en_name << COLOR_RESET << endl;
        }
        else if ( _colorful && infoStat.en_type == 'l' ) {
            cout << BOLD_MAGENTA << infoStat.en_name << COLOR_RESET;
            cout << " -> " << UTIL::readLink(dirName, infoStat.en_name) << endl;
        }
        else if ( _colorful && infoStat.en_perm[2] == 'x' ) {
            cout << NORMAL_RED << infoStat.en_name << COLOR_RESET << endl;
        }
        else {
            cout << infoStat.en_name << endl;
        }
    }
    return returnStat;
}


/****************
 * column print *
 ***************/
bool Printer::columnPrint(const Files& entries) const {
    bool returnStat = true;
    bool PRINT_ALL  = this->checkFlag(LIST_ALL);

    // word length for each property
    int w_usrname = 0;

    // terminal width in char
    int twidth = UTIL::getTermWidth();

    // max length of filename
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& info = entries[i];
        if ( !PRINT_ALL && info->d_name[0] == '.') continue;
        w_usrname = std::max(w_usrname, UTIL::wLength(info->d_name));
    }

    // max number of files in one line
    // add an extra space for 'space'
    w_usrname += 1;
    int nfiles = twidth / w_usrname;

    // print
    int count = 0;
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& info = entries[i];
        if ( !PRINT_ALL && info->d_name[0] == '.') continue;
        cout << left << setw(w_usrname) << info->d_name;
        if ( ++count == nfiles ) {
            count = 0;
            cout << endl;
        }
        else cout << ' ';
    }
    if ( count ) cout << endl;
    return returnStat;
}


/********************
 * helper functions *
 *******************/
bool Printer::setFlag(const char& c) {
    lsFlag f;
    if ( (f = this->getFlag(c)) == UNDEF_FLAG ) {
        _illegal = true;
        return false;
    }
    else {
        _flags |= f;
        return true;
    }
}

bool Printer::checkFlag(const lsFlag& f) const {
    return (_flags & f) == f;
}

bool Printer::illegal() const {
    return _illegal;
}

void Printer::setPrintDirName(bool b) {
    _pDirName = b;
}

lsFlag Printer::getFlag(const char& c) const {
    int code = 1 << (int(c) - int('a'));
    lsFlag f = lsFlag(code);
    if ( f == HUMAN_READABLE ||
         f == LIST_ALL       ||
         f == LIST_XATTR     ||
         f == LIST_LONG) {
        return f;
    }
    else return UNDEF_FLAG;
}

}
