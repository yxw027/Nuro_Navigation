// LoadNC.cpp: implementation of the CLoadNC class.
//
//////////////////////////////////////////////////////////////////////
//#include "windows.h"
#include "LoadNC.h"

#ifndef _USE_NAVICTRL_DLL //don't use Dll, include libNaviCtrl.h indirectly
    #include "libNaviCtrl.h"
#else
    #include "nuroCLib.h"
    #include "libInterface.h"
#endif//Load static library

#ifdef _USE_NAVICTRL_LIB
#pragma comment(lib, "NaviCtrl.lib")
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadNC::CLoadNC()
{
#ifndef _USE_NAVICTRL_DLL
	NC_SetNaviState = LibNC_SetNaviState;
	NC_Start3DDemo  = LibNC_Start3DDemo;
	NC_Close3DDemo  = LibNC_Close3DDemo;
    NC_StartOpenGL  = LibNC_StartOpenGL;
    NC_InitNaviCtrl = LibNC_InitNaviCtrl;
    NC_FreeNaviCtrl = LibNC_FreeNaviCtrl;
    NC_StartNaviCtrl= LibNC_StartNaviCtrl;
    NC_OnPaint      = LibNC_OnPaint;
    NC_KeyDown      = LibNC_KeyDown;
    NC_KeyUp        = LibNC_KeyUp;
    NC_SetUserCfg   = LibNC_SetUserCfg;
    NC_Zoom         = LibNC_Zoom;
    NC_MouseDown    = LibNC_MouseDown;
    NC_MouseUp        = LibNC_MouseUp;
    NC_MouseMove    = LibNC_MouseMove;
    NC_SetMarketPt    = LibNC_SetMarketPt;
    NC_GpsSendIn    = LibNC_GpsSendIn;
    NC_ShilftNaviThread    = LibNC_ShilftNaviThread;
    NC_SoundPlay    = LibNC_SoundPlay;
    NC_SetVoice        = LibNC_SetVoice;
    NC_SetTimer        = LibNC_SetTimer;
    NC_GetRoutingList    = LibNC_GetRoutingList;
    NC_GetGPSData    = LibNC_GetGPSData;
    NC_GetMapInfo    = LibNC_GetMapInfo;
    NC_Navigation   = LibNC_Navigation;
    NC_MoveToMapXY  = LibNC_MoveToMapXY;
    NC_OperateMapXY = LibNC_OperateMapXY;
    NC_IsNavigation = LibNC_IsNavigation;
    NC_MessageBox    = LibNC_MessageBox;
    NC_ConfigRoute  = LibNC_ConfigRoute;
    NC_DelMarketPt  = LibNC_DelMarketPt;
    NC_GetTripInfo  = LibNC_GetTripInfo;
    NC_ShowHideButton = LibNC_ShowHideButton;
    NC_CoverDefaultUserCfg = LibNC_CoverDefaultUserCfg;
    NC_GetNaviMode = LibNC_GetNaviMode;
    NC_GetTraceHead = LibNC_GetTraceHead;
    NC_StartTrace   = LibNC_StartTrace;
    NC_CloseTrace   = LibNC_CloseTrace;
    NC_ShowTrace    = LibNC_ShowTrace;
    NC_ClosShow     = LibNC_ClosShow;
    NC_DeleteTrace  = LibNC_DeleteTrace;
    NC_GetTracePoint= LibNC_GetTracePoint;
    NC_SetMemoData  = LibNC_SetMemoData;
    NC_GetMapObjState = LibNC_GetMapObjState;
    NC_SetMapObjRange = LibNC_SetMapObjRange;
    NC_ShowMap      = LibNC_ShowMap;
    NC_MoveMap      = LibNC_MoveMap;
    NC_GetScaleList = LibNC_GetScaleList;
    NC_SetScale     = LibNC_SetScale;
    NC_NuroButtonEvent = LibNC_NuroButtonEvent;
    NC_SetDrawInfo    = LibNC_SetDrawInfo;
    NC_TraceImport  = LibNC_TraceImport;
    NC_TraceExport  = LibNC_TraceExport;
    NC_SetTimerEx   = LibNC_SetTimerEx;
    NC_KillTimerEx  = LibNC_KillTimerEx;
    NC_GetKernalState = LibNC_GetKernalState;
    NC_ResetTripInfo = LibNC_ResetTripInfo;
    NC_SetNaviThread = LibNC_SetNaviThread;    
    NC_SetGPSFunc = LibNC_SetGPSFunc;
    NC_GetLastError = LibNC_GetLastError;
    NC_GetSystemTime = LibNC_GetSystemTime;
    NC_SetCallBack = LibNC_SetCallBack;
    NC_GetNavigationData = LibNC_GetNavigationData;

    m_hInst = (nuHINSTANCE)(-1);
#else
    m_hInst = NULL;
    NC_InitNaviCtrl = NULL;
    NC_FreeNaviCtrl    = NULL;
#endif
}

