
#include "libMemMgr_SH.h"
#include "NuroModuleApiStruct.h"
#include "NuroDefine.h"

/* System independent API, it must be a DLL
 */

//----- For Static Memory Manager -------------------------//
#define STATIC_MEMORY_SIZE					(800*1024)//(5000*1024)

static nuBYTE*		g_pStaticMemAddr_SH		= NULL;
static nuDWORD		g_nStaticMemSizeUsed_SH	= 0;
static nuDWORD		g_nStatciMemSize_SH		= 0;

static nuBOOL			g_bLockDataMM_SH			= nuFALSE;
static nuLONG			g_nDataMemUsedCount_SH		= 0;
static nuBOOL			g_bDataMemIsMoving_SH		= nuFALSE;

static nuBOOL			g_bMMWorking_SH			= nuFALSE;

//----- For Data Memory Manager --------------------------------//
#define DATA_MEMORY_SIZE_SH							(50*1024*1024)
#define DATA_MEMORY_MASS_MAX_NUM_SH					800
#define DATA_MEMORY_MASS_HEAD_SH						(8)//two DWORD
#define DATA_MEM_MASS_IDX_SH							0
#define DATA_MEM_MASS_SIZE_SH							1
#define DATA_MEMORY_MASS_MAX_SIZE_SH					0x00FFFFFF//the max size of each memory mass allocated

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static nuDWORD		g_pDataMemMassAddr_SH[DATA_MEMORY_MASS_MAX_NUM_SH];//Keep the address of data Memory;
static nuPBYTE		g_pDataMemStartAddr_SH		= NULL;
static nuPBYTE		g_pDataMemIdleAddr_SH		= NULL;
static nuDWORD		g_nDataMemSize_SH			= 0;
static nuDWORD		g_nDataMemUsed_SH			= 0;
static nuWORD		g_nDataMemMassesUsed_SH	= 0;

