// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/25 05:25:45 $
// $Revision: 1.4 $
#ifndef Def_AStar
	#define Def_AStar
	#include "ConstSet.h"
	#include "Class_CtrlAVLTree.h"

	class AStarWork
	{
		public:
			AStarWork();
			virtual ~AStarWork();
			nuLONG lUsedCloseCount;//使用掉的close數量
			#ifdef _DEBUG
				nuLONG lUsedOpenCount;//使用掉的Open數量
				nuLONG lCallCloseCount;//Open->Close的呼叫次數
				nuLONG lCallOpenCount;//資料->Open的呼叫次數
				nuLONG Count_SRT;//現階段Open端總數
				nuLONG Count_STHR;//現階段Close端總數
				nuLONG Count_THC;//現階段Close端總數
				nuLONG Count_TTHR;//現階段Close端總數
				nuLONG Count_TRT;//現階段Close端總數
			#endif

			nuBOOL IniData();
			nuBOOL AddToOpen(AStarData m_AStarData, nuLONG STE_ID, WEIGHTINGSTRU NewWeighting);//加入Open串列

			nuBOOL GetBestOpenToClose(LpAVLTree &m_SAVLTree, WEIGHTINGSTRU &BestChoose);
			nuBOOL GetGivenData(AStarData m_AStarData, LpAVLTree &m_SAVLTree, nuLONG STE_ID);

			CtrlAVLTree m_AVLTree;

		private:
			nuVOID InitConst();
			nuVOID ReleaseCloseStru();//清除Close端資料

	};
#endif