// INaviControl.h: interface for the CINaviControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INAVICONTROL_H__9970953C_BE6F_4517_B573_D7F64A623F57__INCLUDED_)
#define AFX_INAVICONTROL_H__9970953C_BE6F_4517_B573_D7F64A623F57__INCLUDED_

#include "GApiNaviCtrl.h"
#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroOpenedDefine.h"
#include "NuroNaviCtrlConfig.h"
#include "NuroCommonApi.h"
#include "NURO_DEF.h"

class CINaviControl : public CGApiNaviCtrl
{
public:
	CINaviControl();
	virtual ~CINaviControl();

	virtual nuUINT  INcInit(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, nuPVOID* ppUserCfg, NURO_API_GDI* pNcInit, CGApiOpenResource* pApiRs);
    virtual nuVOID  INcFree();

	virtual nuBOOL  INcStartNaviCtrl(nuPVOID lpVoid);	
	virtual nuUINT	INcSetCallBack(SaveMapBmpProc Func);
	virtual nuUINT  INcSetCallBackData(DrawMapIconProc Func);
	virtual nuUINT  INcSet_POI_CallBack(nuPVOID pFunc);
	virtual nuUINT  INcSetNaviThreadCallBack(nuPVOID pFunc);
	#ifdef	USE_HUD_DR_CALLBACK
	virtual nuUINT  INcSet_HUD_CallBack(nuPVOID pFunc);
	virtual nuUINT  INcSet_DR_CallBack(nuPVOID pFunc);
	#endif
	#ifdef ANDROID
	virtual nuUINT INcSet_SOUND_CallBack(nuPVOID pFunc);	
	#endif		
	#ifdef ANDROID_GPS
	virtual nuINT  INcGPS_JNI_RecvData(const nuCHAR *pBuff, nuINT len);
	#endif
	virtual nuUINT  INcDrawAnimation(DrawAnimationProc Func);

	virtual nuBOOL  INcSoundPlay( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								  nuWORD nRoadNameNum, nuWCHAR *pRoadName );
	static nuBOOL  INcNavi_SoundPlay( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								  nuWORD nRoadNameNum, nuWCHAR *pRoadName );
	static nuBOOL   INcSoundPlay1( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								    nuWORD nRoadNameNum, nuWCHAR *pRoadName );
	static nuVOID  INcNavi_CleanSound();
	static nuVOID  INcStopSoundPlaying();
	static nuBOOL  INcSetNaviParser(nuINT nMode,nuINT nType);
	static nuBYTE  INcGetNaviState();
	static nuBYTE  INcSetNaviState(nuBYTE nNaviState);
	static nuBOOL  INcNuro_NT_Exit();//Added by daniel

	virtual nuPVOID INcGetRoutingList(nuINT index, nuINT *ptotalCount);

	virtual nuUINT  INcGetDrawData(NC_DRAR_DATA &drawData);
	virtual nuUINT  INcGetDrawDataType(NC_DRAR_DATA &drawData);
	virtual nuBOOL  INcJumpToScreenXY(nuLONG x, nuLONG y, nuBOOL bMode = nuFALSE); //点击移图
	virtual nuUINT  INcMoveMap(nuBYTE nMoveType, nuBYTE nMoveStep, nuBOOL bStartOrStop); //连续移图

	virtual nuUINT	INcButtonEvent(nuUINT nCode, nuBYTE nButState);

	virtual nuBOOL  INcSetMarketPt(nuPVOID lpData); //设置导航点
	virtual nuBOOL  INcDelMarketPt(nuPVOID lpVoid);
	virtual nuBOOL  INcNavigation(nuBYTE nType); //模拟导航	
	virtual nuBOOL  INcIsNavigation();

	virtual nuUINT  INcSetUserCfg(nuUINT nMode = SET_USERCFG_NOEXTEND);

	virtual nuBOOL  INcSetTimer(nuPVOID pTimerFunc, nuDWORD nTime);
	virtual nuBOOL  INcSetTimerEx(nuDWORD nID, nuDWORD nElapse, nuPVOID lpTimerProc);
	virtual nuBOOL  INcKillTimerEx(nuDWORD nID);
	virtual nuUINT  INcGetNavigationData(PNC_NAVI_DATA pNcNaviData);

