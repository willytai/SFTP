#include "Usage.h"
#include <iostream>

void* operator new (size_t size) {
    std::cout << "(single) allocated " << size << " bytes\n";
    Memory::add(size);
    return malloc(size);
}

void operator delete(void* ptr, size_t size) {
    std::cout << "(single) freed " << size << " bytes\n";
    Memory::sub(size);
    free(ptr);
}

void* operator new [] (size_t size) {
    std::cout << "(array) allocated " << size << " bytes\n";
    Memory::add(size);
    return malloc(size);
}

void operator delete [] (void* ptr, size_t size) {
    std::cout << "(array) freed " << size << " bytes\n";
    Memory::sub(size);
    free(ptr);
}
