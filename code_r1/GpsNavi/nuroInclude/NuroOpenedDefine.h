#ifndef __NURO_OPENED_STRUCTS
#define __NURO_OPENED_STRUCTS

#ifdef WIN32
#pragma warning (disable:4819)
#endif

#define MOVE_STOP					0x00
#define MOVE_LEFT					0x01
#define MOVE_UP						0x02
#define MOVE_RIGHT					0x03
#define MOVE_DOWN					0x04
#define MOVE_UPLEFT					0x05
#define MOVE_UPRIGHT				0x06
#define MOVE_DOWNRIGHT				0x07
#define MOVE_DOWNLEFT				0x08

#define MOVE_TOBMPPOINT				0x80

/* mouse type Down, Up or Move */
#define	MOUSE_TYPE_DOWN					0x01
#define MOUSE_TYPE_UP					0x02
#define MOUSE_TYPE_MOVE					0x03

#define KEY_DOWN						0x00
#define KEY_UP							0x01
/* Used in mouse operation */
//0x000~0x2FF For Function code(Button, Menu, and MsgBox all can use it.
#define MOUSE_NOACTION				0//没�??��?
#define MOUSE_MENU					1//Main Menu Button
#define MOUSE_LEFT					2
#define MOUSE_UP					3
#define MOUSE_RIGHT					4
#define MOUSE_DOWN					5
#define MOUSE_LTUP					6
#define MOUSE_RTUP					7
#define MOUSE_LTDN					8
#define MOUSE_RTDN					9//Moveing button
#define MOUSE_2D3D					10//2D 3D Button
#define MOUSE_BACKTOCAR				11//Back to Car Button
#define MOUSE_ZOOMIN				12//Zoom in Button
#define MOUSE_ZOOMOUT				13//Zoom out Button
#define MOUSE_GPSREADY				14//Gps Ready or not button
#define MOUSE_CARICON				15//Car icon Button
#define MOUSE_TARGET				16//Center Target Button 
#define MOUSE_DIRECTION				17//Direction Icon Button
#define MOUSE_ENDMOVE				18
#define MOUSE_SPLITSCREEN			19//Split screen Button
#define MOUSE_EXIT					20//Exit program Button
#define MOUSE_SUBMENU				21//Sub menu button
#define MOUSE_ROUTELIST				22//Routing List Button
#define MOUSE_ROUTEINFO				23//Routing Info Button
#define MOUSE_BACKTOUI				24//Back to UI menu Button
#define MOUSE_STOPNAVI				25//StopNavi Button
#define MOUSE_SHOWLONGLAT			26//Show longitude and latitude Button
/********add @081028*****************/

#define MOUSE_MAPDISABLE			 27//规�?完�??�Disable??
#define MOUSE_ROADNAME				28  //?�示?��?路�?�?
#define MOUSE_CITYNAME				29  //?�示?��??��?�?
#define MOUSE_BUTTONMID				30  //?�示?��?下�?button，�?路�??��??�称?�示?��???
#define MOUSE_MAPSCALE				31  //?�示当�?比�?尺�??�景??
#define MOUSE_MAPSCALENUM			32  //?�示当�?比�?尺�???
#define MOUSE_Voice                 81

#define MOUSE_SETSTART				0x21//Set Start point
#define MOUSE_SETPASSNODE			0x22//Set mid point
#define MOUSE_SETEND				0x23//Set target point
#define MOUSE_GUIDE					0x24//Navigation but stop car
#define MOUSE_SIMULATION			0x28//start Simulation
#define MOUSE_GPSNAVI				0x30//start GPS navigating
#define	MOUSE_RETURNSTART			0x38//Return start point
#define MOUSE_ENTERPOI				0x39//POI search button
#define MOUSE_ONLEYROUTE			0x40//same to MOUSE_GUIDE
#define MOUSE_GPSINFO				0x41//View GPS infomation Button
#define MOUSE_NAVIINFO				0x42//View Navigation information
#define MOUSE_ADDNAVIPT     		0x43//Add navigatoin point button
#define MOUSE_ADDMEMO				0x44//Add momo button
#define MOUSE_ENTERHOME				0x45
#define MOUSE_NAVIINFOPANA          0x46
#define MOUSE_POIROADCROSS			0x47
#define MOUSE_TV					0x48
#define MOUSE_ADDNAVIPTOFFICE		0x49
#define MOUSE_ENTEROFFICE			0x61

/********add @081028*****************/
#define MOUSE_MAPSPEED				0x50  //?�示当�??�车??
#define MOUSE_MAPVOICE				0x51  //?�示声音
#define MOUSE_MAPDEMO				0x52  //测�??�显示�?试图
#define MOUSE_LIMITSPEED			0x53  //?�示?�路?��??�车??
#define MOUSE_MAPROADIN				0x54  //子面三�???路口?�大
#define MOUSE_MAPROUTLIST			0x55  //子面三�???行�?�?
#define MOUSE_MAPCARINFO			0x56 //子面三�???�?
#define MOUSE_MAPROUTPRE			0x57 //导航路�??�示?�时?��?翻页?�能??
#define MOUSE_MAPROUTNEXT			0x58 //导航路�??�示?�时?��?翻页?�能??
#define MOUSE_MAPSLIPTBK			0x59 //?�割屏�??��??�图
#define MOUSE_MAPZOOMBMP			0x5A //?�示?��??�放大地?��?位置
#define MOUSE_MAPSYSTIME			0x5B //?�示?�时??
#define MOUSE_ANIMATION				0x5C //规�??��??�画?��?
#define MOUSE_SHOWCENTERNEARPOI		0x5D  //POI
#define MOUSE_SACESCREEN			0x60 //保�?屏�??�钮
#define MOUSE_MAPCCD				0x62

/********add @20081107***************/
#define MOUSE_BUT_NOWPOSITION       144
#define MOUSE_BUT_SURE              145
#define MOUSE_BUT_BACK1             146
#define MOUSE_BUT_BACK2             147
#define MOUSE_BUT_ROUTING           148
#define MOUSE_BUT_SUBMENU           200
#define MOUSE_BUT_SMALL_NOWPOI      201
#define MOUSE_BUT_SMALL_BACK        202

/***********add Damon 20090929**********/
#define MOUSE_BUT_ECOS_MENU         210

#define MOUSE_BUT_NAVI				249
#define MOUSE_BUT_SPEED_DOWN		250
#define MOUSE_BUT_SPEED_UP			251
#define MOUSE_BUT_SIM_START			252
#define MOUSE_BUT_SIM_PAUSE			253
#define MOUSE_BUT_SIM_STOP			254

/***********add Wanyu 20101206**********/
#define MOUSE_ROUTE1                210
#define MOUSE_ROUTE2                211
#define MOUSE_ROUTE3                212
#define MOUSE_ROUTE4                213


#define MOUSE_HIDEMENU				0xFF//Hide submenu code

#define MOUSE_DISABLE				0x100//A disable button

//For DrawInfoNavi
#define INFO_NAVI_TIME              0x01
#define INFO_NAVI_LENTH             0x03

//For button used Only
#define MOUSE_BUT_ONLY				0x0200

#define MOUSE_ZOOM_NOACTION			0x03FF//Zoom Screen code

//For MenuCode
#define MOUSE_MENU_OUT				0x0400//Control Show or Hide Menu
#define MOUSE_MENU_HIDE				(MOUSE_MENU_OUT + MOUSE_HIDEMENU)

#define MOUSE_MENU_ENTERPOI			(MOUSE_MENU_OUT + MOUSE_ENTERPOI)
#define MOUSE_MENU_RETURNSTART		(MOUSE_MENU_OUT + MOUSE_RETURNSTART)
#define MOUSE_SUBMENU_SETSTART      (MOUSE_MENU_OUT + MOUSE_SETSTART)
#define MOUSE_SUBMENU_SETPASSNODE   (MOUSE_MENU_OUT + MOUSE_SETPASSNODE)
#define MOUSE_SUBMENU_SETEND		(MOUSE_MENU_OUT + MOUSE_SETEND)

