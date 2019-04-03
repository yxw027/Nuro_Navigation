
#include "libDrawMap.h"
#include "NuroClasses.h"

#include "DrawBlock.h"
#include "RoadName.h"
#include "PoiName.h"
#include "CCDCtrl.h"
#include "FileMapBaseZ.h"
#include "DrawMdwZ.h"
#include "DrawMgrZ.h"
#include "DrawBglZ.h"
#include "DrawSeaZ.h"
#include "DrawCarXQ.h"
#include "DrawKilometerDL.h"//added by danile 20121205

#define MAJOR_POI_NEW

#ifndef MAJOR_POI_NEW
#include "DrawMpoiZ.h"
#else
#include "DrawMajorPoiZK.h"
#endif

#ifdef NURO_USE_LOGFILE
#include "LogFileCtrlZ.h"
#endif

#ifdef NURO_OS_WINDOWS
#define _USE_OPENGLES
#endif

#include <android/log.h>
#ifdef _USE_OPENGLES
#include "Draw3DModelZK.h"
class CDraw3DModelZK		g_draw3DMdlZK;
static  nuBOOL              g_bDrw3DModels = nuFALSE;
#endif

class CDrawBlock*			g_pDrawBlock = NULL;
class CRoadName*			g_pRoadName	= NULL;
class CPoiName*				g_pPoiName	= NULL;
//class CPoiName*				g_pPoiZoom	= NULL;
//class CCCDCtrl*				g_pCcdCtrl	= NULL;
//class CReadBsd*				g_pReadBsd	= NULL;
class CDrawMdwZ*			g_pDrawMdwZ	= NULL;
class CDrawMgrZ*			g_pDrawMgrZ = NULL;
class CDrawBglZ*			g_pDrawBglZ = NULL;
class CDrawSeaZ*			g_pDrawSeaZ = NULL;
class CDrawKilometerDL*		g_pDrawKilometerDL = NULL;//added by danile 20121205
class CDrawCarXQ*			g_pDrawCarXQ = NULL;
#ifndef MAJOR_POI_NEW
class CDrawMpoiZ*			g_pDrawMpoiZ = NULL;
#else
class CDrawMajorPoiZK*		g_pDrawMajorPoiZK	= NULL;
#endif

PGDIAPI         g_pDMLibGDI = NULL;
PMEMMGRAPI      g_pDMLibMM = NULL;
PFILESYSAPI     g_pDMLibFS = NULL;
PMATHTOOLAPI    g_pDMLibMT = NULL;
//PINNAVIAPI		g_pDMLibIN = NULL;
PGLOBALENGINEDATA   g_pDMGdata = NULL;
PMAPCONFIG          g_pDMMapCfg = NULL;

