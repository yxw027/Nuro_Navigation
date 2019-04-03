// GpsThreadZK.h: interface for the CGpsThreadZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPSTHREADZK_H__5A3ABBDD_62E0_45A3_AD6C_03B7F5A3D41D__INCLUDED_)
#define AFX_GPSTHREADZK_H__5A3ABBDD_62E0_45A3_AD6C_03B7F5A3D41D__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

#define GPS_STACK_SIZE				(16*1024)

class CGpsThreadZK  
{
public:
	CGpsThreadZK();
	virtual ~CGpsThreadZK();

	static nuDWORD GpsThreadAction(nuPVOID lpVoid);

	nuBOOL InitGpsThread(class CNaviThread*	pNaviThd);
	nuVOID FreeGpsThread();

protected:
	class CNaviThread*	m_pclsNaviThread;

	nuHANDLE			m_hThread;
	//TASK_ID_TYPE		m_dwThreadID;
	nuBOOL				m_bQuit;
	nuBOOL				m_bOutThreadLoop;
#if defined(NURO_OS_ECOS) 
	nuINT				m_pStack[GPS_STACK_SIZE];
#endif
};

#endif // !defined(AFX_GPSTHREADZK_H__5A3ABBDD_62E0_45A3_AD6C_03B7F5A3D41D__INCLUDED_)
