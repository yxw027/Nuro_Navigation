// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMapData.cpp,v 1.1 2009/09/11 06:56:56 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/09/11 06:56:56 $
// $Locker:  $
// $Revision: 1.1 $
// $Source: /home/nuCode/libNaviCtrl/LoadMapData.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadMapData.h"


#ifdef _USE_MAPDATA_DLL
#include "NuroDefine.h"
#else
//#include "libMapData.h"
#endif

#ifdef _USE_MAPDATA_LIB
//#pragma comment(lib, "MapData.lib")
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadMapData::CLoadMapData()
{
	m_hInst = NULL;
}

CLoadMapData::~CLoadMapData()
{

}

bool CLoadMapData::InitModule(PAPISTRUCTADDR pApiAddr)
{
#if 0
	m_hInst = NULL;
#ifdef _USE_MAPDATA_DLL
	TCHAR sFile[NURO_MAX_PATH];
	GetModuleFileName(NULL, sFile, NURO_MAX_PATH);
	for(int i = nuTcslen(sFile) - 1; i >= 0; i--)
	{
		if( sFile[i] == '\\' )
		{
			sFile[i+1] = '\0';
			break;
		}
	}
	nuTcscat(sFile, nuTEXT("Setting\\MapData.dll"));
	m_hInst = nuLoadLibrary(sFile);
	if( NULL == m_hInst )
	{
		return false;
	}
	MD_InitMapDataProc MD_InitMapData = (MD_InitMapDataProc)nuGetProcAddress(m_hInst, "LibMD_InitMapData");
	if( NULL == MD_InitMapData || !MD_InitMapData(pApiAddr) )
	{
		return false;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibMD_InitMapData(pApiAddr) )
	{
		return false;
	}
#endif
	return true;
#else
    return true;
#endif
}

void CLoadMapData::UnLoadModule()
{
    return;
#if 0
#ifdef _USE_MAPDATA_DLL
	if( NULL != m_hInst )
	{
		MD_FreeMapDataProc MD_FreeMapData = (MD_FreeMapDataProc)nuGetProcAddress(m_hInst, "LibMD_FreeMapData");
		if( NULL != MD_FreeMapData )
		{
			MD_FreeMapData();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if( NULL != m_hInst )
	{
		LibMD_FreeMapData();
	}
#endif
	m_hInst = NULL;
#endif
}
