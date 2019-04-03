#ifndef __NURO_ENGINE_STRUCTS_20080118
#define __NURO_ENGINE_STRUCTS_20080118

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "nuRouteData.h"
/*************************************************************************
 * 这个结构将贯穿一次画图的始末，由这个参数可以很详细的知道这次画图的情况。
 * 所以与这个结构有关的函数，是程序内部逻辑控制的最重要环节，依循这个结构
 * 可以了解程序的逻辑结构。这个部分应该也是程序最难控制和了解的部分，这里
 * 对程序新功能的增加很重要，了解了这个部分的逻辑才能找到比较好的方法去增
 * 加新的功能。
 *************************************************************************/
//--- For nCarCenterMode --------------------------//
#define CAR_CENTER_MODE_REMAIN					0x00
#define	CAR_CENTER_MODE_ONE_HALF_XY				0x01
#define CAR_CENTER_MODE_THREE_QUARTERS_Y		0x02
#define CAR_CENTER_MODE_X_ONE_Y_THREE			0x03
#define CAR_CENTER_MODE_XY_PARAMETER			0xFF
//--- For nBmpSizeMode ----------------------------//
#define BMPSIZE_EXTEND_NO						0x00
#define BMPSIZE_EXTEND_LEFT						0x01
#define BMPSIZE_EXTEND_UP						0x02
#define BMPSIZE_EXTEND_RIGHT					0x04
#define BMPSIZE_EXTEND_DOWN						0x08
//--- For nDrawMode -------------------------------//
#define DRAWMODE_STOP							0x00
#define DRAWMODE_MOVE							0x01

#define DRAWMODE_GPS							0x10
#define DRAWMODE_GPS_FAKE						0x11//not a real gps point
#define DRAWMODE_SIM							0x20
#define DRAWMODE_SIM_START						0x21

#define DRAWMODE_UI								0x30
#define DRAWMODE_POWERON						0x31
#define DRAWMODE_NUROJUMPMOVE					0x40
#define DRAWMODE_NUROGLIDEMOVE					0x41
#define DRAWMODE_TIMER							0x50

typedef struct tagACTIONSTATE
{
	nuBYTE	bResetScreenSize:1;//the Screen size changed
	nuBYTE	bNeedFixed:1;//需要拉回到中心点
	nuBYTE	bResetMapCenter:1;//reset the map center
	nuBYTE	bGlideMoveDraw:1;
	nuBYTE	bResetScale:1;//reset the scale, need to reset the map size. or reset the map Center, the scale will be changed. in smoothmove, skip reset scale.
	nuBYTE	bResetIconCarPos:1;//Reset carIcon position
	nuBYTE	bResetRealCarPos:1;//Reset the real car position
	nuBYTE	bResetMemBmpSize:1;
	nuBYTE	bNeedReloadMap:1;//Need Reload map
	nuBYTE	bDrawBmp1:1;
	nuBYTE	bDrawZoom:1;
	nuBYTE	bDrawInfo:1;
	nuBYTE	bShowMap:1;
	nuBYTE	bDrawRoadName:1;
	nuBYTE	bDrawPOIName:1;
	nuBYTE	bSetAngle:1;
	nuBYTE	bLoad2kBlock:1;
	nuBYTE	bIsSplitting:1;
	nuBYTE	bNeedExtendRotate:1;
	nuBYTE	bReDrawCrossZoom:1;
	nuBYTE	nBmpSizeMode;
	nuBYTE	nCarCenterMode;//Same to Bmp Center mode
	nuBYTE	nBmpLeftExtend;
	nuBYTE	nBmpUpExtend;
	nuBYTE	nBmpRightExtend;
	nuBYTE	nBmpDownExtend;
	nuBYTE	nDrawMode;//
	nuSHORT	nAngle;
	nuSHORT	nSpeed;
	NUROPOINT	ptNewMapCenter;	
	nuLONG	nScale;
}ACTIONSTATE, *PACTIONSTATE;

typedef struct tagZOOMCROSSSTATE
{
//	nuWORD		nDwIdx;
	nuDWORD		nCenterRdBlkIdx;
	nuWORD		nCenterRdIdx;
	nuLONG		nPicID;
	nuroPOINT	ptCenter;
}ZOOMCROSSSTATE, *PZOOMCROSSSTATE;
/********************************************************************************
 * 这个是程序内部会用到的一些全局变量的保存，尽量确定哪些可以是全局变量哪些可以是
 * 局部变量。全局变量在FileSystem中定义，但设置等一些改变最好都放在MathTool，其它
 * 的模块最好只对这个全局变量作读取的操作，可以根据将来的应用情况再来考良一下是否
 * 要改变定义的位置。
 ********************************************************************************/