nuVOID BeginMoveDataMem_SH()
{
	if( !g_bDataMemIsMoving_SH )
	{
		while( g_nDataMemUsedCount_SH )
		{
			//nuShowString(nuTEXT("Collect Loop2"));
			nuSleep(TIMER_TIME_UNIT);
		}
	}
	g_bDataMemIsMoving_SH = nuTRUE;
}
nuVOID EndMoveDataMem_SH()
{
	g_bDataMemIsMoving_SH = nuFALSE;
}
nuBOOL CollectMemData_SH()
{
	if( g_pDataMemStartAddr_SH == NULL )
	{
		return  nuFALSE;
	}
	nuDWORD nSize = 0, nSizeDel = 0;
	nuPDWORD pMemHead;
	nuPBYTE pMemUsed, pMemEnd, pMemDel = g_pDataMemStartAddr_SH;
	//Find a mememory deleted
	while( pMemDel < g_pDataMemIdleAddr_SH )
	{
		pMemHead = (nuPDWORD)pMemDel;
		if( pMemHead[DATA_MEM_MASS_IDX_SH] >= DATA_MEMORY_MASS_MAX_NUM_SH )
		{
			break; 
		}
		pMemDel += pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH; 
	}
	if( pMemDel == g_pDataMemIdleAddr_SH )
	{
		return nuFALSE;
	}
	pMemEnd = pMemDel;
	while( pMemEnd < g_pDataMemIdleAddr_SH )
	{
		pMemHead = (nuPDWORD)pMemEnd;
		pMemUsed = pMemEnd + pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH;
		while( pMemUsed < g_pDataMemIdleAddr_SH )
		{
			pMemHead = (nuPDWORD)pMemUsed;
			if( pMemHead[DATA_MEM_MASS_IDX_SH] < DATA_MEMORY_MASS_MAX_NUM_SH )
			{
				break; 
			}
			pMemUsed += pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH;
		}
		if( pMemUsed == g_pDataMemIdleAddr_SH )
		{
			break;
		}
		pMemHead = (nuPDWORD)pMemUsed;
		pMemEnd = pMemUsed + pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH;
		while( pMemEnd < g_pDataMemIdleAddr_SH )
		{
			pMemHead = (nuPDWORD)pMemEnd;
			if( pMemHead[DATA_MEM_MASS_IDX_SH] >= DATA_MEMORY_MASS_MAX_NUM_SH )
			{
				break; 
			}
			pMemEnd += pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH; 
		}
		nSize		= pMemEnd - pMemUsed;
		nSizeDel	= pMemUsed - pMemDel;
		nuMemmove(pMemDel, pMemUsed, nSize);
		pMemUsed	= pMemDel + nSize;
		while( pMemDel < pMemUsed )
		{
			pMemHead = (nuPDWORD)pMemDel;
			g_pDataMemMassAddr_SH[pMemHead[DATA_MEM_MASS_IDX_SH]] -= nSizeDel;
			pMemDel += pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH;
		}
	}
	g_pDataMemIdleAddr_SH = pMemDel;
	return nuTRUE;
}
nuVOID ResetMemDataIdle_SH()
{
	nuPDWORD pMemHead;
	nuPBYTE pMemIdle = NULL, pMemDel = g_pDataMemStartAddr_SH;
	while( pMemDel < g_pDataMemIdleAddr_SH )
	{
		pMemHead = (nuPDWORD)pMemDel;
		if( pMemHead[DATA_MEM_MASS_IDX_SH] >= DATA_MEMORY_MASS_MAX_NUM_SH )
		{
			if( pMemIdle == NULL )
			{
				pMemIdle = pMemDel;
			}
		}
		else
		{
			pMemIdle = NULL;
		}
		pMemDel += pMemHead[DATA_MEM_MASS_SIZE_SH] + DATA_MEMORY_MASS_HEAD_SH; 
	}
	if( pMemIdle != NULL )
	{
		g_pDataMemIdleAddr_SH = pMemIdle;
	}
}
//---- Static Memory manager functions -------------------------//
MEMMGR_API nuPVOID LibMM_GetStaticMemMass_SH(nuDWORD dwSize)
{
	if( g_pStaticMemAddr_SH == NULL || (dwSize + g_nStaticMemSizeUsed_SH) > g_nStatciMemSize_SH )
	{
		return NULL;
	}
	nuPVOID lpTmp = g_pStaticMemAddr_SH + g_nStaticMemSizeUsed_SH;
	g_nStaticMemSizeUsed_SH += dwSize;
	nuMemset(lpTmp, 0, dwSize);
	return lpTmp;
}
MEMMGR_API nuBOOL LibMM_RelStaticMemMassAfter_SH(nuBYTE* pAddr)//free the memroy mass ater the pAddr
{
	if( g_pStaticMemAddr_SH == NULL || pAddr < g_pStaticMemAddr_SH || pAddr >= g_pStaticMemAddr_SH + g_nStaticMemSizeUsed_SH )
	{
		return nuFALSE;
	}
	g_nStaticMemSizeUsed_SH = (nuDWORD)(pAddr - g_pStaticMemAddr_SH);
	return nuTRUE;
}
MEMMGR_API nuPVOID LibMM_GetTmpStaticMemMass_SH(nuDWORD dwSize)
{
	if( g_pStaticMemAddr_SH == NULL || (dwSize + g_nStaticMemSizeUsed_SH) > g_nStatciMemSize_SH )
	{
		return NULL;
	}
	return (nuPVOID)(g_pStaticMemAddr_SH + g_nStaticMemSizeUsed_SH);
}

