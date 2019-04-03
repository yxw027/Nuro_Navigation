#include "../NuroClib.h"

//#ifdef NURO_OS_LINUX 
#define USE_SYSTEM_DIRECT_API
//#endif

//#define __CORE_DEBUG

#ifdef __CORE_DEBUG
#   include <stdio.h>
#   define PRINT_TRACE() printf("Use Func %s \n", __FUNCTION__)
//#   define PRINT_TRACE() do_trace(__FUNCTION__)
#else
#   define PRINT_TRACE() // do {} while(0)
#endif

#ifdef USE_SYSTEM_DIRECT_API
#   include <stdlib.h>
#   include <string.h>
#endif

#ifdef __CORE_DEBUG
static void do_trace(const char *str)
{
    FILE *fp = fopen("/mnt/sdcard/nurownds/trace.txt", "a+");
    fprintf(fp, "%s\n", str);
    fclose(fp);
}
#endif

/// mem functions
nuVOID *nuMalloc(nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return malloc(n);
#else
    return nu_clib->core_malloc(n);
#endif
}

nuVOID nuFree(nuVOID *p)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    free(p);
#else
	nu_clib->core_free(p);
#endif
}

nuVOID *nuMemset(nuVOID *s, nuINT c, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return memset(s, c, n);
#else
    return nu_clib->core_memset(s, c, n);
#endif
}

nuVOID *nuMemcpy(nuVOID *dest, const nuVOID *src, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return memcpy(dest, src, n);
#else
    return nu_clib->core_memcpy(dest, src, n);
#endif
}

nuVOID *nuMemmove(nuVOID *dest, const nuVOID *src, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return memmove(dest, src, n);
#else
    return nu_clib->core_memmove(dest, src, n);
#endif
}

nuINT  nuMemcmp(const nuVOID *s1, const nuVOID *s2, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return memcmp(s1, s2, n);
#else
    return nu_clib->core_memcmp(s1, s2, n);
#endif
}

/// format string print functions
nuINT  nuSscanf(const nuCHAR *buffer, const nuCHAR *format , ...)
{
	PRINT_TRACE();
	return 0;
}

nuINT  nuSprintf(nuCHAR *buffer, const nuCHAR *format, ...)
{
	PRINT_TRACE();
	return 0;
}

/// format string print functions
nuINT  nuSwscanf(const nuWCHAR *buffer, const nuWCHAR *format , ...)
{
	PRINT_TRACE();
	return 0;
}

nuINT  nuSwprintf(nuWCHAR *buffer, const nuWCHAR *format, ...)
{
	PRINT_TRACE();
	return 0;
}

/// string functions
nuSIZE nuStrlen(const nuCHAR *s)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strlen(s);
#else
    return nu_clib->core_strlen(s);
#endif
}

nuCHAR *nuStrcpy(nuCHAR *dest, const nuCHAR *src)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strcpy(dest, src);
#else
    return nu_clib->core_strcpy(dest, src);
#endif
}

nuCHAR *nuStrncpy(nuCHAR *dest, const nuCHAR *src, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strncpy(dest, src, n);
#else
    return nu_clib->core_strncpy(dest, src, n);
#endif
}

nuCHAR *nuStrcat(nuCHAR *dest, const nuCHAR *src)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strcat(dest, src);
#else
    return nu_clib->core_strcat(dest, src);
#endif
}

nuCHAR *nuStrncat(nuCHAR *dest, const nuCHAR *src, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strncat(dest, src, n);
#else
    return nu_clib->core_strncat(dest, src, n);
#endif
}

nuINT nuStrcmp(const nuCHAR *s1, const nuCHAR *s2)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strcmp(s1, s2);
#else
    /* core_strcmp_impl */
    return nu_clib->core_strcmp(s1, s2);
#endif
}

nuINT nuStrncmp(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strncmp(s1, s2, n);
#else
    /* core_strncmp_impl */
    return nu_clib->core_strncmp(s1, s2, n);
#endif
}

nuINT nuStricmp(const nuCHAR *s1, const nuCHAR *s2)
{
	PRINT_TRACE();
	/* core_strcmp_impl */
	return nu_clib->core_stricmp(s1, s2);
}

nuINT nuStrnicmp(const nuCHAR *s1, const nuCHAR *s2, nuSIZE n)
{
	PRINT_TRACE();
	/* core_strncmp_impl */
	return nu_clib->core_strnicmp(s1, s2, n);
}

nuCHAR *nuStrstr(const nuCHAR *haystack, const nuCHAR *needle)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strstr(haystack, needle);
#else
    /* core_strstr_impl */
    return nu_clib->core_strstr(haystack, needle);
#endif
}

