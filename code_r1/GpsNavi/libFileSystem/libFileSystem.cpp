
#include "libFileSystem.h"
#include "NuroConstDefined.h"

#include "FileNameMgr.h"

#include "NuroBmpL.h"

#include "UserConfigDat.h"
#include "MapConfig.h"
#include "BlockDw.h"
#include "FileRn.h"
#include "FileRni.h"
#include "FilePn.h"
#include "FilePt.h"
#include "ReadLastPos.h"
#include "ReadWcstring.h"
#include "FileCCD.h"
#include "FileCb.h"
#include "FilePa.h"
#include "BlockDwCtrlZ.h"
#include "CheckEEyesZ.h"
#include "MapFileRdwZ.h"
#include "MapFileFhZ.h"
#include "WaypointsMgrZ.h"
#include "FileNewCCDZK.h"
#include "MapFileTMCDataP.h" //prosper
#include "FileMpS.h"                //Shyanx  20120213
#include "FileMapTMCLU.h"//Louis
#include "FIleKmDL.h"				//added by daniel for 20121205
#include "CTMCService.h"
//Library
//#include "SearchDanyinZ.h"

//#include "LogFileCtrlZ.h"

#if (defined(NURO_OS_WINDOWS) && !defined(NURO_OS_WINCE))
//#define _CONVERT_FH_FILE
//#define _CONVERT_DISASTER_FILE
#endif

#ifdef _CONVERT_FH_FILE
#include "TransfDataFhZ.h"
#endif

#ifdef _CONVERT_DISASTER_FILE
#include "TransfBrokenRdZK.h"
#endif

//#include "windows.h"

PMEMMGRAPI							g_pFsMmApi = NULL;
PGDIAPI								g_pFsGdiApi	= NULL;
static PGLOBALENGINEDATA			g_pGlobalEngineData = NULL;
static DRAWINFO						g_drawInfo;
static DRAWMEMORY					g_drawMemData;
static SCREENTRANFS					g_screenTransf;
static TRACEMEMORY					g_traceMem;
static OTHERGLOBAL					g_otherGlobal;

static class CBlockDw*				g_pBlockDw = NULL;
static class CBlockDwCtrlZ*			g_pBlkDwCtrl = NULL;

/* 2010.03.30
static class CFileNameMgr			g_fileNameMgr;
static class CUserConfigDat			g_userCfg;
static class CMapConfig				g_mapCfg;
static class CFileRn				g_fileRn;
static class CFilePn				g_filePn;
static class CFilePt				g_filePt;
static class CReadWcstring			g_readWcs;
*/
//////////////////////////////////////////////////////////////
static class CFileNameMgr*			g_pFileNameMgr = NULL;
static class CUserConfigDat*		g_pUserCfg	= NULL;
static class CMapConfig*			g_pMapCfg	= NULL;
static class CFileRn*				g_pFileRn	= NULL;
static class CFileRni*              g_pFileRni  = NULL;
static class CFilePn*				g_pFilePn	= NULL;
static class CFilePt*				g_pFilePt	= NULL;
static class CReadWcstring*			g_pReadWcs	= NULL;
//static class CFileCCD				g_fileCCD;
//static class CSearchDanyinZ*		g_pSearchDY = NULL;
static class CCheckEEyesZ*			g_pCheckEEyesZ = NULL;
static class CMapFileFhZ*			g_pFileFhZ = NULL;
#ifdef _USE_WAYPOINTS_MGRZ
static class CWaypointsMgrZ*		g_pWaypointsMgrZ = NULL;
#endif
static class CFileNewCCDZK*			g_pNewCcdZK = NULL;
static class CMapFileTMCDataP*	    g_pTMCData = NULL;   //prosper

