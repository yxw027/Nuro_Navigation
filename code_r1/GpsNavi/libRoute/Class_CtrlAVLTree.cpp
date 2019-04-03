// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:30:51 $
// $Revision: 1.20 $
#include "stdAfxRoute.h"
#include "Class_CtrlAVLTree.h"
#include "ConstSet.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
CtrlAVLTree::CtrlAVLTree()
{
	nuLONG i = 0;
	p_BaseData=NULL;
	BaseCount=0;

	WeightingArray=NULL;
	WeightingCount=1;

	for(i=0;i<MAXMappingTreeCount;i++){
		p_MappingTree[i]=NULL;
	}
	m_bSaveSlowClass = nuFALSE;
#ifdef _DEBUG
	AVLAlocCount = 0;
	AVLFreeCount = 0;
#endif
}

CtrlAVLTree::~CtrlAVLTree()
{
	RemoveBaseAVLTreeArray();
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"AVL_MemUesd %d\n", AVLAlocCount - AVLFreeCount);
		fclose(pfile);
	}
#endif
#endif
}

//資料base函式
nuBOOL CtrlAVLTree::CreateBaseAVLTreeArray()
{
	nuLONG i = 0;
	RemoveBaseAVLTreeArray();
	BaseCount=0;
	WeightingCount=1;

	for(i=0;i<MAXMappingTreeCount;i++){
		p_MappingTree[i]=NULL;
	}
	p_BaseData=(TagAVLTree**)g_pRtMMApi->MM_AllocMem(sizeof(TagAVLTree)*MaxAVLCount);
#ifdef _DEBUG
	AVLAlocCount++;
#endif
	if(p_BaseData==NULL)
	{	RemoveBaseAVLTreeArray();	return nuFALSE;		}
	else
	{	nuMemset(*p_BaseData,NULL,MaxAVLCount*sizeof(TagAVLTree));	}

	WeightingArray=(tagWEIGHTINGSTRU**)g_pRtMMApi->MM_AllocMem(sizeof(tagWEIGHTINGSTRU)*MaxAVLWeightCount);
#ifdef _DEBUG
	AVLAlocCount++;
#endif
	if(WeightingArray==NULL)
	{	RemoveBaseAVLTreeArray();	return nuFALSE;		}
	else
	{
		for(i=0;i<MaxAVLWeightCount;i++){
			(*WeightingArray)[i].TargetID=MaxAVLCount;
			(*WeightingArray)[i].TotalDis=0;
			(*WeightingArray)[i].RealDis=0;
		}
		return nuTRUE;
	}
//	LEVEL_Max=0;
//	LEVEL_Count=0;
}

nuVOID CtrlAVLTree::RemoveBaseAVLTreeArray()
{
	nuLONG i;
	for(i=0;i<MAXMappingTreeCount;i++){
		p_MappingTree[i]=NULL;
	}

	if(WeightingArray)
	{
#ifdef _DEBUG
		AVLFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)WeightingArray);
		WeightingArray=NULL;
	}

	WeightingCount=1;
	if(p_BaseData)
	{
#ifdef _DEBUG
		AVLFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_BaseData);
		p_BaseData=NULL;
	}
	BaseCount=0;
	m_bSaveSlowClass = nuFALSE;
}

nuVOID CtrlAVLTree::CleanAVLTreeArray()
{
	nuLONG i;
	if(p_BaseData==NULL) return;
	if(WeightingArray==NULL) return;
	BaseCount=0;
	WeightingCount=1;

	for(i=0;i<MAXMappingTreeCount;i++){
		p_MappingTree[i]=NULL;
	}
	nuMemset(*p_BaseData,NULL,MaxAVLCount*sizeof(TagAVLTree));
	for(i=0;i<MaxAVLWeightCount;i++){
		(*WeightingArray)[i].TargetID=MaxAVLCount;
		(*WeightingArray)[i].TotalDis=0;
		(*WeightingArray)[i].RealDis=0;
	}
	m_bSaveSlowClass = nuFALSE;
}

nuBOOL CtrlAVLTree::AddNewBase(AStarData &m_AStarData,LpAVLTree &m_SAVLTree)//增加一筆資料進入結構
{
	m_SAVLTree=NULL;
	if(BaseCount>=MaxAVLCount) return nuFALSE;
	m_SAVLTree=&(*p_BaseData)[BaseCount];
	nuMemcpy(&((*p_BaseData)[BaseCount].Data),&(m_AStarData),sizeof(TagAStarData));
	m_SAVLTree->p_High=MaxAVLCount;
	m_SAVLTree->p_Low=MaxAVLCount;
	m_SAVLTree->p_Self=BaseCount;
	/*m_SAVLTree->p_SelfL=BaseCount%MAXSELFL;
	m_SAVLTree->p_SelfH=BaseCount/MAXSELFL;*/
	BaseCount++;
	return nuTRUE;
}

