// LoadResources.cpp: implementation of the CLoadResources class.
//
//////////////////////////////////////////////////////////////////////

#include "LoadResources.h"

#ifdef _USE_OPEN_RESOURCE_DLL
#include "NuroDefine.h"
#else
#include "../../OpenResource/libOpenResource.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadResources::CLoadResources()
{
	m_hInst = NULL;
}

CLoadResources::~CLoadResources()
{
	UnLoadModule();
}

//bool CLoadResources::InitModule(PAPISTRUCTADDR pApiAddr)
nuBOOL CLoadResources::InitModule(nuPVOID lpOutRS)
{
	m_hInst = NULL;
#ifdef _USE_RESOURCES_DLL
	TCHAR sFile[NURO_MAX_PATH];
	nuGetModuleFileName(NULL, sFile, NURO_MAX_PATH);
	for( int i = nuTcslen(sFile) -1 ; i >= 0; i-- )
	{
		if( sFile[i] == '\\' )
		{
			sFile[i+1] = '\0';
			break;
		}
	}
	nuTcscat(sFile, nuTEXT("Setting\\OpenResource.dll"));
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return false;
	}
	RS_InitResourcesProc RS_InitResources = 
		(RS_InitResourcesProc)nuGetProcAddress(m_hInst, nuTEXT("LibOPR_InitModule"));
	if( RS_InitResources == NULL || RS_InitResources(lpOutRS) == NULL )
	{
		return false;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( LibOPR_InitModule(lpOutRS) == NULL )
	{
		return false;
	}
#endif
	return nuTRUE;
}

void CLoadResources::UnLoadModule()
{
#ifdef _USE_RESOURCES_DLL
	if( m_hInst != NULL )
	{
		RS_FreeResourcesProc RS_FreeResources = 
			(RS_FreeResourcesProc)nuGetProcAddress(m_hInst, nuTEXT("LibOPR_FreeModule"));
		if( RS_FreeResources != NULL )
		{
			RS_FreeResources();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if( m_hInst != NULL )
	{
		LibOPR_FreeModule();
	}
#endif
	m_hInst = NULL;
}
