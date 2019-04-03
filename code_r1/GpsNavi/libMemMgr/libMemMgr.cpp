
#include "libMemMgr.h"
#include "NuroModuleApiStruct.h"
#include "NuroDefine.h"

/* System independent API, it must be a DLL
 */

//----- For Static Memory Manager -------------------------//
#define STATIC_MEMORY_SIZE					(800*1024)//(5000*1024)

static nuBYTE*		g_pStaticMemAddr		= NULL;
static nuDWORD		g_nStaticMemSizeUsed	= 0;
static nuDWORD		g_nStatciMemSize		= 0;

static nuBOOL			g_bLockDataMM			= nuFALSE;
static nuLONG			g_nDataMemUsedCount		= 0;
static nuBOOL			g_bDataMemIsMoving		= nuFALSE;

static nuBOOL			g_bMMWorking			= nuFALSE;

//----- For Data Memory Manager --------------------------------//
#define DATA_MEMORY_SIZE							(12*1024*1024)
#define DATA_MEMORY_MASS_MAX_NUM					800
#define DATA_MEMORY_MASS_HEAD						(8)//two DWORD
#define DATA_MEM_MASS_IDX							0
#define DATA_MEM_MASS_SIZE							1
#define DATA_MEMORY_MASS_MAX_SIZE					0x00FFFFFF//the max size of each memory mass allocated

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static nuDWORD		g_pDataMemMassAddr[DATA_MEMORY_MASS_MAX_NUM];//Keep the address of data Memory;
static nuPBYTE		g_pDataMemStartAddr		= NULL;
static nuPBYTE		g_pDataMemIdleAddr		= NULL;
static nuDWORD		g_nDataMemSize			= 0;
static nuDWORD		g_nDataMemUsed			= 0;
static nuWORD		g_nDataMemMassesUsed	= 0;

