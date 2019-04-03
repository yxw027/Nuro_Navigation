// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:31:56 $
// $Revision: 1.4 $
#include "stdAfxRoute.h"

#include "Ctrl_MDW.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
CMDWCtrl::CMDWCtrl(){
	InitConst();
#ifdef _DEBUG
	MDWFreeCount = 0;
	MDWAlocCount = 0;
#endif
}
CMDWCtrl::~CMDWCtrl(){
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"MDW_MemUesd %d\n", MDWAlocCount - MDWFreeCount);
		fclose(pfile);
	}
#endif
#endif
	CloseClass();
}

nuBOOL CMDWCtrl::InitClass(){
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID CMDWCtrl::CloseClass(){
	Reset();	
}

nuVOID CMDWCtrl::InitConst(){
	DWFile=NULL;
	ReadHeaderOK=nuFALSE;
	ReadBlockOK=NULL;//判斷是否有讀取Block資料
	Rd=NULL;
	Header.m_ClassCnt=0;
	Header.m_TotalBlockCount=0;
	Header.p_BlockClassStartAddr=NULL;
	Header.p_BlockDataSize=NULL;
	Header.p_BlockRoadAddCount=NULL;
	Header.p_BlockRoadDataAddr=NULL;
	Header.p_GBlockID=NULL;
}

nuBOOL CMDWCtrl::Read_Header(){//讀取表頭資料
	nuULONG  i = 0;
	Remove_Header();
	ReadHeaderOK=nuTRUE;

	DWFile=nuTfopen(FName, NURO_FS_RB);
	if(DWFile==NULL){	
		return nuFALSE;	}	
	if(0!=nuFseek(DWFile,sizeof(nuULONG),NURO_SEEK_SET)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	if(1!=nuFread(&(Header.m_TotalBlockCount),sizeof(nuULONG),1,DWFile)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}

	//創建資料
	Header.p_GBlockID = (nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.m_TotalBlockCount);
	if(Header.p_GBlockID==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	Header.p_BlockRoadDataAddr = (nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.m_TotalBlockCount);
	if(Header.p_BlockRoadDataAddr==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	Header.p_BlockRoadAddCount = (nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.m_TotalBlockCount);
	if(Header.p_BlockRoadAddCount==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	Header.p_BlockDataSize = (nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.m_TotalBlockCount);
	if(Header.p_BlockDataSize==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	ReadBlockOK = (nuBOOL**)g_pRtMMApi->MM_AllocMem(sizeof(nuBOOL)*Header.m_TotalBlockCount);
	if(ReadBlockOK==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}

	if(Header.m_TotalBlockCount!=nuFread(*Header.p_GBlockID,sizeof(nuLONG),Header.m_TotalBlockCount,DWFile)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	if(Header.m_TotalBlockCount!=nuFread(*Header.p_BlockRoadDataAddr,sizeof(nuLONG),Header.m_TotalBlockCount,DWFile))	{
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	if(Header.m_TotalBlockCount!=nuFread(*Header.p_BlockRoadAddCount,sizeof(nuLONG),Header.m_TotalBlockCount,DWFile)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	if(Header.m_TotalBlockCount!=nuFread(*Header.p_BlockDataSize,sizeof(nuLONG),Header.m_TotalBlockCount,DWFile)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	for(i=0;i<Header.m_TotalBlockCount;i++)
	{	(*ReadBlockOK)[i]=nuFALSE;	}

	if(1!=nuFread(&(Header.m_ClassCnt),sizeof(nuULONG),1,DWFile))	{
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}
	Header.p_BlockClassStartAddr = (nuLONG**)g_pRtMMApi->MM_AllocMem(sizeof(nuLONG)*Header.m_ClassCnt);
	if(Header.p_BlockClassStartAddr==NULL)
	{
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_Header();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	if(Header.m_ClassCnt!=nuFread(*Header.p_BlockClassStartAddr,sizeof(nuLONG),Header.m_ClassCnt,DWFile)){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;	}

	if(CreateMainStru(Header.m_TotalBlockCount)==nuFALSE){
		nuFclose(DWFile);	DWFile=NULL;	Remove_Header();	return nuFALSE;		}
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	return nuTRUE;
}
nuVOID CMDWCtrl::Remove_Header(){//移除表頭資料
	ReleaseMainStru();//完整移除資料區塊

	if(Header.p_BlockClassStartAddr!=NULL)
	{
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.p_BlockClassStartAddr);		
		Header.p_BlockClassStartAddr=NULL;	
	}//地標點起始位置串
	if(ReadBlockOK!=NULL)				
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ReadBlockOK);				
		ReadBlockOK=NULL;	
	}
	if(Header.p_BlockDataSize!=NULL)
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.p_BlockDataSize);		
		Header.p_BlockDataSize=NULL;	
	}//地標點起始位置串
	if(Header.p_BlockRoadAddCount!=NULL)
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.p_BlockRoadAddCount);		
		Header.p_BlockRoadAddCount=NULL;	
	}//地標點起始位置串
	if(Header.p_BlockRoadDataAddr!=NULL)
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.p_BlockRoadDataAddr);		
		Header.p_BlockRoadDataAddr=NULL;	
	}//地標點起始位置串
	if(Header.p_GBlockID!=NULL)
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Header.p_GBlockID);		
		Header.p_GBlockID=NULL;	
	}//地標點起始位置串

	Header.m_ClassCnt=0;
	Header.m_TotalBlockCount=0;
	ReadHeaderOK=nuFALSE;
}

