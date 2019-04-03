// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TimerThread.cpp,v 1.17 2010/09/07 03:18:34 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/09/07 03:18:34 $
// $Locker:  $
// $Revision: 1.17 $
// $Source: /home/nuCode/libNaviCtrl/TimerThread.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "TimerThread.h"
#include "NuroModuleApiStruct.h"
#include "NuroOpenedDefine.h"
#include "NuroClasses.h"
#include "NaviThread.h"

#define TIME_THREAD_PRIO_ID     53

extern	FILESYSAPI					g_fileSysApi;
extern DRAWINFOMAPAPI				g_drawIfmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
nuLONG g_lGetGPSBufferGap = 100;
nuDWORD CTimerThread::TiThreadAction(nuPVOID lpVoid)
{
	CTimerThread *pThis = (CTimerThread*)lpVoid;
	nuDWORD nOldTicks = nuGetTickCount();
	nuDWORD	nNowTicks;
	nuDWORD nTmp;
	NUROTIME nuTime;
	pThis->m_pFTimer	= NULL;
	pThis->m_bOutThreadLoop	= nuFALSE;
//	nuMemset(&g_fileSysApi.pGdata->timerData, 0, sizeof(g_fileSysApi.pGdata->timerData));
	nuGetLocalTime(&nuTime);
	g_fileSysApi.pGdata->timerData.nAutoMoveType  = AUTO_MOVE_GPS;
	g_fileSysApi.pGdata->timerData.nMiniSecond		= 0;
	g_fileSysApi.pGdata->timerData.nSecond			= (nuBYTE)nuTime.nSecond;
	g_fileSysApi.pGdata->timerData.nMinute			= (nuBYTE)nuTime.nMinute;
	g_fileSysApi.pGdata->timerData.nHour			= nuTime.nHour;
	g_fileSysApi.pGdata->timerData.nDay				= (nuBYTE)nuTime.nDay;
	g_fileSysApi.pGdata->timerData.nMonth			= (nuBYTE)nuTime.nMonth;
	g_fileSysApi.pGdata->timerData.nYear			= nuTime.nYear;
	g_fileSysApi.pGdata->timerData.bCanUpdateTime	= 0;
	g_fileSysApi.pGdata->timerData.nUpDateTimeTimer		= MAX_UPDATE_SYSTIME/2;
	g_fileSysApi.pGdata->timerData.nDayNightChangeTimer	= MAX_DAY_NIGHT_CHANGE;
	g_fileSysApi.pGdata->timerData.nSysStartTick		= nuGetTickCount();
	g_fileSysApi.pGdata->timerData.bIsRouting			= 0;
	if( !nuReadConfigValue("GPSBUFFERGAP", &g_lGetGPSBufferGap) )
	{
		g_lGetGPSBufferGap = 100;
	}
	while( !pThis->m_bQuit )
	{
		nuSleep(TIMER_TIME_UNIT);
		nNowTicks = nuGetTickCount();
		if(g_fileSysApi.pGdata->bWaitQuit)
		{
			nuSleep(500);
			continue;
		}
		//Set timer
		if( g_fileSysApi.pGdata->timerData.bCanUpdateTime  && 
			g_fileSysApi.pGdata->timerData.nUpDateTimeTimer > MAX_UPDATE_SYSTIME )
		{
			g_fileSysApi.pGdata->timerData.nSecond		= (nuBYTE)(pThis->m_pNaviThd->m_gpsData.nTime.nSecond);
			g_fileSysApi.pGdata->timerData.nMinute		= (nuBYTE)(pThis->m_pNaviThd->m_gpsData.nTime.nMinute);
			g_fileSysApi.pGdata->timerData.nHour		= pThis->m_pNaviThd->m_gpsData.nTime.nHour;
			g_fileSysApi.pGdata->timerData.nDay			= (nuBYTE)(pThis->m_pNaviThd->m_gpsData.nTime.nDay);
			g_fileSysApi.pGdata->timerData.nMonth		= (nuBYTE)(pThis->m_pNaviThd->m_gpsData.nTime.nMonth);
			g_fileSysApi.pGdata->timerData.nYear		= pThis->m_pNaviThd->m_gpsData.nTime.nYear;

			#if !defined(NURO_OS_WINDOWS) || defined(_WIN32_WCE)
				NUROTIME nuTIME;
				nuMemset(&nuTIME, NULL, sizeof(NUROTIME));
				if (g_fileSysApi.pGdata->timerData.nYear < 1000)
				{
					nuTIME.nYear   = g_fileSysApi.pGdata->timerData.nYear + 2000;
				}
				else
				{
					nuTIME.nYear   = g_fileSysApi.pGdata->timerData.nYear;
				}
				
				nuTIME.nMonth  = g_fileSysApi.pGdata->timerData.nMonth;
				nuTIME.nDay    = g_fileSysApi.pGdata->timerData.nDay;
				nuTIME.nHour   = g_fileSysApi.pGdata->timerData.nHour;
				nuTIME.nMinute = g_fileSysApi.pGdata->timerData.nMinute;
				nuTIME.nSecond = g_fileSysApi.pGdata->timerData.nSecond;
				//nuSetLocalTime(&nuTIME);
			#endif


			g_fileSysApi.pGdata->timerData.nUpDateTimeTimer = 0;
		}
		else
		{
			if( nNowTicks < nOldTicks )
			{
				nOldTicks = nNowTicks;
				continue;
			}
			nTmp = (nNowTicks - nOldTicks);
			g_fileSysApi.pGdata->timerData.nUpDateTimeTimer += nTmp;
			pThis->TimerAdd(nTmp);
			/*
			//--------------calculate the time-----------------
			nTmp += g_fileSysApi.pGdata->timerData.nMiniSecond;
			g_fileSysApi.pGdata->timerData.nMiniSecond	= nTmp%1000;
			nTmp /= 1000;
			//Second
			nTmp += g_fileSysApi.pGdata->timerData.nSecond;
			g_fileSysApi.pGdata->timerData.nSecond = (nuBYTE)(nTmp%60);
			nTmp /= 60;
			//Minutor
			nTmp += g_fileSysApi.pGdata->timerData.nMinute;
			g_fileSysApi.pGdata->timerData.nMinute = (nuBYTE)(nTmp%60);
			nTmp /= 60;
			//Hours
			g_fileSysApi.pGdata->timerData.nHour += nTmp;
			*/

		}
//		g_fileSysApi.pGdata->timerData.bCanUpdateTime = 0;
		//-------calculate the GetGps data -------------------------------------
		nTmp = (nNowTicks - nOldTicks);
		g_fileSysApi.pGdata->timerData.nDayNightChangeTimer += nTmp;
		g_fileSysApi.pGdata->timerData.nGetGpsTime	+= (nuWORD)nTmp;
#ifndef    NURO_OS_HV_UCOS	    
		if( g_fileSysApi.pGdata->timerData.nGetGpsTime	>= g_lGetGPSBufferGap )//MAX_GET_GPS_TIME_MS / 50
#else
		if( g_fileSysApi.pGdata->timerData.nGetGpsTime	>= MAX_GET_GPS_TIME_MS  )
#endif 
		{
			pThis->m_pNaviThd->TimeToGetGPS();
			g_fileSysApi.pGdata->timerData.nGetGpsTime = 0;
		}

		//---------Draw Route Animation------------------------------------
		if( g_fileSysApi.pGdata->timerData.bIsRouting )
		{
			g_fileSysApi.pGdata->timerData.nRouteTimer += nTmp;
			if( g_fileSysApi.pGdata->timerData.nRouteTimer > ROUTE_ANIMATION_SLICE )
			{
				g_fileSysApi.pGdata->timerData.nRouteTimer = 0;
				nuLONG nType = 0;
				if( g_fileSysApi.pGdata->timerData.nRouteType  == 1)
				{
					nType = 1;
				}
				pThis->DrawRouteAnimation(nType);
				pThis->m_bRouteStart = nuFALSE;
			}
			g_fileSysApi.pGdata->timerData.bUpdateMap		= 1;
			g_fileSysApi.pGdata->timerData.nUpdateMapTime	= 0;
		}
		else
		{
			pThis->m_bRouteStart = nuTRUE;
			//------calculate the update map time-----------------------------------
			g_fileSysApi.pGdata->timerData.nUpdateMapTime += (nuWORD)nTmp;
			if( g_fileSysApi.pGdata->timerData.nUpdateMapTime >= MAX_UPDATE_MAP_TIME_MS )
			{
				g_fileSysApi.pGdata->timerData.bUpdateMap		= 1;
				g_fileSysApi.pGdata->timerData.nUpdateMapTime	= 0;
			}
		}
		//-------shift to show or hide the voice Icon
		if( g_fileSysApi.pUserCfg->nVoiceIconTimer && g_fileSysApi.pGdata->timerData.bShowVoiceIcon )
		{
			g_fileSysApi.pGdata->timerData.nShowVoiceTimer += 1;//(nuWORD)nTmp;
		}
		//-------shift to show or hide scale value
		if( g_fileSysApi.pUserCfg->nScaleShowMode )
		{
			if( g_fileSysApi.pGdata->timerData.bShowScale )
			{
				g_fileSysApi.pGdata->timerData.nShowScaleTimer += (nuWORD)nTmp;
				if( g_fileSysApi.pGdata->timerData.nShowScaleTimer/1000 > g_fileSysApi.pUserCfg->nScaleShowMode )
				{
					g_fileSysApi.pGdata->timerData.bShowScale = 0;
				}
			}
		}
		else
		{
			g_fileSysApi.pGdata->timerData.bShowScale	= 1;
		}
		//-------Timer Process---------------------------------
//		pThis->m_bTimerFucIsWorking = nuTRUE; 
		/*
		if( pThis->m_pFTimer != NULL )
		{
			pThis->m_nTimerAdd += nTmp;
			if( pThis->m_nTimerAdd > pThis->m_nTimer )
			{
				pThis->m_pFTimer();
				pThis->m_nTimerAdd = 0;
			}
		}
		*/
		pThis->TimerCallProc(nNowTicks);
//		pThis->m_bTimerFucIsWorking = nuFALSE;
		//-----------------------------------------------------------------------
		//Col
		switch( g_fileSysApi.pGdata->timerData.nAutoMoveType )
		{
			case AUTO_MOVE_GPS:
				g_fileSysApi.pGdata->timerData.nGpsTime += (nuWORD)nTmp;
				if( g_fileSysApi.pGdata->timerData.nGpsTime >= MAX_GPS_SPACE_OF_TIME )
				{
					g_fileSysApi.pGdata->timerData.bGpsMode	= 1;
					g_fileSysApi.pGdata->timerData.nGpsTime	= 0;
				}
				break;
			case AUTO_MOVE_SIMULATION:
				g_fileSysApi.pGdata->timerData.nSimTime += (nuWORD)nTmp;
				if( g_fileSysApi.pGdata->timerData.nSimTime >= MAX_SIM_SPACE_OF_TIME )
				{
					g_fileSysApi.pGdata->timerData.bSimMode	= 1;
					g_fileSysApi.pGdata->timerData.nSimTime	= 0;
				}
				break;
			case AUTO_MOVE_TIMER:
				g_fileSysApi.pGdata->timerData.nMoveWaitTimer += (nuWORD)nTmp;
				break;
			default:
				break;
		}
		if( g_fileSysApi.pGdata->timerData.bIsWaitingForMoving )
		{
			if( nNowTicks - g_fileSysApi.pGdata->timerData.nMoveStartTicks > (nuDWORD)pThis->m_pNaviThd->m_nGlideMoveMinMS )
			{
				g_fileSysApi.pGdata->timerData.bIsMovingMap		= 1;
				g_fileSysApi.pGdata->timerData.bIsWaitingForMoving	= 0;
				pThis->m_pNaviThd->NuroMoveStart();
			}
		}
		nOldTicks = nNowTicks;
//		DEBUGSTRING(nuTEXT("Timer Thread sleep"));
		nuSleep(TIMER_TIME_UNIT);
	}
	pThis->m_bQuit = nuTRUE;
	pThis->m_bOutThreadLoop = nuTRUE;
	//nuThreadWaitReturn( (NURO_THREAD_HANDLE*)pThis->m_hThread.handle );
//	pThis->m_hThread = NULL;



    return 0;
}

