// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:51 $
// $Revision: 1.12 $
#include "RouteCtrl.h"
//#include "CRoutingDataCtrl.h"
#include "NuroConstDefined.h"

RouteCtrl::RouteCtrl()
{
	nuLONG i;
	for(i=0;i<MAX_TARGET;i++)
	{
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;
		MkPt[i].FixNode1=0;				MkPt[i].FixNode2=0;
		MkPt[i].FixCoor.x=0;			MkPt[i].FixCoor.y=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;
	}
	b_UseAngle=nuFALSE;
	RideAngle=0;

}

RouteCtrl::~RouteCtrl()
{
	ReleaseNextCrossData();
}

nuVOID RouteCtrl::ReleaseNaviInfo(PROUTINGDATA NaviData)
{
	if(NaviData==NULL) return;
	g_pCRDC->Release_RoutingData(NaviData);
	return;
}

nuBOOL RouteCtrl::AddMarketPt(nuLONG CoorX,nuLONG CoorY,nuLONG Index,nuLONG FileID, nuLONG RtBlockID)//Index 0:起點 1:中途點 2:終點
{
	nuBOOL		b_TmpUseAngle = nuFALSE;
	nuLONG		TmpRideAngle  = 0;

	Class_PtFixToRoad m_PtFixToRoad;
	nuBOOL b_check=nuFALSE;
	if(Index==0){
		b_TmpUseAngle=b_UseAngle;
		TmpRideAngle=RideAngle;
	}
	else{
		b_TmpUseAngle=0;
		TmpRideAngle=nuFALSE;
	}

	if(MAX_TARGET<=Index)		return nuFALSE;

	/*** 初始化現在的規劃點 ***/
	MkPt[Index].MapID=0;				MkPt[Index].RTBID=0;
	MkPt[Index].FixDis=EACHBLOCKSIZE;
	MkPt[Index].FixNode1=0;				MkPt[Index].FixNode2=0;
	MkPt[Index].FixCoor.x=0;			MkPt[Index].FixCoor.y=0;

	b_check=m_PtFixToRoad.PtFixToRoad(CoorX,CoorY,FileID,RtBlockID,TmpRideAngle,b_TmpUseAngle,MkPt[Index]);
	m_PtFixToRoad.~Class_PtFixToRoad();

	return b_check;
}

