// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: NaviThread.h,v 1.16 2010/08/25 09:32:20 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/08/25 09:32:20 $
// $Locker:  $
// $Revision: 1.16 $
// $Source: /home/nuCode/libNaviCtrl/NaviThread.h,v $
//////////////////////////////////////////////////////////////////
#if !defined(AFX_NAVITHREAD_H__B7E9E0B8_5F8D_40B4_B69E_A3C4217DA4E0__INCLUDED_)
#define AFX_NAVITHREAD_H__B7E9E0B8_5F8D_40B4_B69E_A3C4217DA4E0__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "CodeList.h"
#include "JumpMove.h"
#include "GDataMgr.h"
//#include "GlideMove.h"
#include "ZoomDraw.h"
#include "TurningDraw.h"
#include "TraceFileZ.h"
#include "Demo3DObjects.h"

#include "NuroOpenedDefine.h"
#include "NuroClasses.h"
#include "GApiNaviCtrl.h"
#include "HUDCommunication.h"
#include "NURO_DEF.h"

#define TRACE_ACTION_GETHEAD					1
#define TRACE_ACTION_STARTRECORD				2
#define TRACE_ACTION_CLOSERECORD				3
#define TRACE_ACTION_DELETE						4
#define TRACE_ACTION_SHOW						5
#define TRACE_ACTION_UNSHOW						6
#define TRACE_ACTION_GETPOINT					7
#define TRACE_ACTION_EXPORT						8
#define TRACE_ACTION_IMPORT						9

#define NAVI_STACK_SIZE							(1024*16)
#define GPS_BUFF_SIZE						    (1024)

#define MENU_SHOW_DEFAULT						0x00
#define MENU_SHOW_NOACTION						0x01

#define NAVI_MAX_ERROR							0x03


#define WHO_SETMOVEWAIT_DEFAULT					0x00
#define WHO_SETMOVEWAIT_ZOOM					0x01

#define _WAIT_TYPE_DEFAULT                      0x00
#define _WAIT_TYPE_DRAWEND                      0x01

#define _MAPPING_ROAD							0x01
#define _MAPPING_STREET							0x01
#define _MAPPING_SECTION						0x02
#define _MAPPING_ALLEY							0x03
#define _MAPPING_LANE							0x04

#define _UI_MESSAGE_E_LEAD_GPS					49

#ifdef NURO_OS_WINCE
#include "windows.h"
#define HUD_DATA_TYPE 0x60
#endif
typedef struct tagDRAWTOUI
{
	nuUINT	bUIWaiting:1;
    nuUINT  nWaitType:8;
	nuUINT	nReserve:23;
	nuUINT	nReturnCode;
	nuUINT	nExtend;
}DRAWTOUI, *PDRAWTOUI;

//Added by Daniel ================Choose Road===START========== 20150207//
typedef struct tagROAD_INFO
{
	NUROPOINT	ptCar_In_Road;
	nuDWORD		nBlkIdx;
	nuDWORD		nRoadIdx;	
	nuWORD		nDwIdx;
	nuWORD		nRoadClass;
	nuDWORD		nNameAddr;
	nuDWORD		nDis;
	nuWCHAR		wsRoadName[MAX_LISTNAME_NUM];
}ROAD_INFO, *PROAD_INFO;
//Added by Daniel ================Choose Road===END========== 20150207//

class CNaviThread  
{
public:
	typedef struct tagROADNAMESTR
	{
		nuWORD	nSameTimes;
		nuWORD	nDwIdx;
		nuDWORD	nNameAddr;
		nuBYTE	nRoadClass;
	}ROADNAMESTR, *PROADNAMESTR;

	typedef struct tagGPS_NODE
	{
		nuroPOINT	point;
		nuSHORT		nAngle;
	}GPS_NODE, *PGPS_NODE;

public:
	CNaviThread();
	virtual ~CNaviThread();

public:
	static nuVOID ResetActionState(PACTIONSTATE pActionState);

protected:	
	static nuDWORD ThreadAction(nuPVOID lpVoid);

public:
	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL InitThread();
	nuVOID FreeThread();

