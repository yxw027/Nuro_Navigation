/* 7zAlloc.c -- Allocation functions
2008-10-04 : Igor Pavlov : Public domain */

#include "NuroDefine.h"
#include "NuroArchiveInc.h"
extern "C"
{    
#include "./7z/7zAlloc.h"
};

extern "C" void *SzAlloc(void *p, size_t size)
{
    p = p;
    if (size == 0)
        return 0;
    return nuMalloc(size);
}

extern "C" void SzFree(void *p, void *address)
{
    p = p;
    nuFree(address);
}

extern "C"  void *SzAllocTemp(void *p, size_t size)
{
    p = p;
    if (size == 0)
        return 0;
    return nuMalloc(size);
}

extern "C" void SzFreeTemp(void *p, void *address)
{
    p = p;
    nuFree(address);
}
