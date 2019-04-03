// MainCall.cpp: implementation of the CMainCall class.
//
//////////////////////////////////////////////////////////////////////

#include "MainCall.h"
#include "NuroClasses.h"
#include "LoadGdiZK.h"
#include "NuroLoadDll.h"
#include "LoadOpenResource.h"
#include "GApiOpenResource.h"
#include <stdio.h>
#include <android/log.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMainCall::CMainCall()
{
    m_pLdGdi    = NULL;
    m_pLdResource   = NULL;
}

CMainCall::~CMainCall()
{
    MainFree();
}

nuBOOL CMainCall::MainInit(CM_INIT_PARAMETER* mainInit)
{
    nuTcscpy(m_tsPath, mainInit->ptzMainPath);
    nuTCHAR tsDllName[NURO_MAX_PATH];
    m_pLdGdi = new CLoadGdiZK();
    if( NULL == m_pLdGdi )
    {
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"new m_pLdGdi fail ");
        return nuFALSE;
    }
    nuTcscpy(tsDllName, m_tsPath);
    nuTcscat(tsDllName, nuTEXT("Setting\\"));
__android_log_print(ANDROID_LOG_INFO,  "navi" ,"new m_pLdGdi OK ");
    if( !m_pLdGdi->LoadGdi(tsDllName, &mainInit->cmpScreen, NULL) )
    {
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"load m_pLdGdi fail ");
        return nuFALSE;
    }
    __android_log_print(ANDROID_LOG_INFO,  "navi" ,"load LoadGdi OK ");
    /*m_pLdResource = new CLoadOpenResource();
    if( NULL == m_pLdResource )
    {
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"new CLoadOpenResource fail");
        return nuFALSE;
    }
    __android_log_print(ANDROID_LOG_INFO,  "navi" ,"new LoadOpenResource OK ");
    nuPVOID pApiRes = m_pLdResource->LoadOpenResource(tsDllName, NULL);
	if(NULL == pApiRes)
	{
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"load CLoadOpenResource fail");
	}
__android_log_print(ANDROID_LOG_INFO,  "navi" ,"load LoadOpenResource OK ");*/
	m_pLNC = s_pApiNc = (CGApiNaviCtrl*)LoadNC(tsDllName);
	__android_log_print(ANDROID_LOG_INFO,  "navi" ," LoadNC OK ");
	if( NULL == s_pApiNc )
	{
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"LoadNC fail");
		return nuFALSE;
	}

	if( !InitNaviCtrl(mainInit->cmpScreen.sysDiff, 
		          mainInit->cmpScreen.nScreenWidth, 
			  mainInit->cmpScreen.nScreenHeight, 
			  (nuPVOID*)(&s_pUserCfg), 
			  m_pLdGdi->m_pGdi, 
			  (CGApiOpenResource *)pApiRes,//CNrResourceBase::s_pApiResource,
			  (nuPVOID*)&g_APIFORUI))
	{
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"InitNaviCtrl fail");
		return nuFALSE;	
	}
	s_pApiNc->INcSetEngineCallBack(NURO_SET_UI_THREAD_CALLBACK, mainInit->pCallBack2);
	s_pApiNc->INcSetEngineCallBack(NURO_SET_NAVI_THREAD_CALLBACK, mainInit->pCallBack1);
	s_pApiNc->INcSetEngineCallBack(NURO_SET_TIME_THREAD_CALLBACK, mainInit->pCallBack3);
	s_pApiNc->INcSetEngineCallBack(NURO_SET_SOUND_THREAD_CALLBACK, mainInit->pCallBack4);
	s_pApiNc->INcSetEngineCallBack(NURO_SET_OTHER_THREAD_CALLBACK, mainInit->pCallBack5);
        __android_log_print(ANDROID_LOG_INFO,  "navi" ,"InitNaviCtrl OK");
    return nuTRUE;
}
nuPVOID CMainCall::LoadNC(nuTCHAR *ptsDll)
{
    return LibNC_InitModule(NULL);
}
nuBOOL CMainCall::InitNaviCtrl(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, 
							  nuPVOID* ppUserCfg, NURO_API_GDI* pNcInitGdi, CGApiOpenResource* pApiRes, nuPVOID lpVoid)
{
	nuBOOL bStartNaviCtrl = false;
	__android_log_print(ANDROID_LOG_INFO, "navi", "InitNaviCtrl.....");
	if(s_pApiNc != NULL )
	{
		if(!s_pApiNc->INcInit(sysDiffScreen, nWidth, nHeight, ppUserCfg, pNcInitGdi, pApiRes))
		{
	__android_log_print(ANDROID_LOG_INFO, "navi", "s_pApiNc->INcInit fail.....");
			return nuFALSE;
		}
	__android_log_print(ANDROID_LOG_INFO, "navi", "s_pApiNc->INcInit OK.....");
		bStartNaviCtrl = s_pApiNc->INcStartNaviCtrl(lpVoid);
	__android_log_print(ANDROID_LOG_INFO, "navi", "INcStartNaviCtrl %d .....", bStartNaviCtrl);
		return bStartNaviCtrl;
	}
	else
	{
		return nuFALSE;
	}
}

