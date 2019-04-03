// NuroThreadZK.cpp: implementation of the CNuroThreadZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../NuroThreadZK.h"
#include "../../NuroDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNuroThreadZK::CNuroThreadZK()
{
    nuMemset(&m_thdHandle, 0, sizeof(m_thdHandle));
    m_bThrWork      = nuFALSE;
    m_bOutThread    = nuTRUE;
}

CNuroThreadZK::~CNuroThreadZK()
{
    DeleteNuroThread();
}

nuBOOL CNuroThreadZK::CreateNuroThread(nuUINT nStackSize /* = _NURO_THRAD_DEFAULT_STACK_SIZE */, 
                                       nuUINT nPriority /* = NURO_THREAD_PRIORITY_LOW */)
{
    m_bThrWork = nuTRUE;

	m_thdHandle = nuCreateThread(NULL, nStackSize = 0, ThdAction, this, NURO_THREAD_CREATE_DEFAULT, NULL);

    if (NULL == m_thdHandle)
    {
        m_bThrWork   = nuFALSE;
        return nuFALSE;
    }
    
	if (NURO_THREAD_CREATE_DEFAULT != nPriority)
	{
		nuSetThreadPriority(m_thdHandle, nPriority);
	}

	return nuTRUE;
}

nuVOID CNuroThreadZK::DeleteNuroThread()
{
    if( m_bThrWork )
    {
        m_bThrWork = nuFALSE;
        while( !m_bOutThread )
        {
            nuSleep(25);
        }
        nuDelThread(m_thdHandle);
        nuMemset(&m_thdHandle, 0, sizeof(m_thdHandle));
    }
}

nuDWORD CNuroThreadZK::ThdAction(nuPVOID lpVoid)
{
    CNuroThreadZK* pThis = (CNuroThreadZK*)lpVoid;
    pThis->m_bOutThread = nuFALSE;
    nuDWORD nRes = pThis->ThreadAction();
    pThis->m_bOutThread = nuTRUE;
    nuThreadWaitReturn(pThis->m_thdHandle);
    return nRes;
}