//-- for nSplitScreenType --
#define SPLIT_SCREEN_NO				0x00
#define SPLIT_SCREEN_HAND			0x01
#define SPLIT_SCREEN_AUTO			0x02
//#define SPLIT_SCREEN_HAND_CLOSE		0x11
//#define SPLIT_SCREEN_HAND_OPEN		0x12
//#define SPLIT_SCREEN_AUTO_CLOSE		0x21
//#define SPLIT_SCREEN_AUTO_OPEN		0x22
//--- for BSD Draw Map ---------------------------------------------------------
#define BSD_MODE_COMMON				0x00
#define BSD_MODE_5k					0x01
#define BSD_MODE_BGL				0x02
typedef struct tagUISETDATA
{
	nuBYTE	bNightOrDay:1;//白天黑夜模式，1：黑夜，0：白天
	nuBYTE	b3DMode:1;//是否3D模式，1：3D，0：2D
	nuBYTE	bGPSReadyOld:1;//GPS 信号是否Ready
	nuBYTE	bFirstGPSReady:1;
	nuBYTE	bNavigation:1;//是否有规划路径
	nuBYTE	nMapDirectMode:2;
	nuBYTE	b3DViewChanging:1;
	//BYTE 1
	nuBYTE	nBsdDrawMap;//Draw map by Bsd
	//Byte 2
	nuBYTE	nScreenType;//屏幕分割的类型，具体参数的意义在NuroOpenedDefine.h文件中
	nuBYTE	nScaleIdx;//当前比例尺的index
	nuBYTE	nLanguage;//语言类型
	nuBYTE  nSSS;//
	//Byte 3, 4, 5, 6
	nuBYTE	nTTSWaveMode;//语音类型
	nuBYTE  nTotalScales;
	//Byte 7, 8
	nuWORD	nScreenLTx;//内存屏幕显示在真实窗口中的位置（左上角X值）
	//Byte 9, 10
	nuWORD	nScreenLTy;//内存屏幕显示在真实窗口中的位置（左上角Y值）
	nuWORD	nScreenWidth;//内存屏幕的宽
	//Byte 11, 12, 13, 14
	nuWORD  nScreenHeight;//内存屏幕的高
	nuWORD	nSkyHeight;//3D状态时，需要用图片遮掉的屏幕高度
	//Byte 15, 16, 17, 18
	nuBYTE	nBgPic;//19
	nuBYTE	nCarIconType;//20
	nuBYTE	nSplitScreenType;//21 Auto split or Hand split, is not same to the nSplitScreenMode in userConfig.
	nuBYTE	nShowLongLat;//22
	nuBYTE  nSSSS2[2];//23, 24
	nuLONG	nBsdStartScaleVelue;//25 26 27 28
	nuBYTE	bGPSReady;//29 GPS 信号是否Ready
	nuBYTE  nSave[3];//30, 31, 32
	nuLONG	nBglStartScaleValue;//33, 34, 35, 36
	
	PGPSDATA	pGpsData;//37, 38, 39, 40
	nuLONG	nLastError;//41, 42, 43, 44
	nuLONG	nScaleValue;//45, 46, 47, 48
	nuBYTE	n3DViewAngle;//49
	nuBYTE  nDataForDrawInfo[20];//50-69
	nuBYTE	nRESERVE[100-69];
}UISETDATA, *PUISETDATA;
/* Coordinate transformation struct and define */
#define LENCONST								500
#define MAPSCREEN								100//坐标转换用，不是实际的屏幕宽，而是根据精度需要设计的一个值

//#define SCALE_PIXELS							50

typedef struct tagTRANSFDATA
{
	nuBYTE	nBmpSizeMode;
	nuBYTE	nCarCenterMode;//Mark the now BmpCenterMode;
	nuSHORT	nBmpLTx;
	nuSHORT	nBmpLTy;//the top left point of the real bitmap is on the extended bitmap. these two values are both zero in 3D module.
	nuWORD	nBmpWidth;
	nuWORD	nBmpHeight;//the real size of map bitmap that would be shown in the screen.
	nuWORD	nBmpWidthEx;
	nuWORD	nBmpHeightEx;//the extended size of map bitmap part of which will be shown in the screen
	nuWORD	nBmpCenterX;
	nuWORD	nBmpCenterY;
	nuLONG	nSinConst;//show the car direction
	nuLONG	nCosConst;
//	nuLONG	nLengthConst;
	nuLONG	nMapWidth;
	nuLONG	nMapHeight;
	nuLONG	nMapCenterX;
	nuLONG	nMapCenterY;
	nuroSRECT		rtMapScreen;
	NURORECT		nuShowMapSize;
	BLOCKIDARRAY	blockIDShowArray;
	//NUROBITMAP		nuBmpMem1;
	//NUROBITMAP		nuBmpMem2;
	NURO_BMP		nuBmpMem1;
	NURO_BMP		nuBmpMem2;
}TRANSFDATA, *PTRANSFDATA;

typedef struct tagZOOMSCEENDATA
{
	nuBYTE			bZoomScreen:1;//表示现在是否需要显示放大屏幕
	nuBYTE			b3DMode:1;
	nuBYTE			bSetZoomData:1;
//	nuWORD			nZoomScreenLTx;//放大屏幕在内存屏幕的左上角X值
//	nuWORD			nZoomScreenLTy;//放大屏幕在内存屏幕的左上角Y值
//	nuWORD			nZoomScreenWidth;//放大屏幕的宽
//	nuWORD			nZoomScreenHeight;//放大屏幕的高
	nuWORD			nZoomBmpCenterX;
	nuWORD			nZoomBmpCenterY;
	nuWORD			nZoomBmpWidth;
	nuWORD			nZoomBmpHeight;
	nuWORD			nZoomSkyHeight;
	nuLONG			nZoomMapCenterX;
	nuLONG			nZoomMapCenterY;
	nuLONG			nZoomMapWidth;
	nuLONG			nZoomMapHeight;
	nuSHORT			nZoomAgnleDx;//show the car direction
	nuSHORT			nZoomAngleDy;
	nuLONG			nZoomAngleDis;
	nuroSRECT		rtZoomScreen;
	NURORECT		nuZoomMapSize;
	BLOCKIDARRAY	blockIDZoomArray;
	//nuroBITMAP		nuZoomBmp;
	NURO_BMP		nuZoomBmp;
	nuLONG			nCarInCrossX;
	nuLONG			nCarInCrossY;
	nuLONG			nPicID;
}ZOOMSCEENDATA, *PZOOMSCREENDATA;

#define FOLLOW_ROAD_MAX_POINT							10
#define _CCD_CHECK_MAX_DIS								500
#define _CCD_CHECK_MAX_ROADS							10
typedef struct tagCCDROAD
{
	nuWORD		nCcdMapIdx;//out for ccd
	nuWORD		nCcdRdIdx;//out for ccd
	nuDWORD		nCcdBlkIdx;//out for ccd
	nuroPOINT	ptFirst;
	nuroPOINT	ptEnd;
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuBYTE		nReserve[2];
}CCDROAD, *PCCDROAD;
typedef struct tagCCDINFO
{
	nuSHORT		nCarAngle;
	nuWORD		nCarMapIdx;
	nuWORD		nCarRdIdx;
	nuDWORD		nCarBlkIdx;
	nuroPOINT	ptCarNextCoor;
	nuroPOINT	ptCarMapped;
	nuWORD		nDisTotal;//
	nuBYTE		bNewInfo;
	nuBYTE		nCcdRdCount;
	CCDROAD		pCcdRoad[_CCD_CHECK_MAX_ROADS];
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuSHORT		nCCDRoadAngle;
}CCDINFO, *PCCDINFO;