CTimerThread::CTimerThread()
{
	m_bQuit			= nuTRUE;
	#ifndef NURO_OS_ECOS
	m_dwThreadID	= 0;
	//m_hThread.nThdID = 0;
	#endif
	m_hThread		= NULL;
	//m_hThread.handle = NULL;
	m_bOutThreadLoop= nuTRUE;
	m_pFTimer		= NULL;
	m_bTimerFucIsWorking	= nuFALSE;

	m_pfDrawAni    = NULL;
}

CTimerThread::~CTimerThread()
{
	FreeTimerThread();
}

nuBOOL CTimerThread::InitTimerThread(class CNaviThread* pNaviThd)
{
	nuMemset(m_pTimerList, 0, sizeof(m_pTimerList));
	m_bQuit		= nuFALSE;
	m_pNaviThd = pNaviThd;
#ifdef NURO_OS_WINDOWS
//#define NURO_THREAD_CREATE_DEFAULT          0
//#define NURO_THREAD_PRIORITY_HIGHT          20
//#define NURO_THREAD_PRIORIYT_NORMAL         30
//#define NURO_THREAD_PRIORITY_LOW            40
	m_hThread = nuCreateThread(NULL, 0, TiThreadAction, this, 0, &m_dwThreadID);
	//nuSetThreadPriority(m_hThread, NURO_THREAD_PRIORITY_HIGHT);
#endif

#ifdef NURO_OS_LINUX
	m_hThread = nuCreateThread(NULL, 0, TiThreadAction, this, 0, &m_dwThreadID);
	
#endif

#if defined(NURO_OS_UCOS) || defined(NURO_OS_JZ)
	m_hThread = nuCreateThread(NULL, 0, (LPNURO_THREAD_START_ROUTINE)TiThreadAction, this, TASK_TIMER, (nuPDWORD)(&m_pStack[TIMER_STACK_SIZE-1]));
#endif 

#ifdef NURO_OS_ECOS
	NURO_SECURITY_ATTRIBUTES attr;
	attr.nLength = (nuDWORD)(&m_dwThreadID);
	attr.bInheritHandle = (nuBOOL)(&m_hThread);
	m_hThread = nuCreateThread ( &attr,
								 4 * TIMER_STACK_SIZE,
								 (LPNURO_THREAD_START_ROUTINE)TiThreadAction,
								 this,
								 TASK_TIMER,
								 (nuPDWORD)m_pStack );
#endif
#ifdef NURO_OS_HV_UCOS
	m_hThread = nuCreateThread( NULL, TIMER_STACK_SIZE, (LPNURO_THREAD_START_ROUTINE)TiThreadAction,this,TASK_TIMER,NULL );
#endif

	if( m_hThread == NULL )
	{
		return nuFALSE;
	}


	return nuTRUE;
}
nuVOID CTimerThread::FreeTimerThread()
{
	m_bQuit = nuTRUE;
	while( !m_bOutThreadLoop )
	{
		nuSleep(25);
	}

    if (m_hThread)
    {
        nuDelThread(m_hThread);
        m_hThread = NULL;
    }

	m_bQuit = nuFALSE;
}

