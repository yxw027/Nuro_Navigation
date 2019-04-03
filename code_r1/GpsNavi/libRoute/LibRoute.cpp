// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:46 $
// $Revision: 1.21 $
#include "libRoute.h"
//#include "CDontToHere.h"
#include "NuroModuleApiStruct.h"

#include "RouteBase.h"
#include "RouteCtrl.h"
#ifdef LCMM
	nuLONG LCMM_RoutingRule[MAX_RouteRule];
#endif
//------------------------added by daniel for Demo 2010.08.17------------------------------------------//
#include "Class_ReCord.h"
//extern ReCord;
ReCord ReCordata;
nuSHORT DemoState = 0; 
nuLONG RunDataNumber = 0;
//------------------------added by daniel for Demo 2010.08.17------------------------------------------//
RouteCtrl 		*g_pRouteCtrl   = NULL;
PFILESYSAPI		g_pRtFSApi	= NULL;
PMEMMGRAPI		g_pRtMMApi	= NULL;

/*Dll entry. platform dependent*/
#ifdef NURO_OS_WINDOWS
#ifdef _USRDLL
/*
nuBOOL nuAPIENTRY NURO_DLLMAIN(	nuHANDLE hinstDLL, 
								nuDWORD  fdwReason, 
								nuPVOID lpvReserved
							  )
{
	switch(fdwReason)
	{
		case NURO_DLL_PROCESS_ATTACH:
			//LibRT_InitRoute();
			break;
		case NURO_DLL_PROCESS_DETACH:
			//LibRT_FreeRoute();
			break;
		case NURO_DLL_THREAD_ATTACH:
			break;
		case NURO_DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
*/
#endif
#endif

/** LibRT_InitRoute
* @Author	Louis
* @param	lpInMM is Memory control pointer
* @param	lpInFS is 
* @return	
* @note		
*/
ROUTE_API nuBOOL LibRT_InitRoute(nuPVOID lpInMM, nuPVOID lpInFS, nuPVOID lpOutRT)
{
	if ( lpInMM == NULL || lpInFS == NULL || lpOutRT == NULL )
	{
		return nuFALSE;
	}
	g_pRtMMApi	= (PMEMMGRAPI)lpInMM;
	g_pRtFSApi	= (PFILESYSAPI)lpInFS;
	PROUTEAPI pRtApi = (PROUTEAPI)lpOutRT;
	b_StopRoute=nuFALSE;

	GetRootPathStep2();

	g_pCDTH =  new CDontToHere;
	g_pCRDC = new CRoutingDataCtrl;	
	
	g_pRoutingRule= new CCtrl_RuleSet;	
	g_pDB_DwCtrl= new UONERDwCtrl;
	g_pUOneBH= new UONEBHCtrl;
	g_pLocalRtB[0]= new UONERtBCtrl;
	g_pLocalRtB[1]= new UONERtBCtrl;

	g_pRouteCtrl= new RouteCtrl;
	
	pRtApi->RT_StartRoute		= LibRT_StartRoute;
	pRtApi->RT_ReRoute			= LibRT_ReRoute;
	pRtApi->RT_LocalRoute		= LibRT_LocalRoute;
	pRtApi->RT_ReleaseRoute		= LibRT_ReleaseRoute;
	pRtApi->RT_CongestionRoute	= LibRT_CongestionRoute;
	pRtApi->RT_GetNextCrossData	= LibRT_GetNextCrossData;
	pRtApi->RT_ReleaseNextCrossData	= LibRT_ReleaseNextCrossData;
	pRtApi->RT_GetMRouteData		= GetMRouteData;
	pRtApi->RT_Route_Demo		= LibRT_Route_Demo;//added by daniel for demo 2010.08.17
	pRtApi->RT_ReadCoor			= LibRT_ReadCoor;//added by daniel for demo 2010.08.26
	pRtApi->RT_ReleaseNFTC		= LibRT_ReleaseNFTC;//added by daniel for LCMM 2011.04.11
	pRtApi->RT_SetRouteRule		= LibRT_SetRouteRule;//added by daniel for LCMM 2011.04.12
	pRtApi->RT_TMCRoute			= LibRT_TMCRoute;
	nuMemset(LCMM_RoutingRule, -1, sizeof(LCMM_RoutingRule));
	LCMM_RoutingRule[0] = 0;
	return nuTRUE;
}