nuBOOL RouteCtrl::NewAddMarketPt(nuLONG Index,nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,NUROPOINT Now_FixPt)
{
	nuBOOL		b_TmpUseAngle = nuFALSE;
	nuDOUBLE	TmpRideAngle  = 0;

	Class_PtFixToRoad m_PtFixToRoad;
	nuBOOL b_check=nuFALSE;
	if(MAX_TARGET<=Index)		return nuFALSE;

	if(Index==0)
	{	b_TmpUseAngle=b_UseAngle;	TmpRideAngle=RideAngle;	}
	else
	{	b_TmpUseAngle=nuFALSE;		TmpRideAngle=0;		}

	/*** 初始化現在的規劃點 ***/
	MkPt[Index].MapID=0;				MkPt[Index].RTBID=0;				MkPt[Index].FixDis=EACHBLOCKSIZE;
	MkPt[Index].FixNode1=0;				MkPt[Index].FixNode2=0;				MkPt[Index].FixCoor.x=0;			MkPt[Index].FixCoor.y=0;
	MkPt[Index].MapIdx = Now_FileIdx;
	b_check=m_PtFixToRoad.NewPtFixToRoad(Now_FileIdx, Now_BlockIdx, Now_RoadIdx, Now_FixPt, _MAXUNLONG, RideAngle, b_TmpUseAngle, MkPt[Index]);
	m_PtFixToRoad.~Class_PtFixToRoad();
	return b_check;
}
nuBOOL RouteCtrl::NewAddMarketPt_NodeID(nuLONG Index,nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,nuLONG CityID,nuLONG TownID,NUROPOINT Now_FixPt)
{
	nuBOOL		b_TmpUseAngle = nuFALSE;
	nuDOUBLE	TmpRideAngle  = 0;

	Class_PtFixToRoad m_PtFixToRoad;
	nuBOOL b_check=nuFALSE;
	if(MAX_TARGET<=Index)		return nuFALSE;

	if(Index==0)
	{	b_TmpUseAngle=b_UseAngle;	TmpRideAngle=RideAngle;	}
	else
	{	b_TmpUseAngle=nuFALSE;		TmpRideAngle=0;		}

	/*** 初始化現在的規劃點 ***/
	MkPt[Index].MapID=0;				MkPt[Index].RTBID=0;				MkPt[Index].FixDis=EACHBLOCKSIZE;
	MkPt[Index].FixNode1=0;				MkPt[Index].FixNode2=0;				MkPt[Index].FixCoor.x=Now_FixPt.x;			MkPt[Index].FixCoor.y=Now_FixPt.y;
	MkPt[Index].MapIdx = Now_FileIdx;
	b_check=m_PtFixToRoad.NewPtFixToRoad_NodeID(Now_FileIdx,Now_BlockIdx,Now_RoadIdx,CityID,TownID,Now_FixPt,_MAXUNLONG,RideAngle,b_TmpUseAngle,MkPt[Index]);
	m_PtFixToRoad.~Class_PtFixToRoad();
	return b_check;
}
#ifndef LCMM
nuLONG RouteCtrl::StartRouting(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG l_RoutingRule)
{
	NAVIFROMTOCLASS	*save_pNaviFTClass = NULL, *tmp_pNaviFTClass = NULL;
	nuLONG i = 0, FTPtCount = 0;
	nuLONG ReNum=ROUTE_SUCESS;
	m_CalRouting.CarAngle=RideAngle;
	m_CalRouting.b_ReRouting=nuFALSE;

	// 確認起訖點資訊
	FTPtCount=0;
	for(i=0;i<MAX_TARGET;i++)
	{
		if(MkPt[i].FixCoor.x==0 && MkPt[i].FixCoor.y==0)	continue;
		else												FTPtCount++;
	}

	// 創建NaviInfo第一層架構
	if(FTPtCount<=1)
	{
		for(i=0;i<MAX_TARGET;i++)
		{
			MkPt[i].MapID=0;						MkPt[i].RTBID=0;				MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
			MkPt[i].FixDis=EACHBLOCKSIZE;			MkPt[i].FixCoor.x=0;			MkPt[i].FixCoor.y=0;
		}
		return ROUTE_NOFTPT;
	}
	else
	{
		NaviData->NFTCCount=FTPtCount-1;
		for(i=0;i<FTPtCount-1;i++)
		{
			tmp_pNaviFTClass=NULL;
			save_pNaviFTClass=NULL;
			if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass)){	ReNum=ROUTE_ERROR;	break;	}
			ReNum=m_CalRouting.StartRouting(MkPt[i],MkPt[i+1],save_pNaviFTClass,b_MakeThr,l_RoutingRule);
		#ifdef RouteError
			MkPt[i+1].FixCoor.x = save_pNaviFTClass->TCoor.x;
			MkPt[i+1].FixCoor.y = save_pNaviFTClass->TCoor.y;
		#endif
			tmp_pNaviFTClass=NaviData->NFTC;
			if(tmp_pNaviFTClass==NULL){
				NaviData->NFTC=save_pNaviFTClass;
			}
			else{
				while(tmp_pNaviFTClass->NextFT!=NULL){
					tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
				}
				tmp_pNaviFTClass->NextFT=save_pNaviFTClass;
			}
			tmp_pNaviFTClass=NULL;
			save_pNaviFTClass=NULL;
			if(ROUTE_SUCESS!=ReNum) break;
		}
	}

	// 規劃完畢 除了規劃完的資料之外 其餘的都進行移除或初始化
	for(i=0;i<MAX_TARGET;i++)
	{
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;			MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}

	if(ReNum==ROUTE_SUCESS)	{	return ReNum;	}
	else					{	g_pCRDC->Release_RoutingData(NaviData);	return ReNum;	}
}
nuLONG RouteCtrl::ReRoute(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG l_RoutingRule)//重新規劃
{
	NAVIFROMTOCLASS	*save_pNaviFTClass = NULL, *tmp_pNaviFTClass = NULL, *tmp_pNaviFTClass2 = NULL;
	NAVIRTBLOCKCLASS *tmp_pNaviRTBClass = NULL;
	nuLONG i = 0;
	nuLONG ReNum=ROUTE_SUCESS;

	m_CalRouting.b_ReRouting=nuTRUE;
	m_CalRouting.CarAngle=RideAngle;
	
	//先判斷目前已經跑到哪裡了
	//重新規劃到尚未通過的經由點(終點)
	tmp_pNaviFTClass=NaviData->NFTC;
	while(nuTRUE){
		if(tmp_pNaviFTClass==NULL) return ROUTE_ERROR;
		if(tmp_pNaviFTClass->RunPass==nuTRUE){
			tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
		}
		else{
			tmp_pNaviRTBClass=tmp_pNaviFTClass->NRBC;
			while(nuTRUE){
				if(tmp_pNaviRTBClass->NextRTB==NULL) break;
				tmp_pNaviRTBClass=tmp_pNaviRTBClass->NextRTB;
			}
			break;
		}
	}

	MkPt[1].FixCoor=tmp_pNaviRTBClass->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass->TInfo.RTBID;

	save_pNaviFTClass=NULL;
	if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass)){	ReNum=ROUTE_ERROR;	return ROUTE_ERROR;	}
	ReNum=m_CalRouting.StartRouting(MkPt[0],MkPt[1],save_pNaviFTClass,b_MakeThr,l_RoutingRule);
	
	for(i=0;i<MAX_TARGET;i++)
	{
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;		MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}
	//如果規劃成功 就把舊的取代掉
	//如果規劃失敗 則把新的清除
	if(ReNum==ROUTE_SUCESS){
		save_pNaviFTClass->NextFT=tmp_pNaviFTClass->NextFT;
		tmp_pNaviFTClass=NaviData->NFTC;
		while(nuTRUE){
			if(tmp_pNaviFTClass->NextFT==NULL) break;
			if(tmp_pNaviFTClass->RunPass==nuFALSE) break;
			tmp_pNaviFTClass2=tmp_pNaviFTClass->NextFT;
			tmp_pNaviFTClass->NextFT=NULL;
			g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);//把走過的清掉
			tmp_pNaviFTClass=tmp_pNaviFTClass2;
		}
		g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);

		NaviData->NFTC=save_pNaviFTClass;
		return ReNum;	

	}
	else{
		g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass);
		return ReNum;
	}
}


