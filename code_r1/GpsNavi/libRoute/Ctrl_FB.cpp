// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:18:09 $
// $Revision: 1.3 $
#include "stdAfxRoute.h"
#include "Ctrl_FB.h"

FBCtrl::FBCtrl()
{
	SegmentCount=0;//¬q¸¨¼Æ
	BS=NULL;//Block¬q¸¨
	FB=NULL;
}

FBCtrl::~FBCtrl()
{
	CloseClass();
}

nuBOOL FBCtrl::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID FBCtrl::CloseClass()
{
	ReleaseFB();
}

nuBOOL FBCtrl::ReadFB()
{
	ReleaseFB();

	FB=nuTfopen(FName, NURO_FS_RB);
	if(FB==NULL){
		ReleaseFB();
		return nuFALSE;
	}
	if(0!=nuFseek(FB,0,NURO_SEEK_SET)){
		ReleaseFB();
		return nuFALSE;
	}
	if(1!=nuFread(&SegmentCount,sizeof(nuLONG),1,FB)){
		ReleaseFB();
		return nuFALSE;
	}
	BS=(BlockSegmentStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_BlockSegmentStru)*SegmentCount);
	if(BS==NULL){
		ReleaseFB();
		return nuFALSE;
	}
	if(0!=nuFseek(FB,sizeof(nuLONG),NURO_SEEK_SET)){
		ReleaseFB();
		return nuFALSE;
	}
	if(SegmentCount!=nuFread(*BS,sizeof(Tag_BlockSegmentStru),SegmentCount,FB)){
		ReleaseFB();
		return nuFALSE;
	}
	nuFclose(FB);
	FB=NULL;
	return nuTRUE;
}

nuVOID FBCtrl::ReleaseFB()
{
	if(BS!=NULL){	g_pRtMMApi->MM_FreeMem((nuPVOID*)BS);		BS=NULL;	}
	if(FB!=NULL){	nuFclose(FB);		FB=NULL;	}
	SegmentCount=0;
}