#define MOUSE_MENU_SIMULATION		(MOUSE_MENU_OUT + MOUSE_SIMULATION)
#define MOUSE_MENU_STARTNAVI		(MOUSE_MENU_OUT + MOUSE_GPSNAVI)
#define MOUSE_MENU_ONLYROUTE		(MOUSE_MENU_OUT + MOUSE_ONLEYROUTE)
#define MOUSE_MENU_GPSINFO			(MOUSE_MENU_OUT + MOUSE_GPSINFO)
#define MOUSE_MENU_NAVIINFO			(MOUSE_MENU_OUT + MOUSE_NAVIINFO)
#define MOUSE_MENU_ADDNAVIPT	    (MOUSE_MENU_OUT + MOUSE_ADDNAVIPT)
#define MOUSE_MENU_ADDNAVIPTOFFICE	(MOUSE_MENU_OUT + MOUSE_ADDNAVIPTOFFICE)
#define MOUSE_MENU_ADDMEMO			(MOUSE_MENU_OUT + MOUSE_ADDMEMO)
#define MOUSE_MENU_ENTERHOME		(MOUSE_MENU_OUT + MOUSE_ENTERHOME)
#define MOUSE_MENU_NAVIINFOPANA     (MOUSE_MENU_OUT + MOUSE_NAVIINFOPANA)

#define MOUSE_MENU_DISABLE			(MOUSE_MENU_OUT + MOUSE_DISABLE)
//0x0700~0x07FF only for Menu
#define MOUSE_MENU_USEONLY			0x0600
#define MOUSE_MENU_OUT_CLICK		MOUSE_MENU_USEONLY
#define MOUSE_MENU_SHOWSUB			0x0601

//For MsgBox code
#define _MSGBOX_OK							0x00
#define _MSGBOX_CANCEL						0x01
#define _MSGBOX_NO							0x02
#define _MSGBOX_DLG							0x0E
#define _MSGBOX_OUT							0x0F
#define MOUSE_MSGBOX_OUT					0x0800
#define MOUSE_MSGBOX_DLG					0x0801
#define MOUSE_MSGBOX_BUT					0x0810
#define MOUSE_MSGBOX_BUT_OK					(MOUSE_MSGBOX_BUT + _MSGBOX_OK)
#define MOUSE_MSGBOX_BUT_CANCEL				(MOUSE_MSGBOX_BUT + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_BUT_NO					(MOUSE_MSGBOX_BUT + _MSGBOX_NO)
#define MOUSE_MSGBOX_BUT_DLG				(MOUSE_MSGBOX_BUT + _MSGBOX_DLG)
#define MOUSE_MSGBOX_BUT_OUT				(MOUSE_MSGBOX_BUT + _MSGBOX_OUT)
#define MOUSE_MSGBOX_EXIT_					0x0820
#define MOUSE_MSGBOX_EXIT_OK				(MOUSE_MSGBOX_EXIT_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_EXIT_CANCEL			(MOUSE_MSGBOX_EXIT_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_EXIT_OUT				(MOUSE_MSGBOX_EXIT_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_CHANGEDES_				0x0830
#define MOUSE_MSGBOX_CHANGEDES_OK			(MOUSE_MSGBOX_CHANGEDES_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_CHANGEDES_CANCEL		(MOUSE_MSGBOX_CHANGEDES_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_CHANGEDES_OUT			(MOUSE_MSGBOX_CHANGEDES_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_UISTOPNAVI_			0x0840
#define MOUSE_MSGBOX_UISTOPNAVI_OK			(MOUSE_MSGBOX_UISTOPNAVI_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_UISTOPNAVI_CANCEL		(MOUSE_MSGBOX_UISTOPNAVI_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_UISTOPNAVI_OUT			(MOUSE_MSGBOX_UISTOPNAVI_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_ENTERHOME_				0x0850
#define MOUSE_MSGBOX_ENTERHOME_OK			(MOUSE_MSGBOX_ENTERHOME_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_ENTERHOME_CANCEL		(MOUSE_MSGBOX_ENTERHOME_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_ENTERHOME_OUT			(MOUSE_MSGBOX_ENTERHOME_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_REROUTE_				0x0860
#define MOUSE_MSGBOX_REROUTE_OK				(MOUSE_MSGBOX_REROUTE_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_REROUTE_CANCEL			(MOUSE_MSGBOX_REROUTE_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_REROUTE_OUT			(MOUSE_MSGBOX_REROUTE_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_NAVICONTINUE_          0x0870
#define MOUSE_MSGBOX_NAVICONTINUE_OK		(MOUSE_MSGBOX_NAVICONTINUE_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_NAVICONTINUE_CANCEL	(MOUSE_MSGBOX_NAVICONTINUE_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_NAVICONTINUE_OUT		(MOUSE_MSGBOX_NAVICONTINUE_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_GONOWPOI_				0x0880
#define MOUSE_MSGBOX_GONOWPOI_OK			(MOUSE_MSGBOX_GONOWPOI_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_GONOWPOI_CANCEL		(MOUSE_MSGBOX_GONOWPOI_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_GONOWPOI_OUT			(MOUSE_MSGBOX_GONOWPOI_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_GOHOME_				0x0890
#define MOUSE_MSGBOX_GOHOME_OK				(MOUSE_MSGBOX_GOHOME_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_GOHOME_CANCEL			(MOUSE_MSGBOX_GOHOME_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_GOHOME_OUT				(MOUSE_MSGBOX_GOHOME_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_ENTEROFFICCE_			0x08A0
#define MOUSE_MSGBOX_ENTEROFFICCE_OK		(MOUSE_MSGBOX_ENTEROFFICCE_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_ENTEROFFICCE_CANCEL	(MOUSE_MSGBOX_ENTEROFFICCE_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_ENTEROFFICCE_OUT		(MOUSE_MSGBOX_ENTEROFFICCE_ + _MSGBOX_OUT)
#define MOUSE_MSGBOX_GOOFFICCE_				0x08B0
#define MOUSE_MSGBOX_GOOFFICCE_OK			(MOUSE_MSGBOX_GOOFFICCE_ + _MSGBOX_OK)
#define MOUSE_MSGBOX_GOOFFICCE_CANCEL		(MOUSE_MSGBOX_GOOFFICCE_ + _MSGBOX_CANCEL)
#define MOUSE_MSGBOX_GOOFFICCE_OUT			(MOUSE_MSGBOX_GOOFFICCE_ + _MSGBOX_OUT)
//-- MessageBox Type 
#define MSGBOX_CODE_ALTER					0x0010
#define MSGBOX_IF_STOPNAVI					0
#define MSGBOX_IF_EXIT						1	
#define MSGBOX_IF_CHANGEDES					2	
#define MSGBOX_IF_UISTOPNAVI				3
#define MSGBOX_IF_ENTERHOME					4
#define MSGBOX_IF_REROUTE					5
#define MSGBOX_IF_NAVICONTINUE				6
#define MSGBOX_IF_GONOWPOI  				7
#define MSGBOX_IF_GOHOME                    8

#define MSGBOX_ADDDEMO_OK					10
#define MSGBOX_NO_HOME_DATA					11
#define MSGBOX_NOROADINFO					12
#define MSGBOX_ERRORROUT					13
#define MSGBOX_NOHOMEINFO                   14
#define MSGBOX_ERRORSTART					15
#define MSGBOX_IF_ENTEROFFICE				16
#define MSGBOX_NO_OFFICE_DATA				17
#define MSGBOX_THESAMEROUTPOINT             18
#define MSGBOX_ERRORSERIAL                  19
/************************************************************************/
/* If MessageBox(nCode), the return value = nCode * 0x0010 + MOUSE_MSGBOX_BUT_(*).                                                                  */
/************************************************************************/




/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#define MOUSE_MOVE_LEFT						MOUSE_LEFT//51
#define MOUSE_MOVE_UP						MOUSE_UP//52
#define MOUSE_MOVE_RIGHT					MOUSE_RIGHT//53
#define MOUSE_MOVE_DOWN						MOUSE_DOWN//54
#define MOUSE_MOVE_UP_LEFT					MOUSE_LTUP//55
#define MOUSE_MOVE_UP_RIGHT					MOUSE_RTUP//56
#define MOUSE_MOVE_DOWN_LEFT				MOUSE_LTDN//57
#define MOUSE_MOVE_DOWN_RIGHT				MOUSE_RTDN//58

#define MOUSE_CLICK							100

///////////////////////////////////////////////////////////

/**********************************
 This file can been given to the UI.
 ***********************************/
