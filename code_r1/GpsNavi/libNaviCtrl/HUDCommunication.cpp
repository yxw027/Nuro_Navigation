// CommportCtrl.cpp: implementation of the CCommportCtrl class.
//
//////////////////////////////////////////////////////////////////////

//#include "nuApiDevice.h"
#include "HUDCommunication.h"
#include "NuroModuleApiStruct.h"
#include "NuroNaviConst.h"
extern FILESYSAPI		g_fileSysApi;
#ifndef ANDROID
#ifdef NURO_OS_WINCE
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#endif
#define MaxComportNum	12
#define MaxBRType		6
#define NURO_MAXDWORD    0xFFFFFFFF 
nuLONG BRNumber[]={2440, 4800, 9600, 14400, 19200, 38400};

#endif
//long BRNumber[]={38400,19200,9600,4800};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef NURO_OS_WINCE
//#define ELEAD_DEBUG
#endif
#ifndef NURO_OS_WINCE
typedef struct tagCOPYDATASTRUCT {
    nuULONG dwData;
    nuDWORD cbData;
    nuPVOID lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;
#endif
CHUDCOMCTRL::CHUDCOMCTRL()
{
	m_hComPort = NURO_INVALID_HANDLE_VALUE;
	m_lHUDType = 0;
	if( !nuReadConfigValue("HUDTYPE", &m_lHUDType) )
	{
		m_lHUDType = 0;
	}
#ifdef NURO_OS_WINCE
	m_hwndPana = FindWindow(NULL,_T("V850_HMI"));
#endif
	m_nTmpFlag = 0;
	m_nNaviState = -1;
	m_ELEAD_nMapIdx  = -1;
	m_ELEAD_nBlkIdx  = -1;
	m_ELEAD_nRoadIdx = -1;
	m_ELEAD_State_IconType = 0;
	m_ELEAD_State_Speed = 0;
}

CHUDCOMCTRL::~CHUDCOMCTRL()
{
}
nuINT CHUDCOMCTRL::bOpenCom(const nuTCHAR *tsPath)
{
#ifndef ANDROID
	m_bInitialOK			= nuFALSE;
	NURO_DCB			dcb = {0};
    NURO_COMMTIMEOUTS	cto = {0};
	nuLONG				nComPortIdx = 0;
	nuLONG				nBRIdx = 0;//BaudRate
	nuLONG				GPSCommPort = 0;
	nuLONG				GPSBaudrate = 0;
	TCHAR GPS_COMPORT[10] = {0};

	if ( !nuReadConfigValue("HUDCOMPORT", &(GPSCommPort)) )
	{
		GPSCommPort = 7;
	}
	if(  !nuReadConfigValue("HUDCOMBAUDRATE", &(GPSBaudrate)))
	{
		GPSBaudrate = 9600;
	}
	if( !nuReadConfigValue("HUDTYPE", &m_lHUDType) )
	{
		m_lHUDType = 0;
	}

	if(3 == m_lHUDType)
	{
		m_bInitialOK = m_LoadDLL_NECVOX.Initial(tsPath);
		return (nuINT)m_bInitialOK;
	}
	nuMemset( GPS_COMPORT, 0, sizeof(GPS_COMPORT) );
	nuTcscat(GPS_COMPORT,nuTEXT("COM"));
	//
	if(m_hComPort != NURO_INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComPort);
		m_hComPort = NURO_INVALID_HANDLE_VALUE;
	}
	//
	nComPortIdx	= GPSCommPort;
	if( nComPortIdx != 0 )
	{
		nuItot(nComPortIdx, &GPS_COMPORT[3], 10);
		#ifdef NURO_OS_WINCE
		nuTcscat(GPS_COMPORT,nuTEXT(":"));	
		#endif

		m_hComPort = (nuHANDLE)CreateFile( GPS_COMPORT , NURO_GENERIC_READ| NURO_GENERIC_WRITE , 0, 
			NULL, NURO_OPEN_EXISTING,  NURO_FILE_ATTRIBUTE_SYSTEM, NULL);

		if (m_hComPort == NURO_INVALID_HANDLE_VALUE)
		{	
			return HANDLE_INVALID;
		}
	}
	
	if( m_hComPort == NURO_INVALID_HANDLE_VALUE )
	{
		return HANDLE_INVALID;
	}

	nBRIdx = 0;
	if( GetCommState ( m_hComPort, (LPDCB)&dcb ) == nuFALSE )
	{
		return GETCOMMSATE_FAIL;
	}
	dcb.DCBlength = sizeof (NURO_DCB);
	if(GPSBaudrate==0)	
	{
		nBRIdx++;
	}
	else								
	{
		dcb.BaudRate	= GPSBaudrate;
	}
	dcb.ByteSize	= 8;
	dcb.Parity		= NURO_NOPARITY; 
	dcb.StopBits	= NURO_ONESTOPBIT;
	
	if ( SetCommState (m_hComPort,(LPDCB) &dcb) == nuFALSE)
	{
		return SETCOMMSTATE_FAIL;
	}

	if( GetCommTimeouts(m_hComPort, (LPCOMMTIMEOUTS)&cto) == nuFALSE )
	{
		return GETCOMMTIMEOUT_FAIL;
	}
	cto.ReadIntervalTimeout			= 0;
	cto.ReadTotalTimeoutMultiplier	= 0;
	cto.ReadTotalTimeoutConstant	= 0;
	cto.WriteTotalTimeoutConstant   = 500;

	if( SetCommTimeouts (m_hComPort,(LPCOMMTIMEOUTS) &cto) == nuFALSE )
	{
		return SETCOMMTIMEOUT_FAIL;
	}

	if( SetCommMask(m_hComPort, NURO_EV_RXCHAR) == nuFALSE )
	{
		return SETCOMMMASK_FAIL;
	}
	m_bInitialOK = nuTRUE;
#endif
	return OPENCOM_TRUE;
}
nuVOID CHUDCOMCTRL::CalHUDData(nuVOID *pHUDAddress, nuBYTE *SEIIBuf_Type1, nuBYTE *SEIIBuf_Type2, 
							   nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3)
{
	pHUDData pHUDDisplay = (pHUDData)pHUDAddress;
	nuINT  i = 0, nHUDAdded = 0, nIconNum = 0;
	nuLONG lTemp1 , lTemp2 , lTemp3;
	nuWCHAR wsHUDRoadName[13] = {0};
	lTemp1 = lTemp2 = lTemp3 = 0;
	nuMemset(SEIIBuf_Type1, 0x30, sizeof(nuBYTE) * 11);
	SEIIBuf_Type1[0] = 0xA5;
	if(pHUDDisplay->bNavi)
	{
		if(pHUDDisplay->nDistance < 1000)
		{
			SEIIBuf_Type1[3] = 0x30 + pHUDDisplay->nTurnFlag;
			if(pHUDDisplay->nDistance > 0)
			{
				lTemp1 = pHUDDisplay->nDistance / 100;
				lTemp2 = pHUDDisplay->nDistance / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nDistance - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[4] += lTemp1;
				SEIIBuf_Type1[5] += lTemp2;
				SEIIBuf_Type1[6] += lTemp3;
			}
			if(pHUDDisplay->nSpeed > 0)
			{
				lTemp1 = pHUDDisplay->nSpeed / 100;
				lTemp2 = pHUDDisplay->nSpeed / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nSpeed - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[7] += lTemp1;
				SEIIBuf_Type1[8] += lTemp2;
				SEIIBuf_Type1[9] += lTemp3;
			}
		}
		else
		{
			SEIIBuf_Type1[3] = 0x37;
			nuMemset(&SEIIBuf_Type1[4], 0x23, sizeof(nuBYTE) * 6);//Data 4 ~ 9 has to be cleaned
		}
	}
	else
	{
		nuMemset(&SEIIBuf_Type1[1], 0x23, sizeof(nuBYTE) * 9);//Data 1 ~ 9 has to be cleaned
	}
	nuMemcpy(SEIIBuf_Type2, SEIIBuf_Type1, sizeof(nuBYTE) * 10);
	SEIIBuf_Type1[10] = 0x5A;
	SEIIBuf_Type2[10] = pHUDDisplay->nSpeedLimit; //Raod Speed Warning
	SEIIBuf_Type2[11] = 0x30 + pHUDDisplay->nCCD; //CCD Warning
	SEIIBuf_Type2[12] = 0x31;
	SEIIBuf_Type2[13] = 0;//SEIIBuf_Type2[1];
	for(i = 1; i < 13; i++)
	{
		SEIIBuf_Type2[13] ^= SEIIBuf_Type2[i]; 
	}
	SEIIBuf_Type2[14] = 0x5A;
	pHUDDisplay->nIconType = GetHUDIconType(pHUDDisplay->nIconType);
	REGULUSBuf1[0] = 0xA5;
	if(pHUDDisplay->nRoadType == 2 && pHUDDisplay->nDistance < 1000)
	{
		nIconNum = pHUDDisplay->nIconType;
	}
	else if(pHUDDisplay->nRoadType == 1 && pHUDDisplay->nDistance < 300)
	{
		nIconNum = pHUDDisplay->nIconType;
	}
	if(0 == nIconNum)
	{
		if(pHUDDisplay->nIconType > 0x00 || pHUDDisplay->nIconType < 0x42)
		{
			if (0 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 22 || 338 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <=360 ) 
			{
				REGULUSBuf1[1] = _DIRECT_EAST; //0x02
			}
			else if (23  <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 67) 
			{
				REGULUSBuf1[1] = _DIRECT_NORTH_EAST; //0x01
			}
			else if (68  <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 112) 
			{
				REGULUSBuf1[1] = _DIRECT_NORTH; //0x00
			}
			else if (113 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 157) 
			{
				REGULUSBuf1[1] = _DIRECT_NORTH_WEST; //0x07
			}
			else if (158 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 202) 
			{
				REGULUSBuf1[1] = _DIRECT_WEST; //0x06
			}
			else if (203 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 247) 
			{
				REGULUSBuf1[1] = _DIRECT_SOUTH_WEST; //0x05
			}
			else if (248 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 292) 
			{
				REGULUSBuf1[1] = _DIRECT_SOUTH; //0x04
			}
			else if (293 <= pHUDDisplay->nAngle && pHUDDisplay->nAngle <= 337) 
			{
				REGULUSBuf1[1] = _DIRECT_SOUTH_EAST; //0x03
			}
		}
		nuWcscpy(wsHUDRoadName, pHUDDisplay->wsNowRoadName);
	}
	else
	{
		REGULUSBuf1[3] = nIconNum;
		nuWcscpy(wsHUDRoadName, pHUDDisplay->wsNextRoadName);
	}
	REGULUSBuf1[2] = pHUDDisplay->nSpeedLimit; 
	nuMemcpy(&REGULUSBuf1[4] , &SEIIBuf_Type2[4], sizeof(nuBYTE) * 3);//Data 4 ~ 6 Distance
	REGULUSBuf1[11] = 0x30 + pHUDDisplay->nCCD;
	REGULUSBuf1[12] = 0x31;
	for(i = 1; i < 13; i++)
	{
		REGULUSBuf1[13] ^= REGULUSBuf1[i]; 
	}
	REGULUSBuf1[14] = 0x5A;
	//TelNum
	/*if(pHUDDisplay->strTelNum[0] != 0)
	{
		i = 0;
		while(i+1 < sizeof(pHUDDisplay->strTelNum))
		{
			REGULUSBuf2[nHUDAdded] += 0x10 * (pHUDDisplay->strTelNum[i] - 0x30) + (pHUDDisplay->strTelNum[i+1] - 0x30);
			i += 2;
			nHUDAdded++;
		}
	}*/
	//RoadName
	nuMemset(REGULUSBuf3, 0, sizeof(nuBYTE) * 27);
	REGULUSBuf3[0] = 0xA5;
	REGULUSBuf3[1] = 0x0D;
	nHUDAdded = 2;
	for(i = 0; i < _HUDROADNAMELEN && nHUDAdded < 24; i++)
	{
		if(0 == wsHUDRoadName[i])
		{
			break;
		}
		REGULUSBuf3[nHUDAdded] += (wsHUDRoadName[i] & 0xFF00) / 0x100;
		nHUDAdded++;
		REGULUSBuf3[nHUDAdded] += wsHUDRoadName[i] & 0x00FF;
		nHUDAdded++;
	}
	REGULUSBuf3[26] = 0x5A;
}
nuBOOL CHUDCOMCTRL::bWriteCom(nuBYTE *SEIIBuf_Type1, nuBYTE *SEIIBuf_Type2, 
							   nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3)
{
#ifndef ANDROID
	if(3 == m_lHUDType)
	{
		m_LoadDLL_NECVOX.WriteCom(REGULUSBuf1, REGULUSBuf2, REGULUSBuf3);
	}
	else
	{
		nuDWORD BuffGet = 0;
		if(m_bInitialOK)
		{
			if(m_lHUDType == 1 && !WriteFile(m_hComPort, SEIIBuf_Type1,  11, &BuffGet, NULL))
			{
				return nuFALSE;
			}
			else if(m_lHUDType == 2 && !WriteFile(m_hComPort, SEIIBuf_Type2,  15, &BuffGet, NULL))
			{
				return nuFALSE;
			}
		}
		if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
		{
			if(m_lHUDType == 1)
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
				cbParam.lDataLen  = 11;
				cbParam.pVOID	  = (nuVOID*)SEIIBuf_Type1;
				g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
			}
			else if(m_lHUDType == 2)
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
				cbParam.lDataLen  = 15;
				cbParam.pVOID	  = (nuVOID*)SEIIBuf_Type2;
				g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
			}
		}
	}