	nuBOOL NuroMoveStart(); //

	nuBOOL NuroOnPaint();

	nuUINT NuroKeyDown(nuUINT nKey); //
	nuUINT NuroKeyUp(nuUINT nKey); //
	nuUINT MouseEvent(nuLONG x, nuLONG y, nuBYTE nDownOrUp); //

	nuBOOL NuroJumpToScreenXY(nuLONG x, nuLONG y, nuBOOL bMode = nuFALSE);
	nuBOOL NuroMoveToMapXY(nuLONG x, nuLONG y);
	nuBOOL NuroOperateMapXY(nuLONG x, nuLONG y, nuUINT nCode);

	nuBOOL NuroRotateMap(nuLONG nAngle);//0 rotate to car Angle, 360 rotate to north map

	nuBOOL NuroZoom(nuBOOL bInOrOut);

	nuBOOL NuroSetMarket(nuPVOID lpVoid);
	nuBOOL NuroDelMarket(nuPVOID lpVoid);

	nuBOOL NuroNavigation(nuBYTE type);
	nuBOOL NuroSetUserCfg(nuUINT nCode = SETUSERCFG_NOEXTEND);
//	nuBOOL ShilftNaviThread(nuBOOL bOpenOrClose);
	nuBOOL NuroSetVoice(nuWORD nVoiceNum, nuBOOL bShowIcon = nuTRUE);
	nuPVOID NuroGetRoutingList(int index, int *ptotalCount);
	nuPVOID NuroGetMapInfo(nuUINT nMapCode);
	nuBOOL NuroMessageBox(nuUINT nMsgBoxCode, nuPVOID lpVoid); //
	nuBOOL NuroShowHideButton(nuUINT nButCode, nuBYTE nState); //
	nuBOOL GetTripInfo(nuPVOID lpInfo);
	nuBOOL ReSetTripInfo(nuPVOID lpInfo);
	nuBOOL NuroConfigNavi(nuLONG nDis);

	nuBOOL GpsSendIn(nuBYTE *pBuff, nuUINT nCount);	
	nuVOID IntervalGetGps();
	nuVOID IntervalGetGps2();

	nuVOID DrawGpsFakeMove(PACTIONSTATE pAcState = NULL); 

	nuBOOL NuroSetNaviThread(nuBYTE nState);
	nuBOOL NuroSetDrawInfo(nuPVOID pSetData);

	nuBOOL NuroSetScale(nuLONG nScaleIdx);
	nuBOOL NuroSetMAPPT(nuBYTE nState); // Added by Damon For Save Map PT File

	nuUINT TraceAction(nuUINT nActionType, nuDWORD nCode = 0, nuPVOID lp = NULL);

	nuUINT CheckLocalRoute(PACTIONSTATE pActionState);

	nuBOOL NuroMoveMap(nuBYTE nDirection, nuBOOL bStartOrStop);
	nuBOOL NuroMoveMap1(nuBYTE nDirection, nuBYTE nMoveStep, nuBOOL bStartOrStop);
	nuBOOL NuroTMCRoute(nuLONG lTmcRouteCount);//added by daniel 20110830
	nuBOOL NuroTOBERESETGYRO(); // Added by Damon 20110906
	nuBOOL NuroTOBEGETVERSION(); // Added by Louis 20111110
	nuBOOL NuroGetTMCPathData(nuUINT* pbufcount, nuPVOID pvoid);

	nuUINT NuroSetMemoData(nuPVOID pVoid);
	nuUINT NuroSetMemoPtData(nuPVOID pVoid);

	nuUINT NuroShowMap(nuBYTE nType);

	nuUINT NuroButtonEvent(nuUINT nCode, nuBYTE nButState);
	nuUINT NuroPointToRoad(nuroPOINT& point, nuPVOID lpData);

