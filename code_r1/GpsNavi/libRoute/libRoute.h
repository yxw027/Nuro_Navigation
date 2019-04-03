// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2010/09/03 04:28:31 $
// $Revision: 1.7 $
#ifndef __NURO_ROUTE_20080513
#define __NURO_ROUTE_20080513

#ifdef _USRDLL
#ifdef ROUTE_EXPORTS
#define ROUTE_API extern "C" __declspec(dllexport)
#else
#define ROUTE_API extern "C" __declspec(dllimport)
#endif
#else
#define ROUTE_API extern "C" 
#endif

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"

	
	ROUTE_API nuBOOL LibRT_InitRoute(nuPVOID lpInMM, nuPVOID lpInFS, nuPVOID lpOutRT);
	ROUTE_API nuVOID LibRT_FreeRoute();

	ROUTE_API nuVOID SetDriveAngle(nuBOOL b_UseAngle,nuLONG RideAngle);

	ROUTE_API nuLONG LibRT_LocalRoute(NAVIRTBLOCKCLASS** pNRBC);//°Ï°ì³W¹º
	ROUTE_API nuLONG LibRT_StartRoute(ROUTINGDATA *NaviData);
	ROUTE_API nuBOOL LibRT_ReleaseRoute(ROUTINGDATA *NaviData);
	ROUTE_API nuLONG LibRT_ReRoute(ROUTINGDATA *NaviData,TARTGETINFO NewStart);
	ROUTE_API nuLONG LibRT_CongestionRoute(ROUTINGDATA *NaviData, TARTGETINFO NewStart, nuLONG l_CongestionDis);

	ROUTE_API nuVOID LibRT_StopRoute();

	ROUTE_API nuBOOL GetMRouteData(PNAVIFROMTOCLASS pNFTC);

	ROUTE_API nuBOOL LibRT_GetNextCrossData( nuLONG MapID, nuLONG BlockID, nuLONG RoadID, nuLONG CarAngle, NUROPOINT NowCoor, 
		                                     NUROPOINT TNodeCoor, CROSSFOLLOWED **stru_Cross );
	ROUTE_API nuBOOL LibRT_ReleaseNextCrossData(CROSSFOLLOWED **stru_Cross);
	
//----------------------added by daniel for demo 2010.08.17-------------------------//
	ROUTE_API nuVOID LibRT_Route_Demo(nuSHORT DemoWR, nuLONG DemoNumber);//DemoWR->0:No Action, 1:Write, 2:Read
	ROUTE_API nuBOOL LibRT_ReadCoor(NUROPOINT *FCoor, NUROPOINT *TCoor, nuLONG DemoNumber);
//----------------------added by daniel for demo 2010.08.17-------------------------//
	ROUTE_API nuVOID LibRT_ReleaseNFTC(PNAVIFROMTOCLASS pNFTC);
	#ifdef LCMM
		ROUTE_API nuVOID LibRT_SetRouteRule(nuLONG *RouteRule, nuLONG RuleCount);
		ROUTE_API nuLONG LibRT_TMCRoute(NAVIFROMTOCLASS *pNFTC, PNAVIRTBLOCKCLASS pNRBC, TARTGETINFO NewStart, TMC_ROUTER_DATA *TMCData, nuLONG TMCDataCount, nuLONG RouteRule);
	#endif
#endif

