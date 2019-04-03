// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: JumpMove.cpp,v 1.6 2010/05/19 03:20:52 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/05/19 03:20:52 $
// $Locker:  $
// $Revision: 1.6 $
// $Source: /home/nuCode/libNaviCtrl/JumpMove.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "JumpMove.h"
#include "NuroModuleApiStruct.h"
#include "NaviThread.h"
//#include "LogFileCtrlZ.h"
#include "GdiApiTrans.h"

extern class CGdiApiTrans*          g_pGdiApiTrans;

extern MATHTOOLAPI		g_mathtoolApi;
extern FILESYSAPI		g_fileSysApi;
extern INNAVIAPI		g_innaviApi;
extern DRAWMAPAPI		g_drawmapApi;
extern GDIAPI						g_gdiApi;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//ÓÃÓÚÌøÔŸÒÆ¶¯ÖÐ£¬Öð²œÒÆ¶¯µœÄÇžöµã
#define	MAX_MOVE_NUMBERS					20
#define MIN_MOVE_STEP						5

CJumpMove::CJumpMove()
{
	m_nNewDirection	= MOVE_STOP;
	m_nOldDirection	= MOVE_STOP;
	m_nHMoveStep	= MOVE_STEP_MIN;

	m_nVMoveStep	= MOVE_STEP_MIN;

	m_pfSaveDrawData = NULL;

	//nuMemset(&m_bmpMove, 0, sizeof(nuroBITMAP));
	nuMemset(&m_bmpMove, 0, sizeof(NURO_BMP));
	if( !nuReadConfigValue("JUMPMOVETYPE", &m_nJumpMoveType) )
	{
		m_nJumpMoveType = 0;
	}
	m_bJumpMoving = nuFALSE;
}

CJumpMove::~CJumpMove()
{
	Free();
}

nuBOOL CJumpMove::Initialize(class CNaviThread*	pNaviThd)
{
	m_pNaviThd = pNaviThd;
	nuLONG nTmp = 0;
	if( !nuReadConfigValue("JMPMOVESLEEPTIME", &m_nJmpMoveSleepTime) )
	{
		m_nJmpMoveSleepTime = 0;
	}
	if( !nuReadConfigValue("SINGLEJUMPMOVEWAIT", &nTmp) )
	{
		nTmp = 50;
	}
	m_nSingleMoveSleepTime = nuLOWORD(nTmp);
	if( !nuReadConfigValue("SINGLEJUMPMOVESTEP", &nTmp) )
	{
		nTmp = MIN_MOVE_STEP;
	}
	m_nSingleMoveStep = nuLOWORD(nTmp);
	if( !nuReadConfigValue("GLIDEMOVESLPI", &nTmp) )
	{
		nTmp = 5;
	}
	m_nTimesToSleep = nuLOWORD(nTmp);
	if( !nuReadConfigValue("GLIDEMOVESLPT", &nTmp) )
	{
		nTmp = 5;
	}
	m_nGlidMoveWTimes = nuLOWORD(nTmp);
	if( !nuReadConfigValue("GLIDEMOVEDIS", &nTmp) )
	{
		nTmp = 5;
	}
	m_nStartDis = nuLOWORD(nTmp);
	if( !nuReadConfigValue("MOVESTEPONE", &nTmp) )
	{
		nTmp = 10;
	}
	m_nJumpMoveStep = nuLOWORD(nTmp);
	CreateMoveBmp(g_fileSysApi.pGdata->uiSetData.nScreenWidth, g_fileSysApi.pGdata->uiSetData.nScreenHeight);
	return nuTRUE;
}

nuVOID CJumpMove::Free()
{
	DeleteMoveBmp();
}

nuBOOL CJumpMove::MoveStart(nuBYTE nDirection)
{
	if( m_nNewDirection != nDirection )
	{
//		m_nMoveSpeed	= MIN_MOVE_SPEED;
		m_nLastTicks	= 0;
		m_nDisToChangeSpeed = g_fileSysApi.pGdata->transfData.nBmpWidth / 4;
		m_nVStep = MOVE_STEP_MIN;
		
		m_nHStep = MOVE_STEP_MIN * g_fileSysApi.pGdata->transfData.nBmpWidth / g_fileSysApi.pGdata->transfData.nBmpHeight;
		
		m_nNewDirection = nDirection;
	}
	return nuTRUE;
}
nuBOOL CJumpMove::SetMoveStep(nuINT nStep)
{
	if( nStep > 0 && nStep < 5 )
	{
//		m_nMoveSpeed = (nStep + 1) * m_nJumpMoveStep / 2;
		m_nMoveSpeed = nStep * m_nJumpMoveStep;
	}
	return nuTRUE;
}

