// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:16 $
// $Revision: 1.6 $
#include "stdAfxRoute.h"

#include "RouteBase.h"
#include "Ctrl_Thr.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
Class_ThrCtrl::Class_ThrCtrl()
{
	IniConst();
#ifdef _DEBUG
	ThrAlocCount = 0;
	ThrFreeCount = 0;
#endif
}

Class_ThrCtrl::~Class_ThrCtrl()
{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Thr_MemUesd %d\n", ThrAlocCount - ThrFreeCount);
			fclose(pfile);
		}
	#endif
#endif
	CloseClass();
}

nuBOOL Class_ThrCtrl::InitClass(nuLONG mapidx,nuLONG rtrule)
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	MapIdx=mapidx;
	l_RoutingRule=rtrule;
	return nuTRUE;
}
nuVOID Class_ThrCtrl::CloseClass()
{
	ReSet();
}

nuBOOL Class_ThrCtrl::Read_Header()//讀取表頭資料
{
	nuULONG i = 0;
	#ifdef DEF_MAKETHRROUTE
		switch(l_RoutingRule)
		{
			case 0:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("0.thr"));	break;
			case 1:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("1.thr"));	break;
			case 2:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("2.thr"));	break;
			case 3:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("3.thr"));	break;
			case 4:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("4.thr"));	break;
			case 5:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("5.thr"));	break;
			case 6:		nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("6.thr"));	break;
			default:	nuTcscpy(FName, MakeThr_MapPath);	nuTcscat(FName, nuTEXT("0.thr"));	break;
		}		
	#else
		switch(l_RoutingRule)
		{
			case 0:		nuTcscpy(FName, nuTEXT("0.thr"));	break;
			case 1:		nuTcscpy(FName, nuTEXT("1.thr"));	break;
			case 2:		nuTcscpy(FName, nuTEXT("2.thr"));	break;
			case 3:		nuTcscpy(FName, nuTEXT("3.thr"));	break;
			case 4:		nuTcscpy(FName, nuTEXT("4.thr"));	break;
			case 5:		nuTcscpy(FName, nuTEXT("5.thr"));	break;
			case 6:		nuTcscpy(FName, nuTEXT("6.thr"));	break;
			default:	nuTcscpy(FName, nuTEXT("0.thr"));	break;
		}		
		g_pRtFSApi->FS_FindFileWholePath(MapIdx,FName,NURO_MAX_PATH);
	#endif

	Remove_Header();
	ThrFile=nuTfopen(FName, NURO_FS_RB);
	if(ThrFile==NULL)											{	Remove_Header();	return nuFALSE;	}
	if(NULL != nuFseek(ThrFile,0,NURO_SEEK_SET))						{	Remove_Header();	return nuFALSE;	}
	if(1!=nuFread(&ThrHeader,sizeof(Stru_ThrHeader),1,ThrFile))	{	Remove_Header();	return nuFALSE;	}

	if(ThrHeader.RTBCount!=0)
	{
		RTBConnect=(Stru_RTBConnect**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_Stru_RTBConnect)*ThrHeader.RTBCount);
		if(RTBConnect==NULL){	Remove_Header();	return nuFALSE;	}
	#ifdef _DEBUG
		ThrAlocCount++;
	#endif
		for(i=0;i<ThrHeader.RTBCount;i++)
		{
			(*RTBConnect)[i].Connheader.RTBID=0;
			(*RTBConnect)[i].Connheader.RTBConnCount=0;
			(*RTBConnect)[i].ConnnectID=NULL;
			if(1!=nuFread(&((*RTBConnect)[i].Connheader),sizeof(Stru_RTBConnectHeader),1,ThrFile))	{	Remove_Header();	return nuFALSE;	}
			if((*RTBConnect)[i].Connheader.RTBConnCount!=0)
			{
				(*RTBConnect)[i].ConnnectID=(nuULONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuULONG)*(*RTBConnect)[i].Connheader.RTBConnCount);
				if((*RTBConnect)[i].ConnnectID==NULL){	Remove_Header();	return nuFALSE;	}
			#ifdef _DEBUG
				ThrAlocCount++;
			#endif
				if((*RTBConnect)[i].Connheader.RTBConnCount!=nuFread(*(*RTBConnect)[i].ConnnectID,sizeof(nuULONG),(*RTBConnect)[i].Connheader.RTBConnCount,ThrFile))	{	Remove_Header();	return nuFALSE;	}
			}
		}
	}
	else{
		return nuTRUE;
	}
	if(1!=nuFread(&(pThrEachMap.ThrHeader),sizeof(Tag_F_Thr_Header_Stru),1,ThrFile))	{	Remove_Header();	return nuFALSE;	}
	pThrEachMap.ThrMain=(F_Thr_MainData_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_F_Thr_MainData_Stru)*pThrEachMap.ThrHeader.TotalNodeCount);
	if(pThrEachMap.ThrMain==NULL){	Remove_Header();	return nuFALSE;	}