static class CFileMpS*                    g_pMpS=NULL;   //Shyanx  20120213
static class CFileKMDL*					  g_pKMDL = NULL; // added by daniel 20121205
static class CTMCService*				  g_pTMCService = NULL;
nuBOOL bSavingLastPosition = nuFALSE;//added by daniel 20111111
nuBYTE  g_nBgPic = 0;
/* FileSystem's local functions */
nuBOOL FS_InitLastPosition(nuLONG& nMapX, nuLONG& nMapY, short& nCarAngle)
{
	/*
	CReadLastPos readPos;
	if( readPos.ReadLastPos(g_pFileNameMgr->GetAppPath(), nMapX, nMapY, nCarAngle) )
	{
		return nuTRUE;
	}
	*/
	if( CReadLastPos::NewReadLastPos(g_pFileNameMgr->GetAppPath(), nMapX, nMapY, nCarAngle) )
	{
		return nuTRUE;
	}
	else if( nuReadConfigValue("MAPCENTERX", &nMapX) && nuReadConfigValue("MAPCENTERY", &nMapY) )
	{
		nCarAngle = 90;
		return nuTRUE;
	}
	__android_log_print(ANDROID_LOG_INFO, "LastPoint", "nMapX %d, nMapY %d", nMapX, nMapY);
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuBOOL bLastPosition = nuFALSE;
	nuFILE* pFile = NULL;
	if( !bLastPosition )
	{
		nCarAngle = 90;
		nuTcscpy(tsFile, nuTEXT(".BH"));
		if( !g_pFileNameMgr->FindFileWholePath(0, tsFile, NURO_MAX_PATH) )
		{
			return nuFALSE;
		}
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "tsFile %s", tsFile);
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
		BHSTATICHEAD bhHeader = {0};
		if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 &&
			nuFread(&bhHeader, sizeof(BHSTATICHEAD), 1, pFile) == 1 )
		{
			nuroPOINT pt = {0};
			nuMemcpy(&pt, bhHeader.nDefaultXY, 8);
			nMapX = pt.x;
			nMapY = pt.y;
			bLastPosition = nuTRUE;
		}
		nuFclose(pFile);
	}
	__android_log_print(ANDROID_LOG_INFO, "LastPosition", "bLastPosition %d", bLastPosition);
	return bLastPosition;
}
nuBOOL FS_InitGlobalData()
{
	g_pGlobalEngineData->uiSetData.b3DMode			= g_pUserCfg->m_userCfg.b3DMode;
	g_pGlobalEngineData->uiSetData.bGPSReady		= 0;
	g_pGlobalEngineData->uiSetData.bNavigation		= 0;
	g_pGlobalEngineData->uiSetData.bNightOrDay		= g_pUserCfg->m_userCfg.bNightDay;
	if( g_pUserCfg->m_userCfg.bNightDay )
	{
		g_pGlobalEngineData->uiSetData.nBgPic		= g_pUserCfg->m_userCfg.nNightBgPic;
	}
	else
	{
		g_pGlobalEngineData->uiSetData.nBgPic		= g_pUserCfg->m_userCfg.nDayBgPic;
	}
	g_pGlobalEngineData->uiSetData.nScreenLTx		= g_pUserCfg->m_userCfg.nScreenLTx;
	g_pGlobalEngineData->uiSetData.nScreenLTy		= g_pUserCfg->m_userCfg.nScreenLTy;
	g_pGlobalEngineData->uiSetData.nScreenWidth		= g_pUserCfg->m_userCfg.nScreenWidth;
	g_pGlobalEngineData->uiSetData.nScreenHeight	= g_pUserCfg->m_userCfg.nScreenHeight;
	g_pGlobalEngineData->uiSetData.nLanguage		= g_pUserCfg->m_userCfg.nLanguage;
	g_pGlobalEngineData->uiSetData.nTTSWaveMode		= g_pUserCfg->m_userCfg.nWaveTTSType;
	g_pGlobalEngineData->uiSetData.nMapDirectMode	= g_pUserCfg->m_userCfg.nMapDirectMode;
	g_pGlobalEngineData->uiSetData.nCarIconType		= g_pUserCfg->m_userCfg.nCarIconType;
	g_pGlobalEngineData->uiSetData.nShowLongLat		= g_pUserCfg->m_userCfg.bShowLongLat;
	g_pGlobalEngineData->uiSetData.nScreenType		= SCREEN_TYPE_DEFAULT;//g_pUserCfg->m_userCfg.nScreenType;
	g_pGlobalEngineData->uiSetData.nScaleIdx		= g_pUserCfg->m_userCfg.nLastScaleIndex;//g_pMapCfg->m_pMapConfig->commonSetting.nStartScale;
	g_pGlobalEngineData->uiSetData.nTotalScales		= g_pMapCfg->m_pMapConfig->commonSetting.nScaleLayerCount - 1;
	g_pGlobalEngineData->uiSetData.nScaleValue		= g_pMapCfg->m_pMapConfig->scaleSetting.scaleTitle.nScale;
	if( !nuReadConfigValue("BSDSCALEVALUE", &g_pGlobalEngineData->uiSetData.nBsdStartScaleVelue) )
	{
		g_pGlobalEngineData->uiSetData.nBsdStartScaleVelue = 1000;
	}
	if( !nuReadConfigValue("BGLSCALEVALUE", &g_pGlobalEngineData->uiSetData.nBglStartScaleValue) )
	{
		g_pGlobalEngineData->uiSetData.nBglStartScaleValue = 5000;
	}

	// Added by Dengxu @ 2012 11 04
#ifdef USING_FILE_SETTING_VIEW_ANGLE
	nuLONG maxAngle, minAngle;

	if (nuFALSE == nuReadConfigValue("3D_MAX_ANGLE", &maxAngle))
	{
		maxAngle = 23;
	}

	//if (nuFALSE == nuReadConfigValue("3D_MIN_ANGLE", &minAngle))
	{
		minAngle = 1;
	}

	g_pGlobalEngineData->uiSetData.n3DViewAngle	= maxAngle;
#else
	g_pGlobalEngineData->uiSetData.n3DViewAngle	= _3D_MAX_ANGLE;
#endif
	
	//
	nuMemset(&g_pGlobalEngineData->transfData, 0, sizeof(g_pGlobalEngineData->transfData));
	g_pGlobalEngineData->transfData.nCarCenterMode	= CAR_CENTER_MODE_ONE_HALF_XY;
	g_pGlobalEngineData->transfData.nBmpSizeMode	= BMPSIZE_EXTEND_NO;
	nuBOOL bres = FS_InitLastPosition( g_pGlobalEngineData->transfData.nMapCenterX,
									 g_pGlobalEngineData->transfData.nMapCenterY,
									 g_pGlobalEngineData->carInfo.nCarAngle );
	g_pGlobalEngineData->carInfo.nCarAngleRaw   = g_pGlobalEngineData->carInfo.nCarAngle;//added by daniel 20120131
	g_pGlobalEngineData->carInfo.nMapAngle		= 90;
	g_pGlobalEngineData->carInfo.nRotateAngle	= 0;
	/*
	nuBOOL bres = nuTRUE;
	g_pGlobalEngineData->carInfo.nCarAngle = 90;//test
	g_pGlobalEngineData->transfData.nMapCenterX	= 12125674;//test
	g_pGlobalEngineData->transfData.nMapCenterY = 2488191;//test ucos*/
	//
	//For timeData
	nuMemset(&g_pGlobalEngineData->timerData, 0, sizeof(g_pGlobalEngineData->timerData));
	g_pGlobalEngineData->timerData.bShowScale	= 1;
	if( !g_pUserCfg->m_userCfg.nVoiceIconTimer )
	{
		g_pGlobalEngineData->timerData.bShowVoiceIcon = 1;
	}
	/*test
	g_pUserCfg->m_userCfg.nScaleShowMode	= 10;
	*/
	//
	//DrawInfoMap
	nuLONG nTmp = 0;
	if( !nuReadConfigValue("SCALEPIXELS", &nTmp) )
	{
		nTmp = SCALETOSCREEN_LEN;
	}
	g_pGlobalEngineData->drawInfoMap.nScalePixels		= nuLOWORD(nTmp);
	//--- Added @ 2012.12.03 ---
	if( g_pGlobalEngineData->drawInfoMap.nScalePixels > 75 )
	{
		g_pGlobalEngineData->drawInfoMap.nScalePixels = 75;
	}
	//--------------------------
	//DrawInfoNavi
	if(g_pUserCfg->m_userCfg.nRouteListNum > 15 || g_pUserCfg->m_userCfg.nRouteListNum < 3 )
	{
		g_pUserCfg->m_userCfg.nRouteListNum = 8;
	}
	g_pGlobalEngineData->drawInfoNavi.nRtListPageNum	= g_pUserCfg->m_userCfg.nRouteListNum;
	g_pGlobalEngineData->drawInfoNavi.pRtList			= (PSHOWNODE)g_pFsMmApi->MM_GetStaticMemMass(sizeof(SHOWNODE) * g_pGlobalEngineData->drawInfoNavi.nRtListPageNum);
	//
//	g_pGlobalEngineData->NC_SoundPlay = NULL;
	g_pGlobalEngineData->routeGlobal.RoutingRule		= g_pUserCfg->m_userCfg.nRouteMode;
	//
	return bres;
}

nuVOID FS_FreeResidentData()
{
	g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpBg);
	g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmp3D);
	g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpCrossBg);
}

#define BMPBG_SIZE_W					32
#define BMPBG_SIZE_H					32
#define BMP3D_SIZE_W					480
#define BMP3D_SIZE_H					60			
nuBOOL FS_InitDrawInfo()
{
	g_drawInfo.drawInfoCommon.nRealPicID		= 0;
#ifdef _USE_DDB_BITMAP
	g_drawInfo.drawInfoCommon.nuBmpBg.hBitmap	= NULL;
	g_drawInfo.drawInfoCommon.nuBmp3D.hBitmap	= NULL;
	g_drawInfo.drawInfoCommon.nuBmpCrossBg.hBitmap	= NULL;
#else
	g_drawInfo.drawInfoCommon.nuBmpBg.bmpBitCount	= NURO_BMP_BITCOUNT_16;
	//
	g_drawInfo.drawInfoCommon.nuBmp3D.bmpBitCount	= NURO_BMP_BITCOUNT_16;
	g_drawInfo.drawInfoCommon.nuBmpCrossBg.bmpBitCount	= NURO_BMP_BITCOUNT_16;
#endif

	g_drawInfo.drawInfoCommon.pTsPath	= g_pFileNameMgr->GetAppPath();
	g_drawInfo.pcUISet	= &g_pGlobalEngineData->uiSetData;
	g_drawInfo.pcTransf	= &g_pGlobalEngineData->transfData;
	g_drawInfo.pcCarInfo	= &g_pGlobalEngineData->carInfo;
	g_drawInfo.pcTimerData	= &g_pGlobalEngineData->timerData;
	g_drawInfo.pcDrawInfoMap	= &g_pGlobalEngineData->drawInfoMap;
	g_drawInfo.pcDrawInfoNavi	= &g_pGlobalEngineData->drawInfoNavi;
	g_drawInfo.pcZoomScreenData	= &g_pGlobalEngineData->zoomScreenData;
	return nuTRUE;
}