nuBOOL CMDWCtrl::CreateMainStru(nuULONG Count){
	nuULONG i = 0;
	if(Count<=0)	return nuFALSE;
	ReleaseMainStru();
	Rd = (MDWBlockRoadData**)g_pRtMMApi->MM_AllocMem(sizeof(MDWBlockRoadData)*Count);

	if(Rd==NULL )
	{	
		ReleaseMainStru();	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount++;
#endif
	}
	for(i=0;i<Count;i++){
		(*Rd)[i].p_RdData=NULL;
		(*Rd)[i].m_RoadCount=0;
		(*Rd)[i].p_VertexCoor=NULL;
		(*Rd)[i].m_VertexCount=0;
	}
	return nuTRUE;
}
nuVOID CMDWCtrl::ReleaseMainStru()
{
	nuULONG i;
	for(i=0;i<Header.m_TotalBlockCount;i++){
		Remove_BlockData(i);
	}
	if(Rd!=NULL)	
	{
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Rd);		
		Rd=NULL;	
	}
}

nuBOOL CMDWCtrl::Read_BlockData(nuULONG Index)
{
	if(ReadHeaderOK==nuFALSE)					return nuFALSE;
	if(Header.m_TotalBlockCount<=Index)		return nuFALSE;
	if((*ReadBlockOK)[Index]==nuTRUE)			return nuTRUE;
	if(!Read_RoadData(Index)){	Remove_BlockData(Index);	return nuFALSE;	}//讀取道路資料
	nuSleep(_SLEEPTICKCOUNT);
	return nuTRUE;
}
nuVOID CMDWCtrl::Remove_BlockData(nuULONG Index)
{
	if(ReadBlockOK==NULL)							return;	
	if(Header.m_TotalBlockCount<=Index)	return;
	if((*ReadBlockOK)[Index]==nuFALSE)				return;
	if(Rd!=NULL)		Remove_RoadData(Index);//移除道路資料
	(*ReadBlockOK)[Index]=nuFALSE;
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	return;
}

