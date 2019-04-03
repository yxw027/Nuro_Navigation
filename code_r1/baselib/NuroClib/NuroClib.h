#ifndef __NURO_CLIB_H_20120711
#define __NURO_CLIB_H_20120711

#include "../Inc/NuroTypes.h"
#include "NuroCore.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NURO_UNICODE

#	define nuTcslen		nuStrlen
#	define nuTcscpy		nuStrcpy
#	define nuTcsncpy	nuStrncpy
#	define nuTcscat		nuStrcat
#	define nuTcsncat	nuStrncat
#	define nuTcscmp		nuStrcmp
#	define nuTcsncmp	nuStrncmp
#	define nuTcsicmp	nuStricmp
#	define nuTcsnicmp	nuStrnicmp
#	define nuTcsstr		nuStrstr
#	define nuTcschr		nuStrchr
#	define nuTcsrchr	nuStrrchr
#	define nuTcstol		nuStrtol
#	define nuTcstod		nuStrtod
#	define nuTcstok		nuStrtok
#	define nuTcstok_r	nuStrtok_r
#	define nuItot		nuItoa

#else

#	define nuTcslen		nuWcslen
#	define nuTcscpy		nuWcscpy
#	define nuTcsncpy	nuWcsncpy
#	define nuTcscat		nuWcscat
#	define nuTcsncat	nuWcsncat
#	define nuTcscmp		nuWcscmp
#	define nuTcsncmp	nuWcsncmp
#	define nuTcsicmp	nuWcsicmp
#	define nuTcsnicmp	nuWcsnicmp
#	define nuTcsstr		nuWcsstr
#	define nuTcschr		nuWcschr
#	define nuTcsrchr	nuWcsrchr
#	define nuTcstol		nuWcstol
#	define nuTcstod		nuWcstod
#	define nuTcstok		nuWcstok
#	define nuTcstok_r	nuWcstok_r
#	define nuItot		nuItow

#endif


/// mem functions
nuVOID *nuMalloc(nuSIZE n);
nuVOID  nuFree(nuVOID *p);
nuVOID *nuMemset(nuVOID *s, nuINT c, nuSIZE n);
nuVOID *nuMemcpy(nuVOID *dest, const nuVOID *src, nuSIZE n);
nuVOID *nuMemmove(nuVOID *dest, const nuVOID *src, nuSIZE n);
nuINT  nuMemcmp(const nuVOID *s1, const nuVOID *s2, nuSIZE n);

/// format string print functions
nuINT  nuSscanf(const nuCHAR *buffer, const nuCHAR *format , ...);
nuINT  nuSprintf(nuCHAR *buffer, const nuCHAR *format, ...);

/// format string print functions
nuINT  nuSwscanf(const nuWCHAR *buffer, const nuWCHAR *format , ...);
nuINT  nuSwprintf(nuWCHAR *buffer, const nuWCHAR *format, ...);

