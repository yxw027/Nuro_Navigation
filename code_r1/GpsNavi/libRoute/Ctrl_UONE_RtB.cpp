// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:41 $
// $Revision: 1.5 $
#include "stdAfxRoute.h"

#include "Ctrl_UONE_RtB.h"
#ifdef _DEBUG
#include "stdio.h"
#endif

	UONERtBCtrl::UONERtBCtrl()
	{
	#ifdef _DEBUG
		UoneRtBAlocCount = 0;
		UoneRtBFreeCount = 0;
	#endif
		IniConst();
	}

	UONERtBCtrl::~UONERtBCtrl()
	{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"RtB_MemUesd %d\n", UoneRtBAlocCount - UoneRtBFreeCount);
			fclose(pfile);
		}
	#endif
#endif
		CloseClass();
	}

	nuBOOL UONERtBCtrl::InitClass()
	{
		nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
		return nuTRUE;
	}
	nuVOID UONERtBCtrl::CloseClass()
	{
		ReSet();
	}

	nuBOOL UONERtBCtrl::Read_Header(nuLONG StartAddr)
	{
		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)
		{	return nuFALSE;	}
		if(0!=nuFseek(RtBFile,StartAddr,NURO_SEEK_SET))
		{	Remove_Header();	return nuFALSE;	}
		if(1!=nuFread(&(RtBHeader),sizeof(UONEF_RtB_Header_Stru),1,RtBFile))
		{	Remove_Header();	return nuFALSE;	}
		nuFclose(RtBFile);
		RtBFile = NULL;
		ReadHeaderOK = nuTRUE;
		m_StartAddr=StartAddr;
		return nuTRUE;
	}
	nuVOID UONERtBCtrl::Remove_Header()
	{
		RtBHeader.RtConnectStruStartAddr=-1;
		RtBHeader.RtIndexStruStartAddr=-1;
		RtBHeader.RtConnectStruCount=-1;
		RtBHeader.TotalNodeCount=-1;
		RtBHeader.TotalRoadCount=-1;
		ReadHeaderOK=nuFALSE;
		m_StartAddr=0;

		MyState.DwBlockID=-1;
		MyState.DwBlockIdx=-1;
		MyState.MapID=-1;
		MyState.MapIdx=-1;
		MyState.RtBlockID=-1;
		MyState.RtBlockIdx=-1;

		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL UONERtBCtrl::Read_MainData()
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBMain=(UONEF_RtB_MainData_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_MainData_Stru)*RtBHeader.TotalNodeCount);
		if(RtBMain==NULL){	return nuFALSE;	}
	#ifdef _DEBUG
		UoneRtBAlocCount++;
	#endif
		RtBFile=nuTfopen(FName, NURO_FS_RB);
		if(RtBFile==NULL)
		{	Remove_MainData();	return nuFALSE;	}
		if(0!=nuFseek(RtBFile,m_StartAddr+sizeof(UONEF_RtB_Header_Stru),NURO_SEEK_SET))
		{	Remove_MainData();	return nuFALSE;	}
		if(RtBHeader.TotalNodeCount!=nuFread(*RtBMain,sizeof(Tag_UONEF_RtB_MainData_Stru),RtBHeader.TotalNodeCount,RtBFile))
		{	Remove_MainData();	return nuFALSE;	}

		nuFclose(RtBFile);
		RtBFile=NULL;
		nuSleep(_SLEEPTICKCOUNT);
		return nuTRUE;
	}
	nuVOID UONERtBCtrl::Remove_MainData()
	{
		if(RtBMain!=NULL)
		{
		#ifdef _DEBUG
			UoneRtBFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBMain);
			RtBMain=NULL;
		}
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL UONERtBCtrl::Read_Connect()//產生聯通資料儲存陣列
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBConnect=(UONEF_RtB_Connect_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_Connect_Stru)*RtBHeader.RtConnectStruCount);
	#ifdef _DEBUG
		UoneRtBAlocCount++;
	#endif
		if(RtBConnect==NULL){	return nuFALSE;	}

		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)
		{	Remove_Connect();	return nuFALSE;	}
		if(0!=nuFseek(RtBFile,RtBHeader.RtConnectStruStartAddr,NURO_SEEK_SET))
		{	Remove_Connect();	return nuFALSE;	}
		if(RtBHeader.RtConnectStruCount!=nuFread(*RtBConnect,sizeof(Tag_UONEF_RtB_Connect_Stru),RtBHeader.RtConnectStruCount,RtBFile))
		{	Remove_Connect();	return nuFALSE;	}

		nuFclose(RtBFile);
		RtBFile=NULL;

		nuSleep(_SLEEPTICKCOUNT);
		return nuTRUE;
	}

	nuVOID UONERtBCtrl::Remove_Connect()//移除聯通資料儲存陣列
	{
		if(RtBConnect!=NULL)
		{
		#ifdef _DEBUG
			UoneRtBFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBConnect);
			RtBConnect=NULL;
		}
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL UONERtBCtrl::Read_Index()
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBIdx=(UONEF_RtB_Index_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_UONEF_RtB_Index_Stru)*RtBHeader.TotalRoadCount);
		if(RtBIdx==NULL){	return nuFALSE;	}
	#ifdef _DEBUG
		UoneRtBAlocCount++;
	#endif
		RtBFile=nuTfopen(FName, NURO_FS_RB);
		if(RtBFile==NULL)
		{	Remove_Index();	return nuFALSE;	}
		if(0!=nuFseek(RtBFile,RtBHeader.RtIndexStruStartAddr,NURO_SEEK_SET))
		{	Remove_Index();	return nuFALSE;	}
		if(RtBHeader.TotalRoadCount!=nuFread(*RtBIdx,sizeof(UONEF_RtB_Index_Stru),RtBHeader.TotalRoadCount,RtBFile))
		{	Remove_Index();	return nuFALSE;	}

		nuFclose(RtBFile);
		RtBFile=NULL;

		nuSleep(_SLEEPTICKCOUNT);
		return nuTRUE;
	}
	nuVOID UONERtBCtrl::Remove_Index()
	{
		if(RtBIdx!=NULL)
		{	
		#ifdef _DEBUG
			UoneRtBFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBIdx);
			RtBIdx=NULL;
		}
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL UONERtBCtrl::ReSet()//移除Rt檔
	{
		Remove_Index();
		Remove_Connect();
		Remove_MainData();
		Remove_Header();
		IniConst();
		return nuTRUE;
	}

	nuVOID UONERtBCtrl::IniConst()
	{
		ReadHeaderOK=nuFALSE;
		m_StartAddr=0;
		
		RtBMain=NULL;//Rt中的主資料
		RtBConnect=NULL;//Rt中的連結資料
		RtBIdx=NULL;//Rt中RoadID與NodeID的對應資料
		RtBFile=NULL;
	}