nuBOOL CJumpMove::MoveStop(nuBYTE nDirection)
{
	if( nDirection == m_nNewDirection || nDirection == MOVE_STOP )
	{
		m_nNewDirection = MOVE_STOP;
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

#define TIMES_TO_SLEEP					5
nuBOOL CJumpMove::JumpMoveProc(PACTIONSTATE pActionState) //Á¬ÐøÒÆÍŒ
{
	//return nuTRUE;
	if( m_nNewDirection == MOVE_STOP )
	{
		m_nOldDirection = MOVE_STOP;
		return nuFALSE;
	}/*
	if( !g_fileSysApi.pGdata->uiSetData.b3DMode && m_nJumpMoveType )
	{
		return JumpMove2DProc(pActionState);
	}*/
//	CLogFileCtrlZ logFile;
	nuBOOL bIsMoving = nuTRUE;
	nuBOOL bHaveDraw = nuFALSE;
	nuWORD nSleep = TIMES_TO_SLEEP;
	ACTIONSTATE actionState;
	nuroPOINT ptSC, ptCenter;
	ptCenter.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	ptCenter.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp2Dto3D(ptCenter.x, ptCenter.y, MAP_DEFAULT);
	}
	g_fileSysApi.pUserCfg->bMoveMap = nuTRUE;
	while( bIsMoving && !m_pNaviThd->m_bQuit )
	{
		nuSleep(10); // Removed by Dengxu
		m_pNaviThd->ResetActionState(&actionState);
		if( !ColJumpMove(&actionState) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		if( !g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		if( !m_pNaviThd->m_gDataMgr.SetUp(&actionState) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
//		m_pNaviThd->CheckLocalRoute();
		m_pNaviThd->m_gDataMgr.CalMapShowInfo(pActionState);
		/* time test
		logFile.OpenLogFile(nuTEXT("jump.log"));
		logFile.SaveTicks();*/
		if( !g_drawmapApi.DM_LoadMap(&actionState)	)
		{
			bIsMoving = nuFALSE;
			continue;
		}
		/* time test
		logFile.OutTicksPast();
		logFile.SaveTicks();*/
		//nuSleep(TIMER_TIME_UNIT); // Removed by Dengxu
		if(	!g_drawmapApi.DM_DrawMapBmp1(&actionState) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		/* time test
		logFile.OutTicksPast();
		logFile.CloseLogFile();*/
		
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
		}
		
		// Added by Dengxu for not showing POIs' name.
		actionState.bDrawPOIName = 0;
 		
		if(	!g_drawmapApi.DM_DrawMapBmp2(&actionState) )
 		{
 			bIsMoving = nuFALSE;
 			continue;
 		}

		/*
		if( g_fileSysApi.pGdata->uiSetData.b3DMode )
		{
			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_3D_BMP);
		}
		*/
//		g_drawIfmApi.IFM_DrawCenterTarget(ptCenter, g_fileSysApi.pUserCfg->bNightDay);

		//Draw CenterTarget @2011.02.16
		if( m_pfSaveDrawData )
		{
			UI_DRAW_DATA    DrawData;
			DrawData.uddpCanvas = g_pGdiApiTrans->nuGdiGetCanvas();
			//
			g_mathtoolApi.MT_MapToBmp( g_fileSysApi.pGdata->carInfo.ptCarIcon.x, 
				g_fileSysApi.pGdata->carInfo.ptCarIcon.y,
				&g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x,
				&g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y );
			g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x -= g_fileSysApi.pGdata->transfData.nBmpLTx;
			g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y -= g_fileSysApi.pGdata->transfData.nBmpLTy;
			if( g_fileSysApi.pGdata->uiSetData.b3DMode )
			{
				g_mathtoolApi.MT_Bmp2Dto3D( g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x, 
					g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y, 
					MAP_DEFAULT );
			}
			//
			DrawData.uddCarInScreen.x = g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x;
			DrawData.uddCarInScreen.y = g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y;
			m_pfSaveDrawData(DrawData);
		}

		g_mathtoolApi.MT_SaveOrShowBmp(SHOW_MAP_SCREEN);

		bHaveDraw = nuTRUE;
		ptSC = actionState.ptNewMapCenter;
		nuSleep(5); // Removed by Dengxu
#if defined(_WINDOWS) && !defined(_WIN32_WCE)
		//nuSleep(TIMER_TIME_UNIT); // Removed by Dengxu
#else

#ifdef NURO_OS_UCOS
			if( --nSleep == 0 )
			{
				nSleep = TIMES_TO_SLEEP;
				nuSleep(10);
			}
#else
      if( 0 != m_nJmpMoveSleepTime )
	  {
		 nuSleep(m_nJmpMoveSleepTime);
	  }
#endif

#endif
	}



	if( bHaveDraw )
	{
		g_mathtoolApi.MT_SaveOrShowBmp(SAVE_SHOW_MAP_TO_BMP1);//Useful in GlideMove
		pActionState->bDrawBmp1			= 1;
		pActionState->bDrawInfo			= 1;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
		pActionState->ptNewMapCenter	= ptSC;
		pActionState->nDrawMode			= DRAWMODE_NUROJUMPMOVE;
	}
	g_fileSysApi.pUserCfg->bMoveMap = nuFALSE;
	m_nNewDirection = m_nOldDirection = MOVE_STOP;

	return bHaveDraw;
}

nuBOOL CJumpMove::JumpToPoint(PACTIONSTATE pActionState, nuLONG x, nuLONG y) //µã»÷ÒÆÍŒµÄ¶¯Ì¬¹ý³Ì
{
	if( m_pNaviThd == NULL || m_nOldDirection != MOVE_STOP || m_nNewDirection != MOVE_STOP )
	{
		return nuFALSE;
	}
	nuINT nTimes = ColMoveTimes(x, y);
	if( nTimes <= 0 )
	{
		return nuFALSE;
	}
	m_nOldDirection = m_nNewDirection = MOVE_TOBMPPOINT;
	
	nuLONG nOldx = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	nuLONG nOldy = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	g_mathtoolApi.MT_BmpToMap(nOldx, nOldy, &nOldx, &nOldy);
	g_mathtoolApi.MT_BmpToMap(x, y, &x, &y);
	nuLONG dx = x - nOldx;
	nuLONG dy = y - nOldy;
	nuINT nNowIdx = 0;
	nuroPOINT ptLast = pActionState->ptNewMapCenter;
	ACTIONSTATE actionState;
	m_pNaviThd->ResetActionState(&actionState);
	actionState.bDrawBmp1		= 1;
	actionState.bDrawInfo		= 1;
	actionState.bNeedReloadMap	= 1;
	actionState.bResetMapCenter	= 1;
	actionState.bDrawRoadName	= 0;
	actionState.nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
	actionState.nDrawMode		= DRAWMODE_NUROJUMPMOVE;
	actionState.nBmpSizeMode	= BMPSIZE_EXTEND_NO;
	nuLONG nNidx = 0;
	while( !m_pNaviThd->m_bQuit /*&& nNowIdx < nTimes*/ )
	{

		/*
		if( !g_fileSysApi.pGdata->uiSetData.b3DMode )
		{
			if( nNidx >= (nTimes<<1) )
			{
				break;
			}
			nNidx = 1 + nNowIdx * (nNowIdx + 2) / 4;
			if( nNidx >= (nTimes<<1) )
			{
				actionState.ptNewMapCenter.x = nOldx + dx;
				actionState.ptNewMapCenter.y = nOldy + dy;
			}
			else
			{
				actionState.ptNewMapCenter.x = nOldx + nNidx*dx/(nTimes<<1);
				actionState.ptNewMapCenter.y = nOldy + nNidx*dy/(nTimes<<1);
			}
		}
		else*/
		{
			if( nNowIdx >= nTimes )
			{
				break;
			}
			actionState.ptNewMapCenter.x = nOldx + (nNowIdx + 1)*dx/nTimes;
			actionState.ptNewMapCenter.y = nOldy + (nNowIdx + 1)*dy/nTimes;
		}
		/*
		else
		{
			actionState.ptNewMapCenter = pActionState->ptNewMapCenter;
		}
		*/
		if( !g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter)||
			!m_pNaviThd->m_gDataMgr.SetUp(&actionState))// ||
			//!g_drawmapApi.DM_DrawZoomFake(NULL) )
		{
			pActionState->ptNewMapCenter = ptLast;
//			MessageBoxW(NULL, L"Jump Out", L"", 0);
			break;
		}
		if(	!g_drawmapApi.DM_DrawMapBmp1(&actionState) )
		{
			return false;
		}
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
		}
		actionState.bDrawPOIName = 0;
	
		if(	!g_drawmapApi.DM_DrawMapBmp2(&actionState) )
		{
			return false;
		}
		NUROPOINT ptCar;
		g_drawmapApi.DM_DrawMapCar(ptCar);
		g_mathtoolApi.MT_SaveOrShowBmp(SHOW_MAP_SCREEN);
		nNowIdx ++;
		ptLast = actionState.ptNewMapCenter;
		if( !m_pNaviThd->m_bQuit && m_nSingleMoveSleepTime )
		{
			nuSleep(m_nSingleMoveSleepTime);
		}
	}
	m_nOldDirection = m_nNewDirection = MOVE_STOP;
	return nuTRUE;
}

nuBOOL CJumpMove::ColJumpMove(PACTIONSTATE pActionState)
{
	if( m_nOldDirection == MOVE_STOP && m_nNewDirection == MOVE_STOP )
	{
		return nuFALSE;
	}
	if( m_nOldDirection != m_nNewDirection )
	{
//		m_nMoveSpeed	= MIN_MOVE_SPEED;
		m_nLastTicks	= 0;
		m_nDisPast		= 0;
		m_nOldDirection = m_nNewDirection;
	}//ÒÆÍŒ·œÏòžÄ±ä£¬ÐèÒªÖØÐÂ¿ªÊŒÉèÖÃ²ÎÊý»æÖÆ;

	nuDWORD nTicksDif = 0;
	nuDWORD nTicks = nuGetTickCount();
	/*
	if( m_nLastTicks != 0 && (nTicks - m_nLastTicks < SLICE_MOVE_TIME) )
	{
		nuSleep(50);	
		nTicks = nuGetTickCount();
	}//²»ÊÇµÚÒ»ŽÎÒÆÍŒ£¬Èç¹ûÊ±ŒäŒäžôÌ«¶ÌºöÂÔÕâŽÎµÈÏÂÒ»ŽÎ
	*/
	/*
	if( m_nLastTicks != 0 )
	{
		nTicksDif = nTicks - m_nLastTicks;
	}
	else
	*/
	{
		nTicksDif = SLICE_MOVE_TIME;
	}
	m_nLastTicks = nTicks;

	if( m_nDisPast > m_nDisToChangeSpeed )
	{
		m_nDisPast = 0;
		/*
		if( m_nMoveSpeed < MAX_MOVE_SPEED )
		{
			m_nMoveSpeed += MIN_MOVE_SPEED;
		}
		*/
	}
	NUROPOINT ptScreen = {0};
	nuWORD nHLen = 0, nVLen = 0;
	switch( m_nNewDirection )
	{
		case MOVE_LEFT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = 0;
			m_nDisPast += nHLen;
			ptScreen.x = -nHLen;
			ptScreen.y = 0;
			break;
		case MOVE_UP:
			nHLen = 0;
			nVLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME );
			m_nDisPast += nVLen;
			ptScreen.x = 0;
			ptScreen.y = -nVLen;
			break;
		case MOVE_RIGHT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = 0;
			m_nDisPast += nHLen;
			ptScreen.x = nHLen;
			ptScreen.y = 0;
			break;
		case MOVE_DOWN:
			nHLen = 0;
			nVLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			m_nDisPast += nVLen;
			ptScreen.x = 0;
			ptScreen.y = nVLen;
			break;
		case MOVE_UPLEFT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = m_nVStep * nHLen / m_nHStep;
			m_nDisPast += nHLen;
			ptScreen.x = -nHLen;
			ptScreen.y = -nVLen;
			break;
		case MOVE_UPRIGHT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = m_nVStep * nHLen / m_nHStep;
			m_nDisPast += nHLen;
			ptScreen.x = nHLen;
			ptScreen.y = -nVLen;
			break;
		case MOVE_DOWNRIGHT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = m_nVStep * nHLen / m_nHStep;
			m_nDisPast += nHLen;
			ptScreen.x = nHLen;
			ptScreen.y = nVLen;
			break;
		case MOVE_DOWNLEFT:
			nHLen = (nuWORD)(nTicksDif * m_nMoveSpeed / SLICE_MOVE_TIME);
			nVLen = m_nVStep * nHLen / m_nHStep;
			m_nDisPast += nHLen;
			ptScreen.x = -nHLen;
			ptScreen.y = nVLen;
			break;
		default:
			m_nNewDirection = m_nOldDirection = MOVE_STOP;
			return nuFALSE;
	}
	if( !ColMapPoint(ptScreen) )
	{
		m_nNewDirection = m_nOldDirection = MOVE_STOP;
		return nuFALSE;
	}
	pActionState->bDrawBmp1			= 1;
	pActionState->bDrawInfo			= 1;
	pActionState->bNeedReloadMap	= 1;
	pActionState->bResetMapCenter	= 1;
	pActionState->bDrawRoadName		= 0;
	pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
	pActionState->ptNewMapCenter	= ptScreen;
	pActionState->nDrawMode			= DRAWMODE_NUROJUMPMOVE;
	if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale >= 200 )
	{
		pActionState->bDrawPOIName	= 0;
	}
	if( g_fileSysApi.pGdata->uiSetData.nBsdDrawMap )
	{
		pActionState->bLoad2kBlock	= 0;
	}
	return nuTRUE;
}