#define _ROAD_CHECK_MAX_DIS						1000
#define MAX_LISTNAME_NUM						100
typedef struct tagNEXTROAD
{
	nuWORD		nNextMapIdx;
	nuWORD		nNextRdIdx;
	nuDWORD		nNextBlkIdx;
	nuroPOINT	ptFirst;
	nuroPOINT	ptEnd;
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuSHORT		nAngle;
	nuLONG		nNameAddr;
	nuWCHAR		wsRoadName[MAX_LISTNAME_NUM];
}NEXTROAD, *PNEXTROAD;
typedef struct tagNEXTROADINFO
{
	nuSHORT		nCarAngle;
	nuWORD		nRoadMapIdx;
	nuWORD		nRoadRdIdx;
	nuBYTE		bNewInfo;
	nuBYTE		nRoadCount;
	nuDWORD		nRoadBlkIdx;
	nuroPOINT	ptRoadNextCoor;
	nuroPOINT	ptRoadMapped;	
	NEXTROAD	NextRoad;
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuSHORT		nNextRoadAngle;
}NEXTROADINFO, *PNEXTROADINFO;

#define _ICT_CHECK_DIS		500
typedef struct tagCROSSROADINFO
{
	nuSHORT		nCarAngle;
	nuWORD		nRoadMapIdx;
	nuWORD		nRoadRdIdx;
	nuBYTE		bNewInfo;
	nuBYTE		nRoadCount;
	nuDWORD		nRoadBlkIdx;
	nuroPOINT	ptRoadNextCoor;
	nuroPOINT	ptRoadMapped;	
	NEXTROAD	pCrossRoad[16];
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuSHORT		nRoadAngle;
}CROSSROADINFO, *PCROSSROADINFO;

typedef struct tagCARINFO
{
	nuSHORT	nCarSpeed;
//	nuSHORT	nSimSpeed;
	nuSHORT	nShowSpeed;
	nuSHORT	nMiddleSpeed;//For calculating the time to destination
	nuSHORT	nCarAngle;//0~359, east is 0, north is 90
	nuSHORT	nCarAngleRaw;
	nuSHORT	nMapAngle;//0~359, control the map show;
	nuSHORT	nRotateAngle;//UI Rotate angle, -360~360, control the map show
	nuSHORT	nRawGPSAngle;//added by daniel @ 20120204
//	nuDWORD	nDisPassed;//For calculating speed.
	NUROPOINT	ptRawGPS;//added by daniel @ 20120204
	NUROPOINT	ptCarRaw;//the car position from gps or lastposition.txt, the real car position
	NUROPOINT	ptCarFixed;
	NUROPOINT	ptCarIcon;//the point to show the car Icon, Simulation point also will show the car Icon
	NUROPOINT	pRoadPtFollowed[FOLLOW_ROAD_MAX_POINT];//导航取GPS点时可以用来补点
}CARINFO, *PCARINFO;

/* For Kilometer Name by Daniel: */
#define _KM_CHECK_MAX_ROADS		   5
#define _KM_CHECK_MAX_DIS		1600
typedef struct tagKMROAD
{
	nuWORD		nKMMapIdx;//out for ccd
	nuWORD		nKMRdIdx;//out for ccd
	nuDWORD		nKMBlkIdx;//out for ccd
	nuroPOINT	ptFirst;
	nuroPOINT	ptEnd;
	nuWORD		nRoadClass;
	nuBYTE		nOneWay;
	nuBYTE		nReserve;
}KMROAD, *PKMROAD;
typedef struct tagKMINFO
{
	nuSHORT		nCarAngle;
	nuWORD		nCarMapIdx;
	nuWORD		nCarRdIdx;
	nuDWORD		nCarBlkIdx;
	nuroPOINT	ptCarNextCoor;
	nuroPOINT	ptCarMapped;
	nuWORD		nDisTotal;
	nuBYTE		bNewInfo;
	nuBYTE		nKMRdCount;
	nuBYTE		nOneWay;
	nuBYTE		nRoadClass;
	nuBYTE		nReserve[2];
	KMROAD		pKMRoad[_KM_CHECK_MAX_ROADS];
}KMINFO, *PKMINFO;
typedef struct tagPlayKM
{
	nuDWORD	  nDistance;
	nuLONG	  lptCount;
	NUROPOINT *ptKM;
}PlayKM;

/* For Timer: */
/* unit of time is 100 millisecond*/
#define MIN_UP_DOWN_DIF								200
#define MINI_SECOND_TO_MOVE							300

#define TIMER_TIME_UNIT								10//unit is ms
#define TIMER_TIME_NEWUNIT							100//unit is ms

#define MAX_SIM_SPACE_OF_TIME						80//unit is ms
#define MAX_GPS_SPACE_OF_TIME						100//unit is ms
#define MAX_SIM_END_WAITING							5000//unit is ms
#define MAX_NAVI_MOVE_WAIT_TOME						10000//unit is ms
#define MAX_UPDATE_SYSTIME							30000//Seconds

//#define GPS_FAKE_MAX_TIMES						2
//#define GPS_FAKE_SLICE							300

#define ROUTE_ANIMATION_SLICE						500

#define MAX_UPDATE_MAP_TIME_MS						(1000)//
#define MAX_GET_GPS_TIME_MS							(500)
#define MAX_SHOW_VOICE_MS							(100)
#define MAX_DAY_NIGHT_CHANGE						(10000)

#define AUTO_MOVE_STOP								MAP_NAVI_MODE_STOP
#define AUTO_MOVE_GPS								MAP_NAVI_MODE_GPS
#define AUTO_MOVE_SIMULATION						MAP_NAVI_MODE_SIMULATION
#define AUTO_MOVE_TIMER								MAP_NAVI_MODE_TIMER
#define AUTO_MOVE_DEMO3D                            0x11