/* User Config Struct and define. */
//------ For nScreenType ---------------------------------//
#define SCREEN_TYPE_DEFAULT								0x00
#define SCREEN_TYPE_ONE									0x01//one screen
#define SCREEN_TYPE_TWO									0x02//1:1 screen split
#define SCREEN_TYPE_THREE								0x03//2:1 screen split
//------ for nWaveTTSType --------------------------------//
#define WAVETTS_TYPE_WAVE								0x00
#define WAVETTS_TYPE_TTS								0x01
#define WAVETTS_TYPE_WAVETTS							0x02
#define WAVETTS_TYPE_TTSNOROADNAME                      0x03
//------ for nMapDirectMode --------------------------------//
#define MAP_DIRECT_CAR									0x00
#define MAP_DIRECT_NORTH								0x01
//------ for nScaleShowMode ---------------------------------//
#define SCALE_SHOW_ALWAYS								0x00
#define SCALE_HIDE_AFTER_THESE_TIME						0x14//seconds
//------- for nSplitScreenMode ------------------------------//
#define SPLIT_SCREEN_TYPE_DISABLE						0x00
#define SPLIT_SCREEN_TYPE_ONLYAUTO						0x01
#define SPLIT_SCREEN_TYPE_ONLYHAND						0x02
#define SPLIT_SCREEN_TYPE_AUTOHAND						0x03
//-------- for nVoiceControl Mask-----------------------------------
#define VOICE_CONTROL_NAVI								0x01
#define VOICE_CONTROL_CCD								0x02
#define VOICE_CONTROL_LAT								0x04
#define VOICE_CONTROL_FIXEDERROR						0x08
#define VOICE_CONTROL_NAVIERROR							0x10
#define VOICE_CONTROL_MEMO								0x20
#define VOICE_CONTROL_POIINFO							0x40
//--------- for nSimuMode ---------------------------------------------
#define SIMU_MODE_DEFAULT								0x00
#define SIMU_MODE_ENDTOGPS								0x01

//---------- for bAutoDayNight ------------------------------------------
#define DAYNIGHT_MODE_HAND								0x00
#define DAYNIGHT_MODE_AUTO								0x01
#define DAYNIGHT_MODE_LIGHT								0x02

//---------- for UITouchVoice & UIView---------------------------------------
#define TOUCHVOICE                                      0x01
#define VIEWSIMROUTE                                    0x02
#define VIEW3DMOUDLE                                    0x04

//---------- for bNightDay---------------------------------------
#define DAYNIGHTMODE_D									0x00//�]��
#define DAYNIGHTMODE_N									0x01//�鶡
/*
typedef struct tagNUROTIME {
	nuWORD nYear; 
    nuWORD nMonth; 
    nuWORD nDayOfWeek; 
    nuWORD nDay; 
    nuWORD nHour; 
    nuWORD nMinute; 
    nuWORD nSecond; 
    nuWORD nMilliseconds; 
} NUROTIME;
*/
typedef struct tagUSERCONFIG
{
	nuBYTE	bNightDay;         //BYTE 1
	nuBYTE	b3DMode;           //BYTE 2
	nuBYTE	nFixOnRoad;        //BYTE 3
	nuBYTE	bCCDVoice;         //BYTE 4
	nuBYTE	bMoveWaiting;      //BYTE 5
	nuBYTE	nMoveWaitTotalTime;//BYTE 6
	nuBYTE	nScreenType;       //BYTE 7
	nuBYTE  nLanguage;         //BYTE 8
	nuWORD	nScreenLTx;        //BYTE 9,10
	nuWORD	nScreenLTy;        //BYTE 11,12
	nuWORD	nScreenWidth;      //BYTE 13,14
	nuWORD  nScreenHeight;     //BYTE 15,16
	nuBYTE	nRouteListNum;     //BYTE 17
	nuBYTE	nWaveTTSType;      //BYTE 18
	nuBYTE	nMapDirectMode;    //BYTE 19
	nuBYTE	nScaleShowMode;    //BYTE 20
	nuBYTE	bShowLongLat;      //BYTE 21
	nuBYTE  nZoomAutoMode;     //BYTE 22
	nuBYTE	bPlayCCD;          //BYTE 23
	nuBYTE	nPlaySpeedWarning; //BYTE 24
	nuWORD	nSimSpeed;         //BYTE 25, 26;  100 or 200(km)
	nuBYTE	bAutoDayNight;     //BYTE 27
	nuBYTE	nTimeToNight;      //BYTE 28
	nuBYTE	nTimeToDay;        //BYTE 29; ä¸€å¤?4å°æ—¶??40ä»½ï??€ä»¥æ?å°æ—¶?´ç²¾åº¦æ˜¯6?†é?
	nuBYTE	nCarIconType;      //BYTE 30
	nuBYTE	nDayBgPic;         //BYTE 31
	nuBYTE	nNightBgPic;       //BYTE 32
	nuBYTE	nShowPOI;          //BYTE 33; 0 not; !0 yes;
	nuBYTE	nRouteMode;        //34
	nuBYTE	nSplitScreenMode;  //35
	nuBYTE	nNowScreenType;    //36
	nuBYTE	nMoveType;         //37
	nuBYTE	bLastNavigation:1;
	nuBYTE	nPastMarket:7;     //38
	nuWORD	nZoomBmpWidth;     //39, 40
	nuWORD  nZoomBmpHeight;    //41, 42
	nuBYTE	nVoiceControl;     //43
	nuBYTE  nMemoVoice;        //44 
	nuBYTE	nSimuMode;         //45 
	nuBYTE	nLastScaleIndex;   //46
	nuBYTE	nLastAutoMoveType; //47
	nuBYTE  nVoiceIconTimer;   //48
	nuBYTE	bCrossAutoZoom;    //49
	nuBYTE	bBigMapRoadFont;   //50
	nuBYTE	bMoveMap;		   //51
	nuBYTE	nTMCVoice;	   //52
	nuBYTE	nShowTip;	   //53
	nuBYTE	reserve[94-53]; 
	nuBYTE  nOneKeyCallVoice;  //95    onekey call voice;
	nuBYTE  nTMCSwitchFlag;    //96    TMC on or off
	nuBYTE  nTMCFrequency;     //97    TMC Frequency
	nuBYTE  nShowStartWarn;    //98     ???
	nuBYTE  nBoardCast;        //99     for timely Traffic
	nuBYTE  nRefreshRate;      //100
	nuBYTE  nShowRoad;         //101
	nuBYTE  nOneKeyTrack;      //102
	nuBYTE  nOneKeyNavi;       //103
	nuBYTE  nKeySoundSwitch;   //104
	nuBYTE  nSpNumInput;       //105
	nuBYTE  nKeyWordInput;     //106
	nuBYTE  nSpeedWarningPic;  //107
	nuBYTE  nTouchView;        //108
	nuBYTE	nUIType;           //109 0: 
	nuBYTE	nUILanguage;       //110 0: TC 1: SC
	nuBYTE  nNaviTimeMode;     //111 0: 
	nuBYTE	bFriedSate:1;      //112 Tobe Friend 0:close 1:Open
	nuBYTE	nFriedFreq:7;      //112 Tobe Friend frequency        
	nuBYTE  bWheather:1;       //113 Tobe Weather suggestion // 0 close 1 open
	nuBYTE  bLogTraject:1;     //113 Tobe Log 0 Close 1 Open
	nuBYTE  nDYM:6;            //113 No Use
	nuBYTE  nMultiRouteMode1;  //114
	nuBYTE  nMultiRouteMode2;  //115
	nuBYTE  nMultiRouteMode3;  //116
	nuBYTE  bHidenMapRoadInfo; //117
	nuBYTE  nSpeedWarningVoice;//118
	nuBYTE  nCenterTarget;     //119 CnetrTarget Type
	nuBYTE  nTMCConfig;	   //120
	nuBYTE  nRouteOption;      //121 
	nuBYTE  nUiControl;        //122 Bit0 :
	nuBYTE  nDefaultCityID;    //123
	nuBYTE  nRoadInput;        //124
	nuBYTE  nPoiInput;         //125
	nuBYTE  bNightLight:1;     //126
	nuBYTE  bOperateVoice:1;   //126
	nuBYTE  bShowBigPic:1;     // 126 for GoNavi =1 show big Pic 
	nuBYTE  bTargetLine:5;     //If Draw targetLine
	nuBYTE  nScaleIdx;         //127
	nuBYTE  nSoundVolume;      //128
	
}USERCONFIG, *PUSERCONFIG;

