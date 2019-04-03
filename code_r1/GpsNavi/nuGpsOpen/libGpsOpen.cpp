
#include "libGpsOpen.h"
#include "CommportCtrl.h"
#include "GPSDataCol.h"
#include "NuroDefine.h"
#include "GpsLogFileZ.h"
#include "GpsEstimateZ.h"
#define	GPS_FIX_MIN_SPEED					48	//modify for fixroad

#ifdef ANDROID_GPS
#define ANDROID_GPS_LEN 128
static nuBYTE sbuffer[128][ANDROID_GPS_LEN]={0};
static nuINT  slen=0;
static nuCHAR sNmeaBuffer [1024] ; 
static nuINT nPopIdx=-1;
static nuINT nPushIdx=0;
#include <stdio.h>
#include <android/log.h>
#endif
typedef union tagUNIONGPS
{
	nuFILE*			pFile;
	CCommportCtrl*	pComm;
	nuHANDLE		hUcos;
}UNIONGPS,*PUNIONGPS;

#ifdef ANDROID_GPS
int PopJNI(nuBYTE *ReceiveCharBuf)
{
	if(nPopIdx == -1)
	{
		return nuFALSE;
	}		
	int n=0;
	int nlimit = 1024 - ANDROID_GPS_LEN;
	nuMemset(sNmeaBuffer,0,sizeof(sNmeaBuffer));
	int tempIdx=nPopIdx;
	while(sbuffer[nPushIdx][0]!=0)//nPushIdx!=tempIdx)
	{
		nuCHAR cTemp[1024] = {0};	
	
		//nuMemcpy(ReceiveCharBuf,sbuffer[nPushIdx],ANDROID_GPS_LEN);
		nuMemcpy(cTemp,sbuffer[nPushIdx],ANDROID_GPS_LEN);
		nuStrcat(sNmeaBuffer,cTemp);		
		for(int i=0;i<ANDROID_GPS_LEN;i++)
		{	
			if(sbuffer[nPushIdx][i]!=0)
				n++;
			else
				break;		
		}
	
		nuMemset(sbuffer[nPushIdx],0,sizeof(sbuffer[nPushIdx]));
		if(nPushIdx<127)
		nPushIdx++;
		else
		nPushIdx=0;
		
		if(n > nlimit)
			break;		
	}
	nuMemcpy(ReceiveCharBuf,sNmeaBuffer,n);
	__android_log_print(ANDROID_LOG_INFO, "NuroService", "[GPS]%s ",ReceiveCharBuf);
	return n;
}
#endif
//GPSOpen運作中
#define 		DEF_ANGLE			500
#define 		BUF_LENGTH			1024
#define			GPS_NO_LOG			0
#define			GPS_LOG_ALL			1
#define			GPS_LOG_PART		2
nuBOOL			b_GPSOpenWorking=nuFALSE;
UNIONGPS		g_hGps = {0};
CGPSDataCol*	g_pGpsDataCol = NULL;
nuBYTE			g_openType = GPS_OPEN_CLOSE;
nuBYTE			g_SourceType = GPS_OPEN_CLOSE;
nuBYTE			g_nGPSLogMode = 0;
nuLONG			g_nSpeed36ShiftDis = 10;
nuLONG			g_nBaseShiftDis = 20;
nuTCHAR			sSourceFileName[NURO_MAX_PATH] = {0};
nuLONG			g_nGPSLogReadSize = 0;//Louis 使用log為source時 每次讀取的資料量
nuVOID			GetPanaRegistry(nuLONG* nGpsMode);//Kris add @130104

nuBOOL	g_FirstGpsOn = nuFALSE;//Kris add @2012-11-21
nuBOOL ChkFirstGpsOn();							//Kris add @2012-11-21
nuBOOL ReadGFF(nuLONG* iDayHour,nuINT* iTimes, nuINT* iExpiration);		//Kris add @2012-11-21
nuBOOL SaveGFF(nuCHAR xmm[NURO_MAX_PATH],nuINT Len);	//Kris add @2012-11-21

