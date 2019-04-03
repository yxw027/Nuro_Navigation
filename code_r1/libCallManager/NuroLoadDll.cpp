// NuroLoadDll.cpp: implementation of the CNuroLoadDll class.
//
//////////////////////////////////////////////////////////////////////

#include "NuroLoadDll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNuroLoadDll::CNuroLoadDll()
{
    m_hInst = NULL;
}

CNuroLoadDll::~CNuroLoadDll()
{

}

nuPVOID CNuroLoadDll::LoadDll(const nuTCHAR* ptsDllName, const nuCHAR* ptsFcInit, nuPVOID pLpParam)
{
    if( NULL == ptsDllName || NULL == ptsFcInit )
    {
        return NULL;
    }
    if( NULL != m_hInst )
    {
        return NULL;
    }
    m_hInst = nuLoadLibrary(ptsDllName);
    if( NULL == m_hInst )
    {
        return NULL;
    }
    DLL_InitModule initModule = (DLL_InitModule)nuGetProcAddress(m_hInst, ptsFcInit);
    if( NULL == initModule )
    {
        nuFreeLibrary(m_hInst);
        m_hInst = NULL;
        return NULL;
    }
    return initModule(pLpParam);
}

nuVOID CNuroLoadDll::FreeDll(const nuCHAR* ptsFcFree)
{
    if( NULL != m_hInst )
    {
        if( NULL != ptsFcFree )
        {
            DLL_FreeModule freeModule = (DLL_FreeModule)nuGetProcAddress(m_hInst, ptsFcFree);
            if( NULL != freeModule )
            {
                freeModule();
            }
        }
        nuFreeLibrary(m_hInst);
        m_hInst = NULL;
    }
}