ROUTE_API nuVOID LibRT_FreeRoute()
{
	if(g_pCDTH)
	{
		delete g_pCDTH;
		g_pCDTH = NULL;
	}
	
	if(g_pCRDC)
	{
		delete g_pCRDC;
		g_pCRDC = NULL;
	}

	if(g_pRouteCtrl)
	{
		delete g_pRouteCtrl;
		g_pRouteCtrl=NULL;
	}
	
	if(g_pLocalRtB[1])
	{
		delete g_pLocalRtB[1];
		g_pLocalRtB[1]=NULL;
	}

	if(g_pLocalRtB[0])
	{
		delete g_pLocalRtB[0];
		g_pLocalRtB[0]=NULL;
	}


	if(g_pDB_DwCtrl)
	{
		delete g_pDB_DwCtrl;
		g_pDB_DwCtrl=NULL;
	}

	if(g_pUOneBH)
	{
		delete g_pUOneBH;
		g_pUOneBH=NULL;
	}

	if(g_pRoutingRule)
	{
		delete g_pRoutingRule;
		g_pRoutingRule=NULL;
	}
}

ROUTE_API nuVOID SetDriveAngle(nuBOOL b_UseAngle,nuLONG RideAngle)
{
	g_pRouteCtrl->b_UseAngle	=	b_UseAngle;
	g_pRouteCtrl->SetRideAngle(RideAngle);//?묹?p???I?H?U??????
}


//?}?l?W?? ?P ?????W??
ROUTE_API nuLONG LibRT_StartRoute(ROUTINGDATA *NaviData)
{
	__android_log_print(ANDROID_LOG_INFO, "Ròute", "LibRT_StartRoute Start!!!!!");
	nuBOOL re_bool = nuFALSE;
	nuLONG ReNum = 0, i = 0;

	//?M???ª??W??????
	LibRT_ReleaseRoute(NaviData);
	nuSleep(100);
	g_pRtMMApi->MM_CollectDataMem(1);

	b_StopRoute=nuFALSE;
	if(g_pRtFSApi->pGdata->routeGlobal.TargetCount<2){
#ifndef LCMM
		NaviData->NFTC=NULL;		NaviData->NFTCCount=0;
#endif
		return ROUTE_NOFTPT;
	}
#ifdef LCMM
	NaviData->NFTC[0]=NULL;		NaviData->NFTCCount=0;
	NaviData->NFTC[1]=NULL;	
	NaviData->NFTC[2]=NULL;
#endif
	if(g_pRtFSApi->pGdata->routeGlobal.CarSpeed>10){
		g_pRouteCtrl->b_UseAngle	=	nuTRUE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);//?묹?p???I?H?U??????
	}
	else{
		g_pRouteCtrl->b_UseAngle	=	nuFALSE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);//?묹?p???I?H?U??????
	}
	//?]?mTarget?I
	g_pUOneBH->InitClass();
	for(i=0;i<g_pRtFSApi->pGdata->routeGlobal.TargetCount;i++){
		re_bool=g_pRouteCtrl->NewAddMarketPt_NodeID(	i,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].MapIdx,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].BlockIdx,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].RoadIdx,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].CityID,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].TownID,
											g_pRtFSApi->pGdata->routeGlobal.pTargetInfo[i].FixCoor);
		#ifndef DEF_MAKETHRROUTE
			if(re_bool==nuFALSE)	{	return ROUTE_NOFOUNDROADTOENDPOINT;	}
		#endif
	}
	g_pUOneBH->ReleaseBH();
	//?}?l?W???B??
	g_pDB_DwCtrl->Reset();
