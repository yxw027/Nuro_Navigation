// LoadNC.cpp: implementation of the CLoadNC class.
//
//////////////////////////////////////////////////////////////////////
//#include "windows.h"
#ifndef NURO_OS_WINDOWS
#include "LoadCallmgr.h"
#endif

#include "NuroDefine.h"

//#pragma comment(lib, "CallMgr.lib")

LoadCallmgr::LoadCallmgr()
{
	m_hInst = NULL;
	m_bUseDLL = false;
}

LoadCallmgr::~LoadCallmgr()
{
	FreeDll();
}

bool LoadCallmgr::LoadDll()
{
	nuTCHAR sFile[NURO_MAX_PATH];
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
	nuTcscat(sFile, nuTEXT("Setting\\nuCallmgr.dll"));
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return false;
	}

	CA_InitCallMgr    = (CA_InitCallMgrPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_InitCallMgr"));
	CA_InitCallMgrNew = (CA_InitCallMgrNewPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_InitCallMgr_New"));
	CA_FreeCallMgr    = (CA_FreeCallMgrPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_FreeCallMgr"));
	CA_OnPaint        = (CA_OnPaintPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_OnPaint"));
	CA_KeyDown        = (CA_KeyDownPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_KeyDown"));
	CA_KeyUp          = (CA_KeyUpPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_KeyUp"));
	CA_MouseDown      = (CA_MouseDownPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_MouseDown"));
	CA_MouseUp        = (CA_MouseUpPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_MouseUp"));
	CA_MouseMove      = (CA_MouseMovePro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_MouseMove"));

#ifdef _WINDOWS
	CA_vRegisterWindowMessage = (CA_vRegisterWindowMessagePro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_vRegisterWindowMessage"));
	CA_OnCopyData             = (CA_OnCopyDataPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_OnCopyData"));
	CA_WindowProc      	      = (CA_WindowProcPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_WindowProc"));
	CA_OnCommand       	      = (CA_OnCommandPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_OnCommand"));
#endif
	
	CA_SetGPSFunc       	  = (CA_SetGPSFuncPro)nuGetProcAddress(m_hInst, nuTEXT("LibCA_SetGPSFunc"));

	m_bUseDLL = true;
	return true;
}

void LoadCallmgr::FreeDll()
{
	if( m_bUseDLL )
	{
		CA_FreeCallMgr();
		nuFreeLibrary(m_hInst);
		m_hInst = NULL;
		m_bUseDLL = false;
	}
}

bool LoadCallmgr::IsLoaden()
{
	if( m_hInst )
	{
		return true;
	}
	else
	{
		return false;
	}
}