nuLONG RouteCtrl::CongestionRoute(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG l_RoutingRule,nuLONG l_CongestionDis)//重新規劃
{
	NAVIFROMTOCLASS	*save_pNaviFTClass = NULL, *tmp_pNaviFTClass = NULL, *tmp_pNaviFTClass2 = NULL;
	NAVIRTBLOCKCLASS *tmp_pNaviRTBClass = NULL, *tmp_pNaviRTBClass2 = NULL;
	nuULONG i = 0;
	nuLONG ReNum=ROUTE_SUCESS;
	m_CalRouting.b_ReRouting=nuFALSE;

	m_CalRouting.CarAngle=RideAngle;
	//先判斷目前已經跑到哪裡了
	tmp_pNaviFTClass=NaviData->NFTC;
/*#ifdef DEBUG      Calculate Before ReRoute RoadLength
	NAVIRTBLOCKCLASS *pNaviRTBClass = NULL;
	NAVIFROMTOCLASS  *pNaviFTClass = NULL;
	nuLONG dis = 0;
	pNaviFTClass = tmp_pNaviFTClass;
	while(pNaviFTClass != NULL)
	{
		pNaviRTBClass = pNaviFTClass->NRBC;
		while(pNaviRTBClass != NULL)
		{
			dis += pNaviRTBClass->RealDis;
			pNaviRTBClass=pNaviRTBClass->NextRTB;
		}
		pNaviFTClass = pNaviFTClass->NextFT;
	}
#endif*/
	while(nuTRUE){
		if(tmp_pNaviFTClass==NULL) return ROUTE_ERROR;
		if(tmp_pNaviFTClass->RunPass==nuTRUE){
			tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
		}
		else{
			tmp_pNaviRTBClass=tmp_pNaviFTClass->NRBC;
			while(nuTRUE){
				if(tmp_pNaviRTBClass->NextRTB==NULL) break;
				
				for(i=0;i<tmp_pNaviRTBClass->NSSCount;i++)
				{
					if( l_CongestionDis > (NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.x-MkPt[0].FixCoor.x) + NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.y-MkPt[0].FixCoor.y)) )
					{
						break;
					}
				}
				if(i!=tmp_pNaviRTBClass->NSSCount)	break;
				else								tmp_pNaviRTBClass=tmp_pNaviRTBClass->NextRTB;
			}
			break;
		}
	}

	//
/*	g_pCDTH->b_CongestionRoute=nuTRUE;
	for(i=0;i<tmp_pNaviRTBClass->NSSCount;i++)
	{
		if( l_CongestionDis > (NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.x-MkPt[0].FixCoor.x) + NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.y-MkPt[0].FixCoor.y)) ){
			g_pCDTH->JoinNewCongestionID(tmp_pNaviRTBClass->NodeIDList[i+1]);
		}
	}*/

	/*MkPt[1].FixCoor=tmp_pNaviRTBClass->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass->TInfo.RTBID;*/

	tmp_pNaviRTBClass2=tmp_pNaviRTBClass;
	while(nuTRUE){
		if(tmp_pNaviRTBClass2->NextRTB==NULL) break;
		tmp_pNaviRTBClass2=tmp_pNaviRTBClass2->NextRTB;
	}

	MkPt[1].FixCoor=tmp_pNaviRTBClass2->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass2->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass2->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass2->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass2->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass2->TInfo.RTBID;

	save_pNaviFTClass=NULL;
	if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass)){	g_pCDTH->CleanCongestionID(); ReNum=ROUTE_ERROR;	return ROUTE_ERROR;	}
	ReNum=m_CalRouting.StartRouting(MkPt[0],MkPt[1],save_pNaviFTClass,b_MakeThr,l_RoutingRule);
	
	for(i=0;i<MAX_TARGET;i++){
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;		MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}
	//如果規劃成功 就把舊的取代掉
	//如果規劃失敗 則把新的清除
	if(ReNum==ROUTE_SUCESS){
		save_pNaviFTClass->NextFT=tmp_pNaviFTClass->NextFT;
		tmp_pNaviFTClass=NaviData->NFTC;
		while(nuTRUE){
			if(tmp_pNaviFTClass->NextFT==NULL) break;
			if(tmp_pNaviFTClass->RunPass==nuFALSE) break;
			tmp_pNaviFTClass2=tmp_pNaviFTClass->NextFT;
			tmp_pNaviFTClass->NextFT=NULL;
			g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);
			tmp_pNaviFTClass=tmp_pNaviFTClass2;
		}
		g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);
		NaviData->NFTC=save_pNaviFTClass;
		g_pCDTH->CleanCongestionID();

