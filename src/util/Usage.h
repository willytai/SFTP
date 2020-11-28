#ifndef __USAGE_H__
#define __USAGE_H__

/************************************
 * Memory Usage by overriding 'new' *
 ***********************************/
#include <algorithm>
#include <cstdlib>
class Memory
{
public:
    Memory(const Memory&) = delete;
    static Memory& get() {
        static Memory memusg;
        return memusg;
    }

    static void add(size_t size) {
        get().heap_add(size);
    }

    static void sub(size_t size) {
        get().heap_sub(size);
    }

    static size_t CurrentUsage() { return get().calUsage(); }

private:
    Memory() { __heap_total = __heap_free = __heap_max = 0x0; }
    ~Memory() {}

    void heap_add(size_t size) {
        __heap_total += size;
        __heap_max = std::max(__heap_max, __heap_total-__heap_free);
    }

    void heap_sub(size_t size) {
        __heap_free += size;
    }

    size_t calUsage() const {
        return __heap_total - __heap_free;
    }

    size_t __heap_total;
    size_t __heap_free;
    size_t __heap_max;
};

void* operator new (size_t);
void* operator new [] (size_t);
void  operator delete(void*, size_t);
void  operator delete [] (void*, size_t);


/*****************************************
 * Memory/Time Usage from system library *
 ****************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/resource.h>

static struct rusage usg;

// memory/time usage
struct SysUsage
{
    // in MB
    static double getMem() {
        getrusage( RUSAGE_SELF, &usg );
#ifdef __APPLE__
        return usg.ru_maxrss / (double)(1<<20); // bytes
#else
        return usg.ru_maxrss / (double)(1<<10); // Kbytes
#endif
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __USAGE_H__ */