    	nuUINT  NuroStart3DDemo(nuTCHAR *pts3dDemo);
    	nuUINT  NuroClose3DDemo();

	//@2011.02.16
	nuBOOL  SetDrawInfoMap();
	nuBOOL  SetRTFullView(nuLONG x, nuLONG y, nuUINT nCode); //added by xiaoqin 20120517

	nuVOID  RouteLenghView(PACTIONSTATE pActionState);//added by daniel
	nuBOOL  bUISaveLastPosition();//added by daniel 20111111
	nuUINT  NuroSetMapScreenSize(nuWORD nScreenWidth, nuWORD nScreenHeight); //Added by Xiaoqin 2012.06.18
	nuBOOL  NuroResetGPS( PGPSCFG pGpsSetData);
	nuVOID  UIPlaySound(nuWCHAR *wsTTS);//added by danile 2012119 for sound navi
	nuVOID  SeekKMName(PACTIONSTATE pActionState);//added by danile 20121205
	nuVOID  RoadNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount);//added by danile 20121211
	nuUINT	ReciveUIData(nuLONG DefNum, nuPVOID pUIData1, nuPVOID pUIData2);//added by daniel 20130529
	nuVOID  FirstGetDRGPS();//added by daniel 20130620
	nuINT	nRoadNameMapping(SEEKEDROADDATA &roadData, SEEKEDROADDATA &roadsList);
	nuVOID  HUDGetData();
	nuVOID  DRGetData();
	nuUINT  TunnelSim(PACTIONSTATE pActionState);
	nuUINT  UISendGPSData(const nuPVOID pOrGpsData1, const nuPVOID pOrGpsData2);
	nuUINT  TimeToGetGPS();
	nuBOOL  Nuro_NT_Exit();
	
	#ifdef USE_HUD_DR_CALLBACK 
	NC_HUDCallBackProc m_pfHUDCallBack;
	NC_DRCallBackProc  m_pfDRCallBack;
	nuDWORD nTimeStart; 
	nuDWORD nHudTimeStart;
	bool    m_bTunnel2;
	#endif
	
	nuUINT UI_TRIGGER_TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEvent);
	nuUINT TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEven, PACTIONSTATE pActionStatet);
	nuBOOL SetNaviParser(nuINT nMode,nuINT nType);

	//Added by Daniel ================Choose Road===START========== 20150207//
	nuVOID  NuroJavaData();
	nuUINT Nuro_Choose_Car_Road(nuBYTE nSelect);
	//Added by Daniel ================Choose Road===END========== 20150207//

//==================For SDK======================================//
	nuLONG  NuroUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData);
	nuVOID  NURO_UI_GET_USER_DATA(nuPVOID pUIData);
	nuVOID	TransferCoordinate(nuPVOID pUIData, nuPVOID pUIReturnData, nuBOOL bMapToBmp);
	nuPVOID  NURO_GetNavi_List_Data(nuPVOID pRTlist, nuPVOID pNum, nuPVOID pIndex);
	nuVOID  NuroUIGetPicBuffer(PNURO_PACK_PIC_PARAMETER  pPackPicPara, nuBYTE *pPicBuffer);

	nuVOID  Nuro_MouseEventEx(NUROPOINT ptMouse, nuBYTE nMouseType, nuINT nVelocityX, nuINT nVelocityY);
	nuVOID  Nuro_MouseEventEx2(NUROPOINT ptMouse, nuBYTE nMouseType, nuINT nVelocityX, nuINT nVelocityY);
	nuBOOL  BmpCopyEx(nuBYTE *pDesBmpBuff, nuINT desx, nuINT desy, nuINT desw, nuINT desh, nuBYTE *pSrcBmpBuff, nuINT srcx, nuINT srcy, nuINT srcw, nuINT srch);
	nuPVOID NURO_Get_TMC_NAVI_List(nuPVOID pNum, nuPVOID pTMCList);
