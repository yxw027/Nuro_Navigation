// CommportCtrl.cpp: implementation of the CCommportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "CommportCtrl.h"
#include "NuroClasses.h"
#include "GpsLogFileZ.h"


#define MaxComportNum	    12
#define MaxBRType		    8
#define NURO_MAXDWORD       0xFFFFFFFF 
#define WRITECOMWAITTIME	10
#define GPSCOMCONFIG        nuTEXT("UIdata\\ComConfig.dat")
nuLONG BRNumber[]={2440, 4800, 9600, 14400, 19200, 38400, 57600, 115200};
//nuLONG BRNumber[]={38400,19200,9600,4800};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommportCtrl::CCommportCtrl()
{
	m_hComPort = NURO_INVALID_HANDLE_VALUE;
	m_ComConfig.GPSBaudrate = 0;
	m_ComConfig.GPSCommPort = 0;
	l_OpenCom_RW=0;
}

CCommportCtrl::~CCommportCtrl()
{
	ResetClass();
}

nuVOID CCommportCtrl::ResetClass()
{
	nuHANDLE hport=m_hComPort;
	m_hComPort = NURO_INVALID_HANDLE_VALUE;
	if( hport != NURO_INVALID_HANDLE_VALUE )	
	{
		nuCloseCom(hport); 
	}
}

nuHANDLE CCommportCtrl::InitComPort( PCOMCFG pComCfg )
{
#ifndef ANDROID
	if ( pComCfg == NULL )
	{
		if (nuFALSE == bIniComConfig())
		{
			if ( !nuReadConfigValue("COMPORT", &(m_ComConfig.GPSCommPort)) )
			{
				m_ComConfig.GPSCommPort = 1;
			}
			if(  !nuReadConfigValue("COMBAUDRATE", &(m_ComConfig.GPSBaudrate)))
			{
				m_ComConfig.GPSBaudrate = 9600;
			}
		}
	}
	else
	{
		m_ComConfig.GPSCommPort = pComCfg->GPSCommPort;
		m_ComConfig.GPSBaudrate = pComCfg->GPSBaudrate;
	}

	if (0 == m_ComConfig.GPSCommPort &&
		0 == m_ComConfig.GPSBaudrate)
	{  
		return AutoInitComport(pComCfg);
	}

	nuLONG				nComPortIdx;
	//nuLONG				nBRIdx;//BaudRate
	nuTCHAR GPS_COMPORT[10] = {0};
	nuMemset( GPS_COMPORT, 0, sizeof(GPS_COMPORT) );
	nuTcscat(GPS_COMPORT,nuTEXT("COM"));

	//
	if(m_hComPort != NURO_INVALID_HANDLE_VALUE)
	{
		nuCloseCom(m_hComPort);
		m_hComPort = NURO_INVALID_HANDLE_VALUE;
	}
	//
	nComPortIdx	= m_ComConfig.GPSCommPort;
	if( nComPortIdx != 0 )
	{
		nuItot(nComPortIdx, &GPS_COMPORT[3], 10);
#ifdef NURO_OS_WINCE
		nuTcscat(GPS_COMPORT,nuTEXT(":"));	
#endif
		
		if ( !nuReadConfigValue("OPENCOMRW", &l_OpenCom_RW) )
		{
			l_OpenCom_RW = 0;
		}
		m_hComPort = nuOpenCom(GPS_COMPORT, m_ComConfig.GPSBaudrate,l_OpenCom_RW);
		if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
		{	
			nComPortIdx = 0;	
		}
	}

	while( m_hComPort == NURO_INVALID_HANDLE_VALUE && nComPortIdx <= MaxComportNum )
	{

		nuItot(nComPortIdx, &GPS_COMPORT[3], 10);
		nuMemset(&GPS_COMPORT[4], 0, sizeof(nuTCHAR));
#ifdef NURO_OS_WINCE
		nuTcscat(GPS_COMPORT,nuTEXT(":"));	
#endif
		if ( !nuReadConfigValue("OPENCOMRW", &l_OpenCom_RW) )
		{
			l_OpenCom_RW = 0;
		}
		m_hComPort = nuOpenCom(GPS_COMPORT, m_ComConfig.GPSBaudrate,l_OpenCom_RW);
		if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
		{	
			nComPortIdx ++;
			continue;	
		}
		break;
	}

	if( m_hComPort == NURO_INVALID_HANDLE_VALUE )
	{
		return NURO_INVALID_HANDLE_VALUE;
	}

	return m_hComPort;
#else
	return NULL;
#endif
}

