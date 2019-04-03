// CommportCtrl.h: interface for the CCommportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMPORTCTRL_H__C8AF7E02_4988_4203_8045_02DDC3346B64__INCLUDED_)
#define AFX_COMMPORTCTRL_H__C8AF7E02_4988_4203_8045_02DDC3346B64__INCLUDED_


#include "NuroClasses.h"


typedef struct tagCOMCFG
{
	nuLONG	GPSCommPort;
	nuLONG	GPSBaudrate;
}COMCFG,*PCOMCFG;

class CCommportCtrl  
{
	public:
		CCommportCtrl();
		virtual ~CCommportCtrl();
		nuVOID ResetClass();

		nuBOOL	GetBuff(nuBYTE* pBuff, nuDWORD nLen, nuDWORD* pBuffGet);
		nuBOOL	WriteBuff(nuDWORD SETTYPE);

		nuHANDLE InitComPort( PCOMCFG pComCfg = NULL );
		nuHANDLE InitComPortPS( PCOMCFG pComCfg = NULL );
		nuHANDLE InitComPortAuto( PCOMCFG pComCfg = NULL );
		nuHANDLE AutoInitComport( PCOMCFG pComCfg = NULL ); // Added by Damon
		nuBOOL   GetGPSComConfig(nuLONG& GPSCommPort, nuLONG& GPSBaudrate);
		nuVOID	 WriteCom(nuBYTE *Data, nuLONG nLen);
		nuBOOL WriteBinaryReset();
	private:
		nuBOOL bIniComConfig();
		nuBOOL bWriteComConfig();

	public:
		nuHANDLE	m_hComPort;	// set handle of the COM port

	protected:
		COMCFG	m_ComConfig;
		nuLONG	l_OpenCom_RW;		
};

#endif // !defined(AFX_COMMPORTCTRL_H__C8AF7E02_4988_4203_8045_02DDC3346B64__INCLUDED_)
