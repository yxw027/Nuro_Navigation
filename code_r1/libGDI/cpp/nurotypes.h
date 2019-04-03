#ifndef NURO_TYPES_H_FJN2349AJFZNMBYUU
#define NURO_TYPES_H_FJN2349AJFZNMBYUU


namespace nuro
{
	typedef char  S8;
	typedef unsigned char  U8;
	typedef short S16;
	typedef unsigned short U16;
	typedef int   S32;
	typedef unsigned int   U32;
	
#ifdef NURO_NO_SUPPORT_64
	typedef S32 S64
	typedef U32 U64
#else
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		typedef __int64 S64;
		typedef unsigned __int64 U64;
	#else
		typedef long long int S64;
		typedef unsigned long long int U64;
	#endif
#endif
}

typedef nuro::S8  nuroS8;
typedef nuro::U8  nuroU8;
typedef nuro::S16 nuroS16;
typedef nuro::U16 nuroU16;
typedef nuro::S32 nuroS32;
typedef nuro::U32 nuroU32;
typedef nuro::S64 nuroS64;
typedef nuro::U64 nuroU64;

#endif

