// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ZoomDraw.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/ZoomDraw.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "ZoomDraw.h"
#include "NuroModuleApiStruct.h"
#include "NaviThread.h"

extern MATHTOOLAPI		g_mathtoolApi;
extern DRAWMAPAPI		g_drawmapApi;
extern FILESYSAPI		g_fileSysApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZoomDraw::CZoomDraw()
{

}

CZoomDraw::~CZoomDraw()
{
	Free();
}

nuBOOL CZoomDraw::Initialize(class CNaviThread* pNaviThd)
{
	m_nZoomType = ZOOM_NO;
	m_pNaviThd = pNaviThd;
	return nuTRUE;
}

nuVOID CZoomDraw::Free()
{

}

#define ZOOM_FAKE_TIMES							5
nuBOOL CZoomDraw::ZoomProcess(PACTIONSTATE pActionState)
{
	if( m_nZoomType == ZOOM_NO || m_pNaviThd == NULL)
	{
		return nuFALSE;
	}
	else if( m_nZoomType == ZOOM_IN )
	{
		for( nuINT i = 1; i <= ZOOM_FAKE_TIMES; i++ )
		{
			nuINT nScale = (m_nNewScaleValue - m_nOldScaleValue) * i / ZOOM_FAKE_TIMES + m_nOldScaleValue;
			m_pNaviThd->m_gDataMgr.ZoomSet(nScale);
#if defined(_WINDOWS) /*&& !defined(_WIN32_WCE)*/
#ifdef _WIN32_WCE
			//nuSleep(25);
#else
			nuSleep(100);
#endif
#else
			/*
			if( --nSleep == 0 )
			{
				nSleep = TIMES_TO_SLEEP;
				nuSleep(25);
			}*/
#endif
			g_drawmapApi.DM_DrawZoomFake(NULL);
			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_BMP1_SCREEN);
		}
	}
	else if( m_nZoomType == ZOOM_OUT )
	{
		for( nuINT i = 1; i <= ZOOM_FAKE_TIMES; i++ )
		{
			nuINT nScale = (m_nNewScaleValue - m_nOldScaleValue) * i / ZOOM_FAKE_TIMES + m_nOldScaleValue;
			m_pNaviThd->m_gDataMgr.ZoomSet(nScale);
#if defined(_WINDOWS) /*&& !defined(_WIN32_WCE)*/
#ifdef _WIN32_WCE
			nuSleep(25);
#else
			nuSleep(100);
#endif
#else
			/*
			if( --nSleep == 0 )
			{
				nSleep = TIMES_TO_SLEEP;
				nuSleep(25);
			}*/
#endif
			g_drawmapApi.DM_DrawZoomFake(NULL);
			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_BMP1_SCREEN);
		}
	}
	m_nZoomType = ZOOM_NO;
	return nuTRUE;
}
nuBOOL CZoomDraw::ZoomProcessEx(PACTIONSTATE pActionState, nuDOUBLE fZoomDis)
{
	nuLONG nScaleIdx = g_fileSysApi.pUserCfg->nScaleIdx;
	nuDOUBLE fZoom = 0;
__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "DragZoomDraw m_nNewScaleValue %d, fZoomDis %f, m_lZoomScale %d", m_nNewScaleValue, fZoomDis, m_lZoomScale);
	if(fZoomDis >= 1 && (m_nNewScaleValue - fZoomDis) > 10)
	{
		m_nZoomType = ZOOM_IN;	
		m_nNewScaleValue -= fZoomDis;	
		__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "ZOOM_IN m_nNewScaleValue %d, fZoomDis %f", m_nNewScaleValue, fZoomDis);
	}
	else if(fZoomDis >= 0.1 && fZoomDis <= 1)
	{
		m_nZoomType = ZOOM_OUT;	
		fZoom 	    = (1 / fZoomDis);
		m_nNewScaleValue += fZoom;
		__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "ZOOM_IN m_nNewScaleValue %d, fZoom %f", m_nNewScaleValue, fZoom);
	}
	else
	{
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "DragZoomDraw... m_nNewScaleValue %d, fZoomDis %f, m_lZoomScale %d", m_nNewScaleValue, fZoomDis, m_lZoomScale);
	m_lZoomScale = m_nNewScaleValue;
	if( m_nZoomType == ZOOM_NO || m_pNaviThd == NULL) {
		return nuFALSE;
	}
	else if( m_nZoomType == ZOOM_IN ) {
		g_fileSysApi.pGdata->uiSetData.nScaleValue = m_nNewScaleValue;
		if(m_nNewScaleValue < g_fileSysApi.pMapCfg->pScaleList[nScaleIdx])
		{
			if(nScaleIdx-- > 0)
			{
				g_fileSysApi.pUserCfg->nScaleIdx--;
				g_fileSysApi.FS_ReLoadMapConfig();
			}
			else
			{
				return nuTRUE;
			}
		}		
	}
	else if( m_nZoomType == ZOOM_OUT )
	{
		//m_nNewScaleValue = m_lZoomScale;
		g_fileSysApi.pGdata->uiSetData.nScaleValue = m_nNewScaleValue;
		if(m_nNewScaleValue > g_fileSysApi.pMapCfg->pScaleList[nScaleIdx])
		{
			if(nScaleIdx++ < g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount)
			{
				g_fileSysApi.pUserCfg->nScaleIdx++;
				g_fileSysApi.FS_ReLoadMapConfig();
			}
			else
			{
				return nuTRUE;
			}
		}		
	}
	nuINT nScale = (m_nNewScaleValue - m_nOldScaleValue) + m_nOldScaleValue;
	__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "nScale %d, m_nOldScaleValue %d", nScale, m_nOldScaleValue);
	m_pNaviThd->m_gDataMgr.ZoomSet(nScale);
	g_drawmapApi.DM_DrawZoomFake(NULL);
	g_mathtoolApi.MT_SaveOrShowBmp(SHOW_BMP1_SCREEN);
	m_nZoomType = ZOOM_NO;
	return nuTRUE;
}