/*#ifdef DEBUG	Calculate After ReRoute RoadLength 	
		dis = 0;
		pNaviFTClass = NaviData->NFTC;
		while(pNaviFTClass != NULL)
		{
			pNaviRTBClass = pNaviFTClass->NRBC;
			while(pNaviRTBClass != NULL)
			{
				dis += pNaviRTBClass->RealDis;
				pNaviRTBClass=pNaviRTBClass->NextRTB;
			}
			pNaviFTClass = pNaviFTClass->NextFT;
		}
#endif*/
		return ReNum;
	}
	else{
		g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass);
		g_pCDTH->CleanCongestionID();
		return ReNum;
	}
}

#endif
nuLONG RouteCtrl::LocalRoutingU1(PNAVIRTBLOCKCLASS pNRBC,nuLONG l_RoutingRule, nuLONG FMapIdx, nuLONG TMapIdx){//區域規劃
	b_Drive=b_UseAngle;
	m_CalRouting.CarAngle=-1;
	m_CalRouting.b_ReRouting=nuFALSE;
	m_CalRouting.m_bTMCLocalRoute = nuFALSE;
	return m_CalRouting.LocalRouting(pNRBC,l_RoutingRule, FMapIdx, TMapIdx);
}
//------------added by daniel for LCMM-----------------------------------------------------//
nuLONG RouteCtrl::StartRouting_LCMM(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG *LCMM_RoutingRule,nuLONG RouteCount)
{
	NAVIFROMTOCLASS	*save_pNaviFTClass[MAX_RouteRule],*tmp_pNaviFTClass;
	nuLONG i = 0, FTPtCount = 0;
	nuLONG ReNum=ROUTE_SUCESS;
	nuINT RouteCounter_LCMM = 0; 
	m_lRouteCount = RouteCount;
	m_CalRouting.CarAngle=RideAngle;
	m_CalRouting.b_ReRouting=nuFALSE;
	// 確認起訖點資訊
	FTPtCount=0;
	for(i = 0; i < MAX_RouteRule; i++)
	{	
		save_pNaviFTClass[i] = NULL;
	}
	for(i=0;i<MAX_TARGET;i++)
	{
		if(MkPt[i].FixCoor.x==0 && MkPt[i].FixCoor.y==0)	continue;
		else												FTPtCount++;
	}
	m_CalRouting.m_lRouteCount = RouteCount;
	// 創建NaviInfo第一層架構
	if(FTPtCount<=1)
	{
		for(i=0;i<MAX_TARGET;i++)
		{
			MkPt[i].MapID=0;						MkPt[i].RTBID=0;				MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
			MkPt[i].FixDis=EACHBLOCKSIZE;			MkPt[i].FixCoor.x=0;			MkPt[i].FixCoor.y=0;
		}
		return ROUTE_NOFTPT;
	}
	else
	{	
		if(RouteCount > 0)
		{
			m_CalRouting.m_lReadBaseData = 0;
		}
		NaviData->NFTCCount=FTPtCount-1;
		for(i=0;i<FTPtCount-1;i++)
		{
			nuLONG l_RoutingRule = 0, RouteErrorCount = 0;
			for(RouteCounter_LCMM = 0; RouteCounter_LCMM < RouteCount; RouteCounter_LCMM++)
			{
				if(LCMM_RoutingRule[RouteCounter_LCMM] != -1)
				{
					l_RoutingRule = LCMM_RoutingRule[RouteCounter_LCMM];	
					tmp_pNaviFTClass=NULL;
					save_pNaviFTClass[RouteCounter_LCMM]=NULL;
					if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass[RouteCounter_LCMM]))
					{	
						ReNum=ROUTE_ERROR;	
						break;	
					}
					/*if(MkPt[i].FixCoor.x == MkPt[i + 1].FixCoor.x && MkPt[i].FixCoor.y == MkPt[i + 1].FixCoor.y)
					{
						continue;
					}*/
					ReNum = m_CalRouting.StartRouting(MkPt[i],MkPt[i+1],save_pNaviFTClass[RouteCounter_LCMM],b_MakeThr,l_RoutingRule);
					__android_log_print(ANDROID_LOG_INFO, "Route", "RouteCtrl!!!!ReNum....%d",ReNum);
					if(ReNum != ROUTE_SUCESS)//只要有一個規劃失敗，就全部都刪除且回傳規劃失敗
					{
						while(RouteCounter_LCMM > 0)
						{
							g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass[RouteErrorCount]);
							save_pNaviFTClass[RouteErrorCount] = NULL;
							RouteCounter_LCMM--;
							if(RouteCounter_LCMM == 0)
							{
								g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass[RouteErrorCount]);
								save_pNaviFTClass[RouteErrorCount] = NULL;
							}
						}
						g_pCRDC->Release_RoutingData(NaviData);
						NaviData = NULL;
						m_CalRouting.m_lReadBaseData = 0;
						return nuFALSE;
					}
				}
			}
			m_CalRouting.m_lReadBaseData = 0;
			for(RouteCounter_LCMM = 0; RouteCounter_LCMM < RouteCount; RouteCounter_LCMM++)
			{
			#ifdef _DRouteError
				MkPt[i+1].FixCoor.x = save_pNaviFTClass[RouteCounter_LCMM]->TCoor.x;
				MkPt[i+1].FixCoor.y = save_pNaviFTClass[RouteCounter_LCMM]->TCoor.y;
			#endif
				tmp_pNaviFTClass=NaviData->NFTC[RouteCounter_LCMM];
				if(tmp_pNaviFTClass==NULL){
					NaviData->NFTC[RouteCounter_LCMM]=save_pNaviFTClass[RouteCounter_LCMM];
				}
				else{
					while(tmp_pNaviFTClass->NextFT!=NULL){
						tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
					}
					tmp_pNaviFTClass->NextFT=save_pNaviFTClass[RouteCounter_LCMM];
				}
				tmp_pNaviFTClass=NULL;
				//save_pNaviFTClass[k]=NULL;
			}
			//---------------------------直接解開THR------------------------------------/
			/*nuFILE *fileout;
			nuTCHAR Path[100] = {0};
			nuGetModuleFileName(NULL, Path, NURO_MAX_PATH);
			for(int y = nuTcslen(Path) - 1; y >= 0; y--)
			{
				if( Path[y] == nuTEXT('\\') )
				{
					Path[y+1] = nuTEXT('\0');
					break;
				}
			}
			nuTcscat( Path, nuTEXT("openthrtime.TXT"));
			fileout = nuTfopen(Path,NURO_FS_AB);
			if(fileout == NULL)
			{
				return nuFALSE;
			}
			int starttime = nuGetTickCount();

			NAVIRTBLOCKCLASS *pTempNRBC;
			for(RouteCounter_LCMM = 0; RouteCounter_LCMM < RouteCount; RouteCounter_LCMM++)
			{
				pTempNRBC = NaviData->NFTC[RouteCounter_LCMM]->NRBC;
				while(pTempNRBC->NextRTB != NULL )
				{
					if(pTempNRBC->RTBStateFlag == 1)
					{
						LocalRoutingU1_LCMM( pTempNRBC );
					}
					pTempNRBC = pTempNRBC->NextRTB;
				}
			}
			
			int endtime = nuGetTickCount();
			int time = endtime - starttime;
			nuFwrite(&time, sizeof(nuLONG), 1,fileout);
			if(fileout != NULL)
			{
				nuFclose(fileout);
				fileout = NULL;
			}*/
			//---------------------------直接解開THR------------------------------------/
		}
	}

	// 規劃完畢 除了規劃完的資料之外 其餘的都進行移除或初始化
	for(i=0;i<MAX_TARGET;i++)
	{
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;			MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}
	m_CalRouting.m_lReadBaseData = 0;
	if(ReNum==ROUTE_SUCESS)	{	return ReNum;	}
	else					{	g_pCRDC->Release_RoutingData(NaviData);	return ReNum;	}
}
nuLONG RouteCtrl::ReRoute(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG l_RoutingRule)//重新規劃 
{
	NAVIFROMTOCLASS	*save_pNaviFTClass = NULL ,*tmp_pNaviFTClass = NULL ,*tmp_pNaviFTClass2 = NULL;
	NAVIRTBLOCKCLASS *tmp_pNaviRTBClass = NULL;
	nuLONG i= 0, RouteCondition = 0;
	nuLONG ReNum=ROUTE_SUCESS;

	m_CalRouting.b_ReRouting=nuTRUE;
	m_CalRouting.CarAngle=RideAngle;
	
	//先判斷目前已經跑到哪裡了
	//重新規劃到尚未通過的經由點(終點)

//----------for LCMM-----------------------//
	for(i = 0; i < m_lRouteCount; i++)
	{
		if(NaviData->NFTC[i] != NULL)
		{
			tmp_pNaviFTClass=NaviData->NFTC[i];
			RouteCondition = i;
			break;
		}
	}
	if(tmp_pNaviFTClass == NULL)
	{
		return 0;
	}
//----------for LCMM-----------------------//

	while(nuTRUE){
		if(tmp_pNaviFTClass==NULL) return ROUTE_ERROR;
		if(tmp_pNaviFTClass->RunPass==nuTRUE){
			tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
		}
		else{
			tmp_pNaviRTBClass=tmp_pNaviFTClass->NRBC;
			while(nuTRUE){
				if(tmp_pNaviRTBClass->NextRTB==NULL) break;
				tmp_pNaviRTBClass=tmp_pNaviRTBClass->NextRTB;
			}
			break;
		}
	}

	MkPt[1].FixCoor=tmp_pNaviRTBClass->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass->TInfo.RTBID;

	save_pNaviFTClass=NULL;
	if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass)){	ReNum=ROUTE_ERROR;	return ROUTE_ERROR;	}
	m_CalRouting.m_lRouteCount = 1;
	ReNum=m_CalRouting.StartRouting(MkPt[0],MkPt[1],save_pNaviFTClass,b_MakeThr,l_RoutingRule);
	
	for(i=0;i<MAX_TARGET;i++)
	{
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;		MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}
	//如果規劃成功 就把舊的取代掉
	//如果規劃失敗 則把新的清除
	if(ReNum==ROUTE_SUCESS){
		save_pNaviFTClass->NextFT=tmp_pNaviFTClass->NextFT;
		tmp_pNaviFTClass=NaviData->NFTC[RouteCondition];
		while(nuTRUE){
			if(tmp_pNaviFTClass->NextFT==NULL) break;
			if(tmp_pNaviFTClass->RunPass==nuFALSE) break;
			tmp_pNaviFTClass2=tmp_pNaviFTClass->NextFT;
			tmp_pNaviFTClass->NextFT=NULL;
			g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);//把走過的清掉
			tmp_pNaviFTClass=tmp_pNaviFTClass2;
		}
		g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);

		NaviData->NFTC[RouteCondition]=save_pNaviFTClass;
		return ReNum;	

	}
	else{
		g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass);
		return ReNum;
	}
}


