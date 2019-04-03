// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: GlideMove.cpp,v 1.3 2010/05/18 09:54:41 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/05/18 09:54:41 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libNaviCtrl/GlideMove.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "GlideMove.h"
#include "NuroModuleApiStruct.h"
#include "NaviThread.h"

extern GDIAPI		g_gdiApi;
extern MATHTOOLAPI	g_mathtoolApi;
extern FILESYSAPI	g_fileSysApi;
extern INNAVIAPI	g_innaviApi;
extern DRAWMAPAPI	g_drawmapApi;
extern DRAWINFOMAPAPI				g_drawIfmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlideMove::CGlideMove()
{
	m_nNewMoveDirt	= MOVE_STOP;
	m_nOldMoveDirt	= MOVE_STOP;
	m_pNaviThd		= NULL;
	m_nMaxStep		= 100;
}

CGlideMove::~CGlideMove()
{
	Free();
}

nuBOOL CGlideMove::Initialzie(class CNaviThread* pNaviThd)
{
	m_pNaviThd	= pNaviThd;
	nuLONG nTmp = 0;
	if( !nuReadConfigValue("GLIDEMOVEDIS", &nTmp) )
	{
		nTmp = 5;
	}
	m_nStartDis = nuLOWORD(nTmp);
	if( !nuReadConfigValue("GLIDEMOVESLPI", &nTmp) )
	{
		nTmp = 5;
	}
	m_nTimesToSleep = nuLOWORD(nTmp);
	if( !nuReadConfigValue("GLIDEMOVESLPT", &nTmp) )
	{
		nTmp = 10;
	}
	m_nSleepTime = nuLOWORD(nTmp);
	return nuTRUE;
}

nuVOID CGlideMove::Free()
{

}

nuBOOL CGlideMove::StartMove(nuBYTE nDirect)
{
	if( m_pNaviThd == NULL )
	{
		return nuFALSE;
	}
	m_nNewMoveDirt	= nDirect;
	return nuTRUE;
}