/*Dll entry. platform dependent*/
#ifdef _USRDLL
/*nuBOOL nuAPIENTRY NURO_DLLMAIN(	nuHANDLE hModule, 
								nuDWORD  ul_reason_for_call, 
								nuPVOID lpReserved
							  )
{
    switch (ul_reason_for_call)
	{
		case NURO_DLL_PROCESS_ATTACH:
			break;
		case NURO_DLL_THREAD_ATTACH:
			break;
		case NURO_DLL_THREAD_DETACH:
			break;
		case NURO_DLL_PROCESS_DETACH:
			LibFS_FreeFileSystem();
			break;
    }
    return TRUE;
}*/
#endif
/* FileSystem's Api functions */
FILESYSTEM_API nuBOOL LibFS_InitFileSystem(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpOutFs)
{
	g_pFsMmApi	= (PMEMMGRAPI)lpInMM;
	g_pFsGdiApi	= (PGDIAPI)lpInGDI;
	/////////////////////////////////////////////////////////////////////
	g_pFileNameMgr	= new CFileNameMgr;
	g_pUserCfg		= new CUserConfigDat;
	g_pMapCfg		= new CMapConfig;
	g_pFileRn		= new CFileRn;
	g_pFileRni      = new CFileRni;
	g_pFilePn		= new CFilePn;
	g_pFilePt		= new CFilePt;
	g_pReadWcs		= new CReadWcstring;
	if( NULL == g_pFileNameMgr || NULL == g_pUserCfg ||
		NULL == g_pMapCfg	|| NULL == g_pFileRn ||
		NULL == g_pFilePt	|| NULL == g_pReadWcs ||
		NULL == g_pFileRni )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new classes fail.....");
		return nuFALSE;
	}
	////////////////////////////////////////////////////////////////
	g_pBlockDw = new CBlockDw();
	if( g_pBlockDw == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pBlockDw fail.....");
		return nuFALSE;
	}
	g_pBlkDwCtrl = new CBlockDwCtrlZ();
	if( g_pBlkDwCtrl == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pBlkDwCtrl fail.....");
		return nuFALSE;
	}
	g_pCheckEEyesZ = new CCheckEEyesZ();
	if( g_pCheckEEyesZ == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pCheckEEyesZ fail.....");
		return nuFALSE;
	}
	g_pFileFhZ	= new CMapFileFhZ();
	if( g_pFileFhZ == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pFileFhZ fail.....");
		return nuFALSE;
	}
#ifdef _USE_WAYPOINTS_MGRZ
	g_pWaypointsMgrZ = new CWaypointsMgrZ();
	if( g_pWaypointsMgrZ == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pWaypointsMgrZ fail.....");
		return nuFALSE;
	}
#endif
	g_pNewCcdZK	= new CFileNewCCDZK();
	if( NULL == g_pNewCcdZK )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pNewCcdZK fail.....");
		return nuFALSE;
	}
	/*g_pTMCData	= new CMapFileTMCDataP();  //prosper
	if( NULL == g_pTMCData )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pTMCData fail.....");
		return nuFALSE;
	}*/
	g_pMpS	= new CFileMpS();  //Shyanx   20120213
	if( NULL == g_pMpS )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pMpS fail.....");
		return nuFALSE;
	}
	
	g_pTMCService = new CTMCService();
	if( NULL == g_pTMCService )
	{
		return nuFALSE;
	}
	/* The order should be more attention. */
	g_pGlobalEngineData = (PGLOBALENGINEDATA)g_pFsMmApi->MM_GetStaticMemMass(sizeof(GLOBALENGINEDATA));
	if( g_pGlobalEngineData == NULL )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pGlobalEngineData fail.....");
		return nuFALSE;
	}
	g_pGlobalEngineData->drawInfoNavi.pTMCList = NULL;
	g_pGlobalEngineData->drawInfoNavi.bTMCListUsed = 0;
	if( !g_pFileNameMgr->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pFileNameMgr fail.....");
		return nuFALSE;
	}
	g_pGlobalEngineData->pTsPath	= g_pFileNameMgr->GetAppPath();
	if( !g_pUserCfg->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pUserCfg fail.....");
		return nuFALSE;
	}
	if( !g_pMapCfg->Initialize(g_pUserCfg->m_userCfg.bNightDay, g_pUserCfg->m_userCfg.nLastScaleIndex) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pMapCfg fail.....");
		return nuFALSE;
	}
	if( !g_pBlockDw->Initialize(g_pFileFhZ) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pBlockDw fail.....");
		return nuFALSE;
	}
	if( !g_pBlkDwCtrl->Initialize(g_pBlockDw) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pBlkDwCtrl fail.....");
		return nuFALSE;
	}
	if( !g_pReadWcs->Initialize(g_pGlobalEngineData) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pReadWcs fail.....");
		return nuFALSE;
	}
	if( !g_pFileRn->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pFileRn fail.....");
		return nuFALSE;
	}
	if( !g_pFileRni->Initialize())
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pFileRni fail.....");
		return nuFALSE;
	}
	if( !g_pFilePt->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pFilePt fail.....");
		return nuFALSE;
	}
	/*
	if( !g_fileCCD.Initialize() )
	{
		return nuFALSE;
	}
	*/
	if( !g_pCheckEEyesZ->Initialize(g_pGlobalEngineData) )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pCheckEEyesZ fail.....");
		return nuFALSE;
	}
	//
	if( !FS_InitGlobalData() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init !FS_InitGlobalData fail.....");
		return nuFALSE;
	}
	if( !FS_InitDrawInfo() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init !FS_InitDrawInfo fail.....");
		return nuFALSE;
	}
	LibFS_LoadResidentData(RES_DATA_DAYNIGHT_CHANGE|RES_DATA_3DMODE_CHANGE);

	/*Load Global Data*/
	//DrawInfo
