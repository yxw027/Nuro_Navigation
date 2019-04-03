// LoadCallMgrZK.h: interface for the CLoadCallMgrZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADCALLMGRZK_H__1C791232_DAF2_4023_BE25_647FAEE0EEBD__INCLUDED_)
#define AFX_LOADCALLMGRZK_H__1C791232_DAF2_4023_BE25_647FAEE0EEBD__INCLUDED_

#include "NuroDefine.h"
#include "GApiCallMgr.h"

#ifdef USE_DYM_DLL
#define _USE_CALLMANAGER_DLL
#else
#include "libCallManager.h"
#endif

class CLoadCallMgrZK  
{
public:
    typedef nuPVOID(*GCM_InitModuleProc)(nuPVOID);
    typedef nuVOID(*GCM_FreeModuleProc)();

public:
	CLoadCallMgrZK();
	virtual ~CLoadCallMgrZK();

	nuBOOL  OnInitDialog( nuPVOID		pCallBack1,
			      nuPVOID		pCallBack2,
			      nuPVOID		pCallBack3,
			      nuPVOID		pCallBack4,
			      nuPVOID		pCallBack5);
    nuVOID  OnDestroyDialog();
	
    nuUINT  OnPaint();
    nuUINT  MouseDown(nuLONG x, nuLONG y);
    nuUINT  MouseMove(nuLONG x, nuLONG y);
    nuUINT  MouseUp(nuLONG x, nuLONG y);
    nuUINT  KeyDown(nuUINT nKey);
    nuUINT  KeyUp(nuUINT nKey);
    nuUINT  Message(NURO_UI_MESSAGE& uiMsge);
    nuUINT  ResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType);
    nuBOOL GPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len);
    nuBOOL SetHUDCallBackFunc(nuPVOID pFunc);
    nuBOOL SetDRCallBackFunc(nuPVOID pFunc);
    nuBOOL SetSOUNDCallBackFunc(nuPVOID pFunc);	
    nuBOOL SetNaviThreadCallBackFunc(nuPVOID pFunc);	
//---------------------------------------------_SDK-------------------------------//
    nuVOID  MouseEvent(NURO_POINT2 Pt);
    nuLONG  UISetEngine(nuLONG lDefnum, nuPVOID pData, nuPVOID pReturnData);
    nuPVOID UIGetEngineData(nuLONG lDefnum, nuPVOID pData, nuPVOID pReturnData);
    nuVOID  SetEngineCallBack(nuINT iFuncID, nuPVOID pFunc);
//---------------------------------------------_SDK-------------------------------//
protected:
    nuBOOL  LoadCallMgr();
    nuVOID  FreeCallMgr();
	
protected:
    nuHINSTANCE       m_hInst;
    CGApiCallMgr*   m_piCallMgr;
 
    NURO_SCREEN     m_screen;
    nuBOOL          m_bLoaden;
    nuBOOL          m_bFirstPaint;

public:
	nuUINT          m_nUIType;

};

#endif // !defined(AFX_LOADCALLMGRZK_H__1C791232_DAF2_4023_BE25_647FAEE0EEBD__INCLUDED_)

