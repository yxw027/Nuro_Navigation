#ifndef __NURONEW_H__
#define __NURONEW_H__
#if 0
#include "../NuroClib/NuroClib.h"
typedef unsigned size_t;

#ifdef __cplusplus
void *operator new(size_t size);
void operator delete(void *ptr);
void *operator new[](size_t size);
void operator delete[](void *ptr);
#endif
#endif

#endif