nuCHAR *nuStrchr(const nuCHAR *s, nuINT c)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strchr(s, c);
#else
    return nu_clib->core_strchr(s, c);
#endif
}

nuCHAR *nuStrrchr(const nuCHAR *s, nuINT c)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strrchr(s, c);
#else
    return nu_clib->core_strrchr(s, c);
#endif
}

nuLONG nuStrtol(const nuCHAR *nptr, nuCHAR **endptr, nuINT base)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strtol(nptr, endptr, base);
#else
    /* core_strtol_impl */
    return nu_clib->core_strtol(nptr, endptr, base);
#endif
}

nuDOUBLE nuStrtod(const nuCHAR *nptr, nuCHAR **endptr)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strtod(nptr, endptr);
#else
	/* core_strtod_impl */
	return nu_clib->core_strtod(nptr, endptr);
#endif
}

nuCHAR *nuStrtok(nuCHAR *str, const nuCHAR *delim)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strtok(str, delim);
#else
    return nu_clib->core_strtok(str, delim);
#endif
}

nuCHAR *nuStrtok_r(nuCHAR *strToken, const nuCHAR *strDelimit, nuCHAR** pTag)
{
	PRINT_TRACE();
#ifdef USE_SYSTEM_DIRECT_API
    return strtok_r(strToken, strDelimit, pTag);
#else
    return nu_clib->core_strtok_r(strToken, strDelimit, pTag);
#endif
}

nuCHAR *nuItoa(nuLONG i, nuCHAR *pDes, nuINT base)
{
	PRINT_TRACE();
    /* core_itoa_impl */
    return nu_clib->core_itoa(i, pDes, base);
}

/// wide string functions
nuINT nuWcslen(const nuWCHAR *ws)
{
	PRINT_TRACE();
    return nu_clib->core_wcslen(ws);
}

nuWCHAR *nuWcscpy(nuWCHAR *wsDest, const nuWCHAR *wcsSrc)
{
	PRINT_TRACE();
    return nu_clib->core_wcscpy(wsDest, wcsSrc);
}

nuWCHAR *nuWcsncpy(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen)
{
	PRINT_TRACE();
    return nu_clib->core_wcsncpy(wsDest, wcsSrc, nWcsLen);
}

nuWCHAR *nuWcscat(nuWCHAR *wsDest, const nuWCHAR *wcsSrc)
{
	PRINT_TRACE();
    return nu_clib->core_wcscat(wsDest, wcsSrc);
}

nuWCHAR *nuWcsncat(nuWCHAR *wsDest, const nuWCHAR *wcsSrc, nuINT nWcsLen)
{
	PRINT_TRACE();
    return nu_clib->core_wcsncat(wsDest, wcsSrc, nWcsLen);
}

nuINT nuWcscmp(const nuWCHAR *ws1, const nuWCHAR *ws2)
{
	PRINT_TRACE();
    return nu_clib->core_wcscmp(ws1, ws2);
}

nuINT nuWcsncmp(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n)
{
	PRINT_TRACE();
    return nu_clib->core_wcsncmp(ws1, ws2, n);
}

nuINT nuWcsicmp(const nuWCHAR *ws1, const nuWCHAR *ws2)
{
	PRINT_TRACE();
	return nu_clib->core_wcsicmp(ws1, ws2);
}

nuINT nuWcsnicmp(const nuWCHAR *ws1, const nuWCHAR *ws2, nuSIZE n)
{
	PRINT_TRACE();
	return nu_clib->core_wcsnicmp(ws1, ws2, n);
}

nuWCHAR *nuWcsstr(const nuWCHAR *haystack, const nuWCHAR *needle)
{
	PRINT_TRACE();
    return nu_clib->core_wcsstr(haystack, needle);
}

nuWCHAR *nuWcschr(const nuWCHAR *ws, nuINT wc)
{
	PRINT_TRACE();
    return nu_clib->core_wcschr(ws, wc);
}

nuWCHAR *nuWcsrchr(const nuWCHAR *ws, nuINT wc)
{
	PRINT_TRACE();
    return nu_clib->core_wcsrchr(ws, wc);
}

nuLONG nuWcstol(const nuWCHAR *nptr, nuWCHAR **endptr, nuINT base)
{
	PRINT_TRACE();
    return nu_clib->core_wcstol(nptr, endptr, base);
}

nuDOUBLE nuWcstod(const nuWCHAR *nptr, nuWCHAR **endptr)
{
	PRINT_TRACE();
	return nu_clib->core_wcstod(nptr, endptr);
}

nuWCHAR *nuWcstok(nuWCHAR *wstr, const nuWCHAR *delim)
{
	PRINT_TRACE();
	/* core_wcstok_impl */
    return nu_clib->core_wcstok(wstr, delim);
}

