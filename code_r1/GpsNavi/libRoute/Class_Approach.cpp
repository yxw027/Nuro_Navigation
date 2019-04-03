// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:47:23 $
// $Revision: 1.2 $
#include "stdAfxRoute.h"
#include "Class_Approach.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
//Library
#include "NuroDefine.h"

Class_Approach::Class_Approach()
{
	ApproachCount=0;
	p_Approach=NULL;
#ifdef _DEBUG
	ApproachAlocCount = 0;
	ApproachFreeCount = 0;
#endif
}

Class_Approach::~Class_Approach()
{
	ReleaseApproach();
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"Approach_MemUesd %d\n", ApproachAlocCount - ApproachFreeCount);
		fclose(pfile);
	}
#endif
#endif
}

nuBOOL Class_Approach::CreateApproach(nuLONG Count)
{
	if(Count<0) return nuFALSE;
	ReleaseApproach();
	ApproachCount=Count;
	p_Approach=(ApproachStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_ApproachStru)*ApproachCount);
	
#ifdef _DEBUG
	ApproachAlocCount++;
#endif
	if(p_Approach==NULL){	ReleaseApproach();	return nuFALSE;	}
	else				{	return nuTRUE;	}
}

nuBOOL Class_Approach::ReleaseApproach()
{
	if(p_Approach!=NULL){
#ifdef _DEBUG
		ApproachFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_Approach);
		p_Approach=NULL;
	}
	ApproachCount=0;
	return nuTRUE;
}