//----------------------------------------------------------------------------
//PT file structs
typedef struct tagPT_FILE_HEAD
{
	nuLONG nPtCount;
}PT_FILE_HEAD, *PPT_FILE_HEAD;
typedef struct tagPTNODE
{
	nuBYTE	nTypeID1;
	nuBYTE	nTypeID2;
	nuBYTE	nDisplay;
	nuBYTE	reserve;
	nuDWORD	nResID:24;
	nuDWORD nScale0:8;
}PTNODE, *PPTNODE;

//---nPointType
#define POINT_TYPE_SCREEN_XY				0
#define POINT_TYPE_SCREEN_CENTER			1
//
#define POINT_TYPE_MAP_XY					0
#define POINT_TYPE_MAP_CAR					1
#define POINT_TYPE_MAP_GPS					2
#define POINT_TYPE_MAP_XYFIXED				3
#define POINT_TYPE_MAP_CENTER				4
typedef struct tagMARKETPOINT
{
	nuWORD	bIsSetting:1;//Used in the NaviThread. UI don't use it.
	nuWORD	bSetSucess:1;//OUT. 0: false; 1: true;
	nuWORD	bUpdateMap:1;//IN. 0: set navigation point and don't update map, 1: set navigation point and update map;
	nuWORD	bScreenPoint:1;//IN. 0: map point; 1: screen point;
	nuWORD	nPointType:12;//IN. 



	nuSHORT	index;//IN. 0: starting point; 1: passing point; 2: End point;
	nuLONG	x;
	nuLONG	y;
	nuLONG	nDwIdx;
	nuLONG	nBlockIdx;
	nuLONG	nRoadIdx;
	nuroPOINT	ptRaw;//Only Used in POINT_TYPE_MAP_XYFIXED
	nuLONG		nNameAddr;
	nuWCHAR wDesName[512];
}MARKETPOINT, *PMARKETPOINT;
//Navigation type
#define NAVIGATION_STOP				0x00
#define NAVIGATION_SIMROUTE			0x01
#define NAVIGATION_GPSROUTE			0x02
#define NAVIGATION_SETGPSMODE		0x03
#define NAVIGATION_SETSIMMODE		0x04
#define NAVIGATION_IFSTOP			0x05
#define NAVIGATION_ONLYROUTE		0x06
#define NAVIGATION_CONFIGROUTE		0x07
#define	NAVIGATION_SIMROUTE_I		(0x10 + NAVIGATION_SIMROUTE)
#define NAVIGATION_GPSROUTE_I		(0x10 + NAVIGATION_GPSROUTE)
#define NAVIGATION_ONLYROUTE_I		(0x10 + NAVIGATION_ONLYROUTE)
////////////////////////////////////////////////////////////////////
//------For SoundPlay-----------------------------------
#define VOICE_CODE_DEFAULT_SYN				0x00//same voice in different language
#define VOICE_CODE_DEFAULT_ASY				0x01
#define VOICE_CODE_ROAD_SYN					0x02
#define VOICE_CODE_ROAD_ASY					0x03
#define VOICE_CODE_OTHERS_SYN				0x04
#define VOICE_CODE_OTHERS_ASY				0x05
#define VOICE_CODE_UI_DFT_SYN				0x06
#define VOICE_CODE_UI_DFT_ASY				0x07
#define VOICE_CODE_REVISE_VOLUME		    0x08
#define VOICE_CODE_REVISE_SPEED 		    0x09
#define VOICE_CODE_REVISE_PITCH			    0x10
/////////////////////////////////////////////////////////////////////
typedef void(*NuroTimerProc)();

//------------GPS data----------------------------

#define  DATA_TYPE_INVALID					0x00
#define  DATA_TYPE_GPS						0x01
#define  DATA_TYPE_GYRO						0x02
#define  DATA_TYPE_GPSFAKE					0x03

//
#define GPS_OPEN_CLOSE						0x00 
#define	GPS_OPEN_FILE						0x01
#define GPS_OPEN_COMM						0x02
#define GPS_OPEN_UCOS						0x03
#define GPS_OPEN_ECOS                       0x04
#define GPS_OPEN_PS                         0x05
#define GPS_OPEN_WOOJOY						0x06
#define GPS_OPEN_HV			    			0x07
#define GPS_OPEN_NORMALMODE					0x08//�U�μҦ�
#define GPS_OPEN_POWERNAVI					0x09//���(����)��s���
#define GPS_OPEN_SIMPNEMA                   0x0A //�@�� �u��GPRMC���t��
#define GPS_OPEN_JNI                        0x20
#define GPS_OPEN_SIRF_BINARY                0x0B
#define GPS_OPEN_PANA_LEA6R					0x0C//Panasonic����
#define GPS_OPEN_ROYALTEK_ANDROID			0x21//���(����)��s���_android
#define GPS_OPEN_LINUX						0x22//�j���ϥΩ�No DR��
#define GPS_OPEN_JNI_2						0x23//���� JNI ��GPS
#define GPS_OPEN_EL							0x31//�ɧQ����
#define GPS_OPEN_ELEAD_3DR					0x31//�ɧQ(ELEAD) 3D��s��
#define GPS_OPEN_SDC_3DR					0x32//���p(SDC) 3D��s��
#define GPS_OPEN_ROYALTEK_3DR				0x33//����(ROYALTEK) 3D��s��
#define GPS_OPEN_ELEAD_GPS					0x34

#define GPS_SATESYS_GPS						0x00//GPS(���)
#define GPS_SATESYS_BD						0x01//BD(����)
#define GPS_SATESYS_GLONASS					0x02//GLONASS(�Xù��)

#define GPS_SLOPE_FASTUP					2//Panasonic����
#define GPS_SLOPE_SLOWUP					1//Panasonic����
#define GPS_SLOPE_NO						0//Panasonic����
#define GPS_SLOPE_SLOWDOWN					-1//Panasonic����
#define GPS_SLOPE_FASTDOWN					-2//Panasonic����
#define MAX_STAR_NUMBER						48
typedef struct tagSTARDATA
{
	nuUSHORT	nStarID;				//PRN
	nuUSHORT	nAngle_elevation;		//
	nuUSHORT	nSignalNoiseRatio:10;	//�T����
	nuUSHORT	nSateSystem:4;			//�ìP�t�ΧO 0:GPS(���) 1:BD(����_��) 2:GLONASS(�Xù��)
	nuUSHORT	b_GPSOK:1;				//4 kinds of data ok
	nuUSHORT	b_GPSUsed:1;			//Module used
	nuUSHORT	nAngle_position;		//
}STARDATA, *PSTARDATA;

//For LCMM GPS (Louis 20110704)
#define GPSVERSIONLEN		50
#define GPSNMEATXTLEN		128
#define MAXUBXLEN			64

typedef struct TAGGPSMODPANAINFO
{
	nuBYTE	Cal_XGyro;
	nuBYTE	Cal_YGyro;
	nuBYTE	Cal_ZGyro;//Gyro Z���A 0:Not 1:Calib 2:Coarse 3:Fine
	nuBYTE	Cal_XAcc;//Acc X���A 0:Not 1:Calib 2:Coarse 3:Fine
	nuBYTE	Cal_YAcc;
	nuBYTE	Cal_ZAcc;
	nuBYTE	Cal_SpeedTick;//Signgle Tick���A 0:Not 1:Calib 2:Coarse 3:Fine
	nuBYTE	Cal_SFDRStatus;//SFDR  0:no fusion ,1:fusion ,2:disabled temporarily ,3:disabled permanently until receiver reset
	nuBYTE	Data_ReverseSignal;//�˨��T�� 0:���e 1:�˰h
	nuFLOAT	Data_Speed;//���t 0.001m/s
	nuFLOAT	Data_XGyro;
	nuFLOAT	Data_YGyro;
	nuFLOAT	Data_ZGyro;
	nuFLOAT	Data_XAcc;
	nuFLOAT	Data_YAcc;
	nuFLOAT	Data_ZAcc;
	nuFLOAT	Data_SpeedTick;//���t�߽�
}GPSMODPANAINFO,*PGPSMODPANAINFO;