#endif
	return nuTRUE;
}
nuVOID CHUDCOMCTRL::bCloseCom()
{
#ifndef ANDROID
	if(m_hComPort != NURO_INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComPort);
	}
	m_LoadDLL_NECVOX.FreeDLL();
#endif
}
nuLONG CHUDCOMCTRL::GetHUDIconType(nuLONG IconType)
{
	nuCHAR Type[10] = {0};
	nuLONG nTmpIcon = IconType;
	if( nTmpIcon < 50000 )
	{
		if( nTmpIcon < 1214 && nTmpIcon >= 1204 )
		{
			nTmpIcon = 1204;
		}
		else if( nTmpIcon < 1224 && nTmpIcon >= 1214 )
		{
			nTmpIcon = 1214;
		}
		else if( nTmpIcon == 1194 || nTmpIcon == 2004 )
		{
			nTmpIcon = 1194;
		}
		else if( nTmpIcon == 2014 )
		{
			nTmpIcon = 1334;
		}
		if( (nTmpIcon - 1025) % 10 == 0 )
		{
			nTmpIcon = 1025;
		}
		else if( (nTmpIcon - 1026) % 10 == 0 )
		{
			nTmpIcon = 1026;
		}
		else if( (nTmpIcon - 1027) % 10 == 0 )
		{
			nTmpIcon = 1027;
		}
		else if( (nTmpIcon - 1028) % 10 == 0 )
		{
			nTmpIcon = 1028;
		}
		else if( (nTmpIcon - 1029) % 10 == 0 )
		{
			nTmpIcon = 1029;
		}
		else if( (nTmpIcon - 1030) % 10 == 0 )
		{
			nTmpIcon = 1030;
		}
		else if( (nTmpIcon - 1031) % 10 == 0 )
		{
			nTmpIcon = 1031;
		}
		else if( (nTmpIcon - 1032) % 10 == 0 )
		{
			nTmpIcon = 1032;
		}
		else if( (nTmpIcon - 1033) % 10 == 0 )
		{
			nTmpIcon = 1033;
		}
	}
	if(nTmpIcon > 1025 && nTmpIcon < 2023)
	{
		nuItoa(nTmpIcon, Type, 10);
		switch(Type[3])
		{
			case '0': return 6;
			case '1': return 7;
			case '2': return 8;
			case '3': return 9;
			case '4':
				switch(nTmpIcon)
				{
					case 1034: return 10;
					case 1044: return 11;
					case 1054: return 12;
					case 1064: return 13;
					case 1074: return 14;
					case 1084: return 15;
					case 1094: return 16;
					case 1104: return 17;
					case 1114: return 18;
					case 1124: return 19;
					case 1134: return 20;
					case 1144: return 21;
					case 1154: return 22;
					case 1164: return 23;
					case 1174: return 24;
					case 1184: return 25;
					case 1194: return 26;
					case 1204: return 27;
					case 1214: return 28;
					case 1224: return 29;
					case 1234: return 30;
					case 1244: return 31;
					case 1254: return 32;
					case 1264: return 33;
					case 1274: return 34;
					case 1284: return 35;
					case 1294: return 36;
					case 1304: return 37;
					case 1324: return 38;
					case 1334: return 39;
					default: return 0;
				}
			case '5': return 1;
			case '6': return 2;
			case '7': return 3;
			case '8': return 4;
			case '9': return 5;
		  	default: return 0;
		}
	}
	else
	{
		switch(IconType)
		{
			case 51215: return 36;
			case 51216: return 37;
			case 51217: return 38;
			case 51218: return 39;
			case 51219: return 40;
			case 51220: return 41;
			case 51221: return 42;
			default: return 0;
		}
	}
}
nuLONG CHUDCOMCTRL::GetHUDListIconType(nuLONG IconType)
{
	nuLONG TempIconType = IconType;
	if( TempIconType == 5107 || TempIconType == 5109 )
	{
		TempIconType = TempIconType - 4470;
	}
	else
	{
		nuDWORD nTurnFlag = TempIconType % 100;
		if( nTurnFlag != 0 )
		{
			TempIconType = 5000 + nTurnFlag;
		}
		else
		{
			//TempIconType = 5010 + nRoadType;
		}
		TempIconType = TempIconType - 4400;
	}
	switch(TempIconType)
	{
		case 601: return 1;
		case 602: return 2;
		case 603: return 3;
		case 604: return 4;
		case 605: return 5;
		case 606: return 6;
		case 607: return 7;
		case 608: return 8;
		case 609: return 9;

		case 611: return 10;
		case 612: return 11;
		case 613: return 12;
		case 614: return 13;
		case 615: return 14;
		case 616: return 15;
		case 617: return 16;
		case 618: return 17;
		case 619: return 18;
		case 620: return 19;
		case 621: return 20;
		case 622: return 21;
		case 623: return 22;
		case 624: return 23;
		case 625: return 24;
		case 626: return 25;
		case 627: return 26;
		case 628: return 27;
		case 629: return 28;
		case 630: return 29;
		case 631: return 30;
		case 632: return 31;
		case 633: return 32;
		case 634: return 33;
		case 635: return 34;
		case 636: return 35;
		case 637: return 38;
		case 639: return 39;
		default: return 0;
	}
}
nuLONG CHUDCOMCTRL::GetHUDNaviListIconType(nuLONG IconType)
{
        nuLONG TempIconType = lChangeIconIndex(IconType);
	switch(TempIconType)
	{
		case 5001: return 1;
		case 5002: return 2;
		case 5003: return 3;
		case 5004: return 4;
		case 5005: return 5;
		case 5006: return 6;
		case 5007: return 7;
		case 5008: return 8;
		case 5009: return 9;

		case 5011: return 10;
		case 5012: return 11;
		case 5013: return 12;
		case 5014: return 13;
		case 5015: return 14;
		case 5016: return 15;
		case 5017: return 16;
		case 5018: return 17;
		case 5019: return 18;
		case 5020: return 19;
		case 5021: return 20;
		case 5022: return 21;
		case 5023: return 22;
		case 5024: return 23;
		case 5025: return 24;
		case 5026: return 25;
		case 5027: return 26;
		case 5028: return 27;
		case 5029: return 28;
		case 5030: return 29;
		case 5031: return 30;
		case 5032: return 31;
		case 5033: return 32;
		case 5034: return 33;
		case 5035: return 34;
		case 5036: return 35;
		case 5037: return 38;
		case 5039: return 39;
		default: return 0;
	}
}
nuLONG CHUDCOMCTRL::lChangeIconIndex(nuLONG c_lIconIndex)
{
	nuLONG lTmpIconNum = 0;

	if (c_lIconIndex >= 5000)
	{
		return c_lIconIndex;
	}

	nuLONG nTurnFlag = (c_lIconIndex -1024 )%10;

	if (1 <= (c_lIconIndex-1024)/10) 
	{
		if (0 == nTurnFlag)
		{ 
			lTmpIconNum = 5010+(c_lIconIndex-1024)/10;
		}
		else if (18 == (c_lIconIndex - 1024)/10)
		{ 
			lTmpIconNum = 5010+18;
		}
		else if (19 == (c_lIconIndex - 1024)/10)
		{
			lTmpIconNum = 5010+19;
		}
		else 
		{ 
			lTmpIconNum = 5000 +(c_lIconIndex -1024 )%10;
		} 
	} 
	else 
	{
		lTmpIconNum = 5000 +(c_lIconIndex -1024 )%10;
	}
	return lTmpIconNum;
}
nuVOID CHUDCOMCTRL::DebugMessage(nuLONG lValue, nuCHAR *Tempstr)
{
	return ;
#ifdef NURO_OS_WINCE
	HMODULE hModule = NULL;
	TCHAR tsPath[256] = {0};
	GetModuleFileName((HINSTANCE)hModule, tsPath, 256);
	for(nuINT i = _tcslen(tsPath); i >= 0; i--)
	{
		if(tsPath[i] == 0x5c)
		{
			tsPath[i + 1] = 0;
			break;
		}
	}
	nuTcscat(tsPath, TEXT("DANIEL_DEBUG.txt"));
	nuCHAR sss[10] = {0}, strdebug[512] = {0};
	nuStrcpy(strdebug, Tempstr);
	nuItoa(lValue, sss, 10);
	nuStrcat(strdebug, ": ");
	nuStrcat(strdebug, sss);
	nuStrcat(strdebug, "\n");
	FILE *pFile = _tfopen(tsPath, TEXT("a"));
	if(pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);
		fwrite(strdebug, strlen(strdebug), 1, pFile);
		fclose(pFile);
	}
