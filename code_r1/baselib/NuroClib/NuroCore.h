#ifndef __NURO_CORE_H_20120712
#define __NURO_CORE_H_20120712
#include "../Inc/NuroTypes.h"

struct core_struct {
    /// mem functions
    nuVOID  *(  *core_malloc   )(nuSIZE n);
	nuVOID   (  *core_free     )(nuVOID *p);      
    nuVOID  *(  *core_memset   )(nuVOID *s, nuINT c, nuSIZE n);
    nuVOID  *(  *core_memcpy   )(nuVOID *dest, const nuVOID *src, nuSIZE n);
    nuVOID  *(  *core_memmove  )(nuVOID *dest, const nuVOID *src, nuSIZE n);
    nuINT    (  *core_memcmp   )(const nuVOID *s1, const nuVOID *s2, nuSIZE n);

    /// format string print functions
    nuINT    (  *core_sscanf   )(const nuCHAR *buffer, const nuCHAR *format , ...);
    nuINT    (  *core_sprintf  )(nuCHAR *buffer, const nuCHAR *format, ...);

    /// format string print functions
    nuINT    (  *core_swscanf  )(const nuWCHAR *buffer, const nuWCHAR *format , ...);
    nuINT    (  *core_swprintf )(nuWCHAR *buffer, const nuWCHAR *format, ...);

    /// string functions
    nuSIZE   (  *core_strlen   )(const nuCHAR *s);
    nuCHAR  *(  *core_strcpy   )(nuCHAR *dest, const nuCHAR *src);
    nuCHAR  *(  *core_strncpy  )(nuCHAR *dest, const nuCHAR *src, nuSIZE n);
    nuCHAR  *(  *core_strcat   )(nuCHAR *dest, const nuCHAR *src);
    nuCHAR  *(  *core_strncat  )(nuCHAR *dest, const nuCHAR *src, nuSIZE n);
    nuINT    (  *core_strcmp   )(const nuCHAR *s1, const nuCHAR *s2);
    nuINT    (  *core_strncmp  )(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
	nuINT    (  *core_stricmp   )(const nuCHAR *s1, const nuCHAR *s2);
	nuINT    (  *core_strnicmp  )(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
    nuCHAR  *(  *core_strstr   )(const nuCHAR *haystack, const nuCHAR *needle);
    nuCHAR  *(  *core_strchr   )(const nuCHAR *s, nuINT c);
    nuCHAR  *(  *core_strrchr  )(const nuCHAR *s, nuINT c);
    nuLONG   (  *core_strtol   )(const nuCHAR *nptr, nuCHAR **endptr, nuINT base);
	nuDOUBLE (  *core_strtod   )(const nuCHAR *nptr, nuCHAR **endptr);
    nuCHAR  *(  *core_strtok   )(nuCHAR *str, const nuCHAR *delim);
    nuCHAR  *(  *core_strtok_r )(nuCHAR *str, const nuCHAR *delim, nuCHAR** pTag);
    nuCHAR  *(  *core_itoa     )(nuLONG i, nuCHAR *pDes, nuINT base);

    /// wide string functions
    nuINT    (  *core_wcslen   )(const nuWCHAR *ws);
    nuWCHAR *(  *core_wcscpy   )(nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
    nuWCHAR *(  *core_wcsncpy  )(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
    nuWCHAR *(  *core_wcscat   )(nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
    nuWCHAR *(  *core_wcsncat  )(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
    nuINT    (  *core_wcscmp   )(const nuWCHAR *ws1, const nuWCHAR *ws2);
    nuINT    (  *core_wcsncmp  )(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
	nuINT    (  *core_wcsicmp  )(const nuWCHAR *ws1, const nuWCHAR *ws2);
	nuINT    (  *core_wcsnicmp )(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
    nuWCHAR *(  *core_wcsstr   )(const nuWCHAR *haystack, const nuWCHAR *needle);
    nuWCHAR *(  *core_wcschr   )(const nuWCHAR *ws, nuINT c);
    nuWCHAR *(  *core_wcsrchr  )(const nuWCHAR *ws, nuINT c);
    nuLONG   (  *core_wcstol   )(const nuWCHAR *nptr, nuWCHAR **endptr, nuINT base);
	nuDOUBLE (  *core_wcstod   )(const nuWCHAR *nptr, nuWCHAR **endptr);
    nuWCHAR *(  *core_wcstok   )(nuWCHAR *wstr, const nuWCHAR *delim);
    nuWCHAR *(  *core_wcstok_r )(nuWCHAR *wstrToken, const nuWCHAR *wstrDelimit, nuWCHAR** pTag);
    nuWCHAR *(  *core_itow     )(nuLONG i, nuWCHAR *nptr, nuINT base);


    /// string transform function
    nuCHAR  *(  *core_WcstoAsc       )(nuCHAR *pDes, nuSIZE nAscMax, const nuWCHAR *pSrc, nuSIZE nWcsMax);
    nuWCHAR *(  *core_AsctoWcs       )(nuWCHAR *pDes, nuSIZE nWcsMax, const nuCHAR *pSrc, nuSIZE nAscMax);

    nuBOOL   (  *core_WcsSameAs      )(nuWCHAR *A, nuWCHAR *B);
    nuBOOL   (  *core_WcsFuzzyJudge_O)(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
    nuBOOL   (  *core_WcsFuzzyJudge  )(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen, nuWORD endWord);
    nuINT    (  *core_WcsKeyMatch    )(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
    nuBOOL   (  *core_WcsDelSpSign   )(nuWCHAR *pDes, nuWCHAR *pSrc, nuUINT desLen, nuUINT srcLen);

    nuDOUBLE (  *core_cos  )(nuLONG nAngle);
    nuDOUBLE (  *core_sin  )(nuLONG nAngle);
    nuDOUBLE (  *core_tan  )(nuLONG nAngle);
    nuDOUBLE (  *core_atan2)(nuLONG dy, nuLONG dx);
    nuDOUBLE (  *core_sqrt )(nuDOUBLE fValue);
    nuDOUBLE (  *core_pow  )(nuDOUBLE base, nuDOUBLE exp);

    nuLONG   (  *core_GetlBase)(nuVOID);
    nuLONG   (  *core_latan2  )(nuLONG dy, nuLONG dx);
    nuLONG   (  *core_lsin    )(nuLONG nDegree);
    nuLONG   (  *core_lcos    )(nuLONG nDegree);
    nuLONG   (  *core_ltan    )(nuLONG nDegree);
	nuLONG   (  *core_lsqrt   )(nuLONG dx, nuLONG dy);
	nuUSHORT (  *core_lQSqrt  )(nuULONG a);

    nuDOUBLE (  *core_CosJ    )(nuDOUBLE nAngle);
    nuDOUBLE (  *core_SinJ    )(nuDOUBLE nAngle);
};

extern struct core_struct *nu_clib;

#endif
