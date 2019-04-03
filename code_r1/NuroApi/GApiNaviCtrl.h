#ifndef _NURO_API_NAVI_CONTROL_20101223
#define _NURO_API_NAVI_CONTROL_20101223

#include "NuroDefine.h"
#include "NuroCommonApi.h"
#include "NuroModuleApiStruct.h"
#include "GApiOpenResource.h"
#include "NURO_DEF.h"

#define SET_USERCFG_NOEXTEND			0x00
#define SET_USERCFG_ZOOMIN				0x01
#define SET_USERCFG_ZOOMOUT				0x02
#define SET_USERCFG_COVERFROMDFT		0x04	

#define NC_WSTRING_MAX_NUM				50
#define MAX_LISTNAME_NUM                100

#define SET_MAPPT_NORMAL				0x0000
#define SET_MAPPT_COVERFROMDFT		    0x0001				

typedef struct tagLISTNODE
{
	nuWORD		nIconType;
	nuWCHAR		wsName[MAX_LISTNAME_NUM];
	nuDWORD		nDistance;
	nuroPOINT	point;
	nuWCHAR    wsIconName[MAX_LISTNAME_NUM];
}LISTNODE;
typedef struct tagNC_NAVITHREAD_DATA
{
	nuBYTE      nAutoMoveType;
}NC_NAVITHREAD_DATA;
typedef struct tagNC_DRAR_DATA
{
	nuBOOL      nddShowBigPic;
	nuBOOL		bIsMoving;
	nuBOOL      nddb3DMode;
	nuBOOL      nddbNightOrDay;
	nuBOOL      nddMoveWait;
	nuBOOL      nddSaveDataOver;
	nuBOOL      nddSimuReachDes; //模拟导航是否到达目的地

	nuBYTE	    nddMapDirectMode;
	nuBYTE		nddAutoMoveType;
	nuBYTE		nddNaviState;
	nuBYTE      nddScreenType;
	nuBYTE      nddSplitScreenType;
	nuBYTE      nddCrossNearType; //For SplitSreen

	//Car
	nuSHORT	    nddCarAngle; //0~359, east is 0, north is 90
	nuSHORT     nddMapAngle; //0~359, control the map show;

	nuWORD      nddNaviSimSpeed;
	nuWORD      nddShowSpeed;
	nuWORD		nddLimitSpeed;
	nuWORD	    nddCrossIconType; //CrossNode
	nuWORD      nddCCDNum; //For CCD
	nuWORD      nddSkyHeight;
	nuWORD      nddScalePixels; 
	nuWORD      nddNowCenterPoiCount;
	nuWORD      nddMapIndex;
	nuWORD      nddRtListNowNum;
	nuWORD      nddScreenLTx;
	nuWORD      nddScreenLTy;
	nuWORD      nddBmpLTx;
	nuWORD      nddBmpLTy;
	//
	nuWORD      nddCityCode;
	//
	nuUINT		nddMaxScale;
	nuUINT		nddMinScale;
	nuUINT		nddNowScale;
	nuUINT      nddPoiPageIndex; //Poi Out

	nuDWORD     nddDisToTarget;
	nuDWORD     nddDisToNextCross;
	nuDWORD     nddTimeToTarget;
	nuDWORD     nddTotalListCount;
	nuDWORD     nddOtherInfoAddr;
	nuDWORD     nddPoiNameAddr;
	nuDWORD		nddDisRealPic;
	nuDWORD     nddRealPicID; //RealPic
	nuDWORD     nddIgnorePicID; //RealPic

	nuLONG      nddCosConst;
	nuLONG      nddSinConst;
	nuLONG      nddMapCenterX;
	nuLONG      nddMapCenterY;

	nuWCHAR		nddNowRdName[NC_WSTRING_MAX_NUM];
	nuWCHAR		nddNextRdName[NC_WSTRING_MAX_NUM];
	nuWCHAR     nddCityName[NC_WSTRING_MAX_NUM];
	nuWCHAR     nddFriendCityName[NC_WSTRING_MAX_NUM];
	nuWCHAR     nddProvinceName[NC_WSTRING_MAX_NUM];

	nuroPOINT   nddCenterPoint;
	nuroPOINT	nddCarInScreen;
	nuroPOINT   nddCarInMapPt;
	nuroPOINT   nddRawGPSPt;
    nuroPOINT   nddCarGpsPt;
	LISTNODE    nddListNode[3];
    nuWORD      nddPoiIndex;
	nuWORD		nNowCenterPoiTYPECount[4];
	nuDWORD     nddPoiTypeNameAddr[4][4];
	LISTNODE    nddListPoiNode[4][4];
	NC_NAVITHREAD_DATA	nddNaviThreadData;
}NC_DRAR_DATA;

