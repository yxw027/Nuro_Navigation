// LoadCallMgrZK.cpp: implementation of the CLoadCallMgrZK class.
//
//////////////////////////////////////////////////////////////////////

#include "NuroDefine.h"
#include "LoadCallMgrZK.h"
#include <stdio.h>
#include <android/log.h>
//////////////////////////////////////////////////////////////////////s
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadCallMgrZK::CLoadCallMgrZK()
{

	m_hInst                     = NULL;
    m_piCallMgr                 = NULL;
    m_screen.nScreenLeft        = 0;
    m_screen.nScreenTop         = 0;
    long width,height;
    if(!nuReadConfigValue("SCREENWIDTH",&width))
    {
        width = 800;
    }
    if(!nuReadConfigValue("SCREENHEIGHT",&height))
    {
         height = 480;
    }
    m_screen.nScreenWidth       = 800;
    m_screen.nScreenHeight      = 480;
	m_nUIType                   = 0;
#ifdef NURO_OS_WINDOWS
    m_screen.sysDiff.hScreenDC  = NULL;
    m_screen.sysDiff.hWnd       = NULL;
#endif
    m_screen.sysDiff.hScreenBuff = NULL;
    m_bLoaden                   = nuFALSE;
    m_bFirstPaint               = nuTRUE;
}

CLoadCallMgrZK::~CLoadCallMgrZK()
{

}

nuBOOL CLoadCallMgrZK::LoadCallMgr()
{
    m_piCallMgr = (CGApiCallMgr*)GCM_InitModule(NULL);
    if( NULL == m_piCallMgr )
    {
        return nuFALSE;
    }
    return nuTRUE;
}

nuVOID CLoadCallMgrZK::FreeCallMgr()
{
#ifdef _USE_CALLMANAGER_DLL
    if( NULL != m_hInst )
    {
        GCM_FreeModuleProc pfFreeModule = (GCM_FreeModuleProc)nuGetProcAddress(m_hInst, "GCM_FreeModule");
        if( NULL != pfFreeModule )
        {
            pfFreeModule();
        }
        nuFreeLibrary(m_hInst);
        m_piCallMgr = NULL;
        m_hInst = NULL;
    }
#else
    if( NULL != m_piCallMgr )
    {
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
        GCM_FreeModule();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
        m_piCallMgr = NULL;
    }
#endif
}

nuBOOL CLoadCallMgrZK::OnInitDialog(    nuPVOID		pCallBack1,
					nuPVOID		pCallBack2,
					nuPVOID		pCallBack3,
					nuPVOID		pCallBack4,
					nuPVOID		pCallBack5)
{
    if( !LoadCallMgr() )
    {
        return nuFALSE;
    }

    __android_log_print(ANDROID_LOG_INFO,"loadcallmgr","++++++++++++++++++++++oninitaildialg");

    nuTCHAR tsPath[256];
    nuGetModulePath(NULL, tsPath, 256);
    //LoadCallMgr
    CGApiCallMgr::CM_INIT_PARAMETER cmParam;
    cmParam.ptzMainPath = tsPath;
    cmParam.cmpScreen   = m_screen;
    cmParam.nUIType     = m_nUIType;
    cmParam.pCallBack1  = pCallBack1;
    cmParam.pCallBack2  = pCallBack2;
    cmParam.pCallBack3  = pCallBack3;
    cmParam.pCallBack4  = pCallBack4;
    cmParam.pCallBack5  = pCallBack5;
    if( !m_piCallMgr->ICmOnInit(&cmParam) )
    {
		return nuFALSE;
    }
    __android_log_print(ANDROID_LOG_INFO,"loadcallmgr","m_piCallMgr->ICmOnInit()");
    m_screen = cmParam.cmpScreen;
    m_bLoaden = nuTRUE;
    return nuTRUE;
}

nuVOID CLoadCallMgrZK::OnDestroyDialog()
{
    m_bLoaden = nuFALSE;
    if( NULL != m_piCallMgr )
    {
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
        m_piCallMgr->ICmOnFree();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
        m_piCallMgr = NULL;
    }
#ifdef NURO_OS_WINDOWS
    if( m_screen.sysDiff.hScreenDC )
    {
        //DeleteDC(m_screen.sysDiff.hScreenDC);
        m_screen.sysDiff.hScreenDC  = NULL;
        m_screen.sysDiff.hWnd       = NULL;
    }
#endif
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
    FreeCallMgr();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }

	
}

nuUINT CLoadCallMgrZK::OnPaint()
{
    if( m_bFirstPaint )
    {
        m_bFirstPaint = nuFALSE;
	}	
	if( NULL != m_piCallMgr )
	{
		m_piCallMgr->ICmOnPaint();
	}
    return 0;
}

nuUINT CLoadCallMgrZK::MouseDown(nuLONG x, nuLONG y)
{

	if( NULL != m_piCallMgr )
	{
		nuUINT rc = m_piCallMgr->ICmMouseDown(x, y);

        return rc;
	}
	return 0;
}