//新增一筆資料
nuBOOL CtrlAVLTree::Insert(AStarData m_AStarData,nuLONG STE_ID,WEIGHTINGSTRU NewWeighting)
{
	LpAVLTree p_NewData=NULL,p_tempData=NULL,p_tempexchange=NULL;
	nuBOOL b_Exist=nuFALSE;
	nuULONG lTempID = 0;

	if(CheckMapping(m_AStarData,p_tempData,STE_ID)){//判斷是否出現過
		if((m_AStarData.RDis+m_AStarData.NowDis) >= (p_tempData->Data.RDis+p_tempData->Data.NowDis))
		{
#ifdef NT_TO_TT
			if(m_AStarData.Class == 9 && m_AStarData.NNoTurnFlag == 1)
			{
				m_bSaveSlowClass = nuTRUE;
				if((WeightingCount+1)>=(MaxAVLWeightCount-1))
				{
					return nuFALSE;
				}
				if(!AddNewBase(m_AStarData,p_NewData))
				{
					return nuFALSE;
				}
				AddToMapping(p_NewData,6);
				(*WeightingArray)[WeightingCount].TargetID=BaseCount-1;
				(*WeightingArray)[WeightingCount].TotalDis=NewWeighting.TotalDis;
				(*WeightingArray)[WeightingCount].RealDis=NewWeighting.RealDis;
				WeightingCount++;
				MoveWeightingArray(WeightingCount-1);
				return nuTRUE;
			}
			else
#endif
			{
				return nuFALSE;
			}
		}
		//資料內容都換過了 現在剩下的就是如何架構權值樹
		nuMemcpy(&(p_tempData->Data),&(m_AStarData),sizeof(TagAStarData));

		//lTempID=p_tempData->p_SelfL+p_tempData->p_SelfH*MAXSELFL;
		lTempID=p_tempData->p_Self;
		for(nuLONG i=1;i<WeightingCount;i++)
		{
			if((*WeightingArray)[i].TargetID==lTempID)
			{
				(*WeightingArray)[i].TotalDis=NewWeighting.TotalDis;
				(*WeightingArray)[i].RealDis=NewWeighting.RealDis;
				MoveWeightingArray(i);
				return nuFALSE;
			}
		}
		(*WeightingArray)[WeightingCount].TargetID=lTempID;//open weighting要增加一筆資料
		(*WeightingArray)[WeightingCount].TotalDis=NewWeighting.TotalDis;
		(*WeightingArray)[WeightingCount].RealDis=NewWeighting.RealDis;
		WeightingCount++;
		MoveWeightingArray(WeightingCount-1);
		return nuTRUE;
	}
	else{
		if((WeightingCount+1)>=(MaxAVLWeightCount-1))
		{
			return nuFALSE;
		}
		if(!AddNewBase(m_AStarData,p_NewData))
		{
			return nuFALSE;
		}
		AddToMapping(p_NewData,STE_ID);//mapping tree要新增一筆資料
		(*WeightingArray)[WeightingCount].TargetID=BaseCount-1;//open weighting要增加一筆資料
		(*WeightingArray)[WeightingCount].TotalDis=NewWeighting.TotalDis;
		(*WeightingArray)[WeightingCount].RealDis=NewWeighting.RealDis;
		WeightingCount++;
		MoveWeightingArray(WeightingCount-1);
		return nuTRUE;
	}
}

