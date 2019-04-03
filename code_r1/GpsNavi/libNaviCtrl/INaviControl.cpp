// INaviControl.cpp: implementation of the CINaviControl class.
//
//////////////////////////////////////////////////////////////////////

#include "INaviControl.h"
#include "NuroModuleApiStruct.h"
#include "NaviThread.h"
#include "TimerThread.h"
#include "SoundThread.h"
#include "DataThread.h"

#include "LoadRoute.h"
#include "LoadInNavi.h"
#include "LoadMemMgr.h"
#include "GdiApiTrans.h"
#include "LoadMathTool.h"
#include "LoadFileSys.h"
#include "LoadDrawMap.h"
#include "LoadGpsOpen.h"
#include "LoadSearch.h"
#include "LoadResources.h"
#include "GApiNaviCtrl.h"
#include "NURO_DEF.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*global objects in NaviCtrl*/
//maybe used in other cpp
NAVICTRL					g_navictrlApi;
MEMMGRAPI					g_memMgrApi;
MEMMGRAPI_SH				g_memMgr_SHApi;
FILESYSAPI					g_fileSysApi;
ROUTEAPI					g_routeApi;
INNAVIAPI					g_innaviApi;
MATHTOOLAPI					g_mathtoolApi;
DRAWINFONAVIAPI				g_drawIfnApi;
DRAWMAPAPI					g_drawmapApi;
GPSOPENAPI					g_gpsopenApi;
SEARCHAPI					g_searchApi;
MAPDATAAPI					g_mapdataApi;
//----------------------------------SDK----------------------------------//
OPENRSAPI					g_OpenResourceApi;
GDIAPI                      			g_gdiApi;
NURO_API_GDI                			g_newgdiApi; 
//----------------------------------SDK----------------------------------//  

//----------------------------------SDK----------------------------------// 
class CLoadResources*			g_pLoadResources = NULL;
class CGdiApiTrans*         		g_pGdiApiTrans = NULL;	
//----------------------------------SDK----------------------------------// 
class CLoadDrawMap*			g_pLoadDrawMap = NULL;
class CLoadFileSys*			g_pLoadFileSys = NULL;
class CLoadMathTool*		g_pLoadMathTool = NULL;
class CLoadMemMgr*			g_pLoadMemMgr = NULL;
class CLoadRoute*			g_pLoadRoute = NULL;
class CLoadInNavi*			g_pLoadInNavi = NULL;
class CLoadGpsOpen*			g_pLoadGpsOpen = NULL;
class CLoadSearch*			g_pLoadSearch = NULL;
class CNaviThread*			g_pNaviThread = NULL;
class CTimerThread*			g_pTimerThread = NULL;
class CSoundThread*			g_pSoundThread = NULL;
class CDataThread*			g_pDataThread  = NULL;

void SetApi()
{
	g_navictrlApi.IFN_SetStateArray		= g_drawIfnApi.IFN_SetStateArray;
	g_navictrlApi.GPS_ReSet				= g_gpsopenApi.GPS_ReSet;
}

CINaviControl::CINaviControl()
{

}

CINaviControl::~CINaviControl()
{
	INcFree();
}

