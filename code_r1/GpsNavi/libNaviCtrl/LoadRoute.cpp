// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadRoute.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadRoute.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadRoute.h"

#ifdef _USE_ROUTE_DLL
#include "NuroDefine.h"
#else
#include "../libRoute/libRoute.h"
#endif

#ifdef _USE_ROUTE_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "Route.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadRoute::CLoadRoute()
{
	m_hInst = NULL;
}

CLoadRoute::~CLoadRoute()
{
	UnLoadModule();
}

nuBOOL CLoadRoute::InitModule(nuPVOID lpInMM, nuPVOID lpInFS, nuPVOID lpOutRT)
{
	m_hInst = NULL;
#ifdef _USE_ROUTE_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libRoute.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\Route.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	RT_InitRouteProc RT_InitRoute = (RT_InitRouteProc)nuGetProcAddress(m_hInst, "LibRT_InitRoute");
	if( RT_InitRoute == NULL || !RT_InitRoute(lpInMM, lpInFS, lpOutRT) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibRT_InitRoute(lpInMM, lpInFS, lpOutRT) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadRoute::UnLoadModule()
{
#ifdef _USE_ROUTE_DLL
	if( m_hInst != NULL )
	{
		RT_FreeRouteProc RT_FreeRoute = (RT_FreeRouteProc)nuGetProcAddress(m_hInst, "LibRT_FreeRoute");
		if( RT_FreeRoute != NULL )
		{
			RT_FreeRoute();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibRT_FreeRoute();
	}
#endif
	m_hInst = NULL;
}


