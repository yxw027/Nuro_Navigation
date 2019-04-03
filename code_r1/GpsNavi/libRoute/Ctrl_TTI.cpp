// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:16 $
// $Revision: 1.6 $
#include "stdAfxRoute.h"
#include "RouteBase.h"
#include "Ctrl_TTI.h"

Class_TTICtrl::Class_TTICtrl()
{
#ifdef _DEBUG
	TTIAlocCount = 0;
	TTIFreeCount = 0;
#endif
	InitClass();
}

Class_TTICtrl::~Class_TTICtrl()
{
	CloseClass();
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("SDMMC\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"TTI_MemUesd %d\n", TTIAlocCount - TTIFreeCount);
			fclose(pfile);
		}
	#endif
#endif
}

nuBOOL Class_TTICtrl::InitClass()
{
	m_pTTIFile = NULL;
	ppTTIData  = NULL;
	TTICnt	   = 0;
	nuMemset(Reserve, 0, sizeof(nuBYTE) * _TTI_RESERVESIZE);
	nuMemset(m_tsFName, 0, sizeof(nuTCHAR)*NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID Class_TTICtrl::CloseClass()
{
	Release();
}

nuVOID Class_TTICtrl::Release()
{
	if(m_pTTIFile != NULL)
	{
		nuFclose(m_pTTIFile);
		m_pTTIFile = NULL;
	}
	if(ppTTIData != NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ppTTIData);
		ppTTIData = NULL;
	}
#ifdef _DEBUG
	TTIFreeCount++;
#endif
	TTICnt	   = 0;
	nuMemset(Reserve, 0, sizeof(nuBYTE) * _TTI_RESERVESIZE);
	nuMemset(m_tsFName, 0, sizeof(nuTCHAR)*NURO_MAX_PATH);
}

nuBOOL Class_TTICtrl::ReadFile()
{
	if(m_pTTIFile != NULL)
	{
		return nuFALSE;
	}
	m_pTTIFile = nuTfopen(m_tsFName, NURO_FS_RB);
	if(m_pTTIFile == NULL)
	{
		return nuFALSE;
	}
	if(1 != nuFread(&TTICnt,sizeof(nuLONG),1,m_pTTIFile))
	{	
		Release();	
		return nuFALSE;	
	}
	if(TTICnt <= 0)
	{
		Release();
		return nuFALSE;	
	}
	if(_TTI_RESERVESIZE != nuFread(Reserve,sizeof(nuBYTE), _TTI_RESERVESIZE ,m_pTTIFile))
	{	
		Release();	
		return nuFALSE;	
	}
	ppTTIData = (TTI_Data_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(TTI_Data_Stru) * TTICnt);
	if(ppTTIData == NULL)
	{
		Release();	
		return nuFALSE;
	}
#ifdef _DEBUG
	TTIAlocCount++;
#endif
	nuMemset((*ppTTIData), 0, sizeof(TTI_Data_Stru) * TTICnt);
	if(TTICnt != nuFread((*ppTTIData), sizeof(TTI_Data_Stru), TTICnt, m_pTTIFile))
	{
		Release();	
		return nuFALSE;
	}
	nuFclose(m_pTTIFile);
	m_pTTIFile = NULL;	
	return nuTRUE;
}
nuBOOL Class_TTICtrl::GetTTIData(nuLONG Addr, TTI_Data_Stru &TTIData)
{
	if(TTICnt <= 0 || ppTTIData == NULL)
	{
		return nuFALSE;
	}
	else
	{
		nuLONG FileIdx = (Addr-100) / sizeof(TTI_Data_Stru);
		TTIData = (*ppTTIData)[FileIdx];
		return nuTRUE;
	}	
	return nuTRUE;
}