#define DRAW_STEP_NODRAW			0x00
#define DRAW_STEP_BMP1				0x01
#define DRAW_STEP_BMP2				0x02
#define DRAW_STEP_DRAWINFO			0x04
#define DRAW_STEP_SHOWMAP			0x08
nuBYTE			g_nDrawStep = DRAW_STEP_NODRAW;
nuBYTE			g_bDrawLineToTarget = 0;
nuBYTE			g_nIconType = 1;
NURO_BMP 		g_RealPicBmpZoom = {0};
nuLONG			g_lUseTMC = 0;

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
        LibDM_FreeDrawMap();
        break;
    }
    return TRUE;
}*/
#endif
/* Drawmap's Api functions */
DRAWMAP_API nuBOOL LibDM_InitDrawMap(PAPISTRUCTADDR pApiAddr, POPENRSAPI pRsApi)
{
    g_pDrawBlock = new CDrawBlock();
    if( g_pDrawBlock == NULL )
    {
        return nuFALSE;
    }
    /*
    g_pCcdCtrl = new CCCDCtrl();
    if( g_pCcdCtrl == NULL )
    {
    return nuFALSE;
    }
    */
    g_pRoadName = new CRoadName();
    if( g_pRoadName == NULL )
    {
        return nuFALSE;
    }
    g_pPoiName = new CPoiName();
    if( g_pPoiName == NULL )
    {
        return nuFALSE;
    }
    /*
    g_pPoiZoom	= new CPoiName();
    if( g_pPoiName == NULL )
    {
    return nuFALSE;
    }
    g_pReadBsd	= new CReadBsd();
    if( g_pPoiName == NULL )
    {
    return nuFALSE;
    }
    */
    
    g_pDrawMdwZ = new CDrawMdwZ();
    if( !g_pDrawMdwZ )
    {
        return nuFALSE;
    }
    g_pDrawMgrZ = new CDrawMgrZ();
    if( g_pDrawMgrZ == NULL )
    {
        return nuFALSE;
    }
    g_pDrawBglZ = new CDrawBglZ();
    if( g_pDrawBglZ == NULL )
    {
        return nuFALSE;
    }
    g_pDrawSeaZ = new CDrawSeaZ();
    if( g_pDrawSeaZ == NULL )
    {
        return nuFALSE;
    }
    g_pDrawCarXQ = new CDrawCarXQ();
    if(g_pDrawCarXQ == NULL)
    {
	return nuFALSE;
    }
#ifndef MAJOR_POI_NEW
    g_pDrawMpoiZ = new CDrawMpoiZ();
    if( g_pDrawMpoiZ == NULL )
    {
        return nuFALSE;
    }
#else
    g_pDrawMajorPoiZK = new CDrawMajorPoiZK();
    if( NULL == g_pDrawMajorPoiZK )
    {
        return nuFALSE;
    }
#endif
	g_pDrawKilometerDL = new CDrawKilometerDL;
	if(g_pDrawKilometerDL == NULL)
	{
		return nuFALSE;
	}
    g_pDMLibMM	= pApiAddr->pMmApi;
    g_pDMLibFS	= pApiAddr->pFsApi;
    g_pDMLibMT	= pApiAddr->pMtApi;
    g_pDMLibGDI = pApiAddr->pGdiApi;
    //	g_pDMLibIN = pApiAddr->pInApi;
    
    g_pDMGdata = g_pDMLibFS->pGdata;
    g_pDMMapCfg = g_pDMLibFS->pMapCfg;
    
    CFileMapBaseZ::InitData(pApiAddr);
    if( g_pDMMapCfg == NULL )
    {
        return nuFALSE;
    }
    
    if( !g_pRoadName->Initialize(g_pDMMapCfg->commonSetting.nMaxRoadNameCount, g_pDMMapCfg->scaleSetting.pRoadSetting) )
    {
        return nuFALSE;
    }
    
    if( !g_pPoiName->Initialize(g_pRoadName, g_pDMMapCfg->scaleSetting.pPoiSetting) )
    {
        return nuFALSE;
    }
    /*
    if( !g_pPoiZoom->Initialize(NULL, g_pDMMapCfg->zoomSetting.pPoiSetting, MAP_ZOOM) )
    {
    return nuFALSE;
    }
    */
    if( !g_pDrawBlock->Initialize() )
    {
        return nuFALSE;
    }
    /*
    if( !g_pCcdCtrl->Initialize() )
    {
    return nuFALSE;
    }
    */
    /*
    if( !g_pReadBsd->Init() )
    {
    return nuFALSE;
    }
    */
    
    if( !g_pDrawMdwZ->initialize() )
    {
        return nuFALSE;
    }
    if( !g_pDrawMgrZ->Initialize() )
    {
        return nuFALSE;
    }
    if( !g_pDrawBglZ->Initialize() )
    {
        return nuFALSE;
    }
    if( !g_pDrawSeaZ->Initialize() )
    {
        return nuFALSE;
    }
    if(!g_pDrawCarXQ->Initialize())
    {
	return nuFALSE;
    }
#ifndef MAJOR_POI_NEW
    if( !g_pDrawMpoiZ->Initialize() )
    {
        return nuFALSE;
    }
#else
    if( !g_pDrawMajorPoiZK->Initialize() )
    {
        return nuFALSE;
    }
#endif
	if( g_pDrawKilometerDL != NULL )
	{
		g_pDrawKilometerDL->Free();
		delete g_pDrawKilometerDL;
		g_pDrawKilometerDL = NULL;
	}
    //
    pApiAddr->pDmApi->DM_LoadMap		= LibDM_LoadMap;
    pApiAddr->pDmApi->DM_DrawMapBmp1	= LibDM_DrawMapBmp1;
    pApiAddr->pDmApi->DM_DrawMapBmp2	= LibDM_DrawMapBmp2;
    pApiAddr->pDmApi->DM_DrawZoomRoad	= LibDM_DrawZoomRoad;
    pApiAddr->pDmApi->DM_DrawZoomFake	= LibDM_DrawZoomFake;
    pApiAddr->pDmApi->DM_DrawFlag		= LibDM_DrawFlag;
    pApiAddr->pDmApi->DM_DrawRawCar		= LibDM_DrawRawCar;
    pApiAddr->pDmApi->DM_SeekForRoad	= LibDM_SeekForRoad;
    pApiAddr->pDmApi->DM_SeekForPOI		= LibDM_SeekForPOI;
    pApiAddr->pDmApi->DM_CarToRoad		= LibDM_CarToRoad;
    //	pApiAddr->pDmApi->DM_StartTravel	= LibDM_StartTravel;
    //	pApiAddr->pDmApi->DM_PendTravel		= LibDM_PendTravel;
    //	pApiAddr->pDmApi->DM_StopTravel		= LibDM_StopTravel;
    pApiAddr->pDmApi->DM_DrawMap		= LibDM_DrawMap;
    pApiAddr->pDmApi->DM_FreeDataMem	= LibDM_FreeDataMem;
    pApiAddr->pDmApi->DM_StartOpenGL	= LibDM_StartOpenGL;
    pApiAddr->pDmApi->DM_Reset3DModel   = LibDM_Reset3DModel;
    pApiAddr->pDmApi->DM_GetNearKilo    = LibDM_GetNearKilo; //added by daniel 20121205
    pApiAddr->pDmApi->DM_FreeDrawData   = LibDM_FreeDrawData;
//-----------------------------SDK---------------------------//
    pApiAddr->pDmApi->DM_DrawMapCar     = LibDM_DrawMapCar;
    pApiAddr->pDmApi->DM_DrawMapPic	= LibDM_DrawMapPic;
    pApiAddr->pDmApi->DM_CheckSkyBmp    = LibDM_CheckSkyBmp;
//-----------------------------SDK---------------------------//
    g_nDrawStep = DRAW_STEP_NODRAW;
    
    nuLONG nTmp = 0;
    if( !nuReadConfigValue("LINETOTARGET", &nTmp) )
    {
        nTmp = 0;
    }
    g_bDrawLineToTarget = (nuBYTE)nTmp;
	if(!nuReadConfigValue("USETMC",&g_lUseTMC))
	{
		g_lUseTMC = 0;
	}
#ifdef _USE_OPENGLES
    __android_log_print(ANDROID_LOG_INFO, "navi", "libNC InitDrawMap OpenGL pRsApi %x", pRsApi);
    g_draw3DMdlZK.Inialize(g_pDMGdata->pTsPath, pRsApi, g_pDMLibMT);
#endif
    nuMemset(&g_RealPicBmpZoom, 0, sizeof(NURO_BMP));
    return nuTRUE;
}
DRAWMAP_API nuVOID LibDM_FreeDrawMap()
{
#ifndef MAJOR_POI_NEW
    if( g_pDrawMpoiZ != NULL )
    {
        g_pDrawMpoiZ->Free();
        delete g_pDrawMpoiZ;
        g_pDrawMpoiZ = NULL;
    }
#else
    if( NULL != g_pDrawMajorPoiZK )
    {
        g_pDrawMajorPoiZK->Free();
        delete g_pDrawMajorPoiZK;
        g_pDrawMajorPoiZK = NULL;
    }
#endif
    if( g_pDrawSeaZ != NULL )
    {
        g_pDrawSeaZ->Free();
        delete g_pDrawSeaZ;
        g_pDrawSeaZ = NULL;
    }
    if( g_pDrawBglZ != NULL )
    {
        g_pDrawBglZ->Free();
        delete g_pDrawBglZ;
        g_pDrawBglZ = NULL;
    }
    if( g_pDrawMgrZ != NULL )
    {

        g_pDrawMgrZ->Free();
        delete g_pDrawMgrZ;
        g_pDrawMgrZ = NULL;
    }
    if( g_pDrawMdwZ != NULL )
    {
        g_pDrawMdwZ->free();
        delete g_pDrawMdwZ;
        g_pDrawMdwZ	= NULL;
    }
    
    /*
    if( g_pReadBsd != NULL )
    {
    g_pReadBsd->Free();
    delete g_pReadBsd;
    g_pReadBsd = NULL;
    }
    
      if( g_pCcdCtrl != NULL )
      {
      g_pCcdCtrl->Free();
      delete g_pCcdCtrl;
      g_pCcdCtrl = NULL;
}*/
    if( g_pDrawBlock != NULL )
    {
        g_pDrawBlock->Free();
        delete g_pDrawBlock;
        g_pDrawBlock = NULL;
    }
    /*
    if( g_pPoiZoom != NULL )
    {
    g_pPoiZoom->Free();
    delete g_pPoiZoom;
    g_pPoiZoom = NULL;
    }
    */
    if( g_pPoiName != NULL )
    {
        g_pPoiName->Free();
        delete g_pPoiName;
        g_pPoiName = NULL;
    }
    if( g_pRoadName != NULL )
    {
        g_pRoadName->Free();
        delete g_pRoadName;
        g_pRoadName = NULL;
    }
//---------------SDK---------------------------//
    if(g_pDrawCarXQ != NULL)
    {
	delete g_pDrawCarXQ;
	g_pDrawCarXQ = NULL;
    }
    if(g_RealPicBmpZoom.pBmpBuff != NULL)
    {
	delete [] g_RealPicBmpZoom.pBmpBuff;
	g_RealPicBmpZoom.pBmpBuff = NULL;
    }
//---------------SDK---------------------------//
}
DRAWMAP_API nuBOOL LibDM_LoadMap(nuPVOID pVoid)
{
    PACTIONSTATE pAcSt = (PACTIONSTATE)pVoid;
    g_pDrawMgrZ->AllocDrawMem();
    
#ifdef _USE_SEA_DRAW
    g_pDrawSeaZ->LoadData( g_pDMLibFS->pGdata->transfData.nuShowMapSize, 
        g_pDMMapCfg->scaleSetting.scaleTitle.nScale);
#endif
    
#ifndef MAJOR_POI_NEW
    g_pDrawMpoiZ->ReadMpoiData();
#else
    g_pDrawMajorPoiZK->ReadMajorPoi();
#endif
    if( g_pDMGdata->uiSetData.nBsdDrawMap )
    {
        if( g_pDMGdata->uiSetData.nBsdDrawMap == BSD_MODE_5k )
        {
#ifdef _USE_BGL_DRAW
            g_pDrawBglZ->FreeData();
#endif
        }
        else
        {
#ifdef _USE_BGL_DRAW
            g_pDrawBglZ->LoadData( g_pDMLibFS->pGdata->transfData.nuShowMapSize,
                g_pDMMapCfg->scaleSetting.scaleTitle.nScale);
#endif	
        }
    }
    else
    {
#ifdef _USE_BGL_DRAW
        g_pDrawBglZ->FreeData();
#endif
    }
    if( pAcSt->bLoad2kBlock )
    {
    /*
    if( !pAcSt->bNeedExtendRotate &&
    (pAcSt->nDrawMode == DRAWMODE_GPS || pAcSt->nDrawMode == DRAWMODE_SIM) && 
    (g_pDMLibFS->pUserCfg->nVoiceControl & VOICE_CONTROL_CCD) )
    {
    g_pCcdCtrl->LoadCCDData();//祥夔遙涴俴腔弇离善綴醱ㄛ埻秪岆
    }
        */
        g_pDMLibFS->FS_LoadBlock(pVoid);
        
    }
    //
#ifdef _USE_OPENGLES
    if ( g_pDMGdata->uiSetData.b3DMode && 
        !g_pDMGdata->uiSetData.b3DViewChanging && 
        g_pDMGdata->uiSetData.nScaleValue < 200 )
    {
        g_bDrw3DModels = nuTRUE;
    }
    else
    {
        g_bDrw3DModels = nuFALSE;
    }
    //
    if( g_bDrw3DModels )
    {
        g_draw3DMdlZK.Check3DModel( &g_pDMGdata->transfData.nuShowMapSize);
    }        
#endif
    return nuTRUE;
}
DRAWMAP_API nuBOOL LibDM_DrawMapBmp1(nuPVOID pVoid)
{
    PACTIONSTATE pAcSt = (PACTIONSTATE)pVoid;
    //	nuShowString(nuTEXT("DrawMapBmp1 1"));
    g_pDrawBlock->LoadScreenMap();
    //	nuShowString(nuTEXT("DrawMapBmp1 2"));
    if( !g_pDMGdata->uiSetData.nBsdDrawMap || pAcSt->bDrawZoom )
    {
        g_pDrawBlock->CollectScreenMap();
    }
    else
    {
    }
    //	nuShowString(nuTEXT("DrawMapBmp1 3"));
    //Begin drawing
    g_pDMLibMT->MT_SaveOrShowBmp(PAINT_NUROBMP);
    //g_pDMLibMT->MT_DrawObject(DRAW_OBJ_SQUARE, NULL, 0, 0);
    //////////////Log File////////////////////////////////////////////
#ifdef _USE_LOG_FILE_DRAWMAP									//
    CLogFileCtrlZ logFile;										//	
    logFile.OpenLogFile(nuTEXT("Draw.log"));					//
    logFile.OutString(nuTEXT("Draw Sea:"));						//
    logFile.SaveTicks();										//
#endif															//
    //////////////////////////////////////////////////////////////////
#ifdef _USE_SEA_DRAW
    g_pDrawSeaZ->DrawSeaZ();
    g_pDrawSeaZ->ColSeaData();
#endif
    //////////////Log File////////////////////////////////////////////
#ifdef _USE_LOG_FILE_DRAWMAP									//
    logFile.OutTicksPast();										//	
#endif															//
    //////////////////////////////////////////////////////////////////
    
    if( g_pDMGdata->uiSetData.nBsdDrawMap )
    {
        if( g_pDMGdata->uiSetData.nBsdDrawMap == BSD_MODE_5k )
        {
#ifdef _USE_MDW_DRAW
            g_pDrawMgrZ->CollectDrawMem();
            g_pDrawMdwZ->ColMdwData();
            g_pDrawMgrZ->DrawRoad(NULL);
#endif
        }
        else
        {
#ifdef _USE_BGL_DRAW
            g_pDrawMgrZ->CollectDrawMem();
            g_pDrawBglZ->ColBglData();
            g_pDrawMgrZ->DrawBgLine(NULL);
			g_pDrawBglZ->DrawBglRdIcon();
            //g_pDrawMgrZ->DrawRoad(NULL);
#endif
        }
    }
    else
    {
        //////////////Log File////////////////////////////////////////////
#ifdef _USE_LOG_FILE_DRAWMAP									//
        logFile.OutString(nuTEXT("Draw Road:"));				//
        logFile.SaveTicks();									//
#endif															//
        //////////////////////////////////////////////////////////////////
        g_pDrawBlock->DrawBgArea();
        g_pDrawBlock->DrawBgLine();
#ifndef	_USE_NEW_DRAWMAP

		if(!g_pDMLibFS->pUserCfg->nTMCSwitchFlag || 
			g_pDMLibFS->pUserCfg->bMoveMap ||
			0 == g_lUseTMC)
		{
		//normal draw road;
		g_pDrawBlock->DrawRoad(nuTRUE, pAcSt->bDrawRoadName);
		g_pDrawBlock->DrawRoad(nuFALSE);
		}
		//draw DTI road;draw rim;
		//g_pDrawBlock->DrawDTIRoad(nuTRUE, pAcSt->bDrawRoadName);
		//draw DTI road;draw center;
		else
		{
			g_pDrawBlock->DrawRoad(nuTRUE, pAcSt->bDrawRoadName);
			g_pDrawBlock->DrawDTIRoad(nuFALSE);
		}

#else
        g_pDrawMgrZ->CollectDrawMem();
        g_pDrawMgrZ->ColMapRoad(NULL);
        g_pDrawMgrZ->DrawRoad(NULL);
#endif
        //////////////Log File////////////////////////////////////////////
#ifdef _USE_LOG_FILE_DRAWMAP									//
        logFile.OutTicksPast();									//	
#endif															//
        //////////////////////////////////////////////////////////////////
    }
    /*
    if( !pAcSt->bNeedExtendRotate && 
    (pAcSt->nDrawMode == DRAWMODE_GPS || pAcSt->nDrawMode == DRAWMODE_SIM) && 
    (g_pDMLibFS->pUserCfg->nVoiceControl & VOICE_CONTROL_CCD) )
    {
    g_pCcdCtrl->CheckCCDData();
    }
    */
#ifndef MAJOR_POI_NEW
    g_pDrawMpoiZ->ColMpoiData(NULL);
#else
    g_pDrawMajorPoiZK->ColMajorPoi(NULL);
#endif 
    g_pDrawMgrZ->DrawTrace(NULL);
    return nuTRUE;
}
DRAWMAP_API nuBOOL LibDM_DrawMapBmp2(nuPVOID pVoid)
{
    PACTIONSTATE pAcSt = (PACTIONSTATE)pVoid;
    if( g_pDMGdata->uiSetData.nBsdDrawMap )
    {
        ;
    }
    else if( g_pDMLibFS->pUserCfg->nShowPOI)
    {
        if( pAcSt->bDrawPOIName )
        {
            g_pDrawBlock->CollectScreenPoi();
            g_pPoiName->DrawPoi();
        }
	else
	{
		g_pDrawBlock->CollectScreenPoi();
		g_pPoiName->DrawPoiIcon_DX();
	}
    }
    if( pAcSt->bDrawRoadName )
    {
        g_pRoadName->DrawRoadName();
    }
	if( g_pDMLibFS->pGdata->uiSetData.nScaleValue <= 400 )
	{
		g_pDrawMgrZ->DrawMemo(NULL);
	}

	//g_pDrawKilometerDL->DrawKilometers(1);//added by daniel

	g_pDrawMgrZ->DrawTrajectory(NULL); //Gps萸腔寢慫
    //	g_pCcdCtrl->DrawCCDPlayed();
    g_pDrawMgrZ->DrawEEyesPlayed(NULL);
    if( g_pDMGdata->uiSetData.b3DMode )
    {
        g_pDMLibMT->MT_SaveOrShowBmp(SHOW_3D_BMP);
    }
#ifdef _USE_OPENGLES
    if ( g_bDrw3DModels )
    {
        g_draw3DMdlZK.SetViewOption( g_pDMGdata->carInfo.nMapAngle + g_pDMGdata->carInfo.nRotateAngle, 
            g_pDMGdata->drawInfoMap.nScalePixels * 1.0f / g_pDMGdata->uiSetData.nScaleValue,//g_pDMMapCfg->scaleSetting.scaleTitle.nScale, 
            g_pDMLibMT,
            g_pDMGdata->transfData.nBmpWidth,
            g_pDMGdata->transfData.nBmpHeight );
        g_draw3DMdlZK.Draw3DLayer(0, 0, 200, 200, g_pDMLibGDI->GdiGetCanvasBmp());
    }
#endif
    LibDM_DrawFlag(pVoid);

	//////////////////////////////////////////////////////////////////////////
#ifdef VALUE_EMGRT
	// 机芞
	// MinX:119.95;MaxX:122.05;MinY:21.8;MaxY:25.45
	// 怢俜吽蝠籵杅擂訧蹋婃
#define TW_MinX ((long)(119.95 * 100000))
#define TW_MaxX ((long)(122.05 * 100000))
#define TW_MinY ((long)(21.8 * 100000))
#define TW_MaxY ((long)(25.45 * 100000))

#define BOSEA_MinX ((long)(119.5 * 100000))
#define BOSEA_MaxX ((long)(120.5 * 100000))
#define BOSEA_MinY ((long)(38.5 * 100000))
#define BOSEA_MaxY ((long)(39.5 * 100000))

#define YESEA_MinX ((long)(123 * 100000))
#define YESEA_MaxX ((long)(125 * 100000))
#define YESEA_MinY ((long)(34 * 100000))
#define YESEA_MaxY ((long)(36 * 100000))

#define EASTSEA_MinX ((long)(123 * 100000))
#define EASTSEA_MaxX ((long)(125 * 100000))
#define EASTSEA_MinY ((long)(26.5 * 100000))
#define EASTSEA_MaxY ((long)(28.5 * 100000))

#define SOUTHSEA_MinX ((long)(115 * 100000))
#define SOUTHSEA_MaxX ((long)(117 * 100000))
#define SOUTHSEA_MinY ((long)(18 * 100000))
#define SOUTHSEA_MaxY ((long)(20 * 100000))

	if (!g_pDMGdata->uiSetData.nBsdDrawMap)
	{
		long lng, lat;

		lng = g_pDMLibFS->pGdata->transfData.nMapCenterX;
		lat = g_pDMLibFS->pGdata->transfData.nMapCenterY;

		if (TW_MinX < lng && lng < TW_MaxX && TW_MinY < lat && lat < TW_MaxY)
		{
			/*NUROFONT fontinfo;
			WCHAR *wcsNoResouce = L"怢俜吽蝠籵杅擂訧蹋婃";

			nuMemset(&fontinfo, 0, sizeof fontinfo);

			fontinfo.lfEscapement = 1;
			fontinfo.lfHeight = 40;
			fontinfo.lfHeight = 40;
			fontinfo.lfWeight = 2;
			;
			g_pDMLibGDI->GdiSetFont(&fontinfo);
			g_pDMLibGDI->GdiSetTextColor(0x330000);
			//g_pDMLibGDI->GdiExtTextOutWEx(200, 200, 
			//	wcsNoResouce, nuWcslen(wcsNoResouce), true, 0xff00ff);
			nuroRECT rtText = { 250, 280, 800, 480 };
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE, false, 0);
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE, false, 0x330000);*/
		}
 #ifdef WINCE
		else if (BOSEA_MinX < lng && lng < BOSEA_MaxX && BOSEA_MinY < lat && lat < BOSEA_MaxY)
		{
    

			NUROFONT fontinfo;
			WCHAR *wcsNoResouce = (WCHAR*)nuTEXT("眾漆");

			nuMemset(&fontinfo, 0, sizeof fontinfo);

			fontinfo.lfEscapement = 1;
			fontinfo.lfHeight = 40;
			fontinfo.lfHeight = 40;
			fontinfo.lfWeight = 2;

			g_pDMLibGDI->GdiSetFont(&fontinfo);
			g_pDMLibGDI->GdiSetTextColor(0x330000);
			//g_pDMLibGDI->GdiExtTextOutWEx(200, 200, 
			//	wcsNoResouce, nuWcslen(wcsNoResouce), true, 0xff00ff);
			nuroRECT rtText = { 370, 280, 100, 50 };
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE, false, 0);
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE, false, 0x330000);
    
		}
		else if (YESEA_MinX < lng && lng < YESEA_MaxX && YESEA_MinY < lat && lat < YESEA_MaxY)
		{
        
			NUROFONT fontinfo;
			WCHAR *wcsNoResouce =(WCHAR*)nuTEXT("酴漆");

			nuMemset(&fontinfo, 0, sizeof fontinfo);

			fontinfo.lfEscapement = 1;
			fontinfo.lfHeight = 40;
			fontinfo.lfHeight = 40;
			fontinfo.lfWeight = 2;

			g_pDMLibGDI->GdiSetFont(&fontinfo);
			g_pDMLibGDI->GdiSetTextColor(0x330000);
			//g_pDMLibGDI->GdiExtTextOutWEx(200, 200, 
			//	wcsNoResouce, nuWcslen(wcsNoResouce), true, 0xff00ff);
			nuroRECT rtText = { 370, 280, 100, 50 };
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE, false, 0);
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE, false, 0x330000);
		}
		else if (EASTSEA_MinX < lng && lng < EASTSEA_MaxX && EASTSEA_MinY < lat && lat < EASTSEA_MaxY)
		{
			NUROFONT fontinfo;
			WCHAR *wcsNoResouce = (WCHAR*)nuTEXT("陲漆");

			nuMemset(&fontinfo, 0, sizeof fontinfo);

			fontinfo.lfEscapement = 1;
			fontinfo.lfHeight = 40;
			fontinfo.lfHeight = 40;
			fontinfo.lfWeight = 2;

			g_pDMLibGDI->GdiSetFont(&fontinfo);
			g_pDMLibGDI->GdiSetTextColor(0x330000);
			//g_pDMLibGDI->GdiExtTextOutWEx(200, 200, 
			//	wcsNoResouce, nuWcslen(wcsNoResouce), true, 0xff00ff);
			nuroRECT rtText = { 370, 280, 100, 50 };
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE, false, 0);
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE, false, 0x330000);
		}
		else if (SOUTHSEA_MinX < lng && lng < SOUTHSEA_MaxX && SOUTHSEA_MinY < lat && lat < SOUTHSEA_MaxY)
		{
			NUROFONT fontinfo;
			WCHAR *wcsNoResouce = (WCHAR*)nuTEXT("鰍漆");

			nuMemset(&fontinfo, 0, sizeof fontinfo);

			fontinfo.lfEscapement = 1;
			fontinfo.lfHeight = 40;
			fontinfo.lfHeight = 40;
			fontinfo.lfWeight = 2;

			g_pDMLibGDI->GdiSetFont(&fontinfo);
			g_pDMLibGDI->GdiSetTextColor(0x330000);
			//g_pDMLibGDI->GdiExtTextOutWEx(200, 200, 
			//	wcsNoResouce, nuWcslen(wcsNoResouce), true, 0xff00ff);
			nuroRECT rtText = { 370, 280, 100, 50 };
			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE, false, 0);

			g_pDMLibGDI->GdiDrawTextWEx(wcsNoResouce, nuWcslen(wcsNoResouce), 
				&rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE, false, 0x330000);
		}
 #endif
	}