nuBOOL CGlideMove::StopMove(nuBYTE nDirect)
{
	if( nDirect == m_nNewMoveDirt || nDirect == MOVE_STOP )
	{
		m_nNewMoveDirt = MOVE_STOP;
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}
 
#define TIMES_TO_SLEEP						5
nuBOOL CGlideMove::MoveProc(PACTIONSTATE pActionState)
{
	if( m_pNaviThd == NULL )
	{
		return nuFALSE;
	}
	if( m_nNewMoveDirt == MOVE_STOP )
	{
		return nuFALSE;
	}
	nuroPOINT ptTmp = {0}, ptConj = {0}, ptS1 = {0}, ptS2 = {0}, ptCenter = {0};
	nuBOOL bMoving = nuTRUE;
	nuBOOL bDrawNextPage = nuTRUE;
	nuBOOL bHasDrawCenter = nuFALSE;
	nuBYTE nLastDirt      = 0;
	nuLONG nLencopy		  = 0;
	m_nOldMoveDirt = m_nNewMoveDirt;
	/* 20081128
#if defined(_WINDOWS) && !defined(_WIN32_WCE)
	nuWORD nStep = m_nStep = 1;
#endif

#ifdef _WIN32_WCE
	nuWORD nStep = m_nStep = 2;//g_fileSysApi.pGdata->transfData.nBmpWidth/320;
#endif

#ifdef NURO_OS_UCOS
	nuWORD nStep = m_nStep = 5;//g_fileSysApi.pGdata->transfData.nBmpWidth/320;
#endif
	*/
	m_nStep = m_nStartDis;//
	nuWORD nSleep = m_nTimesToSleep;//TIMES_TO_SLEEP;
	//Enter Glide moving, So need to Copy Bmp1 to nuroBmp
//	g_fileSysApi.pGdata->transfData.nBmpLTx = 0;
//	g_fileSysApi.pGdata->transfData.nBmpLTy = 0;
	ptCenter.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	ptCenter.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	g_mathtoolApi.MT_SaveOrShowBmp(SHOWFROMMEM_BMP1);
	m_ptScreen.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	m_ptScreen.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	g_mathtoolApi.MT_BmpToMap(m_ptScreen.x, m_ptScreen.y, &m_ptMap.x, &m_ptMap.y);
	while( bMoving && !m_pNaviThd->m_bQuit )
	{
		if( m_nNewMoveDirt == MOVE_STOP )
		{
			bMoving = nuFALSE;
			continue;
		}
		if( m_nNewMoveDirt != m_nOldMoveDirt || bDrawNextPage )//Need redraw
		{
			ACTIONSTATE actionState;
			NUROPOINT ptNextC = m_ptScreen;
			if( m_nNewMoveDirt != m_nOldMoveDirt )
			{
				m_nStep = m_nStartDis*3/2;//nStep ;
			}
			else
			{
				if( m_nStep < m_nStartDis * 4 && m_nStep < m_nMaxStep )
				{
					m_nStep += m_nStartDis/2;//nStep ;
				}
			}
			m_nOldMoveDirt = m_nNewMoveDirt;
			if( !ColNextPageCenter(m_nOldMoveDirt, &ptNextC) )
			{
				bMoving = nuFALSE;
				continue;
			}
			if( bHasDrawCenter )
			{
				CompositeBmp(nLastDirt, nLencopy);
			}//Change Direct
			g_mathtoolApi.MT_SaveOrShowBmp(SAVE_SHOW_MAP_TO_BMP1);//Keep now map in screen to MemBmp1;
			/*
			g_gdiApi.GdiDrawBMP( 0, 
				0, 
				g_fileSysApi.pGdata->transfData.nBmpWidth,
				g_fileSysApi.pGdata->transfData.nBmpHeight,
				&g_fileSysApi.pGdata->transfData.nuBmpMem1, 
				g_fileSysApi.pGdata->transfData.nBmpLTx,
				g_fileSysApi.pGdata->transfData.nBmpLTy );
			*/
			//
			m_pNaviThd->ResetActionState(&actionState);
			actionState.bDrawBmp1	= 1;
			actionState.bDrawRoadName	= 0;
			actionState.bGlideMoveDraw	= 1;
			actionState.nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
			actionState.bResetMapCenter	= 1;
			actionState.bNeedReloadMap	= 1;
			actionState.nDrawMode		= DRAWMODE_NUROGLIDEMOVE;
			if( g_fileSysApi.pGdata->uiSetData.nBsdDrawMap )
			{
				actionState.bLoad2kBlock	= 0;
			}
			g_mathtoolApi.MT_BmpToMap(ptNextC.x, ptNextC.y, &actionState.ptNewMapCenter.x, &actionState.ptNewMapCenter.y);
			/*
			if( !g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter) )
			{
				bMoving = nuFALSE;
				continue;
			}
			*/
			/**/
			nuBOOL bFix = nuTRUE;
			if( m_nOldMoveDirt == MOVE_LEFT )
			{
				ptS2.x = g_fileSysApi.pGdata->transfData.nBmpWidth * 3 / 2;
				ptS2.y = g_fileSysApi.pGdata->transfData.nBmpHeight / 2;
			}
			else if( m_nOldMoveDirt == MOVE_RIGHT )
			{
				ptS2.x = -g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
				ptS2.y = g_fileSysApi.pGdata->transfData.nBmpHeight / 2;
			}
			else if( m_nOldMoveDirt == MOVE_UP )
			{
				ptS2.x = g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
				ptS2.y = g_fileSysApi.pGdata->transfData.nBmpHeight * 3 / 2;
			}
			else if( m_nOldMoveDirt == MOVE_DOWN )
			{
				ptS2.x = g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
				ptS2.y = -g_fileSysApi.pGdata->transfData.nBmpHeight / 2;
			}
			else
			{
				bFix = nuFALSE;
			}
			ptS1 = m_ptMap;
//			g_mathtoolApi.MT_MapToBmp(ptS1.x, ptS1.y, &ptS1.x, &ptS1.y);
			if( !m_pNaviThd->m_gDataMgr.SetUp(&actionState) )
			{
				bMoving = nuFALSE;
				continue;
			}
			m_pNaviThd->CheckLocalRoute(&actionState);
			if( bFix )
			{
				g_mathtoolApi.MT_MapToBmp(ptS1.x, ptS1.y, &ptS1.x, &ptS1.y);
				ptConj.x = g_fileSysApi.pGdata->transfData.nBmpCenterX - (ptS2.x - ptS1.x) * 3 / 2;
				ptConj.y = g_fileSysApi.pGdata->transfData.nBmpCenterY - (ptS2.y - ptS1.y) * 3 / 2;
				g_mathtoolApi.MT_BmpToMap(ptConj.x, ptConj.y, &actionState.ptNewMapCenter.x, &actionState.ptNewMapCenter.y);
				if( !m_pNaviThd->m_gDataMgr.SetUp(&actionState) )
				{
					bMoving = nuFALSE;
					continue;
				}
			}
			//
			m_pNaviThd->m_gDataMgr.CalMapShowInfo(pActionState);
			if( !g_drawmapApi.DM_LoadMap(&actionState)		||
				!g_drawmapApi.DM_DrawMapBmp1(&actionState)	)
			{
				bMoving = nuFALSE;
				continue;
			}
			if( g_fileSysApi.pGdata->uiSetData.bNavigation )
			{
				g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
			}
			if(	!g_drawmapApi.DM_DrawMapBmp2(&actionState) )
			{
				bMoving = nuFALSE;
				continue;
			}
			g_mathtoolApi.MT_SaveOrShowBmp(SAVE_SHOW_MAP_TO_BMP2);
//			m_ptScreen	= ptNextC;
			ColNowPageCenter(m_nOldMoveDirt, &m_ptScreen);
			m_nLenMoved = 0;
			bDrawNextPage = nuFALSE;
		}
		m_nLenMoved += m_nStep;
		if( m_nLenMoved > m_nMaxLen )
		{
			bDrawNextPage = nuTRUE;
			continue;
		}
//		nuDWORD lllk = nuGetTickCount();
		switch( m_nOldMoveDirt )//Copy map to screen;
		{
			case MOVE_LEFT:
				g_gdiApi.GdiDrawBMP( 0,
									 0,
									 m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpHeight,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem2,
									 g_fileSysApi.pGdata->transfData.nBmpWidth - m_nLenMoved,
									 0 );
				g_gdiApi.GdiDrawBMP( m_nLenMoved,
									 0,
									 g_fileSysApi.pGdata->transfData.nBmpWidth - m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpHeight,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem1,
									 0,
									 0 ); 
				m_ptScreen.x -= m_nStep;
				break;
			case MOVE_UP:
				g_gdiApi.GdiDrawBMP( 0,
									 0,
									 g_fileSysApi.pGdata->transfData.nBmpWidth,
									 m_nLenMoved,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem2,
									 0,
									 g_fileSysApi.pGdata->transfData.nBmpHeight - m_nLenMoved );
				g_gdiApi.GdiDrawBMP( 0,
									 m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpWidth,
									 g_fileSysApi.pGdata->transfData.nBmpHeight - m_nLenMoved,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem1,
									 0,
									 0 );
				m_ptScreen.y -= m_nStep;
				break;
			case MOVE_RIGHT:
				g_gdiApi.GdiDrawBMP( 0,
									 0,
									 g_fileSysApi.pGdata->transfData.nBmpWidth - m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpHeight,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem1,
									 m_nLenMoved,
									 0 );
				g_gdiApi.GdiDrawBMP( g_fileSysApi.pGdata->transfData.nBmpWidth - m_nLenMoved,
									 0,
									 m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpHeight,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem2,
									 0,
									 0 );
				m_ptScreen.x += m_nStep;
				break;
			case MOVE_DOWN:
				g_gdiApi.GdiDrawBMP( 0, 
									 0, 
									 g_fileSysApi.pGdata->transfData.nBmpWidth, 
									 g_fileSysApi.pGdata->transfData.nBmpHeight - m_nLenMoved,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem1,
									 0,
									 m_nLenMoved );
				g_gdiApi.GdiDrawBMP( 0,
									 g_fileSysApi.pGdata->transfData.nBmpHeight - m_nLenMoved,
									 g_fileSysApi.pGdata->transfData.nBmpWidth,
									 m_nLenMoved,
									 &g_fileSysApi.pGdata->transfData.nuBmpMem2,
									 0,
									 0 );
				m_ptScreen.y += m_nStep;
				break;
			default:
				bMoving = nuFALSE;
				continue;
		}
		bHasDrawCenter = nuTRUE;
		nLencopy = m_nLenMoved;
		nLastDirt = m_nOldMoveDirt;
//		g_drawIfmApi.IFM_DrawCenterTarget(ptCenter, g_fileSysApi.pUserCfg->bNightDay);
		g_mathtoolApi.MT_SaveOrShowBmp(SHOW_MAP_SCREEN);
//		_nuDGI(nuGetTickCount() - lllk);//test
		g_mathtoolApi.MT_BmpToMap(m_ptScreen.x, m_ptScreen.y, &ptTmp.x, &ptTmp.y);
		if( !g_fileSysApi.FS_PtInBoundary(ptTmp) )
		{
			bMoving = nuFALSE;
			continue;
		}
		m_ptMap = ptTmp;
		if( --nSleep == 0 )
		{
			nSleep = m_nTimesToSleep;
			nuSleep(m_nSleepTime);
		}
	}
	m_nNewMoveDirt	= MOVE_STOP;
	m_nOldMoveDirt	= MOVE_STOP;
	pActionState->bDrawBmp1		= 1;
	pActionState->bDrawInfo		= 1;
	pActionState->bNeedReloadMap	= 1;
	pActionState->bResetMapCenter	= 1;
	pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
	pActionState->ptNewMapCenter	= m_ptMap;
	pActionState->nDrawMode			= DRAWMODE_NUROGLIDEMOVE;
	m_pNaviThd->m_gDataMgr.SetUp(pActionState);//Useful in JumpMove
	return nuTRUE;
}

nuBOOL CGlideMove::ColNextPageCenter(nuBYTE nDirect, PNUROPOINT pPoint)
{
	switch( nDirect )
	{
		case MOVE_LEFT:
			pPoint->x -= g_fileSysApi.pGdata->transfData.nBmpWidth;
			m_nMaxLen = g_fileSysApi.pGdata->transfData.nBmpWidth;
			m_nMaxStep = g_fileSysApi.pGdata->transfData.nBmpWidth/4;
			break;
		case MOVE_UP:
			pPoint->y -= g_fileSysApi.pGdata->transfData.nBmpHeight;
			m_nMaxLen = g_fileSysApi.pGdata->transfData.nBmpHeight;
			m_nMaxStep = g_fileSysApi.pGdata->transfData.nBmpHeight/4;
			break;
		case MOVE_RIGHT:
			pPoint->x += g_fileSysApi.pGdata->transfData.nBmpWidth;
			m_nMaxLen = g_fileSysApi.pGdata->transfData.nBmpWidth;
			m_nMaxStep = g_fileSysApi.pGdata->transfData.nBmpWidth/4;
			break;
		case MOVE_DOWN:
			pPoint->y += g_fileSysApi.pGdata->transfData.nBmpHeight;
			m_nMaxLen = g_fileSysApi.pGdata->transfData.nBmpHeight;
			m_nMaxStep = g_fileSysApi.pGdata->transfData.nBmpHeight/4;
			break;
		default:
			return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CGlideMove::ColNowPageCenter(nuBYTE nDirect, PNUROPOINT pPoint)
{
	switch( nDirect )
	{
		case MOVE_LEFT:
			pPoint->x = g_fileSysApi.pGdata->transfData.nBmpWidth*3/2;
			pPoint->y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
			break;
		case MOVE_UP:
			pPoint->x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			pPoint->y = g_fileSysApi.pGdata->transfData.nBmpHeight*3/2;
			break;
		case MOVE_RIGHT:
			pPoint->x = -g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			pPoint->y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
			break;
		case MOVE_DOWN:
			pPoint->x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			pPoint->y = -g_fileSysApi.pGdata->transfData.nBmpHeight/2;
			break;
		default:
			return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CGlideMove::IsMoving()
{
	if( m_nNewMoveDirt == MOVE_STOP )
	{
		return nuFALSE;
	}
	else
	{
		return nuTRUE;
	}
}
nuBOOL CGlideMove::CompositeBmp(nuBYTE nDirt, nuLONG nLen)
{
	switch( nDirt )//Copy map to screen;
	{
	case MOVE_LEFT:
		g_gdiApi.GdiDrawBMP( 0,
			0,
			nLen,
			g_fileSysApi.pGdata->transfData.nBmpHeight,
			&g_fileSysApi.pGdata->transfData.nuBmpMem2,
			g_fileSysApi.pGdata->transfData.nBmpWidth - nLen,
			0 );
		g_gdiApi.GdiDrawBMP( nLen,
			0,
			g_fileSysApi.pGdata->transfData.nBmpWidth - nLen,
			g_fileSysApi.pGdata->transfData.nBmpHeight,
			&g_fileSysApi.pGdata->transfData.nuBmpMem1,
			0,
			0 ); 
		m_ptScreen.x -= m_nStep;
		break;
	case MOVE_UP:
		g_gdiApi.GdiDrawBMP( 0,
			0,
			g_fileSysApi.pGdata->transfData.nBmpWidth,
			nLen,
			&g_fileSysApi.pGdata->transfData.nuBmpMem2,
			0,
			g_fileSysApi.pGdata->transfData.nBmpHeight - nLen );
		g_gdiApi.GdiDrawBMP( 0,
			nLen,
			g_fileSysApi.pGdata->transfData.nBmpWidth,
			g_fileSysApi.pGdata->transfData.nBmpHeight - nLen,
			&g_fileSysApi.pGdata->transfData.nuBmpMem1,
			0,
			0 );
		m_ptScreen.y -= m_nStep;
		break;
	case MOVE_RIGHT:
		g_gdiApi.GdiDrawBMP( 0,
			0,
			g_fileSysApi.pGdata->transfData.nBmpWidth - nLen,
			g_fileSysApi.pGdata->transfData.nBmpHeight,
			&g_fileSysApi.pGdata->transfData.nuBmpMem1,
			nLen,
			0 );
		g_gdiApi.GdiDrawBMP( g_fileSysApi.pGdata->transfData.nBmpWidth - nLen,
			0,
			nLen,
			g_fileSysApi.pGdata->transfData.nBmpHeight,
			&g_fileSysApi.pGdata->transfData.nuBmpMem2,
			0,
			0 );
		m_ptScreen.x += m_nStep;
		break;
	case MOVE_DOWN:
		g_gdiApi.GdiDrawBMP( 0, 
			0, 
			g_fileSysApi.pGdata->transfData.nBmpWidth, 
			g_fileSysApi.pGdata->transfData.nBmpHeight - nLen,
			&g_fileSysApi.pGdata->transfData.nuBmpMem1,
			0,
			nLen );
		g_gdiApi.GdiDrawBMP( 0,
			g_fileSysApi.pGdata->transfData.nBmpHeight - nLen,
			g_fileSysApi.pGdata->transfData.nBmpWidth,
			nLen,
			&g_fileSysApi.pGdata->transfData.nuBmpMem2,
			0,
			0 );
		m_ptScreen.y += m_nStep;
		break;
	default:
		return nuFALSE;
	}
	return nuTRUE;
}

