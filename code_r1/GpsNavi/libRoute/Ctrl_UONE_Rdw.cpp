// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:31 $
// $Revision: 1.6 $
#include "stdAfxRoute.h"

#include "AllSystemDataBase.h"
#include "Ctrl_UONE_Rdw.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
UONERDwCtrl::UONERDwCtrl()
{
	InitConst();
#ifdef _DEBUG
	UoneRdwAlocCount = 0;
	UoneRdwFreeCount = 0;
#endif
}
UONERDwCtrl::~UONERDwCtrl()
{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Rdw_MemUesd %d\n", UoneRdwAlocCount - UoneRdwFreeCount);
			fclose(pfile);
		}
	#endif
#endif
	CloseClass();
}

nuBOOL UONERDwCtrl::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	RDWFileIdx=-1;
	return nuTRUE;
}
nuVOID UONERDwCtrl::CloseClass()
{
	Reset();	
}


nuVOID UONERDwCtrl::InitConst()
{
	DWFile=NULL;
	ReadHeaderOK=nuFALSE;
	ReadBlockOK=NULL;//判斷是否有讀取Block資料
	Rd=NULL;
	Header.ClassCnt=0;
	Header.BlockClassStart=NULL;
	RDWFileIdx=-1;
}

nuBOOL UONERDwCtrl::Read_Header()//讀取表頭資料
{
	nuULONG  i = 0;
	Remove_Header();
	ReadHeaderOK=nuTRUE;

	DWFile=nuTfopen(FName, NURO_FS_RB);
	if(DWFile==NULL){
		return nuFALSE;
	}
	if(NULL != nuFseek(DWFile,0,NURO_SEEK_SET)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}
	if(1!=nuFread(&(Header.ClassCnt),sizeof(nuULONG),1,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}

	ReadBlockOK = (nuBOOL**)g_pRtMMApi->MM_AllocMem(sizeof(nuBOOL)*g_pUOneBH->m_BH.m_BHH.BlockCount);
	if(ReadBlockOK==NULL){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}
#ifdef _DEBUG
	UoneRdwAlocCount++;
#endif
	for(i=0;i<g_pUOneBH->m_BH.m_BHH.BlockCount;i++){
		(*ReadBlockOK)[i]=nuFALSE;
	}

	Header.BlockClassStart	=	(nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.ClassCnt*g_pUOneBH->m_BH.m_BHH.BlockCount);
	if(Header.BlockClassStart==NULL){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}
#ifdef _DEBUG
	UoneRdwAlocCount++;
#endif
	if(CreateMainStru(g_pUOneBH->m_BH.m_BHH.BlockCount)==nuFALSE){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}

	if(NULL != nuFseek(DWFile,sizeof(nuULONG),NURO_SEEK_SET)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}
	if(g_pUOneBH->m_BH.m_BHH.BlockCount!=nuFread(*Header.BlockClassStart,sizeof(nuLONG),g_pUOneBH->m_BH.m_BHH.BlockCount,DWFile))	{
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_Header();
		return nuFALSE;
	}

	if(DWFile!=NULL){
		nuFclose(DWFile);
		DWFile=NULL;
	}
	return nuTRUE;
}
nuVOID UONERDwCtrl::Remove_Header()//移除表頭資料
{
	ReleaseMainStru();//完整移除資料區塊
	if(Header.BlockClassStart!=NULL)	
	{	
#ifdef _DEBUG
		UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.BlockClassStart);	
		Header.BlockClassStart=NULL;
	}//地標點起始位置串
	if(ReadBlockOK!=NULL)				
	{	
	#ifdef _DEBUG
		UoneRdwFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ReadBlockOK);				
		ReadBlockOK=NULL;	
	}	
	Header.ClassCnt=0;
	Header.BlockClassStart=NULL;

	ReadHeaderOK=nuFALSE;
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
}