#endif


	LibDM_DrawRawCar(pVoid);
    return nuTRUE;
}
//Draw ZoomScreen
DRAWMAP_API nuBOOL LibDM_DrawZoomRoad(nuPVOID pVoid)
{
    NURO_BMP *pOldBmp = g_pDMLibGDI->GdiSelectCanvasBMP(&g_pDMGdata->zoomScreenData.nuZoomBmp);
    if( pOldBmp == NULL )
    {
        return nuFALSE;
    }
    g_pDMLibMT->MT_SaveOrShowBmp(PAINT_NUROCROSSBMP);
    g_pDrawMgrZ->ZoomCrossProc(pVoid);
    g_pDMLibGDI->GdiSelectCanvasBMP(pOldBmp);
    //	g_pDrawBlock->DrawZoomRoad();
    //	g_pPoiZoom->DrawPoi();
    return nuTRUE;
}
DRAWMAP_API nuBOOL LibDM_DrawZoomFake(nuPVOID pVoid)
{
    g_pDMLibMT->MT_SaveOrShowBmp(PAINT_NUROBMP);
    nuBOOL bRes = nuTRUE;
#ifdef _USE_SEA_DRAW
    g_pDrawSeaZ->DrawSeaZ();
    g_pDrawSeaZ->ColSeaData();
#endif
    g_pDrawBlock->LoadScreenMap();
    if( g_pDMGdata->uiSetData.nBsdDrawMap )
    {
        if( g_pDMGdata->uiSetData.nBsdDrawMap == BSD_MODE_5k )
        {
#ifdef _USE_MDW_DRAW
            g_pDrawMgrZ->CollectDrawMem();
            g_pDrawMdwZ->ColMdwData();
            g_pDrawMgrZ->DrawRoad(NULL, DRAWRD_MODE_FAKELINE);
#endif
        }
        else
        {
#ifdef _USE_BGL_DRAW
            g_pDrawMgrZ->CollectDrawMem();
            g_pDrawBglZ->ColBglData();
            g_pDrawMgrZ->DrawBgLine(NULL, DRAWRD_MODE_FAKELINE);
            g_pDrawMgrZ->DrawRoad(NULL, DRAWRD_MODE_FAKELINE);
#endif
        }
    }
    else
    {
        g_pDrawBlock->DrawBgArea();
        bRes = g_pDrawBlock->DrawZoomFake();
    }
    return bRes;
}

