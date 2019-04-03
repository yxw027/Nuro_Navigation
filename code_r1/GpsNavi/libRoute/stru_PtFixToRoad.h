// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
#ifndef Def_Stru_PtFixToRoad
	#define Def_Stru_PtFixToRoad
	#include "NuroDefine.h"
	typedef struct Tag_PtFixToRoadStru
	{
		nuULONG   MapID;//最近道路所在的檔案ID
		nuULONG   RTBID;//最近道路所在的區塊ID
		NUROPOINT FixCoor;//拉回座標
		nuULONG   FixNode1;//最近道路的節點ID1
		nuULONG   FixNode2;//最近道路的節點ID2
		nuLONG    FixDis;//規劃點與最近道路的距離
	}PtFixToRoadStru,*pPtFixToRoadStru;
#endif