// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:18:52 $
// $Revision: 1.4 $
#include "stdAfxRoute.h"
#include "NuroConstDefined.h"
#include "NuroDefine.h"

#include "Ctrl_UONE_BH.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
//我們只需要用到Road
#define BHARRAYCOUNT	6//需求 6//全讀 10

UONEBHCtrl::UONEBHCtrl()
{
	p_FBH=NULL;//Block段落
	m_BH.m_BHH.BlockCount=0;
	m_BH.m_BHH.Builder[0]=NULL;
	m_BH.m_BHH.MapID[0]=NULL;
	m_BH.m_BHH.Reverser[0]=NULL;
	m_BH.m_BHH.Version[0]=NULL;
	m_BH.BHdata=NULL;
#ifdef _DEBUG
	UoneBHAlocCount = 0;
	UoneBHFreeCount = 0;
#endif
}

UONEBHCtrl::~UONEBHCtrl()
{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"BH_MemUesd %d\n", UoneBHAlocCount - UoneBHFreeCount);
			fclose(pfile);
		}
	#endif
#endif
	ReleaseBH();
	if(p_FBH!=NULL){	nuFclose(p_FBH);		p_FBH=NULL;	}
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
}

nuBOOL UONEBHCtrl::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	BHFileIdx=-1;
	return nuTRUE;
}

nuVOID UONEBHCtrl::CloseClass()
{
	ReleaseBH();
	if(p_FBH!=NULL){	nuFclose(p_FBH);		p_FBH=NULL;	}
}

nuULONG UONEBHCtrl::GlobalBlockID(nuLONG index)//區間唯一碼
{
	nuULONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*0)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::POIDataAddr(nuLONG index)//地標點資料位置
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*1)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::POIDataSize(nuLONG index)//地標點資料大小
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*2)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::RoadDataAddr(nuLONG index)//道路資料位置
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*3)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::RoadAddCount(nuLONG index)//道路累積量
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*4)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::RoadDataSize(nuLONG index)//道路資料大小
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*5)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::BgLDataAddr(nuLONG index)//背景線資料位置
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*6)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::BgLDataSize(nuLONG index)//背景線資料大小
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*7)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::BgADataAddr(nuLONG index)//背景面資料位置
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*8)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
nuLONG UONEBHCtrl::BgADataSize(nuLONG index)//背景面資料大小
{
	nuLONG renum = 0;
	nuMemcpy(&renum,*m_BH.BHdata+((sizeof(nuLONG)*9)*m_BH.m_BHH.BlockCount+sizeof(nuLONG)*index),sizeof(nuLONG));//用來取得最後一筆資料 到時候要用在回推上
	return renum;
}
NUROPOINT UONEBHCtrl::BlockBaseCoor(nuLONG index)//區間基準座標
{
	NUROPOINT BHCoor = {0};
	nuMemcpy(&BHCoor,*m_BH.BHdata+((sizeof(nuLONG)*BHARRAYCOUNT)*m_BH.m_BHH.BlockCount+sizeof(NUROPOINT)*index),sizeof(NUROPOINT));//用來取得最後一筆資料 到時候要用在回推上
	return BHCoor;
}



nuBOOL UONEBHCtrl::ReadBH()
{

	nuULONG i		 = 0;
	NUROPOINT BHCoor = {0};
	nuLONG ShiftIdx  = 0;
	ReleaseBH();
	p_FBH=nuTfopen(FName, NURO_FS_RB);
	if(p_FBH==NULL)												{	ReleaseBH();	return nuFALSE;	}
	if(NULL != nuFseek(p_FBH,0,NURO_SEEK_SET))						{	ReleaseBH();	return nuFALSE;	}
	if(1!=nuFread(&(m_BH.m_BHH),sizeof(BHHeaderStru),1,p_FBH))	{	ReleaseBH();	return nuFALSE;	}

	m_BH.BHdata=(nuBYTE**)g_pRtMMApi->MM_AllocMem((sizeof(nuLONG)*BHARRAYCOUNT+sizeof(NUROPOINT))*m_BH.m_BHH.BlockCount);
#ifdef _DEBUG
	UoneBHAlocCount++;
#endif
	if(m_BH.BHdata==NULL)								{	ReleaseBH();	return nuFALSE;	}
	if(NULL != nuFseek(p_FBH,sizeof(BHHeaderStru),NURO_SEEK_SET))	{	ReleaseBH();	return nuFALSE;	}
	if(((sizeof(nuLONG)*BHARRAYCOUNT)*m_BH.m_BHH.BlockCount)!=nuFread(*m_BH.BHdata,sizeof(nuBYTE),(sizeof(nuLONG)*BHARRAYCOUNT)*m_BH.m_BHH.BlockCount,p_FBH))	{	ReleaseBH();	return nuFALSE;	}
	ShiftIdx=(sizeof(nuLONG)*BHARRAYCOUNT)*m_BH.m_BHH.BlockCount;
	for(i=0;i<m_BH.m_BHH.BlockCount;i++)
	{
		BHCoor.x= (GlobalBlockID(i)%XBLOCKNUM)*EACHBLOCKSIZE - XBLOCKNUM*EACHBLOCKSIZE/2;
		BHCoor.y=	(GlobalBlockID(i)/XBLOCKNUM)*EACHBLOCKSIZE - YBLOCKNUM*EACHBLOCKSIZE/2;
		nuMemcpy(*m_BH.BHdata+ShiftIdx,&BHCoor,sizeof(NUROPOINT));//用來取得最後一筆資料 到時候要用在回推上
		ShiftIdx += sizeof(NUROPOINT);
	}
	nuFclose(p_FBH);
	p_FBH=NULL;
	return nuTRUE;
}

nuVOID UONEBHCtrl::ReleaseBH()
{

	if(m_BH.BHdata!=NULL)	
	{	
#ifdef _DEBUG
	UoneBHFreeCount++;	
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_BH.BHdata);
		m_BH.BHdata=NULL;	
	}

	m_BH.m_BHH.BlockCount=0;
	m_BH.m_BHH.Builder[0]=NULL;
	m_BH.m_BHH.MapID[0]=NULL;
	m_BH.m_BHH.Reverser[0]=NULL;
	m_BH.m_BHH.Version[0]=NULL;
	m_BH.BHdata=NULL;
	
	if(p_FBH!=NULL){	nuFclose(p_FBH);		p_FBH=NULL;	}

	BHFileIdx=-1;

}