//These three functions would been moved to FileSystem
DRAWMAP_API nuBOOL LibDM_SeekForRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData)
{
    return g_pDrawBlock->SeekForRoad(x, y, nCoverDis, pRoadData);
}
DRAWMAP_API nuBOOL LibDM_SeekForPOI(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData)
{
    return nuFALSE;
}
DRAWMAP_API nuBOOL LibDM_CarToRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, nuLONG nCarAngle, PSEEKEDROADDATA pRoadData)
{
    return g_pDrawBlock->CarToRoad(x, y, nCoverDis, nCarAngle, pRoadData);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DRAWMAP_API nuBOOL LibDM_DrawFlag(nuPVOID pVoid)
{
    if( g_pDMGdata == NULL )
    {
        return nuFALSE;
    }
    nuroPOINT point= {0};
    nuLONG index =0;
    nuroSRECT rect = g_pDMGdata->transfData.rtMapScreen;
    nuBYTE i;
    for( i = 0; i < g_pDMGdata->routeGlobal.TargetCount; i ++ )
    {
        g_pDMLibMT->MT_MapToBmp( g_pDMGdata->routeGlobal.pTargetInfo[i].BaseCoor.x,
            g_pDMGdata->routeGlobal.pTargetInfo[i].BaseCoor.y,
            &point.x, 
            &point.y );
        if( !nuPtInSRect(point, rect) )
        {
            continue;
        }
        if( g_pDMGdata->uiSetData.b3DMode )
        {
            g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);

        }
        if( i == 0 )
        {
            index = 128;
        }
        else if( i == g_pDMGdata->routeGlobal.TargetCount - 1 )
        {
            index = 128 + 2;
        }
        else
        {
            index = 128 + 1;

        }
        //g_pDMLibGDI->GdiDrawIcon(point.x - 16, point.y - 32, index);
		g_pDMLibGDI->GdiDrawTargetFlagIcon(point.x, point.y, index);
    }

    if( //( 1 == g_bDrawLineToTarget && g_pDMGdata->routeGlobal.TargetCount > 1 ) ||
        ( g_pDMGdata->uiSetData.bNavigation && g_bDrawLineToTarget ) //||  
		/*( g_pDMLibFS->pUserCfg->bTargetLine && g_pDMGdata->routeGlobal.TargetCount > 1)*/ )
    {
        nuroPEN nuPen= {0};
		if( g_pDMGdata->uiSetData.bNightOrDay   )
		{
			nuPen.nRed		= 255;
			nuPen.nGreen	= 255;
			nuPen.nBlue		= 0;
		}
		else
		{
			nuPen.nRed		= 67;
			nuPen.nGreen	= 57;
			nuPen.nBlue		= 238;
		}
        nuPen.nStyle	= NURO_PS_SOLID;
        nuPen.nWidth	= 2;
        g_pDMLibGDI->GdiSetPen(&nuPen);
        nuroPOINT ptList[2];
        g_pDMLibMT->MT_MapToBmp( g_pDMGdata->carInfo.ptCarIcon.x,
            g_pDMGdata->carInfo.ptCarIcon.y,
            &ptList[0].x, 
            &ptList[0].y );
		 nuBOOL bDrawDisToCar = nuFALSE;
		if(  g_pDMGdata->transfData.nCarCenterMode != CAR_CENTER_MODE_THREE_QUARTERS_Y  )
		{
			ptList[1].x = g_pDMGdata->transfData.nBmpCenterX;
			ptList[1].y = g_pDMGdata->transfData.nBmpCenterY; 
			bDrawDisToCar = nuTRUE;
		}
		else
		{
        i = g_pDMGdata->routeGlobal.TargetCount - 1;
        g_pDMLibMT->MT_MapToBmp( g_pDMGdata->routeGlobal.pTargetInfo[i].BaseCoor.x,
            g_pDMGdata->routeGlobal.pTargetInfo[i].BaseCoor.y,
            &ptList[1].x, 
            &ptList[1].y );
		}
		
		nuroRECT rtSrceen ={0};
		rtSrceen.left   = 0;
		rtSrceen.top    = 0;
		rtSrceen.right  = g_pDMGdata->uiSetData.nScreenWidth;
		rtSrceen.bottom = g_pDMGdata->uiSetData.nScreenHeight;
		nuBOOL bPt1InRt = nuPointInRect(&ptList[0],&rtSrceen );
		nuBOOL bPt2InRt = nuPointInRect(&ptList[1],&rtSrceen );
		nuroPOINT ptSave[2] = {0};
		ptSave[0] = ptList[0];
		ptSave[1] = ptList[1];

		if( !bPt1InRt && !bPt2InRt )
		{
			return nuFALSE;
		}
		
		if( !bPt1InRt || !bPt2InRt )
		{
			//党淏萸
			if( bPt1InRt )
			{
				nuPointFix(ptList[0],ptList[1],rtSrceen );
			}
			else if( bPt2InRt )
			{
				nuPointFix(ptList[1],ptList[0],rtSrceen );
			}

		}

        nuINT nObj = DRAW_OBJ_2D_DASH;
        if( g_pDMGdata->uiSetData.b3DMode )
        {
            nObj = DRAW_OBJ_3D_DASH;
        }
        g_pDMLibMT->MT_DrawObject(nObj, ptList, 2, 8);
		//bDrawDisToCar = nuFALSE;
		if( bDrawDisToCar )
		{
			nuLONG nDis  = 0;
			nuLONG nDis2 = 0 ;
			nuBOOL bSetKm = nuFALSE;
			g_pDMLibMT->MT_BmpToMap(ptSave[0].x,ptSave[0].y,&ptSave[0].x,&ptSave[0].y );
			g_pDMLibMT->MT_BmpToMap(ptSave[1].x,ptSave[1].y,&ptSave[1].x,&ptSave[1].y );
			nuULONG nTmpDx = NURO_ABS( ptSave[1].x - ptSave[0].x );
			//nTmpDx = nTmpDx * nuCos( ptSave[1].y + ptSave[0].y/100000);
			nuULONG nTmpDy = NURO_ABS( ptSave[1].y - ptSave[0].y );
			if( nTmpDx > 0xFFFF || nTmpDy > 0xFFFF )
			{
				nTmpDx /= 1000;
				nTmpDy /= 1000;
				bSetKm = nuTRUE;
			}
			if( bSetKm )
			{
				nDis = nuDis( ptSave[0].y*PI/ 18000000 , ptSave[0].x *PI/ 18000000,  ptSave[1].y*PI/ 18000000, ptSave[1].x *PI/ 18000000 );	
			}
			else
			{
				nDis = (nuLONG)nuSqrt( nTmpDx* nTmpDx + nTmpDy * nTmpDy );
			}
			
			if ( nuFALSE == bSetKm )
			{
				if( nDis > 1000 )
				{
					bSetKm = nuTRUE;
					nDis2 = ( nDis % 1000 ) / 100;
					nDis /= 1000;
				}
			}
			
			nuWCHAR wsDis[16] = {0};
			nuItow( nDis, wsDis, 10 );
			if( bSetKm )
			{
				if( nDis2 > 0 )
				{
					nuLONG nLen = nuWcslen(wsDis);
					wsDis[nLen] = 46; //.
					nuWCHAR tmp[10] = {0};
					nuItow( nDis2,tmp,10 );
					nuWcscat( wsDis, tmp );
				}
				nuLONG nLen = nuWcslen(wsDis);
				wsDis[nLen] = 107; //k
			}
			nuLONG nLen = nuWcslen(wsDis);
			wsDis[nLen] = 109; //m
			
			//NUROLOGFONT lf;
			NUROFONT lf= {0};
			lf.lfHeight = 25;
			lf.lfWidth	= 0;
			lf.lfEscapement = 0;
			g_pDMLibGDI->GdiSetFont(&lf);
			nuCOLORREF crText= 255;
			g_pDMLibGDI->GdiSetTextColor(crText);
			
			nuroRECT rtDrawText = {0};
			rtDrawText.left  = g_pDMGdata->transfData.nBmpCenterX + 20;
			rtDrawText.top = g_pDMGdata->transfData.nBmpCenterY +20 ;
			
			g_pDMLibGDI->GdiDrawTextW( wsDis,-1, &rtDrawText ,NURO_DT_CALCRECT );
			g_pDMLibGDI->GdiDrawTextW( wsDis,-1, &rtDrawText ,NURO_DT_SINGLELINE );
			
		}
    }
    return nuTRUE;
}
DRAWMAP_API nuBOOL LibDM_DrawRawCar(nuPVOID pVoid)
{
    if( g_pDMGdata == NULL || g_pDrawMgrZ == NULL || !g_pDrawMgrZ->m_bDrawRawCar )
    {
        return nuFALSE;
    }
    nuroPOINT point = {0};
	nuroPOINT pt[12] = {0};
    NUROPEN nuPen = {0};
    NUROBRUSH nuBrush = {0};
    g_pDMLibMT->MT_MapToBmp( g_pDMGdata->carInfo.ptRawGPS.x, 
							 g_pDMGdata->carInfo.ptRawGPS.y, 
                             &point.x, 
                             &point.y );
    if( point.y < 0 || point.y > g_pDMGdata->transfData.nBmpHeight )
    {
        return nuFALSE;
    }
    if( g_pDMGdata->uiSetData.b3DMode )
    {
        g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
    }
    point.x -= g_pDMGdata->transfData.nBmpLTx;
    point.y -= g_pDMGdata->transfData.nBmpLTy;
    pt[10].x = pt[9].x = point.x - 8;
    pt[0].x = pt[11].x = pt[8].x = pt[7].x = point.x - 1;
    pt[1].x = pt[2].x = pt[5].x = pt[6].x = point.x + 1;
    pt[3].x = pt[4].x = point.x + 8;
    pt[0].y = pt[1].y = point.y - 8;
    pt[10].y = pt[11].y = pt[2].y = pt[3].y = point.y - 1;
    pt[9].y = pt[8].y = pt[5].y = pt[4].y = point.y + 1;
    pt[7].y = pt[6].y = point.y + 8;
    //
    nuPen.nRed		= 0;
    nuPen.nGreen	= 255;
    nuPen.nBlue		= 0;
    nuPen.nStyle	= NURO_PS_SOLID;
    nuPen.nWidth	= 1;
    g_pDMLibGDI->GdiSetPen(&nuPen);
    nuBrush.nRed	= 0;
    nuBrush.nGreen	= 0;
    nuBrush.nBlue	= 255;
    g_pDMLibGDI->GdiSetBrush(&nuBrush);
    g_pDMLibGDI->GdiPolygon(pt, 12);
    //	g_pDMLibGDI->GdiEllipse(point.x - 5, point.y - 5, point.x + 5, point.y + 5);
    /*
    g_pDMLibMT->MT_MapToBmp(g_pDMGdata->carInfo.ptCarFixed.x, g_pDMGdata->carInfo.ptCarFixed.y, &point.x, &point.y);
    if( g_pDMGdata->uiSetData.b3DMode )
    {
    g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
    }
    pt[10].x = pt[9].x = point.x - 3;
    pt[0].x = pt[11].x = pt[8].x = pt[7].x = point.x - 1;
    pt[1].x = pt[2].x = pt[5].x = pt[6].x = point.x + 1;
    pt[3].x = pt[4].x = point.x + 3;
    pt[0].y = pt[1].y = point.y - 3;
    pt[10].y = pt[11].y = pt[2].y = pt[3].y = point.y - 1;
    pt[9].y = pt[8].y = pt[5].y = pt[4].y = point.y + 1;
    pt[7].y = pt[6].y = point.y + 3;
    //
    nuPen.nRed		= 255;
    nuPen.nGreen	= 0;
    nuPen.nBlue		= 0;
    nuPen.nStyle	= NURO_PS_SOLID;
    nuPen.nWidth	= 1;
    g_pDMLibGDI->SetGdiPen(&nuPen);
    nuBrush.nRed	= 0;
    nuBrush.nGreen	= 0;
    nuBrush.nBlue	= 255;
    g_pDMLibGDI->SetGdiBrush(&nuBrush);
    g_pDMLibGDI->Polygon(pt, 12);*/
    return nuTRUE;
}

