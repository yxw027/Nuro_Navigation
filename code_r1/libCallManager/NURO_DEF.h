

#ifndef NURO_ENGINE_DEF_51805102_8250
#define NURO_ENGINE_DEF_51805102_8250
//int Nuro_NaviType;

typedef struct Tag_NURO_BUTTON_EVENT
{
	int nCode;
	unsigned char nButState;
	long nX;
	long nY;
}NURO_BUTTON_EVENT, *PNURO_BUTTON_EVENT;

typedef struct Tag_NURO_POINT
{
	long nX;
	long nY;
}NURO_POINT;

typedef struct Tag_TRANSFER_NURO_POINT
{
	bool bMaptoBmp;
	long nX;
	long nY;
}TRANSFER_NURO_POINT;

typedef struct Tag_NURO_POINT2
{
	int MouseCount;
	int nMouseType;		
	int VelocityX1;
	int VelocityY1;
	int nX1;
	int nY1;		
	int VelocityX2;
	int VelocityY2;
	int nX2;
	int nY2;
}NURO_POINT2, *PNURO_POINT2;

typedef struct Tag_NURO_NAVI_POINT
{
	int     nNaviIndex;
	int 	nRouteType;
	bool 	bRoute;
	//wchar_t RoadName[48];
	int 	nX;
	int 	nY;
}NURO_NAVI_POINT, *PNURO_NAVI_POINT;

typedef struct Tag_NURO_USER_CONFIG
{
	int	bNightDay;
	int	b3DMode;
	int	nFixOnRoad;
	int	bCCDVoice;
	int	bMoveWaiting;
	int	nMoveWaitTotalTime;
	int	nScreenType;
	int	nLanguage;
	int	nScreenLTx;
	int	nScreenLTy;
	int	nScreenWidth;
	int     nScreenHeight;//12
	int	nRouteListNum;
	int	nWaveTTSType;
	int	nMapDirectMode;
	int	nScaleShowMode;
	int	bShowLongLat;
	int  	nZoomAutoMode;
	int	bPlayCCD;
	int	nPlaySpeedWarning;
	int	nSimSpeed;
	int	bAutoDayNight;
	int	nTimeToNight;
	int	nTimeToDay;
	int	nCarIconType;
	int	nDayBgPic;
	int	nNightBgPic;
	int	nShowPOI;
	int	nRouteMode;
	int	nSplitScreenMode;
	int	nNowScreenType;
	int	nMoveType;
	int	bLastNavigation;
	int	nPastMarket;
	int	nZoomBmpWidth;
	int     nZoomBmpHeight;
	int	nVoiceControl;
	int     nMemoVoice;
	int	nSimuMode;
	int	nLastScaleIndex;//40
	int	nLastAutoMoveType;
	int     nVoiceIconTimer;
	int	bCrossAutoZoom;
	int	bBigMapRoadFont;
	int  	nUserSidebrake;
	int  	nKeySoundSwitch;
	int  	nSpNumInput;
	int     nKeyWordInput;
	int  	nSpeedWarningPic;
	int  	nTouchView;
	int	nUIType;
	int	nUILanguage;
	int  	nNaviTimeMode;//53
	int  	bHidenMapRoadInfo;
	int  	nSpeedWarningVoice;
	int  	nCenterTarget;
	int  	nRouteOption;
	int  	nUiControl;
	int  	nDefaultCityID;
	int  	nRoadInput;
	int  	nPoiInput;
	int  	bNightLight;
	int  	bOperateVoice;
	int  	bShowBigPic;
	int  	bTargetLine;
	int  	nScaleIdx; 
	int  	nSoundVolume;	
}NURO_USER_CONFIG, *PNURO_USER_CONFIG;

typedef struct Tag_NURO_USER_SET_DATA
{
	int 	bSetByDefault;
	int	bNightDay;
	int	b3DMode;
	int 	nLanguage;
	int	nMapDirectMode;
	int 	nZoomAutoMode;
	int 	nPlaySpeedWarning;
	int 	nSimSpeed;
	int 	bAutoDayNight;
	int	nCarIconType;
	int 	nDayBgPic;
	int 	nNightBgPic;
	int 	nRouteMode;
	int 	bLastNavigation;
	int 	nPastMarket;
	int 	nVoiceControl;
	int 	nLastScaleIndex;
	int 	nSpNumInput;
	int 	nKeyWordInput;
	int 	nSpeedWarningPic;
	int 	nUILanguage;
	int 	nSpeedWarningVoice;
	int 	nRoadInput;
	int 	nPoiInput;
	int	nScaleIdx;
	int 	nSoundVolume;
	int	nTMCSwitchFlag;
	int 	nTMCFrequency;
	int 	nTMCVoice;
	int	nTimeToNight;
	int	nTimeToDay;
	int     bCCDVoice;
	int     bBigMapRoadFont;
	int  	nRouteOption;
	int     nWaveTTSType;
	int 	nShowTip;
	int	nShowPOI;
}NURO_USER_SET_DATA, *PNURO_USER_SET_DATA;