CLoadNC::~CLoadNC()
{
    FreeDll();
}

bool CLoadNC::LoadDll()
{
#ifdef _USE_NAVICTRL_DLL
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
    nuTcscat(sFile, nuTEXT("Setting\\NaviCtrl.dll"));
    m_hInst = nuLoadLibrary(sFile);
//    MessageBox(NULL, sFile, nuTEXT(""), 0);
    if( m_hInst == NULL )
    {
        /*test...
        long k = GetLastError();
        TCHAR ts[56];
        wsprintf(ts, nuTEXT("Error = %d"), k);
        MessageBox(NULL, ts, nuTEXT(""), 0);*/
        return false;
    }

	NC_SetNaviState  = (NC_SetNaviStateProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetNaviState"));
	NC_Start3DDemo  = (NC_Start3DDemoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_Start3DDemo"));
	NC_Close3DDemo  = (NC_Close3DDemoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_Close3DDemo"));
    NC_StartOpenGL  = (NC_StartOpenGLProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_StartOpenGL"));
    NC_InitNaviCtrl = (NC_InitNaviCtrlProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_InitNaviCtrl"));
    NC_FreeNaviCtrl = (NC_FreeNaviCtrlProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_FreeNaviCtrl"));
    NC_StartNaviCtrl= (NC_StartNaviCtrlProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_StartNaviCtrl"));
    NC_OnPaint        = (NC_OnPaintProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_OnPaint"));
    NC_KeyDown        = (NC_KeyProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_KeyDown"));
    NC_KeyUp        = (NC_KeyProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_KeyUp"));
    NC_SetUserCfg    = (NC_SetUserCfgProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetUserCfg"));
    NC_Zoom            = (NC_ZoomProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_Zoom"));
    NC_MouseDown    = (NC_MouseProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_MouseDown"));
    NC_MouseUp        = (NC_MouseProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_MouseUp"));
    NC_MouseMove    = (NC_MouseProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_MouseMove"));
    NC_SetMarketPt    = (NC_SetMarketPtProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetMarketPt"));
    NC_DelMarketPt  = (NC_DelMarketPtProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_DelMarketPt"));
    NC_GpsSendIn    = (NC_GpsSendInProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GpsSendIn"));
    NC_ShilftNaviThread    = (NC_ShilftNaviThreadProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ShilftNaviThread"));
    NC_SoundPlay    = (NC_SoundPlayProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SoundPlay"));

    NC_SetVoice        = (NC_SetVoiceProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetVoice"));
    NC_SetTimer        = (NC_SetTimerProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetTimer"));
    NC_GetRoutingList    = (NC_GetRoutingListProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetRoutingList"));
    NC_GetGPSData    = (NC_GetGPSDataProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetGPSData"));
    NC_GetTripInfo  = (NC_GetTripInfoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetTripInfo"));
    NC_GetMapInfo    = (NC_GetMapInfoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetMapInfo"));
    NC_Navigation   = (NC_NavigationProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_Navigation"));
    NC_MoveToMapXY  = (NC_MoveToMapXYProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_MoveToMapXY"));
    NC_OperateMapXY = (NC_OperateMapXYProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_OperateMapXY"));
    NC_MessageBox    = (NC_MessageBoxProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_MessageBox"));
    NC_IsNavigation = (NC_IsNavigationProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_IsNavigation"));
    NC_ConfigRoute  = (NC_ConfigRouteProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ConfigRoute"));
    NC_ShowHideButton = (NC_ShowHideButtonProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ShowHideButton"));
    NC_CoverDefaultUserCfg = (NC_CoverDefaultUserCfgProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_CoverDefaultUserCfg"));
    NC_GetNaviMode = (NC_GetNaviModeProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetNaviMode"));
    NC_GetTraceHead = (NC_GetTraceHeadProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetTraceHead"));
    NC_StartTrace   = (NC_StartTraceProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_StartTrace"));
    NC_CloseTrace   = (NC_CloseTraceProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_CloseTrace"));
    NC_ShowTrace    = (NC_ShowTraceProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ShowTrace"));
    NC_ClosShow     = (NC_ClosShowProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ClosShow"));
    NC_DeleteTrace = (NC_DeleteTraceProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_DeleteTrace"));
    NC_GetTracePoint= (NC_GetTracePointProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetTracePoint"));
    NC_SetMemoData = (NC_SetMemoDataProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetMemoData"));
    NC_GetMapObjState = (NC_GetMapObjStateProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetMapObjState"));
    NC_SetMapObjRange = (NC_SetMapObjRangeProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetMapObjRange"));
    NC_ShowMap = (NC_ShowMapProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ShowMap"));
    NC_MoveMap = (NC_MoveMapProc)nuGetProcAddress(m_hInst, nuTEXT("LinNC_MoveMap"));
    NC_GetScaleList   = (NC_GetScaleListProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetScaleList"));
    NC_SetScale       = (NC_SetScaleProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetScale"));
    NC_NuroButtonEvent = (NC_NuroButtonEventProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_NuroButtonEvent"));

    NC_SetDrawInfo = (NC_SetDrawInfoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetDrawInfo"));
    NC_TraceImport = (NC_TraceImportProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_TraceImport"));
    NC_TraceExport = (NC_TraceExportProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_TraceExport"));
    NC_SetTimerEx = (NC_SetTimerExProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetTimerEx"));
    NC_KillTimerEx = (NC_KillTimerExProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_KillTimerEx"));
    NC_GetKernalState = (NC_GetKernalStateProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetKernalState"));
    NC_ResetTripInfo = (NC_ResetTripInfoProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_ResetTripInfo"));
    NC_SetNaviThread = (NC_SetNaviThreadProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetNaviThread"));
    NC_SetGPSFunc = (NC_SetGPSFuncProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetGPSFunc"));
    NC_GetLastError = (NC_GetLastErrorProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetLastError"));
    NC_GetSystemTime = (NC_GetSystemTimeProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetSystemTime"));
    NC_SetCallBack   = (NC_SetCallBackProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_SetCallBack"));
    NC_GetNavigationData = (NC_GetNavigationDataProc)nuGetProcAddress(m_hInst, nuTEXT("LibNC_GetNavigationData"));
    
    return true;
#else
	NC_SetNaviState  = LibNC_SetNaviState;
	NC_Start3DDemo  = LibNC_Start3DDemo;
	NC_Close3DDemo  = LibNC_Close3DDemo;
    NC_StartOpenGL  = LibNC_StartOpenGL;
    NC_InitNaviCtrl = LibNC_InitNaviCtrl;
    NC_FreeNaviCtrl = LibNC_FreeNaviCtrl;
    NC_StartNaviCtrl= LibNC_StartNaviCtrl;
    NC_OnPaint        = LibNC_OnPaint;
    NC_KeyDown        = LibNC_KeyDown;
    NC_KeyUp        = LibNC_KeyUp;
    NC_SetUserCfg    = LibNC_SetUserCfg;
    NC_Zoom            = LibNC_Zoom;
    NC_MouseDown    = LibNC_MouseDown;
    NC_MouseUp        = LibNC_MouseUp;
    NC_MouseMove    = LibNC_MouseMove;
    NC_SetMarketPt    = LibNC_SetMarketPt;
    NC_GpsSendIn    = LibNC_GpsSendIn;
    NC_ShilftNaviThread    = LibNC_ShilftNaviThread;
    NC_SoundPlay    = LibNC_SoundPlay;
    NC_SetVoice        = LibNC_SetVoice;
    NC_SetTimer        = LibNC_SetTimer;
    NC_GetRoutingList    = LibNC_GetRoutingList;
    NC_GetGPSData    = LibNC_GetGPSData;
    NC_GetMapInfo    = LibNC_GetMapInfo;
    NC_Navigation   = LibNC_Navigation;
    NC_MoveToMapXY  = LibNC_MoveToMapXY;
    NC_OperateMapXY = LibNC_OperateMapXY;
    NC_IsNavigation = LibNC_IsNavigation;
    NC_MessageBox    = LibNC_MessageBox;
    NC_ConfigRoute  = LibNC_ConfigRoute;
    NC_DelMarketPt  = LibNC_DelMarketPt;
    NC_GetTripInfo  = LibNC_GetTripInfo;
    NC_ShowHideButton = LibNC_ShowHideButton;
    NC_CoverDefaultUserCfg = LibNC_CoverDefaultUserCfg;
    NC_GetNaviMode = LibNC_GetNaviMode;
    NC_GetTraceHead = LibNC_GetTraceHead;
    NC_StartTrace   = LibNC_StartTrace;
    NC_CloseTrace   = LibNC_CloseTrace;
    NC_ShowTrace    = LibNC_ShowTrace;
    NC_ClosShow     = LibNC_ClosShow;
    NC_DeleteTrace  = LibNC_DeleteTrace;
    NC_GetTracePoint= LibNC_GetTracePoint;
    NC_SetMemoData  = LibNC_SetMemoData;
    NC_GetMapObjState = LibNC_GetMapObjState;
    NC_SetMapObjRange = LibNC_SetMapObjRange;
    NC_ShowMap      = LibNC_ShowMap;
    NC_MoveMap      = LibNC_MoveMap;
    NC_GetScaleList = LibNC_GetScaleList;
    NC_SetScale     = LibNC_SetScale;
    NC_NuroButtonEvent = LibNC_NuroButtonEvent;
    NC_SetDrawInfo    = LibNC_SetDrawInfo;
    NC_TraceImport  = LibNC_TraceImport;
    NC_TraceExport  = LibNC_TraceExport;
    NC_SetTimerEx   = LibNC_SetTimerEx;
    NC_KillTimerEx  = LibNC_KillTimerEx;
    NC_GetKernalState = LibNC_GetKernalState;
    NC_ResetTripInfo = LibNC_ResetTripInfo;
    NC_SetNaviThread = LibNC_SetNaviThread;    
    NC_SetGPSFunc = LibNC_SetGPSFunc;
    NC_GetLastError = LibNC_GetLastError;
    NC_GetSystemTime = LibNC_GetSystemTime;
    NC_SetCallBack   = LibNC_SetCallBack;
    NC_GetNavigationData = LibNC_GetNavigationData;

    m_hInst = (nuHINSTANCE)(-1);
#endif
    return true;
}

void CLoadNC::FreeDll()
{
    if( IsLoaden() )
    {
        NC_FreeNaviCtrl();
#ifdef _USE_NAVICTRL_DLL
        nuFreeLibrary(m_hInst);
#else
#endif
        m_hInst = NULL;
    }
}

bool CLoadNC::IsLoaden()
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