nuUINT CINaviControl::INcInit(SYSDIFF_SCREEN sysDiffScreen, nuWORD nWidth, nuWORD nHeight, nuPVOID* ppUserCfg, NURO_API_GDI* pNcInit, CGApiOpenResource* pApiRs)
{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC Initial .....");
	g_pLoadDrawMap  = new CLoadDrawMap;
	g_pLoadFileSys	= new CLoadFileSys;
	g_pLoadMathTool = new CLoadMathTool;
	g_pGdiApiTrans  = new CGdiApiTrans;
	g_pLoadMemMgr	= new CLoadMemMgr;
	g_pLoadRoute	= new CLoadRoute;
	g_pLoadInNavi	= new CLoadInNavi;
	g_pLoadGpsOpen	= new CLoadGpsOpen;
	g_pLoadSearch	= new CLoadSearch;
	g_pLoadResources = new CLoadResources;
	if( NULL == g_pLoadDrawMap ||
		NULL == g_pLoadFileSys ||
		NULL == g_pLoadMathTool ||
		NULL == g_pGdiApiTrans || 
		NULL == g_pLoadMemMgr ||
		NULL == g_pLoadRoute ||
		NULL == g_pLoadInNavi ||
		NULL == g_pLoadGpsOpen ||
		NULL == g_pLoadSearch ||
		NULL == g_pLoadResources)
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC new libs fail.....");
		return nuFALSE;
	}
	////////////////////////////////////////////
	g_pNaviThread = new CNaviThread();
	if( g_pNaviThread == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC new g_pNaviThread fail.....");	
		return nuFALSE;
	}
	g_pTimerThread = new CTimerThread();
	if( g_pTimerThread == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC new g_pTimerThread fail.....");
		return nuFALSE;
	}
	g_pSoundThread = new CSoundThread();
	if( g_pSoundThread == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC new g_pSoundThread fail.....");
		return nuFALSE;
	}
	g_pDataThread = new CDataThread();
	if(g_pDataThread == NULL)
	{
		return nuFALSE;
	}

	APISTRUCTADDR apiAddr = {NULL};
	apiAddr.pMmApi		= &g_memMgrApi;
	apiAddr.pGdiApi		= &g_gdiApi;
	apiAddr.pFsApi		= &g_fileSysApi;
	apiAddr.pRtApi		= &g_routeApi;
	apiAddr.pInApi		= &g_innaviApi;
	apiAddr.pMtApi		= &g_mathtoolApi;
	apiAddr.pIfnApi		= &g_drawIfnApi;
	apiAddr.pDmApi		= &g_drawmapApi;
	apiAddr.pGpsApi		= &g_gpsopenApi;
	apiAddr.pShApi		= &g_searchApi;
	apiAddr.pMdApi		= &g_mapdataApi;
	apiAddr.pMm_SHApi   = &g_memMgr_SHApi;
	apiAddr.pRsApi		= &g_OpenResourceApi;

	if( !g_pLoadMemMgr->InitModule(&g_memMgrApi,&g_memMgr_SHApi) )
	{
		return nuFALSE;
	}

	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "CINaviControl::INcInit : , %p %p",g_memMgr_SHApi,g_memMgrApi);
	
	if( pNcInit != NULL )
	{
		g_newgdiApi = *pNcInit;	
	}


	if( !g_pGdiApiTrans->nuGdiInit(&g_newgdiApi, &g_gdiApi) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pGdiApiTrans fail.....");
		return nuFALSE;
	}
	#ifdef ANDROID
	nuTCHAR ptsFontPath[512] = {0};
	if(0 == nuGetModulePath(NULL, ptsFontPath, 512))
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init GetModulePath fail.....");
		return nuFALSE;
	}
	nuTcscat(ptsFontPath,nuTEXT("Media/nuroui/uifont/nuro.nfd"));
	if(!g_newgdiApi.GDI_LoadFontInfo(ptsFontPath))
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init LoadFont fail...%s",ptsFontPath);
		return nuFALSE;
	}
	#endif

	if(!g_pLoadResources->InitModule(&g_OpenResourceApi))
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC g_pLoadResources fail...");
		return nuFALSE;
	}
	if( !g_pLoadFileSys->InitModule(&g_memMgrApi, &g_gdiApi, &g_fileSysApi) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadFileSys fail.....");
		return nuFALSE;
	}

	if( !g_pLoadMathTool->InitModule(&g_memMgrApi, &g_gdiApi, &g_fileSysApi, &g_mathtoolApi) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadMathTool fail.....");
		return nuFALSE;
	}
	if( !g_pLoadDrawMap->InitModule(&apiAddr, &g_OpenResourceApi) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadDrawMap fail.....");
		return nuFALSE;
	}
	if( !g_pLoadInNavi->InitModule(&apiAddr) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadInNavi fail.....");
		return nuFALSE;
	}

	if( !g_pLoadRoute->InitModule(&g_memMgrApi, &g_fileSysApi, &g_routeApi) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadRoute fail.....");
		return nuFALSE;
	}

	if( !g_pLoadGpsOpen->LoadModule(&apiAddr) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadGpsOpen fail.....");
		return nuFALSE;
	}
	if( !g_pLoadSearch->LoadModule(&apiAddr) )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC init g_pLoadSearch fail.....");
		return nuFALSE;
	}
	if( g_fileSysApi.pGdata == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "libNC g_fileSysApi.pGdata == null .....");
		return nuFALSE;
	}

	if( nWidth != 0 )
	{
		g_fileSysApi.pGdata->uiSetData.nScreenWidth	= nWidth;
	}
	if( nHeight != 0 )
	{
		g_fileSysApi.pGdata->uiSetData.nScreenHeight = nHeight;
	}

	//add by chang 2012.11.30;
	//init volume before soundthread;

	nuLONG lVenderType = 0;
	if( !nuReadConfigValue("VENDERTYPE", &lVenderType) )
	{
		lVenderType = 0;
	}

	if(_VENDER_ELEAD != lVenderType)
	{
		nuSetVolume(g_fileSysApi.pUserCfg->nSoundVolume);
	}
	
	if( !g_pTimerThread->InitTimerThread(g_pNaviThread) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC InitTimerThread fail .....");
		return nuFALSE;
	}

	if( !g_pSoundThread->InitSoundThread() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC InitSoundThread fail .....");
		return nuFALSE;
	}
	if( !g_pNaviThread->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC InitSoundThread fail .....");
		return nuFALSE;
	}
	if(!g_pDataThread->InitDataThread(g_pNaviThread))
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC InitDataThread fail .....");
		return nuFALSE;
	}
	
	*ppUserCfg = g_fileSysApi.pUserCfg;

	//SetApi();
	g_fileSysApi.pGdata->NC_SoundPlay = INcSoundPlay1;
	g_fileSysApi.pGdata->NC_Navi_SoundPlay = INcNavi_SoundPlay;
	g_fileSysApi.pGdata->NC_Navi_CleanSound = INcNavi_CleanSound;
	g_fileSysApi.pGdata->NC_StopSoundPlaying= INcStopSoundPlaying;
	g_fileSysApi.pGdata->NC_GetNaviState    = INcGetNaviState;
	g_fileSysApi.pGdata->NC_SetNaviState	= INcSetNaviState;
	g_fileSysApi.pGdata->NC_NT_Exit			= INcNuro_NT_Exit;
	g_navictrlApi.NC_SoundPlay			= INcSoundPlay1;
	g_navictrlApi.NC_Navi_SoundPlay		= INcNavi_SoundPlay;
	g_navictrlApi.NC_Navi_CleanSound    = INcNavi_CleanSound;
	g_navictrlApi.GPS_ReSet				= g_gpsopenApi.GPS_ReSet;
	g_navictrlApi.NC_StopSoundPlaying   = INcStopSoundPlaying;
	g_navictrlApi.NC_SetNaviParser      = INcSetNaviParser;
	g_navictrlApi.NC_GetNaviState		= INcGetNaviState;
	g_navictrlApi.NC_SetNaviState		= INcSetNaviState;
	g_navictrlApi.NC_NT_Exit			= INcNuro_NT_Exit;
	__android_log_print(ANDROID_LOG_INFO, "navi", "libNC Initial OK .....");
	return nuTRUE;
}

