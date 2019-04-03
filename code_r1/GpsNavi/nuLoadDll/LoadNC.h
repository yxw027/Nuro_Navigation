// LoadNC.h: interface for the CLoadNC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADNC_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_)
#define AFX_LOADNC_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

#ifdef _WINDOWS
#define _USE_NAVICTRL_DLL
#endif
//#define _USE_NAVICTRL_LIB

class CLoadNC 
{
public:
    CLoadNC();
    virtual ~CLoadNC();

    bool LoadDll();
    void FreeDll();
    bool IsLoaden();
public:
    
	typedef void(*NC_SetNaviStateProc)(nuUINT NaviState);
    NC_SetNaviStateProc NC_SetNaviState;
	
	typedef nuUINT(*NC_Start3DDemoProc)(nuTCHAR*);
    NC_Start3DDemoProc NC_Start3DDemo;

	typedef nuUINT(*NC_Close3DDemoProc)();
    NC_Close3DDemoProc NC_Close3DDemo;

    typedef nuUINT(*NC_StartOpenGLProc)(nuHWND, nuHDC, const NURORECT*);
    NC_StartOpenGLProc NC_StartOpenGL;

    typedef bool(*NC_InitNaviCtrlProc)(nuHDC, nuWORD, nuWORD, nuPVOID*);
    NC_InitNaviCtrlProc     NC_InitNaviCtrl;
    typedef void(*NC_FreeNaviCtrlProc)();
    NC_FreeNaviCtrlProc NC_FreeNaviCtrl;
    typedef bool(*NC_StartNaviCtrlProc)(nuPVOID);
    NC_StartNaviCtrlProc    NC_StartNaviCtrl;


    typedef bool(*NC_OnPaintProc)();
    NC_OnPaintProc    NC_OnPaint;

    typedef nuUINT(*NC_KeyProc)(nuUINT);
    NC_KeyProc    NC_KeyDown;
    NC_KeyProc    NC_KeyUp;

    typedef bool(*NC_SetUserCfgProc)();
    NC_SetUserCfgProc    NC_SetUserCfg;

    typedef bool(*NC_ZoomProc)(bool);
    NC_ZoomProc    NC_Zoom;

    typedef nuUINT(*NC_MouseProc)(long, long);
    NC_MouseProc    NC_MouseDown;
    NC_MouseProc    NC_MouseUp;
    NC_MouseProc    NC_MouseMove;

    typedef bool(*NC_SetMarketPtProc)(nuPVOID);
    NC_SetMarketPtProc    NC_SetMarketPt;

    typedef bool(*NC_DelMarketPtProc)(nuPVOID);
    NC_DelMarketPtProc NC_DelMarketPt;

    typedef bool(*NC_GpsSendInProc)(char *, nuUINT);
    NC_GpsSendInProc    NC_GpsSendIn;

    typedef bool(*NC_ShilftNaviThreadProc)(bool);
    NC_ShilftNaviThreadProc    NC_ShilftNaviThread;

    typedef bool(*NC_SoundPlayProc)(nuBYTE, nuWORD, nuWORD, nuWORD, nuWORD, nuWCHAR *);
    NC_SoundPlayProc    NC_SoundPlay;

    typedef bool(*NC_SetVoiceProc)(nuWORD, bool);
    NC_SetVoiceProc    NC_SetVoice;

    typedef bool(*NC_SetTimerProc)(nuPVOID, nuDWORD);
    NC_SetTimerProc    NC_SetTimer;
    
    typedef nuPVOID(*NC_GetRoutingListProc)(int, int*);
    NC_GetRoutingListProc NC_GetRoutingList;

    typedef nuPVOID(*NC_GetGPSDataProc)();
    NC_GetGPSDataProc    NC_GetGPSData;

    typedef bool(*NC_GetTripInfoProc)(nuPVOID);
    NC_GetTripInfoProc NC_GetTripInfo;

    typedef bool(*NC_ResetTripInfoProc)(nuPVOID lpVoid);
    NC_ResetTripInfoProc NC_ResetTripInfo;

    typedef nuPVOID(*NC_GetMapInfoProc)(nuUINT);
    NC_GetMapInfoProc    NC_GetMapInfo;

    typedef bool(*NC_MoveToMapXYProc)(long,long);
    NC_MoveToMapXYProc NC_MoveToMapXY;

    typedef bool(*NC_NavigationProc)(nuBYTE);
    NC_NavigationProc NC_Navigation;

    typedef bool(*NC_OperateMapXYProc)(long,long,nuUINT);
    NC_OperateMapXYProc NC_OperateMapXY;

    typedef bool(*NC_MessageBoxProc)(nuUINT, nuPVOID);
    NC_MessageBoxProc    NC_MessageBox;