nuHANDLE CCommportCtrl::InitComPortPS( PCOMCFG pComCfg )
{
#ifndef ANDROID
	nuLONG				nComPortIdx = 0;
	nuTCHAR GPS_COMPORT[10] = {0};
	nuMemset( GPS_COMPORT, 0, sizeof(GPS_COMPORT) );
	nuTcscat(GPS_COMPORT,nuTEXT("COM"));

	if ( pComCfg == NULL )
	{
		if (nuFALSE == bIniComConfig())
		{

			if ( !nuReadConfigValue("COMPORT", &(m_ComConfig.GPSCommPort)) )
			{
				m_ComConfig.GPSCommPort = 7;
			}
			if(  !nuReadConfigValue("COMBAUDRATE", &(m_ComConfig.GPSBaudrate)))
			{
				m_ComConfig.GPSBaudrate = 38400;
			}
		}
	}
	else
	{
		m_ComConfig.GPSCommPort = pComCfg->GPSCommPort;
		m_ComConfig.GPSBaudrate = pComCfg->GPSBaudrate;
	}
	//
	if(m_hComPort != NURO_INVALID_HANDLE_VALUE)
	{
		nuCloseCom(m_hComPort);
		m_hComPort = NURO_INVALID_HANDLE_VALUE;
	}
	//
	nComPortIdx	= m_ComConfig.GPSCommPort;
	if( nComPortIdx != 0 )
	{
		nuItot(nComPortIdx, &GPS_COMPORT[3], 10);
#ifdef NURO_OS_WINCE
		nuTcscat(GPS_COMPORT,nuTEXT(":"));	
#endif

		if ( !nuReadConfigValue("OPENCOMRW", &l_OpenCom_RW) )
		{
			l_OpenCom_RW = 0;
		}
		m_hComPort = nuOpenCom(GPS_COMPORT, m_ComConfig.GPSBaudrate,l_OpenCom_RW);

		if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
		{	
			nComPortIdx = 0;	
		}
	}
	if( m_hComPort == NURO_INVALID_HANDLE_VALUE )
	{
		return NURO_INVALID_HANDLE_VALUE;
	}

	return m_hComPort;
#else
	return NULL;
#endif
}

nuHANDLE CCommportCtrl::InitComPortAuto( PCOMCFG pComCfg/* = NULL */)
{
#ifndef ANDROID
	nuLONG				nComPortIdx = 0;
	nuTCHAR GPS_COMPORT[10] = {0};
	nuMemset( GPS_COMPORT, 0, sizeof(GPS_COMPORT) );
	nuTcscat(GPS_COMPORT,nuTEXT("COM"));

	//
	if(m_hComPort != NURO_INVALID_HANDLE_VALUE)
	{
		nuCloseCom(m_hComPort);
		m_hComPort = NURO_INVALID_HANDLE_VALUE;
	}
	//
	nComPortIdx	= m_ComConfig.GPSCommPort;

	while( m_hComPort == NURO_INVALID_HANDLE_VALUE && nComPortIdx <= MaxComportNum )
	{
		nuItot(nComPortIdx, &GPS_COMPORT[3], 10);
		nuMemset(&GPS_COMPORT[4], 0, sizeof(nuTCHAR));
#ifdef NURO_OS_WINCE
		nuTcscat(GPS_COMPORT,nuTEXT(":"));	
#endif
		if ( !nuReadConfigValue("OPENCOMRW", &l_OpenCom_RW) )
		{
			l_OpenCom_RW = 0;
		}
		m_hComPort = nuOpenCom(GPS_COMPORT, m_ComConfig.GPSBaudrate,l_OpenCom_RW);

		if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
		{	
			nComPortIdx ++;
			continue;
		}
		break;
	}

	if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
	{
		return NURO_INVALID_HANDLE_VALUE;
	}

	return m_hComPort;
#else
	return NULL;
#endif
}