//------------------------added by daniel for Demo 2010.08.17------------------------------------------//
	/*if(DemoState == 2)
	{
		DemoState = 0;
		ReNum = ReCordata.ReCord_Read(NaviData,RunDataNumber);
		if(ReNum != ROUTE_SUCESS)
		{
			g_pCRDC->Release_RoutingData(NaviData);
			return ReNum;
		}
		else
		{
			return ReNum;
		}
	}	
//------------------------added by daniel for Demo 2010.08.17------------------------------------------*/
#ifdef LCMM
	//defualt ?]???Ĥ@?ӱ????A?קK???᭫?s?W???????D?n?????ӱ???
	/*nuLONG RouteRule[3] = {1,4,6};
	LibRT_SetRouteRule(RouteRule,3);
	g_pRtFSApi->pGdata->routeGlobal.RoutingRule = LCMM_RoutingRule[0];
	if(g_pRtFSApi->pGdata->routeGlobal.RouteCount == 0)
	{
		g_pRtFSApi->pGdata->routeGlobal.RouteCount = 1;//default on line
	}*/
	ReNum=g_pRouteCtrl->StartRouting_LCMM(NaviData, nuFALSE, LCMM_RoutingRule, g_pRtFSApi->pGdata->routeGlobal.RouteCount);
	__android_log_print(ANDROID_LOG_INFO, "Route", "LibRoute!!!ReNum....%d",ReNum);
#else
	ReNum=g_pRouteCtrl->StartRouting(NaviData, nuFALSE, g_pRtFSApi->pGdata->routeGlobal.RoutingRule);
#endif
	if(ReNum!=ROUTE_SUCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "Route", "LibRoute Fail!!!!");
		g_pRouteCtrl->ReleaseNaviInfo(NaviData);
		return ReNum;
	}
//------------------------added by daniel for Demo 2010.08.17------------------------------------------//
	if(DemoState == 1)
	{
		ReCordata.ReCord_Write(NaviData,RunDataNumber);
	}
	DemoState = 0;
//------------------------added by daniel for Demo 2010.08.17------------------------------------------*/

#ifdef VALUE_EMGRT
	if(ReNum == ROUTE_SUCESS && !GetMRouteData(NaviData->NFTC[0]))
	{
		__android_log_print(ANDROID_LOG_INFO, "Route", "LibRoute ROUTE_ERROR!!!!");
		g_pRouteCtrl->ReleaseNaviInfo(NaviData);
		return ROUTE_ERROR;
	}
#endif
	if(ReNum && NaviData->NFTC[0]->NRBC != NULL)
	{
		if( NaviData->NFTC[0]->NRBC->RTBStateFlag == 1)
		{	
			ReNum = LibRT_LocalRoute(&NaviData->NFTC[0]->NRBC);
		}
	}
	__android_log_print(ANDROID_LOG_INFO, "Route", "LibRoute!!!!! %d",ReNum);
	return ReNum;
}
ROUTE_API nuBOOL LibRT_ReleaseRoute(ROUTINGDATA *NaviData){
	b_StopRoute=nuFALSE;
	g_pCDTH->InitConst();
	nuDWORD MemoryUsed1 = 0, MemoryUsed2 = 0;
	g_pRouteCtrl->ReleaseNaviInfo(NaviData);
	return nuTRUE;
}

ROUTE_API nuLONG LibRT_LocalRoute(NAVIRTBLOCKCLASS** pNRBC){//?ϰ??W??
	nuLONG FMapIdx = 0, TMapIdx = 0; 
	g_pRtMMApi->MM_CollectDataMem(1);
	b_StopRoute=nuFALSE;
	g_pDB_DwCtrl->Reset();
	FMapIdx = g_pRtFSApi->FS_MapIDIndex((*pNRBC)->FInfo.MapID,1);
	TMapIdx = g_pRtFSApi->FS_MapIDIndex((*pNRBC)->TInfo.MapID,1);
	return	g_pRouteCtrl->LocalRoutingU1(*pNRBC, g_pRtFSApi->pGdata->routeGlobal.RoutingRule,FMapIdx,TMapIdx);
}

