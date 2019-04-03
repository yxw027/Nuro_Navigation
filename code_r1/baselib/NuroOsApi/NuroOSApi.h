#ifndef __NURO_OS_API_H_2012_07_20
#define __NURO_OS_API_H_2012_07_20
#include "../Inc/NuroTypes.h"
#include "../NuroClib/NuroClib.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
//file
#define NURO_FS_RB		1	// _O_RDONLY | _O_BINARY                          =>  "rb"
#define NURO_FS_WB		2	// _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY    =>  "wb"
#define NURO_FS_AB		3	// _O_WRONLY / _O_CREAT | _O_APPEND | _O_BINARY   =>  "ab"
#define NURO_FS_OB		4	// _O_RDWR	| _O_BINARY | _O_CREAT                =>  "rb+"
#define NURO_FS_OB2		5	// _O_RDWR	| _O_BINARY | _O_CREAT | FS_O_TRUNC   =>  "wb+"

#define NURO_SEEK_SET	0
#define NURO_SEEK_CUR	1
#define NURO_SEEK_END	2

#define NURO_MAX_PATH	256

//Thread interface
#define NURO_THREAD_CREATE_DEFAULT          0
#define NURO_THREAD_PRIORITY_HIGHT          20
#define NURO_THREAD_PRIORIYT_NORMAL         30
#define NURO_THREAD_PRIORITY_LOW            40

#define NURO_INVALID_HANDLE_VALUE			((nuHANDLE)-1)


///Open APIs Types.////////////////////////////////////////////////////////
typedef nuDWORD (*LPNURO_THREAD_START_ROUTINE)(nuPVOID arg_addr);

typedef struct _NURO_SECURITY_ATTRIBUTES {
	nuDWORD nLength;
	nuPVOID lpSecurityDescriptor;
	nuBOOL bInheritHandle;
} NURO_SECURITY_ATTRIBUTES, *PNURO_SECURITY_ATTRIBUTES;


///Open APIs Interface.////////////////////////////////////////////////////
// File API

#ifndef NURO_UNICODE
#define nuTfopen nuFopen
#else
#define nuTfopen nuWfopen
#endif

nuFILE *nuFopen(const nuCHAR *sFileName, nuINT nMode);
nuFILE *nuWfopen(const nuWCHAR *sFileName, nuINT nMode);
nuINT	nuFseek(nuFILE *pFile, nuLONG nDis, nuINT nType);
nuSIZE	nuFwrite(const nuVOID *pBuff, nuSIZE size, nuSIZE count, nuFILE *pFile);
nuSIZE	nuFread(nuVOID *pBuff, nuSIZE size, nuSIZE count, nuFILE *pFile);
nuINT	nuFclose(nuFILE *pFile);
nuSIZE	nuFtell(nuFILE *pFile);
nuSIZE	nuFgetlen(nuFILE *pFile);

nuSIZE	nuFreadLine(nuWCHAR *pBuff,  nuSIZE nMaxLen, nuFILE *pFile);
nuSIZE	nuFreadLineA(nuCHAR *pBuff,  nuSIZE nMaxLen, nuFILE *pFile);


// DLL load
#ifndef NURO_UNICODE
#define nuLoadLibrary nuLoadLibraryA
#else
#define nuLoadLibrary nuLoadLibraryW
#endif
nuHMODULE	nuLoadLibraryA(const nuCHAR *sFile);
nuHMODULE	nuLoadLibraryW(const nuWCHAR *wcsFile);
nuBOOL		nuFreeLibrary(nuHMODULE hModule);
nuPVOID		nuGetProcAddress(nuHMODULE hModule, const nuCHAR *sFucName);

// Thread API
nuHANDLE nuCreateThread (PNURO_SECURITY_ATTRIBUTES lpsa, 
					     nuDWORD cbStack, 
					     LPNURO_THREAD_START_ROUTINE lpStartAddr,
					     nuPVOID lpvThreadParam,
					     nuDWORD fdwCreate,
					     nuDWORD *lpIDThread);
nuBOOL  nuDelThread(nuHANDLE hHandle);
nuDWORD nuThreadWaitReturn(nuHANDLE hHandle);
nuBOOL  nuSetThreadPriority(nuHANDLE hHandle, nuINT nPriority);

//Process and data path
nuVOID nuSetModulePath(nuCHAR *strPathName);
#ifndef NURO_UNICODE
#define nuGetModulePath nuGetModulePathA
#else
#define nuGetModulePath nuGetModulePathW
#endif

