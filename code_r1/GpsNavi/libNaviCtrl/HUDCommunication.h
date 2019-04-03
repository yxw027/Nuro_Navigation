#if !defined(AFX_HUDCOMCTRL_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_)
#define AFX_HUDCOMCTRL_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "nurotypes.h"
#include "LoadDLL_NECVOX.h"
//#include "nuPublic.h"
//#include "nuClib.h"

#ifndef ANDROID
#include "windows.h"
#endif

#define OPENCOM_TRUE		1
#define HANDLE_INVALID		2
#define GETCOMMSATE_FAIL	3
#define SETCOMMSTATE_FAIL	4
#define GETCOMMTIMEOUT_FAIL 5
#define SETCOMMTIMEOUT_FAIL	6
#define SETCOMMMASK_FAIL    7

#define _DIRECT_NORTH	     0x00
#define _DIRECT_NORTH_EAST   0x01
#define _DIRECT_EAST		 0x02
#define _DIRECT_SOUTH_EAST	 0x03
#define _DIRECT_SOUTH		 0x04
#define _DIRECT_SOUTH_WEST	 0x05
#define _DIRECT_WEST		 0x06
#define _DIRECT_NORTH_WEST	 0x07

#define _HUDROADNAMELEN		 12 //WCHAR Lenght
#define _ELEADROADNAMELEN	 15
#define PANA_HUD_DATA_TYPE   0x65
#define ELEAD_HUD_DATA		 1  //#define          NAVI_CPDPARAM_BAP_MSG               1 

typedef struct tag_HUDData
{
	nuBYTE   nRoadType;
	nuBYTE   nCCD;
	nuSHORT  nTurnFlag;
	nuSHORT  nAngle;
	nuWORD   nSpeedLimit;
	nuWORD   nSpeed;
	nuLONG	 nDistance;
	nuLONG   nIconType;
	nuCHAR   strTelNum[10];
	nuBYTE   nbRoadSpeedWarning;
	nuBOOL	 bNavi:1;
	nuBYTE   bFixRoad:7;
	nuWCHAR  wsNowRoadName[13];
	nuWCHAR  wsNextRoadName[13];
	NOW_LAME_GUIDE  LaneGuide; 
}HUDData, *pHUDData;

#define _EMPTY_STRUCT 1000
class CHUDCOMCTRL  
{
public:
	typedef struct tagDCB {
		nuDWORD DCBlength;      /* sizeof(DCB)                     */
		nuDWORD BaudRate;       /* Baudrate at which running       */
		nuDWORD fBinary: 1;     /* Binary Mode (skip EOF check)    */
		nuDWORD fParity: 1;     /* Enable parity checking          */
		nuDWORD fOutxCtsFlow:1; /* CTS handshaking on output       */
		nuDWORD fOutxDsrFlow:1; /* DSR handshaking on output       */
		nuDWORD fDtrControl:2;  /* DTR Flow control                */
		nuDWORD fDsrSensitivity:1; /* DSR Sensitivity              */
		nuDWORD fTXContinueOnXoff: 1; /* Continue TX when Xoff sent */
		nuDWORD fOutX: 1;       /* Enable output X-ON/X-OFF        */
		nuDWORD fInX: 1;        /* Enable input X-ON/X-OFF         */
		nuDWORD fErrorChar: 1;  /* Enable Err Replacement          */
		nuDWORD fNull: 1;       /* Enable Null stripping           */
		nuDWORD fRtsControl:2;  /* Rts Flow control                */
		nuDWORD fAbortOnError:1; /* Abort all reads and writes on Error */
		nuDWORD fDummy2:17;     /* Reserved                        */
		nuWORD wReserved;       /* Not currently used              */
		nuWORD XonLim;          /* Transmit X-ON threshold         */
		nuWORD XoffLim;         /* Transmit X-OFF threshold        */
		nuBYTE ByteSize;        /* Number of bits/byte, 4-8        */
		nuBYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
		nuBYTE StopBits;        /* 0,1,2 = 1, 1.5, 2               */
		nuCHAR XonChar;         /* Tx and Rx X-ON character        */
		nuCHAR XoffChar;        /* Tx and Rx X-OFF character       */
		nuCHAR ErrorChar;       /* Error replacement char          */
		nuCHAR EofChar;         /* End of Input character          */
		nuCHAR EvtChar;         /* Received Event character        */
		nuWORD wReserved1;      /* Fill for now.                   */
	} NURO_DCB, *LPNURO_DCB;
	typedef struct tagCOMMTIMEOUTS {
		nuDWORD ReadIntervalTimeout;          /* Maximum time between read chars. */
		nuWORD ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
		nuDWORD ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
		nuDWORD WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
		nuDWORD WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
	} NURO_COMMTIMEOUTS,*LPNURO_COMMTIMEOUTS;

