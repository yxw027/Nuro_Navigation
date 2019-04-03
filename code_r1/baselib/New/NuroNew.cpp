#if 0
#include "./NuroNew.h"

void *operator new(size_t size)
{
    return nuMalloc(size);
}

void operator delete(void *ptr)
{
    nuFree(ptr);
}

void *operator new[](size_t size)
{
    return nuMalloc(size);
}

void operator delete[](void *ptr)
{
    nuFree(ptr);
}
#endif
