#ifndef __NURO_API_CALL_MANAGER
#define __NURO_API_CALL_MANAGER

#include "NuroDefine.h"
#include "GApiNuroUi.h"
#include "NURO_DEF.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _NURO_FILE_NAME_SYS_MAIN_USER_CFG               1
#define _NURO_FILE_NAME_SYS_DLL_LOAD_CFG                2
#define _NURO_FILE_NAME_FOR_USER                        100

#define _USE_VIRTUAL_INTERFACE

class CGApiCallMgr
{
public:
    typedef struct tagCM_INIT_PARAMETER
    {
        NURO_SCREEN 	cmpScreen;
        nuTCHAR*    	ptzMainPath;
	nuUINT      	nUIType;
	nuPVOID		pCallBack1;
	nuPVOID		pCallBack2;
	nuPVOID		pCallBack3;
	nuPVOID		pCallBack4;
	nuPVOID		pCallBack5;
    }CM_INIT_PARAMETER, *PCM_INIT_PARAMETER;

    enum emMessageID
    {
        CM_MSG_ONPAINT = 1,
        CM_MSG_MOUSEDOWN,
        CM_MSG_MOUSEMOVE,
        CM_MSG_MOUSEUP,
        CM_MSG_KEYDOWN,
        CM_MSG_KEYUP
    };
public:
    CGApiCallMgr()
    {
    }
    virtual ~CGApiCallMgr()
    {
    }
public:
#ifdef _USE_VIRTUAL_INTERFACE
    virtual nuUINT  ICmOnInit(CM_INIT_PARAMETER* pcInitParam) = 0;
    virtual nuVOID  ICmOnFree() = 0;
    virtual nuUINT  ICmMessageProc(emMessageID MsgID, nuLONG xParam, nuLONG yParam, nuLONG nExtend) = 0;
	//
    virtual nuUINT  ICmOnPaint() = 0;

	virtual nuUINT  ICmMouseDown(nuLONG x, nuLONG y) = 0;
    virtual nuUINT  ICmMouseMove(nuLONG x, nuLONG y) = 0;
    virtual nuUINT  ICmMouseUp(nuLONG x, nuLONG y) = 0;
	
    virtual nuUINT  ICmKeyDown(nuUINT nKey) = 0;
    virtual nuUINT  ICmKeyUp(nuUINT nKey) = 0;
	virtual nuUINT  ICmMessage(NURO_UI_MESSAGE& uiMsge) = 0;
	virtual nuUINT	ICmSetCallBack(nuVOID* pfFunc, nuUINT nFuncMode) = 0;
	virtual nuUINT  ICmSetReciveStruct(nuPVOID pGet_Data) = 0;
	virtual nuUINT  ICmResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType) = 0;
	virtual nuUINT  ICmSetNaviThreadCallBackFunc(nuVOID* pfFunc) = 0;
	#ifdef ANDROID_GPS	
	virtual nuUINT  ICmGPS_JNI_RecvData(const nuCHAR *pBuff, nuINT len) = 0;
	#endif
	#ifdef USE_HUD_DR_CALLBACK	
	virtual nuUINT	ICmSetHUDCallBack(nuVOID* pfFunc) = 0;
	virtual nuUINT	ICmSetDRCallBack(nuVOID* pfFunc) = 0;
	#endif
	#ifdef	ANDROID
	virtual nuUINT	ICmSetSOUNDCallBack(nuVOID* pfFunc) = 0;
	#endif
	//----------------------------------For SDK----------------------------------//
	virtual nuVOID  ICmMouseEvent(NURO_POINT2 Pt) = 0;
	virtual nuLONG  ICmUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData) = 0;
	virtual nuPVOID  ICmUIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData) = 0;
	virtual nuVOID  ICmSetEngineCallBack(nuINT iFuncID, nuPVOID pFunc) = 0;
	//----------------------------------For SDK----------------------------------//
#endif
};

#endif