nuLONG RouteCtrl::CongestionRoute(ROUTINGDATA *NaviData,nuBOOL b_MakeThr,nuLONG l_RoutingRule,nuLONG l_CongestionDis)//重新規劃
{
	NAVIFROMTOCLASS	*save_pNaviFTClass = NULL, *tmp_pNaviFTClass = NULL, *tmp_pNaviFTClass2 = NULL;
	NAVIRTBLOCKCLASS *tmp_pNaviRTBClass = NULL, *tmp_pNaviRTBClass2 = NULL;
	nuULONG i = 0;
	nuLONG ReNum=ROUTE_SUCESS;
	nuINT  RouteCounter_LCMM = 0;
	m_CalRouting.b_ReRouting=nuFALSE;

	m_CalRouting.CarAngle=RideAngle;
	//先判斷目前已經跑到哪裡了
	tmp_pNaviFTClass=NaviData->NFTC[RouteCounter_LCMM];
/*#ifdef DEBUG      Calculate Before ReRoute RoadLength
	NAVIRTBLOCKCLASS *pNaviRTBClass = NULL;
	NAVIFROMTOCLASS  *pNaviFTClass = NULL;
	nuLONG dis = 0;
	pNaviFTClass = tmp_pNaviFTClass;
	while(pNaviFTClass != NULL)
	{
		pNaviRTBClass = pNaviFTClass->NRBC;
		while(pNaviRTBClass != NULL)
		{
			dis += pNaviRTBClass->RealDis;
			pNaviRTBClass=pNaviRTBClass->NextRTB;
		}
		pNaviFTClass = pNaviFTClass->NextFT;
	}
#endif*/
	while(nuTRUE){
		if(tmp_pNaviFTClass==NULL) return ROUTE_ERROR;
		if(tmp_pNaviFTClass->RunPass==nuTRUE){
			tmp_pNaviFTClass=tmp_pNaviFTClass->NextFT;
		}
		else{
			tmp_pNaviRTBClass=tmp_pNaviFTClass->NRBC;
			while(nuTRUE){
				if(tmp_pNaviRTBClass->NextRTB==NULL) break;
				
				for(i=0;i<tmp_pNaviRTBClass->NSSCount;i++)
				{
					if( l_CongestionDis > (NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.x-MkPt[0].FixCoor.x) + NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.y-MkPt[0].FixCoor.y)) )
					{
						break;
					}
				}
				if(i!=tmp_pNaviRTBClass->NSSCount)	break;
				else								tmp_pNaviRTBClass=tmp_pNaviRTBClass->NextRTB;
			}
			break;
		}
	}

	//