#define WAIT_TYPE_NOWAIT							0x0000
#define WAIT_TYPE_MOVE								0x0001
#define WAIT_TYPE_SIMEND							0x0002

typedef struct tagTIMERDATA
{
	nuBYTE		nOldMoveType;
	nuBYTE		nAutoMoveType;
	nuBYTE		nSecond;
	nuBYTE		nMinute;
	nuBYTE		nDay;
	nuBYTE		nMonth;
	nuWORD		nYear;
	//
	nuWORD		nGetGpsTime;
	nuWORD		bUpdateMap:1;
	nuWORD		nUpdateMapTime:15;

	nuWORD      bResetGPS:1;
	nuWORD      nGPSComPort:15;
	nuWORD      nGPSBaudrate; // Added By Damon For GPS AutoInit
	//
	nuWORD		bSimMode:1;
	nuWORD		nSimTime:15;
	nuWORD		bGpsMode:1;
	nuWORD		nGpsTime:15;
	//
	nuWORD		bMoveWait:1;
	nuWORD		nMoveWaitTimer:15;//For movewaiting
	nuWORD		nMoveWaitType;
	//For Show Voice Icon
	nuDWORD		bShowVoiceIcon:1;
	nuDWORD		nVoiceNume:7;
	nuDWORD		nShowVoiceTimer:24;
	//For Show scale
	nuWORD		bShowScale:1;
	nuWORD		nShowScaleTimer:15;
	nuWORD		nReserve;
	//
	nuDWORD		nHour;//The working time of the process
	nuDWORD		nMiniSecond;
	//
	nuDWORD		bCanUpdateTime:1;
	nuDWORD		nUpDateTimeTimer:31;
	nuDWORD		nDayNightChangeTimer;//

	nuDWORD		bIsWaitingForMoving:1;
	nuDWORD		bIsMovingMap:1;
	nuDWORD		nMoveStartTicks:30;

	nuDWORD		bIsRouting:1;
	nuDWORD		nRouteTimer:14;
	nuDWORD     nRouteType:1;

	nuDWORD		nSysStartTick;

}TIMERDATA, *PTIMERDATA;

#define NAVI_PTS_GOING_MAX_NUM					50
typedef struct tagNAVIPOINTSGOING
{
	nuBYTE		nNowPoint;
	nuLONG		nDis;
	nuroPOINT	ptMapped;
	nuroPOINT	ptList[NAVI_PTS_GOING_MAX_NUM];
}NAVIPOINSGOING;
typedef struct tagSEEKEDROADDATA
{
	nuWORD		nRoadClass;
	nuWORD		nCityID;
	nuWORD		nTownID;
	nuWORD		nDwIdx;
	nuDWORD		nBlockID;
	nuDWORD		nBlkIdx;
	nuDWORD		nRoadIdx;
	nuDWORD		nNameAddr;
	nuWORD		nDis;
	nuWORD		nDisToNextCoor;
	nuroPOINT	ptMapped;
	nuWORD		nPtIdx;
	nuSHORT		nRdDx;
	nuSHORT		nRdDy;
	nuWORD		nLimitSpeed;
	nuroPOINT	ptNextCoor;
	nuBOOL      bGPSState;
	nuBYTE		nOneWay;
	nuBYTE		nMappingIdx;
	nuBYTE		nReserve;
	nuWCHAR		wsRoadName[MAX_LISTNAME_NUM];
	NUROPOINT   ptNode1;
	NUROPOINT   ptNode2;	
#ifdef ZENRIN
	nuBYTE		nSlope;
	nuBYTE		nLevel;
#endif
	nuLONG      nRoadAngle;
}SEEKEDROADDATA, *PSEEKEDROADDATA;
typedef struct tagSEEKEDPOIDATA
{
	nuWORD	nCityID;
	nuWORD	nTownID;
	nuBYTE	nPOIType1;
	nuBYTE	nPOIType2;
	nuBYTE	nPOINameLen;
	nuBYTE	nReserve;
	nuWORD	nMapIdx;
	nuDWORD nBlockID;
	nuDWORD nBlockIdx;
	nuDWORD	nNameAddr;
	nuDWORD	OnterInfoAddr;
	nuLONG	nDis;
	NUROPOINT	pCoor;
}SEEKEDPOIDATA, *PSEEKEDPOIDATA;

#define MAX_CITYTOWN_NAMNUM				50
#define MAX_NAMESTR_LEN					68
#define CENTER_NAME_NO_NEW				0
#define CENTER_NAME_ROAD				1
#define CENTER_NAME_ROAD_NEAR			2
#define CENTER_NAME_POI					3
#define CENTER_NAME_CITYTOWN			4


#define ZOOM_TYPE_DEFAULT				0
#define ZOOM_TYPE_NO_IN					1
#define ZOOM_TYPE_NO_OUT				2

#define POI_CENTER_MAX_NUM				50