DRAWMAP_API nuBOOL LibDM_DrawMap(nuPVOID pVoid, nuWORD nMode)
{
    if( pVoid == NULL )
    {
        return nuFALSE;
    }
    PACTIONSTATE pAcState = (PACTIONSTATE)pVoid;
    while( g_nDrawStep != DRAW_STEP_NODRAW )
    {
		nuSleep(TIMER_TIME_UNIT);
    }
    g_nDrawStep	= DRAW_STEP_BMP1;
    if( pAcState->bDrawBmp1 )
    {
        LibDM_DrawMapBmp1(pVoid);
        /*
        if( g_pDMGdata->uiSetData.bNavigation )
        {
        g_pDMLibIN->IN_DrawNaviRoad();
        }
        *///replaced by a new way
        LibDM_DrawMapBmp2(pVoid);
    }
    
    g_nDrawStep = DRAW_STEP_NODRAW;
    return nuTRUE;
}
DRAWMAP_API nuBOOL LibDM_FreeDataMem()
{
#ifndef MAJOR_POI_NEW
    //	g_pDrawMpoiZ->ReadMpoiData();
#else
    g_pDrawMajorPoiZK->FreeMajorPoi();
#endif
    g_pDrawMgrZ->FreeDrawMem();
    return nuTRUE;
}