//	nuTCHAR tsFile[NURO_MAX_PATH];
	//
	//
	nuMemset(&g_traceMem, 0, sizeof(g_traceMem));
	nuMemset(&g_otherGlobal, 0, sizeof(g_otherGlobal));
	//
	PFILESYSAPI pFileApi = (PFILESYSAPI)lpOutFs;
	pFileApi->FS_SaveUserConfig		= LibFS_SaveUserConfig;
	pFileApi->FS_CoverFromDefault	= LibFS_CoverFromDefault;
	pFileApi->FS_ReLoadMapConfig	= LibFS_ReLoadMapConfig;
	pFileApi->FS_GetData			= LibFS_GetData;
	pFileApi->FS_GetFilePath		= LibFS_GetFilePath;
	pFileApi->FS_FindFileWholePath	= LibFS_FindFileWholePath;
	pFileApi->FS_GetMapCount		= LibFS_GetMapCount;
	pFileApi->FS_PtInBoundary		= LibFS_PtInBoundary;
	pFileApi->FS_LoadBlock			= LibFS_LoadBlock;
	pFileApi->FS_GetScreenMap		= LibFS_GetScreenMap;
	pFileApi->FS_GetRoadName		= LibFS_GetRoadName;
	pFileApi->FS_GetRoadIconInfo    = LibFS_GetRoadIconInfo; //Added by xiaoqin
	pFileApi->FS_GetPoiName			= LibFS_GetPoiName;
	pFileApi->FS_LoadResidentData	= LibFS_LoadResidentData;
	pFileApi->FS_SavePtFile			= LibFS_SavePtFile;
	pFileApi->FS_SavePtFileCoverForDFT = LibFS_SavePtFileCoverForDFT; 
	pFileApi->FS_GetPtDataAddr		= LibFS_GetPtDataAddr;
	pFileApi->FS_GetPtNodeByType	= LibFS_GetPtNodeByType;
	pFileApi->FS_GetInfoBySourceType	= LibFS_GetInfoBySourceType;
	pFileApi->FS_GetInfoByNuroType	= LibFS_GetInfoByNuroType;
	pFileApi->FS_ReadConstWcs		= LibFS_ReadConstWcs;
	pFileApi->FS_ReadWcsFromFile	= LibFS_ReadWcsFromFile;
	pFileApi->FS_ReadInfoMapCenterName	= LibFS_ReadInfoMapCenterName;
	pFileApi->FS_GetCCDData			= LibFS_GetCCDData;
	pFileApi->FS_GetCCDName			= LibFS_GetCCDName;
	pFileApi->FS_SeekForRoad		= LibFS_SeekForRoad;
	pFileApi->FS_PaddingRoad		= LibFS_PaddingRoad;
	pFileApi->FS_SeekForPoi			= LibFS_SeekForPoi;
	pFileApi->FS_SaveLastPos		= LibFS_SaveLastPos;
	pFileApi->FS_ReadCityTownName	= LibFS_ReadCityTownName;
	pFileApi->FS_ReadPaData			= LibFS_ReadPaData;
	pFileApi->FS_MapIDIndex			= LibFS_MapIDIndex;
	pFileApi->FS_LoadDwBlock		= LibFS_LoadDwBlock;
	pFileApi->FS_FreeDwBlock		= LibFS_FreeDwBlock;
	pFileApi->FS_CheckEEyes			= LibFS_CheckEEyes;
	pFileApi->FS_GetMpnData			= LibFS_GetMpnData;
	pFileApi->pGdata				= g_pGlobalEngineData;
	pFileApi->pDrawInfo				= &g_drawInfo;
	pFileApi->pUserCfg				= &g_pUserCfg->m_userCfg;
	pFileApi->pMapCfg				= g_pMapCfg->m_pMapConfig;

	pFileApi->FS_SeekForRoads		= LibFS_SeekForRoads;
	pFileApi->FS_GetRoadSegData		= LibFS_GetRoadSegData;

	pFileApi->FS_SetWaypoints		= LibFS_SetWaypoints;
	pFileApi->FS_DelWaypoints		= LibFS_DelWaypoints;
	pFileApi->FS_GetWaypoints		= LibFS_GetWaypoints;
	pFileApi->FS_KptWaypoints		= LibFS_KptWaypoints;

	pFileApi->FS_SeekForCcdInfo		= LibFS_SeekForCcdInfo;
	pFileApi->FS_CheckNewCcd		= LibFS_CheckNewCcd;

    pFileApi->FS_Get_TMC_VD_SIZE        = LibFS_FS_Get_TMC_VD_SIZE;  //prosper
	pFileApi->FS_Get_TMC_FM_XML_SIZE	= LibFS_Get_TMC_FM_XML_SIZE;
    pFileApi->FS_Get_TMC_VD         = LibFS_FS_Get_TMC_VD;
	pFileApi->FS_Get_TMC_FM_XML	    = LibFS_Get_TMC_FM_XML;
	pFileApi->FS_Get_TMC_FM	        = LibFS_Get_TMC_FM;
	pFileApi->FS_Get_TMC_XML	    = LibFS_Get_TMC_XML;
	pFileApi->FS_Get_TMC_XML_L	    = LibFS_Get_TMC_XML_L;
	pFileApi->FS_Set_TMC_Byte		= LibFS_Set_TMC_Byte;
	pFileApi->FS_Get_TMC_Router     = LibFS_Get_TMC_Router;
	pFileApi->FS_ChangePoiInfo      = LibFS_ChangePoiInfo;   //Shyanx 2012.02.13

	pFileApi->FS_GetKMDataDL	   = LibFS_GetKMDataDL;
	pFileApi->FS_GetKMDataMapping  = LibFS_GetKMDataMapping;
	pFileApi->FS_SeekForKMInfo	   = LibFS_SeekForKMInfo;
	pFileApi->FS_CheckKMInfo       = LibFS_CheckKMInfo;
	pFileApi->FS_GetTMCTrafficInfo         = LibFS_GetTMCTrafficInfo;
    pFileApi->FS_GetTMCTrafficInfoByNodeID = LibFS_GetTMCTrafficInfoByNodeID;
	pFileApi->FS_GetTMCDirInfo         = LibFS_GetTMCDirInfo;
    pFileApi->FS_GetTMCDirInfoByNodeID = LibFS_GetTMCDirInfoByNodeID;
	pFileApi->FS_GetTMCDataInfo          = LibFS_GetTMCDataInfo;
	pFileApi->FS_Initial_TMC_Event_Data = LibFS_Initial_TMC_Event_Data;
	pFileApi->FS_Release_TMC_Event_Data = LibFS_Release_TMC_Event_Data;
	pFileApi->FS_Initial_TMC_Data       = LibFS_Initial_TMC_Data;
	pFileApi->FS_Release_TMC_Data       = LibFS_Release_TMC_Data;
	pFileApi->FS_COPY_TMC_Event_Data	= LibFS_COPY_TMC_Event_Data;

	APISTRUCTADDR apiAddr;
	apiAddr.pFsApi	= (PFILESYSAPI)lpOutFs;
	apiAddr.pMmApi  = (PMEMMGRAPI)lpInMM;
	CFileMapBaseZ::InitData(&apiAddr);
	//	CSearchBaseZK::InitData(&apiAddr);
	CFileSystemBaseZ::Initialize(&apiAddr);
#ifdef _CONVERT_FH_FILE
	CTransfDataFhZ transfFh;
	nuTCHAR tsBh[NURO_MAX_PATH], tsFh[NURO_MAX_PATH];
	nuWORD nCount = LibFS_GetMapCount();
	for(nuWORD n = 0; n < nCount; ++n )
	{
		nuTcscpy(tsBh, nuTEXT(".bh"));
		LibFS_FindFileWholePath(n, tsBh, NURO_MAX_PATH);
		nuTcscpy(tsFh, tsBh);
		nuTcscpy(&tsFh[nuTcslen(tsFh)-2], nuTEXT("fh"));
		transfFh.TransfFh(tsBh, tsFh);
	}
#endif	
	
	

	if( !g_pFileFhZ->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pFileFhZ fail.....");
		return nuFALSE;
	}

#ifdef _USE_WAYPOINTS_MGRZ
	if( !g_pWaypointsMgrZ->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pWaypointsMgrZ fail.....");
		return nuFALSE;
	}
#endif
	if( !g_pNewCcdZK->Initialize() )
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS init pNewCcdZK fail.....");
		return nuFALSE;
	}
	

	//---------added by daniel 20121205----------//
	g_pKMDL = new CFileKMDL();
	if(NULL == g_pKMDL)
	{
	__android_log_print(ANDROID_LOG_INFO, "navi", "libFS new pNewCcdZK fail.....");
		return nuFALSE;
	}
	if(!g_pKMDL->Initial())
	{ 
		g_pGlobalEngineData->drawInfoMap.bUsedKm = nuFALSE;
	} 
	else
	{
		g_pGlobalEngineData->drawInfoMap.bUsedKm = nuTRUE;
	}

	//---------added by daniel 20121205----------//
/******** Add By @zhikun **********************/
//Conert Disaster Road Data
#ifdef	_CONVERT_DISASTER_FILE
	CTransfBrokenRdZK transfRd;
	nuTCHAR tzFileRt[256] = {0};
	nuTCHAR tzFileRaw[256] = {0};
	nuTCHAR tzFileOut[256] = {0};
	nuTcscpy(tzFileRt, nuTEXT(".rt"));
	LibFS_FindFileWholePath(0, tzFileRt, 256);
	LibFS_GetFilePath(FILE_NAME_PATH, tzFileRaw, 256);
	nuTcscpy(tzFileOut, tzFileRaw);
	nuTcscat(tzFileRaw, nuTEXT("Disaster.TW"));
	nuTcscat(tzFileOut, nuTEXT("tw.dst"));
	transfRd.TransfRoadPoint(tzFileRaw, tzFileRt, tzFileOut);