GPSOPEN_API nuBOOL LibGPS_InitGpsOpen(PAPISTRUCTADDR pApiAddr)
{
	g_hGps.pComm	= NULL;
	g_pGpsDataCol	= new CGPSDataCol();
	if( !g_pGpsDataCol )
	{
		return nuFALSE;
	}
	pApiAddr->pGpsApi->GPS_Open				=	LibGPS_Open;
	pApiAddr->pGpsApi->GPS_Close			=	LibGPS_Close;
	pApiAddr->pGpsApi->GPS_RecvData			=	LibGPS_RecvData;
	pApiAddr->pGpsApi->GPS_GetNewData		=	LibGPS_GetNewData;
	pApiAddr->pGpsApi->GPS_TOBEDEBUGINFO	=	LibGPS_TOBEDEBUGINFO;
	pApiAddr->pGpsApi->GPS_TOBERESETGYRO	=	LibGPS_TOBERESETGYRO;
	pApiAddr->pGpsApi->GPS_TOBEGETVERSION	=	LibGPS_TOBEGETVERSION;

	pApiAddr->pGpsApi->GPS_GetGpsXY		= LibGPS_GetGpsXY;
	pApiAddr->pGpsApi->GPS_GetState		= LibGPS_GetState;
	pApiAddr->pGpsApi->GPS_FeedBackInfo = LibGPS_FeedBackInfo;
	pApiAddr->pGpsApi->GPS_FeedBack		= LibGPS_FeedBack;
	pApiAddr->pGpsApi->GPS_ReSet        = LibGPS_ReSet;
	pApiAddr->pGpsApi->GPS_SetGPSChina	= LibGPS_SetGPSChina;
	pApiAddr->pGpsApi->GPS_GetRawData	= LibGPS_GetRawData;//added by daniel @20120204
	pApiAddr->pGpsApi->GPS_GetGPSComConfig = LibGPS_GetGPSComConfig; //Added by Damon 20120705
	pApiAddr->pGpsApi->GPS_ReSet        = LibGPS_ReSet; //Added by Damon 20120705
	pApiAddr->pGpsApi->GPS_ELEAD_TRAN   = LibGPS_ELEAD_TRAN;
	pApiAddr->pGpsApi->GPS_WriteCom		= LibGPS_WriteCom;

	#ifdef ANDROID_GPS
	pApiAddr->pGpsApi->GPS_JNI_RecvData = LibGPS_JNI_RecvData;
	#endif	
	nuMemset( &g_pGpsDataCol->m_GyroBackData, 0, sizeof(g_pGpsDataCol->m_GyroBackData) );
	
	if( !nuReadConfigValue( "SPEED36SHIFTDIS", &g_nSpeed36ShiftDis ) )
	{
		g_nSpeed36ShiftDis = 10;
	}
	if( !nuReadConfigValue( "BASESHIFTDIS", &g_nBaseShiftDis ) )
	{
		g_nBaseShiftDis = 20;
	}
	if( !nuReadConfigValue( "GPSLOGREADSIZE", &g_nGPSLogReadSize ) )
	{
		g_nGPSLogReadSize = 600;
	}
	return nuTRUE;
}
GPSOPEN_API nuVOID LibGPS_FreeGpsOpen()
{
	if( g_pGpsDataCol )
	{
		delete g_pGpsDataCol;
		g_pGpsDataCol = NULL;
	}
}
GPSOPEN_API nuBOOL LibGPS_ReSet( PGPSCFG pGpsSetData)
{
	
	if ( pGpsSetData == NULL )
	{
		return LibGPS_Open();
	}
	else
	{
		nuLONG nGpsMode = 0;
		nuLONG nGYROMODE = 0;
		COMCFG ComCfg = {0};
		//COMM_CONFIG ECOSComCfg ={0};
		ComCfg.GPSBaudrate = pGpsSetData->nGPSBaudrate;
		ComCfg.GPSCommPort = pGpsSetData->nComport;
		//
		nuTCHAR sFilePath[NURO_MAX_PATH] = {0};
		#ifndef ANDROID		
		GetModuleFileName(NULL, sFilePath, NURO_MAX_PATH);
		#else
		nuGetModulePathA(NULL, sFilePath, NURO_MAX_PATH);
		#endif
		for(nuINT i = nuTcslen(sFilePath) - 1; i >= 0; i--)
		{
			if( sFilePath[i] == nuTEXT('\\') )
			{
				sFilePath[i+1] = nuTEXT('\0');
				break;
			}
		}
		LibGPS_Close();
		g_nGPSLogMode = (nuBYTE)pGpsSetData->nGPSLogMode ;
		nGpsMode = pGpsSetData->nGPSOpenMode;

		if(g_SourceType == GPS_OPEN_FILE)
		{
			nuTCHAR sSourceFileName[NURO_MAX_PATH];
			nuMemset( sSourceFileName, 0, sizeof(sSourceFileName) );
			nuTcscat( sSourceFileName, sFilePath);
			nuTcscat( sSourceFileName, nuTEXT("GPSSOURCE.txt"));
			g_hGps.pFile = nuTfopen( sSourceFileName, NURO_FS_RB );
			if(g_hGps.pFile == NULL)
			{
				return nuFALSE;
			}
			nuFseek(g_hGps.pFile, 0, NURO_SEEK_SET);
		}
		else
		{
			switch(nGpsMode)
			{
				case GPS_OPEN_ELEAD_3DR:
				case GPS_OPEN_ROYALTEK_3DR:
				case GPS_OPEN_ROYALTEK_ANDROID: 
				case GPS_OPEN_JNI:  //ID=32 NMEA資????????輸??
				case GPS_OPEN_JNI_2://ID=34 鼎天 JNI 純GPS
					g_hGps.pComm = new CCommportCtrl();
					if( g_hGps.pComm == NULL )
					{
						return nuFALSE;
					}
				break;
				case GPS_OPEN_COMM:
				case GPS_OPEN_LINUX:
				case GPS_OPEN_SIMPNEMA:
				case GPS_OPEN_SDC_3DR:
				case GPS_OPEN_NORMALMODE:
				case GPS_OPEN_PANA_LEA6R:
				case GPS_OPEN_SIRF_BINARY:
				case GPS_OPEN_POWERNAVI:
					g_hGps.pComm = new CCommportCtrl();
					if( g_hGps.pComm == NULL )
					{
						return nuFALSE;
					}
					if( g_hGps.pComm->InitComPort( &ComCfg ) == NURO_INVALID_HANDLE_VALUE )
					{
						delete g_hGps.pComm;
						g_hGps.pComm = NULL;
						return nuFALSE;
					}
					break;
				case GPS_OPEN_UCOS:
					//if ( !nuReadConfigValue("GYROMODE",(nuLONG *)( &nGYROMODE)) )
					//{
					//	nGYROMODE = GYRO;
					//}
					//g_hGps.hUcos = nuOpenGPS(nGYROMODE);
					break;
				case GPS_OPEN_WOOJOY:
				case GPS_OPEN_ECOS:
					//ECOSComCfg.nBaudRate = pGpsSetData->nGPSBaudrate;
					//g_hGps.hUcos = nuOpenComm( &ECOSComCfg );
					break;
				case GPS_OPEN_PS:
					g_hGps.pComm = new CCommportCtrl();
					if( g_hGps.pComm == NULL )
					{
						return nuFALSE;
					}
					if( g_hGps.pComm->InitComPortPS( &ComCfg ) == NURO_INVALID_HANDLE_VALUE )
					{
						delete g_hGps.pComm;
						g_hGps.pComm = NULL;
						return nuFALSE;
					}
					break;
				case GPS_OPEN_HV:
					g_hGps.pFile = (nuFILE *)0xFF;
					break;
				default:
					return nuFALSE;
			}
		}

		g_openType = (nuBYTE)nGpsMode;
		if ( g_nGPSLogMode == GPS_LOG_ALL || g_nGPSLogMode == GPS_LOG_PART )
		{
			CGpsLogFileZ logfile;
			logfile.InitFile(0);
		}
		return nuTRUE;
	}
}