nuBOOL CJumpMove::ColMapPoint(NUROPOINT &point)
{
	point.x += g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	point.y += g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	return g_mathtoolApi.MT_BmpToMap(point.x, point.y, &point.x, &point.y);
}

nuINT CJumpMove::ColMoveTimes(nuLONG x, nuLONG y)
{
	x = x - (g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2);
	y = y - (g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2);
	x = NURO_ABS(x);
	y = NURO_ABS(y);
	x = NURO_MAX(x, y);
	x = (x + m_nSingleMoveStep/2) / m_nSingleMoveStep;
	if( x <= 1 )
	{
		return 0;
	}
	else if( x < MAX_MOVE_NUMBERS )
	{
		return x;
	}	
	else
	{
		return MAX_MOVE_NUMBERS;
	}
}

nuBOOL CJumpMove::ColMapPointFromScreenCenter(nuLONG &x, nuLONG &y)
{
	x += g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	y += g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	return g_mathtoolApi.MT_BmpToMap(x, y, &x, &y);
}

nuBOOL CJumpMove::IsMoving()
{
	if( m_nNewDirection == MOVE_STOP )
	{
		return nuFALSE;
	}
	else
	{
		return nuTRUE;
	}
}

nuBOOL CJumpMove::CreateMoveBmp(nuINT nScreenW, nuINT nScreenH)
{
	if( !m_nJumpMoveType )
	{
		return nuFALSE;
	}
	m_bmpMove.bmpReserve = NURO_BMP_DIB_SECTION;
	m_bmpMove.bmpWidth	= nScreenW * 3/2;
	m_bmpMove.bmpHeight = nScreenH * 3/2;
	if( !g_gdiApi.GdiCreateNuroBMP(&m_bmpMove) )
	{
		m_nJumpMoveType = 0;
	}
	m_bmpMove.bmpType   = NURO_BMP_TYPE_COPY;
	return nuTRUE;
}