nuVOID CINaviControl::INcFree()
{
	//*****test*****
	//	LibNC_CloseTrace(TRACE_CLOSE_STATE_SAVE);
	//*****test*****
	//	LibNC_ShilftNaviThread(nuTRUE)
	if( g_pNaviThread != NULL )
	{
		g_pNaviThread->FreeThread();
		delete g_pNaviThread;
		g_pNaviThread = NULL;
	}
	if( g_pTimerThread != NULL )
	{
		g_pTimerThread->FreeTimerThread();
		delete g_pTimerThread;
		g_pTimerThread = NULL;
	}
	if( g_pSoundThread != NULL )
	{
		g_pSoundThread->FreeSoundThread();
		delete g_pSoundThread;
		g_pSoundThread	= NULL;
	}
	g_fileSysApi.pGdata = NULL;
	if( NULL != g_pLoadRoute )
	{
		g_pLoadRoute->UnLoadModule();
		delete g_pLoadRoute;
		g_pLoadRoute = NULL;
	}
	if( NULL != g_pLoadInNavi )
	{
		g_pLoadInNavi->UnLoadModule();
		delete g_pLoadInNavi;
		g_pLoadInNavi = NULL;
	}
	if( NULL != g_pLoadGpsOpen )
	{
		g_pLoadGpsOpen->UnLoadModule();
		delete g_pLoadGpsOpen;
		g_pLoadGpsOpen = NULL;
	}
	if( NULL != g_pLoadSearch )
	{
		g_pLoadSearch->UnLoadModule();
		delete g_pLoadSearch;
		g_pLoadSearch = NULL;
	}
	if( NULL != g_pLoadDrawMap )
	{
		g_pLoadDrawMap->UnLoadModule();
		delete g_pLoadDrawMap;
		g_pLoadDrawMap = NULL;
	}
	if( NULL != g_pLoadFileSys )
	{
		g_pLoadFileSys->UnLoadModule();
		delete g_pLoadFileSys;
		g_pLoadFileSys = NULL;
	}
	if( NULL != g_pLoadMathTool )
	{
		g_pLoadMathTool->UnLoadModule();
		delete g_pLoadMathTool;
		g_pLoadMathTool = NULL;
	}
	if( NULL != g_pLoadMemMgr )
	{
		g_pLoadMemMgr->UnLoadModule();
		delete g_pLoadMemMgr;
		g_pLoadMemMgr = NULL;
	}
	//
	if( NULL != g_pGdiApiTrans )
	{
		g_pGdiApiTrans->nuGdiFree();
		delete g_pGdiApiTrans;
		g_pGdiApiTrans = NULL;
	}
	if(NULL != g_pLoadResources)
	{
		delete g_pLoadResources;
		g_pLoadResources = NULL;
	}
}

nuBOOL CINaviControl::INcStartNaviCtrl(nuPVOID lpVoid)
{
	if( g_pNaviThread == NULL || !g_pNaviThread->InitThread() )
	{
		return nuFALSE;
	}
	if( lpVoid != NULL )
	{
		PAPIFORUI pApiForUI = (PAPIFORUI)lpVoid;
		pApiForUI->memApi		= g_memMgrApi;
		pApiForUI->gidApi		= g_gdiApi;
		pApiForUI->searchApi	= g_searchApi;
		pApiForUI->naviCtrl		= g_navictrlApi;
		pApiForUI->routeApi     = g_routeApi;		
		pApiForUI->filenameApi.FS_GetFilePath		= g_fileSysApi.FS_GetFilePath;
		pApiForUI->filenameApi.FS_GetPoiName		= g_fileSysApi.FS_GetPoiName;
		pApiForUI->filenameApi.FS_GetRoadName		= g_fileSysApi.FS_GetRoadName;
		pApiForUI->filenameApi.FS_ReadConstWcs		= g_fileSysApi.FS_ReadConstWcs;
		pApiForUI->filenameApi.FS_ReadPaData		= g_fileSysApi.FS_ReadPaData;
		pApiForUI->filenameApi.FS_ReadWcsFromFile	= g_fileSysApi.FS_ReadWcsFromFile;
		pApiForUI->filenameApi.FS_MapIDIndex		= g_fileSysApi.FS_MapIDIndex;
		pApiForUI->filenameApi.FS_SavePtFile		= g_fileSysApi.FS_SavePtFile;
		pApiForUI->filenameApi.FS_SavePtFileCoverForDFT = g_fileSysApi.FS_SavePtFileCoverForDFT;
		pApiForUI->filenameApi.FS_GetPtDataAddr		= g_fileSysApi.FS_GetPtDataAddr;
		pApiForUI->filenameApi.FS_ReadCityTownName	= g_fileSysApi.FS_ReadCityTownName;
		pApiForUI->filenameApi.FS_PtInBoundary		= g_fileSysApi.FS_PtInBoundary;
		pApiForUI->filenameApi.FS_GetMapCount		= g_fileSysApi.FS_GetMapCount;
		pApiForUI->filenameApi.FS_GetMpnData		= g_fileSysApi.FS_GetMpnData;
		pApiForUI->filenameApi.FS_Get_TMC_VD_SIZE   = g_fileSysApi.FS_Get_TMC_VD_SIZE;
		pApiForUI->filenameApi.FS_Get_TMC_FM_XML_SIZE = g_fileSysApi.FS_Get_TMC_FM_XML_SIZE;
		pApiForUI->filenameApi.FS_Get_TMC_VD        = g_fileSysApi.FS_Get_TMC_VD;
		pApiForUI->filenameApi.FS_Get_TMC_FM_XML    =  g_fileSysApi.FS_Get_TMC_FM_XML;
		pApiForUI->filenameApi.FS_Get_TMC_FM   =   g_fileSysApi.FS_Get_TMC_FM;
		pApiForUI->filenameApi.FS_Get_TMC_XML_L    =  g_fileSysApi.FS_Get_TMC_XML_L;
        pApiForUI->filenameApi.FS_Get_TMC_XML       =  g_fileSysApi.FS_Get_TMC_XML; //prosper 06.16
		pApiForUI->filenameApi.FS_Set_TMC_Byte      =  g_fileSysApi.FS_Set_TMC_Byte;
		pApiForUI->filenameApi.FS_Get_TMC_Router    = g_fileSysApi.FS_Get_TMC_Router;
		//
		pApiForUI->mathtoolApi  = g_mathtoolApi;
		pApiForUI->drawmapApi   = g_drawmapApi;
		pApiForUI->filesysApi   = g_fileSysApi;
		pApiForUI->innaviApi    = g_innaviApi;
	}
	return nuTRUE;
}

nuUINT CINaviControl::INcSetCallBack(SaveMapBmpProc Func)
{
	if( Func == NULL )
	{
		return 0;
	}
	g_pNaviThread->m_pfSaveMapBmp = Func;
	return 1;
}

