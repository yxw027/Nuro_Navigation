// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/18 03:18:32 $
// $Revision: 1.4 $

#include "stdAfxRoute.h"

#include "Ctrl_Ref.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
RefCtrl::RefCtrl()
{
	RefStruCount=0;
	pRefStru=NULL;
	RefFile=NULL;
#ifdef _DEBUG
	RefAlocCount = 0;
	RefFreeCount  = 0;
#endif
}

RefCtrl::~RefCtrl()
{
	CloseClass();
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Ref_MemUesd %d\n", RefAlocCount - RefFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
}

nuBOOL RefCtrl::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID RefCtrl::CloseClass()
{
	ReleaseRef();
}


nuBOOL RefCtrl::ReadRefFile()
{
	ReleaseRef();
	RefFile=nuTfopen(FName, NURO_FS_RB);
	if(RefFile==NULL)
	{
		ReleaseRef();
		return nuFALSE;
	}
	if(0!=nuFseek(RefFile,0,NURO_SEEK_SET))
	{
		ReleaseRef();
		return nuFALSE;
	}
	if(1!=nuFread(&RefStruCount,sizeof(nuLONG),1,RefFile))
	{
		ReleaseRef();
		return nuFALSE;
	}
	pRefStru=(RefStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_RefStru)*RefStruCount);
	if(pRefStru==NULL)
	{
		ReleaseRef();
		return nuFALSE;
	}
	else
	{
	#ifdef _DEBUG
		RefAlocCount++;
	#endif
	}
	if(0!=nuFseek(RefFile,sizeof(nuLONG),NURO_SEEK_SET))
	{
		ReleaseRef();
		return nuFALSE;
	}
	if(RefStruCount!=nuFread(*pRefStru,sizeof(Tag_RefStru),RefStruCount,RefFile))
	{
		ReleaseRef();
		return nuFALSE;
	}
	if(RefFile!=NULL)
	{
		nuFclose(RefFile);
		RefFile=NULL;
	}
	return nuTRUE;
}
nuVOID RefCtrl::ReleaseRef()
{
	if(pRefStru!=NULL)
	{
	#ifdef _DEBUG
		RefFreeCount++;
	#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)pRefStru);		
		pRefStru=NULL;	
	}
	
	if(RefFile!=NULL){	nuFclose(RefFile);	RefFile=NULL;	}
	RefStruCount=0;
}
