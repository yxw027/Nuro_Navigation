// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TimerThread.h,v 1.4 2010/09/07 03:18:34 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/09/07 03:18:34 $
// $Locker:  $
// $Revision: 1.4 $
// $Source: /home/nuCode/libNaviCtrl/TimerThread.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERTHREAD_H__F6F38915_4C10_467D_88B6_BF5FEB0417EE__INCLUDED_)
#define AFX_TIMERTHREAD_H__F6F38915_4C10_467D_88B6_BF5FEB0417EE__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "GApiNaviCtrl.h"

#define TIMER_STACK_SIZE				(16*1024)

#define MAX_TIMER_NUM					5
typedef struct tagTIMER_CAL_FUNCS
{
	nuDWORD				nTimerID;//0, unusefullTimer;
	nuDWORD				nTickLast;
	nuDWORD				nElapse;
	NuroTimerProc		pFTimer;
}TIMER_CAL_FUNCS, *PTIMER_CAL_FUNCS;

class CTimerThread  
{
public:
	CTimerThread();
	virtual ~CTimerThread();
	nuBOOL InitTimerThread(class CNaviThread*	pNaviThd);
	nuVOID FreeTimerThread();

	nuBOOL NuroSetTimer(nuPVOID lpVoid, nuDWORD nTimer);

	nuBOOL NuroSetTimerEx(nuDWORD nID, nuDWORD nElapse, nuPVOID lpTimerProc);
	nuBOOL NuroKillTimerEx(nuDWORD nID);

public:
	static nuDWORD TiThreadAction(nuPVOID lpVoid);
	nuVOID DrawRouteAnimation(nuLONG nType);

protected:
	nuVOID TimerCallProc(nuDWORD nNowTicks);
	nuVOID TimerAdd(nuDWORD nTicks);

public:
	nuHANDLE			m_hThread;
	nuDWORD     		m_dwThreadID;
	nuBOOL				m_bQuit;
	nuBOOL				m_bOutThreadLoop;
	nuBOOL				m_bRouteStart;
	nuBOOL				m_bTimerFucIsWorking;
	class CNaviThread*	m_pNaviThd;
	nuDWORD				m_nTimerOldTicks;
	nuDWORD				m_nTimer;
	NuroTimerProc		m_pFTimer;
	DrawAnimationProc   m_pfDrawAni;

	TIMER_CAL_FUNCS		m_pTimerList[MAX_TIMER_NUM];
#if defined(NURO_OS_UCOS) || defined(NURO_OS_ECOS) || defined(NURO_OS_JZ)
	nuINT				m_pStack[TIMER_STACK_SIZE];
#endif
private:
};

#endif // !defined(AFX_TIMERTHREAD_H__F6F38915_4C10_467D_88B6_BF5FEB0417EE__INCLUDED_)