nuVOID CJumpMove::DeleteMoveBmp()
{
	if( m_nJumpMoveType )
	{
		g_gdiApi.GdiDelNuroBMP(&m_bmpMove);
	}
}

nuBOOL CJumpMove::JumpMove2DProc(PACTIONSTATE pActionState)
{
	if( m_nNewDirection == MOVE_STOP )
	{
		m_nOldDirection = MOVE_STOP;
		return nuFALSE;
	}
	nuBOOL bIsMoving = nuTRUE;
	nuBOOL bHaveDraw = nuFALSE;
	nuBOOL bNeedDraw = nuTRUE;
	nuBOOL bHasDrawCenter = nuFALSE;
	nuINT  nLeftCopy, nTopCopy;
	nuWORD nSleep = m_nTimesToSleep;//TIMES_TO_SLEEP;
	nuDWORD nSameTimes;
	ACTIONSTATE actionState;
	nuroPOINT ptSC;
	nuroPOINT ptNowScreenCenter, ptNextPageC, ptCenter;
	//PNUROBITMAP pOldBmp = NULL;
	PNURO_BMP pOldBmp = NULL;
	m_pNaviThd->ResetActionState(&actionState);
	ptCenter.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	ptCenter.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	ptNowScreenCenter.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	ptNowScreenCenter.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	g_mathtoolApi.MT_BmpToMap(ptNowScreenCenter.x, ptNowScreenCenter.y, &ptSC.x, &ptSC.y);
	//
	ptNextPageC = ptNowScreenCenter;
	if( !ColNextBmpCenter(m_nNewDirection, ptNextPageC) )
	{
		return nuFALSE;
	}
	g_mathtoolApi.MT_BmpToMap(ptNextPageC.x, ptNextPageC.y, &actionState.ptNewMapCenter.x, &actionState.ptNewMapCenter.y);
	m_pNaviThd->m_gDataMgr.MapBmpCenterSet( CAR_CENTER_MODE_XY_PARAMETER, 
				g_fileSysApi.pGdata->transfData.nBmpWidth * 3/4, 
				g_fileSysApi.pGdata->transfData.nBmpHeight * 3/4 );
	bHaveDraw = nuTRUE;
	while( bIsMoving && !m_pNaviThd->m_bQuit )
	{
		nuSleep(TIMER_TIME_UNIT);
		m_bJumpMoving = nuTRUE;
		if( m_nNewDirection != m_nOldDirection )
		{
			m_nOldDirection = m_nNewDirection;
			if( !bNeedDraw )
			{
				bNeedDraw = nuTRUE;
				ptNextPageC = ptNowScreenCenter;
				if( !ColNextBmpCenter(m_nNewDirection, ptNextPageC) )
				{
					bIsMoving = nuFALSE;
					continue;
				}
				g_mathtoolApi.MT_BmpToMap(ptNextPageC.x, ptNextPageC.y, &actionState.ptNewMapCenter.x, &actionState.ptNewMapCenter.y);
			}
			m_nVMoveStep = m_nStartDis;
			m_nHMoveStep = m_nVMoveStep * g_fileSysApi.pGdata->transfData.nBmpWidth/
				g_fileSysApi.pGdata->transfData.nBmpHeight;
			nSameTimes = 0;
			//
		}
		if( m_nOldDirection == MOVE_STOP )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		nuSleep(TIMER_TIME_UNIT);
		if( bNeedDraw )
		{
			bNeedDraw = nuFALSE;
			actionState.bDrawBmp1		= 1;
			actionState.bDrawRoadName	= 0;
			actionState.bGlideMoveDraw	= 1;
			actionState.nCarCenterMode	= CAR_CENTER_MODE_REMAIN;
			actionState.bResetMapCenter	= 1;
			actionState.bNeedReloadMap	= 1;
			actionState.nDrawMode		= DRAWMODE_NUROGLIDEMOVE;
			if( !g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter) )
			{
				bIsMoving = nuFALSE;
				continue;
			}
			/*
			m_pNaviThd->m_gDataMgr.MapBmpCenterSet( CAR_CENTER_MODE_XY_PARAMETER, 
				g_fileSysApi.pGdata->transfData.nBmpWidth * 3/4, 
				g_fileSysApi.pGdata->transfData.nBmpHeight * 3/4 );
			*/
			if( !m_pNaviThd->m_gDataMgr.MapSetup(&actionState) )
			{
				bIsMoving = nuFALSE;
				continue;
			}
			m_pNaviThd->CheckLocalRoute(&actionState);
			if( !m_pNaviThd->m_gDataMgr.CalMapLoadInfo(&actionState, 
					g_fileSysApi.pGdata->transfData.nBmpWidth * 3 / 2, 
					g_fileSysApi.pGdata->transfData.nBmpHeight * 3 / 2) )
			{
				bIsMoving = nuFALSE;
				continue;
			}
			if( !g_drawmapApi.DM_LoadMap(&actionState) )
			{
				bIsMoving = nuFALSE;
				continue;
			}
			pOldBmp = g_gdiApi.GdiSelectCanvasBMP(&m_bmpMove);
			if( !g_drawmapApi.DM_DrawMapBmp1(&actionState) 	)
			{
				bIsMoving = nuFALSE;
				continue;
			}
			if( g_fileSysApi.pGdata->uiSetData.bNavigation )
			{
				g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
			}
			if(	!g_drawmapApi.DM_DrawMapBmp2(&actionState) )
			{
				bIsMoving = nuFALSE;
				continue;
			}
			g_gdiApi.GdiSelectCanvasBMP(pOldBmp);
			pOldBmp = NULL;
			m_nDisPast = 0;
			switch(m_nOldDirection)
			{
				case MOVE_UPLEFT:
					m_nHStep = g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
					m_nVStep = g_fileSysApi.pGdata->transfData.nBmpHeight / 2;			
					ptNowScreenCenter.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth;
					ptNowScreenCenter.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight;
					break;
				case MOVE_UPRIGHT:
					m_nHStep = 0;
					m_nVStep = g_fileSysApi.pGdata->transfData.nBmpHeight / 2;
					ptNowScreenCenter.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
					ptNowScreenCenter.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight;
					break;
				case MOVE_DOWNRIGHT:
					m_nHStep = 0;
					m_nVStep = 0;
					ptNowScreenCenter.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth/2;
					ptNowScreenCenter.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
					break;
				case MOVE_DOWNLEFT:
					m_nHStep = g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
					m_nVStep = 0;
					ptNowScreenCenter.x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth;
					ptNowScreenCenter.y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight/2;
					break;
				default:
					bIsMoving = nuFALSE;
					continue;
			}
			//Drawing end
		}
		nuSleep(TIMER_TIME_UNIT);
		switch(m_nOldDirection)
		{
			case MOVE_UPLEFT:
				m_nHStep -= m_nHMoveStep;
				m_nVStep -= m_nVMoveStep;
				ptNowScreenCenter.x -= m_nHMoveStep;
				ptNowScreenCenter.y -= m_nVMoveStep;
				break;
			case MOVE_UPRIGHT:
				m_nHStep += m_nHMoveStep;
				m_nVStep -= m_nVMoveStep;
				ptNowScreenCenter.x += m_nHMoveStep;
				ptNowScreenCenter.y -= m_nVMoveStep;
				break;
			case MOVE_DOWNRIGHT:
				m_nHStep += m_nHMoveStep;
				m_nVStep += m_nVMoveStep;
				ptNowScreenCenter.x += m_nHMoveStep;
				ptNowScreenCenter.y += m_nVMoveStep;
				break;
			case MOVE_DOWNLEFT:
				m_nHStep -= m_nHMoveStep;
				m_nVStep += m_nVMoveStep;
				ptNowScreenCenter.x -= m_nHMoveStep;
				ptNowScreenCenter.y += m_nVMoveStep;
				break;
			default:
				bIsMoving = nuFALSE;
				continue;
		}
		g_mathtoolApi.MT_BmpToMap(ptNowScreenCenter.x,
			ptNowScreenCenter.y, 
			&actionState.ptNewMapCenter.x, 
			&actionState.ptNewMapCenter.y);
		if( !g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		nuSleep(TIMER_TIME_UNIT);
		ptSC = actionState.ptNewMapCenter;
		ptNextPageC = ptNowScreenCenter;
		if( !ColNextBmpCenter(m_nOldDirection, ptNextPageC) )
		{
			bIsMoving = nuFALSE;
			continue;
		}
		g_mathtoolApi.MT_BmpToMap( ptNextPageC.x, 
			ptNextPageC.y, 
			&actionState.ptNewMapCenter.x,
			&actionState.ptNewMapCenter.y );
		g_gdiApi.GdiDrawBMP( 0, 
							 0,
							 g_fileSysApi.pGdata->transfData.nBmpWidth,
							 g_fileSysApi.pGdata->transfData.nBmpHeight,
							 &m_bmpMove,
							 m_nHStep,
							 m_nVStep );
		bHasDrawCenter = nuTRUE;
		nLeftCopy = m_nHStep;
		nTopCopy  = m_nVStep;
//		g_drawIfmApi.IFM_DrawCenterTarget(ptCenter, g_fileSysApi.pUserCfg->bNightDay);
		g_gdiApi.GdiFlush(	g_fileSysApi.pGdata->uiSetData.nScreenLTx,
							g_fileSysApi.pGdata->uiSetData.nScreenLTy,
							g_fileSysApi.pGdata->transfData.nBmpWidth,
							g_fileSysApi.pGdata->transfData.nBmpHeight,
							0, 
							0 );
		m_nDisPast += m_nVMoveStep;
		nuSleep(TIMER_TIME_UNIT);
		if( m_nDisPast + m_nVMoveStep > g_fileSysApi.pGdata->transfData.nBmpHeight / 2 )
		{
			bNeedDraw = nuTRUE;
			++nSameTimes;
			if( (nSameTimes % 2) == 0 )
			{
				if( m_nVMoveStep < m_nStartDis * 2 /*m_nVMoveStep + m_nStartDis <= g_fileSysApi.pGdata->transfData.nBmpHeight / 4*/ )
				{
					m_nVMoveStep += m_nStartDis / 2;
					m_nHMoveStep = m_nVMoveStep * g_fileSysApi.pGdata->transfData.nBmpWidth/
					g_fileSysApi.pGdata->transfData.nBmpHeight;
				}
			}
		}
		else
		{
			if( --nSleep == 0 )
			{
				nSleep = m_nTimesToSleep;
				nuSleep(m_nGlidMoveWTimes);
			}
		}
	}
	if( pOldBmp != NULL )
	{
		g_gdiApi.GdiSelectCanvasBMP(pOldBmp);
	}
	if( bHasDrawCenter )
	{
		g_gdiApi.GdiDrawBMP( 0, 
			0,
			g_fileSysApi.pGdata->transfData.nBmpWidth,
			g_fileSysApi.pGdata->transfData.nBmpHeight,
			&m_bmpMove,
			nLeftCopy,
			nTopCopy );
	}
	m_pNaviThd->m_gDataMgr.MapBmpCenterSet( CAR_CENTER_MODE_ONE_HALF_XY);
	actionState.bDrawBmp1		= 1;
	actionState.bDrawRoadName	= 0;
	actionState.bGlideMoveDraw	= 1;
	actionState.nCarCenterMode	= CAR_CENTER_MODE_REMAIN;
	actionState.bResetMapCenter	= 1;
	actionState.bNeedReloadMap	= 1;
	actionState.nDrawMode		= DRAWMODE_NUROGLIDEMOVE;
	actionState.ptNewMapCenter	= ptSC;
	m_pNaviThd->m_gDataMgr.MapSetup(&actionState);
	if( bHaveDraw )
	{
		g_mathtoolApi.MT_SaveOrShowBmp(SAVE_SHOW_MAP_TO_BMP1);//Useful in GlideMove
		pActionState->bDrawBmp1			= 1;

		pActionState->bDrawInfo			= 1;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
		pActionState->nDrawMode			= DRAWMODE_NUROJUMPMOVE;
		pActionState->ptNewMapCenter	= ptSC;
	}
	m_nNewDirection = m_nOldDirection = MOVE_STOP;
	return bHaveDraw;
}

