// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:20:51 $
// $Revision: 1.6 $
#include "stdAfxRoute.h"

#include "Ctrl_UONE_Rt.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
	UONERtCtrl::UONERtCtrl()
	{
		InitConst();
	#ifdef _DEBUG
		UoneRTAlocCount = 0;
		UoneRTFreeCount = 0;
	#endif
	}

	UONERtCtrl::~UONERtCtrl()
	{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Rt_MemUesd %d\n", UoneRTAlocCount - UoneRTFreeCount);
			fclose(pfile);
		}
	#endif
#endif
		CloseClass();
	}

	nuBOOL UONERtCtrl::InitClass()
	{
		nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
		return nuTRUE;
	}
	nuVOID UONERtCtrl::CloseClass()
	{
		ReSet();
		nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	}

	nuBOOL UONERtCtrl::Read_Header()
	{
		nuLONG i = 0;
		RtFile=nuTfopen(FName,NURO_FS_RB);
		if(RtFile==NULL)
		{	return nuFALSE;	}
		if(0!=nuFseek(RtFile,0,NURO_SEEK_SET)){
			Remove_Header();
			return nuFALSE;
		}
		if(1!=nuFread(&(MapID),sizeof(nuUSHORT),1,RtFile)){
			Remove_Header();
			return nuFALSE;
		}
		if(1!=nuFread(&(Rt_Block_Count),sizeof(nuUSHORT),1,RtFile)){
			Remove_Header();
			return nuFALSE;
		}

		//取得總RTBlock數
		Rt_Block_ID_List=(nuULONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuULONG)*Rt_Block_Count);
		Rt_Block_Addr_List=(nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Rt_Block_Count);
		RT_Block_Coor=(ONECoorStru**)g_pRtMMApi->MM_AllocMem(sizeof(ONECoorStru)*Rt_Block_Count);
		TmpRtB=(UONERtBData**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONERtBlockData)*Rt_Block_Count);
		if( Rt_Block_ID_List==NULL || Rt_Block_Addr_List==NULL || RT_Block_Coor==NULL || TmpRtB==NULL){
			Remove_Header();
			return nuFALSE;
		}
	#ifdef _DEBUG
		UoneRTAlocCount += 4;
	#endif
		if(0!=nuFseek(RtFile,sizeof(nuUSHORT)+sizeof(nuUSHORT),NURO_SEEK_SET)){
			Remove_Header();
			return nuFALSE;
		}
		if(Rt_Block_Count!=nuFread(*Rt_Block_ID_List,sizeof(nuULONG),Rt_Block_Count,RtFile)){
			Remove_Header();
			return nuFALSE;
		}
		if(Rt_Block_Count!=nuFread(*Rt_Block_Addr_List,sizeof(nuLONG),Rt_Block_Count,RtFile)){
			Remove_Header();
			return nuFALSE;
		}
		//之前thr表頭遺留下來的無用資料 先用RT_Block_Coor結構去裝填(因為比較大)
		if(Rt_Block_Count!=nuFread(*RT_Block_Coor,sizeof(nuLONG),Rt_Block_Count,RtFile)){
			Remove_Header();
			return nuFALSE;
		}
		//這一次讀 才是真正資料的開始
		if(Rt_Block_Count!=nuFread(*RT_Block_Coor,sizeof(Tag_ONECoorStru),Rt_Block_Count,RtFile)){
			Remove_Header();
			return nuFALSE;
		}

		for(i=0;i<Rt_Block_Count;i++)
		{
			(*TmpRtB)[i].MyState.DwBlockID=0;				(*TmpRtB)[i].MyState.DwBlockIdx=0;
			(*TmpRtB)[i].MyState.MapID=0;					(*TmpRtB)[i].MyState.MapIdx=0;
			(*TmpRtB)[i].MyState.RtBlockID=0;				(*TmpRtB)[i].MyState.RtBlockIdx=0;

			(*TmpRtB)[i].RtBHeader.RtConnectStruCount=0;	(*TmpRtB)[i].RtBHeader.RtConnectStruStartAddr=0;
			(*TmpRtB)[i].RtBHeader.RtIndexStruStartAddr=0;	(*TmpRtB)[i].RtBHeader.TotalNodeCount=0;
			(*TmpRtB)[i].RtBHeader.TotalRoadCount=0;

			(*TmpRtB)[i].RtBMain=NULL;
			(*TmpRtB)[i].RtBConnect=NULL;
			(*TmpRtB)[i].RtBIdx=NULL;

			if(0!=nuFseek(RtFile,(*Rt_Block_Addr_List)[i],NURO_SEEK_SET)){
				Remove_Header();
				return nuFALSE;
			}
			if(1!=nuFread(&((*TmpRtB)[i].RtBHeader),sizeof(UONEF_RtB_Header_Stru),1,RtFile)){
				Remove_Header();
				return nuFALSE;
			}
		}
		nuFclose(RtFile);
		RtFile=NULL;

		//創建存放資料的class
		ReadHeaderOK=nuTRUE;
		return nuTRUE;
	}
	nuVOID UONERtCtrl::Remove_Header()
	{
		nuLONG i = 0;
		//移除存放資料的class
		if(TmpRtB)
		{
			for(i=0;i<Rt_Block_Count;i++)
			{
				ReSetRTB(i);
			}
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpRtB);
			TmpRtB=NULL;
		}
		//移除ㄧ般的表頭資料
		
		if(RT_Block_Coor)
		{	
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RT_Block_Coor);	
			RT_Block_Coor=NULL;
		}
		if(Rt_Block_Addr_List)
		{	
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)Rt_Block_Addr_List);	
			Rt_Block_Addr_List=NULL;
		}
		if(Rt_Block_ID_List)
		{	
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)Rt_Block_ID_List);	
			Rt_Block_ID_List=NULL;	
		}
		Rt_Block_Count=0;
		MapID=0;
		ReadHeaderOK=nuFALSE;
	
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
	}

	nuBOOL UONERtCtrl::ReadRTB(nuLONG RtBIdx,nuBOOL b_MainConnect,nuBOOL b_Idx)//移除Rt檔
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		if(TmpRtB==NULL) return nuFALSE;
		if(RtBIdx>=Rt_Block_Count) return nuFALSE;

		if(b_MainConnect==nuTRUE)
		{
			if(!Read_RTB_Main(RtBIdx)){		ReSetRTB(RtBIdx);	return nuFALSE;	}
			if(!Read_RTB_Connect(RtBIdx)){	ReSetRTB(RtBIdx);	return nuFALSE;	}
		}
		if(b_Idx==nuTRUE)
		{
			if(!Read_RTB_Index(RtBIdx)){	ReSetRTB(RtBIdx);	return nuFALSE;	}
		}
		return nuTRUE;
	}

	nuVOID UONERtCtrl::ReSetRTB(nuLONG RtBIdx)//移除Rt檔
	{
		if(ReadHeaderOK==nuFALSE) return;
		if(RtBIdx>=Rt_Block_Count) return;
		if(TmpRtB!=NULL)
		{
			Release_RTB_Main(RtBIdx);
			Release_RTB_Connect(RtBIdx);
			Release_RTB_Index(RtBIdx);
		}
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
	}

	nuVOID UONERtCtrl::InitConst()
	{
		MapID=0;
		Rt_Block_Count=0;
		Rt_Block_ID_List=NULL;
		Rt_Block_Addr_List=NULL;
		TmpRtB=NULL;
		RtFile=NULL;
		RT_Block_Coor=NULL;
		ReadHeaderOK=nuFALSE;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
	}

	nuVOID UONERtCtrl::ReSet()
	{
		Remove_Header();
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
		InitConst();
	}

	nuBOOL UONERtCtrl::Read_RTB_Main(nuLONG RtBIdx)
	{
		nuULONG i = 0;
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		Release_RTB_Main(RtBIdx);
		(*TmpRtB)[RtBIdx].RtBMain=(UONEF_RtB_MainData_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_MainData_Stru)*(*TmpRtB)[RtBIdx].RtBHeader.TotalNodeCount);
	#ifdef _DEBUG
		UoneRTAlocCount++;
	#endif
		if((*TmpRtB)[RtBIdx].RtBMain==NULL)
		{
			return nuFALSE;
		}
		for(i=0;i<(*TmpRtB)[RtBIdx].RtBHeader.TotalNodeCount;i++)
		{
			(*(*TmpRtB)[RtBIdx].RtBMain)[i].ConnectCount=0;		(*(*TmpRtB)[RtBIdx].RtBMain)[i].ConnectStruIndex=0;
			(*(*TmpRtB)[RtBIdx].RtBMain)[i].FNodeID=0;
			(*(*TmpRtB)[RtBIdx].RtBMain)[i].FRtBID=0;				(*(*TmpRtB)[RtBIdx].RtBMain)[i].NodeCoor.x=0;
			(*(*TmpRtB)[RtBIdx].RtBMain)[i].NodeCoor.y=0;			(*(*TmpRtB)[RtBIdx].RtBMain)[i].NoTurnItem=0;
		}

		RtFile=nuTfopen(FName,NURO_FS_RB);
		if(RtFile==NULL)
		{	Release_RTB_Main(RtBIdx);	return nuFALSE;	}
		if(0!=nuFseek(RtFile,(*Rt_Block_Addr_List)[RtBIdx]+sizeof(UONEF_RtB_Header_Stru),NURO_SEEK_SET))
		{	Release_RTB_Main(RtBIdx);	return nuFALSE;	}
		if((*TmpRtB)[RtBIdx].RtBHeader.TotalNodeCount!=nuFread((*(*TmpRtB)[RtBIdx].RtBMain),sizeof(UONEF_RtB_MainData_Stru),(*TmpRtB)[RtBIdx].RtBHeader.TotalNodeCount,RtFile))
		{	Release_RTB_Main(RtBIdx);	return nuFALSE;	}

		nuFclose(RtFile);
		RtFile=NULL;
		return nuTRUE;
	}
	nuVOID UONERtCtrl::Release_RTB_Main(nuLONG RtBIdx)
	{
		if(ReadHeaderOK==nuFALSE) return;
		if((*TmpRtB)[RtBIdx].RtBMain!=NULL)
		{
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)(*TmpRtB)[RtBIdx].RtBMain);
			(*TmpRtB)[RtBIdx].RtBMain=NULL;
		}
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
	}


	nuBOOL UONERtCtrl::Read_RTB_Connect(nuLONG RtBIdx)
	{
		nuULONG i = 0;
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		Release_RTB_Connect(RtBIdx);
		(*TmpRtB)[RtBIdx].RtBConnect=(UONEF_RtB_Connect_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_Connect_Stru)*(*TmpRtB)[RtBIdx].RtBHeader.RtConnectStruCount);
	#ifdef _DEBUG
		UoneRTAlocCount++;
	#endif
		if((*TmpRtB)[RtBIdx].RtBConnect==NULL){	return nuFALSE;	}
		for(i=0;i<(*TmpRtB)[RtBIdx].RtBHeader.RtConnectStruCount;i++)
		{
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].Connecttype=0;
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].RoadClass=0;
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].RoadLength=0;
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].TNodeID=0;
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].TRtBID=0;
			(*(*TmpRtB)[RtBIdx].RtBConnect)[i].VoiceRoadType=0;
		}

		RtFile=nuTfopen(FName,NURO_FS_RB);
		if(RtFile==NULL)
		{	Release_RTB_Connect(RtBIdx);	return nuFALSE;	}
		if(0!=nuFseek(RtFile,(*TmpRtB)[RtBIdx].RtBHeader.RtConnectStruStartAddr,NURO_SEEK_SET))
		{	Release_RTB_Connect(RtBIdx);	return nuFALSE;	}
		if((*TmpRtB)[RtBIdx].RtBHeader.RtConnectStruCount!=nuFread(*(*TmpRtB)[RtBIdx].RtBConnect,sizeof(UONEF_RtB_Connect_Stru),(*TmpRtB)[RtBIdx].RtBHeader.RtConnectStruCount,RtFile))
		{	Release_RTB_Connect(RtBIdx);	return nuFALSE;	}
		nuFclose(RtFile);
		RtFile=NULL;

		return nuTRUE;
	}
	nuVOID UONERtCtrl::Release_RTB_Connect(nuLONG RtBIdx)
	{
		if(ReadHeaderOK==nuFALSE) return;
		if((*TmpRtB)[RtBIdx].RtBConnect!=NULL)
		{
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)(*TmpRtB)[RtBIdx].RtBConnect);
			(*TmpRtB)[RtBIdx].RtBConnect=NULL;
		}
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
	}


	nuBOOL UONERtCtrl::Read_RTB_Index(nuLONG RtBIdx)
	{
		nuULONG i = 0;
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		Release_RTB_Index(RtBIdx);
		(*TmpRtB)[RtBIdx].RtBIdx=(UONEF_RtB_Index_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_Index_Stru)*(*TmpRtB)[RtBIdx].RtBHeader.TotalRoadCount);
	#ifdef _DEBUG
		UoneRTAlocCount++;
	#endif
		if((*TmpRtB)[RtBIdx].RtBIdx==NULL){	/*MessageBox(NULL,TEXT("Error Code 129-1"),TEXT("system memory not enough"),0|MB_TOPMOST);*/	return nuFALSE;	}
		for(i=0;i<(*TmpRtB)[RtBIdx].RtBHeader.TotalRoadCount;i++)
		{
			(*(*TmpRtB)[RtBIdx].RtBIdx)[i].BlockIdx=0;
			(*(*TmpRtB)[RtBIdx].RtBIdx)[i].FNodeID=0;		(*(*TmpRtB)[RtBIdx].RtBIdx)[i].FRtBID=0;
			(*(*TmpRtB)[RtBIdx].RtBIdx)[i].RoadIdx=0;
			(*(*TmpRtB)[RtBIdx].RtBIdx)[i].TNodeID=0;		(*(*TmpRtB)[RtBIdx].RtBIdx)[i].TRtBID=0;
		}

		RtFile=nuTfopen(FName,NURO_FS_RB);
		if(RtFile==NULL)
		{	Release_RTB_Index(RtBIdx);	return nuFALSE;	}
		if(0!=nuFseek(RtFile,(*TmpRtB)[RtBIdx].RtBHeader.RtIndexStruStartAddr,NURO_SEEK_SET))
		{	Release_RTB_Index(RtBIdx);	return nuFALSE;	}
		if((*TmpRtB)[RtBIdx].RtBHeader.TotalRoadCount!=nuFread(*(*TmpRtB)[RtBIdx].RtBIdx,sizeof(UONEF_RtB_Index_Stru),(*TmpRtB)[RtBIdx].RtBHeader.TotalRoadCount,RtFile))
		{	Release_RTB_Index(RtBIdx);	return nuFALSE;	}

		nuFclose(RtFile);
		RtFile=NULL;
		return nuTRUE;
	}
	nuVOID UONERtCtrl::Release_RTB_Index(nuLONG RtBIdx)
	{
		if(ReadHeaderOK==nuFALSE) return;
		if((*TmpRtB)[RtBIdx].RtBIdx!=NULL)
		{
		#ifdef _DEBUG
			UoneRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)(*TmpRtB)[RtBIdx].RtBIdx);
			(*TmpRtB)[RtBIdx].RtBIdx=NULL;
		}
		if(RtFile!=NULL){	nuFclose(RtFile);	RtFile=NULL;	}
	}