nuHANDLE CCommportCtrl::AutoInitComport( PCOMCFG pComCfg/* = NULL*/ )
{
	nuBOOL bFineRealComport = nuFALSE;
	do
	{
		if (nuFALSE == bFineRealComport &&
			NURO_INVALID_HANDLE_VALUE != m_hComPort)
		{
			nuHANDLE hport=m_hComPort;
			m_hComPort = NURO_INVALID_HANDLE_VALUE;
			if( hport != NURO_INVALID_HANDLE_VALUE )	
			{
				nuCloseCom(hport); 
			}
			m_ComConfig.GPSCommPort++;
		}

		if (NURO_INVALID_HANDLE_VALUE !=  InitComPortAuto(pComCfg))
		{
			// ????comport & Baudrate
			nuDWORD nTick = nuGetTickCount();
			nuCHAR  ReceiveCharBuf[1024] = {0}; 
			nuDWORD ReceiveCharBufNum = 0;

			while (500 > nuGetTickCount()-nTick)
			{
				nuMemset(ReceiveCharBuf, 0 ,1024 );
				GetBuff((nuBYTE*)ReceiveCharBuf, 1024, &ReceiveCharBufNum);

				nuINT jj = nuStrlen(ReceiveCharBuf);

				for (nuINT i=0; 
					0<(jj-3)&&i<(jj-3); 
					++i)
				{

					if (0 == nuStrncmp(ReceiveCharBuf+i, "$GP", 3))
					{
						bFineRealComport = nuTRUE;
						break;
					}
				}
				nuSleep(100);
			}
		}
	}while(NURO_INVALID_HANDLE_VALUE  != m_hComPort &&
		nuFALSE                       == bFineRealComport);

	if (nuTRUE == bFineRealComport)
	{
		bWriteComConfig();
	}

	return m_hComPort;
}

nuBOOL CCommportCtrl::GetBuff(nuBYTE *pBuff, nuDWORD nLen, nuDWORD *pBuffGet)
{
	if( m_hComPort == NULL )
	{
		return nuFALSE;
	}

	if( nuReadCom(m_hComPort, pBuff, nLen, pBuffGet, NULL) == 0 )
	{
		return nuFALSE;
	}

	return nuTRUE;
}

