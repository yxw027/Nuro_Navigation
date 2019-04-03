// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadFileSys.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadFileSys.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadFileSys.h"

#ifdef _USE_FILESYS_DLL
#include "NuroDefine.h"
#else
#include "../libFileSystem/libFileSystem.h"
#endif

#ifdef _USE_FILESYS_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "FileSystem.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadFileSys::CLoadFileSys()
{
	m_hInst	= NULL;
}

CLoadFileSys::~CLoadFileSys()
{

}

nuBOOL CLoadFileSys::InitModule(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpOutFs)
{
	m_hInst = NULL;
#ifdef _USE_FILESYS_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libFileSystem.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\FileSystem.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	FS_InitFileSystemProc FS_InitFileSystem = (FS_InitFileSystemProc)nuGetProcAddress(m_hInst, "LibFS_InitFileSystem");
	if( FS_InitFileSystem == NULL || !FS_InitFileSystem(lpInMM, lpInGDI, lpOutFs) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibFS_InitFileSystem(lpInMM, lpInGDI, lpOutFs) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadFileSys::UnLoadModule()
{
#ifdef _USE_FILESYS_DLL
	if( m_hInst != NULL )
	{
		FS_FreeFileSystemProc FS_FreeFileSystem = (FS_FreeFileSystemProc)nuGetProcAddress(m_hInst, "LibFS_FreeFileSystem");
		if( FS_FreeFileSystem != NULL )
		{
			FS_FreeFileSystem();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibFS_FreeFileSystem();
	}
#endif
	m_hInst = NULL;
}

