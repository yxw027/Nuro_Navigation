// LoadRenderModelZK.cpp: implementation of the CLoadRenderModelZK class.
//
//////////////////////////////////////////////////////////////////////
//#include "StdAfx.h"
#include "LoadRenderModelZK.h"

#ifdef _USE_RENDERMODEL_DLL
#else
#include "libRenderModel.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 
// #ifdef _USE_RENDERMODEL_LIB
// #ifdef NURO_OS_LINUX
// #else
// #pragma comment(lib, "RenderModel.lib")
// #endif
// #endif


CLoadRenderModelZK::CLoadRenderModelZK()
{
	m_hInst = NULL;
}

CLoadRenderModelZK::~CLoadRenderModelZK()
{
	FreeModuleH();
}

nuBOOL CLoadRenderModelZK::LoadModule(nuPVOID lpApiOut, nuPVOID lpApiIn)
{
	m_hInst = NULL;
#ifdef _USE_RENDERMODEL_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libRenderModel.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\RenderModel.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( NULL == m_hInst )
	{
		return nuFALSE;
	}
	RM_InitRenderModelProc RM_InitRenderModel = (RM_InitRenderModelProc)nuGetProcAddress(m_hInst, "LibRM_InitRenderModel");
	if( NULL == RM_InitRenderModel || !RM_InitRenderModel((PAPI_RENDERMODEL)lpApiOut, lpApiIn) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibRM_InitRenderModel((PAPI_RENDERMODEL)lpApiOut, lpApiIn) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadRenderModelZK::FreeModuleH()
{
#ifdef _USE_RENDERMODEL_DLL
	if( NULL != m_hInst )
	{
		RM_FreeRenderModelProc RM_FreeRenderModel = (RM_FreeRenderModelProc)nuGetProcAddress(m_hInst, "LibRM_FreeRenderModel");
		if( NULL != RM_FreeRenderModel )
		{
			RM_FreeRenderModel();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if( NULL != m_hInst )
	{
		LibRM_FreeRenderModel();
	}
#endif
	m_hInst = NULL;
}