typedef struct TAGGPSMODINFO
{
	nuCHAR HWVERSION[GPSVERSIONLEN];
	nuCHAR EXTVERSION[GPSVERSIONLEN];
	nuCHAR ROMVERSION[GPSVERSIONLEN];
	nuCHAR TXTGPGGA[GPSNMEATXTLEN];
	nuCHAR TXTGPGSV[GPSNMEATXTLEN];
	nuCHAR TXTGPVTG[GPSNMEATXTLEN];
	nuCHAR EKFSTATUS[MAXUBXLEN];
	nuCHAR EKF[MAXUBXLEN];	
	GPSMODPANAINFO MODEINFO;	
}GPSMODINFO,*PGPSMODINFO;

typedef struct tagGPSDATA
{
	nuLONG	nLatitude;//
	nuLONG	nLongitude;//
	nuSHORT	nAngle;		
	nuSHORT	nSpeed;		//
	NUROTIME nTime;
	nuBYTE	nStarNumUsed;	//
	nuBYTE	nStatus;		//
	nuBYTE	nActiveStarNum;
	nuBYTE	nStatus_RMC;		//0:GPS no work  1:GPS 2D  2:GPS 3D
	nuLONG	nAntennaHeight;	//
	nuDWORD nReciveTimes;   //
	STARDATA pStarList[MAX_STAR_NUMBER];
	nuSHORT	nReverseSignal;//�˨��T�� 0:���e ��l:�˰h
	nuBYTE	nNavType;//GPS position fix type
	nuBYTE	SIRFBINARY41INFO[100];//SIRF BINARY ID 41 DATA
	nuSHORT	nOdometerSpeed;		//CANBUS SPEED FOR ���p
	nuSHORT	nSlopeStatus;	//2:�W�Y 1:�w�W�Y  0:���`	-1:�w�U�Y -2:�U�Y	   
	nuSHORT	nSlopeDis;		//���Y�A���Z��(��)
	//�_��
	nuBYTE	nBDStarNumUsed;	//
	nuBYTE	nBDActiveStarNum;
	STARDATA pBDStarList[MAX_STAR_NUMBER];// for�_��ϥ�
	
#ifndef VALUE_EMGRT
	GPSMODINFO nDebugGPSInfo;
#endif
}GPSDATA, *PGPSDATA;

//=============================�ɧQGPS��Ƶ��c===============================
typedef struct tagGYROSATINFO_EL 
{
	unsigned char nPRN;			// Get from $GPGSV
	unsigned char bActived;		//true or false�Ainstead of unsigned short stActiveSat[12]
	unsigned short nSigQuality;	// Get from $GPGSV
	unsigned short nAzimuth;		// Get from $GPGSV
	unsigned short nElevation;		// Get from $GPGSV
}GYROSATINFO_EL,*PGYROSATINFO_EL;

typedef struct GYROGPSDATA_EL 
{
	unsigned char btGpsQuality;	// Get from $GPGGA GPS Quality
	unsigned char btGpsFixMode;	// Get from $GPGSA Fix Mode
	unsigned short nTotalSatsInView; // Max for 36
	unsigned short btYear;		// Get from $GPRMC Date ( 2 numbers )
	unsigned char btMonth;		// Get from $GPRMC Date ( 2 numbers )
	unsigned char btDay;			// Get from $GPRMC Date ( 2 numbers )
	unsigned char btHour;			// Get from $GPRMC Time ( 2 numbers )
	unsigned char btMinute;		// Get from $GPRMC Time ( 2 numbers )
	unsigned char btSecond;		// Get from $GPRMC Time ( 2 numbers )
	unsigned char btStatus;		// Get from $GPRMC Data Valid
	double dGpsAltitude;			// Get from $GPGGA Altitude
	double dLatitude;			// Get from $GPRMC Latitude ( degree:d, ddd.dddd�K )
	double dLongitude;			// Get from $GPRMC Longitude ( degree:d, ddd.dddd�K )
	double dSpeed;				// Get from $GPVTG Speed ( Km/Hr )
	double dCourse;				// Get from $GPRMC Course
	double dPDOP;				// Get from $GPGSA: dilution of precision
	double dHDOP;				// Get from $GPGSA:
	double dVDOP;				// Get from $GPGSA:
	double dHeight;				// Get from $GPGSA:
	unsigned int nSlope;			// Showing the slope is getting up [U] or down [D], normally is flat [F].
	unsigned int nSN;			// serial number for NAVIFeedback to check the delay by message sending.
	bool blGyroMsg;				// A flag for checking the data source from Gyro without GPS
	GYROSATINFO_EL starInfo[36];
} GYROGPSDATA_EL,*PGYROGPSDATA_EL;
//======================================================================

typedef struct tagGPSCFG
{
	nuLONG nGPSOpenMode;
	nuLONG nGPSLogMode;
	nuLONG nComport;
	nuLONG nGPSBaudrate;
}GPSCFG,*PGPSCFG;

//-- For LibNC_GetMapInfo-----------------------------------------
#define CITYTOWN_NAME_MAXNUM				50
#define CENTER_NAME_MAX_NUM					50
typedef struct tagMAPCENTERINFO
{
	nuWORD		nCenterType;//POI, Road ans so on...
	nuSHORT		nDis;//-1 means no near info
	nuWORD		nCityCode;
	nuWORD		nTownCode;
	nuroPOINT	ptCenterRaw;
	nuroPOINT	ptCenterFixed;
	nuLONG		nFixedFileIdx;
	nuDWORD		nFixedBlockIdx;
	nuDWORD		nFixedRoadIdx;
	nuWCHAR		wsCTname[CITYTOWN_NAME_MAXNUM];
	nuWCHAR		wsCenterName[CENTER_NAME_MAX_NUM];
	nuDWORD		nNameAddr;
	nuDWORD		nOtherInfo;
}MAPCENTERINFO, *PMAPCENTERINFO;

#define MAP_INFO_CENTER						1
#define MAP_INFO_CAR						2
#define MAP_INFO_GPS						3
#define MAP_INFO_CENTERPOI					4
//-------------------------------------------------------------

//---NAVICTRL_API bool LibNC_OperateMapXY(nuLONG x, nuLONG y, nuUINT nMouseCode)---
//nMouseCode for common
#define SHOWMENU_COMMON_SUBMENU					0
//nMouseCode for Panasonic only 
#define SHOWMENU_FROM_TARGET					101
#define SHOWMENU_FROM_ADDMEMO					102
#define SHOWMENU_FROM_ENTERHOME					103
#define SHOWMENU_FROM_ADDNAVIPT					104
//nMouseCode for others
#define OP_MAP_CARPOITION						201
#define OP_MAP_ONLYCARPOSITION					202

//--- For LibNC_GetRoutingList-----------------------------------------------------
#define ROUTINGLIST_PAGE_MAX_NUM				10

#define LIST_NAME_NUM							100

typedef struct tagROUTINGLISTNODE
{
	nuWORD		nIconType;
	nuWCHAR		wsName[LIST_NAME_NUM];
	nuDWORD		nDistance;//To next Node
	nuroPOINT	point;
}ROUTINGLISTNODE, *PROUTINGLISTNODE;

//the return value of LibNC_GetNaviMode()
#define MAP_NAVI_MODE_STOP						0x00//stop
#define MAP_NAVI_MODE_GPS						0x01
#define MAP_NAVI_MODE_SIMULATION				0x02
#define MAP_NAVI_MODE_TIMER						0x03//move waiting				

//--- For Travel file header ----------
#define TRAVEL_NAME_MAX_NUM					20
typedef struct tagTRAVELHEADER
{
	nuWORD	nRecordID;
	nuWORD	nYear;
	nuBYTE	nMonth;
	nuBYTE	nDay;
	nuBYTE	nHour;
	nuBYTE	nMinute;
	nuLONG	nStartAdrr;
	nuLONG	nCount;
	nuWCHAR	nTravelName[TRAVEL_NAME_MAX_NUM];
}TRAVELHEADER, *PTRAVELHEADER;