//Mapping Tree Work(比對樹的動作)
nuBOOL CtrlAVLTree::CheckMapping(AStarData m_AStarData,LpAVLTree &m_ReTurnNode,nuLONG STE_ID)
{
	LpAVLTree p_tempData = NULL;
	nuLONG cmprenum=0;
	p_tempData=p_MappingTree[STE_ID];

	while(nuTRUE)
	{
		if(p_tempData==NULL)
		{	
			m_ReTurnNode=NULL;
			return nuFALSE;	
		}

		cmprenum=nuMemcmp(&(p_tempData->Data),&m_AStarData,MappingCheckByte_s);
		if(cmprenum==0 && !(p_tempData->Data.TNoTurnFlag==0 && p_tempData->Data.NNoTurnFlag==0 && p_tempData->Data.FNoTurnFlag==0)){
			cmprenum=nuMemcmp(&(p_tempData->Data),&m_AStarData,MappingCheckByte_l);
		}

		if(	cmprenum>0 ){
			if(p_tempData->p_Low==MaxAVLCount)	
			{
				m_ReTurnNode=NULL;	
				return nuFALSE;
			}
			else								
			{	
				p_tempData=&((*p_BaseData)[p_tempData->p_Low]);	
			}
		}
		else if( cmprenum<0 ){
			if(p_tempData->p_High==MaxAVLCount)	
			{	
				m_ReTurnNode=NULL;	
				return nuFALSE;	
			}
			else								
			{	
				p_tempData=&((*p_BaseData)[p_tempData->p_High]);	
			}
		}
		else
		{
			m_ReTurnNode=p_tempData;
			return nuTRUE;
		}
	}
}

nuBOOL CtrlAVLTree::CheckMappingNew(AStarData m_AStarData,LpAVLTree &m_ReTurnNode,nuLONG STE_ID)
{
	LpAVLTree p_tempData = NULL;
	nuLONG cmprenum=0;
	p_tempData=p_MappingTree[STE_ID];

	while(nuTRUE)
	{
		if(p_tempData==NULL)
		{	
			m_ReTurnNode=NULL;
			return nuFALSE;	
		}

		cmprenum=nuMemcmp(&(p_tempData->Data),&m_AStarData,MappingCheckByte_s);
		if(cmprenum==0)
		{
			cmprenum=nuMemcmp(&(p_tempData->Data),&m_AStarData,MappingCheckByte_l);
#ifdef NT_TO_TT
			if(m_bSaveSlowClass && m_AStarData.RDis == 1 && cmprenum == 0)
			{
				if((p_tempData->Data.FNodeID != m_AStarData.FNodeID ||
					p_tempData->Data.FMapID != m_AStarData.FMapID ||
					p_tempData->Data.FRtBID != m_AStarData.FRtBID ||
					p_tempData->Data.FMapLayer != m_AStarData.FMapLayer))
				{
					m_AStarData.RDis = 0;//已經搜尋過有問題的地方，如果又回這裡，會有無窮回圈的情況出現
					p_tempData = p_MappingTree[6];
					continue;
				}
			}
#endif
		}

		if(	cmprenum>0 )
		{
			if(p_tempData->p_Low==MaxAVLCount)	
			{	
				m_ReTurnNode=NULL;	
				return nuFALSE;	
			}
			else								
			{	
				p_tempData=&((*p_BaseData)[p_tempData->p_Low]);	
			}
		}
		else if( cmprenum<0 )
		{
			if(p_tempData->p_High==MaxAVLCount)	
			{	
				m_ReTurnNode=NULL;	
				return nuFALSE;	
			}
			else								
			{	
				p_tempData=&((*p_BaseData)[p_tempData->p_High]);	
			}
		}
		else{
			m_ReTurnNode=p_tempData;
			return nuTRUE;
		}
	}
}

nuVOID CtrlAVLTree::AddToMapping(LpAVLTree &m_NewAddData,nuLONG STE_ID)
{
	nuINT cmprenum = 0;
	LpAVLTree p_tempData=NULL;

	if(p_MappingTree[STE_ID]==NULL)
	{
		//第一筆資料的加入
		p_MappingTree[STE_ID]=m_NewAddData;//外部加入時 一定要先經過初始化整理
		return;
	}
	else
	{
		p_tempData = p_MappingTree[STE_ID];
//		LEVEL_Count=0;
		while(nuTRUE)
		{
			cmprenum=nuMemcmp(&(p_tempData->Data),&(m_NewAddData->Data),MappingCheckByte_l);
//			LEVEL_Count++;
			if( cmprenum>0 )
			{
				if(p_tempData->p_Low==MaxAVLCount)
				{	
					//p_tempData->p_Low=m_NewAddData->p_SelfL+m_NewAddData->p_SelfH*MAXSELFL;
					p_tempData->p_Low=m_NewAddData->p_Self;
					break;		
				}
				else
				{	p_tempData=&(*p_BaseData)[p_tempData->p_Low];	}
			}
			else
			{
				if(p_tempData->p_High==MaxAVLCount)
				{	
					//p_tempData->p_High=m_NewAddData->p_SelfL+m_NewAddData->p_SelfH*MAXSELFL;
					p_tempData->p_High=m_NewAddData->p_Self;
					break;		
				}
				else
				{	
					p_tempData=&(*p_BaseData)[p_tempData->p_High];	
				}
			}
		}
	}
//	if(LEVEL_Max<LEVEL_Count)	LEVEL_Max=LEVEL_Count;
}