nuBOOL CCommportCtrl::WriteBuff(nuDWORD SETTYPE)
{
	nuBYTE UBLOXSTR[64] = {0};
	nuDWORD BuffGet = 0;
	nuDWORD nWriteLon = 0;
	if( m_hComPort == NULL )
	{
		return nuFALSE;
	}
	nuMemset(UBLOXSTR,0,64);

	switch(SETTYPE)
	{
		case 1:
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x12;
			UBLOXSTR[4]=0x10;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x01;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x0D;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0xAC;	UBLOXSTR[15]=0x0D;
			UBLOXSTR[16]=0xC4;	UBLOXSTR[17]=0x09;	UBLOXSTR[18]=0x19;	UBLOXSTR[19]=0x4B;
			UBLOXSTR[20]=0x08;	UBLOXSTR[21]=0x07;	UBLOXSTR[22]=0x30;	UBLOXSTR[23]=0x67;
			nWriteLon=24;
			break;
		case 2://變成DR模式
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x12;
			UBLOXSTR[4]=0x10;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x86;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x03;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x0D;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0xAC;	UBLOXSTR[15]=0x0D;
			UBLOXSTR[16]=0xC4;	UBLOXSTR[17]=0x09;	UBLOXSTR[18]=0x19;	UBLOXSTR[19]=0x4B;
			UBLOXSTR[20]=0x08;	UBLOXSTR[21]=0x07;	UBLOXSTR[22]=0xB8;	UBLOXSTR[23]=0x58;
			nWriteLon=24;
			break;
		case 3://傳出版本資訊

			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x0A;	UBLOXSTR[3]=0x04;
			UBLOXSTR[4]=0x00;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x0E;	UBLOXSTR[7]=0x34;
			nWriteLon=8;
			break;
		case 4://打開EKFStatus
			//Default Config setting
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x09;
			UBLOXSTR[4]=0x0D;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0xFF;	UBLOXSTR[7]=0xFF;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x00;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0xFF;	UBLOXSTR[15]=0xFF;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x07;	UBLOXSTR[19]=0x1F;
			UBLOXSTR[20]=0x9E;
			nWriteLon=21;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//enable MEAS status
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x01;
			UBLOXSTR[4]=0x08;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x10;	UBLOXSTR[7]=0x02;
			UBLOXSTR[8]=0x01;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x01;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x01;	UBLOXSTR[13]=0x01;	UBLOXSTR[14]=0x27;	UBLOXSTR[15]=0x46;
			nWriteLon=16;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//enable EKFstatus status
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x01;
			UBLOXSTR[4]=0x08;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x01;	UBLOXSTR[7]=0x40;
			UBLOXSTR[8]=0x01;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x01;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x01;	UBLOXSTR[13]=0x01;	UBLOXSTR[14]=0x56;	UBLOXSTR[15]=0x80;
			nWriteLon=16;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//change UBX-CFG-NAV5 position accuracy mask from 100m to 50m
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x24;
			UBLOXSTR[4]=0x24;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0xFF;	UBLOXSTR[7]=0xFF;
			UBLOXSTR[8]=0x04;	UBLOXSTR[9]=0x03;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x00;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x10;	UBLOXSTR[15]=0x27;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x05;	UBLOXSTR[19]=0x00;
			UBLOXSTR[20]=0xFA;	UBLOXSTR[21]=0x00;	UBLOXSTR[22]=0xFA;	UBLOXSTR[23]=0x00;
			UBLOXSTR[24]=0x32;	UBLOXSTR[25]=0x00;	UBLOXSTR[26]=0x2C;	UBLOXSTR[27]=0x01;
			UBLOXSTR[28]=0x00;	UBLOXSTR[29]=0x3C;	UBLOXSTR[30]=0x00;	UBLOXSTR[31]=0x00;
			UBLOXSTR[32]=0x00;	UBLOXSTR[33]=0x00;	UBLOXSTR[34]=0x00;	UBLOXSTR[35]=0x00;
			UBLOXSTR[36]=0x00;	UBLOXSTR[37]=0x00;	UBLOXSTR[38]=0x00;	UBLOXSTR[39]=0x00;
			UBLOXSTR[40]=0x00;	UBLOXSTR[41]=0x00;	UBLOXSTR[42]=0x1E;	UBLOXSTR[43]=0x20;
			nWriteLon=44;

			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//enable NMEA output for invalid fixes and masked fixes
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x17;
			UBLOXSTR[4]=0x04;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x03;	UBLOXSTR[7]=0x23;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x02;	UBLOXSTR[10]=0x49;	UBLOXSTR[11]=0x60;
			nWriteLon=12;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//save config
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x09;
			UBLOXSTR[4]=0x0D;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0xFF;	UBLOXSTR[11]=0xFF;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x00;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x07;	UBLOXSTR[19]=0x21;
			UBLOXSTR[20]=0xAF;
			nWriteLon=21;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//reseting command
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x04;
			UBLOXSTR[4]=0x04;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x02;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x10;	UBLOXSTR[11]=0x68;
			nWriteLon=12;
			break;
		case 5://UBlox Reset Gyro
			//clean calibration data
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x12;
			UBLOXSTR[4]=0x10;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x06;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x08;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x00;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0xC4;	UBLOXSTR[17]=0x09;	UBLOXSTR[18]=0x19;	UBLOXSTR[19]=0x0A;
			UBLOXSTR[20]=0xB0;	UBLOXSTR[21]=0x04;	UBLOXSTR[22]=0xDC;	UBLOXSTR[23]=0x5B;
			nWriteLon=24;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//change to max performance and save config
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x11;
			UBLOXSTR[4]=0x02;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x08;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x21;	UBLOXSTR[9]=0x91;	UBLOXSTR[10]=0xB5;	UBLOXSTR[11]=0x62;
			UBLOXSTR[12]=0x06;	UBLOXSTR[13]=0x09;	UBLOXSTR[14]=0x0D;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x00;	UBLOXSTR[19]=0x00;
			UBLOXSTR[20]=0xFF;	UBLOXSTR[21]=0xFF;	UBLOXSTR[22]=0x00;	UBLOXSTR[23]=0x00;
			UBLOXSTR[24]=0x00;	UBLOXSTR[25]=0x00;	UBLOXSTR[26]=0x00;	UBLOXSTR[27]=0x00;
			UBLOXSTR[28]=0x07;	UBLOXSTR[29]=0x21;	UBLOXSTR[30]=0xAF;
			nWriteLon=31;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//SW hot start reset
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x04;
			UBLOXSTR[4]=0x04;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x02;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x10;	UBLOXSTR[11]=0x68;
			nWriteLon=12;
			break;
		case 6:
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x01;
			UBLOXSTR[4]=0x08;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x10;	UBLOXSTR[7]=0x10;
			UBLOXSTR[8]=0x01;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x01;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x01;	UBLOXSTR[13]=0x01;	UBLOXSTR[14]=0x35;	UBLOXSTR[15]=0xA8;
			nWriteLon=16;
			break;

		case 7://Reset PA30M M8L Module
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x56;
			UBLOXSTR[4]=0x0C;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x01;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x00;	UBLOXSTR[11]=0x00;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x00;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x69;	UBLOXSTR[19]=0x1D;
			nWriteLon=20;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x09;
			UBLOXSTR[4]=0x0D;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0xFF;	UBLOXSTR[11]=0xFF;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x00;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x03;	UBLOXSTR[19]=0x1D;
			UBLOXSTR[20]=0xAB;
			nWriteLon=21;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			//enable EKFstatus status
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x04;
			UBLOXSTR[4]=0x04;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0xFF;	UBLOXSTR[7]=0xB9;
			UBLOXSTR[8]=0x02;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0xC8;	UBLOXSTR[11]=0x8F;
			nWriteLon=12;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);
			break;
		case 8://Reset PA30M M8L Angle one
			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x01;
			UBLOXSTR[4]=0x08;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x10;	UBLOXSTR[7]=0x10;
			UBLOXSTR[8]=0x01;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x01;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x33;	UBLOXSTR[15]=0xA5;
			nWriteLon=16;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x01;
			UBLOXSTR[4]=0x08;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x10;	UBLOXSTR[7]=0x02;
			UBLOXSTR[8]=0x01;	UBLOXSTR[9]=0x01;	UBLOXSTR[10]=0x01;	UBLOXSTR[11]=0x01;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x25;	UBLOXSTR[15]=0x43;
			nWriteLon=16;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x56;
			UBLOXSTR[4]=0x0C;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0x50;	UBLOXSTR[11]=0x46;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x24;	UBLOXSTR[15]=0xFA;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x1C;	UBLOXSTR[19]=0xFA;
			nWriteLon=20;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);

			UBLOXSTR[0]=0xB5;	UBLOXSTR[1]=0x62;	UBLOXSTR[2]=0x06;	UBLOXSTR[3]=0x09;
			UBLOXSTR[4]=0x0D;	UBLOXSTR[5]=0x00;	UBLOXSTR[6]=0x00;	UBLOXSTR[7]=0x00;
			UBLOXSTR[8]=0x00;	UBLOXSTR[9]=0x00;	UBLOXSTR[10]=0xFF;	UBLOXSTR[11]=0xFF;
			UBLOXSTR[12]=0x00;	UBLOXSTR[13]=0x00;	UBLOXSTR[14]=0x00;	UBLOXSTR[15]=0x00;
			UBLOXSTR[16]=0x00;	UBLOXSTR[17]=0x00;	UBLOXSTR[18]=0x03;	UBLOXSTR[19]=0x1D;
			UBLOXSTR[20]=0xAB;
			nWriteLon=21;
			if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
			{
				return nuFALSE;
			}
			nuSleep(WRITECOMWAITTIME);
			nuMemset(UBLOXSTR,0,64);
			break;
		default:
			return nuFALSE;
	}
	if( nuWriteCom(m_hComPort, UBLOXSTR, nWriteLon, &nWriteLon, NULL) == 0 )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL  CCommportCtrl::GetGPSComConfig(nuLONG& GPSCommPort, nuLONG& GPSBaudrate)
{
	GPSCommPort = m_ComConfig.GPSCommPort;
	GPSBaudrate = m_ComConfig.GPSBaudrate;
	return 1;
}

