#ifndef DATA_THREAD_201606061403_BY_DANIEL
#define DATA_THREAD_201606061403_BY_DANIEL

#include "NaviThread.h"
#include "CodeList.h"

class CDataThread
{
public:
		CDataThread();
		virtual ~CDataThread();
		nuBOOL InitDataThread(class CNaviThread*	pNaviThd);
		nuVOID FreeDataThread();
		nuUINT CodePush(CODENODE CodeNode);
		nuUINT CodeProc();
public:
		static nuDWORD DataThreadAction(nuPVOID lpVoid);

public:
		nuHANDLE			m_hThread;
		nuDWORD     		m_dwThreadID;
		nuBOOL				m_bWait;
		nuBOOL				m_bQuit;
		nuBOOL				m_bOutThreadLoop;
		CCodeList		    m_cCodeProc;
		CODENODE			m_CodeNode;
		class CNaviThread*	m_pNaviThd;
};

#endif