////////////////////////////////////////////////////////
/*
#define _CAR_ROAD_SEG_EXTEND_DIS		500
#define _CAR_ROAD_SEG_MAX_MEMORY		(4*1024)
typedef struct tagCAR_ROAD_SEG
{
}CAR_ROAD_SEG, *PCAR_ROAD_SEG;
typedef struct tagCAR_ROAD_NODE
{
	nuWORD				nMapIdx;
	nuWORD				nRoadIdx;
	nuDWORD				nBlkIdx;
	nuroPOINT*			ptCoor;
	nuBYTE				nCoorAddr;
	nuBYTE				nAngle;//nAngle/2
	nuBYTE				bTrunkRoad;//
	nuBYTE				nNextSegCount;
	nuWORD				nDis;
	tagCAR_ROAD_NODE*	pCross;
}CAR_ROAD_NODE, *PCAR_ROAD_NODE;
typedef struct tagCAR_ROAD_DATA
{
	PCAR_ROAD_SEG	pRoadList;//allocat memory from pCarRoadMem
	nuWORD			nMemUsed;
	nuBYTE			pCarRoadMem[_CAR_ROAD_SEG_MAX_MEMORY];
}CAR_ROAD_DATA, *PCAR_ROAD_DATA;
*/
typedef struct tagDRAWINFOMAP
{
	nuWORD			bCarInRoad:1;//0:out of road. 1:on the road. if car in routed line, this value is 1;
	nuWORD			bCarDirectCorrect:1;//0:Direct is opposite to road directtion or routed way direction.
	nuWORD			bShowScale:1;
	nuWORD			nZoomType:2;
	nuWORD			bFindCenterRoad:1;
	nuWORD			bFindCenterPoi:1;
	nuWORD			bCityTownUpdata:1;
	nuWORD			bCenterPoiUpdata:1;
	nuWORD			bCenterNameUpdata:1;
	nuWORD			nSpeedLimit:4;
	nuWORD			nCenterNameType;
	nuWORD			nCenterCityID;
	nuWORD			nCenterTownID;
	nuWORD			nCenterMapIdx;
	nuWORD			nScalePixels;
	nuDWORD			nScaleValue;
	SEEKEDROADDATA	roadCenterOn;
	SEEKEDROADDATA	roadCarOn;
	SEEKEDPOIDATA	poiCenterOn;
	nuWCHAR			wsCityTown[MAX_CITYTOWN_NAMNUM];//half for city name, half for town name
	nuWCHAR			wsCenterName[MAX_NAMESTR_LEN];
	nuWCHAR			wsCenterPoi[MAX_NAMESTR_LEN];
	nuroPOINT		ptCarInScreen;
	nuroPOINT		ptMapCenterInScreen;
	nuWORD			nPoiSltIdx;
	nuWORD			nNowCenterPoiCount;
	SEEKEDPOIDATA	poiCenterNearList[POI_CENTER_MAX_NUM];
	nuWORD			nNowCenterPoiTYPECount[4];
	SEEKEDPOIDATA	poiCenterNearTYPEList[4][4];
	nuLONG			nCarAngleDx;
	nuLONG			nCarAngleDy;
	nuLONG          nCCDNum;
	nuBOOL			bUsedKm;
	nuBOOL			bUsedICT;
	nuBOOL			bGPSState;
	nuBYTE			nGpsStatus;
	nuBYTE			nTMCUsed;
	nuBYTE			nReserve;
}DRAWINFOMAP, *PDRAWINFOMAP;

//DrawInfoNavi struct
//.CrossNearType
#define CROSSNEARTYPE_DEFAULT					0x00
#define CROSSNEARTYPE_SCALE0_IN					0x01
#define CROSSNEARTYPE_SCALE1_IN					0x02
#define CROSSNEARTYPE_COVER						0x10
#define CROSSNEARTYPE_SCALE0_OUT				0x11
#define CROSSNEARTYPE_SCALE1_OUT				0x12
#define CROSSNEARTYPE_PIC_IN					0x05
#define CROSSNEARTYPR_PIC_IN_SMALL				0x06
#define CROSSNEARTYPE_PIC_OUT					0x15
#define CROSSNEARTYPE_HIGHWAY					0x20
#define CROSSNEARTYPE_STOPNAVI					0x30

#define ROUTELIST_TYPE_DEFAULT					0x00
#define ROUTELIST_TYPE_CROSS					0x01
#define ROUTELIST_TYPE_PASSNODE					0x02
#define ROUTELIST_TYPE_MAXTURE					0x03


#define CROSSZOOM_TYPE_DEFAULT					0
#define CROSSZOOM_TYPE_IN						1
#define CROSSZOOM_TYPE_OUT						2
typedef struct tagSHOWNODE
{
	nuWORD		nIconType;
	nuWCHAR		wsName[MAX_LISTNAME_NUM];
	nuDWORD		nDistance;
	nuroPOINT	point;
	nuWCHAR		wsIconName[MAX_LISTNAME_NUM];
}SHOWNODE, *PSHOWNODE;
typedef struct tagTMCDATA
{
	nuUINT		nRoadType;
	nuLONG		nDistance;
}TMCDATA, *PTMCDATA;
#define CROSS_ARROW_POINT_NUM			50
typedef struct tagCROSSNODE
{
	nuWORD		nIconType;
	nuWCHAR		wsName[MAX_LISTNAME_NUM];
	nuSHORT		nAngleInDx;
	nuDWORD		nDistance;
	nuroPOINT	point;
	nuSHORT		nAngleInDy;
	nuSHORT		nAngleOut;
	nuLONG		nRealPicID;
	nuLONG		nCrossIndex;
	nuroPOINT	ptCrossList[CROSS_ARROW_POINT_NUM];
	nuBYTE		nCrossPtCount;
	nuBYTE		nChangeCross;//
	nuBYTE		nIdxCrossInList;//记录ptCrossList中的点，哪个是路口的点。
	nuBYTE		nReserve;
	nuWORD		nDwIdx;
	nuWORD		nCityID;
	nuWORD		nTownID;
	nuWORD		nRoadIdx;
	nuDWORD		nBlkIdx;
	nuLONG		nRealPicDis;
	nuWORD      RoadType;
}CROSSNODE, *PCROSSNODE;
#define MAX_LANE_COUNTS					16
#define MAX_ARROWS_OF_LANE				6
typedef struct tagSINGLE_LANE_INFO
{
	nuBYTE		nLaneType:4;
	nuBYTE		nLaneCar:4;
	nuBYTE		nArrowCount:4;//本车道有几个箭头
	nuBYTE      nLaneNo:4;
	nuBYTE      nLanHighLight;
	nuBYTE		pASList[MAX_ARROWS_OF_LANE];
}SINGLE_LANE_INFO,*PSINGLE_LANE_INFO;
typedef struct tagNOW_LAME_INFO
{
	nuBYTE				nNowCount;//本路段有几个车道
	nuBYTE              bReGetInfo;
	nuBYTE				nReserve[2];
	SINGLE_LANE_INFO	pLaneList[MAX_LANE_COUNTS];
}NOW_LAME_INFO,*PNOW_LAME_INFO;