nuBOOL CCommportCtrl::bIniComConfig()
{
	//	nuTCHAR tPath;
	nuTCHAR tsPath[256] = {0};

	nuGetModulePath(NULL, tsPath, 256);
	nuTcscat(tsPath, GPSCOMCONFIG);

	nuFILE* pFile = nuTfopen(tsPath, NURO_FS_RB);

	if (NULL == pFile)
	{   // NoFile 
		return nuFALSE;
	}

	if (sizeof(COMCFG) != nuFgetlen(pFile))
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if (1 != nuFread(&m_ComConfig, sizeof(COMCFG), 1, pFile))
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}

nuBOOL CCommportCtrl::bWriteComConfig()
{
	nuTCHAR tsPath[256] = {0};
	nuGetModulePath(NULL, tsPath, 256);
	nuTcscat(tsPath, GPSCOMCONFIG);

	nuFILE* pFile = nuTfopen(tsPath, NURO_FS_OB);

	if (NULL == pFile)
	{   // NoFile 
		return nuFALSE;
	}

	if (1 != nuFwrite(&m_ComConfig, sizeof(COMCFG), 1, pFile))
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}
nuVOID CCommportCtrl::WriteCom(nuBYTE *Data, nuLONG nLen)
{
	nuDWORD DataLen = 0;
	if(m_hComPort)
	{
		nuWriteCom(m_hComPort, Data, nLen, &DataLen, NULL);
	}
}

nuBOOL CCommportCtrl::WriteBinaryReset()
{
	nuBYTE RESETSTR[255] = {0};
	nuDWORD BuffGet=0;
	if( m_hComPort == NULL )
	{
		return nuFALSE;
	}
	RESETSTR[0]=0xA0;
	RESETSTR[1]=0xA2;
	RESETSTR[2]=0x00;
	RESETSTR[3]=0x19;
	RESETSTR[4]=0x80;
	RESETSTR[27]=0x0C;
	RESETSTR[28]=0x88;
	RESETSTR[29]=0x01;
	RESETSTR[30]=0x14;
	RESETSTR[31]=0xB0;
	RESETSTR[32]=0xB3;

	WriteCom(RESETSTR,  33);
	return nuTRUE;

}






















