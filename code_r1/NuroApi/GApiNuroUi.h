#ifndef __GNURO_UI_API_20100602
#define __GNURO_UI_API_20100602

#include "NuroCommonApi.h"
#include "NuroOpenedDefine.h"

typedef struct tagNURO_UI_MODULE_INIT
{
    nuTCHAR         *pTsPath;
    PNURO_API_GDI   pGdiAddr;
    nuWORD          nScreenW;
    nuWORD          nScreenH;
	nuUINT          nUIType;
    nuVOID*         pApiResource;
    SYSDIFF_SCREEN  sysDiff;
}NURO_UI_MODULE_INIT, *PNURO_UI_MODULE_INIT;

typedef struct tagNURO_UI_MESSAGE
{
    nuUINT  nMsgID;
    nuDWORD nParam1;
    nuDWORD nParam2;
    nuPVOID pExtend;
}NURO_UI_MESSAGE, *PNURO_UI_MESSAGE;

class CGApiNuroUi
{
public:
    typedef nuUINT(*NsMouseProc)(nuLONG x, nuLONG y);
    typedef nuUINT(*NsKeyProc)(nuUINT nKey);
    typedef nuUINT(*NsOnPaintProc)();
    typedef nuUINT(*NsMessageProc)(NURO_UI_MESSAGE& uiMsg);
	typedef nuUINT(*NsSetCallBackProc)(nuVOID* pfFunc, nuUINT nFuncMode);
	typedef nuUINT(*NsSetComBufferProc)(nuPVOID pGet_ComBuffer, nuPVOID pSend_ComBuffer);
	typedef nuUINT(*NsSetReciveStructProc)(nuPVOID pGet_Data);
	typedef nuUINT(*NsResetScreenProc)(nuINT nWidth, nuINT nHeight, nuBYTE nType);
	typedef nuUINT(*NsSetNaviThreadCallBackFuncProc)(nuVOID *pfFunc);
	typedef nuUINT(*NsNaviThreadCallBackFuncProc)(PCALL_BACK_PARAM pParam);
	#ifdef USE_HUD_DR_CALLBACK
	typedef nuUINT(*NsSetHUDCallBackProc)(nuVOID* pfFunc);
	typedef nuUINT(*NsSetDRCallBackProc)(nuVOID* pfFunc);
	#endif
	#ifdef ANDROID
	typedef nuUINT(*NsSetSOUNDCallBackProc)(nuVOID* pfFunc);
	#endif
	#ifdef ANDROID_GPS
	typedef nuUINT(*GPS_JNI_RecvDataProc)(const nuCHAR *pBuff, nuINT len );
	#endif
public:
    CGApiNuroUi()
    {
    }
    virtual ~CGApiNuroUi()
    {
    }
public:
    NsMouseProc       fpMouseDown;
    NsMouseProc       fpMouseMove;
    NsMouseProc       fpMouseUp;
    NsKeyProc         fpKeyDown;
    NsKeyProc         fpKeyUp;
    NsOnPaintProc     fpOnPaint;
    NsMessageProc     fpMessage;
	NsSetCallBackProc fpSetCallBack;
	NsSetComBufferProc fpSetComBuffer;
	NsSetReciveStructProc fpSetReciveStruct;
	NsResetScreenProc  fpResetScreen;
	NsSetNaviThreadCallBackFuncProc fpSetNaviThreadCallBackFunc;
	NsNaviThreadCallBackFuncProc    fpNaviThreadCallBackFunc;
	#ifdef USE_HUD_DR_CALLBACK
	NsSetHUDCallBackProc fpSetHUDCallBack;
	NsSetHUDCallBackProc fpSetHUDCallBack2;
	NsSetDRCallBackProc fpSetDRCallBack;
	NsSetDRCallBackProc fpSetDRCallBack2;
	#endif
	#ifdef ANDROID
	NsSetSOUNDCallBackProc fpSetSOUNDCallBack;
	NsSetSOUNDCallBackProc fpSetSOUNDCallBack2;
	#endif
	#ifdef ANDROID_GPS
	GPS_JNI_RecvDataProc fpGPSRecvData;
	#endif

    SYSDIFF_SCREEN  sysDiff;
};


#endif
