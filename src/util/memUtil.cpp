#include "memUtil.h"

/*******************************************************************************/
/* All the functions below are not meant to be used outside of this file scope */
/*                       DO NOT CALL THEM EXPLICITLY!!!                        */
/*******************************************************************************/

/* allocates memory with an additional sizeof(size_t) bytes to store the size of the allocated memory */
void* __allocate(size_t size) {
    memUtil::MemUsage::__add(size);
    size_t* ptr = (size_t*)malloc(size + sizeof(size_t));
    ptr[0] = size;
    return (void*)&ptr[1];
}

/* retrieve the allocated size of going back one position */
void __deallocate(void* ptr) {
    size_t* ptr_t = (size_t*)ptr;
    size_t size = ptr_t[-1];
    memUtil::MemUsage::__sub(size);
    free((void*)&ptr_t[-1]);
}

#ifdef MEMDEBUG
void* operator new (size_t size) {
    return __allocate(size);
}

void operator delete (void* ptr) {
    __deallocate(ptr);
}
#endif // MEMDEBUG