typedef struct tagNOW_LAME_GUIDE
{
	nuLONG LaneCount;
	nuBOOL bLight[MAX_LANE_COUNTS];
	nuBYTE LightNum[MAX_LANE_COUNTS];
}NOW_LAME_GUIDE,*pNOW_LAME_GUIDE;

typedef struct tagSPT_DATA
{
	nuBYTE			nSPTCount;
	nuBYTE			nSPTIndex;
	nuSHORT			Reserve;
	nuWCHAR			wsSPTName[MAX_LANE_COUNTS][100];
}SPT_DATA, *pSPT_Data; 

typedef struct tagDRAWINFONAVI
{
	nuWORD			bCarInNaviLine:1;//whether car is in the navigation road
	nuWORD			nCarNotInNaviLineTimer:7;//the times that the car is not in the navigation road continuously
	nuWORD			bCarDirectSameToNaviLine:1;//行车方向是否和导航路径一致
	nuWORD			nRtListPageNum:4;//number of each page;
	nuWORD			nRtListNowNum:4;//现在RouteList中有效的节点数
	nuWORD			nRtListNowIdx;//
	nuBYTE			nRtListType;//RouteList的类型
	nuBYTE			nInfoNaviType;
	nuBYTE			nCrossNearType;
	nuBYTE			nCrossZoomType;
	nuWORD			nDisToCrossZoomType;
	nuWORD          nDisToBeforeCross;
	nuDWORD			nTotalListCount;
	nuDWORD			nDisToTarget;
	nuDWORD			nTimeToTarget;
	CROSSNODE		nextCross;
	CROSSNODE       HighWayCross;
	SHOWNODE		nextCityTown;
	SHOWNODE*		pRtList;
	NOW_LAME_GUIDE  LaneGuide;
	nuLONG          lTurnFlag;
	//NOW_LAME_INFO	laneInfo;//车道信息
	nuLONG          nIgnorePicID;//!=0需要忽略的PICID,=0没有忽略的
	nuLONG          RealPicDis;
	nuLONG          RealPicID;
	nuLONG		    bLastCross;
	NUROPOINT	    PrePt;
	nuBYTE			nRoadState;
	SPT_DATA		SPTData;
	nuLONG          lEleadIconType;
	nuLONG			lUnsureDis;
	nuLONG			lNormalDis;
	nuLONG			lSlowDis;
	nuLONG			lCrowdDis;
	nuLONG          nTMCCount;
	TMCDATA*        pTMCList;
	nuLONG			lTMCCrowdDis;
	nuBYTE          bTMCListUsed;
	nuBYTE			nReserve[2];
}DRAWINFONAVI, *PDRAWINFONAVI;


typedef nuBOOL(*NC_SoundPlayProc)(nuBYTE, nuWORD, nuWORD, nuWORD, nuWORD, nuWCHAR*);
typedef nuVOID(*NC_STOPSOUNDPLAYINGProc)();
typedef nuVOID(*NC_Navi_CleanSoundProc)();
typedef nuBYTE(*NC_GetNaviStateProc)();
typedef nuBYTE(*NC_SetNaviStateProc)(nuBYTE NaviState);
typedef nuBOOL(*NC_NT_ExitProc)();
//--- Route data -----------------
//#include the file defined RouteData struct in the header of this file
#define MAX_TARGET						20
typedef struct tagTARTGETINFO
{
	nuroPOINT	BaseCoor;//使用者的坐标
	nuroPOINT	FixCoor;//拉回的坐标
	nuLONG		MapIdx;
	nuWORD		CityID;
	nuWORD		TownID;
	nuWORD		BlockIdx;
	nuWORD		RoadIdx;
}TARTGETINFO,*PTARTGETINFO;
typedef struct tagROUTEGLOBAL
{
	nuSHORT		CarSpeed;// km per hr
	nuSHORT		nCarAngle;// 0~359, east is 0, north is 90
	nuSHORT		RoutingRule;// 0:short_time ,1:1st_Freeway ,2:2nd_Freeway ,3:No_Freeway
	nuBYTE		RouteCount; //LCMM
	nuBYTE		TargetCount;
	TARTGETINFO pTargetInfo[MAX_TARGET];
	ROUTINGDATA	routingData;
}ROUTEGLOBAL, *PROUTEGLOBAL;

typedef struct tagGLOBALENGINEDATA
{
	const nuTCHAR*		pTsPath;
	nuBOOL				bWaitQuit;
	UISETDATA			uiSetData;
	TRANSFDATA			transfData;
	ZOOMSCEENDATA		zoomScreenData;
	CARINFO				carInfo;
	TIMERDATA			timerData;
	DRAWINFOMAP			drawInfoMap;
	DRAWINFONAVI		drawInfoNavi;
	ROUTEGLOBAL			routeGlobal;
	NC_SoundPlayProc	NC_SoundPlay;
	NC_SoundPlayProc	NC_Navi_SoundPlay;
	NC_Navi_CleanSoundProc NC_Navi_CleanSound;
	PCROSSFOLLOWED		pCrossFollowed;
	CallBackFuncProc	pfCallBack;
	CallBackFuncProc	pfNaviThreadCallBack;
	NC_STOPSOUNDPLAYINGProc NC_StopSoundPlaying;
	NC_GetNaviStateProc	 NC_GetNaviState;
	NC_SetNaviStateProc	 NC_SetNaviState;
	NC_NT_ExitProc		 NC_NT_Exit;
//	CAR_ROAD_DATA		carRoadData;//
}GLOBALENGINEDATA, *PGLOBALENGINEDATA;