#endif
/******** Add on @2009.08.27 ******************/

	return nuTRUE;
}
FILESYSTEM_API nuVOID LibFS_FreeFileSystem()
{
	/*Free Global data*/
	//DrawInfo
//	nuDeleteNuroBitmap( &g_drawInfo.infoMapMenu.nuMenuBmp );
	//
	FS_FreeResidentData();
#ifdef _USE_WAYPOINTS_MGRZ
	if( g_pWaypointsMgrZ )
	{
		g_pWaypointsMgrZ->Free();
		delete g_pWaypointsMgrZ;
		g_pWaypointsMgrZ = NULL;
	}
#endif
	if( g_pFileFhZ )
	{
		g_pFileFhZ->Free();
		delete g_pFileFhZ;
		g_pFileFhZ = NULL;
	}
	if( g_pCheckEEyesZ )
	{
		g_pCheckEEyesZ->Free();
		delete g_pCheckEEyesZ;
		g_pCheckEEyesZ = NULL;
	}
	/////////////////////////////
	if( g_pBlkDwCtrl != NULL )
	{
		g_pBlkDwCtrl->Free();
		delete g_pBlkDwCtrl;
		g_pBlkDwCtrl = NULL;
	}
	if( g_pBlockDw != NULL )
	{
		g_pBlockDw->Free();
		delete g_pBlockDw;
		g_pBlockDw = NULL;
	}
//	g_fileCCD.Free();
	/* 2010.03.30
	g_readWcs.Free();
	g_filePt.Free();
	g_filePn.Free();
	g_fileRn.Free();
	g_mapCfg.Free();
	g_userCfg.Free();
	g_fileNameMgr.Free();
	*/
	if( NULL != g_pFileNameMgr )
	{
		g_pFileNameMgr->Free();
		delete g_pFileNameMgr;
		g_pFileNameMgr = NULL;
	}
	if( NULL != g_pUserCfg )
	{
		g_pUserCfg->Free();
		delete g_pUserCfg;
		g_pUserCfg = NULL;
	}
	if( NULL != g_pMapCfg )
	{
		g_pMapCfg->Free();
		delete g_pMapCfg;
		g_pMapCfg = NULL;
	}
	if( NULL != g_pFileRn )
	{
		g_pFileRn->Free();
		delete g_pFileRn;
		g_pFileRn = NULL;
	}
	if(NULL != g_pFileRni)
	{
		g_pFileRni->Free();
		delete g_pFileRni;
		g_pFileRni = NULL;
	}
	if( NULL != g_pFilePt )
	{
		g_pFilePt->Free();
		delete g_pFilePt;
		g_pFilePt = NULL;
	}
	if( NULL != g_pReadWcs )
	{
		g_pReadWcs->Free();
		delete g_pReadWcs;
		g_pReadWcs = NULL;
	}
	if( NULL != g_pNewCcdZK )
	{
		g_pNewCcdZK->Free();
		delete g_pNewCcdZK;
		g_pNewCcdZK = NULL;
	
	}
	if( NULL != g_pTMCData )
	{
		g_pTMCData->Free();
		delete g_pTMCData;
		g_pTMCData = NULL;
	
	}
	if( NULL != g_pMpS )   //Shyanx  2012.02.13
	{
		g_pMpS->Free();
		delete g_pMpS;
		g_pMpS = NULL;

	}
	if( NULL != g_pFilePn )
	{
		g_pFilePn->Free();
		delete g_pFilePn;
		g_pFilePn = NULL;
	}
	if( NULL != g_pTMCService )
	{
        g_pTMCService->Free();
		delete g_pTMCService;
		g_pTMCService = NULL;
	}


	if(NULL != g_pKMDL)
	{
		g_pKMDL->Free();
		delete g_pKMDL;
		g_pKMDL = NULL;
	}
 
	g_pGlobalEngineData = NULL;


}

FILESYSTEM_API nuBOOL LibFS_SaveUserConfig()
{
	nuTCHAR tsFile[NURO_MAX_FILE_PATH] = {0};
	if( !g_pFileNameMgr->GetFilePath(FILE_NAME_USER_CFG_TMP, tsFile, NURO_MAX_FILE_PATH) )
	{
		return nuFALSE;
	}
//	g_pUserCfg->m_userCfg.nLastScaleIndex = g_pGlobalEngineData->uiSetData.nScaleIdx;
	return g_pUserCfg->SaveUserCfgDat(tsFile);
}

FILESYSTEM_API nuBOOL LibFS_CoverFromDefault()
{
	nuBOOL bres = g_pUserCfg->CoverFromDefault();
	g_pGlobalEngineData->uiSetData.b3DMode			= g_pUserCfg->m_userCfg.b3DMode;
	g_pGlobalEngineData->uiSetData.bNightOrDay		= g_pUserCfg->m_userCfg.bNightDay;
	if( g_pUserCfg->m_userCfg.bNightDay )
	{
		g_pGlobalEngineData->uiSetData.nBgPic		= g_pUserCfg->m_userCfg.nNightBgPic;
	}
	else
	{
		g_pGlobalEngineData->uiSetData.nBgPic		= g_pUserCfg->m_userCfg.nDayBgPic;
	}
//	g_pGlobalEngineData->uiSetData.nScreenType		= g_pUserCfg->m_userCfg.nScreenType;
	g_pGlobalEngineData->uiSetData.nLanguage		= g_pUserCfg->m_userCfg.nLanguage;
	g_pGlobalEngineData->uiSetData.nTTSWaveMode		= g_pUserCfg->m_userCfg.nWaveTTSType;
	g_pGlobalEngineData->uiSetData.nMapDirectMode	= g_pUserCfg->m_userCfg.nMapDirectMode;
	g_pGlobalEngineData->uiSetData.nCarIconType		= g_pUserCfg->m_userCfg.nCarIconType;
	g_pGlobalEngineData->routeGlobal.RoutingRule	= g_pUserCfg->m_userCfg.nRouteMode;
	g_pGlobalEngineData->uiSetData.nShowLongLat		= g_pUserCfg->m_userCfg.bShowLongLat;
	bres &= LibFS_ReLoadMapConfig();
	bres &= LibFS_LoadResidentData(RES_DATA_DAYNIGHT_CHANGE|RES_DATA_3DMODE_CHANGE);
	return bres;
}

FILESYSTEM_API nuBOOL LibFS_ReLoadMapConfig()
{
	nuBOOL bRes = g_pMapCfg->ReadMapSetting( g_pGlobalEngineData->uiSetData.nScaleIdx, 
										 g_pGlobalEngineData->uiSetData.bNightOrDay );
	return bRes;
}
FILESYSTEM_API nuBOOL LibFS_LoadResidentData(nuDWORD nDataType)
{
	nuBOOL bres = nuTRUE;
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuTCHAR tmp[10] = {0};
	if( (nDataType & RES_DATA_DAYNIGHT_CHANGE) || (nDataType & RES_DATA_3DMODE_CHANGE) || g_nBgPic != g_pGlobalEngineData->uiSetData.nBgPic)
	{
		g_nBgPic = g_pGlobalEngineData->uiSetData.nBgPic;
		nuTcscpy(sFile, g_pFileNameMgr->GetAppPath());
		nuTcscat(sFile, _PIC_PAHT);
		nuTcscat(sFile, _BMP_BG);
		nuMemset(tmp,0,sizeof(tmp));
		nuItot(g_pGlobalEngineData->uiSetData.nBgPic, tmp, 10);
		nuTcscat(sFile, tmp);
		if( g_pGlobalEngineData->uiSetData.bNightOrDay )
		{
			nuTcscat(sFile, nuTEXT("_n")); //_BMP_NIGHT
		}
		else
		{
			nuTcscat(sFile, nuTEXT("_d")); //_BMP_DAY
		}
		if( g_pGlobalEngineData->uiSetData.b3DMode )
		{
			nuTcscat(sFile, nuTEXT("_3d.bmp"));
		}
		else
		{
			nuTcscat(sFile, nuTEXT("_2d.bmp"));
		}
		g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpBg);
		bres &= g_pFsGdiApi->GdiLoadNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpBg, sFile);
		//Load cross bg pic
		nuTcscpy(sFile, g_pFileNameMgr->GetAppPath());
		nuTcscat(sFile, _PIC_PAHT);
		nuTcscat(sFile, _BMP_CROSS);
		if( g_pGlobalEngineData->uiSetData.bNightOrDay )
		{
			nuTcscat(sFile, _BMP_NIGHT);
		}
		else
		{
			nuTcscat(sFile, _BMP_DAY);
		}
		g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpCrossBg);
		bres &= g_pFsGdiApi->GdiLoadNuroBMP(&g_drawInfo.drawInfoCommon.nuBmpCrossBg, sFile);
		//Load 3D sky pic
		nuTcscpy(sFile, g_pFileNameMgr->GetAppPath());
		if( g_pGlobalEngineData->uiSetData.bNightOrDay )
		{
			nuTcscat(sFile, FILE_3D_BMP_N);
		}
		else
		{
			nuTcscat(sFile, FILE_3D_BMP_D);
		}
		g_pFsGdiApi->GdiDelNuroBMP(&g_drawInfo.drawInfoCommon.nuBmp3D);
		bres &= g_pFsGdiApi->GdiLoadNuroBMP(&g_drawInfo.drawInfoCommon.nuBmp3D, sFile);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sky__bmpWidth %d", g_drawInfo.drawInfoCommon.nuBmp3D.bmpWidth);
	}
	if( nDataType & RES_DATA_SCALE_CHANGE )
	{
	}
	if( nDataType & RES_DATA_3DMODE_CHANGE )
	{
	}
	return bres;
}

