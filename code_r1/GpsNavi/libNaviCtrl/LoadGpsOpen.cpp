// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadGpsOpen.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadGpsOpen.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadGpsOpen.h"
#ifdef _USE_GPSOPEN_DLL
#include "NuroDefine.h"
#else
#include "../nuGpsOpen/libGpsOpen.h"
#endif

#ifdef _USE_GPSOPEN_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "GpsOpen.lib")
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadGpsOpen::CLoadGpsOpen()
{
	m_hInst	= NULL;
}

CLoadGpsOpen::~CLoadGpsOpen()
{

}

nuBOOL CLoadGpsOpen::LoadModule(PAPISTRUCTADDR pApiAddr)
{
	m_hInst	= NULL;
#ifdef _USE_GPSOPEN_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libGpsOpen.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\GpsOpen.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	GPS_InitGpsOpenProc GPS_InitGpsOpen = (GPS_InitGpsOpenProc)nuGetProcAddress(m_hInst, "LibGPS_InitGpsOpen");
	if( GPS_InitGpsOpen == NULL || !GPS_InitGpsOpen(pApiAddr) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibGPS_InitGpsOpen(pApiAddr) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadGpsOpen::UnLoadModule()
{
#ifdef _USE_GPSOPEN_DLL
	if( m_hInst != NULL )
	{
		GPS_FreeGpsOpenProc GPS_FreeGpsOpen = (GPS_FreeGpsOpenProc)nuGetProcAddress(m_hInst, "LibGPS_FreeGpsOpen");
		if( GPS_FreeGpsOpen != NULL )
		{
			GPS_FreeGpsOpen();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibGPS_FreeGpsOpen();
	}
#endif
	m_hInst = NULL;
}
