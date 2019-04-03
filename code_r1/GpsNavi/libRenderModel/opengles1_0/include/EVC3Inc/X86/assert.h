/* assert.h espically created for EV3 project 
 *
 * For evc4 already have these stuff as a pre-require,
 * for evc3 make such a fake assert.h to make affect the lease for the trunk.
 *
 */


#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <windows.h>

#ifdef NDEBUG

#define assert(x) 

#else

#define assert(x)   \
    do {            \
    if(!(x))        \
        {__asm int 3} \
    }while(0)

#endif



#endif //__ASSERT_H__