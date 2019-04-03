// MainCall.h: interface for the CMainCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINCALL_H__F88A4452_D531_41D5_A23E_CF53F1567385__INCLUDED_)
#define AFX_MAINCALL_H__F88A4452_D531_41D5_A23E_CF53F1567385__INCLUDED_

#include "GApiCallMgr.h"
#include "GApiNuroUi.h"
#include "NuroModuleApiStruct.h"
#include "libNaviControl.h"
#include "../NuroApi/GApiNaviCtrl.h"
#include "NURO_DEF.h"

/*
typedef struct tagNURO_MAIN_INIT
{
    NURO_SCREEN     screen;
    nuTCHAR*        pTsPath;
}NURO_MAIN_INIT, *PNURO_MAIN_INIT;
*/

class CMainCall : public CGApiCallMgr 
{
public:
	CMainCall();
	virtual ~CMainCall();

	//---Virtual Interfaces---------------------------------------------------------------------------------
#ifdef _USE_VIRTUAL_INTERFACE
    virtual nuUINT ICmOnInit(CM_INIT_PARAMETER* pcInitParam)
    {
        //return CmOnInit(pcInitParam);
		return MainInit(pcInitParam);
    }
    virtual nuVOID ICmOnFree()
    {
        //CmOnFree();
		MainFree();
    }
    virtual nuUINT ICmMessageProc(emMessageID MsgID, nuLONG xParam, nuLONG yParam, nuLONG nExtend)
    {
        return 0;
    }
	virtual nuUINT ICmOnPaint()
    {
		return OnPaint();
    }
	virtual nuUINT ICmMouseDown(nuLONG x, nuLONG y)
	{
		return MouseDown(x, y);
	}
    virtual nuUINT ICmMouseMove(nuLONG x, nuLONG y)
	{
		return MouseMove(x, y);
	}
    virtual nuUINT ICmMouseUp(nuLONG x, nuLONG y)
	{
		return MouseUp(x, y);
	}
    virtual nuUINT ICmKeyDown(nuUINT nKey)
	{
		return KeyDown(nKey);
	}
    virtual nuUINT ICmKeyUp(nuUINT nKey)
	{
		return KeyUp(nKey);
	}
	virtual nuUINT  ICmMessage(NURO_UI_MESSAGE& uiMsge)
	{
		return Message(uiMsge);
	}
	virtual nuUINT ICmSetCallBack(nuVOID* pfFunc, nuUINT nFuncMode)
	{
		return SetCallBack(pfFunc, nFuncMode);
	}
	virtual nuUINT ICmSetReciveStruct(nuPVOID pGet_Data)
	{
		return SetReciveStruct(pGet_Data);
	}
	virtual nuUINT  ICmResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType)
	{
		return ResetScreen(nWidth, nHeight, nType);
	}
	virtual nuUINT ICmSetNaviThreadCallBackFunc(nuVOID *pfFunc)
	{
		return SetNaviThreadCallBackFunc(pfFunc);
	}
	virtual nuBOOL  ICmInitNaviCtrl(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, 
							  nuPVOID* ppUserCfg, NURO_API_GDI* pNcInitGdi, CGApiOpenResource* pApiRs, nuPVOID lpVoid)
	{
		return InitNaviCtrl(sysDiffScreen, nWidth, nHeight, 
							  ppUserCfg, pNcInitGdi, pApiRs, lpVoid);
	}
	#ifdef ANDROID_GPS 
	virtual nuUINT  ICmGPS_JNI_RecvData(const nuCHAR *pBuff, nuINT len)
	{
		return GPS_JNI_RecvData(pBuff,len);
	}
	#endif
	#ifdef USE_HUD_DR_CALLBACK 
	virtual nuUINT ICmSetHUDCallBack(nuVOID* pfFunc)
	{
		return SetHUDCallBack(pfFunc);
	}
	virtual nuUINT ICmSetDRCallBack(nuVOID* pfFunc)
	{
		return SetDRCallBack(pfFunc);
	}
	#endif
	#ifdef ANDROID
	virtual nuUINT ICmSetSOUNDCallBack(nuVOID* pfFunc)
	{
		return SetSOUNDCallBack(pfFunc);
	}
	#endif
//=================For SDK=================================================================//
	virtual nuVOID ICmMouseEvent(NURO_POINT2 Pt)
	{
		return MouseEvent(Pt);
	}
	virtual nuLONG ICmUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
	{
		return UISetEngine(DefNum, pUIData, pUIReturnData);
	}
	virtual nuPVOID  ICmUIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
	{
		return UIGetEngineData(DefNum, pUIData, pUIReturnData);
	}
	virtual nuVOID ICmSetEngineCallBack(nuINT iFuncID, nuPVOID pFunc)
	{
		SetEngineCallBack(iFuncID, pFunc);
	}
//=================For SDK=================================================================//
#endif
	//-------------------------------------------------------------------------------------------------------
 
	//----------------------------------For SDK----------------------------------//
	//nuBOOL MainInit(NURO_MAIN_INIT& mainInit);
	nuBOOL InitNaviCtrl(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, 
						  nuPVOID* ppUserCfg, NURO_API_GDI* pNcInitGdi, CGApiOpenResource* pApiRs, nuPVOID lpVoid);
	nuVOID  MouseEvent(NURO_POINT2 Pt);
	nuLONG  UISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData);
	nuPVOID UIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData);
	nuVOID  SetEngineCallBack(nuINT iFuncID, nuPVOID pFunc);
	//----------------------------------For SDK----------------------------------//
	nuPVOID LoadNC(nuTCHAR *ptsDll);
	nuBOOL MainInit(CM_INIT_PARAMETER* mainInit);
	nuVOID MainFree();
	
    nuUINT MouseDown(nuLONG x, nuLONG y);
    nuUINT MouseMove(nuLONG x, nuLONG y);
    nuUINT MouseUp(nuLONG x, nuLONG y);

    nuUINT KeyDown(nuUINT nKey);
    nuUINT KeyUp(nuUINT nKey); 

    nuUINT OnPaint();

    nuUINT Message(NURO_UI_MESSAGE& uiMsge);

	nuUINT SetCallBack(nuVOID* pfFunc, nuUINT nFuncMode);
	nuUINT SetReciveStruct(nuPVOID pGet_Data);
	nuUINT ResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType);
	nuUINT SetNaviThreadCallBackFunc(nuVOID *pfFunc);
	#ifdef USE_HUD_DR_CALLBACK
	nuUINT SetDRCallBack(nuVOID* pfFunc);
	nuUINT SetHUDCallBack(nuVOID* pfFunc);
	#endif
	#ifdef ANDROID
	nuUINT SetSOUNDCallBack(nuVOID* pfFunc);
	#endif
	#ifdef ANDROID_GPS
	nuUINT GPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len );
	#endif
protected:
    nuUINT  CmOnInit(CM_INIT_PARAMETER* pcInitParam);
    nuVOID  CmOnFree();
	
protected:
    class CLoadGdiZK*      m_pLdGdi;
	CGApiNaviCtrl*         m_pLNC;
    //class CLoadResouceZK*  m_pLdResource;
	class CLoadOpenResource*  m_pLdResource;
    nuTCHAR                m_tsPath[NURO_MAX_PATH];
	tagUSERCONFIG*         s_pUserCfg;
	NURO_API_GDI*          s_pGdi;
	APIFORUI               g_APIFORUI;
	CGApiNaviCtrl*	       s_pApiNc;
	CGApiOpenResource*	   pApiRes;
};

#endif // !defined(AFX_MAINCALL_H__F88A4452_D531_41D5_A23E_CF53F1567385__INCLUDED_)
