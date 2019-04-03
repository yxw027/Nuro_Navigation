// LoadModules.cpp: implementation of the CLoadModules class.
//
//////////////////////////////////////////////////////////////////////

#include "../LoadModules.h"
#include "../../NuroDefine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nuTCHAR CLoadModules::s_tsPath[NURO_MAX_PATH];

CLoadModules::CLoadModules()
{
    m_hInst = NULL;
}

CLoadModules::~CLoadModules()
{
//    FreeDll();
}

nuVOID CLoadModules::SetPath(const nuTCHAR* ptsPath)
{
    nuTcscpy(s_tsPath, ptsPath);
}

nuPVOID CLoadModules::LoadDll(nuTCHAR* pTsDllName, nuCHAR* pTsInitFc, nuPVOID pLpParam)
{
    nuTCHAR tsFile[NURO_MAX_PATH];
    nuTcscpy(tsFile, s_tsPath);
    nuTcscat(tsFile, nuTEXT("Setting\\"));
    nuTcscat(tsFile, pTsDllName);
    m_hInst = nuLoadLibrary(tsFile);
    if( NULL == m_hInst )
    {
        return NULL;
    }
    DLL_InitModel InitModel = (DLL_InitModel)nuGetProcAddress(m_hInst, pTsInitFc);
    if( NULL == InitModel )
    {
        return NULL;
    }
    return InitModel(pLpParam);
}

nuVOID CLoadModules::FreeDll(nuCHAR* pTsInitFc)
{
    if( m_hInst )
    {
        DLL_FreeModel FreeModel = (DLL_FreeModel)nuGetProcAddress(m_hInst, pTsInitFc);
        if( NULL != FreeModel )
        {
            FreeModel();
        }
        m_hInst = NULL;
    }
}
