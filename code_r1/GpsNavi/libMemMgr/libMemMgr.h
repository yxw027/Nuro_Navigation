#ifndef __NURO_MEMORY_MANAGER_20080128
#define __NURO_MEMORY_MANAGER_20080128

#include "NuroDefine.h"

#ifdef _USRDLL
#ifdef MEMMGR_EXPORTS
#define MEMMGR_API extern "C" __declspec(dllexport)
#else
#define MEMMGR_API extern "C" __declspec(dllimport)
#endif
#else
#define MEMMGR_API extern "C" 
#endif

MEMMGR_API nuBOOL  LibMM_InitMemMgr(nuPVOID lpOutMM);
MEMMGR_API nuVOID  LibMM_FreeMemMgr();

MEMMGR_API nuPVOID LibMM_GetStaticMemMass(nuDWORD dwSize);
MEMMGR_API nuBOOL  LibMM_RelStaticMemMassAfter(nuBYTE* pAddr);//free the memroy mass ater the pAddr
MEMMGR_API nuPVOID LibMM_GetTmpStaticMemMass(nuDWORD dwSize);

MEMMGR_API nuPVOID LibMM_GetDataMemMass(nuDWORD dwSize, nuPWORD pAddrIdx);
MEMMGR_API nuBOOL  LibMM_RelDataMemMass(nuPWORD pAddrIdx);
MEMMGR_API nuPVOID LibMM_GetDataMassAddr(nuWORD nAddrIdx);
MEMMGR_API nuBOOL  LibMM_CollectDataMem(nuBYTE nType);

MEMMGR_API nuPVOID* LibMM_AllocMem(nuDWORD dwSize);
MEMMGR_API nuBOOL	LibMM_FreeMem(nuPVOID* ppAddr);

MEMMGR_API nuPVOID	LibMM_NewMem(nuDWORD dwSize);
MEMMGR_API nuVOID	LibMM_DelMem(nuPVOID *pMem);

//MEMMGR_API nuVOID		LibMM_LockDataMem();
//MEMMGR_API nuVOID		LibMM_FreeDataMem();

MEMMGR_API nuVOID  LibMM_LockDataMemMove();
MEMMGR_API nuVOID  LibMM_FreeDataMemMove();

MEMMGR_API nuVOID  LibMM_GetNowMemoryUsed(nuDWORD *MemoryUsedCount);//added by daniel

#endif
