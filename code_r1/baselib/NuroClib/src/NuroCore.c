#include "../NuroCore.h"
#include "../NuroNative.h"
#ifdef NURO_OS_WINDOWS
#define _CRT_INSECURE_DEPRECATE(...)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct core_struct nu_clib__ = 
{
    /// mem functions
    (nuVOID  *(  * /* core_malloc */   )(nuSIZE /* n */))                                                             malloc/*0*/,
	(nuVOID   (  * /* core_free */     )(nuVOID * /* p */))                                                           free/*0*/,
    (nuVOID  *(  * /* core_memset */   )(nuVOID * /* s */, nuINT /* c */, nuSIZE /* n */))                            /*memset*/0,
    (nuVOID  *(  * /* core_memcpy */   )(nuVOID * /* dest */, const nuVOID * /* src */, nuSIZE /* n */))              /*memcpy*/0,
    (nuVOID  *(  * /* core_memmove */  )(nuVOID * /* dest */, const nuVOID * /* src */, nuSIZE /* n */))              /*memmove*/0,
    (nuINT    (  * /* core_memcmp */   )(const nuVOID * /* s1 */, const nuVOID * /* s2 */, nuSIZE /* n */))           /*memcmp*/0,

    /// format string print functions
    (nuINT    (  * /* core_sscanf */   )(const nuCHAR * /* buffer */, const nuCHAR * /*format */, ...))               /*sscanf*/0,
    (nuINT    (  * /* core_sprintf */  )(nuCHAR * /* buffer */, const nuCHAR * /* format */, ...))                    /*sprintf*/0,

    /// format string print functions
    (nuINT    (  * /* core_swscanf */  )(const nuWCHAR * /* buffer */, const nuWCHAR * /* format */, ...))            0,
    (nuINT    (  * /* core_swprintf */ )(nuWCHAR * /* buffer */, const nuWCHAR * /* format */, ...))                  0,

    /// string functions
    (nuSIZE   (  * /* core_strlen */   )(const nuCHAR * /* s */))                                                     /*strlen*/0,
    (nuCHAR  *(  * /* core_strcpy */   )(nuCHAR * /* dest */, const nuCHAR * /* src */))                              /*strcpy*/0,
    (nuCHAR  *(  * /* core_strncpy */  )(nuCHAR * /* dest */, const nuCHAR * /* src */, nuSIZE /* n */))              /*strncpy*/0,
    (nuCHAR  *(  * /* core_strcat */   )(nuCHAR * /* dest */, const nuCHAR * /* src */))                              /*strcat*/0,
    (nuCHAR  *(  * /* core_strncat */  )(nuCHAR * /* dest */, const nuCHAR * /* src */, nuSIZE /* n */))              /*strncat*/0,
    (nuINT    (  * /* core_strcmp */   )(const nuCHAR * /* s1 */, const nuCHAR * /* s2 */))                           /*strcmp*/0,
    (nuINT    (  * /* core_strncmp */  )(const nuCHAR * /* s1 */, const nuCHAR * /* s2 */, nuSIZE /* n */))           /*strncmp*/0,
	(nuINT    (  * /* core_stricmp */  )(const nuCHAR * /* s1 */, const nuCHAR * /* s2 */))                           0,
	(nuINT    (  * /* core_strnicmp */ )(const nuCHAR * /* s1 */, const nuCHAR * /* s2 */, nuSIZE /* n */))           0,
    (nuCHAR  *(  * /* core_strstr */   )(const char * /* haystack */, const char * /* needle */))                     /*strstr*/0,
    (nuCHAR  *(  * /* core_strchr */   )(const nuCHAR * /* s */, nuINT /* c */))                                      /*strchr*/0,
    (nuCHAR  *(  * /* core_strrchr */  )(const nuCHAR * /* s */, nuINT /* c */))                                      /*strrchr*/0,
    (nuLONG   (  * /* core_strtol */   )(const nuCHAR * /* nptr */, nuCHAR ** /* endptr */, nuINT /* base */))        /*strtol*/0,
	(nuDOUBLE (  * /* core_strtod */   )(const nuCHAR * /* nptr */, nuCHAR ** /* endptr */))                          /*strtod*/0,
    (nuCHAR  *(  * /* core_strtok */   )(nuCHAR * /* str */, const nuCHAR * /* delim */))                             /*strtok*/0,
    (nuCHAR  *(  * /* core_strtok_r */ )(nuCHAR * /* str */, const nuCHAR * /* delim */, nuCHAR** /* pTag */))        /*strtok_r*/0,
    (nuCHAR  *(  * /* core_itoa */     )(nuLONG /* i */, nuCHAR * /* pDes */, nuINT /* base */))                      /*itoa*/0,

    /// wide string functions
    (nuINT    (  * /* core_wcslen */   )(const nuWCHAR * /* ws */))                                                    0,
    (nuWCHAR *(  * /* core_wcscpy */   )(nuWCHAR * /* wsDest */, const nuWCHAR * /* wcsSrc */))                        0,
    (nuWCHAR *(  * /* core_wcsncpy */  )(nuWCHAR * /* wsDest */, const nuWCHAR * /* wcsSrc */, nuINT /* nWcsLen */))   0,
    (nuWCHAR *(  * /* core_wcscat */   )(nuWCHAR * /* wsDest */, const nuWCHAR * /* wcsSrc */))                        0,
    (nuWCHAR *(  * /* core_wcsncat */  )(nuWCHAR * /* wsDest */, const nuWCHAR * /* wcsSrc */, nuINT /* nWcsLen */))   0,
    (nuINT    (  * /* core_wcscmp */   )(const nuWCHAR * /* ws1 */, const nuWCHAR * /* ws2 */))                        0,
    (nuINT    (  * /* core_wcsncmp */  )(const nuWCHAR * /* ws1 */, const nuWCHAR * /* ws2 */, nuSIZE /* n */))        0,
	(nuINT    (  * /* core_wcsicmp */  )(const nuWCHAR * /* ws1 */, const nuWCHAR * /* ws2 */))                        0,
	(nuINT    (  * /* core_wcsnicmp */ )(const nuWCHAR * /* ws1 */, const nuWCHAR * /* ws2 */, nuSIZE /* n */))        0,
    (nuWCHAR *(  * /* core_wcsstr */   )(const nuWCHAR * /* haystack */, const nuWCHAR * /* needle */))                0,
    (nuWCHAR *(  * /* core_wcschr */   )(const nuWCHAR * /* ws */, nuINT /* c */))                                     0,
    (nuWCHAR *(  * /* core_wcsrchr */  )(const nuWCHAR * /* ws */, nuINT /* c */))                                     0,
    (nuLONG   (  * /* core_wcstol */   )(const nuWCHAR * /* nptr */, nuWCHAR ** /* endptr */, nuINT /* base */))       0,
	(nuDOUBLE (  * /* core_wcstod */   )(const nuWCHAR * /* nptr */, nuWCHAR ** /* endptr */))                         0,
    (nuWCHAR *(  * /* core_wcstok */   )(nuWCHAR * /* wstr */, const nuWCHAR * /* delim */))                           0,
    (nuWCHAR *(  * /* core_wcstok_r */ )(nuWCHAR * /* wstrToken */, const nuWCHAR * /* wstrDelimit */, nuWCHAR ** /* pTag */))  0,
    (nuWCHAR *(  * /* core_itow */     )(nuLONG /* i */, nuWCHAR * /* nptr */, nuINT /* base */))                      0,


    /// string transform function
    (nuCHAR  *(  * /* core_WcstoAsc */       )(nuCHAR * /* pDes */, nuSIZE /* nAscMax */, const nuWCHAR * /* pSrc */, nuSIZE /* nWcsMax */))    0,
    (nuWCHAR *(  * /* core_AsctoWcs */       )(nuWCHAR * /* pDes */, nuSIZE /* nWcsMax */, const nuCHAR * /* pSrc */, nuSIZE /* nAscMax */))    0,

    (nuBOOL   (  * /* core_WcsSameAs */      )(nuWCHAR * /* A */, nuWCHAR * /* B */))                                                     0,
    (nuBOOL   (  * /* core_WcsFuzzyJudge_O */)(nuWCHAR * /* pDes */, nuWCHAR * /* pKey */, nuUINT /* desLen */, nuUINT /* keyLen */))     0,
    (nuBOOL   (  * /* core_WcsFuzzyJudge */  )(nuWCHAR * /* pDes */, nuWCHAR * /* pKey */, nuUINT /* desLen */, nuUINT /* keyLen */, nuWORD /* endWord */)) 0,
    (nuINT    (  * /* core_WcsKeyMatch */    )(nuWCHAR * /* pDes */, nuWCHAR * /* pKey */, nuUINT /* desLen */, nuUINT /* keyLen */))     0,
    (nuBOOL   (  * /* core_WcsDelSpSign */   )(nuWCHAR * /* pDes */, nuWCHAR * /* pSrc */, nuUINT /* desLen */, nuUINT /* srcLen */))     0,

    (nuDOUBLE (  * /* core_cos */  )(nuLONG /* nAngle */))                                                             0,
    (nuDOUBLE (  * /* core_sin */  )(nuLONG /* nAngle */))                                                             0,
    (nuDOUBLE (  * /* core_tan */  )(nuLONG /* nAngle */))                                                             0,
    (nuDOUBLE (  * /* core_atan2 */)(nuLONG /* dy */, nuLONG /* dx */))                                                0,
    (nuDOUBLE (  * /* core_sqrt */ )(nuDOUBLE /* fValue */))                                                           0,
    (nuDOUBLE (  * /* core_pow */  )(nuDOUBLE /* base */, nuDOUBLE /* exp */))                                         0,

    (nuLONG   (  * /* core_GetlBase */ )(nuVOID))                                                                      0,
    (nuLONG   (  * /* core_latan2 */   )(nuLONG /* dy */, nuLONG /* dx */))                                            0,
    (nuLONG   (  * /* core_lsin */     )(nuLONG /* nDegree */))                                                        0,
    (nuLONG   (  * /* core_lcos */     )(nuLONG /* nDegree */))                                                        0,
    (nuLONG   (  * /* core_ltan */     )(nuLONG /* nDegree */))                                                        0,
	(nuLONG   (  * /* core_lsqrt */    )(nuLONG /* dx */, nuLONG /* dy */))                                            0,
	(nuUSHORT (  * /* core_lQSqrt */   )(nuULONG /* a */))                                                             0,

    (nuDOUBLE (  * /* core_CosJ */     )(nuDOUBLE /* nAngle */))                                                       0,
    (nuDOUBLE (  * /* core_SinJ */     )(nuDOUBLE /* nAngle */))                                                       0
};