nuUINT CINaviControl::INcSetCallBackData(DrawMapIconProc Func)
{
	if( Func == NULL )
	{
		return 0;
	}
	g_pNaviThread->m_jumpMove.m_pfSaveDrawData = Func;
	return 1;
}

nuUINT CINaviControl::INcSet_POI_CallBack(nuPVOID pFunc)
{
	if( pFunc == NULL )
	{
		return 0;
	}
	if( g_fileSysApi.pGdata )
	{
		g_fileSysApi.pGdata->pfCallBack = (CallBackFuncProc)pFunc;
	}

	return 1;
}

nuUINT CINaviControl::INcSetNaviThreadCallBack(nuPVOID pFunc)
{
	if( pFunc == NULL )
	{
		return 0;
	}
	if( g_fileSysApi.pGdata )
	{
		g_fileSysApi.pGdata->pfNaviThreadCallBack = (CallBackFuncProc)pFunc;
		CALL_BACK_PARAM Param = {0};
		Param.nCallType = CALLBACK_TYPE_TMCUSED;
		Param.pVOID = (nuVOID*)&g_fileSysApi.pGdata->drawInfoMap.nTMCUsed;
		g_fileSysApi.pGdata->pfNaviThreadCallBack(&Param);
	}

	return 1;
}

nuUINT CINaviControl::INcDrawAnimation(DrawAnimationProc Func)
{
	if( Func == NULL )
	{
		return 0;
	}
	g_pTimerThread->m_pfDrawAni = Func;
	return 1;
}

nuBOOL CINaviControl::INcSoundPlay( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								    nuWORD nRoadNameNum, nuWCHAR *pRoadName )
{
	if( nVoiceCode == VOICE_CODE_DEFAULT_SYN || nVoiceCode == VOICE_CODE_DEFAULT_ASY)
	{
		return g_pSoundThread->NewSoundPlay(VOICE_TYPE_DEFAULT, 
			0, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_DEFAULT_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_ROAD_SYN || nVoiceCode == VOICE_CODE_ROAD_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_ROAD, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_ROAD_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_OTHERS_SYN || nVoiceCode == VOICE_CODE_OTHERS_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_OTHERS, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_OTHERS_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_UI_DFT_SYN || nVoiceCode == VOICE_CODE_UI_DFT_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_UI_DEF,
			0,
			nSound1,
			nSound2,
			nSound3,
			nRoadNameNum,
			pRoadName,
			(nuBYTE)nVoiceCode - VOICE_CODE_UI_DFT_SYN );
	}
	else if(VOICE_CODE_REVISE_VOLUME == nVoiceCode ||
		    VOICE_CODE_REVISE_SPEED == nVoiceCode ||
			VOICE_CODE_REVISE_PITCH == nVoiceCode)
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_UI_DEF,
			REVISED_VOICE,
			nSound1,
			nSound2,
			nSound3,
			nRoadNameNum,
			pRoadName,
			nVoiceCode );
	}
	return nuFALSE;
}