nuDWORD nuGetModulePathA(nuHMODULE hModule, nuCHAR  *lpFilename, nuDWORD nSize);
nuDWORD nuGetModulePathW(nuHMODULE hModule, nuWCHAR *lpFilename, nuDWORD nSize);

#ifndef NURO_UNICODE
#define nuGetDataPath nuGetDataPathA
#else
#define nuGetDataPath nuGetDataPathW
#endif

nuDWORD nuGetDataPathA(nuHMODULE hModule, nuCHAR  *lpFilename, nuDWORD nSize);
nuDWORD nuGetDataPathW(nuHMODULE hModule, nuWCHAR *lpFilename, nuDWORD nSize);

nuINT nuMbsToWcs(nuCHAR *pSrc, nuWCHAR *pDes, nuINT nMaxWords);
nuINT nuWcsToMbs(nuWCHAR *pSrc, nuCHAR *pDes, nuINT nMaxWords);

nuVOID  nuSleep(nuDWORD nMiniseconds);
nuDWORD nuGetTickCount();

nuBOOL  nuReadConfigValue(const nuCHAR *pStr, nuLONG *pValue);
nuSIZE  nuFreadLineIndexW(nuINT nLineIdx, nuWCHAR *pBuff, nuSIZE nWsNum, nuFILE *pFile);

//Time
typedef struct tagNUROTIME {
	nuWORD nYear; 
	nuWORD nMonth; 
	nuWORD nDayOfWeek; 
	nuWORD nDay; 
	nuWORD nHour; 
	nuWORD nMinute; 
	nuWORD nSecond; 
	nuWORD nMilliseconds; 
} NUROTIME;

nuVOID nuGetLocalTime(NUROTIME* pnuTime);
nuBOOL nuSetLocalTime(NUROTIME* pnuTime);

nuVOID	nuSetVolume(nuUINT iVol);
nuUINT	nuGetVolume();

#ifndef NURO_UNICODE
#define nuCopyFile nuCopyFileA
#else
#define nuCopyFile nuCopyFileW
#endif
nuBOOL  nuCopyFileA(const nuCHAR *lpExistingFileName, const nuCHAR *lpNewFileName, nuBOOL bFailIfExists);
nuBOOL  nuCopyFileW(const nuWCHAR *lpExistingFileName, const nuWCHAR *lpNewFileName, nuBOOL bFailIfExists);

#ifndef NURO_UNICODE
#define nuPlaySound nuPlaySoundA
#else
#define nuPlaySound nuPlaySoundW
#endif
nuBOOL  nuPlaySoundA(nuCHAR *pszSound, nuBYTE *soundbuff, nuWORD nSyncOrASync);
nuBOOL  nuPlaySoundW(nuWCHAR *pszSound, nuBYTE *soundbuff, nuWORD nSyncOrASync);

nuBOOL nuGetMemory(nuBYTE **ppStaticMemAddr, nuDWORD dwSMsize, nuBYTE **ppDataMemAddr, nuDWORD dwDMsize);
nuVOID nuRelMemory(nuBYTE *pStaticMemAddr, nuBYTE *pDataMemAddr);


#ifndef NURO_UNICODE
#define nuOpenCom nuOpenComA
#else
#define nuOpenCom nuOpenComW
#endif
nuHANDLE nuOpenComA(nuCHAR *gps_comport, nuDWORD nBaudRate);
nuHANDLE nuOpenComW(nuWCHAR *gps_comport, nuDWORD nBaudRate);
nuBOOL   nuCloseCom(nuHANDLE hCom);

nuBOOL nuReadCom(nuHANDLE hCom, nuPVOID buffer, nuDWORD nNumberOfBytesToRead,
				 nuPDWORD lpNumberOfBytesRead, nuPVOID pOverlapped);
nuBOOL nuWriteCom(nuHANDLE hCom, nuPVOID buffer, nuDWORD nNumberOfBytesToWrite,
				  nuPDWORD lpNumberOfBytesWritten, nuPVOID pOverlapped);
void DebugMessage_NT(nuLONG lValue, nuCHAR *Tempstr);
void DebugMessage_ST(nuLONG lValue, nuCHAR *Tempstr);
void DebugMessage_TT(nuLONG lValue, nuCHAR *Tempstr);
void DebugMessage_UT(nuLONG lValue, nuCHAR *Tempstr);

#ifdef __cplusplus
}
#endif

#endif // File NuroOSApi.h