ROUTE_API nuLONG LibRT_ReRoute(ROUTINGDATA *NaviData,TARTGETINFO NewStart)//???s?W??
{
	nuBOOL re_bool = nuFALSE;
	nuLONG ReNum   = 0;

	g_pRtMMApi->MM_CollectDataMem(1);
	b_StopRoute=nuFALSE;

	if(g_pRtFSApi->pGdata->routeGlobal.CarSpeed>10){
		g_pRouteCtrl->b_UseAngle	=	nuTRUE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}
	else{
		g_pRouteCtrl->b_UseAngle	=	nuFALSE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}

	re_bool=g_pRouteCtrl->NewAddMarketPt(	0,
											NewStart.MapIdx,
											NewStart.BlockIdx,
											NewStart.RoadIdx,
											NewStart.FixCoor	);

	if(re_bool)
	{
		//?}?l?W???B??
		g_pDB_DwCtrl->Reset();
		ReNum=g_pRouteCtrl->ReRoute(NaviData,nuFALSE,g_pRtFSApi->pGdata->routeGlobal.RoutingRule);
	}
	__android_log_print(ANDROID_LOG_INFO, "navi", "LibRT_ReRoute %d\n", ReNum);
	return 1;//return ReNum;//
}

ROUTE_API nuLONG LibRT_CongestionRoute(ROUTINGDATA *NaviData,TARTGETINFO NewStart,nuLONG l_CongestionDis)//???s?W??
{
	nuBOOL re_bool = nuFALSE;
	nuLONG ReNum   = 0;

	g_pRtMMApi->MM_CollectDataMem(1);
	l_CongestionDis=2000;
	b_StopRoute=nuFALSE;

	if(g_pRtFSApi->pGdata->routeGlobal.CarSpeed>10){
		g_pRouteCtrl->b_UseAngle	=	nuTRUE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}
	else{
		g_pRouteCtrl->b_UseAngle	=	nuFALSE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}

	re_bool=g_pRouteCtrl->NewAddMarketPt(	0,
											NewStart.MapIdx,
											NewStart.BlockIdx,
											NewStart.RoadIdx,
											NewStart.FixCoor	);

	//?}?l?W???B??
	g_pDB_DwCtrl->Reset();
#ifdef LCMM
	ReNum=g_pRouteCtrl->CongestionRoute(NaviData,nuFALSE,g_pRtFSApi->pGdata->routeGlobal.RoutingRule,l_CongestionDis);
#else
	ReNum=g_pRouteCtrl->CongestionRoute(NaviData,nuFALSE,&g_pRtFSApi->pGdata->routeGlobal.RoutingRule,l_CongestionDis);
#endif
	return ReNum;
}


ROUTE_API nuVOID LibRT_StopRoute(){
	b_StopRoute=nuTRUE;
}

ROUTE_API nuBOOL GetMRouteData(PNAVIFROMTOCLASS pNFTC)
{
	return g_pRouteCtrl->GetMRouteData(pNFTC, g_pRtFSApi->pGdata->routeGlobal.RoutingRule);
}

ROUTE_API nuBOOL LibRT_GetNextCrossData(nuLONG MapID,nuLONG BlockID,nuLONG RoadID,nuLONG CarAngle,NUROPOINT NowCoor,NUROPOINT TNodeCoor,CROSSFOLLOWED **stru_Cross)
{
#ifdef VALUE_EMGRT
	*stru_Cross=NULL;
	return nuFALSE;
#else
	return g_pRouteCtrl->GetNextCrossData(MapID,BlockID,RoadID,CarAngle,NowCoor,TNodeCoor,stru_Cross);
#endif
}