//==================For SDK======================================//
protected:	
	nuUINT ThreadPowerOn(PACTIONSTATE pActionState);
	nuUINT CodePreProc(PCODENODE pCodeNode, PACTIONSTATE pActionState);
	nuUINT CodeDrawProc(PCODENODE pCodeNode, PACTIONSTATE pActionState);
	nuUINT CodeNaviProc(PCODENODE pCodeNode, PACTIONSTATE pActionState);

	nuUINT AutoMoveProc(PACTIONSTATE pActionState);
	nuUINT DrawReady(PACTIONSTATE pActionState);
	nuUINT DrawAction(PACTIONSTATE pActionState);
	nuUINT DataUpdate(PACTIONSTATE pActionState);
	nuUINT MouseProc(PCODENODE pCodeNode, PACTIONSTATE pActionState); //

	nuUINT UserCfgProc(PCODENODE pCodeNode, PACTIONSTATE pActionState);

	nuUINT SetMarket();
	nuUINT DelMarket();

	nuUINT GetGpsData(PACTIONSTATE pActionState);
	nuUINT GetGpsDataEx(PACTIONSTATE pActionState);

	nuBYTE CheckNaviFixed(nuUINT nRes, PACTIONSTATE pActionState);
	nuUINT KeyProc(nuUINT nKey, nuBYTE nKeyType, PACTIONSTATE pActionState); //

	nuUINT ButtonEvent(PCODENODE pCodeNode, nuUINT nCode, nuBOOL bUpOrDown = nuTRUE);
	nuUINT MenuEvent(nuUINT nCode, nuBOOL bUpOrDown = nuTRUE); //
	nuUINT MsgBoxEvent();

	//Used in Thread action
	nuUINT StopNavi(PACTIONSTATE pActionState);

	nuVOID DataExchange();

	nuVOID SetMoveWait(nuWORD nType = WAIT_TYPE_MOVE, nuBOOL bOnlyChangeTimer = nuFALSE, nuBYTE nWhoSet = WHO_SETMOVEWAIT_DEFAULT);
	nuVOID CoverFromMoveWait();

	nuBOOL SetMapInfo(nuUINT nMapCode);
	nuBYTE ColBmpExtend();

	nuBOOL DrawCrossZoom(PACTIONSTATE pAcState);
	nuUINT SetScaleProc(nuLONG nNewScaleIdx, PACTIONSTATE pAcState, nuBOOL bZoomingAction = nuTRUE);

	nuUINT PointToRoad(nuLONG x, nuLONG y, nuPVOID lpData);
	
	nuUINT SetDrawDataForUI(); //@ 2011.03.18

	nuVOID EndVoiceHandle();
	nuVOID ColEtag();
	nuVOID ColNearPoi();
	
	nuBOOL JumpRoad(PACTIONSTATE pActionState);
private:
	nuBOOL ChooseCenterRoad(SEEKEDROADDATA& roadCenterOn, PSEEK_ROADS_OUT pSeekRdsOut, SEEKEDROADDATA& oldRoadCarOn);
	nuBOOL ChooseCenterRoadEx(SEEKEDROADDATA& roadCenterOn, PSEEK_ROADS_OUT pSeekRdsOut, SEEKEDROADDATA& oldRoadCarOn);
	nuINT  GetRoadWeight(const SEEKEDROADDATA roadCenterOn, const SEEKEDROADDATA SeekRdsOut);
//Added by Daniel ================Choose Road===START========== 20150207//
	nuVOID Choose_Car_Road(nuBYTE nSelect);