nuBOOL CMDWCtrl::Read_RoadData(nuULONG Index)//讀取道路資料
{
	if(ReadHeaderOK==nuFALSE)					return nuFALSE;
	if(Rd==NULL)							return nuFALSE;
	if(Header.m_TotalBlockCount<=Index)		return nuFALSE;
	if((*ReadBlockOK)[Index]==nuTRUE)			return nuTRUE;

	(*Rd)[Index].p_RdData=NULL;
	(*Rd)[Index].m_RoadCount=0;
	(*Rd)[Index].p_VertexCoor=NULL;
	(*Rd)[Index].m_VertexCount=0;
	if((*Header.p_BlockRoadDataAddr)[Index]==0 || (*Header.p_BlockRoadDataAddr)[Index]==-1 ) return nuFALSE;//return nuTRUE;

	DWFile=nuTfopen(FName,NURO_FS_RB);
	if(DWFile==NULL){	Remove_RoadData(Index);		return nuFALSE;	}
	if(0!=nuFseek(DWFile,(*Header.p_BlockRoadDataAddr)[Index],NURO_SEEK_SET))
	{	nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}
	if(1!=nuFread(&((*Rd)[Index].m_RoadCount),sizeof(nuLONG),1,DWFile))
	{	nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}
	if(1!=nuFread(&((*Rd)[Index].m_VertexCount),sizeof(nuLONG),1,DWFile))
	{	nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}
	
	(*Rd)[Index].p_RdData=(MDWRoadData**)g_pRtMMApi->MM_AllocMem(sizeof(TagMDWRoadData)*(*Rd)[Index].m_RoadCount);
	(*Rd)[Index].p_VertexCoor=(NUROPOINT**)g_pRtMMApi->MM_AllocMem(sizeof(NUROPOINT)*(*Rd)[Index].m_VertexCount);

	if((*Rd)[Index].p_RdData==NULL || (*Rd)[Index].p_VertexCoor==NULL)
	{	
		nuFclose(DWFile);	
		DWFile=NULL;	
		Remove_RoadData(Index);	
		return nuFALSE;	
	}
	else
	{
#ifdef _DEBUG
		MDWAlocCount += 2;
#endif
	}

	if(0!=nuFseek(DWFile,(*Header.p_BlockRoadDataAddr)[Index]+8,NURO_SEEK_SET))
	{nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}
	if((*Rd)[Index].m_RoadCount!=nuFread(*((*Rd)[Index].p_RdData),sizeof(TagMDWRoadData),(*Rd)[Index].m_RoadCount,DWFile))
	{	nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}
	if((*Rd)[Index].m_VertexCount!=nuFread(*((*Rd)[Index].p_VertexCoor),sizeof(NUROPOINT),(*Rd)[Index].m_VertexCount,DWFile))
	{	nuFclose(DWFile);	DWFile=NULL;	Remove_RoadData(Index);	return nuFALSE;	}

	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	(*ReadBlockOK)[Index]=nuTRUE;
	return nuTRUE;
}
nuVOID CMDWCtrl::Remove_RoadData(nuULONG Index)//讀取道路資料
{
	if(ReadHeaderOK==nuFALSE)	return;
	if(Rd==NULL) return;
	if(Header.m_TotalBlockCount<=Index) return;
	if((*ReadBlockOK)[Index]==nuFALSE)		  return;

	if((*Rd)[Index].p_VertexCoor!=NULL)	
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)((*Rd)[Index].p_VertexCoor));	
		(*Rd)[Index].p_VertexCoor=NULL;	
	}
	if((*Rd)[Index].p_RdData!=NULL)		
	{	
	#ifdef _DEBUG
		MDWFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)((*Rd)[Index].p_RdData));		
		(*Rd)[Index].p_RdData=NULL;		
	}
	(*Rd)[Index].p_RdData=NULL;
	(*Rd)[Index].m_RoadCount=0;
	(*Rd)[Index].p_VertexCoor=NULL;
	(*Rd)[Index].m_VertexCount=0;
	(*ReadBlockOK)[Index]=nuFALSE;
	return;
}

nuBOOL CMDWCtrl::Reset()
{
	Remove_Header();
	if(DWFile!=NULL){	nuFclose(DWFile);	DWFile=NULL;	}
	InitConst();
	return nuTRUE;
}