nuVOID CMainCall::MainFree()
{
    if( NULL != m_pLdResource )
    {
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        m_pLdResource->FreeOpenResource();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        delete m_pLdResource;
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        m_pLdResource = NULL;
    }
    if( NULL != m_pLdGdi )
    {
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        m_pLdGdi->FreeGdi();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        delete m_pLdGdi;
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO, "navi" ," %s", buf);
            }
        m_pLdGdi = NULL;
    }
}

nuUINT CMainCall::MouseDown(nuLONG x, nuLONG y)
{
    
    return 0;
}

nuUINT CMainCall::MouseMove(nuLONG x, nuLONG y)
{
    
    return 0;
}

nuUINT CMainCall::MouseUp(nuLONG x, nuLONG y)
{
   
    return 0;
}

nuUINT CMainCall::OnPaint()
{
   
    return 0;
}

nuUINT CMainCall::KeyDown(nuUINT nKey)
{
   
    return 0;
}

nuUINT CMainCall::KeyUp(nuUINT nKey)
{
   
    return 0;
}

nuUINT CMainCall::Message(NURO_UI_MESSAGE& uiMsge)
{
   
    return 0;
}

nuUINT CMainCall::SetCallBack(nuVOID* pfFunc, nuUINT nFuncMode)
{
   
    return 0;
}

nuUINT CMainCall::ResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType)
{
	return 0;
}
#ifdef USE_HUD_DR_CALLBACK 
nuUINT CMainCall::SetHUDCallBack(nuVOID* pfFunc)
{
	return 0;
}
nuUINT CMainCall::SetDRCallBack(nuVOID* pfFunc)
{
	return 0;
}
#endif
#ifdef ANDROID
nuUINT CMainCall::SetSOUNDCallBack(nuVOID* pfFunc)
{
	return 0;
}
#endif
#ifdef ANDROID_GPS

nuUINT CMainCall::GPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len )
{
	if( s_pApiNc )
	{
	    s_pApiNc->INcGPS_JNI_RecvData(pBuff, len);
		return 1;
	}
	return 0;
}
#endif
nuUINT CMainCall::SetNaviThreadCallBackFunc(nuVOID* pfFunc)
{
	return 0;//fpSetNaviThreadCallBackFunc(pfFunc);
}
nuUINT CMainCall::SetReciveStruct(nuPVOID pGet_Data)
{
    return 0;
}
//----------------------------------For SDK----------------------------------//
nuVOID CMainCall::MouseEvent(NURO_POINT2 Pt)
{
	s_pApiNc->INcMouseEvent(Pt);
}
nuLONG CMainCall::UISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
{
	return s_pApiNc->INcUISetEngine(DefNum, pUIData, pUIReturnData);
}
nuPVOID CMainCall::UIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
{
	return s_pApiNc->INcUIGetEngineData(DefNum, pUIData, pUIReturnData);
}
nuVOID CMainCall::SetEngineCallBack(nuINT iFuncID, nuPVOID pFunc)
{
	s_pApiNc->INcSetEngineCallBack(iFuncID, pFunc);
}
//----------------------------------For SDK----------------------------------//
