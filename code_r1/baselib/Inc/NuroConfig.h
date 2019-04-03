#ifndef __NUCONFIG_H_2012_7_20
#define __NUCONFIG_H_2012_7_20

#ifdef WIN32
#define NURO_OS_WINDOWS
#endif

#ifdef WINCE
#define NURO_OS_WINDOWS
#define NURO_OS_WINCE
#endif

#ifdef __gnu_linux__
#define NURO_OS_LINUX
#endif

#ifdef ANDROID
#define NURO_OS_LINUX
#endif

#ifdef _UNICODE
#define NURO_UNICODE
#endif

#if (defined NURO_OS_UCOS) || (defined NURO_OS_ECOS) || (defined NURO_OS_LINUX)
#define __ATTRIBUTE_PACKED__	__attribute__((__packed__))
#else
#define __ATTRIBUTE_PACKED__
#endif

#if ( defined NURO_OS_HV_UCOS )
#define __ATTRIBUTE_PRE __packed
#else
#define __ATTRIBUTE_PRE 
#endif


#endif // File nuConfig.h