typedef struct tagUI_DRAW_DATA
{
	PNURO_CANVAS    uddpCanvas;
	nuroPOINT       uddCarInScreen;
}UI_DRAW_DATA;

//2011.01.18
typedef nuUINT(*SaveMapBmpProc)(NURO_CANVAS*);
//2011.02.16
typedef nuUINT(*DrawMapIconProc)(UI_DRAW_DATA);
//2011.03.14
typedef nuUINT(*DrawAnimationProc)(ANIMATIONDATA* pAni);

class CGApiNaviCtrl
{
public:
public:
    typedef struct tagNC_INIT_PARAMETER
    {
        NURO_SCREEN ncpScreen;
        nuPVOID* ppUserCfg;
    }NC_INIT_PARAMETER, *PNC_INIT_PARAMETER;

	enum em
    {
		
    };
	
public:
	CGApiNaviCtrl()
	{
	}
	virtual ~CGApiNaviCtrl()
	{
	}
public:
    virtual nuUINT  INcInit(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, nuPVOID* ppUserCfg, NURO_API_GDI* pNcInit, CGApiOpenResource* pApiRs) = 0;	
    virtual nuVOID  INcFree() = 0;	

	virtual nuBOOL  INcStartNaviCtrl(nuPVOID lpVoid) = 0;
	
	virtual nuUINT	INcSetCallBack(SaveMapBmpProc Func) = 0;
	virtual nuUINT  INcSetCallBackData(DrawMapIconProc Func) = 0;
	virtual nuUINT  INcSet_POI_CallBack(nuPVOID pFunc) = 0;
	virtual nuUINT	INcSetNaviThreadCallBack(nuPVOID pFunc) = 0;
	#ifdef USE_HUD_DR_CALLBACK
	virtual nuUINT  INcSet_HUD_CallBack(nuPVOID pFunc) = 0;
	virtual nuUINT  INcSet_DR_CallBack(nuPVOID pFunc) = 0;
	#endif 
	#ifdef ANDROID
	virtual nuUINT  INcSet_SOUND_CallBack(nuPVOID pFunc) = 0;
	#endif
	#ifdef ANDROID_GPS
	virtual nuINT  INcGPS_JNI_RecvData(const char *pBuff, int len) = 0;
	#endif
	virtual nuUINT  INcDrawAnimation(DrawAnimationProc Func) = 0;

	virtual nuBOOL  INcSoundPlay( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								  nuWORD nRoadNameNum, nuWCHAR *pRoadName ) = 0;
	virtual nuPVOID INcGetRoutingList(nuINT index, nuINT *ptotalCount) = 0;

	virtual nuUINT  INcGetDrawData(NC_DRAR_DATA &drawData) = 0;
	virtual nuUINT  INcGetDrawDataType(NC_DRAR_DATA &drawData) = 0;
	virtual nuBOOL  INcJumpToScreenXY(nuLONG x, nuLONG y, nuBOOL bMode = nuFALSE) = 0; //点击移图
	virtual nuUINT  INcMoveMap(nuBYTE nMoveType, nuBYTE nMoveStep, nuBOOL bStartOrStop) = 0;//连续移图

	virtual nuUINT	INcButtonEvent(nuUINT nCode, nuBYTE nButState) = 0;

	virtual nuBOOL  INcSetMarketPt(nuPVOID lpData) = 0; //设置导航点
	virtual nuBOOL  INcDelMarketPt(nuPVOID lpVoid) = 0;
	virtual nuBOOL  INcNavigation(nuBYTE nType) = 0; //模拟导航 
	virtual nuBOOL  INcIsNavigation() = 0;

	virtual nuUINT  INcSetUserCfg(nuUINT nMode = SET_USERCFG_NOEXTEND) = 0;
	//
	virtual nuBOOL  INcSetTimer(nuPVOID pTimerFunc, nuDWORD nTime) = 0;
	virtual nuBOOL  INcSetTimerEx(nuDWORD nID, nuDWORD nElapse, nuPVOID lpTimerProc) = 0;
	virtual nuBOOL  INcKillTimerEx(nuDWORD nID) = 0;
	virtual nuUINT  INcGetNavigationData(PNC_NAVI_DATA pNcNaviData) = 0;

