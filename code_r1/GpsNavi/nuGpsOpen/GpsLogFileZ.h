// GpsLogFileZ.h: interface for the CGpsLogFileZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPSLOGFILEZ_H__77474152_8359_4B41_826D_D3431969570D__INCLUDED_)
#define AFX_GPSLOGFILEZ_H__77474152_8359_4B41_826D_D3431969570D__INCLUDED_

#include "NuroClasses.h"
class CGpsLogFileZ  
{
public:
	bool InitFile(bool bDelte = true);
	bool AppendGpsData(char *psGpsData,int nLen);
	CGpsLogFileZ( );
	virtual ~CGpsLogFileZ();

private:
	nuTCHAR m_sFilePath[NURO_MAX_PATH];
	nuLONG  m_nVenderType;
};

#endif // !defined(AFX_GPSLOGFILEZ_H__77474152_8359_4B41_826D_D3431969570D__INCLUDED_)