nuBOOL CJumpMove::ColNextBmpCenter(nuBYTE nDirect, nuroPOINT& point)
{
	switch( nDirect )
	{
	case MOVE_UPLEFT:
		point.x -= g_fileSysApi.pGdata->transfData.nBmpWidth/4;
		point.y -= g_fileSysApi.pGdata->transfData.nBmpHeight/4;
		break;
	case MOVE_UPRIGHT:
		point.x += g_fileSysApi.pGdata->transfData.nBmpWidth/4;
		point.y -= g_fileSysApi.pGdata->transfData.nBmpHeight/4;
		break;
	case MOVE_DOWNRIGHT:
		point.x += g_fileSysApi.pGdata->transfData.nBmpWidth/4;
		point.y += g_fileSysApi.pGdata->transfData.nBmpHeight/4;
		break;
	case MOVE_DOWNLEFT:
		point.x -= g_fileSysApi.pGdata->transfData.nBmpWidth/4;
		point.y += g_fileSysApi.pGdata->transfData.nBmpHeight/4;
		break;
	default:
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CJumpMove::ColJumpMoveEx(PACTIONSTATE pActionState, NUROPOINT ptScreen)
{
	if( !ColMapPoint(ptScreen) )
	{
		return nuFALSE;
	}
	pActionState->bDrawBmp1			= 1;
	pActionState->bDrawInfo			= 1;
	pActionState->bNeedReloadMap		= 1;
	pActionState->bResetMapCenter		= 1;
	pActionState->bDrawRoadName		= 0;
	pActionState->nCarCenterMode		= CAR_CENTER_MODE_ONE_HALF_XY;
	pActionState->ptNewMapCenter		= ptScreen;
	pActionState->nDrawMode			= DRAWMODE_NUROJUMPMOVE;
}

nuBOOL CJumpMove::JumpToPointEx(PACTIONSTATE pActionState, nuLONG x, nuLONG y)
{
	nuINT Tick1 = nuGetTickCount();
	if(m_pNaviThd == NULL)
	{
		return nuFALSE;
	}
	nuroPOINT ptCenter;
	ptCenter.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	ptCenter.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	if(g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp2Dto3D(ptCenter.x, ptCenter.y, MAP_DEFAULT);
	}	
	NUROPOINT ptScreen = {0};
	ptScreen.y = y;
	ptScreen.x = x;	
	nuSleep(10); // Removed by Dengxu
	ACTIONSTATE actionState;
	m_pNaviThd->ResetActionState(&actionState);
	if(!ColJumpMoveEx(&actionState, ptScreen) )
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove ColJumpMoveEx Fail\n");
		return nuFALSE;
	}
	if(!g_fileSysApi.FS_PtInBoundary(actionState.ptNewMapCenter) )
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove Judge PtInBoundary Fail\n");
		return nuFALSE;
	}
	if(!m_pNaviThd->m_gDataMgr.SetUp(&actionState) )
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove SetUp Fail\n");
		return nuFALSE;
	}
	m_pNaviThd->m_gDataMgr.CalMapShowInfo(&actionState);

	if(!g_drawmapApi.DM_LoadMap(&actionState)	)
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove LoadMap Fail\n");
		return nuFALSE;
	}
	if(!g_drawmapApi.DM_DrawMapBmp1(&actionState) )
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove DrawMapBmp1 Fail\n");
		return nuFALSE;
	}
	
	if(g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
	}	
	if(!g_drawmapApi.DM_DrawMapBmp2(&actionState) )
	{
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove DrawMapBmp2 Fail\n");
		return nuFALSE;
	}
	g_mathtoolApi.MT_SaveOrShowBmp(SAVE_SHOW_MAP_TO_BMP1);
	NUROPOINT ptCar = {0};
	g_drawmapApi.DM_DrawMapCar(ptCar);

	pActionState->bDrawBmp1			= 1;
	pActionState->bDrawInfo			= 1;
	pActionState->bNeedReloadMap		= 1;
	pActionState->bResetMapCenter		= 1;
	pActionState->nCarCenterMode		= CAR_CENTER_MODE_ONE_HALF_XY;
	pActionState->ptNewMapCenter		= actionState.ptNewMapCenter;
	pActionState->nDrawMode			= DRAWMODE_NUROJUMPMOVE;
	__android_log_print(ANDROID_LOG_INFO, "Mouse2", "DragMove Tick %d\n", nuGetTickCount() - Tick1);
	return nuTRUE;
}

