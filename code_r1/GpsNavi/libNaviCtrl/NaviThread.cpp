// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: NaviThread.cpp,v 1.85 2010/11/02 07:19:23 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/11/02 07:19:23 $
// $Locker:  $
// $Revision: 1.85 $ 
// $Source: /home/nuCode/libNaviCtrl/NaviThread.cpp,v $
///////////////////////////////////////////////////////////////////////// 
#include "NaviThread.h"

#include "NuroModuleApiStruct.h"
//
#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "nuTTSdefine.h"
#include "SoundThread.h"	
#include <stdio.h>
#include "GdiApiTrans.h"
#ifdef ANDROID
#include "NuroCore.h"
#endif
extern class CGdiApiTrans*          g_pGdiApiTrans;
#include "MapInfo.h"
#include "NURO_DEF.h"
// 51 is beginning..
#define NAVI_THREAD_PRIO_ID     51
//#define _USE_LICENSE

//#ifdef _USE_LICENSE
//#include "NuroEncryptZK.h"
//#include "NewDes.h"
//#endif
//#define  _USE_LOG_FILE_NAVITRCL
#ifdef _USE_LOG_FILE_NAVITRCL
#include "LogFileCtrlZ.h"
class CLogFileCtrlZ g_logFile;
#endif
#ifdef ANDROID
static int counter = 0 ;
#endif
//#include <windows.h>
extern OPENRSAPI					g_OpenResourceApi;
extern GDIAPI						g_gdiApi;
extern MATHTOOLAPI					g_mathtoolApi;
extern ROUTEAPI						g_routeApi;
extern FILESYSAPI					g_fileSysApi;
extern INNAVIAPI					g_innaviApi;
//extern DRAWINFOMAPAPI				g_drawIfmApi;
extern DRAWMAPAPI					g_drawmapApi;
//extern DRAWINFONAVIAPI				g_drawIfnApi;
extern MEMMGRAPI					g_memMgrApi;
extern GPSOPENAPI					g_gpsopenApi;
extern class CSoundThread*				g_pSoundThread;

#define DRAW_NO							0x00000000
#define DRAW_YES						0x00000001
#define	DRAW_JUMPMOVE					0x00000002
#define DRAW_USERSET					0x00000004
#define DRAW_GPS						0x00000008
#define DRAW_SIM						0x00000010 
#define DRAW_TIMER						0x00000020

#define DRAW_STOP						0x00000040
//#define DRAW_FIRST					0x10000000

#define GPS_ROADNAME_UPDATE_TIMES	3

#define PLAY_NAVISTART_NO					0x00
#define PLAY_NAVISTART_NEEDPLAY				0x01
#define PLAY_NAVISTART_PLAYED				0x02

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/* static functions */

static nuDWORD g_testTime = 0;
nuVOID CNaviThread::ResetActionState(PACTIONSTATE pActionState)
{
	pActionState->bDrawBmp1			= 0;
	pActionState->bDrawZoom			= 0;
	pActionState->bDrawInfo			= 0;
	pActionState->bNeedFixed		= 0;
	pActionState->bNeedReloadMap	= 0;
	pActionState->bResetIconCarPos	= 0;
	pActionState->bResetMapCenter	= 0;
	pActionState->bGlideMoveDraw	= 0;
	pActionState->bResetRealCarPos	= 0;
	pActionState->bResetMemBmpSize	= 0;
	pActionState->bResetScale		= 0;
	pActionState->bResetScreenSize	= 0;
	pActionState->bShowMap			= 0;
	pActionState->bDrawPOIName		= 1;
	pActionState->bDrawRoadName		= 1;
	pActionState->bSetAngle			= 0;
	pActionState->bLoad2kBlock		= 1;
	pActionState->bIsSplitting		= 0;
	pActionState->bNeedExtendRotate	= 0;
	pActionState->bReDrawCrossZoom	= 0;
	pActionState->nAngle			= -1;
	pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
	pActionState->nBmpDownExtend	= 0;
	pActionState->nBmpLeftExtend	= 0;
	pActionState->nBmpRightExtend	= 0;
	pActionState->nBmpUpExtend		= 0;
	pActionState->nCarCenterMode	= CAR_CENTER_MODE_REMAIN; 
	pActionState->nDrawMode			= DRAWMODE_MOVE;
//	pActionState->nSpeed			= -1;
}

nuDWORD CNaviThread::ThreadAction(nuPVOID lpVoid)
{
//	nuSleep(50);
	class CNaviThread *pThis = (CNaviThread*)lpVoid;
	g_fileSysApi.pGdata->uiSetData.pGpsData = &pThis->m_gpsData;
	ACTIONSTATE actionState  = {0};
	CODENODE codeNode		 = {0};
	nuUINT nDraw			= DRAW_NO;
	pThis->m_pAcState		= &actionState;
	pThis->m_nPlayedStartNaviState = PLAY_NAVISTART_NO;
	pThis->m_bIsMoving = nuFALSE;
	nDraw = pThis->ThreadPowerOn(&actionState);

	while( !pThis->m_bQuit )
	{
		nuSleep(TIMER_TIME_UNIT); //10ms

		if( actionState.nDrawMode != DRAWMODE_POWERON )
		{
			nuSleep(TIMER_TIME_UNIT);
			nDraw = DRAW_NO;
			pThis->DataExchange();
		}
		//Pre process
		while( !pThis->m_bQuit && !nDraw && pThis->m_codePre.PopNode(&codeNode) )
		{
			nDraw |= pThis->CodePreProc(&codeNode, &actionState);
			/*
			if( nDraw & DRAW_STOP )
			{
				pThis->m_bStopThread = nuTRUE;
				break;
			}
			*/
		}
		
		if(g_fileSysApi.pGdata->bWaitQuit)
		{
			nuSleep(500);
			continue;
		}
		
		if( pThis->m_nThdState == NAVI_THD_STOP_ALLL )
		{
			nuSleep(pThis->m_lDrawSleep);
			continue;
		}
		if( !nDraw && pThis->m_nThdState == NAVI_THD_WORK )
		{
			nuBOOL bIsMoving = nuTRUE;
			pThis->m_bIsMoving = nuTRUE;
			/*if( pThis->m_pMsgData )
			{
				pThis->NuroMoveMap(MOVE_STOP, nuFALSE);
				bIsMoving = nuFALSE;
			}*/
			while( bIsMoving )
			{
				//Glide moving
				/*bIsMoving = pThis->m_glideMove.MoveProc(&actionState);
				if( bIsMoving )
				{
					nDraw |= DRAW_JUMPMOVE;
				}*/
				//Jumping move
				pThis->m_DrawDataForUI.bIsMoving = nuTRUE;
				bIsMoving = pThis->m_jumpMove.JumpMoveProc(&actionState);

				if( bIsMoving )
				{
					nDraw |= DRAW_JUMPMOVE;
				}
				else
				{
					pThis->m_DrawDataForUI.bIsMoving = nuFALSE;
				}
			}
			if( nDraw )
			{
				//*****
				pThis->SetMoveWait();
			}
			pThis->m_bIsMoving = nuFALSE;
		}

		//UI action
		while( !pThis->m_bQuit && !nDraw && pThis->m_codeDraw.PopNode(&codeNode) )
		{
			nDraw = pThis->CodeDrawProc(&codeNode, &actionState);
		}

		//pause naviThread
        	if( pThis->m_nThdState == NAVI_THD_STOP_PREDRAW )   
		{
			nuSleep(pThis->m_lDrawSleep);
			g_drawmapApi.DM_FreeDrawData(DRAW_MAP_FREE_BGL);
			continue;
		}
		
		while( !pThis->m_bQuit && !nDraw && pThis->m_codeNavi.PopNode(&codeNode) )
		{
			nDraw |= pThis->CodeNaviProc(&codeNode, &actionState);	
		}

//		nuShowString(("NAVI_THD_STOP_PREDRAW"));
		/*
		if( !pThis->m_bQuit && pThis->m_bStopThread )
		{
			continue;
		}
		*/
		//GPS, simulation and timer process
		if( !nDraw )
		{
			nDraw = pThis->AutoMoveProc(&actionState);
			//GPS fake move...
			if( /*pThis->m_nThdState == NAVI_THD_WORK && */
				pThis->m_nGpsFakeTotalTimes <= 1 &&
				g_fileSysApi.pUserCfg->nMoveType && 
				(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS ||
				 g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION) )
			{
				if( !nDraw )
				{
					pThis->DrawGpsFakeMove();
				}
				else
				{
					pThis->DrawGpsFakeMove(&actionState);
				}
			}
			/*Col BitmapExtend*/
			if( nDraw && 
				(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS ||
				 g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION) )
			{
				actionState.nBmpSizeMode = pThis->ColBmpExtend();
			}
		}
		nDraw |= pThis->DataUpdate(&actionState);
		if( !pThis->m_bQuit && !nDraw )
		{
			/*
			*/
			if( !g_fileSysApi.pGdata->timerData.bUpdateMap )
			{
				nuSleep(pThis->m_lDrawSleep);
				continue;
			}
			if( pThis->m_nThdState != NAVI_THD_WORK )
			{
				g_fileSysApi.pGdata->timerData.bUpdateMap		= 0;
				g_fileSysApi.pGdata->timerData.nUpdateMapTime	= 0;
				nuSleep(pThis->m_lDrawSleep);
				continue;
			}
			actionState.bDrawInfo	= 1;
			pThis->m_bUpdateMap     = 1; //???????2???
			actionState.nDrawMode	= DRAWMODE_TIMER;
		}

		//Process of the zooming.
		/*
		*/
		if( pThis->m_nZoomAction && !pThis->m_bQuit && pThis->m_nThdState == NAVI_THD_WORK )
		{
			pThis->m_DrawDataForUI.bIsMoving = nuTRUE;
			pThis->m_zoomDraw.ZoomProcess(&actionState);
		}
		pThis->DragZoomDraw(&actionState);
		if( pThis->m_bQuit || !pThis->DrawReady(&actionState) )
		{
			nuSleep(pThis->m_lDrawSleep);
			continue;
		}
		if( pThis->m_nThdState == NAVI_THD_STOP_DRAW )
		{
			nuSleep(pThis->m_lDrawSleep);
			continue;
		}
		nuSleep(pThis->m_lDrawSleep);
		pThis->DrawAction(&actionState);
		//For Fake moing
		pThis->m_bGpsFakeMove	= 0;
		pThis->m_nGpsFakeMoveTimer = 0;
		pThis->m_nGpsFakeAdd	= g_fileSysApi.pUserCfg->nMoveType;//GPS_FAKE_MAX_TIMES;
		/*
		if( !g_fileSysApi.pGdata->uiSetData.b3DMode && 
			pThis->m_nGpsFakeTotalTimes <= 1 &&
			g_fileSysApi.pGdata->drawInfoMap.bCarInRoad && 
			( actionState.nDrawMode == DRAWMODE_GPS  ||actionState.nDrawMode == DRAWMODE_SIM ) &&
			g_fileSysApi.pGdata->carInfo.nShowSpeed >= 20 &&
			g_fileSysApi.pGdata->carInfo.nShowSpeed <= 200 &&
			!pThis->m_pMenuInfo->nMenuState && 
			pThis->m_pMsgData == NULL )
		{
			pThis->m_nGpsFakeDis = (nuBYTE)((g_fileSysApi.pGdata->carInfo.nShowSpeed * 10 * g_fileSysApi.pGdata->drawInfoMap.nScalePixels ) / 
				(36 * g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale));
			if( ( actionState.nDrawMode == DRAWMODE_SIM  ||
				 (NURO_ABS(g_fileSysApi.pGdata->carInfo.ptCarIcon.x - pThis->m_pointOldIcon.x) > 3 &&
				  NURO_ABS(g_fileSysApi.pGdata->carInfo.ptCarIcon.y - pThis->m_pointOldIcon.y) > 3) ) && 
				pThis->m_nGpsFakeDis > 0 )
			{
				pThis->m_nGpsFakeMoveTimer  = nuGetTickCount();
				pThis->m_nGpsFakeAdd		= 0;
			}
			pThis->m_pointOldIcon = g_fileSysApi.pGdata->carInfo.ptCarIcon;
		}
		*///20100302
		/* Rotate Map */
//		DEBUGSTRING(nuTEXT("Rotate Map"));
		

//		DEBUGSTRING(nuTEXT("Draw end"));
		//
		nDraw = DRAW_NO;
		ResetActionState(&actionState);

		g_fileSysApi.pGdata->timerData.bUpdateMap		= 0;
		g_fileSysApi.pGdata->timerData.nUpdateMapTime	= 0;
//		g_memMgrApi.MM_CollectDataMem();
		pThis->m_bPowerOn	= nuTRUE;
	}

	pThis->m_bPowerOn	= nuTRUE;
//	g_drawmapApi.DM_StopTravel(nuTRUE);
	g_gpsopenApi.GPS_Close();
	pThis->StopNavi(&actionState);
	pThis->m_bOutThreadLoop = nuTRUE;

	//nuThreadWaitReturn(m_hThread);
//	pThis->m_hThread = NULL;

	return 0;
}

/* member functions */
CNaviThread::CNaviThread()
{
	m_hThread		= NULL;
	//nuMemset(&m_hThread, 0, sizeof(m_hThread));

	#ifndef NURO_OS_ECOS
	//m_dwThreadID	= 0;
	//m_hThread.nThdID = 0;
	#endif
	m_bQuit			= nuTRUE;
	m_bOutThreadLoop= nuTRUE;
//	m_bStopThread	= nuTRUE;
	m_bPowerOn		= nuFALSE;
	m_bGetNewGpsData= nuFALSE;
	m_nNaviErrorTimer	= 0;
	m_nGpsFakeAdd		= 0;
	m_nGpsFakeMoveTimer	= 0;
	nuMemset(&m_gpsData, 0, sizeof(GPSDATA));
//	nuMemset(&m_roadNameStr, 0, sizeof(ROADNAMESTR));
	m_nTotalDisGone			= 0;
	m_nHighSpeed			= 0;
	m_nSvLtPosTimer			= 0;

	m_nOldNaviState		= 0;
	m_pTraceFile	= NULL;
	m_pAcState		= NULL;
	m_nMoveScale	= 1;
	m_bKeyDown		= nuFALSE;
	m_nGpsReadyType	= 0;
	m_pfSaveMapBmp	= NULL;

	m_bSaveMapBmp   = nuFALSE;
	m_bUpdateMap    = nuFALSE;
	m_bSetUserData  = nuFALSE;
	nuMemset(&m_DrawDataForUI, 0, sizeof(NC_DRAR_DATA));
	
	m_bSimuReachDes = nuFALSE;
	m_nAutoScaleTick = 0; // Added by Damon 20120525
	#ifdef USE_HUD_DR_CALLBACK 
	nTimeStart=-1;		  // Prosper 20121125
	nHudTimeStart=-1;
	m_bTunnel=0;
	#endif

	m_bSearchNearPoi = nuFALSE;

	m_nB_Blk	 = -1;
	m_nB_RoadIdx = -1;
	m_nJumpRoadAdd = 0;
	
	m_bJumpRoad = nuFALSE;
	m_bJumpAdd = nuFALSE;
	
}

CNaviThread::~CNaviThread()
{
	FreeThread();
	//
	if( NULL != m_pfSaveMapBmp )
	{
		m_pfSaveMapBmp = NULL;
	}
}

nuBOOL CNaviThread::Initialize()
{
	if( !nuReadConfigValue("MARKETFIXDIS", &m_nMarketFixDis) )
	{
		m_nMarketFixDis = 500;
	}
	if(!nuReadConfigValue("DRAWSLEEP", &m_lDrawSleep))
	{
		m_lDrawSleep = 150;
	}
	nuLONG nTmp = 0;
	if( nuReadConfigValue("ENDTOCLEANMT", &nTmp) && !nTmp )
	{
		m_bCleanMarket = nuFALSE;
	}
	else
	{
		m_bCleanMarket = nuTRUE;
	}
	if( !nuReadConfigValue("GPSREADYTYPE", &nTmp) )
	{
		nTmp = 1;
	}
	m_nGpsReadyType = (nuBYTE)nTmp;
	
	if( !nuReadConfigValue("MAPCENTERX", &m_ptMapCenter.x) )
	{
		m_ptMapCenter.x = 12015760;
	}
	if( !nuReadConfigValue("MAPCENTERY", &m_ptMapCenter.y) )
	{
		m_ptMapCenter.y = 3017949;
	}
	if( !nuReadConfigValue("LASTSCALEVIEW", &nTmp) )
	{
		nTmp = 0;
	}
	if(!nuReadConfigValue("SOURCETYPE", &m_lUseGPSSource))
	{
		m_lUseGPSSource = 0;
	}
	if(!nuReadConfigValue("VENDERTYPE", &m_lVenderType))
	{
		m_lVenderType = 0;
	}
	if(!nuReadConfigValue("SIMETAG", &m_lSIMEtag))
	{
		m_lSIMEtag = 0;
	}
	if(!nuReadConfigValue("FREEROADWEIGHT", &m_nFreeRoadWeight))
	{
		m_nFreeRoadWeight = 30;
	}
	if(!nuReadConfigValue("NAVIROADWEIGHT", &m_nNaviRoadWeight))
	{
		m_nNaviRoadWeight = 30;
	}
	if( nTmp )
	{
		m_nScaleNoNavigation = g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount;
	}
	else
	{
		m_nScaleNoNavigation = g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount - 1;
	}
	m_nLastMapDirectMode = g_fileSysApi.pUserCfg->nMapDirectMode;
	if( g_fileSysApi.pGdata->uiSetData.nScaleIdx >= m_nScaleNoNavigation )
	{
		m_bInNoNaviScale = nuTRUE;
	}
	else
	{
		m_bInNoNaviScale = nuFALSE;
	}
	if( !nuReadConfigValue("ZOOMANIMATION", &nTmp) )
	{
		nTmp = 1;
	}
	m_nZoomAction = (nuBYTE)nTmp;
	if( !nuReadConfigValue("LASTSCALETYPE", &nTmp) )
	{
		nTmp = 0;
	}
	m_nLastScaleType	= (nuBYTE)nTmp;
	if( !nuReadConfigValue("HUDFLUSHTIME", &m_nHUDFlushTime) )
	{
		m_nHUDFlushTime = 500;
	}
	if( !nuReadConfigValue("CEUSEHUD", &m_nUseHUD) )
	{
		m_nUseHUD = 0;
	}
	if( !nuReadConfigValue("LOGHUD", &m_nLogHUD) )
	{
		m_nLogHUD = 0;
	}
	if( !nuReadConfigValue("HUDUSEODB", &m_nHUDUseODB))
	{
		m_nHUDUseODB = 0;
	}
	if(!nuReadConfigValue("USETMC",&m_lUseTMC))
	{
		m_lUseTMC = 0;
	}
	if(!nuReadConfigValue("DRAWSLEEP",&m_lDrawSleep))
	{
		m_lDrawSleep = 25;
	}
	m_lSpeedLimit = 10;
	m_PreRoadCenter.nBlkIdx = 65535;
	m_bFindRoad = nuTRUE;
	m_ccdInfo.bNewInfo	= 0;
	m_bFullViewMode		= nuFALSE;
	m_nUseGPSState		= DATA_TYPE_INVALID;
	//Gps fake moving
	nuMemset(&m_gpsDataOld, 0, sizeof(GPSDATA));
	m_gpsDataOld.nStatus	= DATA_TYPE_INVALID;
	nuMemset(&m_gpsNew, 0, sizeof(GPS_NODE));
	nuMemset(&m_gpsOld, 0, sizeof(GPS_NODE));
	nuMemset(&m_gpsNow, 0, sizeof(GPS_NODE));
	nuMemset(&m_ptTunnelPoint, 0, sizeof(NUROPOINT));
	m_nVelocityX1 = 0;
	m_nVelocityY1 = 0;
	if( !nuReadConfigValue("GPSMOVETIMES", &nTmp) )
	{
		nTmp = 10;
	}
	m_nGpsFakeTotalTimes = m_nGpsFakeNowTimes = nTmp;

	if (!nuReadConfigValue("AUTOSCALETIME", &m_nAutoScaleTime) ) // Added by Damon 20120525
	{
		m_nAutoScaleTime = 5000;
	}
	if(!nuReadConfigValue("HUDTYPE", &m_lHUDType))
	{
		m_lHUDType = 0;
	}
	nuMemset(&m_KMInfo, 0, sizeof(m_KMInfo));
	nuMemset(m_wsKilometer, 0, sizeof(m_wsKilometer));
	nuMemset(m_wsKMName, 0, sizeof(m_wsKMName));
	nuMemset(&m_ptEndPt, 0,sizeof(NUROPOINT));
	m_lMoveConst	= 0;
	m_lKMNum		= 0;
	m_lPreKMNum		= 0;
	m_lPreKMDis		= 0;
	m_lKMTick		= 0;
	m_dAddKiloDis   = 0;
	m_dAddKiloNum   = 0;
	m_dSimTick		= 0;
	m_bFreeTunnel   = nuFALSE;
	m_nMapRoadFontSize = -1;
	m_bFirstTunnel  = nuFALSE;
	m_bFirst  = nuFALSE;
	m_nDis = 0;
	m_bTunnel 		= nuFALSE;
	m_bFreeTunnel 	= nuFALSE;
	m_lSendEtag		= 0;
	m_lHighwayID = 0;
	m_lHighWayCrossID = 0;
//========For SDK========================//
	m_bMoveMapType1 = nuFALSE;
	m_bDragMove  = nuFALSE;
	m_bDragZoom  = nuFALSE;
	m_fZoomDis   = 0;
	m_nRealPicID = 0;
	m_pPackID    = NULL;
	nuMemset(&m_RealPicBmp, 0, sizeof(NURO_BMP));
	nuMemset(&m_PackPicPara, 0, sizeof(NURO_PACK_PIC_PARAMETER));
//========For SDK========================//
	nuMemset(&m_UI_DAIWOO_BT, 0, sizeof(UI_DAIWOO_BT));
#ifdef NURO_OS_WINCE
	if(m_lVenderType == _VENDER_NECVOX)
	{
		m_HudComCtrl.bOpenCom(g_fileSysApi.pGdata->pTsPath);
		m_bSendHUDChange = nuTRUE;
	}
	else if(m_lVenderType == _VENDER_DAIWOO)
	{
		m_Hudhwnd = NULL;
		m_Hudhwnd = ::FindWindow(NULL, nuTEXT("MtkGps"));
	}
	m_nHUDTick = 0;
#endif
	#ifdef USE_HUD_DR_CALLBACK
	m_pfHUDCallBack = NULL;
	m_pfDRCallBack = NULL;
	#endif
	m_bTMC_Event_Data_Initial = nuFALSE;
	m_nFlush_Map_For_TMC = 0;
	g_fileSysApi.pGdata->bWaitQuit = nuFALSE;
	return nuTRUE;
}

nuBOOL CNaviThread::InitThread()
{
	if( m_bPowerOn )
	{
		return nuTRUE;
	}
	nuLONG nTracemode;
	if( !nuReadConfigValue("TRACEFUNCTION", &nTracemode) )
	{
		nTracemode = 0;
	}
    if( !nuReadConfigValue("DISTOSTOPNAVI", &m_nDisToEnd) )
    {
        m_nDisToEnd = 30;
    }
	if( !(m_codeDraw.Initialize() & m_codeNavi.Initialize() & m_codePre.Initialize()) )
	{
		return nuFALSE;
	}
	if( !m_gDataMgr.Initialize() )
	{
		return nuFALSE;
	}
	if( !m_jumpMove.Initialize(this) )
	{
		return nuFALSE;
	}
	/*if( !m_glideMove.Initialzie(this) )
	{
		return nuFALSE;
	}*/
	if( !m_zoomDraw.Initialize(this) )
	{
		return nuFALSE;
	}
	if( !m_turnDraw.Initialize(this) )
	{
		return nuFALSE;
	}
	if( nTracemode )
	{
		m_pTraceFile = new CTraceFileZ();
		if( m_pTraceFile == NULL )
		{
			return nuFALSE;
		}
		if( !m_pTraceFile->Initialize() )
		{
			delete m_pTraceFile;
			m_pTraceFile = NULL;
		}
	}
	else
	{
		m_pTraceFile = NULL;
	}
	m_bQuit		= nuFALSE;
	m_nThdState	= NAVI_THD_WORK;
    m_drawToUI.nWaitType    = _WAIT_TYPE_DEFAULT;
	
	//
#ifdef NURO_OS_WINDOWS
	//////////////////////////////////////////////////////////////////
	m_hThread = nuCreateThread(NULL, 0, ThreadAction, this, 0, &m_dwThreadID);
	//nuSetThreadPriority(m_hThread, NURO_THREAD_PRIORIYT_NORMAL);
#endif

#ifdef NURO_OS_LINUX
	m_hThread = nuCreateThread(NULL, 0, ThreadAction, this, 0, &m_dwThreadID);
#endif

#if defined(NURO_OS_UCOS) || defined(NURO_OS_JZ)
	nuMemset(m_nGpsBuff, 0, sizeof(nuBYTE)*GPS_BUFF_SIZE);
	m_nGpsBuffLen = 0;
	m_hThread = nuCreateThread(NULL, 0, (LPNURO_THREAD_START_ROUTINE)ThreadAction, this, TASK_NAVI, (nuPDWORD)(&m_pStack[NAVI_STACK_SIZE-1]));
#endif

#ifdef NURO_OS_ECOS
	DebugOut("----Create Navigation thread start----\n");
	NURO_SECURITY_ATTRIBUTES attr;
	attr.nLength = (nuDWORD)(&m_hThread.nThdID);
	attr.bInheritHandle = (nuBOOL)(&m_hThread);
	m_hThread = nuCreateThread ( &attr,
								 4 * NAVI_STACK_SIZE,
								 (LPNURO_THREAD_START_ROUTINE)ThreadAction,
								 this,
								 TASK_NAVI,
								 (nuPDWORD)m_pStack );
	//DebugOut("----Create Navigation thread end----\n");
#endif
#ifdef NURO_OS_HV_UCOS
	m_hThread = nuCreateThread( NULL, NAVI_STACK_SIZE, (LPNURO_THREAD_START_ROUTINE)ThreadAction,this,TASK_NAVI,NULL );
#endif
	if( m_hThread == NULL )
	{
		return nuFALSE;
	}
	//???2011.02.22
	//while( !m_bPowerOn )
	//{
	//	nuSleep(25);
	//}
	return nuTRUE;
}

nuVOID CNaviThread::FreeThread()
{
	m_bQuit = nuTRUE;
	while (!m_bOutThreadLoop)
	{
		nuSleep(25);
	}
	if (NULL != m_hThread)
	{
		nuDelThread(m_hThread);
	}
	m_hThread = NULL;
	//m_hThread.handle = NULL;
	m_bPowerOn = nuFALSE;
	//
	if (m_pTraceFile != NULL)
	{
		m_pTraceFile->Free();
		delete m_pTraceFile;
		m_pTraceFile = NULL;
	}
	//
	m_turnDraw.Free();
	m_zoomDraw.Free();
	//m_glideMove.Free();
	m_jumpMove.Free();
	m_gDataMgr.Free();
	m_codePre.Free();
	m_codeNavi.Free();
	m_codeDraw.Free();
#ifdef NURO_OS_WINCE
	if(m_lVenderType == _VENDER_NECVOX)
	{
		m_HudComCtrl.bCloseCom();
	}
#endif
	//--------------SDK------------------------//
	FreeArchivePackPic(m_pPackID, m_RealPicBmp);
	//--------------SDK------------------------//	
}

nuUINT CNaviThread::ThreadPowerOn(PACTIONSTATE pActionState)
{
	//m_pMenuInfo		= NULL;
	//m_pMsgData		= NULL;
	if( !nuReadConfigValue("REROUTETIMES", &m_nErrorTimesToReroute) ) 
	{
		m_nErrorTimesToReroute = 6;
	}
	//m_nErrorTimesToReroute *= 1000;
	if( !nuReadConfigValue("MAXFIXROADDIS", &m_nMaxRoadFixDis) )
	{
		m_nMaxRoadFixDis = 60;
	}
	if( !nuReadConfigValue("MINMOVEDIS", &m_nMinMoveDis) )
	{
		m_nMinMoveDis = 15;
	}
	if( !nuReadConfigValue("MINMOVEANGLE", &m_nMinMoveAngle) )
	{
		m_nMinMoveAngle = 0;
	}
	if( !nuReadConfigValue("GLIDEMOVEMS", &m_nGlideMoveMinMS) )
	{
		m_nGlideMoveMinMS = 400;
	}
	if( !nuReadConfigValue("GPSMODE", &m_nGPSMode) )
	{
		m_nGPSMode = 8;
	}
	if (nuTRUE == g_gpsopenApi.GPS_Open())
	{   // InitGPS Added by Damon 20120705
		nuLONG nGPSComPort, nGPSBaudrate;
		g_fileSysApi.pGdata->timerData.bResetGPS = 0;
		g_gpsopenApi.GPS_GetGPSComConfig(nGPSComPort, nGPSBaudrate);
		g_fileSysApi.pGdata->timerData.nGPSComPort  = nGPSComPort;
	    g_fileSysApi.pGdata->timerData.nGPSBaudrate = nGPSBaudrate;
	}
	
//	m_bStopThread	 = nuFALSE;
	m_bOutThreadLoop = nuFALSE;
	m_bIsDrawing	 = nuFALSE;
	//
	ResetActionState(pActionState);
	pActionState->bDrawBmp1	= 1;
	pActionState->bDrawInfo	= 1;
	pActionState->bNeedReloadMap	= 1;
	pActionState->bResetMapCenter	= 1;
	pActionState->bResetRealCarPos	= 1;
	pActionState->bResetIconCarPos	= 1;
	pActionState->bResetScale		= 1;
	pActionState->bResetScreenSize	= 1;
	pActionState->nSpeed			= 0;
	pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
	pActionState->ptNewMapCenter.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;//12156151;
	pActionState->ptNewMapCenter.y	= g_fileSysApi.pGdata->transfData.nMapCenterY;//2503960;

	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
	{
		pActionState->nAngle	= 90;//g_fileSysApi.pGdata->carInfo.nCarAngle;
	}
	else
	{
		pActionState->nAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
	}
	pActionState->nDrawMode	= DRAWMODE_POWERON;
	g_fileSysApi.pGdata->timerData.nAutoMoveType  = AUTO_MOVE_GPS;
	return DRAW_YES;
}

nuUINT CNaviThread::CodePreProc(PCODENODE pCodeNode, PACTIONSTATE pActionState)
{
//	nuroPOINT point;
//	nuUINT nRes;

	switch( pCodeNode->nActionType )
	{
		case AC_DRAW_SETUSERCFG://there is no waiting time;
			return UserCfgProc(pCodeNode, pActionState);
		/*case AC_DRAW_MOUSEEVENT:
			if( MouseProc(pCodeNode, pActionState) )
			{
				break;
			}
			return DRAW_NO;
		case AC_DRAW_KEYEVENT:
			if( KeyProc(pCodeNode->nX, (nuBYTE)pCodeNode->nExtend, pActionState) )
			{
				break;
			}
			return DRAW_NO;*/
		case AC_DRAW_DRAWINFODLG://Draw dialog in InfoMap
			return DRAW_NO;
		case AC_DRAW_PAUSETHREAD:
			g_memMgrApi.MM_CollectDataMem(0);
			return DRAW_STOP;
		case AC_DRAW_GETROUTELIST:
			//if Success...
			m_drawToUI.nReserve = g_innaviApi.IN_GetRoutingList(m_pRoutingList, ROUTINGLIST_PAGE_MAX_NUM, pCodeNode->nX);
			m_drawToUI.bUIWaiting = 0;
			return DRAW_NO;
		case AC_DRAW_GETMAPINFO:
			m_drawToUI.nReturnCode = SetMapInfo(pCodeNode->nX);
			m_drawToUI.bUIWaiting = 0;
			return DRAW_NO;
		case AC_DRAW_SETNAVITHD:
			g_memMgrApi.MM_CollectDataMem(0);
			if( pCodeNode->nExtend == m_nThdState )
			{
				return DRAW_NO;
			}
			if( pCodeNode->nExtend == NAVI_THD_WORK )
			{
				pActionState->bDrawBmp1			= 1;
				pActionState->bDrawInfo			= 1;
				pActionState->bNeedReloadMap	= 1;
				pActionState->bResetMapCenter	= 1;
				pActionState->bResetScale		= 1;
				m_nThdState = (nuBYTE)pCodeNode->nExtend;
				return DRAW_YES;
			}
			else
			{
				m_nThdState = (nuBYTE)pCodeNode->nExtend;
				return DRAW_YES;
			}
		case AC_DRAW_TRACEACTION:
			if( m_pTraceFile )
			{
				m_drawToUI.nReturnCode = m_pTraceFile->ShowTrace(nuLOWORD(pCodeNode->nX));
				m_drawToUI.bUIWaiting = 0;
			}
			return DRAW_NO;
		case AC_DRAW_GPSGETDATA:
			IntervalGetGps();
			break;
		case AC_DRAW_QIUT:
			g_fileSysApi.pGdata->bWaitQuit = nuTRUE;
			break;
		case AC_DRAW_GETROUTELIST_D:
			m_pUIGetData = GetNaviListData((PROUTINGLISTNODE)pCodeNode->pDataBuf[0], (nuINT*)pCodeNode->pDataBuf[1], (nuINT*)pCodeNode->pDataBuf[2]);	
			m_drawToUI.bUIWaiting = 0;
			break;
		default:
			return DRAW_NO;
	}
	if( m_nThdState == NAVI_THD_WORK )//New StopNTD
//	if( !m_bStopThread )
	{
		//@2011.03.28
		/*while( m_bIsDrawing )
		{
			nuSleep(25);
		}
		m_bIsDrawing = nuTRUE;
		DrawAction(pActionState);
		m_bIsDrawing = nuFALSE;*/
	}
	pActionState->bShowMap	= 0;
	pActionState->bDrawInfo = 0;
	m_bUpdateMap            = 0;
	return DRAW_NO;
}

nuUINT CNaviThread::CodeDrawProc(PCODENODE pCodeNode, PACTIONSTATE pActionState)
{
	pActionState->nDrawMode	= DRAWMODE_UI;
	nuUINT nres = DRAW_YES;
	nuroPOINT point			= {0};
	GPSDATA	  TmpGpsData = {0};

	switch(pCodeNode->nActionType)
	{
		case AC_DRAW_ONPAINT:
			pActionState->bShowMap = 1;
			break;
		case AC_DRAW_REDRAWINFO:
			pActionState->bDrawInfo = 1;
			break;
		case AC_DRAW_REDRAWMAP:
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			break;
		case AC_DRAW_POWERON:
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->bResetRealCarPos	= 1;
			pActionState->bResetIconCarPos	= 1;
			pActionState->bResetScale		= 1;
			pActionState->bResetScreenSize	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
			pActionState->ptNewMapCenter.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
			pActionState->ptNewMapCenter.y	= g_fileSysApi.pGdata->transfData.nMapCenterY;
			if ( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
			{
				pActionState->nAngle	= 90;//g_fileSysApi.pGdata->carInfo.nCarAngle;
			}
			else
			{
				pActionState->nAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
			}
			pActionState->nDrawMode	= DRAWMODE_POWERON;
			g_fileSysApi.pGdata->timerData.nAutoMoveType  = AUTO_MOVE_GPS;
			break;
		case AC_DRAW_JUMPTOMAPXY:
            point.x = pCodeNode->nX;
            point.y = pCodeNode->nY;



            if( !g_fileSysApi.FS_PtInBoundary(point) )
            {
                return DRAW_NO;
			}
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->bResetIconCarPos  = 0;
			pActionState->bResetRealCarPos  = 0;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
			pActionState->ptNewMapCenter.x	= pCodeNode->nX;
			pActionState->ptNewMapCenter.y	= pCodeNode->nY;
			SetMoveWait();
			break;
		case AC_DRAW_JUMPTOSCREENXY:
			__android_log_print(ANDROID_LOG_INFO,  "navi" ,"AC_DRAW_JUMPTOSCREENXY X %d, Y %d", pCodeNode->nX, pCodeNode->nY);
			if( !m_gDataMgr.PointScreenToBmp(pCodeNode->nX, pCodeNode->nY) )
			{
				return DRAW_NO;
			}
			g_mathtoolApi.MT_BmpToMap(pCodeNode->nX, pCodeNode->nY, &point.x, &point.y);
			/**/
			if( !g_fileSysApi.FS_PtInBoundary(point) )
			{
				return DRAW_NO;
			}
            		pActionState->ptNewMapCenter = point;
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
			pActionState->bResetIconCarPos  = 0;
			pActionState->bResetRealCarPos  = 0;
			g_fileSysApi.pUserCfg->bMoveMap = nuTRUE;
			if( m_nThdState == NAVI_THD_WORK )
			{
				m_DrawDataForUI.bIsMoving = nuTRUE;
				if( m_jumpMove.JumpToPoint(pActionState, pCodeNode->nX, pCodeNode->nY) ) //?????????????????
				{
					m_DrawDataForUI.bIsMoving = nuFALSE;
				}	
			}
			g_fileSysApi.pUserCfg->bMoveMap = nuFALSE;
			SetMoveWait();	
			break;
		case AC_DRAW_ROTATEMAP:
			if( !m_gDataMgr.SetAngle( pCodeNode->nX ) )
			{
				return DRAW_NO;
			}
			pActionState->bSetAngle = 1;
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			SetMoveWait();
			break;
		case AC_DRAW_SETUSERCFG://there is waiting time;
			nres |= UserCfgProc(pCodeNode, pActionState);
			break;
		case AC_DRAW_SETMAPPT: // Added by Damon for Save Map PT
			if (SETMAPPT_NORMAL == pCodeNode->nExtend)
			{
				g_fileSysApi.FS_SavePtFile(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx);
			}
			else if (SETMAPPT_COVERFROMDFT == pCodeNode->nExtend)
			{
				g_fileSysApi.FS_SavePtFileCoverForDFT(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx);
			}
			break;
		case AC_DRAW_BACKTOCAR:
			__android_log_print(ANDROID_LOG_INFO,  "navi" ,"AC_DRAW_BACKTOCAR X %d, Y %d", pCodeNode->nX, pCodeNode->nY);
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
			pActionState->ptNewMapCenter.x	= pCodeNode->nX;
			pActionState->ptNewMapCenter.y	= pCodeNode->nY;
			if( g_fileSysApi.pGdata->timerData.nMoveWaitType == WAIT_TYPE_SIMEND ) 
			{
				CODENODE codeNode = {0};
				codeNode.nActionType = AC_NAVI_NEAREND;
				m_codeNavi.PushNode(&codeNode);
			}
			else
			{
				if( /*g_fileSysApi.pGdata->uiSetData.bNavigation &&*/
					g_fileSysApi.pGdata->drawInfoMap.bCarInRoad && 
					g_fileSysApi.pGdata->timerData.nOldMoveType == AUTO_MOVE_GPS )
				{//nuShowString(nuTEXT("Back Start"));
					pActionState->nDrawMode			= DRAWMODE_GPS;
					pActionState->nSpeed			= g_fileSysApi.pGdata->carInfo.nCarSpeed;
					pActionState->ptNewMapCenter	= g_fileSysApi.pGdata->carInfo.ptCarFixed;
					pActionState->nAngle			= -1;//(short)g_fileSysApi.pGdata->carInfo.nCarAngle;
					pActionState->bResetIconCarPos	= 1;
					pActionState->bResetRealCarPos	= 1;
				}
			}
			CoverFromMoveWait();
			break;
		case AC_DRAW_OPERATEMAPXY:
			if( OP_MAP_ONLYCARPOSITION == pCodeNode->nExtend )
			{
				pActionState->ptNewMapCenter.x	= pCodeNode->nX;
				pActionState->ptNewMapCenter.y	= pCodeNode->nY;
				pActionState->bResetIconCarPos = 1;
				pActionState->bDrawBmp1	= 1;
				pActionState->bDrawInfo	= 1;
			}
			else
			{
				if( pCodeNode->nExtend == OP_MAP_CARPOITION )
				{

					//				g_fileSysApi.pGdata->carInfo.ptCarIcon.x = pCodeNode->nX;
					//				g_fileSysApi.pGdata->carInfo.ptCarIcon.y = pCodeNode->nY;
					pActionState->nDrawMode = DRAWMODE_GPS;
					CoverFromMoveWait();
				}
				else
				{
					SetMoveWait();
				}
				pActionState->bDrawBmp1	= 1;
				pActionState->bDrawInfo	= 1;
				pActionState->bNeedReloadMap	= 1;
				pActionState->bResetMapCenter	= 1;
				pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
				pActionState->ptNewMapCenter.x	= pCodeNode->nX;
				pActionState->ptNewMapCenter.y	= pCodeNode->nY;
			}
			break;
		case AC_DRAW_SHOWMENU:
			pActionState->bShowMap = 1;
			break;
		case AC_DRAW_HIDEMENU:
			pActionState->bDrawInfo	= 1;
			pActionState->bShowMap = 1;
			break;
		case AC_DRAW_SETVOICE:
//			g_fileSysApi.pGdata->timerData.bShowVoiceIcon	= 1;
//			g_fileSysApi.pGdata->timerData.nShowVoiceTimer	= 0;
//			g_fileSysApi.pGdata->timerData.nVoiceNume	= (nuWORD)pCodeNode->nX;
			pActionState->bDrawInfo	= 1;
			pActionState->bShowMap = 1;
			break;
		case AC_DRAW_RESUMETHREAD:
			pActionState->bDrawBmp1	= 1;
			pActionState->bDrawInfo	= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			break;
		case AC_DRAW_JUMPTOGPS:
			break;
		case AC_DRAW_MSGBOX:
			//g_drawIfmApi.IFM_SetState(MOUSE_MSGBOX_OUT, STATE_BUT_DEFAULT);
			pActionState->bShowMap = 1;
			nres = DRAW_YES;
            m_drawToUI.nWaitType    = _WAIT_TYPE_DRAWEND;
			break;
		case AC_DRAW_SHOWHIDEBUT:
			//nres = g_drawIfmApi.IFM_SetState(pCodeNode->nX, (nuBYTE)pCodeNode->nExtend);
			if( nres == STATE_UPDATA_DRAWINFO )
			{	
				pActionState->bDrawInfo	= 1;
			}
			else if( nres == STATE_UPDATA_SHOWMAP )
			{
				pActionState->bShowMap	= 1;
			}
			else
			{
				nres = DRAW_NO;
			}
            m_drawToUI.nWaitType    = _WAIT_TYPE_DRAWEND;
			break;
		case AC_DRAW_SETDRAWINFO:
			//nres = g_drawIfmApi.IFM_SetStateArray((PDFMSETTING)pCodeNode->nExtend);
			m_drawToUI.bUIWaiting	= 0;
			break;
		case AC_DRAW_SPLITSCREEN:
			/*
			if( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_DISABLE ||
				g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_ONLYAUTO ||
				( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_AUTOHAND && 
				  g_fileSysApi.pGdata->uiSetData.nSplitScreenType == SPLIT_SCREEN_AUTO ) )
			{
				return DRAW_NO;
			}
			if( g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_ONE ) 
			{
				g_fileSysApi.pGdata->uiSetData.nSplitScreenType  = SPLIT_SCREEN_NO;
				g_fileSysApi.pGdata->uiSetData.nScreenType		 = SCREEN_TYPE_ONE;
			}
			else
			{
				g_fileSysApi.pGdata->uiSetData.nSplitScreenType	= SPLIT_SCREEN_HAND;
				g_fileSysApi.pGdata->uiSetData.nScreenType		= g_fileSysApi.pUserCfg->nScreenType;
			}
			*/
			if(pCodeNode->nX)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_DEFAULT)
				{
					g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_COVER;
				}
				else
				{
					g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_HIGHWAY;
				}
			}
			if( m_gDataMgr.SplitScreenProc(SPLIT_PROC_TYPE_HAND) == SPLIT_PROC_RES_NO )
			{
				return DRAW_NO;
			}
			pActionState->bResetMemBmpSize  = 1;
			pActionState->bDrawBmp1			= 1;
			pActionState->bDrawInfo			= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->ptNewMapCenter.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
			pActionState->ptNewMapCenter.y  = g_fileSysApi.pGdata->transfData.nMapCenterY;
			nres = DRAW_YES;
			break;
		case AC_DRAW_SETSCALE:
			nres = SetScaleProc(pCodeNode->nExtend, pActionState, nuFALSE);
			break;

		case AC_NAVI_RTFULLVIEW: // Added by Xiaoqin 20120517
			SetMoveWait();
			m_bFullViewMode = nuTRUE;
			if( g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_DEFAULT )
			{
				if( m_gDataMgr.SplitScreenProc(SPLIT_PROC_TYPE_HAND) == SPLIT_PROC_RES_NO )
				{
					return DRAW_NO;
				}
			}
			pActionState->bResetMemBmpSize  = 1;
			pActionState->bDrawBmp1			= 1;
			pActionState->bDrawInfo			= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->ptNewMapCenter.x	= pCodeNode->nX;
			pActionState->ptNewMapCenter.y	= pCodeNode->nY;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
			nres = DRAW_YES;
			break;

		case AC_DRAW_RESETSCREEN: //Added @2012.06.18
			pActionState->bResetMemBmpSize  = 1;
			pActionState->bDrawBmp1			= 1;
			pActionState->bDrawInfo			= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetScreenSize	= 1;
			g_fileSysApi.pGdata->uiSetData.nScreenWidth  = pCodeNode->nX;
			g_fileSysApi.pGdata->uiSetData.nScreenHeight = pCodeNode->nY;
			g_fileSysApi.pUserCfg->nScreenWidth          = pCodeNode->nX;
			g_fileSysApi.pUserCfg->nScreenHeight         = pCodeNode->nY;
			g_drawmapApi.DM_Reset3DModel(nuTRUE, pCodeNode->nX, pCodeNode->nY);
			nres = DRAW_YES;
			break;
		case AC_GPSSET_RESET: //Added 20120705
			if (0 == g_fileSysApi.pGdata->timerData.bResetGPS)
			{
				g_fileSysApi.pGdata->timerData.bResetGPS    = 1;
				g_fileSysApi.pGdata->timerData.nGPSComPort  = pCodeNode->nX;
				g_fileSysApi.pGdata->timerData.nGPSBaudrate = pCodeNode->nY;
				nres = DRAW_NO;
			}
			break;
		case AC_DRAW_POINTOTROAD:
			m_drawToUI.nReturnCode = PointToRoad(pCodeNode->nX, pCodeNode->nY, (nuPVOID)pCodeNode->nExtend);
			m_drawToUI.bUIWaiting = 0;
			nres = DRAW_NO;
			break;
		case AC_DRAW_UI_SAVE_LAST_POSITION:
			nuMemcpy(&TmpGpsData,&m_gpsData, sizeof(GPSDATA));
		    TmpGpsData.nLongitude = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
		    TmpGpsData.nLatitude  = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
		    TmpGpsData.nAngle     = g_fileSysApi.pGdata->carInfo.nCarAngle;
		    g_fileSysApi.FS_SaveLastPos(&TmpGpsData);
			nres = DRAW_NO;
			break;
		case AC_DRAW_DAIWOO_BT:
			nuMemcpy(&m_UI_DAIWOO_BT, &m_UI_SEND_DAIWOO_BT, sizeof(UI_DAIWOO_BT));			
			nres = DRAW_NO;
			break;
		case AC_DRAW_TMC_DISPLAY_MAP:
		case AC_DRAW_TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY:
			TMC_EVENT(pCodeNode->nDrawType, pCodeNode->nExtend, pActionState);
			break;
		case AC_DRAW_TMC_FLUSH_MAP:
			TMC_EVENT(pCodeNode->nDrawType, pCodeNode->nExtend, pActionState);
			break;
		case AC_DRAW_NEARPOI:
			ColNearPoi();
			nres = DRAW_NO;
			m_drawToUI.bUIWaiting = 0;
			break;
		case AC_DRAW_JUMPROAD:
			JumpRoad(pActionState);			
			nres = DRAW_YES;
			m_drawToUI.bUIWaiting = 0;
			break;
		case AC_DRAW_CHOSE_ROAD://Added by Daniel for Choosing Road 20150207
			Choose_Car_Road(pCodeNode->nDrawType);
			nres = DRAW_NO;
			break;
		case AC_DRAW_SET_USER_DATA:			
			if(m_UISetUserConfig.bSetByDefault)
			{
				pCodeNode->nExtend = SETUSERCFG_COVERFROMDFT;				
			}
			else
			{
				NURO_UI_SET_USER_DATA();
				pCodeNode->nExtend = SETUSERCFG_NOEXTEND;
			}
			nres |= UserCfgProc(pCodeNode, pActionState);
			m_drawToUI.bUIWaiting = 0;
			break;
		default:
			return DRAW_NO;
	}
	return nres;
}

nuUINT CNaviThread::UserCfgProc(PCODENODE pCodeNode, PACTIONSTATE pActionState)
{
	if( pActionState == NULL )
	{
		return DRAW_NO;
	}
	nuUINT nres = DRAW_NO;
	nuBOOL bSaveUserCfg = nuFALSE;
	nuBOOL bLoadResident = nuFALSE;
	nuDWORD nRsData = 0;
	if( pCodeNode->nExtend == SETUSERCFG_COVERFROMDFT )
	{
		g_fileSysApi.FS_CoverFromDefault();
		pActionState->bResetMemBmpSize  = 1;
		nres = DRAW_USERSET;
	}
	else if( pCodeNode->nExtend == SETUSERCFG_NOEXTEND )
	{
		bSaveUserCfg = nuTRUE;
		if( g_fileSysApi.pUserCfg == NULL )
		{
			return DRAW_NO;
		}
		if( g_fileSysApi.pGdata->uiSetData.b3DMode != g_fileSysApi.pUserCfg->b3DMode )
		{
			g_fileSysApi.pGdata->uiSetData.b3DMode = g_fileSysApi.pUserCfg->b3DMode;
			nRsData |= RES_DATA_3DMODE_CHANGE;
			nres = DRAW_USERSET;
		}
		if( g_fileSysApi.pGdata->uiSetData.nLanguage != g_fileSysApi.pUserCfg->nLanguage )
		{
			g_fileSysApi.pGdata->uiSetData.nLanguage = g_fileSysApi.pUserCfg->nLanguage;
			nres = DRAW_USERSET;
		}
		if( g_fileSysApi.pGdata->uiSetData.bNightOrDay != g_fileSysApi.pUserCfg->bNightDay )
		{
			g_fileSysApi.pGdata->uiSetData.bNightOrDay = g_fileSysApi.pUserCfg->bNightDay;
			g_fileSysApi.FS_ReLoadMapConfig();
			nRsData |= RES_DATA_DAYNIGHT_CHANGE;
			nres = DRAW_USERSET;
			pActionState->bReDrawCrossZoom = 1;
		}
		if( g_fileSysApi.pGdata->uiSetData.bNightOrDay )
		{
			if( g_fileSysApi.pGdata->uiSetData.nBgPic != g_fileSysApi.pUserCfg->nNightBgPic )
			{
				g_fileSysApi.pGdata->uiSetData.nBgPic = g_fileSysApi.pUserCfg->nNightBgPic;
				nRsData |= RES_DATA_DAYNIGHT_CHANGE;
				nres = DRAW_USERSET;
				pActionState->bReDrawCrossZoom = 1;
			}
		}
		else
		{
			if( g_fileSysApi.pGdata->uiSetData.nBgPic != g_fileSysApi.pUserCfg->nDayBgPic )
			{
				g_fileSysApi.pGdata->uiSetData.nBgPic = g_fileSysApi.pUserCfg->nDayBgPic;
				nRsData |= RES_DATA_DAYNIGHT_CHANGE;
				nres = DRAW_USERSET;
				pActionState->bReDrawCrossZoom = 1;
			}
		}
		///////////////////////////////////////////
		if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode != g_fileSysApi.pUserCfg->nMapDirectMode )
		{
			g_fileSysApi.pGdata->uiSetData.nMapDirectMode = g_fileSysApi.pUserCfg->nMapDirectMode;
			if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
			{
				m_gDataMgr.SetAngle(360);
			}
			else
			{
				m_gDataMgr.SetAngle(0);
			}
			pActionState->bSetAngle = 1;
			nres = DRAW_USERSET;
		}
		if( g_fileSysApi.pGdata->uiSetData.nCarIconType != g_fileSysApi.pUserCfg->nCarIconType )
		{
			g_fileSysApi.pGdata->uiSetData.nCarIconType = g_fileSysApi.pUserCfg->nCarIconType;
			nres = DRAW_USERSET;
		}
		if( g_fileSysApi.pGdata->uiSetData.nShowLongLat != g_fileSysApi.pUserCfg->bShowLongLat )
		{
			g_fileSysApi.pGdata->uiSetData.nShowLongLat = g_fileSysApi.pUserCfg->bShowLongLat;
			nres = DRAW_USERSET;
		}
		if( g_fileSysApi.pGdata->routeGlobal.RoutingRule != g_fileSysApi.pUserCfg->nRouteMode )
		{
			g_fileSysApi.pGdata->routeGlobal.RoutingRule = g_fileSysApi.pUserCfg->nRouteMode;
		}
		if( g_fileSysApi.pGdata->uiSetData.nTTSWaveMode != g_fileSysApi.pUserCfg->nWaveTTSType )
		{
			g_fileSysApi.pGdata->uiSetData.nTTSWaveMode = g_fileSysApi.pUserCfg->nWaveTTSType;
		}
	}
	else if( pCodeNode->nExtend == SETUSERCFG_ZOOMIN )//Zoom in
	{
		if( g_fileSysApi.pGdata->uiSetData.nScaleIdx == 1 )
		{
			return DRAW_NO;


		}
		return SetScaleProc(g_fileSysApi.pGdata->uiSetData.nScaleIdx-1, pActionState);
	}
	else if( pCodeNode->nExtend == SETUSERCFG_ZOOMOUT )//Zoom out
	{
		if( g_fileSysApi.pGdata->uiSetData.nScaleIdx == g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount - 1 )
		{
			return DRAW_NO;
		}
		return SetScaleProc(g_fileSysApi.pGdata->uiSetData.nScaleIdx+1, pActionState);
	}
	else
	{
		return DRAW_NO;
	}
	if(g_fileSysApi.pUserCfg->bBigMapRoadFont != m_nMapRoadFontSize)
	{
	    m_nMapRoadFontSize = g_fileSysApi.pUserCfg->bBigMapRoadFont;
	    nres = DRAW_USERSET;
	    bSaveUserCfg = nuTRUE;
	}
	if( nres )
	{
		pActionState->bDrawBmp1	= 1;
		pActionState->bDrawInfo	= 1;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->bResetScale		= 1;
		pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
		pActionState->ptNewMapCenter.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
		pActionState->ptNewMapCenter.y	= g_fileSysApi.pGdata->transfData.nMapCenterY;
	}
	if( bSaveUserCfg )
	{
		g_fileSysApi.FS_SaveUserConfig();
	}
	g_fileSysApi.FS_LoadResidentData(nRsData);
	//g_drawmapApi.DM_CheckSkyBmp();
	return nres;
}

nuUINT CNaviThread::CodeNaviProc(PCODENODE pCodeNode, PACTIONSTATE pActionState)
{
	nuUINT nRes = DRAW_NO;
	nuLONG nResRoute		= ROUTE_ERROR;
	nuLONG RouteRule[3] = {0};
	TARTGETINFO TargetInfo = {0};
	CODENODE codeNode = *pCodeNode;
	PTMC_ROUTER_DATA pTMCRTData = NULL;//added by daniel 20110830
	CALL_BACK_PARAM cbParam = {0};
	NUROPOINT TargetPt = {0};
	switch( pCodeNode->nActionType )
	{
		case AC_NAVI_STARTROUTE:
			if( g_fileSysApi.pGdata->routeGlobal.TargetCount < 2 )
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "g_fileSysApi.pGdata->routeGlobal.TargetCount < 2");
				m_drawToUI.bUIWaiting	= 0;
				return DRAW_NO;
			}
			CoverFromMoveWait();
			g_fileSysApi.pGdata->timerData.nAutoMoveType = (nuBYTE)pCodeNode->nExtend;
			g_fileSysApi.pUserCfg->nLastAutoMoveType	= (nuBYTE)g_fileSysApi.pGdata->timerData.nAutoMoveType;
			if( g_fileSysApi.pGdata->uiSetData.bNavigation )
			{
				// Modified by Damon 20111122
				g_pSoundThread->CleanSound();

				if( m_nPlayedStartNaviState == PLAY_NAVISTART_NO )
				{
					g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_NAVISTART, 0, NULL);
					m_nPlayedStartNaviState = PLAY_NAVISTART_PLAYED;
				}
				if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS )
				{
					g_fileSysApi.pGdata->carInfo.ptCarIcon = g_fileSysApi.pGdata->carInfo.ptCarFixed;
					g_fileSysApi.pGdata->carInfo.nCarAngle = g_fileSysApi.pGdata->carInfo.nCarAngleRaw;
					g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_COVER;
					g_fileSysApi.pGdata->drawInfoNavi.nCrossZoomType = CROSSZOOM_TYPE_DEFAULT;
					pActionState->bResetIconCarPos = 1;

				}
				//else if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
				{
					SIMULATIONDATA simuData;
					g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData);
					g_innaviApi.IN_ColNaviInfo();
				}
				g_fileSysApi.FS_SaveUserConfig();
				m_drawToUI.bUIWaiting	= 0;
				//g_pSoundThread->CleanSound(); // Modified by Damon 20111122
				if(m_bFullViewMode)
				{
					m_gDataMgr.SplitScreenProc();
					g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = g_innaviApi.IN_JudgeZoomRoad();
				}
				m_bFullViewMode = nuFALSE;
				if( g_fileSysApi.pUserCfg->b3DMode)
				{
					g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_CAR;
				}
				return DRAW_NO;
			}
			g_fileSysApi.pGdata->routeGlobal.CarSpeed	= g_fileSysApi.pGdata->carInfo.nCarSpeed;
			g_fileSysApi.pGdata->routeGlobal.nCarAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
			//
			g_fileSysApi.pGdata->timerData.bIsRouting	= 1;
			g_fileSysApi.pGdata->timerData.nRouteType   = 0;
			g_fileSysApi.pGdata->timerData.nRouteTimer  = ROUTE_ANIMATION_SLICE;

			g_drawmapApi.DM_FreeDrawData(DRAW_MAP_FREE_BGL | DRAW_MAP_FREE_SEA);
			g_drawmapApi.DM_FreeDataMem();
			g_memMgrApi.MM_CollectDataMem(2);
			//
			//2011.04.12
			RouteRule[0] = g_fileSysApi.pGdata->routeGlobal.RoutingRule;
			g_routeApi.RT_SetRouteRule(RouteRule, 1);
			m_ptEndPt = g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount - 1].BaseCoor;
			nResRoute = g_routeApi.RT_StartRoute(&g_fileSysApi.pGdata->routeGlobal.routingData);
			__android_log_print(ANDROID_LOG_INFO, "Route", "NaviThread!!!nResRoute.....%d",nResRoute);
			g_innaviApi.IN_NaviStart();
			g_fileSysApi.pGdata->uiSetData.nLastError = nResRoute;
			g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
			//
			if( nResRoute != ROUTE_SUCESS )
			{
				__android_log_print(ANDROID_LOG_INFO, "troute", "ROUTE_FAIL!!!!!");
				nRes = StopNavi(pActionState);
				m_bRouteFirst = nuFALSE;//added by daniel
			}
			else
			{
				__android_log_print(ANDROID_LOG_INFO, "troute", "ROUTE_SUCESS!!!!!");
				m_bRouteEnd = nuFALSE;
				m_bRouteFirst = nuTRUE;//added by daniel
				g_fileSysApi.pGdata->uiSetData.bNavigation = 1;
				SIMULATIONDATA simuData;
				if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
				{
					pActionState->ptNewMapCenter.x = simuData.nX;
					pActionState->ptNewMapCenter.y = simuData.nY;
					if( pCodeNode->nExtend == AUTO_MOVE_SIMULATION )
					{
						pActionState->nAngle = (short)simuData.nAngle;
					}
					else
					{
						pActionState->nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
					}
					pActionState->bResetIconCarPos = 1;
					pActionState->nCarCenterMode   = CAR_CENTER_MODE_THREE_QUARTERS_Y;
					pActionState->nDrawMode		   = DRAWMODE_SIM_START;
				}
				nRes = DRAW_YES;
				if( m_nPlayedStartNaviState == PLAY_NAVISTART_NO )
				{
					g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_NAVISTART, 0, NULL);
					m_nPlayedStartNaviState = PLAY_NAVISTART_PLAYED;
				}
				if( pCodeNode->nExtend != AUTO_MOVE_STOP)
				{
					m_nPlayedStartNaviState = PLAY_NAVISTART_NEEDPLAY;
				}
				if( g_fileSysApi.pGdata->uiSetData.bNavigation )
				{
					#ifdef VALUE_EMGRT
					//m_gDataMgr.SpeedAutoZoom(pActionState,3 );
					#endif
					
					g_fileSysApi.pUserCfg->bLastNavigation		= 1;
					g_fileSysApi.pUserCfg->nPastMarket			= 0;
					g_fileSysApi.pUserCfg->nLastAutoMoveType	= (nuBYTE)g_fileSysApi.pGdata->timerData.nAutoMoveType;
					g_fileSysApi.FS_SaveUserConfig();
				}
				g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_COVER;
				g_fileSysApi.pGdata->drawInfoNavi.nCrossZoomType = CROSSZOOM_TYPE_DEFAULT;
				/*if(g_fileSysApi.pGdata->pfNaviThreadCallBack != NULL)
				{
					TargetPt.x = g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount - 1].FixCoor.x;
					TargetPt.y = g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount - 1].FixCoor.y;
					cbParam.nCallType = CALLBACK_TYPE_EWHERE;
					cbParam.pVOID     = &TargetPt;
					g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
				}*/
			}
			g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
			m_drawToUI.bUIWaiting	= 0;
			m_bFullViewMode = nuFALSE;
			//g_innaviApi.IN_FreeTMCNaviPathData();
			break;
		case AC_NAVI_IFSTOP:
			//g_drawIfmApi.IFM_SetState(MOUSE_MSGBOX_OUT, STATE_BUT_DEFAULT);
			nRes = DRAW_YES;
			break;
		case AC_NAVI_STOPROUTE:
			if(!m_bRouteEnd)
			{
				g_pSoundThread->CleanSound();
			}
			DelMarket();
			//g_innaviApi.IN_FreeTMCNaviPathData();
			nRes = StopNavi(pActionState);
			m_bRouteEnd = nuFALSE;
			if ( g_fileSysApi.pGdata->carInfo.ptCarIcon.x != g_fileSysApi.pGdata->carInfo.ptCarFixed.x	&& 
				 g_fileSysApi.pGdata->carInfo.ptCarIcon.y != g_fileSysApi.pGdata->carInfo.ptCarFixed.y	&&
				 g_fileSysApi.pGdata->carInfo.ptCarIcon.x != g_fileSysApi.pGdata->carInfo.ptCarRaw.x	&&
				 g_fileSysApi.pGdata->carInfo.ptCarIcon.y != g_fileSysApi.pGdata->carInfo.ptCarRaw.y )
			{
				g_fileSysApi.pGdata->carInfo.ptCarIcon = g_fileSysApi.pGdata->carInfo.ptCarFixed;//20081211
				g_fileSysApi.pGdata->carInfo.nCarAngle = g_fileSysApi.pGdata->carInfo.nCarAngleRaw;//added by daniel 20120131
				SetMoveWait(WAIT_TYPE_MOVE);
				pActionState->nCarCenterMode   = CAR_CENTER_MODE_ONE_HALF_XY;
			}
			if( nRes )
			{
				g_fileSysApi.pUserCfg->bLastNavigation  = 0;
				g_fileSysApi.pUserCfg->nPastMarket		= 0;
				g_fileSysApi.FS_SaveUserConfig();
			}
			m_drawToUI.bUIWaiting	= 0;
			g_fileSysApi.pUserCfg->bLastNavigation = g_fileSysApi.pGdata->uiSetData.bNavigation;
//			pActionState->nDrawMode = DRAWMODE_SIM_START
			break;
		case AC_NAVI_REROUTE:
			g_pSoundThread->CleanSound();
			if( g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_NAVIERROR )
			{
				#ifdef VALUE_EMGRT
					//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 20, 0, 0, 0, NULL);
				#else
				//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_REROUTE, 0, NULL);
				#endif
				
			}
			#ifdef VALUE_EMGRT
			g_fileSysApi.pGdata->timerData.bIsRouting	= 1;
			g_fileSysApi.pGdata->timerData.nRouteType   = 0;
			#else
			g_fileSysApi.pGdata->timerData.bIsRouting	= 1;
			g_fileSysApi.pGdata->timerData.nRouteType   = 0;
			g_fileSysApi.pGdata->timerData.nRouteTimer  = ROUTE_ANIMATION_SLICE;
			#endif
			g_fileSysApi.pGdata->routeGlobal.nCarAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
			g_fileSysApi.pGdata->routeGlobal.CarSpeed	= g_fileSysApi.pGdata->carInfo.nCarSpeed;
			TargetInfo.BaseCoor	= g_fileSysApi.pGdata->carInfo.ptCarRaw;
//			if( g_fileSysApi.pGdata->drawInfoMap.bCarInRoad )
			{
				TargetInfo.FixCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
				TargetInfo.MapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
				TargetInfo.BlockIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
				TargetInfo.RoadIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
			}
			/*
			else
			{
				targetInfo.FixCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
				targetInfo.MapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
				targetInfo.BlockIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
				targetInfo.RoadIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			}*/
			g_drawmapApi.DM_FreeDrawData(DRAW_MAP_FREE_BGL | DRAW_MAP_FREE_SEA);
			g_memMgrApi.MM_CollectDataMem(1);
			nResRoute = g_routeApi.RT_ReRoute(&g_fileSysApi.pGdata->routeGlobal.routingData, TargetInfo);
			g_innaviApi.IN_NaviStart();
			g_fileSysApi.pGdata->uiSetData.nLastError = nResRoute;
			//
			#ifdef VALUE_EMGRT
				g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
				//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 21, 0, 0, 0, NULL);
			#else
				g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
			#endif
			if( nResRoute != ROUTE_SUCESS )
			{
				
				CODENODE codeNode = *pCodeNode;
				codeNode.nActionType = AC_NAVI_STOPROUTE;
				nRes = CodeNaviProc(&codeNode, pActionState);
				/*
				return DRAW_NO;*/
			}
			else
			{
				SIMULATIONDATA simuData;
				if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
				{
					pActionState->ptNewMapCenter.x = g_fileSysApi.pGdata->carInfo.ptCarRaw.x;//simuData.nX;
					pActionState->ptNewMapCenter.y = g_fileSysApi.pGdata->carInfo.ptCarRaw.y;//simuData.nY;
					pActionState->nAngle = (short)g_fileSysApi.pGdata->carInfo.nCarAngle;
				}
				g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_DEFAULT;
			}
			m_bRouteEnd = nuFALSE;
			pActionState->bResetIconCarPos	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
			pActionState->nDrawMode			= DRAWMODE_SIM_START;
			nRes = DRAW_YES;
			g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
			//g_innaviApi.IN_FreeTMCNaviPathData();
			break;
		case AC_NAVI_SETMARKETPT://???o?????
			nRes =  SetMarket();
			if( nRes )
			{
				pActionState->ptNewMapCenter.x = m_pMarketPoint->x;
				pActionState->ptNewMapCenter.y = m_pMarketPoint->y;
				pActionState->nCarCenterMode = CAR_CENTER_MODE_ONE_HALF_XY;
			}
			m_pMarketPoint->bIsSetting = 0;
			break;
		case AC_NAVI_DELMARKETPT:
			nRes = DelMarket();
			__android_log_print(ANDROID_LOG_INFO, "navi", "AC_NAVI_DELMARKETPT 2");
			m_pMarketPoint->bIsSetting = 0;
			if ( nRes )
			{
//				pActionState->ptNewMapCenter.x = m_pMarketPoint->x;
//				pActionState->ptNewMapCenter.y = m_pMarketPoint->y;
				pActionState->nCarCenterMode = CAR_CENTER_MODE_ONE_HALF_XY;
			}
			break;
		case AC_NAVI_SETNAVIMODE:
			g_fileSysApi.pGdata->timerData.nOldMoveType	= g_fileSysApi.pGdata->timerData.nAutoMoveType;
			g_fileSysApi.pGdata->timerData.nAutoMoveType	= (nuBYTE)pCodeNode->nX;
			return DRAW_NO;
		case AC_NAVI_PAUSE:
			g_fileSysApi.pGdata->timerData.nOldMoveType	= g_fileSysApi.pGdata->timerData.nAutoMoveType;
			g_fileSysApi.pGdata->timerData.nAutoMoveType	= AUTO_MOVE_STOP;
			return DRAW_NO;
		case AC_NAVI_CONTINUE:
			g_fileSysApi.pGdata->timerData.nAutoMoveType	= g_fileSysApi.pGdata->timerData.nOldMoveType;
			return DRAW_NO;
		case AC_NAVI_NEAREND:
			if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
			{
				SIMULATIONDATA simuData;
				if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
				{
					pActionState->ptNewMapCenter.x	= simuData.nX;
					pActionState->ptNewMapCenter.y	= simuData.nY;
					pActionState->nAngle = (short)simuData.nAngle;
					pActionState->bResetIconCarPos	= 1;
					pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
					pActionState->nDrawMode			= DRAWMODE_SIM_START;
					nRes = DRAW_YES;
				}
				if( g_fileSysApi.pUserCfg->nSimuMode )
				{
					g_fileSysApi.pGdata->timerData.nAutoMoveType = AUTO_MOVE_GPS;
					//
//					pActionState->ptNewMapCenter.x = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
//					pActionState->ptNewMapCenter.y = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
					pActionState->nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
					//g_drawIfmApi.IFM_SetState(MOUSE_MENU_NAVIINFOPANA, STATE_BUT_DEFAULT);
//					g_fileSysApi.pUserCfg->nPastMarket		= 0;
					g_fileSysApi.pUserCfg->nLastAutoMoveType = (nuBYTE)g_fileSysApi.pGdata->timerData.nAutoMoveType;
					g_fileSysApi.FS_SaveUserConfig();
//					nRes = DRAW_YES;
				}
			}
			else
			{
				CODENODE codeNode = *pCodeNode;
				codeNode.nActionType = AC_NAVI_STOPROUTE;
				nRes = CodeNaviProc(&codeNode, pActionState);
			}
			break;
		case AC_NAVI_CONFIGROUTE:
			g_pSoundThread->CleanSound();
			g_fileSysApi.pGdata->timerData.bIsRouting	= 1;
			g_fileSysApi.pGdata->timerData.nRouteType   = 0;
			TargetInfo.BaseCoor	= g_fileSysApi.pGdata->carInfo.ptCarRaw;
			TargetInfo.FixCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
			TargetInfo.MapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
			TargetInfo.BlockIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
			TargetInfo.RoadIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			g_memMgrApi.MM_CollectDataMem(1);
			nResRoute = g_routeApi.RT_CongestionRoute(&g_fileSysApi.pGdata->routeGlobal.routingData, TargetInfo, pCodeNode->nExtend);
			g_innaviApi.IN_NaviStart();
			g_fileSysApi.pGdata->uiSetData.nLastError = nResRoute;
			//
			if( nResRoute != ROUTE_SUCESS )
			{
				codeNode.nActionType = AC_NAVI_STOPROUTE;
				nRes = CodeNaviProc(&codeNode, pActionState);
			}
			else
			{
				SIMULATIONDATA simuData;
				if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
				{
					pActionState->ptNewMapCenter.x = simuData.nX;
					pActionState->ptNewMapCenter.y = simuData.nY;
					pActionState->nAngle = (short)simuData.nAngle;
				}
				g_fileSysApi.pGdata->uiSetData.bNavigation	= 1;
				g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_DEFAULT;
			}
			pActionState->bResetIconCarPos	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
			pActionState->nDrawMode			= DRAWMODE_SIM_START;
			nRes = DRAW_YES;
			g_fileSysApi.pGdata->timerData.bIsRouting	= 0;
			m_drawToUI.bUIWaiting	= 0;
			break;
		case AC_NAVI_TMC_ROUTE://added by daniel 20110830
		    	g_pSoundThread->CleanSound();
			//nuSleep(100);
			TMC_EVENT(pCodeNode->nDrawType, pCodeNode->nExtend, pActionState);
			
		/*	if( g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_NAVIERROR )
			{
			#ifdef VALUE_EMGRT
				g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 20, 0, 0, 0, NULL);
			#else
				g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_REROUTE, 0, NULL);
			#endif
			}	
			targetInfo.BaseCoor	= g_fileSysApi.pGdata->carInfo.ptCarRaw;
			targetInfo.FixCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
			targetInfo.MapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
			targetInfo.BlockIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
			targetInfo.RoadIdx	= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			pTMCRTData = new TMC_ROUTER_DATA[m_nTMCDataCount];
			if(pTMCRTData != NULL)
			{
				g_fileSysApi.FS_Get_TMC_Router(pTMCRTData, 0);
				g_innaviApi.IN_TMCRoute(pTMCRTData, m_nTMCDataCount, targetInfo, g_fileSysApi.pGdata->routeGlobal.RoutingRule);
				delete []pTMCRTData;
				pTMCRTData = NULL;
			}
			m_drawToUI.bUIWaiting = 0;*/
			break;
		case AC_GPSSET_TOBERESETGYRO: // Added by Damon 20110906
			g_gpsopenApi.GPS_TOBERESETGYRO();
			break;

		case AC_GPSSET_TOBEGETVERSION: // Added by Louis 20111110
			g_gpsopenApi.GPS_TOBEGETVERSION();
			break;
		case AC_DRAW_GET_TMC_NAVI_LIST_D:
			if( g_fileSysApi.pGdata->uiSetData.bNavigation )
			{
				g_innaviApi.IN_FreeTMCNaviPathData();
				g_innaviApi.IN_TMCNaviPathData();		
				__android_log_print(ANDROID_LOG_INFO, "TMC", "IN_TMCNaviPathData \n");
			}
			m_TMCForoUI.bUIWaiting = 0;
			break;
		default:
			return DRAW_NO;
	}
	if( nRes )
	{
		pActionState->bDrawBmp1	= 1;
		pActionState->bDrawInfo	= 1;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->bResetScale		= 1;
		pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
	}
	return nRes;
}

nuUINT CNaviThread::AutoMoveProc(PACTIONSTATE pActionState)
{
	nuUINT nRes = DRAW_NO;
	if(g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION)
	{
		m_dSimTick = 0;
	}
	if( !m_bQuit )
	{
		switch( g_fileSysApi.pGdata->timerData.nAutoMoveType )
		{
		case AUTO_MOVE_GPS:
			if( g_fileSysApi.pGdata->timerData.bGpsMode )
			{
				nRes = GetGpsDataEx(pActionState);
				g_fileSysApi.pGdata->timerData.bGpsMode = 0;
				if( g_fileSysApi.pUserCfg->nZoomAutoMode && DRAW_GPS == nRes )
				{
					if (m_nAutoScaleTime < NURO_ABS(nuGetTickCount() - m_nAutoScaleTick))  // Added by Damon 20120525
					{
						m_gDataMgr.SpeedAutoZoom(pActionState);
					}
				}
			}
			break;
		case AUTO_MOVE_SIMULATION:
			if( g_fileSysApi.pGdata->timerData.bSimMode )
			{
				nuSleep(MAX_SIM_SPACE_OF_TIME);
				SIMULATIONDATA simuData = {0};
				nuDOUBLE lTick = nuGetTickCount();
				nuDOUBLE nDis = g_fileSysApi.pUserCfg->nSimSpeed * 1000 / 3600;
				if(m_dSimTick != 0)
				{	
					nDis *= ((lTick - m_dSimTick)/1000);
				}
				m_dSimTick = lTick;
				if( g_innaviApi.IN_GetSimuData(nuFALSE, (nuLONG)nDis, &simuData) )
				{
					pActionState->ptNewMapCenter.x	= simuData.nX;
					pActionState->ptNewMapCenter.y	= simuData.nY;
					pActionState->nAngle			= (nuSHORT)simuData.nAngle;
					pActionState->bResetIconCarPos	= 1;
					pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
					pActionState->nDrawMode			= DRAWMODE_SIM;
					pActionState->bDrawBmp1			= 1;
					pActionState->bDrawInfo			= 1;

					pActionState->bNeedReloadMap	= 1;
					pActionState->bResetMapCenter	= 1;
					pActionState->bResetScale		= 1;
//					pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
					pActionState->nSpeed			= g_fileSysApi.pUserCfg->nSimSpeed;
					nRes	= DRAW_YES;
				}
				if( g_fileSysApi.pUserCfg->nZoomAutoMode )
				{
					if (m_nAutoScaleTime < NURO_ABS(nuGetTickCount() - m_nAutoScaleTick))  // Added by Damon 20120525
					{
						m_gDataMgr.SpeedAutoZoom(pActionState);
					}
				}
				g_fileSysApi.pGdata->timerData.bSimMode = 0;
				//						g_fileSysApi.pGdata->timerData.nSimTime = 0;
			}
			break;
		case AUTO_MOVE_TIMER:
			if( g_fileSysApi.pGdata->timerData.nOldMoveType == AUTO_MOVE_GPS )
			{
				ACTIONSTATE acState;
				acState.nSpeed = pActionState->nSpeed;
				if( GetGpsData(&acState) )
				{
					SEEKEDROADDATA roadData;
					if( g_fileSysApi.FS_SeekForRoad( acState.ptNewMapCenter.x, 
									 acState.ptNewMapCenter.y, 
									 m_nMaxRoadFixDis, 
									 acState.nAngle, 
									 &roadData ) )
					{
						g_fileSysApi.pGdata->drawInfoMap.bCarInRoad = 1;
						g_fileSysApi.pGdata->drawInfoMap.roadCarOn = roadData;
						g_fileSysApi.pGdata->carInfo.ptCarRaw = acState.ptNewMapCenter;
						g_fileSysApi.pGdata->carInfo.ptCarFixed = /*g_fileSysApi.pGdata->carInfo.ptCarIcon = */
							roadData.ptMapped;
//						pActionState->ptNewMapCenter = roadData.ptMapped;
					}
					else
					{
						g_fileSysApi.pGdata->drawInfoMap.bCarInRoad = 0;
						g_fileSysApi.pGdata->carInfo.ptCarRaw = 
							g_fileSysApi.pGdata->carInfo.ptCarFixed = 
							/*g_fileSysApi.pGdata->carInfo.ptCarIcon = */acState.ptNewMapCenter;
//						pActionState->ptNewMapCenter = acState.ptNewMapCenter;
					}
					g_fileSysApi.pGdata->carInfo.nCarAngle = acState.nAngle;
//					pActionState->bResetIconCarPos = 1;
					pActionState->bDrawInfo = 1;
					nRes = DRAW_YES;
				}
				pActionState->nSpeed = acState.nSpeed;
			}
			//
			if( g_fileSysApi.pGdata->timerData.nMoveWaitType == WAIT_TYPE_SIMEND )
			{
				if( g_fileSysApi.pGdata->timerData.nMoveWaitTimer > MAX_SIM_END_WAITING )
				{
					CODENODE codeNode = {0};
					codeNode.nActionType = AC_NAVI_NEAREND;
					m_codeNavi.PushNode(&codeNode);
					CoverFromMoveWait();
				}
			}
			else if( g_fileSysApi.pGdata->timerData.nMoveWaitType == WAIT_TYPE_MOVE )
			{
				if( !g_fileSysApi.pUserCfg->bMoveWaiting )
				{
					g_fileSysApi.pGdata->timerData.nMoveWaitTimer = 0;
				}
				else
				{
					if( g_fileSysApi.pGdata->timerData.nMoveWaitTimer/1000 > g_fileSysApi.pUserCfg->nMoveWaitTotalTime )
					{
						CoverFromMoveWait();
						//if( g_fileSysApi.pGdata->uiSetData.bNavigation )
						{
							pActionState->bDrawBmp1	= 1;
							pActionState->bDrawInfo	= 1;
							pActionState->bNeedReloadMap	= 1;
							pActionState->bResetMapCenter	= 1;
							pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
							pActionState->ptNewMapCenter	= g_fileSysApi.pGdata->carInfo.ptCarIcon;
							nRes	= DRAW_YES;
						}
					}
				}
			}
			break;
        case AUTO_MOVE_DEMO3D:
            {
				DEMO_3D_DATA demo3D = {0};
                if( m_demo3D.Demo3D(demo3D) )
                {
                    pActionState->ptNewMapCenter	= demo3D.point;
//                     pActionState->ptNewMapCenter.x  = 10;
//                     pActionState->ptNewMapCenter.y  = 10;
                    pActionState->nAngle			= (short)(demo3D.nAngle % 360);
//                    pActionState->bResetIconCarPos	= 1;
                    pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
                    pActionState->nDrawMode			= DRAWMODE_SIM;
                    pActionState->bDrawBmp1			= 1;
                    pActionState->bDrawInfo			= 1;
                    pActionState->bNeedReloadMap	= 1;
                    pActionState->bResetMapCenter	= 1;
                    pActionState->bResetScale		= 1;
                    pActionState->nSpeed			= 0;
					nRes	                        = DRAW_YES;
                }
            }
            break;
		default:
			break;
		}
	}
	return nRes;
}

nuUINT CNaviThread::MouseProc(PCODENODE pCodeNode, PACTIONSTATE pActionState)
{
	return 0;
/*	NUROPOINT point;
	nuUINT nRes = DRAW_NO;
	point.x = pCodeNode->nX;
	point.y = pCodeNode->nY;

	//nRes = g_drawIfmApi.IFM_MouseProc(point, (nuBYTE)pCodeNode->nExtend, &m_drawToUI.nReturnCode);
//???	g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = g_innaviApi.IN_JudgeZoomRoad();
	if( nRes == STATE_UPDATA_DRAWINFO )
	{
		pActionState->bDrawInfo	= 1;
		nRes = DRAW_YES;
	}
	else if( nRes == STATE_UPDATA_SHOWMAP )
	{
		pActionState->bShowMap	= 1;
		nRes = DRAW_YES;
	}
	else// if( nRes == STATE_UPDATA_NOCHANGE )
	{
		nRes = DRAW_NO;
	}
	
	else
	{
		m_drawToUI.nReturnCode	= g_drawIfmApi.IFM_ItemsJudge(point, (nuBYTE)pCodeNode->nExtend);
	}*/
/*	m_drawToUI.bUIWaiting	= 0;
	return nRes;
*/
}

nuUINT CNaviThread::GetGpsData(PACTIONSTATE pActionState)
{
	nuUINT nRes = DRAW_NO;
	if( m_bGetNewGpsData && m_gpsData.nStatus )
	{
		m_bGetNewGpsData = nuFALSE;
		nuroPOINT point = {0};
		point.x = m_gpsData.nLongitude;
		point.y = m_gpsData.nLatitude;
		if( !g_fileSysApi.FS_PtInBoundary(point) )
		{
			return DRAW_NO;
		}
		pActionState->nDrawMode	= DRAWMODE_GPS;
		pActionState->nSpeed	= m_gpsData.nSpeed;
		short nAngleDif = m_gpsData.nAngle - g_fileSysApi.pGdata->carInfo.nCarAngle;
		while(nAngleDif < 0)
		{
			nAngleDif += 360;
		}
		nAngleDif %= 360;
		if( nAngleDif > 180 )
		{
			nAngleDif = 360 - nAngleDif;
		}
		nuLONG nDis = NURO_ABS(m_gpsData.nLongitude - g_fileSysApi.pGdata->carInfo.ptCarRaw.x) +
			NURO_ABS(m_gpsData.nLatitude - g_fileSysApi.pGdata->carInfo.ptCarRaw.y);
		if( (m_gpsData.nSpeed > 20 && nDis > 5)	  || 
			(m_gpsData.nSpeed <= 20 && m_gpsData.nSpeed > 5 && nDis > m_nMinMoveDis)  || 
			(m_gpsData.nSpeed <= 5 && nDis > 80 ) ||
			(m_nMinMoveAngle != 0 && m_nMinMoveAngle <= nAngleDif) )
		{
			pActionState->ptNewMapCenter.x	= m_gpsData.nLongitude;
			pActionState->ptNewMapCenter.y	= m_gpsData.nLatitude;
			pActionState->nAngle = (short)m_gpsData.nAngle;
			pActionState->bResetIconCarPos	= 1;
			pActionState->bResetRealCarPos	= 1;
			pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
			pActionState->bDrawBmp1			= 1;
			pActionState->bDrawInfo			= 1;
			pActionState->bNeedReloadMap	= 1;
			pActionState->bResetMapCenter	= 1;
			pActionState->bResetScale		= 1;
			pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
			nRes =  DRAW_YES;
		}
		//Save lastposition
		if(2 == m_nGPSMode)
		{
			m_nSvLtPosTimer ++;
			if( m_gpsData.nSpeed >= 20 && 
				( m_nSvLtPosTimer >= 10 || (m_gpsData.nSpeed <= 30 && m_nSvLtPosTimer >= 5) ) && 
				nDis < 500  && nDis > m_nMinMoveDis )
			{
				g_fileSysApi.FS_SaveLastPos(&m_gpsData);//Keep Old Position
				m_nSvLtPosTimer = 0;
			}
		}
	}
	return nRes;
}

nuUINT CNaviThread::GetGpsDataEx(PACTIONSTATE pActionState)
{

	nuUINT nRes = DRAW_NO;
	nuBOOL bNewGpsUpdate = nuFALSE;
	if(2 != m_nGPSMode)
	{
		pActionState->nSpeed = m_gpsData.nSpeed;
	}
	
	if( m_bGetNewGpsData && m_gpsData.nStatus )
	{
		m_bGetNewGpsData = nuFALSE;
		nuroPOINT point  = {0};
		point.x = m_gpsData.nLongitude;
		point.y = m_gpsData.nLatitude;
		if( !g_fileSysApi.FS_PtInBoundary(point) )
		{
			return DRAW_NO;
		}
		pActionState->nDrawMode	= DRAWMODE_GPS;
		pActionState->nSpeed	= m_gpsData.nSpeed;
		nuSHORT nAngleDif = m_gpsData.nAngle - g_fileSysApi.pGdata->carInfo.nCarAngle;
		while(nAngleDif < 0)
		{
			nAngleDif += 360;
		}
		nAngleDif %= 360;
		if( nAngleDif > 180 )
		{
			nAngleDif = 360 - nAngleDif;
		}
		nuLONG nDis = NURO_ABS(m_gpsData.nLongitude - g_fileSysApi.pGdata->carInfo.ptCarRaw.x) +
			NURO_ABS(m_gpsData.nLatitude - g_fileSysApi.pGdata->carInfo.ptCarRaw.y);
		
		//modify for fixroad
		if(2 == m_nGPSMode)
		{
#ifdef __ANDROID			
			if ((m_gpsData.nSpeed > 20) && (nDis > 5))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_gpsData.nSpeed <= 20) && (m_gpsData.nSpeed > 5) && (nDis > m_nMinMoveDis))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_gpsData.nSpeed <= 5) && (nDis > 15))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_nMinMoveAngle != 0) && (m_nMinMoveAngle <= nAngleDif))
			{
				bNewGpsUpdate = nuTRUE;
			}
#else
			if ((m_gpsData.nSpeed > 20) && (nDis > 5))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_gpsData.nSpeed <= 20) && (m_gpsData.nSpeed > 5) && (nDis > m_nMinMoveDis))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_gpsData.nSpeed <= 5) && (nDis > 80))
			{
				bNewGpsUpdate = nuTRUE;
			}
			else if ((m_nMinMoveAngle != 0) && (m_nMinMoveAngle <= nAngleDif))
			{
				bNewGpsUpdate = nuTRUE;
			}
#endif	
		}
		else
		{
			if(nDis > 5)
			{
				bNewGpsUpdate = nuTRUE;
			}
		}
		
		//Save lastposition
		nuLONG lGapTime = 0, lTickTime = nuGetTickCount();
		if(m_nSvLtPosTimer == 0)
		{
			m_nSvLtPosTimer = lTickTime;
		}
		else
		{
			lGapTime = lTickTime - m_nSvLtPosTimer;
			m_nSvLtPosTimer = lTickTime;
			//m_nSvLtPosTimer ++;
			if( m_gpsData.nSpeed >= 10 && 
				40 >= m_gpsData.nSpeed &&
				( lGapTime >= 3000  ) && //|| (m_gpsData.nSpeed <= 30 && lGapTime >= 5000)
				nDis < 500  && 
				nDis > m_nMinMoveDis )
			{
				g_fileSysApi.FS_SaveLastPos(&m_gpsData);//Keep Old Position
				//m_nSvLtPosTimer = 0;
			}
		}
	}
	//
	nuLONG dx = 0, dy = 0;
	if( bNewGpsUpdate )
	{
		m_nNewGpsTicks = nuGetTickCount();
		m_gpsOld = m_gpsNew;
		m_gpsNew.point.x = m_gpsData.nLongitude;
		m_gpsNew.point.y = m_gpsData.nLatitude;
		m_gpsNew.nAngle	 = m_gpsData.nAngle;
		dx = m_gpsNew.point.x - m_gpsOld.point.x;
		dy = m_gpsNew.point.y - m_gpsOld.point.y;
		dx = NURO_ABS(dx);
		dy = NURO_ABS(dy);
		
		if( m_nGpsFakeTotalTimes <= 1 )
		{
			m_gpsNow = m_gpsNew;
			nRes =  DRAW_GPS;
		}
		else
		{
			if( dx > 50 || dy > 50 )// || g_fileSysApi.pGdata->uiSetData.nScaleValue > 200
			{
				m_gpsNow = m_gpsNew;
				m_nGpsFakeNowTimes = m_nGpsFakeTotalTimes;
			}
			else
			{
				m_nGpsFakeNowTimes = 0;
			}
		}
	}
	//
	if( m_nGpsFakeTotalTimes > 0 &&
		m_nGpsFakeTotalTimes > m_nGpsFakeNowTimes )
	{
		bNewGpsUpdate = nuTRUE;
		if( 0 == m_nGpsFakeNowTimes )//first update
		{
            m_nTicksDiff = 1000 / m_nGpsFakeTotalTimes;
            m_nGpsFakeNowTimes = 1;
		}
		else
		{
			nuDWORD nTicks = nuGetTickCount();
			m_nTicksDiff = nTicks - m_nNewGpsTicks;
			////////////////////////////////////////////////////////////
			if( m_nTicksDiff >= 1000 )
			{
				m_nGpsFakeNowTimes = m_nGpsFakeTotalTimes;
			}
			else
			{
                nuUINT nT = (m_nTicksDiff * m_nGpsFakeTotalTimes + 900) / 1000;
                if( nT <= m_nGpsFakeNowTimes )
                {
                    return DRAW_NO;
                }
                else
                {
                    m_nGpsFakeNowTimes = nT;
                }
			}
		}
		//
		if( m_nGpsFakeTotalTimes == m_nGpsFakeNowTimes )
		{
			m_gpsNow = m_gpsNew;
			nRes =  DRAW_GPS;
		}
		else
		{
			//point 
			dx = m_gpsNew.point.x - m_gpsOld.point.x;
			dy = m_gpsNew.point.y - m_gpsOld.point.y;
			m_gpsNow.point.x = m_gpsOld.point.x + (dx * m_nTicksDiff + 600) / 1000;
			m_gpsNow.point.y = m_gpsOld.point.y + (dy * m_nTicksDiff + 600) / 1000;
			//angle
            
			/*nuSHORT nDif = m_gpsNew.nAngle - m_gpsOld.nAngle;
			if( nDif < -180 )
			{
				nDif += 360;
			}
			else if( nDif > 180 )
			{
				nDif -= 360;
			}
			m_gpsNow.nAngle = m_gpsOld.nAngle + (nDif * m_nTicksDiff + 600) / 1000;*/
            
            m_gpsNow.nAngle = m_gpsNew.nAngle;
			nRes =  DRAW_GPS;
		}
	}
	if( bNewGpsUpdate )
	{
		if((m_nUseGPSState == DATA_TYPE_GPS || m_nUseGPSState == DATA_TYPE_GYRO) && m_gpsData.nSpeed < 5)
		{
			return nRes;
		}
		m_nUseGPSState = g_fileSysApi.pGdata->uiSetData.bGPSReady;
		pActionState->nDrawMode			= DRAWMODE_GPS;
		//pActionState->nSpeed			= m_gpsData.nSpeed;
		pActionState->ptNewMapCenter	= m_gpsNow.point;
		pActionState->nAngle			= (nuSHORT)m_gpsNow.nAngle;
		pActionState->bResetIconCarPos	= 1;
		pActionState->bResetRealCarPos	= 1;
		pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
		pActionState->bDrawBmp1			= 1;
		pActionState->bDrawInfo			= 1;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->bResetScale		= 1;
		pActionState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
		nRes =  DRAW_GPS;
	}
	return nRes;
}

nuUINT CNaviThread::SetMarket() 
{
	SEEKEDROADDATA roadData = {0};
	NUROPOINT ptMap			= {0};
	nuBOOL bNeedFindRoad = nuTRUE;
	nuBOOL bFindMappingRoad = nuFALSE;
	nuINT  nIdx = 0;
	nuLONG nAngle = -1;
	nuBOOL bAdd = nuFALSE;
	if( m_pMarketPoint->index >= 0 && m_pMarketPoint->index < g_fileSysApi.pGdata->routeGlobal.TargetCount )
	{
		nIdx = m_pMarketPoint->index;
	}
	else
	{
		if( g_fileSysApi.pGdata->routeGlobal.TargetCount < MAX_TARGET )
		{
			nIdx = g_fileSysApi.pGdata->routeGlobal.TargetCount;
			bAdd = nuTRUE;
		}
		else
		{
			nIdx = g_fileSysApi.pGdata->routeGlobal.TargetCount - 1;
		}
	}
	m_pMarketPoint->bSetSucess = 0;
	//
	if( m_pMarketPoint->bScreenPoint )
	{
		if( m_pMarketPoint->nPointType == POINT_TYPE_SCREEN_XY )
		{
			if( !m_gDataMgr.PointScreenToBmp(m_pMarketPoint->x, m_pMarketPoint->y) )
			{
				return DRAW_NO;
			}
		}
		else if( m_pMarketPoint->nPointType == POINT_TYPE_SCREEN_CENTER )
		{
			m_gDataMgr.GetScreenCenter(m_pMarketPoint->x, m_pMarketPoint->y);
		}
		else
		{
			return DRAW_NO;
		}
		//
		if( !g_mathtoolApi.MT_BmpToMap(m_pMarketPoint->x, m_pMarketPoint->y, &ptMap.x, &ptMap.y) )
		{
			return DRAW_NO;
		}
		/*
		if( nIdx == 0 )
		{
			nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
		}
		*/
	}//Set screen point to navigatoin point
	else
	{
		if( m_pMarketPoint->nPointType == POINT_TYPE_MAP_XY )
		{
			ptMap.x = m_pMarketPoint->x;
			ptMap.y = m_pMarketPoint->y;
		}
		else if( m_pMarketPoint->nPointType == POINT_TYPE_MAP_CAR )
		{
			ptMap.x = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
			ptMap.y = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
			if( g_fileSysApi.pGdata->drawInfoMap.bCarInRoad )
			{
				m_pMarketPoint->nDwIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
				m_pMarketPoint->nBlockIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
				m_pMarketPoint->nRoadIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
				m_pMarketPoint->x			= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped.x;
				m_pMarketPoint->y			= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped.y;
				m_pMarketPoint->nNameAddr	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nNameAddr;
				bNeedFindRoad = nuFALSE;
			}
		}
		else if( m_pMarketPoint->nPointType == POINT_TYPE_MAP_GPS )
		{
//			if( !g_gpsopenApi.GPS_GetGpsXY(&ptMap.x, &ptMap.y) )
			if( m_gpsData.nStatus )
			{
				ptMap.x = m_gpsData.nLongitude;
				ptMap.y = m_gpsData.nLatitude;
			}
			else
			{
				ptMap = g_fileSysApi.pGdata->carInfo.ptCarFixed;
			}
		}
		else if( m_pMarketPoint->nPointType == POINT_TYPE_MAP_XYFIXED )
		{
			ptMap.x = m_pMarketPoint->x;
			ptMap.y = m_pMarketPoint->y;
//			ptMap = m_pMarketPoint.ptRaw;
			bNeedFindRoad = nuFALSE;
		}
		else if( m_pMarketPoint->nPointType == POINT_TYPE_MAP_CENTER )
		{
			if( g_fileSysApi.pGdata->drawInfoMap.nCenterNameType != CENTER_NAME_ROAD_NEAR &&
				g_fileSysApi.pGdata->drawInfoMap.nCenterNameType != CENTER_NAME_ROAD )
			{
				return DRAW_NO;
			}
			ptMap.x = g_fileSysApi.pGdata->transfData.nMapCenterX;
			ptMap.y = g_fileSysApi.pGdata->transfData.nMapCenterY;
			m_pMarketPoint->nDwIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
			m_pMarketPoint->nBlockIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
			m_pMarketPoint->nRoadIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			m_pMarketPoint->x			= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped.x;
			m_pMarketPoint->y			= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped.y;
			m_pMarketPoint->nNameAddr	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nNameAddr;
			bNeedFindRoad = nuFALSE;
		}
		else
		{
			return DRAW_NO;
		}
		/*
		if( nIdx == 0 )
		{
			nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
		}
		*/
	}//Set map point to navigation point
	if( bNeedFindRoad )
	{
		if( nIdx == 0 )
		{
			nuLONG dx = ptMap.x - g_fileSysApi.pGdata->carInfo.ptCarRaw.x;
			nuLONG dy = ptMap.y - g_fileSysApi.pGdata->carInfo.ptCarRaw.y;
			if( NURO_ABS(dx) + NURO_ABS(dy) < DISTANCE_NEED_ANGLE_INSETMARKET )
			{
				nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
			}
		}
		/*
		if( !g_fileSysApi.FS_SeekForRoad(ptMap.x, ptMap.y, m_nMarketFixDis, nAngle, &roadData) )
		{
			return DRAW_NO;
		}
		*/
		SEEK_ROADS_IN	seekRdsIn  = {0};
		SEEK_ROADS_OUT	seekRdsOut = {0};
		seekRdsIn.point		= ptMap;
		seekRdsIn.nSeekDis	= m_nMarketFixDis;
		seekRdsIn.nAngle	= (nuSHORT)nAngle;
		if( !g_fileSysApi.FS_SeekForRoads(&seekRdsIn, &seekRdsOut, nuTRUE) || 0 == seekRdsOut.nCountSeeked)
		{
			seekRdsIn.nSeekDis = m_nMarketFixDis * 3;
			if(!g_fileSysApi.FS_SeekForRoads(&seekRdsIn, &seekRdsOut, nuTRUE) || 0 == seekRdsOut.nCountSeeked)
			{
				return DRAW_NO;
			}
		}
		roadData = seekRdsOut.roadsList[0];
	#ifndef VALUE_EMGRT
		for(nuBYTE j = 0; j < seekRdsOut.nCountSeeked; ++j)
		{
			seekRdsOut.roadsList[j].nMappingIdx = nRoadNameMapping(roadData, seekRdsOut.roadsList[j]);
			if(seekRdsOut.roadsList[j].nMappingIdx != 0)
			{
				bFindMappingRoad = nuTRUE;
			}
		}
	#endif
		if(bFindMappingRoad)
		{
			roadData = seekRdsOut.roadsList[0];
			for(nuBYTE i = 1; i < seekRdsOut.nCountSeeked; ++i)
			{
				if(seekRdsOut.roadsList[i].nMappingIdx > roadData.nMappingIdx)
				{
					roadData = seekRdsOut.roadsList[i];
				}
				else if(seekRdsOut.roadsList[i].nMappingIdx == roadData.nMappingIdx &&
					seekRdsOut.roadsList[i].nDis < roadData.nDis)
				{
					roadData = seekRdsOut.roadsList[i];
				}
			}
		}
		else
		{
			for(nuBYTE i = 1; i < seekRdsOut.nCountSeeked; ++i)
			{
				if( 4 >= roadData.nRoadClass /*|| 5 == roadData.nRoadClass*/ )
				{
					if( 4 >= seekRdsOut.roadsList[i].nRoadClass /*|| 5 == seekRdsOut.roadsList[i].nRoadClass*/ )
					{
						if( roadData.nDis > seekRdsOut.roadsList[i].nDis )
						{
							roadData = seekRdsOut.roadsList[i];
						}
					}
					else
					{
						if( roadData.nDis < 20 )
						{
							if( roadData.nDis > seekRdsOut.roadsList[i].nDis )
							{
								roadData = seekRdsOut.roadsList[i];
							}
						}
						else
						{
							roadData = seekRdsOut.roadsList[i];
						}
					}
				}
				else
				{
					if( 4 >= seekRdsOut.roadsList[i].nRoadClass /*|| 5 == seekRdsOut.roadsList[i].nRoadClass*/ )
					{
						if( seekRdsOut.roadsList[i].nDis < 20 &&
							seekRdsOut.roadsList[i].nDis < roadData.nDis )
						{
							roadData = seekRdsOut.roadsList[i];
						}
					}
					else
					{
						if( roadData.nDis > seekRdsOut.roadsList[i].nDis )
						{
							roadData = seekRdsOut.roadsList[i];
						}
					}
				}
			}
		}//
		m_pMarketPoint->nDwIdx		= roadData.nDwIdx;
		m_pMarketPoint->nBlockIdx	= roadData.nBlkIdx;
		m_pMarketPoint->nRoadIdx	= roadData.nRoadIdx;
		m_pMarketPoint->x			= roadData.ptMapped.x;
		m_pMarketPoint->y			= roadData.ptMapped.y;
		m_pMarketPoint->nNameAddr	= roadData.nNameAddr;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].TownID	= roadData.nTownID;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].CityID	= roadData.nCityID;
	}
	m_pMarketPoint->ptRaw		= ptMap;
	m_pMarketPoint->bSetSucess = 1;
	/**/
	
	//---------------added by daniel------------------------------------------------------//
	POI_Info_Data	PoiData;
	nuMemset(&PoiData, 0, sizeof(POI_Info_Data));
	if(g_fileSysApi.FS_ChangePoiInfo(ptMap ,PoiData))
	{
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].TownID		=  PoiData.TownID;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].CityID		=  PoiData.CityID;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].BaseCoor		=  ptMap;
	#ifdef VALUE_EMGRT
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].MapIdx		=  PoiData.MapIndex;
	#else
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].MapIdx		=  0;
	#endif
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].BlockIdx		=  PoiData.BlockIndex;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].RoadIdx		=  PoiData.RoadIndex;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].FixCoor.x    =  PoiData.x;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].FixCoor.y	=  PoiData.y;
	}
	else
	{
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].BaseCoor		= ptMap;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].MapIdx		= m_pMarketPoint->nDwIdx;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].BlockIdx		= (nuWORD)m_pMarketPoint->nBlockIdx;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].RoadIdx		= (nuWORD)m_pMarketPoint->nRoadIdx;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].FixCoor.x	= m_pMarketPoint->x;
		g_fileSysApi.pGdata->routeGlobal.pTargetInfo[nIdx].FixCoor.y	= m_pMarketPoint->y;
	}
	//---------------added by daniel------------------------------------------------------//
	if( bAdd/*g_fileSysApi.pGdata->routeGlobal.TargetCount < MAX_TARGET*/ )
	{
		g_fileSysApi.pGdata->routeGlobal.TargetCount ++;
	}
	if( m_pMarketPoint->bUpdateMap )
	{
		return DRAW_YES;
	}
	else
	{
		return DRAW_NO;
	}
}
nuUINT CNaviThread::DelMarket()
{
	if( g_fileSysApi.pGdata->routeGlobal.TargetCount == 0 )
	{
		m_pMarketPoint->bSetSucess = 0;
		return DRAW_NO;
	}
	if( m_pMarketPoint->index < 0 )
	{
		g_fileSysApi.pGdata->routeGlobal.TargetCount = 0;
	}
	else if( g_fileSysApi.pGdata->routeGlobal.TargetCount >= m_pMarketPoint->index )
	{
		g_fileSysApi.pGdata->routeGlobal.TargetCount --;
		m_pMarketPoint->x	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount].FixCoor.x;
		m_pMarketPoint->y	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount].FixCoor.y;
		m_pMarketPoint->nDwIdx		= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount].MapIdx;
		m_pMarketPoint->nBlockIdx	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount].BlockIdx;
		m_pMarketPoint->nRoadIdx	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[g_fileSysApi.pGdata->routeGlobal.TargetCount].RoadIdx;
	}
	else
	{
		m_pMarketPoint->x	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[m_pMarketPoint->index].FixCoor.x;
		m_pMarketPoint->y	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[m_pMarketPoint->index].FixCoor.y;
		m_pMarketPoint->nDwIdx		= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[m_pMarketPoint->index].MapIdx;
		m_pMarketPoint->nBlockIdx	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[m_pMarketPoint->index].BlockIdx;
		m_pMarketPoint->nRoadIdx	= g_fileSysApi.pGdata->routeGlobal.pTargetInfo[m_pMarketPoint->index].RoadIdx;
		for (int i = m_pMarketPoint->index; i < g_fileSysApi.pGdata->routeGlobal.TargetCount-1; i ++)
		{
			g_fileSysApi.pGdata->routeGlobal.pTargetInfo[i] = g_fileSysApi.pGdata->routeGlobal.pTargetInfo[i-1];
		}
		g_fileSysApi.pGdata->routeGlobal.TargetCount --;
	}
	m_pMarketPoint->bSetSucess = 1;
	__android_log_print(ANDROID_LOG_INFO, "navi", "AC_NAVI_DELMARKETPT finished");
	if( m_pMarketPoint->bUpdateMap )
	{
		return DRAW_YES;
	}
	else
	{
		return DRAW_NO;
	}
}

nuUINT CNaviThread::StopNavi(PACTIONSTATE pActionState)
{
	nuUINT nRes = DRAW_NO;
	m_nPlayedStartNaviState = PLAY_NAVISTART_NO;
	if( g_fileSysApi.pGdata == NULL )
	{
		return DRAW_NO;
	}
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		g_innaviApi.IN_NaviStop();
		g_routeApi.RT_ReleaseRoute(&g_fileSysApi.pGdata->routeGlobal.routingData);
		g_fileSysApi.pGdata->uiSetData.bNavigation	= 0;
		if(!m_bRouteEnd)
		{
			g_pSoundThread->CleanSound();
		}
		nRes = DRAW_YES;
	}
	if( g_fileSysApi.pGdata->routeGlobal.TargetCount > 0 && m_bCleanMarket )
	{
		g_fileSysApi.pGdata->routeGlobal.TargetCount	= 0;
		nRes = DRAW_YES;
	}
	if( nRes )
	{
		if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
		{
			//m_gDataMgr.SetAngle(360);
			pActionState->bSetAngle = 1;
			pActionState->nSpeed	= 0;
		}
	}
//	g_drawIfmApi.IFM_SetState(MOUSE_MENU_SIMULATION, STATE_BUT_DISABLE);

//	g_drawIfmApi.IFM_SetState(MOUSE_MENU_STARTNAVI, STATE_BUT_DISABLE);
	if( g_fileSysApi.pGdata->uiSetData.nSplitScreenType  == SPLIT_SCREEN_HAND ) 
	{
		g_fileSysApi.pGdata->uiSetData.nSplitScreenType  = SPLIT_SCREEN_AUTO;
	}
	g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_STOPNAVI;
	g_fileSysApi.pGdata->drawInfoNavi.nCrossZoomType = CROSSZOOM_TYPE_DEFAULT;

	if( g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_TIMER )
	{
		g_fileSysApi.pGdata->timerData.nAutoMoveType = AUTO_MOVE_GPS;
	}
	else
	{
		g_fileSysApi.pGdata->timerData.nOldMoveType = AUTO_MOVE_GPS;
	}
	m_nNaviErrorTimer	= 0;
	return nRes;
}

nuUINT CNaviThread::CheckLocalRoute(PACTIONSTATE pActionState)
{
	nuUINT i = 0;
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		if( g_innaviApi.IN_CheckScreenRoad(g_fileSysApi.pGdata->transfData.nuShowMapSize))
		{
			pActionState->bDrawBmp1 = 1;
			i = 1;
		}
	}
	return i;
}

nuUINT CNaviThread::DrawReady(PACTIONSTATE pActionState)
{
	m_DrawDataForUI.nddNaviThreadData.nAutoMoveType = g_fileSysApi.pGdata->timerData.nAutoMoveType;
	nuBYTE nSeekCenterRoadName = 1;
	nuBYTE bFixedToRoad = 0;
	nuLONG Dis = 0;
	nuBOOL bFindETG = nuFALSE;
	if( pActionState->bResetMapCenter &&
		!g_fileSysApi.FS_PtInBoundary(pActionState->ptNewMapCenter) )//If the point is out of the map' boundary, skip drawing
	{
        pActionState->bResetMapCenter = 0;
		return DRAW_NO;
	}
	if( !m_gDataMgr.SetUp(pActionState) )//Set up the drawing parameters
	{
		return DRAW_NO;
	}
    	if( m_nThdState != NAVI_THD_STOP_PREDRAW )   
	{
		if( CheckLocalRoute(pActionState) )//Load Local Routing;
		{
			pActionState->bNeedReloadMap = 1;
		}
	}
	if( pActionState->nDrawMode == DRAWMODE_SIM )
	{
		g_fileSysApi.pGdata->drawInfoNavi.PrePt = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
	}
	else
	{
		g_fileSysApi.pGdata->drawInfoNavi.PrePt = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
	}
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "\n\nDrawReady");
	//NaviLine process
	nuUINT	nInPtRes = INNAVI_POINT_TO_NAVI_ERROR;
	if(    g_fileSysApi.pGdata->drawInfoNavi.nIgnorePicID != 0 ||
		  ( pActionState->bResetMapCenter && 
		( (pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS || 
		    (pActionState->nDrawMode & 0xF0) == DRAWMODE_SIM  )) || m_bJumpRoad )
	{
		//m_NaviRoadCenterOn = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn;
		g_fileSysApi.pGdata->drawInfoMap.nCenterNameType = CENTER_NAME_NO_NEW;
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{	
			nuSHORT nAngle = pActionState->nAngle;/*g_fileSysApi.pGdata->carInfo.nCarAngle;*///@Zhikun 2009.08.19
			nInPtRes = g_innaviApi.IN_PointToNaviLine( pActionState->ptNewMapCenter.x, 
				pActionState->ptNewMapCenter.y,
				m_nMaxRoadFixDis,
				nAngle,
				&g_fileSysApi.pGdata->drawInfoMap.roadCenterOn);
			bFixedToRoad = CheckNaviFixed( nInPtRes, pActionState );
			__android_log_print(ANDROID_LOG_INFO, "RoadList", "Navi bFixedToRoad");
		}
		if(!bFixedToRoad)
		{
			SEEK_ROADS_IN	seekRdsIn = {0};
			SEEK_ROADS_OUT	seekRdsOut = {0}; 
			seekRdsIn.point		= pActionState->ptNewMapCenter;
			seekRdsIn.nSeekDis	= CENTERROAD_SHOW_MAX_LEN;
			seekRdsIn.nAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
			nuBOOL bChooseRoad  = nuFALSE;
			//m_FreeRoadCenterOn = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn;
			if( g_fileSysApi.FS_SeekForRoads(&seekRdsIn, &seekRdsOut, nuFALSE) )
			{
				bFixedToRoad = 1;
				bChooseRoad = ChooseCenterRoad( g_fileSysApi.pGdata->drawInfoMap.roadCenterOn,
								&seekRdsOut,
								g_fileSysApi.pGdata->drawInfoMap.roadCarOn );
			}			
			if( !g_fileSysApi.pGdata->uiSetData.bNavigation && 
				bChooseRoad && 
				(pActionState->nDrawMode == DRAWMODE_GPS) &&
				AUTO_MOVE_GPS == g_fileSysApi.pGdata->timerData.nAutoMoveType)
			{
				g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = g_innaviApi.IN_JudgeRoadType(&g_fileSysApi.pGdata->drawInfoMap.roadCenterOn);
			}
		}
		/*if(bFixedToRoad)
		{
			if( g_fileSysApi.pGdata->uiSetData.bNavigation && 
				(pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS &&
				AUTO_MOVE_GPS == g_fileSysApi.pGdata->timerData.nAutoMoveType)
			{
				if( (m_NaviRoadCenterOn.nBlkIdx != m_FreeRoadCenterOn.nBlkIdx || 
					m_NaviRoadCenterOn.nRoadIdx != m_FreeRoadCenterOn.nRoadIdx))
				{
					if(m_nNaviErrorTimer == 0 && m_gpsData.nSpeed > 16)//Speed by David
					{
						__android_log_print(ANDROID_LOG_INFO, "RoadList", "m_nNaviErrorTimer-1");
						m_nNaviErrorTimer = nuGetTickCount();
					}
				}
				else if((m_NaviRoadCenterOn.nBlkIdx == m_FreeRoadCenterOn.nBlkIdx &&
					m_NaviRoadCenterOn.nRoadIdx == m_FreeRoadCenterOn.nRoadIdx) &&
					m_NaviRoadCenterOn.nOneWay != m_FreeRoadCenterOn.nOneWay)
				{
					if(m_nNaviErrorTimer == 0 && m_gpsData.nSpeed > 16)//Speed by David
					{
						__android_log_print(ANDROID_LOG_INFO, "RoadList", "m_nNaviErrorTimer-2");
						m_nNaviErrorTimer = nuGetTickCount();
					}
				}
				else if(nInPtRes != INNAVI_POINT_TO_NAVI_RIGHT)
				{
					if(m_nNaviErrorTimer == 0)
					{					
						__android_log_print(ANDROID_LOG_INFO, "RoadList", "m_nNaviErrorTimer-3");		
						m_nNaviErrorTimer = nuGetTickCount();
					}
				}
				else
				{
					__android_log_print(ANDROID_LOG_INFO, "RoadList", "m_nNaviErrorTimer-2");
					m_nNaviErrorTimer = 0;
				}
				if(nInPtRes != INNAVI_POINT_TO_NAVI_RIGHT)
				{
					__android_log_print(ANDROID_LOG_INFO, "RoadList", "Navi m_FreeRoadCenterOn");
					g_fileSysApi.pGdata->drawInfoMap.roadCenterOn = m_FreeRoadCenterOn;
				}
				else
				{
					__android_log_print(ANDROID_LOG_INFO, "RoadList", "Navi m_NaviRoadCenterOn");
					g_fileSysApi.pGdata->drawInfoMap.roadCenterOn = m_NaviRoadCenterOn;
				}
			}
			else if((pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS)
			{
				__android_log_print(ANDROID_LOG_INFO, "RoadList", "Gps m_FreeRoadCenterOn");
				g_fileSysApi.pGdata->drawInfoMap.roadCenterOn = m_FreeRoadCenterOn;
			}
			else
			{
				__android_log_print(ANDROID_LOG_INFO, "RoadList", "Gps m_NaviRoadCenterOn");
				g_fileSysApi.pGdata->drawInfoMap.roadCenterOn = m_NaviRoadCenterOn;
			}
		}*/
		m_bFindRoad = bFixedToRoad;
	}
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "roadCenterOn blockidx %d, roadidx %d, class %d", g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx,
g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx, g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass);
	if( m_gDataMgr.SetCrossNearEx( pActionState, 
		g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType,
		g_fileSysApi.pGdata->drawInfoNavi.nCrossZoomType ) )
	{
		pActionState->bNeedReloadMap = 1;
	}
	//
	if( pActionState->bNeedReloadMap )//Reload block's data
	{
		m_gDataMgr.CalMapShowInfo(pActionState);
		if( !g_drawmapApi.DM_LoadMap(pActionState) )
		{
			pActionState->bDrawBmp1	= 0;
		}
		else
		{
			pActionState->bDrawBmp1	= 1;
		}
		g_memMgrApi.MM_CollectDataMem(0);
	}
	else 
	{
		pActionState->bDrawBmp1	= 0;
	}
	if( nInPtRes == INNAVI_POINT_TO_NAVI_RIGHT )
	{
		g_fileSysApi.FS_PaddingRoad(&g_fileSysApi.pGdata->drawInfoMap.roadCenterOn);
	}
	if( pActionState->bResetMapCenter )
	{
		//If that is a GPS or Simulation point, need to fix it to road
		nuBOOL bMoveMapSet = nuTRUE;
		if( (pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS || 
			(pActionState->nDrawMode & 0xF0) == DRAWMODE_SIM ||
			pActionState->nDrawMode == DRAWMODE_POWERON )
		{
			//GPS mode, fixed to the same road N times, change the road Name.
			if( bFixedToRoad )
			{
				pActionState->ptNewMapCenter	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
				m_gDataMgr.FixUp(pActionState);
				if ( pActionState->nDrawMode == DRAWMODE_GPS ||
					 pActionState->nDrawMode  == DRAWMODE_POWERON )
				{
					g_fileSysApi.pGdata->drawInfoMap.bCarInRoad		= 1;
					g_fileSysApi.pGdata->drawInfoMap.roadCarOn		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn;
					//Update Name
					if( m_roadNameStr.nDwIdx	== g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx &&
						m_roadNameStr.nNameAddr	== g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nNameAddr )
					{
						m_roadNameStr.nSameTimes ++;
					}
					else
					{
						m_roadNameStr.nSameTimes = 0;
						m_roadNameStr.nDwIdx	 = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
						m_roadNameStr.nNameAddr	 = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nNameAddr;
					}
				}
				if( (pActionState->nDrawMode == DRAWMODE_GPS && 
					 m_roadNameStr.nSameTimes >= GPS_ROADNAME_UPDATE_TIMES) ||
					pActionState->nDrawMode == DRAWMODE_SIM ||
					pActionState->nDrawMode == DRAWMODE_POWERON )
				{
					g_fileSysApi.pGdata->drawInfoMap.nCenterNameType	= CENTER_NAME_ROAD;
					m_roadNameStr.nSameTimes = 0;
					//Read the name of the road which the car is running on.
				}
			}//GPS or Simulation, ????????????????路
			else
			{
				m_roadNameStr.nSameTimes = 0;
				g_fileSysApi.pGdata->drawInfoMap.bCarInRoad	= 0;
			}//GPS or Simulation, ?????????????????路
			//ReRouting
			if(bFixedToRoad && m_nNaviErrorTimer > m_nErrorTimesToReroute/*NAVI_MAX_ERROR*/ )
			{
				CODENODE codeNode    = {0};
				codeNode.nActionType = AC_NAVI_REROUTE;
				m_codeNavi.PushNode(&codeNode);
				m_nNaviErrorTimer = 0;
				//g_fileSysApi.pGdata->drawInfoMap.roadCenterOn = m_FreeRoadCenterOn;
				//g_fileSysApi.pGdata->drawInfoMap.roadCarOn = m_FreeRoadCenterOn;
				//__android_log_print(ANDROID_LOG_INFO, "RoadList", "AC_NAVI_REROUTE");
			}
			bMoveMapSet = nuFALSE;
		}
		if( g_fileSysApi.pUserCfg->nPlaySpeedWarning && bFixedToRoad && pActionState->nDrawMode == DRAWMODE_GPS )
		{
			m_gDataMgr.OverSpeedCheck(g_fileSysApi.pUserCfg->nPlaySpeedWarning);
		}//overspeed warning
		if( !bFixedToRoad && pActionState->nDrawMode != DRAWMODE_STOP )//failed to map to road or other drawModes
		{
			if( g_drawmapApi.DM_SeekForRoad( pActionState->ptNewMapCenter.x, 
										pActionState->ptNewMapCenter.y, 
										CENTERROAD_SEEK_MAX_LEN, 
										&g_fileSysApi.pGdata->drawInfoMap.roadCenterOn ) )
			{
				if( g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDis <= CENTERROAD_SHOW_MAX_LEN )//Show road name
				{
					g_fileSysApi.pGdata->drawInfoMap.nCenterNameType = CENTER_NAME_ROAD_NEAR;
				}
				else//Show poi or CityTown name
				{
					g_fileSysApi.pGdata->drawInfoMap.nCenterNameType = CENTER_NAME_CITYTOWN;
				}
				//Read road, poi or CityTown Name...
				bFixedToRoad = 1;
				if( pActionState->nDrawMode == DRAWMODE_POWERON && 
					!g_fileSysApi.pGdata->drawInfoMap.bCarInRoad )
				{
					g_fileSysApi.pGdata->drawInfoMap.bCarInRoad = 1;
					g_fileSysApi.pGdata->drawInfoMap.roadCarOn  = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn;
				}
			}
		}
		g_fileSysApi.pGdata->drawInfoMap.bFindCenterRoad = bFixedToRoad;
		//__android_log_print(ANDROID_LOG_INFO, "navi", "bFixedToRoad %d", bFixedToRoad);
		if( bFixedToRoad )
		{
			if( m_CenterRdName.nDwIdx != g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx ||
				m_CenterRdName.nNameAddr != g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nNameAddr ||
				m_CenterRdName.nRoadClass != g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass )
			{
				RNEXTEND rnEx;
				m_CenterRdName.nDwIdx = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
				m_CenterRdName.nNameAddr = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nNameAddr;
				m_CenterRdName.nRoadClass = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass;
				g_fileSysApi.FS_GetRoadName( g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx,
					g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nNameAddr,
					g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass,
					g_fileSysApi.pGdata->drawInfoMap.wsCenterName,
					sizeof(g_fileSysApi.pGdata->drawInfoMap.wsCenterName),
									 &rnEx );

			}
			g_fileSysApi.pGdata->drawInfoMap.nCenterCityID  = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nCityID;
			g_fileSysApi.pGdata->drawInfoMap.nCenterTownID	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nTownID;
			g_fileSysApi.pGdata->drawInfoMap.nCenterMapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
		}
		g_fileSysApi.pGdata->drawInfoMap.bFindCenterPoi = g_fileSysApi.FS_SeekForPoi( pActionState->ptNewMapCenter.x,
			pActionState->ptNewMapCenter.y,
			CENTERPOI_SEEK_MAX_DIS,
			g_fileSysApi.pGdata->carInfo.nCarAngle,
			&g_fileSysApi.pGdata->drawInfoMap.poiCenterOn );
		if( g_fileSysApi.pGdata->drawInfoMap.bFindCenterPoi )
		{
			if( !bFixedToRoad ||
				g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nDis < g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDis )
			{
				g_fileSysApi.pGdata->drawInfoMap.nCenterCityID = g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nCityID;
				g_fileSysApi.pGdata->drawInfoMap.nCenterTownID = g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nTownID;
				g_fileSysApi.pGdata->drawInfoMap.nCenterMapIdx = g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nMapIdx;
			}
		}//Find Center road or poi
	}
	if( !pActionState->bNeedExtendRotate && 
		(pActionState->nDrawMode == DRAWMODE_GPS || pActionState->nDrawMode == DRAWMODE_SIM) && 
		bFixedToRoad )
	{
		//new ccd way...
		nuroRECT rtMap			= {0};
		m_ccdInfo.nCarAngle		= g_fileSysApi.pGdata->carInfo.nCarAngle;
		if( pActionState->nDrawMode == DRAWMODE_SIM )
		{
			rtMap.left	= g_fileSysApi.pGdata->transfData.nMapCenterX - _CCD_CHECK_MAX_DIS;
			rtMap.right	= g_fileSysApi.pGdata->transfData.nMapCenterX + _CCD_CHECK_MAX_DIS;
			rtMap.top	= g_fileSysApi.pGdata->transfData.nMapCenterY - _CCD_CHECK_MAX_DIS;
			rtMap.bottom= g_fileSysApi.pGdata->transfData.nMapCenterY + _CCD_CHECK_MAX_DIS;
			m_ccdInfo.nCarMapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
			m_ccdInfo.nCarBlkIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
			m_ccdInfo.nCarRdIdx		= (nuWORD)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			m_ccdInfo.ptCarMapped	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
			m_ccdInfo.nOneWay		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nOneWay;
			m_ccdInfo.ptCarNextCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptNextCoor;
		}
		else
		{
			rtMap.left	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x - _CCD_CHECK_MAX_DIS;
			rtMap.right	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x + _CCD_CHECK_MAX_DIS;
			rtMap.top	= g_fileSysApi.pGdata->carInfo.ptCarFixed.y - _CCD_CHECK_MAX_DIS;
			rtMap.bottom= g_fileSysApi.pGdata->carInfo.ptCarFixed.y + _CCD_CHECK_MAX_DIS;
			m_ccdInfo.nCarMapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
			m_ccdInfo.nCarBlkIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
			m_ccdInfo.nCarRdIdx		= (nuWORD)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
			m_ccdInfo.ptCarMapped	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
			m_ccdInfo.nOneWay		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nOneWay;
			m_ccdInfo.ptCarNextCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptNextCoor;
		}
		/**/
		m_ccdInfo.bNewInfo = 0;
		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			m_ccdInfo.bNewInfo = g_innaviApi.IN_GetCCDRoadInfo(&m_ccdInfo);
		}
		if( !m_ccdInfo.bNewInfo )
		{
			nuLONG dx = m_ccdInfo.ptCarNextCoor.x - m_ccdInfo.ptCarMapped.x;
			nuLONG dy = m_ccdInfo.ptCarNextCoor.y - m_ccdInfo.ptCarMapped.y;
			if(NURO_ABS(dx) > 5 ||	NURO_ABS(dy) > 5)
				m_ccdInfo.nCCDRoadAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
			else
				m_ccdInfo.nCCDRoadAngle = m_gpsData.nAngle;
			m_ccdInfo.bNewInfo = (nuBYTE)g_fileSysApi.FS_SeekForCcdInfo(rtMap, &m_ccdInfo);
		}
		//
		if( m_ccdInfo.bNewInfo )
		{
			g_fileSysApi.FS_CheckNewCcd(&m_ccdInfo);
		}
	}
	if(!pActionState->bNeedExtendRotate && 
		g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_DEMO3D && 
		g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_CAR)
	{
		nuLONG angle = nulAtan2(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRdDy , g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRdDx);
		if(NURO_ABS(g_fileSysApi.pGdata->carInfo.nCarAngle - angle) < m_gDataMgr.m_nRotateMapAngle)
		{
			g_fileSysApi.pGdata->carInfo.nMapAngle = angle;
			pActionState->bSetAngle = 1;
			m_gDataMgr.SetAngleParameter();
			pActionState->bDrawBmp1 = 1;
		}
	}
	return DRAW_YES;
}
extern nuSHORT *gRgb565PicScreen;
nuUINT CNaviThread::DrawAction(PACTIONSTATE pActionState)
{
	NURORECT Rect;
	NUROPOINT ptCar;
	m_bSaveMapBmp = nuFALSE;
	m_bDrawRealPic = nuFALSE;
	if( m_nThdState >= NAVI_THD_STOP_DRAW )
	{
		return DRAW_NO;
	}
	if( pActionState->bDrawBmp1 || m_nFlush_Map_For_TMC)//Draw Map
	{	
		m_nFlush_Map_For_TMC = 0;
		if( !g_drawmapApi.DM_DrawMapBmp1(pActionState)  )
		{
			return DRAW_NO;
		}

		if( g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			g_innaviApi.IN_DrawNaviRoad(MAP_DEFAULT);
		}
		if( !g_drawmapApi.DM_DrawMapBmp2(pActionState) )
		{
			return DRAW_NO;
		}
		SetDrawDataForUI();
		//--------------SDK------------------//
		if(g_fileSysApi.pGdata->uiSetData.bNavigation && m_PackPicPara.bShowPic)//&& m_PackPicPara.bEngineShow 
		{
			if(m_DrawDataForUI.nddRealPicID > 0)
			{
				if(LoadArchivePack(0, true))
				{
					if(!m_RealPicBmp.bmpHasLoad || m_nRealPicID != m_DrawDataForUI.nddRealPicID)
					{
						m_nRealPicID = m_DrawDataForUI.nddRealPicID;
						m_bDrawRealPic = ChangeRealPackPic(m_nRealPicID);
					}

					Rect.top    = m_PackPicPara.nTop;
					Rect.left   = m_PackPicPara.nLeft;
					Rect.right  = m_PackPicPara.nRight;
					Rect.bottom = m_PackPicPara.nBottom;
					nuINT nShowPicW = Rect.right - Rect.left;
					nuINT nSHowPicH = Rect.bottom - Rect.top;
					m_bDrawRealPic = g_drawmapApi.DM_DrawMapPic(Rect, m_RealPicBmp, gRgb565PicScreen);
					__android_log_print(ANDROID_LOG_INFO, "3dpic", "m_bDrawRealPic %d, Rect.top %d, Rect.left %d, Rect.right %d, Rect.bottom %d", 
					m_bDrawRealPic, Rect.top, Rect.left, Rect.right, Rect.bottom);
				}
			}
		}
		g_drawmapApi.DM_DrawMapCar(ptCar);
		//--------------SDK------------------//
		g_memMgrApi.MM_CollectDataMem(0);
		g_mathtoolApi.MT_SaveOrShowBmp(SAVETOMEM_BMP1);//Save map to MemBmp1
		m_DrawDataForUI.bIsMoving = nuFALSE;
		NuroJavaData();
	}	
	m_bSaveMapBmp = nuTRUE;	
	g_mathtoolApi.MT_SaveOrShowBmp(SHOW_DEFAULT);//Get the pro
	return DRAW_YES;
}

#define FAKE_SPEED_FAST				10
nuVOID CNaviThread::DrawGpsFakeMove(PACTIONSTATE pAcState)
{
	if( !m_bIsDrawing && 
		m_nGpsFakeAdd < g_fileSysApi.pUserCfg->nMoveType && 
		m_nThdState == NAVI_THD_WORK )  
	{
		m_bIsDrawing = nuTRUE;
		nuDWORD nNowTicks = nuGetTickCount();
		int nTimesOfUpdate = MAX_GPS_SPACE_OF_TIME;
		int nSpeedMultiple = 1;
		if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
		{
			nTimesOfUpdate = MAX_SIM_SPACE_OF_TIME;
			nSpeedMultiple = MAX_GPS_SPACE_OF_TIME / MAX_SIM_SPACE_OF_TIME;
			nSpeedMultiple = NURO_MAX( nSpeedMultiple, 1 );
		}
		//
		if( nNowTicks - m_nGpsFakeMoveTimer >= (nuDWORD)(nTimesOfUpdate/(g_fileSysApi.pUserCfg->nMoveType+1)) )
		{
			++m_nGpsFakeAdd;
//			nuLONG nFakeDis = (g_fileSysApi.pGdata->carInfo.nShowSpeed + FAKE_SPEED_FAST) * nSpeedMultiple / (36*(g_fileSysApi.pUserCfg->nMoveType+1));
			nuLONG nFakeDis = g_fileSysApi.pGdata->carInfo.nShowSpeed * 10 / (36*(g_fileSysApi.pUserCfg->nMoveType+1));
			nFakeDis /= nSpeedMultiple;
//2009.06.15			if( m_gDataMgr.CarFakeMove(nFakeDis, pAcState) )
			if( m_gDataMgr.CarFakeMove(m_nGpsFakeDis/nSpeedMultiple, (g_fileSysApi.pUserCfg->nMoveType+1), m_nGpsFakeAdd-1, pAcState) )
			{
				//Draw InfoMapMenu here
				g_mathtoolApi.MT_SaveOrShowBmp(PAINT_NUROBMP);
				g_mathtoolApi.MT_SaveOrShowBmp(SHOWFROMMEM_BMP1);
				if( g_fileSysApi.pGdata->uiSetData.b3DMode )
				{
					g_mathtoolApi.MT_SaveOrShowBmp(SHOW_3D_BMP);
				}
				//g_drawIfmApi.IFM_DrawItems(NULL, g_fileSysApi.pDrawInfo);
//				if( g_fileSysApi.pGdata->uiSetData.bNavigation )
				{
					//g_drawIfnApi.IFN_DrawItems(g_fileSysApi.pDrawInfo);
				}
				g_drawmapApi.DM_DrawRawCar(NULL);
				g_mathtoolApi.MT_SaveOrShowBmp(SHOW_DEFAULT);
				g_fileSysApi.pGdata->timerData.bUpdateMap		= 0;
				g_fileSysApi.pGdata->timerData.nUpdateMapTime	= 0;
			}
			m_nGpsFakeMoveTimer = nNowTicks;
			m_bGpsFakeMove = 1;
		}
		m_bIsDrawing = nuFALSE;
	}
}

nuUINT CNaviThread::DataUpdate(PACTIONSTATE pActionState)
{
	if( g_fileSysApi.pUserCfg->nVoiceIconTimer )
	{
		if( g_fileSysApi.pGdata->timerData.bShowVoiceIcon && 
			g_fileSysApi.pGdata->timerData.nShowVoiceTimer*TIMER_TIME_UNIT/1000 >= g_fileSysApi.pUserCfg->nVoiceIconTimer/*MAX_SHOW_VOICE_MS*/ )
		{
			g_fileSysApi.pGdata->timerData.nShowVoiceTimer = 0;
			g_fileSysApi.pGdata->timerData.bShowVoiceIcon	= 0;
			pActionState->bShowMap	= 1;
			pActionState->bDrawInfo	= 1;
			return DRAW_YES;
		}
	}
	else if(g_fileSysApi.pGdata->timerData.bCanUpdateTime)
	{
		pActionState->bNeedReloadMap = 1;
		//pActionState->bDrawBmp1 = 1;
		pActionState->bShowMap	= 1;
		pActionState->bDrawInfo	= 1;	
		return DRAW_YES;
	}
	return DRAW_NO;
}

nuUINT CNaviThread::KeyProc(nuUINT nKey, nuBYTE nKeyType, PACTIONSTATE pActionState)
{
	return 0;
/*	nuUINT nRes = DRAW_NO;
	//nRes = g_drawIfmApi.IFM_KeyProc(nKey, nKeyType, &m_drawToUI.nReturnCode);
	if( nRes == STATE_UPDATA_DRAWINFO )
	{
		pActionState->bDrawInfo	= 1;
		nRes = DRAW_YES;
	}
	else if( nRes == STATE_UPDATA_SHOWMAP )
	{
		pActionState->bShowMap	= 1;
		nRes = DRAW_YES;
	}
	else
	{
		nRes = DRAW_NO;
	}
	m_drawToUI.bUIWaiting = 0;
	return nRes;
*/
}

nuUINT CNaviThread::ButtonEvent(PCODENODE pCodeNode, nuUINT nCode, nuBOOL bUpOrDown)
{
//	MARKETPOINT marketPoint;
	switch(nCode)
	{
	case MOUSE_2D3D:
		if( g_fileSysApi.pUserCfg->b3DMode )
		{

			g_fileSysApi.pUserCfg->b3DMode = 0;
		}
		else
		{

			g_fileSysApi.pUserCfg->b3DMode = 1;
		}
		pCodeNode->nActionType	= AC_DRAW_SETUSERCFG;
		pCodeNode->nDrawType	= DRAWTYPE_UI;
		pCodeNode->nExtend		= SETUSERCFG_NOEXTEND;
		m_codeDraw.PushNode(pCodeNode);
		break;
	case MOUSE_BACKTOCAR:
		pCodeNode->nActionType	= AC_DRAW_BACKTOCAR;
		pCodeNode->nDrawType	= DRAWTYPE_UI;
		pCodeNode->nExtend		= SETUSERCFG_NOEXTEND;
		pCodeNode->nX			= g_fileSysApi.pGdata->carInfo.ptCarIcon.x;
		pCodeNode->nY			= g_fileSysApi.pGdata->carInfo.ptCarIcon.y;
		m_codeDraw.PushNode(pCodeNode);
//		NuroSetVoice(2, 1);
		break;
	case MOUSE_ZOOMIN:
		m_nAutoScaleTick = nuGetTickCount();
		NuroZoom(nuTRUE);
		break;
	case MOUSE_ZOOMOUT:
		m_nAutoScaleTick = nuGetTickCount();
		NuroZoom(nuFALSE);
		break;
	case MOUSE_DIRECTION:
		if( m_bInNoNaviScale )
		{
			return 0;
		}
		if( g_fileSysApi.pUserCfg->nMapDirectMode == MAP_DIRECT_NORTH )
		{
			g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_CAR;
		}
		else
		{
			g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_NORTH;
		}
		pCodeNode->nActionType	= AC_DRAW_SETUSERCFG;
		pCodeNode->nDrawType	= DRAWTYPE_UI;
		pCodeNode->nExtend		= SETUSERCFG_NOEXTEND;
		m_codeDraw.PushNode(pCodeNode);
		break;
	case MOUSE_SPLITSCREEN:
		pCodeNode->nActionType	= AC_DRAW_SPLITSCREEN;
		pCodeNode->nDrawType	= DRAWTYPE_UI;
		m_codeDraw.PushNode(pCodeNode);
		break;
	case MOUSE_SUBMENU:
		NuroShowHideButton(MOUSE_MENU_OUT, STATE_BUT_DEFAULT);
		break;
		/*
	case MOUSE_SIMULATION:
		NuroNavigation(NAVIGATION_SIMROUTE);
		return 0;
	case MOUSE_GPSNAVI:
		NuroNavigation(NAVIGATION_GPSROUTE);
		return 0;
	case MOUSE_STOPNAVI:
		NuroNavigation(NAVIGATION_IFSTOP);
		break;
	case MOUSE_ONLEYROUTE:
		NuroNavigation(NAVIGATION_ONLYROUTE);
		break;
		*/
	case MOUSE_HIDEMENU:
		NuroShowHideButton(MOUSE_MENU_OUT, STATE_BUT_HIDE);
		break;
		/*
	case MOUSE_SETSTART:
		marketPoint.index			= 0;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
	case MOUSE_SETPASSNODE:
		marketPoint.index			= 1;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
	case MOUSE_SETEND:
		MARKETPOINT marketPoint;
		marketPoint.index			= 2;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
		*/
	case MOUSE_RETURNSTART:
//		if( !g_gpsopenApi.GPS_GetState() || 
//			!g_gpsopenApi.GPS_GetGpsXY(&pCodeNode->nX, &pCodeNode->nY) )
		if( m_gpsData.nStatus )
		{
			pCodeNode->nX = m_gpsData.nLongitude;
			pCodeNode->nY = m_gpsData.nLatitude;
		}
		else
		{
			pCodeNode->nX = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
			pCodeNode->nY = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
		}
		pCodeNode->nActionType	= AC_DRAW_BACKTOCAR;
		pCodeNode->nDrawType	= DRAWTYPE_UI;
		m_codeDraw.PushNode(pCodeNode);
		break;
	default:
		return 0;
	}	
	m_drawToUI.nReturnCode	= MOUSE_NOACTION;
	return 1;
}
nuUINT CNaviThread::MenuEvent(nuUINT nCode, nuBOOL bUpOrDown)
{
	return 0;
	/*switch(nCode)
	{
		case MOUSE_MENU_SHOWSUB:
			break;
		default:
			return 0;
	}
	return 1;*/
}

nuBOOL CNaviThread::NuroOnPaint()
{
	return nuFALSE;
	CODENODE codeNode	 = {0};
	codeNode.nActionType = AC_DRAW_ONPAINT;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ISEMPTY);
}

nuBOOL CNaviThread::NuroMoveMap(nuBYTE nDirection, nuBOOL bStartOrStop)
{
	if( bStartOrStop )
	{/*
		if( g_fileSysApi.pGdata->uiSetData.b3DMode || nDirection == MOVE_UPLEFT	|| 
			nDirection == MOVE_UPRIGHT	|| nDirection == MOVE_DOWNRIGHT ||
			nDirection == MOVE_DOWNLEFT ) */
		{
//			m_glideMove.StopMove(MOVE_STOP);
			m_jumpMove.SetMoveStep(m_nMoveScale);
			return m_jumpMove.MoveStart(nDirection);
		}/*
		else
		{
			m_jumpMove.MoveStop(MOVE_STOP);
			return m_glideMove.StartMove(nDirection);
		}*/
	}
	else 
	{
		m_nMoveScale = 1;
		m_jumpMove.MoveStop(nDirection);
		//m_glideMove.StopMove(nDirection);
		return nuTRUE;
	}
}

nuBOOL CNaviThread::NuroMoveMap1(nuBYTE nDirection, nuBYTE nMoveStep, nuBOOL bStartOrStop)
{
	if( bStartOrStop )
	{
		m_jumpMove.SetMoveStep(nMoveStep);
		return m_jumpMove.MoveStart(nDirection);
	}
	else 
	{
		m_nMoveScale = 1;	
		m_jumpMove.MoveStop(nDirection);

		//@2011.03.24 ?????????????????????
		m_bSaveMapBmp = nuFALSE;
		nuINT i = 0;
		while( !m_bSaveMapBmp )
		{
			nuSleep(25);
			i++;
			if (i == 80)
			{
				break;
			}
		}		
		//m_glideMove.StopMove(nDirection);
		return nuTRUE;
	}
}

nuBOOL CNaviThread::NuroMoveStart()
{
	return 0;
/*	switch(m_drawToUI.nReturnCode)
	{
		case MOUSE_LEFT:
			return NuroMoveMap(MOVE_LEFT, nuTRUE);
		case MOUSE_UP:
			return NuroMoveMap(MOVE_UP, nuTRUE);
		case MOUSE_RIGHT:
			return NuroMoveMap(MOVE_RIGHT, nuTRUE);
		case MOUSE_DOWN:
			return NuroMoveMap(MOVE_DOWN, nuTRUE);
		case MOUSE_LTUP:
			return NuroMoveMap(MOVE_UPLEFT, nuTRUE);
		case MOUSE_RTUP:
			return NuroMoveMap(MOVE_UPRIGHT, nuTRUE);
		case MOUSE_LTDN:
			return NuroMoveMap(MOVE_DOWNLEFT, nuTRUE);
		case MOUSE_RTDN:
			return NuroMoveMap(MOVE_DOWNRIGHT, nuTRUE);
		default:
			return nuFALSE;
	}
*/
}

nuUINT CNaviThread::NuroKeyDown(nuUINT nKey)
{
	return 0;
	/*
	if( m_pMenuInfo->nMenuState || m_pMsgData != NULL )
	{
		switch( nKey )
		{
			case NURO_VK_LEFT:
			case NURO_VK_UP:
			case NURO_VK_RIGHT:
			case NURO_VK_DOWN:
				break;
			default:
				break;
		}
		CODENODE codeNode = {0};
		codeNode.nX	= nKey;
		codeNode.nDrawType	= DRAWTYPE_UI;
		codeNode.nActionType= AC_DRAW_KEYEVENT;
		codeNode.nExtend	= KEY_DOWN;
		m_codePre.PushNode(&codeNode, PUSH_ADD_ISEMPTY);//PUSH_ADD_ATHEAD); //20081210
	}
	else
	{
		if( m_bKeyDown )
		{
			if( m_nMoveScale < 3 )
			{
				++m_nMoveScale;
			}
		}
		else
		{
			m_nMoveScale = 1;
		}
		switch( nKey )
		{
			case NURO_VK_LEFT:
				NuroMoveMap(MOVE_LEFT, nuTRUE);
				break;
			case NURO_VK_UP:
				NuroMoveMap(MOVE_UP, nuTRUE);
				break;
			case NURO_VK_RIGHT:
				NuroMoveMap(MOVE_RIGHT, nuTRUE);
				break;
			case NURO_VK_DOWN:
				NuroMoveMap(MOVE_DOWN, nuTRUE);
				break;
			case NURO_VK_LEFTUP:
				NuroMoveMap(MOVE_UPLEFT, nuTRUE);
				break;
			case NURO_VK_RIGUP:
				NuroMoveMap(MOVE_UPRIGHT, nuTRUE);
				break;
			case NURO_VK_LEFTDOWN:
				NuroMoveMap(MOVE_DOWNLEFT, nuTRUE);
				break;
			case NURO_VK_RIGDOWN:
				NuroMoveMap(MOVE_DOWNRIGHT, nuTRUE);
				break;
			default:
				break;
		}
	}
	m_bKeyDown = nuTRUE;
	return MOUSE_NOACTION;*/
}
nuUINT CNaviThread::NuroKeyUp(nuUINT nKey)
{
	return 0;
	/*
	m_bKeyDown = nuFALSE;
	CODENODE codeNode = {0};
	if( m_pMenuInfo->nMenuState || m_pMsgData != NULL )
	{
		NuroMoveMap(MOVE_STOP, nuFALSE);
		switch( nKey )
		{
			case NURO_VK_LEFT:
			case NURO_VK_RIGHT:
			case NURO_VK_ENTER:
				break;
			default:
				break;
		}
		codeNode.nX	= nKey;
		codeNode.nDrawType	= DRAWTYPE_UI;
		codeNode.nActionType= AC_DRAW_KEYEVENT;
		codeNode.nExtend	= KEY_UP;
		m_drawToUI.nReturnCode	= MOUSE_NOACTION;
		m_drawToUI.bUIWaiting	= 1;
		m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		while( !m_bQuit && m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		if( m_drawToUI.nReturnCode >= MOUSE_MSGBOX_OUT )
		{
			MsgBoxEvent();
		}
		else if( m_drawToUI.nReturnCode >= MOUSE_MENU_USEONLY )
		{
			MenuEvent(m_drawToUI.nReturnCode);
		}
		else
		{
			ButtonEvent(&codeNode, m_drawToUI.nReturnCode - MOUSE_MENU_OUT);

		}
		return m_drawToUI.nReturnCode;
	}
	else
	{
		switch( nKey )
		{
			case NURO_VK_SETUSERCFG:
				NuroSetUserCfg();
				break;
			case NURO_VK_LEFT:
				NuroMoveMap(MOVE_LEFT, nuFALSE);
				break;
			case NURO_VK_UP:
				NuroMoveMap(MOVE_UP, nuFALSE);
				break;
			case NURO_VK_RIGHT:
				NuroMoveMap(MOVE_RIGHT, nuFALSE);
				break;
			case NURO_VK_DOWN:
				NuroMoveMap(MOVE_DOWN, nuFALSE);
				break;
			case NURO_VK_LEFTUP:
				NuroMoveMap(MOVE_UPLEFT, nuFALSE);
				break;
			case NURO_VK_RIGUP:
				NuroMoveMap(MOVE_UPRIGHT, nuFALSE);
				break;
			case NURO_VK_LEFTDOWN:
				NuroMoveMap(MOVE_DOWNLEFT, nuFALSE);
				break;
			case NURO_VK_RIGDOWN:
				NuroMoveMap(MOVE_DOWNRIGHT, nuFALSE);
				break;
			case NURO_VK_ZOOMIN:
				NuroZoom(nuTRUE);
				break;
			case NURO_VK_ZOOMOUT:
				NuroZoom(nuFALSE);
				break;
			case NURO_VK_DAYNIGHT:
				if( g_fileSysApi.pUserCfg != NULL )
				{
					if( g_fileSysApi.pUserCfg->bNightDay )
					{
						g_fileSysApi.pUserCfg->bNightDay = 0;
//						g_fileSysApi.pUserCfg->nNightBgPic ++;
//						g_fileSysApi.pUserCfg->nNightBgPic %= 3;
					}
					else
					{
						g_fileSysApi.pUserCfg->bNightDay = 1;
//						g_fileSysApi.pUserCfg->nDayBgPic ++;
//						g_fileSysApi.pUserCfg->nDayBgPic %= 3;
					}
					NuroSetUserCfg();
				}
				break;
			case NURO_VK_2D3D:
				if( g_fileSysApi.pUserCfg != NULL )
				{
					if( g_fileSysApi.pUserCfg->b3DMode )
					{
						g_fileSysApi.pUserCfg->b3DMode = 0;
					}
					else
					{
						g_fileSysApi.pUserCfg->b3DMode = 1;
					}
					NuroSetUserCfg();
				}
				break;
			case NURO_VK_SPLITSCREEN:
				codeNode.nActionType	= AC_DRAW_SPLITSCREEN;
				codeNode.nDrawType		= DRAWTYPE_UI;
				m_codeDraw.PushNode(&codeNode);
				break;
			case NURO_VK_SUBMENU:
				NuroShowHideButton(MOUSE_MENU_OUT, STATE_BUT_DEFAULT);
				break;
			case NURO_VK_CMARKET:
				{
					MARKETPOINT marketPoint;
					marketPoint.index			= -1;
					marketPoint.bScreenPoint	= 1;
					marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
					marketPoint.bUpdateMap		= 1;
					NuroSetMarket(&marketPoint);
				}
				break;
			case NURO_VK_SIMNAVI:
				NuroNavigation(NAVIGATION_SIMROUTE);
				break;
			case NURO_VK_GPSNAVI:
				NuroNavigation(NAVIGATION_GPSROUTE);
				break;
			case NURO_VK_STOPNAVI:
				NuroNavigation(NAVIGATION_STOP);
				break;
			case NURO_VK_ESCAPE:
				if( g_fileSysApi.pGdata->timerData.bMoveWait )
				{
					codeNode.nActionType	= AC_DRAW_BACKTOCAR;
					codeNode.nDrawType		= DRAWTYPE_UI;
					codeNode.nExtend		= SETUSERCFG_NOEXTEND;
					codeNode.nX			= g_fileSysApi.pGdata->carInfo.ptCarIcon.x;
					codeNode.nY			= g_fileSysApi.pGdata->carInfo.ptCarIcon.y;
					m_codeDraw.PushNode(&codeNode);
				}
				break;
			default:
				break;
		}
	}
	return MOUSE_NOACTION;*/
}

#define MOUSE_MOVE_SLICE				20
nuUINT CNaviThread::MouseEvent(nuLONG x, nuLONG y, nuBYTE nDownOrUp)
{
	return 0;
/*
	if( m_hThread.handle == NULL )
	{
		return MOUSE_NOACTION;
	}
	nuDWORD nDif;
	if( nDownOrUp == MOUSE_TYPE_DOWN )//Mouse Down
	{
		m_nMoveTicks = m_nDownTicks = nuGetTickCount();
		if( m_glideMove.IsMoving() || m_jumpMove.IsMoving() )
		{
			return MOUSE_NOACTION;
		}
	}
	else if( nDownOrUp == MOUSE_TYPE_UP )//Mouse Up
	{
		m_nUpTicks = nuGetTickCount();
		nDif = m_nUpTicks - m_nDownTicks;
		//
		g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 0;
		if( g_fileSysApi.pGdata->timerData.bIsMovingMap || m_glideMove.IsMoving() || m_jumpMove.IsMoving() )
		{
			g_fileSysApi.pGdata->timerData.bIsMovingMap	= 0;
			NuroMoveMap(MOVE_STOP, nuFALSE);
			return MOUSE_NOACTION;
		}
	}
	else 
	{
		nuDWORD nTicks = nuGetTickCount();
		if( nTicks - m_nMoveTicks < MOUSE_MOVE_SLICE )
		{
			return MOUSE_NOACTION;
		}
		m_nMoveTicks = nTicks;
		if( m_glideMove.IsMoving() || m_jumpMove.IsMoving() )
		{
			nuroPOINT pt = {x, y};
			//nuUINT nCode = g_drawIfmApi.IFM_ItemsJudge(pt, MOUSE_TYPE_MOVE);
			//m_nMoveScale = (nuBYTE)(g_drawIfmApi.IFM_GetPtScale(pt) + 1);
//			if( nCode != m_drawToUI.nReturnCode )
			{
				//m_drawToUI.nReturnCode = nCode;
				NuroMoveStart();
			}
		}
		return MOUSE_NOACTION;
	}
	CODENODE codeNode = {0};
	codeNode.nX	= x;
	codeNode.nY	= y;
	codeNode.nDrawType	 = DRAWTYPE_UI;
	codeNode.nActionType = AC_DRAW_MOUSEEVENT;
	codeNode.nExtend	 = nDownOrUp;
	m_drawToUI.nReturnCode	= MOUSE_NOACTION;
	m_drawToUI.bUIWaiting	= 1;
	m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	while( !m_bQuit && m_drawToUI.bUIWaiting )
	{
		nuSleep(25);
	}
	nuPrintfNum(m_drawToUI.nReturnCode);
	if( !m_bQuit )
	{
		if( nDownOrUp == MOUSE_TYPE_UP)
		{
			if( m_drawToUI.nReturnCode == MOUSE_NOACTION ||
				(m_drawToUI.nReturnCode >= MOUSE_MOVE_LEFT && m_drawToUI.nReturnCode <= MOUSE_MOVE_DOWN_RIGHT) )//jump to the sceen Point
			{
				if( nDif < (nuDWORD)m_nGlideMoveMinMS )
				{
					NuroJumpToScreenXY(x, y);
					return MOUSE_CLICK;
				}
			}
			if( m_MouseDownCode == m_drawToUI.nReturnCode )
			{
				if( m_drawToUI.nReturnCode >= MOUSE_MSGBOX_OUT )
				{
					MsgBoxEvent();
				}
				else if( m_drawToUI.nReturnCode >= MOUSE_MENU_USEONLY )
				{
					MenuEvent(m_drawToUI.nReturnCode);
				}
				else if( m_drawToUI.nReturnCode > MOUSE_MENU_OUT )
				{
					m_drawToUI.nReturnCode -= MOUSE_MENU_OUT;
					ButtonEvent(&codeNode, m_drawToUI.nReturnCode);
				}
				else if( m_drawToUI.nReturnCode == MOUSE_MENU_OUT )
				{
					;
				}
				else
				{
					ButtonEvent(&codeNode, m_drawToUI.nReturnCode);
				}
			}
			else
			{
				nuPrintfNum(m_drawToUI.nReturnCode);
			}
		}
		else if( nDownOrUp == MOUSE_TYPE_DOWN )
		{
			nuroPOINT pt = {x, y};
			m_MouseDownCode = m_drawToUI.nReturnCode;
			switch(m_drawToUI.nReturnCode)
			{
				case MOUSE_LEFT:
				case MOUSE_UP:
				case MOUSE_RIGHT:
				case MOUSE_DOWN:
				case MOUSE_LTUP:
				case MOUSE_RTUP:
				case MOUSE_LTDN:
				case MOUSE_RTDN:
					//m_nMoveScale = (nuBYTE)(g_drawIfmApi.IFM_GetPtScale(pt) + 1);
					g_fileSysApi.pGdata->timerData.nMoveStartTicks	= m_nDownTicks;
					g_fileSysApi.pGdata->timerData.bIsMovingMap		= 0;
					g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 1;
					break;
				default:
					break;
			}
		}
	}
	return m_drawToUI.nReturnCode;
*/
}

nuBOOL CNaviThread::NuroJumpToScreenXY(nuLONG x, nuLONG y, nuBOOL bMode /* = nuFALSE */)
{
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		return nuFALSE;
	}
	// Added By Damon For JumpMove Not Add MoveMsg 20110812
	if( m_DrawDataForUI.bIsMoving && !bMode )
	{
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "Mouse", "NuroJumpToScreenXY");
	// -----------------------------------------------------
	g_fileSysApi.pGdata->timerData.bIsMovingMap			= 0; 
	g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 0;
	CODENODE codeNode = {0};
	codeNode.nX	= x;
	codeNode.nY	= y;
	codeNode.nDrawType	= DRAWTYPE_UI;
	codeNode.nActionType	= AC_DRAW_JUMPTOSCREENXY;
	codeNode.nExtend	= 0;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATEND);
}
nuBOOL CNaviThread::NuroMoveToMapXY(nuLONG x, nuLONG y)
{
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_JUMPTOMAPXY;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= 0;
	codeNode.nX				= x;
	codeNode.nY				= y;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
}
nuBOOL CNaviThread::NuroOperateMapXY(nuLONG x, nuLONG y, nuUINT nCode)
{
	if( nCode == OP_MAP_CARPOITION )
	{
		g_fileSysApi.pGdata->carInfo.ptCarIcon.x = x;
		g_fileSysApi.pGdata->carInfo.ptCarIcon.y = y;
		g_fileSysApi.pGdata->carInfo.ptCarRaw = g_fileSysApi.pGdata->carInfo.ptCarFixed = g_fileSysApi.pGdata->carInfo.ptCarIcon;
	}
	else if( OP_MAP_ONLYCARPOSITION == nCode )
	{
		g_fileSysApi.pGdata->carInfo.ptCarIcon.x = x;
		g_fileSysApi.pGdata->carInfo.ptCarIcon.y = y;
		g_fileSysApi.pGdata->carInfo.ptCarRaw = g_fileSysApi.pGdata->carInfo.ptCarFixed = g_fileSysApi.pGdata->carInfo.ptCarIcon;
	}
	CODENODE codeNode = {0};
	codeNode.nActionType	= AC_DRAW_OPERATEMAPXY;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= nCode;
	codeNode.nX				= x;
	codeNode.nY				= y;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
}
nuBOOL CNaviThread::NuroRotateMap(nuLONG nAngle)
{
	CODENODE codeNode	= {0};
	codeNode.nX	= nAngle;
	codeNode.nDrawType	= DRAWTYPE_UI;
	codeNode.nActionType= AC_DRAW_ROTATEMAP;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATEND);
}

nuBOOL CNaviThread::NuroZoom(nuBOOL bInOrOut)
{
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
	{
		return nuFALSE;
	}
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_SETUSERCFG;
	codeNode.nDrawType		= DRAWTYPE_UI;
	if( bInOrOut )
	{
		codeNode.nExtend	= SETUSERCFG_ZOOMIN;
	}
	else
	{
		codeNode.nExtend	= SETUSERCFG_ZOOMOUT;

	}
    /*
    TCHAR ts3dDemo[256];
    nuTcscpy(ts3dDemo, g_fileSysApi.pGdata->pTsPath);
    nuTcscat(ts3dDemo, "3DObjDemo.Dat");
    NuroStart3DDemo(ts3dDemo);
    */
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATEND);
}

nuBOOL CNaviThread::NuroSetMarket(nuPVOID lpVoid)
{
	if( lpVoid == NULL || 
		g_fileSysApi.pGdata->uiSetData.bNavigation || 
		//m_glideMove.IsMoving() || 
		m_jumpMove.IsMoving() )
	{
		return nuFALSE;
	}
	m_pMarketPoint = (PMARKETPOINT)lpVoid;
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		m_pMarketPoint->bUpdateMap = 0;
		SetMarket();
	}
	else
	{
		m_pMarketPoint->bIsSetting	= 1;
		CODENODE codeNode	 = {0};
		codeNode.nActionType = AC_NAVI_SETMARKETPT;
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			m_pMarketPoint->bIsSetting = 0;

			return nuFALSE;
		}
		//waiting for the result of setting.
		while( !m_bQuit && m_pMarketPoint->bIsSetting )
		{
			nuSleep(25);
		}
	}
	if( m_pMarketPoint->bSetSucess )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}
nuBOOL CNaviThread::NuroDelMarket(nuPVOID lpVoid)
{
	if( lpVoid == NULL || g_fileSysApi.pGdata->uiSetData.bNavigation || /*m_glideMove.IsMoving() ||*/ m_jumpMove.IsMoving() )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "NuroDelMarket return ");
		return nuFALSE;
	}
	m_pMarketPoint = (PMARKETPOINT)lpVoid;
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		m_pMarketPoint->bUpdateMap = 0;
		if( DelMarket() )
		{
			return nuTRUE;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroDelMarket fail");
			return nuFALSE;
		}
	}
	else
	{
		m_pMarketPoint->bIsSetting	= 1;
		CODENODE codeNode	 = {0};
		codeNode.nActionType = AC_NAVI_DELMARKETPT;
		__android_log_print(ANDROID_LOG_INFO, "navi", "AC_NAVI_DELMARKETPT");
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			m_pMarketPoint->bIsSetting = 0;
			__android_log_print(ANDROID_LOG_INFO, "navi", "AC_NAVI_DELMARKETPT fail");
			return nuFALSE;
		}
		//waiting for the result of setting.
		while( !m_bQuit && m_pMarketPoint->bIsSetting )
		{
			nuSleep(25);
		}
		if( m_pMarketPoint->bSetSucess )
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "m_pMarketPoint->bSetSucess true");
			return nuTRUE;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "m_pMarketPoint->bSetSucess fail");
			return nuFALSE;
		}
	}
}

nuBOOL CNaviThread::NuroNavigation(nuBYTE nType)
{
	if( g_fileSysApi.pGdata == NULL || /*m_glideMove.IsMoving() ||*/ m_jumpMove.IsMoving() )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 1");
		return nuFALSE;
	}
	m_bSimuReachDes = nuFALSE;
	CODENODE codeNode = {0};
		__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation nType  %d", nType);

	if( nType == NAVIGATION_IFSTOP )
	{
		if( !g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 2");
			return nuFALSE;
		}
		return NuroMessageBox(MSGBOX_IF_STOPNAVI, NULL);
		/*
		codeNode.nActionType = AC_NAVI_IFSTOP;
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			return nuFALSE;
		}
		return nuTRUE;
		*/
	}
	else if( nType == NAVIGATION_STOP )
	{
		if( !g_fileSysApi.pGdata->uiSetData.bNavigation )
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 3");
			return nuFALSE;
		}
		m_drawToUI.bUIWaiting	= 1;
		codeNode.nActionType = AC_NAVI_STOPROUTE;
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting	= 0;
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 4");
			return nuFALSE;
		}
		while( !m_bQuit && m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		return nuTRUE;
	}
	else if( nType == NAVIGATION_SIMROUTE || nType == NAVIGATION_GPSROUTE || nType == NAVIGATION_ONLYROUTE )
	{
		codeNode.nActionType = AC_NAVI_STARTROUTE;
		if( nType == NAVIGATION_SIMROUTE )
		{
		__android_log_print(ANDROID_LOG_INFO, "navi", "Simulation");
			codeNode.nExtend	= AUTO_MOVE_SIMULATION;
		}
		else if( nType == NAVIGATION_GPSROUTE )
		{
			codeNode.nExtend	= AUTO_MOVE_GPS;
		}
		else
		{
			codeNode.nExtend	= AUTO_MOVE_STOP;
		}
		m_drawToUI.bUIWaiting	= 1;
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting	= 0;
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 5");
			return nuFALSE;
		}
		while( !m_bQuit && m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation bNavigation %d",g_fileSysApi.pGdata->uiSetData.bNavigation);
		return g_fileSysApi.pGdata->uiSetData.bNavigation;
	}
	else if( nType == NAVIGATION_SIMROUTE_I || nType == NAVIGATION_GPSROUTE_I || nType == NAVIGATION_ONLYROUTE_I )
	{
		codeNode.nActionType = AC_NAVI_STARTROUTE;
		if( nType == NAVIGATION_SIMROUTE_I )
		{
			codeNode.nExtend	= AUTO_MOVE_SIMULATION;
		}
		else if( nType == NAVIGATION_GPSROUTE_I )
		{
			codeNode.nExtend	= AUTO_MOVE_GPS;
		}
		else
		{
			codeNode.nExtend	= AUTO_MOVE_STOP;
        }
        m_drawToUI.bUIWaiting	= 1;
		if( m_codeNavi.PushNode(&codeNode) )
		{
            nuSleep(25);
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 6");
			return nuTRUE;
		}
		else
		{
            m_drawToUI.bUIWaiting	= 0;
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 7");
			return nuFALSE;
		}
	}
	else
	{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroNavigation 8");
		return nuFALSE;
	}
}

nuBOOL CNaviThread::NuroSetUserCfg(nuUINT nCode/* = SETUSERCFG_NOEXTEND*/)
{
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_SETUSERCFG;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= nCode;
	/*
	*/
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		if( UserCfgProc(&codeNode, m_pAcState) )
		{
			return nuTRUE;
		}
		else
		{
			return nuFALSE;
		}
	}
	else
	{
		return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	}
}

#ifdef ANDROID_D
GPSDATA g_gpsdata = {0};

#endif

nuVOID CNaviThread::IntervalGetGps()
{
#ifdef ANDROID_D

    if (0 >= g_gpsdata.nStarNumUsed)
    {
        return;
    }
#if 0
	g_gpsdata.nLatitude = 3017913;
	g_gpsdata.nLongitude = 12015753;
	g_gpsdata.nAngle = 160;
	g_gpsdata.nSpeed = 0;
	g_gpsdata.nYear = 2012;
	g_gpsdata.nMonth = 6;
	g_gpsdata.nDay = 30;
	g_gpsdata.nHours = 10;
	g_gpsdata.nMinutes = 50;
	g_gpsdata.nSeconds = 49;
	g_gpsdata.nStarNumUsed = 0;
	g_gpsdata.nStatus = 1;
	g_gpsdata.nActiveStarNum = 7;
	g_gpsdata.nGyroState = 0;
	g_gpsdata.nReserve = 0;
	g_gpsdata.nAntennaHeight = 37;
	g_gpsdata.nReciveTimes = 1;
#endif
	//g_gpsdata.nLatitude = 3017913;
	//g_gpsdata.nLongitude = 12015753;
    //g_gpsdata.nStatus = 1;

	nuMemcpy(&m_gpsDataOld, &m_gpsData, sizeof(GPSDATA));
	nuMemcpy(&m_gpsData, &g_gpsdata, sizeof(GPSDATA));

	g_fileSysApi.pGdata->uiSetData.bGPSReady = g_gpsdata.nStatus;
	g_fileSysApi.pGdata->uiSetData.bGPSReady = DATA_TYPE_GPS;
	m_nGpsReadyType = 1;
	g_fileSysApi.pGdata->uiSetData.bFirstGPSReady = 1;
    
    if (0 != g_gpsdata.nStatus)
    {
	    m_bGetNewGpsData = true;
    }
    else
    {
        m_bGetNewGpsData = false;
    }

	g_fileSysApi.pGdata->timerData.bCanUpdateTime	= 1;

#ifdef NURO_DEBUG
    //FILE *fp = fopen("/mnt/sdcard/NuroWnds/log.txt", "a+");
    //fprintf(fp, "---okgood>>> %ld %ld\n", g_gpsdata.nLongitude, g_gpsdata.nLatitude);
    //fclose(fp);
#endif
    nuSleep(11);

#else

    IntervalGetGps2();

#endif

}

nuVOID CNaviThread::IntervalGetGps2()
{
	/*
	if( g_fileSysApi.pGdata->timerData.bIsRouting )
	{
		return ;
	}*/
//#ifdef _USE_LICENSE
    //CNuroEncryptZK encrypt;
    //char pccc[] = {"computer"};
   // encrypt.SetKey((nuBYTE*)pccc, 8);
    //DES_DATA data;
    //nuMemcpy(data.pData, "tsinghua", 8);

    //encrypt.Encrypt(data);
	/*DESJ des;
	 unsigned char bufIn[8] = {0};
	bufIn[0] = 0;
	bufIn[1] = 0;
	bufIn[2] = 0;
	bufIn[3] = 0x32;
	bufIn[4] = 0xFF;
	bufIn[5] = 0xFF;
	bufIn[6] = 0xFF;
	bufIn[7] = 0xFF;

	unsigned char bufOut[8] = {0};
	const unsigned char Key[9]="F0D137A8";
	//????
	des.CDesEnter( bufIn,bufOut,sizeof(bufOut),Key,0);
	//????
	unsigned char bufOut2[8] = {0};
	des.CDesEnter( bufOut,bufOut2,sizeof(bufOut2),Key,1);
         */
	
//#endif
#ifdef NURO_OS_UCOS
	if( !g_gpsopenApi.GPS_RecvData((const char*)m_nGpsBuff, m_nGpsBuffLen) )
	{
		m_nGpsBuffLen = 0;
		return;
	}
	m_nGpsBuffLen = 0;
#endif
	
#ifndef ANDROID
	// ------?W?[?@?? Reset GPS?????@------ Added by Damon 20120705
	if (1 == g_fileSysApi.pGdata->timerData.bResetGPS)
	{
		nuMemset(&m_gpsDataOld, 0, sizeof(GPSDATA));
		nuMemset(&m_gpsData, 0, sizeof(GPSDATA));

		GPSCFG nuGpsCfg = {0};
		if( !nuReadConfigValue("GPSMODE", &nuGpsCfg.nGPSOpenMode) )
		{
			nuGpsCfg.nGPSOpenMode = GPS_OPEN_COMM;
		}

		nuGpsCfg.nComport =  g_fileSysApi.pGdata->timerData.nGPSComPort;
		nuGpsCfg.nGPSBaudrate = g_fileSysApi.pGdata->timerData.nGPSBaudrate;
		
		g_gpsopenApi.GPS_ReSet(&nuGpsCfg);

		g_fileSysApi.pGdata->timerData.bResetGPS = 0;
		nuLONG nGPSComPort = 0, nGPSBaudrate = 0;
		g_gpsopenApi.GPS_GetGPSComConfig(nGPSComPort, nGPSBaudrate);
		g_fileSysApi.pGdata->timerData.nGPSComPort  = nGPSComPort;
		g_fileSysApi.pGdata->timerData.nGPSBaudrate = nGPSBaudrate;
	}	
	// ------------------------------------
#endif
	if((GPS_OPEN_ELEAD_3DR != m_nGPSMode || m_lUseGPSSource) && !g_gpsopenApi.GPS_RecvData(NULL, 0) )
	{
		return ;
	}
	GPSDATA TempGpsData = {0};
	g_gpsopenApi.GPS_GetRawData(&TempGpsData);
	if(g_fileSysApi.pGdata->uiSetData.bFirstGPSReady == 1)
	{
		g_fileSysApi.pGdata->carInfo.ptRawGPS.x = TempGpsData.nLongitude;
		g_fileSysApi.pGdata->carInfo.ptRawGPS.y = TempGpsData.nLatitude;
		g_fileSysApi.pGdata->carInfo.nRawGPSAngle = TempGpsData.nAngle;
	}
	/*NUROPOINT ptGPS = {0};
	ptGPS.x = TempGpsData.nLongitude;
	ptGPS.y = TempGpsData.nLatitude;
	if(!g_fileSysApi.FS_PtInBoundary(ptGPS))
	{
		return ;
	}*/
	if(GPS_OPEN_ELEAD_3DR == m_nGPSMode)
	{
		nuMemcpy(&g_fileSysApi.pGdata->uiSetData.pGpsData->pStarList, &TempGpsData.pStarList, sizeof(STARDATA) * TempGpsData.nActiveStarNum);
	}

	FEEDBACKDATA fdBackData = {0};
	fdBackData.bCarOnRoad	= g_fileSysApi.pGdata->drawInfoMap.bCarInRoad;
	fdBackData.ptRaw		= g_fileSysApi.pGdata->carInfo.ptCarRaw;
	fdBackData.ptFixedOnRoad= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
	fdBackData.nRoadDx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDx;
	fdBackData.nRoadDy		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDy;
	fdBackData.nDisFixedToRoad	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDis;
	nuBYTE nTunnel = 0;
	if(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.bGPSState)
	{
		nTunnel = 0x0E;
	}
	g_gpsopenApi.GPS_FeedBack(&fdBackData);
	g_gpsopenApi.GPS_FeedBackInfo(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped.x,
		g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped.y,
		g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDx,
		g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDy,
		nTunnel,
		nuMAKELONG(g_fileSysApi.pGdata->drawInfoMap.bCarInRoad, g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDis) );
	
	PGPSDATA pGpsData = NULL;
	if( g_gpsopenApi.GPS_GetNewData(&pGpsData) )
	{   // oooo
#ifndef VALUE_EMGRT
		//	Louis 20110704 for LCMM Reset Gyro
#ifndef ANDROID	
		g_gpsopenApi.GPS_TOBEDEBUGINFO(&pGpsModInfo);
		//Louis 20110906 for LCMM Reset Gyro
		//g_gpsopenApi.GPS_TOBERESETGYRO();
		nuMemcpy(&(g_fileSysApi.pGdata->uiSetData.pGpsData->nDebugGPSInfo),pGpsModInfo,sizeof(GPSMODINFO));
		nuMemcpy(&(pGpsData->nDebugGPSInfo),pGpsModInfo,sizeof(GPSMODINFO));
#endif
#endif
		NUROPOINT pt = {0};
		pt.x = pGpsData->nLongitude;
		pt.y = pGpsData->nLatitude;
		if(!g_fileSysApi.FS_PtInBoundary(pt))
		{
			return ;
		}
		g_fileSysApi.pGdata->uiSetData.bGPSReady = pGpsData->nStatus;
		if( !g_fileSysApi.pGdata->uiSetData.bFirstGPSReady )
		{
			if(  (g_fileSysApi.pGdata->uiSetData.bGPSReady == DATA_TYPE_GPS) || 
				  (g_fileSysApi.pGdata->uiSetData.bGPSReady == DATA_TYPE_GYRO && m_nGpsReadyType) )
			{
				g_fileSysApi.pGdata->uiSetData.bFirstGPSReady = 1;
				//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_GPSREADY, 0, NULL);
			}
			m_nTotalDisGone			= 0;
			m_nTotalTimeForSpeed	= 0;
			m_nOldTimeForSpeed		= 0;
			m_nHighSpeed			= 0;
#if !defined(NURO_OS_WINDOWS) || defined(_WIN32_WCE)
			if(pGpsData->nTime.nYear > 0)
			{
				NUROTIME nuTIME;
				nuMemset(&nuTIME, 0, sizeof(NUROTIME));
				nuTIME.nYear   = pGpsData->nTime.nYear+2000;
				nuTIME.nMonth  = pGpsData->nTime.nMonth;
				nuTIME.nDay    = pGpsData->nTime.nDay;
				nuTIME.nHour   = pGpsData->nTime.nHour ;
				nuTIME.nMinute = pGpsData->nTime.nMinute;
				nuTIME.nSecond = pGpsData->nTime.nSecond;
				if (2011 <= nuTIME.nYear)
				{
					//OutputDebugString(nuTEXT("====== Navi nuSetLocalTime ====== \r\n"));
					//nuSetLocalTime(&nuTIME);
					//if(nuSetLocalTime(&nuTIME))
					//{
						//OutputDebugString(nuTEXT("=Time up\r\n"));
					//}
					//else
					//{
						//OutputDebugString(nuTEXT("=Time up fail\r\n"));
					//}
				}
				g_fileSysApi.pGdata->timerData.nSecond		= pGpsData->nTime.nSecond;
				g_fileSysApi.pGdata->timerData.nMinute		= pGpsData->nTime.nMinute;
				g_fileSysApi.pGdata->timerData.nHour		= pGpsData->nTime.nHour;
				g_fileSysApi.pGdata->timerData.nDay			= pGpsData->nTime.nDay;
				g_fileSysApi.pGdata->timerData.nMonth		= pGpsData->nTime.nMonth;
				g_fileSysApi.pGdata->timerData.nYear		= pGpsData->nTime.nYear;
				g_fileSysApi.pGdata->timerData.nUpDateTimeTimer = 0;
			  //g_fileSysApi.pGdata->timerData.nUpDateTimeTimer		= MAX_UPDATE_SYSTIME/2;
			}
#endif
		}
		
		nuMemcpy(&m_gpsDataOld, &m_gpsData, sizeof(GPSDATA));
		if(pGpsData->nTime.nYear > 0)
		{
			nuMemcpy(&m_gpsData, pGpsData, sizeof(GPSDATA));
		}
		else
		{
			nuMemcpy(&m_gpsData, pGpsData, sizeof(GPSDATA));
			m_gpsData.nTime.nSecond = g_fileSysApi.pGdata->timerData.nSecond;
			m_gpsData.nTime.nMinute = g_fileSysApi.pGdata->timerData.nMinute;
			m_gpsData.nTime.nHour = g_fileSysApi.pGdata->timerData.nHour;
			m_gpsData.nTime.nDay = g_fileSysApi.pGdata->timerData.nDay;
			m_gpsData.nTime.nMonth = g_fileSysApi.pGdata->timerData.nMonth;
			m_gpsData.nTime.nYear = g_fileSysApi.pGdata->timerData.nYear;
		}

		g_fileSysApi.pGdata->carInfo.nCarAngleRaw = m_gpsData.nAngle;
		m_bGetNewGpsData = nuTRUE;
		//?j????????
		g_fileSysApi.pGdata->timerData.bCanUpdateTime	= 1;
		if( m_gpsData.nStatus == DATA_TYPE_INVALID /*!g_fileSysApi.pGdata->uiSetData.bFirstGPSReady*/ )
		{
			return ;
		}
		//
#ifndef ANDROID
		if( m_gpsData.nStatus == DATA_TYPE_GPS )
		{
			g_fileSysApi.pGdata->timerData.bCanUpdateTime	= 1;
		}
		else
		{
			g_fileSysApi.pGdata->timerData.bCanUpdateTime	= 0;
		}
#endif
		//
		nuDWORD	nSecond = ((m_gpsData.nTime.nDay * 24 + m_gpsData.nTime.nHour) * 60 + m_gpsData.nTime.nMinute) * 60 + m_gpsData.nTime.nSecond;
		if( m_nOldTimeForSpeed == 0 || m_gpsData.nSpeed < 5 )//First settime
		{
			m_nOldTimeForSpeed	= nuGetTickCount();
		}
		else
		{
			nuDWORD nNowTick = nuGetTickCount();
			m_nTotalDisGone += m_gpsData.nSpeed * (nNowTick - m_nOldTimeForSpeed) / 36;
			m_nTotalTimeForSpeed += nNowTick - m_nOldTimeForSpeed;
			m_nOldTimeForSpeed	= nNowTick;
			if( m_gpsData.nSpeed > m_nHighSpeed )
			{
				m_nHighSpeed	= m_gpsData.nSpeed;
			}
		}
		//For Pend Trace
		if( m_pTraceFile && !g_fileSysApi.pGdata->timerData.bIsRouting )
		{
			m_pTraceFile->PendTrace(m_gpsData.nLongitude, m_gpsData.nLatitude);
		}
	}
}

nuBOOL CNaviThread::NuroTMCRoute(nuLONG lTmcRouteCount)//added by daniel 20110830
{
	m_nTMCDataCount = lTmcRouteCount;
	CODENODE codeNode    = {0};
	codeNode.nActionType = AC_NAVI_TMC_ROUTE;
	m_codeNavi.PushNode(&codeNode);
	m_drawToUI.bUIWaiting	= 1;
	while( !m_bQuit && m_drawToUI.bUIWaiting )
	{
		nuSleep(25);
	}
	m_drawToUI.bUIWaiting = 0;
	return nuTRUE;
}

nuBOOL CNaviThread::NuroTOBEGETVERSION()
{

	return nuFALSE;
}
nuBOOL CNaviThread::NuroGetTMCPathData(nuUINT* pbufcount,nuPVOID pvoid)
{
	while (g_fileSysApi.pGdata->drawInfoNavi.bTMCListUsed)
	{
		nuSleep(50);

	}
	__android_log_print(ANDROID_LOG_INFO, "TMC", "NuroGetTMCPathData nTMCCount: %ld", g_fileSysApi.pGdata->drawInfoNavi.nTMCCount);
	g_fileSysApi.pGdata->drawInfoNavi.bTMCListUsed = 1;
	if (pvoid && pbufcount && g_fileSysApi.pGdata->drawInfoNavi.pTMCList)
	{
		if (*pbufcount > g_fileSysApi.pGdata->drawInfoNavi.nTMCCount)
		{
			//just test
			nuMemcpy(pvoid,g_fileSysApi.pGdata->drawInfoNavi.pTMCList,sizeof(TMCDATA)*g_fileSysApi.pGdata->drawInfoNavi.nTMCCount);
			*pbufcount = g_fileSysApi.pGdata->drawInfoNavi.nTMCCount;
			__android_log_print(ANDROID_LOG_INFO, "TMC", "NuroGetTMCPathData nTMCCount: %ld", *pbufcount );
		}
		else if(g_fileSysApi.pGdata->drawInfoNavi.nTMCCount < 100)//< or =;
		{
			nuMemcpy(pvoid,g_fileSysApi.pGdata->drawInfoNavi.pTMCList,sizeof(TMCDATA)*(*pbufcount));
			*pbufcount = g_fileSysApi.pGdata->drawInfoNavi.nTMCCount;
			__android_log_print(ANDROID_LOG_INFO, "TMC", "NuroGetTMCPathData nTMCCount2: %ld", *pbufcount );
		}
	}
	g_fileSysApi.pGdata->drawInfoNavi.bTMCListUsed = 0;
	return nuTRUE;
}
nuBOOL CNaviThread::NuroTOBERESETGYRO()
{
	CODENODE codeNode	 = {0};
	codeNode.nActionType = AC_GPSSET_TOBERESETGYRO;
	m_codeNavi.PushNode(&codeNode);
/*
	m_drawToUI.bUIWaiting	= 1;
	while( !m_bQuit && m_drawToUI.bUIWaiting )
	{
		nuSleep(25);
	}
	m_drawToUI.bUIWaiting = 0;
*/
	return nuTRUE;
}
nuBOOL CNaviThread::NuroSetNaviThread(nuBYTE nState)
{
	if( m_nThdState	== nState )
	{
		return nuTRUE;
	}
//	if( m_bIsMoving )
	//	{
	//		return nuFALSE;
	//	}
	__android_log_print(ANDROID_LOG_INFO, "Mouse", "NuroSetNaviThread nState %d\n", nState);
    NuroMoveMap(MOVE_STOP, nuFALSE);
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_SETNAVITHD;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= nState;
	m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	while( m_nThdState != nState )
	{
		nuSleep(25);
//		nuShowString(nuTEXT("SetnaviThread sleep"));
	}
//	nuShowString(nuTEXT("SetnaviThread Out"));
	return nuTRUE;
}
/*
nuBOOL CNaviThread::ShilftNaviThread(nuBOOL bOpenOrClose)
{
	if( bOpenOrClose )
	{
		if( !m_bStopThread )
		{
			return nuTRUE;
		}
		m_bStopThread = nuFALSE;
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_RESUMETHREAD;
		codeNode.nDrawType		= DRAWTYPE_UI;
		codeNode.nExtend		= 0;
		m_codePre.PushNode(&codeNode);
		return nuTRUE;
	}
	else
	{
		if( m_bStopThread )
		{
			return nuTRUE;
		}
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_PAUSETHREAD;
		codeNode.nDrawType		= DRAWTYPE_UI;
		codeNode.nExtend		= 0;
		m_codePre.PushNode(&codeNode);
		while( !m_bStopThread )
		{
			nuSleep(25);
		}
		return nuTRUE;
	}
}
*/
nuBOOL CNaviThread::NuroSetVoice(nuWORD nVoiceNum, nuBOOL bShowIcon/* = nuTRUE*/)
{
	
	g_fileSysApi.pGdata->timerData.bShowVoiceIcon	= bShowIcon;
	g_fileSysApi.pGdata->timerData.nShowVoiceTimer	= 0;
	g_fileSysApi.pGdata->timerData.nVoiceNume		= nVoiceNum;
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_SETVOICE;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= bShowIcon;
	codeNode.nX				= nVoiceNum;
	return m_codeDraw.PushNode(&codeNode);
}

nuBOOL CNaviThread::GpsSendIn(nuBYTE *pBuff, nuUINT nCount)
{
#ifdef NURO_OS_UCOS
	if( m_nGpsBuffLen + nCount > GPS_BUFF_SIZE )
	{
		m_nGpsBuffLen = 0;
	}
	nuMemcpy(&m_nGpsBuff[m_nGpsBuffLen], pBuff, nCount);
	m_nGpsBuffLen += nCount;
	return nuTRUE;
#endif
	return nuTRUE;
}

nuPVOID CNaviThread::NuroGetRoutingList(int index, int *ptotalCount)
{
	if( !g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		return NULL;
	}
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		*ptotalCount = g_innaviApi.IN_GetRoutingList(m_pRoutingList, ROUTINGLIST_PAGE_MAX_NUM, index);
		return m_pRoutingList;
	}
	else
	{
		CODENODE codeNode	= {0};
		codeNode.nX			= index;
		codeNode.nDrawType	= DRAWTYPE_UI;
		codeNode.nActionType= AC_DRAW_GETROUTELIST;
		m_drawToUI.bUIWaiting	= 1;
		m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		while( !m_bQuit && m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		*ptotalCount = m_drawToUI.nReserve;
		return m_pRoutingList;
	}
}

nuPVOID CNaviThread::NuroGetMapInfo(nuUINT nMapCode)
{
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		if( SetMapInfo(nMapCode) )
		{
			return &m_mapInfo;
		}
	}
	else
	{
		CODENODE codeNode	= {0};
		codeNode.nX			= nMapCode;
		codeNode.nDrawType	= DRAWTYPE_UI;
		codeNode.nActionType= AC_DRAW_GETMAPINFO;
		m_drawToUI.bUIWaiting	= 1;
		m_drawToUI.nReturnCode	= 0;
		m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		while( !m_bQuit && m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		};
		if( m_drawToUI.nReturnCode )
		{
			return &m_mapInfo;
		}
	}
	return NULL;
}

nuBOOL CNaviThread::SetMapInfo(nuUINT nMapCode)
{
	if( nMapCode == MAP_INFO_CENTER )
	{
		/*
		if( !g_fileSysApi.pGdata->drawInfoMap.bFindCenterRoad )
		{
			return nuFALSE;
		}
		*/
		m_mapInfo.nCenterType	= g_fileSysApi.pGdata->drawInfoMap.nCenterNameType;
		m_mapInfo.ptCenterRaw.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
		m_mapInfo.ptCenterRaw.y = g_fileSysApi.pGdata->transfData.nMapCenterY;
		m_mapInfo.nCityCode		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nCityID;
		m_mapInfo.nTownCode		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nTownID;
		m_mapInfo.ptCenterFixed	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
		m_mapInfo.nDis			= (short)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDis;
		m_mapInfo.nFixedFileIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
		m_mapInfo.nFixedBlockIdx= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
		m_mapInfo.nFixedRoadIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
		m_mapInfo.nNameAddr		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nNameAddr;
//		nuWcsncpy(m_mapInfo.wsCTname, g_fileSysApi.pGdata->drawInfoMap.wsCityTown, CITYTOWN_NAME_MAXNUM - 1);
		g_fileSysApi.FS_ReadCityTownName( m_mapInfo.nFixedFileIdx,
			nuMAKELONG(m_mapInfo.nTownCode, m_mapInfo.nCityCode),
			m_mapInfo.wsCTname, 
			CITYTOWN_NAME_MAXNUM );
		nuWcsncpy(m_mapInfo.wsCenterName, g_fileSysApi.pGdata->drawInfoMap.wsCenterName, CENTER_NAME_MAX_NUM - 1);
		if( !g_fileSysApi.pGdata->drawInfoMap.bFindCenterRoad )
		{
			m_mapInfo.nDis = -1;
		}
		return nuTRUE;
	}
	else if( nMapCode == MAP_INFO_CENTERPOI )
	{
		if( g_fileSysApi.pGdata->drawInfoMap.bFindCenterPoi )
		{
			m_mapInfo.nCenterType	= g_fileSysApi.pGdata->drawInfoMap.nCenterNameType;
			m_mapInfo.ptCenterRaw.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
			m_mapInfo.ptCenterRaw.y = g_fileSysApi.pGdata->transfData.nMapCenterY;
			m_mapInfo.nCityCode		= g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nCityID;
			m_mapInfo.nTownCode		= g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nTownID;
			m_mapInfo.ptCenterFixed	= g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.pCoor;
			m_mapInfo.nDis			= (short)g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nDis;
			m_mapInfo.nFixedFileIdx	= -1;
			m_mapInfo.nNameAddr		= g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.nNameAddr;
			m_mapInfo.nOtherInfo	= g_fileSysApi.pGdata->drawInfoMap.poiCenterOn.OnterInfoAddr;
//			nuWcsncpy(m_mapInfo.wsCTname, g_fileSysApi.pGdata->drawInfoMap.wsCityTown, CITYTOWN_NAME_MAXNUM - 1);
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pGdata->drawInfoMap.nCenterMapIdx,
				nuMAKELONG(m_mapInfo.nTownCode, m_mapInfo.nCityCode),
				m_mapInfo.wsCTname, 
				CITYTOWN_NAME_MAXNUM );
			nuWcsncpy(m_mapInfo.wsCenterName, g_fileSysApi.pGdata->drawInfoMap.wsCenterPoi, CENTER_NAME_MAX_NUM - 1);
			return nuTRUE;
		}
		else if( g_fileSysApi.pGdata->drawInfoMap.nNowCenterPoiCount > 0 )
		{
			m_mapInfo.ptCenterRaw.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
			m_mapInfo.ptCenterRaw.y = g_fileSysApi.pGdata->transfData.nMapCenterY;
			m_mapInfo.nCityCode		= g_fileSysApi.pGdata->drawInfoMap.nCenterCityID;
			m_mapInfo.nTownCode		= g_fileSysApi.pGdata->drawInfoMap.nCenterTownID;
			m_mapInfo.nFixedFileIdx	= -1;
			m_mapInfo.ptCenterFixed = g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[0].pCoor;
			m_mapInfo.nDis			= (short)g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[0].nDis;
			m_mapInfo.nNameAddr		= g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[0].nNameAddr;
			m_mapInfo.nOtherInfo	= g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[0].OnterInfoAddr;
			
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pGdata->drawInfoMap.nCenterMapIdx,

				nuMAKELONG(m_mapInfo.nTownCode, m_mapInfo.nCityCode),
				m_mapInfo.wsCTname, 
				CITYTOWN_NAME_MAXNUM );
			return nuTRUE;
		}
		return nuFALSE;
	}
	else if( nMapCode == MAP_INFO_GPS )
	{
		if( !g_fileSysApi.pGdata->uiSetData.bFirstGPSReady )
		{
			return nuFALSE;
		}
		m_mapInfo.ptCenterRaw.x	= m_gpsData.nLongitude;
		m_mapInfo.ptCenterRaw.y = m_gpsData.nLatitude;
		m_mapInfo.nFixedFileIdx	= -1;
		return nuTRUE;
	}
	else if( nMapCode == MAP_INFO_CAR )
	{
		m_mapInfo.ptCenterRaw = g_fileSysApi.pGdata->carInfo.ptCarRaw;
		if( g_fileSysApi.pGdata->drawInfoMap.bCarInRoad )
		{
			m_mapInfo.nCityCode		= g_fileSysApi.pGdata->drawInfoMap.nCenterCityID;
			m_mapInfo.nTownCode		= g_fileSysApi.pGdata->drawInfoMap.nCenterTownID;
			m_mapInfo.ptCenterFixed = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
			m_mapInfo.nFixedFileIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
			m_mapInfo.nFixedBlockIdx= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
			m_mapInfo.nFixedRoadIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
			m_mapInfo.nNameAddr		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nNameAddr;
			m_mapInfo.nOtherInfo	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass;
		}
		else
		{
			m_mapInfo.ptCenterFixed = g_fileSysApi.pGdata->carInfo.ptCarRaw;//UI maybe used this parameter.
			m_mapInfo.nFixedFileIdx	= -1;
		}
		/*g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pGdata->drawInfoMap.nCenterMapIdx,
			nuMAKELONG( g_fileSysApi.pGdata->drawInfoMap.nCenterTownID, g_fileSysApi.pGdata->drawInfoMap.nCenterCityID ),
			m_mapInfo.wsCTname, 
			CITYTOWN_NAME_MAXNUM );*/
		g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx,
			nuMAKELONG(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nTownID, g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nCityID),
			m_mapInfo.wsCTname, 
			CITYTOWN_NAME_MAXNUM );
		//nuWcsncpy(m_mapInfo.wsCenterName, g_fileSysApi.pGdata->drawInfoMap.roadCarOn.wsRoadName, CENTER_NAME_MAX_NUM - 1);
		
		//add by xiaoqin and xiaochang
		RNEXTEND rnExtend;
	    g_fileSysApi.FS_GetRoadName( g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx, 
			g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nNameAddr, 
			g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx, 
			m_mapInfo.wsCenterName, 
			CENTER_NAME_MAX_NUM, 
			&rnExtend );
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID CNaviThread::SetMoveWait(nuWORD nType /* = WAIT_TYPE_MOVE */, 
							  nuBOOL bOnlyChangeTimer /* = nuFALSE */, 
							  nuBYTE nWhoSet /* = WHO_SETMOVEWAIT_DEFAULT */)
{
	if( bOnlyChangeTimer )
	{
		g_fileSysApi.pGdata->timerData.nMoveWaitTimer	= 0;
	}
	else
	{
		if( !g_fileSysApi.pGdata->timerData.bMoveWait )
		{
__android_log_print(ANDROID_LOG_INFO, "Mouse", "SetMoveWait nAutoMoveType %d", g_fileSysApi.pGdata->timerData.nAutoMoveType);
			g_fileSysApi.pGdata->timerData.bMoveWait		= 1;
			g_fileSysApi.pGdata->timerData.nOldMoveType		= g_fileSysApi.pGdata->timerData.nAutoMoveType;
			g_fileSysApi.pGdata->timerData.nAutoMoveType	= AUTO_MOVE_TIMER;
			//g_drawIfmApi.IFM_SetState(MOUSE_BACKTOCAR, STATE_BUT_DEFAULT);
			//g_drawIfmApi.IFM_SetState(MOUSE_SHOWLONGLAT, STATE_BUT_DEFAULT);
			//g_drawIfmApi.IFM_SetState(MOUSE_SHOWCENTERNEARPOI, STATE_BUT_DEFAULT);
			//g_drawIfmApi.IFM_SetState(MOUSE_MAPSPEED, STATE_BUT_DEFAULT);
		}
		g_fileSysApi.pGdata->timerData.nMoveWaitTimer	= 0;
		g_fileSysApi.pGdata->timerData.nMoveWaitType	= nType;
	}
}

nuVOID CNaviThread::CoverFromMoveWait()
{
	if( g_fileSysApi.pGdata->timerData.bMoveWait )
	{
		g_fileSysApi.pGdata->timerData.bMoveWait		= 0;
		g_fileSysApi.pGdata->timerData.nMoveWaitType	= WAIT_TYPE_NOWAIT;
		if( g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_GPS && 
			g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION )
		{
			g_fileSysApi.pGdata->timerData.nAutoMoveType	= g_fileSysApi.pGdata->timerData.nOldMoveType;
		}
		//g_drawIfmApi.IFM_SetState(MOUSE_BACKTOCAR, STATE_BUT_HIDE);
		//g_drawIfmApi.IFM_SetState(MOUSE_SHOWLONGLAT, STATE_BUT_HIDE);
		//g_drawIfmApi.IFM_SetState(MOUSE_SHOWCENTERNEARPOI, STATE_BUT_HIDE);
		//g_drawIfmApi.IFM_SetState(MOUSE_MAPSPEED, STATE_BUT_DEFAULT);
//		g_drawIfmApi.IFM_SetState(MOUSE_LIMITSPEED, STATE_BUT_DEFAULT);
	}
}

#define SIM_DIS_TO_END					5
#define GPS_DIS_TO_END					30
nuBYTE CNaviThread::CheckNaviFixed(nuUINT nRes, PACTIONSTATE pActionState)
{
	nuBYTE nReturn = 0;
	nuBOOL bColNaviInfo = nuFALSE;
	if( nRes != INNAVI_POINT_TO_NAVI_RIGHT && pActionState->nDrawMode == DRAWMODE_SIM_START )
	{
		g_innaviApi.IN_ColNaviInfo();
		if(!m_bFullViewMode)
		{
			g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = g_innaviApi.IN_JudgeZoomRoad();	
		}
	}
	if( nRes != INNAVI_POINT_TO_NAVI_ERROR )
	{
		if( nRes == INNAVI_POINT_TO_NAVI_RIGHT )
		{
			nuBYTE nLastPastNode  = g_fileSysApi.pUserCfg->nPastMarket;
			if( g_innaviApi.IN_ColNaviInfo() )
			{
				//check near
				/*nuDWORD nDis = SIM_DIS_TO_END;// revised by daniel for GPS and Route Near Target have the same situation  20111004
				if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS )
				{
					nDis = m_nDisToEnd; //GPS_DIS_TO_END;
				}*/ 
				//
				if( g_fileSysApi.pGdata->drawInfoNavi.nDisToTarget < m_nDisToEnd )
				{
					m_bRouteEnd = nuTRUE;
					g_pSoundThread->CleanSound();//added by daniel 20111230
					EndVoiceHandle();
					//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_NAVIEND, 0, NULL);
					if( g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
					{
						g_fileSysApi.pGdata->carInfo.nCarAngle = g_fileSysApi.pGdata->carInfo.nCarAngleRaw;//added by daniel 20120131
__android_log_print(ANDROID_LOG_INFO, "navi", "CheckNaviFixed, nAutoMoveType %d", g_fileSysApi.pGdata->timerData.nAutoMoveType);
						SetMoveWait(WAIT_TYPE_SIMEND);
						m_bSimuReachDes = nuTRUE;
					}
					else
					{
						CODENODE codeNode = {0};
						codeNode.nActionType = AC_NAVI_NEAREND;
						m_codeNavi.PushNode(&codeNode);
					}
					g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = CROSSNEARTYPE_COVER;
					g_fileSysApi.pGdata->drawInfoNavi.nCrossZoomType = CROSSZOOM_TYPE_DEFAULT;
				}
				else
				{
					m_bSimuReachDes = nuFALSE;
					if( !(pActionState->nDrawMode & 0x0F) || g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID != 0 )
					{
						if( !(pActionState->nDrawMode & 0x0F))
						{
							g_innaviApi.IN_PlayNaviSound();
						}
					}
					if(!m_bFullViewMode)
					{
						g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType = g_innaviApi.IN_JudgeZoomRoad();
					}
				}
				if( pActionState->nDrawMode == DRAWMODE_SIM_START )
				{
					g_fileSysApi.pDrawInfo->drawInfoCommon.nRealPicID = 0;
				}
			}
			if( nLastPastNode != g_fileSysApi.pUserCfg->nPastMarket )
			{
				g_fileSysApi.FS_SaveUserConfig();//Save the changeMode of NaviPoints
			}
			nReturn = 1;
		}
		else
		{
			//Play sound please u-turn
			if( (g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_NAVIERROR) && 
				((pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS) )
			{
//				g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_UTURN, 0, NULL);
				//g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_SYN, 1, 0, 0, 0, NULL);
			}
		}
	}

	else
	{
		//Play sound out of NaviLine
		if( (g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_FIXEDERROR) &&
			((pActionState->nDrawMode & 0xF0) == DRAWMODE_GPS) )
		{
//			g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_NAVIDEPARTURE, 0, NULL);
//			g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_SYN, 1, 0, 0, 0, NULL);
		}
	}
	//
	if( nReturn )
	{
		m_nNaviErrorTimer	= 0;
	}
	else


	{
		if( g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_FIXEDERROR )
		{
//			g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_SYN, 1, 0, 0, 0, NULL);
		}
		m_nNaviErrorTimer ++;
	}
	return nReturn;
}

nuVOID CNaviThread::DataExchange()
{
//#ifdef NURO_OS_UCOS
	/*if( g_fileSysApi.pGdata->timerData.nGetGpsTime	>= MAX_GET_GPS_TIME_MS )
	{
		IntervalGetGps();
		g_fileSysApi.pGdata->timerData.nGetGpsTime = 0;
	}*/
//#endif
	if( g_fileSysApi.pGdata->uiSetData.bFirstGPSReady && //2009.09.04
		g_fileSysApi.pGdata->timerData.nDayNightChangeTimer >= MAX_DAY_NIGHT_CHANGE )
	{
		g_fileSysApi.pGdata->timerData.nDayNightChangeTimer = 0;
		if( g_fileSysApi.pUserCfg->bAutoDayNight == DAYNIGHT_MODE_AUTO )
		{
			nuBYTE nHourToDay = g_fileSysApi.pUserCfg->nTimeToDay/10;
			nuBYTE nMinuteToDay = (g_fileSysApi.pUserCfg->nTimeToDay%10)*6;
			nuBYTE nHourToNight = g_fileSysApi.pUserCfg->nTimeToNight/10;
			nuBYTE nMinuteToNight = (g_fileSysApi.pUserCfg->nTimeToNight%10)*6;
			nuBOOL bDay = nuFALSE;
			if( ( g_fileSysApi.pGdata->timerData.nHour > nHourToDay	|| 
				( g_fileSysApi.pGdata->timerData.nHour == nHourToDay && g_fileSysApi.pGdata->timerData.nMinute >= nMinuteToDay) ) &&
				( g_fileSysApi.pGdata->timerData.nHour < nHourToNight ||
				( g_fileSysApi.pGdata->timerData.nHour == nHourToNight && g_fileSysApi.pGdata->timerData.nMinute < nMinuteToNight ) ) )
			{
				bDay = nuTRUE;
			}
			if( g_fileSysApi.pGdata->uiSetData.bNightOrDay )
			{
				if( bDay )
				{
					g_fileSysApi.pUserCfg->bNightDay	= 0;
					NuroSetUserCfg();
				}
			}
			else
			{
				if( !bDay )
				{
					g_fileSysApi.pUserCfg->bNightDay	= 1;
					NuroSetUserCfg();
				}
			}
		}
	}
}

nuUINT CNaviThread::MsgBoxEvent()
{
	return 0;
/*	if( m_drawToUI.nReturnCode == MOUSE_MSGBOX_BUT_OK )
	{
		NuroNavigation(NAVIGATION_STOP);
		m_drawToUI.nReturnCode = MOUSE_MSGBOX_OUT;
	}
	return m_drawToUI.nReturnCode;*/
}

nuBOOL CNaviThread::NuroMessageBox(nuUINT nMsgBoxCode, nuPVOID lpVoid)
{
	return nuFALSE;
    /*if( NAVI_THD_WORK != m_nThdState )
    {
        m_nNewMsgType = nMsgBoxCode;
        //g_drawIfmApi.IFM_SetState(MOUSE_MSGBOX_OUT, STATE_BUT_DEFAULT);
        return nuTRUE;
    }
    else
    {
        CODENODE codeNode = {0};
        codeNode.nActionType	= AC_DRAW_MSGBOX;
        codeNode.nX				= nMsgBoxCode;
        m_drawToUI.bUIWaiting   = 1;
        if( !m_codeDraw.PushNode(&codeNode) )
        {
            m_drawToUI.bUIWaiting   = 0;
            return nuFALSE;
        }
        else
        {
            while( !m_bQuit && m_drawToUI.bUIWaiting )
            {
                nuSleep(25);
            }
            return nuTRUE;
        }
    }*/
	/*
	CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_MSGBOX;
		codeNode.nX	= nMsgBoxCode;
		return m_codeDraw.PushNode(&codeNode);*/
	
}

nuBOOL CNaviThread::NuroShowHideButton(nuUINT nButCode, nuBYTE nState)
{
	return nuFALSE;
	/*if( m_nThdState != NAVI_THD_WORK )//New StopNTD
	{
		//g_drawIfmApi.IFM_SetState(nButCode, nState);
		return nuTRUE;
	}
	else
	{
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_SHOWHIDEBUT;
		codeNode.nX				= nButCode;
		codeNode.nExtend		= nState;
        m_drawToUI.bUIWaiting   = 1;
		if( !m_codeDraw.PushNode(&codeNode) )
        {
            m_drawToUI.bUIWaiting   = 0;
            return nuFALSE;
        }
        else
        {
            while( !m_bQuit && m_drawToUI.bUIWaiting )
            {
                nuSleep(25);
            }
            return nuTRUE;
        }
	}*/
}

nuBOOL CNaviThread::GetTripInfo(nuPVOID lpInfo)
{
	if( lpInfo == NULL )
	{
		return nuFALSE;
	}
	PTRIPDATA pTripData = (PTRIPDATA)lpInfo;
	pTripData->nTotalDisGone	= m_nTotalDisGone / 100;
	pTripData->nTotalTimeGone	= (nuGetTickCount() - g_fileSysApi.pGdata->timerData.nSysStartTick)/ 1000;
	pTripData->nHighestSpeed	= m_nHighSpeed;
	if( m_nTotalTimeForSpeed == 0 )
	{
		pTripData->nAverageSpeed = 0;
	}
	else
	{
		pTripData->nAverageSpeed	= m_nTotalDisGone * 36 / m_nTotalTimeForSpeed;
	}
	pTripData->nHightAboveSea	= m_gpsData.nAntennaHeight; 
	pTripData->nSecond			= g_fileSysApi.pGdata->timerData.nSecond;
	pTripData->nMinute			= g_fileSysApi.pGdata->timerData.nMinute;
	pTripData->nHour			= (nuBYTE)g_fileSysApi.pGdata->timerData.nHour;
	pTripData->nDay				= g_fileSysApi.pGdata->timerData.nDay;
	pTripData->nYear			= g_fileSysApi.pGdata->timerData.nYear;
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		pTripData->nDisToTarget		= g_fileSysApi.pGdata->drawInfoNavi.nDisToTarget;
		pTripData->nTimeToTarget	= g_fileSysApi.pGdata->drawInfoNavi.nTimeToTarget;
	}
	else
	{
		pTripData->nDisToTarget		= -1;
		pTripData->nTimeToTarget	= -1;
	}
	return nuTRUE;
}

nuBOOL CNaviThread::ReSetTripInfo(nuPVOID lpInfo)
{
	PTRIPDATA pTripData = (PTRIPDATA)lpInfo;
	m_nTotalDisGone = 0;
	m_nTotalTimeForSpeed = 0;
	g_fileSysApi.pGdata->timerData.nSysStartTick = nuGetTickCount();
	m_nHighSpeed = 0;
	//
	pTripData->nTotalDisGone	= m_nTotalDisGone / 100;
	pTripData->nTotalTimeGone	= (nuGetTickCount() - g_fileSysApi.pGdata->timerData.nSysStartTick)/ 1000;
	pTripData->nHighestSpeed	= m_nHighSpeed;
	if( m_nTotalTimeForSpeed == 0 )
	{
		pTripData->nAverageSpeed = 0;
	}
	else
	{
		pTripData->nAverageSpeed	= m_nTotalDisGone * 36 / m_nTotalTimeForSpeed;
	}
	pTripData->nHightAboveSea	= m_gpsData.nAntennaHeight;
	pTripData->nSecond			= g_fileSysApi.pGdata->timerData.nSecond;
	pTripData->nMinute			= g_fileSysApi.pGdata->timerData.nMinute;
	pTripData->nHour			= (nuBYTE)g_fileSysApi.pGdata->timerData.nHour;
	pTripData->nDay				= g_fileSysApi.pGdata->timerData.nDay;
	pTripData->nYear			= g_fileSysApi.pGdata->timerData.nYear;
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		pTripData->nDisToTarget		= g_fileSysApi.pGdata->drawInfoNavi.nDisToTarget; //路程
		pTripData->nTimeToTarget	= g_fileSysApi.pGdata->drawInfoNavi.nTimeToTarget;//时间
	}
	else
	{
		pTripData->nDisToTarget		= -1;
		pTripData->nTimeToTarget	= -1;
	}
	return nuTRUE;
}

nuBOOL CNaviThread::NuroConfigNavi(nuLONG nDis)
{
	CODENODE codeNode = {0};
	codeNode.nActionType = AC_NAVI_CONFIGROUTE; //???u????路??
	codeNode.nExtend	= nDis;
	
	m_drawToUI.bUIWaiting	= 1;
	if( !m_codeNavi.PushNode(&codeNode) )
	{
		m_drawToUI.bUIWaiting = 0;
		return nuFALSE;
	}
    else
    {
        nuSleep(25);
        return nuTRUE;
    }
    /*
	while( !m_bQuit && m_drawToUI.bUIWaiting )
	{
		nuSleep(25);
	}
	return g_fileSysApi.pGdata->uiSetData.bNavigation;
    */
}

nuBOOL CNaviThread::NuroSetDrawInfo(nuPVOID pSetData)
{
	//g_drawIfmApi.IFM_SetStateArray((PDFMSETTING)pSetData);
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD 
//	if( m_bStopThread )
	{
		//g_drawIfmApi.IFM_SetStateArray((PDFMSETTING)pSetData);
		return nuTRUE;
	}
	else
	{
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_SETDRAWINFO;
		codeNode.nExtend		= (nuLONG)(pSetData);
		m_drawToUI.bUIWaiting   = 1;
		if( !m_codeDraw.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting =  0;
		}
		while( m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		return nuTRUE;
	}
}
nuBOOL CNaviThread::NuroSetScale(nuLONG nScaleIdx)
{
	if( m_nThdState != NAVI_THD_WORK )//New StopNTD
//	if( m_bStopThread )
	{
		return nuFALSE;
	}
	else
	{
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_SETSCALE;
		codeNode.nExtend		= nScaleIdx;
		if( !m_codeDraw.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting =  0;
		}
		return nuTRUE;
	}
}

nuBOOL CNaviThread::NuroSetMAPPT(nuBYTE nState)
{
	/*if( m_nThdState != NAVI_THD_WORK )//New StopNTD
		//	if( m_bStopThread )
	{
		return nuFALSE;
	}
	else*/
	{
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_SETMAPPT; 
		codeNode.nExtend		= nState;
		if( !m_codeDraw.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting =  0;
		}
		return nuTRUE;
	}
}

nuBYTE CNaviThread::ColBmpExtend()
{
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		return BMPSIZE_EXTEND_NO;
	}
	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH ) //??????? ??
	{
		short nAngle = g_fileSysApi.pGdata->carInfo.nCarAngle; //??????
		while( nAngle < 0 )
		{
			nAngle += 360;
		}

		nAngle %= 360;
		nuBYTE nRes = BMPSIZE_EXTEND_NO;
		if( nAngle >= 0 && nAngle <= 180 )
		{
			nRes |= BMPSIZE_EXTEND_UP;
		}
		else
		{
			nRes |= BMPSIZE_EXTEND_DOWN;
		}
		//
		if( nAngle >= 90 && nAngle <= 270 )
		{
			nRes |= BMPSIZE_EXTEND_LEFT;
		}
		else
		{
			nRes |= BMPSIZE_EXTEND_RIGHT;
		}
		return nRes;
	}
	else
	{
		return BMPSIZE_EXTEND_UP;
	}
}

nuBOOL CNaviThread::DrawCrossZoom(PACTIONSTATE pAcState)
{
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		if( !m_nOldNaviState )
		{
			m_nOldNaviState = g_fileSysApi.pGdata->uiSetData.bNavigation;
			m_nLastCrossIndex = -1;
		}
		//
		if( !pAcState->bReDrawCrossZoom &&
			/*m_nLastCrossIndex == g_fileSysApi.pGdata->drawInfoNavi.nextCross.nCrossIndex && */
			!g_fileSysApi.pGdata->drawInfoNavi.nextCross.nChangeCross 
		 && !g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nChangeCross )
		{
			return nuFALSE;
		}
	}
	else
	{
		m_nOldNaviState = 0;
		return nuFALSE;
	}
	ZOOMCROSSSTATE zoomState;
	if( g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nChangeCross )
	{
		m_gDataMgr.ZoomMapSetup(g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.point, 
			g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nAngleInDx,
			g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nAngleInDy, 
			g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nRealPicID);
		//		g_fileSysApi.pGdata->zoomScreenData.nPicID = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID;
		zoomState.nPicID   = g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nRealPicID;
		zoomState.ptCenter = g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.point;
		zoomState.nCenterRdBlkIdx	= g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nBlkIdx;
		zoomState.nCenterRdIdx	= g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nRoadIdx;
		m_nLastCrossBlkIdx	= zoomState.nCenterRdBlkIdx;
		m_nLastCrossRdIdx	= zoomState.nCenterRdIdx;
		g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nChangeCross = 0;
	}






	else if( g_fileSysApi.pGdata->drawInfoNavi.nextCross.nChangeCross )
	{
		m_gDataMgr.ZoomMapSetup(g_fileSysApi.pGdata->drawInfoNavi.nextCross.point, 
			g_fileSysApi.pGdata->drawInfoNavi.nextCross.nAngleInDx,
			g_fileSysApi.pGdata->drawInfoNavi.nextCross.nAngleInDy, 
			g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID);
//		g_fileSysApi.pGdata->zoomScreenData.nPicID = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID;
		zoomState.nPicID   = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID;
		zoomState.ptCenter = g_fileSysApi.pGdata->drawInfoNavi.nextCross.point;
		zoomState.nCenterRdBlkIdx	= g_fileSysApi.pGdata->drawInfoNavi.nextCross.nBlkIdx;
		zoomState.nCenterRdIdx	= g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRoadIdx;
		m_nLastCrossBlkIdx	= zoomState.nCenterRdBlkIdx;
		m_nLastCrossRdIdx	= zoomState.nCenterRdIdx;
		g_fileSysApi.pGdata->drawInfoNavi.nextCross.nChangeCross = 0;
	}
	else
	{
		if( g_fileSysApi.pGdata->zoomScreenData.nPicID )
		{
			return nuFALSE;
		}
		zoomState.nPicID	= 0;
		zoomState.ptCenter.x	= g_fileSysApi.pGdata->zoomScreenData.nZoomMapCenterX;
		zoomState.ptCenter.y	= g_fileSysApi.pGdata->zoomScreenData.nZoomMapCenterY;
		zoomState.nCenterRdBlkIdx	= m_nLastCrossBlkIdx;
		zoomState.nCenterRdIdx		= m_nLastCrossRdIdx;
	}
	g_drawmapApi.DM_DrawZoomRoad(&zoomState);
//	g_mathtoolApi.MT_SaveOrShowBmp(SAVE_ZOOM_MAP);
	g_fileSysApi.pGdata->zoomScreenData.nPicID = zoomState.nPicID;
	m_nLastCrossIndex = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nCrossIndex;
	return nuTRUE;
}

nuUINT CNaviThread::TraceAction(nuUINT nActionType, nuDWORD nCode /* = 0 */, nuPVOID lp /* = NULL */)
{
	CODENODE codeNode		= {0};
	if( !m_pTraceFile )
	{
		return DRAW_NO;
	}
	switch (nActionType)
	{
	case TRACE_ACTION_GETHEAD:
		return (nuUINT)m_pTraceFile->GetTraceHead();
	case TRACE_ACTION_STARTRECORD:
		return m_pTraceFile->StartTrace(nuLOBYTE(nuLOWORD(nCode)));
	case TRACE_ACTION_CLOSERECORD:
		return m_pTraceFile->CloseTrace(nuLOBYTE(nuLOWORD(nCode)));
	case TRACE_ACTION_DELETE:
		return m_pTraceFile->DeleteTrace(nuLOWORD(nCode));
	case TRACE_ACTION_SHOW:
		codeNode.nActionType	= AC_DRAW_TRACEACTION;
		codeNode.nX				= nCode;
		codeNode.nExtend		= nActionType;
		m_drawToUI.bUIWaiting   = 1;
		if( !m_codePre.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting =  0;
		}
		while( m_drawToUI.bUIWaiting )
		{
			nuSleep(25);
		}
		return m_drawToUI.nReturnCode;
	case TRACE_ACTION_UNSHOW:
		return m_pTraceFile->CloseShow();
	case TRACE_ACTION_GETPOINT:
		{
			PTRACEPOINT pTracePoint = (PTRACEPOINT)lp;
			return m_pTraceFile->GetTracePoint(pTracePoint->point, nuLOWORD(nCode), pTracePoint->nIdx);
		}
	case TRACE_ACTION_EXPORT:
		return m_pTraceFile->ExportTrace();
	case TRACE_ACTION_IMPORT:
		return m_pTraceFile->ImportTrace();
	default:
		break;
	}
	return DRAW_NO;
}

nuUINT CNaviThread::NuroShowMap(nuBYTE nType)
{
	CODENODE codeNode = {0};
	if( nType == _UI_SHOW_MAP_ALL )
	{
		codeNode.nActionType = AC_DRAW_REDRAWMAP;
	}
	else if( nType == _UI_SHOW_MAP_DRAWINFO )
	{
		codeNode.nActionType = AC_DRAW_REDRAWINFO;
	}
	else if( nType == _UI_SHOW_MAP_MENUORMSG )
	{
		codeNode.nActionType = AC_DRAW_ONPAINT;
	}
	else
	{
		return 0;
	}
	return m_codeDraw.PushNode(&codeNode);
}
nuUINT CNaviThread::NuroSetMemoData(nuPVOID pVoid)
{
	/*
	if( pVoid == NULL )
	{
		return 0;
	}
	*/
	POTHERGLOBAL pOtherG = (POTHERGLOBAL)g_fileSysApi.FS_GetData(DATA_OTHERGLOBAL);
	if( pOtherG == NULL )
	{
		return 0;
	}
	while( m_bIsDrawing )
	{
		nuSleep(10);
	}
	m_bIsDrawing = nuTRUE;
	pOtherG->pMemoData = (PMEMODATA)pVoid;
	m_bIsDrawing = nuFALSE;
 
	// Requeset NaviCtrl ReDrawMap
	CODENODE codeNode = {0};
	codeNode.nActionType = AC_DRAW_REDRAWMAP;
	m_codeDraw.PushNode(&codeNode);
	// -------------------------------

	return 1;
}
//
nuUINT CNaviThread::NuroSetMemoPtData(nuPVOID pVoid)
{
	POTHERGLOBAL pOtherG = (POTHERGLOBAL)g_fileSysApi.FS_GetData(DATA_OTHERGLOBAL);
	if( pOtherG == NULL )
	{
		return 0;
	}
	while( m_bIsDrawing )
	{
		nuSleep(10);
	}
	m_bIsDrawing = nuTRUE;
	pOtherG->pMemoPtData = (PMEMOPTDATA)pVoid;
	m_bIsDrawing = nuFALSE;
	return 1;
}

nuUINT CNaviThread::NuroButtonEvent(nuUINT nCode, nuBYTE nButState)
{
	//	MARKETPOINT marketPoint;
	CODENODE codeNode = {0};
	switch(nCode)
	{
	case MOUSE_2D3D:
		if( g_fileSysApi.pUserCfg->b3DMode )
		{
			g_fileSysApi.pUserCfg->b3DMode = 0;
		}
		else
		{
			g_fileSysApi.pUserCfg->b3DMode = 1;
		}
		codeNode.nActionType	= AC_DRAW_SETUSERCFG;
		codeNode.nDrawType		= DRAWTYPE_UI;
		codeNode.nExtend		= SETUSERCFG_NOEXTEND;
		m_codeDraw.PushNode(&codeNode);
		break;
	case MOUSE_BACKTOCAR:
		codeNode.nActionType= AC_DRAW_BACKTOCAR;
		codeNode.nDrawType	= DRAWTYPE_UI;
		codeNode.nExtend	= SETUSERCFG_NOEXTEND;
		codeNode.nX			= g_fileSysApi.pGdata->carInfo.ptCarIcon.x;
		codeNode.nY			= g_fileSysApi.pGdata->carInfo.ptCarIcon.y;
		m_codeDraw.PushNode(&codeNode);
		break;
	case MOUSE_ZOOMIN:
		m_nAutoScaleTick = nuGetTickCount();
		NuroZoom(nuTRUE);
		break;
	case MOUSE_ZOOMOUT:
		m_nAutoScaleTick = nuGetTickCount();
		NuroZoom(nuFALSE);
		break;
	case MOUSE_DIRECTION: 
		if( m_bInNoNaviScale )
		{
			//return 0;
		}
		if( g_fileSysApi.pUserCfg->nMapDirectMode == MAP_DIRECT_NORTH )
		{
			g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_CAR;
		}
		else
		{
			g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_NORTH;
		}
		codeNode.nActionType	= AC_DRAW_SETUSERCFG;
		codeNode.nDrawType		= DRAWTYPE_UI;
		codeNode.nExtend		= SETUSERCFG_NOEXTEND;
		m_codeDraw.PushNode(&codeNode);
		break;
	case MOUSE_SPLITSCREEN: 
		codeNode.nActionType	= AC_DRAW_SPLITSCREEN;
		codeNode.nDrawType		= DRAWTYPE_UI;
		codeNode.nX				= nButState;
		m_codeDraw.PushNode(&codeNode);
		break;
	case MOUSE_SUBMENU:
		NuroShowHideButton(MOUSE_MENU_OUT, STATE_BUT_DEFAULT);
		break;
		/*
	case MOUSE_SIMULATION:
		NuroNavigation(NAVIGATION_SIMROUTE);
		return 0;
	case MOUSE_GPSNAVI:
		NuroNavigation(NAVIGATION_GPSROUTE);
		return 0;
	case MOUSE_STOPNAVI:
		NuroNavigation(NAVIGATION_IFSTOP);

		break;
	case MOUSE_ONLEYROUTE:
		NuroNavigation(NAVIGATION_ONLYROUTE);
		break;
		*/
	case MOUSE_HIDEMENU:
		NuroShowHideButton(MOUSE_MENU_OUT, STATE_BUT_HIDE);

		break;
		/*
	case MOUSE_SETSTART:
		marketPoint.index			= 0;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
	case MOUSE_SETPASSNODE:
		marketPoint.index			= 1;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
	case MOUSE_SETEND:
		MARKETPOINT marketPoint;
		marketPoint.index			= 2;
		marketPoint.bScreenPoint	= 1;
		marketPoint.nPointType		= POINT_TYPE_SCREEN_CENTER;
		marketPoint.bUpdateMap		= 1;
		NuroSetMarket(&marketPoint);
		break;
		*/
	case MOUSE_RETURNSTART:
//		if( !g_gpsopenApi.GPS_GetState() || 
//			!g_gpsopenApi.GPS_GetGpsXY(&pCodeNode->nX, &pCodeNode->nY) )
		if( m_gpsData.nStatus )
		{
			codeNode.nX = m_gpsData.nLongitude; //????
			codeNode.nY = m_gpsData.nLatitude; //纬??
		}
		else
		{
			codeNode.nX = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
			codeNode.nY = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
		}
		codeNode.nActionType	= AC_DRAW_BACKTOCAR;
		codeNode.nDrawType		= DRAWTYPE_UI;
		m_codeDraw.PushNode(&codeNode);
		break;
	default:
		return 0;
	}
	return 1;
}
nuUINT CNaviThread::SetScaleProc(nuLONG nNewScaleIdx, PACTIONSTATE pAcState, nuBOOL bZoomingAction /* = nuTRUE */)
{
	if( g_fileSysApi.pGdata->uiSetData.nScaleIdx == (nuBYTE)nNewScaleIdx ||
		nNewScaleIdx < 1 ||
		nNewScaleIdx >= g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount )
	{
		return DRAW_NO;
	}
	
	if( bZoomingAction )
	{
		if( nNewScaleIdx < g_fileSysApi.pGdata->uiSetData.nScaleIdx )


		{
			m_zoomDraw.m_nZoomType	= ZOOM_IN;
		}
		else
		{
			m_zoomDraw.m_nZoomType	= ZOOM_OUT;
		}
	}
	
	g_fileSysApi.pGdata->uiSetData.nScaleIdx = (nuBYTE)nNewScaleIdx;
	if( g_fileSysApi.pGdata->uiSetData.nScaleIdx == 1 )
	{
		g_fileSysApi.pGdata->drawInfoMap.nZoomType	= ZOOM_TYPE_NO_IN;
	}
	else if( g_fileSysApi.pGdata->uiSetData.nScaleIdx == g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount - 1)
	{
		g_fileSysApi.pGdata->drawInfoMap.nZoomType	= ZOOM_TYPE_NO_OUT;
	}
	else
	{
		g_fileSysApi.pGdata->drawInfoMap.nZoomType	= ZOOM_TYPE_DEFAULT; 
	}
	g_fileSysApi.pGdata->timerData.bShowScale	= 1;
	g_fileSysApi.pGdata->timerData.nShowScaleTimer = 0;
	pAcState->bDrawBmp1	= 1;
	pAcState->bDrawInfo	= 1;
	pAcState->bNeedReloadMap	= 1;
	pAcState->bResetMapCenter	= 1;
	pAcState->bResetScale		= 1;
	pAcState->nBmpSizeMode		= BMPSIZE_EXTEND_NO;
	pAcState->ptNewMapCenter.x	= g_fileSysApi.pGdata->transfData.nMapCenterX;
	pAcState->ptNewMapCenter.y	= g_fileSysApi.pGdata->transfData.nMapCenterY;
	nuBOOL bOnlyChangeTime = nuTRUE;
	if( nNewScaleIdx >= m_nScaleNoNavigation )
	{
		if( !m_bInNoNaviScale )  //Modified By Damon For LCMM
		{
			if( !g_fileSysApi.pGdata->timerData.bMoveWait )
			{
				m_ptLastMapCenter.x = g_fileSysApi.pGdata->transfData.nMapCenterX;
				m_ptLastMapCenter.y = g_fileSysApi.pGdata->transfData.nMapCenterY;
			}
			if( m_nLastScaleType == 1)
			{
				m_nLastMapDirectMode = g_fileSysApi.pGdata->uiSetData.nMapDirectMode;
				if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode != MAP_DIRECT_NORTH )
				{
					g_fileSysApi.pGdata->uiSetData.nMapDirectMode = g_fileSysApi.pUserCfg->nMapDirectMode = MAP_DIRECT_NORTH;
					m_gDataMgr.SetAngle(360);
					pAcState->bSetAngle = 1;
				}
				pAcState->ptNewMapCenter = g_fileSysApi.pGdata->carInfo.ptCarRaw;
			}
			else if(m_nLastScaleType == 2)
			{
				pAcState->ptNewMapCenter = m_ptMapCenter;
			}
			//
			m_bInNoNaviScale = nuTRUE;
			m_zoomDraw.m_nZoomType = ZOOM_NO;
		}
		//bOnlyChangeTime = nuFALSE;
	}
	else
	{
		/**/  
		if( m_bInNoNaviScale && m_nLastMapDirectMode != MAP_DIRECT_NORTH/*g_fileSysApi.pGdata->uiSetData.nMapDirectMode*/)
		{
			//g_fileSysApi.pGdata->uiSetData.nMapDirectMode = g_fileSysApi.pUserCfg->nMapDirectMode = m_nLastMapDirectMode;
			//m_gDataMgr.SetAngle(0);
			//pAcState->bSetAngle = 1;
//			m_zoomDraw.m_nZoomType = ZOOM_NO;
		}
		if( m_bInNoNaviScale )
		{
			//pAcState->ptNewMapCenter = m_ptLastMapCenter;
		}
		m_bInNoNaviScale = nuFALSE;
	}
	m_zoomDraw.m_nOldScaleValue	= g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
	g_fileSysApi.FS_ReLoadMapConfig();
	m_zoomDraw.m_nNewScaleValue	= g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
	g_fileSysApi.pGdata->uiSetData.nScaleValue = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
	if( bOnlyChangeTime )
	{
		g_fileSysApi.pUserCfg->nLastScaleIndex = g_fileSysApi.pGdata->uiSetData.nScaleIdx;
		g_fileSysApi.FS_SaveUserConfig();
	}
__android_log_print(ANDROID_LOG_INFO, "navi", "SetScale nAutoMoveType %d", g_fileSysApi.pGdata->timerData.nAutoMoveType);
	SetMoveWait(WAIT_TYPE_MOVE, bOnlyChangeTime);  
	return DRAW_YES;
}
nuUINT CNaviThread::NuroPointToRoad(nuroPOINT& point, nuPVOID lpData)
{
	if( lpData == NULL )
	{
		return NC_PT_TO_RD_FAILURE;
	}
	m_drawToUI.nReturnCode = NC_PT_TO_RD_FAILURE;
	if( m_nThdState != NAVI_THD_WORK )
	{
		m_drawToUI.nReturnCode = PointToRoad(point.x, point.y, lpData);
	}
	else
	{
		CODENODE codeNode = {0};
		codeNode.nActionType	= AC_DRAW_POINTOTROAD;
		codeNode.nX				= point.x;
		codeNode.nY				= point.y;
		codeNode.nExtend		= (nuLONG)lpData;
		m_drawToUI.bUIWaiting	= 1;
		if( !m_codeDraw.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting =  0;
		}
		while( m_drawToUI.bUIWaiting )
		{
			nuSleep(50);
		}
	}
	return m_drawToUI.nReturnCode;
}
nuUINT CNaviThread::PointToRoad(nuLONG x, nuLONG y, nuPVOID lpData)
{
	PPT_TO_ROAD_DATA pPtToRd = (PPT_TO_ROAD_DATA)lpData;
	SEEKEDROADDATA roadData = {0};
	if( !pPtToRd->bPointScreen )
	{
		m_gDataMgr.PointScreenToBmp(x, y);
		g_mathtoolApi.MT_BmpToMap(x, y, &x, &y);
	}
	if( !g_fileSysApi.FS_SeekForRoad(x, y, pPtToRd->nMaxFixDis, pPtToRd->nAngle, &roadData) )
	{
		return NC_PT_TO_RD_FAILURE;
	}
	pPtToRd->nCityID	= roadData.nCityID;
	pPtToRd->nTownID	= roadData.nTownID;
	pPtToRd->nMapIdx	= roadData.nDwIdx;

	pPtToRd->nDisToRoad = (short)roadData.nDis;
	pPtToRd->nBlkIdx	= roadData.nBlkIdx;
	pPtToRd->nRoadIdx	= roadData.nRoadIdx;
	pPtToRd->nRoadCls	= roadData.nRoadClass;
	pPtToRd->nNameAddr	= roadData.nNameAddr;
	pPtToRd->point.x	= x;//OUT the points in map
	pPtToRd->point.y	= y;
	pPtToRd->ptMappped	= roadData.ptMapped;
	return NC_PT_TO_RD_SUCCESS;
}

nuBOOL CNaviThread::ChooseCenterRoad(SEEKEDROADDATA& roadCenterOn, PSEEK_ROADS_OUT pSeekRdsOut, SEEKEDROADDATA& oldRoadCarOn)
{
	return ChooseCenterRoadEx(roadCenterOn, pSeekRdsOut, oldRoadCarOn);
}

nuBOOL CNaviThread::ChooseCenterRoadEx(SEEKEDROADDATA& RoadCenterOn, PSEEK_ROADS_OUT pSeekRdsOut, SEEKEDROADDATA& OldRoadCarOn)
{
	nuINT i = 0, nIdx = 0, j = 0;
	nuBOOL  bRecord = nuTRUE;

	__android_log_print(ANDROID_LOG_INFO, "RoadList", "GPSTime %d/%d/%d-%d:%d:%d, Speed %d, Angle %d", m_gpsData.nTime.nYear, m_gpsData.nTime.nMonth, m_gpsData.nTime.nDay,
m_gpsData.nTime.nHour, m_gpsData.nTime.nMinute, m_gpsData.nTime.nSecond, m_gpsData.nSpeed, m_gpsData.nAngle);

	__android_log_print(ANDROID_LOG_INFO, "RoadList", "Current RoadClass %d, BlockIdx %d, RoadIdx %d, Angle %d",m_SeekedRoadData.nRoadClass, m_SeekedRoadData.nBlkIdx, m_SeekedRoadData.nRoadIdx, m_SeekedRoadData.nRoadAngle);

	for(; i < pSeekRdsOut->nCountSeeked; i++)
	{
		__android_log_print(ANDROID_LOG_INFO, "RoadList", "[%d] RoadClass %d, BlockIdx %d, RoadIdx %d, Dis %d, Angle %d",i, pSeekRdsOut->roadsList[i].nRoadClass, pSeekRdsOut->roadsList[i].nBlkIdx, pSeekRdsOut->roadsList[i].nRoadIdx, pSeekRdsOut->roadsList[i].nDis, pSeekRdsOut->roadsList[i].nRoadAngle);
		
		//Added by Daniel ================Choose Road===START========== 20150207//
		if(_CHOOSE_MODE_PASS_CURRENT_ROAD == m_nChosenRoadMode)
		{
			for(j = 0; j < m_nChosenRoadCount; j++)
			{
				if(m_Road_Info_Data[j].nDwIdx == pSeekRdsOut->roadsList[i].nDwIdx && 
					m_Road_Info_Data[j].nBlkIdx == pSeekRdsOut->roadsList[i].nBlkIdx &&
					m_Road_Info_Data[j].nRoadIdx == pSeekRdsOut->roadsList[i].nRoadIdx)
				{
					pSeekRdsOut->roadsList[i].nDis += m_Road_Info_Data[j].nDis;				//Can Not Mapping Passed Road
					break;
				}

			}
			if(m_bChooseRoad)//No chosen, so m_nChosenRoadMode is _CHOOSE_MODE_PASS_CURRENT_ROAD




			{
				if(pSeekRdsOut->roadsList[i].nDwIdx == m_SeekedRoadData.nDwIdx	&&
					pSeekRdsOut->roadsList[i].nBlkIdx == m_SeekedRoadData.nBlkIdx &&
					pSeekRdsOut->roadsList[i].nRoadIdx == m_SeekedRoadData.nRoadIdx)
				{
					pSeekRdsOut->roadsList[i].nDis					   += _CURREN_ROAD_WEIGHT;//Last road can not be chosen
					for(j = 0; j < m_nChosenRoadCount; j++)
					{
						if(m_Road_Info_Data[j].nDwIdx == pSeekRdsOut->roadsList[i].nDwIdx && 
							m_Road_Info_Data[j].nBlkIdx == pSeekRdsOut->roadsList[i].nBlkIdx &&

							m_Road_Info_Data[j].nRoadIdx == pSeekRdsOut->roadsList[i].nRoadIdx)
						{
							m_Road_Info_Data[j].nDis  += _CURREN_ROAD_WEIGHT;
							bRecord = nuFALSE;
							break;
						}
					}
					if(bRecord)
					{
						m_Road_Info_Data[m_nChosenRoadCount].nDwIdx		=  pSeekRdsOut->roadsList[i].nDwIdx;
						m_Road_Info_Data[m_nChosenRoadCount].nBlkIdx		=  pSeekRdsOut->roadsList[i].nBlkIdx;
						m_Road_Info_Data[m_nChosenRoadCount].nRoadIdx		=  pSeekRdsOut->roadsList[i].nRoadIdx;
						m_Road_Info_Data[m_nChosenRoadCount].ptCar_In_Road	=  pSeekRdsOut->roadsList[i].ptMapped;
						m_Road_Info_Data[m_nChosenRoadCount].nNameAddr		=  pSeekRdsOut->roadsList[i].nNameAddr;
						m_Road_Info_Data[m_nChosenRoadCount].nRoadClass     	=  pSeekRdsOut->roadsList[i].nRoadClass;
					    	m_Road_Info_Data[m_nChosenRoadCount].nDis           	= _CURREN_ROAD_WEIGHT;
						m_nChosenRoadCount++;
					}
				}
			}
		}
		//Added by Daniel ================Choose Road===END========== 20150207//
		pSeekRdsOut->roadsList[i].nDis += GetRoadWeight(RoadCenterOn, pSeekRdsOut->roadsList[i]);
		__android_log_print(ANDROID_LOG_INFO, "RoadList", "[%d], pSeekRdsOut->roadsList[i].nDis", pSeekRdsOut->roadsList[i].nDis);
		if(pSeekRdsOut->roadsList[nIdx].nDis > pSeekRdsOut->roadsList[i].nDis)
		{
			nIdx = i;
		}
	}

	if(!m_bChooseRoad && 
		(pSeekRdsOut->roadsList[nIdx].nDwIdx != m_SeekedRoadData.nDwIdx || 
		pSeekRdsOut->roadsList[nIdx].nBlkIdx != m_SeekedRoadData.nBlkIdx || 
		pSeekRdsOut->roadsList[nIdx].nRoadIdx != m_SeekedRoadData.nRoadIdx))
	{
		m_nChosenRoadCount = 0;

		m_nChosenRoadMode = _CHOOSE_MODE_DEFAULT;
		nuMemset(m_Road_Info_Data, 0, sizeof(ROAD_INFO) * SEEK_ROADS_MAX_COUNT);
	}
	else
	{
		OldRoadCarOn = pSeekRdsOut->roadsList[nIdx];
	}
	m_bChooseRoad     = nuFALSE;

	RoadCenterOn = pSeekRdsOut->roadsList[nIdx];
	m_SeekedRoadData = pSeekRdsOut->roadsList[nIdx];
	return nuTRUE;
}
nuINT CNaviThread::GetRoadWeight(const SEEKEDROADDATA RoadCenterOn, const SEEKEDROADDATA SeekRdsOut)
{
	nuINT Weight = 0;
	if(RoadCenterOn.nRoadClass > 5 && SeekRdsOut.nRoadClass < 5)
	{
		Weight += 30;
	}
	if(RoadCenterOn.nRoadClass <= 5 && SeekRdsOut.nRoadClass > 5)
	{
		Weight += 30;
	}
	if(RoadCenterOn.nRoadClass != SeekRdsOut.nRoadClass ||
		RoadCenterOn.nNameAddr != SeekRdsOut.nNameAddr)
	{
		if(m_gpsData.nSpeed >= 40 && RoadCenterOn.nRoadClass > 5 && SeekRdsOut.nRoadClass > 5)
		{
			Weight += 15;
		}
		else
		{
			Weight += 3;
		}
	}
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		if(m_NaviRoadCenterOn.nBlkIdx != SeekRdsOut.nBlkIdx || m_NaviRoadCenterOn.nRoadIdx != SeekRdsOut.nRoadIdx)
		{
			if(m_NaviRoadCenterOn.nRoadClass <= 5)
			{
				Weight += m_nFreeRoadWeight;
			}
			else

			{
				Weight += m_nNaviRoadWeight;
			}
		}
	}
	
	//modify for fixroad
	/*nuLONG CutLineAngle =0;	
	CutLineAngle = (nuLONG)(nuAtan2(SeekRdsOut.nRdDy, SeekRdsOut.nRdDx) * 180/PI);
	CutLineAngle = CutLineAngle%360; 
	if(NURO_ABS(m_gpsNew.nAngle - CutLineAngle) < 45)
	{
		m_gpsNow.nAngle = CutLineAngle;
	}*/
	Weight += SeekRdsOut.nRoadAngle / 3;
	return Weight;
}

nuUINT CNaviThread::NuroStart3DDemo(nuTCHAR *pts3dDemo)
{
    if( !m_demo3D.Start3DDemo(pts3dDemo) )
    {
        return 0;
    }
    m_nOldMoveType = g_fileSysApi.pGdata->timerData.nAutoMoveType;
    g_fileSysApi.pGdata->timerData.nAutoMoveType = AUTO_MOVE_DEMO3D;
    return 1;
}

nuUINT CNaviThread::NuroClose3DDemo()
{

    if( m_demo3D.Close3DDemo() )
	{
		 g_fileSysApi.pGdata->timerData.nAutoMoveType = m_nOldMoveType ;
	}
    return 1;
}

//@2011.02.16
nuBOOL CNaviThread::SetDrawInfoMap()
{
	//Get car point 
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

	//calculate the car angle
	nuLONG nDis = g_fileSysApi.pGdata->drawInfoMap.nScaleValue * 5;
	nuLONG nx = g_fileSysApi.pGdata->carInfo.ptCarIcon.x + (nuLONG)( nDis * nuCos(g_fileSysApi.pGdata->carInfo.nCarAngle) );
	nuLONG ny = g_fileSysApi.pGdata->carInfo.ptCarIcon.y + (nuLONG)( nDis * nuSin(g_fileSysApi.pGdata->carInfo.nCarAngle) );
	g_mathtoolApi.MT_MapToBmp(nx, ny, &nx, &ny);
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp2Dto3D(nx, ny, MAP_DEFAULT);
	}
	g_fileSysApi.pGdata->drawInfoMap.nCarAngleDx = nx - g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x;
	g_fileSysApi.pGdata->drawInfoMap.nCarAngleDy = ny - g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y;

	//Get Map Center point, 
	g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp2Dto3D( g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.x, g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.y, MAP_DEFAULT);
	}
	//Set Scale value
	g_fileSysApi.pGdata->drawInfoMap.nScaleValue = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;

	return nuTRUE;
}

nuUINT CNaviThread::SetDrawDataForUI()
{
	SetDrawInfoMap();

	//Save CarInfo
	m_DrawDataForUI.nddCarAngle = g_fileSysApi.pGdata->carInfo.nCarAngle;
	m_DrawDataForUI.nddMapAngle = g_fileSysApi.pGdata->carInfo.nMapAngle;
	m_DrawDataForUI.nddCarInScreen = g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen;
	m_DrawDataForUI.nddSkyHeight = g_fileSysApi.pGdata->uiSetData.nSkyHeight;
	m_DrawDataForUI.nddCarInMapPt = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
	m_DrawDataForUI.nddRawGPSPt = g_fileSysApi.pGdata->carInfo.ptRawGPS;
	
	//Save Center Point
	m_DrawDataForUI.nddCenterPoint = g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen;
	m_DrawDataForUI.nddMapCenterX = g_fileSysApi.pGdata->transfData.nMapCenterX;
	m_DrawDataForUI.nddMapCenterY = g_fileSysApi.pGdata->transfData.nMapCenterY;
	//
	m_DrawDataForUI.nddb3DMode  = g_fileSysApi.pGdata->uiSetData.b3DMode;
	m_DrawDataForUI.nddAutoMoveType = g_fileSysApi.pGdata->timerData.nAutoMoveType;
	m_DrawDataForUI.nddbNightOrDay = g_fileSysApi.pGdata->uiSetData.bNightOrDay;
	m_DrawDataForUI.nddMapDirectMode = g_fileSysApi.pGdata->uiSetData.nMapDirectMode;

	m_DrawDataForUI.nddCosConst = g_fileSysApi.pDrawInfo->pcTransf->nCosConst;
	m_DrawDataForUI.nddSinConst = g_fileSysApi.pDrawInfo->pcTransf->nSinConst;

	//Save Scale
	m_DrawDataForUI.nddNowScale = g_fileSysApi.pGdata->uiSetData.nScaleValue;
	m_DrawDataForUI.nddScalePixels = g_fileSysApi.pGdata->drawInfoMap.nScalePixels;

	//Save CcdNum
	m_DrawDataForUI.nddCCDNum = g_fileSysApi.pGdata->drawInfoMap.nCCDNum;
	//Save PoiInfo
	/*m_DrawDataForUI.nddNowCenterPoiCount = g_fileSysApi.pGdata->drawInfoMap.nNowCenterPoiCount;
	if( m_DrawDataForUI.nddNowCenterPoiCount > 0 )
	{
		SEEKEDPOIDATA TmpPoiNode;
		nuMemset(&TmpPoiNode, 0, sizeof(TmpPoiNode));
		TmpPoiNode = g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[0];
		m_DrawDataForUI.nddMapIndex = TmpPoiNode.nMapIdx;
		m_DrawDataForUI.nddPoiNameAddr = TmpPoiNode.nNameAddr;
		m_DrawDataForUI.nddOtherInfoAddr = TmpPoiNode.OnterInfoAddr;

	}*/	

	//Save Province, City and Road Name
	m_DrawDataForUI.nddCityName[0]  = 0;
	m_DrawDataForUI.nddNowRdName[0] = 0;
	m_DrawDataForUI.nddProvinceName[0] = 0;
	m_DrawDataForUI.nddCityCode = 0;
	m_DrawDataForUI.nddMapIndex = 0;
#ifdef VALUE_EMGRT
	if(m_bFindRoad)
#endif
	{		
		if( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nScaleValue >= 1000 )
		{

			nuWORD nMapCount = g_fileSysApi.FS_GetMapCount();
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx,
				nuMAKELONG(g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterTownID, g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterCityID),
				m_DrawDataForUI.nddCityName, sizeof(m_DrawDataForUI.nddCityName)/sizeof(nuWCHAR) );
			if( nMapCount > 1 )
			{
				PMAPNAMEINFO pTmp = (PMAPNAMEINFO)g_fileSysApi.FS_GetMpnData();
				nuWCHAR pNameTmp[256] = {0};
				if( pTmp && g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx < nMapCount )
				{
					nuWcscpy( m_DrawDataForUI.nddProvinceName, (pTmp + g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx)->wMapFriendName );
					nuWcscat( pNameTmp, (pTmp + g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx)->wMapFriendName );
					if( nuWcscmp (pNameTmp, m_DrawDataForUI.nddCityName) == 0 )
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "m_DrawDataForUI.nddCityName");
						//nuMemset( m_DrawDataForUI.nddCityName, 0, sizeof(pNameTmp) ); //error:size is wrong;
						nuMemset( m_DrawDataForUI.nddCityName, 0, sizeof(m_DrawDataForUI.nddCityName) );
						//======================SDK======================================================//
						nuMemcpy(m_DrawDataForUI.nddCityName, m_DrawDataForUI.nddProvinceName, sizeof(m_DrawDataForUI.nddCityName));
						//======================SDK======================================================//
					}
					else
					{
						nuWcscat( m_DrawDataForUI.nddProvinceName, m_DrawDataForUI.nddCityName);
						nuMemset( m_DrawDataForUI.nddCityName, 0, sizeof(m_DrawDataForUI.nddCityName));
						nuWcscat( m_DrawDataForUI.nddCityName, m_DrawDataForUI.nddProvinceName);
					}
				}
			}
		}
		else if( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nScaleValue < 400 )
		{
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx, 
				nuMAKELONG(g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterTownID, g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterCityID), 
				m_DrawDataForUI.nddCityName, 
				sizeof(m_DrawDataForUI.nddCityName)/sizeof(nuWCHAR) );
			nuMemset(m_DrawDataForUI.nddCityName, 0, sizeof(m_DrawDataForUI.nddCityName)/2);
			nuMemmove(m_DrawDataForUI.nddCityName, (char *)m_DrawDataForUI.nddCityName + sizeof(m_DrawDataForUI.nddCityName)/2, sizeof(m_DrawDataForUI.nddCityName)/2);
	
		#ifndef VALUE_EMGRT
			RoadNamefilter(m_DrawDataForUI.nddNowRdName, nuWcslen(m_DrawDataForUI.nddNowRdName));
			nuLONG Dis = 0;
			nuWCHAR wsSpace[3] = {0x002D, 0}, wsKiloTemp[4] = {0x004B, 0x8655, 0, 0}, wsKiloK[2] = {0x004B, 0};
			if(g_fileSysApi.pGdata->drawInfoMap.bUsedKm && m_wsKMName[0] != 0)
			{
				//nuWcscpy((nuWCHAR*)m_DrawDataForUI.nddNowRdName, wsSpace);
				nuWcscpy((nuWCHAR*)m_DrawDataForUI.nddNowRdName, m_wsKMName);
				nuWcscat((nuWCHAR*)m_DrawDataForUI.nddNowRdName, wsKiloK);
				//nuWcscat((nuWCHAR*)m_DrawDataForUI.nddNowRdName, wsKiloTemp);	
			}
		#endif
		}
		else
		{	
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx, 
				nuMAKELONG(g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterTownID, g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterCityID ), 
				m_DrawDataForUI.nddCityName,
				sizeof(m_DrawDataForUI.nddCityName)/sizeof(nuWCHAR) );
			g_fileSysApi.FS_ReadCityTownName( g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterMapIdx, 
				nuMAKELONG(g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterTownID, g_fileSysApi.pDrawInfo->pcDrawInfoMap->nCenterCityID), 
				m_DrawDataForUI.nddFriendCityName, 
				sizeof(m_DrawDataForUI.nddCityName)/sizeof(nuWCHAR) );
			nuMemset(m_DrawDataForUI.nddFriendCityName, 0, sizeof(m_DrawDataForUI.nddFriendCityName)/2);
			nuMemmove(m_DrawDataForUI.nddFriendCityName, (char *)m_DrawDataForUI.nddFriendCityName + sizeof(m_DrawDataForUI.nddFriendCityName)/2, sizeof(m_DrawDataForUI.nddFriendCityName)/2);
		}
		nuMemcpy((nuWCHAR*)m_DrawDataForUI.nddNowRdName, g_fileSysApi.pDrawInfo->pcDrawInfoMap->wsCenterName, sizeof(nuWCHAR) * NC_WSTRING_MAX_NUM);
		nuWcscpy((nuWCHAR*)m_wsNowRoadName, m_DrawDataForUI.nddNowRdName);
		m_DrawDataForUI.nddCityCode = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nCityID ;
		nuWORD nMapCount = g_fileSysApi.FS_GetMapCount();
		if(nMapCount > 1)
		{
			PMAPNAMEINFO pTmpMPN = (PMAPNAMEINFO)g_fileSysApi.FS_GetMpnData();
			m_DrawDataForUI.nddMapIndex = pTmpMPN[g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx].nMapID;//g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
		}
		else
		{
			m_DrawDataForUI.nddMapIndex = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
		}
	}

	//Save NowSpeed
	m_DrawDataForUI.nddShowSpeed = g_fileSysApi.pGdata->carInfo.nShowSpeed;
	if(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS)
	{
		m_DrawDataForUI.nddLimitSpeed = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed * 10;
	}
	else
	{
		m_DrawDataForUI.nddLimitSpeed = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nLimitSpeed * 10;
	}
	if(m_DrawDataForUI.nddLimitSpeed <= 0)
	{
		m_DrawDataForUI.nddLimitSpeed = 20;
	}
	//Save data for draw navi
	m_DrawDataForUI.nddNaviState = g_fileSysApi.pGdata->uiSetData.bNavigation;
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{
		m_DrawDataForUI.nddDisToNextCross = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance;
		m_DrawDataForUI.nddDisToTarget = g_fileSysApi.pGdata->drawInfoNavi.nDisToTarget;
		m_DrawDataForUI.nddTimeToTarget = g_fileSysApi.pGdata->drawInfoNavi.nTimeToTarget;
		nuWcscpy(m_DrawDataForUI.nddNextRdName, g_fileSysApi.pGdata->drawInfoNavi.nextCross.wsName);
	#ifndef VALUE_EMGRT
		RoadNamefilter((nuWCHAR*)m_DrawDataForUI.nddNextRdName, nuWcslen(m_DrawDataForUI.nddNextRdName));
	#endif
		m_DrawDataForUI.nddTotalListCount = g_fileSysApi.pGdata->drawInfoNavi.nTotalListCount;
		m_DrawDataForUI.nddCrossIconType = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nIconType;

		m_DrawDataForUI.nddNaviSimSpeed = g_fileSysApi.pUserCfg->nSimSpeed;
	}

	//For SplitSreen
	m_DrawDataForUI.nddCrossNearType = g_fileSysApi.pGdata->drawInfoNavi.nCrossNearType;
	//m_DrawDataForUI.nddCrossNearType = g_fileSysApi.pDrawInfo->pcDrawInfoNavi->nCrossNearType;
	if( g_fileSysApi.pGdata->drawInfoNavi.nRtListNowNum )
	{	
		m_DrawDataForUI.nddRtListNowNum = g_fileSysApi.pGdata->drawInfoNavi.nRtListNowNum;

		m_DrawDataForUI.nddListNode[0] = *(LISTNODE*)g_fileSysApi.pGdata->drawInfoNavi.pRtList;
		m_DrawDataForUI.nddListNode[1] = *(LISTNODE*)(g_fileSysApi.pGdata->drawInfoNavi.pRtList+1);
		m_DrawDataForUI.nddListNode[2] = *(LISTNODE*)(g_fileSysApi.pGdata->drawInfoNavi.pRtList+2);
	}
#ifndef VALUE_EMGRT
	RoadNamefilter((nuWCHAR*)m_DrawDataForUI.nddListNode[0].wsName, nuWcslen(m_DrawDataForUI.nddListNode[0].wsName));
	RoadNamefilter((nuWCHAR*)m_DrawDataForUI.nddListNode[1].wsName, nuWcslen(m_DrawDataForUI.nddListNode[1].wsName));
	RoadNamefilter((nuWCHAR*)m_DrawDataForUI.nddListNode[2].wsName, nuWcslen(m_DrawDataForUI.nddListNode[2].wsName));
#endif
	m_DrawDataForUI.nddScreenType = g_fileSysApi.pGdata->uiSetData.nScreenType;
	m_DrawDataForUI.nddSplitScreenType = g_fileSysApi.pGdata->uiSetData.nSplitScreenType;
	m_DrawDataForUI.nddShowBigPic = g_fileSysApi.pUserCfg->bShowBigPic;

	//Save RealPicID
//#ifdef ZENRIN
	m_DrawDataForUI.nddRealPicID = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID;
	if( g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID )
	{
		m_DrawDataForUI.nddDisRealPic = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicDis;
	}
/*#else
	if( g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nRealPicID )
	{
		m_DrawDataForUI.nddRealPicID = g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nRealPicID;//g_fileSysApi.pDrawInfo->drawInfoCommon.nRealPicID;
		m_DrawDataForUI.nddDisRealPic = g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nDistance;
	}
	else if( g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID )
	{
		m_DrawDataForUI.nddRealPicID = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nRealPicID;
		m_DrawDataForUI.nddDisRealPic = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance;
	}
	m_DrawDataForUI.nddIgnorePicID = g_fileSysApi.pGdata->drawInfoNavi.nIgnorePicID;
//#endif*/
	m_DrawDataForUI.nddMoveWait = g_fileSysApi.pGdata->timerData.bMoveWait;
	m_DrawDataForUI.nddSaveDataOver = 1;
	m_DrawDataForUI.nddSimuReachDes = m_bSimuReachDes;
	return 1;
} 

nuVOID CNaviThread::RouteLenghView(PACTIONSTATE pActionState)//added by daniel
{
	return;
	g_innaviApi.IN_ColNaviInfo();
	nuroPOINT PtMaxCoor, PtMinCoor, PtMidCoor, PtBmpPoint;
	nuLONG lMaxCoorDis = 0, lScaleIdx = 0, ltmpdx = 0, ltmpdy = 0;
	g_innaviApi.IN_GetMinAndMaxCoor_D(&PtMaxCoor, &PtMinCoor);
	ltmpdx = NURO_ABS(PtMaxCoor.x - PtMinCoor.x);
	ltmpdy = NURO_ABS(PtMaxCoor.y - PtMinCoor.y);
	//lMaxCoorDis = nuSqrt((ltmpdx * ltmpdx) + (ltmpdy * ltmpdy));
	if(ltmpdx < 1230 && ltmpdy < 700)//738



	{
		lScaleIdx = 4;//100m
	}
	else if(ltmpdx < 2460 && ltmpdy < 1430)//1476
	{
		lScaleIdx = 5;//200m
	}
	else if(ltmpdx < 4920 && ltmpdy < 2910)//2952
	{
		lScaleIdx = 6;//400
	}
	else if( ltmpdx < 12300 && ltmpdy < 7340)//7380
	{
		lScaleIdx = 7;//1km
	}
	else if( ltmpdx < 24600 && ltmpdy < 14720)//14760
	{
		lScaleIdx = 8;//2km
	}
	else if( ltmpdx < 61500 && ltmpdy < 36860)//36900
	{
		lScaleIdx = 9;//5km
	}
	else if( ltmpdx < 123000 && ltmpdy < 73760)//73800
	{
		lScaleIdx = 10;//10km
	}
	else if( ltmpdx < 246000 && ltmpdy < 147560)//147600
	{
		lScaleIdx = 11;//20km
	}

	else if( ltmpdx < 393600 && ltmpdy < 236120)//236160
	{
		lScaleIdx = 12;//32km
	}
	else
	{
		lScaleIdx = 13;//64km
	}
	//g_fileSysApi.pGdata->uiSetData.nScaleIdx = lScaleIdx;
	//SetScaleProc(lScaleIdx, pActionState, nuTRUE);
	NuroSetScale(lScaleIdx);
	PtMidCoor.x = ((PtMaxCoor.x + PtMinCoor.x) / 2);
	PtMidCoor.y = ((PtMaxCoor.y + PtMinCoor.y) / 2);
	g_mathtoolApi.MT_MapToBmp(PtMidCoor.x, PtMidCoor.y, &PtBmpPoint.x, &PtBmpPoint.y);
	//NuroOperateMapXY(PtBmpPoint.x, PtBmpPoint.y, 0);
	pActionState->ptNewMapCenter.x = PtBmpPoint.x;//g_fileSysApi.pGdata->transfData.nMapCenterX;
	pActionState->ptNewMapCenter.y = PtBmpPoint.y;//g_fileSysApi.pGdata->transfData.nMapCenterY;
	NuroJumpToScreenXY(PtBmpPoint.x , PtBmpPoint.y);
}

nuBOOL CNaviThread::bUISaveLastPosition()//added by daniel 20111111
{
	CODENODE codeNode		= {0};
	codeNode.nActionType	= AC_DRAW_UI_SAVE_LAST_POSITION;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
}

nuBOOL CNaviThread::SetRTFullView(nuLONG x, nuLONG y, nuUINT nCode) //added by xiaoqin 20120517
{
	CODENODE codeNode = {0};
	codeNode.nActionType	= AC_NAVI_RTFULLVIEW;
	codeNode.nDrawType		= DRAWTYPE_UI;
	codeNode.nExtend		= nCode;
	codeNode.nX				= x;
	codeNode.nY				= y;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
}

nuUINT CNaviThread::NuroSetMapScreenSize(nuWORD nScreenWidth, nuWORD nScreenHeight) 
{
	CODENODE codeNode = {0};
	codeNode.nActionType = AC_DRAW_RESETSCREEN;
	codeNode.nX          = nScreenWidth;
	codeNode.nY          = nScreenHeight;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_CIRCLE);
}

nuBOOL CNaviThread::NuroResetGPS( PGPSCFG pGpsSetData)
{
	CODENODE codeNode = {0};
	codeNode.nActionType = AC_GPSSET_RESET;
	codeNode.nX          = pGpsSetData->nComport;
	codeNode.nY          = pGpsSetData->nGPSBaudrate;
	return m_codeDraw.PushNode(&codeNode, PUSH_ADD_CIRCLE);
}
nuVOID CNaviThread::UIPlaySound(nuWCHAR *wsTTS)
{
	g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_SOUNDNAVITTS, nuWcslen(wsTTS), wsTTS);
}
nuVOID CNaviThread::HUDGetData()
{
	/*HUDData HUDDisplay = {0};
	HUDDisplay.bFixRoad = m_bFindRoad;
	nuMemset(&HUDDisplay, 0, sizeof(HUDData));
	HUDDisplay.bNavi = g_fileSysApi.pGdata->uiSetData.bNavigation;
	nuBYTE SEIIBuf_Type1[11] = {0}, SEIIBuf_Type2[15] = {0};
	nuBYTE REGULUSBuf1[15] = {0}, REGULUSBuf2[15] = {0}, REGULUSBuf3[27] = {0};
	if(g_fileSysApi.pGdata->uiSetData.bNavigation)
	{
		switch(g_fileSysApi.pGdata->drawInfoNavi.lTurnFlag)
		{
		case T_Afore:
			HUDDisplay.nTurnFlag = 7;
			break;
		case T_Left:
			HUDDisplay.nTurnFlag = 1;
			break;
		case T_Right:
			HUDDisplay.nTurnFlag = 2;
			break;
		case T_LeftSide:
			HUDDisplay.nTurnFlag = 3;
			break;
		case T_RightSide:
			HUDDisplay.nTurnFlag = 4;
			break;
		case T_LUturn:
			HUDDisplay.nTurnFlag = 5;
			break;
		case T_RUturn:
			HUDDisplay.nTurnFlag = 6;
			break;
		default:
			HUDDisplay.nTurnFlag = 7;
			break;
		}
		if ( g_fileSysApi.pGdata->drawInfoNavi.HighWayCross.nDistance > 0 )
		{
			HUDDisplay.nRoadType = 2;//freeway
			HUDDisplay.nDistance = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance;
		}
		else if ( g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance > 0  )
		{
			HUDDisplay.nRoadType = 1;//generalway
			HUDDisplay.nDistance = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance;	
		} 
		HUDDisplay.nIconType = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nIconType;
		nuMemcpy(&HUDDisplay.LaneGuide, &g_fileSysApi.pGdata->drawInfoNavi.LaneGuide, sizeof(NOW_LAME_GUIDE));
	}
	if(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
	{
		HUDDisplay.nSpeed = g_fileSysApi.pUserCfg->nSimSpeed;
		HUDDisplay.nSpeedLimit = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nLimitSpeed * 10;
		if(HUDDisplay.nSpeedLimit == 0)
		{
			HUDDisplay.nSpeedLimit = 20;
		}
	}
	else
	{
		HUDDisplay.nSpeed = m_gpsData.nSpeed;
		HUDDisplay.nSpeedLimit = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed * 10;
		if(HUDDisplay.nSpeedLimit == 0)
		{
			HUDDisplay.nSpeedLimit = m_lSpeedLimit;
		}
		else
		{
			m_lSpeedLimit = HUDDisplay.nSpeedLimit;
		}
	}
	
	HUDDisplay.nAngle = g_fileSysApi.pDrawInfo->pcCarInfo->nCarAngle;
	nuMemcpy(HUDDisplay.wsNowRoadName , g_fileSysApi.pGdata->drawInfoMap.wsCenterName, sizeof(nuWCHAR) * 12);
	nuMemcpy(HUDDisplay.wsNextRoadName , g_fileSysApi.pGdata->drawInfoNavi.nextCross.wsName, sizeof(nuWCHAR) * 12);
	RoadNamefilter(HUDDisplay.wsNowRoadName , nuWcslen(HUDDisplay.wsNowRoadName));
	RoadNamefilter(HUDDisplay.wsNextRoadName , nuWcslen(HUDDisplay.wsNextRoadName));
	HUDDisplay.nbRoadSpeedWarning = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed * 10;
	//nuStrcat(HUDDisplay.strTelNum, "036571879");
	if( g_fileSysApi.pGdata->drawInfoMap.nCCDNum > 0 )
	{
		HUDDisplay.nCCD = 1;
	}
	
#ifdef NURO_OS_WINCE
	if(m_lVenderType == _VENDER_NECVOX )//&& m_HudComCtrl.m_bInitialOK
	{
		m_HudComCtrl.CalHUDData(&HUDDisplay, SEIIBuf_Type1, SEIIBuf_Type2, REGULUSBuf1, REGULUSBuf2, REGULUSBuf3);
		m_HudComCtrl.bWriteCom(SEIIBuf_Type1, SEIIBuf_Type2, REGULUSBuf1, REGULUSBuf2, REGULUSBuf3);
	}
	else if(m_lVenderType == _VENDER_DAIWOO)
	{
		if(m_nHUDUseODB)
		{
			HUDDisplay.nSpeed = m_gpsData.nOBD_Speed;
		}
		ColHUDBuffer_DAIWOO(&HUDDisplay);
	}
	else if(m_lVenderType == _VENDER_ELEAD)
	{
		m_HudComCtrl.CalHUDData(&HUDDisplay, SEIIBuf_Type1, SEIIBuf_Type2, REGULUSBuf1, REGULUSBuf2, REGULUSBuf3);
		m_HudComCtrl.ELead_SendMessage(SEIIBuf_Type2, 15, &HUDDisplay);
	}
	else if(m_lVenderType == _VENDER_PANA)
	{
		m_HudComCtrl.CalPanaHUDData(&HUDDisplay);
	}	
	if(m_lVenderType != _VENDER_ELEAD)
	{
		m_HudComCtrl.SEII_HUD(&HUDDisplay);
	}
#endif

#ifdef USE_HUD_DR_CALLBACK	
	bool bRepaint=true;//false;

	if(nHudTimeStart == -1 )
	{
		nHudTimeStart=nuGetTickCount();
	}
		
	nuDWORD nHudTimeTemp = nuGetTickCount(); //Per 1 sec, Sleep 10ms
	if((nHudTimeTemp-nHudTimeStart) > 500 ) 
	{
		nuLONG lSpeedOver=0;
		if(HUDDisplay.nSpeed > HUDDisplay.nSpeedLimit)
		  lSpeedOver=1;

		m_pfHUDCallBack(HUDDisplay.nRoadType, HUDDisplay.nTurnFlag, HUDDisplay.nSpeed, HUDDisplay.nDistance, HUDDisplay.nCCD,lSpeedOver,HUDDisplay.bNavi,HUDDisplay.nIconType,HUDDisplay.nSpeedLimit,SEIIBuf_Type2);
		nHudTimeStart=nuGetTickCount();
		bRepaint=true;
	}
	
#endif*/
}/*
nuVOID CNaviThread::ColHUDBuffer_DAIWOO(pHUDData pHUDDisplay)
{
#ifdef NURO_OS_WINCE
	nuBYTE SEIIBuf_Type1[11] = {0}, SEIIBuf_Type2[15] = {0};
	nuBYTE HITOP[128] = {0}, nRoadNameCount = 0;
	nuINT  i = 0, nHUDAdded = 0, nLength = 0, CheckSum = 0;
	nuUINT nKM = 0, nM = 0, nDirect = 0;
	nuLONG lTemp1 = 0, lTemp2 = 0, lTemp3 = 0;
	nuCHAR strDis[8] = {0}, strRaodNameBig5[128] = {0}, strTmp[15] = {0};
	nuMemset(SEIIBuf_Type1, 0x30, sizeof(SEIIBuf_Type1));
	SEIIBuf_Type1[0] = 0xA5;
	if(pHUDDisplay->bNavi)
	{
		if(pHUDDisplay->nDistance < 1000)
		{
			SEIIBuf_Type1[3] = 0x30 + pHUDDisplay->nTurnFlag;
			if(pHUDDisplay->nDistance > 0)
			{
				lTemp1 = pHUDDisplay->nDistance / 100;
				lTemp2 = pHUDDisplay->nDistance / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nDistance - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[4] += lTemp1;
				SEIIBuf_Type1[5] += lTemp2;
				SEIIBuf_Type1[6] += lTemp3;
			}
			if(pHUDDisplay->nSpeed > 0)
			{
				lTemp1 = pHUDDisplay->nSpeed / 100;
				lTemp2 = pHUDDisplay->nSpeed / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nSpeed - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[7] += lTemp1;
				SEIIBuf_Type1[8] += lTemp2;
				SEIIBuf_Type1[9] += lTemp3;
			}
		}
		else
		{
			SEIIBuf_Type1[3] = 0x37;
			nuMemset(&SEIIBuf_Type1[4], 0x23, sizeof(nuBYTE) * 6);//Data 4 ~ 9 has to be cleaned
		}
	}
	else
	{
		nuMemset(&SEIIBuf_Type1[1], 0x23, sizeof(nuBYTE) * 9);//Data 1 ~ 9 has to be cleaned
	}
	nuMemcpy(SEIIBuf_Type2, SEIIBuf_Type1, sizeof(nuBYTE) * 10);
	SEIIBuf_Type1[10] = 0x5A;
	SEIIBuf_Type2[10] = pHUDDisplay->nSpeedLimit; //Raod Speed Warning
	SEIIBuf_Type2[11] = 0x30 + pHUDDisplay->nCCD; //CCD Warning
	SEIIBuf_Type2[12] = 0x31;
	SEIIBuf_Type2[13] = 0;//SEIIBuf_Type2[1];
	for(i = 1; i < 13; i++)
	{
		SEIIBuf_Type2[13] ^= SEIIBuf_Type2[i]; 
	}
	SEIIBuf_Type2[14] = 0x5A;


	HITOP[0] = '$';
	HITOP[1] = 'G';
	HITOP[2] = 'P';
	HITOP[3] = 'H';
	HITOP[4] = 'U';
	HITOP[5] = 'D';
	HITOP[6] = ',';//Data 1 $GPHUD
	if(!m_UI_DAIWOO_BT.nSta)
	{
		HITOP[7] = 0x31 + pHUDDisplay->bNavi;
	}
	else
	{
		HITOP[7] = 0x30;
	}
	HITOP[8] = ',';//Data 2 Mode
	nHUDAdded = 9;
	nDirect  = (360 - (pHUDDisplay->nAngle - 90))%360 / 22.5;
	if(nDirect > 10)
	{
		HITOP[nHUDAdded++] = 0x30 + nDirect / 10;
	}


	HITOP[nHUDAdded++] = 0x30 + nDirect % 10;
	HITOP[nHUDAdded++] = ',';//Data 3 vDir
	lTemp1 = pHUDDisplay->nSpeed / 100;
	lTemp2 = pHUDDisplay->nSpeed / 10 - lTemp1 * 10;
	lTemp3 = pHUDDisplay->nSpeed - (lTemp1 * 100 + lTemp2 * 10);
	if(0 != lTemp1)
	{
		HITOP[nHUDAdded++] = 0x30 + lTemp1;
		HITOP[nHUDAdded++] = 0x30 + lTemp2;
		HITOP[nHUDAdded++] = 0x30 + lTemp3;
	}
	else if(0 != lTemp2)
	{
		HITOP[nHUDAdded++] = 0x30 + lTemp2;
		HITOP[nHUDAdded++] = 0x30 + lTemp3;
	}
	else
	{
		HITOP[nHUDAdded++] = 0x30 + lTemp3;
	}
	HITOP[nHUDAdded++] = ',';//Data4 vSpeed
	if(pHUDDisplay->nSpeedLimit > 0)
	{
		lTemp1 = pHUDDisplay->nSpeedLimit / 100;
		lTemp2 = pHUDDisplay->nSpeedLimit / 10 - lTemp1 * 10;
		lTemp3 = pHUDDisplay->nSpeedLimit - (lTemp1 * 100 + lTemp2 * 10);
		if(0 != lTemp1)
		{
			HITOP[nHUDAdded++] = 0x30 + lTemp1;
			HITOP[nHUDAdded++] = 0x30 + lTemp2;
			HITOP[nHUDAdded++] = 0x30 + lTemp3;
		}
		else if(0 != lTemp2)
		{
			HITOP[nHUDAdded++] = 0x30 + lTemp2;
			HITOP[nHUDAdded++] = 0x30 + lTemp3;
		}
		else if(0 != lTemp3)
		{
			HITOP[nHUDAdded++] = 0x30 + lTemp3;
		}
	}
	else
	{
		HITOP[nHUDAdded++] = 0x30;
	}
	HITOP[nHUDAdded++] = ',';//Data5 vRoadLmt
	HITOP[nHUDAdded++] = 0x30 + pHUDDisplay->nCCD ;//Data5 vRoadLmt
	HITOP[nHUDAdded++] = ',';//Data6 Camera
	if(!pHUDDisplay->bNavi)
	{
		if(!m_UI_DAIWOO_BT.nSta)
		{
#ifndef VALUE_EMGRT
			nLength=WideCharToMultiByte(950,0,pHUDDisplay->wsNowRoadName,-1,NULL,0,NULL,NULL);
			if(12 < nLength)//char length
			{
				nLength = 12;
			}
			WideCharToMultiByte(950,0,pHUDDisplay->wsNowRoadName,-1,strRaodNameBig5,nLength,NULL,NULL);
			nuMemcpy(&HITOP[nHUDAdded], strRaodNameBig5, sizeof(nuBYTE) * nLength);
			if(nLength < 12)
			{
				nLength--;//null has be removed
			}
			nHUDAdded += nLength;
#else
			for(i = 0; i < 6; i++)
			{
				if(0 == pHUDDisplay->wsNowRoadName[i])
				{
					break;
				}
				HITOP[nHUDAdded++] = (pHUDDisplay->wsNowRoadName[i] & 0xFF00) / 0x100;
				HITOP[nHUDAdded++] = pHUDDisplay->wsNowRoadName[i] & 0x00FF;
			}
#endif
		}
		HITOP[nHUDAdded++] = ',';//Data7 currRoad
		HITOP[nHUDAdded++] = ',';//Data8 nextRoad
		HITOP[nHUDAdded++] = ',';//Data9 nextDist
		HITOP[nHUDAdded++] = ',';//Data10 nextTurn
	}
	else
	{
		HITOP[nHUDAdded++] = ',';//Data7 currRoad
		if(!m_UI_DAIWOO_BT.nSta)
		{
#ifndef VALUE_EMGRT
			nLength=WideCharToMultiByte(950,0,pHUDDisplay->wsNextRoadName,-1,NULL,0,NULL,NULL);
			if(12 < nLength)//char length
			{
				nLength = 12;
			}
			WideCharToMultiByte(950,0,pHUDDisplay->wsNextRoadName,-1,strRaodNameBig5,nLength,NULL,NULL);
			nuMemcpy(&HITOP[nHUDAdded], strRaodNameBig5, sizeof(nuBYTE) * nLength);
			if(nLength < 12)
			{
				nLength--;//null has be removed
			}
			nHUDAdded += nLength;
#else
			for(i = 0; i < 6; i++)
			{
				if(0 == pHUDDisplay->wsNextRoadName[i])
				{
					break;
				}
				HITOP[nHUDAdded++] = (pHUDDisplay->wsNextRoadName[i] & 0xFF00) / 0x100;
				HITOP[nHUDAdded++] = pHUDDisplay->wsNextRoadName[i] & 0x00FF;
			}
#endif
		}
		HITOP[nHUDAdded++] = ',';//Data8 nextRoad
		if(!m_UI_DAIWOO_BT.nSta)
		{
			nuCHAR stemp[10] = {0};
			if( pHUDDisplay->nDistance < 1000 )
			{	
				nuItoa(pHUDDisplay->nDistance, stemp, 10);
				nuStrcpy(strDis, stemp);
				nuStrcat(strDis,"M");
			}
			else
			{
				nKM = pHUDDisplay->nDistance / 1000;
				nM = (pHUDDisplay->nDistance % 1000 ) / 100;
				//????
				nuItoa(nKM, stemp, 10);
				nuStrcpy(strDis, stemp);
				if( nKM < 100 )
				{
					//???
					nuStrcat(strDis,".");
					nuMemset(stemp, 0, sizeof(stemp));
					nuItoa(nM, stemp, 10);
					nuStrcat(strDis, stemp);
				}	
				nuStrcat(strDis,"KM");
			}
			for(i = 0; i < nuStrlen(strDis); i++)
			{
				if(0 == strDis[i])
				{
					break;
				}
				HITOP[nHUDAdded++] = strDis[i];
			}	
		}
		HITOP[nHUDAdded++] = ',';//Data9 nextDist
		if(!m_UI_DAIWOO_BT.nSta)
		{
			pHUDDisplay->nIconType = m_HudComCtrl.GetHUDIconType(pHUDDisplay->nIconType);
			if(pHUDDisplay->nIconType > 10)
			{
				HITOP[nHUDAdded++] = 0x30 + pHUDDisplay->nIconType / 10;
			}
			HITOP[nHUDAdded++] = 0x30 + pHUDDisplay->nIconType % 10;
		}
		HITOP[nHUDAdded++] = ',';//Data10 nextTurn
	}
	if(m_UI_DAIWOO_BT.nSta)
	{
#ifndef VALUE_EMGRT
		nLength=WideCharToMultiByte(950,0,m_UI_DAIWOO_BT.szName,-1,NULL,0,NULL,NULL);
        WideCharToMultiByte(950,0,m_UI_DAIWOO_BT.szName,-1,strRaodNameBig5,nLength,NULL,NULL);
		/*if(m_UI_DAIWOO_BT.szName[17] > 0x7E)
		{
			nLength = 16;
		}
		else if(nLength > 8)
		{
			nLength = 8;
		}
		for(i = 0; nRoadNameCount < 16; i++)
		{
			if(m_UI_DAIWOO_BT.szName[i] <= 0xFF)
			{
				nRoadNameCount++;
			}
			else
			{
				nRoadNameCount += 2;
				if(nRoadNameCount > 16)
				{
					nRoadNameCount-=2;
					break;
				}				
			}
		}
		//nRoadNameCount = i+1;
		for(i = 0; i < nRoadNameCount; i++)
		{
			//if(0x00 != strRaodNameBig5[i])
			{
				HITOP[nHUDAdded++] = strRaodNameBig5[i];
			}
		}
#else
		for(i = 0; i < 6; i++)
		{
			if(0 == m_UI_DAIWOO_BT.szName[i])

			{
				break;
			}


			HITOP[nHUDAdded++] = (m_UI_DAIWOO_BT.szName[i] & 0xFF00) / 0x100;
			HITOP[nHUDAdded++] = m_UI_DAIWOO_BT.szName[i] & 0x00FF;
		}
#endif
		//nuMemcpy(&HITOP[nHUDAdded], strRaodNameBig5, sizeof(nuBYTE) * nLength);
		//nHUDAdded += nLength - 1;
		HITOP[nHUDAdded++] = ',';//Data11 btName
		nLength = 0;
		for(i = 0; i < 12 && nLength < 12; i++, nLength++)
		{
			if(0x00 == m_UI_DAIWOO_BT.szNo[i] & 0x00FF)
			{
				continue;
			}
			HITOP[nHUDAdded++] = m_UI_DAIWOO_BT.szNo[i] & 0x00FF;			
		}		
		HITOP[nHUDAdded++] = ',';//Data12 btNumber
	}
	else
	{
		HITOP[nHUDAdded++] = ',';//Data11 btName
		HITOP[nHUDAdded++] = ',';//Data12 btNumber
	}
	for(i = 1; i < nHUDAdded; i++)
	{
		CheckSum ^= HITOP[i];
	}
	HITOP[nHUDAdded++] = '*';//Data13
	sprintf(strTmp, "%X", CheckSum);

	if(strTmp[1] == 0)
	{
		strTmp[1] = strTmp[0];
		strTmp[0] = 0x30;
	}
	HITOP[nHUDAdded++] = strTmp[0];
	HITOP[nHUDAdded++] = strTmp[1];
	HITOP[nHUDAdded++] = 0x0D;
	HITOP[nHUDAdded++] = 0x0A;
	if(m_nLogHUD)
	{
		m_HudComCtrl.DebugMessage_BYTE(HITOP, nHUDAdded);
	}
#ifdef NURO_OS_WINCE
	COPYDATASTRUCT CDS = {0};
	CDS.dwData = HUD_DATA_TYPE;
	if(m_Hudhwnd != NULL)
	{
		if(m_lHUDType == 3)
		{
			/*CDS.cbData = nHUDAdded;
			CDS.lpData = HITOP;
			::SendMessage(m_Hudhwnd, WM_COPYDATA, 0, (LPARAM)&CDS);
			if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
				cbParam.lDataLen  = nHUDAdded + 1;
				cbParam.pVOID  = (nuVOID*)HITOP;
				g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
			}
		}
		else if(m_lHUDType == 4)
		{
			CDS.cbData = sizeof(SEIIBuf_Type2);
			CDS.lpData = SEIIBuf_Type2;
			::SendMessage(m_Hudhwnd, WM_COPYDATA, 0, (LPARAM)&CDS);
			CDS.cbData = nHUDAdded;
			CDS.lpData = HITOP;
			::SendMessage(m_Hudhwnd, WM_COPYDATA, 0, (LPARAM)&CDS);
		}
	}
#endif
#endif
}*/
#ifdef USE_HUD_DR_CALLBACK 
nuVOID CNaviThread::DRGetData()
{
	return;

}

#endif
nuVOID CNaviThread::SeekKMName(PACTIONSTATE pActionState)
{
	nuBOOL bNeedFindKM = nuFALSE, bCalDis = nuFALSE;
	nuroRECT rtMap = {0};
	m_KMInfo.nCarAngle		= g_fileSysApi.pGdata->carInfo.nCarAngle;
	nuMemset(m_wsKilometer, 0, sizeof(m_wsKilometer));
	nuLONG	 lKMNum = 0, lKMDis = 0, lKMAddDis = 0;
	nuDOUBLE dDisToKm = 0;
	nuLONG Tick = nuGetTickCount();
	nuWCHAR TmpKMName[6] = {0}, wsTmpKM[32] = {0};
	if( pActionState->nDrawMode == DRAWMODE_SIM )
	{
		if(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass < 5)
		{
			rtMap.left	= g_fileSysApi.pGdata->transfData.nMapCenterX - _KM_CHECK_MAX_DIS;

			rtMap.right	= g_fileSysApi.pGdata->transfData.nMapCenterX + _KM_CHECK_MAX_DIS;
			rtMap.top	= g_fileSysApi.pGdata->transfData.nMapCenterY - _KM_CHECK_MAX_DIS;
			rtMap.bottom= g_fileSysApi.pGdata->transfData.nMapCenterY + _KM_CHECK_MAX_DIS;
			m_KMInfo.nCarMapIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
			m_KMInfo.nCarBlkIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nBlkIdx;
			m_KMInfo.nCarRdIdx		= (nuWORD)g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			m_KMInfo.ptCarMapped	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
			m_KMInfo.ptCarNextCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptNextCoor;
			m_KMInfo.nOneWay		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nOneWay;
			m_KMInfo.nRoadClass		= g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass;
			bNeedFindKM = nuTRUE;
		}
	}
	else if(pActionState->nDrawMode == DRAWMODE_GPS)
	{
		if(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass < 5)
		{
			rtMap.left	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x - _KM_CHECK_MAX_DIS;
			rtMap.right	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x + _KM_CHECK_MAX_DIS;
			rtMap.top	= g_fileSysApi.pGdata->carInfo.ptCarFixed.y - _KM_CHECK_MAX_DIS;
			rtMap.bottom= g_fileSysApi.pGdata->carInfo.ptCarFixed.y + _KM_CHECK_MAX_DIS;
			m_KMInfo.nCarMapIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
			m_KMInfo.nCarBlkIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
			m_KMInfo.nCarRdIdx		= (nuWORD)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
			m_KMInfo.ptCarMapped	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
			m_KMInfo.ptCarNextCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptNextCoor;
			m_KMInfo.nOneWay		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nOneWay;
			m_KMInfo.nRoadClass		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass;
			bNeedFindKM = nuTRUE;
		}
	}
	if(bNeedFindKM)
	{
		if(m_lKMTick == 0)
		{
			m_lPreKMNum = m_lKMNum = -1;
			m_lKMTick = nuGetTickCount();
			return;
		}

		m_KMInfo.bNewInfo = 0;
		if( g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			m_KMInfo.bNewInfo = g_innaviApi.IN_GetCarInfoForKM(&m_KMInfo);
		}
		if( !m_KMInfo.bNewInfo )
		{
			m_KMInfo.bNewInfo = (nuBYTE)g_fileSysApi.FS_SeekForKMInfo(rtMap, &m_KMInfo);
		}
		//
		if( m_KMInfo.bNewInfo )
		{			
			if(g_fileSysApi.FS_CheckKMInfo(&m_KMInfo, TmpKMName, lKMNum, lKMDis, wsTmpKM))
			{
				m_wsKMName[0] = 0; //nuMemset(m_wsKMName, 0, sizeof(m_wsKMName));
				nuWcscpy(m_wsKMName, wsTmpKM);
				if((m_lKMNum < 0) ||  
					(NURO_ABS(lKMNum - m_lKMNum) <= 200))
				{
					m_lKMNum = lKMNum;
					m_lPreKMNum = m_lKMNum;
				}
			}
		}		
	}
	else
	{
		m_lPreKMNum = m_lKMNum = -1;
		m_wsKMName[0] = 0;
	}
}
//--------------------------------------------------------------*/
nuVOID CNaviThread::RoadNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount)
{
	nuINT i = 1, j = 0;
	nuBOOL bRest = nuFALSE, bFind1 = nuFALSE, bFind2 = nuFALSE;
	nuWCHAR wsName1[MAX_LISTNAME_NUM] = {0};
	nuWCHAR	wsName2[MAX_LISTNAME_NUM] = {0};
	nuWCHAR	wsName3[MAX_LISTNAME_NUM] = {0};
	nuWCHAR	wsName[MAX_LISTNAME_NUM] = {0};
#ifdef ZENRIN
	if(wsRoadName[0] == 0x6536 && 
		wsRoadName[1] == 0x8CBB && 
		wsRoadName[2] == 0x7AD9)
	{
		nuMemcpy(wsName1, wsRoadName, sizeof(nuWCHAR) * 3);//???O??
		for(i = 3; i < nuWcslen(wsRoadName); i++)
		{
			if(0x005F == wsRoadName[i])
			{
				i++;
				if(bFind1)
				{
					bFind2 = nuTRUE;
					j = 0;
				}
				bFind1 = nuTRUE;
			}
			if(bFind1 && !bFind2)
			{
				wsName2[j] = wsRoadName[i];
				j++;
			}
			else if(bFind2)
			{
				if(wsRoadName[i] == nuTEXT('N'))
				{
					wsName3[0] = 0x5317;
					wsName3[1] = 0x4E0A;
				}
				else if(wsRoadName[i] == nuTEXT('S'))
				{
					wsName3[0] = 0x5357;
					wsName3[1] = 0x4E0B;
				}
				break;
			}
		}
		wsRoadName[0] = 0;
		nuWcscpy(wsRoadName, wsName2);
		nuWcscat(wsRoadName, wsName1);
		nuWcscat(wsRoadName, wsName3);
	}
#else
	if(wsRoadName[0] == 0x0028)
	{
		return ;
	}
	while(i < nWordCount)
	{//?P?_?O?_?? (XX???) (XX?A???)
		if(wsRoadName[i] == 0x0028 )
		{
			i+=1;
			while( i < (nWordCount-2))
			{
				if((wsRoadName[i] == 0x670D && wsRoadName[i+1] == 0x52D9 && wsRoadName[i+2] == 0x5340 ) ||
					(wsRoadName[i] == 0x4F11 && wsRoadName[i+1] == 0x606F && wsRoadName[i+2] == 0x7AD9 ))
				{
					bRest = nuTRUE;
					break;
				}
				i++;
			}
		}
		if (bRest)
		{
			break;
		}
		i++;
	}
	if (bRest)
	{//???????()?????????A???W?l
		i=1;
		while(i < nWordCount)
		{
			if(wsRoadName[i] == 0x0028 )
			{
				i+=1;
				while( i < nWordCount )
				{
					if (wsRoadName[i] != 0x0029)
					{
						wsName[j] = wsRoadName[i];	
					}
					else
					{
						break;
					}
					j++;
					i++;
				}
			}
			i++;
		}
		nuWcscpy(wsRoadName, wsName);
	}
	else
	{
		i=1;
		while(i < nWordCount)
		{
			if((wsRoadName[i] >= 0x0021 && wsRoadName[i] < 0x0030) ||
				(wsRoadName[i] > 0x0039 && wsRoadName[i] < 0x00C0))
			{
				wsRoadName[i] = 0;
				break;
			}
			i++;
		}
	}	
#endif
}
nuVOID CNaviThread::EndVoiceHandle()
{
	NUROPOINT FPt = g_fileSysApi.pGdata->drawInfoNavi.PrePt;
	NUROPOINT NPt = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped;
	NUROPOINT TPt = m_ptEndPt;
	nuLONG p     = 0;
	nuroPOINT p1 = FPt;   //?_?I
	nuroPOINT p3 = TPt;      //?P?_??
	nuroPOINT p2 = NPt;   //???I
	p=(p1.x - (p1.x - 1)) * (p3.y - p2.y) + (p2.x - (p1.x - 1)) * (p1.y - p3.y) + (p3.x - (p1.x - 1)) * (p2.y - p1.y); 

	if(p==0)
	{
	    g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_REACH_DESTINATION, 0, NULL);
	}
	else if(p < 0)
	{
	    g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_LEFT_DESTINATION, 0, NULL);
	}
	else if(p > 0)
	{
	    g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_RIGHT_DESTINATION, 0, NULL);
	}	
}
#define _MAX_NAME_MAPPING 256
nuINT CNaviThread::nRoadNameMapping(SEEKEDROADDATA &roadData, SEEKEDROADDATA &roadsList)
{
	nuINT nMappingIdx = 0, Addr = _MAX_NAME_MAPPING / 2;
	nuWCHAR wsCityName[_MAX_NAME_MAPPING] = {0}, wsTownName[_MAX_NAME_MAPPING] = {0}, wsRoadName[_MAX_NAME_MAPPING] = {0};
	RNEXTEND rnExtend = {0};
	if(!g_fileSysApi.FS_ReadCityTownName( roadsList.nDwIdx, 
		nuMAKELONG(roadsList.nTownID, roadsList.nCityID), 
		wsCityName, 
		sizeof(wsCityName)/sizeof(nuWCHAR) ))
	{
		return 0;
	}
	nuMemcpy(wsTownName, &wsCityName[Addr], sizeof(wsCityName)/sizeof(nuWCHAR));
	//nuMemmove(wsTownName, wsCityName + sizeof(wsCityName)/2, sizeof(wsCityName)/2);
	nuWcscpy((nuWCHAR*)wsRoadName, wsCityName);
	nuWcscat((nuWCHAR*)wsRoadName, wsTownName);
	nuMemset((nuWCHAR*)wsCityName, 0, sizeof(wsCityName));
	if(!g_fileSysApi.FS_GetRoadName(roadsList.nDwIdx, roadsList.nNameAddr, 
		roadsList.nRoadClass, wsCityName, RN_MAX_LEN, &rnExtend))
	{
		return 0;
	}
	nuWcscat((nuWCHAR*)wsRoadName, wsCityName);
	nuMemcpy((nuWCHAR*)roadsList.wsRoadName, wsRoadName, sizeof(nuWCHAR) * MAX_LISTNAME_NUM);
	for(nuINT i = 0; i < nuWcslen(wsRoadName); i++)
	{
		if(m_pMarketPoint->wDesName[i] != wsRoadName[i])
		{
			break;
		}
		else if(0x8DEF == wsRoadName[i])
		{
			nMappingIdx = _MAPPING_ROAD;
		}
		else if(0x8857 == wsRoadName[i])
		{
			nMappingIdx = _MAPPING_STREET;
		}
		else if(0x6BB5 == wsRoadName[i])
		{
			nMappingIdx = _MAPPING_SECTION;
		}
		else if(0x5DF7 == wsRoadName[i])
		{
			nMappingIdx = _MAPPING_ALLEY;
		}
		else if(0x5F04 == wsRoadName[i])
		{
			nMappingIdx = _MAPPING_LANE;
		}
	}
	return nMappingIdx;
}

nuUINT CNaviThread::TunnelSim(PACTIONSTATE pActionState)
{
	nuUINT nRes = DRAW_NO;
	SIMULATIONDATA simuData = {0};
	nuLONG nDis = 0;
	nuLONG LastTick  = 0;
	nuLONG TickTime  = 0;
	if(m_bFirstTunnel)
	{
		m_bFirstTunnel = nuFALSE;
		g_innaviApi.IN_SetSimulation();
		m_lMoveConst = nuGetTickCount();
		return 0;
	}
	if(nDis == 0)
	{
		LastTick  = nuGetTickCount();
		TickTime  = LastTick - m_lMoveConst;
		nDis		     = m_gpsData.nSpeed * (TickTime);
		nDis			 /= 3600;
		m_lMoveConst	 = LastTick;
	}
	if( g_innaviApi.IN_GetSimuTunnelData(nuFALSE, nDis, &simuData) )
 	{
		pActionState->ptNewMapCenter.x	= simuData.nX;
		pActionState->ptNewMapCenter.y	= simuData.nY;
		pActionState->nAngle			= (nuSHORT)simuData.nAngle;
		pActionState->bResetIconCarPos	= 1;
		pActionState->bDrawBmp1			= 1;
		pActionState->bDrawInfo			= 1;
		pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
		pActionState->nDrawMode			= DRAWMODE_GPS;
		pActionState->bNeedReloadMap	= 1;
		pActionState->bResetMapCenter	= 1;
		pActionState->bResetScale		= 1;
		pActionState->nSpeed			= m_gpsData.nSpeed;
		nRes	= DRAW_YES;
	}
	return nRes;
}
nuUINT CNaviThread::TimeToGetGPS()
{
	CODENODE codeNode = {0};
	codeNode.nActionType = AC_DRAW_GPSGETDATA;
	return m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
}
nuUINT CNaviThread::UISendGPSData(const nuPVOID pOrGpsData1, const nuPVOID pOrGpsData2)
{
	if(GPS_OPEN_ELEAD_3DR == m_nGPSMode)
	{
		g_gpsopenApi.GPS_ELEAD_TRAN((GYROGPSDATA_EL*)pOrGpsData1);
	}
	return TimeToGetGPS();
}
nuUINT CNaviThread::UI_TRIGGER_TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEvent)
{
	CODENODE codeNode = {0};
	codeNode.nDrawType = nShowTMCEvent;
	codeNode.nExtend   = nTMCMode;
	__android_log_print(ANDROID_LOG_INFO, "navi", "UI_TRIGGER_TMC_EVENT TMCMode %d, ShowTMCEvent %d", nTMCMode, nShowTMCEvent);
	if(0 == m_lUseTMC)
	{
		return 0;
	}
	switch(nTMCMode)
	{
	case _TMC_DISPLAY_MAP:
		codeNode.nActionType = AC_DRAW_TMC_DISPLAY_MAP;
		codeNode.nDrawType = nTMCMode;
		return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	case _TMC_ROUTING:
		codeNode.nActionType = AC_NAVI_TMC_ROUTE;
		return m_codeNavi.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	case _TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY:
		if(g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			codeNode.nActionType = AC_DRAW_TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY;
			return m_codeNavi.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		}
	case _TMC_FLUSH_MAP:
		codeNode.nActionType = AC_DRAW_TMC_FLUSH_MAP;
		return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	}
	return 0;
}
nuUINT CNaviThread::ReciveUIData(nuLONG DefNum, nuPVOID pUIData1, nuPVOID pUIData2)
{
	CODENODE codeNode = {0};
	MARKETPOINT CurrentMarket = {0};
	UI_DESTPOINT   *pTmpDESTPOINT = {0};
	CurrentMarket.index = 1;
	if(pUIData1 == NULL)
	{
		return 0;
	}
	switch(DefNum)
	{
	case _GET_UI_DAIWOO_BT_DATA:
		codeNode.nActionType = AC_DRAW_DAIWOO_BT;
		nuMemcpy(&m_UI_SEND_DAIWOO_BT, pUIData1, sizeof(UI_DAIWOO_BT));
		return m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	case _GET_UI_E_LEAD_GPS_DATA:
		//GPSOPEN Transfer
		if(GPS_OPEN_ELEAD_3DR == m_nGPSMode)
		{
			g_gpsopenApi.GPS_ELEAD_TRAN((GYROGPSDATA_EL*)pUIData1);
		}
		return TimeToGetGPS();
	case _GET_UI_NAVI_DES:
		/*if(g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			NuroNavigation(NAVIGATION_STOP);
		}
		NuroDelMarket(&CurrentMarket);
		CurrentMarket.x = g_fileSysApi.pDrawInfo->pcCarInfo->ptCarFixed.x;
		CurrentMarket.y = g_fileSysApi.pDrawInfo->pcCarInfo->ptCarFixed.y;
		m_pMarketPoint = &CurrentMarket;
		NuroSetMarket(m_pMarketPoint);
		pTmpDESTPOINT  = (UI_DESTPOINT*)pUIData1;
		nuMemset(m_pMarketPoint, 0, sizeof(MARKETPOINT));
		m_pMarketPoint->index = 1;
		m_pMarketPoint->x	  = pTmpDESTPOINT->pos.lon*100000;
		m_pMarketPoint->y	  = pTmpDESTPOINT->pos.lat*100000;
		NuroSetMarket(m_pMarketPoint);

		g_fileSysApi.pGdata->routeGlobal.RouteCount = 1;
		NuroNavigation(NAVIGATION_GPSROUTE);*/
		break;
	default:
		break;
	}
	return 0;
}
nuVOID CNaviThread::FirstGetDRGPS()
{
#ifndef NURO_OS_WINCE
	return;
#endif
	nuINT i = 0;
	for(i = 0; i < 3; i++)
	{
		IntervalGetGps();
		nuSleep(100);
		if(g_fileSysApi.pGdata->uiSetData.bFirstGPSReady)
		{
			break;
		}
	}
}
nuBOOL CNaviThread::SetNaviParser(nuINT nMode,nuINT nType)
{
	CODENODE codeNode = {0};
	nuBOOL   bRt	  = nuFALSE;
	if(nType)
	{
		if(nMode == UI_SET_DATA_NEARPOI)
		{
			codeNode.nActionType = AC_DRAW_NEARPOI;
		}
		if(nMode == UI_SET_DATA_JUMPROAD)
		{
			if(g_fileSysApi.pGdata->timerData.nAutoMoveType != AUTO_MOVE_TIMER)
			{
				codeNode.nActionType = AC_DRAW_JUMPROAD;
			}
			else
			{
				return 0;
			}
		}
		bRt = m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		m_drawToUI.bUIWaiting	= 1;
		while(m_drawToUI.bUIWaiting)
		{
			nuSleep(50);
		}
	}
	return bRt;
}
nuVOID CNaviThread::ColEtag()
{
	nuLONG dx = 0, dy = 0, Dis = 0, HighwayID = -1, HighWayCrossID = -1;
	nuBOOL bFindETG = nuFALSE, bEnd = nuFALSE, bSearchCrossRoad = nuFALSE;
	CROSSROADINFO CrossRoadInfo = {0};
	nuMemset(&CrossRoadInfo, 0, sizeof(CROSSROADINFO));
	//get etag id
	if(m_lSIMEtag && g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
	{
		if(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadClass <= 5)
		{
			g_innaviApi.IN_GetCorssRoadData(CrossRoadInfo);
			bSearchCrossRoad = nuTRUE;
		}
		else if(m_lSendEtag > 0)
		{
			bEnd = nuTRUE;
		}
	}
	else if(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_GPS)
	{
		if(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass <= 5)
		{
			if(g_fileSysApi.pGdata->uiSetData.bNavigation)
			{
				g_innaviApi.IN_GetCorssRoadData(CrossRoadInfo);
			}
			else
			{
				nuroRECT rtMap;				
				rtMap.left	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x - _ICT_CHECK_DIS;
				rtMap.right	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x + _ICT_CHECK_DIS;
				rtMap.top	= g_fileSysApi.pGdata->carInfo.ptCarFixed.y - _ICT_CHECK_DIS;
				rtMap.bottom= g_fileSysApi.pGdata->carInfo.ptCarFixed.y + _ICT_CHECK_DIS;
				CrossRoadInfo.nRoadBlkIdx    = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
				CrossRoadInfo.nRoadRdIdx     = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
				CrossRoadInfo.ptRoadNextCoor = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptNextCoor;
				CrossRoadInfo.nOneWay        = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nOneWay;
				CrossRoadInfo.nRoadClass     = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass;
				CrossRoadInfo.nRoadMapIdx    = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
				g_fileSysApi.FS_SeekForCorssRoadInfo(rtMap, &CrossRoadInfo);
			}
			bSearchCrossRoad = nuTRUE;
		}
		else if(m_lSendEtag > 0)
		{
			bEnd = nuTRUE;
		}
	}
	if(bSearchCrossRoad)
	{
		for(nuINT i = 0; i < CrossRoadInfo.nRoadCount; i++)
		{
			if(g_fileSysApi.FS_SearchICT(CrossRoadInfo.pCrossRoad[i].nNextBlkIdx, 
				CrossRoadInfo.pCrossRoad[i].nNextRdIdx,
				g_fileSysApi.pGdata->carInfo.ptCarIcon, 
				m_ptETAG,
				HighwayID,
				HighWayCrossID))
			{

				bFindETG = nuTRUE;
				break;
			}
		}
	}
	
	CALL_BACK_PARAM cbParam;
	m_ICT_DATA.HighWayID = HighwayID;
	m_ICT_DATA.HighWayCrossID = HighWayCrossID;
	cbParam.nCallType = CALLBACK_TYPE_ETAG;
	cbParam.pVOID	  = &m_ICT_DATA;
	cbParam.lDataLen  = sizeof(m_ICT_DATA);
	if(bFindETG && (m_lHighwayID != HighwayID || m_lHighWayCrossID != HighWayCrossID) && !bEnd)
	{
		m_lHighwayID = HighwayID;
		m_lHighWayCrossID = HighWayCrossID;
		if(m_lSendEtag <= 0)
		{
			m_ICT_DATA.nPassState = _ICT_ENTER;
			m_lSendEtag++;
			g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
		}
		else
		{
			m_ICT_DATA.nPassState = _ICT_PASS;
			m_lSendEtag++;
			g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
		}
	}
	else if(bEnd)
	{
		m_ICT_DATA.nPassState = _ICT_END;
		m_ICT_DATA.HighWayCrossID = m_lHighWayCrossID;
		m_ICT_DATA.HighWayID = m_lHighwayID;
		m_lSendEtag = 0;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
		m_lHighwayID = 0;
		m_lHighWayCrossID = 0;
	}
}
nuVOID CNaviThread::ColNearPoi()
{
	g_fileSysApi.FS_SeekForPoiType( g_fileSysApi.pGdata->carInfo.ptCarFixed.x,
		g_fileSysApi.pGdata->carInfo.ptCarFixed.y,
		CENTERPOI_SEEK_MAX_DIS,
		g_fileSysApi.pGdata->carInfo.nCarAngle,
		&g_fileSysApi.pGdata->drawInfoMap.poiCenterOn );
}

nuBOOL CNaviThread::JumpRoad(PACTIONSTATE pActionState)
{
	/*m_bNavi = nuFALSE;
	
	nuBOOL bChooseRoad  = nuFALSE;
	SEEK_ROADS_IN	seekRdsIn  = {0};
	SEEK_ROADS_OUT	seekRdsOut = {0};
	seekRdsIn.point		= pActionState->ptNewMapCenter;
	seekRdsIn.nSeekDis	= CENTERROAD_SHOW_MAX_LEN;
	seekRdsIn.nAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;

	SEEKEDROADDATA roadCenterOn = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn;
	PSEEK_ROADS_OUT pSeekRdsOut = &seekRdsOut;
	SEEKEDROADDATA oldRoadCarOn = g_fileSysApi.pGdata->drawInfoMap.roadCarOn;


	nuINT nIdx = 0;
	if( g_fileSysApi.FS_SeekForRoads(&seekRdsIn, &seekRdsOut, nuFALSE) )
	{	
		RNEXTEND rnEx = {0};
		static nuINT OldCarAngle=0;



		if( pSeekRdsOut == NULL || pSeekRdsOut->nCountSeeked == 0 )
		{
			OldCarAngle=g_fileSysApi.pGdata->carInfo.nCarAngle;
			return nuFALSE;
		}
		
		for( nuBYTE i = 0; i < pSeekRdsOut->nCountSeeked; ++i )
		{

			if(	pSeekRdsOut->roadsList[i].nBlkIdx  == m_SeekedRoadData.nBlkIdx &&
				( pSeekRdsOut->roadsList[i].nRoadIdx == m_SeekedRoadData.nRoadIdx))					
			{
				pSeekRdsOut->roadsList[i].nDis	+= 50;
			}

			if(pSeekRdsOut->roadsList[i].nLevel == m_SeekedRoadData.nLevel)
			{
				pSeekRdsOut->roadsList[i].nDis	+= 30;
			}
			for(nuINT j=0;j<m_nJumpRoadAdd;j++)
			{	
				if(m_nJumpRoadBlkIdx[j]== pSeekRdsOut->roadsList[i].nBlkIdx && m_nJumpRoadIdx[j]== pSeekRdsOut->roadsList[i].nRoadIdx)
				{
					pSeekRdsOut->roadsList[i].nDis	+= m_nJumpRoadWeight[j];
				}
			}
			if( pSeekRdsOut->roadsList[i].nDis  < pSeekRdsOut->roadsList[nIdx].nDis )
			{
				nIdx = i;
			}
			continue;
		}
	}
	if	(pSeekRdsOut->nCountSeeked ==1 ||
		(pSeekRdsOut->roadsList[nIdx].nBlkIdx    == m_SeekedRoadData.nBlkIdx  &&
		(pSeekRdsOut->roadsList[nIdx].nRoadIdx   == m_SeekedRoadData.nRoadIdx  )))
	{
		bChooseRoad = nuTRUE;
	}
	if(bChooseRoad)
	{
		return nuFALSE;
	}	
	
	m_bNavi = nuFALSE;
	
	if( g_fileSysApi.pGdata->uiSetData.bNavigation )
	{

		m_bNavi = nuTRUE;

		m_nTargetCount =  g_fileSysApi.pGdata->routeGlobal.TargetCount- g_fileSysApi.pUserCfg->nPastMarket ;
		

		for(nuINT i=1,j=1 ; j < m_nTargetCount; i++)
			TargetDestInfo[j++] = g_fileSysApi.pGdata->routeGlobal.pTargetInfo[ g_fileSysApi.pUserCfg->nPastMarket+i];


		CODENODE codeNode = {0};
		nuBOOL   bRt	  = nuFALSE;
		m_nType = g_fileSysApi.pGdata->timerData.nAutoMoveType;
		codeNode.nActionType = AC_NAVI_STOPROUTE;
		if( !m_codeNavi.PushNode(&codeNode) )
		{
			m_drawToUI.bUIWaiting	= 0;
			//return nuFALSE;
		}

	}
	pActionState->nCarCenterMode	= CAR_CENTER_MODE_THREE_QUARTERS_Y;
	pActionState->bResetIconCarPos	= 1;
	pActionState->bResetRealCarPos	= 1;
	pActionState->bResetMapCenter =1;
	m_bJumpRoad = nuTRUE;
*/
	return nuTRUE;
}
nuUINT CNaviThread::TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEvent, PACTIONSTATE pActionState)
{
	TARTGETINFO TargetInfo = {0};
	PTMC_ROUTER_DATA pTMCRTData = NULL;
	nuLONG nResRoute		= ROUTE_ERROR;
	SIMULATIONDATA simuData;
	NURO_DrawInfo DrawInfo = {0};
	CALL_BACK_PARAM cbParam = {0};
	/*if(!m_bTMC_Event_Data_Initial)
	{
		if(nShowTMCEvent)
		{
			if(!g_fileSysApi.FS_Initial_TMC_Event_Data(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx))
				return 0;
			m_bTMC_Event_Data_Initial = nuTRUE;
		}
		else
		{
			//g_fileSysApi.FS_Release_TMC_Event_Data(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx);
			return 1;
		}
	}*/
	switch(nTMCMode)
	{
	case _TMC_DISPLAY_MAP:
//		OutputDebugStringA("_TMC_DISPLAY_MAP\n");
		pActionState->bDrawBmp1 = nuTRUE;
		pActionState->bDrawInfo = nuTRUE;
		m_nFlush_Map_For_TMC = nuTRUE;
		g_fileSysApi.FS_COPY_TMC_Event_Data(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx);
		break;
	case _TMC_ROUTING:
//		OutputDebugStringA("\n_TMC_ROUTING*******************************   A  \n");
		if( g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			/*
			SIMULATIONDATA simuData;
			if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
			{
				pActionState->ptNewMapCenter.x = simuData.nX;
				pActionState->ptNewMapCenter.y = simuData.nY;
				pActionState->nAngle		   = (short)simuData.nAngle;
			}*/
			
			g_pSoundThread->CleanSound();	
			TargetInfo.BaseCoor	= g_fileSysApi.pGdata->carInfo.ptCarRaw;
			TargetInfo.FixCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
			TargetInfo.MapIdx	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
			TargetInfo.BlockIdx	= (nuSHORT)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
			TargetInfo.RoadIdx	= (nuSHORT)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
			pTMCRTData = NULL;
			m_nTMCDataCount = 0;

			DrawInfo.bRouting = 1;
			cbParam.nCallType = CALLBACK_TYPE_JAVA_DATA;
			cbParam.pVOID	  = &DrawInfo;
			g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);

			//g_innaviApi.IN_TMCRoute(pTMCRTData, m_nTMCDataCount, TargetInfo, g_fileSysApi.pGdata->routeGlobal.RoutingRule);
			g_fileSysApi.pGdata->timerData.bIsRouting = 1;
			g_memMgrApi.MM_CollectDataMem(1);
			nResRoute = g_innaviApi.IN_TMCRoute(pTMCRTData, m_nTMCDataCount, TargetInfo, g_fileSysApi.pGdata->routeGlobal.RoutingRule);

			g_innaviApi.IN_NaviStart();
			
			if( g_innaviApi.IN_GetSimuData(nuTRUE, 0, &simuData) )
			{
				pActionState->ptNewMapCenter.x = g_fileSysApi.pGdata->carInfo.ptCarRaw.x;//simuData.nX;
				pActionState->ptNewMapCenter.y = g_fileSysApi.pGdata->carInfo.ptCarRaw.y;//simuData.nY;

				pActionState->nAngle = (short)g_fileSysApi.pGdata->carInfo.nCarAngle;
			}

			g_fileSysApi.pGdata->timerData.bIsRouting = 0;
			DrawInfo.bRouting = 0;
			cbParam.nCallType = CALLBACK_TYPE_JAVA_DATA;
			cbParam.pVOID	  = &DrawInfo;
			g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
		}
		break;
	case _TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY:
		if( g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			g_innaviApi.IN_TMCNaviPathData();		
		}
		break;
	}
	return 1;
}

nuBOOL CNaviThread::Nuro_NT_Exit()
{
	CODENODE codeNode	 = {0};
	codeNode.nActionType = AC_DRAW_QIUT;
	m_codePre.PushNode(&codeNode);
	g_fileSysApi.pGdata->bWaitQuit = nuFALSE;
	while(g_fileSysApi.pGdata->bWaitQuit)
	{
		nuSleep(50);
	}
	return nuTRUE;	
}
//Added by Daniel ================Choose Road===START========== 20150207//
nuUINT CNaviThread::Nuro_Choose_Car_Road(nuBYTE nChosenRoad)
{
	CODENODE codeNode		 = {0};
	codeNode.nActionType	 = AC_DRAW_CHOSE_ROAD;
	codeNode.nDrawType		 = nChosenRoad;
	return m_codeDraw.PushNode(&codeNode);
}
nuVOID CNaviThread::Choose_Car_Road(nuBYTE nChosenRoad)
{
	CODENODE		CodeNode   = {0};
	SEEK_ROADS_IN	SeekRdsIn  = {0};
	SEEK_ROADS_OUT	SeekRdsOut = {0}; 
	m_bChooseRoad			   = nuTRUE;
	if(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
	{
		return ;

	}

	if(_NO_CHOOSE == nChosenRoad)
	{
		m_nChosenRoadMode = _CHOOSE_MODE_PASS_CURRENT_ROAD;
		if(g_fileSysApi.pGdata->uiSetData.bNavigation)
		{
			SeekRdsIn.point		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
			SeekRdsIn.nSeekDis	= CENTERROAD_SHOW_MAX_LEN;
			SeekRdsIn.nAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle;
			if( g_fileSysApi.FS_SeekForRoads(&SeekRdsIn, &SeekRdsOut, nuFALSE) )
			{
				ChooseCenterRoad( g_fileSysApi.pGdata->drawInfoMap.roadCenterOn,
									&SeekRdsOut,
									g_fileSysApi.pGdata->drawInfoMap.roadCarOn );
			}
			/*if(g_fileSysApi.pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
			{
				CodeNode.nActionType = AC_NAVI_STOPROUTE;
				m_codeNavi.PushNode(&CodeNode);
				CodeNode.nActionType = AC_NAVI_STARTROUTE;
				CodeNode.nExtend     = AUTO_MOVE_SIMULATION;
				m_codeNavi.PushNode(&CodeNode);
			}
			else*/
			{
				if(m_nChosenRoadCount > 0)
				{
					if( (m_Road_Info_Data[m_nChosenRoadCount - 1].nDwIdx != m_SeekedRoadData.nDwIdx	||
						 m_Road_Info_Data[m_nChosenRoadCount - 1].nBlkIdx != m_SeekedRoadData.nBlkIdx ||
						 m_Road_Info_Data[m_nChosenRoadCount - 1].nRoadIdx != m_SeekedRoadData.nRoadIdx))
					{
						CodeNode.nActionType = AC_NAVI_REROUTE;
						m_codeNavi.PushNode(&CodeNode);
					}
				}
			}
		}
	}
}
//Added by Daniel ================Choose Road===END========== 20150207//
nuVOID CNaviThread::NuroJavaData()
{
	if(NULL == g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		return;
	}
	nuUINT nCount = 0;
	NURO_DrawInfo DrawInfo = {0};
	CALL_BACK_PARAM cbParam = {0};
	
	//Navi Mode
	DrawInfo.bNaviState = m_DrawDataForUI.nddNaviState;
	//Move Mode
	DrawInfo.nMoveState = g_fileSysApi.pGdata->timerData.nAutoMoveType;
	//==================================DrawMapInfo=============================
	//CCD
 	if(m_DrawDataForUI.nddCCDNum > 0)
	{
		DrawInfo.DrawMapInfo.bCCD = nuTRUE;
	}
	else
	{
		DrawInfo.DrawMapInfo.bCCD = nuFALSE;
	}
	DrawInfo.DrawMapInfo.bShowPic  = m_bDrawRealPic;
	//CCD Speed
	DrawInfo.DrawMapInfo.lCCDSpeed = m_DrawDataForUI.nddLimitSpeed;
	//CCD Distance
	
	//Car Point
	DrawInfo.DrawMapInfo.ptCurrentCarX = m_DrawDataForUI.nddCarInScreen.x;
	DrawInfo.DrawMapInfo.ptCurrentCarY = m_DrawDataForUI.nddCarInScreen.y;
	//Mavp Car Point
	DrawInfo.DrawMapInfo.ptMapCarX = g_fileSysApi.pGdata->carInfo.ptCarFixed.x;
	DrawInfo.DrawMapInfo.ptMapCarY = g_fileSysApi.pGdata->carInfo.ptCarFixed.y;
	//Mavp Car Point
	DrawInfo.DrawMapInfo.ptMapCenterX = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped.x;
	DrawInfo.DrawMapInfo.ptMapCenterY = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.ptMapped.y;

	//car angle
	DrawInfo.DrawMapInfo.lCarAngle     = m_DrawDataForUI.nddCarAngle;
	//map angle
	DrawInfo.DrawMapInfo.lMapAngle     = m_DrawDataForUI.nddMapAngle;
	//Speed Limit
	DrawInfo.DrawMapInfo.nSpeedLimit = m_DrawDataForUI.nddLimitSpeed;
	//CarOnMapID
	DrawInfo.DrawMapInfo.CarOnMapID = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
	//CarOnCityID
	DrawInfo.DrawMapInfo.CarOnCityID = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nCityID;
	//CarOnTownID
	DrawInfo.DrawMapInfo.CarOnTownID = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nTownID;
	//CenterOnMapID
	DrawInfo.DrawMapInfo.CenterOnMapID = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
	//CenterOnCityID
	DrawInfo.DrawMapInfo.CenterOnCityID = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nCityID;
	//CenterOnTownID
	DrawInfo.DrawMapInfo.CenterOnTownID = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nTownID;
	//ScaleValue
	DrawInfo.DrawMapInfo.nScaleValue = m_DrawDataForUI.nddNowScale;
	//DirectionAngle
	DrawInfo.DrawMapInfo.nDirectionAngle = m_DrawDataForUI.nddMapAngle;
	//CityName
	nuMemset(DrawInfo.DrawMapInfo.wsCityName, 0, sizeof(DrawInfo.DrawMapInfo.wsCityName));
	nuMemcpy((nuWCHAR*)DrawInfo.DrawMapInfo.wsCityName, m_DrawDataForUI.nddCityName, sizeof(nuWCHAR) * _MAX_CITYNAME);
	//RoadName
	nuMemset(DrawInfo.DrawMapInfo.wsRoadName, 0, sizeof(DrawInfo.DrawMapInfo.wsRoadName));
	if(g_fileSysApi.pGdata->drawInfoMap.bFindCenterRoad)
	{
		nuMemcpy((nuWCHAR*)DrawInfo.DrawMapInfo.wsRoadName, g_fileSysApi.pGdata->drawInfoMap.wsCenterName, sizeof(nuWCHAR) * _MAX_ROADNAME);
	}
	//PoiName
	nuMemset(DrawInfo.DrawMapInfo.wsCenterPoi, 0, sizeof(DrawInfo.DrawMapInfo.wsCenterPoi));
	//KM Name
	nuMemset(DrawInfo.DrawMapInfo.wsKiloName, 0, sizeof(DrawInfo.DrawMapInfo.wsKiloName));
	nuMemcpy((nuWCHAR*)DrawInfo.DrawMapInfo.wsKiloName, m_wsKMName, sizeof(nuWCHAR) * 32);
	//=============================NaviInfo======================================
	//Next Distance
	DrawInfo.DrawNaviInfo.lDistance = m_DrawDataForUI.nddDisToNextCross;
	//Target Distance
	DrawInfo.DrawNaviInfo.lDisToTarget = m_DrawDataForUI.nddDisToTarget;
	//Target Time
	DrawInfo.DrawNaviInfo.lTimeToTarget = m_DrawDataForUI.nddTimeToTarget;
	//Next IconType
	DrawInfo.DrawNaviInfo.nIconType = m_DrawDataForUI.nddCrossIconType;
	DrawInfo.DrawNaviInfo.nIconType = m_HudComCtrl.GetHUDIconType(DrawInfo.DrawNaviInfo.nIconType);
	//__android_log_print(ANDROID_LOG_INFO,  "NaviIcon" ,"Navi nIconType %d:%d ", DrawInfo.DrawNaviInfo.nIconType, m_DrawDataForUI.nddCrossIconType);
	//Next RoadName
	nuMemset(DrawInfo.DrawNaviInfo.wsRoadName, 0, sizeof(DrawInfo.DrawNaviInfo.wsRoadName));
	nuMemcpy((nuWCHAR*)DrawInfo.DrawNaviInfo.wsRoadName, m_DrawDataForUI.nddNextRdName, sizeof(nuWCHAR) * _MAX_ROADNAME);
	//TotalList
	DrawInfo.DrawNaviInfo.lNextRoadCount = m_DrawDataForUI.nddTotalListCount;
	//RealPic Distance
	DrawInfo.DrawNaviInfo.lRealCrossdDis = m_DrawDataForUI.nddDisRealPic;
	//RealPic ID
	DrawInfo.DrawNaviInfo.lRealID	     = m_DrawDataForUI.nddRealPicID;
	//PassNum
	DrawInfo.bRouting = g_fileSysApi.pGdata->timerData.bIsRouting;
	//RoadListNum
	DrawInfo.DrawNaviInfo.RoadListNum = m_DrawDataForUI.nddRtListNowNum;
	//ListNode
	for(nCount = 0;nCount < _MAX_NEXTROADLIST; nCount ++)
	{
		nuMemset(&DrawInfo.DrawNaviInfo.RoadList[nCount], 0, sizeof(NURO_NaviList));
		DrawInfo.DrawNaviInfo.RoadList[nCount].lDistance = m_DrawDataForUI.nddListNode[nCount].nDistance;
		DrawInfo.DrawNaviInfo.RoadList[nCount].nIconType = m_HudComCtrl.GetHUDListIconType(m_DrawDataForUI.nddListNode[nCount].nIconType);
         	//__android_log_print(ANDROID_LOG_INFO,  "NaviIcon" ,"Navi List nIconType %d:%d", DrawInfo.DrawNaviInfo.RoadList[nCount].nIconType, m_DrawDataForUI.nddListNode[nCount].nIconType);
		//nuMemcpy(DrawInfo.DrawNaviInfo.RoadList[nCount].wsIconNum, m_DrawDataForUI.nddListNode[nCount].wsIconName, sizeof(nuWCHAR) * _MAX_ICONNUM);
		nuMemcpy(DrawInfo.DrawNaviInfo.RoadList[nCount].wsRoadName, m_DrawDataForUI.nddListNode[nCount].wsName, sizeof(nuWCHAR) * _MAX_ROADLISTNAME);
	}
	//LaneGuideData
	nuMemset(DrawInfo.DrawNaviInfo.LaneGuideData.bLaneEnable, 0, sizeof(DrawInfo.DrawNaviInfo.LaneGuideData.bLaneEnable));
	nuMemset(DrawInfo.DrawNaviInfo.LaneGuideData.nLaneID, 0, sizeof(DrawInfo.DrawNaviInfo.LaneGuideData.nLaneID));
	DrawInfo.DrawNaviInfo.LaneGuideData.lLaneCount = g_fileSysApi.pGdata->drawInfoNavi.LaneGuide.LaneCount;
	for(nCount = 0; nCount < DrawInfo.DrawNaviInfo.LaneGuideData.lLaneCount; nCount++)
	{
		DrawInfo.DrawNaviInfo.LaneGuideData.bLaneEnable[nCount] = g_fileSysApi.pGdata->drawInfoNavi.LaneGuide.bLight[nCount];
		DrawInfo.DrawNaviInfo.LaneGuideData.nLaneID[nCount] = g_fileSysApi.pGdata->drawInfoNavi.LaneGuide.LightNum[nCount];
__android_log_print(ANDROID_LOG_INFO, "LaneGuide", "Send Enable %d, LightID %d", g_fileSysApi.pGdata->drawInfoNavi.LaneGuide.bLight[nCount], g_fileSysApi.pGdata->drawInfoNavi.LaneGuide.LightNum[nCount]); 
	}
	nuINT i = 0 , j = 0;
	nuTCHAR tsFile[128] = {0}, tsMapPath[128] = {0};
	nuTcscpy(tsFile, nuTEXT(".BH"));
	g_fileSysApi.FS_FindFileWholePath(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx, tsFile, NURO_MAX_PATH);
	for(i = nuTcslen(tsFile); i > 0; i--)
	{
		if(tsFile[i] == '\\')
		{
			i++;
			break;
		}
	}
	for(; i < nuTcslen(tsFile); i++, j++)
	{
		if(tsFile[i] == '.')
		{
			break;
		}
		tsMapPath[j] = tsFile[i];
	}
	nuTcscpy(DrawInfo.DrawMapInfo.tsMapPath, tsMapPath);
	DrawInfo.DrawMapInfo.lDate = m_gpsData.nTime.nYear * 10000 + m_gpsData.nTime.nMonth * 100 + m_gpsData.nTime.nDay;
	DrawInfo.DrawMapInfo.lTime = m_gpsData.nTime.nHour * 10000 + m_gpsData.nTime.nMinute * 100 + m_gpsData.nTime.nSecond;
	DrawInfo.DrawMapInfo.dcourse = m_gpsData.nAngle;
	DrawInfo.DrawMapInfo.lGPSX   = m_gpsData.nLongitude;
	DrawInfo.DrawMapInfo.lGPSY   = m_gpsData.nLatitude;
	DrawInfo.DrawMapInfo.lSpeed  = m_gpsData.nSpeed;
	DrawInfo.DrawMapInfo.nGPSState = m_gpsData.nStatus;
	DrawInfo.DrawMapInfo.nRoadDis = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDis;
	DrawInfo.DrawMapInfo.nRdDx   = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDx;
	DrawInfo.DrawMapInfo.nRdDy   = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRdDy;
	
	//__android_log_print(ANDROID_LOG_INFO,  "navi" ,"NuroJavaData..");
	cbParam.nCallType = CALLBACK_TYPE_JAVA_DATA;
	cbParam.pVOID	  = &DrawInfo;
	g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
}
nuLONG CNaviThread::NuroUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
{
	CODENODE codeNode		 = {0};
	NURO_BUTTON_EVENT  ButtonEvent   = {0};
	NURO_POINT  Pt			 = {0};
	NURO_POINT2 Pt2			 = {0};
	nuUINT	  nMode			 = 0;
	NURO_NAVI_POINT	NaviPoint	 = {0};
	NURO_TMC_EVENT  TMC_DATA	 = {0};
	NUROPOINT ptMouse		 = {0};
	switch(DefNum)
	{
	case NURO_DEF_UI_SET_BUTTON_EVENT:
		if(pUIData != NULL)
		{	
			nuMemcpy(&ButtonEvent, pUIData, sizeof(NURO_BUTTON_EVENT));
			NuroButtonEvent(ButtonEvent.nCode, ButtonEvent.nButState);
		}		
		break;
	case NURO_DEF_UI_SET_MOVE_MAP:
		break;
	case NURO_DEF_UI_SET_USER_DATA:
		if(pUIData != NULL)
		{
			m_bSetUserData = nuTRUE;
			nuMemcpy(&m_UISetUserConfig, pUIData, sizeof(NURO_USER_SET_DATA));
			codeNode.nActionType	    = AC_DRAW_SET_USER_DATA;
			codeNode.nDrawType	    = DRAWTYPE_UI;
			m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
			m_drawToUI.bUIWaiting	= 1;
			while(m_drawToUI.bUIWaiting)
			{
				nuSleep(50);
			}
			m_bSetUserData = nuFALSE;
		}		
		break;
	case NURO_DEF_UI_SET_CAR_POSITION:
		break;
	case NURO_DEF_UI_SET_NAVI_POINT:
		nuMemset(&m_MarketPoint, 0, sizeof(m_MarketPoint));
		nuMemset(&NaviPoint, 0, sizeof(NaviPoint));
		if(pUIData != NULL)
		{
			nuMemcpy(&NaviPoint, pUIData, sizeof(NURO_NAVI_POINT));
		}
		if(NaviPoint.nRouteType == -1)
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroDelMarket");
			m_MarketPoint.index = -1;
			NuroDelMarket((nuPVOID)&m_MarketPoint);
		}
		else
		{
			m_MarketPoint.x = NaviPoint.nX;
			m_MarketPoint.y = NaviPoint.nY;
			m_MarketPoint.index = NaviPoint.nNaviIndex;	
			NuroSetMarket((nuPVOID)&m_MarketPoint);
			if(NaviPoint.bRoute)
			{
				g_fileSysApi.pGdata->routeGlobal.RoutingRule = NaviPoint.nRouteType;
				NuroNavigation(2);
			}
			g_fileSysApi.pUserCfg->bLastNavigation = g_fileSysApi.pGdata->uiSetData.bNavigation;
		}	
		break;
	case NURO_DEF_UI_SET_MAP_CENTER:
		if(pUIData != NULL)
		{
			nuMemcpy(&Pt, pUIData, sizeof(NURO_POINT));
			codeNode.nActionType	    = AC_DRAW_OPERATEMAPXY;
			codeNode.nDrawType	    = DRAWTYPE_UI;
			codeNode.nX		    = Pt.nX;
			codeNode.nY		    = Pt.nY;
			m_codeDraw.PushNode(&codeNode, PUSH_ADD_ATHEAD);
		}
		break;
	case NURO_DEF_UI_SET_NAVIGATION:
		if(pUIData != NULL)
		{
			nuMemcpy(&nMode, pUIData, sizeof(nuINT));
			NuroNavigation((nuBYTE)nMode);
		}
		break;
	case NURO_DEF_UI_SET_PACK_PIC:
		NuroUIGetPicBuffer((PNURO_PACK_PIC_PARAMETER)pUIData, (nuBYTE*)pUIReturnData);
		break;
	case NURO_DEF_UI_SET_TMC_EVENT:
		if(pUIData != NULL)
		{
			nuMemcpy(&TMC_DATA, pUIData, sizeof(NURO_TMC_EVENT));
			UI_TRIGGER_TMC_EVENT(TMC_DATA.nMode, TMC_DATA.nShowEvent);
		}
		break;
	case NURO_DEF_UI_SET_CAR_ROAD:
		Nuro_Choose_Car_Road(_NO_CHOOSE);
		break;
	default:
		break;
	}
	return 0;
}

nuVOID CNaviThread::NURO_UI_SET_USER_DATA()
{
	if(!m_bSetUserData)
	{
		return ;
	}
	g_fileSysApi.pUserCfg->bNightDay 		= 	(nuBYTE)m_UISetUserConfig.bNightDay;
	g_fileSysApi.pUserCfg->b3DMode 			=       (nuBYTE)m_UISetUserConfig.b3DMode;
	g_fileSysApi.pUserCfg->nLanguage		=     	(nuBYTE)m_UISetUserConfig.nLanguage;
	g_fileSysApi.pUserCfg->nMapDirectMode		=     	(nuBYTE)m_UISetUserConfig.nMapDirectMode;
	g_fileSysApi.pUserCfg->nZoomAutoMode		=     	(nuBYTE)m_UISetUserConfig.nZoomAutoMode;
	g_fileSysApi.pUserCfg->nPlaySpeedWarning	=     	(nuBYTE)m_UISetUserConfig.nPlaySpeedWarning;
	g_fileSysApi.pUserCfg->nSimSpeed		=     	(nuWORD)m_UISetUserConfig.nSimSpeed;
	g_fileSysApi.pUserCfg->bAutoDayNight		=     	(nuBYTE)m_UISetUserConfig.bAutoDayNight;
	g_fileSysApi.pUserCfg->nCarIconType		= 	(nuBYTE)m_UISetUserConfig.nCarIconType;
	g_fileSysApi.pUserCfg->nDayBgPic		=     	(nuBYTE)m_UISetUserConfig.nDayBgPic;
	g_fileSysApi.pUserCfg->nNightBgPic		=     	(nuBYTE)m_UISetUserConfig.nNightBgPic;
	g_fileSysApi.pUserCfg->nRouteMode		=     	(nuBYTE)m_UISetUserConfig.nRouteMode;
	g_fileSysApi.pUserCfg->bLastNavigation		=     	(nuBYTE)m_UISetUserConfig.bLastNavigation;
	g_fileSysApi.pUserCfg->nPastMarket		=     	(nuBYTE)m_UISetUserConfig.nPastMarket;
	g_fileSysApi.pUserCfg->nVoiceControl		=     	(nuBYTE)m_UISetUserConfig.nVoiceControl;
	g_fileSysApi.pUserCfg->nLastScaleIndex		=     	(nuBYTE)m_UISetUserConfig.nLastScaleIndex;
	g_fileSysApi.pUserCfg->nSpNumInput		=     	(nuBYTE)m_UISetUserConfig.nSpNumInput;
	g_fileSysApi.pUserCfg->nKeyWordInput		=     	(nuBYTE)m_UISetUserConfig.nKeyWordInput;
	g_fileSysApi.pUserCfg->nSpeedWarningPic		=     	(nuBYTE)m_UISetUserConfig.nSpeedWarningPic;
	g_fileSysApi.pUserCfg->nUILanguage		=     	(nuBYTE)m_UISetUserConfig.nUILanguage;
	g_fileSysApi.pUserCfg->nSpeedWarningVoice	=     	(nuBYTE)m_UISetUserConfig.nSpeedWarningVoice;
	g_fileSysApi.pUserCfg->nRoadInput		=     	(nuBYTE)m_UISetUserConfig.nRoadInput;
	g_fileSysApi.pUserCfg->nPoiInput		=     	(nuBYTE)m_UISetUserConfig.nPoiInput;
	g_fileSysApi.pUserCfg->nScaleIdx		=     	(nuBYTE)m_UISetUserConfig.nScaleIdx;
	g_fileSysApi.pUserCfg->nSoundVolume		=     	(nuBYTE)m_UISetUserConfig.nSoundVolume;
	g_fileSysApi.pUserCfg->nTMCSwitchFlag		=     	(nuBYTE)m_UISetUserConfig.nTMCSwitchFlag;
	g_fileSysApi.pUserCfg->nTMCFrequency		=     	(nuBYTE)m_UISetUserConfig.nTMCFrequency;
	g_fileSysApi.pUserCfg->nTMCVoice		=     	(nuBYTE)m_UISetUserConfig.nTMCVoice;
	g_fileSysApi.pUserCfg->nTimeToNight		=     	(nuBYTE)m_UISetUserConfig.nTimeToNight;
	g_fileSysApi.pUserCfg->nTimeToDay		=     	(nuBYTE)m_UISetUserConfig.nTimeToDay;
	g_fileSysApi.pUserCfg->bCCDVoice		=     	(nuBYTE)m_UISetUserConfig.bCCDVoice;
	g_fileSysApi.pUserCfg->bBigMapRoadFont		=     	(nuBYTE)m_UISetUserConfig.bBigMapRoadFont;
	g_fileSysApi.pUserCfg->nRouteOption		=     	(nuBYTE)m_UISetUserConfig.nRouteOption;
	g_fileSysApi.pUserCfg->nWaveTTSType		=     	(nuBYTE)m_UISetUserConfig.nWaveTTSType;
	g_fileSysApi.pUserCfg->nShowTip			=     	(nuBYTE)m_UISetUserConfig.nShowTip;
	g_fileSysApi.pUserCfg->nShowPOI			=     	(nuBYTE)m_UISetUserConfig.nShowPOI;
}
nuVOID CNaviThread::NURO_UI_GET_USER_DATA(nuPVOID pUIData)
{
	if(NULL == pUIData)
	{
		return ;
	}
	m_UISetUserConfig.bNightDay 			= g_fileSysApi.pUserCfg->bNightDay;

    	m_UISetUserConfig.b3DMode 			= g_fileSysApi.pUserCfg->b3DMode;
	m_UISetUserConfig.nLanguage 			= g_fileSysApi.pUserCfg->nLanguage;
	m_UISetUserConfig.nMapDirectMode 		= g_fileSysApi.pUserCfg->nMapDirectMode;
	m_UISetUserConfig.nZoomAutoMode			= g_fileSysApi.pUserCfg->nZoomAutoMode;
	m_UISetUserConfig.nPlaySpeedWarning 		= g_fileSysApi.pUserCfg->nPlaySpeedWarning;
	m_UISetUserConfig.nSimSpeed		        = g_fileSysApi.pUserCfg->nSimSpeed;
	m_UISetUserConfig.bAutoDayNight 		= g_fileSysApi.pUserCfg->bAutoDayNight;
	m_UISetUserConfig.nCarIconType 			= g_fileSysApi.pUserCfg->nCarIconType;
  	m_UISetUserConfig.nDayBgPic 			= g_fileSysApi.pUserCfg->nDayBgPic;
  	m_UISetUserConfig.nNightBgPic 			= g_fileSysApi.pUserCfg->nNightBgPic;
 	m_UISetUserConfig.nRouteMode 			= g_fileSysApi.pUserCfg->nRouteMode;
	m_UISetUserConfig.bLastNavigation 		= g_fileSysApi.pUserCfg->bLastNavigation;

	m_UISetUserConfig.nPastMarket 			= g_fileSysApi.pUserCfg->nPastMarket;
	m_UISetUserConfig.nVoiceControl 		= g_fileSysApi.pUserCfg->nVoiceControl;
	m_UISetUserConfig.nLastScaleIndex 		= g_fileSysApi.pUserCfg->nLastScaleIndex;
	m_UISetUserConfig.nSpNumInput			= g_fileSysApi.pUserCfg->nSpNumInput;
	m_UISetUserConfig.nKeyWordInput    		= g_fileSysApi.pUserCfg->nKeyWordInput;
	m_UISetUserConfig.nSpeedWarningPic    		= g_fileSysApi.pUserCfg->nSpeedWarningPic;
	m_UISetUserConfig.nUILanguage 			= g_fileSysApi.pUserCfg->nUILanguage;
	m_UISetUserConfig.nSpeedWarningVoice 		= g_fileSysApi.pUserCfg->nSpeedWarningVoice;
	m_UISetUserConfig.nRoadInput 			= g_fileSysApi.pUserCfg->nRoadInput;
	m_UISetUserConfig.nPoiInput 			= g_fileSysApi.pUserCfg->nPoiInput;
 	m_UISetUserConfig.nScaleIdx			= g_fileSysApi.pUserCfg->nScaleIdx;
 	m_UISetUserConfig.nSoundVolume 			= g_fileSysApi.pUserCfg->nSoundVolume;
	m_UISetUserConfig.nTMCSwitchFlag		= g_fileSysApi.pUserCfg->nTMCSwitchFlag;
	m_UISetUserConfig.nTMCFrequency			= g_fileSysApi.pUserCfg->nTMCFrequency;
	m_UISetUserConfig.nTMCVoice			= g_fileSysApi.pUserCfg->nTMCVoice;
	m_UISetUserConfig.nTimeToNight 			= g_fileSysApi.pUserCfg->nTimeToNight;
	m_UISetUserConfig.nTimeToDay 			= g_fileSysApi.pUserCfg->nTimeToDay;
	m_UISetUserConfig.bCCDVoice 			= g_fileSysApi.pUserCfg->bCCDVoice;
	m_UISetUserConfig.bBigMapRoadFont 		= g_fileSysApi.pUserCfg->bBigMapRoadFont;
	m_UISetUserConfig.nRouteOption 			= g_fileSysApi.pUserCfg->nRouteOption;	
	m_UISetUserConfig.nWaveTTSType			= g_fileSysApi.pUserCfg->nWaveTTSType;
	m_UISetUserConfig.nShowTip			= g_fileSysApi.pUserCfg->nShowTip;
	m_UISetUserConfig.nShowPOI			= g_fileSysApi.pUserCfg->nShowPOI;
	nuMemcpy(pUIData, &m_UISetUserConfig, sizeof(NURO_USER_SET_DATA));
}
nuVOID CNaviThread::TransferCoordinate(nuPVOID pUIData, nuPVOID pUIReturnData, nuBOOL bMapToBmp)
{
	if(NULL == pUIData || NULL == pUIReturnData)
	{
		return;
	}
	NURO_POINT ptMap, ptBmp;	
	if(bMapToBmp)
	{
		nuMemcpy(&ptMap, pUIData, sizeof(NURO_POINT));
		g_mathtoolApi.MT_MapToBmp(ptMap.nX, ptMap.nY, &ptBmp.nX, &ptBmp.nY);		
		nuMemcpy(pUIReturnData, &ptBmp, sizeof(NURO_POINT));
	}
	else
	{
		nuMemcpy(&ptBmp, pUIData, sizeof(NURO_POINT));
		g_mathtoolApi.MT_BmpToMap(ptBmp.nX, ptBmp.nY, &ptMap.nX, &ptMap.nY);		
		nuMemcpy(pUIReturnData, &ptMap, sizeof(NURO_POINT));
	}
}
nuVOID CNaviThread::NURO_DragZoomDraw(NURO_POINT2 Pt)
{
	nuDOUBLE fZoomDis = 0, dx = 0, dy = 0;
	if(m_bMouseDown && Pt.nMouseType == NURO_DEF_ANDROID_ACTION_POINTER_DOWN)
	{
		m_zoomDraw.m_nNewScaleValue	= g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
		m_zoomDraw.m_lZoomScale = g_fileSysApi.pGdata->uiSetData.nScaleValue;
		dx = Pt.nX1 - Pt.nX2;
		dy = Pt.nY1 - Pt.nY2;
		m_fZoomDis = nuSqrt(dx*dx + dy*dy);
		m_bMousZoomDraw = nuFALSE;
		__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "MOUSE_DOWN x1 %d, y1 %d, x2 %d, y2 %d, m_fZoomDis %lf, fZoomDis %lf, m_bDragZoom %d\n", Pt.nX1, Pt.nY1, Pt.nX2, Pt.nY2, m_fZoomDis, fZoomDis, m_bDragZoom);
	}
	else if(m_bMouseDown && Pt.nMouseType == NURO_DEF_ANDROID_ACTION_MOVE)
	{
		m_bDragZoom     = nuTRUE;
		dx = Pt.nX1 - Pt.nX2;
		dy = Pt.nY1 - Pt.nY2;
		fZoomDis = nuSqrt(dx*dx + dy*dy);
		if(m_fZoomDis == 0)
		{
			return ;
		}
		if(NURO_ABS(fZoomDis - m_fZoomDis) >= 5)
		{
			m_fZoomDis2  = fZoomDis / m_fZoomDis;
			m_fZoomDis   = fZoomDis;
			if(m_fZoomDis2 > 1000000 || m_fZoomDis2 < 0)
			{
				return ;
			}
			m_bMousZoomDraw = nuTRUE;
		}		
		else
		{
			m_bMousZoomDraw = nuFALSE;
		}
		__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "MOUSE_MOVE x1 %d, y1 %d, x2 %d, y2 %d, m_fZoomDis2 %lf, m_bMousZoomDraw %d\n", Pt.nX1, Pt.nY1, Pt.nX2, Pt.nY2, m_fZoomDis2, m_bMousZoomDraw);
		nuSleep(10);
	}
	else if(m_bMouseDown && Pt.nMouseType == NURO_DEF_ANDROID_ACTION_POINTER_UP)
	{		
		m_bDragZoom   = nuFALSE;
		m_bMouseDown  = nuFALSE;
		__android_log_print(ANDROID_LOG_INFO, "DrawZoom", "MOUSE_UP x1 %d, y1 %d, x2 %d, y2 %d, m_fZoomDis %lf, fZoomDis %lf, m_bDragZoom %d\n", Pt.nX1, Pt.nY1, Pt.nX2, Pt.nY2, m_fZoomDis, fZoomDis, m_bDragZoom);
	}		
}
nuVOID CNaviThread::DragZoomDraw(PACTIONSTATE pActionState)
{
	while(m_bDragZoom)
	{
		if(m_bMousZoomDraw)
		{
			/*m_gDataMgr.SetUp(pActionState);
			m_gDataMgr.CalMapShowInfo(pActionState);
			g_drawmapApi.DM_LoadMap(pActionState);
			g_memMgrApi.MM_CollectDataMem(0);*/
			m_zoomDraw.ZoomProcessEx(pActionState, m_fZoomDis2);
		}
		nuSleep(10);
	}
	pActionState->bDrawBmp1	= 1;
	pActionState->bDrawInfo	= 1;
	pActionState->bNeedReloadMap	= 1;
	pActionState->bDrawRoadName     = 1;
}

nuPVOID CNaviThread::NURO_GetNavi_List_Data(nuPVOID pRTlist, nuPVOID pNum, nuPVOID pIndex)
{
	CODENODE codeNode	= {0};
	codeNode.pDataBuf[1] 	= pNum;
	codeNode.pDataBuf[2] 	= pIndex;
	codeNode.nDrawType	= DRAWTYPE_UI;
	codeNode.nActionType	= AC_DRAW_GETROUTELIST_D;
	m_drawToUI.bUIWaiting	= 1;
	m_codePre.PushNode(&codeNode, PUSH_ADD_ATHEAD);
	while( !m_bQuit && m_drawToUI.bUIWaiting )
	{
		nuSleep(25);
	}
	nuINT *pListCount = (nuINT*)pNum;
	PNURO_NAVI_LIST pNaviList = (PNURO_NAVI_LIST)m_pUIGetData;
	for(nuINT i = 0; i < *pListCount; i++)
	{
		pNaviList[i].nIconType = m_HudComCtrl.GetHUDIconType(pNaviList[i].nIconType);
	}
	return m_pUIGetData;
}
nuPVOID CNaviThread::GetNaviListData(PROUTINGLISTNODE pRTlist, nuINT *pNum, nuINT *pIndex)
{
	return g_innaviApi.IN_GetRoutingListEx(pRTlist, pNum, pIndex);
}

nuVOID CNaviThread::NuroUIGetPicBuffer(PNURO_PACK_PIC_PARAMETER  pPackPicPara, nuBYTE *pPicBuffer)
{
	nuINT nShowPicW = pPackPicPara->nRight - pPackPicPara->nLeft;
	nuINT nSHowPicH = pPackPicPara->nBottom - pPackPicPara->nTop;
	if(pPackPicPara->bInitSet)
	{
		nuMemcpy(&m_PackPicPara, pPackPicPara, sizeof(NURO_PACK_PIC_PARAMETER));
		return ;
	}
	if(!pPackPicPara->bShowPic)
	{
		FreeArchivePackPic(m_pPackID, m_RealPicBmp);	
		m_pPackID = NULL;
		return ;	
	}
	/*if( NULL == m_RealPicBmp.pBmpBuff || m_pPackID == NULL)//pPackPicPara->nPicID != m_PackPicPara.nPicID ||
	{
		FreeArchivePackPic(m_pPackID, m_RealPicBmp);
		m_pPackID = NULL;
		if(!LoadArchivePack(pPackPicPara->nPackPathID, pPackPicPara->bLoadMemory))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NuroUIGetPicBuffer LoadArchivePack Load Fail");
			return ;
		}
		if(!LoadPic(pPackPicPara->nPicID, m_RealPicBmp))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "LoadPic Fail");
			return ;
		}
	}
	if(!pPackPicPara->bEngineShow)
	{
		nuMemcpy(pPicBuffer, m_RealPicBmp.pBmpBuff, sizeof(nuBYTE) * 2 * nSHowPicH * nShowPicW);
	}*/
}
nuBOOL CNaviThread::ChangeRealPackPic(nuINT nPicID)
{
	if( m_RealPicBmp.pBmpBuff != NULL)
	{
		g_OpenResourceApi.RS_UnLoadBmp(m_RealPicBmp);
		m_RealPicBmp.pBmpBuff = NULL;
	}
	return LoadPic(nPicID, m_RealPicBmp);
}
nuVOID CNaviThread::FreeArchivePackPic(nuPVOID pUIPackID, NURO_BMP &Bmp)
{
	if(pUIPackID != NULL)
	{
		g_OpenResourceApi.RS_UnloadPack(pUIPackID);
	}
	if( Bmp.pBmpBuff != NULL)
	{
		g_OpenResourceApi.RS_UnLoadBmp(Bmp);
		Bmp.pBmpBuff = NULL;
	}
}
nuBOOL CNaviThread::LoadArchivePack(nuINT nPackPathID, nuBOOL bLoadMemory)
{
	nuINT nLoadPackIDIndex = -1, nPackID = 0;
	nuINT i = 0;
	nuTCHAR tsPack[NURO_MAX_PATH] = {0};
	nuUINT nTotalLen = 0;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsPack, nuTEXT("_3dPic.pck"));
	if(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx == m_nPicDwIdx && m_pPackID != NULL)
	{
		return nuTRUE;
	}
	else if(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx != m_nPicDwIdx && m_pPackID != NULL)
	{
		g_OpenResourceApi.RS_UnloadPack(m_pPackID);
		m_pPackID = NULL;		
	}
	m_nPicDwIdx = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx;
	if( !g_fileSysApi.FS_FindFileWholePath(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nDwIdx, tsPack, NURO_MAX_PATH) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "Load Archive Pack FindFileWholePath Fail");
		return nuFALSE;
	}
	m_pPackID = g_OpenResourceApi.RS_LoadPack(tsPack, bLoadMemory, nTotalLen); 
	if( m_pPackID == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "Load Archive Pack Fail");
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CNaviThread::LoadPic(nuINT nPicID, NURO_BMP &Bmp)
{

	nuCHAR tsBmp[256] = {0};
	nuVOID* pfile = NULL;
	tsBmp[0] = '/';
	nuItoa( nPicID, &tsBmp[1], 10);
	nuStrcat(tsBmp, ".bmp");	
	if(m_pPackID == NULL){
		if(!LoadArchivePack(m_PackPicPara.nPackPathID, m_PackPicPara.bLoadMemory)){
			__android_log_print(ANDROID_LOG_INFO, "navi", "LoadPic--Load Archive Pack Fail");
			return nuFALSE;
		}
	}
	pfile = g_OpenResourceApi.RS_FindFile(m_pPackID, tsBmp);
	if( pfile == NULL ){
		__android_log_print(ANDROID_LOG_INFO, "navi", "Find Pack Pic Fail");
		return nuFALSE;
	}
	return g_OpenResourceApi.RS_LoadBmp(m_pPackID, pfile, Bmp);
}
//Added by Daniel ================DragMove============= 20160420//
nuBYTE *g_pDesBmpBuff = NULL, *g_pSrcBmpBuff = NULL;
nuWORD nWidth	    = 800,   nHeight	 = 480;
nuBOOL CNaviThread::BmpCopyEx(nuBYTE *pDesBmpBuff, nuINT desx, nuINT desy, nuINT desw, nuINT desh, nuBYTE *pSrcBmpBuff, nuINT srcx, nuINT srcy, nuINT srcw, nuINT srch)
{
	if( desx >= nWidth || desy >= nHeight || pSrcBmpBuff == NULL)
	{
		return nuFALSE;
	}
	nuWORD bytesPerLine = nWidth<<1;
	nuLONG xrIntFloat_16 = (srcw<<16)/desw+1;
	nuLONG yrIntFloat_16 = (srch<<16)/desh+1;
	nuWORD* pDstLine = 0;
	nuWORD* pSrcLine = 0;    
	nuLONG y_16 = 0;
	nuWORD transColor = (nuWORD)1440;
	nuLONG bytesWidthDes = bytesPerLine>>1;
	nuLONG bytesWidthSrc = bytesPerLine>>1;
	nuLONG x_16 = 0;
	pDstLine = (nuWORD*)(pDesBmpBuff + bytesPerLine*desy);
	nuINT ytag = 0;
	pSrcLine = (nuWORD*)(pSrcBmpBuff + bytesPerLine*(srcy+(y_16>>16)));
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
NUROPOINT g_ptFirstMove = {-1, -1}, g_ptDown = {0, 0}, g_ptUp = {0, 0}, g_ptMove = {0,0};
nuBYTE *g_pCvsBmpBuff = NULL, *g_pBgShowBmpBuff = NULL;
nuWORD g_nCvsWidth = 800, g_nCvsHeight = 480, g_nBgShowWidth = 800, g_nBgShowHeight = 480;
nuBOOL g_bChangeSize = nuFALSE, g_bMouseDown = nuFALSE, g_bCreateBmp = nuFALSE;
NURO_CANVAS *g_pCvs = NULL;
nuVOID  CNaviThread::Nuro_MouseEventEx(NUROPOINT ptMouse, nuBYTE nMouseType, nuINT nVelocityX, nuINT nVelocityY)
{
	NUROPOINT ptPoint ={0,0};
	if(nMouseType == NURO_DEF_ANDROID_ACTION_DOWN)
	{
		NuroSetNaviThread(NAVI_THD_STOP_ALLL);
		m_bMoveMapType1 = nuTRUE;
		g_ptDown = ptMouse;
		g_bMouseDown = nuTRUE;
		m_bJumpMove = nuFALSE;
		__android_log_print(ANDROID_LOG_INFO, "Mouse", "MouseDown x %d, y %d\n", ptMouse.x, ptMouse.y);
		return ;
	}
	if(nMouseType == NURO_DEF_ANDROID_ACTION_UP && m_nThdState == NAVI_THD_STOP_ALLL)
	{
		g_bCreateBmp = nuFALSE;
		m_pAcState->bDrawBmp1		= 1;
		m_pAcState->bDrawInfo		= 1;
		m_pAcState->bNeedReloadMap	= 1;
		m_pAcState->bDrawRoadName       = 1;
		m_pAcState->bResetMapCenter	= 1;
		m_pAcState->nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
		m_pAcState->bResetIconCarPos    = 0;
		m_pAcState->bResetRealCarPos    = 0;
		g_ptUp = ptMouse;
		g_bMouseDown = nuFALSE;					
		if(!m_bJumpMove)
		{
			m_bJumpMove = nuFALSE;
			if( !m_gDataMgr.PointScreenToBmp(g_ptUp.x, g_ptUp.y) )
			{
				return ;
			}
			g_mathtoolApi.MT_BmpToMap(g_ptUp.x, g_ptUp.y, &ptPoint.x, &ptPoint.y);
			if( !g_fileSysApi.FS_PtInBoundary(ptPoint) )
			{
				return ;
			}
			m_pAcState->ptNewMapCenter = ptPoint;			
			__android_log_print(ANDROID_LOG_INFO, "Mouse", "m_bJumpMove");
			
		}
		else
		{
			ptPoint.x = g_ptDown.x - g_ptUp.x + g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterX;
			ptPoint.y = g_ptDown.y - g_ptUp.y + g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterY;
			NuroJumpToScreenXY(ptPoint.x, ptPoint.y);
			g_mathtoolApi.MT_BmpToMap(ptPoint.x, 
						  ptPoint.y, 
				&m_pAcState->ptNewMapCenter.x, 
				&m_pAcState->ptNewMapCenter.y);
			if(g_pBgShowBmpBuff != NULL)
			{
				delete [] g_pBgShowBmpBuff;
				g_pBgShowBmpBuff = NULL;
			}
			if(g_pCvsBmpBuff != NULL)
			{
				delete [] g_pCvsBmpBuff;
				g_pCvsBmpBuff = NULL;
			}
		}
		g_fileSysApi.pGdata->timerData.bIsMovingMap		= 0; 
		g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 0;
		m_jumpMove.JumpToPoint(m_pAcState, g_ptUp.x, g_ptUp.y);
		SetMoveWait();
		NuroSetNaviThread(NAVI_THD_WORK);
		__android_log_print(ANDROID_LOG_INFO, "Mouse", "MouseUp x %d, y %d\n", ptMouse.x, ptMouse.y);	
		return ;
	}
	if(nMouseType == NURO_DEF_ANDROID_ACTION_MOVE && m_nThdState == NAVI_THD_STOP_ALLL)
	{
		if(g_bMouseDown && (ptMouse.x != g_ptDown.x || ptMouse.y != g_ptDown.y))
		{
			m_bJumpMove = nuTRUE;
			if(!g_bCreateBmp)
			{
				g_bCreateBmp = nuTRUE;
				g_pCvs = g_pGdiApiTrans->nuGdiGetCanvas();
			}

			if(g_pCvsBmpBuff == NULL)
			{						
				g_pBgShowBmpBuff = new nuBYTE[g_nBgShowWidth*g_nBgShowHeight*2];
				g_pCvsBmpBuff = new nuBYTE[g_nCvsWidth*g_nCvsHeight*2];
				nuMemcpy(g_pCvsBmpBuff, g_pCvs->bitmap.pBmpBuff, g_pCvs->bitmap.bmpBuffLen);
				g_ptFirstMove = g_ptDown;
			}
			nuINT desx = 0, desy = 0, desw = 0, desh = 0;
			nuINT srcx = 0, srcy = 0, srcw = 0, srch = 0;

			desw = g_nBgShowWidth;
			desh = g_nBgShowHeight;
			srcw = g_fileSysApi.pDrawInfo->drawInfoCommon.nuBmpBg.bmpWidth;
			srch = g_fileSysApi.pDrawInfo->drawInfoCommon.nuBmpBg.bmpHeight;

			BmpCopyEx(g_pBgShowBmpBuff, desx, desy, desw, desh, 
				g_fileSysApi.pDrawInfo->drawInfoCommon.nuBmpBg.pBmpBuff, srcx, srcy, srcw, srch);
			
			nuLONG dx = g_ptFirstMove.x - ptMouse.x;
			nuLONG dy = g_ptFirstMove.y - ptMouse.y;
			
			srcx = dx;
			srcy = dy;
			srcw = g_nCvsWidth;
			srch = g_nCvsHeight;

			desx = 0; 
			desy = 0;
			desw = g_nBgShowWidth;
			desh = g_nBgShowHeight;
			if (g_nCvsWidth < srcx + g_nBgShowWidth)
			{
				desw = g_nCvsWidth - srcx;
			}
			if (g_nCvsHeight < srcy + g_nBgShowHeight)
			{
				desh = g_nCvsHeight - srcy;
			}
			if (0 > srcx)
			{
				desx = NURO_ABS(srcx);
				desw -= desx; 
				srcx = 0;
			}
			if (0 > srcy)
			{
				desy = NURO_ABS(srcy);
				desh -= desy;
				srcy = 0;
			}
			BmpCopyEx(g_pBgShowBmpBuff, desx, desy, desw, desh, g_pCvsBmpBuff, srcx, srcy, srcw, srch);
			nuMemcpy(g_pCvs->bitmap.pBmpBuff, g_pBgShowBmpBuff, g_pCvs->bitmap.bmpBuffLen);
			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_DEFAULT);
			__android_log_print(ANDROID_LOG_INFO, "Mouse", "MouseMove x %d, y %d\n", ptMouse.x, ptMouse.y);
		}
	}
}
nuVOID  CNaviThread::Nuro_MouseEventEx2(NUROPOINT ptMouse, nuBYTE nMouseType, nuINT nVelocityX, nuINT nVelocityY)
{
	NUROPOINT  ptMoveDis;
	NUROPOINT  ptPoint = {0,0};
	nuINT	   nIndex  = 0, MoveTimes = 0;
	if(nMouseType == NURO_DEF_ANDROID_ACTION_DOWN)
	{
		NuroSetNaviThread(NAVI_THD_STOP_ALLL);
		g_ptDown = ptMouse;
		g_ptMove = ptMouse;
		g_bMouseDown = nuTRUE;
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "MouseDown x %d, y %d\n", ptMouse.x, ptMouse.y);
		return ;
	}
	if(nMouseType == NURO_DEF_ANDROID_ACTION_UP)
	{
		m_pAcState->bResetMapCenter = 1;
		m_pAcState->bDrawRoadName = 1;
		m_pAcState->bDrawPOIName = 1;	
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "MouseUp x %d, y %d, nVelocityX %d, nVelocityY %d\n", ptMouse.x, ptMouse.y, nVelocityX, nVelocityY);
		if((ptMouse.x - g_ptDown.x == 0) && (ptMouse.y - g_ptDown.y == 0))
		{
			m_pAcState->ptNewMapCenter.x =  g_ptDown.x - g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterX;
			m_pAcState->ptNewMapCenter.y =  g_ptDown.y - g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterY;
			m_jumpMove.JumpToPointEx(m_pAcState, m_pAcState->ptNewMapCenter.x, m_pAcState->ptNewMapCenter.y);
			__android_log_print(ANDROID_LOG_INFO, "Mouse2", "(ptMouse.x - g_ptDown.x == 0) && (ptMouse.y - g_ptDown.y == 0)\n");
		}
		/*else if(nVelocityX != 0 || nVelocityY != 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "Mouse2", "nVelocityX != 0 || nVelocityY != 0\n");
			m_pAcState->ptNewMapCenter.x =  ptMouse.x - g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterX + nVelocityX;
			m_pAcState->ptNewMapCenter.y =  ptMouse.y - g_fileSysApi.pDrawInfo->pcTransf->nBmpCenterY + nVelocityY;
			ptMoveDis.x = (ptMouse.x + nVelocityX) / m_jumpMove.m_nSingleMoveStep; 
			ptMoveDis.y = (ptMouse.y + nVelocityY) / m_jumpMove.m_nSingleMoveStep;
			__android_log_print(ANDROID_LOG_INFO, "Mouse2", "m_nSingleMoveStep %d\n", m_jumpMove.m_nSingleMoveStep);
			for(nIndex = 0; nIndex < m_jumpMove.m_nSingleMoveStep; nIndex++)
			{
				m_jumpMove.JumpToPointEx(m_pAcState, ptMoveDis.x, ptMoveDis.y);
				__android_log_print(ANDROID_LOG_INFO, "Mouse2", "MoveDis.x %d , MoveDis.y %d\n", ptMoveDis.x, ptMoveDis.y);
				nuSleep(10);
			}
		}*/
		g_fileSysApi.pGdata->timerData.bIsMovingMap		= 0; 
		g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 0;
		SetMoveWait();
		NuroSetNaviThread(NAVI_THD_WORK);
		g_bMouseDown = nuFALSE;		
		return ;
	}
	if(nMouseType == NURO_DEF_ANDROID_ACTION_MOVE && m_nThdState == NAVI_THD_STOP_ALLL)
	{
		if(g_bMouseDown)
		{
			ptMoveDis.x = g_ptMove.x - ptMouse.x;
			ptMoveDis.y = g_ptMove.y - ptMouse.y;
			if(ptMoveDis.x == 0 && ptMoveDis.y == 0)
			{
				return ;
			}
			g_ptMove = ptMouse;
			m_jumpMove.JumpToPointEx(m_pAcState, ptMoveDis.x, ptMoveDis.y);
			g_mathtoolApi.MT_SaveOrShowBmp(SHOW_DEFAULT);
		}
		__android_log_print(ANDROID_LOG_INFO, "Mouse2", "MouseMove x %d, y %d\n", ptMouse.x, ptMouse.y);
	}
}
nuPVOID CNaviThread::NURO_Get_TMC_NAVI_List(nuPVOID pNum, nuPVOID pTMCList)
{
	CODENODE codeNode	= {0};
	codeNode.pDataBuf[1] 	= pNum;
	codeNode.nDrawType	= DRAWTYPE_UI;
	codeNode.nActionType	= AC_DRAW_GET_TMC_NAVI_LIST_D;
	m_TMCForoUI.bUIWaiting	= 1;
	m_codeNavi.PushNode(&codeNode, PUSH_ADD_ATHEAD);	
	while( !m_bQuit && m_TMCForoUI.bUIWaiting )
	{
		nuSleep(25);
	}
	nuUINT *pListCount = (nuUINT*)pNum;
	*pListCount = g_fileSysApi.pGdata->drawInfoNavi.nTMCCount;
	__android_log_print(ANDROID_LOG_INFO, "TMC", "NURO_Get_TMC_NAVI_List %d, %d\n", *pListCount, g_fileSysApi.pGdata->drawInfoNavi.nTMCCount);
	return (nuPVOID)g_fileSysApi.pGdata->drawInfoNavi.pTMCList;
}

