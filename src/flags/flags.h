#ifndef __FLAGS_H__
#define __FLAGS_H__

#include "cmdParser.h"

namespace flags
{

enum intflag
{
    // since 0x0 is taken by UNDEF, an additional 1 shift bit is required
    HUMAN = 0x100,       // (1 << (int('h')-int('a')+1))
    FORCE = 0x40,        // (1 << (int('f')-int('a')+1))
    LONG = 0x1000,       // (1 << (int('l')-int('a')+1))
    ALL = 0x2,           // (1 << (int('a')-int('a')+1))
    RECURSIVE = 0x40000, // (1 << (int('r')-int('a')+1))
    XATTR = 0x8000000,   // 1 << 26+1 for -@

    UNDEF = 0x0          // 0 (will be ignored by |=)
};

bool checkOpts(std::vector<std::string_view>&, const cmdExec*);

}

#endif /* __FLAGS_H__ */