//------ Data memory manager functions -------------------------------//
MEMMGR_API nuPVOID LibMM_GetDataMemMass_SH(nuDWORD dwSize, nuPWORD pAddrIdx)
{
	if( pAddrIdx == NULL || dwSize == 0)
	{
		return NULL;
	}
	dwSize = ((dwSize + 3) / 4 ) * 4;
	if( g_pDataMemStartAddr_SH == NULL ||
		dwSize + DATA_MEMORY_MASS_HEAD_SH > g_nDataMemSize_SH || 
		g_nDataMemMassesUsed_SH == DATA_MEMORY_MASS_MAX_NUM_SH )
	{
		return NULL;
	}
	while( g_bMMWorking_SH )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking_SH = nuTRUE;
	if( ((nuDWORD)g_pDataMemIdleAddr_SH - (nuDWORD)g_pDataMemStartAddr_SH + dwSize + DATA_MEMORY_MASS_HEAD_SH ) > g_nDataMemSize_SH )
	{/*
		return NULL;
		 20090401*/
		BeginMoveDataMem_SH();
		CollectMemData_SH();
		EndMoveDataMem_SH();
		if( ((nuDWORD)g_pDataMemIdleAddr_SH - (nuDWORD)g_pDataMemStartAddr_SH + dwSize + DATA_MEMORY_MASS_HEAD_SH) > g_nDataMemSize_SH )
		{
			g_bMMWorking_SH = nuFALSE;
			return NULL;
		}
		
	}
	nuPVOID pRet = NULL;
	nuPDWORD pMassHead;
	for(nuWORD i = 1; i < DATA_MEMORY_MASS_MAX_NUM_SH; i++)
	{
		if( g_pDataMemMassAddr_SH[i] == 0 )
		{
			g_pDataMemMassAddr_SH[i] = (nuDWORD)g_pDataMemIdleAddr_SH + DATA_MEMORY_MASS_HEAD_SH;
			pMassHead = (nuPDWORD)g_pDataMemIdleAddr_SH;
			pMassHead[0] = i;
			pMassHead[1] = dwSize;
			g_nDataMemMassesUsed_SH++;
			g_pDataMemIdleAddr_SH += dwSize + DATA_MEMORY_MASS_HEAD_SH;
			g_nDataMemUsed_SH += dwSize + DATA_MEMORY_MASS_HEAD_SH;
			*pAddrIdx = i;
			nuMemset((nuVOID*)g_pDataMemMassAddr_SH[i], 0, dwSize);
			pRet = (nuPVOID)g_pDataMemMassAddr_SH[i];
			break;
		}
	}
	g_bMMWorking_SH  = nuFALSE;
	return pRet;
}
 
MEMMGR_API nuBOOL LibMM_RelDataMemMass_SH(nuPWORD pAddrIdx)
{
	while( g_bMMWorking_SH )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking_SH = nuTRUE;
	if( g_pDataMemStartAddr_SH == NULL || 
		pAddrIdx == NULL || 
		*pAddrIdx >= DATA_MEMORY_MASS_MAX_NUM_SH || 
		g_pDataMemMassAddr_SH[*pAddrIdx] == 0 )
	{
		g_bMMWorking_SH = nuFALSE;
		return nuFALSE;
	}
	//////////////////////////////////////

	nuPBYTE pAddr = (nuPBYTE)(g_pDataMemMassAddr_SH[*pAddrIdx] - DATA_MEMORY_MASS_HEAD_SH);
	nuPDWORD pMassHead = (nuPDWORD)pAddr;
	nuDWORD dwSize = pMassHead[DATA_MEM_MASS_SIZE_SH];
	dwSize += DATA_MEMORY_MASS_HEAD_SH;
	g_nDataMemUsed_SH -= dwSize;
	//
	pMassHead[DATA_MEM_MASS_IDX_SH]  = DATA_MEMORY_MASS_MAX_NUM_SH;
	g_pDataMemMassAddr_SH[*pAddrIdx] = NULL;//delete the mass
	*pAddrIdx = -1;
	g_nDataMemMassesUsed_SH --;
	//Don't collect memory
	if( pAddr + dwSize == g_pDataMemIdleAddr_SH )
	{
		g_pDataMemIdleAddr_SH -= dwSize;
	}
	/*如果后面的内存都已删除了，重新设置g_pDataMemIdleAddr_SH这个指针，使可用内存数量增加*/
//	ResetMemDataIdle(); 确定一个时间点放开这个动作
	//
	g_bMMWorking_SH = nuFALSE;
	return nuTRUE;
}