FILESYSTEM_API nuPVOID LibFS_GetData(nuBYTE byDataType)
{
	switch( byDataType )
	{
		case DATA_USERCONFIG:
			return &g_pUserCfg->m_userCfg;
		case DATA_MAPCONFIG:
			return g_pMapCfg->m_pMapConfig;
		case DATA_GLOBAL:
			return g_pGlobalEngineData;
		case DATA_DRAWINFO:
			return &g_drawInfo;
		case DATA_DRAWMEM:
			return &g_drawMemData;
		case DATA_SCREENTRANSF:
			return &g_screenTransf;
		case DATA_TRACEMEM:
			return &g_traceMem;
		case DATA_OTHERGLOBAL:
			return &g_otherGlobal;
		case DATA_EEYESPLAYED:
			return g_pNewCcdZK->m_pCcd2Played;
			break;
		case DATA_EFFECTIVEMAPDATA:
			return g_pFileNameMgr->GetEffectiveMapData();
		default:
			return NULL;
	}
	return NULL;
}

FILESYSTEM_API nuBOOL LibFS_GetFilePath(nuWORD nFileType, nuTCHAR *sWholePath, nuWORD len)
{
	return g_pFileNameMgr->GetFilePath(nFileType, sWholePath, len, g_pGlobalEngineData->uiSetData.nLanguage);
}
FILESYSTEM_API nuBOOL LibFS_FindFileWholePath(nuLONG nMapIdx, nuTCHAR *sWholePath, nuWORD num)
{
	return g_pFileNameMgr->FindFileWholePath(nMapIdx, sWholePath, num);
}
FILESYSTEM_API nuWORD LibFS_GetMapCount()
{
	return g_pFileNameMgr->GetMapCount();
}
FILESYSTEM_API nuBOOL LibFS_PtInBoundary(nuroPOINT pt)
{
	return g_pFileFhZ->PtInBoundary(pt);
//	return g_pBlockDw->m_fileFB.PtInBoundary(pt);
}
FILESYSTEM_API nuBOOL LibFS_LoadBlock(nuPVOID lpVoid)
{
	if( g_pGlobalEngineData == NULL )
	{
		return nuFALSE;
	}
	BLOCKIDARRAY blockIDMap = g_pGlobalEngineData->transfData.blockIDShowArray;
	if( g_pGlobalEngineData->uiSetData.nBsdDrawMap )
	{
		nuroRECT rect;
		rect.left	= g_pGlobalEngineData->transfData.nMapCenterX - 100;
		rect.top	= g_pGlobalEngineData->transfData.nMapCenterY - 100;
		rect.right	= g_pGlobalEngineData->transfData.nMapCenterX + 100;
		rect.bottom = g_pGlobalEngineData->transfData.nMapCenterY + 100;
		nuRectToBlockIDArray(rect, &blockIDMap);
//		blockIDMap = g_pGlobalEngineData->zoomScreenData.blockIDZoomArray;
	}
	nuBOOL bres = g_pBlockDw->LoadBlocks(blockIDMap, g_pGlobalEngineData->zoomScreenData.blockIDZoomArray);
	return bres;
}
FILESYSTEM_API nuPVOID LibFS_GetScreenMap()
{
	return g_pBlockDw->GetScreenMap();
}
FILESYSTEM_API nuBOOL LibFS_GetRoadName(nuWORD nDwIdx, nuLONG nNameAddr, nuINT nRoadClass, nuPVOID pNameBuf, nuWORD nBufLen, nuPVOID pRnEx)
{
	return g_pFileRn->GetRoadName(nDwIdx, nNameAddr, nRoadClass, pNameBuf, nBufLen, (PRNEXTEND)pRnEx);
}

FILESYSTEM_API nuBOOL LibFS_GetRoadIconInfo(nuWORD nDwIdx, nuLONG nScaleValue, nuLONG nBGLStartScale, nuPVOID pRnEx, nuWORD *pRoadIconInfo)
{
	return g_pFileRni->GetRoadIconInfo(nDwIdx, nScaleValue, nBGLStartScale, (PRNEXTEND)pRnEx, pRoadIconInfo);
}

FILESYSTEM_API nuBOOL LibFS_GetPoiName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuPWORD pBufLen)
{
	return g_pFilePn->GetPoiName(nDwIdx, nNameAddr, pNameBuf, pBufLen);
}

FILESYSTEM_API nuBOOL LibFS_SavePtFile(nuINT DwIdx)
{
	return g_pFilePt->SavePtFile(DwIdx);
}

FILESYSTEM_API nuBOOL LibFS_SavePtFileCoverForDFT(nuINT DwIdx)
{
	return g_pFilePt->SavePtFileCoverForDFT(DwIdx);
}

FILESYSTEM_API const nuPVOID LibFS_GetPtDataAddr(nuLONG nDWIdx, nuLONG* pCount)
{
	return g_pFilePt->GetPtDataAddr(nDWIdx, pCount);
}

FILESYSTEM_API const nuPVOID LibFS_GetPtNodeByType(nuLONG nDWIdx, nuBYTE ID1, nuBYTE ID2)
{
	return g_pFilePt->GetPtNodeByType(nDWIdx, ID1, ID2);
}

FILESYSTEM_API nuBOOL LibFS_GetInfoBySourceType(nuLONG nSTypeID1, nuLONG nSTypeID2, nuLONG* pTypeID1, nuLONG* pTypeID2, nuLONG* pDisPlay, nuLONG* pIcon)
{
	return g_pFilePt->GetInfoBySourceType(nSTypeID1, nSTypeID2, pTypeID1, pTypeID2, pDisPlay, pIcon);
}
FILESYSTEM_API nuBOOL LibFS_GetInfoByNuroType(nuLONG nTypeID1, nuLONG nTypeID2, nuLONG* pSTypeID1, nuLONG* pSTypeID2, nuLONG* pDisPlay, nuLONG* pIcon)
{
	return g_pFilePt->GetInfoByNuroType(nTypeID1, nTypeID2, pSTypeID1, pSTypeID2, pDisPlay, pIcon);
}
FILESYSTEM_API nuINT LibFS_ReadConstWcs(nuBYTE nWcsType, nuLONG nIndex, nuWCHAR* pBuf, nuWORD nLen)
{
	return g_pReadWcs->ReadConstWcs(nWcsType, nIndex, pBuf, nLen);
}
FILESYSTEM_API nuINT LibFS_ReadWcsFromFile(nuTCHAR *tsFile, nuDWORD nLineIdx, nuWCHAR* pBuf, nuWORD nLen)
{
	return g_pReadWcs->ReadConstTxt(nLineIdx, pBuf, nLen, tsFile);
}

