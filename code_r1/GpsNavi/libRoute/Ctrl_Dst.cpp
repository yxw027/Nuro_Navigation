// Ctrl_Rel.cpp: implementation of the CCtrl_Rel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfxRoute.h"

#include "Ctrl_Dst.h"
#ifdef _DEBUG
#include "stdio.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl_Dst::CCtrl_Dst()
{
	InitDstStruct();
#ifdef _DEBUG
	DstFreeCount = 0;
	DstAlocCount = 0;
#endif
}

CCtrl_Dst::~CCtrl_Dst()
{
	CloseClass();
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"Dst_MemUesd %d\n", DstAlocCount - DstFreeCount);
		fclose(pfile);
	}
#endif
#endif
}

nuBOOL CCtrl_Dst::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}

nuVOID CCtrl_Dst::CloseClass()
{
	ReleaseStruct();
}

nuBOOL CCtrl_Dst::ReadStruct()
{	
	DstFile  = nuTfopen(FName, NURO_FS_RB);
	if(DstFile != NULL)
	{
		if(1 != nuFread(&m_nTotalDstCount,sizeof(nuLONG),1,DstFile))
		{
			return nuTRUE;//排除如果沒有檔案的情況
			//return nuFALSE;
		}
		Dstdata = (Tag_StruDstdata**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_StruDstdata) * m_nTotalDstCount);
		if(Dstdata != NULL)
		{	
			#ifdef _DEBUG
				DstAlocCount++;
			#endif
			if(m_nTotalDstCount != nuFread(*Dstdata,sizeof(Tag_StruDstdata),m_nTotalDstCount,DstFile))
			{
				ReleaseStruct();
				return nuFALSE;
			}
		}
		else
		{
			return nuFALSE;
		}
		nuFclose(DstFile);
	}
	else
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CCtrl_Dst::ReleaseStruct()
{
	if(Dstdata != NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)Dstdata);
		Dstdata = NULL;
		#ifdef _DEBUG
			DstFreeCount++;
		#endif
	}
	if(DstFile != NULL)
	{
		nuFclose(DstFile);
		DstFile = NULL;
	}	
}

nuVOID CCtrl_Dst::InitDstStruct()
{
	DstFile  = NULL;
	Dstdata = NULL;
}
		
