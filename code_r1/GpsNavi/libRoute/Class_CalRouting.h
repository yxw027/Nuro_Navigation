// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2010/11/08 05:53:52 $
// $Revision: 1.14 $

#include "NuroClasses.h"

#ifndef Def_Class_CalRouting
	#define Def_Class_CalRouting
	#include "RouteBase.h"
	#include "Class_PtFixToRoad.h"

	#include "NuroEngineStructs.h"
	#include "AStar.h"
	#include "Class_Approach.h"
	#include "ConstSet.h"
	#include "Class_CalMRT.h"
	#include "Ctrl_ND6.h"
	#include "Ctrl_TTI.h"
	#include "Ctrl_CMP.h"
	
	typedef struct Tag_TmpRoutingInfo
	{
		PTFIXTOROADSTRU PtLocalRoute;
		nuLONG MapIdx;
		nuLONG RtBIdx;
		nuLONG FixDis1;
		nuLONG FixDis2;
	}TmpRoutingInfo,*pTmpRoutingInfo;

	class Class_CalRouting// : public MathTool
	{
		public:
			Class_CalRouting();
			virtual ~Class_CalRouting();
			nuBOOL InitClass();
			nuVOID CloseClass();
			//MapID與MapIdx的對應表相關函式
			nuBOOL InitMapIDtoMapIdx();//讀取MapID與MapIdx的對應表
			nuVOID ReleaseMapIDtoMapIdx();//移除MapId與MapIdx對應表所使用的記憶體

			nuLONG StartRouting( PTFIXTOROADSTRU &FPt, PTFIXTOROADSTRU &TPt, PNAVIFROMTOCLASS pNFTC, nuBOOL b_MakeThr, 
				                 nuLONG l_RoutingRule );//起訖點規劃
			
			//nuLONG LocalRouting(PNAVIRTBLOCKCLASS pNRBC,nuLONG l_RoutingRule);//RtB單區規劃
			nuLONG LocalRouting(PNAVIRTBLOCKCLASS pNRBC, nuLONG l_RoutingRule, nuLONG FMapIdx, nuLONG TMapIdx);
			nuVOID SetAstarBase(nuBOOL b_Local);
			nuBOOL GetMRouteData(PNAVIFROMTOCLASS pNFTC, nuLONG l_RoutingRule);

			nuBOOL GetNextCrossData(PTFIXTOROADSTRU nowMkPt, NUROPOINT TNodeCoor, CROSSFOLLOWED **stru_Cross);
			nuVOID LeaveGetNextCrossData();
			nuBOOL ReleaseNextCrossData();
			CROSSFOLLOWED s_SaveCross;

		private:
			//nuBOOL ColationDataAgain(PTFIXTOROADSTRU &FPt,PTFIXTOROADSTRU &TPt,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule);//由座標找出規劃所需基本資料
			nuBOOL ColationDataAgain( PTFIXTOROADSTRU &FPt, PTFIXTOROADSTRU &TPt, TmpRoutingInfo &CLRS_F,
				                      TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG l_RoutingRule, 
									  nuLONG FMapIdx, nuLONG TMapIdx );
			nuVOID ReleaseColationDataAgain(UONERtCtrl &TmpRtCtrl);//清除由CollationDataAgain所產生的記憶體
			nuVOID GetTwoSiteDis(UONERtBCtrl *TmpRtBCtrl,TmpRoutingInfo &CLRS);//從Road去取得最佳的起訖規劃NodeID
			nuBOOL ReadRtBIndex(UONERtBCtrl *TmpRtBCtrl);
			nuBOOL GetNaviInfo_FromRtFile(PNAVIRTBLOCKCLASS pNRBC,UONERtBCtrl *TmpRtBCtrl);//從Rt檔取得所需的規劃結果

			nuBOOL GetBlockRoadIdx( nuUSHORT *BlockIdx, nuUSHORT *RoadIdx, nuULONG FNodeID, nuULONG TNodeID, 
				                    nuLONG *Idx, UONERtBCtrl *TmpRtBCtrl, nuUSHORT dontfindB1, nuUSHORT dontfindR1,
									nuUSHORT dontfindB2, nuUSHORT dontfindR2 );//透過NodeID去取得所在的BlockID與RoadID
			
			nuLONG UONEGetGuessDistance2(NUROPOINT Pt1, NUROPOINT PT2);//計算預估距離(現在公式與計算距離相同 如果有更好的預估公式 那當然就改變計算的方式)
			nuBOOL GetWeighting(nuLONG Class);//取得道路加權值
			nuLONG GetDistance(NUROPOINT Pt1, NUROPOINT PT2);//計算距離
			nuLONG  AveLatitude_l;

			//nuVOID GetRouteNodeID(RtBCtrl &TmpRtBCtrl,TmpRoutingInfo &CLRS,nuBOOL b_StartNode);//從Road去取得最佳的起訖規劃NodeID
			nuBOOL GetNodeIDList(AStarWork &m_AStar,TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T);//從AStar的Close端取得最佳路徑的NodeID
			nuBOOL CreateRoutingData(PNAVIFROMTOCLASS TmpNaviFromToClass);//根據Approach的結果 先創立要儲存資料的結構
			nuBOOL CreateLocalRoutingData(PNAVIRTBLOCKCLASS TmpNaviRtBlockClass);//根據Approach的結果 先創立要儲存資料的結構
			nuBOOL AStarSuccessWork(PNAVIFROMTOCLASS pNFTC);//AStar規劃成功的後續資料整理
			nuBOOL LocalAStarSuccessWorkStep1(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL LocalAStarSuccessWorkStep2(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL LocalAStarSuccessWorkStep3(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL GetNaviInfo_FromDwFile(PNAVIRTBLOCKCLASS pNRBC);//從Dw檔取出所需的規劃結果
			nuLONG GetAngle(NUROPOINT FPt,NUROPOINT NPt,NUROPOINT TPt);//由起經訖點計算角度差
			nuBOOL ReColFTRoad(PNAVIRTBLOCKCLASS pNRBC);//重新整理頭尾資訊
			nuBOOL ColTurnFlag(PNAVIRTBLOCKCLASS pNRBC);//整理轉向旗標

			nuVOID LeaveStartRouting(AStarWork &m_AStar);//移除StartRouteing所使用的記憶體
			nuVOID LeaveLocalRouting(AStarWork &m_AStar);//移除LoaclRouting所使用的記憶體

			//Louis 20060720 
			nuBOOL FindNearEnd7Net( AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, 
				                    nuLONG &Dis_Rd7, nuULONG &LimitNodeID );
			nuBOOL PassNoTurn(nuBOOL b_TargetArea, AStarData &m_AStarData, AStarWork &m_AStar, TagAStarData &m_SAVLData);
			
			//added by Daniel for LCMM 20110614
			nuBOOL ReFindPointNodeID(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuBOOL b_Local);//重新找過屬於起訖點在道路兩端的Node
			nuBOOL CompareRTBIndex( PTFIXTOROADSTRU &FPt, PTFIXTOROADSTRU &TPt, TmpRoutingInfo &CLRS_F, 
				                    TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG FMapIdx, nuLONG TMapIdx );//找到RTBindex
			nuVOID LeaveStartRouting_ForMultiConditionRoute(AStarWork &m_AStar);
			nuVOID ReMoveThrData();

			nuVOID RTSleepTimeCtrl(nuLONG lSleepTime);
//#ifdef _USETTI
			nuVOID ReleaseTTI();
			nuBOOL ReadTTI(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T);
			nuBOOL TTIPass(nuBOOL bTargetArea, nuLONG TTIAddr);
			nuBOOL ModePass_KD(nuSHORT Mode);
//#endif

#ifdef _DDst
			nuBOOL Dstroadsituation(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar);
#endif
			
#ifdef _DTMC
			nuBOOL Tmcroadsituation(nuBOOL b_TargetArea,AStarData &m_AStarData,AStarWork &m_AStar);
#endif

#ifdef _DAR
			nuBOOL  NodeID_Compare(nuULONG FNodeID, nuULONG FRTBID, nuULONG *ReNodeID, nuULONG *ReRTBID);

			nuVOID  Astar_AR( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuLONG EndBoundaryRange, nuDWORD LimitNodeID_7, 
				              TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, 
							  TmpRoutingInfo &CLRS_T, WEIGHTINGSTRU BestChoose );
#endif
			nuVOID  Astar_SRT( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuLONG EndBoundaryRange, nuDWORD LimitNodeID_7, 
				               TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, 
							   nuBOOL &b_StartLevelCut,WEIGHTINGSTRU BestChoose,nuLONG l_RoutingRule );//規劃

			nuVOID  Astar_TRT( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuDWORD Run8E_Dis, nuLONG EndBoundaryRange, 
				               nuDWORD LimitNodeID_7, TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F,
							   TmpRoutingInfo &CLRS_T, nuBOOL &b_StartLevelCut, nuBOOL &b_CloseStart, 
							   WEIGHTINGSTRU BestChoose, nuLONG l_RoutingRule );//規劃

			nuVOID  Astar_STHR( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuLONG EndBoundaryRange, nuDWORD LimitNodeID_7,
				                TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, 
								TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose );//規劃

			nuVOID  Astar_TTHR( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuLONG EndBoundaryRange, nuDWORD LimitNodeID_7,
				                TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, 
								TmpRoutingInfo &CLRS_T,WEIGHTINGSTRU BestChoose );//規劃

			nuVOID  Astar_THC( nuBOOL &b_CloseThr, NUROPOINT TargetCoor, nuLONG EndBoundaryRange, nuDWORD LimitNodeID_7,
				               TagAStarData &m_SAVLData, AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, 
							   TmpRoutingInfo &CLRS_T, WEIGHTINGSTRU BestChoose );//規劃
		    #if (defined VALUE_EMGRT) || (defined ZENRIN)
				//處理大陸易圖通收費站資料
				nuBOOL ReadPTS(nuLONG FMapID);
				nuLONG CheckPTS(nuULONG RtBID,nuULONG NodeID);
				nuVOID ReMovePTS();
			#endif
#ifdef _USERT6
				nuBOOL bCheckRT6(PNAVIRTBLOCKCLASS pNRBC);
#endif
		public:
			//為了轉thr 特別開放出來的API
			nuBOOL ReadBaseData(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG l_RoutingRule);//讀取頭尾RtBlock與thr的資料

			//讀取禁轉檔(因為記憶體清除 跟創建時所需要的記憶體整理時間 在sunplus中會很久 因此我們要減少這種整理的動作)
			nuBOOL ReadSysNTData(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG l_RoutingRule);//讀取頭尾RtBlock與thr的資料
			//移除禁轉檔
			nuVOID ReMoveSysNTData();

			//讀取即時路況檔
#ifdef _DDst
			nuBOOL ReadSysDstData(TmpRoutingInfo &CLRS_F,TmpRoutingInfo &CLRS_T,nuBOOL b_Local,nuLONG l_RoutingRule);
			nuVOID ReMoveSysDstData();
#endif

#ifdef _DTMC
			nuBOOL ReadSysTmcData(TmpRoutingInfo &CLRS_T, TMC_ROUTER_DATA *TMCData, nuLONG TMCDataCount, nuBOOL *bTMCRTB);
			nuVOID ReMoveSysTmcData();
#endif	

#ifdef _DAR
			nuBOOL ReadSysARData(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG l_RoutingRule);
			nuVOID ReMoveSysARData();
			nuBOOL RT_Enter_AR();
#endif
			//起訖區域的RTB存取與釋放
			nuBOOL ReadSysRTData(TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuBOOL b_Local, nuLONG l_RoutingRule);//讀取頭尾RtBlock與thr的資料
			nuVOID ReMoveSysRTData();
			
			nuVOID ReMoveBaseData();//移除所創建的頭尾RtBlock與thr的資料
			nuBOOL AStar(AStarWork &m_AStar, TmpRoutingInfo &CLRS_F, TmpRoutingInfo &CLRS_T, nuLONG l_RoutingRule);//路徑規劃
			//UONERtBCtrl			LocalRtB[2];//起訖點RTBlock的資料暫存區
//			#ifdef DEF_MAKETHRROUTE
				nuLONG ThrGDis;
				nuLONG ThrRDis;
				nuLONG TargetClass;
//			#endif
			nuLONG CarAngle;
			Class_CalMRT	m_calmrt;

			nuBOOL b_ReRouting;
		private:
			nuLONG			ClassWeighting;//等級權值(分子)
			//AStarWork		m_AStar;//AStar運算 核心物件

			Class_Approach	Tmp_Approach;//Astar規畫完之後留下來的資料(approach結束)
			//Louis 20060720 將權值等屬性 移往外部處理 內部針對外部設定檔的設定來動作

			AStarData		m_ApprocahAStarData;
			nuLONG Renum_RoutingState;

			NUROPOINT Temp_S_DiffCoor;//起點區域相對座標差值,終點區域相對座標差值
			NUROPOINT Temp_T_DiffCoor;//起點區域相對座標差值,終點區域相對座標差值

			nuBOOL b_LocalRut;//是否只進行區域規劃
			nuSHORT SRTTypeCount[20];
			nuSHORT TRTTypeCount[20];
			nuBOOL b_FirstRdEffect;
//			nuBOOL b_PassTollgate;//是否排除收費路段
			nuDWORD TWalkingNode;

			Class_ThrCtrl*		m_pGThr[2];//跨區檔(thr)的資料暫存區
			Class_ThrCtrl*		m_pThr_F;//跨區檔(thr)的資料暫存區
			Class_ThrCtrl*		m_pThr_T;//跨區檔(thr)的資料暫存區
			CCtrl_3dp*			m_pcls3dp;//3D實景圖檔	
			CCtrl_ECI*			m_pECI3dp;//Zenrin 3DP
			/*#ifdef VALUE_EMGRT
				CCtrl_TT*		m_pNT_F;
				CCtrl_TT*		m_pNT_T;
				CCtrl_PTS*		m_PTS_F;
				CCtrl_PTS*		m_PTS_T;
				CCtrl_TT*		m_pclsNT[2];//大陸禁轉檔
				CCtrl_PTS*		m_pclsPTS[2];//大陸高速公路名稱檔	
			#else
				CCtrl_NT*		m_pNT_F;
				CCtrl_NT*		m_pNT_T;
				CCtrl_NT*		m_pclsNT[2];//台灣禁轉檔	
				
			#endif*/
			CCtrl_TT*		m_pNT_F;
			CCtrl_TT*		m_pNT_T;
			CCtrl_TT*		m_pclsNT[2];//大陸禁轉檔
	        #if (defined VALUE_EMGRT) || (defined ZENRIN)
				CCtrl_PTS*		m_PTS_F;
				CCtrl_PTS*		m_PTS_T;
				CCtrl_PTS*		m_pclsPTS[2];//大陸高速公路名稱檔	
			#else
				//CCtrl_NT*		m_pNT_F;
				//CCtrl_NT*		m_pNT_T;
				//CCtrl_NT*		m_pclsNT[2];//台灣禁轉檔	
				
			#endif

			#ifdef _DTMC
				CCtrl_Tmc*		m_pclsTmc;//Tmc專用
			#endif

			#ifdef _DDst
				CCtrl_Dst*		m_pclsDst;//台灣即時路況檔
			#endif
			
			#ifdef _DAR
				CCtrl_AR*		m_pclsAR;//Nearby City
				nuBOOL			m_bARFindnode;
				nuBOOL			m_bARNearbyAR;
				AStarData		m_sARData[5];
				nuINT				m_iARCount;
			#endif
			
			#ifdef _DROUTE_ERROR
				nuLONG   m_lShortestDis;
				nuULONG   m_lShortestCount;
				AStarData		m_ShortestAStarData;
				WEIGHTINGSTRU   m_ShortestChoose;
				AStarData		*m_pShortestData; 
				WEIGHTINGSTRU   *m_pShortestweight;
				nuBOOL			m_bErrorExceptionHandle;
			#endif
			NUROTIME		nuTIME;	
			CCtrl_Thc*		m_pclsThc;//跨國檔
			nuLONG			m_lMaxRoadClassCount;
			nuLONG			m_nRTSTARTSAMECLASSROAD;
			nuLONG			m_nRTFINDRTIDX;
			nuLONG			m_nRTSleep;
			nuLONG          m_nTMCSlowWeighting;
			nuLONG			m_nTMCCrowdWeighting;
	public:
	#ifdef LCMM
			nuLONG			m_lReadBaseData;
			nuLONG			m_lRouteCount;
	#endif
	#ifdef _DTMC
			nuBOOL			m_bTMCLocalRoute;
			nuLONG			m_lTMCDataCount;
			TMC_ROUTER_DATA *m_pTMCPassData;
	#endif
#ifdef _USERT6
			nuBOOL m_bReadRt6;
#endif
	#ifdef THRNodeList
			nuBOOL GetTHRNodeIDList(nuLONG MAPID,nuLONG FNodeID, nuLONG TNodeID, nuLONG RTBID);
	#endif	
//	#ifdef _USETTI
			NUROTIME m_NowTime;
			Class_TTICtrl *m_pTTI_F;
			Class_TTICtrl *m_pTTI_T;
//	#endif

	#ifdef THC_THR_LINK
			CCMPCtrl m_CMP;
			nuLONG m_Dif_X;
			nuLONG m_Dif_Y;
	#endif
	};

#endif