GPSOPEN_API nuBOOL LibGPS_Open()
{
	nuLONG	nGpsMode = 0;
	nuLONG	nSOURCETYPE = 0;
	nuLONG	nGYROMODE = 0;
	nuLONG	nComBaudrate = 0;
//	COMM_CONFIG ComCfg ={0};

	//
	nuTCHAR sFilePath[NURO_MAX_PATH];
	#ifndef ANDROID
	GetModuleFileName(NULL, sFilePath, NURO_MAX_PATH);
	#else
	nuGetModulePathA(NULL, sFilePath, NURO_MAX_PATH);	
	#endif
	
	for(nuINT i = nuTcslen(sFilePath) - 1; i >= 0; i--)
	{
		if( sFilePath[i] == nuTEXT('\\') )
		{
			sFilePath[i+1] = nuTEXT('\0');
			break;
		}
	}
	LibGPS_Close();
	if( !nuReadConfigValue("SOURCETYPE", &nSOURCETYPE) )
	{
		nSOURCETYPE = GPS_OPEN_CLOSE;
	}

	if( !nuReadConfigValue("GPSMODE", &nGpsMode) )
	{
		nGpsMode = GPS_OPEN_COMM;
	}
	#ifdef ANDROID
		if (nGpsMode == GPS_OPEN_COMM )
		{
			nGpsMode = GPS_OPEN_JNI;
		}
	#endif
	if(nGpsMode==GPS_OPEN_SIRF_BINARY)
	{
		GetPanaRegistry(&nGpsMode);
	}
	nuLONG nTemp;
	if ( !nuReadConfigValue("GPSLOGMODE",&nTemp) )
	{
		nTemp = 0;
	}
	g_nGPSLogMode = (nuBYTE)nTemp;

	if ( !nuReadConfigValue("GPSLOGRESATRT",&nTemp) )
	{
		nTemp = 0;
	}

	if ( g_nGPSLogMode == GPS_LOG_ALL || g_nGPSLogMode == GPS_LOG_PART )
    {
		CGpsLogFileZ logfile;
		if(nTemp==0)
		{
			logfile.InitFile(nuFALSE);
		}
		else
		{
			logfile.InitFile(nuTRUE);
		}
    }
	g_SourceType = (nuBYTE)nSOURCETYPE;
	#ifdef ANDROID
	if(g_SourceType == GPS_OPEN_FILE)
	{
		nuMemset( sSourceFileName, 0, sizeof(sSourceFileName) );
		nuTcscat( sSourceFileName, sFilePath);
		nuTcscat( sSourceFileName, nuTEXT("GPSSOURCE.txt"));		
		FILE *fp = fopen(sSourceFileName,  "r");
		if(fp == NULL)
		{
			g_SourceType = 0;
		}	
		else
		{
			fclose(fp);			
		}
	}
	#endif		
	if(g_SourceType == GPS_OPEN_FILE)
	{
		nuMemset( sSourceFileName, 0, sizeof(sSourceFileName) );
		nuTcscat( sSourceFileName, sFilePath);
		nuTcscat( sSourceFileName, nuTEXT("GPSSOURCE.txt"));

		g_hGps.pFile = nuTfopen( sSourceFileName, NURO_FS_RB );
		if(g_hGps.pFile == NULL)
		{
			return nuFALSE;
		}
		nuFseek(g_hGps.pFile, 0, NURO_SEEK_SET);
	}
	else
	{
		switch(nGpsMode)
		{
			case GPS_OPEN_ELEAD_3DR:
			case GPS_OPEN_ROYALTEK_3DR:
			case GPS_OPEN_ROYALTEK_ANDROID:
			case GPS_OPEN_JNI://ID=32 NMEA資????????輸??
			case GPS_OPEN_JNI_2:
			g_hGps.pComm = new CCommportCtrl();
			if( g_hGps.pComm == NULL )
			{
				return nuFALSE;
			}
			break;
			case GPS_OPEN_COMM:
			case GPS_OPEN_LINUX:
			case GPS_OPEN_SIMPNEMA:
			case GPS_OPEN_SDC_3DR:
			case GPS_OPEN_NORMALMODE:
			case GPS_OPEN_PANA_LEA6R:
			case GPS_OPEN_SIRF_BINARY:
			case GPS_OPEN_POWERNAVI:
				g_hGps.pComm = new CCommportCtrl();
				if( g_hGps.pComm == NULL )
				{
					return nuFALSE;
				}
				if( g_hGps.pComm->InitComPort() == NURO_INVALID_HANDLE_VALUE )
				{
					delete g_hGps.pComm;
					g_hGps.pComm = NULL;
					return nuFALSE;
				}
				break;
			case GPS_OPEN_UCOS:
				//if ( !nuReadConfigValue("GYROMODE",(nuLONG *)( &nGYROMODE)) )
				//{
				//	nGYROMODE = GYRO;
				//}
				//g_hGps.hUcos = nuOpenGPS(nGYROMODE);
				break;
			case GPS_OPEN_WOOJOY:
			case GPS_OPEN_ECOS:
				//nComBaudrate = 0;
				//if(  !nuReadConfigValue("COMBAUDRATE", &nComBaudrate))
				//{
				//	nComBaudrate = 9600;
				//}
				//ComCfg.nBaudRate = nComBaudrate;
				//g_hGps.hUcos = nuOpenComm( &ComCfg );
				break;
			case GPS_OPEN_PS:
				g_hGps.pComm = new CCommportCtrl();
				if( g_hGps.pComm == NULL )
				{
					return nuFALSE;
				}
				if( g_hGps.pComm->InitComPortPS() == NURO_INVALID_HANDLE_VALUE )
				{
					delete g_hGps.pComm;
					g_hGps.pComm = NULL;
					return nuFALSE;
				}
				break;	
			case GPS_OPEN_HV:
				g_hGps.pFile = (nuFILE *)0xFF;
				break;
			default:
				return nuFALSE;
		}
	}
	
	g_openType = (nuBYTE)nGpsMode;
	b_GPSOpenWorking=nuTRUE;

	return nuTRUE;
}
GPSOPEN_API nuBOOL LibGPS_Close()
{
	b_GPSOpenWorking=nuFALSE;
	nuSleep(1);
	nuSleep(1);
	nuSleep(1);
	nuSleep(1);
	nuSleep(1);
	if(g_SourceType == GPS_OPEN_FILE)
	{
		if( g_hGps.pFile )
		{
			nuFclose( g_hGps.pFile );
			g_hGps.pFile = NULL;
		}
	}
	else
	{
		switch(g_openType)
		{
			case GPS_OPEN_JNI://ID=32 NMEA資????????輸??
			case GPS_OPEN_JNI_2:
			case GPS_OPEN_COMM:
			case GPS_OPEN_LINUX:
			case GPS_OPEN_SIMPNEMA:
			case GPS_OPEN_SDC_3DR:
			case GPS_OPEN_ELEAD_3DR:
			case GPS_OPEN_ROYALTEK_3DR:
			case GPS_OPEN_NORMALMODE:
			case GPS_OPEN_PANA_LEA6R:
			case GPS_OPEN_SIRF_BINARY:
			case GPS_OPEN_POWERNAVI:
			case GPS_OPEN_ROYALTEK_ANDROID:	
				if( g_hGps.pComm )
				{
					g_hGps.pComm->ResetClass();
					delete g_hGps.pComm;
					g_hGps.pComm = NULL;
				}
				break;
			case GPS_OPEN_UCOS:
				//if( g_hGps.hUcos != NULL )
				//{
				//	nuCloseGPS(g_hGps.hUcos);
				//	g_hGps.hUcos = NULL;
				//}
				break;
			case GPS_OPEN_WOOJOY:
			case GPS_OPEN_ECOS:
				//if ( g_hGps.hUcos != NULL )
				//{
				//	nuCloseComm( g_hGps.hUcos ); 
				//}
				break;
			case GPS_OPEN_PS:
				if( g_hGps.pComm )
				{
					g_hGps.pComm->ResetClass();
					delete g_hGps.pComm;
					g_hGps.pComm = NULL;
				}
				break;
			case GPS_OPEN_HV:
				break;
			default:
				return nuFALSE;
		}
	}

	g_SourceType = GPS_OPEN_CLOSE;
	g_openType = GPS_OPEN_CLOSE;
	g_pGpsDataCol->g_bHaveNewData	= nuFALSE;
	return nuTRUE;
}