//Added by Daniel ================Choose Road===END========== 20150207//
//========For SDK========================//
	nuVOID NURO_UI_SET_USER_DATA();
	nuVOID NURO_DragZoomDraw(NURO_POINT2 Pt);
	nuVOID DragZoomDraw(PACTIONSTATE pActionState);
	nuPVOID GetNaviListData(PROUTINGLISTNODE pRTlist, nuINT *pNum, nuINT *pIndex);
	nuBOOL ChangeRealPackPic(nuINT nPicID);	
	nuVOID FreeArchivePackPic(nuPVOID pUIPackID, NURO_BMP &Bmp);
	nuBOOL LoadArchivePack(nuINT nPackPathID, nuBOOL bLoadMemory);
	nuBOOL LoadPic(nuINT nPicID, NURO_BMP &Bmp);
//========For SDK========================//
public:
	class CCodeList		m_codeDraw;
	class CCodeList		m_codeNavi;
	class CCodeList		m_codePre;
	class CJumpMove		m_jumpMove;
	//class CGlideMove	m_glideMove;
	class CGDataMgr		m_gDataMgr;
	class CZoomDraw		m_zoomDraw;
	class CTurningDraw	m_turnDraw;
	class CTraceFileZ*	m_pTraceFile;
	class CHUDCOMCTRL       m_HudComCtrl;

	nuHANDLE			m_hThread;
    nuDWORD             m_dwThreadID;

	//Union can used below
	nuUINT				m_MouseDownCode;
	DRAWTOUI			m_drawToUI;
	PMARKETPOINT		m_pMarketPoint;
	nuBOOL				m_bCleanMarket;
	nuBYTE				m_nThdState;
	nuBYTE				m_nPlayedStartNaviState;
	nuBOOL				m_bQuit;
	nuBOOL				m_bOutThreadLoop;
//	nuBOOL				m_bStopThread;
	nuBOOL				m_bPowerOn;
	nuBOOL				m_bGetNewGpsData;
	GPSDATA				m_gpsData;
	GPSDATA				m_gpsDataOld;
	nuDWORD				m_nTotalDisGone;
	nuDWORD				m_nTotalTimeForSpeed;
	nuDWORD				m_nOldTimeForSpeed;
	nuSHORT				m_nHighSpeed;
	nuLONG              m_nGlideMoveMinMS;
	nuBOOL              m_bIsMoving;
	SaveMapBmpProc      m_pfSaveMapBmp;
	NC_DRAR_DATA	    m_DrawDataForUI;
	nuBOOL              m_bSaveMapBmp;
	nuBOOL              m_bUpdateMap;
	nuBOOL			    m_bRouteEnd;
	GPSMODINFO			*pGpsModInfo;
	nuBYTE				m_nUseGPSState;
	nuLONG			    m_nTMCDataCount;
	nuDWORD             m_nAutoScaleTick; // Added by Damon 20120525
	nuLONG			    m_nAutoScaleTime; // Added by Damon 20120525
	nuLONG				m_lUseGPSSource;
	nuLONG			    m_lVenderType;
	nuBOOL				m_bFirstTunnel;
	nuBOOL				m_bFirst;
	nuLONG				m_nDis;
	nuLONG			    m_lDrawSleep;
	NUROPOINT			m_ptTunnelPoint;
	NUROPOINT			m_ptETAG;
	
protected:
	MAPCENTERINFO		m_mapInfo;
	ROUTINGLISTNODE		m_pRoutingList[ROUTINGLIST_PAGE_MAX_NUM];
	nuDWORD				m_nDownTicks;
	nuDWORD				m_nUpTicks;
	nuDWORD				m_nMoveTicks;
	ROADNAMESTR			m_roadNameStr;
private:
	nuLONG		        m_nMarketFixDis;
	nuLONG              m_nMinMoveAngle;
	nuLONG              m_nMinMoveDis;
	nuLONG		        m_nMaxRoadFixDis;
	nuLONG              m_nErrorTimesToReroute;
	nuLONG              m_nDisToEnd;
	//for fake moving
	nuroPOINT	        m_pointOldIcon;
	nuDWORD		        m_nGpsFakeMoveTimer;
	nuBYTE		        m_nGpsFakeDis;
	nuBYTE		        m_nGpsFakeAdd;
	nuBYTE		        m_bGpsFakeMove;
	nuBOOL		        m_bIsDrawing;
	nuBYTE		        m_nNaviErrorTimer;
	nuBYTE		        m_nSvLtPosTimer;
	//For Scale changing
	nuBYTE		        m_bInNoNaviScale;
	nuBYTE		        m_nLastMapDirectMode;
	nuLONG		        m_nScaleNoNavigation;
	nuroPOINT		    m_ptLastMapCenter;
	nuroPOINT		    m_ptMapCenter;
	PACTIONSTATE	    m_pAcState;
	nuBYTE			    m_nLastScaleType;