nuBOOL CINaviControl::INcSoundPlay1( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								     nuWORD nRoadNameNum, nuWCHAR *pRoadName )
{
	if( nVoiceCode == VOICE_CODE_DEFAULT_SYN || nVoiceCode == VOICE_CODE_DEFAULT_ASY)
	{
		return g_pSoundThread->NewSoundPlay(VOICE_TYPE_DEFAULT, 
			0, 
			nSound1, 
			nSound2, 
			nSound3,






			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_DEFAULT_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_ROAD_SYN || nVoiceCode == VOICE_CODE_ROAD_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_ROAD, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_ROAD_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_OTHERS_SYN || nVoiceCode == VOICE_CODE_OTHERS_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_OTHERS, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_OTHERS_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_UI_DFT_SYN || nVoiceCode == VOICE_CODE_UI_DFT_ASY )
	{
		return g_pSoundThread->NewSoundPlay( VOICE_TYPE_UI_DEF,
			0,
			nSound1,
			nSound2,
			nSound3,
			nRoadNameNum,
			pRoadName,
			(nuBYTE)nVoiceCode - VOICE_CODE_UI_DFT_SYN );



	}
	else if(VOICE_CODE_REVISE_VOLUME == nVoiceCode ||
			VOICE_CODE_REVISE_SPEED == nVoiceCode ||
			VOICE_CODE_REVISE_PITCH == nVoiceCode )
	{
		return g_pSoundThread->NewSoundPlay( nVoiceCode,
			0,
			nSound1,
			nSound2,
			nSound3,
			nRoadNameNum,
			pRoadName,
			nVoiceCode );
	}
	return nuFALSE;
}
nuBOOL CINaviControl::INcSet_TTS_Volume(const nuWORD TTSVolum )
{
	return INcSoundPlay(VOICE_CODE_REVISE_VOLUME, TTSVolum, 0, 0, NULL, NULL);
}
nuBOOL CINaviControl::INcSet_TTS_Speed(const nuWORD TTSSpeed )
{
	return INcSoundPlay(VOICE_CODE_REVISE_SPEED, 0, TTSSpeed, 0, NULL, NULL);
}
nuBOOL CINaviControl::INcSet_TTS_Pitch(const nuWORD TTSPitch )
{
	return INcSoundPlay(VOICE_CODE_REVISE_PITCH, 0, 0, TTSPitch, NULL, NULL);
}
nuUINT CINaviControl::INcGetDrawData(NC_DRAR_DATA &drawData)
{
	if( NULL == g_pNaviThread )
	{
		return 0;
	}
	g_pNaviThread->m_DrawDataForUI.nddPoiPageIndex = drawData.nddPoiPageIndex;
	drawData = g_pNaviThread->m_DrawDataForUI;

	//Save PoiInfo
	if( g_fileSysApi.pGdata )
	{
		drawData.nddNowCenterPoiCount = g_fileSysApi.pGdata->drawInfoMap.nNowCenterPoiCount;
	}
	if( drawData.nddNowCenterPoiCount > 0 && drawData.nddPoiPageIndex < POI_CENTER_MAX_NUM)
	{
		SEEKEDPOIDATA TmpPoiNode;
		nuMemset(&TmpPoiNode, 0, sizeof(TmpPoiNode));
		TmpPoiNode = g_fileSysApi.pGdata->drawInfoMap.poiCenterNearList[drawData.nddPoiPageIndex];
		nuWORD nMapCount = g_fileSysApi.FS_GetMapCount();
// 		if(nMapCount > 1)
// 		{
// 			PMAPNAMEINFO pTmpMPN = (PMAPNAMEINFO)g_fileSysApi.FS_GetMpnData();
// 			drawData.nddMapIndex = pTmpMPN[g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nDwIdx].nMapID;
// 		}
// 		else
// 		{
			drawData.nddMapIndex = TmpPoiNode.nMapIdx;
		//}
		drawData.nddPoiNameAddr = TmpPoiNode.nNameAddr;
		drawData.nddOtherInfoAddr = TmpPoiNode.OnterInfoAddr;
	}
	//
	if( g_fileSysApi.pGdata )
	{
		drawData.nddBmpLTx = g_fileSysApi.pGdata->transfData.nBmpLTx;
		drawData.nddBmpLTy = g_fileSysApi.pGdata->transfData.nBmpLTy;
	}
	return 1;
}
nuUINT CINaviControl::INcGetDrawDataType(NC_DRAR_DATA &drawData)
{
	if( NULL == g_pNaviThread )
	{
		return 0;
	}
	nuMemcpy(drawData.nNowCenterPoiTYPECount,g_fileSysApi.pGdata->drawInfoMap.nNowCenterPoiTYPECount,sizeof(g_fileSysApi.pGdata->drawInfoMap.nNowCenterPoiTYPECount)); 
	for(nuINT i=0;i<4;i++)
	{
		for(nuINT j=0;j<4;j++)
		{
			drawData.nddPoiTypeNameAddr[i][j]=g_fileSysApi.pGdata->drawInfoMap.poiCenterNearTYPEList[i][j].nNameAddr;
			drawData.nddListPoiNode[i][j].nDistance=g_fileSysApi.pGdata->drawInfoMap.poiCenterNearTYPEList[i][j].nDis;
			drawData.nddListPoiNode[i][j].point=g_fileSysApi.pGdata->drawInfoMap.poiCenterNearTYPEList[i][j].pCoor;
			drawData.nddListPoiNode[i][j].nIconType = g_fileSysApi.pGdata->drawInfoMap.poiCenterNearTYPEList[i][j].nPOIType1*100+g_fileSysApi.pGdata->drawInfoMap.poiCenterNearTYPEList[i][j].nPOIType2;
		}
	}	
	
	return 1;
}

nuBOOL CINaviControl::INcJumpToScreenXY(nuLONG x, nuLONG y, nuBOOL bMode /* = nuFALSE */) //点击移图
{
	return g_pNaviThread->NuroJumpToScreenXY(x, y, bMode);
}

nuUINT CINaviControl::INcMoveMap(nuBYTE nMoveType, nuBYTE nMoveStep, nuBOOL bStartOrStop) //连续移图
{
	return g_pNaviThread->NuroMoveMap1(nMoveType, nMoveStep, bStartOrStop);
}

nuUINT CINaviControl::INcButtonEvent(nuUINT nCode, nuBYTE nButState)
{
	return g_pNaviThread->NuroButtonEvent(nCode, nButState);
}

nuBOOL CINaviControl::INcSetMarketPt(nuPVOID lpData) //设置导航点
{
	return g_pNaviThread->NuroSetMarket(lpData);
}

nuBOOL CINaviControl::INcDelMarketPt(nuPVOID lpVoid)
{
	return g_pNaviThread->NuroDelMarket(lpVoid);
}

nuBOOL CINaviControl::INcNavigation(nuBYTE nType) //模拟导航
{
	return g_pNaviThread->NuroNavigation(nType);
}

nuBOOL CINaviControl::INcIsNavigation()
{
	if( g_fileSysApi.pGdata != NULL )
	{
		return g_fileSysApi.pGdata->uiSetData.bNavigation;
	}
	else
	{
		return nuFALSE;
	}
}

nuUINT CINaviControl::INcSetUserCfg(nuUINT nMode)
{
	return g_pNaviThread->NuroSetUserCfg(nMode);
}
//
nuBOOL CINaviControl::INcSetTimer(nuPVOID pTimerFunc, nuDWORD nTime)
{
	if( g_pTimerThread == NULL )
	{
		return nuFALSE;
	}
	else
	{
		return g_pTimerThread->NuroSetTimer(pTimerFunc, nTime);
	}
}
nuBOOL CINaviControl::INcSetTimerEx(nuDWORD nID, nuDWORD nElapse, nuPVOID lpTimerProc)
{
	if( g_pTimerThread == NULL )
	{
		return nuFALSE;
	}
	else
	{
		return g_pTimerThread->NuroSetTimerEx(nID, nElapse, lpTimerProc);
	}
}
nuBOOL CINaviControl::INcKillTimerEx(nuDWORD nID)
{
	if( g_pTimerThread == NULL )
	{
		return nuFALSE;
	}
	else
	{
		return g_pTimerThread->NuroKillTimerEx(nID);
	}
}

nuUINT CINaviControl::INcGetNavigationData(PNC_NAVI_DATA pNcNaviData)
{
	if( pNcNaviData )
	{
		pNcNaviData->nErrorCode	= g_fileSysApi.pGdata->uiSetData.nLastError;
		pNcNaviData->bIsRouting	= g_fileSysApi.pGdata->timerData.bIsRouting;
		pNcNaviData->bRouted	= g_fileSysApi.pGdata->uiSetData.bNavigation;
        pNcNaviData->bUINeedWaiting = g_pNaviThread->m_drawToUI.bUIWaiting;
		return 1;
	}
	else
	{
		return 0;
	}
}

nuUINT CINaviControl::INcStart3DDemo(nuTCHAR *pts3DName)
{
	return g_pNaviThread->NuroStart3DDemo(pts3DName);
}
nuUINT CINaviControl::INcClose3DDemo()
{
	return g_pNaviThread->NuroClose3DDemo();
}
nuUINT CINaviControl::INcSetScale(nuDWORD nScaleIdx)
{
	return g_pNaviThread->NuroSetScale(nScaleIdx);
}
nuUINT CINaviControl::INcStartOpenGL(SYSDIFF_SCREEN sysDiffScreen, const NURORECT* pRtScreen)
{
	return g_drawmapApi.DM_StartOpenGL(sysDiffScreen, pRtScreen);
}

nuBOOL CINaviControl::INcOperateMapXY(nuLONG x, nuLONG y, nuUINT nMouseCode)
{
	//return g_pNaviThread->NuroMoveToMapXY(x, y);
	return g_pNaviThread->NuroOperateMapXY(x, y, nMouseCode);
}

nuPVOID CINaviControl::INcGetMapInfo(nuUINT nMapCode)
{
	return g_pNaviThread->NuroGetMapInfo(nMapCode);
}

nuPVOID CINaviControl::INcGetGPSData()
{
	return &(g_pNaviThread->m_gpsData);
}

nuUINT CINaviControl::INcSetMemoData(nuPVOID lpVoid)
{
	return g_pNaviThread->NuroSetMemoData(lpVoid);
}

nuUINT CINaviControl::INcSetMemoPtData(nuPVOID lpVoid) //轨迹
{
	return g_pNaviThread->NuroSetMemoPtData(lpVoid);
}

nuPVOID CINaviControl::INcGetLaneInfo() //车道信息
{
	return &g_fileSysApi.pGdata->drawInfoNavi.LaneGuide;
}

nuVOID CINaviControl::INcGetSPTInfo(SPT_DATA &SPTData) 
{
	nuMemcpy(&SPTData, &g_fileSysApi.pGdata->drawInfoNavi.SPTData, sizeof(SPT_DATA));
}
nuLONG* CINaviControl::INcGetScaleList(nuLONG *pScaleCount)
{
	*pScaleCount = g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount;
	return g_fileSysApi.pMapCfg->pScaleList;
}

nuBOOL CINaviControl::INcSetNaviThread(nuBYTE nNaviState)
{
	return g_pNaviThread->NuroSetNaviThread(nNaviState);
}

nuBOOL CINaviControl::INcShilftNaviThread(nuBYTE bOpenOrClose)	
{
	if( 1 == bOpenOrClose )
	{
		return INcSetNaviThread(NAVI_THD_WORK);
	}
	else if( 0 == bOpenOrClose )
	{
		return INcSetNaviThread(NAVI_THD_STOP_PREDRAW);
	}
	else if( 2 == bOpenOrClose )
	{	
		return INcSetNaviThread(NAVI_THD_STOP_DRAW);
	}
	return 0;
}

nuUINT  CINaviControl::INcGetSystemTime(PNURO_SYS_TIME pNuroTime)
{
	if( NULL == pNuroTime )
	{
		return 0;
	}
	pNuroTime->nYear	= g_fileSysApi.pGdata->timerData.nYear;
	pNuroTime->nMonth	= g_fileSysApi.pGdata->timerData.nMonth;
	pNuroTime->nDay		= g_fileSysApi.pGdata->timerData.nDay;
	pNuroTime->nHour	= nuLOBYTE(g_fileSysApi.pGdata->timerData.nHour);
	pNuroTime->nMinute	= g_fileSysApi.pGdata->timerData.nMinute;
	pNuroTime->nSecond	= g_fileSysApi.pGdata->timerData.nSecond;
	pNuroTime->nMilliseconds	= nuLOWORD(g_fileSysApi.pGdata->timerData.nMiniSecond);
	if(pNuroTime->nYear < 2000)
	{
		pNuroTime->nYear += 2000;
	}
	return 1;
}

nuPVOID CINaviControl::INcGetRoutingList(nuINT index, nuINT *ptotalCount)
{
	return g_pNaviThread->NuroGetRoutingList(index, ptotalCount);
}

nuBOOL CINaviControl::INcTMCRoute(nuLONG lTmcRouteCount)//added by daniel 20110830
{
	return g_pNaviThread->NuroTMCRoute(lTmcRouteCount);
}

nuBOOL CINaviControl::INcTOBERESETGYRO() // Added by Damon 20110906
{
	return g_pNaviThread->NuroTOBERESETGYRO();
}

nuBOOL CINaviControl::INcTOBEGETVERSION() // Added by Louis 20111110
{
	return g_pNaviThread->NuroTOBEGETVERSION();
}

nuBOOL CINaviControl::INcSaveLastPosition()//added by daniel  20111111
{
	return g_pNaviThread->bUISaveLastPosition();
}

nuBOOL CINaviControl::INcNuroSetMAPPT(nuBYTE nSaveType) // Added by Damon 20120110
{
	return g_pNaviThread->NuroSetMAPPT(nSaveType);
}

nuBOOL CINaviControl::INcSetRTFullView(nuLONG x, nuLONG y, nuUINT nCode) //added by xiaoqin 20120517
{
	return g_pNaviThread->SetRTFullView(x, y, nCode);
}

nuBOOL CINaviControl::INcResetScreenSize(nuWORD nWidth, nuWORD nHeight)
{
	return g_pNaviThread->NuroSetMapScreenSize(nWidth, nHeight);
}

nuBOOL CINaviControl::INcGPS_ReSet(PGPSCFG pGpsSetData)
{
	return g_pNaviThread->NuroResetGPS(pGpsSetData);
}

nuBOOL CINaviControl::INcGetTripInfo(nuPVOID lpInfo) //add by chang 2012.7.3
{
	return g_pNaviThread->GetTripInfo(lpInfo);
}

nuBOOL CINaviControl::INcReSetTripInfo(nuPVOID lpInfo)//add by chang 2012.7.3
{
	return g_pNaviThread->ReSetTripInfo(lpInfo);
}

nuBOOL CINaviControl::INcConfigNavi(nuLONG nDis)
{
	return g_pNaviThread->NuroConfigNavi(nDis);
}
nuVOID CINaviControl::INcUIPlaySound(nuWCHAR *wsTTS)
{
	g_pNaviThread->UIPlaySound(wsTTS);
}
nuUINT CINaviControl::INcUI_TRIGGER_TMC_EVENT(nuBYTE nTMCMode, nuBYTE nShowTMCEvent)
{
	return g_pNaviThread->UI_TRIGGER_TMC_EVENT(nTMCMode, nShowTMCEvent);
}
nuBOOL CINaviControl::INcSetNaviParser(nuINT nMode,nuINT nType) 
{
	return g_pNaviThread->SetNaviParser(nMode,nType);
}
nuBOOL  CINaviControl::INcGetTMCPathData(nuPVOID pvoid,nuUINT* pbufcount)
{
	return g_pNaviThread->NuroGetTMCPathData(pbufcount,pvoid);
}
nuBOOL CINaviControl::INcNuro_NT_Exit()
{
	return g_pNaviThread->Nuro_NT_Exit();
}
//Added by Daniel ================Choose Road===START========== 20150207//
nuUINT CINaviControl::INcChoose_Car_Road(nuBYTE nChosenRoad)
{
	return g_pNaviThread->Nuro_Choose_Car_Road(nChosenRoad);
}
//Added by Daniel ================Choose Road===ENDT========== 20150207//
#ifdef ANDROID_GPS
nuINT CINaviControl::INcGPS_JNI_RecvData(const nuCHAR *pBuff, nuINT len)
{
    nuINT Ret = 0;
    if(NULL != g_gpsopenApi.GPS_JNI_RecvData)
    {
	     Ret = (nuINT)g_gpsopenApi.GPS_JNI_RecvData(pBuff,  len);
	}
	return Ret;
}
#endif
#ifdef	USE_HUD_DR_CALLBACK 
nuUINT CINaviControl::INcSet_HUD_CallBack(nuPVOID pFunc)
{
	if( pFunc == NULL )
	{
 		return 0;
	}
	if( g_fileSysApi.pGdata )
	{
		if(g_pNaviThread->m_pfHUDCallBack==NULL)
		{
			g_pNaviThread->m_pfHUDCallBack = (NC_HUDCallBackProc)pFunc;
	}
	}
	return 1;
	
}
nuUINT CINaviControl::INcSet_DR_CallBack(nuPVOID pFunc)
{
	if( pFunc == NULL )
	{
		return 0;
	}
	if( g_fileSysApi.pGdata )
	{
                if(g_pNaviThread->m_pfDRCallBack==NULL)
		g_pNaviThread->m_pfDRCallBack = (NC_DRCallBackProc)pFunc;
	}

	return 1;
}
#endif
#ifdef ANDROID
nuUINT CINaviControl::INcSet_SOUND_CallBack(nuPVOID pFunc)
{
	if( pFunc == NULL )
	{
		return 0;
	}
	if( g_fileSysApi.pGdata )
	{
		//__android_log_print(ANDROID_LOG_INFO, "s", "CINaviControl SOUND ");
		//g_pNaviThread->m_pfPlayWave=NULL;		
		g_pSoundThread->m_pfPlayWave = (NC_SOUNDCallBackProc)pFunc;
		
	}

	return 1;
}
#endif
nuUINT CINaviControl::INcUISendGPSData(const nuPVOID pOrGpsData1, const nuPVOID pOrGpsData2)
{
	return g_pNaviThread->UISendGPSData(pOrGpsData1, pOrGpsData2);
}
nuBOOL CINaviControl::INcNavi_SoundPlay( nuBYTE nVoiceCode, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								   nuWORD nRoadNameNum, nuWCHAR *pRoadName )
{
	if( nVoiceCode == VOICE_CODE_DEFAULT_SYN || nVoiceCode == VOICE_CODE_DEFAULT_ASY)
	{
		return g_pSoundThread->Navi_SoundPlay(VOICE_TYPE_DEFAULT, 
			0, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_DEFAULT_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_ROAD_SYN || nVoiceCode == VOICE_CODE_ROAD_ASY )
	{
		return g_pSoundThread->Navi_SoundPlay( VOICE_TYPE_ROAD, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_ROAD_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_OTHERS_SYN || nVoiceCode == VOICE_CODE_OTHERS_ASY )
	{
		return g_pSoundThread->Navi_SoundPlay( VOICE_TYPE_OTHERS, 
			g_fileSysApi.pGdata->uiSetData.nLanguage, 
			nSound1, 
			nSound2, 
			nSound3,
			nRoadNameNum, 
			pRoadName,
			(nuBYTE)(nVoiceCode - VOICE_CODE_OTHERS_SYN) );
	}
	else if( nVoiceCode == VOICE_CODE_UI_DFT_SYN || nVoiceCode == VOICE_CODE_UI_DFT_ASY )
	{
		return g_pSoundThread->Navi_SoundPlay( VOICE_TYPE_UI_DEF,
			0,
			nSound1,
			nSound2,
			nSound3,
			nRoadNameNum,
			pRoadName,
			(nuBYTE)nVoiceCode - VOICE_CODE_UI_DFT_SYN );
	}
	return nuFALSE;
}

nuVOID CINaviControl::INcNavi_CleanSound()
{
	g_pSoundThread->Navi_CleanSound();
}

nuVOID CINaviControl::INcStopSoundPlaying() 
{
	g_pSoundThread->CleanSound();
	g_pSoundThread->StopSoundPlaying();
}
nuBYTE	CINaviControl::INcGetNaviState()
{
	return g_pNaviThread->m_nThdState;
}
nuBYTE  CINaviControl::INcSetNaviState(nuBYTE nNaviState)
{
	return g_pNaviThread->NuroSetNaviThread(nNaviState);
}
nuUINT CINaviControl::INcReciveUIData(nuLONG DefNum, nuPVOID pUIData1, nuPVOID pUIData2)
{
	return g_pNaviThread->ReciveUIData(DefNum, pUIData1, pUIData2);
}
//----------------------------------For SDK----------------------------------//
nuVOID	CINaviControl::INcMouseEvent(NURO_POINT2 Pt2)
{
	NUROPOINT ptMouse		 = {0};
	ptMouse.x 			 = Pt2.nX1;
	ptMouse.y 			 = Pt2.nY1;
	if(Pt2.nMouseType != NURO_DEF_ANDROID_ACTION_UP)
	{
		g_pDataThread->m_bWait = nuTRUE;
	}
	else
	{
		g_pDataThread->m_bWait = nuFALSE;
	}
	if(Pt2.VelocityX2)
	{	
		g_pNaviThread->Nuro_MouseEventEx(ptMouse, Pt2.nMouseType, Pt2.VelocityX1, Pt2.VelocityY1);
	}
	else
	{
		g_pNaviThread->Nuro_MouseEventEx2(ptMouse, Pt2.nMouseType, Pt2.VelocityX1, Pt2.VelocityY1); 
	}
}
nuLONG	CINaviControl::INcUISetEngine(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
{
__android_log_print(ANDROID_LOG_INFO, "navi", "INcUISetEngine-------------DefNum %d", DefNum);
	if(DefNum < NURO_DEF_NAVI_MAX)
	{
		if(DefNum == NURO_DEF_UI_SET_TMC_EVENT && pUIData != NULL)
		{
			NURO_TMC_EVENT *pTMCDATA = (NURO_TMC_EVENT *)pUIData;
			if(pTMCDATA->nMode == 0 && pTMCDATA->nShowEvent == _TMC_DISPLAY_MAP)
			{	
				__android_log_print(ANDROID_LOG_INFO, "TMC", "_TMC_DISPLAY_MAP");
				CODENODE CodeNode = {0};
				INcSetNaviThread(NAVI_THD_WORK);
				return g_pDataThread->CodePush(CodeNode);
			}
			else if(!g_fileSysApi.pGdata->uiSetData.bNavigation)
			{
				return 0;
			}
		}
		INcSetNaviThread(NAVI_THD_WORK);
		return g_pNaviThread->NuroUISetEngine(DefNum, pUIData, pUIReturnData);
	}
	else if(DefNum == NURO_DEF_SEARCH_END)
	{
		CALL_BACK_PARAM* cbParam = (CALL_BACK_PARAM*)pUIData;
		g_searchApi.SH_NEWSEARCH_SDK(cbParam);
		return INcSetNaviThread(NAVI_THD_WORK);
	}
	else
	{
		INcSetNaviThread(NAVI_THD_STOP_ALLL);
		CALL_BACK_PARAM* cbParam = (CALL_BACK_PARAM*)pUIData;
		if(DefNum == 0x0111/*NURO_DEF_SEARCH_NEARPOI*/)
		{
			__android_log_print(ANDROID_LOG_INFO, "nearpoi", "0x0111");
			PMAPCENTERINFO pMapInfo = (PMAPCENTERINFO)INcGetMapInfo(MAP_INFO_CENTER);
			__android_log_print(ANDROID_LOG_INFO, "nearpoi", "INDEX: %ld",pMapInfo->nFixedFileIdx );
			__android_log_print(ANDROID_LOG_INFO, "nearpoi", "pos: %ld %ld",pMapInfo->ptCenterRaw.x , pMapInfo->ptCenterRaw.y);
			nuroPOINT pos = {0};
			pos = pMapInfo->ptCenterRaw;
			cbParam-> nCallType=305;
			cbParam->pVOID = pMapInfo;
			g_searchApi.SH_NEWSEARCH_SDK(cbParam);	
		}
		else
		{		
			g_searchApi.SH_NEWSEARCH_SDK(cbParam);
		}
	}
	return 0;
}
nuPVOID	CINaviControl::INcUIGetEngineData(nuLONG DefNum, nuPVOID pUIData, nuPVOID pUIReturnData)
{
	if(NURO_DEF_UI_GET_UI_CONFIG == DefNum)
	{
		g_pNaviThread->NURO_UI_GET_USER_DATA(pUIData);
	}
	else if(NURO_DEF_UI_GET_ROUTE_LIST == DefNum)
	{
		NURO_SET_ROUTE_LIST *pSetNaviList = (NURO_SET_ROUTE_LIST*)pUIData;
		NAVI_LIST  *pRTlist = (NAVI_LIST *)pUIReturnData;
		return g_pNaviThread->NURO_GetNavi_List_Data(pUIReturnData, &(pSetNaviList->nNum), &(pSetNaviList->nIndex));
	}
	else if(NURO_DEF_UI_GET_MAP_TO_BMP_POINT == DefNum)
	{
		g_pNaviThread->TransferCoordinate(pUIData, pUIReturnData, nuTRUE);
	}
	else if(NURO_DEF_UI_GET_BMP_TO_MAP_POINT == DefNum)
	{
		g_pNaviThread->TransferCoordinate(pUIData, pUIReturnData, nuFALSE);
	}
	else if(NURO_DEF_UI_GET_TMC_PATH_DATA == DefNum)
	{	
		return g_pNaviThread->NURO_Get_TMC_NAVI_List((nuUINT*)pUIData, pUIReturnData);
	}
	return NULL;
}
nuVOID CINaviControl::INcSetEngineCallBack(nuINT iFuncID, nuPVOID pFunc)
{
	if( pFunc != NULL )
	{
		switch(iFuncID)
		{
		case NURO_SET_UI_THREAD_CALLBACK:
			break;
		case NURO_SET_NAVI_THREAD_CALLBACK:
			if( g_fileSysApi.pGdata )
			{
				g_fileSysApi.pGdata->pfNaviThreadCallBack = (CallBackFuncProc)pFunc;
			}
			break;
		case NURO_SET_TIME_THREAD_CALLBACK:
			break;
		case NURO_SET_SOUND_THREAD_CALLBACK:
			if(pFunc != NULL && g_pSoundThread != NULL)
			{
				g_pSoundThread->m_pfPlayWave = (NC_SOUNDCallBackProc)pFunc;
				__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_SET_SOUND_THREAD_CALLBACK");
			}
			break;
		case NURO_SET_OTHER_THREAD_CALLBACK:
			break;
		}
	}
}
//----------------------------------For SDK----------------------------------//