#define BUF_LENGTH	1024
GPSOPEN_API nuBOOL LibGPS_RecvData( const nuCHAR *pBuff, nuINT len )
{
	static nuUINT tmptickcount=0;
	nuBYTE  ReceiveCharBuf[BUF_LENGTH] = {0}; 
	nuDWORD ReceiveCharBufNum = 0;
	nuMemset(ReceiveCharBuf, 0 ,BUF_LENGTH );
	ReceiveCharBufNum=0;

	if(b_GPSOpenWorking==nuFALSE)	return nuFALSE;
	//確認資料來源是否ok
	if( g_hGps.pComm == NULL )
	{
		if(g_SourceType == GPS_OPEN_FILE)
		{
			g_hGps.pFile = nuTfopen( sSourceFileName, NURO_FS_RB );
			if(g_hGps.pFile == NULL)
			{
				return nuFALSE;
			}
		}
		return nuFALSE;
	}

	//接收資料的處理
	if(g_SourceType == GPS_OPEN_FILE)
	{
		g_nGPSLogReadSize=((g_openType==GPS_OPEN_ELEAD_3DR || g_openType==GPS_OPEN_ELEAD_GPS || g_openType==GPS_OPEN_EL)?sizeof(GYROGPSDATA_EL):g_nGPSLogReadSize);
		ReceiveCharBufNum = nuFread(ReceiveCharBuf, 1, g_nGPSLogReadSize, g_hGps.pFile);

		if ( ReceiveCharBufNum < g_nGPSLogReadSize && ( nuFtell(g_hGps.pFile) == nuFgetlen(g_hGps.pFile) ))
		{
			nuFclose(g_hGps.pFile);
			g_hGps.pFile=NULL;
			g_hGps.pFile = nuTfopen( sSourceFileName, NURO_FS_RB );
			if(g_hGps.pFile == NULL)
			{
				return nuFALSE;
			}
			nuFseek( g_hGps.pFile, 0, NURO_SEEK_SET );
		}
		if( ReceiveCharBufNum == 0 )
		{
			return nuFALSE;
		}
	}
	else
	{
		switch(g_openType) 
		{
			case GPS_OPEN_COMM:
			case GPS_OPEN_LINUX:
			case GPS_OPEN_SIMPNEMA:
			case GPS_OPEN_SDC_3DR:
			case GPS_OPEN_NORMALMODE:
			case GPS_OPEN_PANA_LEA6R:
			case GPS_OPEN_SIRF_BINARY:
			case GPS_OPEN_PS:
			case GPS_OPEN_POWERNAVI:
				if( !g_hGps.pComm->GetBuff(ReceiveCharBuf, BUF_LENGTH, &ReceiveCharBufNum) )//If the function fails, the return value is zero
				{
					return nuFALSE;
				}		
				break;
			case GPS_OPEN_UCOS:
				if( pBuff != NULL )
				{
					nuStrncat((nuCHAR*)ReceiveCharBuf, pBuff, len);
					ReceiveCharBufNum = len;
				}
				break;			
			
			case GPS_OPEN_JNI://ID=32 NMEA資????????輸??
			case GPS_OPEN_JNI_2:
			case GPS_OPEN_ROYALTEK_ANDROID:	
			case GPS_OPEN_ROYALTEK_3DR:
			{
				#ifdef ANDROID_GPS
				ReceiveCharBufNum = PopJNI(ReceiveCharBuf);	
				#endif				
			}
				break;
			case GPS_OPEN_WOOJOY:
			case GPS_OPEN_ECOS:
				//ReceiveCharBufNum = nuReadComm( g_hGps.hUcos, (nuCHAR *)ReceiveCharBuf, BUF_LENGTH );
				break;
			case GPS_OPEN_HV:
				//ReceiveCharBufNum = nuReadComm( NULL,(nuCHAR *)ReceiveCharBuf, BUF_LENGTH );
			case GPS_OPEN_EL:
				break;
			default:
				return nuFALSE;
		}
		if( ReceiveCharBufNum == 0 )
		{
			return nuFALSE;
		}
	}

	if (  g_nGPSLogMode == GPS_LOG_ALL )
	{
		CGpsLogFileZ LogFile;
		LogFile.AppendGpsData( (nuCHAR *)ReceiveCharBuf,ReceiveCharBufNum );
	}

	switch(g_openType)
	{
		case GPS_OPEN_SIMPNEMA:
			break;
		case GPS_OPEN_SIRF_BINARY:
			if (g_pGpsDataCol->ColPanaBinary(ReceiveCharBuf,ReceiveCharBufNum,g_openType)==-1)
			{
				 if(g_SourceType != GPS_OPEN_FILE && g_hGps.pComm)
				 {
					g_hGps.pComm->WriteBinaryReset();
				 }
			}
			break;
		case GPS_OPEN_EL:
		case GPS_OPEN_ELEAD_GPS:
			g_pGpsDataCol->Col_ELeadData((GYROGPSDATA_EL*)ReceiveCharBuf);
			break;
		default:
			if(g_pGpsDataCol->NewConvertNewData(ReceiveCharBuf, ReceiveCharBufNum,g_openType))
			{
				g_pGpsDataCol->g_bHaveNewData = nuTRUE; 
			}
			break;
	}
	return g_pGpsDataCol->g_bHaveNewData;
}