DRAWMAP_API nuUINT LibDM_StartOpenGL(SYSDIFF_SCREEN sysDiff, const NURORECT* pRtScreen)
{
#ifdef _USE_OPENGLES
    //	MessageBox(NULL, L"Test", L"", 0);
    return g_draw3DMdlZK.StartOpenGL(sysDiff, pRtScreen);
#else
    return 0;
#endif
}

DRAWMAP_API nuVOID LibDM_Reset3DModel(nuBOOL bReSetSize, nuWORD nWidth, nuWORD nHeight)
{
#ifdef _USE_OPENGLES
	if( bReSetSize )
	{
		nuroRECT RtScreen= {0};
		RtScreen.top  = 0;
		RtScreen.left = 0;
		RtScreen.right = nWidth;
		RtScreen.bottom = nHeight;
		SYSDIFF_SCREEN sysDiff= {0};
		g_draw3DMdlZK.StartOpenGL(sysDiff, &RtScreen);
	}
#endif
}
DRAWMAP_API nuBOOL LibDM_GetNearKilo(nuLONG* pDis, nuWCHAR* pwsKilos)
{
	return g_pDrawKilometerDL->GetNearKilo(pDis, pwsKilos);
}
DRAWMAP_API nuVOID LibDM_FreeDrawData(nuLONG nFreeData)
{
	if(nFreeData & DRAW_MAP_FREE_BGL)
		g_pDrawBglZ->FreeData();

	if(nFreeData & DRAW_MAP_FREE_SEA)
		g_pDrawSeaZ->FreeData();
		
}
//---------------------------------__SDK------------------------------------------------------------//
DRAWMAP_API nuUINT LibDM_DrawMapCar(NUROPOINT ptCar)
{
	g_pDMLibMT->MT_MapToBmp( g_pDMLibFS->pGdata->carInfo.ptCarIcon.x, 
		                     g_pDMLibFS->pGdata->carInfo.ptCarIcon.y,
		                     &g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.x, 
		                     &g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.y );
	if( !nuPtInSRect(g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen, g_pDMGdata->transfData.rtMapScreen) )
    	{
       	 	return 0;
    	}
	if( g_pDMLibFS->pGdata->uiSetData.b3DMode )
	{
		g_pDMLibMT->MT_Bmp2Dto3D( g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.x, 
			                      g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.y, MAP_DEFAULT );
	}
	return g_pDrawCarXQ->DrawCar( g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.x, 
			                g_pDMLibFS->pGdata->drawInfoMap.ptCarInScreen.y,
								g_nIconType);
}
DRAWMAP_API nuBOOL LibDM_DrawMapPic(NURORECT Rect, NURO_BMP Bmp, nuSHORT *pBmp)
{
	if(!Bmp.bmpHasLoad || Bmp.pBmpBuff == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "3dpic", "DM_DrawMapPic--Bmp.pBmpBuff == NULL");
		return nuFALSE;
	}
	if(Bmp.bmpWidth != (Rect.right - Rect.left) || Bmp.bmpHeight != (Rect.bottom - Rect.top))
	{
		if( NULL == g_RealPicBmpZoom.pBmpBuff )
		{
			g_RealPicBmpZoom.bmpBitCount = Bmp.bmpBitCount;
			g_RealPicBmpZoom.bmpBufferType = Bmp.bmpBufferType;
			g_RealPicBmpZoom.bmpTransfColor = Bmp.bmpTransfColor;
			g_RealPicBmpZoom.bmpType = Bmp.bmpType;
			g_RealPicBmpZoom.bmpWidth = Rect.right - Rect.left;
			g_RealPicBmpZoom.bmpHeight = Rect.bottom - Rect.top;
			g_RealPicBmpZoom.bmpBuffLen = 2 * g_RealPicBmpZoom.bmpWidth * g_RealPicBmpZoom.bmpHeight;
			g_RealPicBmpZoom.bytesPerLine = 2 * g_RealPicBmpZoom.bmpWidth;
			g_RealPicBmpZoom.pBmpBuff = new nuBYTE[g_RealPicBmpZoom.bmpWidth * g_RealPicBmpZoom.bmpHeight * 2];
			if(NULL == g_RealPicBmpZoom.pBmpBuff)
			{
				return nuFALSE;
			}	
			__android_log_print(ANDROID_LOG_INFO, "3dpic", "DrawZoomMapPic-- W %d, H %d", g_RealPicBmpZoom.bmpWidth, g_RealPicBmpZoom.bmpHeight);
		}
		if(!g_pDMLibGDI->GdiZoomBmp(&g_RealPicBmpZoom, 0, 0, g_RealPicBmpZoom.bmpWidth, g_RealPicBmpZoom.bmpHeight, &Bmp, 0, 0, Bmp.bmpWidth, Bmp.bmpHeight))
		{
			__android_log_print(ANDROID_LOG_INFO, "3dpic", "DM_DrawMapPic--Zoom Fail == NULL");
			return nuFALSE;
		}
	}
	nuMemcpy(pBmp, g_RealPicBmpZoom.pBmpBuff, g_RealPicBmpZoom.bmpWidth * g_RealPicBmpZoom.bmpHeight * 2);
	/*NURO_BMP TmpBmp = {0};
	nuMemcpy(&TmpBmp, &g_RealPicBmpZoom, sizeof(NURO_BMP));
	TmpBmp.pBmpBuff = (nuBYTE*)pBmp;
	return g_pDMLibGDI->GdiDrawBMP(Rect.left, Rect.top, g_RealPicBmpZoom.bmpWidth, g_RealPicBmpZoom.bmpHeight, &TmpBmp, 0, 0);*/
	//return g_pDMLibGDI->GdiDrawBMP(Rect.left, Rect.top, g_RealPicBmpZoom.bmpWidth, g_RealPicBmpZoom.bmpHeight, &g_RealPicBmpZoom, 0, 0);
	return nuTRUE;
}
DRAWMAP_API nuVOID LibDM_CheckSkyBmp()
{
	NURO_BMP TmpBmp = {0};
	if(g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth != g_pDMLibFS->pGdata->transfData.nBmpWidth)
	{
		TmpBmp.bmpBitCount    	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBitCount;
		TmpBmp.bmpBufferType  	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBufferType;
		TmpBmp.bmpTransfColor 	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpTransfColor;
		TmpBmp.bmpType 		= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpType;
		TmpBmp.bmpWidth 	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth;
		TmpBmp.bmpHeight 	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight;
		TmpBmp.bmpBuffLen 	= 2 * TmpBmp.bmpWidth * TmpBmp.bmpHeight;
		TmpBmp.bytesPerLine 	= 2 * TmpBmp.bmpWidth;
		TmpBmp.pBmpBuff 	= new nuBYTE[TmpBmp.bmpWidth * TmpBmp.bmpHeight * 2];
		if(NULL == TmpBmp.pBmpBuff)
		{
			return;
		}	
		nuMemcpy(TmpBmp.pBmpBuff, g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.pBmpBuff, TmpBmp.bmpBuffLen);
		__android_log_print(ANDROID_LOG_INFO, "navi", "CopySkyBmp-- W %d, H %d", TmpBmp.bmpWidth, TmpBmp.bmpHeight);
		g_pDMLibGDI->GdiDelNuroBMP(&g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D);
		if( NULL == g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.pBmpBuff )
		{
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBitCount    	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBitCount;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBufferType  	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBufferType;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpTransfColor 	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpTransfColor;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpType 		= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpType;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth 		= g_pDMLibFS->pGdata->transfData.nBmpWidth;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight 	= g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpBuffLen 	= 2 * g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth * g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bytesPerLine 	= 2 * g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth;
			g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.pBmpBuff 	= new nuBYTE[g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth * g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight * 2];
			if(NULL == g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.pBmpBuff)
			{
				return;
			}	
			__android_log_print(ANDROID_LOG_INFO, "navi", "ZoomSky3DBmp-- W %d, H %d", g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth, g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight);
		}
		if(!g_pDMLibGDI->GdiZoomBmp(&g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D, 0, 0, g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth, g_pDMLibFS->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight, &TmpBmp, 0, 0, TmpBmp.bmpWidth, TmpBmp.bmpHeight))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "Sky3DBmp--Zoom Fail");
			return;
		}
	}		
}
//---------------------------------__SDK------------------------------------------------------------//