MEMMGR_API nuBOOL	LibMM_CollectDataMem_SH(nuBYTE nType)
{
	if( nType == 2 )
	{
		nuTCHAR tsTest[200], tsNum[20];
		nuTcscpy(tsTest, nuTEXT("MemCollect"));
		nuItot((nuINT)g_pDataMemIdleAddr_SH, tsNum, 10);
		nuTcscat(tsTest, tsNum);
		
		nuTcscat(tsTest, nuTEXT("MemDataUsed =") );
		nuItot(g_nDataMemUsed_SH, tsNum, 10);
		nuTcscat(tsTest, tsNum);
		//nuShowString(tsTest);
	}
	while( g_bMMWorking_SH )
	{
		//nuShowString(nuTEXT("Collect Loop"));
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking_SH = nuTRUE;
	BeginMoveDataMem_SH();
	CollectMemData_SH();
	EndMoveDataMem_SH();
	g_bMMWorking_SH = nuFALSE;
	return nuTRUE;
}
MEMMGR_API nuPVOID LibMM_GetDataMassAddr_SH(nuWORD nAddrIdx)
{
	if( g_pDataMemStartAddr_SH == NULL || nAddrIdx >= DATA_MEMORY_MASS_MAX_NUM_SH )
	{
		return NULL;
	}
	return (nuPVOID)g_pDataMemMassAddr_SH[nAddrIdx];
}
MEMMGR_API nuPVOID* LibMM_AllocMem_SH(nuDWORD dwSize)
{
	if( !dwSize || 
		g_pDataMemStartAddr_SH == NULL || 
		g_nDataMemMassesUsed_SH == DATA_MEMORY_MASS_MAX_NUM_SH || 
		dwSize + DATA_MEMORY_MASS_HEAD_SH > g_nDataMemSize_SH)
	{
		return NULL;
	}
	dwSize = ((dwSize + 3) / 4 ) * 4;
	if( ((nuDWORD)g_pDataMemIdleAddr_SH - (nuDWORD)g_pDataMemStartAddr_SH + dwSize + DATA_MEMORY_MASS_HEAD_SH ) > g_nDataMemSize_SH )
	{
		/*
		return NULL;
		 20090401*/
		BeginMoveDataMem_SH();
		CollectMemData_SH();
		EndMoveDataMem_SH();
		if( ((nuDWORD)g_pDataMemIdleAddr_SH - (nuDWORD)g_pDataMemStartAddr_SH + dwSize + DATA_MEMORY_MASS_HEAD_SH) > g_nDataMemSize_SH )
		{
			return NULL;
		}
		//
	}
	nuPDWORD pMassHead;
	for(nuWORD i = 1; i < DATA_MEMORY_MASS_MAX_NUM_SH; i++)
	{
		if( g_pDataMemMassAddr_SH[i] == 0 )
		{
			g_pDataMemMassAddr_SH[i] = (nuDWORD)g_pDataMemIdleAddr_SH + DATA_MEMORY_MASS_HEAD_SH;
			pMassHead = (nuPDWORD)g_pDataMemIdleAddr_SH;
			pMassHead[0] = i;
			pMassHead[1] = dwSize;
			g_nDataMemMassesUsed_SH++;
			g_pDataMemIdleAddr_SH += dwSize + DATA_MEMORY_MASS_HEAD_SH;
			g_nDataMemUsed_SH += dwSize + DATA_MEMORY_MASS_HEAD_SH;
			nuMemset((nuVOID*)g_pDataMemMassAddr_SH[i], 0, dwSize);
			return (nuPVOID*)(&g_pDataMemMassAddr_SH[i]);
		}
	}
	return NULL;
}
MEMMGR_API nuBOOL	LibMM_FreeMem_SH(nuPVOID* ppAddr)
{
	if( g_pDataMemStartAddr_SH == NULL		|| 
		ppAddr == NULL					|| 
		*ppAddr < g_pDataMemStartAddr_SH	|| 
		*ppAddr > g_pDataMemStartAddr_SH + g_nDataMemSize_SH )
	{
		return nuFALSE;
	}
	nuPBYTE pAddr = (nuPBYTE)((nuLONG)(*ppAddr) - DATA_MEMORY_MASS_HEAD_SH);
	nuPDWORD pMassHead = (nuPDWORD)pAddr;
	nuDWORD dwSize = pMassHead[1];
	dwSize += DATA_MEMORY_MASS_HEAD_SH;
	g_nDataMemUsed_SH	-= dwSize;
	//Added on 20081226
	if( pAddr + dwSize == g_pDataMemIdleAddr_SH )
	{
		g_pDataMemIdleAddr_SH -= dwSize;
	}
	pMassHead[DATA_MEM_MASS_IDX_SH]  = DATA_MEMORY_MASS_MAX_NUM_SH;
	*ppAddr = NULL;//delete the mass
	g_nDataMemMassesUsed_SH --;
	/* 20081226
	//Copy the memory
	nuPBYTE pTmp = pAddr + dwSize;
	size_t nCount = (size_t)(g_pDataMemIdleAddr_SH - pTmp);
	
	nuMemmove(pAddr, pTmp, nCount);
	//
	*ppAddr = NULL;//delete the mass
	g_pDataMemIdleAddr_SH -= dwSize;
	g_nDataMemMassesUsed_SH --;
	//reset the addressed of massed 
	
	pTmp = pAddr;
	while( pTmp < g_pDataMemIdleAddr_SH )
	{
		pMassHead = (nuPDWORD)pTmp;
		g_pDataMemMassAddr_SH[pMassHead[0]] -= dwSize;
		pTmp += (pMassHead[1] + DATA_MEMORY_MASS_HEAD_SH);
	}
	*/
	return nuTRUE;
}

//Initialize and free the library.
MEMMGR_API nuBOOL LibMM_InitMemMgr_SH(nuPVOID lpOutMM)
{
	nuLONG nMemSize, nStaticMem;
	if( !nuReadConfigValue((nuCHAR *)"SHMEM", &nMemSize) )
	{
		nMemSize = DATA_MEMORY_SIZE_SH;
	}
	else
	{
		nMemSize *= 1024 * 1024;
	}
	nStaticMem = 32;

	if( !nuGetMemory(&g_pStaticMemAddr_SH, nStaticMem, &g_pDataMemStartAddr_SH, nMemSize) )
	{
		return nuFALSE;
	}
	g_nStatciMemSize_SH = nStaticMem;
	//
	g_nStaticMemSizeUsed_SH	= 0;
	g_pDataMemIdleAddr_SH		= g_pDataMemStartAddr_SH;
	g_nDataMemSize_SH			= nMemSize;
	g_nDataMemMassesUsed_SH	= 0;
	nuMemset(g_pDataMemMassAddr_SH, 0, sizeof(g_pDataMemMassAddr_SH));
	if( lpOutMM != NULL )
	{
		PMEMMGRAPI_SH pMMApi = (PMEMMGRAPI_SH)lpOutMM;
		pMMApi->MM_AllocMem				= LibMM_AllocMem_SH;
		pMMApi->MM_FreeMem				= LibMM_FreeMem_SH;
		pMMApi->MM_GetDataMassAddr		= LibMM_GetDataMassAddr_SH;
		pMMApi->MM_GetDataMemMass		= LibMM_GetDataMemMass_SH;
		pMMApi->MM_GetStaticMemMass		= LibMM_GetStaticMemMass_SH;
		pMMApi->MM_GetTmpStaticMemMass	= LibMM_GetTmpStaticMemMass_SH;
		pMMApi->MM_RelDataMemMass		= LibMM_RelDataMemMass_SH;
		pMMApi->MM_RelStaticMemMassAfter= LibMM_RelStaticMemMassAfter_SH;
		pMMApi->MM_CollectDataMem		= LibMM_CollectDataMem_SH;
		pMMApi->MM_LockDataMemMove		= LibMM_LockDataMemMove_SH;
		pMMApi->MM_FreeDataMemMove		= LibMM_FreeDataMemMove_SH;
		pMMApi->MM_GetNowMemoryUsed		= LibMM_GetNowMemoryUsed_SH;//added by daniel
	
	}
	g_bLockDataMM_SH = nuFALSE;
	return nuTRUE;
}
MEMMGR_API nuVOID LibMM_FreeMemMgr_SH()
{
	if( g_pStaticMemAddr_SH != NULL || g_pDataMemStartAddr_SH != NULL )
	{
		nuRelMemory(g_pStaticMemAddr_SH, g_pDataMemStartAddr_SH);
		g_pStaticMemAddr_SH	= NULL;
		g_pDataMemStartAddr_SH	= NULL;
	}
}


MEMMGR_API nuPVOID	LibMM_NewMem_SH(nuDWORD dwSize)
{
	return NULL;//(new nuBYTE[dwSize]);
}
MEMMGR_API nuVOID		LibMM_DelMem_SH(nuPVOID *pMem)
{
	delete []pMem;
}
MEMMGR_API nuVOID		LibMM_LockDataMemMove_SH()
{
	while( g_bDataMemIsMoving_SH )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	++g_nDataMemUsedCount_SH;
}
MEMMGR_API nuVOID		LibMM_FreeDataMemMove_SH()
{
	--g_nDataMemUsedCount_SH;
}

MEMMGR_API nuVOID    LibMM_GetNowMemoryUsed_SH(nuDWORD *MemoryUsedCount)//added by daniel
{
	*MemoryUsedCount = g_nDataMemUsed_SH;
}