nuBOOL UONERDwCtrl::CreateMainStru(nuULONG Count)
{
	nuULONG i = 0;
	if(Count<=0)	return nuFALSE;
	ReleaseMainStru();
	Rd = (UONEBlockRoadData**)g_pRtMMApi->MM_AllocMem(sizeof(UONEBlockRoadData)*Count);
	if( Rd==NULL ){
		ReleaseMainStru();
		return nuFALSE;
	}
#ifdef _DEBUG
	UoneRdwAlocCount++;
#endif
	for(i=0;i<Count;i++){
		(*Rd)[i].RdBoundary=NULL;
		(*Rd)[i].RdCommCount=0;
		(*Rd)[i].RdCommData=NULL;
		(*Rd)[i].RdData=NULL;
		(*Rd)[i].RoadCount=0;
		(*Rd)[i].VertexCoor=NULL;
		(*Rd)[i].VertexCount=0;
	}
	return nuTRUE;
}
nuVOID UONERDwCtrl::ReleaseMainStru()
{
	nuULONG i = 0;
	for(i=0;i<g_pUOneBH->m_BH.m_BHH.BlockCount;i++){
		Remove_BlockData(i);
	}
	if(Rd!=NULL)	
	{	
#ifdef _DEBUG
	UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Rd);		
		Rd=NULL;	
	}
}

nuBOOL UONERDwCtrl::Read_BlockData(nuULONG Index)
{
	if(ReadHeaderOK==nuFALSE)						return nuFALSE;
	if(g_pUOneBH->m_BH.m_BHH.BlockCount<=Index)	return nuFALSE;
	if((*ReadBlockOK)[Index]==nuTRUE)				return nuTRUE;
	Remove_BlockData(Index);
	if(!Read_RoadData(Index)){	Remove_BlockData(Index);	return nuFALSE;	}//讀取道路資料
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
//	nuSleep(10);
	return nuTRUE;
}
nuVOID UONERDwCtrl::Remove_BlockData(nuULONG Index)
{
	if(ReadBlockOK==NULL)						return;	
	if(g_pUOneBH->m_BH.m_BHH.BlockCount<=Index)	return;
	if((*ReadBlockOK)[Index]==nuFALSE)			return;
	if(Rd!=NULL)		Remove_RoadData(Index);//移除道路資料
	(*ReadBlockOK)[Index]=nuFALSE;
	return;
}