#endif
}

nuVOID CHUDCOMCTRL::DebugMessage_BYTE(nuBYTE *Tempstr, nuLONG Size)
{
	return ;
#ifdef NURO_OS_WINCE
	HMODULE hModule = NULL;
	TCHAR tsPath[256] = {0};
	GetModuleFileName((HINSTANCE)hModule, tsPath, 256);
	for(nuINT i = _tcslen(tsPath); i >= 0; i--)
	{
		if(tsPath[i] == 0x5c)
		{
			tsPath[i + 1] = 0;
			break;
		}
	}
	nuTcscat(tsPath, TEXT("DANIEL_HUD.txt"));
	FILE *pFile = _tfopen(tsPath, TEXT("a"));
	nuCHAR line[3] = {0};
	//nuStrcpy(line, "\n");
	if(pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);
		fwrite(Tempstr, sizeof(nuBYTE), Size, pFile);
		//fwrite(line, strlen(line), 1, pFile);
		fclose(pFile);
	}
#endif
}

nuVOID CHUDCOMCTRL::CalPanaHUDData(nuVOID *pHUDAddress)
{
#ifdef NURO_OS_WINCE
	pHUDData pHUDDisplay = (pHUDData)pHUDAddress;
	COPYDATASTRUCT   CDS = {0};
	nuINT  i = 0, nHUDAdded = 0, nIconNum = 0;
	nuCHAR strPanaHUD[512] = {0}, stemp[10] = {0}, strDis[10] = {0};
	nuLONG nKM = 0, nM = 0;
	nuLONG nDirect  = (360 - (pHUDDisplay->nAngle - 90))%360 / 22.5;
	if(nDirect > 10)
	{
		nDirect = nDirect / 10;
	}
	nDirect = nDirect % 10;

	strPanaHUD[0] = '$';//Start Byte
	strPanaHUD[1] = 'N';
	strPanaHUD[2] = 'U';
	strPanaHUD[3] = 'R';
	strPanaHUD[4] = 'O';
	strPanaHUD[5] = pHUDDisplay->bNavi;//Mode
	strPanaHUD[6] = nDirect;//Direct
	strPanaHUD[7] = pHUDDisplay->nSpeed;//Speed
	strPanaHUD[8] = pHUDDisplay->nSpeedLimit;//Current Road Speed Limit
	strPanaHUD[9] = pHUDDisplay->nCCD;//Camera
	nHUDAdded += 10;
	for(i = 0; i < _HUDROADNAMELEN && nHUDAdded < 34; i++)
	{
		if(0 == pHUDDisplay->wsNowRoadName[i])
		{
			break;
		}
		strPanaHUD[nHUDAdded++] += (pHUDDisplay->wsNowRoadName[i] & 0xFF00) / 0x100;
		strPanaHUD[nHUDAdded++] += pHUDDisplay->wsNowRoadName[i] & 0x00FF;
	}
	if(pHUDDisplay->bNavi)
	{
		strPanaHUD[34] = GetHUDIconType(pHUDDisplay->nIconType);
		if( pHUDDisplay->nDistance < 1000 )
		{	
			nuItoa(pHUDDisplay->nDistance, stemp, 10);
			nuStrcpy(strDis, stemp);
			nuStrcat(strDis,"M");
		}
		else
		{
			nKM = pHUDDisplay->nDistance / 1000;
			nM = (pHUDDisplay->nDistance % 1000 ) / 100;
			nuItoa(nKM, stemp, 10);
			nuStrcpy(strDis, stemp);
			if( nKM < 100 )
			{
				nuStrcat(strDis,".");
				nuMemset(stemp, 0, sizeof(stemp));
				nuItoa(nM, stemp, 10);
				nuStrcat(strDis, stemp);
			}	
			nuStrcat(strDis,"KM");
		}
		for(i = 0; i < nuStrlen(strDis); i++)
		{
			if(0 == strDis[i])
			{
				break;
			}
			strPanaHUD[nHUDAdded++] = strDis[i];
		}
		nHUDAdded = 42;
		for(i = 0; i < _HUDROADNAMELEN && nHUDAdded < 66; i++)
		{
			if(0 == pHUDDisplay->wsNextRoadName[i])
			{
				break;
			}
			strPanaHUD[nHUDAdded++] += (pHUDDisplay->wsNextRoadName[i] & 0xFF00) / 0x100;
			strPanaHUD[nHUDAdded++] += pHUDDisplay->wsNextRoadName[i] & 0x00FF;
		}
		for(i = 0; i < pHUDDisplay->LaneGuide.LaneCount && i < 16; i++)
		{
			strPanaHUD[nHUDAdded++] = pHUDDisplay->LaneGuide.bLight[i] * 0x0F + pHUDDisplay->LaneGuide.LightNum[i] - 1;
		}
	}	
	for(i = 5; i < 81; i++)
	{
		strPanaHUD[82] ^= strPanaHUD[i];//Checksum
	}
	strPanaHUD[83] = '*';//Stop Byte
	strPanaHUD[84] = 'S';
	strPanaHUD[85] = 'T';
	strPanaHUD[86] = 'O';
	strPanaHUD[87] = 'P';
	strPanaHUD[88] = '*';

	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = PANA_HUD_DATA_TYPE;
		cbParam.lDataLen  = strlen(strPanaHUD);
		cbParam.pVOID	  = strPanaHUD;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
#ifdef NURO_OS_WINCE
		//RETAILMSG(1, (TEXT("\n********************************************\nSending HUD\n********************************************\n")));
#endif
	}