nuBOOL CTimerThread::NuroSetTimer(nuPVOID lpVoid, nuDWORD nTimer)
{
	/*while ( m_bTimerFucIsWorking )
	{
		nuSleep(25);
	}*/
	m_nTimerOldTicks	= nuGetTickCount();
	m_nTimer	= nTimer;
	m_pFTimer	= (NuroTimerProc)lpVoid;
	return nuTRUE;
}

nuBOOL CTimerThread::NuroSetTimerEx(nuDWORD nID, nuDWORD nElapse, nuPVOID lpTimerProc)
{
	if(nID == 0 && lpTimerProc == NULL )
	{
		return nuFALSE;
	}
	nuINT nIdx = -1;
	for( nuINT i = 0; i < MAX_TIMER_NUM; ++i )
	{
		if( nID == m_pTimerList[i].nTimerID )
		{
			nIdx = i;
			break;
		}//Same ID 
		if( nIdx == -1 && m_pTimerList[i].nTimerID == 0 )
		{
			nIdx = i;
		}
	}
	if( nIdx == -1 )
	{
		return nuFALSE;
	}
	/*
	while ( m_bTimerFucIsWorking )
	{
		nuSleep(25);
	}
	*/
	m_pTimerList[nIdx].nTimerID		= nID;
	m_pTimerList[nIdx].nElapse		= nElapse;
	m_pTimerList[nIdx].nTickLast	= nuGetTickCount();
	m_pTimerList[nIdx].pFTimer		= (NuroTimerProc)lpTimerProc;
	return nuTRUE;
}

