// GpsThreadZK.cpp: implementation of the CGpsThreadZK class.
//
//////////////////////////////////////////////////////////////////////

#include "GpsThreadZK.h"
#include "NaviThread.h"
#include "NuroModuleApiStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern	FILESYSAPI					g_fileSysApi;

CGpsThreadZK::CGpsThreadZK()
{
	m_pclsNaviThread	= NULL;
	m_hThread			= NULL;
	m_bQuit				= nuTRUE;
	m_bOutThreadLoop	= nuTRUE;
}

CGpsThreadZK::~CGpsThreadZK()
{

}

nuDWORD CGpsThreadZK::GpsThreadAction(nuPVOID lpVoid)
{
	CGpsThreadZK* pThis = (CGpsThreadZK*)lpVoid;
	pThis->m_bOutThreadLoop	= nuFALSE;
	pThis->m_bQuit = nuFALSE;
	while( !pThis->m_bQuit )
	{
		nuSleep(500);
		pThis->m_pclsNaviThread->IntervalGetGps();
		g_fileSysApi.pGdata->timerData.nGetGpsTime = 0;
	}
	pThis->m_bQuit = nuTRUE;
	pThis->m_bOutThreadLoop	= nuTRUE;
	nuThreadWaitReturn(pThis->m_hThread);
	return 0;
}

nuBOOL CGpsThreadZK::InitGpsThread(class CNaviThread* pNaviThd)
{
	if( NULL == pNaviThd )
	{
		return nuFALSE;
	}
	m_pclsNaviThread = pNaviThd;

	m_bQuit		= nuFALSE;
	
#ifdef NURO_OS_ECOS
	NURO_SECURITY_ATTRIBUTES attr = {0};
	attr.nLength = (nuDWORD)(&m_dwThreadID);
	attr.bInheritHandle = (nuBOOL)(&m_hThread);
	m_hThread = nuCreateThread ( &attr,
		4 * GPS_STACK_SIZE,
		(LPNURO_THREAD_START_ROUTINE)GpsThreadAction,
		this,
		TASK_GPS,
		(nuPDWORD)m_pStack );
#endif
	if( m_hThread == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CGpsThreadZK::FreeGpsThread()
{
	m_bQuit = nuTRUE;
	m_bOutThreadLoop = nuFALSE;
	while( !m_bOutThreadLoop )
	{
		nuSleep(25);
	}
	m_bQuit = nuFALSE;
}