	#define NURO_GENERIC_READ						(0x80000000L)
	#define NURO_GENERIC_WRITE						(0x40000000L)
	#define NURO_GENERIC_EXECUTE					(0x20000000L)
	#define NURO_GENERIC_ALL						(0x10000000L)

	//For dwCreationDisposition
	#define NURO_CREATE_NEW							1
	#define NURO_CREATE_ALWAYS						2
	#define NURO_OPEN_EXISTING						3
	#define NURO_OPEN_ALWAYS						4

	//For dwFlagsAndAttributes
	#define NURO_FILE_ATTRIBUTE_READONLY            0x00000001  
	#define NURO_FILE_ATTRIBUTE_HIDDEN              0x00000002  
	#define NURO_FILE_ATTRIBUTE_SYSTEM              0x00000004  
	#define NURO_FILE_ATTRIBUTE_DIRECTORY           0x00000010  
	#define NURO_FILE_ATTRIBUTE_ARCHIVE             0x00000020  
	#define NURO_FILE_ATTRIBUTE_ENCRYPTED           0x00000040  
	#define NURO_FILE_ATTRIBUTE_NORMAL              0x00000080  
	#define NURO_FILE_ATTRIBUTE_TEMPORARY           0x00000100  
	#define NURO_FILE_ATTRIBUTE_SPARSE_FILE         0x00000200  
	#define NURO_FILE_ATTRIBUTE_REPARSE_POINT       0x00000400  
	#define NURO_FILE_ATTRIBUTE_COMPRESSED          0x00000800  
	#define NURO_FILE_ATTRIBUTE_OFFLINE             0x00001000  
	#define NURO_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 0x00002000  

	#define NURO_EV_RXCHAR           0x0001  // Any Character received
	#define NURO_EV_RXFLAG           0x0002  // Received certain character
	#define NURO_EV_TXEMPTY          0x0004  // Transmitt Queue Empty
	#define NURO_EV_CTS              0x0008  // CTS changed state
	#define NURO_EV_DSR              0x0010  // DSR changed state
	#define NURO_EV_RLSD             0x0020  // RLSD changed state
	#define NURO_EV_BREAK            0x0040  // BREAK received
	#define NURO_EV_ERR              0x0080  // Line status error occurred
	#define NURO_EV_RING             0x0100  // Ring signal detected
	#define NURO_EV_PERR             0x0200  // Printer error occured
	#define NURO_EV_RX80FULL         0x0400  // Receive buffer is 80 percent full
	#define NURO_EV_EVENT1           0x0800  // Provider specific event 1
	#define NURO_EV_EVENT2           0x1000  // Provider specific event 2

	#define NURO_NOPARITY            0
	#define NURO_ODDPARITY           1
	#define NURO_EVENPARITY          2
	#define NURO_MARKPARITY          3
	#define NURO_SPACEPARITY         4


	#define NURO_ONESTOPBIT          0
	#define NURO_ONE5STOPBITS        1
	#define NURO_TWOSTOPBITS         2