nuBOOL CTimerThread::NuroKillTimerEx(nuDWORD nID)
{
	/*
	while ( m_bTimerFucIsWorking )
	{
		nuSleep(25);
	}
	*/
	for( nuINT i = 0; i < MAX_TIMER_NUM; ++i )
	{
		if( nID == m_pTimerList[i].nTimerID )
		{
			m_pTimerList[i].nTimerID	= 0;
			m_pTimerList[i].nElapse		= 0;
			m_pTimerList[i].nTickLast	= 0;
			m_pTimerList[i].pFTimer		= NULL;
		}
	}
	return nuFALSE;
}

nuVOID CTimerThread::TimerCallProc(nuDWORD nNowTicks)
{
	/*
	while ( m_bTimerFucIsWorking )
	{
		nuSleep(25);
	}
	*/
	m_bTimerFucIsWorking = nuTRUE;
	if( m_pFTimer != NULL )
	{
		if( nNowTicks < m_nTimerOldTicks )
		{
			m_nTimerOldTicks = nNowTicks;
		}
		else if( nNowTicks >= m_nTimerOldTicks + m_nTimer )
		{
			m_pFTimer();
			m_nTimerOldTicks = nNowTicks;
		}
	}
	for( nuINT i = 0; i < MAX_TIMER_NUM; ++i )
	{
		if( m_pTimerList[i].nTimerID && m_pTimerList[i].pFTimer )
		{
			if( nNowTicks < m_pTimerList[i].nTickLast )
			{
				m_pTimerList[i].nTickLast = nNowTicks;
				continue;
			}
			//
			nuDWORD nDiff = nNowTicks - m_pTimerList[i].nTickLast;
			if( nDiff >= m_pTimerList[i].nElapse )
			{
				m_pTimerList[i].pFTimer();
				m_pTimerList[i].nTickLast = nNowTicks;
			}
		}
	}
	m_bTimerFucIsWorking = nuFALSE;
}

