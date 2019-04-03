#ifndef __NURO_INNAVE_20080401
#define __NURO_INNAVE_20080401

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroOpenedDefine.h"
#include "NuroModuleApiStruct.h"


#ifdef _USRDLL
#ifdef INNAVI_EXPORTS
#define INNAVI_API extern "C" __declspec(dllexport)
#else
#define INNAVI_API extern "C" __declspec(dllimport)
#endif
#else
#define INNAVI_API extern "C" 
#endif


INNAVI_API nuBOOL LibIN_InitInNavi(PAPISTRUCTADDR pApiAddr);
INNAVI_API nuVOID LibIN_FreeInNavi();

INNAVI_API nuBOOL LibIN_NaviStart();
INNAVI_API nuBOOL LibIN_NaviStop();

INNAVI_API nuBOOL LibIN_GetSimuData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpData);
INNAVI_API nuBOOL LibIN_DrawNaviRoad(nuBYTE bZoom);
INNAVI_API nuUINT LibIN_PointToNaviLine(nuLONG nX, nuLONG nY, nuLONG nCoverDis, nuSHORT nAngle, PSEEKEDROADDATA pRoadData);
INNAVI_API nuBOOL LibIN_ColNaviInfo();
INNAVI_API nuPVOID LibIN_CheckScreenRoad(const nuroRECT& rtRange);
INNAVI_API nuUINT LibIN_GetRoutingList(PROUTINGLISTNODE pRTlist, nuINT nNum, nuINT nIndex);
INNAVI_API nuBOOL LibIN_PlayNaviSound();
INNAVI_API nuBYTE LibIN_JudgeZoomRoad();
INNAVI_API nuBOOL LibIN_DrawArrow(nuBYTE bZoom);

INNAVI_API nuBOOL LibIN_ColNaviLine(nuBOOL bNaviPtStart);
INNAVI_API  nuBYTE LibIN_JudgeRoadType(PSEEKEDROADDATA pNowRoadInfo);

INNAVI_API nuBOOL LibIN_GetCCDRoadInfo(PCCDINFO pCcdInfo);
INNAVI_API nuVOID LibIN_GetMinAndMaxCoor_D(NUROPOINT *MaxPoint, NUROPOINT *MinPoint);//added by daniel
INNAVI_API nuBOOL LibIN_GetCarInfoForKM(PKMINFO pKMInfo);//added by daniel 20121205
//-----------------------SDK----------------------------------------------------------------//
INNAVI_API nuPVOID LibIN_GetRoutingListEx(PROUTINGLISTNODE pRTlist, nuINT *pNum, nuINT *pIndex);
//-----------------------SDK----------------------------------------------------------------//
#ifdef LCMM
	INNAVI_API nuVOID LibIN_EstimateTimeAndDistance(nuLONG *TargetDis, nuLONG *TargetTime);
	INNAVI_API nuVOID LibIN_ChooseRouteLineth(nuLONG RouteLineth);
	INNAVI_API nuVOID LibIN_DisplayRoutePath(nuLONG SelectRoutePath);
	INNAVI_API nuLONG LibIN_TMCRoute(TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount, TARTGETINFO NewStart, nuLONG RouteRule);
#endif
/**********************Add*********************/
	INNAVI_API nuVOID LibIN_SetSimulation();
	INNAVI_API nuBOOL LibIN_GetSimuTunnelData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpData);
	INNAVI_API nuBOOL LibIN_TMCNaviPathData();
	INNAVI_API nuVOID LibIN_FreeTMCNaviPathData();
//INNAVI_API nuPVOID LibIN_GetRTData(nuPVOID *p);
#endif
