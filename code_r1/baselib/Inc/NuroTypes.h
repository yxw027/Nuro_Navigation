#ifndef __NURO_TYPES_H_20120712
#define __NURO_TYPES_H_20120712

#include "NuroConfig.h"
#include "bits/NuroTypes.h"

#define nuVOID void

// integers
// 8 bits
typedef   ___nuro_char_t  nuCHAR;
typedef   ___nuro_uint8_t nuUCHAR;
typedef   ___nuro_uint8_t nuUINT8;


// 16 bits
typedef  ___nuro_int16_t nuSHORT;
typedef  ___nuro_uint16_t nuUSHORT;
typedef  ___nuro_wchar_t nuWCHAR;
typedef ___nuro_uint16_t nuWORD;


// 32 bits
typedef ___nuro_int32_t  nuINT;
typedef ___nuro_uint32_t nuUINT;
typedef ___nuro_long_t   nuLONG;
typedef ___nuro_ulong_t  nuULONG;
typedef ___nuro_ulong_t  nuDWORD;


// 64 bits
typedef  ___nuro_int64_t nuINT64;
typedef ___nuro_uint64_t nuUINT64;

// float ,double
typedef  ___nuro_float_t nuFLOAT;
typedef ___nuro_double_t nuDOUBLE;



// Extended for SVr4, 4.3BSD, C89, C99 and POSIX.1-2001 and WINDOWS-LIKE types.
typedef ___nuro_size_t   nuSIZE;
typedef ___nuro_bool_t   nuBOOL;
typedef ___nuro_uint8_t  nuBYTE;

typedef struct ___nuro_file_struct
{
	char unused;
} nuFILE;

typedef void             *nuHINSTANCE;
typedef void             *nuPVOID;
typedef nuBYTE           *nuPBYTE;
typedef nuWORD           *nuPWORD;
typedef nuDWORD          *nuPDWORD;
typedef void             *nuHANDLE;
typedef void             *nuHMODULE;
typedef void			 *nuHDC;
typedef void			 *nuHWND;

typedef nuDWORD	          nuCOLORREF;


#ifndef NURO_UNICODE
typedef nuCHAR           nuTCHAR;
#else
typedef nuWCHAR          nuTCHAR;
#endif


#ifndef NURO_NULL_IN_C
#	define NURO_NULL_IN_C ((void *)0)
#endif

#ifndef NURO_NULL_IN_CPP
#	define NURO_NULL_IN_CPP 0
#endif

#ifndef __cplusplus
#	ifndef NURO_NULL
#		define NURO_NULL NURO_NULL_IN_C
#	endif
#else
#	ifndef NURO_NULL
#		define NURO_NULL NURO_NULL_IN_CPP
#	endif
#endif

#ifndef NULL
#	define NULL NURO_NULL
#endif

#ifndef nuNULL
#	define nuNULL NURO_NULL
#endif

#define nuFALSE 0
#define nuTRUE  1

#define NURO_UNUSED(x) (void)(x)

#ifndef NURO_UNICODE
#	define	nuTEXT(x) x
#else
#	define	nuTEXT(x) L##x
#endif


//////////////////////////////////////////////////////////////////////////

#ifdef NURO_OS_LINUX
typedef void *HDC;
typedef void *HBITMAP;
typedef void *HFONT;
typedef void *HPEN;
typedef void *HBRUSH;
typedef void *HWND;
#endif

#ifdef NURO_OS_LINUX
#define NURO_BI_BITFIELDS 0x3L
#define NURO_BI_RGB       0
#endif

#ifdef NURO_OS_WINDOWS
#define NURO_BI_BITFIELDS BI_BITFIELDS
#define NURO_BI_RGB       BI_RGB
#endif

// Graphics
typedef struct nuroRECT
{
	nuLONG		left;
	nuLONG		top;
	nuLONG		right;
	nuLONG		bottom;
} NURORECT, *PNURORECT, *LPNURORECT;
typedef struct nuroSRECT
{
	nuSHORT		left;
	nuSHORT		top;
	nuSHORT		right;
	nuSHORT		bottom;
}NUROSRECT, *PNUROSRECT, *LPNUROSRECT;
typedef struct nuroPOINT
{
	nuLONG		x;
	nuLONG		y;
}NUROPOINT, *PNUROPOINT, *LPNUROPOINT;
typedef struct nuroSPOINT
{
	nuSHORT		x;
	nuSHORT		y;
}NUROSPOINT, *PNUROSPOINT, *LPNUROSPOINT;

typedef struct tagBLOCKIDARRAY
{
	nuLONG		nYstart;
	nuLONG		nYend;
	nuLONG		nXstart;
	nuLONG		nXend;
}BLOCKIDARRAY, *PBLOCKIDARRAY;



#endif // File NuroTypes.h

