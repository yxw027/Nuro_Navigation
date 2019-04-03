  // Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/06/09 10:44:19 $
// $Revision: 1.95 $

#include "stdAfxRoute.h"
#include "Class_CalRouting.h"
#include "AllSystemDataBase.h"
#include "ConstSet.h"


#define MaxNextCrossCount	30

Class_CalRouting::Class_CalRouting()
{
	m_nRTSTARTSAMECLASSROAD = 0;
	m_nRTFINDRTIDX		= 0;
	if(!nuReadConfigValue( "RTSTARTSAMECLASSROAD", &m_nRTSTARTSAMECLASSROAD))
	{
		m_nRTSTARTSAMECLASSROAD = 6000;
	}
	if(!nuReadConfigValue( "RTFINDRTIDX", &m_nRTFINDRTIDX))
	{
		m_nRTFINDRTIDX = 0;
	}
	if(!nuReadConfigValue( "RTSLEEP", &m_nRTSleep))
	{
	#ifdef ANDROID
		m_nRTSleep = TOBESLEEPTIMER;
	#else
		m_nRTSleep = 0;
	#endif
	}
	if(!nuReadConfigValue( "TMCSLOWWEIGHTING", &m_nRTSTARTSAMECLASSROAD))
	{
		m_nTMCSlowWeighting = 60;//speed is 20
	}
	if(!nuReadConfigValue( "TMCCROWDWEIGHTING", &m_nRTSTARTSAMECLASSROAD))
	{
		m_nTMCCrowdWeighting = 250; //speed is 5
	}
	m_MapIDtoMapIdx.MapIDList  = NULL;
	m_MapIDtoMapIdx.MapIdxList = NULL;
	m_MapIDtoMapIdx.MapCount   = 0;
	m_lReadBaseData            = 0;
	m_lRouteCount			   = 0;
#ifdef _DAR
	m_iARCount                 = 0;
#endif
	ClassWeighting=0;
	b_ReRouting=nuFALSE;
	
	Renum_RoutingState=ROUTE_ERROR;

#ifdef _DTMC
	m_pclsTmc  = NULL;
#endif

#ifdef _DDST
	m_pclsDst  = NULL;
#endif

#ifdef _DAR
	m_pclsAR   = NULL;
#endif
	m_pclsNT[0]= NULL;
	m_pclsNT[1]= NULL;
	m_pcls3dp  = NULL;
	m_pECI3dp  = NULL;
	m_pclsThc=NULL;
	m_pGThr[0]=NULL;
	m_pGThr[1]=NULL;
	m_pThr_F=NULL;
	m_pThr_T=NULL;
	m_pNT_F=NULL;
	m_pNT_T=NULL;
//#ifdef _USETTI
	m_pTTI_F = NULL;
	m_pTTI_T = NULL;
//#endif
	#if (defined VALUE_EMGRT) || (defined ZENRIN)
		m_PTS_F=NULL;
		m_PTS_T=NULL;
	#endif

	s_SaveCross.pRdSegList=NULL;
	s_SaveCross.nRdSegCount=0;
	m_bTMCLocalRoute = nuFALSE;
}

Class_CalRouting::~Class_CalRouting()
{
	ReleaseMapIDtoMapIdx();
//#ifdef _USETTI
	if(m_pTTI_F != NULL)
	{
		delete m_pTTI_F;
	}
	if(m_pTTI_T != NULL)
	{
		delete m_pTTI_T;
	}
//#endif
#ifdef _DTMC
	m_pclsTmc  = NULL;
#endif

#ifdef _DDST
	m_pclsDst  = NULL;
#endif

#ifdef _DAR
	m_pclsAR   = NULL;
#endif
	m_pclsNT[0]=NULL;
	m_pclsNT[1]=NULL;
	m_pcls3dp=NULL;
	m_pECI3dp= NULL;
	m_pclsThc=NULL;
	m_pGThr[0]=NULL;
	m_pGThr[1]=NULL;
	m_pThr_F=NULL;
	m_pThr_T=NULL;
//#ifdef _USETTI
	m_pTTI_F	= NULL;
	m_pTTI_T	= NULL;
//#endif
}

nuBOOL Class_CalRouting::InitClass()
{
	m_pclsNT[0]->InitClass();
	m_pclsNT[1]->InitClass();
	m_pcls3dp->InitClass();
	return nuTRUE;
}
nuVOID Class_CalRouting::CloseClass()
{
	m_pThr_F->CloseClass();//跨區檔(thr)的資料暫存區
	m_pThr_T->CloseClass();//跨區檔(thr)的資料暫存區
	m_pclsThc->CloseClass();
	m_pcls3dp->CloseClass();
	ReMoveSysNTData();
//#ifdef _USETTI
	ReleaseTTI();
//#endif
#ifdef _DDST
	ReMoveSysDstData();
#endif
#ifdef _DTMC
	ReMoveSysTmcData();
#endif
#ifdef _DAR
	ReMoveSysARData();
#endif
#ifdef THC_THR_LINK
	m_CMP.CloseClass();
#endif

}

//首次規劃 一定要跑這流程
nuLONG Class_CalRouting::StartRouting(PTFIXTOROADSTRU &FPt,PTFIXTOROADSTRU &TPt,PNAVIFROMTOCLASS pNFTC,nuBOOL b_MakeThr,nuLONG l_RoutingRule)
{
	/* debug 偵測時間用
		nuDWORD TickCount[5];
		TickCount[0] = nuGetTickCount();
	*/
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "b_MakeThr %d, l_RoutingRule %d",b_MakeThr, l_RoutingRule);
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "SPt MapIdx %d, MapID %d, RTBID %d, FixCoor.x %d, FixCoor.y %d, FixNode1 %d, FixNode2 %d, FixDis %d", FPt.MapIdx, FPt.MapID, FPt.RTBID, FPt.FixCoor.x, FPt.FixCoor.y, FPt.FixNode1, FPt.FixNode2, FPt.FixDis);
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "TPt MapIdx %d, MapID %d, RTBID %d, FixCoor.x %d, FixCoor.y %d, FixNode1 %d, FixNode2 %d, FixDis %d", TPt.MapIdx, TPt.MapID, TPt.RTBID, TPt.FixCoor.x, TPt.FixCoor.y, TPt.FixNode1, TPt.FixNode2, TPt.FixDis);	
	AStarWork		m_AStar;//AStar運算 核心物件
	TmpRoutingInfo	CLRS_F,CLRS_T;//起訖點整理過後的資料存放區
	if((FPt.FixCoor.x <= 0 || FPt.FixCoor.y <= 0) ||
		(TPt.FixCoor.x <= 0 || TPt.FixCoor.y <= 0))
	{
		return nuFALSE;
	}
	m_bTMCLocalRoute = nuFALSE;
	//沒有目標資料區 根本就不能處理
	if(pNFTC==NULL){
		LeaveStartRouting(m_AStar);
		return ROUTE_ERROR;
	}
	nuSleep(TOBESLEEPTIMER);
	//AStar路徑規劃法
	SetAstarBase(b_MakeThr);

	if(nuFALSE==g_pRoutingRule->ReadRoutingRule(l_RoutingRule)){
		LeaveStartRouting(m_AStar);
		return ROUTE_ERROR;
	}
	nuSleep(TOBESLEEPTIMER);

	if(m_nRTFINDRTIDX == 0) 
	{
		//取得MapID跟MapIdx的對照表
		if(nuFALSE==InitMapIDtoMapIdx()){
			LeaveStartRouting(m_AStar);
			return ROUTE_ERROR;
		}
	}
	nuSleep(TOBESLEEPTIMER);

	//判斷起訖點是否為同個點
	if( FPt.FixCoor.x==TPt.FixCoor.x &&
		FPt.FixCoor.y==TPt.FixCoor.y ){
		LeaveStartRouting(m_AStar);
		return ROUTE_SAMEPT;
	}
		
	pNFTC->FCoor.x=FPt.FixCoor.x;
	pNFTC->FCoor.y=FPt.FixCoor.y;
	pNFTC->TCoor.x=TPt.FixCoor.x;
	pNFTC->TCoor.y=TPt.FixCoor.y;
	pNFTC->FromFileID=FPt.MapID;
	pNFTC->ToFileID=TPt.MapID;
	pNFTC->FromRtBID=FPt.RTBID;
	pNFTC->ToRtBID=TPt.RTBID;
	pNFTC->RunPass=nuFALSE;
	
	
	if(m_lRouteCount == 1)
	{
		//重新整理一次起訖點資訊取得較佳的起訖導航Node
		if(nuFALSE==ColationDataAgain(FPt,TPt,CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule, FPt.MapIdx, TPt.MapIdx)){
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
	}
	nuSleep(TOBESLEEPTIMER);
	if(!m_AStar.m_AVLTree.CreateBaseAVLTreeArray()){
		LeaveStartRouting(m_AStar);
		return ROUTE_NEWASTARSTRUCTFALSE;
	}
	nuSleep(TOBESLEEPTIMER);
#ifdef LCMM
	if(m_lRouteCount >= 3)
	{
		m_lReadBaseData++;
		if(nuFALSE == CompareRTBIndex(FPt,TPt,CLRS_F,CLRS_T,b_MakeThr,FPt.MapIdx, TPt.MapIdx))
		{
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
		if(nuFALSE==ReadBaseData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule))//Read THR
		{
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
		m_lMaxRoadClassCount = 0;
		if(m_lReadBaseData == 1)
		{	
		#ifdef _DAR
			if( nuFALSE == ReadSysARData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule) )
			{
				LeaveStartRouting(m_AStar);
				return Renum_RoutingState;
			}
		#endif
			if( nuFALSE == ReadSysRTData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule))//讀取RTB的檔案
			{
				LeaveStartRouting(m_AStar);
				return Renum_RoutingState;
			}
			if(nuFALSE==ReadSysNTData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule))
			{
				LeaveStartRouting(m_AStar);
				return Renum_RoutingState;
			}
		}
		if(nuFALSE==ReFindPointNodeID(CLRS_F,CLRS_T,b_MakeThr))//這個應該不會消耗時間  所以多算應該沒關係 不然又要多存資料到class裡
		{
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
	}
	else if(m_lRouteCount == 1)
	{
	 #ifdef _DAR
		if( nuFALSE == ReadSysARData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule) )
		{
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
	#endif
		if( nuFALSE == ReadSysNTData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule) )
		{
			LeaveStartRouting(m_AStar);
			return Renum_RoutingState;
		}
	}
#else	
	nuSleep(TOBESLEEPTIMER);
	if(nuFALSE==ReadSysNTData(CLRS_F,CLRS_T,b_MakeThr,l_RoutingRule)){
		LeaveStartRouting(m_AStar);
		return Renum_RoutingState;
	}
#endif	
	
	//確認是否為同張地圖同個區塊且起訖點為同一點
	if( CLRS_F.PtLocalRoute.MapID==CLRS_T.PtLocalRoute.MapID &&
		CLRS_F.PtLocalRoute.RTBID==CLRS_T.PtLocalRoute.RTBID &&
		CLRS_F.PtLocalRoute.FixNode1==CLRS_T.PtLocalRoute.FixNode1 &&
		CLRS_F.PtLocalRoute.FixNode2==CLRS_T.PtLocalRoute.FixNode2 &&
		CLRS_F.PtLocalRoute.FixNode1==CLRS_F.PtLocalRoute.FixNode2	){
		LeaveStartRouting(m_AStar);
		return ROUTE_SAMEPT;
	}

#ifdef _DROUTE_ERROR
	if(!b_LocalRut)
	{
		m_bErrorExceptionHandle = nuFALSE;
		m_pShortestData = new AStarData[_DNEARTARGETNODECOUNT];
		if(m_pShortestData == NULL)
		{
			return nuFALSE;
		}
		m_pShortestweight = new WEIGHTINGSTRU[_DNEARTARGETNODECOUNT];
		if(m_pShortestweight == NULL)
		{
			return nuFALSE;
		}
		m_lShortestCount = 0;
		m_lShortestDis = 0;
	}
#endif

	if(nuFALSE==AStar(m_AStar,CLRS_F,CLRS_T,l_RoutingRule)){
		LeaveStartRouting(m_AStar);
		return Renum_RoutingState;
	}
	nuSleep(TOBESLEEPTIMER);

#ifdef _DROUTE_ERROR
	if(!b_LocalRut)
	{
		if(m_pShortestData != NULL)
		{
			delete [] m_pShortestData;
			m_pShortestData = NULL;
		}
		if(m_pShortestweight != NULL)
		{
			delete [] m_pShortestweight;
			m_pShortestweight = NULL;
		}
		m_lShortestCount = 0;
		m_lShortestDis = 0;
	}
#endif

#ifdef LCMM
	if(m_lRouteCount >= 3)
	{
		if(m_lReadBaseData >= 3)
		{
			ReMoveSysNTData();
		}
	}
	else if(m_lRouteCount == 1)
	{
		ReMoveSysNTData();
	}
#else
		ReMoveSysNTData();
#endif
	nuSleep(TOBESLEEPTIMER);
		
	//整理Close端的Node資訊 並清除AStar過程所產生的暫存資料
	if(!GetNodeIDList(m_AStar,CLRS_F,CLRS_T)){
		LeaveStartRouting(m_AStar);
		return ROUTE_APPROACHFALSE;
	}
	m_AStar.IniData();
	m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
	//產生最終的規劃結構並填充資料
	if(!CreateRoutingData(pNFTC)){
		LeaveStartRouting(m_AStar);
		return ROUTE_CREATEROUTEDATAFASLE;
	}
		
	nuSleep(TOBESLEEPTIMER);
	if(!AStarSuccessWork(pNFTC)){
		LeaveStartRouting(m_AStar);
		return Renum_RoutingState;
	}
	nuSleep(TOBESLEEPTIMER);
#ifdef LCMM
	if(m_lRouteCount >= 3)
	{
		if(m_lReadBaseData >= 3)
		{
			m_lReadBaseData = 0;
			LeaveStartRouting(m_AStar);//將規劃好的資料傳出 並將內部清理乾淨 
		}
		else
		{
			LeaveStartRouting_ForMultiConditionRoute(m_AStar);
		}
	}
	else if(m_lRouteCount == 1)
	{
		LeaveStartRouting(m_AStar);//將規劃好的資料傳出 並將內部清理乾淨 
	}
#else
	LeaveStartRouting(m_AStar);//將規劃好的資料傳出 並將內部清理乾淨 
#endif
	return ROUTE_SUCESS;
}

nuBOOL Class_CalRouting::GetMRouteData(PNAVIFROMTOCLASS pNFTC,nuLONG l_RoutingRule)
{
	if(pNFTC==NULL) return nuFALSE;
	//驗證thc檔初次解開
	//先找出thc所在
	PNAVIRTBLOCKCLASS	pTmpNaviRTBClass,pLastNaviRTBClass,pTHCNaviRTBClass[256] = {0},pSaveNaviRTBClass[256] = {0};
	PNAVIFROMTOCLASS	ptmpNFTC,pNAVIFROMTOCLASS[256] = {0};
	pLastNaviRTBClass=NULL;
	nuINT RTBCounter = 0, THCCounter =0, NFTCCounter = 0; 
	//讀取權重參數
	if(!g_pRoutingRule->ReadRoutingRule(l_RoutingRule))
	{
		g_pRoutingRule->ReleaseRoutingRule(); 
		return nuFALSE;
	}
	ptmpNFTC=pNFTC;
	while(ptmpNFTC)
	{
		pNAVIFROMTOCLASS[NFTCCounter] = ptmpNFTC;
		NFTCCounter++;
		pTmpNaviRTBClass  = ptmpNFTC->NRBC;
		while(pTmpNaviRTBClass!=NULL)
		{
			pSaveNaviRTBClass[RTBCounter] = pTmpNaviRTBClass;
			RTBCounter++;
			if(pTmpNaviRTBClass->RTBStateFlag==2)
			{
				if(nuFALSE==m_calmrt.MRTRouting(pTmpNaviRTBClass,pTmpNaviRTBClass->FInfo.FixNode1,pTmpNaviRTBClass->TInfo.FixNode1,l_RoutingRule)){//測試函數功能
					g_pRoutingRule->ReleaseRoutingRule();
					return nuFALSE;
				}
				else
				{
					pLastNaviRTBClass = pTmpNaviRTBClass->NextRTB;
					pTHCNaviRTBClass[THCCounter]  = pTmpNaviRTBClass;
					THCCounter++;
//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
//先將THC解開前的路徑長暫時存入解開後RTB的路徑長，因為解開後的THR都沒有路徑長，會導致重新規劃後的路俓規劃出問題
					pLastNaviRTBClass->RealDis = pTmpNaviRTBClass->RealDis;
					pLastNaviRTBClass->WeightDis = pTmpNaviRTBClass->WeightDis;
//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
					pTmpNaviRTBClass = pLastNaviRTBClass;
					RTBCounter--;
					continue;
				}
			}
			pTmpNaviRTBClass = pTmpNaviRTBClass->NextRTB;
		}
		for(nuINT j = 0; j < THCCounter; j++)
		{
			g_pCRDC->Release_NaviRtBlockClass(pTHCNaviRTBClass[j]);
			pTHCNaviRTBClass[j] = NULL;
		}
		for(nuINT i = 0; i + 1 < RTBCounter; i++)
		{
			pSaveNaviRTBClass[i]->NextRTB = pSaveNaviRTBClass[i + 1];
		}
		ptmpNFTC->NRBC = pSaveNaviRTBClass[0];
		RTBCounter = 0;
		THCCounter = 0;
		ptmpNFTC = ptmpNFTC->NextFT;
	}
	for(nuINT z = 0; z + 1 < NFTCCounter; z++)
	{
		pNAVIFROMTOCLASS[z]->NextFT = pNAVIFROMTOCLASS[z + 1];
	}
	pNFTC = pNAVIFROMTOCLASS[0];
	/*if(pNFTC==NULL) return nuFALSE;
	//驗證thc檔初次解開
	//先找出thc所在
	PNAVIRTBLOCKCLASS	pTmpNaviRTBClass,pLastNaviRTBClass;
	PNAVIFROMTOCLASS	ptmpNFTC;
	pLastNaviRTBClass=NULL;

	//讀取權重參數
	if(!g_pRoutingRule->ReadRoutingRule(l_RoutingRule))
	{
		g_pRoutingRule->ReleaseRoutingRule(); 
		return nuFALSE;
	}
	
	ptmpNFTC=pNFTC;
	//安插資料
	//081127 處理出來的資料有問題 RTB之間的座標對應不起來
	while(ptmpNFTC)
	{
		pTmpNaviRTBClass=ptmpNFTC->NRBC;
		while(pTmpNaviRTBClass!=NULL){
			if(pTmpNaviRTBClass->RTBStateFlag==2){
				if(nuFALSE==m_calmrt.MRTRouting(pTmpNaviRTBClass,pTmpNaviRTBClass->FInfo.FixNode1,pTmpNaviRTBClass->TInfo.FixNode1,l_RoutingRule)){//測試函數功能
					g_pRoutingRule->ReleaseRoutingRule();
					return nuFALSE;
				}
				if(pLastNaviRTBClass!=NULL){
					pLastNaviRTBClass->NextRTB=pTmpNaviRTBClass->NextRTB;
					pTmpNaviRTBClass->NextRTB=NULL;
					(*g_pCRDC)->Release_NaviRtBlockClass(pTmpNaviRTBClass);
				}
				break;
			}
			pLastNaviRTBClass=pTmpNaviRTBClass;
			pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
		}
		ptmpNFTC=ptmpNFTC->NextFT;
	}*/

	//移除規範檔內容
	g_pRoutingRule->ReleaseRoutingRule();
	return nuTRUE;
}


nuLONG Class_CalRouting::LocalRouting(PNAVIRTBLOCKCLASS pNRBC,nuLONG l_RoutingRule, nuLONG FMapIdx, nuLONG TMapIdx)
{
nuDWORD TickCount[2];
TickCount[0] = nuGetTickCount();
	AStarWork		m_AStar;//AStar運算 核心物件
	TmpRoutingInfo CLRS_F,CLRS_T;//起訖點整理過後的資料存放區
#ifndef THRNodeList
	if(pNRBC==NULL){
		LeaveLocalRouting(m_AStar);
		return ROUTE_ERROR;
	}
	if(nuFALSE==g_pRoutingRule->ReadRoutingRule(l_RoutingRule)){
		LeaveLocalRouting(m_AStar);
		return ROUTE_ERROR;
	}
#endif
	nuSleep(TOBESLEEPTIMER);
	if(m_nRTFINDRTIDX == 0 || (FMapIdx == -1 || TMapIdx == -1)) 
	{
		if(nuFALSE==InitMapIDtoMapIdx()){//取得MapID跟MapIdx的對照表
			LeaveLocalRouting(m_AStar);
			return ROUTE_ERROR;
		}
	}
	if(nuFALSE==ColationDataAgain(pNRBC->FInfo,pNRBC->TInfo,CLRS_F,CLRS_T,nuTRUE,l_RoutingRule, FMapIdx, TMapIdx)){	LeaveLocalRouting(m_AStar);	return ROUTE_OUTROUTELIMIT;	}
/*#ifdef _DTMC
	if(m_bTMCLocalRoute)
	{
		nuBOOL bTMCRTB[20] = {nuFALSE};
		ReadSysTmcData(CLRS_T,m_pTMCPassData,m_lTMCDataCount, bTMCRTB);
		if(!bTMCRTB[pNRBC->FInfo.RTBID])
		{
			LeaveLocalRouting(m_AStar);
			return ROUTE_ERROR;
		}
	}
#endif*/
	nuSleep(TOBESLEEPTIMER);
#ifdef _USERT6
	bCheckRT6(pNRBC);
#endif
#ifndef THRNodeList
		if(!m_AStar.m_AVLTree.CreateBaseAVLTreeArray()){	LeaveLocalRouting(m_AStar);	return ROUTE_NEWASTARSTRUCTFALSE;	}
		if(nuFALSE==ReadSysNTData(CLRS_F,CLRS_T,nuTRUE,l_RoutingRule)){	LeaveLocalRouting(m_AStar);	return Renum_RoutingState;		}
//#ifdef _USETTI
		if(nuFALSE == ReadTTI(CLRS_F, CLRS_T)){	LeaveStartRouting(m_AStar);	 return Renum_RoutingState;	}
//#endif
	#ifdef _DDst
		if(nuFALSE==ReadSysDstData(CLRS_F,CLRS_T,nuTRUE,l_RoutingRule)){	LeaveLocalRouting(m_AStar);	return Renum_RoutingState;		}
	#endif
	nuSleep(TOBESLEEPTIMER);

	SetAstarBase(nuTRUE);
	if(nuFALSE==AStar(m_AStar,CLRS_F,CLRS_T,l_RoutingRule)){	LeaveLocalRouting(m_AStar);	return nuFALSE;		}
	nuSleep(TOBESLEEPTIMER);
	ReMoveSysNTData();
//#ifdef _USETTI
	ReleaseTTI();
//#endif
	#ifdef _DDst
		ReMoveSysDstData();
	#endif
	/*#ifdef _DTMC
		ReMoveSysTmcData();
	#endif*/
		if(!GetNodeIDList(m_AStar,CLRS_F,CLRS_T)){	LeaveLocalRouting(m_AStar);	return ROUTE_ERROR;		}
		m_AStar.IniData();
		m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
#else	
	GetTHRNodeIDList(pNRBC->FInfo.MapID ,pNRBC->FInfo.FixNode1, pNRBC->TInfo.FixNode1, pNRBC->FInfo.RTBID);
#endif
	nuSleep(TOBESLEEPTIMER);
	CreateLocalRoutingData(pNRBC);
	nuSleep(TOBESLEEPTIMER);
	pNRBC->RTBStateFlag=1;

	if(!LocalAStarSuccessWorkStep1(pNRBC)){
		g_pCRDC->Release_NaviSubClass(pNRBC);
		g_pCRDC->Release_NodeIDList(pNRBC);
		LeaveLocalRouting(m_AStar);	
		return ROUTE_ERROR;	
	}
	nuSleep(TOBESLEEPTIMER);
#ifdef ECI
	m_pECI3dp = new CCtrl_ECI;
	if(NULL == m_pECI3dp)
	{
		Renum_RoutingState=ROUTE_SUCCESSSTEP2;
		return nuFALSE;
	}
	else
	{
		m_pECI3dp->InitClass();				
		nuTcscpy(m_pECI3dp->ECIFName, nuTEXT(".ECI"));
		g_pRtFSApi->FS_FindFileWholePath(pNRBC->FileIdx,m_pECI3dp->ECIFName,NURO_MAX_PATH);
		if(!m_pECI3dp->ReadFile())
		{
			Renum_RoutingState=ROUTE_SUCCESSSTEP2;
			return nuFALSE;
		}
	}
#endif
	if(!LocalAStarSuccessWorkStep2(pNRBC)){	
		g_pCRDC->Release_NaviSubClass(pNRBC);
		g_pCRDC->Release_NodeIDList(pNRBC);
		g_pDB_DwCtrl->Reset();
		g_pUOneBH->ReleaseBH();
		m_pcls3dp->ReleaseFile();
		LeaveLocalRouting(m_AStar);
		return ROUTE_ERROR;	
	}
#ifdef ECI
	if(NULL!=m_pECI3dp)
	{
	delete m_pECI3dp;
	m_pECI3dp = NULL;
	}
#endif
	nuSleep(TOBESLEEPTIMER);
	g_pDB_DwCtrl->Reset();
	g_pUOneBH->ReleaseBH();
	m_pcls3dp->ReleaseFile();
	if(!LocalAStarSuccessWorkStep3(pNRBC)){
		g_pCRDC->Release_NaviSubClass(pNRBC);
		g_pCRDC->Release_NodeIDList(pNRBC);
		LeaveLocalRouting(m_AStar);
		return ROUTE_ERROR;	
	}
	nuSleep(TOBESLEEPTIMER);

	//最後要留下pNFTC的資訊 其他的資料要完全清空 恢復原貌
	LeaveLocalRouting(m_AStar);//這個class要讓外界承接 內部則不進行掌控
TickCount[1] = nuGetTickCount();

/*
FILE *fff;
nuTcscpy((*ppclsThc)->FName, nuTEXT(".txt"));
g_pRtFSApi->FS_FindFileWholePath(-1,(*ppclsThc)->FName,NURO_MAX_PATH);
fff=_wfopen((*ppclsThc)->FName,L"a");
fprintf(fff,"%ld\r\n",TickCount[1]-TickCount[0]);
fclose(fff);
*/
	pNRBC->RTBStateFlag=0;
	return ROUTE_SUCESS;
}

nuVOID Class_CalRouting::LeaveLocalRouting(AStarWork &m_AStar)
{
	//清除TmpApproach的資料
		Tmp_Approach.ReleaseApproach();
	//移除禁轉資料
		ReMoveSysNTData();
//#ifdef _USETTI
		ReleaseTTI();
//#endif
	//移除即時狀況資料
#ifdef _DDST
		ReMoveSysDstData();
#endif
#ifdef _DTMC
		ReMoveSysTmcData();
#endif
	//清理AStar
		m_AStar.IniData();
	//清理規畫中所用到的Thr與RtB Class
		ReMoveBaseData();
	//移除MapID與MapIdx對應表
		ReleaseMapIDtoMapIdx();
	//移除規劃規則
		g_pRoutingRule->ReleaseRoutingRule();
}

nuBOOL Class_CalRouting::InitMapIDtoMapIdx()
{
	nuULONG i;
	UONERtCtrl TmpRtCtrl;
	ReleaseMapIDtoMapIdx();
	/*** 創建MapId與MapIdx的對應表 並給予初始化 ***/
	#ifdef DEF_MAKETHRROUTE
		m_MapIDtoMapIdx.MapCount = 1;//m_Lib_FileName.GetMainNameCount();
	#else//讀取地圖數量
		m_MapIDtoMapIdx.MapCount = g_pRtFSApi->FS_GetMapCount();//m_Lib_FileName.GetMainNameCount();
	#endif//建立地圖表單結構
	m_MapIDtoMapIdx.MapIDList	=	(nuULONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuULONG)*m_MapIDtoMapIdx.MapCount);
	m_MapIDtoMapIdx.MapIdxList	=	(nuULONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuULONG)*m_MapIDtoMapIdx.MapCount);

	if(m_MapIDtoMapIdx.MapIDList==NULL || m_MapIDtoMapIdx.MapIdxList==NULL){	ReleaseMapIDtoMapIdx();	return nuFALSE;	}
	//初始化地圖結構
	for(i=0;i<m_MapIDtoMapIdx.MapCount;i++){	(*m_MapIDtoMapIdx.MapIDList)[i] = 0;	(*m_MapIDtoMapIdx.MapIdxList)[i] = 0;	}

	/*** 2.	讀取Rt檔的表頭部份 取得MapID資訊 並搭配MapIdx 將對應表建構起來 ***/
	for(i=0;i<m_MapIDtoMapIdx.MapCount;i++)
	{
		#ifdef DEF_MAKETHRROUTE
			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
			nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
		#else
			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));//Get Pdw file path
			g_pRtFSApi->FS_FindFileWholePath(i,TmpRtCtrl.FName,NURO_MAX_PATH);
		#endif
		if(nuFALSE==TmpRtCtrl.Read_Header()){	TmpRtCtrl.ReSet();	continue;	}//讀取RT的Header
		(*m_MapIDtoMapIdx.MapIdxList)[i]=i;//取得地圖索引
		(*m_MapIDtoMapIdx.MapIDList)[i]=TmpRtCtrl.MapID;//取得地圖編號
		TmpRtCtrl.ReSet();//RT移除表頭
	}
	return nuTRUE;
}

nuVOID Class_CalRouting::ReleaseMapIDtoMapIdx()
{
	if(m_MapIDtoMapIdx.MapIdxList!=NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_MapIDtoMapIdx.MapIdxList);
		m_MapIDtoMapIdx.MapIdxList=NULL;
	}
	if(m_MapIDtoMapIdx.MapIDList!=NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_MapIDtoMapIdx.MapIDList);
		m_MapIDtoMapIdx.MapIDList=NULL;	
	}
	m_MapIDtoMapIdx.MapCount=0;
}

nuVOID Class_CalRouting::LeaveStartRouting_ForMultiConditionRoute(AStarWork &m_AStar)
{
		Tmp_Approach.ReleaseApproach();//清除TmpApproach的資料
		m_AStar.IniData();//清理AStar
		m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
		ReleaseMapIDtoMapIdx();//移除MapID與Mapidx的對應
		ReMoveThrData();//移除THR
		g_pRoutingRule->ReleaseRoutingRule();//移除規範檔內容
}

nuVOID Class_CalRouting::LeaveStartRouting(AStarWork &m_AStar)
{
	//清除TmpApproach的資料
		Tmp_Approach.ReleaseApproach();
	//移除禁轉資料
		ReMoveSysNTData();
//#ifdef _USETTI
		ReleaseTTI();
//#endif
    //移除即時路況資料
#ifdef _DDST
		ReMoveSysDstData();
#endif
#ifdef _DTMC
	ReMoveSysTmcData();
#endif
#ifdef _DAR
		ReMoveSysARData();
#endif
	//清理AStar
		m_AStar.IniData();
		m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
	//清理規畫中所用到的Thr與RtB Class
		ReMoveBaseData();
	//移除MapID與Mapidx的對應
		ReleaseMapIDtoMapIdx();
	//移除規範檔內容
		g_pRoutingRule->ReleaseRoutingRule();
	//
#ifdef THC_THR_LINK
		m_CMP.CloseClass();
#endif
}

nuBOOL Class_CalRouting::CreateRoutingData(PNAVIFROMTOCLASS TmpNaviFromToClass)//進來的資料是起訖點的資料
{
	nuLONG i,TmpFConnectIndex,TmpTConnectIndex;
	nuULONG j,TmpFConnectCount,TmpTConnectCount,TmpMapLayer,TmpListCount=0,TmpRtBCount=0;
	PNAVIRTBLOCKCLASS pSaveNaviRTBClass,pTmpNaviRTBClass;
	ApproachStru **tmpp_Approach;
	tmpp_Approach=Tmp_Approach.p_Approach;
	//先統計要創建多少資料結構

	pSaveNaviRTBClass = NULL;//存入正在運算的資料
	pTmpNaviRTBClass  = NULL;//拿來做比較用的
	//確定現有資料所需的所有物件
	TmpListCount = 0;			
	TmpRtBCount  = 0;
	TmpMapLayer  = _MAPLAYER_DEF;
	for(i=0;i<Tmp_Approach.ApproachCount;i++){		
		if((*tmpp_Approach)[i].FMapLayer==(*tmpp_Approach)[i].TMapLayer)
		{//如果道路的起訖點位於同個圖層
			if(	(*tmpp_Approach)[i].FMapLayer == _MAPLAYER_STHR	|| //跨區處理
				(*tmpp_Approach)[i].FMapLayer ==  _MAPLAYER_THC		||
				(*tmpp_Approach)[i].FMapLayer == _MAPLAYER_TTHR)
			{//這是很重要的分隔
				//其實這裡的動作都一樣，只是判別NRBC是否為空的，是空的就加入新的結構，重點在於統計同個圖層的節點數和子道路數
				TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;

				if(TmpNaviFromToClass->NRBC==NULL)//判斷pSaveNaviRTBClass是否為零
				{
					pSaveNaviRTBClass=NULL;
					g_pCRDC->Create_NaviRtBlockClass(pSaveNaviRTBClass);
					TmpNaviFromToClass->NRBC=pSaveNaviRTBClass;
				}
				else
				{
					pSaveNaviRTBClass=NULL;//將pSaveNaviRTBClass的值清為0，所以值目前只保留在TmpNaviFromToClass->NRBC
					g_pCRDC->Create_NaviRtBlockClass(pSaveNaviRTBClass);
					pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
					while(pTmpNaviRTBClass!=NULL)
					{//將pTmpNaviRTBClass尾端的資料變成pSaveNaviRTBClass
						if(pTmpNaviRTBClass->NextRTB==NULL)
						{
							pTmpNaviRTBClass->NextRTB=pSaveNaviRTBClass;
							break;
						}
						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
					}
				}
			}
			else
			{
				if(TmpMapLayer==(*tmpp_Approach)[i].TMapLayer)//RT MapLayer 
				{//統計同個NodeID數量
					pSaveNaviRTBClass->NodeIDCount++;
					pSaveNaviRTBClass->NSSCount++;
				}
				else{
					if(TmpNaviFromToClass->NRBC==NULL)
					{
						pSaveNaviRTBClass=NULL;
						g_pCRDC->Create_NaviRtBlockClass(pSaveNaviRTBClass);
						TmpNaviFromToClass->NRBC=pSaveNaviRTBClass;
					}
					else
					{
						pSaveNaviRTBClass=NULL;
						g_pCRDC->Create_NaviRtBlockClass(pSaveNaviRTBClass);
						pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
						while(pTmpNaviRTBClass!=NULL)
						{
							if(pTmpNaviRTBClass->NextRTB==NULL)
							{
								pTmpNaviRTBClass->NextRTB=pSaveNaviRTBClass;
								break;
							}
							pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
						}
					}
					TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;
					pSaveNaviRTBClass->NodeIDCount=2;
					pSaveNaviRTBClass->NSSCount=1;
				}
			}
		}
	}

	pTmpNaviRTBClass=NULL;
	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
	while(pTmpNaviRTBClass!=NULL)//建造NodeIDList和NSSCount的結構
	{
		if(pTmpNaviRTBClass->NodeIDCount!=0 && pTmpNaviRTBClass->NSSCount!=0)
		{
			g_pCRDC->Create_NodeIDList(pTmpNaviRTBClass,pTmpNaviRTBClass->NodeIDCount);//賦予NodeIDList記憶體，並複製NodeIDcount
			g_pCRDC->Create_NaviSubClass(pTmpNaviRTBClass,pTmpNaviRTBClass->NSSCount);//賦予NSSCount記憶體，並複製NSSCount
		}
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
	pTmpNaviRTBClass=NULL;

	//開始填資料
	TmpRtBCount  = 0;			
	TmpListCount = 0;
	TmpMapLayer  = _MAPLAYER_DEF;
	for(i=0;i<Tmp_Approach.ApproachCount;i++){
		if((*tmpp_Approach)[i].FMapLayer==(*tmpp_Approach)[i].TMapLayer)
		{//前後同層 才表示在同一平面
			if( (*tmpp_Approach)[i].FMapLayer == _MAPLAYER_TTHR )
			{//這是很重要的分隔
				TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;
				TmpListCount=0;
				if(pTmpNaviRTBClass==NULL)	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;//判斷是否為串列頭
				else						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
				//填充起訖連接點資訊
				TmpFConnectIndex=(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
				TmpTConnectIndex=(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].ConnectStruIndex;
				TmpFConnectCount=(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].ConnectCount;
				TmpTConnectCount=(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].ConnectCount;
				for(j=0;j<TmpFConnectCount;j++){
					if( (*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TMapLayer==2 &&
						(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID==(*tmpp_Approach)[i].TNodeID	&&
						(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID==(*tmpp_Approach)[i].TRtBID	){

						pTmpNaviRTBClass->WeightDis	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].WeightingLength;
						pTmpNaviRTBClass->RealDis	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].Length;
						pTmpNaviRTBClass->FileIdx	=	m_pThr_T->ThrHeader.MapID;
						pTmpNaviRTBClass->RTBIdx	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->NSSCount	=	0;
					}
				}					
				for(j=0;j<TmpFConnectCount;j++)
				{
					if(	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TMapLayer==1 &&
						(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID==pTmpNaviRTBClass->RTBIdx )
					{
						pTmpNaviRTBClass->FInfo.MapID		=	m_pThr_T->ThrHeader.MapID;
						pTmpNaviRTBClass->FInfo.RTBID		=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->FInfo.FixNode1	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->FInfo.FixNode2	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->FInfo.FixCoor		=	(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor;
						pTmpNaviRTBClass->FInfo.FixDis		=	0;
					}
				}
				for(j=0;j<TmpTConnectCount;j++)
				{
					if(	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TRtBID==pTmpNaviRTBClass->RTBIdx &&
						(*m_pThr_T->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TMapLayer==1 )
					{
						pTmpNaviRTBClass->TInfo.MapID		=	m_pThr_T->ThrHeader.MapID;
						pTmpNaviRTBClass->TInfo.RTBID		=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->TInfo.FixNode1	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->TInfo.FixNode2	=	(*m_pThr_T->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->TInfo.FixCoor		=	(*m_pThr_T->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor;
						pTmpNaviRTBClass->TInfo.FixDis		=	0;
					}
				}
				pTmpNaviRTBClass->RTBStateFlag	=	1;
			}
			else if((*tmpp_Approach)[i].FMapLayer == _MAPLAYER_THC){//這是很重要的分隔
				
				#ifdef	DEFNEWTHCFORMAT
					TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;
					TmpListCount=0;
					if(pTmpNaviRTBClass==NULL)	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
					else						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;

					TmpFConnectIndex=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
					if((*tmpp_Approach)[i].FNodeID==m_pclsThc->TotalFNodeCount-1)
					{
						TmpFConnectCount=m_pclsThc->TotalConnectCount-(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
					}
					else
					{
						TmpFConnectCount=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID+1].ConnectStruIndex-(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
					}

					if(m_pclsThc->ReadConnect(TmpFConnectIndex,TmpFConnectCount)){
						for(j=0;j<TmpFConnectCount;j++){
							if( (*m_pclsThc->p_StruConnect)[j].TMapLayer==3 &&
								(*m_pclsThc->p_StruConnect)[j].TNodeID==(*tmpp_Approach)[i].TNodeID ){
								pTmpNaviRTBClass->WeightDis	=	(*m_pclsThc->p_StruConnect)[j].WeightingLength*CONST_UNITOFTHCDIS;
								pTmpNaviRTBClass->RealDis	=	(*m_pclsThc->p_StruConnect)[j].Length*CONST_UNITOFTHCDIS;
								pTmpNaviRTBClass->FileIdx	=	0;
								pTmpNaviRTBClass->RTBIdx	=	0;
								pTmpNaviRTBClass->NSSCount	=	0;
								//拿這兩個參數來權充ViaNodeData的起始位置跟個數
								pTmpNaviRTBClass->FInfo.FixNode1	=	(*m_pclsThc->p_StruConnect)[j].lViaCityIndex;
								pTmpNaviRTBClass->FInfo.FixNode2	=	(*m_pclsThc->p_StruConnect)[j].lViaCityIndex;
								pTmpNaviRTBClass->TInfo.FixNode1	=	(*m_pclsThc->p_StruConnect)[j].lViaCityCount;
								pTmpNaviRTBClass->TInfo.FixNode2	=	(*m_pclsThc->p_StruConnect)[j].lViaCityCount;
							}
						}
					}
					else{
						return nuFALSE;
					}
					pTmpNaviRTBClass->FInfo.MapID		=	0;
					pTmpNaviRTBClass->FInfo.RTBID		=	0;
					pTmpNaviRTBClass->FInfo.FixCoor.x	=	0;
					pTmpNaviRTBClass->FInfo.FixCoor.y	=	0;
					pTmpNaviRTBClass->FInfo.FixDis		=	0;
					
					pTmpNaviRTBClass->TInfo.MapID		=	0;
					pTmpNaviRTBClass->TInfo.RTBID		=	0;
					pTmpNaviRTBClass->TInfo.FixCoor.x	=	0;
					pTmpNaviRTBClass->TInfo.FixCoor.y	=	0;
					pTmpNaviRTBClass->TInfo.FixDis		=	0;
					pTmpNaviRTBClass->RTBStateFlag		=	2;
				#else
// */
					TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;
					TmpListCount=0;
					if(pTmpNaviRTBClass==NULL)	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
					else						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;

					TmpFConnectIndex=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
					TmpTConnectIndex=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].TNodeID].ConnectStruIndex;
					TmpFConnectCount=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].ConnectCount;
					TmpTConnectCount=(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].TNodeID].ConnectCount;

					if((*ppclsThc)->ReadConnect(TmpFConnectIndex,TmpFConnectCount)){
						for(j=0;j<TmpFConnectCount;j++){
							if( (*m_pclsThc->p_StruConnect)[j].TMapLayer==3 &&
								(*m_pclsThc->p_StruConnect)[j].TNodeID==(*tmpp_Approach)[i].TNodeID ){
								pTmpNaviRTBClass->WeightDis	=	(*m_pclsThc->p_StruConnect)[j].WeightingLength*CONST_UNITOFTHCDIS;
								pTmpNaviRTBClass->RealDis	=	(*m_pclsThc->p_StruConnect)[j].Length*CONST_UNITOFTHCDIS;
								pTmpNaviRTBClass->FileIdx	=	0;
								pTmpNaviRTBClass->RTBIdx	=	0;
								pTmpNaviRTBClass->NSSCount	=	0;
							}
						}
					}
					else{
						return nuFALSE;
					}
					pTmpNaviRTBClass->FInfo.MapID		=	0;
					pTmpNaviRTBClass->FInfo.RTBID		=	0;
					pTmpNaviRTBClass->FInfo.FixNode1	=	(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].MRTID;
					pTmpNaviRTBClass->FInfo.FixNode2	=	(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].FNodeID].MRTID;
					pTmpNaviRTBClass->FInfo.FixCoor.x	=	0;
					pTmpNaviRTBClass->FInfo.FixCoor.y	=	0;
					pTmpNaviRTBClass->FInfo.FixDis		=	0;
					
					pTmpNaviRTBClass->TInfo.MapID		=	0;
					pTmpNaviRTBClass->TInfo.RTBID		=	0;
					pTmpNaviRTBClass->TInfo.FixNode1	=	(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].TNodeID].MRTID;
					pTmpNaviRTBClass->TInfo.FixNode2	=	(*m_pclsThc->p_StruFNode)[(*tmpp_Approach)[i].TNodeID].MRTID;
					pTmpNaviRTBClass->TInfo.FixCoor.x	=	0;
					pTmpNaviRTBClass->TInfo.FixCoor.y	=	0;
					pTmpNaviRTBClass->TInfo.FixDis		=	0;
					pTmpNaviRTBClass->RTBStateFlag		=	2;
// /*				
				#endif
// */
			}
			else if((*tmpp_Approach)[i].FMapLayer == _MAPLAYER_STHR){//這是很重要的分隔
				TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;
				TmpListCount=0;
				if(pTmpNaviRTBClass==NULL)	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
				else						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;

				TmpFConnectIndex=(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].ConnectStruIndex;
				TmpTConnectIndex=(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].ConnectStruIndex;
				TmpFConnectCount=(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].ConnectCount;
				TmpTConnectCount=(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].ConnectCount;
				for(j=0;j<TmpFConnectCount;j++){
					if( (*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TMapLayer==2 &&
						(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID==(*tmpp_Approach)[i].TNodeID &&
						(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID==(*tmpp_Approach)[i].TRtBID ){
						pTmpNaviRTBClass->WeightDis	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].WeightingLength;
						pTmpNaviRTBClass->RealDis	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].Length;
						pTmpNaviRTBClass->FileIdx	=	m_pThr_F->ThrHeader.MapID;
						pTmpNaviRTBClass->RTBIdx	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->NSSCount	=	0;
					}
				}
				for(j=0;j<TmpFConnectCount;j++){
					if(	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TMapLayer==1 &&
						(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID==pTmpNaviRTBClass->RTBIdx ){
						pTmpNaviRTBClass->FInfo.MapID		=	m_pThr_F->ThrHeader.MapID;
						pTmpNaviRTBClass->FInfo.RTBID		=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->FInfo.FixNode1	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->FInfo.FixNode2	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpFConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->FInfo.FixCoor		=	(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor;
						pTmpNaviRTBClass->FInfo.FixDis		=	0;
					}
				}
				for(j=0;j<TmpTConnectCount;j++){
					if(	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TRtBID==pTmpNaviRTBClass->RTBIdx &&
						(*m_pThr_F->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TMapLayer==1 ){
						pTmpNaviRTBClass->TInfo.MapID		=	m_pThr_F->ThrHeader.MapID;
						pTmpNaviRTBClass->TInfo.RTBID		=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TRtBID;
						pTmpNaviRTBClass->TInfo.FixNode1	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->TInfo.FixNode2	=	(*m_pThr_F->pThrEachMap.ThrConnect)[TmpTConnectIndex+j].TNodeID;
						pTmpNaviRTBClass->TInfo.FixCoor		=	(*m_pThr_F->pThrEachMap.ThrMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor;
						pTmpNaviRTBClass->TInfo.FixDis		=	0;
					}
				}
				pTmpNaviRTBClass->RTBStateFlag	=	1;
			}
			else{
				if(TmpMapLayer==(*tmpp_Approach)[i].TMapLayer){
					pTmpNaviRTBClass->NodeIDList[TmpListCount]=(*tmpp_Approach)[i].TNodeID;	TmpListCount++;
					if(TmpListCount==pTmpNaviRTBClass->NodeIDCount){
						pTmpNaviRTBClass->TInfo.MapID =(*tmpp_Approach)[i].TMapID;
						pTmpNaviRTBClass->TInfo.RTBID = (*tmpp_Approach)[i].TRtBID;
						pTmpNaviRTBClass->TInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
						pTmpNaviRTBClass->TInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;

#ifdef VALUE_EMGRT//判別在同個圖層同個區塊以取得座標點
						if(g_pRtB_F->MyState.MapID==(*tmpp_Approach)[i].TMapID && g_pRtB_F->MyState.RtBlockID==(*tmpp_Approach)[i].FRtBID){
							pTmpNaviRTBClass->TInfo.FixCoor.x =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
							pTmpNaviRTBClass->TInfo.FixCoor.y =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
						}
						else{
							pTmpNaviRTBClass->TInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
							pTmpNaviRTBClass->TInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
						}
#else
						if(g_pRtB_F->MyState.MapID==(*tmpp_Approach)[i].TMapID && g_pRtB_F->MyState.RtBlockID==(*tmpp_Approach)[i].FRtBID){
							pTmpNaviRTBClass->TInfo.FixCoor.x =	g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
							pTmpNaviRTBClass->TInfo.FixCoor.y =	g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
						}
						else{
							pTmpNaviRTBClass->TInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
							pTmpNaviRTBClass->TInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
						}
#endif
						pTmpNaviRTBClass->TInfo.FixDis = 0;
					}
				}
				else
				{
					if(pTmpNaviRTBClass==NULL)	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;//判斷是否為串列的頭
					else						pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
					//填充RT的資料
					TmpMapLayer=(*tmpp_Approach)[i].TMapLayer;

					pTmpNaviRTBClass->TInfo.FixDis = 0;
					pTmpNaviRTBClass->FInfo.MapID = (*tmpp_Approach)[i].FMapID;
					pTmpNaviRTBClass->FInfo.RTBID = (*tmpp_Approach)[i].FRtBID;
					pTmpNaviRTBClass->FInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
					pTmpNaviRTBClass->FInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;

					pTmpNaviRTBClass->FInfo.FixDis = 0;
					pTmpNaviRTBClass->TInfo.MapID = (*tmpp_Approach)[i].TMapID;
					pTmpNaviRTBClass->TInfo.RTBID = (*tmpp_Approach)[i].TRtBID;
					pTmpNaviRTBClass->TInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
					pTmpNaviRTBClass->TInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;

#ifdef VALUE_EMGRT
					if(g_pRtB_T->MyState.MapID==(*tmpp_Approach)[i].FMapID && g_pRtB_T->MyState.RtBlockID==(*tmpp_Approach)[i].FRtBID){
						pTmpNaviRTBClass->FInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->FInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;	
						pTmpNaviRTBClass->TInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->TInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;	
					}
					else{
						pTmpNaviRTBClass->FInfo.FixCoor.x =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->FInfo.FixCoor.y =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;	
						pTmpNaviRTBClass->TInfo.FixCoor.x =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->TInfo.FixCoor.y =	(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;	
					}
#else
					if(g_pRtB_T->MyState.MapID==(*tmpp_Approach)[i].FMapID && g_pRtB_T->MyState.RtBlockID==(*tmpp_Approach)[i].FRtBID){
						pTmpNaviRTBClass->FInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->FInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;	
						pTmpNaviRTBClass->TInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->TInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;	
					}
					else{
						pTmpNaviRTBClass->FInfo.FixCoor.x =	g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->FInfo.FixCoor.y =	g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;	
						pTmpNaviRTBClass->TInfo.FixCoor.x =	g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;	
						pTmpNaviRTBClass->TInfo.FixCoor.y =	g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;	
					}
#endif
					//開始填充Node資訊
					TmpListCount=0;
					pTmpNaviRTBClass->NodeIDList[TmpListCount]=(*tmpp_Approach)[i].FNodeID;	TmpListCount++;
					pTmpNaviRTBClass->NodeIDList[TmpListCount]=(*tmpp_Approach)[i].TNodeID;	TmpListCount++;
					pTmpNaviRTBClass->WeightDis	=	0;
					pTmpNaviRTBClass->RealDis	=	0;
					pTmpNaviRTBClass->RTBStateFlag	=	0;
				}
			}
		}
	}
	
	//修正頭尾
	pTmpNaviRTBClass=TmpNaviFromToClass->NRBC;
	pTmpNaviRTBClass->FInfo.FixCoor = TmpNaviFromToClass->FCoor;
	while(pTmpNaviRTBClass!=NULL){
		if(pTmpNaviRTBClass->NextRTB==NULL)
		{	
			#ifdef _DROUTE_ERROR
				if(m_bErrorExceptionHandle)
				{
					TmpNaviFromToClass->TCoor       = pTmpNaviRTBClass->TInfo.FixCoor;
				}
				else
				{
					pTmpNaviRTBClass->TInfo.FixCoor = TmpNaviFromToClass->TCoor;
				}
			#else
				pTmpNaviRTBClass->TInfo.FixCoor = TmpNaviFromToClass->TCoor;		
			#endif
				break;
		}
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
	
	return nuTRUE;
}

nuBOOL Class_CalRouting::CreateLocalRoutingData(PNAVIRTBLOCKCLASS TmpNaviRtBlockClass)
{
	nuLONG i,TmpRtBCount=0,TmpNSSCount=0;
	TmpRtBCount=0;
	TmpNSSCount=0;

	ApproachStru **tmpp_Approach;
	tmpp_Approach=Tmp_Approach.p_Approach;

	g_pCRDC->Create_NaviSubClass(TmpNaviRtBlockClass,Tmp_Approach.ApproachCount);
	g_pCRDC->Create_NodeIDList(TmpNaviRtBlockClass,Tmp_Approach.ApproachCount+1);

	TmpRtBCount=0;
	TmpNSSCount=0;
	//開始填資料
	for(i=0;i<Tmp_Approach.ApproachCount;i++)
	{
		if((*tmpp_Approach)[i].FMapID==(*tmpp_Approach)[i].TMapID)
		{
			if(i==0)
			{
				TmpRtBCount++;	TmpNSSCount=0;
				TmpNaviRtBlockClass->FInfo.MapID = (*tmpp_Approach)[i].FMapID;
				TmpNaviRtBlockClass->FInfo.RTBID = (*tmpp_Approach)[i].FRtBID;
				TmpNaviRtBlockClass->FInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
				TmpNaviRtBlockClass->FInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;
#ifdef VALUE_EMGRT
				if(!m_bTMCLocalRoute)
				{
				TmpNaviRtBlockClass->FInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;
				TmpNaviRtBlockClass->FInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;
				}
#else
				TmpNaviRtBlockClass->FInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.x;
				TmpNaviRtBlockClass->FInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].FNodeID].NodeCoor.y;
#endif
				TmpNaviRtBlockClass->FInfo.FixDis = 0;
					
				TmpNaviRtBlockClass->TInfo.MapID = (*tmpp_Approach)[i].TMapID;
				TmpNaviRtBlockClass->TInfo.RTBID = (*tmpp_Approach)[i].TRtBID;
				TmpNaviRtBlockClass->TInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
				TmpNaviRtBlockClass->TInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;
#ifdef VALUE_EMGRT
				if(!m_bTMCLocalRoute)
				{
				TmpNaviRtBlockClass->TInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
				TmpNaviRtBlockClass->TInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
				}
#else
				TmpNaviRtBlockClass->TInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
				TmpNaviRtBlockClass->TInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
#endif
				TmpNaviRtBlockClass->TInfo.FixDis = 0;

				TmpNSSCount=0;
				TmpNaviRtBlockClass->NodeIDList[TmpNSSCount]=(*tmpp_Approach)[i].FNodeID;	TmpNSSCount++;
				TmpNaviRtBlockClass->NodeIDList[TmpNSSCount]=(*tmpp_Approach)[i].TNodeID;	TmpNSSCount++;
			}
			else
			{
				TmpNaviRtBlockClass->NodeIDList[TmpNSSCount]=(*tmpp_Approach)[i].TNodeID;	TmpNSSCount++;
				TmpNaviRtBlockClass->TInfo.MapID = (*tmpp_Approach)[i].TMapID;
				TmpNaviRtBlockClass->TInfo.RTBID = (*tmpp_Approach)[i].TRtBID;
				TmpNaviRtBlockClass->TInfo.FixNode1 = (*tmpp_Approach)[i].FNodeID;
				TmpNaviRtBlockClass->TInfo.FixNode2 = (*tmpp_Approach)[i].TNodeID;
				if(g_pRtB_T->MyState.MapID==(*tmpp_Approach)[i].TMapID && g_pRtB_T->MyState.RtBlockID==(*tmpp_Approach)[i].TRtBID)
				{
#ifdef VALUE_EMGRT
					if(!m_bTMCLocalRoute)
					{
					TmpNaviRtBlockClass->TInfo.FixCoor.x =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
					TmpNaviRtBlockClass->TInfo.FixCoor.y =	(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
					}
#else
					TmpNaviRtBlockClass->TInfo.FixCoor.x =	g_pRtB_T->OrgCoor.x+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.x;
					TmpNaviRtBlockClass->TInfo.FixCoor.y =	g_pRtB_T->OrgCoor.y+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[(*tmpp_Approach)[i].TNodeID].NodeCoor.y;
#endif
				}
				TmpNaviRtBlockClass->TInfo.FixDis = 0;
			}
		}
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::ReadBaseData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule)
{	
	nuFILE *TmpFile=NULL;
	nuPWORD lpwstr=NULL;
#ifdef LCMM
	if(m_lReadBaseData == 1)
	{
		ReMoveBaseData();
	}
	else
	{
		if(m_pclsThc)
		{
			m_pclsThc->Remove();
			delete m_pclsThc;
		}
		m_pclsThc=NULL;

		//移除thr資料
		#ifndef DEF_MAKETHRROUTE
			m_pThr_F=NULL;
			m_pThr_T=NULL;
			if(m_pGThr[0])
			{
				m_pGThr[0]->ReSet();
				delete m_pGThr[0];
				m_pGThr[0]=NULL;
			}
			if(m_pGThr[1])
			{
				m_pGThr[1]->ReSet();
				delete m_pGThr[1];
				m_pGThr[1]=NULL;
			}
			if(NULL != m_pcls3dp)
			{
				delete m_pcls3dp;
				m_pcls3dp=NULL;
			}
		#endif
	}
#else
		ReMoveBaseData();
#endif
	/*** 判斷是不是跨區規劃 ***/
	if(b_Local==nuFALSE)//讀取跨省用的thc檔
	{
		m_pclsThc= new CCtrl_Thc;
		m_pclsThc->InitClass(l_RoutingRule);

		m_pclsThc->Read();
	}

	//thr檔的讀取(因為是指標 所以處理的模式 是採用先分析再掛載)
	#ifndef DEF_MAKETHRROUTE
		m_pcls3dp= new CCtrl_3dp;
		m_pGThr[0]= new Class_ThrCtrl;
		m_pGThr[1]= new Class_ThrCtrl;
		if(NULL == m_pcls3dp ||
		   NULL == m_pGThr[0] ||
		   NULL == m_pGThr[1])
		{
			return nuFALSE;
		}
		if(b_Local==nuFALSE)
		{
			if(CLRS_F.MapIdx==CLRS_T.MapIdx)//有差就讀兩個檔 沒有差異 就讀一個檔
			{
				m_pGThr[0]->InitClass(CLRS_F.MapIdx,l_RoutingRule);
				if(nuFALSE == m_pGThr[0]->Read_Header()){	ReMoveBaseData();	m_pThr_F = NULL;		m_pThr_T = NULL;		return nuFALSE;	}
				else								{	m_pThr_F = m_pGThr[0];	m_pThr_T = m_pGThr[0];					}
			}
			else
			{
				m_pGThr[0]->InitClass(CLRS_F.MapIdx,l_RoutingRule);
				if(nuFALSE == m_pGThr[0]->Read_Header()){	ReMoveBaseData();	m_pThr_F=NULL;		return nuFALSE;	}
				else								{	m_pThr_F = m_pGThr[0];					}

				m_pGThr[1]->InitClass(CLRS_T.MapIdx,l_RoutingRule);
				if(nuFALSE == m_pGThr[1]->Read_Header()){	ReMoveBaseData();	m_pThr_T=NULL;		return nuFALSE;	}
				else								{	m_pThr_T = m_pGThr[1];					}				
			}
		}
	#endif		
	return nuTRUE;
}
nuVOID Class_CalRouting::ReMoveThrData()
{
	//移除thr資料
		#ifndef DEF_MAKETHRROUTE
			m_pThr_F=NULL;
			m_pThr_T=NULL;
			if(m_pGThr[0])
			{
				m_pGThr[0]->ReSet();
				delete m_pGThr[0];
				m_pGThr[0]=NULL;
			}
			if(m_pGThr[1])
			{
				m_pGThr[1]->ReSet();
				delete m_pGThr[1];
				m_pGThr[1]=NULL;
			}
			delete m_pcls3dp;
			m_pcls3dp=NULL;
		#endif
}

nuVOID Class_CalRouting::ReMoveBaseData()
{
	if(m_pclsThc)
	{
		m_pclsThc->Remove();
		delete m_pclsThc;
	}
	m_pclsThc=NULL;

	//移除thr資料
	#ifndef DEF_MAKETHRROUTE
		m_pThr_F=NULL;
		m_pThr_T=NULL;
		if(m_pGThr[0])
		{
			m_pGThr[0]->ReSet();
			delete m_pGThr[0];
			m_pGThr[0]=NULL;
		}
		if(m_pGThr[1])
		{
			m_pGThr[1]->ReSet();
			delete m_pGThr[1];
			m_pGThr[1]=NULL;
		}
		if(NULL != m_pcls3dp)
		{
			delete m_pcls3dp;
			m_pcls3dp=NULL;
		}	
	#endif

	ReMoveSysRTData();
//#ifdef _USETTI
	ReleaseTTI();
//#endif
	//移除禁轉資料
	ReMoveSysNTData();
	//移除Dst資料
#ifdef _DDST
	ReMoveSysDstData();
#endif
#ifdef _DTMC
	ReMoveSysTmcData();
#endif
#ifdef _DAR
		ReMoveSysARData();
#endif
}

nuBOOL Class_CalRouting::ReadSysNTData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule)//讀取頭尾RtBlock與thr的資料
{
	//#ifdef VALUE_EMGRT
		m_pclsNT[0]= new CCtrl_TT;
		m_pclsNT[1]= new CCtrl_TT;
	/*#else
		m_pclsNT[0]= new CCtrl_NT;
		m_pclsNT[1]= new CCtrl_NT;
	#endif*/
	//nt檔的讀取
	if(CLRS_F.MapIdx==CLRS_T.MapIdx)
	{
		#ifdef DEF_MAKETHRROUTE
			m_pclsNT[0]->InitClass();
			nuTcscpy(m_pclsNT[0]->FName, MakeThr_MapPath);
		//	#ifdef VALUE_EMGRT
				nuTcscat(m_pclsNT[0]->FName, nuTEXT(".tt"));
			/*#else
				nuTcscat(m_pclsNT[0]->FName, nuTEXT(".nt"));
			#endif*/
		#else
			m_pclsNT[0]->InitClass();
		//#ifdef _USETTI
			nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".NTT"));
		//#else
			//nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".TT"));
		//#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx, m_pclsNT[0]->FName, NURO_MAX_PATH);
		#endif
		//m_Lib_FileName.NewGetFileNamePath(CLRS_F.MapIdx,17,*(ppclsNT[0].FName));
		if(!m_pclsNT[0]->ReadStruct())	
		{	
			m_pclsNT[0]->ReleaseStruct();	
			m_pNT_F=NULL;				
			m_pNT_T=NULL;
			m_pclsNT[0]->InitClass();
			nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".TT"));
			g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx, m_pclsNT[0]->FName, NURO_MAX_PATH);
			if(!m_pclsNT[0]->ReadStruct())
			{
				m_pclsNT[0]->ReleaseStruct();	
				m_pNT_F=NULL;				
				m_pNT_T=NULL;
			}
			else								
			{	
				m_pclsNT[0]->m_bUSETT = nuTRUE;
				m_pNT_F=m_pclsNT[0];					
				m_pNT_T=m_pclsNT[0];					
			}
		}
		else								
		{	
			m_pclsNT[0]->m_bUSETT = nuFALSE;
			m_pNT_F=m_pclsNT[0];					
			m_pNT_T=m_pclsNT[0];					
		}
	}
	else
	{
		#ifdef DEF_MAKETHRROUTE
			m_pclsNT[1]->InitClass();
			nuTcscpy(m_pclsNT[1]->FName, MakeThr_MapPath);
			//#ifdef VALUE_EMGRT
				nuTcscat(m_pclsNT[1]->FName, nuTEXT(".tt"));
			/*#else
				nuTcscat(m_pclsNT[1]->FName, nuTEXT(".nt"));
			#endif*/
		#else
			m_pclsNT[1]->InitClass();
		//#ifdef _USETTI
			nuTcscpy(m_pclsNT[1]->FName, nuTEXT(".NTT"));
		//#else
			//nuTcscpy(m_pclsNT[1]->FName, nuTEXT(".TT"));
		//#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx,m_pclsNT[1]->FName,NURO_MAX_PATH);
		#endif
		if(!m_pclsNT[1]->ReadStruct())	
		{	
			m_pclsNT[1]->ReleaseStruct();	
			m_pNT_T=NULL;
			m_pclsNT[1]->InitClass();
			nuTcscpy(m_pclsNT[1]->FName, nuTEXT(".TT"));
			g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx, m_pclsNT[1]->FName, NURO_MAX_PATH);
			if(!m_pclsNT[1]->ReadStruct())
			{
				m_pclsNT[1]->ReleaseStruct();	
				m_pNT_T=NULL;
			}
			else								
			{	
				m_pclsNT[1]->m_bUSETT = nuTRUE;
				m_pNT_T=m_pclsNT[1];					
			}
		}
		else								
		{	
			m_pclsNT[1]->m_bUSETT = nuFALSE;
			m_pNT_T=m_pclsNT[1];
		}

		if(b_Local==nuFALSE)
		{
			#ifdef DEF_MAKETHRROUTE
				m_pclsNT[0]->InitClass();
				nuTcscpy(m_pclsNT[0]->FName, MakeThr_MapPath);
				//#ifdef VALUE_EMGRT
					nuTcscat(m_pclsNT[0]->FName, nuTEXT(".tt"));
				/*#else
					nuTcscat(m_pclsNT[0]->FName, nuTEXT(".nt"));
				#endif*/
			#else
				m_pclsNT[0]->InitClass();
			//#ifdef _USETTI
				nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".NTT"));
			//#else
				//nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".TT"));
			//#endif
				g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,m_pclsNT[0]->FName,NURO_MAX_PATH);
			#endif
			if(!m_pclsNT[0]->ReadStruct())	
			{	
				m_pclsNT[0]->ReleaseStruct();	
				m_pNT_F=NULL;	
				m_pclsNT[0]->InitClass();
				nuTcscpy(m_pclsNT[0]->FName, nuTEXT(".TT"));
				g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx, m_pclsNT[0]->FName, NURO_MAX_PATH);
				if(!m_pclsNT[0]->ReadStruct())
				{
					m_pclsNT[0]->ReleaseStruct();	
					m_pNT_F=NULL;
				}
				else								
				{	
					m_pclsNT[0]->m_bUSETT = nuTRUE;
					m_pNT_F=m_pclsNT[0];				
				}
			}
			else								
			{	
				m_pclsNT[0]->m_bUSETT = nuFALSE;
				m_pNT_F=m_pclsNT[0];					
			}
		}
	}
	return nuTRUE;
}

nuVOID Class_CalRouting::ReMoveSysNTData()
{
	m_pNT_F=NULL;
	m_pNT_T=NULL;
	if(m_pclsNT[1])
	{
		m_pclsNT[1]->CloseClass();
		delete m_pclsNT[1];
		m_pclsNT[1]=NULL;
	}
	if(m_pclsNT[0])
	{
		m_pclsNT[0]->CloseClass();
	delete m_pclsNT[0];
	m_pclsNT[0]=NULL;
	}
}
#ifdef _DAR
	nuBOOL Class_CalRouting::ReadSysARData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule)
	{
		nuULONG C_add = 0, B_add = 0;
		m_pclsAR = new CCtrl_AR; 
		if(m_pclsAR == NULL){return nuFALSE;}	
		m_pclsAR->InitClass();	
		nuTcscpy(m_pclsAR->FName, nuTEXT(".AR"));
		g_pRtFSApi->FS_FindFileWholePath(-1,m_pclsAR->FName,NURO_MAX_PATH);
		if(!m_pclsAR->ReadAR_Head())
		{	
			m_pclsAR -> ReleaseStruct();
			ReMoveSysARData();
			return nuFALSE;
		}
		if(!m_pclsAR->AR_NearbyCity_Compare(CLRS_F.PtLocalRoute.MapID,CLRS_T.PtLocalRoute.MapID,CLRS_F.PtLocalRoute.RTBID,
			CLRS_T.PtLocalRoute.RTBID,&C_add, &B_add))
		{//如果起訖點不位於相鄰RT
			m_bARNearbyAR = nuFALSE;
			m_pclsAR -> ReleaseStruct();
			ReMoveSysARData();
		}
		else
		{//相鄰才進來讀資料
			m_bARNearbyAR = nuTRUE;
			if(!m_pclsAR->ReadAR_Node(C_add, B_add))
			{
				m_bARNearbyAR = nuFALSE;
				m_pclsAR -> ReleaseStruct();
				ReMoveSysARData();
				return nuFALSE;
			}
		}
		return nuTRUE;
	}

	nuVOID Class_CalRouting::ReMoveSysARData()
	{
		if(m_pclsAR)
		{
			m_bARNearbyAR = nuFALSE;
			m_pclsAR -> CloseClass();
			delete m_pclsAR;
			m_pclsAR = NULL;
		}	
	}
#endif
//*************************-讀取Dst檔-*****************************//
//-----------------------------------------------------------------//
#ifdef _DDST
	nuBOOL Class_CalRouting::ReadSysDstData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule)
	{
		m_pclsDst = new CCtrl_Dst; 
		if(m_pclsDst != NULL)
		{ 	
			m_pclsDst->InitClass();	
			nuTcscpy(m_pclsDst->FName, nuTEXT(".dst"));
			g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx,m_pclsDst->FName,NURO_MAX_PATH);
			if(!m_pclsDst -> ReadStruct())
			{
				m_pclsDst -> ReleaseStruct();
				ReMoveSysDstData();
				return nuFALSE;
			}
			else
			{//Read File Success
				nuGetLocalTime(&nuTIME);
			}
		}
		return nuTRUE;
	}

	nuVOID Class_CalRouting::ReMoveSysDstData()
	{
		if(m_pclsDst)
		{
			m_pclsDst -> CloseClass();
			delete m_pclsDst;
			m_pclsDst = NULL;
		}	
	}
#endif
//------------------------------------------------------------------//
//******************************************************************//
//---------------------------讀取Tmc檔----------------------------------------------------------------------------------//
#ifdef _DTMC
	nuBOOL Class_CalRouting::ReadSysTmcData(TmpRoutingInfo &CLRS_T, TMC_ROUTER_DATA *TMCData, nuLONG TMCDataCount, nuBOOL *bTMCRTB)
	{
		m_pclsTmc = new CCtrl_Tmc; 
		if(m_pclsTmc != NULL)
		{ 	
			m_pclsTmc->InitClass();	
			nuTcscpy(m_pclsTmc->FName, nuTEXT(".Tmc"));
			g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx,m_pclsTmc->FName,NURO_MAX_PATH);
			/*StructTMCPassData *TMCPassData = NULL;
			TMCPassData = new StructTMCPassData[TMCDataCount];
			if(TMCPassData == NULL)
			{
				return nuFALSE;
			}*/
			if(!m_pclsTmc -> ReadTMC(TMCData, TMCDataCount, bTMCRTB))
			{
				m_pclsTmc -> ReleaseStruct();
				ReMoveSysTmcData();
				return nuFALSE;
			}
			//delete [TMCDataCount] TMCPassData;
			//TMCPassData = NULL;
		}
		return nuTRUE;
	}

	nuVOID Class_CalRouting::ReMoveSysTmcData()
	{
		if(m_pclsTmc)
		{
			m_pclsTmc -> CloseClass();
			delete m_pclsTmc;
			m_pclsTmc = NULL;
		}	
	}
#endif
//--------------------------------------------------------------------------------------------------------------------------//
#if (defined VALUE_EMGRT) || (defined ZENRIN)
	nuBOOL Class_CalRouting::ReadPTS(nuLONG FMapID)
	{
		m_pclsPTS[0] = new CCtrl_PTS;
		m_pclsPTS[1] = new CCtrl_PTS;
		if(m_pclsPTS[0] == NULL || m_pclsPTS[1] == NULL){return nuFALSE;}
		
		m_pclsPTS[0]->InitClass();
		nuTcscpy(m_pclsPTS[0]->FName, nuTEXT(".pts"));
		g_pRtFSApi->FS_FindFileWholePath(FMapID,m_pclsPTS[0]->FName,NURO_MAX_PATH);
		if(!m_pclsPTS[0]->ReadPTSFile())	{	m_pclsPTS[0]->ReleasePTS();	m_PTS_F=NULL; m_PTS_T=NULL;	return nuFALSE;	}
		else								{	m_PTS_F = m_pclsPTS[0];					m_PTS_T = m_pclsPTS[0];					}
		return nuTRUE;
	}

	nuLONG Class_CalRouting::CheckPTS(nuULONG RtBID,nuULONG NodeID)
	{
		nuULONG i;
		for(i=0;i<m_PTS_F->PTSStruCount;i++){
			if(	(*m_PTS_F->pPTSStru)[i].NodeID==NodeID &&
				(*m_PTS_F->pPTSStru)[i].RtBID==RtBID		)
			{
				return (*m_PTS_F->pPTSStru)[i].NameAddr;
			}
		}
		return nuTRUE;
	}

	nuVOID Class_CalRouting::ReMovePTS()
	{
		m_PTS_F=NULL;
		m_pclsPTS[0]->CloseClass();
		delete m_pclsPTS[1];
		m_pclsPTS[1]=NULL;
		delete m_pclsPTS[0];
		m_pclsPTS[0]=NULL;
	}
#endif

nuBOOL Class_CalRouting::ReadSysRTData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule)//讀取頭尾RtBlock與thr的資料
{
	UONERtCtrl TmpRtCtrl;
	nuULONG i;
	/*** 讀取RTB ***/
	if(CLRS_F.MapIdx==CLRS_T.MapIdx && CLRS_F.RtBIdx==CLRS_T.RtBIdx)//如果是在同個區塊
	{
		#ifdef DEF_MAKETHRROUTE
			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
			nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
			g_pLocalRtB[0]->InitClass();
			nuTcscpy(g_pLocalRtB[0]->FName, MakeThr_MapPath);
			nuTcscat(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
		#else
			TmpRtCtrl.InitClass();
#ifndef _USERT6
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
#else
			if(b_LocalRut && m_bReadRt6)
			{
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt6"));
			}
			else
			{
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
			}
#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
			g_pLocalRtB[0]->InitClass();
#ifndef _USERT6
			nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
#else
			if(b_LocalRut && m_bReadRt6)
			{
				nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt6"));
			}
			else
			{
				nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
			}
#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,g_pLocalRtB[0]->FName,NURO_MAX_PATH);
		#endif
		if(nuFALSE==TmpRtCtrl.Read_Header()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
		for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++)
		{
			if((*TmpRtCtrl.Rt_Block_ID_List)[i]==CLRS_F.PtLocalRoute.RTBID)
			{
				g_pLocalRtB[0]->MyState.MapID=TmpRtCtrl.MapID;
				g_pLocalRtB[0]->MyState.MapIdx=CLRS_F.MapIdx;
				g_pLocalRtB[0]->MyState.RtBlockID=(*TmpRtCtrl.Rt_Block_ID_List)[i];
				g_pLocalRtB[0]->MyState.RtBlockIdx=i;
				g_pLocalRtB[0]->OrgCoor=(*TmpRtCtrl.RT_Block_Coor)[i].ORG_Coor;
				if(nuFALSE==g_pLocalRtB[0]->Read_Header((*TmpRtCtrl.Rt_Block_Addr_List)[i])){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
				if(nuFALSE==g_pLocalRtB[0]->Read_MainData()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
				if(nuFALSE==g_pLocalRtB[0]->Read_Connect()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
			}
		}
		TmpRtCtrl.ReSet();
		g_pRtB_F=g_pLocalRtB[0];
		g_pRtB_T=g_pLocalRtB[0];
	}
	else
	{
		#ifdef DEF_MAKETHRROUTE
			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
			nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
			g_pLocalRtB[1]->InitClass();
			nuTcscpy(g_pLocalRtB[1]->FName, MakeThr_MapPath);
			nuTcscat(g_pLocalRtB[1]->FName, nuTEXT(".rt"));
		#else
			TmpRtCtrl.InitClass();
#ifndef _USERT6
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
#else
			if(b_LocalRut && m_bReadRt6)
			{
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt6"));
			}
			else
			{
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
			}
#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
			g_pLocalRtB[1]->InitClass();
#ifndef _USERT6
			nuTcscpy(g_pLocalRtB[1]->FName, nuTEXT(".rt"));
#else
			if(b_LocalRut && m_bReadRt6)
			{
				nuTcscpy(g_pLocalRtB[1]->FName, nuTEXT(".rt6"));
			}
			else
			{
				nuTcscpy(g_pLocalRtB[1]->FName, nuTEXT(".rt"));
			}
#endif
			g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx,g_pLocalRtB[1]->FName,NURO_MAX_PATH);
		#endif
		if(nuFALSE==TmpRtCtrl.Read_Header()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READERTFALSE;	return nuFALSE;	}
		for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++)
		{
			if((*TmpRtCtrl.Rt_Block_ID_List)[i]==CLRS_T.PtLocalRoute.RTBID)//讀取終點資料檔，先找到要讀取的RT檔才讀取
			{
				g_pLocalRtB[1]->MyState.MapID=TmpRtCtrl.MapID;
				g_pLocalRtB[1]->MyState.MapIdx=CLRS_T.MapIdx;
				g_pLocalRtB[1]->MyState.RtBlockID=(*TmpRtCtrl.Rt_Block_ID_List)[i];
				g_pLocalRtB[1]->MyState.RtBlockIdx=i;
				g_pLocalRtB[1]->OrgCoor=(*TmpRtCtrl.RT_Block_Coor)[i].ORG_Coor;
				if(nuFALSE==g_pLocalRtB[1]->Read_Header((*TmpRtCtrl.Rt_Block_Addr_List)[i])){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READERTFALSE;	return nuFALSE;	}
				if(nuFALSE==g_pLocalRtB[1]->Read_MainData()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READERTFALSE;	return nuFALSE;	}
				if(nuFALSE==g_pLocalRtB[1]->Read_Connect()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READERTFALSE;	return nuFALSE;	}
			}
		}
		TmpRtCtrl.ReSet();
		g_pRtB_T=g_pLocalRtB[1];

		if(b_Local==nuFALSE)
		{
			#ifdef DEF_MAKETHRROUTE
				TmpRtCtrl.InitClass();
				nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
				nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
				g_pLocalRtB[0]->InitClass();
				nuTcscpy(g_pLocalRtB[0]->FName, MakeThr_MapPath);
				nuTcscat(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
			#else
				TmpRtCtrl.InitClass();
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
				g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
				g_pLocalRtB[0]->InitClass();
				nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
				g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,g_pLocalRtB[0]->FName,NURO_MAX_PATH);
			#endif
			if(nuFALSE==TmpRtCtrl.Read_Header()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
			for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++)
			{
				if((*TmpRtCtrl.Rt_Block_ID_List)[i]==CLRS_F.PtLocalRoute.RTBID)
				{
					g_pLocalRtB[0]->MyState.MapID=TmpRtCtrl.MapID;
					g_pLocalRtB[0]->MyState.MapIdx=CLRS_F.MapIdx;
					g_pLocalRtB[0]->MyState.RtBlockID=(*TmpRtCtrl.Rt_Block_ID_List)[i];
					g_pLocalRtB[0]->MyState.RtBlockIdx=i;
					g_pLocalRtB[0]->OrgCoor=(*TmpRtCtrl.RT_Block_Coor)[i].ORG_Coor;
					
					if(nuFALSE==g_pLocalRtB[0]->Read_Header((*TmpRtCtrl.Rt_Block_Addr_List)[i])){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
					if(nuFALSE==g_pLocalRtB[0]->Read_MainData()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
					if(nuFALSE==g_pLocalRtB[0]->Read_Connect()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	return nuFALSE;	}
				}
			}
			TmpRtCtrl.ReSet();
			g_pRtB_F=g_pLocalRtB[0];
		}
	}
	return nuTRUE;
}
nuVOID Class_CalRouting::ReMoveSysRTData()
{
	//移除RTB資料
	g_pRtB_F=NULL;
	g_pRtB_T=NULL;
	g_pLocalRtB[0]->ReSet();
	g_pLocalRtB[1]->ReSet();
}

nuBOOL Class_CalRouting::CompareRTBIndex(PTFIXTOROADSTRU &FPt,PTFIXTOROADSTRU &TPt,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG FMapIdx, nuLONG TMapIdx)
{
	nuUSHORT j;
	nuULONG i;
	UONERtCtrl TmpRtCtrl;
	/*** 將FixInfo的資訊添加在規劃專用起訖點結構 ***/
	CLRS_F.PtLocalRoute = FPt;
	CLRS_T.PtLocalRoute = TPt;
		
	/*** 開啟Rt檔 比對MapID RTBID以取得MApIdx RtBIdx ***/
	if(m_nRTFINDRTIDX == 0 || (FMapIdx == -1 || TMapIdx == -1)) 
	{
		for(i=0;i<m_MapIDtoMapIdx.MapCount;i++)
		{
			if( (*m_MapIDtoMapIdx.MapIDList)[i]!=FPt.MapID && (*m_MapIDtoMapIdx.MapIDList)[i]!=TPt.MapID ) continue;
			
			#ifdef DEF_MAKETHRROUTE
				TmpRtCtrl.InitClass();
				nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
				nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
			#else
				TmpRtCtrl.InitClass();
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
				g_pRtFSApi->FS_FindFileWholePath(i,TmpRtCtrl.FName,NURO_MAX_PATH);
			#endif
			if(nuFALSE==TmpRtCtrl.Read_Header()){	ReleaseColationDataAgain(TmpRtCtrl);	Renum_RoutingState=ROUTE_READRTFALSE;	return nuFALSE;	}//讀取RT的Header
			for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
			{//對照起訖點(同張地圖同個區塊的資訊)
				if((*m_MapIDtoMapIdx.MapIDList)[i]==FPt.MapID && (*TmpRtCtrl.Rt_Block_ID_List)[j]==FPt.RTBID)
				{	CLRS_F.MapIdx = (*m_MapIDtoMapIdx.MapIdxList)[i];	CLRS_F.RtBIdx=j;	}
				if((*m_MapIDtoMapIdx.MapIDList)[i]==TPt.MapID && (*TmpRtCtrl.Rt_Block_ID_List)[j]==TPt.RTBID)
				{	CLRS_T.MapIdx = (*m_MapIDtoMapIdx.MapIdxList)[i];	CLRS_T.RtBIdx=j;	}
			}
			TmpRtCtrl.ReSet();
		}
	}
	else
	{
		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(FMapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header())
		{	
			ReleaseColationDataAgain(TmpRtCtrl);	
			Renum_RoutingState=ROUTE_READRTFALSE;	
			return nuFALSE;	
		}//讀取RT的Header
		if(TmpRtCtrl.MapID != FPt.MapID)
		{
			return nuFALSE;
		}
		for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
		{//對照起訖點(同張地圖同個區塊的資訊)
			if((*TmpRtCtrl.Rt_Block_ID_List)[j]==FPt.RTBID)
			{	
				CLRS_F.MapIdx = FMapIdx;	
				CLRS_F.RtBIdx = j;	
			}
		}
		TmpRtCtrl.ReSet();
		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(TMapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header())
		{	
			ReleaseColationDataAgain(TmpRtCtrl);	
			Renum_RoutingState=ROUTE_READRTFALSE;	
			return nuFALSE;	
		}//讀取RT的Header
		if(TmpRtCtrl.MapID != TPt.MapID)
		{
			return nuFALSE;
		}
		for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
		{//對照起訖點(同張地圖同個區塊的資訊)
			if((*TmpRtCtrl.Rt_Block_ID_List)[j]==TPt.RTBID)
			{	
				CLRS_T.MapIdx = TMapIdx;	
				CLRS_T.RtBIdx = j;	
			}
		}
		TmpRtCtrl.ReSet();
	}
	return nuTRUE;
}
nuBOOL Class_CalRouting::ReFindPointNodeID(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T, nuBOOL b_Local)
{
		nuDWORD i = 0;
#ifndef VALUE_EMGRT
	nuLONG tmpX,tmpY;
#endif
#ifdef VALUE_EMGRT
		for(i=0;i<g_pRtB_F->RtBHeader.TotalNodeCount;i++){
			if(	CheckPtCloseLU(CLRS_F.PtLocalRoute.FixCoor,(*g_pRtB_F->RtBMain)[i].NodeCoor,2) )
			{
				CLRS_F.PtLocalRoute.FixNode1=i;
				CLRS_F.PtLocalRoute.FixNode2=i;
				CLRS_F.PtLocalRoute.FixDis=0;
				break;
			}
		}

#else	//計算相對座標差
		tmpX=CLRS_F.PtLocalRoute.FixCoor.x-g_pRtB_F->OrgCoor.x;
		tmpY=CLRS_F.PtLocalRoute.FixCoor.y-g_pRtB_F->OrgCoor.y;
		for(i=0;i<g_pRtB_F->RtBHeader.TotalNodeCount;i++){
			if(	tmpX==(nuLONG)((*g_pRtB_F->RtBMain)[i].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[i].NodeCoor.x) &&
				tmpY==(nuLONG)((*g_pRtB_F->RtBMain)[i].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[i].NodeCoor.y) ){
				CLRS_F.PtLocalRoute.FixNode1=i;//最近道路的節點ID1
				CLRS_F.PtLocalRoute.FixNode2=i;//最近道路的節點ID2
				CLRS_F.PtLocalRoute.FixDis=0;
				break;
			}
		}
#endif

	//重新取得NodeID
#ifdef VALUE_EMGRT
		for(i=0;i<g_pRtB_T->RtBHeader.TotalNodeCount;i++){
			if(	CheckPtCloseLU(CLRS_T.PtLocalRoute.FixCoor,(*g_pRtB_T->RtBMain)[i].NodeCoor,2) )
			{
				CLRS_T.PtLocalRoute.FixNode1=i;
				CLRS_T.PtLocalRoute.FixNode2=i;
				CLRS_T.PtLocalRoute.FixDis=0;
				break;
			}
		}
#else   
		tmpX=CLRS_T.PtLocalRoute.FixCoor.x-g_pRtB_T->OrgCoor.x;
		tmpY=CLRS_T.PtLocalRoute.FixCoor.y-g_pRtB_T->OrgCoor.y;
		for(i=0;i<g_pRtB_T->RtBHeader.TotalNodeCount;i++){
			if(	tmpX==(nuLONG)((*g_pRtB_T->RtBMain)[i].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[i].NodeCoor.x) &&
				tmpY==(nuLONG)((*g_pRtB_T->RtBMain)[i].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[i].NodeCoor.y) ){
				CLRS_T.PtLocalRoute.FixNode1=i;//最近道路的節點ID1
				CLRS_T.PtLocalRoute.FixNode2=i;//最近道路的節點ID2
				CLRS_T.PtLocalRoute.FixDis=0;
				break;
			}
		}
#endif

	// 取得道路兩端規劃點
	if(b_Local==nuTRUE)
	{
			GetTwoSiteDis(g_pRtB_T,CLRS_F);
			GetTwoSiteDis(g_pRtB_T,CLRS_T);
	}
	else
	{
			GetTwoSiteDis(g_pRtB_F,CLRS_F);//取得起始點和道路起始點的距離
			GetTwoSiteDis(g_pRtB_T,CLRS_T);//取得終點和道路終點的距離
	}
	return nuTRUE;
}
nuBOOL Class_CalRouting::ColationDataAgain(PTFIXTOROADSTRU &FPt,PTFIXTOROADSTRU &TPt,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule,nuLONG FMapIdx, nuLONG TMapIdx)
{
#ifndef VALUE_EMGRT
	nuLONG tmpX,tmpY;
#endif
	nuUSHORT j;
	nuULONG i;
	UONERtCtrl TmpRtCtrl;
	/*** 將FixInfo的資訊添加在規劃專用起訖點結構 ***/
	CLRS_F.PtLocalRoute = FPt;
	CLRS_T.PtLocalRoute = TPt;
		
	/*** 開啟Rt檔 比對MapID RTBID以取得MApIdx RtBIdx ***/
	if(m_nRTFINDRTIDX == 0 || (FMapIdx == -1 || TMapIdx == -1)) 
	{
		for(i=0;i<m_MapIDtoMapIdx.MapCount;i++)
		{
			if( (*m_MapIDtoMapIdx.MapIDList)[i]!=FPt.MapID && (*m_MapIDtoMapIdx.MapIDList)[i]!=TPt.MapID ) continue;
			
			#ifdef DEF_MAKETHRROUTE
				TmpRtCtrl.InitClass();
				nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
				nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
			#else
				TmpRtCtrl.InitClass();
#ifndef _USERT6
				nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
#else
				if(b_LocalRut && m_bReadRt6)
				{
					nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt6"));
				}
				else
				{
					nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
				}
#endif
				g_pRtFSApi->FS_FindFileWholePath(i,TmpRtCtrl.FName,NURO_MAX_PATH);
			#endif
			if(nuFALSE==TmpRtCtrl.Read_Header()){	ReleaseColationDataAgain(TmpRtCtrl);	Renum_RoutingState=ROUTE_READRTFALSE;	return nuFALSE;	}//讀取RT的Header
			for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
			{//對照起訖點(同張地圖同個區塊的資訊)
				if((*m_MapIDtoMapIdx.MapIDList)[i]==FPt.MapID && (*TmpRtCtrl.Rt_Block_ID_List)[j]==FPt.RTBID)
				{	CLRS_F.MapIdx = (*m_MapIDtoMapIdx.MapIdxList)[i];	CLRS_F.RtBIdx=j;	}
				if((*m_MapIDtoMapIdx.MapIDList)[i]==TPt.MapID && (*TmpRtCtrl.Rt_Block_ID_List)[j]==TPt.RTBID)
				{	CLRS_T.MapIdx = (*m_MapIDtoMapIdx.MapIdxList)[i];	CLRS_T.RtBIdx=j;	}
			}
			TmpRtCtrl.ReSet();
		}
	}
	else
	{
		TmpRtCtrl.InitClass();
#ifndef _USERT6
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
#else
		if(b_LocalRut && m_bReadRt6)
		{
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt6"));
		}
		else
		{
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		}
#endif
		g_pRtFSApi->FS_FindFileWholePath(FMapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header())
		{	
			ReleaseColationDataAgain(TmpRtCtrl);	
			Renum_RoutingState=ROUTE_READRTFALSE;	
			return nuFALSE;	
		}//讀取RT的Header
		if(TmpRtCtrl.MapID != FPt.MapID)
		{
			return nuFALSE;
		}
		for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
		{//對照起訖點(同張地圖同個區塊的資訊)
			if((*TmpRtCtrl.Rt_Block_ID_List)[j]==FPt.RTBID)
			{	
				CLRS_F.MapIdx = FMapIdx;	
				CLRS_F.RtBIdx = j;	
			}
		}
		TmpRtCtrl.ReSet();
		TmpRtCtrl.InitClass();
#ifndef _USERT6
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
#else
		if(b_LocalRut && m_bReadRt6)
		{
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt6"));
		}
		else
		{
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		}
#endif
		g_pRtFSApi->FS_FindFileWholePath(TMapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header())
		{	
			ReleaseColationDataAgain(TmpRtCtrl);	
			Renum_RoutingState=ROUTE_READRTFALSE;	
			return nuFALSE;	
		}//讀取RT的Header
		if(TmpRtCtrl.MapID != TPt.MapID)
		{
			return nuFALSE;
		}
		for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
		{//對照起訖點(同張地圖同個區塊的資訊)
			if((*TmpRtCtrl.Rt_Block_ID_List)[j]==TPt.RTBID)
			{	
				CLRS_T.MapIdx = TMapIdx;	
				CLRS_T.RtBIdx = j;	
			}
		}
		TmpRtCtrl.ReSet();
	}
	nuSleep(TOBESLEEPTIMER);
	
	// 讀取規劃用資料
	if(nuFALSE==ReadBaseData(CLRS_F,CLRS_T,b_Local,l_RoutingRule)) return nuFALSE;//讀取THr的檔案
	nuSleep(TOBESLEEPTIMER);
	if(nuFALSE==ReadSysRTData(CLRS_F,CLRS_T,b_Local,l_RoutingRule)) return nuFALSE;//讀取RTB的檔案
	nuSleep(TOBESLEEPTIMER);
//#ifdef _USETTI
	if(nuFALSE == ReadTTI(CLRS_F, CLRS_T))
	{
		return nuFALSE;
	}
//#endif

	//重新取得NodeID
#ifdef VALUE_EMGRT
		for(i=0;i<g_pRtB_F->RtBHeader.TotalNodeCount;i++){
			if(	CheckPtCloseLU(CLRS_F.PtLocalRoute.FixCoor,(*g_pRtB_F->RtBMain)[i].NodeCoor,2) )
			{
				CLRS_F.PtLocalRoute.FixNode1=i;
				CLRS_F.PtLocalRoute.FixNode2=i;
				CLRS_F.PtLocalRoute.FixDis=0;
				break;
			}
		}

#else	//計算相對座標差
		tmpX=CLRS_F.PtLocalRoute.FixCoor.x-g_pRtB_F->OrgCoor.x;
		tmpY=CLRS_F.PtLocalRoute.FixCoor.y-g_pRtB_F->OrgCoor.y;
		for(i=0;i<g_pRtB_F->RtBHeader.TotalNodeCount;i++){
			if(	tmpX==(nuLONG)((*g_pRtB_F->RtBMain)[i].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[i].NodeCoor.x) &&
				tmpY==(nuLONG)((*g_pRtB_F->RtBMain)[i].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[i].NodeCoor.y) ){
				CLRS_F.PtLocalRoute.FixNode1=i;//最近道路的節點ID1
				CLRS_F.PtLocalRoute.FixNode2=i;//最近道路的節點ID2
				CLRS_F.PtLocalRoute.FixDis=0;
				break;
			}
		}
#endif

	//重新取得NodeID
#ifdef VALUE_EMGRT
		for(i=0;i<g_pRtB_T->RtBHeader.TotalNodeCount;i++){
			if(	CheckPtCloseLU(CLRS_T.PtLocalRoute.FixCoor,(*g_pRtB_T->RtBMain)[i].NodeCoor,2) )
			{
				CLRS_T.PtLocalRoute.FixNode1=i;
				CLRS_T.PtLocalRoute.FixNode2=i;
				CLRS_T.PtLocalRoute.FixDis=0;
				break;
			}
		}
#else   
		tmpX=CLRS_T.PtLocalRoute.FixCoor.x-g_pRtB_T->OrgCoor.x;
		tmpY=CLRS_T.PtLocalRoute.FixCoor.y-g_pRtB_T->OrgCoor.y;
		for(i=0;i<g_pRtB_T->RtBHeader.TotalNodeCount;i++){
			if(	tmpX==(nuLONG)((*g_pRtB_T->RtBMain)[i].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[i].NodeCoor.x) &&
				tmpY==(nuLONG)((*g_pRtB_T->RtBMain)[i].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[i].NodeCoor.y) ){
				CLRS_T.PtLocalRoute.FixNode1=i;//最近道路的節點ID1
				CLRS_T.PtLocalRoute.FixNode2=i;//最近道路的節點ID2
				CLRS_T.PtLocalRoute.FixDis=0;
				break;
			}
		}
#endif
	nuSleep(TOBESLEEPTIMER);

	// 取得道路兩端規劃點
	if(b_Local==nuTRUE)
	{
			GetTwoSiteDis(g_pRtB_T,CLRS_F);
			GetTwoSiteDis(g_pRtB_T,CLRS_T);
	}
	else
	{
			GetTwoSiteDis(g_pRtB_F,CLRS_F);//取得起始點和道路起始點的距離
			GetTwoSiteDis(g_pRtB_T,CLRS_T);//取得終點和道路終點的距離
	}

	return nuTRUE;
}

	nuVOID Class_CalRouting::ReleaseColationDataAgain(UONERtCtrl &TmpRtCtrl)
	{
		nuLONG i;
		for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++)
		{
			TmpRtCtrl.ReSetRTB(i);
		}
		TmpRtCtrl.Remove_Header();
	}

//nuVOID Class_CalRouting::GetRouteNodeID(RtBCtrl &TmpRtBCtrl,TmpRoutingInfo &CLRS,nuBOOL b_StartNode)
	nuVOID Class_CalRouting::GetTwoSiteDis(UONERtBCtrl *TmpRtBCtrl,TmpRoutingInfo &CLRS)//從Road去取得最佳的起訖規劃NodeID
	{
		NUROPOINT GTSD_Pt[3];
		nuLONG TmpDis[2];
		nuULONG i;
		CLRS.FixDis1=0;
		CLRS.FixDis2=0;

		GTSD_Pt[0].x=CLRS.PtLocalRoute.FixCoor.x;
		GTSD_Pt[0].y=CLRS.PtLocalRoute.FixCoor.y;
#ifdef VALUE_EMGRT
		GTSD_Pt[1].x=(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].NodeCoor.x;
		GTSD_Pt[1].y=(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].NodeCoor.y;
		GTSD_Pt[2].x=(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].NodeCoor.x;
		GTSD_Pt[2].y=(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].NodeCoor.y;
#else
		GTSD_Pt[1].x=TmpRtBCtrl->OrgCoor.x+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].CoorLVX*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].NodeCoor.x;
		GTSD_Pt[1].y=TmpRtBCtrl->OrgCoor.y+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].CoorLVY*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].NodeCoor.y;
		GTSD_Pt[2].x=TmpRtBCtrl->OrgCoor.x+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].CoorLVX*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].NodeCoor.x;
		GTSD_Pt[2].y=TmpRtBCtrl->OrgCoor.y+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].CoorLVY*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].NodeCoor.y;
#endif

		/*nuPtFixToLineSeg(CLRS.PtLocalRoute.FixCoor,GTSD_Pt[1],GTSD_Pt[2],&GTSD_Pt[0],&TmpDis[0]);

		if(GTSD_Pt[1].x==GTSD_Pt[0].x && GTSD_Pt[1].y==GTSD_Pt[0].y){
			CLRS.FixDis1=0;
			CLRS.FixDis2=0;
			CLRS.PtLocalRoute.FixNode2=CLRS.PtLocalRoute.FixNode1;
			return;
		}
		else if(GTSD_Pt[2].x==GTSD_Pt[0].x && GTSD_Pt[2].y==GTSD_Pt[0].y){
			CLRS.FixDis1=0;
			CLRS.FixDis2=0;
			CLRS.PtLocalRoute.FixNode1=CLRS.PtLocalRoute.FixNode2;
			return;
		}*/

		/*** 計算出發點與所在道路的兩端點距離 ***/
		TmpDis[0]=NURO_ABS(GTSD_Pt[1].x-GTSD_Pt[0].x)+NURO_ABS(GTSD_Pt[1].y-GTSD_Pt[0].y);
		TmpDis[1]=NURO_ABS(GTSD_Pt[2].x-GTSD_Pt[0].x)+NURO_ABS(GTSD_Pt[2].y-GTSD_Pt[0].y);
		/*** 規劃點即端點的處理 ***/
		if(TmpDis[0]<3)		{	CLRS.FixDis1=0;	CLRS.FixDis2=0;	CLRS.PtLocalRoute.FixNode2=CLRS.PtLocalRoute.FixNode1;	return;	}
		else if(TmpDis[1]<3)	{	CLRS.FixDis1=0;	CLRS.FixDis2=0;	CLRS.PtLocalRoute.FixNode1=CLRS.PtLocalRoute.FixNode2;	return;	}
		else
		{
			//進行長度處理(粗估)
			for(i=0;i<(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].ConnectCount;i++)
			{
				if( (*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].ConnectStruIndex-1+i].TRtBID==CLRS.PtLocalRoute.RTBID &&
					(*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].ConnectStruIndex-1+i].TNodeID==CLRS.PtLocalRoute.FixNode2)
				{
					CLRS.FixDis2=(*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode1].ConnectStruIndex-1+i].RoadLength*TmpDis[1]/(TmpDis[0]+TmpDis[1]);
				}
			}
			
			for(i=0;i<(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].ConnectCount;i++)
			{
				if( (*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].ConnectStruIndex-1+i].TRtBID==CLRS.PtLocalRoute.RTBID &&
					(*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].ConnectStruIndex-1+i].TNodeID==CLRS.PtLocalRoute.FixNode1)
				{
					CLRS.FixDis1=(*TmpRtBCtrl->RtBConnect)[(*TmpRtBCtrl->RtBMain)[CLRS.PtLocalRoute.FixNode2].ConnectStruIndex-1+i].RoadLength*TmpDis[0]/(TmpDis[0]+TmpDis[1]);
				}
			}
		}
	}

//希望能找出最近的七號路網點(class 0~7)
//希望可以達到規劃加速的效果
//這裡所用的搜尋方式 跟一般的規劃是一樣的
nuBOOL Class_CalRouting::FindNearEnd7Net(AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuLONG &Dis_Rd7,nuULONG &LimitNodeID_7)
{
	nuBOOL bRadiusOK = nuFALSE;
	LpAVLTree m_SAVLTree;
	AStarData m_AStarData;
	nuULONG i;
	nuBOOL b_GotoNext=nuTRUE;
	m_AStar.IniData();
	nuLONG TmpDis[2],tmp_GuessDis;
	WEIGHTINGSTRU BestChoose,NewWeighting;

	TWalkingNode=_DEFNODEID;

	TmpDis[0]=0;
	TmpDis[1]=0;
	
	NUROPOINT Temp_DifCoor,Temp_FDifCoor;
#ifdef VALUE_EMGRT
	Temp_DifCoor.x=CLRS_F.PtLocalRoute.FixCoor.x;
	Temp_DifCoor.y=CLRS_F.PtLocalRoute.FixCoor.y;
#else
	Temp_DifCoor.x=CLRS_F.PtLocalRoute.FixCoor.x-g_pRtB_T->OrgCoor.x;
	Temp_DifCoor.y=CLRS_F.PtLocalRoute.FixCoor.y-g_pRtB_T->OrgCoor.y;
#endif
	//初始化 先加兩筆從終點出發 往起點的資料
	if(CLRS_T.PtLocalRoute.FixNode1==CLRS_T.PtLocalRoute.FixNode2)
	{
		for(i=0;i<(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectCount;i++)
		{
			m_AStarData.FMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.FRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.FNodeID=CLRS_T.PtLocalRoute.FixNode1;
			m_AStarData.NMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.NRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.NNodeID=CLRS_T.PtLocalRoute.FixNode1;
			if((*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID == _THRRTBID)
			{
				continue;
			}
			else{
				m_AStarData.TMapID=g_pRtB_T->MyState.MapID;
			}
			m_AStarData.TRtBID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID;
			m_AStarData.TNodeID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TNodeID;
			m_AStarData.Class=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadClass;
#ifdef VALUE_EMGRT
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#else
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_DifCoor);
			NewWeighting.RealDis=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadLength;
			if(!GetWeighting(m_AStarData.Class)) continue;
			m_AStarData.NowDis = (*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadLength * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis   = 0;
			NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
			if(m_AStarData.Class == _WALKINGRDLEVEL)	
			{
				m_AStarData.WalkingFlag=1;
			}
			else
			{
				m_AStarData.WalkingFlag=0;
			}
#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				if((m_lShortestCount < _DNEARTARGETNODECOUNT) && m_AStarData.Class <= 12)
				{	
					//m_pShortestData[m_lShortestCount] = m_AStarData;
					m_pShortestData[m_lShortestCount].NNodeID = m_AStarData.FNodeID;
					m_pShortestData[m_lShortestCount].NRtBID = m_AStarData.FRtBID;
					m_pShortestData[m_lShortestCount].NMapID = m_AStarData.FMapID;
					m_pShortestData[m_lShortestCount].TNodeID = m_AStarData.NNodeID;
					m_pShortestData[m_lShortestCount].TRtBID = m_AStarData.NRtBID;				
					m_pShortestData[m_lShortestCount].TMapID = m_AStarData.NMapID;					
					m_pShortestData[m_lShortestCount].NMapLayer = _MAPLAYER_TRT;
					m_pShortestData[m_lShortestCount].TMapLayer = _MAPLAYER_TRT;
					m_pShortestweight[m_lShortestCount] = NewWeighting;
					m_lShortestCount++;
				}
			}
#endif
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStar.AddToOpen(m_AStarData,0,NewWeighting);
			break;
		}
	}
	else
	{
		TmpDis[0]=NURO_ABS((*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].NodeCoor.x-CLRS_T.PtLocalRoute.FixCoor.x)+NURO_ABS((*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].NodeCoor.y-CLRS_T.PtLocalRoute.FixCoor.y);
		TmpDis[1]=NURO_ABS((*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].NodeCoor.x-CLRS_T.PtLocalRoute.FixCoor.x)+NURO_ABS((*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].NodeCoor.y-CLRS_T.PtLocalRoute.FixCoor.y);

		for(i=0;i<(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectCount;i++){
			m_AStarData.FMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.FRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.FNodeID=CLRS_T.PtLocalRoute.FixNode1;
			m_AStarData.NMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.NRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.NNodeID=CLRS_T.PtLocalRoute.FixNode1;
			if((*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID == _THRRTBID)
			{//如果遇到跨區就排除
				continue;
			}
			else{
				m_AStarData.TMapID=g_pRtB_T->MyState.MapID;//地圖索引值
			}
			m_AStarData.TRtBID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID;
			m_AStarData.TNodeID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TNodeID;
			if(m_AStarData.TMapID!=CLRS_T.PtLocalRoute.MapID)		continue;
			if(m_AStarData.TRtBID!=CLRS_T.PtLocalRoute.RTBID)		continue;
			if(m_AStarData.TNodeID!=CLRS_T.PtLocalRoute.FixNode2)	continue;
			m_AStarData.Class=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadClass;

#ifdef VALUE_EMGRT
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#else
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_DifCoor);
			if((TmpDis[0]+TmpDis[1])==0)	NewWeighting.RealDis=0;
			else							NewWeighting.RealDis=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadLength*TmpDis[1]/(TmpDis[0]+TmpDis[1]);
			if(!GetWeighting(m_AStarData.Class)) continue;
			m_AStarData.NowDis = NewWeighting.RealDis * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis=0;
			NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
			if(m_AStarData.Class == _WALKINGRDLEVEL)	
			{
				m_AStarData.WalkingFlag=1;
			}
			else
			{
				m_AStarData.WalkingFlag=0;
			}
#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				if((m_lShortestCount < _DNEARTARGETNODECOUNT)  && m_AStarData.Class <= 12)
				{		
					/*m_pShortestData[m_lShortestCount] = m_AStarData;*/
					m_pShortestData[m_lShortestCount].NNodeID = m_AStarData.FNodeID;
					m_pShortestData[m_lShortestCount].TNodeID = m_AStarData.NNodeID;
					m_pShortestData[m_lShortestCount].NRtBID = m_AStarData.FRtBID;
					m_pShortestData[m_lShortestCount].TRtBID = m_AStarData.NRtBID;
					m_pShortestData[m_lShortestCount].NMapID = m_AStarData.FMapID;
					m_pShortestData[m_lShortestCount].TMapID = m_AStarData.NMapID;
					m_pShortestData[m_lShortestCount].NMapLayer = _MAPLAYER_TRT;
					m_pShortestData[m_lShortestCount].TMapLayer = _MAPLAYER_TRT;
					m_pShortestweight[m_lShortestCount] = NewWeighting;
					m_lShortestCount++;
				}
			}
#endif
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStar.AddToOpen(m_AStarData,0,NewWeighting);
			break;
		}

		for(i=0;i<(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectCount;i++)
		{//m_AStarData.FMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.FRtBID=CLRS_T.PtLocalRoute.RTBID;是重複的
			m_AStarData.FMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.FRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.FNodeID=CLRS_T.PtLocalRoute.FixNode2;
			m_AStarData.NMapID=CLRS_T.PtLocalRoute.MapID;	m_AStarData.NRtBID=CLRS_T.PtLocalRoute.RTBID;	m_AStarData.NNodeID=CLRS_T.PtLocalRoute.FixNode2;
			if((*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TRtBID == _THRRTBID)
			{
				continue;
			}
			else
			{
				m_AStarData.TMapID=g_pRtB_T->MyState.MapID;
			}
			m_AStarData.TRtBID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TRtBID;
			m_AStarData.TNodeID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TNodeID;
			if(m_AStarData.TMapID!=CLRS_T.PtLocalRoute.MapID)		continue;
			if(m_AStarData.TRtBID!=CLRS_T.PtLocalRoute.RTBID)		continue;
			if(m_AStarData.TNodeID!=CLRS_T.PtLocalRoute.FixNode1)	continue;
			m_AStarData.Class=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].RoadClass;

#ifdef VALUE_EMGRT
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#else
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_DifCoor);
			if((TmpDis[0]+TmpDis[1])==0)	NewWeighting.RealDis=0;
			else							NewWeighting.RealDis=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[CLRS_T.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].RoadLength*TmpDis[0]/(TmpDis[0]+TmpDis[1]);
			if(!GetWeighting(m_AStarData.Class)) continue;
			m_AStarData.NowDis = NewWeighting.RealDis * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis   = 0;
			NewWeighting.TotalDis = tmp_GuessDis + m_AStarData.NowDis + m_AStarData.RDis;
			if(m_AStarData.Class == _WALKINGRDLEVEL)
			{
				m_AStarData.WalkingFlag=1;
			}
			else	
			{
				m_AStarData.WalkingFlag=0;
			}
#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				if((m_lShortestCount < _DNEARTARGETNODECOUNT) && m_AStarData.Class <= 12)
				{		
					/*m_pShortestData[m_lShortestCount] = m_AStarData;*/
					m_pShortestData[m_lShortestCount].NNodeID = m_AStarData.FNodeID;
					m_pShortestData[m_lShortestCount].TNodeID = m_AStarData.NNodeID;
					m_pShortestData[m_lShortestCount].NRtBID = m_AStarData.FRtBID;
					m_pShortestData[m_lShortestCount].TRtBID = m_AStarData.NRtBID;
					m_pShortestData[m_lShortestCount].NMapID = m_AStarData.FMapID;
					m_pShortestData[m_lShortestCount].TMapID = m_AStarData.NMapID;
					m_pShortestData[m_lShortestCount].NMapLayer = _MAPLAYER_TRT;
					m_pShortestData[m_lShortestCount].TMapLayer = _MAPLAYER_TRT;
					m_pShortestweight[m_lShortestCount] = NewWeighting;
					m_lShortestCount++;
				}
			}
#endif
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStar.AddToOpen(m_AStarData,0,NewWeighting);
			break;
		}
	}

	nuLONG SleepCount;
	SleepCount=0;
	while(nuTRUE)
	{
		#ifndef DEF_MAKETHRROUTE
			#ifdef TOBE_SYSTEM
				SleepCount++;
				if(SleepCount==100){
					SleepCount=0;
					nuSleep(TOBESLEEPTIMER);
				}
			#endif
		#endif
		//取得最優Open端資料 填入Close端
		if(!m_AStar.GetBestOpenToClose(m_SAVLTree,BestChoose))
		{
	#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				m_lShortestDis = 0;
			}
	#endif 
			return nuFALSE;
		}
		/*if(m_SAVLTree->Data.Class <= _CONNECTLEVELCLASSLIMIT)
		{
			Dis_Rd7=BestChoose.RealDis;
			LimitNodeID_7=m_SAVLTree->Data.FNodeID;
	#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				m_lShortestDis = 0;
			}
	#endif 
			return nuTRUE;
		}*/
		if(bRadiusOK)//
		{
			Dis_Rd7=BestChoose.RealDis;
			LimitNodeID_7=m_SAVLTree->Data.FNodeID;
	#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				m_lShortestDis = 0;
			}
	#endif 
			return nuTRUE;
		}

		//只使用終點區塊的資料
		if(!(m_SAVLTree->Data.TMapID==g_pRtB_T->MyState.MapID) || !(m_SAVLTree->Data.TRtBID==g_pRtB_T->MyState.RtBlockID)){	continue;	}
		
		for(i=0;i<(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectCount;i++)
		{
			b_GotoNext=nuTRUE;
			m_AStarData.FMapID=m_SAVLTree->Data.NMapID;	m_AStarData.FRtBID=m_SAVLTree->Data.NRtBID;	m_AStarData.FNodeID=m_SAVLTree->Data.NNodeID;
			m_AStarData.NMapID=m_SAVLTree->Data.TMapID;	m_AStarData.NRtBID=m_SAVLTree->Data.TRtBID;	m_AStarData.NNodeID=m_SAVLTree->Data.TNodeID;

			if((*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].TRtBID == _THRRTBID)
			{
				continue;
			}
			else
			{
				m_AStarData.TMapID=g_pRtB_T->MyState.MapID;
			}
			m_AStarData.TRtBID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].TRtBID;
			m_AStarData.TNodeID=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].TNodeID;

			//如果連通點不是在現在這個區域的 就捨棄囉
			if(!(m_AStarData.TMapID==g_pRtB_T->MyState.MapID && m_AStarData.TRtBID==g_pRtB_T->MyState.RtBlockID)) continue;

			m_AStarData.Class=(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].RoadClass;

#ifdef VALUE_EMGRT
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#else
			Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
#endif
			//tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_DifCoor);
			NewWeighting.RealDis=BestChoose.RealDis+(*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].RoadLength;

			if(!GetWeighting(m_AStarData.Class)) continue;
			m_AStarData.NowDis = (*g_pRtB_T->RtBConnect)[(*g_pRtB_T->RtBMain)[m_SAVLTree->Data.TNodeID].ConnectStruIndex+i-1].RoadLength * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis=m_SAVLTree->Data.RDis+m_SAVLTree->Data.NowDis;
			NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;

			if(m_AStarData.Class == _WALKINGRDLEVEL && m_SAVLTree->Data.WalkingFlag==1)
			{
				m_AStarData.WalkingFlag=1;
			}
			else
			{
				m_AStarData.WalkingFlag=0;
			}

			if(m_AStarData.WalkingFlag==0 && TWalkingNode==_DEFNODEID){
				TWalkingNode=m_SAVLTree->Data.TNodeID;
			}
			if( m_AStarData.FNodeID==m_AStarData.TNodeID &&
				m_AStarData.FRtBID==m_AStarData.TRtBID &&
				m_AStarData.FMapID==m_AStarData.TMapID	)//如果起訖點為同一點
			{
				continue;
			}
#ifdef _DROUTE_ERROR
			if((m_lShortestCount < _DNEARTARGETNODECOUNT) && m_AStarData.Class <= 12)
			{		
				//m_pShortestData[m_lShortestCount] = m_AStarData;
				m_pShortestData[m_lShortestCount].NNodeID = m_AStarData.FNodeID;
				m_pShortestData[m_lShortestCount].TNodeID = m_AStarData.NNodeID;
				m_pShortestData[m_lShortestCount].NRtBID = m_AStarData.FRtBID;
				m_pShortestData[m_lShortestCount].TRtBID = m_AStarData.NRtBID;
				m_pShortestData[m_lShortestCount].NMapID = m_AStarData.FMapID;
				m_pShortestData[m_lShortestCount].TMapID = m_AStarData.NMapID;
				m_pShortestData[m_lShortestCount].NMapLayer = _MAPLAYER_TRT;
				m_pShortestData[m_lShortestCount].TMapLayer = _MAPLAYER_TRT;
				m_pShortestweight[m_lShortestCount] = NewWeighting;
				m_lShortestCount++;
			}
#endif
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStar.AddToOpen(m_AStarData,0,NewWeighting);
		}
#ifdef _DROUTE_ERROR
		if(m_lShortestCount >= 15)// 
		{
			bRadiusOK = nuTRUE;
		}
#endif
	}
	return nuFALSE;
}

nuVOID Class_CalRouting::SetAstarBase(nuBOOL b_Local)
{
	b_LocalRut=b_Local;
}

nuBOOL Class_CalRouting::AStar(AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuLONG l_RoutingRule)//規劃
{
	m_lMaxRoadClassCount = 0;
	nuMemset(SRTTypeCount, 0, sizeof(nuSHORT) * 20);
	nuLONG SleepCount;
	#ifndef DEF_MAKETHRROUTE
		g_pCDTH->SystemReRouting();
	#endif
	LpAVLTree p_SAVLTree;
	AStarData m_AStarData,tmp_SAVLData;
	nuBOOL b_StartLevelCut=nuFALSE,b_CloseStart=nuFALSE,b_CloseThr=nuFALSE;//等級篩選判斷因子
	nuLONG Dis_Rd7,DifAng,EndBoundaryRange=20000,tmp_GuessDis;
	nuULONG i,LimitNodeID_7 = _MAXUNLONG, Run8E_Dis = 0;
	NUROPOINT TargetCoor,Temp_FDifCoor;
	WEIGHTINGSTRU BestChoose;
	TWalkingNode=_DEFNODEID;

	#ifdef VALUE_EMGRT
		Temp_S_DiffCoor.x=CLRS_T.PtLocalRoute.FixCoor.x;
		Temp_S_DiffCoor.y=CLRS_T.PtLocalRoute.FixCoor.y;
		Temp_T_DiffCoor.x=CLRS_T.PtLocalRoute.FixCoor.x;
		Temp_T_DiffCoor.y=CLRS_T.PtLocalRoute.FixCoor.y;
	#else//求區域原點和起訖點的座標差
		Temp_S_DiffCoor.x=CLRS_T.PtLocalRoute.FixCoor.x-g_pRtB_F->OrgCoor.x;
		Temp_S_DiffCoor.y=CLRS_T.PtLocalRoute.FixCoor.y-g_pRtB_F->OrgCoor.y;
		Temp_T_DiffCoor.x=CLRS_T.PtLocalRoute.FixCoor.x-g_pRtB_T->OrgCoor.x;
		Temp_T_DiffCoor.y=CLRS_T.PtLocalRoute.FixCoor.y-g_pRtB_T->OrgCoor.y;
	#endif
//	if(l_RoutingRule & 0x01000000)	b_PassTollgate=nuTRUE;//是否排除收費路段
//	else							b_PassTollgate=nuFALSE;//是否排除收費路段


	TargetCoor=CLRS_T.PtLocalRoute.FixCoor;//終點座標點
	AveLatitude_l = nulCos(NURO_ABS((CLRS_T.PtLocalRoute.FixCoor.y+CLRS_F.PtLocalRoute.FixCoor.y) / 2) /
		_EACHDEGREECOORSIZE) * 1113 / _LONLATBASE;

	//找出最近的8號路網點及距離
	//且距離8號路網點至少要2.5公里以上才行
	Dis_Rd7 = _LEVELCHOOSEDIS1;//2.5KM
#ifdef _DROUTE_ERROR
	if(!b_LocalRut)
	{
		FindNearEnd7Net(m_AStar,CLRS_F,CLRS_T,Dis_Rd7,LimitNodeID_7);
	}
#endif
	EndBoundaryRange=Dis_Rd7+1000;
	if(EndBoundaryRange < _LEVELCHOOSEDIS1) 
	{
		EndBoundaryRange = _LEVELCHOOSEDIS1;
	}

	// 初始化AStar暫存資料結構
	m_AStar.IniData();

	//初始化區域關閉機制
	b_CloseStart=nuFALSE;
	b_CloseThr=nuFALSE;

	//起點兩端NodeID相同(在端點上)	則直接由這一點擴散
	//起點兩端NodeID不同(在路上)	則往兩端通行均可
	m_AStarData.FMapID=CLRS_F.PtLocalRoute.MapID;
	m_AStarData.FRtBID=CLRS_F.PtLocalRoute.RTBID;
	m_AStarData.NMapID=CLRS_F.PtLocalRoute.MapID;
	m_AStarData.NRtBID=CLRS_F.PtLocalRoute.RTBID;
	if(CLRS_F.PtLocalRoute.FixNode1==CLRS_F.PtLocalRoute.FixNode2){
		m_AStarData.FNodeID=CLRS_F.PtLocalRoute.FixNode1;
		m_AStarData.NNodeID=CLRS_F.PtLocalRoute.FixNode1;
		m_AStarData.TMapID=CLRS_F.PtLocalRoute.MapID;
		m_AStarData.TRtBID=CLRS_F.PtLocalRoute.RTBID;
		m_AStarData.TNodeID=CLRS_F.PtLocalRoute.FixNode1;
		if( m_AStarData.TMapID==g_pRtB_T->MyState.MapID && m_AStarData.TRtBID==g_pRtB_T->MyState.RtBlockID )
		{	m_AStarData.FMapLayer=m_AStarData.NMapLayer=m_AStarData.TMapLayer=_MAPLAYER_TRT;	}
		else
		{	
			m_AStarData.FMapLayer = m_AStarData.NMapLayer = m_AStarData.TMapLayer = _MAPLAYER_SRT;	
		}
		m_AStarData.Class = _GENERALRDLEVEL;

		#ifdef VALUE_EMGRT
			Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
		#else
			Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
			Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
		#endif
		tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
		BestChoose.RealDis=0;
		m_AStarData.NowDis=0;
		m_AStarData.RDis=0;
		BestChoose.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
		m_AStarData.FNoTurnFlag=1;
		m_AStarData.NNoTurnFlag=1;
		m_AStarData.TNoTurnFlag=1;
		m_AStarData.WalkingFlag=1;//轉彎標示

		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,BestChoose);
		b_FirstRdEffect=nuFALSE;
	}
	else
	{
		for(i=0;i<(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ConnectCount;i++){
			m_AStarData.FNodeID=CLRS_F.PtLocalRoute.FixNode1;
			m_AStarData.NNodeID=CLRS_F.PtLocalRoute.FixNode1;
			m_AStarData.TMapID=g_pRtB_F->MyState.MapID;
			if((*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID == 4094
#ifdef _DAR
				|| (*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ARFlag == 1
#endif
			  )
			{
				m_AStarData.TRtBID = m_AStarData.NRtBID;
			}
			else
			{
				m_AStarData.TRtBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TRtBID;
			}
			m_AStarData.TNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].TNodeID;

			if(m_AStarData.TMapID!=CLRS_F.PtLocalRoute.MapID)		continue;
			if(m_AStarData.TRtBID!=CLRS_F.PtLocalRoute.RTBID)		continue;
			if(m_AStarData.TNodeID!=CLRS_F.PtLocalRoute.FixNode2)	continue;

			if(m_AStarData.TMapID==g_pRtB_T->MyState.MapID && m_AStarData.TRtBID==g_pRtB_T->MyState.RtBlockID)
			{	m_AStarData.FMapLayer=m_AStarData.NMapLayer=m_AStarData.TMapLayer=_MAPLAYER_TRT;	}
			else
			{
				m_AStarData.FMapLayer=m_AStarData.NMapLayer=m_AStarData.TMapLayer = _MAPLAYER_SRT;	
			}

			m_AStarData.Class=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].ConnectStruIndex+i-1].RoadClass;

			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#endif
			if(l_RoutingRule==6 && m_AStarData.Class<5) continue;//l_RoutingRule=6為機踏車模式，避開高速
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);

			BestChoose.RealDis=CLRS_F.FixDis2;
			if(!GetWeighting(m_AStarData.Class)) continue;

			m_AStarData.NowDis = CLRS_F.FixDis2 * ClassWeighting / _WEIGHTINGBASE;

			#ifndef DEF_MAKETHRROUTE
				if(CarAngle!=-1){
					DifAng=CarAngle-nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
					if(90<DifAng && DifAng<270)	
					{
						m_AStarData.NowDis += _DOUBLE_CARDIRECTIONEFFECTDIS;
					}
					if(-270<DifAng && DifAng<-90)
					{
						m_AStarData.NowDis += _DOUBLE_CARDIRECTIONEFFECTDIS;
					}
				}
			#endif
			m_AStarData.RDis=0;
			BestChoose.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
			m_AStarData.FNoTurnFlag=1;
			m_AStarData.NNoTurnFlag=1;
			m_AStarData.TNoTurnFlag=1;

			if(m_AStarData.Class == _WALKINGRDLEVEL)
			{
				m_AStarData.WalkingFlag=1;
			}
			m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,BestChoose);
			break;
		}

		for(i=0;i<(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ConnectCount;i++){
			m_AStarData.FNodeID=CLRS_F.PtLocalRoute.FixNode2;
			m_AStarData.NNodeID=CLRS_F.PtLocalRoute.FixNode2;
			m_AStarData.TMapID=g_pRtB_F->MyState.MapID;
			if((*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TRtBID == 4094
#ifdef _DAR
				|| (*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ARFlag == 1
#endif
				)
			{
				m_AStarData.TRtBID = m_AStarData.NRtBID;
			}
			else
			{
				m_AStarData.TRtBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TRtBID;
			}

			m_AStarData.TNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].TNodeID;
			if(m_AStarData.TMapID!=CLRS_F.PtLocalRoute.MapID)		continue;
			if(m_AStarData.TRtBID!=CLRS_F.PtLocalRoute.RTBID)		continue;
			if(m_AStarData.TNodeID!=CLRS_F.PtLocalRoute.FixNode1)	continue;

			if(m_AStarData.TMapID==g_pRtB_T->MyState.MapID && m_AStarData.TRtBID==g_pRtB_T->MyState.RtBlockID)
			{	m_AStarData.FMapLayer=m_AStarData.NMapLayer=m_AStarData.TMapLayer=_MAPLAYER_TRT;	}
			else
			{	m_AStarData.FMapLayer = m_AStarData.NMapLayer = m_AStarData.TMapLayer = _MAPLAYER_SRT;	}

			m_AStarData.Class=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].ConnectStruIndex+i-1].RoadClass;

			if(l_RoutingRule==6 && m_AStarData.Class<5) continue;
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
			BestChoose.RealDis=CLRS_F.FixDis1;
			if(!GetWeighting(m_AStarData.Class)) continue;

			m_AStarData.NowDis = CLRS_F.FixDis1 * ClassWeighting / _WEIGHTINGBASE;


		#ifndef DEF_MAKETHRROUTE			
			if(CarAngle!=-1){
				DifAng=CarAngle-nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
				if(90<DifAng && DifAng<270)		
				{
					m_AStarData.NowDis += _DOUBLE_CARDIRECTIONEFFECTDIS;
				}
				if(-270<DifAng && DifAng<-90)
				{
					m_AStarData.NowDis += _DOUBLE_CARDIRECTIONEFFECTDIS;
				}
			}
		#endif
			m_AStarData.RDis=0;
			BestChoose.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
			m_AStarData.FNoTurnFlag=1;
			m_AStarData.NNoTurnFlag=1;
			m_AStarData.TNoTurnFlag=1;
			if(m_AStarData.Class == _WALKINGRDLEVEL)
			{
				m_AStarData.WalkingFlag = 1;
			}
			m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,BestChoose);	
			break;

		}
		CarAngle=-1;//車頭方向功能消失
		b_FirstRdEffect=nuTRUE;
	}
#ifdef _DAR
	if(!b_LocalRut && m_bARNearbyAR)
	{
		if(m_pclsAR->AR_Node_Compare(CLRS_F.PtLocalRoute.FixNode1,m_AStarData.NRtBID))
		{
			m_AStarData.FNodeID=CLRS_F.PtLocalRoute.FixNode2;
			m_AStarData.NNodeID=CLRS_F.PtLocalRoute.FixNode1;

			m_AStarData.TRtBID	  = m_AStarData.NRtBID;
			m_AStarData.TMapID    = g_pRtB_T->MyState.MapID;
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStarData.TNodeID   = m_pclsAR->ARtrans_C.NodeID;//取得道路編號 
			m_AStarData.TRtBID    = m_pclsAR->ARtrans_C.RTBID;
			m_AStarData.Class	  = (*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].ConnectStruIndex].RoadClass;

			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
			BestChoose.RealDis=CLRS_F.FixDis1;
			GetWeighting(m_AStarData.Class);
	
			m_AStarData.NowDis  = CLRS_F.FixDis1 * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis    = 0;
			BestChoose.TotalDis = tmp_GuessDis + m_AStarData.NowDis + m_AStarData.RDis;
			m_AStarData.FNoTurnFlag = 1;
			m_AStarData.NNoTurnFlag = 1;
			m_AStarData.TNoTurnFlag = 1;
			if(m_AStarData.Class == _WALKINGRDLEVEL) 
			{
				m_AStarData.WalkingFlag=1;	
			}
			m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,BestChoose);
		}
		else if(m_pclsAR->AR_Node_Compare(CLRS_F.PtLocalRoute.FixNode2,m_AStarData.NRtBID))
		{
			m_AStarData.FNodeID=CLRS_F.PtLocalRoute.FixNode1;
			m_AStarData.NNodeID=CLRS_F.PtLocalRoute.FixNode2;
		
			m_AStarData.TRtBID = m_AStarData.NRtBID;
			m_AStarData.TMapID    = g_pRtB_T->MyState.MapID;
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
			m_AStarData.TNodeID   = m_pclsAR->ARtrans_C.NodeID;//取得道路編號 
			m_AStarData.TRtBID    = m_pclsAR->ARtrans_C.RTBID;
			m_AStarData.Class	  = (*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].ConnectStruIndex].RoadClass;

			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
			BestChoose.RealDis=CLRS_F.FixDis1;
			GetWeighting(m_AStarData.Class);

			m_AStarData.NowDis  = CLRS_F.FixDis1 * ClassWeighting / _WEIGHTINGBASE;
			m_AStarData.RDis    = 0;
			BestChoose.TotalDis = tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
			m_AStarData.FNoTurnFlag = 1;
			m_AStarData.NNoTurnFlag = 1;
			m_AStarData.TNoTurnFlag = 1;
			if(m_AStarData.Class == _WALKINGRDLEVEL)
			{
				m_AStarData.WalkingFlag=1;		
			}
			m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,BestChoose);
		}
	}
#endif

#ifdef THC_THR_LINK
	if(!m_CMP.ReadFile())
	{
		m_CMP.CloseClass();
	}
#endif
	//針對大陸圖資所要使用的特殊限制方式
	SleepCount=0;
	while(nuTRUE)
	{
		#ifndef DEF_MAKETHRROUTE	
			//#ifdef TOBE_SYSTEM
				SleepCount++;
				if(SleepCount==100){
					SleepCount=0;
					nuSleep(TOBESLEEPTIMER);
				}
			//#endif
		#endif
		if(b_StopRoute) return nuFALSE;
		//取得最優Open端資料 填入Close端
		if(!m_AStar.GetBestOpenToClose(p_SAVLTree,BestChoose))
		{	
		#ifdef _DROUTE_ERROR
			if(!b_LocalRut)
			{
				LpAVLTree ReturnNode = NULL;
				for(nuULONG z = 0; z < m_lShortestCount; z++)
				{
					if(!m_AStar.GetGivenData(m_pShortestData[z],ReturnNode,m_pShortestData[z].NMapLayer))
					{
						continue;
					}
					else
					{
						nuMemcpy(&m_ApprocahAStarData,&(m_pShortestData[z]),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
						m_ApprocahAStarData.NMapLayer=m_pShortestData[z].NMapLayer;
						m_ApprocahAStarData.TMapLayer=m_pShortestData[z].TMapLayer;
						ThrGDis=m_pShortestweight[z].TotalDis;
						ThrRDis=m_pShortestweight[z].RealDis;
						TargetClass=m_pShortestData[z].Class;
						m_bErrorExceptionHandle = nuTRUE;
						return nuTRUE;
					}
				}
				nuMemcpy(&m_ApprocahAStarData,&(m_ShortestAStarData),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=m_ShortestAStarData.NMapLayer;
				m_ApprocahAStarData.TMapLayer=m_ShortestAStarData.TMapLayer;
				ThrGDis=m_ShortestChoose.TotalDis;
				ThrRDis=m_ShortestChoose.RealDis;
				TargetClass=m_ShortestAStarData.Class;
				m_bErrorExceptionHandle = nuTRUE;
				return nuTRUE;
			}
			else
			{
				#ifdef _DEBUG
			__android_log_print(ANDROID_LOG_INFO, "RoadList", " SRT %d STHR %d THC %d, TTHR %d, TRT %d, lUsedOpenCount %d, lCallOpenCount %d, lCallCloseCount %d\n", m_AStar.Count_SRT, m_AStar.Count_STHR, m_AStar.Count_THC, m_AStar.Count_TTHR,m_AStar.Count_TRT,m_AStar.lUsedOpenCount, m_AStar.lCallOpenCount, m_AStar.lCallCloseCount);
				#endif
				ThrGDis=-1;
				ThrRDis=-1;
				TargetClass=-1;
				if(m_AStar.lUsedCloseCount==MaxAVLCount)	Renum_RoutingState=ROUTE_USEALLASTARBUFFER;//區域規劃資料整理失敗
				else										Renum_RoutingState=ROUTE_NOFOUNDROADTOENDPOINT;//區域規劃資料整理失敗
				return nuFALSE;
			}
		#else
			#ifdef _DEBUG
			__android_log_print(ANDROID_LOG_INFO, "RoadList", " SRT %d STHR %d THC %d, TTHR %d, TRT %d, lUsedOpenCount %d, lCallOpenCount %d, lCallCloseCount %d\n", m_AStar.Count_SRT, m_AStar.Count_STHR, m_AStar.Count_THC, m_AStar.Count_TTHR,m_AStar.Count_TRT,m_AStar.lUsedOpenCount, m_AStar.lCallOpenCount, m_AStar.lCallCloseCount);
			#endif
			ThrGDis=-1;
			ThrRDis=-1;
			TargetClass=-1;
			if(m_AStar.lUsedCloseCount==MaxAVLCount)	Renum_RoutingState=ROUTE_USEALLASTARBUFFER;//區域規劃資料整理失敗
			else										Renum_RoutingState=ROUTE_NOFOUNDROADTOENDPOINT;//區域規劃資料整理失敗
			return nuFALSE;
		#endif
		}


		if(CLRS_T.PtLocalRoute.FixNode1==CLRS_T.PtLocalRoute.FixNode2){	//到達目標Node點
			if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID && 
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.NRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode1)){//終點碰到就結束	
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}
			else if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID && 
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.FMapLayer == _MAPLAYER_THC && 
				p_SAVLTree->Data.NMapLayer == _MAPLAYER_TTHR && 
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode1))
			{//終點碰到就結束	
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}
		}
		else{
			//比對倒數兩點 兩點要為鄰近點才行
			//b_ReRouting==nuFALSE && 
			if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID &&
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.NRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode1 && 
				p_SAVLTree->Data.NNodeID==CLRS_T.PtLocalRoute.FixNode2))
			{
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}
			else if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID && 
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.NRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode2 && 
				p_SAVLTree->Data.NNodeID==CLRS_T.PtLocalRoute.FixNode1))
			{
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}

			else if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID &&
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.FMapLayer == _MAPLAYER_THC  &&
				p_SAVLTree->Data.NMapLayer == _MAPLAYER_TTHR  && 
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode1 && 
				p_SAVLTree->Data.NNodeID==CLRS_T.PtLocalRoute.FixNode2))
			{
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}
			
			else if((p_SAVLTree->Data.TMapID==CLRS_T.PtLocalRoute.MapID && 
				p_SAVLTree->Data.TRtBID==CLRS_T.PtLocalRoute.RTBID && 
				p_SAVLTree->Data.FMapLayer == _MAPLAYER_THC  &&
				p_SAVLTree->Data.NMapLayer == _MAPLAYER_TTHR  &&  
				p_SAVLTree->Data.TNodeID==CLRS_T.PtLocalRoute.FixNode2 && 
				p_SAVLTree->Data.NNodeID==CLRS_T.PtLocalRoute.FixNode1))
			{
				nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
				m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
				m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
				ThrGDis=BestChoose.TotalDis;
				ThrRDis=BestChoose.RealDis;
				TargetClass=p_SAVLTree->Data.Class;
				return nuTRUE;
			}
		}
		#ifdef _DAR
			if(m_bARNearbyAR && m_iARCount != 0)
			{// Connect near RT
				for(nuLONG ar = 0; ar < m_iARCount; ar++)
				{
					Astar_AR(b_CloseThr,TargetCoor,EndBoundaryRange,LimitNodeID_7,m_sARData[ar],m_AStar,CLRS_F,CLRS_T,BestChoose);
				}
				m_iARCount	   = 0;
			}
		#endif
		switch(p_SAVLTree->Data.TMapLayer)
		{
			#ifndef DEF_MAKETHRROUTE
				case _MAPLAYER_SRT:
					if(b_CloseStart) break;
			#ifdef _DAR
					if(!m_bARNearbyAR && l_RoutingRule != 4)//非相鄰非最短路徑
			#else
					if(l_RoutingRule != 4)
			#endif
					{
						if(p_SAVLTree->Data.Class > _IMPORTANTRDLEVEL && m_lMaxRoadClassCount > m_nRTSTARTSAMECLASSROAD)
						{
							continue;//當9號路網擴散到了一定的數量，即停止低等級路網的擴散
						}
						if(p_SAVLTree->Data.Class < _IMPORTANTRDLEVEL)
						{
							m_lMaxRoadClassCount++;
						}
					}
					else
					{
						if(l_RoutingRule == 3)
						{
							if(SRTTypeCount[p_SAVLTree->Data.Class] > 15000)
							{
								continue;//起點同等級道路且為少走高速超過15000條就不需再繼續擴散了
							}
						}
						else
						{
							if(SRTTypeCount[p_SAVLTree->Data.Class] > 6000)
							{
								continue;//起點同等級道路超過6000條就不需再繼續擴散了
							}
						}
					}
					SRTTypeCount[p_SAVLTree->Data.Class]++;
					
					nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
					Astar_SRT(b_CloseThr,TargetCoor,EndBoundaryRange,LimitNodeID_7,tmp_SAVLData,m_AStar,CLRS_F,CLRS_T,b_StartLevelCut,BestChoose,l_RoutingRule);		
					break;
				case _MAPLAYER_STHR:
					if(b_CloseThr) break;
					nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
					Astar_STHR(b_CloseThr,TargetCoor,EndBoundaryRange,LimitNodeID_7,tmp_SAVLData,m_AStar,CLRS_F,CLRS_T,BestChoose);
					break;
				case _MAPLAYER_THC:
					if(b_CloseThr) break;
					nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
					Astar_THC(b_CloseThr,TargetCoor,EndBoundaryRange,LimitNodeID_7,tmp_SAVLData,m_AStar,CLRS_F,CLRS_T,BestChoose);	
					break;
				case _MAPLAYER_TTHR:
					if(b_CloseThr) break;
					nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
					Astar_TTHR(b_CloseThr,TargetCoor,EndBoundaryRange,LimitNodeID_7,tmp_SAVLData,m_AStar,CLRS_F,CLRS_T,BestChoose);	
					break;
			#endif
			case _MAPLAYER_TRT:
				if(p_SAVLTree->Data.Class < _CONNECTLEVELCLASSLIMIT)
				{
					Run8E_Dis += p_SAVLTree->Data.NowDis;
				}
				nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
				Astar_TRT(b_CloseThr,TargetCoor,Run8E_Dis,EndBoundaryRange,LimitNodeID_7,tmp_SAVLData,m_AStar,CLRS_F,CLRS_T,b_StartLevelCut,b_CloseStart,BestChoose,l_RoutingRule);	
				break;
		}
		CarAngle=-1;
	}
	return nuFALSE;
}

nuVOID  Class_CalRouting::Astar_SRT(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL &b_StartLevelCut,WEIGHTINGSTRU BestChoose,nuLONG l_RoutingRule)
{
#ifndef VALUE_EMGRT
	nuDWORD j = 0;
#endif
	nuLONG DifAng = 0,tmp_GuessDis = 0;
	nuDWORD i,NowConnectIdx = 0;
	AStarData m_AStarData;
	NUROPOINT Temp_FDifCoor;
	WEIGHTINGSTRU NewWeighting;

	//填充起訖點資訊
	m_AStarData.FMapID=m_SAVLData.NMapID;
	m_AStarData.FRtBID=m_SAVLData.NRtBID;
	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
	m_AStarData.NMapID=m_SAVLData.TMapID;
	m_AStarData.NRtBID=m_SAVLData.TRtBID;
	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
	m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
	m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
#ifdef NT_TO_TT
	m_AStarData.FFMapID=m_SAVLData.FMapID;
	m_AStarData.FFRtBID=m_SAVLData.FRtBID;
	m_AStarData.FFNodeID=m_SAVLData.FNodeID;
	m_AStarData.FFMapLayer = m_SAVLData.FMapLayer;
	m_AStarData.FTNoUseFlag = 0;
#endif
	for(i=0;i<(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].ConnectCount;i++)
	{
		NowConnectIdx=(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].ConnectStruIndex+i-1;
		if(NowConnectIdx>=g_pRtB_F->RtBHeader.RtConnectStruCount)
		{
			break;//排除異常資料
		}
		if((*g_pRtB_F->RtBConnect)[NowConnectIdx].TNodeID==_DEFNODEID)//14為計畫道路
		{
			continue;
		}
/*
		if(b_PassTollgate && (*g_pRtB_F->RtBConnect)[NowConnectIdx].VoiceRoadType==VRT_Tollgate)
		{
			continue;//日後若有收費機制的選項 就用這個地方來處理
		}
*/
		m_AStarData.TMapID=g_pRtB_F->MyState.MapID;

		if((*g_pRtB_F->RtBConnect)[NowConnectIdx].TRtBID == _THRRTBID)//遇到要跨區
		{
			m_AStarData.TRtBID=m_AStarData.NRtBID;
			m_AStarData.TMapLayer = _MAPLAYER_STHR;//第一層
		}
#ifdef _DAR
		else if((*g_pRtB_F->RtBConnect)[NowConnectIdx].TRtBID==4094 || 
			(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].ARFlag == 1)
		{
			m_AStarData.TMapLayer = _MAPLAYER_SRT;
			m_AStarData.TRtBID = m_AStarData.NRtBID;
		#ifdef _DAR
			if(!b_LocalRut && m_bARNearbyAR)
			{
				m_bARFindnode = nuTRUE;
			}	
		#endif
		}
#endif
		else
		{
			m_AStarData.TRtBID=(*g_pRtB_F->RtBConnect)[NowConnectIdx].TRtBID;
			m_AStarData.TMapLayer = _MAPLAYER_SRT;//第零層
		}
		m_AStarData.TNodeID=(*g_pRtB_F->RtBConnect)[NowConnectIdx].TNodeID;//取得道路編號 
		if(m_AStarData.NNodeID == m_AStarData.TNodeID)
		{
			continue;
		}

		m_AStarData.Class=(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass;//取得道路等級
		if(m_AStarData.Class == 14)
		{
			continue;
		}
		if(m_AStarData.Class == _WALKINGRDLEVEL && m_SAVLData.WalkingFlag == 1)//人行道
		{
			m_AStarData.WalkingFlag=1;
		}
		else
		{
			m_AStarData.WalkingFlag=0;
		}

		if(m_AStarData.TMapLayer == _MAPLAYER_SRT)//規劃條件的大本營
		{
			if(l_RoutingRule==6 && m_AStarData.Class<5)//為機踏車模式(模式為6)且為高速要避開
			{
				continue;
			}
		}

		if((*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NoTurnItem==0)//如果禁彎Flag為0
		{
			m_AStarData.NNoTurnFlag=0;//AStar比對資料 特別處理   
		}
		else
		{
			m_AStarData.NNoTurnFlag=1;//AStar比對資料 特別處理	
		}
		
		m_AStarData.TNoTurnFlag=0;
		if(	m_AStarData.NNoTurnFlag!=0					&&
			m_AStarData.NMapLayer == _MAPLAYER_SRT		&&
			m_AStarData.TMapLayer == _MAPLAYER_SRT		&&
			!PassNoTurn(nuFALSE,m_AStarData,m_AStar,m_SAVLData)	)
		{
	#ifdef NT_TO_TT
			m_SAVLData.FTNoUseFlag = 1;
	#endif
			continue;//禁轉判斷
		}
		
//*********************即時路況判斷***********************************************//
//--------------------------------------------------------------------------------//
#ifdef _DST
		if(m_pclsDst != NULL && !Dstroadsituation(nuFALSE,m_AStarData,m_AStar)	)
		{
			continue;
		}
#endif
#ifdef _DTMC
		if(m_pclsTmc != NULL && !Tmcroadsituation(nuFALSE,m_AStarData,m_AStar)	)
		{
			continue;
		}
#endif
//--------------------------------------------------------------------------------//
//********************************************************************************//

		if(m_AStarData.TMapLayer == _MAPLAYER_SRT)
		{
			if(m_AStarData.NNodeID==m_AStarData.TNodeID)//路的頂點
			{
				continue;
			}

			if((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NoTurnItem==0)//判斷會不會遇到禁轉
			{
				m_AStarData.TNoTurnFlag=0;//AStar比對資料 特別處理
			}
			else
			{
				m_AStarData.TNoTurnFlag=1;//AStar比對資料 特別處理
			}
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
				for(j=0;j<(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVX;j++){
					Temp_FDifCoor.x+=_COORLEVELNUM;
				}
				for(j=0;j<(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].CoorLVY;j++){
					Temp_FDifCoor.y+=_COORLEVELNUM;
				}
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
		}
		else if(m_AStarData.TMapLayer == _MAPLAYER_STHR)
		{
			if(b_LocalRut)
			{
				continue;
			}
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
		}
		else
		{
			continue;
		}

		NewWeighting.RealDis=BestChoose.RealDis+(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadLength;
//*******//2009/12/2   Daniel修改成如果前方的路比上交流道短就壓低交流道的權值往前直行 ****************************************//
		/*if((*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass == 5 && m_SAVLData.Class > 5)// && 
			//(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadLength <= 600)
		{
			if(m_SAVLData.Class + 1 == 9)
			{
				if(!GetWeighting(m_SAVLData.Class + 2)) continue;
			}
			else
			{
				if(!GetWeighting(m_SAVLData.Class + 1)) continue;
			}
		}*/
#ifndef VALUE_EMGRT
		/*else if(m_SAVLData.Class == 9 && 
			(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass < 8 &&
			(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass >= 4)
		{
		   nuLONG Ang1,Ang2;
		   Ang1=nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.FNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.FNodeID].NodeCoor.x);
		   Ang2=nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
		   DifAng=(Ang1-Ang2+360)%360;
		   if(75<DifAng && DifAng<105)
		   {
				if(!GetWeighting(0)) continue;
		   }
		   else if(255<DifAng && DifAng<285)
		   {
				if(!GetWeighting(0)) continue;
		   }
		   else
		   {
				if(!GetWeighting(m_AStarData.Class)) continue;
		   }
		}*/
#endif
	//else
		{
			if(!GetWeighting(m_AStarData.Class)) continue;
		}
//****************************************************************************************************************************//
		//m_bTMCLocalRoute =false;
		if(m_bTMCLocalRoute)
		{
			nuULONG nStartNodeID = (m_AStarData.NRtBID << 20) + m_AStarData.NNodeID;
			nuULONG nEndNodeID = (m_AStarData.TRtBID << 20) + m_AStarData.TNodeID;
			nuUINT nWeighting = g_pRtFSApi->FS_GetTMCTrafficInfoByNodeID(CLRS_F.MapIdx,nStartNodeID,nEndNodeID);
			switch(nWeighting)
			{
				case 0://不確定
				case 1://正常
					break;
				case 2:
					ClassWeighting = m_nTMCSlowWeighting; // 緩慢 可通行也盡量不走(除非附近只有此路可通行)
					break;
				case 3:
					ClassWeighting = m_nTMCCrowdWeighting;// 壅塞 可通行但盡量不走(除非附近只有此路可通行)
					break;
				default:
					break;
			}
		}
		m_AStarData.NowDis = ((*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadLength * ClassWeighting) / _WEIGHTINGBASE;
		#ifndef DEF_MAKETHRROUTE
			if(m_AStarData.TMapLayer == _MAPLAYER_SRT)
			{
				if( g_pCDTH->CheckCongestionID(m_AStarData.TNodeID) )
				{
					m_AStarData.NowDis += ((*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadLength*5 * ClassWeighting) / _WEIGHTINGBASE;
				}
			}
		#endif
		m_AStarData.NowDis += 3;
		if(FOLLOWWEIGHTING>0)
		{
			DifAng=nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.FNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.FNodeID].NodeCoor.x)-nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
			if(DifAng>180) DifAng=DifAng-360;
			if(DifAng<-180) DifAng=DifAng+360;
			if(NURO_ABS(DifAng)>60) m_AStarData.NowDis += FOLLOWWEIGHTING;
		}
		//初路段 鼓勵直行機制
		#ifndef DEF_MAKETHRROUTE
			if(_FIRSTROADRETURNDIS > NewWeighting.RealDis)
			{
				m_AStarData.NNoTurnFlag=1;
			}
			if(CarAngle!=-1)
			{
				DifAng=CarAngle-nulAtan2((*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_F->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_F->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
				if(135<DifAng && DifAng<225){
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS;
				}
				if(-225<DifAng && DifAng<-135){
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS;
				}
				if(45<DifAng && DifAng<135){
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS / 2;
				}
				if(-135<DifAng && DifAng<-45){
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS / 2;
				}
			}
			else
			{
				if( m_AStarData.FMapLayer==m_AStarData.TMapLayer &&
					m_AStarData.FNodeID==m_AStarData.TNodeID &&
					m_AStarData.FRtBID==m_AStarData.TRtBID &&
					m_AStarData.FMapID==m_AStarData.TMapID
					) continue;
			}

		#else
			if( m_AStarData.FMapLayer==m_AStarData.TMapLayer &&
				m_AStarData.FNodeID==m_AStarData.TNodeID &&
				m_AStarData.FRtBID==m_AStarData.TRtBID &&
				m_AStarData.FMapID==m_AStarData.TMapID
				) continue;
		#endif
		//步行道處理規範
		/*if(TWalkingNode==m_SAVLData.TNodeID){
			if(m_AStarData.Class == _WALKINGRDLEVEL || m_AStarData.Class == _PLANRDLEVEL)
			{
				m_AStarData.WalkingFlag=1;
			}
			else
			{
				m_AStarData.WalkingFlag=0;
			}
		}
		else{
			if(m_AStarData.Class == _WALKINGRDLEVEL || m_AStarData.Class == _PLANRDLEVEL)
			{
				if( m_SAVLData.WalkingFlag==1 ){
					m_AStarData.WalkingFlag=1;
				}
				else
				{
					continue;
				}
			}
			else{
				m_AStarData.WalkingFlag=0;
			}
		}	*/	

		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis;
		if(m_AStarData.TMapLayer == _MAPLAYER_STHR)	m_AStarData.RDis++;
		if(m_AStarData.TMapLayer == _MAPLAYER_TRT)		m_AStarData.RDis++;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;//將計算過的資料填入open端
	#ifdef _DAR
		if(m_bARFindnode)
		{	
			nuMemcpy(&m_sARData[m_iARCount],&m_AStarData,sizeof(AStarData));
			m_iARCount++;
			m_bARFindnode = nuFALSE;
		}
	#endif
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
	b_FirstRdEffect=nuTRUE;//控制車頭燈
}
nuVOID  Class_CalRouting::Astar_TRT(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuDWORD Run8E_Dis,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL &b_StartLevelCut,nuBOOL &b_CloseStart,WEIGHTINGSTRU BestChoose,nuLONG l_RoutingRule)
{
	#ifndef VALUE_EMGRT
		nuDWORD j = 0;
	#endif
	nuLONG tmp_GuessDis,DifAng = 0;
	nuDWORD i = 0,NowConnectIdx = 0;
	AStarData m_AStarData;
	NUROPOINT Temp_FDifCoor;
	WEIGHTINGSTRU NewWeighting;

	//填充起訖點資訊
	m_AStarData.FMapID=m_SAVLData.NMapID;
	m_AStarData.FRtBID=m_SAVLData.NRtBID;
	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
	m_AStarData.NMapID=m_SAVLData.TMapID;
	m_AStarData.NRtBID=m_SAVLData.TRtBID;
	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
	m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
	m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
#ifdef NT_TO_TT
	m_AStarData.FFMapID=m_SAVLData.FMapID;
	m_AStarData.FFRtBID=m_SAVLData.FRtBID;
	m_AStarData.FFNodeID=m_SAVLData.FNodeID;
	m_AStarData.FFMapLayer = m_SAVLData.FMapLayer;
	m_AStarData.FTNoUseFlag = 0;
#endif
	for(i=0;i<(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].ConnectCount;i++)
	{
		NowConnectIdx=(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].ConnectStruIndex+i-1;

		if(NowConnectIdx>=g_pRtB_T->RtBHeader.RtConnectStruCount) break;
		if((*g_pRtB_T->RtBConnect)[NowConnectIdx].TNodeID==_DEFNODEID)//14為計畫道路 
		{	
			continue;
		}
//		if(b_PassTollgate && (*g_pRtB_T->RtBConnect)[NowConnectIdx].VoiceRoadType==VRT_Tollgate)	continue;
		//設置連通資料

		m_AStarData.TMapID=g_pRtB_T->MyState.MapID;
		if((*g_pRtB_T->RtBConnect)[NowConnectIdx].TRtBID == _THRRTBID)
		{
			#ifdef DEF_MAKETHRROUTE
				continue;//終點區域已經往下走了 就不可以再回到thr層(避免不必要的逆走)
			#else
				m_AStarData.TRtBID=m_AStarData.NRtBID;
				m_AStarData.TMapLayer=_MAPLAYER_TTHR;
			#endif
		}
#ifdef _DAR	
		else if((*g_pRtB_T->RtBConnect)[NowConnectIdx].TRtBID == 4094 || 
			(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].ARFlag == 1)
		{
			m_AStarData.TRtBID=m_AStarData.NRtBID;
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
		}
#endif
		else
		{
			m_AStarData.TRtBID=(*g_pRtB_T->RtBConnect)[NowConnectIdx].TRtBID;
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
		}
		m_AStarData.TNodeID=(*g_pRtB_T->RtBConnect)[NowConnectIdx].TNodeID;
		if(m_AStarData.NNodeID == m_AStarData.TNodeID)
		{
			continue;
		}
		#ifdef DEF_MAKETHRROUTE
			if( m_AStarData.FMapLayer==m_AStarData.TMapLayer &&
				m_AStarData.FNodeID==m_AStarData.TNodeID &&
				m_AStarData.FRtBID==m_AStarData.TRtBID &&
				m_AStarData.FMapID==m_AStarData.TMapID
				) 
			{
				continue;
			}
		#endif

		//如果為區域規劃就不該上THR
		if(b_LocalRut && m_AStarData.TMapLayer==_MAPLAYER_TTHR){
			continue;
		}
		
		if((*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NoTurnItem==0)	m_AStarData.NNoTurnFlag=0;//AStar比對資料 特別處理
		else														m_AStarData.NNoTurnFlag=1;//AStar比對資料 特別處理
		m_AStarData.TNoTurnFlag=0;

		if(m_AStarData.NNoTurnFlag!=0 && 
			m_AStarData.NMapLayer==_MAPLAYER_TRT && 
			m_AStarData.TMapLayer==_MAPLAYER_TRT && 
			!PassNoTurn(nuTRUE,m_AStarData,m_AStar, m_SAVLData))
		{
	#ifdef NT_TO_TT
			m_SAVLData.FTNoUseFlag = 1;
	#endif
			continue;//禁轉相關處理
		}
//*******************************************即時路況相關處理***************************************************************************************************************************//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#ifdef _DDST
		if(ppclsDst != NULL && !Dstroadsituation(nuTRUE,m_AStarData,m_AStar))
		{
			continue;
		}
#endif
#ifdef _DTMC
		if(m_bTMCLocalRoute && m_pclsTmc != NULL && !Tmcroadsituation(nuFALSE,m_AStarData,m_AStar)	)
		{
			continue;
		}
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//**************************************************************************************************************************************************************************************//
		m_AStarData.Class=(*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadClass;//複製道路層級
		if(m_AStarData.Class == 14)
		{
			continue;
		}
		nuLONG lVoiceType = (*g_pRtB_T->RtBConnect)[NowConnectIdx].VoiceRoadType;
		if(l_RoutingRule==6 && lVoiceType != 0)
		{
			if(lVoiceType <= 21 || lVoiceType == 34)
			{
				continue;
			}
		}
		if(m_AStarData.TMapLayer==_MAPLAYER_TRT)
		{//如果已經來到第四層
			if((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NoTurnItem==0)
			{
				m_AStarData.TNoTurnFlag=0;//AStar比對資料 特別處理
			}
			else
			{
				m_AStarData.TNoTurnFlag=1;//AStar比對資料 特別處理
			}
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX;j++){
					Temp_FDifCoor.x+=_COORLEVELNUM;
				}
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY;j++){
					Temp_FDifCoor.y+=_COORLEVELNUM;
				}
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_T_DiffCoor);
		}
		else if(m_AStarData.TMapLayer==_MAPLAYER_TTHR)
		{
			if(b_LocalRut) continue;
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
		}
		else{
			continue;
		}
		NewWeighting.RealDis=BestChoose.RealDis+(*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadLength;

		if( m_AStarData.NMapLayer==_MAPLAYER_TRT && m_AStarData.TMapLayer==_MAPLAYER_TRT )
		{//確認已經走到了這個圖層
			if( m_AStarData.NNodeID==CLRS_T.PtLocalRoute.FixNode1 )	m_AStarData.NNoTurnFlag=1;
			if( m_AStarData.NNodeID==CLRS_T.PtLocalRoute.FixNode2 )	m_AStarData.NNoTurnFlag=1;
			if( m_AStarData.TNodeID==CLRS_T.PtLocalRoute.FixNode1 )	m_AStarData.NNoTurnFlag=1;
			if( m_AStarData.TNodeID==CLRS_T.PtLocalRoute.FixNode2 )	m_AStarData.NNoTurnFlag=1;
		}
	
		//規劃條件的大本營
		if(m_AStarData.NMapLayer==_MAPLAYER_TRT && m_AStarData.TMapLayer==_MAPLAYER_TRT)
		{
//			if(b_StartLevelCut == nuFALSE && m_AStarData.Class <= _GENERALRDLEVEL) b_StartLevelCut = nuTRUE;
			if(Run8E_Dis > _LEVELCHOOSEDIS2 && tmp_GuessDis < EndBoundaryRange)
			{//高等級道路走了一段時間 再進來判斷會比較好
//				if(!b_CloseStart)	b_CloseStart=nuTRUE;
				if(LimitNodeID_7==m_AStarData.NNodeID)
				{
					if(!b_CloseThr)	b_CloseThr=nuTRUE;
				}
			}
			else
			{
#ifdef VALUE_EMGRT
//				if( Run8E_Dis > _LEVELCHOOSEDIS1 && m_AStarData.Class > _GENERALRDLEVEL ) continue;//如果高等級路網已經使用了LevelChooseDis1 這麼長的距離了 應該可以確定已經在重要路網上了
#endif		
			}
		}		
//*********//2009/12/2   Daniel修改成如果前方的路比上交流道短就壓低交流道的權值往前直行 ***************************************//		
	/*	if((*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadClass == 5 && m_SAVLData.Class > 5)// && 
			//(*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadLength <= 600)
		{
			if(m_SAVLData.Class + 1 == 9)
			{
				if(!GetWeighting(m_SAVLData.Class + 2)) continue;
			}
			else
			{
				if(!GetWeighting(m_SAVLData.Class + 1)) continue;
			}
		}*/
#ifndef VALUE_EMGRT 
		/*else if(m_SAVLData.Class == 9 && 
			(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass < 8 &&
			(*g_pRtB_F->RtBConnect)[NowConnectIdx].RoadClass >= 4)//快車道轉慢車道右轉的機制
		{
		    nuLONG Ang1,Ang2;
		    Ang1=nulAtan2((*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.y-(*g_pRtB_T->RtBMain)[m_AStarData.FNodeID].NodeCoor.y,(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.x-(*g_pRtB_T->RtBMain)[m_AStarData.FNodeID].NodeCoor.x);
		    Ang2=nulAtan2((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
		    DifAng=(Ang1-Ang2+360)%360;
		    if(75<DifAng && DifAng<105)
			{
				if(!GetWeighting(0)) continue;
			}
		    else if(255<DifAng && DifAng<285)
			{
				if(!GetWeighting(0)) continue;
			}
			else
			{
				if(!GetWeighting(m_AStarData.Class)) continue;
			}
		 }*/
#endif
		//else
		//{
			if(!GetWeighting(m_AStarData.Class)) 
			{
				continue;
			}
		//}
//*****************************************************************************************************************************//
		//m_bTMCLocalRoute =false;
		if(m_bTMCLocalRoute)
		{
			nuULONG nStartNodeID = (m_AStarData.NRtBID << 20) + m_AStarData.NNodeID;
			nuULONG nEndNodeID = (m_AStarData.TRtBID << 20) + m_AStarData.TNodeID;
			nuUINT nWeighting = g_pRtFSApi->FS_GetTMCTrafficInfoByNodeID(CLRS_F.MapIdx,nStartNodeID,nEndNodeID);
			switch(nWeighting)
			{
				case 0://不確定
				case 1://正常
					break;
				case 2:
					ClassWeighting = m_nTMCSlowWeighting; // 緩慢 可通行也盡量不走(除非附近只有此路可通行)
					break;
				case 3:
					ClassWeighting = m_nTMCCrowdWeighting;// 壅塞 可通行但盡量不走(除非附近只有此路可通行)
					break;
				default:
					break;
			}
		}
		m_AStarData.NowDis=((*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadLength*ClassWeighting) / _WEIGHTINGBASE;
		#ifndef DEF_MAKETHRROUTE
			if(m_AStarData.TMapLayer==_MAPLAYER_TRT)
			{
				if( g_pCDTH->CheckCongestionID(m_AStarData.TNodeID) )
				{//塞車的規劃機制
					m_AStarData.NowDis += ((*g_pRtB_T->RtBConnect)[NowConnectIdx].RoadLength*5*ClassWeighting) / _WEIGHTINGBASE;
				}
			}

			if(_FIRSTROADRETURNDIS > NewWeighting.RealDis)
			{
				m_AStarData.NNoTurnFlag=1;
			}
			if(CarAngle!=-1)
			{
				DifAng=CarAngle-nulAtan2((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
				if(135<DifAng && DifAng<225)
				{
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS;
				}
				if(-225<DifAng && DifAng<-135)
				{
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS;
				}
				if(45<DifAng && DifAng<135)
				{
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS / 2;
				}
				if(-135<DifAng && DifAng<-45)
				{
					m_AStarData.NowDis += _CARDIRECTIONEFFECTDIS / 2;
				}
			}
			else
			{
				if( m_AStarData.FMapLayer==m_AStarData.TMapLayer &&//如果起訖點為同一點
					m_AStarData.FNodeID==m_AStarData.TNodeID &&
					m_AStarData.FRtBID==m_AStarData.TRtBID &&
					m_AStarData.FMapID==m_AStarData.TMapID && 
					tmp_GuessDis > 50)
				{
					continue;
				}
			}
		#endif
		m_AStarData.NowDis += 3;

		if(FOLLOWWEIGHTING>0 && m_AStarData.Class > 5)
		{
			DifAng=nulAtan2((*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.y-(*g_pRtB_T->RtBMain)[m_AStarData.FNodeID].NodeCoor.y,
				(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.x-(*g_pRtB_T->RtBMain)[m_AStarData.FNodeID].NodeCoor.x) -
				nulAtan2((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.y,
				(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x-(*g_pRtB_T->RtBMain)[m_AStarData.NNodeID].NodeCoor.x);
			if(DifAng>180) DifAng=DifAng-360;
			if(DifAng<-180) DifAng=DifAng+360;
			if(NURO_ABS(DifAng)>60) m_AStarData.NowDis += FOLLOWWEIGHTING;
		}
		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis;
		if(m_AStarData.TMapLayer==_MAPLAYER_TTHR)	m_AStarData.RDis++;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;

		//步行道處理規範
		/*if(TWalkingNode==m_SAVLData.TNodeID){
			if(m_AStarData.Class == _WALKINGRDLEVEL  || m_AStarData.Class == _PLANRDLEVEL)
			{
				m_AStarData.WalkingFlag=1;
			}
			else
			{
				m_AStarData.WalkingFlag=0;
			}
		}
		else{
			if(m_AStarData.Class == _WALKINGRDLEVEL  || m_AStarData.Class == _PLANRDLEVEL)
			{
				if( m_SAVLData.WalkingFlag==1 )
				{
					m_AStarData.WalkingFlag=1;
				}
				else
				{
					continue;
				}
			}
			else{
				m_AStarData.WalkingFlag=0;
			}
		}*/
#ifdef _DROUTE_ERROR
	if(!b_LocalRut)
	{
		if(m_lShortestDis == 0)
		{
			m_lShortestDis = tmp_GuessDis;
		}
		else 
		{
			if(m_lShortestDis > tmp_GuessDis)
			{
				m_lShortestDis	    = tmp_GuessDis;
				m_ShortestAStarData = m_AStarData;
				m_ShortestChoose    = NewWeighting;
			}
		}
	}
#endif
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
	b_FirstRdEffect=nuTRUE;//控制車頭燈
}
nuVOID  Class_CalRouting::Astar_STHR(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose)
{
	nuLONG tmp_GuessDis = 0;
	nuBOOL b_allpass;
	nuDWORD i = 0,j = 0,k = 0,tmpNodeIDIdx = 0;
	AStarData m_AStarData;
	NUROPOINT Temp_FDifCoor;
	WEIGHTINGSTRU NewWeighting;

	m_AStarData.FMapID=m_SAVLData.NMapID;
	m_AStarData.FRtBID=m_SAVLData.NRtBID;
	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
	m_AStarData.NMapID=m_SAVLData.TMapID;
	m_AStarData.NRtBID=m_SAVLData.TRtBID;
	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
	m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
	m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
	m_AStarData.TNoTurnFlag=0;

	for(i=0;i<(*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.NNodeID].ConnectCount;i++)
	{
		tmpNodeIDIdx=(*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.NNodeID].ConnectStruIndex+i;
		if((*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID==_DEFNODEID) continue;
		//如果Thr的起點NodeID=最大值則跳過
		if(tmpNodeIDIdx>=m_pThr_F->pThrEachMap.ThrHeader.RtConnectStruCount)
		{
			break;		
		}
		m_AStarData.TMapID=m_pThr_F->ThrHeader.MapID;

		switch((*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TMapLayer){
			case 1:
				if(m_AStarData.TMapID==g_pRtB_T->MyState.MapID && (*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID==g_pRtB_T->MyState.RtBlockID)
				{
					m_AStarData.TRtBID=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID;
					m_AStarData.TMapLayer=_MAPLAYER_TRT;	
					break;
				}
				continue;
			case 2:
				m_AStarData.TRtBID=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID;
				m_AStarData.TMapLayer = _MAPLAYER_STHR;
				break;
			case 3:
				m_AStarData.TRtBID	  = m_AStarData.NRtBID;
				m_AStarData.TMapLayer = _MAPLAYER_THC;
			#ifdef THR_TO_THC
				for(int error = 0; error < 3; error++)
				{
					for(int k = 0; k < m_CMP.CMPHeader.DataCnt; k++)
					{
						m_Dif_X = NURO_ABS((*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor.x - m_CMP.pCMPMainData[k].x);
						m_Dif_Y = NURO_ABS((*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor.y - m_CMP.pCMPMainData[k].y);
						if(m_Dif_X <= error && m_Dif_Y <= error)
						{
							error = 3;
							(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID = m_CMP.pCMPMainData[k].thcid;
							break;
						}
					}
				}
			#endif
				break;
			default:	
				continue;
		}
		m_AStarData.TNodeID=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID;
		if(m_AStarData.FMapLayer==m_AStarData.TMapLayer && m_AStarData.FMapID==m_AStarData.TMapID && m_AStarData.FRtBID==m_AStarData.TRtBID && m_AStarData.FNodeID==m_AStarData.TNodeID) continue;
		m_AStarData.WalkingFlag=0;
		
		if(m_AStarData.TMapLayer==_MAPLAYER_TRT){
			//阻擋rt通到thr之後還是走同一區
			//if(m_AStarData.NRtBID==m_AStarData.TRtBID) continue;
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX;j++){
					Temp_FDifCoor.x=Temp_FDifCoor.x+_COORLEVELNUM;
				}
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY;j++){
					Temp_FDifCoor.y=Temp_FDifCoor.y+_COORLEVELNUM;
				}
			#endif		
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_T_DiffCoor);
		}
		else if(!b_LocalRut && m_AStarData.TMapLayer == _MAPLAYER_STHR){
			//加強thr的效率 不馬上回頭 雖然會降低一些可能性 但是 同一地區馬上進出 是很奇怪的
			//if(m_AStarData.NMapLayer==_MAPLAYER_STHR && m_AStarData.TRtBID==m_AStarData.NRtBID) continue;
			//阻擋rt通到thr之後還是走同一區
			//if(m_AStarData.FMapLayer== _MAPLAYER_SRT && m_AStarData.NMapLayer==_MAPLAYER_STHR && m_AStarData.FRtBID==m_AStarData.TRtBID) continue;
			if(m_AStarData.NNodeID==m_AStarData.TNodeID) continue;
			if((*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].Length<=1) continue;//如果原始距離小於1

			b_allpass=nuFALSE;//<-從這裡開始到下面結束，這一段在判斷需要排除的資料
			if((*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].Class==1){//如果等級為1則進行處理
				for(j=0;j<m_pThr_F->ThrHeader.RTBCount;j++){
					if( (*m_pThr_F->RTBConnect)[j].Connheader.RTBID==m_AStarData.TRtBID ){//如果找到同一個區塊
						for(k=0;k<(*m_pThr_F->RTBConnect)[j].Connheader.RTBConnCount;k++){
							if( (*(*m_pThr_F->RTBConnect)[j].ConnnectID)[k]==g_pRtB_T->MyState.RtBlockID ){
								b_allpass=nuTRUE;//如果與此區塊的連接ID相同則b_allpass=nuTRUE
								break;
							}
						}//如果已經找到與現在區塊相連接的區塊就跳出
						if(b_allpass) break;
					}
				}//所有迴圈結束後將判斷是否要將此資料放入AVL樹中
				if( !b_allpass )	continue;
			}
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
		}
		else if(!b_LocalRut && m_AStarData.TMapLayer == _MAPLAYER_THC)
		{
			if(!m_pclsThc->b_ThcReady) continue;
			//因為thc本身沒有座標 所以需要以其他方式來取得座標
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_F->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor,TargetCoor);
		}
		else
		{	continue;	}

		m_AStarData.NNoTurnFlag=0;//AStar比對資料 特別處理
		m_AStarData.Class=0;
		NewWeighting.RealDis=BestChoose.RealDis+(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].Length;
		m_AStarData.NowDis=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].WeightingLength;
		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis+1;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
}
nuVOID  Class_CalRouting::Astar_TTHR(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose)
{
	nuLONG tmp_GuessDis = 0;
	nuBOOL b_allpass;
	nuDWORD i = 0,j = 0,k = 0,tmpNodeIDIdx = 0;
	AStarData m_AStarData;
	NUROPOINT Temp_FDifCoor;
	WEIGHTINGSTRU NewWeighting;

	if( m_SAVLData.TMapID != m_pThr_T->ThrHeader.MapID) return;

	m_AStarData.FMapID=m_SAVLData.NMapID;	m_AStarData.FRtBID=m_SAVLData.NRtBID;	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
	m_AStarData.NMapID=m_SAVLData.TMapID;	m_AStarData.NRtBID=m_SAVLData.TRtBID;	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
	m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
	m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
	m_AStarData.TNoTurnFlag=0;

	for(i=0;i<(*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.NNodeID].ConnectCount;i++)
	{
		tmpNodeIDIdx=(*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.NNodeID].ConnectStruIndex+i;

		if(tmpNodeIDIdx>=m_pThr_T->pThrEachMap.ThrHeader.RtConnectStruCount) break;		
		if((*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID==_DEFNODEID) continue;
		m_AStarData.TMapID=m_pThr_T->ThrHeader.MapID;
		switch((*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TMapLayer)
		{
			case 1://STHRmap
				if(m_AStarData.TMapID==g_pRtB_T->MyState.MapID && (*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID==g_pRtB_T->MyState.RtBlockID)
				{
					m_AStarData.TRtBID=(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID;
					m_AStarData.TMapLayer=_MAPLAYER_TRT;
					break;
				}
				continue;
			case 2://TTHRmap
				m_AStarData.TRtBID=(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID;
				m_AStarData.TMapLayer=_MAPLAYER_TTHR;
				break;
			case 3://THCmap
				m_AStarData.TRtBID=m_AStarData.NRtBID;
				m_AStarData.TMapLayer = _MAPLAYER_THC;
			#ifdef THR_TO_THC
				for(int error = 0; error < 3; error++)
				{
					for(int k = 0; k < m_CMP.CMPHeader.DataCnt; k++)
					{
						m_Dif_X = NURO_ABS((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor.x - m_CMP.pCMPMainData[k].x);
						m_Dif_Y = NURO_ABS((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor.y - m_CMP.pCMPMainData[k].y);
						if(m_Dif_X <= error && m_Dif_Y <= error)
						{
							error = 3;
							(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID = m_CMP.pCMPMainData[k].thcid;
							break;
						}
					}
				}
			#endif
				break;
			default:
				continue;
		}

		m_AStarData.TNodeID=(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID;
		if(	m_AStarData.FMapLayer==m_AStarData.TMapLayer	&&
			m_AStarData.FMapID==m_AStarData.TMapID			&&
			m_AStarData.FRtBID==m_AStarData.TRtBID			&&
			m_AStarData.FNodeID==m_AStarData.TNodeID		)
		{
			continue;
		}
		m_AStarData.WalkingFlag=0;

		if(m_AStarData.TMapLayer==_MAPLAYER_TRT)
		{
		//	if(m_AStarData.FMapLayer!=MAPLAYER_THC && m_AStarData.NRtBID==m_AStarData.TRtBID) continue;
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX;j++){
					Temp_FDifCoor.x+=_COORLEVELNUM;
				}
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY;j++){
					Temp_FDifCoor.y+=_COORLEVELNUM;
				}			
			#endif		
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_T_DiffCoor);
		}
		else if(m_AStarData.TMapLayer==_MAPLAYER_TTHR)
		{
			//加強thr的效率 不馬上回頭 雖然會降低一些可能性 但是 同一地區馬上進出 是很奇怪的
			//if(m_AStarData.TMapID==m_AStarData.NMapID && m_AStarData.TRtBID==m_AStarData.NRtBID && m_AStarData.NMapLayer==m_AStarData.TMapLayer && m_AStarData.FMapLayer==m_AStarData.TMapLayer) continue;
			//阻擋rt通到thr之後還是走同一區
			//if(m_AStarData.FMapLayer==MAPLAYER_TRT && m_AStarData.FMapID==m_AStarData.TMapID && m_AStarData.FRtBID==m_AStarData.TRtBID) continue;
			if(m_AStarData.NNodeID==m_AStarData.TNodeID) continue;
			b_allpass=nuFALSE;
			if((*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].Class==1){
				for(j=0;j<m_pThr_T->ThrHeader.RTBCount;j++){
					if( (*m_pThr_T->RTBConnect)[j].Connheader.RTBID==m_AStarData.TRtBID ){
						for(k=0;k<(*m_pThr_T->RTBConnect)[j].Connheader.RTBConnCount;k++){
							if( (*(*m_pThr_T->RTBConnect)[j].ConnnectID)[k]==g_pRtB_T->MyState.RtBlockID ){
								b_allpass=nuTRUE;
								break;
							}
						}
						if(b_allpass) break;
					}
				}
				if( !b_allpass )	continue;
			}
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
		}
		else if(!b_LocalRut && m_AStarData.TMapLayer == _MAPLAYER_THC)
		{
			if(!m_pclsThc->b_ThcReady) continue;
			if(m_AStarData.TMapLayer==m_AStarData.FMapLayer) continue;//下來又馬上上去 這等於是找跳板而已
			//因為thc本身沒有座標 所以需要以其他方式來取得座標
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.NNodeID].NodeCoor,TargetCoor);
		}
		else
		{	continue;	}
		m_AStarData.NNoTurnFlag=0;//AStar比對資料 特別處理(處理方式與STHR相似)
		m_AStarData.Class=0;
		NewWeighting.RealDis=BestChoose.RealDis+(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].Length;
		m_AStarData.NowDis=(*m_pThr_T->pThrEachMap.ThrConnect)[tmpNodeIDIdx].WeightingLength;
		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis+1;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
}
nuVOID  Class_CalRouting::Astar_THC(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose)
{
	nuBOOL b_findit;
	nuLONG tmp_GuessDis = 0;
	nuDWORD i=0,j=0,lThcConnectCount = 0;
	nuDWORD tempcount = 1;
	AStarData m_AStarData;
	WEIGHTINGSTRU NewWeighting;

	m_AStarData.FMapID=m_SAVLData.NMapID;
	m_AStarData.FRtBID=m_SAVLData.NRtBID;
	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
	m_AStarData.NMapID=m_SAVLData.TMapID;
	m_AStarData.NRtBID=m_SAVLData.TRtBID;
	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
	m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
	m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
	m_AStarData.TNoTurnFlag=0;
	
	if(m_AStarData.NNodeID==m_pclsThc->TotalFNodeCount-1)
	{
		lThcConnectCount=m_pclsThc->TotalConnectCount-(*m_pclsThc->p_StruFNode)[m_AStarData.NNodeID].ConnectStruIndex;
	}
	else
	{
		lThcConnectCount=(*m_pclsThc->p_StruFNode)[m_AStarData.NNodeID+1].ConnectStruIndex-(*m_pclsThc->p_StruFNode)[m_AStarData.NNodeID].ConnectStruIndex;
	}
	if(!m_pclsThc->ReadConnect((*m_pclsThc->p_StruFNode)[m_AStarData.NNodeID].ConnectStruIndex,lThcConnectCount)){
		return;
	}
	
	for(i=0;i<lThcConnectCount;i++)
	{
		if((*m_pclsThc->p_StruConnect)[i].TNodeID==_DEFNODEID) continue;

		#ifdef	DEFNEWTHCFORMAT
			if(	(*m_pclsThc->p_StruConnect)[i].TGMapID1!=g_pRtB_T->MyState.MapID )
			{
				if((*m_pclsThc->p_StruConnect)[i].TGMapID2!=g_pRtB_T->MyState.MapID || (*m_pclsThc->p_StruConnect)[i].TGMapID2==1023)
				{
					continue;
				}
				else
				{
					m_AStarData.TMapID=(*m_pclsThc->p_StruConnect)[i].TGMapID2;
				}
			}
			else
			{
				m_AStarData.TMapID=(*m_pclsThc->p_StruConnect)[i].TGMapID1;
				/*//以下是要讀取跨越兩省的資料取得RTBID，此為處理起訖點在同一個地圖裡的資訊
				//舉例：從江蘇省南京市六合區的寧達高速公路跨越安徽省滁州市天長市回到江蘇省江蘇省淮安市的寧達高速公路
				if(!(*ppclsThc)->NewReadViaNodeData((*(*ppclsThc)->p_StruConnect)[i].lViaCityIndex,2)){
					continue;//tempcount只是拿來充數的資料，基本上並沒有要用到
				}
				m_AStarData.TRtBID=(*(*ppclsThc)->ppstuViaNodeData)[1].RTBID;//TRtBID只是暫時存放THC起點的RTBID	
				g_pRtMMApi->MM_FreeMem((nuPVOID*)(*ppclsThc)->ppstuViaNodeData);//釋放THC B3資料的記憶體
				(*ppclsThc)->ppstuViaNodeData = NULL;*/
			}
		#else
			if((*(*ppclsThc)->p_StruConnect)[i].TGMapID!=g_pRtB_T->MyState.MapID)	continue;//只有終點區域能拿來使用 其他區域 一律pass
			m_AStarData.TMapID=(*m_pclsThc->p_StruConnect)[i].TGMapID;
		#endif
			
		m_AStarData.TNodeID=(*m_pclsThc->p_StruConnect)[i].TNodeID;
		m_AStarData.WalkingFlag=0;
		
		switch((*m_pclsThc->p_StruConnect)[i].TMapLayer){
			case 2:	m_AStarData.TMapLayer = _MAPLAYER_TTHR;
			#ifdef THC_TO_THR
				if((m_pThr_T->ThrHeader.MapID == (*m_pclsThc->p_StruConnect)[i].TGMapID1 && (*m_pclsThc->p_StruConnect)[i].TGMapID2 != 1023) ||
					(m_pThr_T->ThrHeader.MapID == (*m_pclsThc->p_StruConnect)[i].TGMapID2 && (*m_pclsThc->p_StruConnect)[i].TGMapID1 != 1023))
				{
					for(int error = 0; error < 3; error++)
					{
						for(int j=0;j<m_pThr_T->pThrEachMap.ThrHeader.TotalNodeCount;j++)
						{
							m_Dif_X = NURO_ABS((*m_pThr_T->pThrEachMap.ThrMain)[j].NodeCoor.x - m_CMP.pCMPMainData[m_AStarData.NNodeID].x);
							m_Dif_Y = NURO_ABS((*m_pThr_T->pThrEachMap.ThrMain)[j].NodeCoor.y - m_CMP.pCMPMainData[m_AStarData.NNodeID].y);
							if(m_Dif_X <= error && m_Dif_Y <= error)
							{
								error = 3;
								(*m_pclsThc->p_StruConnect)[i].TNodeID = (*m_pThr_T->pThrEachMap.ThrMain)[j].FNodeID;
								(*m_pclsThc->p_StruConnect)[i].WeightingLength = 1;
								(*m_pclsThc->p_StruConnect)[i].Length = 1;
								m_AStarData.TNodeID = (*m_pclsThc->p_StruConnect)[i].TNodeID;
								break;
							}
						}
					}
				}
			#endif
				break;
			case 3:	m_AStarData.TMapLayer = _MAPLAYER_THC;		break;
			default:										continue;
		}

		if((*m_pclsThc->p_StruConnect)[i].WeightingLength==0 && m_AStarData.TMapLayer == _MAPLAYER_THC) continue;

		if(m_AStarData.TMapLayer==_MAPLAYER_TTHR){
			tmp_GuessDis=UONEGetGuessDistance2((*m_pThr_T->pThrEachMap.ThrMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
		}
		else if(!b_LocalRut && m_AStarData.TMapLayer == _MAPLAYER_THC){
			/*if(m_AStarData.TMapID==m_AStarData.NMapID && m_AStarData.NRtBID == m_AStarData.TRtBID)
			{
				continue;//走thc避免進入同一個區域
			}*/
			b_findit=nuFALSE;
			tmp_GuessDis=0;
		}
		else
		{	
			continue;	
		}
		m_AStarData.TRtBID=0;//thc裡並沒有RtBID，所以還原成0
		m_AStarData.NNoTurnFlag=0;//AStar比對資料 特別處理
		m_AStarData.Class=0;
		NewWeighting.RealDis=BestChoose.RealDis+(*m_pclsThc->p_StruConnect)[i].Length*CONST_UNITOFTHCDIS;
#ifdef KD
		if((*m_pclsThc->p_StruConnect)[i].bKmFlag == 1) 
		{
			m_AStarData.NowDis=(*m_pclsThc->p_StruConnect)[i].WeightingLength;
		}
		else
		{
			m_AStarData.NowDis=(*m_pclsThc->p_StruConnect)[i].WeightingLength*CONST_UNITOFTHCDIS;
		}
#else
		m_AStarData.NowDis=(*m_pclsThc->p_StruConnect)[i].WeightingLength*CONST_UNITOFTHCDIS;
#endif
		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis+1;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
 		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
}
#ifdef _DAR
	nuVOID  Class_CalRouting::Astar_AR(nuBOOL &b_CloseThr,NUROPOINT TargetCoor,nuLONG EndBoundaryRange,nuDWORD LimitNodeID_7,TagAStarData &m_SAVLData,AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose)
	{//只是純粹用來跳到TRT用的
	#ifndef VALUE_EMGRT
		nuDWORD j;
	#endif
		nuLONG tmp_GuessDis;
		AStarData m_AStarData;
		NUROPOINT Temp_FDifCoor;
		WEIGHTINGSTRU NewWeighting;
		
		//填充起訖點資訊
		m_AStarData.FMapID=m_SAVLData.NMapID;
		m_AStarData.FRtBID=m_SAVLData.NRtBID;
		m_AStarData.FNodeID=m_SAVLData.NNodeID;
		m_AStarData.FMapLayer=m_SAVLData.NMapLayer;
		m_AStarData.NMapID=m_SAVLData.TMapID;
		m_AStarData.NRtBID=m_SAVLData.TRtBID;
		m_AStarData.NNodeID=m_SAVLData.TNodeID;
		m_AStarData.NMapLayer=m_SAVLData.TMapLayer;
		m_AStarData.FNoTurnFlag=m_SAVLData.NNoTurnFlag;
		m_AStarData.NNoTurnFlag=m_SAVLData.TNoTurnFlag;
		m_AStarData.TNoTurnFlag=0;

		if(!m_pclsAR->AR_Node_Compare(m_AStarData.NNodeID,m_AStarData.NRtBID))
		{
			return;
		}
		
		if(m_AStarData.NMapLayer == _MAPLAYER_SRT)
		{
			m_AStarData.TMapID    = g_pRtB_T->MyState.MapID;
			m_AStarData.TMapLayer = _MAPLAYER_TRT;
		}
		m_AStarData.TNodeID   = m_pclsAR->ARtrans_C.NodeID;//取得道路編號 
		m_AStarData.TRtBID    = m_pclsAR->ARtrans_C.RTBID;
		m_AStarData.Class     = m_SAVLData.Class;//取得道路等級

		if(m_AStarData.NNodeID==m_AStarData.TNodeID)//路的頂點
		{
			return;
		}

		if((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NoTurnItem==0)//判斷會不會遇到禁轉
		{
			m_AStarData.TNoTurnFlag=0;//AStar比對資料 特別處理
		}
		else
		{
			m_AStarData.TNoTurnFlag=1;//AStar比對資料 特別處理
		}

		if(m_AStarData.TMapLayer==_MAPLAYER_TRT)
		{
			if((*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NoTurnItem==0)//判斷會不會遇到禁轉
			{
				m_AStarData.TNoTurnFlag=0;//AStar比對資料 特別處理
			}
			else
			{
				m_AStarData.TNoTurnFlag=1;//AStar比對資料 特別處理
			}
			#ifdef VALUE_EMGRT
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
			#else
				Temp_FDifCoor.x=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.x;
				Temp_FDifCoor.y=(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].NodeCoor.y;
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVX;j++){
					Temp_FDifCoor.x+=_COORLEVELNUM;
				}
				for(j=0;j<(*g_pRtB_T->RtBMain)[m_AStarData.TNodeID].CoorLVY;j++){
					Temp_FDifCoor.y+=_COORLEVELNUM;
				}
			#endif
			tmp_GuessDis=UONEGetGuessDistance2(Temp_FDifCoor,Temp_S_DiffCoor);
		}

		//if(tmp_GuessDis >= 150) return;
		if(!GetWeighting(m_AStarData.Class)) return ;
		m_AStarData.NowDis = (1*ClassWeighting) / _WEIGHTINGBASE;
		NewWeighting.RealDis = BestChoose.RealDis;
		
		m_AStarData.RDis=m_SAVLData.NowDis+m_SAVLData.RDis;
		if(m_AStarData.TMapLayer==_MAPLAYER_TRT)		m_AStarData.RDis++;
		NewWeighting.TotalDis=m_AStarData.NowDis+m_AStarData.RDis;//將計算過的資料填入open端
		
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
#endif

//#ifdef VALUE_EMGRT
	nuBOOL Class_CalRouting::PassNoTurn(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar,TagAStarData &m_SAVLData ){
		nuLONG MaxID,MinID,MidID,i,j,NTEndCount;
		LpAVLTree p_AVLTree=NULL;
		CCtrl_TT	*tmpNT;
		if(b_TargetArea){
			if(m_pNT_T==NULL)	return nuTRUE;
			tmpNT=m_pNT_T;
		}
		else{
			if(m_pNT_F==NULL)	return nuTRUE;
			tmpNT=m_pNT_F;
		}
		if(tmpNT->TotalNoTurnCount==0) return nuTRUE;

		MaxID=(tmpNT->TotalNoTurnCount)+1;
		MinID=0; 

		while(nuTRUE){
			MidID=(MaxID+MinID)/2;
			if(m_AStarData.NRtBID>(*tmpNT->m_TTItem)[MidID].EndRtBID)		{	MinID=MidID;	}
			else if(m_AStarData.NRtBID<(*tmpNT->m_TTItem)[MidID].EndRtBID)	{	MaxID=MidID;	}
			else{
				if(m_AStarData.TNodeID>(*tmpNT->m_TTItem)[MidID].EndNodeID)			{	MinID=MidID;	}
				else if(m_AStarData.TNodeID<(*tmpNT->m_TTItem)[MidID].EndNodeID)	{	MaxID=MidID;	}
				else{
					if(m_AStarData.NNodeID>(*tmpNT->m_TTItem)[MidID].PassNodeID)		{	MinID=MidID;	}
					else if(m_AStarData.NNodeID<(*tmpNT->m_TTItem)[MidID].PassNodeID)	{	MaxID=MidID;	}
					else{
						for(i=0;i<7;i++){
							NTEndCount=tmpNT->GetData(tmpNT->m_bUSETT,(*tmpNT->m_TTItem)[MidID].B_addr,i);
							if(NTEndCount==0) break;
							for(j=0;j<NTEndCount;j++){
								if(j==0){
									if(m_AStarData.FNodeID==tmpNT->TTIDList[j].NodeID){
										if(j==(NTEndCount-1))
										{
									//#ifdef _USETTI
											if(!tmpNT->m_bUSETT)
											{
												if(!TTIPass(b_TargetArea, tmpNT->TTI_Address))
												{
													return nuFALSE;
												}
												continue;
											}
											else
											{
												return nuFALSE;
											}
									//#else
											//return nuFALSE;	
									//#endif
										}
										continue;
									}
								}
								else if(j==1){
									m_ApprocahAStarData.NMapID=m_AStarData.FMapID;
									m_ApprocahAStarData.NRtBID=m_AStarData.FRtBID;
									m_ApprocahAStarData.NNodeID=m_AStarData.FNodeID;
									m_ApprocahAStarData.NMapLayer=m_AStarData.FMapLayer;
									m_ApprocahAStarData.TMapID=m_AStarData.NMapID;
									m_ApprocahAStarData.TRtBID=m_AStarData.NRtBID;
									m_ApprocahAStarData.TNodeID=m_AStarData.NNodeID;
									m_ApprocahAStarData.TMapLayer=m_AStarData.NMapLayer;
									m_ApprocahAStarData.NNoTurnFlag=1;
									m_ApprocahAStarData.RDis = 0;
									if(m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer)){
										if(p_AVLTree->Data.FNodeID==tmpNT->TTIDList[j].NodeID)
										{
											if(j==(NTEndCount-1))	
											{	
										#ifdef NT_TO_TT
												if(m_SAVLData.FNodeID != tmpNT->TTIDList[j].NodeID)
												{
													continue;
												}
										#endif
										//#ifdef _USETTI
												if(!tmpNT->m_bUSETT)
												{
													if(!TTIPass(b_TargetArea, tmpNT->TTI_Address))
													{
														return nuFALSE;
													}
													continue;
												}
												else
												{
													return nuFALSE;
												}
										//#else
												//return nuFALSE;	
										//#endif
											}
											continue;
										}
									}
								}
								else{
									m_ApprocahAStarData.NMapID=p_AVLTree->Data.FMapID;
									m_ApprocahAStarData.NRtBID=p_AVLTree->Data.FRtBID;
									m_ApprocahAStarData.NNodeID=p_AVLTree->Data.FNodeID;
									m_ApprocahAStarData.NMapLayer=p_AVLTree->Data.FMapLayer;
									m_ApprocahAStarData.TMapID=p_AVLTree->Data.NMapID;
									m_ApprocahAStarData.TRtBID=p_AVLTree->Data.NRtBID;
									m_ApprocahAStarData.TNodeID=p_AVLTree->Data.NNodeID;
									m_ApprocahAStarData.TMapLayer=p_AVLTree->Data.NMapLayer;
									m_ApprocahAStarData.NNoTurnFlag=1;
									if(m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer)){
										if(p_AVLTree->Data.FNodeID==tmpNT->TTIDList[j].NodeID){
											if(j==(NTEndCount-1))
											{
										//#ifdef _USETTI
												if(!tmpNT->m_bUSETT)
												{
													if(!TTIPass(b_TargetArea, tmpNT->TTI_Address))
													{
														return nuFALSE;
													}
													continue;
												}
												else
												{
													return nuFALSE;
												}
										//#else
												//return nuFALSE;	
										//#endif
											}
											continue;
										}
									}
								}
								if(m_AStarData.FNodeID==tmpNT->TTIDList[j].NodeID){
									if(j==(NTEndCount-1))
									{
									//#ifdef _USETTI
										if(!tmpNT->m_bUSETT)
										{
											if(!TTIPass(b_TargetArea, tmpNT->TTI_Address))
											{
												return nuFALSE;
											}
											continue;
										}
										else
										{
											return nuFALSE;
										}
									//#else
										//return nuFALSE;	
									//#endif
									}
									continue;
								}
								break;
							}
						}
						return nuTRUE;
					}
				}
			}
			if((MaxID-MinID)<=1) break;
		}
		return nuTRUE;
	}
/*#else
	nuBOOL Class_CalRouting::PassNoTurn(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar){
		nuLONG MaxID,MinID,MidID;
		LpAVLTree p_AVLTree=NULL;
		CCtrl_NT	*tmpNT;
		if(b_TargetArea){
			if(m_pNT_T==NULL)	return nuTRUE;
			tmpNT=m_pNT_T;
		}
		else{
			if(m_pNT_F==NULL)	return nuTRUE;
			tmpNT=m_pNT_F;
		}
		if(tmpNT->TotalNoTurnCount==0) return nuTRUE;
		MaxID=(tmpNT->TotalNoTurnCount)+1;
		MinID=0;
		while(nuTRUE){
			MidID=(MaxID+MinID)/2;
			if(m_AStarData.NRtBID>(*tmpNT->m_NTItem)[MidID].PassRtBID)		{	MinID=MidID;	}
			else if(m_AStarData.NRtBID<(*tmpNT->m_NTItem)[MidID].PassRtBID)	{	MaxID=MidID;	}
			else{
				if(m_AStarData.NNodeID>(*tmpNT->m_NTItem)[MidID].PassNodeID)		{	MinID=MidID;	}
				else if(m_AStarData.NNodeID<(*tmpNT->m_NTItem)[MidID].PassNodeID)	{	MaxID=MidID;	}
				else{
					if(m_AStarData.FNodeID>(*tmpNT->m_NTItem)[MidID].StartNodeID)		{	MinID=MidID;	}
					else if(m_AStarData.FNodeID<(*tmpNT->m_NTItem)[MidID].StartNodeID)	{	MaxID=MidID;	}
					else{
						if(m_AStarData.TNodeID>(*tmpNT->m_NTItem)[MidID].EndNodeID)			{	MinID=MidID;	}
						else if(m_AStarData.TNodeID<(*tmpNT->m_NTItem)[MidID].EndNodeID)	{	MaxID=MidID;	}
						else																{	return nuFALSE;	}
					}
				}
			}
			if((MaxID-MinID)<=1) break;
		}
		return nuTRUE;
	}
#endif*/

//****************************即時路況相關處理****************************************************//
//------------------------------------------------------------------------------------------------//
#ifdef _DDST
	nuBOOL Class_CalRouting::Dstroadsituation(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar)
	{
		if(m_pclsDst==NULL)
		{
			return nuTRUE;
		}
		LpAVLTree p_AVLTree = NULL;
		nuLONG MaxID, MidID, MinID;
		nuBOOL bDate = nuFALSE;

		if(m_pclsDst -> m_nTotalDstCount == 0)
		{
			return nuTRUE;
		}
		MaxID = m_pclsDst -> m_nTotalDstCount;
		MinID = 0;	
		while(1)//如果從道路前端進入(先判斷是否有符合的道路ID，才進行日期判斷)
		{
			MidID = (MaxID + MinID) / 2;
			if(m_AStarData.FNodeID > (*m_pclsDst -> Dstdata)[MidID].S_NODEID)
			{
				MinID = MidID;
			}
			else if(m_AStarData.FNodeID < (*m_pclsDst -> Dstdata)[MidID].S_NODEID)
			{
				MaxID = MidID;
			}
			else//找到道路前端入口，判斷道路尾端是否符合
			{
				if(m_AStarData.TNodeID > (*m_pclsDst -> Dstdata)[MidID].T_NODEID)
				{
					MinID = MidID;
				}
				else if(m_AStarData.TNodeID < (*m_pclsDst -> Dstdata)[MidID].T_NODEID)
				{
					MaxID = MidID;
				}
				else
				{
					bDate = nuTRUE;
					break;
				}
				return nuFALSE;
			}
			if((MaxID-MinID) <= 1)//找不到
			{
				break;
			}
		}
		while(bDate == nuFALSE)//從道路尾端進入(先判斷是否有符合的道路ID，才進行日期判斷)
		{
			MidID = (MaxID + MinID) / 2;
			if(m_AStarData.TNodeID > (*m_pclsDst -> Dstdata)[MidID].S_NODEID)
			{
				MinID = MidID;
			}
			else if(m_AStarData.TNodeID < (*m_pclsDst -> Dstdata)[MidID].S_NODEID)
			{
				MaxID = MidID;
			}
			else//找到道路尾端入口，判斷道路前端是否符合
			{
				if(m_AStarData.FNodeID > (*m_pclsDst -> Dstdata)[MidID].T_NODEID)
				{
					MinID = MidID;
				}
				else if(m_AStarData.FNodeID < (*m_pclsDst -> Dstdata)[MidID].T_NODEID)
				{
					MaxID = MidID;
				}
				else
				{
					bDate = nuTRUE;
					break;
				}
				return nuFALSE;
			}
			if((MaxID-MinID) <= 1)//找不到
			{
				return nuTRUE;	
			}
		}//如果現在的日期比修復路段的日期還要大，表示已修復
		if(bDate)
		{
			if(nuTIME.nYear > (*m_pclsDst -> Dstdata)[MidID].T_YY)
			{
				return nuTRUE;
			}
			else if(nuTIME.nYear == (*m_pclsDst -> Dstdata)[MidID].T_YY)
			{
				if(nuTIME.nMonth > (*m_pclsDst -> Dstdata)[MidID].T_MM)
				{
					return nuTRUE;
				}
				else if(nuTIME.nMonth == (*m_pclsDst -> Dstdata)[MidID].T_MM)
				{
					if(nuTIME.nDay > (*m_pclsDst -> Dstdata)[MidID].T_DD)
					{
						return nuTRUE;
					}
				}
				else
				{
					return nuFALSE;
				}
			}
			else//如果現在的日期比修復路段的日期還要小，則表示路段尚未完成修復
			{
				return nuFALSE;
			}
		}
		return nuTRUE;
	}
#endif
//-----------------------------------------------------------------------------------------------//
//***********************************************************************************************//
#ifdef _DTMC
	nuBOOL Class_CalRouting::Tmcroadsituation(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar)
	{
		if(m_pclsTmc == NULL) {return nuTRUE;} 

		nuLONG Mid = 0,Min = 0,Max = 0;

		if(m_AStarData.TRtBID != (*m_pclsTmc->m_ppTMCNodeIDList)[Min].RTBID)
		{
			return nuTRUE;
		}
		if((*m_pclsTmc->m_ppTMCNodeIDList)[0].NodeID == m_AStarData.TNodeID)
		{
			return nuFALSE;
		}

		Max = m_pclsTmc->TMCNodeIDCount - 1;
		Min = 0;
		while(Min <= Max)//Do not care from node
		{
			//Mid = (Max - Min) / 2;
			if((*m_pclsTmc->m_ppTMCNodeIDList)[Max].NodeID != (*m_pclsTmc->m_ppTMCNodeIDList)[Min].NodeID)
			{
				Mid = (Max - Min) * (m_AStarData.TNodeID - (*m_pclsTmc->m_ppTMCNodeIDList)[Min].NodeID) / 
				((*m_pclsTmc->m_ppTMCNodeIDList)[Max].NodeID-(*m_pclsTmc->m_ppTMCNodeIDList)[Min].NodeID) + Min; 
				if(Mid < Min || Mid > Max)
				{
					return nuTRUE; 
				} 
			}
			if((*m_pclsTmc->m_ppTMCNodeIDList)[Mid].NodeID > m_AStarData.TNodeID){ Max = Mid - 1; }
			else if((*m_pclsTmc->m_ppTMCNodeIDList)[Mid].NodeID < m_AStarData.TNodeID){ Min = Mid + 1; }
			else
			{						
				return nuFALSE;
			}
			if((Max - Min) <= 1)
			{
				if((*m_pclsTmc->m_ppTMCNodeIDList)[Max].NodeID != m_AStarData.TNodeID)
				{
					return nuFALSE;
				}
				else if((*m_pclsTmc->m_ppTMCNodeIDList)[Min].NodeID != m_AStarData.TNodeID)
				{
					return nuFALSE;
				}
				return nuTRUE;
			}
		}
		return nuTRUE;
	}
#endif
nuBOOL Class_CalRouting::GetWeighting(nuLONG Class)
{
	if(g_pRoutingRule->l_TotalSupportCount==0)//使用舊方式
	{
		switch(Class)
		{
			case 0:	ClassWeighting=10;	break;// 110/110
			case 1:	ClassWeighting=14;	break;// 110/80
			case 2:	ClassWeighting=16;	break;// 110/70
			case 3:	ClassWeighting=16;	break;// 110/70
			case 4:	ClassWeighting=22;	break;// 110/50
			case 5:	ClassWeighting=28;	break;// 110/40
			case 6:	ClassWeighting=37;	break;// 110/30
			case 7:	ClassWeighting=37;	break;// 110/30
			case 8:	ClassWeighting=37;	break;// 110/30
			case 9:	ClassWeighting=37;	break;// 110/30
			case 10:	ClassWeighting=37;	break;// 110/30
			default:	ClassWeighting=37;	break;// 110/30
		}
	}
	else 
	{   
		if(Class == 14)
		{
			Class = 13;	
		}
		if(Class>=g_pRoutingRule->l_TotalSupportCount)
		{
			ClassWeighting=(*(g_pRoutingRule->l_Weighting))[g_pRoutingRule->l_TotalSupportCount-1];
		}
		else
		{
			ClassWeighting=(*(g_pRoutingRule->l_Weighting))[Class];
		}
	}
	if(ClassWeighting==0) return nuFALSE;
	return nuTRUE;
}


nuBOOL Class_CalRouting::AStarSuccessWork(PNAVIFROMTOCLASS pNFTC)//AStar規劃成功的動作
{
/*
	nuDWORD TickCount[5];
	TickCount[0] = nuGetTickCount();
*/
	PNAVIRTBLOCKCLASS pTmpNaviRTBClass;
	pTmpNaviRTBClass=pNFTC->NRBC;
	while(pTmpNaviRTBClass!=NULL)
	{
		if(!LocalAStarSuccessWorkStep1(pTmpNaviRTBClass)){	Renum_RoutingState=ROUTE_SUCCESSSTEP1;	return nuFALSE;	};
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
#ifdef LCMM
	if(m_lReadBaseData >= 3)
	{
		ReMoveSysRTData();
	}
#else
	    ReMoveSysRTData();
#endif
	nuSleep(TOBESLEEPTIMER);
#ifdef ECI
	m_pECI3dp = new CCtrl_ECI;
	if(NULL == m_pECI3dp)
	{
		Renum_RoutingState=ROUTE_SUCCESSSTEP2;
		return nuFALSE;
	}
	else
	{
		m_pECI3dp->InitClass();				
		nuTcscpy(m_pECI3dp->ECIFName, nuTEXT(".ECI"));
		g_pRtFSApi->FS_FindFileWholePath(pNFTC->NRBC->FileIdx,m_pECI3dp->ECIFName,NURO_MAX_PATH);
		if(!m_pECI3dp->ReadFile())
		{
			Renum_RoutingState=ROUTE_SUCCESSSTEP2;
			return nuFALSE;
		}
	}
#endif
	pTmpNaviRTBClass=pNFTC->NRBC;
	while(pTmpNaviRTBClass!=NULL)
	{//處理3D檔和DW道路描點檔
		if(!LocalAStarSuccessWorkStep2(pTmpNaviRTBClass))
		{	
		#ifdef ECI
			m_pECI3dp->CloseClass();
			delete m_pECI3dp;
			m_pECI3dp = NULL;
		#endif
			Renum_RoutingState=ROUTE_SUCCESSSTEP2;	
			return nuFALSE;	
		}
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
#ifdef ECI
	if(NULL != m_pECI3dp)
	{
	delete m_pECI3dp;
	m_pECI3dp = NULL;
	}
#endif
	g_pDB_DwCtrl->Reset();
	g_pUOneBH->ReleaseBH();
	m_pcls3dp->ReleaseFile();
	pTmpNaviRTBClass=pNFTC->NRBC;
	nuSleep(TOBESLEEPTIMER);
	while(pTmpNaviRTBClass!=NULL)
	{
		if(!LocalAStarSuccessWorkStep3(pTmpNaviRTBClass)){	Renum_RoutingState=ROUTE_SUCCESSSTEP3;	return nuFALSE;	}
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::LocalAStarSuccessWorkStep1(PNAVIRTBLOCKCLASS pNRBC)//AStar規劃成功的動作
{
	nuULONG i;
	/*** 找出相對應MapID的MapIdx ***/
	for(i=0;i<m_MapIDtoMapIdx.MapCount;i++){
		if(pNRBC->FInfo.MapID==(*m_MapIDtoMapIdx.MapIDList)[i])
		{
			pNRBC->FileIdx=(nuWORD)((*m_MapIDtoMapIdx.MapIdxList)[i]);
			pNRBC->FInfo.MapIdx = pNRBC->FileIdx;
			break;
		}
	}

	/*** 如果block的規劃道路總數大於零 表示這個RTB是規劃過的 而非預規的 ***/
	if(pNRBC->NSSCount>0)
	{
		/*** 從RT檔整理需要的資料 ***/
		if(pNRBC->FInfo.MapID==g_pRtB_T->MyState.MapID && pNRBC->FInfo.RTBID==g_pRtB_T->MyState.RtBlockID)
		{
			if(!ReadRtBIndex(g_pRtB_T)) return nuFALSE;
			nuSleep(TOBESLEEPTIMER);
			GetNaviInfo_FromRtFile(pNRBC,g_pRtB_T);
			//nuSleep(_SLEEPTICKCOUNT);
			g_pRtB_T->Remove_Index();

			for(i=0;i<pNRBC->NSSCount;i++){
				pNRBC->NSS[i].PTSNameAddr=0;
			}
			#if (defined VALUE_EMGRT) || (defined ZENRIN)
				if(ReadPTS(g_pRtB_T->MyState.MapIdx)){
					for(i=0;i<pNRBC->NSSCount;i++){//處理收費站功能
						pNRBC->NSS[i].PTSNameAddr=CheckPTS(pNRBC->FInfo.RTBID,pNRBC->NodeIDList[i]);
					}
				}
				ReMovePTS();
			#endif
		}//直接讓內部去處理class
		else if(pNRBC->FInfo.MapID==g_pRtB_F->MyState.MapID && pNRBC->FInfo.RTBID==g_pRtB_F->MyState.RtBlockID)
		{
			if(!ReadRtBIndex(g_pRtB_F)) return nuFALSE;
			nuSleep(TOBESLEEPTIMER);
			GetNaviInfo_FromRtFile(pNRBC,g_pRtB_F);
			//nuSleep(_SLEEPTICKCOUNT);
			g_pRtB_F->Remove_Index();

			for(i=0;i<pNRBC->NSSCount;i++){
				pNRBC->NSS[i].PTSNameAddr=0;
			}
			#if (defined VALUE_EMGRT) || (defined ZENRIN)
				if(ReadPTS(g_pRtB_F->MyState.MapIdx)){
					for(i=0;i<pNRBC->NSSCount;i++){
						pNRBC->NSS[i].PTSNameAddr=CheckPTS(pNRBC->FInfo.RTBID,pNRBC->NodeIDList[i]);
					}
				}
				ReMovePTS();
			#endif
		}//直接讓內部去處理class
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::LocalAStarSuccessWorkStep2(PNAVIRTBLOCKCLASS pNRBC)//區域規劃(Local AStar)成功的後續資料整理
{
	nuULONG i, j, NodeCount = 0;
	nuSHORT NodeIdx = 0; 
	nuLONG ALPReadID = 0;
	/*** 如果block的規劃道路總數大於零 表示這個RTB是規劃過的 而非預規的 ***/
	if(pNRBC->NSSCount>0){

		if(g_pUOneBH->BHFileIdx!=pNRBC->FileIdx)
		{
			//先清資料
			g_pDB_DwCtrl->Reset();
			g_pUOneBH->ReleaseBH();
			m_pcls3dp->ReleaseFile();
			nuSleep(TOBESLEEPTIMER);

			#ifdef DEF_MAKETHRROUTE
				m_pcls3dp->InitClass();
				if(	g_pRoutingRule->l_3dp3ds == 0){//設一個參數決定用哪個檔 3D實景圖豪華版 3D實景圖標準版
					nuTcscpy(m_pcls3dp->FName, MakeThr_MapPath);
					nuTcscat(m_pcls3dp->FName, nuTEXT(".3ds"));
				}
				else{
					nuTcscpy(m_pcls3dp->FName, MakeThr_MapPath);
					nuTcscat(m_pcls3dp->FName, nuTEXT(".3dp"));
				}
			#else
				#ifndef ZENRIN
					m_pcls3dp->InitClass();
					if(	g_pRoutingRule->l_3dp3ds == 0){//設一個參數決定用哪個檔 3D實景圖豪華版 3D實景圖標準版
						nuTcscpy(m_pcls3dp->FName, nuTEXT(".3ds"));
						g_pRtFSApi->FS_FindFileWholePath(pNRBC->FileIdx,m_pcls3dp->FName,NURO_MAX_PATH);
					}
					else{
						nuTcscpy(m_pcls3dp->FName, nuTEXT(".3dp"));
						g_pRtFSApi->FS_FindFileWholePath(pNRBC->FileIdx,m_pcls3dp->FName,NURO_MAX_PATH);
					}
				#endif
			#endif
			
			nuSleep(TOBESLEEPTIMER);

			//開啟Dw檔
			#ifdef DEF_MAKETHRROUTE
				nuTcscpy(g_pUOneBH->FName, MakeThr_MapPath);
				nuTcscat(g_pUOneBH->FName, nuTEXT(".bh"));
			#else
				nuTcscpy(g_pUOneBH->FName, nuTEXT(".bh"));
				g_pRtFSApi->FS_FindFileWholePath(pNRBC->FileIdx,(g_pUOneBH)->FName,NURO_MAX_PATH);
			#endif
			g_pUOneBH->ReadBH();
			nuSleep(TOBESLEEPTIMER);
			g_pUOneBH->BHFileIdx=pNRBC->FileIdx;
			//開啟RDW檔
			#ifdef DEF_MAKETHRROUTE
				g_pDB_DwCtrl->InitClass();
				nuTcscpy(g_pDB_DwCtrl->FName, MakeThr_MapPath);
				nuTcscat(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
			#else
				g_pDB_DwCtrl->InitClass();
				nuTcscpy(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
				g_pRtFSApi->FS_FindFileWholePath(pNRBC->FileIdx,g_pDB_DwCtrl->FName,NURO_MAX_PATH);
			#endif
			if(nuFALSE==g_pDB_DwCtrl->Read_Header())	{	g_pDB_DwCtrl->Reset();	return nuFALSE;	}//讀取Dw檔頭
			g_pDB_DwCtrl->RDWFileIdx=pNRBC->FileIdx;
		}
		if(!GetNaviInfo_FromDwFile(pNRBC)){	return nuFALSE;	}/*** 從Dw檔整理需要的資料 ***/
		nuSleep(TOBESLEEPTIMER);
	#ifndef ECI
		if(m_pcls3dp->ReadFile())
		{
			for(i=0;i<(pNRBC->NSSCount-1);i++){//不能去計算最後一筆 不然i+1的地方會出錯
				pNRBC->NSS[i].Real3DPic=m_pcls3dp->CheckFNTCoor(pNRBC->NSS[i].StartCoor,pNRBC->NSS[i].EndCoor,pNRBC->NSS[i+1].EndCoor);
			}
		}
	#else
		for(i = 0; i < pNRBC->NodeIDCount; i++)
		{//不能去計算最後一筆 不然i+1的地方會出錯
			if(m_pECI3dp->CheckECI(pNRBC->RTBIdx, &pNRBC->NodeIDList[i], NodeCount, ALPReadID, pNRBC->NodeIDCount - i))
			{
				for(j = NodeCount / 2; j < NodeCount; j++)
				{
					if(0 == pNRBC->NSS[i + j].Real3DPic)
					{
						pNRBC->NSS[i + j].Real3DPic = ALPReadID;
					}
				}
			}
		}
	#endif
		pNRBC->NSS[pNRBC->NSSCount-1].Real3DPic=0;
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::LocalAStarSuccessWorkStep3(PNAVIRTBLOCKCLASS pNRBC)//區域規劃(Local AStar)成功的後續資料整理
{
	nuULONG j;
	if(pNRBC->NSSCount>0)
	{
		/*** 將頭尾的資料不必要的弧段剔除 ***/
		ReColFTRoad(pNRBC);//重新整理頭段資訊
		nuSleep(TOBESLEEPTIMER);
		/*** 整理轉向的旗標供語音播報判斷用 ***/
		ColTurnFlag(pNRBC);//整理轉向旗標
		nuSleep(TOBESLEEPTIMER);

		/*** 計算出這段路徑的完整距離 ***/
		pNRBC->RealDis =0;
		for(j=0;j<pNRBC->NSSCount;j++)
		{
			pNRBC->RealDis += pNRBC->NSS[j].RoadLength;//現在已行駛距離加上道路長度
			if((pNRBC->NSSCount-1)==j)
			{
				CarAngle=nulAtan2(pNRBC->NSS[j].ArcPt[pNRBC->NSS[j].ArcPtCount-1].y-pNRBC->NSS[j].ArcPt[0].y,pNRBC->NSS[j].ArcPt[pNRBC->NSS[j].ArcPtCount-1].x-pNRBC->NSS[j].ArcPt[0].x);
			}
		}	
	}

	if(g_pRoutingRule->BaseSpeed_KMH2MS<=0){
		pNRBC->WeightDis=pNRBC->WeightDis/20;
	}
	else{
		pNRBC->WeightDis=(pNRBC->WeightDis*10)/(g_pRoutingRule->BaseSpeed_KMH2MS*7);
	}

	return nuTRUE;
}



/*** 整理轉向旗標 ***/
nuBOOL Class_CalRouting::ColTurnFlag(PNAVIRTBLOCKCLASS pNRBC)
{
	nuLONG labsangle,SXTU,LR,TmpCrossCount,CrossDiffAng;//LR:左右(-1~+1) SXTU:直岔轉迴(0~4) Side:邊(-1~+1)
	nuBOOL b_LSide,b_RSide,b_LargeDiffAngle,b_SXCross;
	nuBOOL b_OtherCross_S,b_OtherCross_X,b_OtherCross_T;
	nuBOOL b_SameRoad;
	nuDWORD RoadClass= -1,VoiceType = -1;
	nuDWORD	i,j;
	nuLONG StraightCount;

	for(i=0;i<pNRBC->NSSCount;i++)
	{
		//目標值初始化
//#ifdef VALUE_EMGRT
		for(j = 0; j < pNRBC->NSS[i].CrossCount; j++)
		{
			if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)				
			{
				if((pNRBC->NSS[i].RoadVoiceType == VRT_TrafficCircle && 
					pNRBC->NSS[i].NCC[j].CrossVoiceType != VRT_TrafficCircle) ||
				   (pNRBC->NSS[i].RoadVoiceType != VRT_TrafficCircle && 
					pNRBC->NSS[i].NCC[j].CrossVoiceType == VRT_TrafficCircle))
				{
					pNRBC->NSS[i].TurnFlag = -1;
					break;
				}
			}
		}
		if(pNRBC->NSS[i].TurnFlag == -1)
		{
			pNRBC->NSS[i].TurnFlag = 0;
			continue;
		}
//#endif
		pNRBC->NSS[i].TurnFlag = T_No;
/*
		if(pNRBC->NSS[i].RoadLength<CONST_COMBINEROADSIZE && i!=0 && pNRBC->NSS[i].RoadVoiceType!=VRT_TrafficCircle){
			continue;
		}
*/
		//參數初始化
		TmpCrossCount		= 0;
		b_LSide				= nuTRUE;
		b_RSide				= nuTRUE;
		b_LargeDiffAngle	= nuTRUE;
		b_SXCross			= nuFALSE;//週邊道路是直,岔路
		b_OtherCross_S		= nuFALSE;
		b_OtherCross_X		= nuFALSE;
		b_OtherCross_T		= nuFALSE;
		b_SameRoad			= nuFALSE;//判斷是否為同等級同路名的道路
		LR					= 1;
		SXTU				= _TSCODE;
		StraightCount       = 0;

		RoadClass = -1;
		VoiceType = -1;
		/*** 取得絕對角度 ***/
		labsangle = NURO_ABS(pNRBC->NSS[i].TurnAngle);



		/*** 判定是往左還是往右 ***/
		if(pNRBC->NSS[i].TurnAngle > 0)
		{
			LR=1;
		}
		else
		{
			LR=-1;
		}

		/*** 判定直轉岔迴 ***/
		if(labsangle < _TSANG)		
		{
			SXTU = _TSCODE;	
		}
		else if(labsangle < _TXANG)
		{	
			SXTU = _TXCODE;	
		}
		else if(labsangle <= _TTANG)
		{	
			SXTU = _TTCODE;	
		}
		else						
		{	
			SXTU = _TUCODE;	
		}
		
		/*** 獲取整個路口狀態 判斷在路口時 導航路徑與可通行道路的關係(是否為側邊 有無相鄰的道路) ***/
		for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
		{
			if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)
			{
				continue;//原路排除
			}
			if( _TUANG < NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) )
			{
				continue;// 不將迴轉的道路視為路口可通行數目(迴轉的道路並不直觀)
			}
			//if(labsangle < _DEF_SHORTANGLE &&  _VRT_SHORTCROSS == pNRBC->NSS[i].NCC[j].CrossVoiceType ) continue;//直行路線 超短距岔路 不考慮(輔路排除條款)	
		#ifndef VALUE_EMGRT
			if(pNRBC->NSS[i].NCC[j].CrossClass == 14 )
			{
				continue;//將台灣計畫道路排除
			}
		#endif
			TmpCrossCount++;//運算路口可通行道路數目(不完全使用 只選擇與通行方向相關的資訊)

			CrossDiffAng=NURO_ABS(pNRBC->NSS[i].TurnAngle-pNRBC->NSS[i].NCC[j].CrossAngle);
			if(CrossDiffAng < _LARGEDIFFANGLE)
			{	
				b_LargeDiffAngle=nuFALSE;	
			}//判斷下一條路行走時左右有無角度大於30度的路，如果沒有為nuTRUE
			if(CrossDiffAng < _TSANG)			
			{	
				b_OtherCross_S=nuTRUE;	
			}//判斷下一條路行走時左右有無角度大於15度的路，如果有為nuTRUE
			else if(CrossDiffAng < _TXANG)	
			{	
				b_OtherCross_X=nuTRUE;	
			}//判斷下一條路行走時左右有無角度大於40度的路，如果有為nuTRUE
			else if(CrossDiffAng < _TTANG)	
			{	
				b_OtherCross_T=nuTRUE;	
			}//判斷下一條路行走時左右有無角度大於135度的路，如果有為nuTRUE
			
			//判斷準則：由現在的路為中線，下一條路向左角度為正，下一條路向右為負
			if(pNRBC->NSS[i].TurnAngle < pNRBC->NSS[i].NCC[j].CrossAngle)
			{
				b_LSide=nuFALSE;//如果下一條路的左邊有路，表示下一條路不在最左邊
			}
			if(pNRBC->NSS[i].TurnAngle > pNRBC->NSS[i].NCC[j].CrossAngle)	
			{
				b_RSide=nuFALSE;//如果下一條路的右邊有路，表示下一條路不在最右邊
			}
			if(NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) < _TXANG)
			{
				b_SXCross=nuTRUE;	
			}//如果下一條叉路是直路或岔路為nuTRUE
		}
		if(pNRBC->NSS[i].RoadClass != 5)
		{
			if(i + 2 < pNRBC->NSSCount)
			{
	#ifdef VALUE_EMGRT
				if((pNRBC->NSS[i + 1].RoadClass == 9 || pNRBC->NSS[i + 2].RoadClass == 9) &&
					(pNRBC->NSS[i].RoadClass <= 7)  && (pNRBC->NSS[i + 1].RoadLength <= 15))
				{
					if(pNRBC->NSS[i].CrossCount >= 2)
					{
						//pNRBC->NSS[i + 1].TurnFlag = -1;
						if(pNRBC->NSS[i].TurnAngle >= 0 && NURO_ABS(pNRBC->NSS[i].TurnAngle) < 50)
						{
							pNRBC->NSS[i].TurnFlag = T_LeftSide;
							continue;
						}
						else if(pNRBC->NSS[i].TurnAngle < 0 && NURO_ABS(pNRBC->NSS[i].TurnAngle) < 50)
						{
							pNRBC->NSS[i].TurnFlag = T_RightSide;
							continue;
						}
					}
				}
	#else
				if((pNRBC->NSS[i].RoadClass <= 7)  && 
					(pNRBC->NSS[i + 1].RoadClass == 9 && 
					pNRBC->NSS[i + 2].RoadClass == 9))
				{
					pNRBC->NSS[i + 1].TurnFlag = T_No;
				}
	#endif
			}

			if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _STRAIGHT)//直行不播報機制，先判斷是否為直路
			{
				for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
				{
					if((pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle))		
					{//判斷此岔路是否為下一條路
						RoadClass = pNRBC->NSS[i].NCC[j].CrossClass;//找到下一條路的等級
						VoiceType =  pNRBC->NSS[i].NCC[j].CrossVoiceType;//找到下一條路的路型
						if((pNRBC->NSS[i].RoadNameAddr != -1) && (pNRBC->NSS[i].RoadNameAddr == pNRBC->NSS[i].NCC[j].CrossRoadNameAddr) && 
							(pNRBC->NSS[i].RoadClass >= pNRBC->NSS[i].NCC[j].CrossClass))		
						{//判斷是否為同路名且現在道路速限低於下一條道路，意義為由慢或相等的等級到快速道路就不播報	
							b_SameRoad = nuTRUE;
							break;
						}										
					}
				}
				if(pNRBC->NSS[i].RoadClass > 5 && (RoadClass == 5 || VoiceType == 3) && (b_RSide || b_LSide))
				{
					for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
					{
						if((pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)
							|| NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) > 110 )
						{//排除下一條路和原路反方向的路
							continue;
						}
						if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TSANG && NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle - pNRBC->NSS[i].TurnAngle) > 50)
						{
							continue;
						}
						if( pNRBC->NSS[i].NCC[j].CrossAngle < pNRBC->NSS[i].TurnAngle)
						{
							b_SameRoad = nuTRUE;
							if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TXANG)
							{
								pNRBC->NSS[i].TurnFlag = T_LeftSide;
							}
							else if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TTANG)
							{
								pNRBC->NSS[i].TurnFlag = T_Left;
							}
							break;
						}
						else
						{
							b_SameRoad = nuTRUE;
							if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TXANG)
							{
								pNRBC->NSS[i].TurnFlag = T_RightSide;
							}
							else if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TTANG)
							{
								pNRBC->NSS[i].TurnFlag = T_Right;
							}
							break;
						}
					}
				}
				
				for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
				{
					if((NURO_ABS(pNRBC->NSS[i].TurnAngle) == NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle)))
					{//排除下一條路和原路反方向的路
						continue;
					}
					/*else if((i + 1) < pNRBC->NSSCount  &&  pNRBC->NSS[i].RoadLength < _SHORTESTROADLEN)  //如果下一條道路距離下下條道路只有不到70公尺的距離				
					{
						b_SameRoad = nuTRUE;
					}*/
					if((pNRBC->NSS[i].NCC[j].CrossClass == RoadClass &&
						NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) <= _VERTICAL))
					{//如果都為同等級道路，就該判斷方向，但是如果另一條路角度過大，則不判斷
						b_SameRoad = nuFALSE;
					}
					if(pNRBC->NSS[i].RoadClass > pNRBC->NSS[i].NCC[j].CrossClass &&
						NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle - pNRBC->NSS[i].TurnAngle) < _VERTICAL)//如果叉路的速限比較高則須讓使用者作判別
					{
						b_SameRoad = nuFALSE;
						break;
					}
					if(pNRBC->NSS[i].CrossCount == 2 && NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle - pNRBC->NSS[i].TurnAngle) < 10)
					{
						b_SameRoad = nuFALSE;
						break;
					}
				}
			}
		
			if(!b_SameRoad && pNRBC->NSS[i].CrossCount == 2) //如果下一條道路只有兩條路可走，其中一條道路的角度非常大，則角度大的道路不列入判斷			
			{
				for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
				{
					if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle || 
						NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) == 180) 
					{
						continue;
					}

					if(NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle - pNRBC->NSS[i].TurnAngle) > 70 &&
						NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) > 100 && NURO_ABS(pNRBC->NSS[i].TurnAngle) <= 50)
					{
						b_SameRoad = nuTRUE;
						break;
					}
					if((i + 1 < pNRBC->NSSCount) &&
						(pNRBC->NSS[i].NCC[j].CrossClass == 9 && pNRBC->NSS[i + 1].RoadClass != 9) && (pNRBC->NSS[i + 1].RoadLength <= 30) &&
						 pNRBC->NSS[i].NCC[j].CrossRoadNameAddr == pNRBC->NSS[i + 1].RoadNameAddr)
					{
						pNRBC->NSS[i].TurnFlag = T_No;
						b_SameRoad			   = nuTRUE;
						break;
					}
				}
			}
		}				

		// for multiple-lane road
		nuLONG k = 0,RL = 0;
		nuBOOL b_Original = nuFALSE, b_HaveRTurnType = nuFALSE;
		if(TmpCrossCount == 1 && NURO_ABS(pNRBC->NSS[i].TurnAngle) < _STRAIGHT)
		{
			for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
			{
				if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle
					|| NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) == 180)
				{
					continue;
				}
				if(pNRBC->NSS[i].NCC[j].CrossVoiceType == VRT_RTURN)
				{
					b_HaveRTurnType = nuTRUE;
					k = i+1;
					while(k < pNRBC->NSSCount)
					{
						if((pNRBC->NSS[k].TurnAngle < _TTANG &&  pNRBC->NSS[k].TurnAngle >= _TXANG)
							&& (pNRBC->NSS[i].RoadClass == 6 || pNRBC->NSS[i].RoadClass == 7))
						{
							b_Original = nuTRUE;
							b_SameRoad = nuFALSE;
							break;
						}
						if(RL > 100 || NURO_ABS(pNRBC->NSS[k].TurnAngle) > _TSANG)
						{
							break;
						}
						RL += pNRBC->NSS[k].RoadLength;
						k++;
					}
				}
			}
			if(!b_Original && b_HaveRTurnType)
			{
				b_SameRoad = nuTRUE;
				pNRBC->NSS[i].TurnFlag = T_Afore;
			}
		}
		//for straight road
		nuLONG lNextVoiceType = 0;
		if(!b_SameRoad && TmpCrossCount == 1 && !b_HaveRTurnType && NURO_ABS(pNRBC->NSS[i].TurnAngle) < 5)
		{
			for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
			{
				if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)
				{
					lNextVoiceType = pNRBC->NSS[i].NCC[j].CrossVoiceType;
				}
			}
				
			for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
			{
				if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle
					|| NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) == 180)
				{
					continue;
				}
				if(lNextVoiceType != 31 && RoadClass <= pNRBC->NSS[i].NCC[j].CrossClass && NURO_ABS(pNRBC->NSS[i].TurnAngle) < 5 &&
					NURO_ABS(pNRBC->NSS[i].TurnAngle - pNRBC->NSS[i].NCC[j].CrossAngle) > 15)
				{
					b_SameRoad = nuTRUE;
					pNRBC->NSS[i].TurnFlag = T_Afore;
					break;
				}
			}
		}
		//modify for road connect count equal to one
		if(!b_SameRoad && TmpCrossCount == 0 && 
			pNRBC->NSS[i].RoadClass > 5 && NURO_ABS(pNRBC->NSS[i].TurnAngle) > _TXANG && NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TTANG)
		{
			for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
			{
				if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)
				{
					if(pNRBC->NSS[i].RoadNameAddr != -1 && (pNRBC->NSS[i].RoadNameAddr != pNRBC->NSS[i].NCC[j].CrossRoadNameAddr)
						&& pNRBC->NSS[i].RoadClass <= pNRBC->NSS[i].NCC[j].CrossClass)
					{
						if(LR == 1)
						{
							pNRBC->NSS[i].TurnFlag = T_Left;
							b_SameRoad = nuTRUE;
							break;
						}
						else if(LR == -1)
						{
							pNRBC->NSS[i].TurnFlag = T_Right;
							b_SameRoad = nuTRUE;
							break;
						}
					}
				}
			}
		}

		if(!b_SameRoad)
		{
			if(pNRBC->NSS[i].CrossCount == 2) 				
			{
				for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
				{
					if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle)
					{
						continue;
					}
					if(NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) > 100)
					{
						if(b_LSide)
						{
							b_RSide = nuTRUE;
							b_LSide = nuFALSE;
							break;
						}
						else if(b_RSide)
						{	
							b_RSide = nuFALSE;
							b_LSide = nuTRUE;
							break;
						}
					}
					else if(NURO_ABS(pNRBC->NSS[i].TurnAngle) < _SMALLSTRAIGHT && NURO_ABS(pNRBC->NSS[i].TurnAngle - pNRBC->NSS[i].NCC[j].CrossAngle) >= _XROADANGLE)
					{
						TmpCrossCount = -1;
						break;
					}
				}
			}
			//modify for the road have three crosscount that crossangle is large than 50
			if(TmpCrossCount == 2 && NURO_ABS(pNRBC->NSS[i].TurnAngle) < _TXANG
				&& b_RSide == nuFALSE && b_LSide == nuFALSE
				&& b_SXCross && (b_OtherCross_S || b_OtherCross_X)
				&& VoiceType != VRT_ROADCROSS)
			{
				for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
				{
					if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle
						|| NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) == 180)
					{
						continue;
					}
					if(NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) > _VERYLARGEDIFFANGLE)
					{
						continue;
					}
					else
					{
						StraightCount++;
					}
				}
				if(StraightCount == 1)
				{
					for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
					{
						if(pNRBC->NSS[i].TurnAngle == pNRBC->NSS[i].NCC[j].CrossAngle
							|| NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) == 180)
						{
							continue;
						}
						if(NURO_ABS(pNRBC->NSS[i].NCC[j].CrossAngle) > _VERYLARGEDIFFANGLE)
						{
							continue;
						}
						if(pNRBC->NSS[i].TurnAngle < pNRBC->NSS[i].NCC[j].CrossAngle)
						{
							b_RSide = nuTRUE;
							break;
						}
						else
						{
							b_LSide = nuTRUE;
							break;
						}
					}
				}
			}
			/*** 依導航路徑的狀態決定使用的轉向旗標 ***/
			switch(TmpCrossCount)
			{
				case 0://大角度的轉彎 一定要播報
					switch(SXTU)
					{
						case _TSCODE://直
						case _TXCODE://叉
							pNRBC->NSS[i].TurnFlag = T_No;
							break;
						case _TTCODE://轉
						case _TUCODE://迴
						default:
							if(LR == 1)		
							{	
								pNRBC->NSS[i].TurnFlag = T_Left;	
							}
							else if(LR == -1)	
							{	 
								pNRBC->NSS[i].TurnFlag = T_Right;	
							}
							break;
					}
					break;
				case 1:
					switch(SXTU)
					{
						case _TSCODE://直
							if(b_OtherCross_S == nuTRUE || b_OtherCross_X == nuTRUE) 
							{		
								//if(b_RSide==nuTRUE && LR==-1){	pNRBC->NSS[i].TurnFlag=T_RightSide;	}
								//else if(b_LSide==nuTRUE && LR==1) {	pNRBC->NSS[i].TurnFlag=T_LeftSide;	}
								//else {pNRBC->NSS[i].TurnFlag=T_No;}
								if(b_RSide == nuTRUE)		
								{	
									pNRBC->NSS[i].TurnFlag = T_RightSide;	
								}
								else					
								{	
									pNRBC->NSS[i].TurnFlag = T_LeftSide;
								}
							}
							else if(b_Original && b_HaveRTurnType)
							{
								if(b_RSide == nuTRUE)		
								{	
									pNRBC->NSS[i].TurnFlag = T_RightSide;	
								}
								else					
								{	
									pNRBC->NSS[i].TurnFlag = T_LeftSide;
								}
							}
							else
							{
								pNRBC->NSS[i].TurnFlag = T_No;
							}
							break;
						case _TXCODE://叉
							if(b_OtherCross_S == nuTRUE || b_OtherCross_X == nuTRUE)
							{
								if(b_RSide == nuTRUE)		
								{	
									pNRBC->NSS[i].TurnFlag = T_RightSide;	
								}
								else					
								{	
									pNRBC->NSS[i].TurnFlag = T_LeftSide;	
								}
							}
							else
							{
								if(b_RSide == nuTRUE)		
								{	
									pNRBC->NSS[i].TurnFlag = T_RightSide;	
								}
								else if(b_LSide == nuTRUE)	
								{	
									pNRBC->NSS[i].TurnFlag = T_LeftSide;	
								}
								else					
								{   
									pNRBC->NSS[i].TurnFlag = T_No;        
								}
							}
							break;
						case _TTCODE://轉
							if(LR == -1 && b_RSide == nuTRUE)		
							{	
								pNRBC->NSS[i].TurnFlag = T_Right;	
							}
							else if(LR == 1 && b_LSide == nuTRUE)	
							{	
								pNRBC->NSS[i].TurnFlag = T_Left;	
							}
							else
							{
								if(LR == -1)		
								{	
									pNRBC->NSS[i].TurnFlag = T_Right;	
								}
								else if(LR == 1)	
								{	
									pNRBC->NSS[i].TurnFlag = T_Left;	
								}
							}
							break;
						case _TUCODE://迴
							if(LR == 1)		
							{	
								pNRBC->NSS[i].TurnFlag = T_LUturn;	
							}
							else if(LR == -1)	
							{	 
								pNRBC->NSS[i].TurnFlag = T_RUturn;	
							}
							break;
						default:
							pNRBC->NSS[i].TurnFlag = T_Direction;
							break;
					}
					break;
				case 2:
					switch(SXTU)
					{
						case _TSCODE://直
							if(!b_SXCross)
							{	
								pNRBC->NSS[i].TurnFlag = T_No;	
								break;	
							}
							else
							{
								if(b_LargeDiffAngle == nuTRUE)
								{	
									pNRBC->NSS[i].TurnFlag = T_No;
								}
								else
								{
									if( b_RSide == nuTRUE)
									{	
										pNRBC->NSS[i].TurnFlag = T_RightSide;
									}
									else if( b_LSide == nuTRUE)	
									{	
										pNRBC->NSS[i].TurnFlag = T_LeftSide;
									}
									else
									{
										pNRBC->NSS[i].TurnFlag = T_MidWay;//T_Afore;
									}
								}
							}
							break;
						case _TXCODE://叉
							if(b_SXCross)
							{
								if(b_RSide == nuTRUE)		{	pNRBC->NSS[i].TurnFlag=T_RightSide;	}
								else if(b_LSide == nuTRUE)	{	pNRBC->NSS[i].TurnFlag=T_LeftSide;	}
								/*else	
								{
									if(LR==1)		{	pNRBC->NSS[i].TurnFlag=T_LeftSide;	}			
									else if(LR==-1)		{	pNRBC->NSS[i].TurnFlag=T_RightSide;	}
								}*/
								else 					{	pNRBC->NSS[i].TurnFlag=T_MidWay;	}//取消向中線行駛
							}
							else
							{
								pNRBC->NSS[i].TurnFlag=T_No;
							}
							break;
						case _TTCODE://轉
							if(LR == -1 && b_RSide == nuTRUE)		
							{	
								pNRBC->NSS[i].TurnFlag = T_Right;
							}
							else if(LR == 1 && b_LSide == nuTRUE)	
							{	
								pNRBC->NSS[i].TurnFlag = T_Left;
							}
							else
							{
								if(LR == -1)		
								{	
									pNRBC->NSS[i].TurnFlag = T_Right;
								}
								else if(LR == 1)	 
								{	
									pNRBC->NSS[i].TurnFlag = T_Left;
								}
							}
							break;
						case _TUCODE://迴
							if(LR == 1)		
							{	
								pNRBC->NSS[i].TurnFlag = T_LUturn;
							}
							else if(LR == -1)	
							{	
								pNRBC->NSS[i].TurnFlag = T_RUturn;	
							}
							break;
						default:
							pNRBC->NSS[i].TurnFlag     = T_Direction;
							break;
					}
					break;
				default:
					switch(SXTU)
					{
						case _TSCODE://直
							if(b_SXCross)
							{	
								pNRBC->NSS[i].TurnFlag = T_Afore;	
								break;	
							}
							pNRBC->NSS[i].TurnFlag = T_No;
							break;
						case _TXCODE://叉
							if(b_SXCross)
							{
								if(b_RSide == nuTRUE)		
								{	
									pNRBC->NSS[i].TurnFlag = T_RightSide;
								}
								else if(b_LSide == nuTRUE)	
								{	
									pNRBC->NSS[i].TurnFlag = T_LeftSide;
								}
								else					
								{
									if(LR==1)		
									{	
										pNRBC->NSS[i].TurnFlag = T_LeftSide;
									}
									else			
									{	
										pNRBC->NSS[i].TurnFlag = T_RightSide;
									}
								}
							}
							else
							{
								pNRBC->NSS[i].TurnFlag=T_No;
							}
							break;
						case _TTCODE://轉
							if(LR==-1 && b_RSide==nuTRUE)		
							{	
								pNRBC->NSS[i].TurnFlag = T_Right;
							}
							else if(LR==1 && b_LSide == nuTRUE)	
							{	
								pNRBC->NSS[i].TurnFlag = T_Left;
							}
							else
							{
								if(LR==-1)		
								{	
									pNRBC->NSS[i].TurnFlag = T_Right;
								}
								else			
								{	
									pNRBC->NSS[i].TurnFlag = T_Left;
								}
							}
							break;
						case _TUCODE://迴
							if(LR==1)		
							{	
								pNRBC->NSS[i].TurnFlag = T_LUturn;	
							}
							else if(LR == -1)	 
							{	
								pNRBC->NSS[i].TurnFlag = T_RUturn;
							}
							break;
						default:
							pNRBC->NSS[i].TurnFlag     = T_Direction;
							break;
					}
					break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::ReColFTRoad(PNAVIRTBLOCKCLASS pNRBC)//重新整理頭尾資訊
{
	NUROPOINT Tmp_FixPt;
	nuLONG Tmp_Dis,i,j,StartVertex,EndVertex,NowNSSIdx,cutidx;
	nuBOOL b_StartSide=nuFALSE;
	/*** 以變數控制處理的是頭還是尾 ***/
	for(j=0;j<2;j++)
	{
		/*** 參數初始化 ***/
		cutidx=0;
		if(j==0){	b_StartSide=nuTRUE;		NowNSSIdx=0;					}
		else	{	b_StartSide=nuFALSE;		NowNSSIdx=pNRBC->NSSCount-1;		}
		StartVertex=0;
		EndVertex=pNRBC->NSS[NowNSSIdx].ArcPtCount-1;

		/*** 判定起訖點所在道路位置 ***/
		for(i=StartVertex;i<EndVertex;i++)
		{
			if(b_StartSide==nuTRUE)
			{
				nuPtFixToLineSeg(pNRBC->FInfo.FixCoor,pNRBC->NSS[NowNSSIdx].ArcPt[i],pNRBC->NSS[NowNSSIdx].ArcPt[i+1],&Tmp_FixPt,&Tmp_Dis);
				cutidx=i;
				if(Tmp_Dis<3)	break;
			}
			else
			{
				nuPtFixToLineSeg(pNRBC->TInfo.FixCoor,pNRBC->NSS[NowNSSIdx].ArcPt[EndVertex-i],pNRBC->NSS[NowNSSIdx].ArcPt[EndVertex-i-1],&Tmp_FixPt,&Tmp_Dis);
				cutidx=i;
				if(Tmp_Dis<3)	break;
			}
		}
		/*** 進行路段修正 ***/
		for(i=StartVertex;i<=EndVertex;i++)
		{
			if(b_StartSide==nuTRUE)
			{
				pNRBC->NSS[NowNSSIdx].ArcPt[i]=Tmp_FixPt;
				if(cutidx==i)	break;
			}
			else
			{
				pNRBC->NSS[NowNSSIdx].ArcPt[EndVertex-i]=Tmp_FixPt;
				if(cutidx==i)	break;
			}
		}

		/*** 重算起訖點道路長度 ***/
		pNRBC->NSS[NowNSSIdx].RoadLength=0;
		for(i=StartVertex;i<EndVertex;i++)
		{
			pNRBC->NSS[NowNSSIdx].RoadLength += GetDistance(pNRBC->NSS[NowNSSIdx].ArcPt[i],pNRBC->NSS[NowNSSIdx].ArcPt[i+1]);
		}
		GetWeighting(pNRBC->NSS[NowNSSIdx].RoadClass);
		pNRBC->WeightDis += pNRBC->NSS[NowNSSIdx].RoadLength * ClassWeighting / _WEIGHTINGBASE;

		/*** 如果道路長度 小於轉彎旗標最小判定距離 那就視之為直行 ***/
//		if(pNRBC.NSS[NowNSSIdx].RoadLength<_TURNFLAGDIFFDIS) 
//		{	pNRBC.NSS[NowNSSIdx].TurnAngle=0;	}
	}
	if((NURO_ABS(pNRBC->TInfo.FixCoor.x - pNRBC->NSS[NowNSSIdx].ArcPt[pNRBC->NSS[NowNSSIdx].ArcPtCount-1].x) <= 2) &&
		(NURO_ABS(pNRBC->TInfo.FixCoor.y - pNRBC->NSS[NowNSSIdx].ArcPt[pNRBC->NSS[NowNSSIdx].ArcPtCount-1].y) <= 2))
	{
		for(nuINT k = pNRBC->NSS[NowNSSIdx].ArcPtCount - 1; k > 0; k--)
		{
			if((NURO_ABS(pNRBC->TInfo.FixCoor.x - pNRBC->NSS[NowNSSIdx].ArcPt[k].x) > 2) ||
				(NURO_ABS(pNRBC->TInfo.FixCoor.y - pNRBC->NSS[NowNSSIdx].ArcPt[k].y) > 2))
			{
				break;
			}
			pNRBC->NSS[NowNSSIdx].ArcPt[k].x = pNRBC->TInfo.FixCoor.x;
			pNRBC->NSS[NowNSSIdx].ArcPt[k].y = pNRBC->TInfo.FixCoor.y;
		}
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::GetNaviInfo_FromRtFile(PNAVIRTBLOCKCLASS pNRBC,UONERtBCtrl *TmpRtBCtrl)
{
	nuULONG i,j;
	nuLONG TmpConnectIdx,TmpIdx,TmpConnectCount;
	nuUCHAR WeightingClass;
	nuUSHORT dontfindB[2];
	nuUSHORT dontfindR[2];
	pNRBC->FileIdx=(nuWORD)TmpRtBCtrl->MyState.MapIdx;//取得地圖的索引值
	pNRBC->RTBIdx=TmpRtBCtrl->MyState.RtBlockIdx;

	/*** 取得路段起訖Node座標 並取得ToNode的叉路數(不包含起訖這一段路) ***/
	for(i=0;i<pNRBC->NSSCount;i++){
#ifdef VALUE_EMGRT//取得在單一圖層下所有路徑NodeID的座標
		pNRBC->NSS[i].StartCoor.x=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.x;
		pNRBC->NSS[i].StartCoor.y=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.y;
		pNRBC->NSS[i].EndCoor.x=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.x;
		pNRBC->NSS[i].EndCoor.y=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.y;
#else
		pNRBC->NSS[i].StartCoor.x=TmpRtBCtrl->OrgCoor.x+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].CoorLVX*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.x;
		pNRBC->NSS[i].StartCoor.y=TmpRtBCtrl->OrgCoor.y+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].CoorLVY*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.y;
		pNRBC->NSS[i].EndCoor.x=TmpRtBCtrl->OrgCoor.x+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].CoorLVX*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.x;
		pNRBC->NSS[i].EndCoor.y=TmpRtBCtrl->OrgCoor.y+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].CoorLVY*_COORLEVELNUM+(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.y;
		pNRBC->NSS[i].LaneGuideFlag = (*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].LaneGuideFlag;//added by daniel for LaneGuide 20111013
#endif

		TmpConnectCount=0;//計算每個節點所擁有交叉路口的數量
		for(j=0;j<(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].ConnectCount;j++){
			TmpConnectIdx=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].ConnectStruIndex+j-1;
			if( (*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TRtBID == _THRRTBID)
			{
				continue;//遇到BlockID為4095的話全都避開(因為跨區) 
			}
			TmpConnectCount++;
		}
		g_pCRDC->Create_CrossData(pNRBC->NSS[i],TmpConnectCount);//建立交叉路口結構
	}
	
	/*** 取得路段RoadID 並蒐集行徑路段及交叉路口的長度和型態 ***/
	for(i=0;i<pNRBC->NSSCount;i++)
	{
		//這兩個參數暫時儲存RoadID
		pNRBC->NSS[i].CityID	 = _MAXUNSHORT;
		pNRBC->NSS[i].TownID	 = _MAXUNSHORT;
		pNRBC->NSS[i].Flag		 = _MAXUNSHORT;
		pNRBC->NSS[i].TurnFlag	 = _MAXUNSHORT;
		WeightingClass			 = 0;
		pNRBC->NSS[i].RoadLength = _ROUTEDATABASERDLEN;
		/*** 由起訖NodeID得到RoadID ***/
		pNRBC->NSS[i].BlockIdx	 = _MAXUNSHORT;
		pNRBC->NSS[i].RoadIdx	 = _MAXUNSHORT;
		GetBlockRoadIdx(&(pNRBC->NSS[i].BlockIdx),&(pNRBC->NSS[i].RoadIdx),pNRBC->NodeIDList[i],pNRBC->NodeIDList[i+1],&TmpIdx,TmpRtBCtrl,_MAXUNSHORT,_MAXUNSHORT,_MAXUNSHORT,_MAXUNSHORT);

		pNRBC->NSS[i].CityID=pNRBC->NSS[i].BlockIdx;	pNRBC->NSS[i].TownID=pNRBC->NSS[i].RoadIdx;
		pNRBC->NSS[i].BlockIdx = _MAXUNSHORT;	
		pNRBC->NSS[i].RoadIdx  = _MAXUNSHORT;
		if(pNRBC->NSS[i].CityID != _MAXUNSHORT)
		{
			GetBlockRoadIdx(&(pNRBC->NSS[i].BlockIdx),&(pNRBC->NSS[i].RoadIdx),pNRBC->NodeIDList[i],pNRBC->NodeIDList[i+1],&TmpIdx,TmpRtBCtrl,(nuUSHORT)pNRBC->NSS[i].CityID,(nuUSHORT)pNRBC->NSS[i].TownID,_MAXUNSHORT,_MAXUNSHORT);
		}

		for(j=0;j<(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].ConnectCount;j++){
			TmpConnectIdx=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].ConnectStruIndex+j-1;//同個NodeID可能有很多連接點
			if((*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TNodeID==pNRBC->NodeIDList[i+1]){//此目的在於對照計算出來的NodeID是否與檔案中的相符合
				/*** 蒐集行徑路段及交叉路口的長度和語音播報型態 ***/
				if(pNRBC->NSS[i].RoadLength>(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].RoadLength){
					pNRBC->NSS[i].RoadLength	=	(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].RoadLength;
					pNRBC->NSS[i].RoadVoiceType =	0;
					pNRBC->NSS[i].RoadVoiceType	=	(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].VoiceRoadType;
					WeightingClass				=	(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].RoadClass;
				}
			#ifdef VALUE_EMGRT
				pNRBC->NSS[i].SPTFlag           =   0;
				//pNRBC->NSS[i].PTSFlag		    =   0;
				//pNRBC->NSS[i].ARFlag            =   0;
				pNRBC->NSS[i].SPTFlag           =   (*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].SPTFlag;
				//pNRBC->NSS[i].PTSFlag           =   (*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].PTSFlag;
				//pNRBC->NSS[i].ARFlag            =   (*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i]].ARFlag;
			#endif
			}
		}
		if(i!=0 && i!=(pNRBC->NSSCount-1)){//計算加權
			GetWeighting(WeightingClass);
			pNRBC->WeightDis +=	pNRBC->NSS[i].RoadLength * ClassWeighting / _WEIGHTINGBASE;
		}
	}

	/*** 填補ToNode交叉路段的RoadID ***/
	for(i=0;i<pNRBC->NSSCount;i++){
		dontfindB[0] = _MAXUNSHORT;
		dontfindB[1] = _MAXUNSHORT;
		dontfindR[0] = _MAXUNSHORT;
		dontfindR[1] = _MAXUNSHORT;
		TmpConnectCount=0;
		for(j=0;j<(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].ConnectCount;j++){
			TmpConnectIdx=(*TmpRtBCtrl->RtBMain)[pNRBC->NodeIDList[i+1]].ConnectStruIndex+j-1;
			/*** 排除跨區的情形 ***/
			if( (*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TRtBID == _THRRTBID)
			{
				continue;
			}
			/*** 排除行徑的路段 ***/
			GetBlockRoadIdx(&(pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx),//取得所有交叉路的BlockIdx及RoadIdx
							&(pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx),
							pNRBC->NodeIDList[i+1],
							(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TNodeID,
							&TmpIdx,
							TmpRtBCtrl,
							dontfindB[0],
							dontfindR[0],
							dontfindB[1],
							dontfindR[1]);

			if(	(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TRtBID==pNRBC->FInfo.RTBID &&
				(*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].TNodeID==pNRBC->NodeIDList[i]){
				pNRBC->NSS[i].NCC[TmpConnectCount].CrossClass=-1;//暫時用這個參數當成來源參考
				if(dontfindR[0] == _MAXUNSHORT)
				{	dontfindB[0]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx;	dontfindR[0]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx;	}
				else					
				{	dontfindB[1]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx;	dontfindR[1]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx;	}
			}
			else{
				pNRBC->NSS[i].NCC[TmpConnectCount].CrossClass=-2;//暫時用這個參數當成來源參考
				if(dontfindR[0] == _MAXUNSHORT)
				{	dontfindB[0]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx;	dontfindR[0]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx;	}
				else					
				{	dontfindB[1]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx;	dontfindR[1]=pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx;	}
			}

//			if((i+1)<pNRBC->NSSCount){
//				if(pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx==pNRBC->NSS[i+1].CityID && pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx==pNRBC->NSS[i+1].TownID) continue;
//				if(pNRBC->NSS[i].NCC[TmpConnectCount].CrossBlockIdx==pNRBC->NSS[i+1].BlockIdx && pNRBC->NSS[i].NCC[TmpConnectCount].CrossRoadIdx==pNRBC->NSS[i+1].RoadIdx) continue;
//			}

			//填充交叉路口的語音狀態
			pNRBC->NSS[i].NCC[TmpConnectCount].CrossVoiceType = 0;
			pNRBC->NSS[i].NCC[TmpConnectCount].CrossVoiceType = (*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].VoiceRoadType;
//			if((*TmpRtBCtrl->RtBConnect)[TmpConnectIdx].RoadLength<CONST_COMBINEROADSIZE)	pNRBC->NSS[i].NCC[TmpConnectCount].CrossVoiceType = _VRT_SHORTCROSS;
			TmpConnectCount++;
		}
//		pNRBC->NSS[i].CrossCount=TmpConnectCount;
	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::ReadRtBIndex(UONERtBCtrl *TmpRtBCtrl)
{
	#ifdef DEF_MAKETHRROUTE
		nuTcscpy(TmpRtBCtrl->FName, MakeThr_MapPath);
		nuTcscat(TmpRtBCtrl->FName, nuTEXT(".rt"));
	#else
#ifndef _USERT6
		nuTcscpy(TmpRtBCtrl->FName, nuTEXT(".rt"));
#else
		if(b_LocalRut && m_bReadRt6)
		{
			nuTcscpy(TmpRtBCtrl->FName, nuTEXT(".rt6"));
		}
		else
		{
			nuTcscpy(TmpRtBCtrl->FName, nuTEXT(".rt"));
		}
#endif
		g_pRtFSApi->FS_FindFileWholePath(TmpRtBCtrl->MyState.MapIdx,TmpRtBCtrl->FName,NURO_MAX_PATH);
	#endif

	if(nuFALSE==TmpRtBCtrl->Read_Index()){	TmpRtBCtrl->ReSet();	return nuFALSE;	}
	return nuTRUE;
}

nuBOOL Class_CalRouting::GetNaviInfo_FromDwFile(PNAVIRTBLOCKCLASS pNRBC)
{
	nuLONG l;
	nuDWORD i,j,k,TmpSaveCount;
	nuUSHORT TmpBkIdx[2],TmpRdIdx[2];
	nuLONG FindRdIndex,FindRdVertexcount;
	UONERoadData RD;
	UONERoadCommonData RDComm;
	NUROPOINT tempPt,tempPt2,BHCoor;

nuDWORD TickCount[5];
TickCount[0] = nuGetTickCount();
	/*** 經由BlockID ,RoadID 取得路名位置 路段座標點數 路段座標點 邊界(RoadNameAddr ArcPtCount Bound ArcPt) ***/
	for(i=0;i<pNRBC->NSSCount;i++){
		TmpSaveCount=0;
		if(pNRBC->NSS[i].CityID != _MAXUNSHORT){
			TmpBkIdx[0]=(nuUSHORT)pNRBC->NSS[i].CityID;
			TmpRdIdx[0]=(nuUSHORT)pNRBC->NSS[i].TownID;
			TmpSaveCount++;
		}
		if(pNRBC->NSS[i].RoadIdx != _MAXUNSHORT){	
			TmpBkIdx[1]=(nuUSHORT)pNRBC->NSS[i].BlockIdx;
			TmpRdIdx[1]=(nuUSHORT)pNRBC->NSS[i].RoadIdx;
			TmpSaveCount++;
		}

		FindRdIndex=0;
		FindRdVertexcount=9999;
		if(TmpSaveCount>1){
			for(j=0;j<TmpSaveCount;j++){
				//載入失敗 就清空內存再試一次 若真的不行才放棄
				if(!g_pDB_DwCtrl->Read_BlockData(TmpBkIdx[j])){
					for(k=0;k<g_pUOneBH->m_BH.m_BHH.BlockCount;k++){
						g_pDB_DwCtrl->Remove_BlockData(k);
					}
					if(!g_pDB_DwCtrl->Read_BlockData(TmpBkIdx[j])){
						return nuFALSE;
					}
				}

				RD=(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[j]].RdData)[TmpRdIdx[j]];
				if(RD.VertexCount<FindRdVertexcount){
					FindRdIndex=j;
					FindRdVertexcount=RD.VertexCount;
				}
			}
		}


		//載入失敗 就清空內存再試一次 若真的不行才放棄
		if(!g_pDB_DwCtrl->Read_BlockData(TmpBkIdx[FindRdIndex])){
			for(k=0;k<g_pUOneBH->m_BH.m_BHH.BlockCount;k++){
				g_pDB_DwCtrl->Remove_BlockData(k);
			}
			if(!g_pDB_DwCtrl->Read_BlockData(TmpBkIdx[FindRdIndex])){
				return nuFALSE;
			}
		}

		RD=(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].RdData)[TmpRdIdx[FindRdIndex]];
		RDComm=(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].RdCommData)[RD.RoadCommonIndex];

		pNRBC->NSS[i].BlockIdx=TmpBkIdx[FindRdIndex];
		pNRBC->NSS[i].RoadIdx=TmpRdIdx[FindRdIndex];
		pNRBC->NSS[i].RoadNameAddr=RDComm.RoadNameAddr;
		pNRBC->NSS[i].CityID=RDComm.CityID;
		pNRBC->NSS[i].TownID=RDComm.TownID;
		pNRBC->NSS[i].Bound=(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].RdBoundary)[TmpRdIdx[FindRdIndex]];
		pNRBC->NSS[i].RoadClass=RDComm.RoadType;
		pNRBC->NSS[i].bGPSState=RD.bGPSState;
		g_pCRDC->Create_ArcPt(pNRBC->NSS[i],RD.VertexCount);
		
		BHCoor=g_pUOneBH->BlockBaseCoor(TmpBkIdx[FindRdIndex]);
		tempPt.x = BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex].x;
		tempPt.y = BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex].y;
		tempPt2.x = BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1].x;
		tempPt2.y = BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1].y;
			
		/*if(i == 1)//因為第一個點位置不確定會落在描點路徑中的哪一點，所以需用一開始的起點作為判斷，用BH一開始的點位來做判斷會造成方向顛倒
		{
			if(GetDistance(tempPt2,pNRBC->FInfo.FixCoor)>GetDistance(tempPt,pNRBC->FInfo.FixCoor))//判斷行經方向性
			{
				for(k=0;k<RD.VertexCount;k++)
				{
					pNRBC->NSS[i].ArcPt[k].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+k].x;
					pNRBC->NSS[i].ArcPt[k].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+k].y;
				}
			}
			else
			{
				for(k=0;k<RD.VertexCount;k++)
				{
					pNRBC->NSS[i].ArcPt[k].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-k].x;
					pNRBC->NSS[i].ArcPt[k].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-k].y;
				}
			}
		}
		else*/
		{
			if(GetDistance(tempPt2,pNRBC->NSS[i].StartCoor)>GetDistance(tempPt,pNRBC->NSS[i].StartCoor))//判斷行經方向性
			{
				pNRBC->NSS[i].nOneWay = 1;
				for(k=0;k<RD.VertexCount;k++)
				{
					pNRBC->NSS[i].ArcPt[k].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+k].x;
					pNRBC->NSS[i].ArcPt[k].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+k].y;
				}
			}
			else
			{
				pNRBC->NSS[i].nOneWay = 2;
				for(k=0;k<RD.VertexCount;k++)
				{
					pNRBC->NSS[i].ArcPt[k].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-k].x;
					pNRBC->NSS[i].ArcPt[k].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[TmpBkIdx[FindRdIndex]].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-k].y;
				}
			}
		}
	}
TickCount[1] = nuGetTickCount();
	/*** 透過座標 換算路口的角度 ***/
	NUROPOINT Pt[3];
	for(i=0;i<(pNRBC->NSSCount-1);i++){
		//預設叉路角度
		pNRBC->NSS[i].TurnAngle=0;
		if(pNRBC->NSS[i].RoadLength<CONST_COMBINEROADSIZE && i!=0){
			continue;
		}
		else{
			Pt[1]=pNRBC->NSS[i].EndCoor;//此決定Pt[1]和Pt[0]為一段，此兩點距離需大於30
			for(k=1;k<pNRBC->NSS[i].ArcPtCount;k++)
			{
				Pt[0]=pNRBC->NSS[i].ArcPt[pNRBC->NSS[i].ArcPtCount-1-k];
				if((NURO_ABS(Pt[0].x - Pt[1].x) + NURO_ABS(Pt[0].y - Pt[1].y)) > _TURNFLAGDIFFDIS)
				{
					break;
				}
			}

			for(j=i+1;j<pNRBC->NSSCount;j++)
			{
				if(pNRBC->NSS[j].RoadLength < CONST_COMBINEROADSIZE && j < (pNRBC->NSSCount-1))
				{
					continue;
				}
				else
				{
					for(k = 1; k < pNRBC->NSS[j].ArcPtCount; k++)
					{//此決定Pt[1]和Pt[2]為另一段，此兩點距離需大於30
						Pt[2] = pNRBC->NSS[j].ArcPt[k];
						if((NURO_ABS(Pt[2].x-Pt[1].x) + NURO_ABS(Pt[2].y-Pt[1].y)) > _TURNFLAGDIFFDIS)
						{
							break;
						}
					}
					break;
				}
			}
			pNRBC->NSS[i].TurnAngle = GetAngle(Pt[0],Pt[1],Pt[2]);
		}
	}
	pNRBC->NSS[pNRBC->NSSCount-1].TurnAngle = 0;

	//TickCount[2] = nuGetTickCount();
	/*** 經由交叉路口的RoadID 取得交叉路口路名位置 並計算交叉路口角度 ***/
	for(i=0;i<pNRBC->NSSCount;i++)
	{
		if(pNRBC->NSS[i].RoadLength >= CONST_COMBINEROADSIZE || i==0)
		{
			//先定下來原點
			Pt[1] = pNRBC->NSS[i].EndCoor;
			for(k = 1; k < pNRBC->NSS[i].ArcPtCount; k++)
			{
				Pt[0]=pNRBC->NSS[i].ArcPt[pNRBC->NSS[i].ArcPtCount-1-k];
				if((NURO_ABS(Pt[0].x-Pt[1].x) + NURO_ABS(Pt[0].y-Pt[1].y)) > _TURNFLAGDIFFDIS) 
				{
					break;
				}
			}
		}

		for(j=0;j<pNRBC->NSS[i].CrossCount;j++)
		{
			if(pNRBC->NSS[i].NCC[j].CrossRoadIdx == _MAXUNSHORT)
			{
				continue;
			}

			if(!g_pDB_DwCtrl->Read_BlockData(pNRBC->NSS[i].NCC[j].CrossBlockIdx))
			{
				//載入失敗 就清空內存再試一次 若真的不行才放棄
				for(k = 0; k < g_pUOneBH->m_BH.m_BHH.BlockCount; k++)
				{
					g_pDB_DwCtrl->Remove_BlockData(k);
				}
				if(!g_pDB_DwCtrl->Read_BlockData(pNRBC->NSS[i].NCC[j].CrossBlockIdx))
				{
					return nuFALSE;
				}
			}
			RD=(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].RdData)[pNRBC->NSS[i].NCC[j].CrossRoadIdx];
			pNRBC->NSS[i].NCC[j].CrossRoadNameAddr=(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].RdCommData)[RD.RoadCommonIndex].RoadNameAddr;
			pNRBC->NSS[i].NCC[j].CrossClass=(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].RdCommData)[RD.RoadCommonIndex].RoadType;

			BHCoor=g_pUOneBH->BlockBaseCoor(pNRBC->NSS[i].NCC[j].CrossBlockIdx);
			tempPt.x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex].x;
			tempPt.y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex].y;
			if((NURO_ABS(tempPt.x - Pt[1].x) <= 1 ) && (NURO_ABS(tempPt.y - Pt[1].y) <= 1))
			{
				for(l = 1; l < RD.VertexCount; l++)
				{//讓Pt[2]抓取起始座標點資料
					Pt[2].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex+l].x;
					Pt[2].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex+l].y;
					if((NURO_ABS(Pt[2].x-Pt[1].x)+NURO_ABS(Pt[2].y-Pt[1].y)) > _TURNFLAGDIFFDIS)
					{
						break;
					}
				}
			}
			else{
				for(l = 1; l < RD.VertexCount; l++)
				{
					Pt[2].x=BHCoor.x+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-l].x;
					Pt[2].y=BHCoor.y+(*(*g_pDB_DwCtrl->Rd)[pNRBC->NSS[i].NCC[j].CrossBlockIdx].VertexCoor)[RD.StartVertexIndex+RD.VertexCount-1-l].y;
					if((NURO_ABS(Pt[2].x-Pt[1].x)+NURO_ABS(Pt[2].y-Pt[1].y)) > _TURNFLAGDIFFDIS)
					{
						break;
					}
				}
			}
			if( i+1<pNRBC->NSSCount && pNRBC->NSS[i+1].BlockIdx==pNRBC->NSS[i].NCC[j].CrossBlockIdx && pNRBC->NSS[i+1].RoadIdx==pNRBC->NSS[i].NCC[j].CrossRoadIdx)
			{
				pNRBC->NSS[i].NCC[j].CrossAngle=(nuSHORT)pNRBC->NSS[i].TurnAngle;
			}
			else
			{
				pNRBC->NSS[i].NCC[j].CrossAngle=(nuSHORT)GetAngle(Pt[0],Pt[1],Pt[2]);
			}
		}
	}
TickCount[3] = nuGetTickCount();
	return nuTRUE;
}

nuBOOL Class_CalRouting::GetNodeIDList(AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T)
{
	nuULONG i,CheckCount=0;
	nuULONG TmpCount,New_TmpCount;
	LpAVLTree p_AVLTree=NULL;
	NodeListStru **TmpList=NULL;
	ApproachStru **tmpp_Approach;

	/*** 初始化存儲結構 ***/
	Tmp_Approach.ReleaseApproach();

	/*** 產生存放NodeList的過程結構 ***/
	TmpCount=0;
	if(m_AStar.lUsedCloseCount>1000)
	{	TmpList=(NodeListStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_NodeListStru)*(900+m_AStar.lUsedCloseCount/10));		CheckCount=900+m_AStar.lUsedCloseCount/10;	}
	else
	{	TmpList=(NodeListStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_NodeListStru)*(m_AStar.lUsedCloseCount+2));		CheckCount=m_AStar.lUsedCloseCount+2;	}

	if(TmpList==NULL)
	{
		/*MessageBox(NULL,TEXT("Error Code 100"),TEXT("system memory not enough"),0|MB_TOPMOST);*/
		return nuFALSE;
	}

	/*** 取得通往終端Node的資料 ***/
	m_ApprocahAStarData.NNoTurnFlag=1;
	#ifdef NT_TO_TT
		m_ApprocahAStarData.RDis = 1;
	#endif
	if(!m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer))//<-從樹中找到資料，失敗表示樹有問題
	{	g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	TmpList=NULL;	return nuFALSE;	}

	/*** 補尾的動作處理完了 就要開始整理完整的過程囉 ***/
	(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.TNodeID;
	(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.TRtBID;
	(*TmpList)[TmpCount].MapID=p_AVLTree->Data.TMapID;
	(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.TMapLayer;
	TmpCount++;
	(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.NNodeID;
	(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.NRtBID;
	(*TmpList)[TmpCount].MapID=p_AVLTree->Data.NMapID;
	(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.NMapLayer;
	TmpCount++;
	//如果起始點資料和現在資料不完全相同
	if(!(p_AVLTree->Data.NNodeID==p_AVLTree->Data.FNodeID && p_AVLTree->Data.NRtBID==p_AVLTree->Data.FRtBID && p_AVLTree->Data.NMapID==p_AVLTree->Data.FMapID))
	{
		while(nuTRUE)
		{//在這裡會把NodeID由大到小依序排好(資料回推的動作)
			m_ApprocahAStarData.NMapID=p_AVLTree->Data.FMapID;
			m_ApprocahAStarData.NRtBID=p_AVLTree->Data.FRtBID;
			m_ApprocahAStarData.NNodeID=p_AVLTree->Data.FNodeID;
			m_ApprocahAStarData.NMapLayer=p_AVLTree->Data.FMapLayer;
			m_ApprocahAStarData.TMapID=p_AVLTree->Data.NMapID;
			m_ApprocahAStarData.TRtBID=p_AVLTree->Data.NRtBID;
			m_ApprocahAStarData.TNodeID=p_AVLTree->Data.NNodeID;
			m_ApprocahAStarData.TMapLayer=p_AVLTree->Data.NMapLayer;
		#ifdef NT_TO_TT
			m_ApprocahAStarData.FNodeID = p_AVLTree->Data.FFNodeID;
			m_ApprocahAStarData.FMapID = p_AVLTree->Data.FFMapID;
			m_ApprocahAStarData.FRtBID = p_AVLTree->Data.FFRtBID;
			m_ApprocahAStarData.FMapLayer = p_AVLTree->Data.FFMapLayer;
			m_ApprocahAStarData.RDis = 1;
		#endif
			if(!m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer)){//<-從樹中找到資料，失敗表示樹有問題
				g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	TmpList=NULL;	return nuFALSE;
			}

			if(CheckCount<=TmpCount){//複製資料的總數要小於原有資料總數
				return nuFALSE;
			}
			//將資料複製到NodeIDlist裡
			(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.NNodeID;
			(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.NRtBID;
			(*TmpList)[TmpCount].MapID=p_AVLTree->Data.NMapID;
			(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.NMapLayer;
//			(*TmpList)[TmpCount].RunDis=p_AVLTree->Data.RealDis;

			TmpCount++;
			
			if(	(p_AVLTree->Data.FNodeID==p_AVLTree->Data.NNodeID && p_AVLTree->Data.FRtBID==p_AVLTree->Data.NRtBID && p_AVLTree->Data.FMapID==p_AVLTree->Data.NMapID  && p_AVLTree->Data.FMapLayer==p_AVLTree->Data.NMapLayer)	)
				break;	//如果起始點的資訊和現在的資訊相同，表示不正常
		}
	}
	else
	{
		if(CLRS_F.FixDis1>CLRS_F.FixDis2 && CLRS_F.PtLocalRoute.FixNode1==CLRS_T.PtLocalRoute.FixNode1 && CLRS_F.FixDis1>CLRS_T.FixDis1){
			(*TmpList)[2]=(*TmpList)[1];
			(*TmpList)[1]=(*TmpList)[0];
			(*TmpList)[0]=(*TmpList)[2];
		}
		/*else if(CLRS_F.FixDis1<CLRS_F.FixDis2 && CLRS_F.PtLocalRoute.FixNode1==CLRS_T.PtLocalRoute.FixNode1 && CLRS_F.FixDis2>CLRS_T.FixDis2){
			(*TmpList)[2]=(*TmpList)[1];
			(*TmpList)[1]=(*TmpList)[0];
			(*TmpList)[0]=(*TmpList)[2];
		}*/
	}

	//重新整理完畢之後 還需要將所有資料經過一次判斷 將跨縣市的部分分解開來
	New_TmpCount=0;
	for(i=1;i<TmpCount;i++){	
		if((*TmpList)[i].MapLayer==(*TmpList)[i-1].MapLayer){
			New_TmpCount++;//計算同個圖層的Node數量
		}
	}

	if(nuFALSE==Tmp_Approach.CreateApproach(New_TmpCount))//建造p_Approach的結構
	{	g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	(*TmpList)=NULL;	return nuFALSE;	}

	tmpp_Approach=Tmp_Approach.p_Approach;

	New_TmpCount=0;
	for(i=1;i<TmpCount;i++)
	{
		if((*TmpList)[TmpCount-i].MapLayer==(*TmpList)[TmpCount-i-1].MapLayer)
		{
			(*tmpp_Approach)[New_TmpCount].FMapID		=	(*TmpList)[TmpCount-i].MapID;
			(*tmpp_Approach)[New_TmpCount].FMapLayer	=	(*TmpList)[TmpCount-i].MapLayer;
			(*tmpp_Approach)[New_TmpCount].FRtBID		=	(*TmpList)[TmpCount-i].RtBID;
			(*tmpp_Approach)[New_TmpCount].FNodeID		=	(*TmpList)[TmpCount-i].NodeID;
			(*tmpp_Approach)[New_TmpCount].TMapID		=	(*TmpList)[TmpCount-i-1].MapID;
			(*tmpp_Approach)[New_TmpCount].TMapLayer	=	(*TmpList)[TmpCount-i-1].MapLayer;
			(*tmpp_Approach)[New_TmpCount].TRtBID		=	(*TmpList)[TmpCount-i-1].RtBID;
			(*tmpp_Approach)[New_TmpCount].TNodeID		=	(*TmpList)[TmpCount-i-1].NodeID;
			New_TmpCount++;
		}
	}

	for(i=0;i<EachTimeJoinCount;i++)//這個的用意是在使用者不想走之前規劃好的路，必須重新規劃，表示前方可能塞車
	{
		if((TmpCount-2)<=i) break;
		g_pCDTH->JoinNewDontTo((*tmpp_Approach)[i].FMapID,(*tmpp_Approach)[i].FRtBID,(*tmpp_Approach)[i].FNodeID,i+1);
	}
	//資料已經存在於tmpp_Approach
	// 將過程資料移除
	if(TmpList!=NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);
		TmpList=NULL;
	}
	TmpCount=0;
	return nuTRUE;
}

/*** 透過起訖點NodeID找出該路段在Dw中的BlockIdx與RoadIdx ***/
nuBOOL Class_CalRouting::GetBlockRoadIdx(nuUSHORT *BlockIdx,nuUSHORT *RoadIdx,nuULONG FNodeID,nuULONG TNodeID,nuLONG *Idx,UONERtBCtrl *TmpRtBCtrl,nuUSHORT dontfindB1,nuUSHORT dontfindR1,nuUSHORT dontfindB2,nuUSHORT dontfindR2)
{
	/*** 使用二分法 對索引表進行兩次二分搜尋法 找出對應的數值 如果沒有搜尋到 回傳nuFALSE並將數值初始化 ***/
	*BlockIdx = _MAXUNSHORT;
	*RoadIdx  = _MAXUNSHORT;
	*Idx=-1;
	nuLONG m_Max,m_Min,m_Mid;
	m_Min=-1;
	m_Max=TmpRtBCtrl->RtBHeader.TotalRoadCount;
	m_Mid=(m_Max+m_Min)/2;

	//只要能以快速的方式尋找 並排除掉不可用的點 那就是好方法
	while(nuTRUE)
	{
		if(FNodeID>(*TmpRtBCtrl->RtBIdx)[m_Mid].FNodeID)
		{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else if(FNodeID<(*TmpRtBCtrl->RtBIdx)[m_Mid].FNodeID)
		{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else
		{
			if(TNodeID>(*TmpRtBCtrl->RtBIdx)[m_Mid].TNodeID)
			{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else if(TNodeID<(*TmpRtBCtrl->RtBIdx)[m_Mid].TNodeID)
			{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else
			{
				if(dontfindB1==(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx && dontfindR1==(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx)
				{	m_Mid=m_Min+1;	m_Min=m_Min+1;	continue;	}
				if(dontfindB2==(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx && dontfindR2==(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx)
				{	m_Mid=m_Min+1;	m_Min=m_Min+1;	continue;	}
				*BlockIdx=(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx;
				*RoadIdx=(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx;
				*Idx=m_Mid;
				return nuTRUE;
			}
		}
		if((m_Max-m_Min)<=1) break;
	}

	m_Min=-1;//將終點和起點做比較(方向相反)
	m_Max=TmpRtBCtrl->RtBHeader.TotalRoadCount;
	m_Mid=(m_Max+m_Min)/2;
	while(nuTRUE)
	{
		if(TNodeID>(*TmpRtBCtrl->RtBIdx)[m_Mid].FNodeID)
		{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else if(TNodeID<(*TmpRtBCtrl->RtBIdx)[m_Mid].FNodeID)
		{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else
		{
			if(FNodeID>(*TmpRtBCtrl->RtBIdx)[m_Mid].TNodeID)
			{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else if(FNodeID<(*TmpRtBCtrl->RtBIdx)[m_Mid].TNodeID)
			{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else
			{
				if(dontfindB1==(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx && dontfindR1==(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx)
				{	m_Mid=m_Min+1;	m_Min=m_Min+1;	continue;	}
				if(dontfindB2==(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx && dontfindR2==(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx)
				{	m_Mid=m_Min+1;	m_Min=m_Min+1;	continue;	}
				*BlockIdx=(*TmpRtBCtrl->RtBIdx)[m_Mid].BlockIdx;
				*RoadIdx=(*TmpRtBCtrl->RtBIdx)[m_Mid].RoadIdx;
				*Idx=m_Mid;
				return nuTRUE;
			}
		}
		if((m_Max-m_Min)<=1) break;
	}
	return nuFALSE;
}

nuLONG Class_CalRouting::GetAngle(NUROPOINT FPt,NUROPOINT NPt,NUROPOINT TPt)
{
	nuLONG  ct,ct1,ct2;
	ct1=nulAtan2(FPt.y-NPt.y,FPt.x-NPt.x);
	ct2=nulAtan2(NPt.y-TPt.y,NPt.x-TPt.x);

	//夾角為
	ct = ct2-ct1;
//	ct = ct1-ct2;
	if(ct>180){
		ct=ct-360;
	}
	else if(ct<(-180)){
		ct=ct+360;
	}
	return ct;
}

	nuLONG Class_CalRouting::UONEGetGuessDistance2(NUROPOINT Pt1,NUROPOINT PT2)//計算預估距離(現在公式與計算距離相同 如果有更好的預估公式 那當然就改變計算的方式)
	{
/*
		double aaa;
		nuLONG X,Y;
		X=NURO_ABS(Pt1.x-PT2.x)*AveLatitude_l/nuGetlBase();
		Y=NURO_ABS(Pt1.y-PT2.y)*1109/ _LONLATBASE;
		aaa=(double)X*(double)X+(double)Y*(double)Y;
		return (nuLONG)nuSqrt(aaa);

*/
		nuLONG X,Y;
		X = NURO_ABS(Pt1.x - PT2.x) * AveLatitude_l / nuGetlBase();
		Y = NURO_ABS(Pt1.y - PT2.y) * 1109 / _LONLATBASE;
		return nulSqrt(X,Y);
	}

nuLONG Class_CalRouting::GetDistance(NUROPOINT Pt1,NUROPOINT PT2)
{
	if(Pt1.x == PT2.x && Pt1.y == PT2.y) return 0;
	nuDOUBLE X,Y;
	X = NURO_ABS(Pt1.x-PT2.x) * nulCos(NURO_ABS((Pt1.y+PT2.y)/2) / _EACHDEGREECOORSIZE) / nuGetlBase() * 1113 / 1000;
	if(X < 1)
	{
		X = 1;
	}
	Y = NURO_ABS(Pt1.y-PT2.y) * 1109 / _LONLATBASE;
	if(Y < 1)
	{
		Y = 1;
	}
	return nulSqrt(X,Y);
}

nuBOOL Class_CalRouting::GetNextCrossData(PTFIXTOROADSTRU nowMkPt,NUROPOINT TNodeCoor,CROSSFOLLOWED **stru_Cross)
{
	#ifdef VALUE_EMGRT
		*stru_Cross=NULL;
		return nuFALSE;
	#else
		//讀取RTB資料
		//比對RT Index的區塊 確認當前道路的兩端點 並依照行車方向判斷應該往哪個方向找尋 下一路口以及其相關連點(同時考慮道路等級)
		//依此類推 不過若前後點距離太近 就要再往前尋找新點(同路口的排除)
		//填充資料
		nuULONG i,j,k;
		nuLONG ElementCrossCount;
		nuLONG tmpNodeIDIdx;
		UONERtCtrl TmpRtCtrl;
		TmpRoutingInfo	CLRS_F;//起訖點整理過後的資料存放區

		//清除資料
		ReleaseNextCrossData();

		*stru_Cross=NULL;

		//取得MapID跟MapIdx的對照表
		if(nuFALSE==InitMapIDtoMapIdx()){		LeaveGetNextCrossData();	return nuFALSE;	}
		/*** 將FixInfo的資訊添加在規劃專用起訖點結構 ***/
		CLRS_F.PtLocalRoute = nowMkPt;
		/*** 開啟Rt檔 比對MapID RTBID以取得MApIdx RtBIdx ***/
		for(i=0;i<m_MapIDtoMapIdx.MapCount;i++){
			if( (*m_MapIDtoMapIdx.MapIDList)[i]!=nowMkPt.MapID ) continue;
			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
			g_pRtFSApi->FS_FindFileWholePath(i,TmpRtCtrl.FName,NURO_MAX_PATH);
			if(nuFALSE==TmpRtCtrl.Read_Header())
			{
				Renum_RoutingState=ROUTE_READRTFALSE;
				TmpRtCtrl.ReSet();
				LeaveGetNextCrossData();
				return nuFALSE;
			}//讀取RT的Header
			for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++){
				if((*m_MapIDtoMapIdx.MapIDList)[i]==nowMkPt.MapID && (*TmpRtCtrl.Rt_Block_ID_List)[j]==nowMkPt.RTBID)
				{	CLRS_F.MapIdx = (*m_MapIDtoMapIdx.MapIdxList)[i];	CLRS_F.RtBIdx=j;	}
			}
			TmpRtCtrl.ReSet();
		}

		// 讀取規劃用資料 
		m_pGThr[0]= new Class_ThrCtrl;
		(m_pGThr[0])->InitClass(CLRS_F.MapIdx,0);
		if(nuFALSE==(m_pGThr[0])->Read_Header())
		{
			ReMoveBaseData();
			m_pThr_F=NULL;	
			m_pThr_T=NULL;	
			LeaveGetNextCrossData();
			return nuFALSE;
		}
		else
		{
			m_pThr_F=m_pGThr[0];
			m_pThr_T=m_pGThr[0];
		}

		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		g_pLocalRtB[0]->InitClass();
		nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,g_pLocalRtB[0]->FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	LeaveGetNextCrossData();	return nuFALSE;	}
		for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++){
			if((*TmpRtCtrl.Rt_Block_ID_List)[i]==CLRS_F.PtLocalRoute.RTBID){
				(g_pLocalRtB[0])->MyState.MapID=TmpRtCtrl.MapID;
				(g_pLocalRtB[0])->MyState.MapIdx=CLRS_F.MapIdx;
				(g_pLocalRtB[0])->MyState.RtBlockID=(*TmpRtCtrl.Rt_Block_ID_List)[i];
				(g_pLocalRtB[0])->MyState.RtBlockIdx=i;
				(g_pLocalRtB[0])->OrgCoor=(*TmpRtCtrl.RT_Block_Coor)[i].ORG_Coor;
				if(nuFALSE==(g_pLocalRtB[0])->Read_Header((*TmpRtCtrl.Rt_Block_Addr_List)[i])){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	LeaveGetNextCrossData();	return nuFALSE;	}
				if(nuFALSE==(g_pLocalRtB[0])->Read_MainData()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	LeaveGetNextCrossData();	return nuFALSE;	}
				if(nuFALSE==(g_pLocalRtB[0])->Read_Connect()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	LeaveGetNextCrossData();	return nuFALSE;	}
				if(nuFALSE==(g_pLocalRtB[0])->Read_Index()){	ReMoveBaseData();	Renum_RoutingState=ROUTE_READSRTFALSE;	LeaveGetNextCrossData();	return nuFALSE;	}
			}
		}
		TmpRtCtrl.ReSet();
		g_pRtB_F=g_pLocalRtB[0];
		g_pRtB_T=g_pLocalRtB[0];

		NUROPOINT dpt,dpt2;
		ApproachStru	SaveData[MaxNextCrossCount];
		nuLONG DifAng,SaveDataIdx;
		nuLONG StartMapID,StartRTBID,StartNodeID,TargetMapID,TargetRTBID,TargetNodeID;
		nuBOOL bhavenew,b_thrdown;

		SaveDataIdx=0;
		dpt.x=((g_pRtB_F)->OrgCoor.x+(*(g_pRtB_F)->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].NodeCoor.x);
		dpt.y=((g_pRtB_F)->OrgCoor.y+(*(g_pRtB_F)->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode2].NodeCoor.y);
		dpt2.x=nowMkPt.FixCoor.x;
		dpt2.y=nowMkPt.FixCoor.y;
		DifAng=CarAngle-nulAtan2(dpt.y-dpt2.y,dpt.x-dpt2.x);

		dpt.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].NodeCoor.x);
		dpt.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[CLRS_F.PtLocalRoute.FixNode1].NodeCoor.y);
		dpt2.x=nowMkPt.FixCoor.x;
		dpt2.y=nowMkPt.FixCoor.y;
		DifAng=CarAngle-nulAtan2(dpt.y-dpt2.y,dpt.x-dpt2.x);
		if(90<DifAng && DifAng<270){
			StartMapID=CLRS_F.PtLocalRoute.MapID;
			StartRTBID=CLRS_F.PtLocalRoute.RTBID;
			StartNodeID=CLRS_F.PtLocalRoute.FixNode1;
			TargetMapID=CLRS_F.PtLocalRoute.MapID;
			TargetRTBID=CLRS_F.PtLocalRoute.RTBID;
			TargetNodeID=CLRS_F.PtLocalRoute.FixNode2;
		}
		else if(-270<DifAng && DifAng<-90){	
			StartMapID=CLRS_F.PtLocalRoute.MapID;
			StartRTBID=CLRS_F.PtLocalRoute.RTBID;
			StartNodeID=CLRS_F.PtLocalRoute.FixNode1;
			TargetMapID=CLRS_F.PtLocalRoute.MapID;
			TargetRTBID=CLRS_F.PtLocalRoute.RTBID;
			TargetNodeID=CLRS_F.PtLocalRoute.FixNode2;
		}
		else{
			StartMapID=CLRS_F.PtLocalRoute.MapID;
			StartRTBID=CLRS_F.PtLocalRoute.RTBID;
			StartNodeID=CLRS_F.PtLocalRoute.FixNode2;
			TargetMapID=CLRS_F.PtLocalRoute.MapID;
			TargetRTBID=CLRS_F.PtLocalRoute.RTBID;
			TargetNodeID=CLRS_F.PtLocalRoute.FixNode1;
		}

		SaveData[SaveDataIdx].FMapID=StartMapID;
		SaveData[SaveDataIdx].FRtBID=StartRTBID;
		SaveData[SaveDataIdx].FNodeID=StartNodeID;
		SaveData[SaveDataIdx].TMapID=TargetMapID;
		SaveData[SaveDataIdx].TRtBID=TargetRTBID;
		SaveData[SaveDataIdx].TNodeID=TargetNodeID;
		SaveData[SaveDataIdx].Reserval=12;
		SaveDataIdx++;

		s_SaveCross.pRdSegList = new ROADSEGDATA[MaxNextCrossCount];
		b_thrdown=nuFALSE;
		while(nuTRUE){
			if(!b_thrdown){//填資料進入岔路結構
				s_SaveCross.nRdSegCount=SaveDataIdx;
				s_SaveCross.pRdSegList[SaveDataIdx-1].nMapIdx=SaveData[SaveDataIdx-1].TMapID;
				s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount=0;

				s_SaveCross.pRdSegList[SaveDataIdx-1].RoadLength=0;
				s_SaveCross.pRdSegList[SaveDataIdx-1].RoadVoiceType=0;
				s_SaveCross.pRdSegList[SaveDataIdx-1].RoadClass=0;
				s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross=NULL;

				for(k=0;k<(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectCount;k++){
					TargetRTBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].TRtBID;
					TargetNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].TNodeID;
					if(TargetRTBID==_THRRTBID)
					{//如果已經是可以跨界了 那就表示是邊界點 就不需要再考慮其他的問題了 直接跨過去另一區
					}
					else{
						if(TargetNodeID!=SaveData[SaveDataIdx-1].TNodeID)	continue;
						s_SaveCross.pRdSegList[SaveDataIdx-1].RoadLength=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].RoadLength;
						s_SaveCross.pRdSegList[SaveDataIdx-1].RoadVoiceType=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].VoiceRoadType;
						s_SaveCross.pRdSegList[SaveDataIdx-1].RoadClass=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].RoadClass;
						break;
					}
				}
				if(s_SaveCross.pRdSegList[SaveDataIdx-1].RoadLength==0){
					StartNodeID=SaveData[SaveDataIdx-1].FNodeID;
					TargetNodeID=SaveData[SaveDataIdx-1].TNodeID;
					SaveData[SaveDataIdx-1].FNodeID=TargetNodeID;
					SaveData[SaveDataIdx-1].TNodeID=StartNodeID;
					for(k=0;k<(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectCount;k++){
						TargetRTBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].TRtBID;
						TargetNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].TNodeID;
						if(TargetRTBID == _THRRTBID){//如果已經是可以跨界了 那就表示是邊界點 就不需要再考慮其他的問題了 直接跨過去另一區
						}
						else
						{
							if(TargetNodeID!=SaveData[SaveDataIdx-1].TNodeID)	continue;
							s_SaveCross.pRdSegList[SaveDataIdx-1].RoadLength=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].RoadLength;
							s_SaveCross.pRdSegList[SaveDataIdx-1].RoadVoiceType=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].VoiceRoadType;
							s_SaveCross.pRdSegList[SaveDataIdx-1].RoadClass=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].ConnectStruIndex+k-1].RoadClass;
							break;
						}
					}
				}
				ElementCrossCount=0;
				for(j=0;j<g_pRtB_F->RtBHeader.TotalRoadCount;j++){
					if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID && SaveData[SaveDataIdx-1].FNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID){
						s_SaveCross.pRdSegList[SaveDataIdx-1].BlockIdx=(*g_pRtB_F->RtBIdx)[j].BlockIdx;
						s_SaveCross.pRdSegList[SaveDataIdx-1].RoadIdx=(*g_pRtB_F->RtBIdx)[j].RoadIdx;
					}
					else if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID && SaveData[SaveDataIdx-1].FNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID){
						s_SaveCross.pRdSegList[SaveDataIdx-1].BlockIdx=(*g_pRtB_F->RtBIdx)[j].BlockIdx;
						s_SaveCross.pRdSegList[SaveDataIdx-1].RoadIdx=(*g_pRtB_F->RtBIdx)[j].RoadIdx;
					}
					else{
						if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID)	s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount++;	
						if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID)	s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount++;
					}
				}

				s_SaveCross.pRdSegList[SaveDataIdx-1].EndCoor.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].NodeCoor.x);
				s_SaveCross.pRdSegList[SaveDataIdx-1].EndCoor.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].NodeCoor.y);

				dpt.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].NodeCoor.x);
				dpt.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].FNodeID].NodeCoor.y);

				if(s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount>0){
					s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross = new ROADCROSSDATA[s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount];
					for(j=0;j<g_pRtB_F->RtBHeader.TotalRoadCount;j++){
						if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID && SaveData[SaveDataIdx-1].FNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID){
						}
						else if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID && SaveData[SaveDataIdx-1].FNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID){
						}
						else{
							//可通行的才會視為岔路
							if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].TNodeID){//處理MapIdx,BlockIdx,RoadIdx
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].nMapIdx=SaveData[SaveDataIdx-1].TMapID;
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].BlockIdx=(*g_pRtB_F->RtBIdx)[j].BlockIdx;
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadIdx=(*g_pRtB_F->RtBIdx)[j].RoadIdx;
								//
								for(k=0;k<(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].TNodeID].ConnectCount;k++){
									TargetRTBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].TNodeID].ConnectStruIndex+k-1].TRtBID;
									TargetNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].TNodeID].ConnectStruIndex+k-1].TNodeID;
									/*if(TargetRTBID==THRRTBID){
									}*/
									if(TargetRTBID != _THRRTBID)//如果已經是可以跨界了 那就表示是邊界點 就不需要再考慮其他的問題了 直接跨過去另一區
									{
										if(TargetNodeID!=(*g_pRtB_F->RtBIdx)[j].FNodeID)
										{
											continue;
										}
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadVoiceType=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].TNodeID].ConnectStruIndex+k-1].VoiceRoadType;
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadClass=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].TNodeID].ConnectStruIndex+k-1].RoadClass;
										dpt2.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.x);
										dpt2.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.y);
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadAngle=GetAngle(dpt,s_SaveCross.pRdSegList[SaveDataIdx-1].EndCoor,dpt2);
										ElementCrossCount++;
										break;
									}
								}
								s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount=ElementCrossCount;
							}
							if(SaveData[SaveDataIdx-1].TNodeID==(*g_pRtB_F->RtBIdx)[j].FNodeID){
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].nMapIdx=SaveData[SaveDataIdx-1].TMapID;
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].BlockIdx=(*g_pRtB_F->RtBIdx)[j].BlockIdx;
								s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadIdx=(*g_pRtB_F->RtBIdx)[j].RoadIdx;
								for(k=0;k<(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].FNodeID].ConnectCount;k++){
									TargetRTBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].FNodeID].ConnectStruIndex+k-1].TRtBID;
									TargetNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].FNodeID].ConnectStruIndex+k-1].TNodeID;
									if(TargetRTBID == _THRRTBID){//如果已經是可以跨界了 那就表示是邊界點 就不需要再考慮其他的問題了 直接跨過去另一區
									}
									else{
										if(TargetNodeID!=(*g_pRtB_F->RtBIdx)[j].TNodeID)	continue;
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadVoiceType=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[(*g_pRtB_F->RtBIdx)[j].FNodeID].ConnectStruIndex+k-1].VoiceRoadType;
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadClass=(*g_pRtB_F->RtBConnect)[(*(g_pRtB_F->RtBMain))[(*g_pRtB_F->RtBIdx)[j].FNodeID].ConnectStruIndex+k-1].RoadClass;
										dpt2.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.x);
										dpt2.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.y);
										s_SaveCross.pRdSegList[SaveDataIdx-1].pRdCross[ElementCrossCount].RoadAngle=GetAngle(dpt,s_SaveCross.pRdSegList[SaveDataIdx-1].EndCoor,dpt2);
										ElementCrossCount++;
										break;
									}
								}
								s_SaveCross.pRdSegList[SaveDataIdx-1].nCrossCount=ElementCrossCount;
							}
						}
					}
				}
			}

			if(SaveDataIdx>=MaxNextCrossCount) break;
			bhavenew=nuFALSE;
			if(b_thrdown) b_thrdown=nuFALSE;
			for(i=0;i<(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectCount;i++){
				if(b_thrdown==nuTRUE) break;

				TargetMapID=SaveData[SaveDataIdx-1].TMapID;
				TargetRTBID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].TRtBID;
				TargetNodeID=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].TNodeID;
				
				if(TargetRTBID == _THRRTBID)
				{
					if(SaveData[SaveDataIdx-1].FRtBID!=SaveData[SaveDataIdx-1].TRtBID) continue;
					//如果已經是可以跨界了 那就表示是邊界點 就不需要再考慮其他的問題了 直接跨過去另一區
					for(j=0;j<(*m_pThr_F->pThrEachMap.ThrMain)[TargetNodeID].ConnectCount;j++){
						tmpNodeIDIdx=(*m_pThr_F->pThrEachMap.ThrMain)[TargetNodeID].ConnectStruIndex+j;
						
						if((*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TMapLayer==1 && (*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID!=g_pRtB_T->MyState.RtBlockID){
							b_thrdown=nuTRUE;
							SaveData[SaveDataIdx-1].TMapID=m_pThr_F->ThrHeader.MapID;
							SaveData[SaveDataIdx-1].TRtBID=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TRtBID;
							SaveData[SaveDataIdx-1].TNodeID=(*m_pThr_F->pThrEachMap.ThrConnect)[tmpNodeIDIdx].TNodeID;
							
							//重新掛資料
							ReMoveSysRTData();
							TmpRtCtrl.InitClass();
							nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
							g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
							g_pLocalRtB[0]->InitClass();
							nuTcscpy(g_pLocalRtB[0]->FName, nuTEXT(".rt"));
							g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx,g_pLocalRtB[0]->FName,NURO_MAX_PATH);
							if(nuFALSE==TmpRtCtrl.Read_Header())
							{
								ReMoveBaseData();
								Renum_RoutingState=ROUTE_READSRTFALSE;
								LeaveGetNextCrossData();
								return nuFALSE;
							}
							for(k=0;k<TmpRtCtrl.Rt_Block_Count;k++){
								if((*TmpRtCtrl.Rt_Block_ID_List)[k]==SaveData[SaveDataIdx-1].TRtBID){
									(g_pLocalRtB[0])->MyState.MapID=TmpRtCtrl.MapID;
									(g_pLocalRtB[0])->MyState.MapIdx=CLRS_F.MapIdx;
									(g_pLocalRtB[0])->MyState.RtBlockID=(*TmpRtCtrl.Rt_Block_ID_List)[k];
									(g_pLocalRtB[0])->MyState.RtBlockIdx=k;
									(g_pLocalRtB[0])->OrgCoor=(*TmpRtCtrl.RT_Block_Coor)[k].ORG_Coor;
									if(nuFALSE==(g_pLocalRtB[0])->Read_Header((*TmpRtCtrl.Rt_Block_Addr_List)[k]))
									{
										ReMoveBaseData();
										Renum_RoutingState=ROUTE_READSRTFALSE;	
										LeaveGetNextCrossData();
										return nuFALSE;
									}
									if(nuFALSE==(g_pLocalRtB[0])->Read_MainData())
									{
										ReMoveBaseData();
										Renum_RoutingState=ROUTE_READSRTFALSE;
										LeaveGetNextCrossData();
										return nuFALSE;
									}
									if(nuFALSE==(g_pLocalRtB[0])->Read_Connect())
									{
										ReMoveBaseData();
										Renum_RoutingState=ROUTE_READSRTFALSE;	
										LeaveGetNextCrossData();
										return nuFALSE;
									}
									if(nuFALSE==(g_pLocalRtB[0])->Read_Index())
									{
										ReMoveBaseData();
										Renum_RoutingState=ROUTE_READSRTFALSE;
										LeaveGetNextCrossData();
										return nuFALSE;
									}
								}
							}
							TmpRtCtrl.ReSet();
							g_pRtB_F=g_pLocalRtB[0];
							g_pRtB_T=g_pLocalRtB[0];

							break;
						}
						if(j==((*m_pThr_F->pThrEachMap.ThrMain)[TargetNodeID].ConnectCount-1)){
							b_thrdown=nuFALSE;
							bhavenew=nuFALSE;
						}
					}
				}
				else{
					if(TargetNodeID==SaveData[SaveDataIdx].FNodeID)	continue;
					dpt.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.x);
					dpt.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[TargetNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[TargetNodeID].NodeCoor.y);
					dpt2.x=(g_pRtB_F->OrgCoor.x+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].CoorLVX*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].NodeCoor.x);
					dpt2.y=(g_pRtB_F->OrgCoor.y+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].CoorLVY*_COORLEVELNUM+(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].NodeCoor.y);
					DifAng=CarAngle-nulAtan2(dpt.y-dpt2.y,dpt.x-dpt2.x);
					if(i==0){
						SaveData[SaveDataIdx].FMapID=SaveData[SaveDataIdx-1].TMapID;
						SaveData[SaveDataIdx].FRtBID=SaveData[SaveDataIdx-1].TRtBID;
						SaveData[SaveDataIdx].FNodeID=SaveData[SaveDataIdx-1].TNodeID;
						SaveData[SaveDataIdx].TMapID=TargetMapID;
						SaveData[SaveDataIdx].TRtBID=TargetRTBID;
						SaveData[SaveDataIdx].TNodeID=TargetNodeID;
						SaveData[SaveDataIdx].Reserval=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass;
						bhavenew=nuTRUE;
					}
					else{
						if(SaveData[SaveDataIdx].Reserval>=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass)
						{
							if(60<DifAng && DifAng<300){
								if(SaveData[SaveDataIdx].Reserval>(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass){
									SaveData[SaveDataIdx].FMapID=SaveData[SaveDataIdx-1].TMapID;
									SaveData[SaveDataIdx].FRtBID=SaveData[SaveDataIdx-1].TRtBID;
									SaveData[SaveDataIdx].FNodeID=SaveData[SaveDataIdx-1].TNodeID;
									SaveData[SaveDataIdx].TMapID=TargetMapID;
									SaveData[SaveDataIdx].TRtBID=TargetRTBID;
									SaveData[SaveDataIdx].TNodeID=TargetNodeID;
									SaveData[SaveDataIdx].Reserval=(*(g_pRtB_F->RtBConnect))[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass;
									bhavenew=nuTRUE;
								}
							}//上下兩個判別是幾乎都一樣，連角度的判斷都一樣
							else if(-300<DifAng && DifAng<-60){
								if(SaveData[SaveDataIdx].Reserval>(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass){
									SaveData[SaveDataIdx].FMapID=SaveData[SaveDataIdx-1].TMapID;
									SaveData[SaveDataIdx].FRtBID=SaveData[SaveDataIdx-1].TRtBID;
									SaveData[SaveDataIdx].FNodeID=SaveData[SaveDataIdx-1].TNodeID;
									SaveData[SaveDataIdx].TMapID=TargetMapID;
									SaveData[SaveDataIdx].TRtBID=TargetRTBID;
									SaveData[SaveDataIdx].TNodeID=TargetNodeID;
									SaveData[SaveDataIdx].Reserval=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass;
									bhavenew=nuTRUE;
								}
							}
							else{
								if(SaveData[SaveDataIdx].Reserval>=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass){
									SaveData[SaveDataIdx].FMapID=SaveData[SaveDataIdx-1].TMapID;
									SaveData[SaveDataIdx].FRtBID=SaveData[SaveDataIdx-1].TRtBID;
									SaveData[SaveDataIdx].FNodeID=SaveData[SaveDataIdx-1].TNodeID;
									SaveData[SaveDataIdx].TMapID=TargetMapID;
									SaveData[SaveDataIdx].TRtBID=TargetRTBID;
									SaveData[SaveDataIdx].TNodeID=TargetNodeID;
									SaveData[SaveDataIdx].Reserval=(*g_pRtB_F->RtBConnect)[(*g_pRtB_F->RtBMain)[SaveData[SaveDataIdx-1].TNodeID].ConnectStruIndex+i-1].RoadClass;
									bhavenew=nuTRUE;
								}
							}
						}
					}
				}
			}
			if(!b_thrdown){
				if(!bhavenew) break;
				SaveDataIdx++;
			}
		}


		for(i=0;i<SaveDataIdx;i++){
			for(j=0;j<s_SaveCross.pRdSegList[i].nCrossCount;j++){
				if(s_SaveCross.pRdSegList[i].RoadIdx==s_SaveCross.pRdSegList[i].pRdCross[j].RoadIdx){
					for(k=j+1;k<s_SaveCross.pRdSegList[i].nCrossCount;k++){
						s_SaveCross.pRdSegList[i].pRdCross[k-1]=s_SaveCross.pRdSegList[i].pRdCross[k];
					}
					s_SaveCross.pRdSegList[i].nCrossCount--;
					break;
				}
			}
			if(i<(SaveDataIdx-1)){
				for(j=0;j<s_SaveCross.pRdSegList[i].nCrossCount;j++){
					if(s_SaveCross.pRdSegList[i+1].RoadIdx==s_SaveCross.pRdSegList[i].pRdCross[j].RoadIdx){
						for(k=j+1;k<s_SaveCross.pRdSegList[i].nCrossCount;k++){
							s_SaveCross.pRdSegList[i].pRdCross[k-1]=s_SaveCross.pRdSegList[i].pRdCross[k];
						}
						s_SaveCross.pRdSegList[i].nCrossCount--;
						break;
					}
				}
			}
		}
		
		*stru_Cross=&s_SaveCross;

		LeaveGetNextCrossData();
		return nuTRUE;
	#endif
}

nuVOID Class_CalRouting::LeaveGetNextCrossData(){
	//清理規畫中所用到的Thr與RtB Class
		ReMoveBaseData();
	//移除MapID與Mapidx的對應
		ReleaseMapIDtoMapIdx();
}

nuBOOL Class_CalRouting::ReleaseNextCrossData(){
	nuDWORD i;
	if(s_SaveCross.pRdSegList){
		for(i=0;i<s_SaveCross.nRdSegCount;i++){
			if(s_SaveCross.pRdSegList[i].pRdCross){
				delete [] s_SaveCross.pRdSegList[i].pRdCross;
				s_SaveCross.pRdSegList[i].pRdCross=NULL;
			}
		}
		delete [] s_SaveCross.pRdSegList;
		s_SaveCross.pRdSegList=NULL;
		s_SaveCross.nRdSegCount=0;
	}
	return nuTRUE;
}
#ifdef _USERT6
nuBOOL Class_CalRouting::bCheckRT6(PNAVIRTBLOCKCLASS pNRBC)
{
	Class_ND6Ctrl cND6;
	nuBOOL bReadRt6 = nuFALSE;
	m_bReadRt6		= nuFALSE;
	PNAVIRTBLOCKCLASS pTmpNRBC = pNRBC;
	while (pTmpNRBC->NextRTB->NextRTB == NULL )
	{
		return nuFALSE;
	}
	cND6.InitClass();

	nuTcscpy(cND6.m_tsFName, nuTEXT(".ND6"));
	g_pRtFSApi->FS_FindFileWholePath(pNRBC->FInfo.MapIdx, cND6.m_tsFName, NURO_MAX_PATH);

	if(cND6.ReadFile())
	{
		bReadRt6 = cND6.MappingFile(pNRBC->FInfo.RTBID, pNRBC->FInfo.FixNode1, pNRBC->FInfo.FixCoor);
		if(bReadRt6)
		{
			m_bReadRt6 = cND6.MappingFile(pNRBC->TInfo.RTBID, pNRBC->TInfo.FixNode1, pNRBC->TInfo.FixCoor);
		}
	}
	cND6.CloseClass();
	return nuTRUE;
}
#endif

#ifdef THRNodeList	
	nuBOOL Class_CalRouting::GetTHRNodeIDList(nuLONG MAPID,nuLONG FNodeID, nuLONG TNodeID, nuLONG RTBID)
	{
		nuULONG i,CheckCount=0;
		nuULONG TmpCount,New_TmpCount;
		ApproachStru **tmpp_Approach;

		nuLONG *NodeList = NULL;
		Str_RTBHead	  RTBHead;

		nuFILE *THRFILE;
		nuTCHAR Path[100] = {0};
		
		nuGetModuleFileName(NULL, Path, NURO_MAX_PATH);
		for(i = nuTcslen(Path) - 1; i >= 0; i--)
		{
			if( Path[i] == nuTEXT('\\') )
			{
				Path[i+1] = nuTEXT('\0');
				break;
			}
		}
		nuTcscat( Path, nuTEXT("Map\\ThrNodeList.thr0"));//這裡要設定檔名
		
		THRFILE = nuTfopen(Path,NURO_FS_AB);
		if(THRFILE == NULL)
		{
			return nuFALSE;
		}
		if(0 != nuFseek(THRFILE,0,NURO_SEEK_SET))//僅讀取所需要的RTB資訊
		{
			return nuFALSE;
		} 
		if(1 != nuFread(&RTBHead.TotalNodeCount,sizeof(nuLONG),1,THRFILE))
		{
			return nuFALSE;
		}
		if(0 != nuFseek(THRFILE,RTBID*sizeof(Str_RTBIndex)+sizeof(nuLONG),NURO_SEEK_SET))//僅讀取所需要的RTB資訊
		{
			return nuFALSE;
		} 
		if(1 != nuFread(&RTBHead.RTBIndex,sizeof(Str_RTBIndex),1,THRFILE))
		{
			return nuFALSE;
		} 
		if(0 != nuFseek(THRFILE,(RTBHead.RTBIndex.RTBNodeIndex-1)*sizeof(Str_THRNodeID) + sizeof(Str_RTBIndex) * 17 + sizeof(nuLONG),NURO_SEEK_SET))
		{
			return nuFALSE;
		} 
		RTBHead.THRNodeID = (Str_THRNodeID**)g_pRtMMApi->MM_AllocMem(sizeof(Str_THRNodeID)*RTBHead.RTBIndex.RTBNodeCount);
		if(RTBHead.THRNodeID == NULL)
		{
			return nuFALSE;
		}
		nuMemset(*RTBHead.THRNodeID, 0 , sizeof(Str_THRNodeID) * RTBHead.RTBIndex.RTBNodeCount);
		if(RTBHead.RTBIndex.RTBNodeCount != nuFread(*RTBHead.THRNodeID,sizeof(Str_THRNodeID),RTBHead.RTBIndex.RTBNodeCount,THRFILE))
		{
			return nuFALSE;
		}
		
		nuLONG Max = RTBHead.RTBIndex.RTBNodeCount;
		nuLONG Min = 0;
		nuLONG Mid = 0;
		nuLONG tempindex = 0;
		nuLONG tempcount = 0;

		//比對Fnode，後比對Tnode
		while(Max >= Min)
		{ 
			Mid = (Max + Min) / 2;
			if(FNodeID > (*RTBHead.THRNodeID)[Mid].FNodeID)
			{
				Min = Mid+1;
			}
			else if(FNodeID < (*RTBHead.THRNodeID)[Mid].FNodeID)
			{
				Max = Mid-1;
			}
			else
			{
				if(TNodeID > (*RTBHead.THRNodeID)[Mid].TNodeID)
				{
					Min = Mid+1;
				}
				else if(TNodeID < (*RTBHead.THRNodeID)[Mid].TNodeID)
				{
					Max = Mid-1;
				}
				else
				{
					tempindex = (*RTBHead.THRNodeID)[Mid].NodeIDListIndex;
					tempcount = (*RTBHead.THRNodeID)[Mid].NodeIDListCount;
					if(RTBHead.THRNodeID  != NULL)
					{
						g_pRtMMApi->MM_FreeMem((nuPVOID*)RTBHead.THRNodeID);
						RTBHead.THRNodeID = NULL;
					}
					
					RTBHead.pNodeList = (Str_NODEIDLIST**)g_pRtMMApi->MM_AllocMem(sizeof(Str_NODEIDLIST)*tempcount);
					if(RTBHead.pNodeList == NULL)
					{
						return nuFALSE;
					}
					//讀取NodeList
					if(0 != nuFseek(THRFILE,tempindex*sizeof(Str_NODEIDLIST) + sizeof(nuLONG) +
						sizeof(Str_RTBIndex)*17 + 
						RTBHead.TotalNodeCount * sizeof(Str_THRNodeID),NURO_SEEK_SET))
					{
						return nuFALSE;
					}
					if(tempcount != nuFread(*RTBHead.pNodeList,sizeof(Str_NODEIDLIST),tempcount,THRFILE))
					{
						return nuFALSE;
					}
					break;
				}
			}
		}
		//檔案讀取完了，就可以開始存放Node資料了
		
		/*** 初始化存儲結構 ***/
		Tmp_Approach.ReleaseApproach();

		/*** 產生存放NodeList的過程結構 ***/		
		if(nuFALSE==Tmp_Approach.CreateApproach(tempcount))//建造p_Approach的結構
		{return nuFALSE;	}

		tmpp_Approach=Tmp_Approach.p_Approach;
		New_TmpCount = 0;
		for(i=0;i<tempcount;i++)
		{
			(*tmpp_Approach)[New_TmpCount].FMapID		=	MAPID;
			(*tmpp_Approach)[New_TmpCount].FMapLayer	=	0;
			(*tmpp_Approach)[New_TmpCount].FRtBID		=	RTBID;
			(*tmpp_Approach)[New_TmpCount].FNodeID		=	(*RTBHead.pNodeList)[i].Node;
			(*tmpp_Approach)[New_TmpCount].TMapID		=	MAPID;
			(*tmpp_Approach)[New_TmpCount].TMapLayer	=	0;
			(*tmpp_Approach)[New_TmpCount].TRtBID		=	RTBID;
			(*tmpp_Approach)[New_TmpCount].TNodeID		=	(*RTBHead.pNodeList)[i+1].Node;
			New_TmpCount++;
		}
		if(RTBHead.pNodeList != NULL)
		{
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RTBHead.pNodeList);
			RTBHead.pNodeList = NULL;
		}
		return nuTRUE;
	}
#endif
	nuVOID Class_CalRouting::RTSleepTimeCtrl(nuLONG lSleepTime)
	{
		if(lSleepTime <= 0 || m_nRTSleep <= 0)
		{
			return ;
		}
		nuSleep(m_nRTSleep);
	}
//#ifdef _USETTI
nuBOOL Class_CalRouting::ReadTTI(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T)
{	
	if(m_pTTI_F != NULL)
	{
		delete m_pTTI_F;
		m_pTTI_F = NULL;
	}
	if(m_pTTI_T != NULL)
	{
		delete m_pTTI_T;
		m_pTTI_T = NULL;
	} 
	if(b_LocalRut || (CLRS_F.MapIdx == CLRS_T.MapIdx && CLRS_F.RtBIdx == CLRS_T.RtBIdx))
	{
		m_pTTI_T = new Class_TTICtrl;
		if(m_pTTI_T == NULL)
		{
			return nuFALSE;
		}
		nuTcscpy(m_pTTI_T->m_tsFName, nuTEXT(".TTI"));
		g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx, m_pTTI_T->m_tsFName, NURO_MAX_PATH);
		m_pTTI_T->ReadFile();
	}
	else
	{
		m_pTTI_F = new Class_TTICtrl;
		if(m_pTTI_F == NULL)
		{
			return nuFALSE;
		}
		nuTcscpy(m_pTTI_F->m_tsFName, nuTEXT(".TTI"));
		g_pRtFSApi->FS_FindFileWholePath(CLRS_F.MapIdx, m_pTTI_F->m_tsFName, NURO_MAX_PATH);
		m_pTTI_F->ReadFile();

		m_pTTI_T = new Class_TTICtrl;
		if(m_pTTI_T == NULL)
		{
			return nuFALSE;
		}
		nuTcscpy(m_pTTI_T->m_tsFName, nuTEXT(".TTI"));
		g_pRtFSApi->FS_FindFileWholePath(CLRS_T.MapIdx, m_pTTI_T->m_tsFName, NURO_MAX_PATH);
		m_pTTI_T->ReadFile();
	}
	nuGetLocalTime(&m_NowTime);
	/*m_NowTime.nYear		 = g_pRtFSApi->pGdata->timerData.nYear;
	m_NowTime.nMonth	 = g_pRtFSApi->pGdata->timerData.nMonth;
	m_NowTime.nDay		 = g_pRtFSApi->pGdata->timerData.nDay ;
	m_NowTime.nDayOfWeek = g_pRtFSApi->pGdata->timerData.nDayOfWeek;
	m_NowTime.nHour		 = g_pRtFSApi->pGdata->timerData.nHour;
	m_NowTime.nMinute	 = g_pRtFSApi->pGdata->timerData.nMinute;
	m_NowTime.nSecond	 = g_pRtFSApi->pGdata->timerData.nSecond;*/
#ifdef _WIN32_WCE
	SYSTEMTIME st = {0}, st2={0};
	nuMemcpy(&st, &m_NowTime, sizeof(SYSTEMTIME));
	nuDOUBLE dt = 0;
	if ( SystemTimeToVariantTime(&st, &dt))
	{
		VariantTimeToSystemTime(dt, &st2);	
		nuMemcpy(&m_NowTime, &st2, sizeof(SYSTEMTIME));
	}
#endif
	switch(m_NowTime.nDayOfWeek)
	{
	case 0:
		m_NowTime.nDayOfWeek = _TTI_SUNDAY;
		break;
	case 1:
		m_NowTime.nDayOfWeek = _TTI_MONDAY;
		break;
	case 2:
		m_NowTime.nDayOfWeek = _TTI_TUESDAY;
		break;
	case 3:
		m_NowTime.nDayOfWeek = _TTI_WEDNESDAY;
		break;
	case 4:
		m_NowTime.nDayOfWeek = _TTI_THURSDAY;
		break;
	case 5:
		m_NowTime.nDayOfWeek = _TTI_FRIDAY;
		break;
	case 6:
		m_NowTime.nDayOfWeek = _TTI_SATURDAY;
		break;
	}
	return nuTRUE;
}
nuVOID Class_CalRouting::ReleaseTTI()
{
	if(m_pTTI_F != NULL)
	{
		delete m_pTTI_F;
		m_pTTI_F = NULL;
	}
	if(m_pTTI_T != NULL)
	{
		delete m_pTTI_T;
		m_pTTI_T = NULL;
	}	
}
nuBOOL Class_CalRouting::TTIPass(nuBOOL bTargetArea, nuLONG TTIAddr)
{
	Class_TTICtrl *m_pTTICtrl = NULL;
	if(bTargetArea)
	{
		m_pTTICtrl = m_pTTI_T;
	}
	else
	{
		m_pTTICtrl = m_pTTI_F;
	}
	if(TTIAddr == -1)
	{
		/*switch(g_pRtFSApi->pUserCfg->nRouteMode)
		{
			case 5:
			case 6:
			case 8:
			case 9:
				return nuTRUE;
		}*/
		return nuFALSE;
	}
	TTI_Data_Stru TTIData = {0};
	if(!m_pTTICtrl->GetTTIData(TTIAddr, TTIData))
	{
		return nuFALSE;
	}
#ifdef RCLASS_NAVIMODE
	if(!ModePass(g_RoutingRule, TTIData.CARTYPE))//剛好與擴散相反，路段是沒比對到就false　這裡是比對到就false
	{
		return nuTRUE;
	}
#endif
	if((TTIData.DAY & m_NowTime.nDayOfWeek) == 0)
	{
		return nuTRUE;
	}
	for(nuINT i = 0; i < _TTI_TIME_MAX; i++)
	{
		if(TTIData.TimeData[i].SDATE <= -1)
		{
			continue;
		}
		nuLONG Now_Day = m_NowTime.nMonth * 100 + m_NowTime.nDay;
		nuLONG Now_Time = m_NowTime.nHour * 100 + m_NowTime.nMinute;
		if((TTIData.TimeData[i].SDATE <= Now_Day && 
			TTIData.TimeData[i].EDATE >= Now_Day) &&
			(TTIData.TimeData[i].SHOUR <= Now_Time && 
			TTIData.TimeData[i].EHOUR >= Now_Time))
		{
			   if(!ModePass_KD(TTIData.CARTYPE))
               {
               		return nuFALSE;     
			   }
		}
	}
	return nuTRUE;
}
nuBOOL Class_CalRouting::ModePass_KD(nuSHORT Mode)
{
	if(Mode & 64 || Mode & 128 )
	{
		return nuFALSE;
	}
	else
	{
		return nuTRUE;
	}
}
//#endif
