// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadDrawMap.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadDrawMap.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadDrawMap.h"

#ifdef _USE_DRAWMAP_DLL
#include "NuroDefine.h"
#else
#include "../libDrawMap/libDrawMap.h"
#endif

#ifdef _USE_DRAWMAP_LIB
#ifdef NURO_OS_WINDOWS
#pragma comment(lib, "DrawMap.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadDrawMap::CLoadDrawMap()
{
	m_hInst	= NULL;
}

CLoadDrawMap::~CLoadDrawMap()
{
	UnLoadModule();
}

nuBOOL CLoadDrawMap::InitModule(PAPISTRUCTADDR pApiAddr, POPENRSAPI pRsApi)
{
	m_hInst = NULL;
#ifdef _USE_DRAWMAP_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libDrawMap.so"));//
#else
	nuTcscat(sFile, nuTEXT("Setting\\DrawMap.dll"));//
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	DM_InitDrawMapProc DM_InitDrawMap = (DM_InitDrawMapProc)nuGetProcAddress(m_hInst, "LibDM_InitDrawMap");
	if( DM_InitDrawMap == NULL || !DM_InitDrawMap(pApiAddr, pRsApi) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibDM_InitDrawMap(pApiAddr, pRsApi) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadDrawMap::UnLoadModule()
{
#ifdef _USE_DRAWMAP_DLL
	if( m_hInst != NULL )
	{
		DM_FreeDrawMapProc DM_FreeDrawMap = (DM_FreeDrawMapProc)nuGetProcAddress(m_hInst, "LibDM_FreeDrawMap");
		if( DM_FreeDrawMap != NULL )
		{
			DM_FreeDrawMap();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibDM_FreeDrawMap();
	}
#endif
	m_hInst = NULL;
}
