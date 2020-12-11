#include "flags.h"
#include "util.h"

extern errorMgr errMgr;

namespace flags
{

inline int getIntFlag(const char& c) {
    intflag f = intflag( (1 << (int(c)-int('a')+1)) );
    if ( f == intflag::HUMAN ||
         f == intflag::FORCE ||
         f == intflag::LONG  ||
         f == intflag::ALL   ||
         f == intflag::XATTR ||
         f == intflag::RECURSIVE) {
        return (int)f;
    }
    return (int)intflag::UNDEF;
}

bool checkOpts(std::vector<std::string_view>& tokens, const cmdExec* cmdPlacehoder) {
    bool returnStat = true;
    const auto& availOptions = cmdPlacehoder->getAvailOptions();
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        auto& t = *it;
        if ( t.size() < 2 ) continue;
        if ( t.front() != '-' ) continue;

        // check through all the options
        bool valid = false;
        for (const auto& opt : availOptions) {
            if ( UTIL::strcmp( t, opt.c_str(), opt.size() ) == 0 ) {
                cmdPlacehoder->setFlag( getIntFlag( t[t.find_first_not_of('-')] ) );
                valid = true;
                break;
            }
        }
        if ( !valid ) {
            t.remove_prefix( t.find_first_not_of('-') );
            errMgr.setErrOpt( t );
            returnStat = false;
        }

        // the order cannot be changed, so simply deleting element despite its inefficiency
        tokens.erase( it );
        --it;
    }
    return returnStat;
}

}