typedef struct tagDRAWINFOCOMMON
{
	nuBYTE			bShowMoveBut;//8 buttons for moving
	nuBYTE			bShowRouteListBut;
	nuBYTE			bShowStartSimBut;
	nuBYTE			bShowStartGpsBut;
//	nuBYTE			bShowRouteInfoBut;
//	nuBYTE			bShowBackToUI;
//	nuBYTE			bShowLangLat;
	nuLONG			nRealPicID;
	nuWORD			nMapIdx;
	/*NUROBITMAP		nuBmpBg;
	NUROBITMAP		nuBmp3D;
	NUROBITMAP		nuBmpCrossBg;*/
	NURO_BMP		nuBmpBg;
	NURO_BMP		nuBmp3D;
	NURO_BMP		nuBmpCrossBg;
	const nuTCHAR*	pTsPath;
}DRAWINFOCOMMON, *PDRAWINFOCOMMON;
typedef struct tagDRAWINFO
{
	const UISETDATA*		pcUISet;
	const TRANSFDATA*		pcTransf;
	const CARINFO*			pcCarInfo;
	const TIMERDATA*		pcTimerData;
	const DRAWINFOMAP*		pcDrawInfoMap;
	const DRAWINFONAVI*		pcDrawInfoNavi;
	const ZOOMSCEENDATA*	pcZoomScreenData;
	DRAWINFOCOMMON	drawInfoCommon;
}DRAWINFO, *PDRAWINFO;


#define MENU_TYPE_DEFAULT					0x00

//#define M_STATE_CLEAN_NEW					0xF0
#define M_STATE_DEFAULT						0x00
#define M_STATE_DOWN						0x01
#define M_STATE_UP							0x01
#define M_STATE_FOCUS						0x02
#define M_STATE_HIDE						0x0E
#define M_STATE_DISABLE						0x0F

#define MENU_SELECT_MODE_NONE				0x00
#define MENU_SELECT_MODE_MAIN				0x01
#define MENU_SELECT_MODE_SUB				0x02	
#define SUM_MENU_MAX_BUT_MAX				4
typedef struct tagSUBMENUINFO
{
	nuSHORT		nLeft;
	nuSHORT		nTop;
	nuSHORT		nWidth;
	nuSHORT		nHeight;
	nuBYTE		nSubMenuID[SUM_MENU_MAX_BUT_MAX];
	nuBYTE		nSubState[SUM_MENU_MAX_BUT_MAX];
	//NUROBITMAP	subMenuBmp;
	NURO_BMP	subMenuBmp;
}SUBMENUINFO, *PSUBMENUINFO;	
#define MAX_MENU_ITEM_NUM					8		
#define MAX_BTN_ITEM_NUM					30		
typedef struct tagNUROMENUINFO
{
	nuBYTE		nMenuState;//0 hide, !0 show;
	nuBYTE		nMenuShowType;
	nuBYTE		nMenuType;
	nuBYTE		nNowButNum;//
//	nuBYTE		nShowAlign;//
	nuBYTE		nIdxButSelected;
	/*NUROBITMAP	menuBmp;
	NUROBITMAP	menuBmpBK;
	NUROBITMAP	menuBmpOld;*/
	NURO_BMP	menuBmp;
	NURO_BMP	menuBmpBK;
	NURO_BMP	menuBmpOld;
	nuSHORT		nX;
	nuSHORT		nY;
	nuWORD		nMenuButID[MAX_MENU_ITEM_NUM];
	nuBYTE		nStatue[MAX_MENU_ITEM_NUM];
	nuBYTE		nSubMenuNum[MAX_MENU_ITEM_NUM];
	nuBYTE		nIdxSubButSelected;
	nuBYTE		bShowSubMenu:1;
	nuBYTE		bDrawSubMenu:1;
	nuBYTE		nSelectMode:6;
	SUBMENUINFO	subMenu;
}NUROMENUINFO, *PNUROMENUINFO;

#define RN_MAX_LEN							100
#define NO_NAME_ROAD						(0xFF)
typedef struct tagRNEXTEND
{
	nuBYTE	nRNLen;
	nuBYTE	nIconType;
	nuWORD	nIconNum;
}RNEXTEND, *PRNEXTEND;


//for get simulation data from InNavi
typedef struct tagSIMULATIONDATA
{
	nuLONG	nX;
	nuLONG	nY;
	nuLONG	nAngle;
}SIMULATIONDATA, *PSIMULATIONDATA;

typedef struct tagANIMATIONDATA
{
	nuBOOL	bReset;
	nuBYTE	nShowMode;
	nuINT		nType;
	NURORECT ShowRect;
	const nuTCHAR*	pPath;
}ANIMATIONDATA,*PANIMATIONDATA;

#define  MAX_BOX_ITEM_NUM		4
#define  NURO_MB_OK				_MSGBOX_OK
#define  NURO_MB_CANCEL			_MSGBOX_CANCEL

#define	 NURO_MB_DLG			_MSGBOX_DLG
#define  NURO_MB_OUT			_MSGBOX_OUT

typedef struct tagMESSAGEBOXDATA
{
	nuINT		nType;
	nuBYTE  nBtnNum;
	nuBYTE	nSelectBut;
	nuBYTE	nButState[MAX_BOX_ITEM_NUM];
}MESSAGEBOXDATA,*PMESSAGEBOXDATA;

//**For Screen change
typedef struct tagSCREENTRANSF
{
	nuSHORT	nOldCenterX;
	nuSHORT	nOldCenterY;
	nuSHORT	nNewCenterX;
	nuSHORT	nNewCenterY;
	nuSHORT	nAngle;//angle diff of new and old screen
	nuSHORT	nZoomN;
	nuSHORT	nZoomD;//the new screen Scale = (nZoomN/nZoomD) * the old Screen Scale;
}SCREENTRANFS, *PSCREENTRANSF;


//**For NaviLine Data*********************************
#define LINE_ZOOMRD_MAX_POINTS				(256)
#define LINE_NAVIRD_MAX_POINTS				(1000)
#define MEM_SIZE_NAVI_LINE					((LINE_ZOOMRD_MAX_POINTS + LINE_NAVIRD_MAX_POINTS)*sizeof(nuroPOINT) + 64)
#define MEM_SIZE_MAP_POINTS					(50000)
#define MEM_SIZE_MAP_SHPNUM					(10000)
#define MEM_SIZE_MAP_OBJ					(4*1024)