#ifdef _DEBUG
	ThrAlocCount++;
#endif
	pThrEachMap.ThrConnect=(F_Thr_Connect_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_F_Thr_Connect_Stru)*pThrEachMap.ThrHeader.RtConnectStruCount);
	if(pThrEachMap.ThrConnect==NULL){	Remove_Header();	return nuFALSE;	}
#ifdef _DEBUG
	ThrAlocCount++;
#endif

	if(pThrEachMap.ThrHeader.TotalNodeCount!=nuFread(*pThrEachMap.ThrMain,sizeof(Tag_F_Thr_MainData_Stru),pThrEachMap.ThrHeader.TotalNodeCount,ThrFile))
	{	Remove_Header();	return nuFALSE;	}
	if(pThrEachMap.ThrHeader.RtConnectStruCount!=nuFread(*pThrEachMap.ThrConnect,sizeof(Tag_F_Thr_Connect_Stru),pThrEachMap.ThrHeader.RtConnectStruCount,ThrFile))
	{	Remove_Header();	return nuFALSE;	}
	
	if(ThrFile!=NULL){	nuFclose(ThrFile);	ThrFile=NULL;	}
	nuSleep(_SLEEPTICKCOUNT);
	return nuTRUE;
}

nuBOOL Class_ThrCtrl::Remove_Header()//移除表頭資料
{
	nuULONG i = 0;
	if(NULL!=pThrEachMap.ThrMain)
	{	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)pThrEachMap.ThrMain);	
		pThrEachMap.ThrMain=NULL;
#ifdef _DEBUG
		ThrFreeCount++;
#endif		
	}
	if(NULL!=pThrEachMap.ThrConnect)
	{	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)pThrEachMap.ThrConnect);	
		pThrEachMap.ThrConnect=NULL;	
#ifdef _DEBUG
		ThrFreeCount++;
#endif
	}

	pThrEachMap.ThrHeader.RtConnectStruCount=0;
	pThrEachMap.ThrHeader.RtConnectStruStartAddr=0;
	pThrEachMap.ThrHeader.TotalNodeCount=0;
	if(RTBConnect!=NULL){
		for(i=0;i<ThrHeader.RTBCount;i++)
		{
			if((*RTBConnect)[i].ConnnectID!=NULL)
			{	
			#ifdef _DEBUG
				ThrFreeCount++;
			#endif
				g_pRtMMApi->MM_FreeMem((nuPVOID*)(*RTBConnect)[i].ConnnectID);	
				(*RTBConnect)[i].ConnnectID=NULL;	
			}
		
		}
	#ifdef _DEBUG
		ThrFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)RTBConnect);
		RTBConnect=NULL;
	}
	if(ThrFile!=NULL){	nuFclose(ThrFile);	ThrFile=NULL;	}
	return nuTRUE;
}

nuBOOL Class_ThrCtrl::ReSet()
{
	Remove_Header();
	IniConst();
	if(ThrFile!=NULL){	nuFclose(ThrFile);	ThrFile=NULL;	}
	return nuTRUE;
}

nuVOID Class_ThrCtrl::IniConst()
{
	ThrHeader.MapID=0;
	ThrHeader.RTBCount=0;
	RTBConnect=NULL;
	nuMemset(&pThrEachMap,0,sizeof(Stru_ThrEachMap));
/*
	pThrEachMap.ThrConnect=NULL;
	pThrEachMap.ThrMain=NULL;
	pThrEachMap.ThrHeader.RtConnectStruCount=0;
	pThrEachMap.ThrHeader.RtConnectStruStartAddr=0;
	pThrEachMap.ThrHeader.TotalNodeCount=0;
*/
	ThrFile=NULL;
	MapIdx=0;
	l_RoutingRule=0;
}