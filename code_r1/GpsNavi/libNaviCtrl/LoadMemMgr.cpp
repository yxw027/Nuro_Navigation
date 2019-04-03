// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMemMgr.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadMemMgr.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadMemMgr.h"

#ifdef _USE_MEMMGR_DLL
#include "NuroDefine.h"
#else
#include "../libMemMgr/libMemMgr.h"
#include "../libMemMgr/libMemMgr_SH.h"
#endif

#ifdef _USE_MEMMGR_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "MemMgr.lib")
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadMemMgr::CLoadMemMgr()
{
	m_hInst = NULL;
}

CLoadMemMgr::~CLoadMemMgr()
{
	UnLoadModule();
}

nuBOOL CLoadMemMgr::InitModule(nuPVOID lpOutMM,nuPVOID lpOutMM2)
{
	m_hInst = NULL;
#ifdef _USE_MEMMGR_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libMemMgr.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\MemMgr.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	MM_InitMemMgrProc MM_InitMemMgr = (MM_InitMemMgrProc)nuGetProcAddress(m_hInst, "LibMM_InitMemMgr");
	if( MM_InitMemMgr == NULL || !MM_InitMemMgr(lpOutMM) )
	{
		return nuFALSE;
	}
	MM_InitMemMgrProc MM_InitMemMgr_SH = (MM_InitMemMgrProc)nuGetProcAddress(m_hInst, "LibMM_InitMemMgr_SH");
	if( MM_InitMemMgr_SH == NULL || !MM_InitMemMgr_SH(lpOutMM2) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibMM_InitMemMgr(lpOutMM) )
	{
		return false;
	}
	if( !LibMM_InitMemMgr_SH(lpOutMM2) )
	{
		return false;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadMemMgr::UnLoadModule()
{
#ifdef _USE_MEMMGR_DLL
	if( m_hInst != NULL )
	{
		MM_FreeMemMgrProc MM_FreeMemMgr = (MM_FreeMemMgrProc)nuGetProcAddress(m_hInst, "LibMM_FreeMemMgr");
		if( MM_FreeMemMgr != NULL )
		{
			MM_FreeMemMgr();
		}
		MM_FreeMemMgrProc MM_FreeMemMgr_SH = (MM_FreeMemMgrProc)nuGetProcAddress(m_hInst, "LibMM_FreeMemMgr_SH");
		if( MM_FreeMemMgr_SH != NULL )
		{
			MM_FreeMemMgr_SH();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibMM_FreeMemMgr();
		LibMM_FreeMemMgr_SH();
	}
#endif
	m_hInst = NULL;
}