GPSOPEN_API nuBOOL LibGPS_GetNewData(PGPSDATA* ppGpsDat)
{
	static nuLONG FixCount=0;
	static nuLONG lTickCount=0;
	nuLONG nowLen = 0,LatEffect = 0,CaseType = 0;
	NUROPOINT temppt = {0};
	nuLONG Sht_X = 0,Sht_Y = 0;
	if(b_GPSOpenWorking==nuFALSE)	return nuFALSE;

	nuINT t1 = nuGetTickCount();
	if( !g_pGpsDataCol->g_bHaveNewData )
	{
		if( NURO_ABS(t1-lTickCount)>4000)
		{
			nuMemcpy(&g_pGpsDataCol->m_GyroBackData,&g_pGpsDataCol->m_BackData,sizeof(GPSDATA));
			g_pGpsDataCol->m_GyroBackData.nStatus	=	DATA_TYPE_INVALID;
			*ppGpsDat		= &g_pGpsDataCol->m_GyroBackData;
			return nuTRUE;
		}
		return nuFALSE;
	}
	lTickCount=t1;

	nuMemcpy(&g_pGpsDataCol->m_GyroBackData,&g_pGpsDataCol->m_BackData,sizeof(GPSDATA));
	if(g_openType==GPS_OPEN_COMM && g_pGpsDataCol->m_GyroBackData.nStatus!=DATA_TYPE_GPS) 
	{
		g_pGpsDataCol->m_GyroBackData.nStatus=DATA_TYPE_INVALID;
		*ppGpsDat		= &g_pGpsDataCol->m_GyroBackData;
		ChkFirstGpsOn();//Kris add @2012-11-21
		g_pGpsDataCol->g_bHaveNewData	= nuFALSE;
		return nuTRUE;//nuFALSE;
	}
	else
	{
		nowLen	=	g_nBaseShiftDis+((g_nSpeed36ShiftDis*g_pGpsDataCol->m_GyroBackData.nSpeed)/GPS_FIX_MIN_SPEED); //以比例來還原情況
		if( nowLen > 0 && g_pGpsDataCol->m_GyroBackData.nSpeed>0 ){
			if(LatEffect==0)
			{
				Sht_X = nowLen*nuCos(g_pGpsDataCol->m_GyroBackData.nAngle)*EFFECTBASE;
				Sht_Y = nowLen*nuSin(g_pGpsDataCol->m_GyroBackData.nAngle)*EFFECTBASE;
				g_pGpsDataCol->m_GyroBackData.nLongitude += Sht_X/LONEFFECTCONST;
				g_pGpsDataCol->m_GyroBackData.nLatitude += Sht_Y/LATEFFECTCONST;
			}
			else
			{
				Sht_X = nowLen*nuCos(g_pGpsDataCol->m_GyroBackData.nAngle)*EFFECTBASE;
				Sht_Y = nowLen*nuSin(g_pGpsDataCol->m_GyroBackData.nAngle)*EFFECTBASE;
				g_pGpsDataCol->m_GyroBackData.nLongitude += Sht_X*EFFECTBASE/(LatEffect*LONEFFECTCONST);
				g_pGpsDataCol->m_GyroBackData.nLatitude += Sht_Y/LATEFFECTCONST;
			}
		}
	}
	*ppGpsDat		= &g_pGpsDataCol->m_GyroBackData;
	ChkFirstGpsOn();//Kris add @2012-11-21
	g_pGpsDataCol->g_bHaveNewData	= nuFALSE;
	return nuTRUE;
}