	//3D Demo
	virtual nuUINT  INcStart3DDemo(nuTCHAR *pts3DName);
	virtual nuUINT  INcClose3DDemo();
	virtual nuUINT  INcSetScale(nuDWORD nScaleIdx);
	//TMC Route
	virtual nuBOOL	INcTMCRoute(nuLONG lTmcRouteCount);//added by daniel 20110830

	virtual nuBOOL  INcSaveLastPosition();//added by daniel 20111111
	
	virtual nuBOOL  INcTOBERESETGYRO(); // Added by Damon 20110906
	virtual nuBOOL  INcTOBEGETVERSION(); // Added by Damon 20110906

	virtual nuUINT  INcStartOpenGL(SYSDIFF_SCREEN sysDiffScreen, const NURORECT* pRtScreen); //OpenGLES API

	virtual nuBOOL  INcOperateMapXY(nuLONG x, nuLONG y, nuUINT nMouseCode);
	virtual nuPVOID INcGetMapInfo(nuUINT nMapCode);
	virtual nuPVOID INcGetGPSData();

	virtual nuUINT  INcSetMemoData(nuPVOID lpVoid);	//CCD

	virtual nuUINT  INcSetMemoPtData(nuPVOID lpVoid); //轨迹
	virtual nuPVOID INcGetLaneInfo(); //车道数信息

	virtual nuLONG* INcGetScaleList(nuLONG *pScaleCount);
	virtual nuBOOL  INcSetNaviThread(nuBYTE nNaviState);
	virtual nuBOOL  INcShilftNaviThread(nuBYTE bOpenOrClose);	
	virtual nuUINT  INcGetSystemTime(PNURO_SYS_TIME pNuroTime);
	
	virtual nuBOOL  INcNuroSetMAPPT(nuBYTE nSaveType); //SET MAP PT FILE
	
	virtual nuBOOL  INcSetRTFullView(nuLONG x, nuLONG y, nuUINT nCode); //Added by xiaoqin @2012.05.17
	virtual nuBOOL  INcResetScreenSize(nuWORD nWidth, nuWORD nHeight);
	virtual nuBOOL  INcGPS_ReSet(PGPSCFG pGpsSetData);

	virtual nuBOOL  INcGetTripInfo(nuPVOID lpVoid); //add by chang 2012.7.3
	virtual nuBOOL  INcReSetTripInfo(nuPVOID lpVoid); //add by chang 2012.7.3

	virtual nuBOOL  INcConfigNavi(nuLONG nDis); //Added by xiaoqin for ReRoute @2012.07.20
	virtual nuVOID	INcUIPlaySound(nuWCHAR *wsTTS);//Added by daniel for SoundNavi 20121119
	
	virtual nuUINT  INcUI_TRIGGER_TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEvent);//Added by Daniel For TMC Event 20141030
	virtual nuBOOL  INcGetTMCPathData(nuPVOID pvoid,nuUINT* pbufcount);
	
	virtual nuUINT  INcUISendGPSData(const nuPVOID pOrGpsData1, const nuPVOID pOrGpsData2);//Added by daniel
	virtual nuBOOL  INcSet_TTS_Volume(const nuWORD TTSVolum );//Added by daniel
	virtual nuBOOL  INcSet_TTS_Speed(const nuWORD TTSSpeed );//Added by daniel
	virtual nuBOOL  INcSet_TTS_Pitch(const nuWORD TTSPitch );//Added by daniel
	virtual nuVOID  INcGetSPTInfo(SPT_DATA &SPTData);//Added by daniel
	virtual nuUINT  INcChoose_Car_Road(nuBYTE nChosenRoad);//Added by Daniel ================Choose Road============ 20150207//
	virtual nuUINT	INcReciveUIData(nuLONG DefNum, nuPVOID pUIData1, nuPVOID pUIData2);
	//----------------------------------For SDK----------------------------------//	
	virtual nuVOID	INcMouseEvent(NURO_POINT2 Pt);
	virtual nuLONG	INcUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData);
	virtual nuPVOID	INcUIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData);
	virtual nuVOID  INcSetEngineCallBack(nuINT iFuncID, nuPVOID pFunc);
	//----------------------------------For SDK----------------------------------//
	
};

#endif // !defined(AFX_INAVICONTROL_H__9970953C_BE6F_4517_B573_D7F64A623F57__INCLUDED_)