//--- For LibNC_GetTripInfo() --------------//
typedef struct tagTRIPDATA
{
	nuDWORD		nTotalDisGone;//(meter)unit
	nuDWORD		nTotalTimeGone;//(second)unit
	nuDWORD		nAverageSpeed;//(km/h)unit
	nuDWORD		nHighestSpeed;//(km/h)unit
	nuDWORD		nDisToTarget;//(meter)unit
	nuDWORD		nTimeToTarget;//(second)unit
	nuDWORD		nHightAboveSea;//(meter)unit
	nuBYTE		nSecond;
	nuBYTE		nMinute;
	nuBYTE		nHour;
	nuBYTE		nDay;
	nuWORD		nYear;
}TRIPDATA, *PTRIPDATA;
//
typedef struct tagDRAWCARINFO
{
	nuBYTE	nIconType;
	bool	bNorth;
	bool	b3DMode;
}DRAWCARINFO,*PDRAWCARINFO;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Engine API struct///////////////
#define GET_API_KERNAL						1
#define GET_API_COMMON						2
//--- LibNC_ShowHideButton() ----------------------
#define BUTTON_HIDE							0x00
#define BUTTON_DEFAULT						0x01
#define BUTTON_DOWN							0x02
#define BUTTON_FOCUS						0x03
#define BUTTON_DISABLE						0x0F
//--- LibNC_SetDrawInfo()---------------------------
#define DFM_MAX_BUTS_SET					50
typedef struct tagDFMBUTSET
{
	nuDWORD	nButID;
	nuBYTE  nButState;
}DFMBUTSET, *PDFMBUTSET;
typedef struct tagDFMSETTING
{
	nuBYTE	nButsSet;
	DFMBUTSET	pButInfo[DFM_MAX_BUTS_SET];
}DFMSETTING, *PDFMSETTING;


//******* Trace Data and remark****************************
#define MAX_TRACE_RECOAD_NUM				10
#define MAX_TRACE_NAME_NUM					50
typedef struct tagTRACEFILEHEAD
{
	nuBYTE	nVersion[3];
	nuBYTE	nNowRecordNum;//
	nuDWORD	nNowTotalPoints;//
	nuDWORD	nNowIdleAddres;//
	nuBYTE	nSss[12];
}TRACEFILEHEAD, *PTRACEFILEHEAD;

typedef struct tagTRACEHEAD
{
	nuWORD	nID;//
	nuWORD	nYear;
	nuBYTE	nMonth;
	nuBYTE	nDay;
	nuBYTE	nHour;
	nuBYTE	nMinute;
	nuDWORD	nPtAddress;
	nuWORD	nPtCount;
	nuWORD	nReserve[43];
}TRACEHEAD, *PTRACEHEAD;

typedef struct tagTRACEFORUI
{
	TRACEFILEHEAD	fileHead;
	TRACEHEAD		nNowTraceHead;
	TRACEHEAD		pFileTraceHead[MAX_TRACE_RECOAD_NUM];
}TRACEFORUI, *PTRACEFORUI;

typedef struct tagTRACEPOINT
{
	nuroPOINT	point;
	nuDWORD		nIdx;
}TRACEPOINT, *PTRACEPOINT;
//NAVICTRL_API nuPVOID LibNC_GetTraceHead();
//Retrun: TRACEFORUI struct.ֻ
//LibNC_StartTrace(nuBYTE nState);
//nState:
#define TRACE_OPEN_STATE_NEW			0x01//
#define TRACE_OPEN_STATE_RESUME			0x02//
//Return:
#define TRACE_OPEN_FAILURE				0
#define TRACE_OPEN_HASOPENED			1
#define TRACE_OPEN_SUCCESS_NEW			2
#define TRACE_OPEN_SUCCESS_RESUME		3

//LibNC_CloseTrace(nuBYTE nState);
//nState:
#define TRACE_CLOSE_STATE_DEFAULT		0x00//
#define TRACE_CLOSE_STATE_SUSPEND		0x01//
#define TRACE_CLOSE_STATE_SAVE			0x02//
#define TRACE_CLOSE_STATE_COVEROLD		0x03//
//Return:
#define TRACE_CLOSE_FAILURE				0
#define TRACE_CLOSE_SUCCESS				1
#define TRACE_CLOSE_WILLCOVEROLD		2
#define TRACE_CLOSE_HASECLOSEN			3
//LibNC_DeleteTrace(nuWORD nID);
//Return
#define TRACE_DELETE_FAILURE			0
#define TRACE_DELETE_NOTHISRECORD		1
#define TRACE_DELETE_SUCESS				2
//LibNC_ShowTrace(nuWORD nID);
#define TRACE_SHOW_FAILURE				0
#define TRACE_SHOW_SUCESS				1
//*************** End Trace Data *****************************************


//*************** For Memo  *************************************************
#define _MEMO_VOICE_DISTANCE				220
typedef struct tagMEMONODE
{
	nuLONG	x;
	nuLONG	y;
	nuWORD	nIconIdx;
	nuWORD	nVoiceIdx;
	nuWCHAR wsIconName[32]; //@2011.07.27 for FriendName
}MEMONODE, *PMEMONODE;
typedef struct tagMEMODATA
{
	nuWORD		nCount;
	nuWORD		nNowCount;
	PMEMONODE	pMemoList;
}MEMODATA, *PMEMODATA;
typedef struct tagMEMOPTDATA
{
	nuDWORD     nCount;
	nuroPOINT*  pMemoPtList;
}MEMOPTDATA, *PMEMOPTDATA;
//***************************************************************************

//*** For LibNC_GetKernalState()****************************
typedef struct tagNC_KERNAL_STATE
{
	nuDWORD	bIsNavigation:1;//1:Navigation; 0:is Not;
	nuDWORD	bNightOrDay:1;//1:Night; 0:Day;
	nuDWORD	b3DMode:1;//1:3D; 0:2D;
	nuDWORD	bFirstGPSReady:1;//whether playing "GPS ready" voice.
	nuDWORD	nMapDirection:2;//macro: MAP_DIRECT_NORTH or MAP_DIRECT_CAR
	nuBYTE	nAutoMoveType;
	nuWORD	nSkyHeight;//the height of the 3D sky picture
	nuWORD	nTotalScale;
	nuWORD	nNowScaleIdx;
}NC_KERNAL_STATE, *PNC_KERNALE_STATE;

typedef struct tagNURO_SYS_TIME 
{
	nuWORD nYear; 
    nuBYTE nMonth; 
    nuBYTE nDayOfWeek; 
    nuBYTE nDay; 
    nuBYTE nHour; 
    nuBYTE nMinute; 
    nuBYTE nSecond; 
    nuWORD nMilliseconds; 
} NURO_SYS_TIME, *PNURO_SYS_TIME;

typedef struct tagNC_NAVI_DATA
{
	nuLONG	nErrorCode;//the returned error code of Route();
	nuBYTE	bIsRouting;//If it is routing;
	nuBYTE	bRouted;//
    nuBYTE  bUINeedWaiting;
}NC_NAVI_DATA, *PNC_NAVI_DATA;


//*********** FOR SEARCH **************************************
#define DY_KEY_MAX_NUM					73
typedef struct tagDYKEYMASK
{
	nuBYTE	nNowCount;
	nuWCHAR	pKeyEnable[DY_KEY_MAX_NUM];
}DYKEYMASK, *PDYKEYMASK;



//---Parameters of SEARCHNEW_API nuUINT LibSH_PYSetMode(nuUINT nMode)
#define SH_PY_MODE_NO					0x0000
#define SH_PY_MODE_POI					0x0001
#define SH_PY_MODE_ROAD					0x0002
#define SH_PY_MODE_STR_B				0x0100
#define SH_PY_MODE_STR_C				0x0200

//---------------------------------------------------------------------

#define SEARCH_DY_SET_MAPIDX			0x01
#define SEARCH_DY_SET_CITYIDX			0x02
#define SEARCH_DY_SET_TOWNIDX			0x03

#define SEARCH_NDY_SET_NAME			0x01
#define SEARCH_NDY_SET_DANNY			0x02

#define SEARCH_DY_SET_MAPIDX			0x01
#define SEARCH_DY_SET_CITYIDX			0x02
#define SEARCH_DY_SET_TOWNIDX			0x03

#define SEARCH_MODE_NO					0
#define SEARCH_MODE_DANYIN_				0x00000010
#define SEARCH_MODE_DANYIN_ALL			(SEARCH_MODE_DANYIN_ + 1)
#define SEARCH_MODE_DANYIN_POI			(SEARCH_MODE_DANYIN_ + 2)
#define SEARCH_MODE_DANYIN_ROAD			(SEARCH_MODE_DANYIN_ + 3)
//
#define SEARCH_MODE_ROAD_				0x00000100