	//3D Demo
	virtual nuUINT  INcStart3DDemo(nuTCHAR *pts3DName) = 0;
	virtual nuUINT  INcClose3DDemo() = 0;
	virtual nuUINT  INcSetScale(nuDWORD nScaleIdx) = 0;

	//OpenGLES API
	virtual nuUINT  INcStartOpenGL(SYSDIFF_SCREEN sysDiffScreen, const NURORECT* pRtScreen) = 0;
	//
	virtual nuBOOL  INcOperateMapXY(nuLONG x, nuLONG y, nuUINT nMouseCode) = 0;
	//
	virtual nuPVOID INcGetMapInfo(nuUINT nMapCode) = 0;
	virtual nuPVOID INcGetGPSData() = 0;
	//
	virtual nuUINT  INcSetMemoData(nuPVOID lpVoid) = 0;
	virtual nuUINT  INcSetMemoPtData(nuPVOID lpVoid) = 0; //轨迹
	virtual nuPVOID INcGetLaneInfo() = 0; //车道数信息
	//
	virtual nuLONG* INcGetScaleList(nuLONG *pScaleCount) = 0;
	virtual nuBOOL  INcShilftNaviThread(nuBOOL bOpenOrClose) = 0;	

	virtual nuUINT  INcGetSystemTime(PNURO_SYS_TIME pNuroTime) = 0;
	virtual nuBOOL	INcTMCRoute(nuLONG lTmcRouteCount) = 0;//added by daniel 20110830
	virtual nuBOOL	INcTOBERESETGYRO() = 0;//Added by Damon 20110906
	virtual nuBOOL	INcTOBEGETVERSION() = 0;//Added by Louis 20111110
	virtual nuBOOL	INcSaveLastPosition() = 0;
	virtual nuBOOL  INcNuroSetMAPPT(nuBYTE nSaveType) = 0; // Added by Damon 20120110
	virtual nuBOOL  INcSetRTFullView(nuLONG x, nuLONG y, nuUINT nCode) = 0; //Added by Xiaoqin 20120517
	virtual nuBOOL  INcResetScreenSize(nuWORD nWidth, nuWORD nHeight) = 0;
	virtual nuBOOL  INcGPS_ReSet(PGPSCFG pGpsSetData) = 0;
	virtual nuBOOL  INcGetTripInfo(nuPVOID lpVoid) = 0; //add by chang 2012.7.3
	virtual nuBOOL  INcReSetTripInfo(nuPVOID lpVoid) = 0; //add by chang 2012.7.3
	virtual nuBOOL  INcConfigNavi(nuLONG nDis) = 0; //Added by xiaoqin for ReRoute @2012.07.20
	virtual nuVOID	INcUIPlaySound(nuWCHAR *wsTTS) = 0;//Added by daniel for SoundNavi 20121119
	virtual nuUINT  INcUISendGPSData(const nuPVOID pOrGpsData1, const nuPVOID pOrGpsData2) = 0;//Added by daniel
	virtual nuBOOL  INcSet_TTS_Volume(const nuWORD TTSVolum ) = 0;//Added by daniel
	virtual nuBOOL  INcSet_TTS_Speed(const nuWORD TTSSpeed ) = 0;//Added by daniel
	virtual nuBOOL  INcSet_TTS_Pitch(const nuWORD TTSPitch ) = 0;//Added by daniel
	virtual nuVOID  INcGetSPTInfo(SPT_DATA &SPTData) = 0;//Added by daniel
	virtual nuUINT  INcReciveUIData(nuLONG DefNum, nuPVOID pUIData1, nuPVOID pUIData2) = 0;//Added by dnaiel 20130529
	virtual nuUINT  INcChoose_Car_Road(nuBYTE nChosenRoad) = 0;//Added by Daniel For Choose Road 20150207
	//----------------------------------For SDK----------------------------------//
	virtual nuVOID	INcMouseEvent(NURO_POINT2 Pt) = 0;
	virtual nuLONG	INcUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData) = 0;
	virtual nuPVOID	INcUIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData) = 0;
	virtual nuVOID  INcSetEngineCallBack(nuINT iFuncID, nuPVOID pFunc) = 0;
	//----------------------------------For SDK----------------------------------//
};

#endif