nuWCHAR* nuWcstok_r(nuWCHAR *strToken, const nuWCHAR *strDelimit, nuWCHAR** pTag)
{
	PRINT_TRACE();
	/* core_wcstok_r_impl */
    return nu_clib->core_wcstok_r(strToken, strDelimit, pTag);
}

nuWCHAR* nuItow(nuLONG i, nuWCHAR *pDes, nuINT base)
{
	PRINT_TRACE();
	/* core_itow_impl */
    return nu_clib->core_itow(i, pDes, base);
}

// string transform function
nuCHAR *nuWcstoAsc(nuCHAR *pDes, nuSIZE nAscMax, const nuWCHAR *pSrc, nuSIZE nWcsMax)
{
	PRINT_TRACE();
	/* core_WcstoAsc_impl */
    return nu_clib->core_WcstoAsc(pDes, nAscMax, pSrc, nWcsMax);
}

nuWCHAR *nuAsctoWcs(nuWCHAR *pDes, nuSIZE nWcsMax, const nuCHAR *pSrc, nuSIZE nAscMax)
{
	PRINT_TRACE();
    return nu_clib->core_AsctoWcs(pDes, nWcsMax, pSrc, nAscMax);
}

nuBOOL  nuWcsSameAs(nuWCHAR *A, nuWCHAR *B)
{
	PRINT_TRACE();
    return nu_clib->core_WcsSameAs(A, B);
}

nuBOOL  nuWcsFuzzyJudge_O(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen)
{
	PRINT_TRACE();
    return nu_clib->core_WcsFuzzyJudge_O(pDes, pKey, desLen, keyLen);
}

nuBOOL  nuWcsFuzzyJudge(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen, nuWORD endWord)
{
	PRINT_TRACE();
    return nu_clib->core_WcsFuzzyJudge(pDes, pKey, desLen, keyLen, endWord);
}

nuINT   nuWcsKeyMatch(nuWCHAR *pDes, nuWCHAR *pKey, nuUINT desLen, nuUINT keyLen)
{
	PRINT_TRACE();
    return nu_clib->core_WcsKeyMatch(pDes, pKey, desLen, keyLen);
}

nuBOOL  nuWcsDelSpSign(nuWCHAR *pDes, nuWCHAR *pSrc, nuUINT desLen, nuUINT srcLen)
{
	PRINT_TRACE();
    return nu_clib->core_WcsDelSpSign(pDes, pSrc, desLen, srcLen);
}

nuDOUBLE nuCos(nuLONG nAngle)
{
	PRINT_TRACE();
    return nu_clib->core_cos(nAngle);
}

nuDOUBLE nuSin(nuLONG nAngle)
{
	PRINT_TRACE();
    return nu_clib->core_sin(nAngle);
}

nuDOUBLE nuTan(nuLONG nAngle)
{
	PRINT_TRACE();
    return nu_clib->core_tan(nAngle);
}

nuDOUBLE nuAtan2(nuLONG dy, nuLONG dx)
{
	PRINT_TRACE();
    return nu_clib->core_atan2(dy, dx);
}

nuDOUBLE nuSqrt(nuDOUBLE fValue)
{
	PRINT_TRACE();
    return nu_clib->core_sqrt(fValue);
}

nuDOUBLE nuPow(nuDOUBLE base, nuDOUBLE exp)
{
	PRINT_TRACE();
    return nu_clib->core_pow(base, exp);
}

nuLONG nuGetlBase()
{
	PRINT_TRACE();
    return nu_clib->core_GetlBase();
}

nuLONG nulAtan2(nuLONG dy, nuLONG dx)
{
	PRINT_TRACE();
    return nu_clib->core_latan2(dy, dx);
}

nuLONG nulSin(nuLONG nDegree)
{
	PRINT_TRACE();
    return nu_clib->core_lsin(nDegree);
}

nuLONG nulCos(nuLONG nDegree)
{
	PRINT_TRACE();
    return nu_clib->core_lcos(nDegree);
}

nuLONG nulTan(nuLONG nDegree)
{
	PRINT_TRACE();
    return nu_clib->core_ltan(nDegree);
}

nuLONG	 nulSqrt(nuLONG dx, nuLONG dy)
{
	PRINT_TRACE();
	return nu_clib->core_lsqrt(dx, dy);
}

nuUSHORT nulQSqrt(nuULONG a)
{
	PRINT_TRACE();
	return nu_clib->core_lQSqrt(a);
}


nuDOUBLE	nuCosJ(nuDOUBLE nAngle)
{
	PRINT_TRACE();
    return nu_clib->core_CosJ(nAngle);
}

nuDOUBLE	nuSinJ(nuDOUBLE nAngle)
{
	PRINT_TRACE();
    return nu_clib->core_SinJ(nAngle);
}

