#ifndef __USAGE_H__
#define __USAGE_H__

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
        return (double)usg.ru_maxrss / (double)(1<<20); // bytes
#else
        return (double)usg.ru_maxrss / (double)(1<<10); // Kbytes
#endif
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __USAGE_H__ */


/************************************
 * Memory Usage by overriding 'new' *
 ***********************************/
// -- These overloads doesn't work for some unknown reason
//    The new [] (size) operator calls the new (size) operator
//    so the sum of the allocated memory will contain both, whereas
//    the delete (size) operator works but delete [] (size) just won't bother.
//    So I can't obtain a correct memory profiling by overriding the four news and deletes
/*
#ifdef MEMDEBUG
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
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

    static void Usage() {
        std::cout << "Current Usage: " << get().calUsage() << " bytes" << std::endl;
        std::cout << "Peak    Usage: " << get().__heap_max << " bytes" << std::endl;
    }


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

void* operator new (size_t size);
void  operator delete(void* memory, std::size_t size) noexcept;
void* operator new [] (size_t size);
void  operator delete [] (void* memory, std::size_t size) noexcept;
#endif
*/

