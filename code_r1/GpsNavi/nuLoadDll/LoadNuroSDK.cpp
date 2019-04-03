#ifdef NURO_OS_WINDOWS

#include "LoadNuroSDK.h"

CLoadNuroSDK::CLoadNuroSDK()
{
	m_hInst = NULL;
}

CLoadNuroSDK::~CLoadNuroSDK()
{
}

bool CLoadNuroSDK::LoadDll()
{
	if (true == IsLoaden())
	{   // 已加載過了
		return false;
	}	

	TCHAR tsFile[256];
	memset(tsFile, NULL, sizeof(tsFile));

	GetModuleFileName(NULL, tsFile, sizeof(tsFile));

	for (int i=_tcslen(tsFile); i>=0; i-- )
	{
		if( tsFile[i] == '\\' )
		{
			tsFile[i+1] = '\0';
			break;
		}
	}

	_tcscat(tsFile, TEXT("Setting\\NuroSDK.dll"));

	m_hInst = LoadLibrary(tsFile);

	if( m_hInst == NULL )
	{
		return false;
	}

	SDK_InitNuroSDK         = (SDK_InitNuroSDKProc)GetProcAddress(m_hInst, TEXT("LibSDK_InitNuroSDK"));
	SDK_FreeNuroSDK         = (SDK_FreeNuroSDKProc)GetProcAddress(m_hInst, TEXT("LibSDK_FreeNuroSDK"));
	SDK_GetNowScale         = (SDK_GetNowScaleProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetNowScale"));
	SDK_GetScaleList        = (SDK_GetScaleListProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetScaleList"));
	SDK_SetScale            = (SDK_SetScaleProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetScale"));
	SDK_SetScaleZoomIn      = (SDK_SetScaleZoomInProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetScaleZoomIn"));
	SDK_SetScaleZoomOut     = (SDK_SetScaleZoomOutProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetScaleZoomOut"));
	SDK_SetMapCenterXY      = (SDK_SetMapCenterXYProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetMapCenterXY"));
	SDK_GetMapCenterXY      = (SDK_GetMapCenterXYProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetMapCenterXY"));
	SDK_MouseDown           = (SDK_MouseDownProc)GetProcAddress(m_hInst, TEXT("LibSDK_MouseDown"));
	SDK_MouseUp             = (SDK_MouseUpProc)GetProcAddress(m_hInst, TEXT("LibSDK_MouseUp"));
	SDK_MouseMove           = (SDK_MouseMoveProc)GetProcAddress(m_hInst, TEXT("LibSDK_MouseMove"));
	SDK_KeyUp               = (SDK_KeyXXProc)GetProcAddress(m_hInst, TEXT("LibSDK_KeyUp"));
	SDK_KeyDown             = (SDK_KeyXXProc)GetProcAddress(m_hInst, TEXT("LibSDK_KeyDown"));
	SDK_GetDayNightType     = (SDK_GetDayNightTypeProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetDayNightType"));
	SDK_SetDayNightType     = (SDK_SetDayNightTypeProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetDayNightType"));
	SDK_GetMapViewType      = (SDK_GetMapViewTypeProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetMapViewType"));
	SDK_SetMapViewType      = (SDK_SetMapViewTypeProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetMapViewType"));
	SDK_GetMapDirectMode    = (SDK_GetMapDirectModeProc)GetProcAddress(m_hInst, TEXT("LibSDK_GetMapDirectMode"));
	SDK_SetMapDirectMode    = (SDK_SetMapDirectModeProc)GetProcAddress(m_hInst, TEXT("LibSDK_SetMapDirectMode"));
	SDK_SH_StartSearch      = (SDK_SH_StartSearchProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_StartSearch"));
	SDK_SH_EndSearch        = (SDK_SH_EndSearchProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_EndSearch"));
	SDK_SH_GetMapNameAll    = (SDK_SH_GetMapNameAllProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetMapNameAll"));
	SDK_SH_GetCityNameAll   = (SDK_SH_GetCityNameAllProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetCityNameAll"));
	SDK_SH_GetTownNameAll   = (SDK_SH_GetTownNameAllProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetTownNameAll"));
	SDK_SH_SetMapId         = (SDK_SH_SetMapIdProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetMapId"));
	SDK_SH_SetCityId        = (SDK_SH_SetCityIdProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetCityId"));
	SDK_SH_SetTownId        = (SDK_SH_SetTownIdProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetTownId"));
	SDK_SH_GetPoiTP1All     = (SDK_SH_GetPoiTP1AllProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetPoiTP1All"));
	SDK_SH_GetPoiTP2All     = (SDK_SH_GetPoiTP2AllProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetPoiTP2All"));
	SDK_SH_SetPoiTP1        = (SDK_SH_SetPoiTP1Proc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetPoiTP1"));
	SDK_SH_SetPoiTP2        = (SDK_SH_SetPoiTP2Proc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetPoiTP2"));
	SDK_SH_SetPoiKeyName    = (SDK_SH_SetPoiKeyNameProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetPoiKeyName"));
	SDK_SH_SetSearchArea    = (SDK_SH_SetSearchAreaProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetSearchArea"));
	SDK_SH_GetPoiTotal      = (SDK_SH_GetPoiTotalProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetPoiTotal"));
	SDK_SH_GetPoiList       = (SDK_SH_GetPoiListProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetPoiList"));
	SDK_SH_GetPoiPos        = (SDK_SH_GetPoiPosProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetPoiPos"));
	SDK_SH_SetRoadKeyName   = (SDK_SH_SetRoadKeyNameProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetRoadKeyName"));
	SDK_SH_GetRoadTotal     = (SDK_SH_GetRoadTotalProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadTotal"));
	SDK_SH_GetRoadNameList  = (SDK_SH_GetRoadNameListProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadNameList"));
	SDK_SH_SetRoadIdx       = (SDK_SH_SetRoadIdxProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_SetRoadIdx"));
	SDK_SH_GetRoadPos       = (SDK_SH_GetRoadPosProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadPos"));
	SDK_SH_GetRoadCrossTotal= (SDK_SH_GetRoadCrossTotalProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadCrossTotal"));
	SDK_SH_GetRoadCrossList = (SDK_SH_GetRoadCrossListProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadCrossList"));
	SDK_SH_GetRoadCrossPos  = (SDK_SH_GetRoadCrossPosProc)GetProcAddress(m_hInst, TEXT("LibSDK_SH_GetRoadCrossPos"));
	SDK_bSetStartNode       = (SDK_bSetXXXXNodeProc)GetProcAddress(m_hInst, TEXT("LibSDK_bSetStartNode"));
	SDK_bSetPassNode        = (SDK_bSetXXXXNodeProc)GetProcAddress(m_hInst, TEXT("LibSDK_bSetPassNode"));
	SDK_bNavigation_SimRoute= (SDK_bNavigation_XXXRouteProc)GetProcAddress(m_hInst, TEXT("LibSDK_bNavigation_SimRoute"));
	SDK_bNavigation_GpsRoute= (SDK_bNavigation_XXXRouteProc)GetProcAddress(m_hInst, TEXT("LibSDK_bNavigation_GpsRoute"));

	return true;
}

void CLoadNuroSDK::FreeDll()
{
	if (true == IsLoaden())
	{
		SDK_FreeNuroSDK();
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

bool CLoadNuroSDK::IsLoaden()
{
	if (NULL != m_hInst)
	{
		return true;
	}
	return false;
}
#endif
