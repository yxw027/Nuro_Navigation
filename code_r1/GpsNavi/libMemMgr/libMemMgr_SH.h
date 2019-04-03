#ifndef __NURO_MEMORY_MANAGER_20160627
#define __NURO_MEMORY_MANAGER_20160627

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

MEMMGR_API nuBOOL  LibMM_InitMemMgr_SH(nuPVOID lpOutMM);
MEMMGR_API nuVOID  LibMM_FreeMemMgr_SH();

MEMMGR_API nuPVOID LibMM_GetStaticMemMass_SH(nuDWORD dwSize);
MEMMGR_API nuBOOL  LibMM_RelStaticMemMassAfter_SH(nuBYTE* pAddr);//free the memroy mass ater the pAddr
MEMMGR_API nuPVOID LibMM_GetTmpStaticMemMass_SH(nuDWORD dwSize);

MEMMGR_API nuPVOID LibMM_GetDataMemMass_SH(nuDWORD dwSize, nuPWORD pAddrIdx);
MEMMGR_API nuBOOL  LibMM_RelDataMemMass_SH(nuPWORD pAddrIdx);
MEMMGR_API nuPVOID LibMM_GetDataMassAddr_SH(nuWORD nAddrIdx);
MEMMGR_API nuBOOL  LibMM_CollectDataMem_SH(nuBYTE nType);

MEMMGR_API nuPVOID* LibMM_AllocMem_SH(nuDWORD dwSize);
MEMMGR_API nuBOOL	LibMM_FreeMem_SH(nuPVOID* ppAddr);

MEMMGR_API nuPVOID	LibMM_NewMem_SH(nuDWORD dwSize);
MEMMGR_API nuVOID	LibMM_DelMem_SH(nuPVOID *pMem);

//MEMMGR_API nuVOID		LibMM_LockDataMem();
//MEMMGR_API nuVOID		LibMM_FreeDataMem();

MEMMGR_API nuVOID  LibMM_LockDataMemMove_SH();
MEMMGR_API nuVOID  LibMM_FreeDataMemMove_SH();

MEMMGR_API nuVOID  LibMM_GetNowMemoryUsed_SH(nuDWORD *MemoryUsedCount);//added by daniel

#endif