/*
#define DY_KEY_MAX_NUM					42
typedef struct tagDYKEYMASK
{
	nuBYTE	nNowCount;
	nuWCHAR	pKeyEnable[DY_KEY_MAX_NUM];
}DYKEYMASK, *PDYKEYMASK;
*/
typedef struct tagDYCOMMONFORUI
{
	PDYKEYMASK	pDyKeyMask;
	nuDWORD		nTotalCount;//TotalCount 
}DYCOMMONFORUI, *PDYCOMMONFORUI;

#define DY_NAME_FORUI_MAX_COUNT				8//
#define DY_NAME_FORUI_NUM					64//
#define DY_NAME_FORUI_CITYORTOWN_NUM		6//
#define DY_NAME_FORUI_OTHER_NUM				130
#define DY_NAME_MAX							2000//
typedef struct tagDYNODEFORUI
{
	nuBYTE	nStroke;
	nuBYTE	nType;
	nuWORD	nCityID:6;
	nuWORD	nTownID:10;
	nuWCHAR	wsName[DY_NAME_FORUI_NUM];
	nuWCHAR	wsCityName[DY_NAME_FORUI_CITYORTOWN_NUM];
	nuWCHAR	wsTownName[DY_NAME_FORUI_CITYORTOWN_NUM];
}DYNODEFORUI, *PDYNODEFORUI;
typedef struct tagNDYDETAILFORUI
{
	nuINT nCityID;
	nuINT nTownID;
	nuINT nType;
	nuroPOINT	point;
	nuWCHAR	wsName[DY_NAME_FORUI_NUM];
	nuWCHAR	wsCityName[DY_NAME_FORUI_CITYORTOWN_NUM];
	nuWCHAR	wsTownName[DY_NAME_FORUI_CITYORTOWN_NUM];
	nuWCHAR	wsOther[DY_NAME_FORUI_OTHER_NUM];
	nuLONG nDistance;
}NDYDETAILFORUI, *PNDYDETAILFORUI;



//Prosper 20130104
#define SHNRN_WORD_DATA_LIMIT						   2000//Road 
#define SH_WORD_DATA_LIMIT							200//Road , 
#define SH_POI_WORD_DATA_LIMIT                         2500 //poi DY_MAX_KEPT_RECORD (1000)
#define SH_NAME_LEN									32	
typedef struct tagDYWORD
{
	nuUINT  nFindCount;
	nuWCHAR	name[SH_NAME_LEN-1];
	nuWORD	townID;
}DYWORD, *PDYWORD;
typedef struct tagDYWORDFORUI
{
	nuUINT  nTotalCount;
	DYWORD	Data[SH_POI_WORD_DATA_LIMIT];
}DYWORDFORUI, *PDYWORDFORUI;
typedef struct tagDYDATAFORUI
{
	nuBYTE	nMinStroke;//Out
	nuBYTE	nMaxStroke;//Out
	nuBYTE  nPageItemCount;//In 
	nuBYTE	nThisPageCount;//In/out
	nuBYTE	nIdxSelected;//In/out
	nuBYTE	nNowCityID;//In/out; -1 means all cities
	nuWORD	nNowTownID;//In/out; -1 means all towns
	nuWORD	nNowPageItemIdx;//In/out
	nuDWORD nTotalCount;//out
	nuDWORD	pNodeIdxList[DY_NAME_FORUI_MAX_COUNT];
	DYNODEFORUI	dyNodeList[DY_NAME_FORUI_MAX_COUNT];//out
}DYDATAFORUI, *PDYDATAFORUI;
typedef struct tagDYDETAILFORUI
{
	nuroPOINT	point;
	nuINT	nTYPE;
	nuWCHAR	wsOther[DY_NAME_FORUI_OTHER_NUM];
	nuWCHAR	wsAddrOther[DY_NAME_FORUI_OTHER_NUM];

}DYDETAILFORUI, *PDYDETAILFORUI;

#define DY_NAME_CITYTOWN_MAX_NUM					5
#define DY_NAME_CITYTOWN_MAX_COUNT					50
typedef struct tagDYCITYTOWNNAME
{
	nuBYTE	nTotalCount;//out
	nuWORD	pIdList[DY_NAME_CITYTOWN_MAX_COUNT];
	nuWCHAR	ppName[DY_NAME_CITYTOWN_MAX_COUNT][DY_NAME_CITYTOWN_MAX_NUM];
}DYCITYTOWNNAME, *PDYCITYTOWNNAME;
//*************************************************************
#define SH_NDYNAME_LEN									48	
typedef struct tagNEWDYNODEFORUI
{
	nuBYTE	nStroke;
	nuBYTE	nType;
	nuWORD	nCityID:6;
	nuWORD	nTownID:10;
	nuWCHAR	wsName[SH_NDYNAME_LEN/2];
	nuWCHAR	wsCityName[DY_NAME_FORUI_CITYORTOWN_NUM];
	nuWCHAR	wsTownName[DY_NAME_FORUI_CITYORTOWN_NUM];
}NEWDYNODEFORUI, *PNEWDYNODEFORUI;
typedef struct tagNEWDYWORD
{
	nuUINT  nFindCount;
	nuWCHAR	name[SH_NDYNAME_LEN-1];
	nuWORD	townID;
}NEWDYWORD, *PNEWDYWORD;
typedef struct tagNEWDYWORDFORUI
{
	nuUINT  nTotalCount;
	NEWDYWORD	Data[SH_POI_WORD_DATA_LIMIT];
}NEWDYWORDFORUI, *PNEWDYWORDFORUI;
typedef struct tagNEWDYDATAFORUI
{
	nuBYTE	nMinStroke;//Out
	nuBYTE	nMaxStroke;//Out
	nuBYTE  nPageItemCount;//In 
	nuBYTE	nThisPageCount;//In/out
	nuBYTE	nIdxSelected;//In/out
	nuBYTE	nNowCityID;//In/out; -1 means all cities
	nuWORD	nNowTownID;//In/out; -1 means all towns
	nuWORD	nNowPageItemIdx;//In/out
	nuDWORD nTotalCount;//out
	nuDWORD	pNodeIdxList[DY_NAME_FORUI_MAX_COUNT];
	NEWDYNODEFORUI	dyNodeList[DY_NAME_FORUI_MAX_COUNT];//out
}NEWDYDATAFORUI, *PNEWDYDATAFORUI;

//******************* New Api*********************************
//NAVICTRL_API nuUINT  LibNC_ShowMap(nuBYTE nCode);
//For nCode
#define _UI_SHOW_MAP_ALL					0xFF//Redraw the map, drawinfo, (msgbox, menu), and show map
#define _UI_SHOW_MAP_DRAWINFO				0x01//DrawInfo,(msgbox, menu) and Show map
#define _UI_SHOW_MAP_MENUORMSG				0x02//(msgBox, menu) and show map


#define _ROUTE_LIST_TYPE_FROMCAR			0x00
#define _ROUTE_LIST_TYPE_FROMSTART			0x01
//For NAVICTRL_API nuUINT  LibNC_PointToRoad(nuroPOINT point, nuPVOID lpData);
//lpData struct
typedef struct tagPT_TO_ROAD_DATA
{
	nuWORD		bPointScreen:1;//IN 1:Screen Point, 0;Map Point
	nuWORD		nMaxFixDis:15;//IN;
	nuSHORT		nAngle;//IN
	nuWORD		nCityID;//OUT
	nuWORD		nTownID;//OUT
	nuWORD		nMapIdx;//OUT
	nuSHORT		nDisToRoad;//OUT
	nuWORD		nRoadCls;
	nuDWORD		nBlkIdx;//OUT
	nuDWORD		nRoadIdx;//OUT
	nuDWORD		nNameAddr;//OUT
	nuroPOINT	point;//OUT the points in map
	nuroPOINT	ptMappped;//OUT the points in road
}PT_TO_ROAD_DATA, *PPT_TO_ROAD_DATA;
//return define

// Map Show SEARCH_DETAILINFO struct
#define ROAD_DETAILINFO   0x01
#define CROSS_DETAILINFO  0x02
#define POI_DETAILINFO    0x03

