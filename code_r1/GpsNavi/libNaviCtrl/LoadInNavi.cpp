// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadInNavi.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadInNavi.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadInNavi.h"

#ifdef _USE_INNAVI_DLL
#include "NuroDefine.h"
#else
#include "../libInNavi/libInNavi.h"
#endif

#ifdef _USE_INNAVI_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "InNavi.lib")
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadInNavi::CLoadInNavi()
{
	m_hInst = NULL;
}

CLoadInNavi::~CLoadInNavi()
{
	UnLoadModule();
}

nuBOOL CLoadInNavi::InitModule(PAPISTRUCTADDR pApiAddr)
{
	m_hInst = NULL;
#ifdef _USE_INNAVI_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
    nuTcscat(sFile, nuTEXT("Setting\\libInNavi.so"));
#else
    nuTcscat(sFile, nuTEXT("Setting\\InNavi.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	IN_InitInNaviProc IN_InitInNavi = (IN_InitInNaviProc)nuGetProcAddress(m_hInst, "LibIN_InitInNavi");
	if( IN_InitInNavi == NULL || !IN_InitInNavi(pApiAddr) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibIN_InitInNavi(pApiAddr) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadInNavi::UnLoadModule()
{
#ifdef _USE_INNAVI_DLL
	if( m_hInst != NULL )
	{
		IN_FreeInNaviProc IN_FreeInNavi = (IN_FreeInNaviProc)nuGetProcAddress(m_hInst, "LibIN_FreeInNavi");
		if( IN_FreeInNavi != NULL )
		{
			IN_FreeInNavi();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibIN_FreeInNavi();
	}
#endif
	m_hInst = NULL;
}
