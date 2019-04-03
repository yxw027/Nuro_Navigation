#ifndef __NURO_MATHTOOL_20080128
#define __NURO_MATHTOOL_20080128

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroCommonStructs.h"
#include "NuroConstDefined.h"
#include "NuroModuleApiStruct.h"

#ifdef _USRDLL
#ifdef MATHTOOL_EXPORTS
#define MATHTOOL_API extern "C" __declspec(dllexport)
#else
#define MATHTOOL_API extern "C" __declspec(dllimport)
#endif
#else
#define MATHTOOL_API extern "C" 
#endif

MATHTOOL_API nuBOOL LibMT_InitMathTool(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpInFS, nuPVOID lpOutMT);
MATHTOOL_API nuVOID LibMT_FreeMathTool();

MATHTOOL_API nuBOOL LibMT_SetBitmap(PNURO_BMP pNuroBmp);
MATHTOOL_API nuBOOL LibMT_Set3DParam(nuWORD nType, short nStep);


MATHTOOL_API nuBOOL LibMT_MapToBmp(nuLONG nMapX, nuLONG nMapY, nuLONG *lpBmpX, nuLONG *lpBmpY);
MATHTOOL_API nuBOOL LibMT_BmpToMap(nuLONG nBmpX, nuLONG nBmpY, nuLONG *lpMapX, nuLONG *lpMapY);
MATHTOOL_API nuBOOL LibMT_ZoomMapToBmp(nuLONG nMapX, nuLONG nMapY, nuLONG *lpBmpX, nuLONG *lpBmpY);
MATHTOOL_API nuBOOL LibMT_ZoomBmpToMap(nuLONG nBmpX, nuLONG nBmpY, nuLONG *lpMapX, nuLONG *lpMapY);

MATHTOOL_API nuBOOL LibMT_SaveOrShowBmp(nuBYTE nBmpType);

MATHTOOL_API nuINT LibMT_SelectObject(nuBYTE nDrawType, nuBYTE nShowMode, const nuVOID* lpSet, nuLONG *pOut);
MATHTOOL_API nuBOOL LibMT_DrawObject(nuINT nObjType, PNUROPOINT pPt, nuINT nCount, nuLONG nExtend);
MATHTOOL_API nuVOID LibMT_Bmp2Dto3D(nuLONG& x, nuLONG &y, nuBYTE nMapType);
MATHTOOL_API nuVOID LibMT_Bmp3Dto2D(nuLONG& x, nuLONG &y, nuBYTE nMapType);

MATHTOOL_API nuLONG LibMT_Get3DThick(nuLONG thick, nuLONG y);
MATHTOOL_API nuVOID LibMT_Get3DParam(PCTRL_3D_PARAM pCtrl3DParam);

// Added by Dengxu @ 2012 11 05
#ifdef USING_FILE_SETTING_VIEW_ANGLE
MATHTOOL_API nuLONG LibMT_GetMaxViewAngle();
MATHTOOL_API nuLONG LibMT_GetMinViewAngle();
#endif


#endif