FILESYSTEM_API nuBOOL LibFS_ReadInfoMapCenterName()
{
	if( g_pGlobalEngineData->drawInfoMap.nCenterNameType == CENTER_NAME_NO_NEW )
	{
		return nuFALSE;
	}
	if( g_pGlobalEngineData->drawInfoMap.nCenterNameType == CENTER_NAME_ROAD || 
		g_pGlobalEngineData->drawInfoMap.nCenterNameType == CENTER_NAME_ROAD_NEAR  )
	{
		RNEXTEND rnEx = {0};
		CFileCb::ReadCityTownName(g_pGlobalEngineData->drawInfoMap.roadCenterOn.nDwIdx,
								  nuMAKELONG(g_pGlobalEngineData->drawInfoMap.roadCenterOn.nTownID, g_pGlobalEngineData->drawInfoMap.roadCenterOn.nCityID),
								  g_pGlobalEngineData->drawInfoMap.wsCityTown,
								  MAX_CITYTOWN_NAMNUM);
		return g_pFileRn->GetRoadName( g_pGlobalEngineData->drawInfoMap.roadCenterOn.nDwIdx,
									 g_pGlobalEngineData->drawInfoMap.roadCenterOn.nNameAddr,
									 g_pGlobalEngineData->drawInfoMap.roadCenterOn.nRoadClass,
									 g_pGlobalEngineData->drawInfoMap.wsCenterName,
									 sizeof(g_pGlobalEngineData->drawInfoMap.wsCenterName),
									 &rnEx );
	}
	else if( g_pGlobalEngineData->drawInfoMap.nCenterNameType == CENTER_NAME_POI )
	{
		nuWORD nPoiNamLen = sizeof(nuWCHAR)*MAX_NAMESTR_LEN;
		CFileCb::ReadCityTownName( g_pGlobalEngineData->drawInfoMap.poiCenterOn.nMapIdx,
								   nuMAKELONG(g_pGlobalEngineData->drawInfoMap.poiCenterOn.nTownID, g_pGlobalEngineData->drawInfoMap.poiCenterOn.nCityID),
								   g_pGlobalEngineData->drawInfoMap.wsCityTown,
								   MAX_CITYTOWN_NAMNUM);
		return CFilePn::GetPoiName( g_pGlobalEngineData->drawInfoMap.poiCenterOn.nMapIdx,
									g_pGlobalEngineData->drawInfoMap.poiCenterOn.nNameAddr,
									g_pGlobalEngineData->drawInfoMap.wsCenterPoi,
									&nPoiNamLen );
	}
	else if( g_pGlobalEngineData->drawInfoMap.nCenterNameType == CENTER_NAME_CITYTOWN )
	{
		CFileCb::ReadCityTownName( g_pGlobalEngineData->drawInfoMap.nCenterMapIdx,
									  nuMAKELONG(g_pGlobalEngineData->drawInfoMap.nCenterTownID, g_pGlobalEngineData->drawInfoMap.nCenterCityID),
									  g_pGlobalEngineData->drawInfoMap.wsCityTown,
									  MAX_CITYTOWN_NAMNUM);
	}
	return nuTRUE;
}

FILESYSTEM_API nuPVOID LibFS_GetCCDData(const nuroRECT& rtRange)
{
	return NULL;//g_fileCCD.GetDataCCD(rtRange, g_pGlobalEngineData->drawInfoMap.nCenterMapIdx);
}
FILESYSTEM_API nuUINT LibFS_GetCCDName(nuLONG nMapIdx, nuDWORD nNameAddr, nuPVOID pBuff, nuUINT nBuffLen)
{
	return 0;//g_fileCCD.GetCCDName((nuWORD)nMapIdx, nNameAddr, (nuWCHAR*)pBuff, nBuffLen);
}
FILESYSTEM_API nuBOOL LibFS_SeekForRoad(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDROADDATA pRoadData)
{
	return g_pBlockDw->SeekForNaviPt(x, y, nCoverDis, nAngle, pRoadData);
}

FILESYSTEM_API nuBOOL LibFS_PaddingRoad(PSEEKEDROADDATA pRoadData)
{
	return g_pBlockDw->PaddingRoad(pRoadData);
}

FILESYSTEM_API nuBOOL LibFS_SeekForPoi(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDPOIDATA pPoiData)
{
	return g_pBlockDw->SeekForPoi(x, y, nCoverDis, nAngle, pPoiData);
}
FILESYSTEM_API nuBOOL LibFS_SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, nuBOOL bSetMarket)
{
	return g_pBlockDw->SeekForRoads(pSeekRdsIn, pSeekRdsOut, bSetMarket);
}

FILESYSTEM_API nuBOOL LibFS_SaveLastPos(nuPVOID lpVoid)
{
	while(bSavingLastPosition)//added by daniel 20111111
	{
		nuSleep(10);
	}
	bSavingLastPosition = nuTRUE;
	nuBOOL bRes = CReadLastPos::SaveLastGps(g_pGlobalEngineData->pTsPath, lpVoid);
	bSavingLastPosition = nuFALSE;
	return bRes; 
}

FILESYSTEM_API nuBOOL LibFS_ReadCityTownName(nuLONG nMapIdx, nuDWORD nCityTownCode, nuWCHAR *wsName, nuWORD nWsNum)
{
	return CFileCb::ReadCityTownName(nMapIdx, nCityTownCode, wsName, nWsNum);
}

FILESYSTEM_API nuBOOL LibFS_ReadPaData(nuLONG nMapIdx, nuDWORD nAddress, PPOIPANODE pBuff)
{
	return CFilePa::GetPa(nMapIdx, nAddress, pBuff);
}

FILESYSTEM_API nuLONG LibFS_MapIDIndex(nuLONG nId, nuBYTE nType)
{
	return g_pFileNameMgr->MapIDIndex(nId, nType);
}

FILESYSTEM_API nuPVOID LibFS_LoadDwBlock(const nuroRECT& rtMap, nuDWORD nDataMode)
{
//	if( nDataMode & DW_DATA_MODE_SMALL )//for extend
	{
		return g_pBlkDwCtrl->LoadDwDataSmall(rtMap, nDataMode);
	}
}
FILESYSTEM_API nuVOID LibFS_FreeDwBlock(nuDWORD nDataMode)
{
//	if( nDataMode & DW_DATA_MODE_SMALL )//for extend
	{
		g_pBlkDwCtrl->FreeDwDataSmall();
	}
}
FILESYSTEM_API nuBOOL LibFS_CheckEEyes(nuroPOINT point, nuLONG nDis, short nAngle, PROADFOREEYES pRdForEEyes)
{
	if( g_pCheckEEyesZ )
	{
		return g_pCheckEEyesZ->CheckEEyes(point, nDis,nAngle, pRdForEEyes);
	}
	else
	{
		return nuFALSE;
	}
}
FILESYSTEM_API nuBOOL LibFS_GetRoadSegData(nuWORD nDwIdx, PROAD_SEG_DATA pRoadSegData)
{
	CMapFileRdwZ fileRdw;
	return fileRdw.GetRoadSegData(nDwIdx, pRoadSegData);
}

//For Search API
/*
FILESYSTEM_API nuUINT LibFS_SHDYStart()
{
	if( g_pSearchDY == NULL )
	{
		g_pSearchDY = new CSearchDanyinZ();
		if( g_pSearchDY == NULL )
		{
			return 0;
		}
		if( !g_pSearchDY->Initialize() )
		{
			delete g_pSearchDY;
			g_pSearchDY = NULL;
			return 0;
		}
	}
	return 1;
}
FILESYSTEM_API nuUINT LibFS_SHDYStop()
{
	if( g_pSearchDY != NULL )
	{
		g_pSearchDY->Free();
		delete g_pSearchDY;
		g_pSearchDY = NULL;
		return 1;
	}
	return 0;
}
FILESYSTEM_API nuUINT LibFS_SHDYSetRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchRegion(nSetMode, nData);
}
FILESYSTEM_API nuUINT LibFS_SHDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchDanyin(pWsDy, nWsNum);
}
FILESYSTEM_API nuUINT LibFS_SHDYGetHead(PDYCOMMONFORUI pDyHead)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataHead(pDyHead);
}
FILESYSTEM_API nuUINT LibFS_SHDYGetPageData(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataPage(pDyDataForUI, nItemIdx);
}
FILESYSTEM_API nuUINT LibFS_SHDYGetPageStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataStroke(pDyDataForUI, nStroke);
}
FILESYSTEM_API nuUINT LibFS_SHDYGetOneDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDyDataDetail(pDyDetailForUI, nIdx);
}
FILESYSTEM_API nuUINT LibFS_SHDYCityTownName(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDyCityTownName(pDyCTname, nCityID);
}
*/


