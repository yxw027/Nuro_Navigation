#ifndef Def_Class_CalMRT
	#define Def_Class_CalMRT
	#include "NuroDefine.h"

	#include "RouteBase.h"
	#include "NuroEngineStructs.h"
	#include "AStar.h"
	#include "Class_Approach.h"
	#include "ConstSet.h"
	#include "Ctrl_MDW.h"
	#include "Ctrl_MRT.h"
	#include "Ctrl_Thc.h"
	#include "Ctrl_Thr.h"
	
	class Class_CalMRT
	{
		public:
			Class_CalMRT();
			virtual ~Class_CalMRT();

			nuBOOL AStar(AStarWork &m_AStar, nuULONG FMRTNodeID, nuULONG TMRTNodeID);//路徑規劃
			nuVOID Astar_SRT(NUROPOINT TargetCoor, TagAStarData &m_SAVLData, AStarWork &m_AStar, nuULONG FMRTNodeID, nuULONG TMRTNodeID, WEIGHTINGSTRU BestChoose);
			nuLONG UONEGetGuessDistance(NUROPOINT Pt1,NUROPOINT PT2);//計算預估距離(現在公式與計算距離相同 如果有更好的預估公式 那當然就改變計算的方式)
			nuBOOL GetWeighting(nuLONG Class);
			nuBOOL GetNodeIDList(AStarWork &m_AStar);
			nuBOOL CreateLocalRoutingData(PNAVIRTBLOCKCLASS TmpNaviRtBlockClass);

			//Louis 081111 提供給轉檔專用
			nuLONG CalMRTRouting_Coor(NUROPOINT PT_F, NUROPOINT PT_T, nuLONG &m_MRTGDis, nuLONG &m_MRTRDis);
			nuLONG CalMRTRouting_NodeID(nuLONG MRTID_F, nuLONG MRTID_T, nuLONG &m_MRTGDis, nuLONG &m_MRTRDis);
			nuVOID LeaveCalMRTRouting(AStarWork &m_AStar);

			//Louis 081111 提供給Route進行遠距離推算
			nuLONG MRTRouting(PNAVIRTBLOCKCLASS pNRBC, nuLONG MRTID_F, nuLONG MRTID_T, nuLONG l_RoutingRule);
			nuVOID LeaveMRTRouting(AStarWork &m_AStar);

			nuBOOL LocalAStarSuccessWorkStep1(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL LocalAStarSuccessWorkStep2(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL LocalAStarSuccessWorkStep3(PNAVIRTBLOCKCLASS pNRBC);//區域規劃(Local AStar)成功的後續資料整理
			nuBOOL GetBlockRoadIdx(nuUSHORT *BlockIdx, nuUSHORT *RoadIdx, nuULONG FNodeID, nuULONG TNodeID);
			nuLONG GetDistance(NUROPOINT Pt1, NUROPOINT PT2);//計算距離
			nuLONG GetAngle(NUROPOINT FPt, NUROPOINT NPt, NUROPOINT TPt);

			nuBOOL LoadCalMRTRoutingData(nuLONG l_RoutingRule);
			nuBOOL FindNearEnd6Net(AStarWork &m_AStar, NUROPOINT &CLRS_T, nuLONG &Dis_Rd6, nuULONG &TMRTNodeID, nuULONG &LimitNodeID_6);
			nuVOID FreeCalMRTRoutingData();

			//轉thc檔專用function
			nuLONG NewCalMRTRouting_NodeID(nuLONG MRTID_F, nuLONG MRTID_T, THCROUTEDATA &stuThcRouteData);

		private:
			CMDWCtrl		m_mdw;
			CMRTCtrl		m_mrt;
			nuLONG          ClassWeighting;//等級權值(分子)
			nuLONG          AveLatitude;

			nuLONG          MRTGDis;
			nuLONG          MRTRDis;
			nuLONG          TargetClass;
			AStarData		m_ApprocahAStarData;
			Class_Approach	Tmp_Approach;//Astar規畫完之後留下來的資料(approach結束)
			CCtrl_Thc**		ppclsThc;//跨國檔
			Class_ThrCtrl**	ppclsGThr;//跨區檔(thr)的資料暫存區
	};

#endif

