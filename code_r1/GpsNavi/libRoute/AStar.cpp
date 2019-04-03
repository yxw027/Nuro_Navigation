// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2009/12/30 09:45:13 $
// $Revision: 1.6 $
#include "AStar.h"

AStarWork::AStarWork()
{
	InitConst();
}

AStarWork::~AStarWork()
{
	IniData();
 	m_AVLTree.RemoveBaseAVLTreeArray();
}

nuVOID AStarWork::InitConst()
{
	#ifdef _DEBUG
		lUsedOpenCount=0;//現階段Open端總數
		lCallOpenCount=0;
		lCallCloseCount=0;

		Count_SRT=0;//現階段Open端總數
		Count_STHR=0;//現階段Close端總數
		Count_THC=0;//現階段Close端總數
		Count_TTHR=0;//現階段Close端總數
		Count_TRT=0;//現階段Close端總數
	#endif
	lUsedCloseCount=0;//現階段Close端總數
}

nuBOOL AStarWork::IniData()
{
	ReleaseCloseStru();
	InitConst();
	return nuTRUE;
}

nuVOID AStarWork::ReleaseCloseStru()//清除Close端資料
{
	m_AVLTree.CleanAVLTreeArray();
	#ifdef _DEBUG
		lUsedOpenCount=0;//現階段Open端總數
	#endif
	lUsedCloseCount=0;//現階段Close端總數
}

nuBOOL AStarWork::AddToOpen(AStarData m_AStarData, nuLONG STE_ID, WEIGHTINGSTRU NewWeighting)//加入Open串列
{
	#ifdef _DEBUG
		lCallOpenCount++;
		if(STE_ID==0)	Count_SRT++;
		else if(STE_ID==1)	Count_STHR++;
		else if(STE_ID==2)	Count_THC++;
		else if(STE_ID==3)	Count_TTHR++;
		else if(STE_ID==4)	Count_TRT++;
	#endif
	if(!m_AVLTree.Insert(m_AStarData,STE_ID,NewWeighting))
	{
		return nuFALSE;
	}
	else
	{
		#ifdef _DEBUG
			lUsedOpenCount++;
		#endif
		return nuTRUE;
	}
}

nuBOOL AStarWork::GetBestOpenToClose(LpAVLTree &m_SAVLTree, WEIGHTINGSTRU &BestChoose)
{
	#ifdef _DEBUG
		lCallCloseCount++;
	#endif
	if(!m_AVLTree.GetBestOpenToClose(m_SAVLTree,BestChoose))
	{
		#ifdef _DEBUG
			lUsedOpenCount--;
		#endif
		return nuFALSE;
	}
	else
	{
		lUsedCloseCount++;
		#ifdef _DEBUG
			lUsedOpenCount--;
		#endif
		return nuTRUE;
	}
}

nuBOOL AStarWork::GetGivenData(AStarData m_AStarData, LpAVLTree &m_SAVLTree, nuLONG STE_ID)
{
//	return m_AVLTree.CheckMapping_S(m_AStarData,m_SAVLTree);
	if(m_AVLTree.CheckMappingNew(m_AStarData,m_SAVLTree,STE_ID))
	{
		return nuTRUE;
	}
	else
	{
		return m_AVLTree.CheckMapping(m_AStarData,m_SAVLTree,STE_ID);
	}
}