nuUINT CLoadCallMgrZK::MouseMove(nuLONG x, nuLONG y)
{
	if( NULL != m_piCallMgr )
	{ 
		return m_piCallMgr->ICmMouseMove(x, y);
	}
	return 0;
}

nuUINT CLoadCallMgrZK::MouseUp(nuLONG x, nuLONG y)
{
	if( NULL != m_piCallMgr )
	{
		return m_piCallMgr->ICmMouseUp(x, y);
	}
	return 0;
}

nuUINT CLoadCallMgrZK::KeyDown(nuUINT nKey)
{
	if( NULL != m_piCallMgr )
	{
		return m_piCallMgr->ICmKeyDown(nKey);
	}
	return 0;
}

nuUINT CLoadCallMgrZK::KeyUp(nuUINT nKey)
{
	if( NULL != m_piCallMgr )
	{
		return m_piCallMgr->ICmKeyUp(nKey);
	}
	return 0;
}

nuUINT  CLoadCallMgrZK::Message(NURO_UI_MESSAGE& uiMsge)
{
	if( NULL != m_piCallMgr )
	{
		return m_piCallMgr->ICmMessage(uiMsge);
	}
	return 0;
}

nuUINT CLoadCallMgrZK::ResetScreen(nuINT nWidth, nuINT nHeight, nuBYTE nType)
{
	if( NULL != m_piCallMgr )
	{
		return m_piCallMgr->ICmResetScreen(nWidth, nHeight, nType);
	}
	return 0;

}
//#include <stdio.h>
nuBOOL CLoadCallMgrZK::GPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len)
{

	if( NULL != m_piCallMgr && NULL != pBuff)
	{
		#ifdef ANDROID_GPS
		//return m_piCallMgr->ICmGPS_RecvData(pBuff, len);
		return m_piCallMgr->ICmGPS_JNI_RecvData(pBuff,len);
		#endif		
		////m_libNC.NC_GPS_JNI_RecvData(pBuff,len);
	}
	return 0;
}
//-----------For HUD_Test---------------------------//
#ifdef USE_HUD_DR_CALLBACK
nuBOOL CLoadCallMgrZK::SetHUDCallBackFunc(nuPVOID pFunc)
{
	/*FILE *pfile = NULL;
	pfile = fopen("/mnt/sdcard/SetHUDCallBackFunc.txt","a+");
		
	fclose(pfile);
	*/
	if(pFunc == NULL)
	{
	/*	FILE *pfile = NULL;
	pfile = fopen("/mnt/sdcard/CLoadCallMgrZKFAIL.txt","a+");
		
	fclose(pfile);
	*/	
	return false;
	}
	return m_piCallMgr->ICmSetHUDCallBack(pFunc);
}
nuBOOL CLoadCallMgrZK::SetDRCallBackFunc(nuPVOID pFunc)
{
	if(pFunc == NULL)
	{
		return false;
	}
    	return m_piCallMgr->ICmSetDRCallBack(pFunc);
}
#endif
nuBOOL CLoadCallMgrZK::SetSOUNDCallBackFunc(nuPVOID pFunc)
{
    __android_log_print(ANDROID_LOG_INFO,"SetSoundCallBackFunc","begin-----");
	if(pFunc == NULL)
	{
		return false;
	}

    __android_log_print(ANDROID_LOG_INFO,"SetSoundCallBackFunc","pFunc is not null-----");
    return m_piCallMgr->ICmSetSOUNDCallBack(pFunc);
}
nuBOOL CLoadCallMgrZK::SetNaviThreadCallBackFunc(nuPVOID pFunc)
{
	__android_log_print(ANDROID_LOG_INFO, "NaviThreadCallBack", "SetNaviThreadCallBackFunc set la ...................");
	if(pFunc == NULL)
	{
		return false;
	}
	
    	return m_piCallMgr->ICmSetNaviThreadCallBackFunc(pFunc);
}
//---------------------------------------------_SDK-------------------------------//
nuVOID CLoadCallMgrZK::MouseEvent(NURO_POINT2 Pt)
{
	m_piCallMgr->ICmMouseEvent(Pt);
}
nuLONG CLoadCallMgrZK::UISetEngine(nuLONG lDefnum, nuPVOID pData, nuPVOID pReturnData)
{
    	return m_piCallMgr->ICmUISetEngine(lDefnum, pData, pReturnData);
}
nuPVOID CLoadCallMgrZK::UIGetEngineData(nuLONG lDefnum, nuPVOID pData, nuPVOID pReturnData)
{
    	return m_piCallMgr->ICmUIGetEngineData(lDefnum, pData, pReturnData);
}
nuVOID  CLoadCallMgrZK::SetEngineCallBack(nuINT iFuncID, nuPVOID pFunc)
{
	m_piCallMgr->ICmSetEngineCallBack(iFuncID, pFunc);
}
//---------------------------------------------_SDK-------------------------------//
