#ifndef __NURO_LOADNUROSDK_20091123
#define __NURO_LOADNUROSDK_20091123

#ifdef NURO_OS_WINDOWS
#include <windows.h>
#include <tchar.h>
#include "nuSDKDefine.h"

class CLoadNuroSDK
{
public:
	CLoadNuroSDK();
	virtual ~CLoadNuroSDK();

	bool LoadDll();
	void FreeDll();
	bool IsLoaden();

public:
	typedef bool(*SDK_InitNuroSDKProc)(HWND, HDC, WORD, WORD);
	SDK_InitNuroSDKProc SDK_InitNuroSDK;

	typedef void(*SDK_FreeNuroSDKProc)();
	SDK_FreeNuroSDKProc SDK_FreeNuroSDK;

	typedef long(*SDK_GetNowScaleProc)(long&, bool&);
	SDK_GetNowScaleProc SDK_GetNowScale;

	typedef long*(*SDK_GetScaleListProc)(long*);
	SDK_GetScaleListProc SDK_GetScaleList;

	typedef bool(*SDK_SetScaleProc)(const long, const bool);
	SDK_SetScaleProc SDK_SetScale;

	typedef void(*SDK_SetScaleZoomInProc)();
	SDK_SetScaleZoomInProc SDK_SetScaleZoomIn;

	typedef void(*SDK_SetScaleZoomOutProc)();
	SDK_SetScaleZoomOutProc SDK_SetScaleZoomOut;

	typedef bool(*SDK_SetMapCenterXYProc)(const long , const long );
	SDK_SetMapCenterXYProc SDK_SetMapCenterXY;

	typedef bool(*SDK_GetMapCenterXYProc)(long& , long& );
	SDK_GetMapCenterXYProc SDK_GetMapCenterXY;

	typedef UINT(*SDK_MouseDownProc)(const long, const long); 
	SDK_MouseDownProc SDK_MouseDown;

	typedef UINT(*SDK_MouseUpProc)(const long, const long);
	SDK_MouseUpProc SDK_MouseUp;

	typedef UINT(*SDK_MouseMoveProc)(const long, const long);
	SDK_MouseMoveProc SDK_MouseMove;

	typedef UINT(*SDK_KeyXXProc)(const nuUINT);
	SDK_KeyXXProc SDK_KeyUp;
	SDK_KeyXXProc SDK_KeyDown;

	typedef BYTE(*SDK_GetDayNightTypeProc)();
	SDK_GetDayNightTypeProc SDK_GetDayNightType;

	typedef void(*SDK_SetDayNightTypeProc)(const nuBYTE);
	SDK_SetDayNightTypeProc SDK_SetDayNightType;

	typedef BYTE(*SDK_GetMapViewTypeProc)();
	SDK_GetMapViewTypeProc SDK_GetMapViewType;

	typedef void(*SDK_SetMapViewTypeProc)(const nuBYTE);
	SDK_SetMapViewTypeProc SDK_SetMapViewType;

	typedef BYTE(*SDK_GetMapDirectModeProc)();
	SDK_GetMapDirectModeProc SDK_GetMapDirectMode;

	typedef void(*SDK_SetMapDirectModeProc)(const nuBYTE);
	SDK_SetMapDirectModeProc SDK_SetMapDirectMode;

	typedef bool(*SDK_SH_StartSearchProc)();
	SDK_SH_StartSearchProc SDK_SH_StartSearch;

	typedef void(*SDK_SH_EndSearchProc)();
	SDK_SH_EndSearchProc SDK_SH_EndSearch;

	typedef int(*SDK_SH_GetMapNameAllProc)(WCHAR*&);
	SDK_SH_GetMapNameAllProc SDK_SH_GetMapNameAll;

	typedef int(*SDK_SH_GetCityNameAllProc)(WCHAR*&);
	SDK_SH_GetCityNameAllProc SDK_SH_GetCityNameAll;

	typedef int(*SDK_SH_GetTownNameAllProc)(WCHAR*&);
	SDK_SH_GetTownNameAllProc SDK_SH_GetTownNameAll;