/*	g_pCDTH->b_CongestionRoute=nuTRUE;
	for(i=0;i<tmp_pNaviRTBClass->NSSCount;i++)
	{
		if( l_CongestionDis > (NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.x-MkPt[0].FixCoor.x) + NURO_ABS(tmp_pNaviRTBClass->NSS[i].EndCoor.y-MkPt[0].FixCoor.y)) ){
			g_pCDTH->JoinNewCongestionID(tmp_pNaviRTBClass->NodeIDList[i+1]);
		}
	}*/

	/*MkPt[1].FixCoor=tmp_pNaviRTBClass->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass->TInfo.RTBID;*/

	tmp_pNaviRTBClass2=tmp_pNaviRTBClass;
	while(nuTRUE){
		if(tmp_pNaviRTBClass2->NextRTB==NULL) break;
		tmp_pNaviRTBClass2=tmp_pNaviRTBClass2->NextRTB;
	}

	MkPt[1].FixCoor=tmp_pNaviRTBClass2->TInfo.FixCoor;
	MkPt[1].FixDis=tmp_pNaviRTBClass2->TInfo.FixDis;
	MkPt[1].FixNode1=tmp_pNaviRTBClass2->TInfo.FixNode1;
	MkPt[1].FixNode2=tmp_pNaviRTBClass2->TInfo.FixNode2;
	MkPt[1].MapID=tmp_pNaviRTBClass2->TInfo.MapID;
	MkPt[1].RTBID=tmp_pNaviRTBClass2->TInfo.RTBID;

	save_pNaviFTClass=NULL;
	if(!g_pCRDC->Create_NaviFromToClass(save_pNaviFTClass)){	g_pCDTH->CleanCongestionID(); ReNum=ROUTE_ERROR;	return ROUTE_ERROR;	}
	ReNum=m_CalRouting.StartRouting(MkPt[0],MkPt[1],save_pNaviFTClass,b_MakeThr,l_RoutingRule);
	
	for(i=0;i<MAX_TARGET;i++){
		MkPt[i].MapID=0;				MkPt[i].RTBID=0;		MkPt[i].FixNode1=0;		MkPt[i].FixNode2=0;
		MkPt[i].FixDis=EACHBLOCKSIZE;	MkPt[i].FixCoor.x=0;		MkPt[i].FixCoor.y=0;
	}
	//如果規劃成功 就把舊的取代掉
	//如果規劃失敗 則把新的清除
	if(ReNum==ROUTE_SUCESS){
		save_pNaviFTClass->NextFT=tmp_pNaviFTClass->NextFT;
		tmp_pNaviFTClass=NaviData->NFTC[RouteCounter_LCMM];
		while(nuTRUE){
			if(tmp_pNaviFTClass->NextFT==NULL) break;
			if(tmp_pNaviFTClass->RunPass==nuFALSE) break;
			tmp_pNaviFTClass2=tmp_pNaviFTClass->NextFT;
			tmp_pNaviFTClass->NextFT=NULL;
			g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);
			tmp_pNaviFTClass=tmp_pNaviFTClass2;
		}
		g_pCRDC->Release_NaviFromToClass(tmp_pNaviFTClass);
		NaviData->NFTC[RouteCounter_LCMM]=save_pNaviFTClass;
		g_pCDTH->CleanCongestionID();

