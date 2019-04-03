#include "LoadComOpen.h"
#include "NuroDefine.h"
//#include "libComOpen.h"
#include "NuroModuleApiStruct.h"

CLoadComOpen::CLoadComOpen()
{
#ifdef  NURO_GPSMA
    #ifndef _USE_COMOPEN_DLL
		COM_InitComOpen  = LibCOM_InitComOpen;
		COM_FreeComOpen  = LibCOM_FreeComOpen;
		COM_Open         = LibCOM_Open;
		COM_Close        = LibCOM_Close;
		COM_RecvData     = LibCOM_RecvData;
		COM_GetXY        = LibCOM_GetXY;
		COM_ReturnXY     = LibCOM_ReturnXY
		m_hInst = (nuHINSTANCE)(-1);

	#else
		m_hInst = NULL;
		COM_InitComOpen = NULL;
		COM_FreeComOpen    = NULL;
	#endif
#endif
}

CLoadComOpen::~CLoadComOpen()
{
    //FreeDll();
}

bool CLoadComOpen::LoadDll()
{
#ifdef  NURO_GPSMA
	#ifdef _USE_COMOPEN_DLL
		TCHAR sFile[NURO_MAX_PATH];
		nuGetModuleFileName(NULL, sFile, NURO_MAX_PATH);
		for(int i = nuTcslen(sFile) - 1; i >= 0; i--)
		{
        if( sFile[i] == '\\' )
        {
            sFile[i+1] = '\0';
            break;
        }
		}
		nuTcscat(sFile, nuTEXT("Setting\\ComOpen.dll"));
		m_hInst = nuLoadLibrary(sFile);

		if( m_hInst == NULL )
		{
			return false;
		}
		COM_InitComOpen  = (COM_InitComOpenProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_InitComOpen"));
		COM_FreeComOpen  = (COM_FreeComOpenProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_FreeComOpen"));
		COM_Open		 = (COM_OpenProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_Open"));
		COM_Close		 = (COM_CloseProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_Close"));
		COM_RecvData	 = (COM_RecvDataProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_RecvData"));
		COM_GetXY		 = (COM_GetXYProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_GetXY"));
		COM_ReturnXY	 = (COM_ReturnXYProc)nuGetProcAddress(m_hInst, nuTEXT("LibCOM_ReturnXY"));
		return true;

	#else

		COM_InitComOpen  = LibCOM_InitComOpen;
		COM_FreeComOpen  = LibCOM_FreeComOpen;
		COM_Open         = LibCOM_Open;
		COM_Close        = LibCOM_Close;
		COM_RecvData     = LibCOM_RecvData;
		COM_GetXY        = LibCOM_GetXY;
		COM_ReturnXY     = LibCOM_ReturnXY;
		m_hInst = (nuHINSTANCE)(-1);

	#endif
#endif
    return true;
    
}

void CLoadComOpen::FreeDll()
{
#ifdef  NURO_GPSMA
    if( IsLoaden())
    {
        if(NULL != COM_FreeComOpen)
        {
            COM_FreeComOpen();
        }

	#ifdef _USE_COMOPEN_DLL
        nuFreeLibrary(m_hInst);
	#else
	#endif
        m_hInst = NULL;
    }
#endif
}

bool CLoadComOpen::IsLoaden()
{
#ifdef  NURO_GPSMA
    if( m_hInst )
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
	return true;
}
