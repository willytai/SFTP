#include "Usage.h"

// unable to make the compiler call the overriden delete [] (size)
// not profiling these now
/*
#ifdef MEMDEBUG
void* operator new (size_t size) {
    std::cout << "(single) allocated " << size << " bytes\n";
    Memory::add(size);
    return malloc(size);
}

void operator delete(void* memory, std::size_t size) noexcept {
    std::cout << "(single) freed " << size << " bytes\n";
    Memory::sub(size);
    free(memory);
}
void* operator new [] (size_t size) {
    std::cout << "(array) allocated " << size << " bytes\n";
    Memory::add(size);
    return malloc(size);
}

void operator delete [] (void* memory, std::size_t size) noexcept {
    std::cout << "(array) freed " << size << " bytes\n";
    Memory::sub(size);
    free(memory);
}
#endif
*/
