// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:31:28 $
// $Revision: 1.4 $
#ifndef Def_Class_PtFixToRoad
	#define Def_Class_PtFixToRoad

	#include "ConstSet.h"
	#include "NuroEngineStructs.h"

	#include "AllSystemDataBase.h"
	#include "Ctrl_UONE_Rt.h"
	#include "NuroDefine.h"

	class Class_PtFixToRoad// : public MathTool
	{
		public:
			Class_PtFixToRoad();
			virtual ~Class_PtFixToRoad();
			nuBOOL PtFixToRoad(nuLONG CoorX,nuLONG CoorY,nuLONG FileIdx,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngl,PTFIXTOROADSTRU &PtFixedInfo);
			nuBOOL NewPtFixToRoad(nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,NUROPOINT Now_FixPt,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo);
			nuBOOL NewPtFixToRoad_NodeID(nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG CityID, nuLONG TownID, nuLONG Now_RoadIdx,NUROPOINT Now_FixPt,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo);
		private:
			nuBOOL GetFixInfo(nuLONG CoorX,nuLONG CoorY,nuULONG DwBlockIdx,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl);
			nuBOOL GetFixInfo_CheckRTBID(nuLONG DWBlockIdx,nuLONG DWRoadIdx,nuULONG RTBlockID,UONERtCtrl &TmpRtCtrl,nuLONG &RTBIdx,nuULONG &FixNodeID1,nuULONG &FixNodeID2);
			nuBOOL NewGetFixInfo(nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,NUROPOINT Now_FixPt,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl);
			nuBOOL NewGetFixInfo_NodeID(nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,nuLONG CityID, nuLONG TownID, NUROPOINT Now_FixPt,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,class UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl);
			UONERtCtrl	TmpRtCtrl;//~~~要有開檔的動作~~~//
			//nuBOOL CoverAngle(double TargetAngle,double SourceAngle);
			nuBOOL CoverAngle(nuLONG TargetAngle,nuLONG SourceAngle);
			nuBOOL WorkNow;
			nuLONG GetBlockID(nuLONG CoorX,nuLONG CoorY);

			RefCtrl**		ppclsRef;
	};

#endif