nuBOOL UONERDwCtrl::Read_RoadData(nuULONG Index)//讀取道路資料
{
	nuLONG  j = 0;
	nuULONG i = 0;
	NUROPOINT BHCoor = {0};
	if(ReadHeaderOK==nuFALSE)	return nuFALSE;
	if(Rd==NULL) return nuFALSE;
	if(g_pUOneBH->m_BH.m_BHH.BlockCount<=Index) return nuFALSE;
	if((*ReadBlockOK)[Index]==nuTRUE)			  return nuTRUE;

	(*Rd)[Index].RdBoundary=NULL;
	(*Rd)[Index].RdCommCount=0;
	(*Rd)[Index].RdCommData=NULL;
	(*Rd)[Index].RdData=NULL;
	(*Rd)[Index].RoadCount=0;
	(*Rd)[Index].VertexCoor=NULL;
	(*Rd)[Index].VertexCount=0;
	nuLONG SSS=g_pUOneBH->RoadDataAddr(Index);
	if(g_pUOneBH->RoadDataAddr(Index)==0 || g_pUOneBH->RoadDataAddr(Index)==-1 ){
		return nuFALSE;//return nuTRUE;
	}

	BHCoor=g_pUOneBH->BlockBaseCoor(Index);

	DWFile=nuTfopen(FName,NURO_FS_RB);
	if(DWFile==NULL){
		nuFclose(DWFile);
		DWFile=NULL;
		return nuFALSE;
	}

	if(NULL != nuFseek(DWFile,g_pUOneBH->RoadDataAddr(Index),NURO_SEEK_SET)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		return nuFALSE;
	}
	if(1!=nuFread(&((*Rd)[Index].RoadCount),sizeof(nuLONG),1,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	if(1!=nuFread(&((*Rd)[Index].RdCommCount),sizeof(nuLONG),1,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	if(1!=nuFread(&((*Rd)[Index].VertexCount),sizeof(nuLONG),1,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}

	(*Rd)[Index].RdData=(UONERoadData**)g_pRtMMApi->MM_AllocMem(sizeof(TagUONERoadData)*(*Rd)[Index].RoadCount);
	(*Rd)[Index].RdBoundary=(NURORECT**)g_pRtMMApi->MM_AllocMem(sizeof(nuroRECT)*(*Rd)[Index].RoadCount);
	(*Rd)[Index].RdCommData=(UONERoadCommonData**)g_pRtMMApi->MM_AllocMem(sizeof(TagUONERoadCommonData)*(*Rd)[Index].RdCommCount);
	(*Rd)[Index].VertexCoor=(NUROSPOINT**)g_pRtMMApi->MM_AllocMem(sizeof(nuroSPOINT)*(*Rd)[Index].VertexCount);

	if((*Rd)[Index].RdData==NULL || (*Rd)[Index].RdBoundary==NULL || (*Rd)[Index].RdCommData==NULL || (*Rd)[Index].VertexCoor==NULL){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
#ifdef _DEBUG
	UoneRdwAlocCount += 4;
#endif
	if(NULL != nuFseek(DWFile,g_pUOneBH->RoadDataAddr(Index)+12,NURO_SEEK_SET)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	if((*Rd)[Index].RoadCount!=nuFread(*(*Rd)[Index].RdData,sizeof(UONERoadData),(*Rd)[Index].RoadCount,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	if((*Rd)[Index].RdCommCount!=nuFread(*(*Rd)[Index].RdCommData,sizeof(UONERoadCommonData),(*Rd)[Index].RdCommCount,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	if((*Rd)[Index].VertexCount!=nuFread(*(*Rd)[Index].VertexCoor,sizeof(nuroSPOINT),(*Rd)[Index].VertexCount,DWFile)){
		if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
		Remove_RoadData(Index);
		return nuFALSE;
	}
	
	nuLONG TmpNum = 0;
	for(i=0;i<(*Rd)[Index].RoadCount;i++)
	{
		(*(*Rd)[Index].RdBoundary)[i].left		=	(*(*Rd)[Index].VertexCoor)[(*(*Rd)[Index].RdData)[i].StartVertexIndex].x;
		(*(*Rd)[Index].RdBoundary)[i].top		=	(*(*Rd)[Index].VertexCoor)[(*(*Rd)[Index].RdData)[i].StartVertexIndex].y;
		(*(*Rd)[Index].RdBoundary)[i].right		=	(*(*Rd)[Index].VertexCoor)[(*(*Rd)[Index].RdData)[i].StartVertexIndex].x;
		(*(*Rd)[Index].RdBoundary)[i].bottom	=	(*(*Rd)[Index].VertexCoor)[(*(*Rd)[Index].RdData)[i].StartVertexIndex].y;
		for(j=1;j<(*(*Rd)[Index].RdData)[i].VertexCount;j++)
		{
			TmpNum=(*(*Rd)[Index].RdData)[i].StartVertexIndex+j;
			if((*(*Rd)[Index].RdBoundary)[i].left>(*(*Rd)[Index].VertexCoor)[TmpNum].x)		(*(*Rd)[Index].RdBoundary)[i].left	=(*(*Rd)[Index].VertexCoor)[TmpNum].x;
			if((*(*Rd)[Index].RdBoundary)[i].top>(*(*Rd)[Index].VertexCoor)[TmpNum].y)		(*(*Rd)[Index].RdBoundary)[i].top	=(*(*Rd)[Index].VertexCoor)[TmpNum].y;
			if((*(*Rd)[Index].RdBoundary)[i].right<(*(*Rd)[Index].VertexCoor)[TmpNum].x)	(*(*Rd)[Index].RdBoundary)[i].right	=(*(*Rd)[Index].VertexCoor)[TmpNum].x;
			if((*(*Rd)[Index].RdBoundary)[i].bottom<(*(*Rd)[Index].VertexCoor)[TmpNum].y)	(*(*Rd)[Index].RdBoundary)[i].bottom=(*(*Rd)[Index].VertexCoor)[TmpNum].y;
		}

		(*(*Rd)[Index].RdBoundary)[i].left		+=	BHCoor.x;
		(*(*Rd)[Index].RdBoundary)[i].top		+=	BHCoor.y;
		(*(*Rd)[Index].RdBoundary)[i].right		+=	BHCoor.x;
		(*(*Rd)[Index].RdBoundary)[i].bottom	+=	BHCoor.y;
	}
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	(*ReadBlockOK)[Index]=nuTRUE;
	nuSleep(_SLEEPTICKCOUNT);
	return nuTRUE;
}
nuVOID UONERDwCtrl::Remove_RoadData(nuULONG Index)//讀取道路資料
{
	if(ReadHeaderOK==nuFALSE)	return;
	if(Rd==NULL) return;
	if(g_pUOneBH->m_BH.m_BHH.BlockCount<=Index) return;
	if((*ReadBlockOK)[Index]==nuFALSE)		  return;

	if((*Rd)[Index].VertexCoor!=NULL)	
	{	
#ifdef _DEBUG
		UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)(*Rd)[Index].VertexCoor);	
		(*Rd)[Index].VertexCoor=NULL;	
	}
	if((*Rd)[Index].RdCommData!=NULL)	
	{
#ifdef _DEBUG
		UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)(*Rd)[Index].RdCommData);	
		(*Rd)[Index].RdCommData=NULL;	
	}
	if((*Rd)[Index].RdBoundary!=NULL)	
	{
#ifdef _DEBUG
		UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)(*Rd)[Index].RdBoundary);	
		(*Rd)[Index].RdBoundary=NULL;	
	}
	if((*Rd)[Index].RdData!=NULL)		
	{	
#ifdef _DEBUG
		UoneRdwFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)(*Rd)[Index].RdData);		
		(*Rd)[Index].RdData=NULL;		
	}
	(*Rd)[Index].RdBoundary=NULL;
	(*Rd)[Index].RdCommCount=0;
	(*Rd)[Index].RdCommData=NULL;
	(*Rd)[Index].RdData=NULL;
	(*Rd)[Index].RoadCount=0;
	(*Rd)[Index].VertexCoor=NULL;
	(*Rd)[Index].VertexCount=0;
	(*ReadBlockOK)[Index]=nuFALSE;
	return;
}

nuBOOL UONERDwCtrl::GetCityTownID(nuULONG Now_BlockIdx,nuULONG Now_RoadIdx,nuULONG &NowCityID,nuULONG &NowTownID)
{
	if(g_pUOneBH->m_BH.m_BHH.BlockCount<=Now_BlockIdx) return nuFALSE;

	UONEBlockRoadData	TmpBLOCKRDDATA = {0};
	UONERoadData		TmpRDDATA	    = {0};
	UONERoadCommonData	TmpRDCOMMONDATA = {0};
	TmpBLOCKRDDATA.RdBoundary=NULL;
	TmpBLOCKRDDATA.RdCommCount=0;
	TmpBLOCKRDDATA.RdCommData=NULL;
	TmpBLOCKRDDATA.RdData=NULL;
	TmpBLOCKRDDATA.RoadCount=0;
	TmpBLOCKRDDATA.VertexCoor=NULL;
	TmpBLOCKRDDATA.VertexCount=0;
	if(g_pUOneBH->RoadDataAddr(Now_BlockIdx)==0 || g_pUOneBH->RoadDataAddr(Now_BlockIdx)==-1 ) return nuFALSE;//return nuTRUE;

	DWFile=nuTfopen(FName,NURO_FS_RB);
	if(DWFile==NULL){
		return nuFALSE;
	}
	if(NULL != nuFseek(DWFile,g_pUOneBH->RoadDataAddr(Now_BlockIdx),NURO_SEEK_SET))
	{return nuFALSE;}
	if(1 != nuFread(&(TmpBLOCKRDDATA.RoadCount),sizeof(nuLONG),1,DWFile))
	{return nuFALSE;}
	if(1 != nuFread(&(TmpBLOCKRDDATA.RdCommCount),sizeof(nuLONG),1,DWFile))
	{return nuFALSE;}
	if(1 != nuFread(&(TmpBLOCKRDDATA.VertexCount),sizeof(nuLONG),1,DWFile))
	{return nuFALSE;}
	if(NULL != nuFseek(DWFile,g_pUOneBH->RoadDataAddr(Now_BlockIdx)+12+sizeof(UONERoadData)*Now_RoadIdx,NURO_SEEK_SET))
	{return nuFALSE;}
	if(1 != nuFread(&TmpRDDATA,sizeof(UONERoadData),1,DWFile))
	{return nuFALSE;}
	if(NULL != nuFseek(DWFile,g_pUOneBH->RoadDataAddr(Now_BlockIdx)+12+sizeof(UONERoadData)*TmpBLOCKRDDATA.RoadCount+sizeof(UONERoadCommonData)*TmpRDDATA.RoadCommonIndex,NURO_SEEK_SET))
	{return nuFALSE;}	
	if(1 != nuFread(&TmpRDCOMMONDATA,sizeof(UONERoadCommonData),1,DWFile))
	{return nuFALSE;}	

	NowCityID=TmpRDCOMMONDATA.CityID;
	NowTownID=TmpRDCOMMONDATA.TownID;

	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	return nuTRUE;
}


nuBOOL UONERDwCtrl::Reset()
{
	Remove_Header();
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	InitConst();
	return nuTRUE;
}
