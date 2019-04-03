
#include "libInNavi.h"
#include "NuroModuleApiStruct.h"
#include "NaviData.h"
//Test
//#include "LoadFS.h"
//#include "LoadMT.h"

PFILESYSAPI		g_pInFsApi = NULL;
PMEMMGRAPI		g_pInMmApi = NULL;
PMATHTOOLAPI	g_pInMtApi = NULL;
PROUTEAPI		g_pInRTApi   = NULL;

class CNaviData* g_pNaviData = NULL;

#define nuAPIENTRY      APIENTRY
#define NURO_DLLMAIN    DLLMAIN

#define NURO_DLL_PROCESS_ATTACH	 DLL_PROCESS_ATTACH 
#define NURO_DLL_THREAD_ATTACH	 DLL_THREAD_ATTACH		
#define NURO_DLL_THREAD_DETACH	 DLL_THREAD_DETACH
#define NURO_DLL_PROCESS_DETACH	 DLL_PROCESS_DETACH

#ifdef NURO_OS_WINDOWS
#ifdef _USRDLL
nuBOOL nuAPIENTRY NURO_DLLMAIN(	nuHANDLE hModule, 
								nuDWORD  ul_reason_for_call, 
								nuPVOID lpReserved
							  )
{
    switch (ul_reason_for_call)
	{
		case NURO_DLL_PROCESS_ATTACH:
			break;
		case NURO_DLL_THREAD_ATTACH:
			break;
		case NURO_DLL_THREAD_DETACH:
			break;
		case NURO_DLL_PROCESS_DETACH:
			LibIN_FreeInNavi();
			break;
    }
    return TRUE;
}
#endif
#endif

INNAVI_API nuBOOL LibIN_InitInNavi(PAPISTRUCTADDR pApiAddr)
{
	g_pNaviData = new CNaviData();
	if( g_pNaviData == NULL )
	{
		return nuFALSE;
	}
	g_pInMmApi = pApiAddr->pMmApi;
	g_pInFsApi = pApiAddr->pFsApi;
	g_pInMtApi = pApiAddr->pMtApi;
	g_pInRTApi   = pApiAddr->pRtApi;
	if( !g_pNaviData->Initialize() )
	{
		return nuFALSE;
	}
	PINNAVIAPI pInApi = pApiAddr->pInApi;
	pInApi->IN_DrawNaviRoad		= LibIN_DrawNaviRoad;
	pInApi->IN_GetSimuData		= LibIN_GetSimuData;
	pInApi->IN_ColNaviInfo		= LibIN_ColNaviInfo;
	pInApi->IN_PointToNaviLine	= LibIN_PointToNaviLine;
	pInApi->IN_CheckScreenRoad	= LibIN_CheckScreenRoad;
	pInApi->IN_GetRoutingList	= LibIN_GetRoutingList;
	pInApi->IN_PlayNaviSound	= LibIN_PlayNaviSound;
	pInApi->IN_JudgeZoomRoad	= LibIN_JudgeZoomRoad;
	pInApi->IN_DrawArrow		= LibIN_DrawArrow;
	pInApi->IN_NaviStart		= LibIN_NaviStart;
	pInApi->IN_NaviStop			= LibIN_NaviStop;
	pInApi->IN_JudgeRoadType    = LibIN_JudgeRoadType;
	pInApi->IN_GetCCDRoadInfo   = LibIN_GetCCDRoadInfo;
	pInApi->IN_ChooseRouteLineth = LibIN_ChooseRouteLineth;//added by daniel for LCMM
	pInApi->IN_EstimateTimeAndDistance = LibIN_EstimateTimeAndDistance;
	pInApi->IN_DisplayRoutePath = LibIN_DisplayRoutePath;
	pInApi->IN_GetMinAndMaxCoor_D = LibIN_GetMinAndMaxCoor_D;//added by daniel
	pInApi->IN_TMCRoute			  = LibIN_TMCRoute;//added by daniel for LCMM 20110513
	pInApi->IN_GetCarInfoForKM  = LibIN_GetCarInfoForKM;//added by daniel 20121205
	pInApi->IN_SetSimulation    = LibIN_SetSimulation;
	pInApi->IN_GetSimuTunnelData= LibIN_GetSimuTunnelData;
	pInApi->IN_TMCNaviPathData =  LibIN_TMCNaviPathData;
	pInApi->IN_FreeTMCNaviPathData =  LibIN_FreeTMCNaviPathData;
//--------------------------------SDK-------------------------------//
	pInApi->IN_GetRoutingListEx = LibIN_GetRoutingListEx;
//--------------------------------SDK-------------------------------//
	return nuTRUE;
}
INNAVI_API nuVOID LibIN_FreeInNavi()
{
	if( g_pNaviData != NULL )
	{
		g_pNaviData->Free();
		delete g_pNaviData;
		g_pNaviData = NULL;
	}
//	g_libMT.FreeDll();
//	g_libFS.FreeDll();
}