nuBOOL CJumpMove::DrawCenterPoint(PACTIONSTATE pActionState) //萸僻痄芞腔雄怓徹最
{
	if( m_pNaviThd == NULL)
	{
		return nuFALSE;
	}
	nuINT nNowIdx = 0;
	m_pNaviThd->ResetActionState(pActionState);
	pActionState->bDrawBmp1		= 1;
	pActionState->bDrawInfo		= 1;
	pActionState->bNeedReloadMap	= 1;
	pActionState->bResetMapCenter	= 1;
	pActionState->bDrawRoadName	= 1;
	pActionState->bDrawPOIName	= 1;
	nuLONG nNidx = 0;
	if( !g_fileSysApi.FS_PtInBoundary(pActionState->ptNewMapCenter)||
		!m_pNaviThd->m_gDataMgr.SetUp(pActionState) )
	{
		return false;
	}
	if( !g_drawmapApi.DM_LoadMap(pActionState)	)
	{
		return false;
	}
	if(	!g_drawmapApi.DM_DrawMapBmp1(pActionState) )
	{
		return false;
	}
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
	}	
	if(	!g_drawmapApi.DM_DrawMapBmp2(pActionState) )
	{
		return false;
	}
	NUROPOINT ptCar;
	g_drawmapApi.DM_DrawMapCar(ptCar);
	g_mathtoolApi.MT_SaveOrShowBmp(SHOW_DEFAULT);
	return nuTRUE;
}