	CHUDCOMCTRL();
	virtual ~CHUDCOMCTRL();
	nuBOOL bWriteCom(nuBYTE *SEIIBuf_Type1, nuBYTE *SEIIBuf_Type2, 
							   nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3);
	nuVOID CalHUDData(nuVOID *pHUDAddress, nuBYTE *SEIIBuf_Type1, nuBYTE *SEIIBuf_Type2, 
							   nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3);
	nuVOID CalPanaHUDData(nuVOID *pHUDAddress);
	nuVOID Free();
	nuINT  bOpenCom(const nuTCHAR *tsPath);
	nuVOID bCloseCom();
	nuLONG GetHUDIconType(nuLONG IconType);
	nuLONG GetHUDListIconType(nuLONG IconType);
	nuLONG GetHUDNaviListIconType(nuLONG IconType);
	nuLONG lChangeIconIndex(nuLONG c_lIconIndex);
	nuVOID DebugMessage(nuLONG lValue, nuCHAR *Tempstr);
	nuVOID DebugMessage_BYTE(nuBYTE *Tempstr, nuLONG Size);
#ifdef NURO_OS_WINCE
	nuVOID ELead_SendMessage(nuBYTE *SendMessage, nuLONG Len, nuVOID *pHUDAddress);
	nuVOID SEII_HUD(pHUDData pHUDDisplay);

	nuVOID ELEAD_SendISNavi(nuLONG bNavi);
	nuVOID ELEAD_SendTurnFlag(nuLONG IconType, nuLONG TurnFlag);
	nuVOID ELEAD_SendDistance(nuLONG lDistance, nuLONG lTurnFlag);
	nuVOID ELEAD_SendSpeed(nuLONG lSpeed);
	nuVOID ELEAD_SendSpeedAndCCD(nuLONG lSpeedLimit, nuBOOL bCCD);
	nuVOID ELEAD_SendNextRoadName(nuWCHAR *wsRoadName);
	nuVOID ELEAD_SendCurrentRoadName(nuWCHAR *wsRoadName);
	nuVOID ELEAD_MapDirect(nuSHORT nAngle);
	nuVOID ELEAD_ColFreeNaviRoadName(nuWCHAR *wsRaodName);
	nuVOID ELEAD_SelectRoad_ForFreeNavi(nuWCHAR *wsRoadName, CROSSROADINFO &CrossRoadInfo, NEXTROAD &NextCrossRoad1, 
		NEXTROAD &NextCrossRoad2, NEXTROAD &ExtendCrossRoadInfo, nuBOOL bPassStraight);
	nuVOID ELEAD_SendFreeNaviRoadName(NEXTROAD NextCrossRoad, nuLONG ID);
	nuVOID ELEAD_SendFreeCrossRoadDis(NUROPOINT ptCar, NEXTROAD NextCrossRoad, nuLONG ID);
	nuVOID ELEAD_SelfDirect(nuBYTE nELEADTurnFlag);
	nuVOID ELEAD_OffRoad();
#endif
	nuINT enc_unicode_to_utf8(nuULONG unic, nuBYTE* lputf, nuINT utfsize);
	LoadDLL_NECVOX m_LoadDLL_NECVOX;

	nuHANDLE  m_hComPort;	// set handle of the COM port
	nuBOOL    m_bInitialOK;
	nuLONG	  m_lHUDType;
	nuLONG	  m_lEleadDistance;

	nuHWND	  m_ELeadHudhwnd;
	nuHWND	  m_hwndPana;

	nuBOOL    m_ELEAD_State_bNavi;
	nuLONG	  m_ELEAD_State_Speed;
	nuLONG    m_ELEAD_State_SpeedLimit;
	nuLONG    m_ELEAD_State_CCD;
	nuLONG	  m_ELEAD_State_TurnFlag;
	nuLONG	  m_ELEAD_State_Distance;
	nuLONG	  m_ELEAD_State_Free_Distance;
	nuLONG	  m_ELEAD_State_IconType;
	nuWCHAR	  m_ELEAD_State_RoadName[_ELEADROADNAMELEN];
	nuWCHAR	  m_ELEAD_State_NowRoadName[_ELEADROADNAMELEN];
	nuWCHAR	  m_wsELEAD_FirstRoadName[_ELEADROADNAMELEN];
	nuWCHAR	  m_wsELEAD_SencondRoadName[_ELEADROADNAMELEN];
	nuLONG	  m_ELEAD_nMapIdx;
	nuLONG	  m_ELEAD_nBlkIdx;
	nuLONG	  m_ELEAD_nRoadIdx;
	nuLONG    m_ELEAD_State_FreeRoadName_Count;

	nuLONG    m_nTmpFlag;
	nuLONG	  m_nNaviState;
};

#endif // !defined(AFX_GLIDEMOVE_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_)