    typedef bool(*NC_IsNavigationProc)();
    NC_IsNavigationProc  NC_IsNavigation;

    typedef bool(*NC_ConfigRouteProc)(long);
    NC_ConfigRouteProc NC_ConfigRoute;

    typedef bool(*NC_ShowHideButtonProc)(nuUINT,nuBYTE);
    NC_ShowHideButtonProc NC_ShowHideButton;

    typedef bool(*NC_CoverDefaultUserCfgProc)();
    NC_CoverDefaultUserCfgProc NC_CoverDefaultUserCfg;

    typedef nuUINT(*NC_GetNaviModeProc)();
    NC_GetNaviModeProc NC_GetNaviMode;

    typedef nuPVOID(*NC_GetTraceHeadProc)();
    NC_GetTraceHeadProc NC_GetTraceHead;

    typedef nuUINT(*NC_StartTraceProc)(nuBYTE);
    NC_StartTraceProc NC_StartTrace;

    typedef nuUINT(*NC_CloseTraceProc)(nuBYTE);
    NC_CloseTraceProc NC_CloseTrace;

    typedef nuUINT(*NC_ShowTraceProc)(nuWORD);
    NC_ShowTraceProc NC_ShowTrace;

    typedef nuUINT(*NC_ClosShowProc)();
    NC_ClosShowProc NC_ClosShow;

    typedef nuUINT(*NC_DeleteTraceProc)(nuWORD);
    NC_DeleteTraceProc NC_DeleteTrace;

    typedef nuUINT(*NC_GetTracePointProc)(nuroPOINT&, nuWORD, nuDWORD);
    NC_GetTracePointProc NC_GetTracePoint;

    typedef nuUINT(*NC_SetMemoDataProc)(nuPVOID);
    NC_SetMemoDataProc NC_SetMemoData;

    typedef nuUINT(*NC_GetMapObjStateProc)(nuUINT, nuBYTE&);
    NC_GetMapObjStateProc NC_GetMapObjState;

    typedef nuUINT(*NC_SetMapObjRangeProc)(nuUINT, nuroSRECT&, short&);
    NC_SetMapObjRangeProc NC_SetMapObjRange;

    typedef nuUINT(*NC_ShowMapProc)(nuBYTE);
    NC_ShowMapProc NC_ShowMap;

    typedef nuUINT(*NC_MoveMapProc)(nuBYTE, bool);
    NC_MoveMapProc NC_MoveMap;

    typedef long*(*NC_GetScaleListProc)(long*);
    NC_GetScaleListProc NC_GetScaleList;

    typedef bool(*NC_SetScaleProc)(long);
    NC_SetScaleProc NC_SetScale;

    typedef nuUINT(*NC_NuroButtonEventProc)(nuUINT,nuBYTE);
    NC_NuroButtonEventProc NC_NuroButtonEvent;

    typedef bool(*NC_SetDrawInfoProc)(nuPVOID);
    NC_SetDrawInfoProc    NC_SetDrawInfo;

    typedef nuUINT(*NC_TraceImportProc)();
    NC_TraceImportProc NC_TraceImport;

    typedef nuUINT(*NC_TraceExportProc)();
    NC_TraceExportProc NC_TraceExport;

    typedef bool(*NC_SetTimerExProc)(nuDWORD,nuDWORD, nuPVOID);
    NC_SetTimerExProc NC_SetTimerEx;

    typedef bool(*NC_KillTimerExProc)(nuDWORD);
    NC_KillTimerExProc NC_KillTimerEx;

    typedef nuUINT(*NC_GetKernalStateProc)(PNC_KERNALE_STATE pNcKernalState);
    NC_GetKernalStateProc NC_GetKernalState;

    typedef bool(*NC_SetNaviThreadProc)(nuBYTE nNaviState);
    NC_SetNaviThreadProc NC_SetNaviThread;

    typedef nuUINT(*NC_SetGPSFuncProc)(nuPVOID pFunc);
    NC_SetGPSFuncProc NC_SetGPSFunc;

    typedef nuUINT(*NC_GetLastErrorProc)();
    NC_GetLastErrorProc NC_GetLastError;

    typedef nuUINT(*NC_GetSystemTimeProc)(PNURO_SYS_TIME pNuroTime);
    NC_GetSystemTimeProc NC_GetSystemTime;

    typedef    nuUINT(*NC_SetCallBackProc)(nuPVOID pFunc);
    NC_SetCallBackProc NC_SetCallBack;

    typedef nuUINT(*NC_GetNavigationDataProc)(PNC_NAVI_DATA pNcNaviData);
    NC_GetNavigationDataProc NC_GetNavigationData;

protected:
    nuHINSTANCE    m_hInst;
};

#endif // !defined(AFX_LOADNC_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_)