typedef struct tagZOOMNAVILINE
{
	nuBYTE	nLine1Count;
	nuBYTE	nLine2Count;
	nuBYTE	nLine3Count;
	nuBYTE	nLine4Count;
	nuroPOINT	ptZoomLine[LINE_ZOOMRD_MAX_POINTS];
}ZOOMNAVILINE, *PZOOMNAVILINE;
typedef struct tagNAVILINEPOINT
{
	nuWORD	nLine1Count;
	nuWORD	nLine2Count;
	nuWORD	nLine3Count;
	nuWORD	nLine4Count;
	nuroPOINT	ptNaviLine[LINE_NAVIRD_MAX_POINTS];
}NAVILINEPOINT, *PNAVILINEPOINT;
typedef struct tagNAVILINEDATA
{
	ZOOMNAVILINE	zoomLine;
	NAVILINEPOINT	naviLine;
}NAVILINEDATA, *PNAVILINEDATA;

//**For Shape memory*******************************
//For Mdw Screen Point
typedef struct tagSCREENHEAD
{
	nuWORD	nShapCount;
	nuWORD	nTotalVertex;
}SCREENHEAD, *PSCREENHEAD;
#define SHPOJB_TYPE_COUNT		0x03
#define SHPOBJ_TYPE_ROAD		0x00
#define SHPOBJ_TYPE_BGLINE		0x01
#define SHPOBJ_TYPE_BGAREA		0x02
typedef struct tagSCREENOBJSHP
{
	nuBYTE	objType;//Road bgline or bgarea
	nuBYTE	nType;//Road class, bgline area...
	nuWORD	nPointCount;
}SCREENOBJSHP, *PSCREENOBJSHP;

typedef struct tagSCREENOBJ
{
	SCREENHEAD		head;
	SCREENHEAD		pShpCount[SHPOJB_TYPE_COUNT];
	SCREENOBJSHP	pShape[MEM_SIZE_MAP_SHPNUM];
	nuroPOINT		ptVertex[MEM_SIZE_MAP_POINTS];
}SCREENOBJ, *PSCREENOBJ;

typedef struct tagDRAWMEMORY
{
	nuWORD	nMapSHPMemIdx;//For map shape(Road, bgline, bgarea and so on...);
	nuWORD	nMapObjMemIdx;//For map objects(Road name, POI and so on...)
	nuWORD	nLineMemIdx;//For NaviLine Data Map and zoom Screen;
}DRAWMEMORY, *PDRAWMEMORY;

//-------------------------------------------
#define MAX_TRACE_ONERECORD_NUM				10000
#define MAX_TRACE_TOTAL_POINTS				(MAX_TRACE_ONERECORD_NUM*MAX_TRACE_RECOAD_NUM)

#define TRACE_FILE_HEAD_SIZE				(sizeof(TRACEFILEHEAD) + sizeof(TRACEHEAD) * MAX_TRACE_RECOAD_NUM )
#define MAX_TOTAL_TRACE_FILE_SIZE			(sizeof(TRACEFILEHEAD) + sizeof(TRACEHEAD) * MAX_TRACE_RECOAD_NUM + sizeof(nuroPOINT) * MAX_TRACE_TOTAL_POINTS)
//For tagTRACEMEMORY::nTraceState
#define TRACE_STATE_CLOSE			0x00
#define TRACE_STATE_OPEN			0x01
#define TRACE_STATE_SUSPEND			0x02
//For tagTRACEMEMORY::nShowTrace
#define TRACESHOW_STATE_NO			0x00
#define TRACESHOW_STATE_NOW			0x01
#define TRACESHOW_STATE_RECORD		0x02
typedef struct tagTRACEMEMORY
{
	nuBYTE	nReserve[1];
	nuBYTE	bLock;
	nuBYTE	nTraceState;
	nuBYTE	nShowTrace;//0: NO; 1: NowTrace; 2: RecordTrace
	nuWORD	nPointCount;
	nuWORD	nShowCount;
	nuWORD	nNewTraceMemIdx;
	nuWORD	nShowTraceMemIdx;
}TRACEMEMORY, *PTRACEMEMORY;

typedef struct tagOTHERGLOBAL
{
	PMEMODATA	pMemoData;
	PMEMOPTDATA pMemoPtData;
}OTHERGLOBAL, *POTHERGLOBAL;

//For LibFS_SeekForRoads()
#define SEEK_ROADS_MAX_COUNT				20
typedef struct tagSEEK_ROADS_IN
{
	nuroPOINT	point;
	nuLONG		nSeekDis;
	nuSHORT		nAngle;// <= 0 means ignoring angle;
}SEEK_ROADS_IN, *PSEEK_ROADS_IN;
typedef struct tagSEEK_ROADS_OUT
{
	nuBYTE			nCountSeeked;
	SEEKEDROADDATA	roadsList[SEEK_ROADS_MAX_COUNT];
}SEEK_ROADS_OUT, *PSEEK_ROADS_OUT;

//For LibFS_GetRoadSegData()
typedef struct tagROAD_SEG_ITEM
{
	nuDWORD	nBlockIdx;//in
	nuDWORD	nRoadIdx;//in
	nuDWORD	nNameAddress;//out
}ROAD_SEG_ITEM, *PROAD_SEG_ITEM;
typedef struct tagROAD_SEG_DATA
{
	nuBYTE			nCountUsed;//in
	PROAD_SEG_ITEM	pRoadSeg;
}ROAD_SEG_DATA, *PROAD_SEG_DATA;

//For GpsOpen
typedef struct tagFEEDBACKDATA
{
	nuBOOL		bCarOnRoad;
	nuroPOINT	ptRaw;
	nuroPOINT	ptFixedOnRoad;
	nuLONG		nRoadDx;
	nuLONG		nRoadDy;
	nuLONG		nDisFixedToRoad;
}FEEDBACKDATA, *PFEEDBACKDATA;
//For GDI
//typedef nuLONG(*Get3DThickProc)(nuLONG thick, nuLONG y);
typedef struct tagFUNC_FOR_GDI
{
//	Get3DThickProc	Get3DThick;
	nuPVOID		lpFcGet3DThick;
}FUNC_FOR_GDI, *PFUNC_FOR_GDI;


#endif