GPSOPEN_API nuBOOL LibGPS_GetNewData2(PGPSDATA* ppGpsDat)
{
	return LibGPS_GetNewData(ppGpsDat);
}


GPSOPEN_API nuBOOL LibGPS_GetGpsXY(nuLONG *pX, nuLONG *pY)
{
	if( g_pGpsDataCol->m_BackData.nStatus )
	{
		*pX = g_pGpsDataCol->m_BackData.nLongitude;
		*pY = g_pGpsDataCol->m_BackData.nLatitude;
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}
GPSOPEN_API nuBYTE LibGPS_GetState()
{
	return g_pGpsDataCol->m_BackData.nStatus;
}

//nRoadx,nRoady:座標點 , dx,dy:角度 , nSpeed:速度 , nTime:保留位 尚未使用
GPSOPEN_API nuVOID  LibGPS_FeedBackInfo(nuLONG nRoadx, nuLONG nRoady, nuSHORT dx, nuSHORT dy, nuSHORT nSpeed, nuINT nTime)
{
	g_pGpsDataCol->GPSFeedBack_Speed(nRoadx, nRoady, dx, dy, nSpeed, nTime);
}

GPSOPEN_API nuVOID LibGPS_FeedBack(const FEEDBACKDATA* pFeedBackInfo)
{
	g_pGpsDataCol->GPSFeedBack(pFeedBackInfo);
}

/**/
//nLon(冪僅) ,nLat(帠僅) ,nHeight(漆匿詢僅)腔等弇飲岆譙ㄛtimeRecv岆麼氪森棒劑怹釴梓腔奀潔﹝
GPSOPEN_API nuBOOL  LibGPS_FIXPOINT( nuUINT &nLon, nuUINT &nLat,const nuINT &nHeight,const NUROTIME &timeRecv )
{
	return nuTRUE;//g_pGpsDataCol->FixToChina( nLon, nLat, nHeight, timeRecv );
}

GPSOPEN_API nuVOID LibGPS_SetGPSChina(nuPVOID lpFunc)
{
	CGpsEstimateZ::s_pWgToChina = (GPS_WgToChinaProc)lpFunc;
}


GPSOPEN_API nuVOID LibGPS_TOBEDEBUGINFO(PGPSMODINFO* ppGpsModInfo)
{
	nuLONG RUNCount=0;
	RUNCount=0;
	g_pGpsDataCol->b_updateModInfo=nuTRUE;
	*ppGpsModInfo	= &g_pGpsDataCol->m_GpsModInfo;
}


GPSOPEN_API nuVOID LibGPS_TOBERESETGYRO()
{
	if(  g_SourceType != GPS_OPEN_FILE && g_hGps.pComm )
	{
		g_hGps.pComm->WriteBinaryReset();
		g_hGps.pComm->WriteBuff(5);//Reset Gyro
		g_hGps.pComm->WriteBuff(4);//設定系統輸出EKFSTATUS
	}
}

GPSOPEN_API nuVOID LibGPS_TOBEGETVERSION()
{
	if(  g_SourceType != GPS_OPEN_FILE && g_hGps.pComm )
	{
		g_hGps.pComm->WriteBuff(3);//Get Gyro Version
	}
}

GPSOPEN_API nuVOID LibGPS_GetRawData(PGPSDATA pGpsData)//added by daniel @20120204
{	
	nuMemcpy(pGpsData, &g_pGpsDataCol->m_GPSRawInfo, sizeof(GPSDATA));//og_pGpsDataCol->m_GyroBackData
}

nuVOID GetPanaRegistry(nuLONG* nGpsMode)
{
#ifndef _WIN32_WCE
	return;
#endif
#ifndef ANDROID
	TCHAR *Str = NULL;
	nuDWORD Result;
	HKEY hKey;
	nuDWORD NumBytes = 0;
	nuDWORD Type;
	//HANDLE UserEvent = INVALID_HANDLE_VALUE;
 
	// Open the Registry Key
//================
	TCHAR   tKey[255];
	nuMemset(&tKey, NULL, sizeof(tKey));
    nuTcscpy(tKey, nuTEXT("Platform\\VerInfo"));
	Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, tKey, 0, 0, &hKey);


	if( ERROR_SUCCESS == Result )
	{
		nuTcscpy(tKey, nuTEXT("GPS")); 
		Result = RegQueryValueEx( hKey, tKey, NULL, &Type, NULL, &NumBytes );
		if( NumBytes > 0 )
		{
			// Now we know how big the string is allocate and read it
			Str = (TCHAR *)malloc( NumBytes );
			if( Str != NULL )	
				Result = RegQueryValueEx( hKey, tKey, NULL, &Type, (LPBYTE)Str, &NumBytes );

			if ((Str[0]=='N') && (Str[1]=='o') && (Str[2]=='w') && (Str[3]==' ') && (Str[4]=='i') && (Str[5]=='s') && (Str[6]==' ') && (Str[7]=='6') && (Str[8]=='R'))
			{
				*nGpsMode = GPS_OPEN_PANA_LEA6R;
			}
		}
		RegCloseKey( hKey );
 		free( Str );
	}
#endif
    return;
}