FILESYSTEM_API nuVOID LibFS_RDNameProc(nuWCHAR *pWsName, nuWORD nLen)
{
	g_pFileRn->RoadNameProc(pWsName, nLen, NULL);
}
FILESYSTEM_API nuPVOID LibFS_GetMpnData()
{
	return g_pFileNameMgr->GetEffectiveMapData();
}
FILESYSTEM_API nuUINT LibFS_SetWaypoints(nuWORD idx, nuLONG x, nuLONG y, nuWCHAR* pwsName)
{
#ifdef _USE_WAYPOINTS_MGRZ
	short nAnge = -1;
	if( idx == 0 )
	{
		nAnge = g_pGlobalEngineData->carInfo.nCarAngle;
	}
	return g_pWaypointsMgrZ->SetWaypoints(idx, x, y, pwsName, nAnge);
#endif 
	return 0;
}
FILESYSTEM_API nuUINT LibFS_DelWaypoints(nuWORD idx)
{
#ifdef _USE_WAYPOINTS_MGRZ
	return g_pWaypointsMgrZ->DltWaypoints(idx, g_pGlobalEngineData->carInfo.nCarAngle);
#endif
	return 0;
}
FILESYSTEM_API nuUINT LibFS_GetWaypoints(PPARAM_GETWAYPOINTS pParam)
{
#ifdef _USE_WAYPOINTS_MGRZ
	return g_pWaypointsMgrZ->GetWaypoints(pParam);
#endif
	return 0;
}
FILESYSTEM_API nuUINT LibFS_KptWaypoints(nuLONG x, nuLONG y)
{
#ifdef _USE_WAYPOINTS_MGRZ
	return g_pWaypointsMgrZ->KptWaypoints(x, y);
#endif
	return (-1);
}

FILESYSTEM_API nuUINT LibFS_SeekForCcdInfo(nuroRECT rtMap, PCCDINFO pCcdInfo)
{
	return g_pBlkDwCtrl->SeekForCcdInfo(rtMap, pCcdInfo);
}

FILESYSTEM_API nuUINT LibFS_CheckNewCcd(PCCDINFO pCcdInfo)
{
	return g_pNewCcdZK->CheckCcd2(pCcdInfo);
}
FILESYSTEM_API nuUINT LibFS_FS_Get_TMC_VD_SIZE(nuCHAR *sTime)
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
//	return g_pTMCData->Get_TMC_VD_SIZE();
    return g_pTMCData->GetVDCount(sTime);
	
}
FILESYSTEM_API nuBOOL LibFS_Get_TMC_FM_XML_SIZE(nuUINT& nTotal_Count,nuUINT* nFm_Count,nuUINT& nXML_Count,nuUINT* nXML_Complex_Count)
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	return g_pTMCData->Get_TMC_FM_XML_SIZE(nTotal_Count, nFm_Count,nXML_Count,nXML_Complex_Count);
}
FILESYSTEM_API nuBOOL LibFS_FS_Get_TMC_VD(nuVOID*pTmcBuf,const nuUINT size)
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	return g_pTMCData->GetVD(pTmcBuf,size);
}
FILESYSTEM_API nuBOOL LibFS_Get_TMC_FM_XML(nuVOID*pTmcBuf,const nuUINT size)
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	
	return 0;//g_pTMCData->Get_TMC_FM_XML((nuBYTE*)pTmcBuf,size);
}
FILESYSTEM_API nuBOOL LibFS_Get_TMC_FM(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode) //Prosper
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	return g_pTMCData->Get_TMC_XML_L((nuBYTE*)pTmcBuf,size,nmode);

}
FILESYSTEM_API nuBOOL LibFS_Get_TMC_XML(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode) //Prosper 08.26
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	
	return g_pTMCData->Get_TMC_XML_FM((nuBYTE*)pTmcBuf,size,nmode);
	
}
FILESYSTEM_API nuBOOL LibFS_Get_TMC_XML_L(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode) //Prosper 08
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
		return g_pTMCData->Get_TMC_VD((nuBYTE*)pTmcBuf,size,nmode);
}
FILESYSTEM_API nuBOOL LibFS_Set_TMC_Byte(nuVOID*pTmcBuf) //Prosper 07.25
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	return g_pTMCData->Set_TMC_Byte((nuBYTE*)pTmcBuf);//(nuBYTE*)pTmcBuf
}


FILESYSTEM_API nuBOOL LibFS_Get_TMC_Router(nuVOID*pTmcBuf,const nuUINT size) //Prosper 08
{
    if(NULL == g_pTMCData)
    {
        return 0;
    }
	return g_pTMCData->Get_TMC_Router((nuBYTE*)pTmcBuf,size);
}

FILESYSTEM_API nuBOOL  LibFS_ChangePoiInfo(/*InPut*/NUROPOINT PoiCoor ,/*output*/POI_Info_Data& PoiData)//Shyanx 2012.02
{
	return g_pMpS->ChangePoiInfo(/*InPut*/PoiCoor ,/*output*/PoiData);
}
/*
FILESYSTEM_API nuUINT LibFS_LookForNaviRoad(PLOOKFOR_NAVI_ROAD_IN pNaviIn, PSEEKEDROADDATA pRoadData)
{
	return 1;
}
*/
FILESYSTEM_API KMInfoData *LibFS_GetKMDataDL(nuLONG BlockIdx, /*output*/nuLONG &Counter)
{
	return  g_pKMDL->GetKMDataDL(BlockIdx, Counter);
}
FILESYSTEM_API KMInfoData *LibFS_GetKMDataMapping(const SEEKEDROADDATA &RoadData, nuLONG &Counter)
{
	return  g_pKMDL->GetKMDataMapping(RoadData, Counter);
}
FILESYSTEM_API nuUINT LibFS_SeekForKMInfo(nuroRECT rtMap, PKMINFO pKMInfo)
{
	return g_pBlkDwCtrl->SeekForKMInfo(rtMap, pKMInfo);
}
FILESYSTEM_API nuBOOL LibFS_CheckKMInfo(PKMINFO pKMInfo, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName)
{
	return g_pKMDL->CheckKMInfo(pKMInfo, wsKilometer, lKMNum, lKMDis, wsKMName);
}
FILESYSTEM_API nuUINT LibFS_GetTMCTrafficInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID)
{
	return g_pTMCService->GetTMCTrafficInfo(nDWIdx, BlockID, RoadID);
}
FILESYSTEM_API nuUINT LibFS_GetTMCTrafficInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2)
{
	return g_pTMCService->GetTMCTrafficInfoByNodeID(nDWIdx, NodeID1, NodeID2);
}
FILESYSTEM_API nuUINT LibFS_GetTMCDirInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID)
{
	return g_pTMCService->GetTMCDirInfo(nDWIdx, BlockID, RoadID);
}
FILESYSTEM_API nuUINT LibFS_GetTMCDirInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2)
{
	return g_pTMCService->GetTMCDirInfoByNodeID(nDWIdx, NodeID1, NodeID2);
}
FILESYSTEM_API nuBOOL LibFS_COPY_TMC_Event_Data(nuBYTE nDWIdx)
{
	return g_pTMCService->Copy_TMC_Event_Data(nDWIdx);
}

FILESYSTEM_API TMCRoadInfoStru* LibFS_GetTMCDataInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID)
{
	return g_pTMCService->GetTMCDataInfo(nDWIdx, BlockID, RoadID);
}
FILESYSTEM_API nuBOOL LibFS_Initial_TMC_Event_Data(nuBYTE nDWIdx)
{
    return g_pTMCService->Initial_TMC_Event_Data(nDWIdx);
}
FILESYSTEM_API nuVOID LibFS_Release_TMC_Event_Data(nuBYTE nDWIdx)
{
	return;// g_pTMCService->Release_TMC_Event_Data(nDWIdx);
}

FILESYSTEM_API nuBOOL LibFS_Initial_TMC_Data(nuINT nCount)
{
	return g_pTMCService->Initial_TMC_Data(nCount);
}
FILESYSTEM_API nuVOID LibFS_Release_TMC_Data()
{
	g_pTMCService->Release_TMC_Data();
}
