#ifndef __NURO_NATIVE_H_20120713
#define __NURO_NATIVE_H_20120713
#include "../Inc/NuroTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

nuVOID  *core_malloc_impl   (nuSIZE n);
nuVOID   core_free_impl     (nuVOID *p);
nuVOID  *core_memset_impl   (nuVOID *s, nuINT c, nuSIZE n);
nuVOID  *core_memcpy_impl   (nuVOID *dest, const nuVOID *src, nuSIZE n);
nuVOID  *core_memmove_impl  (nuVOID *dest, const nuVOID *src, nuSIZE n);
nuINT    core_memcmp_impl   (const nuVOID *s1, const nuVOID *s2, nuSIZE n);

/// format string print functions
nuINT    core_sscanf_impl   (const nuCHAR *buffer, const nuCHAR *format , ...);
nuINT    core_sprintf_impl  (nuCHAR *buffer, const nuCHAR *format, ...);

/// format string print functions
nuINT    core_swscanf_impl  (const nuWCHAR *buffer, const nuWCHAR *format , ...);
nuINT    core_swprintf_impl (nuWCHAR *buffer, const nuWCHAR *format, ...);

/// string functions
nuSIZE   core_strlen_impl   (const nuCHAR *s);
nuCHAR  *core_strcpy_impl   (nuCHAR *dest, const nuCHAR *src);
nuCHAR  *core_strncpy_impl  (nuCHAR *dest, const nuCHAR *src, nuSIZE n);
nuCHAR  *core_strcat_impl   (nuCHAR *dest, const nuCHAR *src);
nuCHAR  *core_strncat_impl  (nuCHAR *dest, const nuCHAR *src, nuSIZE n);
nuINT    core_strcmp_impl   (const nuCHAR *s1, const nuCHAR *s2);
nuINT    core_strncmp_impl  (const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
nuINT    core_stricmp_impl  (const nuCHAR *s1, const nuCHAR *s2);
nuINT    core_strnicmp_impl (const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
nuCHAR  *core_strstr_impl   (const nuCHAR *haystack, const nuCHAR *needle);
nuCHAR  *core_strchr_impl   (const nuCHAR *s, nuINT c);
nuCHAR  *core_strrchr_impl  (const nuCHAR *s, nuINT c);
nuLONG   core_strtol_impl   (const nuCHAR *nptr, nuCHAR **endptr, nuINT base);
nuDOUBLE core_strtod_impl   (const nuCHAR *nptr, nuCHAR **endptr);
nuCHAR  *core_strtok_impl   (nuCHAR *str, const nuCHAR *delim);
nuCHAR  *core_strtok_r_impl (nuCHAR *str, const nuCHAR *delim, nuCHAR** pTag);
nuCHAR  *core_itoa_impl     (nuLONG i, nuCHAR *nptr, nuINT base);

/// wide string functions
nuINT    core_wcslen_impl   (const nuWCHAR *ws);
nuWCHAR *core_wcscpy_impl   (nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
nuWCHAR *core_wcsncpy_impl  (nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
nuWCHAR *core_wcscat_impl   (nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
nuWCHAR *core_wcsncat_impl  (nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
nuINT    core_wcscmp_impl   (const nuWCHAR *ws1, const nuWCHAR *ws2);
nuINT    core_wcsncmp_impl  (const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
nuINT    core_wcsicmp_impl  (const nuWCHAR *ws1, const nuWCHAR *ws2);
nuINT    core_wcsnicmp_impl (const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
nuWCHAR *core_wcsstr_impl   (const nuWCHAR *haystack, const nuWCHAR *needle);
nuWCHAR *core_wcschr_impl   (const nuWCHAR *ws, nuINT c);
nuWCHAR *core_wcsrchr_impl  (const nuWCHAR *ws, nuINT c);
nuLONG   core_wcstol_impl   (const nuWCHAR *nptr, nuWCHAR **endptr, nuINT base);
nuDOUBLE core_wcstod_impl   (const nuWCHAR *nptr, nuWCHAR **endptr);
nuWCHAR *core_wcstok_impl   (nuWCHAR *wstr, const nuWCHAR *delim);
nuWCHAR *core_wcstok_r_impl (nuWCHAR *wstrToken, const nuWCHAR *wstrDelimit, nuWCHAR** pTag);
nuWCHAR *core_itow_impl     (nuLONG i, nuWCHAR *nptr, nuINT base);


/// string transform function
nuCHAR  *core_WcstoAsc_impl       (nuCHAR *pDes, nuSIZE nAscMax, const nuWCHAR *pSrc, nuSIZE nWcsMax);
nuWCHAR *core_AsctoWcs_impl       (nuWCHAR *pDes, nuSIZE nWcsMax, const nuCHAR *pSrc, nuSIZE nAscMax);

nuBOOL   core_WcsSameAs_impl      (nuWCHAR *A, nuWCHAR *B);
nuBOOL   core_WcsFuzzyJudge_O_impl(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
nuBOOL   core_WcsFuzzyJudge_impl  (nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen, nuWORD endWord);
nuINT    core_WcsKeyMatch_impl    (nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
nuBOOL   core_WcsDelSpSign_impl   (nuWCHAR *pDes, nuWCHAR *pSrc, nuUINT desLen, nuUINT srcLen);

nuDOUBLE core_cos_impl  (nuLONG nAngle);
nuDOUBLE core_sin_impl  (nuLONG nAngle);
nuDOUBLE core_tan_impl  (nuLONG nAngle);
nuDOUBLE core_atan2_impl(nuLONG dy, nuLONG dx);
nuDOUBLE core_sqrt_impl (nuDOUBLE fValue);
nuDOUBLE core_pow_impl  (nuDOUBLE base, nuDOUBLE exp);

nuLONG   core_GetlBase_impl(nuVOID);
nuLONG   core_latan2_impl  (nuLONG dy, nuLONG dx);
nuLONG   core_lsin_impl    (nuLONG nDegree);
nuLONG   core_lcos_impl    (nuLONG nDegree);
nuLONG   core_ltan_impl    (nuLONG nDegree);
nuLONG   core_lsqrt_impl   (nuLONG dx, nuLONG dy);
nuUSHORT core_lQSqrt_impl  (nuULONG a);

/// 原来nuClib中的函数
nuDOUBLE core_CosJ_impl    (nuDOUBLE nAngle);
nuDOUBLE core_SinJ_impl    (nuDOUBLE nAngle);


#ifdef __cplusplus
extern "C" {
#endif

#endif // NuroNative.h