GPSOPEN_API nuBOOL LibGPS_GetGPSComConfig(nuLONG& GPSCommPort, nuLONG& GPSBaudrate) //Added by Damon 20120705 
{
	if(b_GPSOpenWorking==nuFALSE)
	{
		return nuFALSE;
	}
	if( g_hGps.pComm == NULL )
	{
		return nuFALSE;
	}
	g_hGps.pComm->GetGPSComConfig(GPSCommPort, GPSBaudrate);
	return nuTRUE;
}

GPSOPEN_API nuBOOL LibGPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len )
{
	#ifdef ANDROID_GPS
	if(len<1)
	{
		return nuFALSE;
	}
	len = nuStrlen(pBuff);
	if(nPopIdx<127)
	{
		nuMemset(sbuffer[nPopIdx+1],0,ANDROID_GPS_LEN);
		nuMemcpy(sbuffer[nPopIdx+1],pBuff,len);
		nPopIdx++;
		//nPushIdx++;
//		__android_log_print(ANDROID_LOG_INFO, "JNIMsg2", "%d XXX ",nPopIdx);
	}
	else
	{
		nuMemset(sbuffer[0],0,ANDROID_GPS_LEN);
		nuMemcpy(sbuffer[0],pBuff,len);
		nPopIdx=0;
		//nPushIdx=0;

	}
	//nuMemset(sbuffer[nPopIdx],0,ANDROID_GPS_LEN);
	//slen=len;
	//nuMemcpy(sbuffer[nPopIdx],pBuff,len);
	

		
#endif	
	return nuTRUE;
}