nuVOID BeginMoveDataMem()
{
	if( !g_bDataMemIsMoving )
	{
		while( g_nDataMemUsedCount )
		{
			//nuShowString(nuTEXT("Collect Loop2"));
			nuSleep(TIMER_TIME_UNIT);
		}
	}
	g_bDataMemIsMoving = nuTRUE;
}
nuVOID EndMoveDataMem()
{
	g_bDataMemIsMoving = nuFALSE;
}
nuBOOL CollectMemData()
{
	if( g_pDataMemStartAddr == NULL )
	{
		return  nuFALSE;
	}
	nuDWORD nSize = 0, nSizeDel = 0;
	nuPDWORD pMemHead;
	nuPBYTE pMemUsed, pMemEnd, pMemDel = g_pDataMemStartAddr;
	//Find a mememory deleted
	while( pMemDel < g_pDataMemIdleAddr )
	{
		pMemHead = (nuPDWORD)pMemDel;
		if( pMemHead[DATA_MEM_MASS_IDX] >= DATA_MEMORY_MASS_MAX_NUM )
		{
			break; 
		}
		pMemDel += pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD; 
	}
	if( pMemDel == g_pDataMemIdleAddr )
	{
		return nuFALSE;
	}
	pMemEnd = pMemDel;
	while( pMemEnd < g_pDataMemIdleAddr )
	{
		pMemHead = (nuPDWORD)pMemEnd;
		pMemUsed = pMemEnd + pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD;
		while( pMemUsed < g_pDataMemIdleAddr )
		{
			pMemHead = (nuPDWORD)pMemUsed;
			if( pMemHead[DATA_MEM_MASS_IDX] < DATA_MEMORY_MASS_MAX_NUM )
			{
				break; 
			}
			pMemUsed += pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD;
		}
		if( pMemUsed == g_pDataMemIdleAddr )
		{
			break;
		}
		pMemHead = (nuPDWORD)pMemUsed;
		pMemEnd = pMemUsed + pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD;
		while( pMemEnd < g_pDataMemIdleAddr )
		{
			pMemHead = (nuPDWORD)pMemEnd;
			if( pMemHead[DATA_MEM_MASS_IDX] >= DATA_MEMORY_MASS_MAX_NUM )
			{
				break; 
			}
			pMemEnd += pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD; 
		}
		nSize		= pMemEnd - pMemUsed;
		nSizeDel	= pMemUsed - pMemDel;
		nuMemmove(pMemDel, pMemUsed, nSize);
		pMemUsed	= pMemDel + nSize;
		while( pMemDel < pMemUsed )
		{
			pMemHead = (nuPDWORD)pMemDel;
			g_pDataMemMassAddr[pMemHead[DATA_MEM_MASS_IDX]] -= nSizeDel;
			pMemDel += pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD;
		}
	}
	g_pDataMemIdleAddr = pMemDel;
	return nuTRUE;
}
nuVOID ResetMemDataIdle()
{
	nuPDWORD pMemHead;
	nuPBYTE pMemIdle = NULL, pMemDel = g_pDataMemStartAddr;
	while( pMemDel < g_pDataMemIdleAddr )
	{
		pMemHead = (nuPDWORD)pMemDel;
		if( pMemHead[DATA_MEM_MASS_IDX] >= DATA_MEMORY_MASS_MAX_NUM )
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
		pMemDel += pMemHead[DATA_MEM_MASS_SIZE] + DATA_MEMORY_MASS_HEAD; 
	}
	if( pMemIdle != NULL )
	{
		g_pDataMemIdleAddr = pMemIdle;
	}
}
//---- Static Memory manager functions -------------------------//
MEMMGR_API nuPVOID LibMM_GetStaticMemMass(nuDWORD dwSize)
{
	if( g_pStaticMemAddr == NULL || (dwSize + g_nStaticMemSizeUsed) > g_nStatciMemSize )
	{
		return NULL;
	}
	nuPVOID lpTmp = g_pStaticMemAddr + g_nStaticMemSizeUsed;
	g_nStaticMemSizeUsed += dwSize;
	nuMemset(lpTmp, 0, dwSize);
	return lpTmp;
}
MEMMGR_API nuBOOL LibMM_RelStaticMemMassAfter(nuBYTE* pAddr)//free the memroy mass ater the pAddr
{
	if( g_pStaticMemAddr == NULL || pAddr < g_pStaticMemAddr || pAddr >= g_pStaticMemAddr + g_nStaticMemSizeUsed )
	{
		return nuFALSE;
	}
	g_nStaticMemSizeUsed = (nuDWORD)(pAddr - g_pStaticMemAddr);
	return nuTRUE;
}
MEMMGR_API nuPVOID LibMM_GetTmpStaticMemMass(nuDWORD dwSize)
{
	if( g_pStaticMemAddr == NULL || (dwSize + g_nStaticMemSizeUsed) > g_nStatciMemSize )
	{
		return NULL;
	}
	return (nuPVOID)(g_pStaticMemAddr + g_nStaticMemSizeUsed);
}

//------ Data memory manager functions -------------------------------//
MEMMGR_API nuPVOID LibMM_GetDataMemMass(nuDWORD dwSize, nuPWORD pAddrIdx)
{
	if( pAddrIdx == NULL || dwSize == 0)
	{
		return NULL;
	}
	dwSize = ((dwSize + 3) / 4 ) * 4;
	if( g_pDataMemStartAddr == NULL ||
		dwSize + DATA_MEMORY_MASS_HEAD > g_nDataMemSize || 
		g_nDataMemMassesUsed == DATA_MEMORY_MASS_MAX_NUM )
	{
		return NULL;
	}
	while( g_bMMWorking )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking = nuTRUE;
	if( ((nuDWORD)g_pDataMemIdleAddr - (nuDWORD)g_pDataMemStartAddr + dwSize + DATA_MEMORY_MASS_HEAD ) > g_nDataMemSize )
	{/*
		return NULL;
		 20090401*/
		BeginMoveDataMem();
		CollectMemData();
		EndMoveDataMem();
		if( ((nuDWORD)g_pDataMemIdleAddr - (nuDWORD)g_pDataMemStartAddr + dwSize + DATA_MEMORY_MASS_HEAD) > g_nDataMemSize )
		{
			g_bMMWorking = nuFALSE;
			return NULL;
		}
		
	}
	nuPVOID pRet = NULL;
	nuPDWORD pMassHead;
	for(nuWORD i = 1; i < DATA_MEMORY_MASS_MAX_NUM; i++)
	{
		if( g_pDataMemMassAddr[i] == 0 )
		{
			g_pDataMemMassAddr[i] = (nuDWORD)g_pDataMemIdleAddr + DATA_MEMORY_MASS_HEAD;
			pMassHead = (nuPDWORD)g_pDataMemIdleAddr;
			pMassHead[0] = i;
			pMassHead[1] = dwSize;
			g_nDataMemMassesUsed++;
			g_pDataMemIdleAddr += dwSize + DATA_MEMORY_MASS_HEAD;
			g_nDataMemUsed += dwSize + DATA_MEMORY_MASS_HEAD;
			*pAddrIdx = i;
			nuMemset((nuVOID*)g_pDataMemMassAddr[i], 0, dwSize);
			pRet = (nuPVOID)g_pDataMemMassAddr[i];
			break;
		}
	}
	g_bMMWorking  = nuFALSE;
	return pRet;
}
 