	typedef	bool(*SDK_SH_SetMapIdProc)(long);
	SDK_SH_SetMapIdProc SDK_SH_SetMapId;

	typedef	bool(*SDK_SH_SetCityIdProc)(long);
	SDK_SH_SetCityIdProc SDK_SH_SetCityId;

	typedef	bool(*SDK_SH_SetTownIdProc)(long);
	SDK_SH_SetTownIdProc SDK_SH_SetTownId;
	
	typedef int(*SDK_SH_GetPoiTP1AllProc)(WCHAR*&);
	SDK_SH_GetPoiTP1AllProc SDK_SH_GetPoiTP1All;

	typedef int(*SDK_SH_GetPoiTP2AllProc)(WCHAR*&);
	SDK_SH_GetPoiTP2AllProc SDK_SH_GetPoiTP2All;

	typedef bool(*SDK_SH_SetPoiTP1Proc)(long);
	SDK_SH_SetPoiTP1Proc SDK_SH_SetPoiTP1;

	typedef bool(*SDK_SH_SetPoiTP2Proc)(long);
	SDK_SH_SetPoiTP2Proc SDK_SH_SetPoiTP2;

	typedef bool(*SDK_SH_SetPoiKeyNameProc)(WCHAR*);
	SDK_SH_SetPoiKeyNameProc SDK_SH_SetPoiKeyName;

	typedef bool(*SDK_SH_SetSearchAreaProc)(const long);
	SDK_SH_SetSearchAreaProc SDK_SH_SetSearchArea;

	typedef UINT(*SDK_SH_GetPoiTotalProc)();
	SDK_SH_GetPoiTotalProc SDK_SH_GetPoiTotal;

	typedef UINT(*SDK_SH_GetPoiListProc)(WORD, WCHAR*&);
    SDK_SH_GetPoiListProc SDK_SH_GetPoiList;

	typedef bool(*SDK_SH_GetPoiPosProc)(UINT, long&, long&);
	SDK_SH_GetPoiPosProc SDK_SH_GetPoiPos;

	typedef bool(*SDK_SH_SetRoadKeyNameProc)(WCHAR *pName);
	SDK_SH_SetRoadKeyNameProc SDK_SH_SetRoadKeyName;

	typedef UINT(*SDK_SH_GetRoadTotalProc)();
	SDK_SH_GetRoadTotalProc SDK_SH_GetRoadTotal;

	typedef UINT(*SDK_SH_GetRoadNameListProc)(WORD, WCHAR*&);
	SDK_SH_GetRoadNameListProc SDK_SH_GetRoadNameList;

	typedef bool(*SDK_SH_SetRoadIdxProc)(UINT);
	SDK_SH_SetRoadIdxProc SDK_SH_SetRoadIdx;

	typedef bool(*SDK_SH_GetRoadPosProc)(long&, long&);
	SDK_SH_GetRoadPosProc SDK_SH_GetRoadPos;

	typedef UINT(*SDK_SH_GetRoadCrossTotalProc)();
	SDK_SH_GetRoadCrossTotalProc SDK_SH_GetRoadCrossTotal;

	typedef int(*SDK_SH_GetRoadCrossListProc)(WORD, WCHAR*&);
	SDK_SH_GetRoadCrossListProc SDK_SH_GetRoadCrossList;
	
	typedef bool(*SDK_SH_GetRoadCrossPosProc)(UINT, long&, long&);
	SDK_SH_GetRoadCrossPosProc SDK_SH_GetRoadCrossPos;

	typedef bool(*SDK_bSetXXXXNodeProc)(const long c_lX = 0,
		                                const long c_ly = 0,
						                WCHAR*  pwName  = NULL);
	SDK_bSetXXXXNodeProc SDK_bSetStartNode;
	SDK_bSetXXXXNodeProc SDK_bSetPassNode;

	typedef bool(*SDK_bNavigation_XXXRouteProc)();
	SDK_bNavigation_XXXRouteProc SDK_bNavigation_SimRoute;
	SDK_bNavigation_XXXRouteProc SDK_bNavigation_GpsRoute;
	
protected:
	HINSTANCE	m_hInst;
};
#endif

#endif // !defined(AFX_LOADNC_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_)