nuVOID CTimerThread::DrawRouteAnimation(nuLONG nType)
{
	ANIMATIONDATA anim;
	anim.ShowRect.left	= g_fileSysApi.pGdata->uiSetData.nScreenLTx;
	anim.ShowRect.top	= g_fileSysApi.pGdata->uiSetData.nScreenLTy;
	anim.ShowRect.right	= g_fileSysApi.pGdata->uiSetData.nScreenLTx + g_fileSysApi.pGdata->uiSetData.nScreenWidth;
	anim.ShowRect.bottom	= g_fileSysApi.pGdata->uiSetData.nScreenLTy + g_fileSysApi.pGdata->uiSetData.nScreenHeight;
	anim.bReset	= m_bRouteStart;
	anim.nType	= nType;
	anim.pPath	= g_fileSysApi.pGdata->pTsPath;
//	g_drawIfmApi.IFM_DrawAnimation(&anim);
	if( m_pfDrawAni )
	{
		m_pfDrawAni(&anim);
	}
}

nuVOID CTimerThread::TimerAdd(nuDWORD nTicks)
{
	//--------------calculate the time-----------------http
	g_fileSysApi.pGdata->timerData.nMiniSecond += nTicks;
	if( g_fileSysApi.pGdata->timerData.nMiniSecond >= 1000 ) 
	{
		g_fileSysApi.pGdata->timerData.nMiniSecond %= 1000;
		++ g_fileSysApi.pGdata->timerData.nSecond;
		if( g_fileSysApi.pGdata->timerData.nSecond >= 60 )
		{
			g_fileSysApi.pGdata->timerData.nSecond %= 60;
			++ g_fileSysApi.pGdata->timerData.nMinute;
			if( g_fileSysApi.pGdata->timerData.nMinute >= 60 )
			{
				g_fileSysApi.pGdata->timerData.nMinute %= 60;
				++ g_fileSysApi.pGdata->timerData.nHour;
				if( g_fileSysApi.pGdata->timerData.nHour >= 24 )
				{
					g_fileSysApi.pGdata->timerData.nHour %= 24;
					++ g_fileSysApi.pGdata->timerData.nDay;
					nuBYTE nMonthDays = 31;
					if( 4 == g_fileSysApi.pGdata->timerData.nMonth	||
						6 == g_fileSysApi.pGdata->timerData.nMonth	||
						9 == g_fileSysApi.pGdata->timerData.nMonth	||
						11 == g_fileSysApi.pGdata->timerData.nMonth	 )
					{
						nMonthDays = 30;
					}
					else if( 2 == g_fileSysApi.pGdata->timerData.nMonth )
					{
						if( (g_fileSysApi.pGdata->timerData.nYear % 4) == 0 &&
							(g_fileSysApi.pGdata->timerData.nYear % 100) != 0 )
						{
							nMonthDays = 29;
						}
						else
						{
							nMonthDays = 28;
						}
					}
					if( g_fileSysApi.pGdata->timerData.nDay > nMonthDays )
					{
						g_fileSysApi.pGdata->timerData.nDay %= nMonthDays;
						++ g_fileSysApi.pGdata->timerData.nMonth;
						if( g_fileSysApi.pGdata->timerData.nMonth > 12 )
						{
							g_fileSysApi.pGdata->timerData.nMonth %= 12;
							++ g_fileSysApi.pGdata->timerData.nYear;
						}
					}
				}
			}
		}
	}
}