MEMMGR_API nuBOOL LibMM_RelDataMemMass(nuPWORD pAddrIdx)
{
	while( g_bMMWorking )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking = nuTRUE;
	if( g_pDataMemStartAddr == NULL || 
		pAddrIdx == NULL || 
		*pAddrIdx >= DATA_MEMORY_MASS_MAX_NUM || 
		g_pDataMemMassAddr[*pAddrIdx] == 0 )
	{
		g_bMMWorking = nuFALSE;
		return nuFALSE;
	}
	//////////////////////////////////////

	nuPBYTE pAddr = (nuPBYTE)(g_pDataMemMassAddr[*pAddrIdx] - DATA_MEMORY_MASS_HEAD);
	nuPDWORD pMassHead = (nuPDWORD)pAddr;
	nuDWORD dwSize = pMassHead[DATA_MEM_MASS_SIZE];
	dwSize += DATA_MEMORY_MASS_HEAD;
	g_nDataMemUsed -= dwSize;
	//
	pMassHead[DATA_MEM_MASS_IDX]  = DATA_MEMORY_MASS_MAX_NUM;
	g_pDataMemMassAddr[*pAddrIdx] = NULL;//delete the mass
	*pAddrIdx = -1;
	g_nDataMemMassesUsed --;
	//Don't collect memory
	if( pAddr + dwSize == g_pDataMemIdleAddr )
	{
		g_pDataMemIdleAddr -= dwSize;
	}
	/*如果后面的内存都已删除了，重新设置g_pDataMemIdleAddr这个指针，使可用内存数量增加*/
//	ResetMemDataIdle(); 确定一个时间点放开这个动作
	//
	g_bMMWorking = nuFALSE;
	return nuTRUE;
}

