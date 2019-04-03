// LoadGdiZK.cpp: implementation of the CLoadGdiZK class.
//
//////////////////////////////////////////////////////////////////////

#include "LoadGdiZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _USE_GDI_DLL
static const nuTCHAR* PG_GDI_CONST_STRING[] = {
#ifdef NURO_OS_WINDOWS
    nuTEXT("Gdi.dll")
#endif
#ifdef NURO_OS_LINUX
    nuTEXT("libGDI.so")
#endif
};
static const nuCHAR*  PG_GDI_CONST_STRING_SYMBOL[] = {
	"LibGDI_InitModule",
	"LibGDI_FreeModule"
};
#endif

#ifdef _USE_GDI_LIB
#ifdef NURO_OS_WINDOWS
#pragma comment(lib, "nuGDI.lib")
#endif
#endif
CLoadGdiZK::CLoadGdiZK()
{
    m_pGdi = NULL;
}

CLoadGdiZK::~CLoadGdiZK()
{
    FreeGdi();
}

nuBOOL CLoadGdiZK::LoadGdi(const nuTCHAR* ptsDllFold, NURO_SCREEN* pScreen, nuTCHAR* ptsFontFile)
{
    if( NULL != m_pGdi )
    {
        return nuTRUE;
    }
    if( NULL == pScreen )
    {
        return nuFALSE;
    }
    m_pGdi = (PNURO_API_GDI)LibGDI_InitModule(NULL);
    if( NULL == m_pGdi )
    {
        return nuFALSE;
    }
    __android_log_print(ANDROID_LOG_INFO, "navi", "init pGdi ok.....");
    if( !m_pGdi->GDI_Initialize(pScreen) || 
        !m_pGdi->GDI_CreateCanvas(NULL, pScreen->nScreenWidth, pScreen->nScreenHeight) )
    {
        return nuFALSE;
    }
    
	//m_pGdi->GDI_ShowStartUpBmp();
	__android_log_print(ANDROID_LOG_INFO, "navi", "init GDI_Initialize ok.....");
    return nuTRUE;
}

nuVOID CLoadGdiZK::FreeGdi()
{
    if( NULL != m_pGdi )
    {
        m_pGdi->GDI_DeleteCanvas(NULL);
        m_pGdi->GDI_Free();
#ifdef _USE_GDI_DLL
        FreeDll(PG_GDI_CONST_STRING_SYMBOL[1]);
#else
        LibGDI_FreeModule();
#endif
        m_pGdi = NULL;
    }
}