struct core_struct *nu_clib = &nu_clib__;

#ifdef NURO_OS_LINUX
static int core_before(void) __attribute__((constructor));
static int core_after(void)  __attribute__((destructor));
#endif

#define CHECK_FUNC(func) nu_clib__.func = (NULL == nu_clib__.func ? func##_impl : nu_clib__.func)

static int core_before(void)  
{   
    CHECK_FUNC(core_malloc);
	CHECK_FUNC(core_free);
    CHECK_FUNC(core_memset);
    CHECK_FUNC(core_memcpy);
    CHECK_FUNC(core_memmove);
    CHECK_FUNC(core_memcmp);
    
    /// format string print functions
    CHECK_FUNC(core_sscanf);
    CHECK_FUNC(core_sprintf);
    
    /// format string print functions
    CHECK_FUNC(core_swscanf);
    CHECK_FUNC(core_swprintf);
    
    /// string functions
    CHECK_FUNC(core_strlen);
    CHECK_FUNC(core_strcpy);
    CHECK_FUNC(core_strncpy);
    CHECK_FUNC(core_strcat);
    CHECK_FUNC(core_strncat);
    CHECK_FUNC(core_strcmp);
    CHECK_FUNC(core_strncmp);
	CHECK_FUNC(core_stricmp);
	CHECK_FUNC(core_strnicmp);
    CHECK_FUNC(core_strstr);
    CHECK_FUNC(core_strchr);
    CHECK_FUNC(core_strrchr);
    CHECK_FUNC(core_strtod);
    CHECK_FUNC(core_strtol);
    CHECK_FUNC(core_strtok);
    CHECK_FUNC(core_strtok_r);
    CHECK_FUNC(core_itoa);
    
    /// wide string functions
    CHECK_FUNC(core_wcslen);
    CHECK_FUNC(core_wcscpy);
    CHECK_FUNC(core_wcsncpy);
    CHECK_FUNC(core_wcscat);
    CHECK_FUNC(core_wcsncat);
    CHECK_FUNC(core_wcscmp);
    CHECK_FUNC(core_wcsncmp);
	CHECK_FUNC(core_wcsicmp);
	CHECK_FUNC(core_wcsnicmp);
    CHECK_FUNC(core_wcsstr);
    CHECK_FUNC(core_wcschr);
    CHECK_FUNC(core_wcsrchr);
    CHECK_FUNC(core_wcstod);
    CHECK_FUNC(core_wcstol);
    CHECK_FUNC(core_wcstok);
    CHECK_FUNC(core_wcstok_r);
    CHECK_FUNC(core_itow);
   
    /// string transform function
    CHECK_FUNC(core_WcstoAsc);
    CHECK_FUNC(core_AsctoWcs);
 
    CHECK_FUNC(core_WcsSameAs);
    CHECK_FUNC(core_WcsFuzzyJudge_O);
    CHECK_FUNC(core_WcsFuzzyJudge);
    CHECK_FUNC(core_WcsKeyMatch);
    CHECK_FUNC(core_WcsDelSpSign);

    CHECK_FUNC(core_cos);
    CHECK_FUNC(core_sin);
    CHECK_FUNC(core_tan);
    CHECK_FUNC(core_atan2);
    CHECK_FUNC(core_sqrt);
    CHECK_FUNC(core_pow);

    CHECK_FUNC(core_GetlBase);
    CHECK_FUNC(core_latan2);
    CHECK_FUNC(core_lsin);
    CHECK_FUNC(core_lcos);
    CHECK_FUNC(core_ltan);
    CHECK_FUNC(core_lsqrt);
	CHECK_FUNC(core_lsqrt);
	CHECK_FUNC(core_lQSqrt);

    /// 原来nuClib中的函数
    CHECK_FUNC(core_CosJ);
    CHECK_FUNC(core_SinJ);
    
    return 0;  
}  

static int core_after(void)  
{  
    return 0;  
}  

#ifdef NURO_OS_WINDOWS
typedef int (*_pre_action_type)(void);
#pragma data_seg(".CRT$XIU")
static _pre_action_type before_function_array[] = { core_before };
#pragma data_seg(".CRT$XPU")
static _pre_action_type after_function_array[] = { core_after };
#pragma data_seg()
#endif