ROUTE_API nuBOOL LibRT_ReleaseNextCrossData(CROSSFOLLOWED **stru_Cross)
{
	*stru_Cross=NULL;
#ifdef VALUE_EMGRT
	return nuTRUE;
#else
	return g_pRouteCtrl->ReleaseNextCrossData();
#endif
}
//------------------------added by daniel for Demo 2010.08.17------------------------------------------//
ROUTE_API nuVOID LibRT_Route_Demo(nuSHORT DemoWR,nuLONG DemoNumber)//DemoWR->0:No Action, 1:Write, 2:Read
{
	DemoState     = 0;
	RunDataNumber = 0;
	DemoState    = DemoWR;
	RunDataNumber= DemoNumber;
}
ROUTE_API nuBOOL LibRT_ReadCoor(NUROPOINT *FCoor,NUROPOINT *TCoor,nuLONG DemoNumber)//DemoWR->0:No Action, 1:Write, 2:Read
{
	if(ReCordata.ReCord_ReadCoor(FCoor,TCoor,DemoNumber))
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

//------------------------added by daniel for Demo 2010.08.17------------------------------------------*/
ROUTE_API nuVOID LibRT_ReleaseNFTC(PNAVIFROMTOCLASS pNFTC)
{
	PNAVIFROMTOCLASS pTmpNFTC = NULL;
	while(pNFTC != NULL)
	{
		pTmpNFTC = pNFTC->NextFT;
		g_pCRDC->Release_NaviFromToClass(pNFTC);
		if(pTmpNFTC == NULL || pTmpNFTC->NextFT == NULL)
		{
			break;
		}
		pNFTC = pTmpNFTC->NextFT;
	}
	pNFTC = NULL;
}
#ifdef LCMM

ROUTE_API nuVOID LibRT_SetRouteRule(nuLONG *RouteRule,nuLONG RuleCount)//?o?̱ƧǩM?R???P?˱???g?k?i?H?A?[?j(?ثe?ĥΤg?k?ҿ??A?į??̧C@@)
{
	
	nuLONG i = 0, j = 0, count = 0, temp = 0;
	nuMemset(LCMM_RoutingRule , -1 , sizeof(nuLONG)*MAX_RouteRule);
	nuMemcpy(LCMM_RoutingRule , RouteRule,sizeof(nuLONG)*RuleCount);

	g_pRtFSApi->pGdata->routeGlobal.RouteCount = RuleCount;
}

ROUTE_API nuLONG LibRT_TMCRoute(NAVIFROMTOCLASS *pNFTC, PNAVIRTBLOCKCLASS pNRBC,TARTGETINFO NewStart, TMC_ROUTER_DATA *TMCData, nuLONG TMCDataCount, nuLONG RouteRule)
{
	nuBOOL re_bool = nuFALSE;
	g_pRtMMApi->MM_CollectDataMem(1);
	b_StopRoute=nuFALSE;

	if(g_pRtFSApi->pGdata->routeGlobal.CarSpeed>10){
		g_pRouteCtrl->b_UseAngle	=	nuTRUE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}
	else{
		g_pRouteCtrl->b_UseAngle	=	nuFALSE;
		g_pRouteCtrl->SetRideAngle(g_pRtFSApi->pGdata->routeGlobal.nCarAngle);
	}

	if(!g_pRouteCtrl->NewAddMarketPt(	0,
											pNRBC->FileIdx,
											g_pRtFSApi->pGdata->drawInfoMap.roadCarOn.nBlkIdx,
											g_pRtFSApi->pGdata->drawInfoMap.roadCarOn.nRoadIdx,
											g_pRtFSApi->pGdata->drawInfoMap.roadCarOn.ptMapped	))
	{
		return nuFALSE;
	}

	g_pDB_DwCtrl->Reset();
	pNRBC->FInfo.FixCoor.x = NewStart.FixCoor.x; 
	pNRBC->FInfo.FixCoor.y = NewStart.FixCoor.y; 
	__android_log_print(ANDROID_LOG_INFO, "TMC", "LibRT_TMCRoute!!!!!");
	return g_pRouteCtrl->TMCLocalRoute(pNFTC, pNRBC, TMCData, TMCDataCount, RouteRule);
}
#endif