nuBOOL ChkFirstGpsOn()//Kris add @2012-11-21
{
	if((!g_FirstGpsOn))
	{
		if((g_pGpsDataCol->g_bHaveNewData)&&(g_pGpsDataCol->m_GyroBackData.nStatus==DATA_TYPE_GPS))
		{
			g_FirstGpsOn = nuTRUE;
			nuLONG lOldCode = 0;
			nuINT  iOldTimes = 0;
			nuINT  iExpiration = 0;
			ReadGFF(&lOldCode, &iOldTimes,&iExpiration);

			nuBYTE StrDayHour[NURO_MAX_PATH] = {0},StrTimes[NURO_MAX_PATH] = {0}, StrExpiration[NURO_MAX_PATH] = {0};
			nuMemset(StrDayHour,0,sizeof(StrDayHour));
			nuINT iYear		= g_pGpsDataCol->m_GyroBackData.nTime.nYear;
			nuINT iMomth	= g_pGpsDataCol->m_GyroBackData.nTime.nMonth;
			nuINT iDay		= g_pGpsDataCol->m_GyroBackData.nTime.nDay;
			nuINT iHour		= g_pGpsDataCol->m_GyroBackData.nTime.nHour;
			nuLONG lNowCode	= iDay*100000000+iMomth*1000000+(iYear+2000)*100+iHour;
			nuItoa(lNowCode,(nuCHAR *)StrDayHour,16);

			if((iOldTimes>=10)||(lNowCode==lOldCode))	return nuFALSE;//紀錄10次之後 就視為已在正常使用,保固日正式起算
			if(g_SourceType == GPS_OPEN_FILE)	return nuFALSE;//若使用GPSSOURCETYPE = 1,不會讓SaveGFF作用
			
			//當前次數
			nuMemset(StrTimes,0,sizeof(StrTimes));
			iOldTimes++;
			nuItoa(iOldTimes,(nuCHAR *)StrTimes,16);

			//有效月數
			nuMemset(StrExpiration,0,sizeof(StrExpiration));
			nuItoa(iExpiration,(nuCHAR *)StrExpiration,16);

			if(nuStrlen((const nuCHAR*)StrExpiration)==1)
			{
				nuStrcat((nuCHAR*)StrTimes,"0");
			}
			nuStrcat((nuCHAR*)StrTimes,(const nuCHAR*)StrExpiration);
			
			if(nuStrlen((const nuCHAR *)StrDayHour)==7)//補齊偶數位前置0
			{
				nuStrcat((nuCHAR*)StrTimes,"0");
			}
			nuStrcat((nuCHAR*)StrTimes,(const nuCHAR*)StrDayHour);
			
			SaveGFF((nuCHAR*)StrTimes,nuStrlen((const nuCHAR*)StrTimes));
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuBOOL ReadGFF(nuLONG* iDayHour,nuINT* iTimes, nuINT* iExpiration)		//Kris add @2012-11-21
{
	nuFILE *pFile = NULL;
	nuTCHAR filename[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, filename, NURO_MAX_PATH);
	nuTcscat(filename, nuTEXT("setting\\GFF.dat"));//GpsFirstFix.dat
	pFile = nuTfopen(filename, NURO_FS_OB);
	if(pFile == NULL)
	{
		return nuFALSE;
	}
	nuBYTE  tmp[BUF_LENGTH]={0}; 
	nuDWORD tmpNum = 0;
	nuMemset(tmp, 0 ,sizeof(tmp));
	tmpNum = nuFread(tmp, sizeof(nuBYTE), BUF_LENGTH, pFile);
	for(nuINT i=0;i<11;i++)
	{
		nuUCHAR hex=tmp[i];
		if(hex>='a')
		{
			hex=hex-'a'+10;
		}
		if(hex>='A')
		{
			hex=hex-'A'+10;
		}
		if(hex>='0')
		{
			hex=hex-'0';
		}
		if(i==0)
		{
			*iTimes = hex;
		}
		if((i>=1)&&(i<=2))
		{
			*iExpiration = *iExpiration*16+hex;
		}
		if(i>=3)
		{
			*iDayHour = *iDayHour*16+hex;
		}
	}
	nuFclose(pFile); 
	return nuTRUE;
}


nuBOOL SaveGFF(nuCHAR xmm[NURO_MAX_PATH],nuINT Len)//Kris add @2012-11-21
{
	nuFILE *pFile = NULL;
	nuTCHAR filename[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, filename, NURO_MAX_PATH);
	nuTcscat(filename, nuTEXT("setting\\GFF.dat"));//GpsFirstFix.dat
	pFile = nuTfopen(filename, NURO_FS_WB);

	if(pFile == NULL)
	{
		return nuFALSE;
	}
					
	nuBYTE iCheckSum = 0;
	for(nuINT j=0;j<Len;j++)
	{
		iCheckSum=iCheckSum^xmm[j];
	}

	nuBYTE StrChksum[3] = {0,0,0};
	nuMemset(StrChksum,0,sizeof(StrChksum));
	nuItoa(iCheckSum,(nuCHAR *)StrChksum,16);

	if(nuStrlen((const nuCHAR*)StrChksum)==1)
	{
		StrChksum[1] = StrChksum[0];
		StrChksum[0] = '0';
	}
	
	nuFwrite(xmm, Len, 1, pFile);
	nuFwrite(StrChksum, 2, 1, pFile);

	nuFclose(pFile);
	return nuTRUE;
}

GPSOPEN_API nuBOOL LibGPS_ELEAD_TRAN(const GYROGPSDATA_EL* pEL_GyroData)//ELEAD data transter怡利資料轉換
{
	if (( g_nGPSLogMode == 1 || g_nGPSLogMode == 2 )&&(g_SourceType != GPS_OPEN_FILE))
	{
		CGpsLogFileZ LogFile;
		LogFile.AppendGpsData( (nuCHAR *)pEL_GyroData,sizeof(GYROGPSDATA_EL) );
	}

	b_GPSOpenWorking=nuTRUE;
	g_pGpsDataCol->Col_ELeadData(pEL_GyroData);
	g_pGpsDataCol->g_bHaveNewData=nuTRUE;
//	nuMemcpy(&g_pGpsDataCol->m_GPSRawInfo, &g_pGpsDataCol->m_SaveInfo, sizeof(GPSDATA));
	return nuTRUE;
}
GPSOPEN_API nuVOID LibGPS_WriteCom(nuBYTE *Data, nuLONG nLen)
{
	if(b_GPSOpenWorking==nuFALSE)
	{
		return ;
	}
	if( g_SourceType != GPS_OPEN_FILE && g_hGps.pComm)
	{
		g_hGps.pComm->WriteCom(Data, nLen);
	}
}