#endif
}
#ifdef NURO_OS_WINCE
nuVOID CHUDCOMCTRL::ELead_SendMessage(nuBYTE *SendMessage, nuLONG Len, nuVOID *pHUDAddress)
{
	if(NULL == g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		return;
	}
	nuWCHAR wsNextRoadName[512] = {0}, wsCurrentRoadName[512] = {0};
	if(m_lHUDType == 2 || m_lHUDType == 3)//m_ELeadHudhwnd != NULL && 
	{
		pHUDData pHUDDisplay = (pHUDData)pHUDAddress;
		if(m_nNaviState != pHUDDisplay->bNavi)
		{
			m_nNaviState = pHUDDisplay->bNavi;
			ELEAD_SendISNavi(pHUDDisplay->bNavi);
		}
		/*if(!pHUDDisplay->bFixRoad)
		{
			ELEAD_OffRoad();
		}
		else*/ if(0 != nuMemcmp(m_ELEAD_State_NowRoadName, pHUDDisplay->wsNowRoadName, nuWcslen(pHUDDisplay->wsNowRoadName)))
		{
			nuMemset(m_ELEAD_State_NowRoadName, 0, sizeof(nuWCHAR) * _ELEADROADNAMELEN);
			nuWcscpy(m_ELEAD_State_NowRoadName, pHUDDisplay->wsNowRoadName);
			ELEAD_SendCurrentRoadName(pHUDDisplay->wsNowRoadName);
		}	
		/*if(m_ELEAD_State_Speed != pHUDDisplay->nSpeed)
		{
			m_ELEAD_State_Speed = pHUDDisplay->nSpeed;
			ELEAD_SendSpeed(pHUDDisplay->nSpeed);
		}*/
		if(pHUDDisplay->bNavi)
		{		
			ELEAD_SendDistance(pHUDDisplay->nDistance, pHUDDisplay->nTurnFlag);
			if(m_ELEAD_State_IconType != pHUDDisplay->nIconType || m_ELEAD_State_TurnFlag != pHUDDisplay->nTurnFlag)
			{
				m_ELEAD_State_IconType = pHUDDisplay->nIconType;
				m_ELEAD_State_TurnFlag = pHUDDisplay->nTurnFlag;
				ELEAD_SendTurnFlag(pHUDDisplay->nIconType, pHUDDisplay->nTurnFlag);
				//ELEAD_SelfDirect(g_fileSysApi.pGdata->drawInfoNavi.lEleadIconType);
			}
			if(0 != nuMemcmp(m_ELEAD_State_RoadName, pHUDDisplay->wsNextRoadName, nuWcslen(pHUDDisplay->wsNextRoadName)))
			{
				nuMemset(m_ELEAD_State_RoadName, 0, sizeof(nuWCHAR) * _ELEADROADNAMELEN);
				nuWcscpy(m_ELEAD_State_RoadName, pHUDDisplay->wsNextRoadName);
				ELEAD_SendNextRoadName(pHUDDisplay->wsNextRoadName);
			}
		}
		else if(m_lHUDType == 3)
		{
			ELEAD_ColFreeNaviRoadName(pHUDDisplay->wsNowRoadName);
		}
		if(m_lHUDType == 3)
		{
			ELEAD_SendTotoalDisAndTime(pHUDDisplay->nTotalTime, pHUDDisplay->nTotalDis);
			ELEAD_MapDirect(pHUDDisplay->nAngle);		
		}
		if(m_ELEAD_State_SpeedLimit != pHUDDisplay->nSpeedLimit || m_ELEAD_State_CCD != pHUDDisplay->nCCD)
		{
			m_ELEAD_State_SpeedLimit = pHUDDisplay->nSpeedLimit;
			m_ELEAD_State_CCD = pHUDDisplay->nCCD;
			ELEAD_SendSpeedAndCCD(pHUDDisplay->nSpeedLimit, pHUDDisplay->nCCD);
		}
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendISNavi(nuLONG bNavi)
{
	COPYDATASTRUCT CDS  = {0};
	nuMemset(&CDS, 0, sizeof(COPYDATASTRUCT));
	nuBYTE	Buffer[512] = {0};
	Buffer[0]  = 0x91;
	Buffer[1]  = 2; 
	Buffer[2]  = bNavi;
	CDS.cbData = 4;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[3] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
#ifdef NURO_OS_WINCE
		//RETAILMSG(1, (TEXT("\n********************************************\nSending HUD\n********************************************\n")));
#endif
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendTurnFlag(nuLONG IconType, nuLONG TurnFlag)
{
	COPYDATASTRUCT CDS  = {0};
	nuMemset(&CDS, 0, sizeof(COPYDATASTRUCT));
	nuBYTE	Buffer[512] = {0};
	nuLONG	FlagType	= 0;
	nuLONG  TurnIcon	= IconType;
	FlagType = TurnFlag;
	/*switch(TurnIcon)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:	
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
		FlagType = TurnFlag;
		if(FlagType == 0)
		{
			FlagType++;
		}
		break;
	default:
		FlagType = TurnIcon;
		break;
	}*/	
	CDS.dwData = ELEAD_HUD_DATA;
	/*if(m_nTmpFlag == 9)
	{
		m_nTmpFlag = 13;
	}
	else if(m_nTmpFlag == 16)
	{
		m_nTmpFlag = 20;
	}
	else if(m_nTmpFlag == 21)
	{
		m_nTmpFlag = 23;
	}
	else if(m_nTmpFlag == 24)
	{
		m_nTmpFlag = 1;
	}*/
	//FlagType = m_nTmpFlag;
	//m_nTmpFlag++;
#ifdef ELEAD_DEBUG
	//RETAILMSG(1, (TEXT("\n********************************************\n SendTurnFlag FlagType %d \n********************************************\n"), FlagType));
#endif
	switch(FlagType)
	{
	case 1:
		//left
		//0x96	8	0x01	0x0D	0x40	0x00	0x02	0x00	0xC0	checksum
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0D;
		Buffer[4]  = 0x40;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData = 6;
		//OutputDebugStringA("Left\n");
		break;
	case 2:
		//right
		//0x96	8	0x01	0x0D	0xC0	0x00	0x02	0x00	0x40	checksum
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0D;
		Buffer[4]  = 0xC0;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData = 6;
		//OutputDebugStringA("Right\n");
		break;
	case 3:
		//leftside
		//0x96	9	0x01	0x0D	0x20		0x03	0x00	0x40	0xC0	
		Buffer[0]  = 0x96;
		Buffer[1]  = 9;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0D;
		Buffer[4]  = 0x20;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x03;
		Buffer[7]  = 0x00;
		Buffer[8]  = 0x40;
		Buffer[9]  = 0xC0;
		CDS.cbData = 9;
		//OutputDebugStringA("LeftSide\n");
		break;
	case 4:
		//rightside
		//0x96	9	0x01	0x0D	0xE0		0x03	0x00	0x40	0xC0	
		Buffer[0]  = 0x96;
		Buffer[1]  = 9;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0D;
		Buffer[4]  = 0xE0;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x03;
		Buffer[7]  = 0x00;
		Buffer[8]  = 0x40;
		Buffer[9]  = 0xC0;
		CDS.cbData = 9;
		//OutputDebugStringA("RightSide\n");
		break;
	case 5:
		//Left U Turn
		//0x96	6	0x01	0x19	0x40		0x00	
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x19;
		Buffer[4]  = 0x40;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData = 6;
		//OutputDebugStringA("Left U Turn\n");
		break;
	case 6:
		//Right U Turn
		//0x96	6	0x01	0x19	0xC0		0x00	
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x19;
		Buffer[4]  = 0xC0;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData	 = 6;
		//OutputDebugStringA("Right U Turn\n");
		break;
	case 7://ahead
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0B;
		Buffer[4]  = 0xFF;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData = 6;
		//Buffer[5]  = 0x11;
		//OutputDebugStringA("Ahead\n");
		break;
	/*case 8:
		//mid
		//0x96	8	0x01	0x0D	0x00	0x00	0x02	0x20	0xE0	checksum
		Buffer[0]  = 0x96;
		Buffer[1]  = 6;
		Buffer[2]  = 0x01;
		Buffer[3]  = 0x0B;
		Buffer[4]  = 0xFF;
		Buffer[5]  = 0x00;
		Buffer[6]  = 0x00;
		CDS.cbData = 6;
		//OutputDebugStringA("Mid\n");
		break;
	case 13://enter bridge
		//ID	Len	路型	文字長度	文字byte數	CheckSum
		//0x98	0x03-0x022	0x05	0x00-0x1D	1~30	
		Buffer[0]    = 0x98;
		Buffer[1]    = 0x03;
		Buffer[2]    = 0x05;
		Buffer[3]    = 0x00;
		CDS.cbData	 = 3;
		//OutputDebugStringA("Enter Bridge\n");
		break;
	case 14://enter freeway
		//0x98	3	0x02	0x02	checksum
		Buffer[0]  = 0x98;
		Buffer[1]  = 3;
		Buffer[2]  = 0x02;
		Buffer[3]  = 0x01;
		CDS.cbData = 3;
		//OutputDebugStringA("Enter Freeway\n");
		break;
	case 15://Tunnel
		//ID	Len	路型	文字長度	文字byte數	CheckSum
		//0x98	0x03-0x022	0x07	0x00-0x1D	1~30	
		Buffer[0]  = 0x98;
		Buffer[1]  = 0x03;
		Buffer[2]  = 0x00;
		Buffer[3]  = 0x07;
		Buffer[4]  = 0x00;
		CDS.cbData = 4;
		//OutputDebugStringA("Tunnel\n");
		break;
	case 20://leave freeway
		//0x98	3	0x02	0x04	checksum
		Buffer[0]  = 0x98;
		Buffer[1]  = 0x03;
		Buffer[2]  = 0x02;
		Buffer[3]  = 0x04;
		CDS.cbData = 3;
		//OutputDebugStringA("Leave Freeway\n");
		break;
	case 23://Tollgate
		//ID	Len	路型	文字長度	文字byte數	CheckSum
		//0x98	0x03-0x022	0x06	0x00-0x1D	1~30	
		Buffer[0]  = 0x98;
		Buffer[1]  = 0x03;
		Buffer[2]  = 0x06;
		Buffer[3]  = 0x00;
		CDS.cbData = 3;
		//OutputDebugStringA("Tollgate\n");
		break;*/
	default:
		return ;
	}
	CDS.cbData++;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData+1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
#ifdef NURO_OS_WINCE
		//RETAILMSG(1, (TEXT("\n********************************************\nSending HUD\n********************************************\n")));
#endif
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendTotoalDisAndTime(nuLONG lTotalTime, nuLONG lDistance)
{
	COPYDATASTRUCT CDS  = {0};
	CDS.dwData			= ELEAD_HUD_DATA ;
	nuBYTE	Buffer[512] = {0};
	if(lDistance > 1000)
	{
		Buffer[5] = 1;
		lDistance /= 100;
	}
	else
	{
		lDistance *= 10;
	}
	Buffer[0]   = 0x93;
	Buffer[1]   = 9;
	Buffer[2]   = lDistance & 0xFF;
	Buffer[3]   = ((lDistance & 0xFF00) >> 8);
	Buffer[4]   = ((lDistance & 0xFF0000) >> 16);

	Buffer[6] = 0x12;//24hr
	Buffer[7] = (lTotalTime % 3600) / 60;
	Buffer[8] = lTotalTime / 3600 % 24;

	if(m_ELEAD_State_Distance != lDistance)
	{
		m_ELEAD_State_Distance = lDistance;
	}
	else
	{
		return ;
	}
	CDS.cbData = 10;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[9] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendDistance(nuLONG lDistance, nuLONG lTurnFlag)
{
	/*if(m_lEleadDistance == 0)
	{
		m_lEleadDistance = lDistance;
	}
	else
	{
		if(lDistance >= 300 && (NURO_ABS(m_lEleadDistance - lDistance) < 100))
		{
			m_lEleadDistance = lDistance;
			return ;
		}
		else if(lDistance < 300 && (NURO_ABS(m_lEleadDistance - lDistance) < 30))
		{
			m_lEleadDistance = lDistance;
			return ;
		}
	}
	m_lEleadDistance	= lDistance;*/
	COPYDATASTRUCT CDS  = {0};
	CDS.dwData			= ELEAD_HUD_DATA ;
	nuBYTE	Buffer[512] = {0};
	if(lDistance > 1000)
	{
		Buffer[5] = 1;
		lDistance /= 100;
	}
	else
	{
		lDistance *= 10;
	}
	Buffer[0]   = 0x92;
	Buffer[1]   = 6;
	Buffer[2]   = lDistance & 0xFF;
	Buffer[3]   = ((lDistance & 0xFF00) >> 8);
	Buffer[4]   = ((lDistance & 0xFF0000) >> 16);
	
	if(m_ELEAD_State_Distance != lDistance)
	{
		m_ELEAD_State_Distance = lDistance;
	}
	else
	{
		return ;
	}
	if(lTurnFlag > 1)
	{
		Buffer[6] = 1;
	}
	CDS.cbData = 8;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[7] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendSpeed(nuLONG lSpeed)
{
	COPYDATASTRUCT CDS  = {0};
	CDS.dwData			= ELEAD_HUD_DATA;//;
	nuBYTE	Buffer[512] = {0};
	nuMemset(Buffer, 0, 512);
	Buffer[0] = 0x9A;
	Buffer[1] = 4;
	Buffer[2] = lSpeed & 0x00FF;
	Buffer[3] = lSpeed & 0xFF00;
	Buffer[4] = 0x00;
	CDS.cbData = 5;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[5] ^= Buffer[i];
	}	
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData+1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendSpeedAndCCD(nuLONG lSpeedLimit, nuBOOL bCCD)
{
	COPYDATASTRUCT CDS  = {0};
	CDS.dwData			= ELEAD_HUD_DATA;//;
	nuBYTE	Buffer[512] = {0};
	nuMemset(Buffer, 0, 512);
	Buffer[0]   = 0x9B;
	Buffer[1] = 5;
	Buffer[2] = lSpeedLimit & 0x00FF;
	Buffer[3] = lSpeedLimit & 0xFF00;
	Buffer[4] = 0;
	if(bCCD)
	{
		Buffer[5] = 1;
	}
#ifdef ELEAD_DEBUG
	//RETAILMSG(1, (TEXT("\n********************************************\n SendSpeedAndCCD  %d \n********************************************\n"), bCCD));
#endif
	CDS.cbData = 7;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[6] ^= Buffer[i];
	}	
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
	//::SendMessage(m_ELeadHudhwnd, WM_COPYDATA, 0, (LPARAM)&CDS);
	//DebugMessage(0, "SendMessage ELEAD_SendSpeedAndCCD");
}
nuVOID CHUDCOMCTRL::ELEAD_SendCurrentRoadName(nuWCHAR *wsRoadName)
{
	COPYDATASTRUCT CDS		= {0};
	CDS.dwData				= ELEAD_HUD_DATA;
	nuBYTE	Buffer[512]		= {0};
	nuINT i					= 0, WordIndex = 0, WordLen = 0;
	nuBYTE	TmpChar[512]    = {0};
	//nuWcscpy(wsRoadName, nuTEXT("省道"));
#ifndef	NURO_OS_WINCE
	OutputDebugString(wsRoadName);
#endif
	nuINT WcharLen			= nuWcslen(wsRoadName), Len = 0;
	if(WcharLen > 6)
	{
		WcharLen = 6;
	}
	Buffer[0]	= 0x93;
	//Buffer[1]	= ;
	/*nuINT len = WideCharToMultiByte(CP_UTF8, 0, wsRoadName, -1, NULL, 0, NULL, NULL);
	if(len > 18)
	{
		len = 18;
	}
	nuCHAR utf8[20] = {0};
	WideCharToMultiByte(CP_UTF8, 0, wsRoadName, -1, utf8, len, NULL, NULL);*/
	//nuMemcpy(&Buffer[3], utf8, len);
	for(i = 0; i < WcharLen; i++)
	{
		WordIndex+=enc_unicode_to_utf8((nuULONG)wsRoadName[i], &TmpChar[WordIndex], Len);
		if(WordIndex > 18)
		{
			break;
		}
		WordLen = WordIndex;
	}
	nuMemcpy(&Buffer[3], TmpChar, WordLen);
	Buffer[1]	= WordLen + 2;
	Buffer[2]	= WordLen;
	CDS.cbData  = Buffer[1] + 1;
	for(i = 1; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData + 1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendNextRoadName(nuWCHAR *wsRoadName)
{
	COPYDATASTRUCT CDS		= {0};
	CDS.dwData				= ELEAD_HUD_DATA;
	nuBYTE	Buffer[512]		= {0};
	nuINT i					= 0, WordIndex = 0, WordLen = 0;
	nuBYTE	TmpChar[512]    = {0};
	nuINT WcharLen			= nuWcslen(wsRoadName), Len = 0;
	if(WcharLen > 6)
	{
		WcharLen = 6;
	}
	Buffer[0]	= 0x94;
	for(i = 0; i < WcharLen; i++)
	{
		WordIndex+=enc_unicode_to_utf8((nuULONG)wsRoadName[i], &TmpChar[WordIndex], Len);
		if(WordIndex > 18)
		{
			break;
		}
		WordLen = WordIndex;
	}
	nuMemcpy(&Buffer[3], TmpChar, WordLen);
	Buffer[1]	= WordLen + 2;
	Buffer[2]	= WordLen;
	CDS.cbData  = Buffer[1] + 1;
	for(i = 1; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData + 1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_OffRoad()
{
	nuBYTE	Buffer[512]		= {0};
	COPYDATASTRUCT CDS		= {0};
	CDS.dwData				= ELEAD_HUD_DATA;
	CDS.cbData				= 7;
	Buffer[0]				= 0x96;
	Buffer[1]				= 6;
	Buffer[2]				= 0x01;
	Buffer[3]				= 0x06;
	Buffer[4]				= 0x00;
	Buffer[5]				= 0x00;
	Buffer[6]				= 0x00;

	for(nuINT i = 0; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData + 1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuVOID CHUDCOMCTRL::ELEAD_MapDirect(nuSHORT nAngle)
{
	nuBYTE	Buffer[512]		= {0};
	COPYDATASTRUCT CDS		= {0};
	nuLONG nDirect  = (360 - (nAngle - 90))%360 / 45;
	CDS.dwData				= ELEAD_HUD_DATA;
	CDS.cbData				= 3;
	Buffer[0]				= 0x90;
	Buffer[1]				= 2;
	switch(nDirect)
	{
	case 0:
		Buffer[2] = 0x01;
		break;
	case 1:
		Buffer[2] = 0x02;
		break;
	case 2:
		Buffer[2] = 0x04;
		break;
	case 3:
		Buffer[2] = 0x06;
		break;
	case 4:
		Buffer[2] = 0x08;
		break;
	case 5:
		Buffer[2] = 0x0A;
		break;
	case 6:
		Buffer[2] = 0x0D;
		break;
	case 7:
		Buffer[2] = 0x0E;
		break;
	default:
		Buffer[2] = 0xFF;
		break;
	}

	for(nuINT i = 0; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 1;
		cbParam.lDataLen  = CDS.cbData + 1;
		cbParam.pVOID	  = (nuVOID*)Buffer;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}
nuINT CHUDCOMCTRL::enc_unicode_to_utf8(nuULONG unic, nuBYTE* lputf, nuINT utfsize)
{
	if(lputf == NULL)
	{
		//printf("input parameter has error!\n");
		return -1;
	}
	if(unic < 0x00000080)   //1字? 0000 0000 - 0000 007F
	{
		*lputf = unic & 0x7f;
		return 1;
	}
	else if(unic < 0x00000800)
	{
		*(lputf + 1) = (unic & 0x3f) | 0x80;
		*lputf = ((unic >> 6) & 0x1f) | 0xc0;
		return 2;
	}
	else if(unic < 0x00010000)
	{
		*(lputf + 2) = (unic & 0x3f) | 0x80;
		*(lputf + 1) = ((unic >> 6) & 0x3f) | 0x80;
		*lputf = ((unic >> 12) & 0x0f) | 0xe0;
		return 3;
	}
	else if(unic < 0x00200000)
	{
		*(lputf + 3) = (unic & 0x3f) | 0x80;
		*(lputf + 2) = ((unic >> 6) & 0x3f) | 0x80;
		*(lputf + 1) = ((unic >> 12) & 0x3f) | 0x80;
		*lputf = ((unic >> 18) & 0x07) | 0xf0;
		return 4;
	}
	else if(unic < 0x04000000)
	{
		*(lputf + 4) = (unic & 0x3f) | 0x80;
		*(lputf + 3) = ((unic >> 6) & 0x3f) | 0x80;
		*(lputf + 2) = ((unic >> 12) & 0x3f) | 0x80;
		*(lputf + 1) = ((unic >> 18) & 0x3f) | 0x80;
		*lputf = ((unic >> 24) & 0x03) | 0xf8;
		return 5;
	}
	else
	{
		*(lputf + 5) = (unic & 0x3f) | 0x80;
		*(lputf + 4) = ((unic >> 6) & 0x3f) | 0x80;
		*(lputf + 3) = ((unic >> 12) & 0x3f) | 0x80;
		*(lputf + 2) = ((unic >> 18) & 0x3f) | 0x80;
		*(lputf + 1) = ((unic >> 24) & 0x3f) | 0x80;
		*lputf = ((unic >> 30) & 0x01) | 0xfc;
		return 6;
	}
	return 0;
}
nuVOID CHUDCOMCTRL::SEII_HUD(pHUDData pHUDDisplay)
{
	nuBYTE SEIIBuf_Type1[11] = {0}, SEIIBuf_Type2[27] = {0};
	nuBYTE HITOP[128] = {0}, nRoadNameCount = 0;
	nuINT  i = 0, nHUDAdded = 0, nLength = 0, CheckSum = 0;
	nuUINT nKM = 0, nM = 0, nDirect = 0;
	nuLONG lTemp1 = 0, lTemp2 = 0, lTemp3 = 0;
	nuCHAR strDis[8] = {0}, strRaodNameBig5[128] = {0}, strTmp[15] = {0};
	nuMemset(SEIIBuf_Type1, 0x30, sizeof(SEIIBuf_Type1));
	nuMemset(SEIIBuf_Type2, NULL, sizeof(SEIIBuf_Type2));
	SEIIBuf_Type1[0] = 0xA5;
	if(pHUDDisplay->bNavi)
	{
		if(pHUDDisplay->nDistance < 1000)
		{
			SEIIBuf_Type1[3] = 0x30 + pHUDDisplay->nTurnFlag;
			if(pHUDDisplay->nDistance > 0)
			{
				lTemp1 = pHUDDisplay->nDistance / 100;
				lTemp2 = pHUDDisplay->nDistance / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nDistance - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[4] += lTemp1;
				SEIIBuf_Type1[5] += lTemp2;
				SEIIBuf_Type1[6] += lTemp3;
			}
			if(pHUDDisplay->nSpeed > 0)
			{
				lTemp1 = pHUDDisplay->nSpeed / 100;
				lTemp2 = pHUDDisplay->nSpeed / 10 - lTemp1 * 10;
				lTemp3 = pHUDDisplay->nSpeed - (lTemp1 * 100 + lTemp2 * 10);
				SEIIBuf_Type1[7] += lTemp1;
				SEIIBuf_Type1[8] += lTemp2;
				SEIIBuf_Type1[9] += lTemp3;
			}
		}
		else
		{
			SEIIBuf_Type1[3] = 0x37;
			nuMemset(&SEIIBuf_Type1[4], 0x23, sizeof(nuBYTE) * 6);//Data 4 ~ 9 has to be cleaned
		}
	}
	else
	{
		nuMemset(&SEIIBuf_Type1[1], 0x23, sizeof(nuBYTE) * 9);//Data 1 ~ 9 has to be cleaned
	}
	nuMemcpy(SEIIBuf_Type2, SEIIBuf_Type1, sizeof(nuBYTE) * 10);
	SEIIBuf_Type1[10] = 0x5A;
	SEIIBuf_Type2[10] = pHUDDisplay->nSpeedLimit; //Raod Speed Warning
	SEIIBuf_Type2[11] = 0x30 + pHUDDisplay->nCCD; //CCD Warning
	SEIIBuf_Type2[12] = 0x31;
	SEIIBuf_Type2[13] = 0;//SEIIBuf_Type2[1];
	for(i = 1; i < 13; i++)
	{
		SEIIBuf_Type2[13] ^= SEIIBuf_Type2[i]; 
	}
	SEIIBuf_Type2[14] = 0x5A;
	nHUDAdded = 15;
	if(pHUDDisplay->bNavi)
	{
		for(i = 0; i < _HUDROADNAMELEN && nHUDAdded < 27; i++)
		{
			if(NULL == pHUDDisplay->wsNextRoadName[i])
			{
				//SEIIBuf_Type2[nHUDAdded] = 0x00;
				//nHUDAdded++;
				//continue;
				break;
			}
			SEIIBuf_Type2[nHUDAdded] = (pHUDDisplay->wsNextRoadName[i] & 0xFF00) / 0x100;
			nHUDAdded++;
			SEIIBuf_Type2[nHUDAdded] = pHUDDisplay->wsNextRoadName[i] & 0x00FF;
			nHUDAdded++;
		}
	}
	if(NULL != g_fileSysApi.pGdata->pfNaviThreadCallBack)
	{
		CALL_BACK_PARAM cbParam = {0};
		cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
		cbParam.lCallID   = 0x310A;
		cbParam.lDataLen  = sizeof(SEIIBuf_Type2);
		cbParam.pVOID	  = (nuVOID*)SEIIBuf_Type2;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
	}
}

nuVOID CHUDCOMCTRL::ELEAD_ColFreeNaviRoadName(nuWCHAR *wsRoadName)
{
	NEXTROAD NextCrossRoad = {0}, ExtendNextCrossRoad = {0};;
	CROSSROADINFO CrossRoadInfo = {0};
	nuroRECT rtMap = {0};
	nuBYTE	 nSendNum = 0;
	nuBOOL   bPassStraight  = nuFALSE;
	if(g_fileSysApi.pGdata->uiSetData.bNavigation)
	{
		return ;
	}			
	rtMap.left	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x - _ROAD_CHECK_MAX_DIS;
	rtMap.right	= g_fileSysApi.pGdata->carInfo.ptCarFixed.x + _ROAD_CHECK_MAX_DIS;
	rtMap.top	= g_fileSysApi.pGdata->carInfo.ptCarFixed.y - _ROAD_CHECK_MAX_DIS;
	rtMap.bottom= g_fileSysApi.pGdata->carInfo.ptCarFixed.y + _ROAD_CHECK_MAX_DIS;
	CrossRoadInfo.nRoadMapIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx;
	CrossRoadInfo.nRoadBlkIdx		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nBlkIdx;
	CrossRoadInfo.nRoadRdIdx		= (nuWORD)g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadIdx;
	CrossRoadInfo.ptRoadMapped	    = g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptMapped;
	CrossRoadInfo.ptRoadNextCoor	= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.ptNextCoor;
	CrossRoadInfo.nOneWay			= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nOneWay;
	CrossRoadInfo.nRoadClass		= g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nRoadClass;

	m_ELEAD_nMapIdx  = CrossRoadInfo.nRoadMapIdx;
	m_ELEAD_nBlkIdx  = CrossRoadInfo.nRoadBlkIdx;
	m_ELEAD_nRoadIdx = CrossRoadInfo.nRoadRdIdx;
	if(!g_fileSysApi.FS_SeekForCorssRoadInfo(rtMap, &CrossRoadInfo))
	{
		return ;
	}
	ELEAD_SelectRoad_ForFreeNavi(wsRoadName, CrossRoadInfo, NextCrossRoad, ExtendNextCrossRoad, nuTRUE);
	if(NextCrossRoad.nAngle != _EMPTY_STRUCT)
	{
		ELEAD_SendFreeNaviRoadName(NextCrossRoad, 0x01);
		ELEAD_SendFreeCrossRoadDis(g_fileSysApi.pGdata->carInfo.ptCarFixed,NextCrossRoad, 0x02);
		nSendNum++;
	}
	if(nSendNum >= 2)
	{
		return ;
	}
	int count = 0;
	while(ExtendNextCrossRoad.nAngle == _EXTEND_STRUCTURE)
	{
		if(count >= 30)
		{
			break;	
		}
		count++;
		rtMap.left	= ExtendNextCrossRoad.ptEnd.x - _ROAD_CHECK_MAX_DIS;
		rtMap.right	= ExtendNextCrossRoad.ptEnd.x + _ROAD_CHECK_MAX_DIS;
		rtMap.top	= ExtendNextCrossRoad.ptEnd.y - _ROAD_CHECK_MAX_DIS;
		rtMap.bottom= ExtendNextCrossRoad.ptEnd.y + _ROAD_CHECK_MAX_DIS;
		CrossRoadInfo.nRoadMapIdx		= ExtendNextCrossRoad.nNextMapIdx;
		CrossRoadInfo.nRoadBlkIdx		= ExtendNextCrossRoad.nNextBlkIdx;
		CrossRoadInfo.nRoadRdIdx		= ExtendNextCrossRoad.nNextRdIdx;
		CrossRoadInfo.ptRoadMapped		= ExtendNextCrossRoad.ptFirst;
		CrossRoadInfo.ptRoadNextCoor	= ExtendNextCrossRoad.ptEnd;
		CrossRoadInfo.nOneWay			= ExtendNextCrossRoad.nOneWay;
		if(!g_fileSysApi.FS_SeekForCorssRoadInfo(rtMap, &CrossRoadInfo))
		{
			if(nSendNum == 1)
			{
				nuMemset(&NextCrossRoad, 0, sizeof(NEXTROAD));
				ELEAD_SendFreeNaviRoadName(NextCrossRoad, 0x03);
				ELEAD_SendFreeCrossRoadDis(g_fileSysApi.pGdata->carInfo.ptCarFixed,NextCrossRoad, 0x04);
			}
			return;
		}
		ELEAD_SelectRoad_ForFreeNavi(ExtendNextCrossRoad.wsRoadName, CrossRoadInfo, NextCrossRoad, ExtendNextCrossRoad, nuFALSE);
		if(NextCrossRoad.nAngle != _EMPTY_STRUCT)
		{
			if(nSendNum == 0)
			{
				if(0 == nuMemcmp(ExtendNextCrossRoad.wsRoadName, NextCrossRoad.wsRoadName, sizeof(nuWCHAR) * 12))
				{
					continue;
				}
				ELEAD_SendFreeNaviRoadName(NextCrossRoad, 0x01);
				ELEAD_SendFreeCrossRoadDis(g_fileSysApi.pGdata->carInfo.ptCarFixed,NextCrossRoad, 0x02);
			}
			else if(nSendNum == 1)
			{
				if(0 == nuMemcmp(ExtendNextCrossRoad.wsRoadName, NextCrossRoad.wsRoadName, sizeof(nuWCHAR) * 12))
				{
					continue;
				}
				ELEAD_SendFreeNaviRoadName(NextCrossRoad, 0x03);
				ELEAD_SendFreeCrossRoadDis(g_fileSysApi.pGdata->carInfo.ptCarFixed,NextCrossRoad, 0x04);
			}
			nSendNum++;
			if(nSendNum >= 2)
			{
				return ;
			}
		}
	}
	if(nSendNum == 1)
	{
		nuMemset(&NextCrossRoad, 0, sizeof(NEXTROAD));
		ELEAD_SendFreeNaviRoadName(NextCrossRoad, 0x03);
		ELEAD_SendFreeCrossRoadDis(g_fileSysApi.pGdata->carInfo.ptCarFixed,NextCrossRoad, 0x04);
	}
}

nuVOID CHUDCOMCTRL::ELEAD_SelectRoad_ForFreeNavi(nuWCHAR *wsRoadName, CROSSROADINFO &CrossRoadInfo, NEXTROAD &NextCrossRoad, 
											  NEXTROAD &ExtendCrossRoadInfo, nuBOOL bPassStraight)
{
	nuLONG Angle1, Angle2, nRightSide = 0, nLeftSide = 0, SameRoadIdx = 0;
	nuLONG RightIndex[16] = {0}, LeftIndex[16] = {0}, nAheadRoadCnt = 0;
	NextCrossRoad.nAngle = _EMPTY_STRUCT;
	ExtendCrossRoadInfo.nAngle = _EMPTY_STRUCT;
	NEXTROAD SwapCrossRoad = {0};
	nuBOOL bFindSameRoad = nuFALSE;
	nuINT i = 0, nRoadIdx = -1;
	if(CrossRoadInfo.nRoadCount == 1)
	{		
		Angle1 = nulAtan2(CrossRoadInfo.ptRoadMapped.y - CrossRoadInfo.ptRoadNextCoor.y,
			              CrossRoadInfo.ptRoadMapped.x - CrossRoadInfo.ptRoadNextCoor.x);
		Angle2 = nulAtan2(CrossRoadInfo.ptRoadMapped.y - CrossRoadInfo.pCrossRoad[0].ptEnd.y,
						  CrossRoadInfo.ptRoadMapped.x - CrossRoadInfo.pCrossRoad[0].ptEnd.x);

		CrossRoadInfo.pCrossRoad[0].nAngle = Angle2 - Angle1;
		if(CrossRoadInfo.pCrossRoad[0].nAngle > 180)
		{
			CrossRoadInfo.pCrossRoad[0].nAngle -= 360;
		}
		else if(CrossRoadInfo.pCrossRoad[0].nAngle < (-180))
		{
			CrossRoadInfo.pCrossRoad[0].nAngle += 360;
		}
		CrossRoadInfo.pCrossRoad[0].nAngle = _EXTEND_STRUCTURE;
		ExtendCrossRoadInfo = CrossRoadInfo.pCrossRoad[0];
		return;
	}
	for(i = 0; i < CrossRoadInfo.nRoadCount; i++)
	{
		Angle1 = nulAtan2(CrossRoadInfo.ptRoadMapped.y - CrossRoadInfo.ptRoadNextCoor.y,
			              CrossRoadInfo.ptRoadMapped.x - CrossRoadInfo.ptRoadNextCoor.x);
		Angle2 = nulAtan2(CrossRoadInfo.ptRoadMapped.y - CrossRoadInfo.pCrossRoad[i].ptEnd.y,
						  CrossRoadInfo.ptRoadMapped.x - CrossRoadInfo.pCrossRoad[i].ptEnd.x);

		CrossRoadInfo.pCrossRoad[i].nAngle = Angle2 - Angle1;
		if(CrossRoadInfo.pCrossRoad[i].nAngle > 180)
		{
			CrossRoadInfo.pCrossRoad[i].nAngle -= 360;
		}
		else if(CrossRoadInfo.pCrossRoad[i].nAngle < (-180))
		{
			CrossRoadInfo.pCrossRoad[i].nAngle += 360;
		}
		if(NURO_ABS(CrossRoadInfo.pCrossRoad[i].nAngle) >= 100 || CrossRoadInfo.pCrossRoad[i].nNameAddr == -1)
		{
			CrossRoadInfo.pCrossRoad[i].nAngle = _EMPTY_STRUCT;
		}
		if(nuMemcmp(CrossRoadInfo.pCrossRoad[i].wsRoadName,wsRoadName, sizeof(nuWCHAR) * 12) == 0)
		{
			CrossRoadInfo.pCrossRoad[i].nAngle = _EXTEND_STRUCTURE;
			ExtendCrossRoadInfo = CrossRoadInfo.pCrossRoad[i];
		}
	}
	for(i = 0; i < CrossRoadInfo.nRoadCount; i++)
	{
		for(nuINT j = 0; j < CrossRoadInfo.nRoadCount; j++)
		{
			if(CrossRoadInfo.pCrossRoad[i].nAngle < CrossRoadInfo.pCrossRoad[j].nAngle)
			{
				SwapCrossRoad = CrossRoadInfo.pCrossRoad[i];
				CrossRoadInfo.pCrossRoad[i] = CrossRoadInfo.pCrossRoad[j];
				CrossRoadInfo.pCrossRoad[j] = SwapCrossRoad;
			}
		}
	}
	if(ExtendCrossRoadInfo.nAngle != _EXTEND_STRUCTURE)
	{
		for(i = 0; i < CrossRoadInfo.nRoadCount; i++)
		{
			if(NURO_ABS(CrossRoadInfo.pCrossRoad[i].nAngle) <= 20)
			{
				CrossRoadInfo.pCrossRoad[i].nAngle = _EXTEND_STRUCTURE;
				ExtendCrossRoadInfo = CrossRoadInfo.pCrossRoad[i];
				break;
			}
		}
	}
	for(i = 0; i < CrossRoadInfo.nRoadCount; i++)
	{
		for(nuINT j = 0; j < CrossRoadInfo.nRoadCount; j++)
		{
			if(CrossRoadInfo.pCrossRoad[i].nAngle != _EMPTY_STRUCT && CrossRoadInfo.pCrossRoad[i].nAngle != _EXTEND_STRUCTURE &&
				0 == nuMemcmp(CrossRoadInfo.pCrossRoad[i].wsRoadName, CrossRoadInfo.pCrossRoad[j].wsRoadName, sizeof(nuWCHAR) * 12))
			{
				if(CrossRoadInfo.pCrossRoad[i].nAngle > CrossRoadInfo.pCrossRoad[j].nAngle)
				{
					CrossRoadInfo.pCrossRoad[i].nAngle = _EMPTY_STRUCT;
				}
			}
		}
	}
	for(i = 0; i < CrossRoadInfo.nRoadCount; i++)
	{
		if(CrossRoadInfo.pCrossRoad[i].nAngle == _EMPTY_STRUCT ||
			CrossRoadInfo.pCrossRoad[i].nAngle == _EXTEND_STRUCTURE)
		{
			continue;
		}
		NextCrossRoad = CrossRoadInfo.pCrossRoad[i];
		return;
	}
}
nuVOID CHUDCOMCTRL::ELEAD_SendFreeNaviRoadName(NEXTROAD NextCrossRoad, nuLONG ID)
{
	if(ID == 0x01)
	{
		if(0 != nuMemcmp(NextCrossRoad.wsRoadName,m_wsELEAD_FirstRoadName, sizeof(nuWCHAR) * _ELEADROADNAMELEN))
		{
			nuMemset(m_wsELEAD_FirstRoadName, 0, sizeof(m_wsELEAD_FirstRoadName));
			nuWcscpy(m_wsELEAD_FirstRoadName, NextCrossRoad.wsRoadName);
		}
		else
		{
			return ;
		}
	}
	else if(ID == 0x03)
	{
		if(0 != nuMemcmp(NextCrossRoad.wsRoadName,m_wsELEAD_SencondRoadName, sizeof(nuWCHAR) * _ELEADROADNAMELEN))
		{
			nuMemset(m_wsELEAD_SencondRoadName, 0, sizeof(m_wsELEAD_SencondRoadName));
			nuWcscpy(m_wsELEAD_SencondRoadName, NextCrossRoad.wsRoadName);
		}
		else
		{
			return ;
		}
	}
	COPYDATASTRUCT   CDS	= {0};
	nuBYTE	Buffer[512]		= {0};
	nuINT i					= 0, WordIndex = 0, WordLen = 0;
	nuBYTE	TmpChar[512]    = {0};
	nuINT WcharLen			= 0, Len = 0;
	WcharLen = nuWcslen(NextCrossRoad.wsRoadName);
	if(WcharLen > 6)
	{
		WcharLen = 6;
		for(i = 0; i < WcharLen; i++)
		{
			WordIndex+=enc_unicode_to_utf8((nuULONG)NextCrossRoad.wsRoadName[i], &TmpChar[WordIndex], Len);
			if(WordIndex > 18 || NextCrossRoad.wsRoadName[i] == 0)
			{
				break;
			}
			WordLen = WordIndex;
		}
	}
	else if(WcharLen == 0)
	{
		WcharLen = 8;
	}
	
	Buffer[0]	= 0x99;
	Buffer[1]	= WordLen + 4;
	Buffer[2]	= ID;
	Buffer[3]	= WordLen;
	nuMemcpy(&Buffer[4], TmpChar, WordLen);
	CDS.cbData  = Buffer[1] + 1;
	for(i = 1; i < CDS.cbData; i++)
	{
		Buffer[CDS.cbData] ^= Buffer[i];
	}
	CALL_BACK_PARAM cbParam = {0};
	cbParam.lCallID   = 1;
	cbParam.lDataLen  = CDS.cbData + 1;
	cbParam.pVOID	  = (nuVOID*)Buffer;
	g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
}
nuVOID CHUDCOMCTRL::ELEAD_SendFreeCrossRoadDis(NUROPOINT ptCar, NEXTROAD NextCrossRoad, nuLONG ID)
{
	COPYDATASTRUCT CDS  = {0};
	CDS.dwData			= ELEAD_HUD_DATA ;
	nuBYTE	Buffer[512] = {0};
	nuLONG dx = 0, dy = 0, lDistance = 0;
	if(NextCrossRoad.ptFirst.x == 0 || NextCrossRoad.ptFirst.y == 0)
	{
		lDistance = 0;
	}
	else
	{
		dx = NextCrossRoad.ptFirst.x - ptCar.x;
		dy = NextCrossRoad.ptFirst.y - ptCar.y;
		lDistance = nuSqrt(dx*dx + dy*dy);
		if(lDistance > 1000)
		{
			Buffer[6] = 1;
			lDistance /= 100;
		}
		else
		{
			lDistance *= 10;
		}
	}
	Buffer[0]   = 0x99;
	Buffer[1]   = 6;
	Buffer[2]   = ID;
	Buffer[3]   = lDistance & 0xFF;
	Buffer[4]   = ((lDistance & 0xFF00) >> 8);
	Buffer[5]   = ((lDistance & 0xFF0000) >> 16);
	
	if(m_ELEAD_State_Free_Distance != lDistance)
	{
		m_ELEAD_State_Free_Distance = lDistance;
	}
	else
	{
		return ;
	}
	CDS.cbData = 8;
	for(nuINT i = 1; i < CDS.cbData; i++)
	{
		Buffer[7] ^= Buffer[i];
	}
	CALL_BACK_PARAM cbParam = {0};
	cbParam.lCallID   = 1;
	cbParam.lDataLen  = CDS.cbData;
	cbParam.pVOID	  = (nuVOID*)Buffer;
	g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
}
nuVOID CHUDCOMCTRL::ELEAD_SelfDirect(nuBYTE nELEADTurnFlag)
{
	nuBYTE	Buffer[512]		= {0};	
	CALL_BACK_PARAM cbParam = {0};
	
	switch(nELEADTurnFlag)
	{
	case Elead_Roundabout_Right://96 03 01 15
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x15;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Roundabout_Right//96 03 01 15\n");
#endif
		break;
	case Elead_Roundabout_Left://96 03 01 16
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x01;
		Buffer[3]				= 16;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Roundabout_Left//96 03 01 16\n");
#endif
		break;
	case Elead_In_TrafficCircle://96 03 01 17
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x17;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_In_TrafficCircle//96 03 01 17\n");
#endif
		break;
	case Elead_Straight://96 06 01 0B FF 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0B;
		Buffer[4]				= 0xFF;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Straight//96 06 01 0B FF 00 00\n");
#endif
		break;
	case Elead_Slight_Left://96 09 01 0D 20 00 03 40 C0 E0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0x20;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x03;
		Buffer[7]				= 0x40;
		Buffer[8]				= 0xC0;
		Buffer[9]				= 0xE0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Slight_Left//96 09 01 0D 20 00 03 40 C0 E0\n");
#endif
		break;
	case Elead_Slight_Right://96 09 01 0D E0 00 03 40 C0 E0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0xE0;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x03;
		Buffer[7]				= 0x40;
		Buffer[8]				= 0xC0;
		Buffer[9]				= 0xE0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Slight_Right//96 09 01 0D E0 00 03 40 C0 E0\n");
#endif
		break;
	case Elead_Keep_Left://96 09 01 0D 20 00 03 00 40 C0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0x20;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x03;
		Buffer[7]				= 0x00;
		Buffer[8]				= 0x40;
		Buffer[9]				= 0xC0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Keep_Left//96 09 01 0D 20 00 03 00 40 C0\n");
#endif
		break;
	case Elead_Keep_Right://96 09 01 0D E0 00 03 00 40 C0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0xE0;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x03;
		Buffer[7]				= 0x00;
		Buffer[8]				= 0x40;
		Buffer[9]				= 0xC0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Keep_Right//96 09 01 0D E0 00 03 00 40 C0\n");
#endif
		break;
	case Elead_Left://96 06 01 0D 40 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0x40;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Left//96 06 01 0D 40 00 00\n");
#endif
		break;
	case Elead_Right://96 06 01 0D C0 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0xC0;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Right//96 06 01 0D C0 00 00\n");
#endif
		break;
	case Elead_Hard_Left://96 09 01 0D 60 00 03 00 40 C0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0x60;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x03;
		Buffer[7]				= 0x00;
		Buffer[8]				= 0x40;
		Buffer[9]				= 0xC0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Hard_Left//96 09 01 0D 60 00 03 00 40 C0\n");
#endif
		break;
	case Elead_Hard_Right://96 09 01 0D A0 00 03 00 40 C0
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x09;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x0D;
		Buffer[4]				= 0xA0;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x30;
		Buffer[7]				= 0x00;
		Buffer[8]				= 0x40;
		Buffer[9]				= 0xC0;
		cbParam.lDataLen		= 11;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Hard_Right//96 09 01 0D A0 00 03 00 40 C0\n");
#endif
		break;
	case Elead_Uturn_Left://96 06 01 19 40 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x19;
		Buffer[4]				= 0x40;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Uturn_Left//96 06 01 19 40 00 00\n");
#endif
		break;
	case Elead_Uturn_Right://96 06 01 19 C0 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x19;
		Buffer[4]				= 0xC0;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Uturn_Right//96 06 01 19 C0 00 00\n");
#endif
		break;
	case Elead_Destination://96 06 01 03 00 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x03;
		Buffer[4]				= 0x00;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Destination//96 06 01 03 00 00 00\n");
#endif
		break;
	case Elead_LeaveNaviLine://96 06 01 06 00 00 00
		Buffer[0]				= 0x96;
		Buffer[1]				= 0x06;
		Buffer[2]				= 0x01;
		Buffer[3]				= 0x06;
		Buffer[4]				= 0x00;
		Buffer[5]				= 0x00;
		Buffer[6]				= 0x00;
		cbParam.lDataLen		= 8;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_LeaveNaviLine//96 06 01 06 00 00 00\n");
#endif
		break;
	case Elead_Enter_Bridge://98 03 05 00
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x05;
		Buffer[3]				= 0x00;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enter_Bridge//98 03 05 00\n");
#endif
		break;
	case Elead_Enterfreeway_Left://98 03 02 01
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x02;
		Buffer[3]				= 0x01;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enterfreeway_Left//98 03 02 01\n");
#endif
		break;
	case Elead_Enterfreeway_Right://98 03 02 02
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x02;
		Buffer[3]				= 0x02;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enterfreeway_Right://98 03 02 02\n");
#endif
		break;
	case Elead_Exitfreeway_Left://98 03 02 03
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x02;
		Buffer[3]				= 0x03;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Exitfreeway_Left://98 03 02 03\n");
#endif
		break;
	case Elead_Exitfreeway_Right://98 03 02 04
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x02;
		Buffer[3]				= 0x04;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Exitfreeway_Right://98 03 02 04\n");
#endif
		break;
	case Elead_Toll_Booth://98 03 06 00
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x06;
		Buffer[3]				= 0x00;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Toll_Booth://98 03 06 00\n");
#endif
		break;
	case Elead_Enter_Tunnel://98 03 07 00
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x07;
		Buffer[3]				= 0x00;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enter_Tunnel://98 03 07 00\n");
#endif
		break;
	case Elead_Enterferry_Left://98 03 03 01
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x03;
		Buffer[3]				= 0x01;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enterferry_Left://98 03 03 01\n");
#endif
		break;
	case Elead_Enterferry_Right://98 03 03 02
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x03;
		Buffer[3]				= 0x02;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Enterferry_Right://98 03 03 02\n");
#endif
		break;
	case Elead_Exitferry_Left://98 03 03 03
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x03;
		Buffer[3]				= 0x03;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Exitferry_Left://98 03 03 03\n");
#endif
		break;
	case Elead_Exitferry_Right://98 03 03 04
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x03;
		Buffer[3]				= 0x04;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Exitferry_Right://98 03 03 04\n");
#endif
		break;
	case Elead_Exitferry://0x98 0x03 0x03 0x05
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x03;
		Buffer[3]				= 0x05;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Exitferry://0x98 0x03 0x03 0x05\n");
#endif
		break;
	case Elead_Takeramp_Right://98 03 08 00
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x08;
		Buffer[3]				= 0x00;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Takeramp_Right://98 03 08 00\n");
#endif
		break;
	case Elead_Takeramp_Left://98 03 09 00
		Buffer[0]				= 0x98;
		Buffer[1]				= 0x03;
		Buffer[2]				= 0x09;
		Buffer[3]				= 0x00;
		cbParam.lDataLen		= 5;
#ifdef DANIEL_DEBUG
		printf("NURO Send Elead_Takeramp_Left://98 03 09 00\n");
#endif
		break;
	default:
		break;
	}
	for(nuINT i = 0; i < cbParam.lDataLen; i++)
	{
		Buffer[cbParam.lDataLen - 1] ^= Buffer[i];
	}
	cbParam.nCallType = CALLBACK_TYPE_SEND_HUD;
	cbParam.lCallID   = ELEAD_HUD_DATA;
	cbParam.pVOID	  = (nuVOID*)Buffer;
	g_fileSysApi.pGdata->pfNaviThreadCallBack(&cbParam);
}
#endif
