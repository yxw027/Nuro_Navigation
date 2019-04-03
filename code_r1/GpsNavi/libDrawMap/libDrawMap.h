#ifndef __NURO_DRAWMAP_20080226
#define __NURO_DRAWMAP_20080226

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroModuleApiStruct.h"

#ifdef _USRDLL
#ifdef DRAWMAP_EXPORTS
#define DRAWMAP_API extern "C" __declspec(dllexport)
#else
#define DRAWMAP_API extern "C" __declspec(dllimport)
#endif
#else
#define DRAWMAP_API extern "C" 
#endif

extern PGDIAPI         g_pDMLibGDI;
extern PMEMMGRAPI      g_pDMLibMM;
extern PFILESYSAPI     g_pDMLibFS;
extern PMATHTOOLAPI    g_pDMLibMT;
extern PGLOBALENGINEDATA   g_pDMGdata;
extern PMAPCONFIG          g_pDMMapCfg;

DRAWMAP_API nuBOOL LibDM_InitDrawMap(PAPISTRUCTADDR pApiAddr, POPENRSAPI pRsApi);
DRAWMAP_API nuVOID LibDM_FreeDrawMap();

DRAWMAP_API nuBOOL LibDM_LoadMap(nuPVOID pVoid);
DRAWMAP_API nuBOOL LibDM_DrawMapBmp1(nuPVOID pVoid);
DRAWMAP_API nuBOOL LibDM_DrawMapBmp2(nuPVOID pVoid);
DRAWMAP_API nuBOOL LibDM_DrawZoomRoad(nuPVOID pVoid);

DRAWMAP_API nuBOOL LibDM_DrawZoomFake(nuPVOID pVoid);

DRAWMAP_API nuBOOL LibDM_DrawMap(nuPVOID pVoid, nuWORD nMode);

DRAWMAP_API nuBOOL LibDM_DrawFlag(nuPVOID pVoid);
DRAWMAP_API nuBOOL LibDM_DrawRawCar(nuPVOID pVoid);

DRAWMAP_API nuBOOL LibDM_SeekForRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData);
DRAWMAP_API nuBOOL LibDM_SeekForPOI(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData);
DRAWMAP_API nuBOOL LibDM_CarToRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, nuLONG nCarAngle, PSEEKEDROADDATA pRoadData);

DRAWMAP_API nuBOOL LibDM_FreeDataMem();

DRAWMAP_API nuUINT LibDM_StartOpenGL(SYSDIFF_SCREEN sysDiff, const NURORECT* pRtScreen);
DRAWMAP_API nuVOID LibDM_Reset3DModel(nuBOOL bReSetSize, nuWORD nWidth, nuWORD nHeight); //Added @ 2012.06.18
DRAWMAP_API nuBOOL LibDM_GetNearKilo(nuLONG* pDis, nuWCHAR* pwsKilos);//added by daniel 20121205
DRAWMAP_API nuVOID LibDM_FreeDrawData(nuLONG nFreeData);
//-----------------------------------SDK-----------------------------//
DRAWMAP_API nuUINT LibDM_DrawMapCar(NUROPOINT ptCar);
DRAWMAP_API nuBOOL LibDM_DrawMapPic(NURORECT Rect, NURO_BMP Bmp, nuSHORT *pBmp);
DRAWMAP_API nuVOID LibDM_CheckSkyBmp();
//-----------------------------------SDK-----------------------------//
#endif