#if defined(NURO_OS_UCOS) || defined(NURO_OS_ECOS) || defined(NURO_OS_JZ)
	int			        m_pStack[NAVI_STACK_SIZE];
#endif

#ifdef NURO_OS_UCOS
	nuBYTE		        m_nGpsBuff[GPS_BUFF_SIZE];
	nuWORD		        m_nGpsBuffLen;
#endif

	nuINT		        m_nLastCrossIndex; //for ZoomCross
	nuBYTE		        m_nOldNaviState;
	nuWORD		        m_nLastCrossRdIdx;
	nuDWORD		        m_nLastCrossBlkIdx;
	nuBYTE		        m_nMoveScale; //for movemap
	nuBOOL		        m_bKeyDown;
	nuBYTE		        m_nGpsReadyType;
	nuBYTE		        m_nZoomAction; //for Zoom control
	SEEKEDROADDATA	    m_SeekedRoadData;
	SEEKEDROADDATA	    m_NaviRoadCenterOn;
	SEEKEDROADDATA	    m_FreeRoadCenterOn;
	CCDINFO		        m_ccdInfo;
	//for new GPS fake moving
	nuUINT		        m_nGpsFakeTotalTimes;
	nuUINT		        m_nGpsFakeNowTimes;
	nuLONG		        m_nNewGpsTicks;
	nuLONG		        m_nTicksDiff;
	GPS_NODE	        m_gpsNew;
	GPS_NODE	        m_gpsOld;
	GPS_NODE	        m_gpsNow;
    
    CDemo3DObjects     m_demo3D; //For 3D Demo
    nuBYTE             m_nOldMoveType;
	nuBOOL             m_bSimuReachDes; //模拟导航是否到达目的地(add by xiaoqin @2011.11.16)

	//added by daniel 20121205------------------//
	SEEKEDROADDATA	   m_PreRoadCenter;
	UI_DAIWOO_BT	   m_UI_SEND_DAIWOO_BT;    //added by daniel
	UI_DAIWOO_BT	   m_UI_DAIWOO_BT;	  //added by daniel
	ICT_DATA		   m_ICT_DATA;
	nuLONG			   m_lSendEtag;
	nuLONG			   m_lHighwayID;
	nuLONG			   m_lHighWayCrossID;
	nuBOOL             m_bRouteFirst;     //added by daniel
	nuDOUBLE		   m_dSimTick;
	nuLONG		       m_nUseHUD;
	nuLONG			   m_nHUDTick;
	nuLONG			   m_nHUDFlushTime;
	nuLONG			   m_lHUDType;
	nuLONG			   m_nLogHUD;
	nuLONG			   m_nHUDUseODB;
	nuLONG			   m_nGPSMode;
	nuLONG			   m_lSpeedLimit;
	nuLONG			   m_lUseTMC;
	nuBOOL			   m_bSendHUDChange;//綰HUD ㄢ掸Dataゲ斗ユ传癳
	nuBOOL			   m_bFindRoad;
	KMINFO			   m_KMInfo;
	nuWCHAR			   m_wsKilometer[6];
	nuWCHAR			   m_wsKMName[32];
	nuLONG			   m_lKMNum;
	nuLONG			   m_lPreKMNum;
	nuLONG			   m_lPreKMDis;
	nuLONG			   m_lKMTick;
	nuDOUBLE		   m_dAddKiloDis;
	nuDOUBLE		   m_dAddKiloNum;
	nuBOOL			   m_bFullViewMode;
	NUROPOINT		   m_ptEndPt;
	nuLONG			   m_nMapRoadFontSize;
	nuBOOL			   m_bTunnel;
	nuBOOL			   m_bFreeTunnel;
	nuDWORD			   m_lMoveConst;
	nuBOOL			   m_bTMC_Event_Data_Initial;
	nuWCHAR			   m_wsNowRoadName[MAX_LISTNAME_NUM];
	nuBYTE			   m_nFlush_Map_For_TMC;
	nuLONG			   m_lSIMEtag;
	nuLONG			   m_nFreeRoadWeight;
	nuLONG			   m_nNaviRoadWeight;
	ROADNAMESTR		   m_CenterRdName;
	//added by daniel 20121205------------------//

	//=====For SDK============================//
	NUROPOINT 		   m_ptMouseMove1;
	NUROPOINT 		   m_ptMouseMove2;
	NUROPOINT 		   m_ptMouseDown;
	NUROPOINT 		   m_ptMouseUp;
	nuBOOL			   m_bMoveMapType1;
	nuBOOL			   m_bDrawRealPic;
	nuBOOL	  		   m_bJumpMove;
	nuBOOL	  		   m_bMouseDown;
	nuBOOL	  		   m_bMouseUp;
	nuBOOL	  		   m_bDragMove;
	nuBOOL			   m_bDragZoom;
	nuBOOL			   m_bMousZoomDraw;
	nuDOUBLE		   m_fZoomDis;
	nuDOUBLE		   m_fZoomDis2;
	nuLONG			   m_lOldZoomScale;
	NURO_USER_SET_DATA         m_UISetUserConfig;
	nuPVOID			   m_pUIGetData;
	NURO_TMC_EVENT		   m_pUIGetTMCData[100];
	nuINT			   m_nRealPicID;
	NURO_BMP		   m_RealPicBmp;
	OPR_PACK*		   m_pPackID;
	NURO_PACK_PIC_PARAMETER    m_PackPicPara;
	nuLONG			   m_nVelocityX1;
	nuLONG			   m_nVelocityY1;
	MARKETPOINT		   m_MarketPoint;
	DRAWTOUI		   m_TMCForoUI;
	nuINT			   m_nPicDwIdx;
	//=====For SDK============================//

	//Added by Daniel ================Choose Road===START========== 20150207//
	ROAD_INFO			m_Road_Info_Data[SEEK_ROADS_MAX_COUNT];
	ROAD_INFO			m_User_Chosen_Road;
	nuBOOL				m_bChooseRoad;
	nuBYTE				m_nChosenRoadMode;
	nuBYTE				m_nChosenRoadCount;
//Added by Daniel ================Choose Road===ENDT========== 20150207//
#ifdef NURO_OS_WINCE
	HWND				m_Hudhwnd;
#endif
	nuBOOL				m_bSearchNearPoi;
	
	
	nuBOOL  m_bNavi;
	nuBOOL  m_bSetUserData;
	nuINT   m_nTargetCount;
	
	TARTGETINFO TargetDestInfo[MAX_TARGET];
	
	

	nuDWORD m_nB_Blk;
	nuDWORD m_nB_RoadIdx;
	nuINT	m_nJumpRoadWeight[17];
	nuDWORD m_nJumpRoadBlkIdx[17];
	nuDWORD m_nJumpRoadIdx[17];
	nuINT   m_nJumpRoadAdd;
	nuBOOL	m_bJumpRoad;
	nuBOOL  m_bJumpAdd;
	nuINT	m_nType;
	nuBOOL  m_bSetMovePt1;	
	GPSDATA				m_RawGpsData;
};

#endif // !defined(AFX_NAVITHREAD_H__B7E9E0B8_5F8D_40B4_B69E_A3C4217DA4E0__INCLUDED_)
