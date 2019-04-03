
#include "DataThread.h"

extern	FILESYSAPI					g_fileSysApi;

CDataThread::CDataThread()
{
	
}

CDataThread::~CDataThread()
{
	FreeDataThread();
}

nuBOOL CDataThread::InitDataThread(class CNaviThread* pNaviThd)
{
	m_bQuit		= nuFALSE;
	m_bWait		= nuFALSE;
	m_pNaviThd  = pNaviThd;
#ifdef NURO_OS_WINDOWS
	m_hThread = nuCreateThread(NULL, 0, DataThreadAction, this, 0, &m_dwThreadID);
#endif

#ifdef NURO_OS_LINUX
	m_hThread = nuCreateThread(NULL, 0, DataThreadAction, this, 0, &m_dwThreadID);
	
#endif

#if defined(NURO_OS_UCOS) || defined(NURO_OS_JZ)
	m_hThread = nuCreateThread(NULL, 0, (LPNURO_THREAD_START_ROUTINE)DataThreadAction, this, TASK_TIMER, (nuPDWORD)(&m_pStack[TIMER_STACK_SIZE-1]));
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
	m_hThread = nuCreateThread( NULL, TIMER_STACK_SIZE, (LPNURO_THREAD_START_ROUTINE)DataThreadAction,this,TASK_TIMER,NULL );
#endif

	if( m_hThread == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CDataThread::FreeDataThread()
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

nuUINT CDataThread::CodePush(CODENODE CodeNode)
{
	__android_log_print(ANDROID_LOG_INFO, "TMC", "CodePush");
	return m_cCodeProc.PushNode(&CodeNode);
}

nuUINT CDataThread::CodeProc()
{
	__android_log_print(ANDROID_LOG_INFO, "TMC", "CodeProc");
	return g_fileSysApi.FS_Initial_TMC_Event_Data(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx);
}

nuDWORD CDataThread::DataThreadAction(nuPVOID lpVoid)
{
	CDataThread *pThis = (CDataThread*)lpVoid; 
	CODENODE CodeNode;
	while(!pThis->m_bQuit)
	{
		if(pThis->m_cCodeProc.PopNode(&CodeNode))
		{
			while(pThis->m_bWait)
			{
				nuSleep(500);
			}
			if(pThis->CodeProc())
			{
				__android_log_print(ANDROID_LOG_INFO, "TMC", "_TMC_DISPLAY_MAP");
				pThis->m_pNaviThd->UI_TRIGGER_TMC_EVENT(0,  _TMC_DISPLAY_MAP); //copy to navithread
				nuSleep(20000);
			}
		}
		nuSleep(500);
	}
	pThis->m_bQuit = nuTRUE;
	pThis->m_bOutThreadLoop = nuTRUE;
	return 1;
}
