// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:18:28 $
// $Revision: 1.3 $

#include "stdAfxRoute.h"

#include "Ctrl_PTS.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
CCtrl_PTS::CCtrl_PTS()
{
	PTSStruCount=0;
	pPTSStru=NULL;
	PTSFile=NULL;
#ifdef _DEBUG
	PTSAlocCount = 0;
	PTSFreeCount = 0;
#endif
}

CCtrl_PTS::~CCtrl_PTS()
{
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"NT_MemUesd %d\n", PTSAlocCount - PTSFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
	CloseClass();
}

nuBOOL CCtrl_PTS::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID CCtrl_PTS::CloseClass()
{
	ReleasePTS();
}


nuBOOL CCtrl_PTS::ReadPTSFile()
{
	ReleasePTS();
	PTSFile=nuTfopen(FName, NURO_FS_RB);
	if(PTSFile==NULL)																{	ReleasePTS();	return nuFALSE;	}
	if(0!=nuFseek(PTSFile,0,NURO_SEEK_SET))											{	ReleasePTS();	return nuFALSE;	}
	if(1!=nuFread(&PTSStruCount,sizeof(nuLONG),1,PTSFile))							{	ReleasePTS();	return nuFALSE;	}
	pPTSStru=(PTSStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_PTSStru)*PTSStruCount);
	if(pPTSStru==NULL)																{	ReleasePTS();	return nuFALSE;	}
#ifdef _DEBUG
	PTSAlocCount++;
#endif
	if(0!=nuFseek(PTSFile,sizeof(nuLONG),NURO_SEEK_SET))								{	ReleasePTS();	return nuFALSE;	}
	if(PTSStruCount!=nuFread(*pPTSStru,sizeof(Tag_PTSStru),PTSStruCount,PTSFile))	{	ReleasePTS();	return nuFALSE;	}
	if(PTSFile!=NULL)																{	nuFclose(PTSFile);	PTSFile=NULL;	}
	return nuTRUE;
}
nuVOID CCtrl_PTS::ReleasePTS()
{
	if(pPTSStru!=NULL)
	{
	#ifdef _DEBUG
		PTSFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)pPTSStru);		
		pPTSStru=NULL;	
	}

	if(PTSFile!=NULL){	nuFclose(PTSFile);	PTSFile=NULL;	}
	PTSStruCount=0;
}
