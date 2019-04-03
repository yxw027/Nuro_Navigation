// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadSearch.cpp,v 1.2 2009/08/28 03:16:34 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:16:34 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadSearch.cpp,v $

#include "LoadSearch.h"

#ifdef _USE_SEARCHNEW_DLL
#include "NuroDefine.h"
#else
#include "../libSearchNew/libSearchNew.h"
#endif

#ifdef _USE_SEARCHNEW_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "SearchNew.lib")
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadSearch::CLoadSearch()
{
	m_hInst	= NULL;
}

CLoadSearch::~CLoadSearch()
{

}

nuBOOL CLoadSearch::LoadModule(PAPISTRUCTADDR pApiAddr)
{
	m_hInst = NULL;
#ifdef _USE_SEARCHNEW_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libSearchNew.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\SearchNew.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	SH_InitSearchZProc SH_InitSearchZ = (SH_InitSearchZProc)nuGetProcAddress(m_hInst, "LibSH_InitSearchZ");
	if( SH_InitSearchZ == NULL || !SH_InitSearchZ(pApiAddr) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibSH_InitSearchZ(pApiAddr) )
	{
		return false;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadSearch::UnLoadModule()
{
#ifdef _USE_SEARCHNEW_DLL
	if( m_hInst != NULL )
	{
		SH_FreeSearchZProc SH_FreeSearchZ = (SH_FreeSearchZProc)nuGetProcAddress(m_hInst, "LibSH_FreeSearchZ");
		if( SH_FreeSearchZ != NULL )
		{
			SH_FreeSearchZ();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibSH_FreeSearchZ();
	}
#endif
	m_hInst = NULL;
}