nuBOOL CJumpMove::BmpCopyEx2(PNURO_BMP pDesBmp, nuINT desx, nuINT desy, PNURO_BMP pSrcBmp, nuINT srcx, nuINT srcy,nuINT desw, nuINT desh, nuINT srcw, nuINT srch)
{
	//desw = pDesBmp->bmpWidth;
	//desh = pDesBmp->bmpHeight;
	//srcw = pSrcBmp->bmpWidth;
	//srch = pSrcBmp->bmpHeight;
	if( desx >= pDesBmp->bmpWidth || desy >= pDesBmp->bmpHeight || pSrcBmp->pBmpBuff == NULL)
	{
		return nuFALSE;
	}
	nuLONG xrIntFloat_16 = (srcw<<16)/desw+1;
	nuLONG yrIntFloat_16 = (srch<<16)/desh+1;
	nuWORD* pDstLine = 0;
	nuWORD* pSrcLine = 0;    
	nuLONG y_16 = 0;
	nuWORD transColor = (nuWORD)pSrcBmp->bmpTransfColor;
	nuLONG bytesWidthDes = pDesBmp->bytesPerLine/2;
	nuLONG bytesWidthSrc = pSrcBmp->bytesPerLine/2;
	nuLONG x_16 = 0;
	pDstLine = (nuWORD*)(pDesBmp->pBmpBuff + pDesBmp->bytesPerLine*desy);
	nuINT ytag = 0;
	pSrcLine = (nuWORD*)(pSrcBmp->pBmpBuff + pSrcBmp->bytesPerLine*(srcy+(y_16>>16)));
	nuWORD* pDstTag = 0;
	nuWORD* pSrcTag = 0;
	nuINT xtag = 0;
	for( nuLONG y = 0; y < desh; ++y )
	{
		x_16 = 0;
		pDstTag = pDstLine + desx;
		xtag = 0;
		pSrcTag = pSrcLine + srcx;
		for( nuLONG x = 0; x < desw; ++x )
		{
			if( *pSrcTag != transColor )
			{
				*pDstTag = *pSrcTag;
			}
			++pDstTag;
			x_16 += xrIntFloat_16;
			if( (x_16>>16) > xtag )
			{
				++xtag;
				++pSrcTag;
			}
		}
		y_16 += yrIntFloat_16;
		pDstLine += bytesWidthDes;
		if( (y_16>>16) > ytag )
		{
			++ytag;
			pSrcLine += bytesWidthSrc;
		}
	}
	return nuTRUE;
}