INNAVI_API nuBOOL LibIN_NaviStart()
{
	g_pNaviData->bRestNaviData2();
	return nuTRUE;
}
INNAVI_API nuBOOL LibIN_NaviStop()
{
	g_pNaviData->FreeNaviData();
	return nuTRUE;
}
INNAVI_API nuBOOL LibIN_GetSimuData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpData)
{
 	return g_pNaviData->GetSimuData(bReStart, nLen, lpData);
// 	return g_pNaviData->GetSimulationData(bReStart, nLen, lpData);
}
INNAVI_API nuBOOL LibIN_DrawNaviRoad(nuBYTE bZoom)
{
	return g_pNaviData->DrawNaviRoad(bZoom);
}
INNAVI_API nuUINT LibIN_PointToNaviLine(nuLONG nX, nuLONG nY, nuLONG nCoverDis, short nAngle, PSEEKEDROADDATA pRoadData)
{
	return g_pNaviData->PointToNaviLine(nX, nY, nCoverDis, nAngle, pRoadData);
}
INNAVI_API nuBOOL LibIN_ColNaviInfo()
{
	return g_pNaviData->ColNaviInfo();
}

INNAVI_API nuPVOID LibIN_CheckScreenRoad(const nuroRECT& rtRange)
{
	return g_pNaviData->CheckScreenRoad(rtRange);
}
INNAVI_API nuUINT LibIN_GetRoutingList( PROUTINGLISTNODE pRTlist, nuINT nNum, nuINT nIndex )
{
	return g_pNaviData->GetRoutingList( pRTlist, nNum, nIndex);
}
INNAVI_API nuBOOL LibIN_PlayNaviSound()
{
	return g_pNaviData->PlayNaviSound();
}
INNAVI_API  nuBYTE LibIN_JudgeZoomRoad()
{
	return g_pNaviData->JudgeZoomRoad();
}

INNAVI_API nuBOOL LibIN_DrawArrow(nuBYTE bZoom)
{
	return g_pNaviData->DrawArrow(bZoom);
}
INNAVI_API nuBOOL LibIN_ColNaviLine(nuBOOL bNaviPtStart)
{
	return g_pNaviData->ColNaviLineZ(bNaviPtStart);
}

INNAVI_API  nuBYTE LibIN_JudgeRoadType(PSEEKEDROADDATA pNowRoadInfo)
{
	return g_pNaviData->JudgeRoadType(pNowRoadInfo);
}
INNAVI_API nuBOOL LibIN_GetCCDRoadInfo(PCCDINFO pCcdInfo)
{
	return g_pNaviData->GetCarInfoForCCD(pCcdInfo);
}
#ifdef LCMM
	INNAVI_API nuVOID LibIN_EstimateTimeAndDistance(nuLONG *TargetDis, nuLONG *TargetTime)
	{
		g_pNaviData->EstimateTimeAndDistance(TargetDis,TargetTime);
	}
	INNAVI_API nuVOID LibIN_ChooseRouteLineth(nuLONG RouteLineth)
	{
		g_pNaviData->ChooseRouteLineth(RouteLineth);
	}
	INNAVI_API nuVOID LibIN_DisplayRoutePath(nuLONG SelectRoutePath)
	{
		g_pNaviData->DisplayRoutePath(SelectRoutePath);
	}
	INNAVI_API nuLONG LibIN_TMCRoute(TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount,TARTGETINFO NewStart,nuLONG RouteRule)
	{
		return g_pNaviData->TMCRoute(pTMCData, TMCDataCount, NewStart, RouteRule);
	}
	
#endif
/**********************Add********************
INNAVI_API nuPVOID LibIN_GetRTData(nuPVOID *p)
{
	*p = &(g_pNaviData->m_NodeVec); 
	return 0;
}
********************Add*********************/
INNAVI_API nuVOID LibIN_GetMinAndMaxCoor_D(NUROPOINT *MaxPoint,NUROPOINT *MinPoint)
{//added by daniel
	MaxPoint->x = g_pNaviData->m_PtRouteEndMax.x;
	MaxPoint->y = g_pNaviData->m_PtRouteEndMax.y;
	MinPoint->x = g_pNaviData->m_PtRouteEndMin.x;
	MinPoint->y = g_pNaviData->m_PtRouteEndMin.y;
}
INNAVI_API nuBOOL LibIN_GetCarInfoForKM(PKMINFO pKMInfo)
{
	return g_pNaviData->GetCarInfoForKM(pKMInfo);
}
INNAVI_API nuVOID LibIN_SetSimulation()
{
	g_pNaviData->SetSimulation();
}
INNAVI_API nuBOOL LibIN_GetSimuTunnelData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpData)
{
 	return g_pNaviData->GetSimuTunnelData(bReStart, nLen, lpData);
// 	return g_pNaviData->GetSimulationData(bReStart, nLen, lpData);
}
INNAVI_API nuBOOL LibIN_TMCNaviPathData()
{
	return g_pNaviData->TMCNaviPathData();
}
INNAVI_API nuVOID LibIN_FreeTMCNaviPathData()
{
	return g_pNaviData->FreeTMCNaviPathData();
}
//-----------------------------------------SDK------------------------------------------------//
INNAVI_API nuPVOID LibIN_GetRoutingListEx(PROUTINGLISTNODE pRTlist, nuINT *pNum, nuINT *pIndex)
{
	return g_pNaviData->GetRoutingListEx(&pRTlist, pNum, pIndex);
}
//-----------------------------------------SDK------------------------------------------------//
