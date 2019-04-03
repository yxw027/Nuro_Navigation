// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TurningDraw.cpp,v 1.3 2010/10/29 03:13:00 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/10/29 03:13:00 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libNaviCtrl/TurningDraw.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "TurningDraw.h"
#include "NuroModuleApiStruct.h"
#include "NaviThread.h"
#include "GdiApiTrans.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern MATHTOOLAPI		g_mathtoolApi;
extern FILESYSAPI		g_fileSysApi;
extern INNAVIAPI		g_innaviApi;
extern DRAWMAPAPI		g_drawmapApi;
//extern DRAWINFOMAPAPI	g_drawIfmApi;
extern DRAWINFONAVIAPI	g_drawIfnApi;
extern class CGdiApiTrans*          g_pGdiApiTrans;

#define MAX_ANGLE_EACH_TURN					30
#define MAX_ANGLE_TURN_NUMBER				5

CTurningDraw::CTurningDraw()
{
	m_pNaviThd	= NULL;
	m_nOldMapAngle = 90;
	m_nNewMapAngle = 90;
}

CTurningDraw::~CTurningDraw()
{
	Free();
}

nuBOOL CTurningDraw::Initialize(class CNaviThread* pNaviThd)
{
	m_pNaviThd = pNaviThd;
	return nuTRUE;
}

nuVOID CTurningDraw::Free()
{

}

nuVOID CTurningDraw::SetTurnDraw(PACTIONSTATE pAcState, nuSHORT nMapAngle /* = 90 */)
{
	m_nNewMapAngle = nMapAngle;
	
	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode != MAP_DIRECT_CAR || 
		pAcState->nAngle < 0 || 
		(pAcState->nDrawMode != DRAWMODE_GPS && pAcState->nDrawMode != DRAWMODE_SIM) )
	{
		m_nOldMapAngle = m_nNewMapAngle;
		return ;
	}
	
	nuSHORT nAngle = m_nNewMapAngle - m_nOldMapAngle;
	nAngle = NURO_ABS(nAngle);
	if ( nAngle > 180 )
	{
		nAngle = 360 - 180;
	}
	if( nAngle <= MAX_ANGLE_EACH_TURN )
	{
		m_nOldMapAngle = m_nNewMapAngle;
	}
	else
	{
		m_pNaviThd->m_gDataMgr.ResetMapAngle(m_nOldMapAngle);
	}
}

nuUINT CTurningDraw::TurningProc(PACTIONSTATE pAcState)
{
	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode != MAP_DIRECT_CAR || 
		pAcState->nAngle < 0 || 
		(pAcState->nDrawMode != DRAWMODE_GPS && pAcState->nDrawMode != DRAWMODE_SIM) )
	{
		return 0;
	}
	m_nNewMapAngle = pAcState->nAngle;
	if( m_nOldMapAngle == m_nNewMapAngle )
	{
		return 0;
	}
	//
	nuSHORT nAngle = m_nOldMapAngle - m_nNewMapAngle;
	nAngle = NURO_ABS(nAngle);
	if ( nAngle > 180 )
	{
		nAngle = 360 - 180;
	}
	if( nAngle <= MAX_ANGLE_EACH_TURN )
	{
		m_nOldMapAngle = m_nNewMapAngle;
		return 0;
	}
	nuSHORT nDiff= m_nOldMapAngle - m_nNewMapAngle;
	if( nDiff > 0 )
	{
		if( nDiff < 180 )
		{
			nDiff = -MAX_ANGLE_EACH_TURN;
		}
		else
		{
			nDiff = MAX_ANGLE_EACH_TURN;
		}
	}
	else
	{
		nDiff = NURO_ABS(nDiff);
		if( nDiff < 180 )
		{
			nDiff = MAX_ANGLE_EACH_TURN;
		}
		else
		{
			nDiff = -MAX_ANGLE_EACH_TURN;
		}
	}
	m_pNaviThd->m_gDataMgr.ResetMapAngle(m_nOldMapAngle);
	while ( nAngle > MAX_ANGLE_EACH_TURN )
	{
		/*
		if( NURO_ABS(nDiff) > nAngle )
		{
			nDiff = (nDiff > 0) ? nAngle : (-nAngle);
		}
		*/
		nAngle -= MAX_ANGLE_EACH_TURN;
		m_pNaviThd->m_gDataMgr.ResetMapAngle(nDiff, MAPANGLE_SETTYPE_PROGRESS);
		m_pNaviThd->m_gDataMgr.CalMapShowInfo(pAcState);//@zhikun 2009.07.21
		if( !g_drawmapApi.DM_LoadMap(pAcState) ||	//@zhikun 2009.07.21
			!g_drawmapApi.DM_DrawMapBmp1(pAcState) )
		{
			continue;
		}
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
		}
		if(	!g_drawmapApi.DM_DrawMapBmp2(pAcState) )
		{
			continue;
		}