typedef struct tagSEARCH_DETAILINFO
{
	nuBYTE nubyType;
	nuWCHAR  wPoiRoadName[128];
	nuWCHAR  wCrossName[128];
	nuWCHAR  wCity[32];
	nuWCHAR  wTown[32];
	nuWCHAR  wPhone[32];
	nuWCHAR  wFex[32];
	nuWCHAR  wNameext[128];
	nuWCHAR  wAddr[128];
	nuWCHAR  wWeb[128];
	nuWCHAR  wTime[128];
}SEARCH_DETAILINFO, *PSEARCH_DETAILINFO;
// ---------------------------

#define NC_PT_TO_RD_SUCCESS					(-1)
#define NC_PT_TO_RD_FAILURE					0

//----------------------------

#define MAPNAMEMAXLEN				42//the max map name lenght is 32 bytes
#define MAPFRIENDNAMELEN			20
typedef struct tagMAPNAMEINFO
{
	nuLONG	nMapID;
	nuTCHAR	sMapName[MAPNAMEMAXLEN/sizeof(nuTCHAR)];
	nuWCHAR	wMapFriendName[MAPFRIENDNAMELEN/sizeof(nuWCHAR)];
	nuroPOINT	point;
}MAPNAMEINFO, *PMAPNAMEINFO;

//For Waypoints
#define WAYPOINTS_NAME_MAX_NUM				20
typedef struct tagWAYPOINTS_NODE
{
	nuWORD		bRunPass;
	nuWORD		nMapIdx;
	nuDWORD		nBlkIdx;
	nuDWORD		nRoadIdx;
	nuWORD		nCityID;
	nuWORD		nTownID;
	nuroPOINT	ptRaw;
	nuroPOINT	ptFixed;
	nuWCHAR		wsName[WAYPOINTS_NAME_MAX_NUM];//End '\0'
}WAYPOINTS_NODE, *PWAYPOINTS_NODE;

////////////////////////////////////////////////////////////
#define CALLBACK_TYPE_SYSTEMEXIT				1
#define CALLBACK_TYPE_VOICESTART				2
#define CALLBACK_TYPE_VOICEEND					3
#define CALLBACK_TYPE_CCDSTART					4
#define CALLBACK_TYPE_CCDEND					5
#define CALLBACK_TYPE_SPECCH_START				6
#define CALLBACK_TYPE_SPECCH_END				7
#define CALLBACK_TYPE_SEND_HUD					8
#define CALLBACK_TYPE_ETAG						9
#define CALLBACK_TYPE_VOICE_WAV_START		   10
#define CALLBACK_TYPE_VOICE_WAV_END			   11
#define CALLBACK_TYPE_EWHERE				   12
#define CALLBACK_TYPE_JAVA_DATA			   1001

#define CALLBACK_TYPE_TMCUSED					1002
#define CALLBACK_TYPE_MAP_NAME					1003
#define CALL_BACK_TYPE_GPS_TIME					1004
#define CALLBACK_TYPE_JAVA_ROUTING				1005
typedef struct tagCALL_BACK_PARAM
{
	nuUINT	nCallType;
	nuLONG  lDataLen;
	nuUINT	lCallID;
	nuVOID  *pVOID;
}CALL_BACK_PARAM, *PCALL_BACK_PARAM;
typedef nuUINT(*CallBackFuncProc)(PCALL_BACK_PARAM pParam);

//Prosper 20121112
#define SH_VOICE_NAME_LEN							4//
#define SH_VOICE_NAME_LEN2							8//
#define SH_VOICE_NAME_LEN3							16//
#define SH_VOICE_NAME_LEN4							32//
#define SH_VOICE_ROADNAME							128//
typedef struct tagVOICEROADFORUI
{
	nuWCHAR wCITY[SH_VOICE_NAME_LEN];
	nuWCHAR wTOWN[SH_VOICE_NAME_LEN];
	nuWCHAR wROAD1[SH_VOICE_NAME_LEN3];
	nuWCHAR wROAD2[SH_VOICE_NAME_LEN2];
	nuWCHAR wROAD3[SH_VOICE_NAME_LEN2];
	nuWCHAR wDOORNUM1[SH_VOICE_NAME_LEN3];
	nuINT	nCity;
	nuINT	nTown;
	nuINT	nDoorNum1;
	nuINT	nDoorNum2;
}VOICEROADFORUI, *PVOICEROADFORUI;

typedef struct tagVOICEPOIFORUI
{
	nuWCHAR wPOINAME [SH_VOICE_ROADNAME]; //POI�W��
	nuINT   nIdx;                           //�^��Idx
}VOICEPOIFORUI, *PVOICEPOIFORUI;

//------------NaviCtrl define--------------//
#define _GET_UI_DAIWOO_BT_DATA	0x00
#define _GET_UI_E_LEAD_GPS_DATA	0x01
#define _GET_UI_NAVI_DES	    0x02
//------------NaviCtrl Setdefine--------------//
#define UI_SET_DATA_NEARPOI				0x00
#define UI_SET_DATA_JUMPROAD			0x01

//------------NaviCtrl define--------------//

//-----------------DaiWoo BT Structure--------------------//
enum
{
	BtDataStaNull,			// No Phone State
	BtDataStaCalling,		// Call In Or Call Out
	BtDataStaActive,		// Talking
};
typedef struct tag_UI_DAIWOO_BT
{
	nuINT   nSta;			// BT State
	nuTCHAR szName[32];		// UNICODE Phone Name
	nuTCHAR szNo[32];			// UNICODE Phone Number

}UI_DAIWOO_BT, *PUI_DAIWOO_BT;

typedef struct tag_UI_GET_GPS_BUFFER
{
	nuBYTE  pBuffer[1024];			// BT State
	nuDWORD  nLen;
}UI_GET_GPS_BUFFER, *PUI_GET_GPS_BUFFER;

#define _MAX_NBEST_NUM_ (20)

typedef struct tagUI_SPEECHDATA
{
	nuUINT   nCommand;      //��O�P�_
	nuUINT   nOrder;        //��ܪ�����
	nuUINT   nRoadCount;    //�D���
	VOICEROADFORUI pRoadBuffer[_MAX_NBEST_NUM_];   //�D����
	nuUINT   nPOICount;     //POI����
	VOICEPOIFORUI pPOIBuffer[_MAX_NBEST_NUM_];    //POI���
}UI_SPEECHDATA, *PUI_SPEECHDATA;


typedef struct tagUI_LonLat
{
    nuDOUBLE lon;  //in degree
    nuDOUBLE lat;  //in degree
}UI_LONLAT, *lpUI_LONLAT; 

typedef struct tagUI_DestPoint
{
    nuWCHAR name[128];
    UI_LONLAT pos;
}UI_DESTPOINT, *lpUI_DESTPOINT;


typedef struct tagUI_SEARCHSDKDATA
{
	nuINT   nSEARCH_MODE;
	nuWCHAR wKeyName[128]; //KEYWORD

}UI_SEARCHSDKDATA,*PUI_SEARCHSDKDATA;

typedef struct tagSDK_LISTDATA
{
	nuINT   nType;
	nuWCHAR wsName[128];	
	nuWCHAR wsCityName[128];
	nuWCHAR wsTownName[128];
	nuWCHAR wsOther[128];
	nuUINT  nDis;
	nuroPOINT pos;
}SDK_LISTDATA, *PSDK_LISTDATA;

typedef struct tagUI_GET_DATA
{
	UI_DAIWOO_BT		 DAIWOO_BT;
	GYROGPSDATA_EL		 ELEAD_GPS_STRUCT;
	UI_SPEECHDATA        SPEECHDATA;
	UI_DESTPOINT		 DESTPOINT;
	UI_GET_GPS_BUFFER    GET_GPS_BUFFER; 
	UI_SEARCHSDKDATA 	   SEARCH_SDK_DATA;
}UI_GET_DATA, *PUI_GET_DATA;

typedef struct tagICT_DATA
{
	nuBYTE nPassState;
	nuBYTE nReserve[3];
	nuLONG HighWayID;
	nuLONG HighWayCrossID;
}ICT_DATA, *PICT_DATA;



#define _ICT_ENTER 0x01
#define _ICT_PASS  0x02
#define _ICT_END   0x03
#endif