typedef struct tagNAVI_LIST
{
	unsigned short		nIconType;
	wchar_t			wsName[LIST_NAME_NUM];
	unsigned long		nDistance;
	nuroPOINT		pt;
}NAVI_LIST, *PNAVI_LIST;

typedef struct tagNURO_NAVI_LIST
{
	unsigned short		nIconType;
	wchar_t			wsName[LIST_NAME_NUM];
	unsigned long 		nReserve;
	unsigned long		nDistance;
	long			nX;
	long			nY;
}NURO_NAVI_LIST, *PNURO_NAVI_LIST;

typedef struct Tag_NURO_PACK_PIC_PARAMETER
{
	int  nTop;
	int  nBottom;
	int  nLeft;
	int  nRight;	
	int  nPackPathID;
	int  nPicID;
	bool bInitSet;
	bool bLoadMemory;
	bool bEngineShow;
	bool bShowPic;
}NURO_PACK_PIC_PARAMETER, *PNURO_PACK_PIC_PARAMETER;

typedef struct Tag_NURO_SET_ROUTE_LIST
{
	bool bCount;
	int nNum;
	int nIndex;
}NURO_SET_ROUTE_LIST, *PNURO_SET_ROUTE_LIST;

typedef struct Tag_NURO_TMC_EVENT
{
	nuBYTE nMode;
	nuBYTE nShowEvent;
}NURO_TMC_EVENT, *pNURO_TMC_EVENT;

typedef struct Tag_NURO_SET_LANEGUIDE
{
	bool bCount;
}NURO_SET_LANEGUIDE, *PNURO_SET_LANEGUIDE;

#define NURO_DEF_ANDROID_ACTION_DOWN					0x00
#define NURO_DEF_ANDROID_ACTION_UP					0x01
#define NURO_DEF_ANDROID_ACTION_MOVE					0x02
#define NURO_DEF_ANDROID_ACTION_POINTER_DOWN				0x105
#define NURO_DEF_ANDROID_ACTION_POINTER_UP				0x06
//---------------------Set Engine------------------------------------//
#define NURO_DEF_UI_SET_BUTTON_EVENT				0x0001
#define NURO_DEF_UI_SET_MOVE_MAP				0x0002
#define NURO_DEF_UI_SET_USER_CONFIG				0x0003
#define NURO_DEF_UI_SET_NAVI_POINT				0x0004
#define NURO_DEF_UI_SET_CAR_POSITION				0x0005
#define NURO_DEF_UI_SET_USER_DATA				0x0006
#define NURO_DEF_UI_SET_MAP_CENTER				0x0007
#define NURO_DEF_UI_SET_NAVIGATION				0x0008
#define NURO_DEF_UI_SET_PACK_PIC				0x0009
#define NURO_DEF_UI_SET_TMC_EVENT				0x0010
#define NURO_DEF_UI_SET_CAR_ROAD				0x0011
#define NURO_DEF_UI_SET_EXIT					0x0012
#define NURO_DEF_UI_SET_NAVITHREAD				0x0013
//---------------------Set Engine------------------------------------//

//---------------------Get Engine------------------------------------//
#define NURO_DEF_UI_GET_UI_CONFIG				0x0001
#define NURO_DEF_UI_GET_ROUTE_LIST				0x0002
#define NURO_DEF_UI_GET_MAP_TO_BMP_POINT			0x0003
#define NURO_DEF_UI_GET_BMP_TO_MAP_POINT			0x0004
#define NURO_DEF_UI_GET_TMC_PATH_DATA				0x0005
//---------------------Get Engine------------------------------------//


#define NURO_DEF_NAVI_MAX					0x00FF
#define NURO_DEF_SEARCH_END					0x0100

#endif