/*#ifdef DEBUG	Calculate After ReRoute RoadLength 	
		dis = 0;
		pNaviFTClass = NaviData->NFTC;
		while(pNaviFTClass != NULL)
		{
			pNaviRTBClass = pNaviFTClass->NRBC;
			while(pNaviRTBClass != NULL)
			{
				dis += pNaviRTBClass->RealDis;
				pNaviRTBClass=pNaviRTBClass->NextRTB;
			}
			pNaviFTClass = pNaviFTClass->NextFT;
		}
#endif*/
		return ReNum;
	}
	else{
		g_pCRDC->Release_NaviFromToClass(save_pNaviFTClass);
		g_pCDTH->CleanCongestionID();
		return ReNum;
	}
}
nuLONG RouteCtrl::LocalRoutingU1_LCMM(PNAVIRTBLOCKCLASS pNRBC, nuLONG FMapIdx, nuLONG TMapIdx){//區域規劃
	b_Drive=b_UseAngle;
	m_CalRouting.CarAngle=-1;
	m_CalRouting.b_ReRouting=nuFALSE;
	m_CalRouting.m_bTMCLocalRoute = nuFALSE;
	return m_CalRouting.LocalRouting(pNRBC,0,FMapIdx,TMapIdx);//因為採用THR直接讀出，不需要規劃條件
}
#ifdef _DTMC
nuLONG RouteCtrl::TMCLocalRoute(NAVIFROMTOCLASS *pNFTC, PNAVIRTBLOCKCLASS pNRBC,TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount, nuLONG RouteRule)
{
	__android_log_print(ANDROID_LOG_INFO, "TMC", "RouteCtrl !!! TMC LocalRoute START!!!!");
	nuBOOL bTMCRTB[20] = {nuFALSE}; 
	nuLONG renum = 0, i = 0;
	nuLONG TMCLocalRouteCounter = 0;
	m_CalRouting.m_bTMCLocalRoute = nuTRUE;
	m_CalRouting.m_lTMCDataCount = TMCDataCount;
	m_CalRouting.m_pTMCPassData = pTMCData;
	m_CalRouting.m_lRouteCount = 1;
	while(pNFTC != NULL)
	{
		pNRBC = pNFTC->NRBC;
		while(pNRBC != NULL)
		{
			if(pNRBC->RTBStateFlag != 0)
			{
				pNRBC = pNRBC->NextRTB;
			}
			else
			{
				//if(bTMCRTB[pNRBC->FInfo.RTBID])
				{
					if(TMCLocalRouteCounter == 0)
					{
						//nuMemcpy(&pNRBC->FInfo, &MkPt[0],sizeof(PTFIXTOROADSTRU));
						TMCLocalRouteCounter++;
					}
					if(pNRBC->FileIdx != MkPt[0].MapIdx || pNRBC->FInfo.RTBID != MkPt[0].RTBID )
					{
						__android_log_print(ANDROID_LOG_INFO, "TMC", "MAPIDX OR RTBID NOT THE SAME!!!!!");
						return 0;
					}
					renum = m_CalRouting.LocalRouting(pNRBC,RouteRule,pNRBC->FileIdx ,MkPt[0].MapIdx);
					//TMCLocalRouteCounter++;
				}
				//if(TMCLocalRouteCounter >= 2)
				{
					//m_CalRouting.m_bTMCLocalRoute = nuFALSE;
					//return renum;
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	m_CalRouting.m_bTMCLocalRoute = nuFALSE;
	__android_log_print(ANDROID_LOG_INFO, "TMC", "RouteCtrl !!! TMCLocalRoute1 =  %d",renum);
	return renum;
}
#endif
//------------added by daniel for LCMM-----------------------------------------------------//

nuBOOL RouteCtrl::InitMapIDtoMapIdx(){
	return m_CalRouting.InitMapIDtoMapIdx();
}

nuBOOL RouteCtrl::GetMRouteData(PNAVIFROMTOCLASS pNFTC,nuLONG l_RoutingRule){
	m_CalRouting.b_ReRouting=nuFALSE;
	m_CalRouting.InitMapIDtoMapIdx();
	m_CalRouting.GetMRouteData(pNFTC,l_RoutingRule);
	m_CalRouting.ReleaseMapIDtoMapIdx();
	return nuTRUE;
}

nuBOOL RouteCtrl::GetNextCrossData(nuLONG MapID,nuLONG BlockID,nuLONG RoadID,nuLONG CarAngle,NUROPOINT NowCoor,NUROPOINT TNodeCoor,CROSSFOLLOWED **stru_Cross){
	#ifdef VALUE_EMGRT
		*stru_Cross=NULL;
		return nuFALSE;
	#else
		PTFIXTOROADSTRU	nowMkPt;
		Class_PtFixToRoad m_PtFixToRoad;
		if(m_PtFixToRoad.NewPtFixToRoad(MapID,BlockID,RoadID,NowCoor,_MAXUNLONG,CarAngle,nuTRUE,nowMkPt)){
			return m_CalRouting.GetNextCrossData(nowMkPt,TNodeCoor,stru_Cross);
		}
		*stru_Cross=NULL;
		return nuFALSE;
	#endif
}

nuBOOL RouteCtrl::ReleaseNextCrossData(){
	#ifdef VALUE_EMGRT
		return nuFALSE;
	#else
		return m_CalRouting.ReleaseNextCrossData();
	#endif
}

nuVOID RouteCtrl::SetRideAngle(nuLONG lAngle)
{
	RideAngle=lAngle;
}