// 		if( g_fileSysApi.pGdata->uiSetData.b3DMode )
// 		{
// 			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_3D_BMP);
// 		}
//		g_drawIfmApi.IFM_DrawItems(NULL, g_fileSysApi.pDrawInfo);
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_drawIfnApi.IFN_DrawItems(g_fileSysApi.pDrawInfo);
		}
		g_mathtoolApi.MT_SaveOrShowBmp(SHOW_MAP_SCREEN);
#if defined(_WINDOWS) && !defined(_WIN32_WCE)
		nuSleep(100);
#endif
	}
	m_pNaviThd->m_gDataMgr.ResetMapAngle(m_nNewMapAngle);
	m_nOldMapAngle = m_nNewMapAngle;
	return 1;
}
nuUINT CTurningDraw::TurningProc(PACTIONSTATE pAcState, nuSHORT nNowAngle, nuSHORT nDesAngle)
{
	//
	nuSHORT nAngle = nDesAngle - nNowAngle;
	nAngle = NURO_ABS(nAngle);
	if ( nAngle > 180 )
	{
		nAngle = 360 - nAngle;
	}
	nuSHORT nEachTurnAngle = nAngle;
	if(nEachTurnAngle % MAX_ANGLE_TURN_NUMBER == 0)
	{
		nEachTurnAngle /= MAX_ANGLE_TURN_NUMBER;
	}
	else
	{
		nEachTurnAngle /= MAX_ANGLE_TURN_NUMBER;
		nEachTurnAngle = nEachTurnAngle +1;
	}
	nuSHORT nDiff= nNowAngle - nDesAngle;
	if( nDiff > 0 )
	{
		if( nDiff < 180 )
		{		
			nDiff = -nEachTurnAngle;
		}
		else
		{
			nDiff = nEachTurnAngle;
		}
	}
	else
	{
		nDiff = NURO_ABS(nDiff);
		if( nDiff < 180 )
		{
			nDiff = nEachTurnAngle;
		}
		else
		{
			nDiff = -nEachTurnAngle;
		}
	}
	while ( nAngle > 0)
	{
		if( NURO_ABS(nDiff) >= nAngle )
		{
			m_pNaviThd->m_gDataMgr.ResetMapAngle( nDesAngle );
		}
		else
		{
			m_pNaviThd->m_gDataMgr.ResetMapAngle(nDiff, MAPANGLE_SETTYPE_PROGRESS);
		}
		nAngle -= nEachTurnAngle;
		pAcState->bDrawPOIName = 0;
		pAcState->bDrawRoadName = 0;
		if( !g_drawmapApi.DM_DrawMapBmp1(pAcState) )
		{
			continue;
		}
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
		}
		if(	!g_drawmapApi.DM_DrawMapBmp2(pAcState) )
		{
			continue;
		}
		g_mathtoolApi.MT_SaveOrShowBmp(SAVETOMEM_BMP1);//Save map to MemBmp1
		if(m_pNaviThd->m_pfSaveMapBmp )
		{
			if( NULL != g_pGdiApiTrans )
			{
				m_pNaviThd->m_pfSaveMapBmp(g_pGdiApiTrans->nuGdiGetCanvas());	
		}
		}
		//g_mathtoolApi.MT_SaveOrShowBmp(SHOWFROMMEM_BMP1);//Get the pro

		//g_mathtoolApi.MT_SaveOrShowBmp(SHOW_MAP_SCREEN);
#if defined(_WINDOWS) && !defined(_WIN32_WCE)
		nuSleep(80);
#else
		nuSleep(5);
#endif
	}
	return 1;
}