/// string functions
nuSIZE nuStrlen(const nuCHAR *s);
nuCHAR *nuStrcpy(nuCHAR *dest, const nuCHAR *src);
nuCHAR *nuStrncpy(nuCHAR *dest, const nuCHAR *src, nuSIZE n);
nuCHAR *nuStrcat(nuCHAR *dest, const nuCHAR *src);
nuCHAR *nuStrncat(nuCHAR *dest, const nuCHAR *src, nuSIZE n);
nuINT nuStrcmp(const nuCHAR *s1, const nuCHAR *s2);
nuINT nuStrncmp(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
nuINT nuStricmp(const nuCHAR *s1, const nuCHAR *s2);
nuINT nuStrnicmp(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n);
nuCHAR *nuStrstr(const nuCHAR *haystack, const nuCHAR *needle);
nuCHAR *nuStrchr(const nuCHAR *s, nuINT c);
nuCHAR *nuStrrchr(const nuCHAR *s, nuINT c);
nuLONG nuStrtol(const nuCHAR *nptr, nuCHAR **endptr, nuINT base);
nuDOUBLE nuStrtod(const nuCHAR *nptr, nuCHAR **endptr);
nuCHAR *nuStrtok(nuCHAR *str, const nuCHAR *delim);
nuCHAR *nuStrtok_r(nuCHAR *strToken, const nuCHAR *strDelimit, nuCHAR** pTag);
nuCHAR *nuItoa(nuLONG i, nuCHAR *pDes, nuINT base);

/// wide string functions
nuINT    nuWcslen(const nuWCHAR *ws);
nuWCHAR *nuWcscpy(nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
nuWCHAR *nuWcsncpy(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
nuWCHAR *nuWcscat(nuWCHAR *wsDest, const nuWCHAR *wcsSrc);
nuWCHAR *nuWcsncat(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen);
nuINT    nuWcscmp(const nuWCHAR *ws1, const nuWCHAR *ws2);
nuINT    nuWcsncmp(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
nuINT    nuWcsicmp(const nuWCHAR *ws1, const nuWCHAR *ws2);
nuINT    nuWcsnicmp(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n);
nuWCHAR *nuWcsstr(const nuWCHAR *haystack, const nuWCHAR *needle);
nuWCHAR *nuWcschr(const nuWCHAR *ws, nuINT wc);
nuWCHAR *nuWcsrchr(const nuWCHAR *ws, nuINT wc);
nuLONG   nuWcstol(const nuWCHAR *nptr, nuWCHAR **endptr, nuINT base);
nuDOUBLE nuWcstod(const nuWCHAR *nptr, nuWCHAR **endptr);
nuWCHAR *nuWcstok(nuWCHAR *wstr, const nuWCHAR *delim);
nuWCHAR *nuWcstok_r(nuWCHAR *wstrToken, const nuWCHAR *wstrDelimit, nuWCHAR** pTag);
nuWCHAR *nuItow(nuLONG i, nuWCHAR *pDes, nuINT base);


/// string transform function
nuCHAR  *nuWcstoAsc(nuCHAR *pDes, nuSIZE nAscMax, const nuWCHAR *pSrc, nuSIZE nWcsMax);
nuWCHAR *nuAsctoWcs(nuWCHAR *pDes, nuSIZE nWcsMax, const nuCHAR *pSrc, nuSIZE nAscMax);

nuBOOL  nuWcsSameAs(nuWCHAR *A, nuWCHAR *B);
nuBOOL  nuWcsFuzzyJudge_O(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
nuBOOL  nuWcsFuzzyJudge(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen, nuWORD endWord);
nuINT   nuWcsKeyMatch(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen);
nuBOOL  nuWcsDelSpSign(nuWCHAR *pDes, nuWCHAR *pSrc, nuUINT desLen, nuUINT srcLen);

nuDOUBLE nuCos(nuLONG nAngle);
nuDOUBLE nuSin(nuLONG nAngle);
nuDOUBLE nuTan(nuLONG nAngle);
nuDOUBLE nuAtan2(nuLONG dy, nuLONG dx);
nuDOUBLE nuSqrt(nuDOUBLE fValue);
nuDOUBLE nuPow(nuDOUBLE base, nuDOUBLE exp);

nuLONG   nuGetlBase();
nuLONG   nulAtan2(nuLONG dy, nuLONG dx);
nuLONG   nulSin(nuLONG nDegree);
nuLONG   nulCos(nuLONG nDegree);
nuLONG   nulTan(nuLONG nDegree);
nuLONG	 nulSqrt(nuLONG dx, nuLONG dy);
nuUSHORT nulQSqrt(nuULONG a);

nuDOUBLE	nuCosJ(nuDOUBLE nAngle);
nuDOUBLE	nuSinJ(nuDOUBLE nAngle);

#ifdef __cplusplus
}
#endif

#endif