//基礎資料中已經確認這筆新資料了 要對他進行位置搬遷
nuVOID CtrlAVLTree::MoveWeightingArray(nuLONG N_Idx)
{
	nuLONG U_Idx = 0;
	WEIGHTINGSTRU tmp_Idx = {0};
	if(WeightingCount<=2) return;

	while(N_Idx!=1)
	{
		U_Idx=N_Idx/2;
		if((*WeightingArray)[U_Idx].TotalDis>(*WeightingArray)[N_Idx].TotalDis)
		{
			tmp_Idx=(*WeightingArray)[N_Idx];
			(*WeightingArray)[N_Idx]=(*WeightingArray)[U_Idx];
			(*WeightingArray)[U_Idx]=tmp_Idx;
			N_Idx=U_Idx;
		}
		else{
			break;
		}
	}
}

nuBOOL CtrlAVLTree::GetBestOpenToClose(LpAVLTree &m_SAVLTree,WEIGHTINGSTRU &BestChoose)
{
	nuLONG N_Idx = 0, DL_Idx = 0, DR_Idx = 0;
	WEIGHTINGSTRU tmp_Idx = {0};
	if(WeightingCount<=1) return nuFALSE;
	m_SAVLTree=&((*p_BaseData)[(*WeightingArray)[1].TargetID]);

	BestChoose.TotalDis=(*WeightingArray)[1].TotalDis;
	BestChoose.RealDis=(*WeightingArray)[1].RealDis;
	BestChoose.TargetID=(*WeightingArray)[1].TargetID;

	(*WeightingArray)[1]=(*WeightingArray)[WeightingCount-1];
	(*WeightingArray)[WeightingCount-1].TargetID=MaxAVLCount;
	(*WeightingArray)[WeightingCount-1].TotalDis=0;
	(*WeightingArray)[WeightingCount-1].RealDis=0;
	WeightingCount--;
	//重整權值樹
	//往下重整
	N_Idx=1;

	while(nuTRUE){
		DL_Idx=N_Idx*2;
		DR_Idx=DL_Idx+1;
		if(DL_Idx<WeightingCount && DR_Idx<WeightingCount){
			if((*WeightingArray)[DL_Idx].TotalDis<(*WeightingArray)[DR_Idx].TotalDis){
				if((*WeightingArray)[DL_Idx].TotalDis<(*WeightingArray)[N_Idx].TotalDis){
					tmp_Idx=(*WeightingArray)[N_Idx];
					(*WeightingArray)[N_Idx]=(*WeightingArray)[DL_Idx];
					(*WeightingArray)[DL_Idx]=tmp_Idx;
					N_Idx=DL_Idx;
				}
				else
					break;
			}
			else{
				if((*WeightingArray)[DR_Idx].TotalDis<(*WeightingArray)[N_Idx].TotalDis){
					tmp_Idx=(*WeightingArray)[N_Idx];
					(*WeightingArray)[N_Idx]=(*WeightingArray)[DR_Idx];
					(*WeightingArray)[DR_Idx]=tmp_Idx;
					N_Idx=DR_Idx;
				}
				else
					break;
			}
		}
		else if(DL_Idx<WeightingCount){
			if((*WeightingArray)[DL_Idx].TotalDis<(*WeightingArray)[N_Idx].TotalDis){
				tmp_Idx=(*WeightingArray)[N_Idx];
				(*WeightingArray)[N_Idx]=(*WeightingArray)[DL_Idx];
				(*WeightingArray)[DL_Idx]=tmp_Idx;
				N_Idx=DL_Idx;
			}
			break;
		}
		else{
			break;
		}
	}
	return nuTRUE;
}

/*nuINT	CtrlAVLTree::nucmp(const nuVOID *pDes, const nuVOID *pSrc, size_t nLen)
{
	for(nuINT i=0; i < nLen; i++)
	{
		if(((nuLONG*)pDes)[i] == ((nuLONG*)pSrc)[i])
		{
			return 0;
		}
		if(((nuLONG*)pDes)[i] > ((nuLONG*)pSrc)[i])
		{
			return 1;
		}
		if(((nuLONG*)pDes)[i] < ((nuLONG*)pSrc)[i])
		{
			return -1;
		}
	}
}*/