MEMMGR_API nuBOOL	LibMM_CollectDataMem(nuBYTE nType)
{
	if( nType == 2 )
	{
		nuTCHAR tsTest[200], tsNum[20];
		nuTcscpy(tsTest, nuTEXT("MemCollect"));
		nuItot((nuINT)g_pDataMemIdleAddr, tsNum, 10);
		nuTcscat(tsTest, tsNum);
		
		nuTcscat(tsTest, nuTEXT("MemDataUsed =") );
		nuItot(g_nDataMemUsed, tsNum, 10);
		nuTcscat(tsTest, tsNum);
		//nuShowString(tsTest);
	}
	while( g_bMMWorking )
	{
		//nuShowString(nuTEXT("Collect Loop"));
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bMMWorking = nuTRUE;
	BeginMoveDataMem();
	CollectMemData();
	EndMoveDataMem();
	g_bMMWorking = nuFALSE;
	return nuTRUE;
}
MEMMGR_API nuPVOID LibMM_GetDataMassAddr(nuWORD nAddrIdx)
{
	if( g_pDataMemStartAddr == NULL || nAddrIdx >= DATA_MEMORY_MASS_MAX_NUM )
	{
		return NULL;
	}
	return (nuPVOID)g_pDataMemMassAddr[nAddrIdx];
}
MEMMGR_API nuPVOID* LibMM_AllocMem(nuDWORD dwSize)
{
	if( !dwSize || 
		g_pDataMemStartAddr == NULL || 
		g_nDataMemMassesUsed == DATA_MEMORY_MASS_MAX_NUM || 
		dwSize + DATA_MEMORY_MASS_HEAD > g_nDataMemSize)
	{
		return NULL;
	}
	dwSize = ((dwSize + 3) / 4 ) * 4;
	if( ((nuDWORD)g_pDataMemIdleAddr - (nuDWORD)g_pDataMemStartAddr + dwSize + DATA_MEMORY_MASS_HEAD ) > g_nDataMemSize )
	{
		/*
		return NULL;
		 20090401*/
		BeginMoveDataMem();
		CollectMemData();
		EndMoveDataMem();
		if( ((nuDWORD)g_pDataMemIdleAddr - (nuDWORD)g_pDataMemStartAddr + dwSize + DATA_MEMORY_MASS_HEAD) > g_nDataMemSize )
		{
			return NULL;
		}
		//
	}
	nuPDWORD pMassHead;
	for(nuWORD i = 1; i < DATA_MEMORY_MASS_MAX_NUM; i++)
	{
		if( g_pDataMemMassAddr[i] == 0 )
		{
			g_pDataMemMassAddr[i] = (nuDWORD)g_pDataMemIdleAddr + DATA_MEMORY_MASS_HEAD;
			pMassHead = (nuPDWORD)g_pDataMemIdleAddr;
			pMassHead[0] = i;
			pMassHead[1] = dwSize;
			g_nDataMemMassesUsed++;
			g_pDataMemIdleAddr += dwSize + DATA_MEMORY_MASS_HEAD;
			g_nDataMemUsed += dwSize + DATA_MEMORY_MASS_HEAD;
			nuMemset((nuVOID*)g_pDataMemMassAddr[i], 0, dwSize);
			return (nuPVOID*)(&g_pDataMemMassAddr[i]);
		}
	}
	return NULL;
}
MEMMGR_API nuBOOL	LibMM_FreeMem(nuPVOID* ppAddr)
{
	if( g_pDataMemStartAddr == NULL		|| 
		ppAddr == NULL					|| 
		*ppAddr < g_pDataMemStartAddr	|| 
		*ppAddr > g_pDataMemStartAddr + g_nDataMemSize )
	{
		return nuFALSE;
	}
	nuPBYTE pAddr = (nuPBYTE)((nuLONG)(*ppAddr) - DATA_MEMORY_MASS_HEAD);
	nuPDWORD pMassHead = (nuPDWORD)pAddr;
	nuDWORD dwSize = pMassHead[1];
	dwSize += DATA_MEMORY_MASS_HEAD;
	g_nDataMemUsed	-= dwSize;
	//Added on 20081226
	if( pAddr + dwSize == g_pDataMemIdleAddr )
	{
		g_pDataMemIdleAddr -= dwSize;
	}
	pMassHead[DATA_MEM_MASS_IDX]  = DATA_MEMORY_MASS_MAX_NUM;
	*ppAddr = NULL;//delete the mass
	g_nDataMemMassesUsed --;
	/* 20081226
	//Copy the memory
	nuPBYTE pTmp = pAddr + dwSize;
	size_t nCount = (size_t)(g_pDataMemIdleAddr - pTmp);
	
	nuMemmove(pAddr, pTmp, nCount);
	//
	*ppAddr = NULL;//delete the mass
	g_pDataMemIdleAddr -= dwSize;
	g_nDataMemMassesUsed --;
	//reset the addressed of massed 
	
	pTmp = pAddr;
	while( pTmp < g_pDataMemIdleAddr )
	{
		pMassHead = (nuPDWORD)pTmp;
		g_pDataMemMassAddr[pMassHead[0]] -= dwSize;
		pTmp += (pMassHead[1] + DATA_MEMORY_MASS_HEAD);
	}
	*/
	return nuTRUE;
}

//Initialize and free the library.
MEMMGR_API nuBOOL LibMM_InitMemMgr(nuPVOID lpOutMM)
{
	nuLONG nMemSize, nStaticMem;
	if( !nuReadConfigValue((nuCHAR *)"MEMORYSIZE", &nMemSize) )
	{
		nMemSize = DATA_MEMORY_SIZE;
	}
	else
	{
		nMemSize *= 1024 * 1024;
	}
	if( !nuReadConfigValue((nuCHAR *)"STATICMEMSIZE", &nStaticMem) )
	{
		nStaticMem = STATIC_MEMORY_SIZE;
	}
	else
	{
		nStaticMem *= 1024;
	}
	if( !nuGetMemory(&g_pStaticMemAddr, nStaticMem, &g_pDataMemStartAddr, nMemSize) )
	{
		return nuFALSE;
	}
	g_nStatciMemSize = nStaticMem;
	//
	g_nStaticMemSizeUsed	= 0;
	g_pDataMemIdleAddr		= g_pDataMemStartAddr;
	g_nDataMemSize			= nMemSize;
	g_nDataMemMassesUsed	= 0;
	nuMemset(g_pDataMemMassAddr, 0, sizeof(g_pDataMemMassAddr));
	if( lpOutMM != NULL )
	{
		PMEMMGRAPI pMMApi = (PMEMMGRAPI)lpOutMM;
		pMMApi->MM_AllocMem				= LibMM_AllocMem;
		pMMApi->MM_FreeMem				= LibMM_FreeMem;
		pMMApi->MM_GetDataMassAddr		= LibMM_GetDataMassAddr;
		pMMApi->MM_GetDataMemMass		= LibMM_GetDataMemMass;
		pMMApi->MM_GetStaticMemMass		= LibMM_GetStaticMemMass;
		pMMApi->MM_GetTmpStaticMemMass	= LibMM_GetTmpStaticMemMass;
		pMMApi->MM_RelDataMemMass		= LibMM_RelDataMemMass;
		pMMApi->MM_RelStaticMemMassAfter= LibMM_RelStaticMemMassAfter;
		pMMApi->MM_CollectDataMem		= LibMM_CollectDataMem;
		pMMApi->MM_LockDataMemMove		= LibMM_LockDataMemMove;
		pMMApi->MM_FreeDataMemMove		= LibMM_FreeDataMemMove;
		pMMApi->MM_GetNowMemoryUsed		= LibMM_GetNowMemoryUsed;//added by daniel
	}
	g_bLockDataMM = nuFALSE;
	return nuTRUE;
}
MEMMGR_API nuVOID LibMM_FreeMemMgr()
{
	if( g_pStaticMemAddr != NULL || g_pDataMemStartAddr != NULL )
	{
		nuRelMemory(g_pStaticMemAddr, g_pDataMemStartAddr);
		g_pStaticMemAddr	= NULL;
		g_pDataMemStartAddr	= NULL;
	}
}


MEMMGR_API nuPVOID	LibMM_NewMem(nuDWORD dwSize)
{
	return NULL;//(new nuBYTE[dwSize]);
}
MEMMGR_API nuVOID		LibMM_DelMem(nuPVOID *pMem)
{
	delete []pMem;
}
/*
MEMMGR_API nuVOID		LibMM_LockDataMem()
{
	while( g_bLockDataMM )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	g_bLockDataMM	= nuTRUE;
}
MEMMGR_API nuVOID		LibMM_FreeDataMem()
{
	g_bLockDataMM	= nuFALSE;
}
*/
MEMMGR_API nuVOID		LibMM_LockDataMemMove()
{
	while( g_bDataMemIsMoving )
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	++g_nDataMemUsedCount;
}
MEMMGR_API nuVOID		LibMM_FreeDataMemMove()
{
	--g_nDataMemUsedCount;
}

MEMMGR_API nuVOID    LibMM_GetNowMemoryUsed(nuDWORD *MemoryUsedCount)//added by daniel
{
	*MemoryUsedCount = g_nDataMemUsed;
}
