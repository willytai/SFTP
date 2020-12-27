#ifndef __MEM_UTIL_H__
#define __MEM_UTIL_H__

#include <stdlib.h>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace memUtil
{

    class MemUsage
    {
    public:
        MemUsage(const MemUsage& m) = delete;

        static MemUsage& get() {
            static MemUsage m;
            return m;
        }

#ifdef MEMDEBUG
        static void usage() {
            cout << "Current Usage:    " << get().calUsage() << " bytes" << endl;
            cout << "Peak    Usage:    " << get().__heap_peak << " bytes" << endl;
            cout << "Total Allocation: " << get().__heap_allocated << " bytes" << endl;
        }

        static void leakCheck() {
            if (get().calUsage()) {
                cout << "Memory Leak Detected!" << endl;
            }
            else {
                cout << "No Memory Leak!" << endl;
            }
        }
#else
        static void usage() {
            cerr << "Memory debug mode disabled! Define MEMDEBUG or add the flag to the compiler." << endl;
        }

        static void leakCheck() {
            cerr << "Memory debug mode disabled! Define MEMDEBUG or add the flag to the compiler." << endl;
        }
#endif // MEMDEBUG

        static void __add(size_t size) {
            get().add(size);
        }

        static void __sub(size_t size) {
            get().sub(size);
        }

    private:
        MemUsage() :
            __heap_allocated(0),
            __heap_freed(0),
            __heap_peak(0)
        {}
        ~MemUsage() {}

        void add(size_t size) {
            __heap_allocated += size;
            __heap_peak = std::max(__heap_peak, __heap_allocated - __heap_freed);
        }

        void sub(size_t size) {
            __heap_freed += size;
        }

        size_t calUsage() const {
            return __heap_allocated - __heap_freed;
        }

        size_t __heap_allocated;
        size_t __heap_freed;
        size_t __heap_peak;
    };

}

#ifdef MEMDEBUG
void* operator new (size_t size);
void operator delete (void* ptr);
#endif // MEMDEBUG

#endif // __MEM_UTIL_H__
