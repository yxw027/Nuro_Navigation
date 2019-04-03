// LoadDllZK.cpp: implementation of the CLoadDllZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../LoadDllZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef NURO_OS_LINUX
const nuTCHAR CLoadDllZK::sc_tsPostfix[] = nuTEXT(".so");
#else
const nuTCHAR CLoadDllZK::sc_tsPostfix[] = nuTEXT(".dll");
#endif
CLoadDllZK::CLoadDllZK(const nuTCHAR* pDllName, const nuCHAR* pDllInitName, const nuCHAR* pDllFreeName):
m_pcDllName(pDllName),
m_pcDllInitName(pDllInitName),
m_pcDllFreeName(pDllFreeName)
{
    m_hInst = NULL;
}

CLoadDllZK::~CLoadDllZK()
{
    FreeDll();
}

nuPVOID CLoadDllZK::LoadDll(const nuTCHAR* ptsDll, nuPVOID pParam)
{
    if( NULL == m_pcDllName || NULL != m_hInst )
    {
        return NULL;
    }
    nuTCHAR tsFile[NURO_MAX_PATH];
    nuTcscpy(tsFile, ptsDll);
#ifdef NURO_OS_LINUX
    nuTcscat(tsFile, "lib");
#endif
    nuTcscat(tsFile, m_pcDllName);
    nuTcscat(tsFile, sc_tsPostfix);
    //m_hInst = sysLoadLibrary(tsFile);
	m_hInst = nuLoadLibrary(tsFile);
    if( NULL == m_hInst )
    {
        return NULL;
    }
    //DLL_InitModelProc InitModel = (DLL_InitModelProc)sysGetProcAddress(m_hInst, m_pcDllInitName);
	DLL_InitModelProc InitModel = (DLL_InitModelProc)nuGetProcAddress(m_hInst, m_pcDllInitName);
    if( NULL == InitModel )
    {
        return NULL;
    }
    return InitModel(pParam);
}

nuVOID CLoadDllZK::FreeDll()
{
    if( m_hInst )
    {
        //DLL_FreeModelProc FreeModel = (DLL_FreeModelProc)sysGetProcAddress(m_hInst, m_pcDllFreeName);
		DLL_FreeModelProc FreeModel = (DLL_FreeModelProc)nuGetProcAddress(m_hInst, m_pcDllFreeName);
        if( NULL != FreeModel )
        {
            FreeModel();
        }
        //sysFreeLibrary(m_hInst);
		nuFreeLibrary(m_hInst);
        m_hInst = NULL;
    }
}
