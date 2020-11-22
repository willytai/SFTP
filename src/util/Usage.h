#ifndef __USAGE_H__
#define __USAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/resource.h>

static struct rusage usg;

// memory/time usage
struct Usage
{
    // in MB
    static double getMem() {
        getrusage( RUSAGE_SELF, &usg );
#ifdef __APPLE__
        return usg.ru_maxrss / (double)(1<<20); // bytes
#else
        return usg.ru_maxrss / (double)(1<<10); //Kbytes
#endif
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __USAGE_H__ */

