
#include "libMathTool.h"
#include "GdiObjectK.h"
#include "ViewTransf.h"
#include "Object3DDrawZK.h"

//Library
//#include "LoadGDI.h"
//#include "LoadMM.h"
//#include "LoadFS.h"
PFILESYSAPI			g_pMtFsApi = NULL;
PGDIAPI				g_pMtGdiApi = NULL;
PMEMMGRAPI			g_pMtMmApi = NULL;
class CGdiObjectK*			g_pGdiObject = NULL;
class CObject3DDrawZK*		g_pObj3DDraw = NULL;

// Added by Dengxu @ 2012 11 14
static nuINT s_MaxSkyHeight = 0;

// Added by Dengxu @ 2012 11 04
#ifdef USING_FILE_SETTING_VIEW_ANGLE
#define _3D_MAX_ANGLE g_nMaxViewAngle
#define _3D_MIN_ANGLE g_nMinViewAngle
static nuLONG g_nMaxViewAngle = 23;
static nuLONG g_nMinViewAngle = 10;

MATHTOOL_API nuLONG LibMT_GetMaxViewAngle()
{
	return g_nMaxViewAngle;
}

MATHTOOL_API nuLONG LibMT_GetMinViewAngle()
{
	return g_nMinViewAngle;
}
#endif

static nuLONG g_SKY_GRADIENT_LEN = 0; // Default is 20
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
			LibMT_FreeMathTool();
			break;
    }
    return TRUE;
}*/
#endif
/*********************************************************************************
 *********************************************************************************
 *********************************************************************************/
/* MathTool's Api functions */
MATHTOOL_API nuBOOL LibMT_InitMathTool(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpInFS, nuPVOID lpOutMT)
{
	g_pGdiObject = new CGdiObjectK();
	if( g_pGdiObject == NULL )
	{
		return nuFALSE;
	}
	g_pObj3DDraw = new CObject3DDrawZK();
	if( NULL == g_pGdiObject )
	{
		return nuFALSE;
	}
	g_pMtMmApi	= (PMEMMGRAPI)lpInMM;
	g_pMtGdiApi	= (PGDIAPI)lpInGDI;
	g_pMtFsApi	= (PFILESYSAPI)lpInFS;
	if( !g_pGdiObject->Initialize() )
	{
		return nuFALSE;
	}
	if( !g_pObj3DDraw->Initialize(g_pMtGdiApi) )
	{
		return nuFALSE;
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
		minAngle = 10;
	}

	g_nMaxViewAngle = maxAngle;
	g_nMinViewAngle = minAngle;
#endif
	// Modified by Dengxu @ 2012 11 14
	nuLONG sky_gradinet;
	if (nuFALSE == nuReadConfigValue("SKY_GRADIENT_PIXEL_SIZE", &sky_gradinet)
		&& nuFALSE == nuReadConfigValue("SKY_GRADINET_PIXEL_SIZE", &sky_gradinet))
	{
		sky_gradinet = 20;
	}
	g_SKY_GRADIENT_LEN = sky_gradinet;
	s_MaxSkyHeight = g_pMtFsApi->pGdata->uiSetData.nSkyHeight;


	FUNC_FOR_GDI funForGdi = {0};
	funForGdi.lpFcGet3DThick = (nuPVOID)LibMT_Get3DThick;
	g_pMtGdiApi->GdiSet3DPara(&funForGdi);
	PMATHTOOLAPI pMtApi = (PMATHTOOLAPI)lpOutMT;
	pMtApi->MT_BmpToMap		= LibMT_BmpToMap;
	pMtApi->MT_MapToBmp		= LibMT_MapToBmp;
	pMtApi->MT_ZoomMapToBmp	= LibMT_ZoomMapToBmp;
	pMtApi->MT_ZoomBmpToMap	= LibMT_ZoomBmpToMap;
	pMtApi->MT_SaveOrShowBmp= LibMT_SaveOrShowBmp;
	pMtApi->MT_SelectObject	= LibMT_SelectObject;
	pMtApi->MT_DrawObject	= LibMT_DrawObject;
	pMtApi->MT_SetBitmap	= LibMT_SetBitmap;
	pMtApi->MT_Set3DParam	= LibMT_Set3DParam;
	pMtApi->MT_Bmp2Dto3D	= LibMT_Bmp2Dto3D;
	pMtApi->MT_Bmp3Dto2D	= LibMT_Bmp3Dto2D;
	pMtApi->MT_Get3DParam	= LibMT_Get3DParam;
	return nuTRUE;
}
MATHTOOL_API nuVOID LibMT_FreeMathTool()
{
	if( g_pGdiObject != NULL )
	{
		g_pGdiObject->Free();
		delete g_pGdiObject;
		g_pGdiObject = NULL;
		
	}
    if( NULL != g_pObj3DDraw)
    { 
          delete g_pObj3DDraw;
		  g_pObj3DDraw = NULL;
	}
	//
}

MATHTOOL_API nuBOOL LibMT_SetBitmap(PNURO_BMP pNuroBmp)
{
	pNuroBmp->bmpWidth	= (pNuroBmp->bmpWidth + 3) / 4 * 4;
	if( !g_pMtGdiApi->GdiInitBMP(pNuroBmp->bmpWidth, pNuroBmp->bmpHeight) )
	{
		return nuFALSE;
	}
	return nuTRUE;
}


MATHTOOL_API nuBOOL LibMT_Set3DParam(nuWORD nType, short nStep)
{
//	g_pGdiObject->SetViewParam();
	nuINT nAngle = g_pMtFsApi->pGdata->uiSetData.n3DViewAngle;
	if( _3D_PARAM_TYPE_SCALE == nType )
	{
		if( g_pMtFsApi->pGdata->uiSetData.nScaleValue >= _3D_MAX_SCALE_V )
		{
			nAngle = _3D_MAX_ANGLE;
		}
		else if( g_pMtFsApi->pGdata->uiSetData.nScaleValue <= _3D_MIN_SCALE_V )
		{
			nAngle = _3D_MIN_ANGLE;
		}
		else 
		{
			nAngle = _3D_MIN_ANGLE + 
				(g_pMtFsApi->pGdata->uiSetData.nScaleValue - _3D_MIN_SCALE_V) * (_3D_MAX_ANGLE - _3D_MIN_ANGLE) /
				(_3D_MAX_SCALE_V - _3D_MIN_SCALE_V);
		}
	}
	else if( _3D_PARAM_TYPE_STEP_IN == nType )
	{
		if( _3D_MIN_ANGLE == g_pMtFsApi->pGdata->uiSetData.n3DViewAngle ||
			nStep <= 0 )
		{
			return nuFALSE;
		}
		nAngle = g_pMtFsApi->pGdata->uiSetData.n3DViewAngle - nStep;
		if( nAngle < _3D_MIN_ANGLE )
		{
			nAngle = _3D_MIN_ANGLE;
		}
	}
	else if( _3D_PARAM_TYPE_STEP_OUT == nType )
	{
		if( _3D_MAX_ANGLE == g_pMtFsApi->pGdata->uiSetData.n3DViewAngle ||
			nStep <= 0 )
		{
			return nuFALSE;
		}
		nAngle = g_pMtFsApi->pGdata->uiSetData.n3DViewAngle + nStep;
		if( nAngle >= _3D_MAX_ANGLE )
		{
			nAngle = _3D_MAX_ANGLE;
		}
	}
	else if( _3D_PARAM_TYPE_MAX == nType )
	{
		nAngle = _3D_MAX_ANGLE;
	}
	nuINT dz = (nuINT)(g_pMtFsApi->pGdata->transfData.nBmpHeight * 3 * nuTan(nAngle) / 4);
	nuINT nVdis = 80 * g_pMtFsApi->pGdata->transfData.nBmpHeight / 240;
	g_pObj3DDraw->m_obj3DCtrl.SetUp3D( g_pMtFsApi->pGdata->transfData.nBmpWidth, 
		g_pMtFsApi->pGdata->transfData.nBmpHeight,
		-1,
		dz,
		-1,
		-1,
		nVdis );
	g_pMtFsApi->pGdata->uiSetData.nSkyHeight	= g_pObj3DDraw->m_obj3DCtrl.m_nSkyH;

	//Added by Dengxu @ 2012 11 14
	if (0 == s_MaxSkyHeight)
	{
		s_MaxSkyHeight = g_pMtFsApi->pGdata->uiSetData.nSkyHeight;
	}
	
	g_pMtFsApi->pGdata->uiSetData.n3DViewAngle	= nAngle;
	if( _3D_MAX_ANGLE == nAngle )
	{
		g_pMtFsApi->pGdata->uiSetData.b3DViewChanging = 0;
	}
	else
	{
		g_pMtFsApi->pGdata->uiSetData.b3DViewChanging = 1;
	}
	return nuTRUE;
}

MATHTOOL_API nuBOOL LibMT_MapToBmp(nuLONG nMapX, nuLONG nMapY, nuLONG *lpBmpX, nuLONG *lpBmpY)
{
	/********************************************************************
	 * The formula of a Point(x0, y0) rotating with a fixed angle(a) clockwise is
	 * x1 = x0 * cos(a) + y0 * sin(a);
	 * y1 = y0 * cos(a) - x0 * sin(a);
	 ********************************************************************/
	
	if( g_pMtFsApi->pGdata->transfData.nMapWidth	== 0 ||
		g_pMtFsApi->pGdata->transfData.nMapHeight	== 0 )
	{
		__android_log_print(ANDROID_LOG_INFO, "MapToBmp", "nMapHeight %d, nMapWidth %d", g_pMtFsApi->pGdata->transfData.nMapHeight, g_pMtFsApi->pGdata->transfData.nMapWidth);
		return nuFALSE;
	}
	
	nuLONG X = (nMapX - g_pMtFsApi->pGdata->transfData.nMapCenterX) * MAPSCREEN / 
			 g_pMtFsApi->pGdata->transfData.nMapWidth;
	nuLONG Y = (g_pMtFsApi->pGdata->transfData.nMapCenterY - nMapY) * MAPSCREEN / 
			 g_pMtFsApi->pGdata->transfData.nMapHeight;
	*lpBmpX = (X * g_pMtFsApi->pGdata->transfData.nCosConst + Y * g_pMtFsApi->pGdata->transfData.nSinConst) / 
			  LENCONST + g_pMtFsApi->pGdata->transfData.nBmpCenterX;
	*lpBmpY = (Y * g_pMtFsApi->pGdata->transfData.nCosConst - X * g_pMtFsApi->pGdata->transfData.nSinConst) / 
			  LENCONST + g_pMtFsApi->pGdata->transfData.nBmpCenterY;
	return nuTRUE;
}
MATHTOOL_API nuBOOL LibMT_BmpToMap(nuLONG nBmpX, nuLONG nBmpY, nuLONG *lpMapX, nuLONG *lpMapY)
{
	
	if( g_pMtFsApi->pGdata->transfData.nBmpWidthEx	== 0 ||
		g_pMtFsApi->pGdata->transfData.nBmpHeightEx	== 0 )
	{
		return nuFALSE;
	}
	
	nuLONG sx = ((nBmpX - g_pMtFsApi->pGdata->transfData.nBmpCenterX) * g_pMtFsApi->pGdata->transfData.nCosConst - 
			   (nBmpY - g_pMtFsApi->pGdata->transfData.nBmpCenterY) * g_pMtFsApi->pGdata->transfData.nSinConst) / 
			  LENCONST;
	nuLONG sy = ((nBmpY - g_pMtFsApi->pGdata->transfData.nBmpCenterY) * g_pMtFsApi->pGdata->transfData.nCosConst + 
			   (nBmpX - g_pMtFsApi->pGdata->transfData.nBmpCenterX) * g_pMtFsApi->pGdata->transfData.nSinConst ) / 
			  LENCONST;//
	*lpMapX = ( sx * g_pMtFsApi->pGdata->transfData.nMapWidth / MAPSCREEN ) + 
			  g_pMtFsApi->pGdata->transfData.nMapCenterX;
	*lpMapY = ((-sy) * g_pMtFsApi->pGdata->transfData.nMapHeight / MAPSCREEN ) + 
			  g_pMtFsApi->pGdata->transfData.nMapCenterY;
	/*
	if( *lpMapX < -XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		*lpMapX = -XBLOCKNUM * EACHBLOCKSIZE / 2;
		return nuFALSE;
	}
	else if( *lpMapX > XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		*lpMapX = XBLOCKNUM * EACHBLOCKSIZE / 2 ;
		return nuFALSE;
	}
	if( *lpMapY < -YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		*lpMapY = -YBLOCKNUM * EACHBLOCKSIZE / 2;
		return nuFALSE;
	}
	else if( *lpMapY > YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		*lpMapY = YBLOCKNUM * EACHBLOCKSIZE / 2;
		return nuFALSE;
	}
	*/
	return nuTRUE;
}

MATHTOOL_API nuBOOL LibMT_ZoomMapToBmp(nuLONG nMapX, nuLONG nMapY, nuLONG *lpBmpX, nuLONG *lpBmpY)
{
	nuLONG X = (nMapX - g_pMtFsApi->pGdata->zoomScreenData.nZoomMapCenterX) * MAPSCREEN / 
			 g_pMtFsApi->pGdata->zoomScreenData.nZoomMapWidth;
	nuLONG Y = (g_pMtFsApi->pGdata->zoomScreenData.nZoomMapCenterY - nMapY) * MAPSCREEN / 
			 g_pMtFsApi->pGdata->zoomScreenData.nZoomMapHeight;
	*lpBmpX = (X * g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDy + Y * g_pMtFsApi->pGdata->zoomScreenData.nZoomAgnleDx) / 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDis + g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterX;
	*lpBmpY = (Y * g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDy - X * g_pMtFsApi->pGdata->zoomScreenData.nZoomAgnleDx) / 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDis + g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterY;
	return nuTRUE;
}
MATHTOOL_API nuBOOL LibMT_ZoomBmpToMap(nuLONG nBmpX, nuLONG nBmpY, nuLONG *lpMapX, nuLONG *lpMapY)
{
	nuLONG sx = ((nBmpX - g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterX) * g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDy - 
			   (nBmpY - g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterY) * g_pMtFsApi->pGdata->zoomScreenData.nZoomAgnleDx) / 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDis;
	nuLONG sy = ((nBmpY - g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterY) * g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDy + 
			   (nBmpX - g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpCenterX) * g_pMtFsApi->pGdata->zoomScreenData.nZoomAgnleDx ) / 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomAngleDis;//
	*lpMapX = ( sx * g_pMtFsApi->pGdata->zoomScreenData.nZoomMapWidth / MAPSCREEN ) + 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomMapCenterX;
	*lpMapY = ((-sy) * g_pMtFsApi->pGdata->zoomScreenData.nZoomMapHeight / MAPSCREEN ) + 
			  g_pMtFsApi->pGdata->zoomScreenData.nZoomMapCenterY;
	/*
	if( *lpMapX < -1800*EACHBLOCKSIZE )
	{
		*lpMapX = -1800*EACHBLOCKSIZE;
		return nuFALSE;
	}
	else if( *lpMapX > 1800*EACHBLOCKSIZE )
	{
		*lpMapX = 1800*EACHBLOCKSIZE ;
		return nuFALSE;
	}
	if( *lpMapY < -900*EACHBLOCKSIZE )
	{
		*lpMapY = -900*EACHBLOCKSIZE;
		return nuFALSE;
	}
	else if( *lpMapY > 900*EACHBLOCKSIZE )
	{
		*lpMapY = 900*EACHBLOCKSIZE;
		return nuFALSE;
	}*/
	return nuTRUE;
}


// Modified by Dengxu @ 2012 11 02
//#define _SKY_GRADINET_LEN			30
MATHTOOL_API nuBOOL LibMT_SaveOrShowBmp(nuBYTE nBmpType)
{
	nuINT nSrcy = 0;
	switch(nBmpType)
	{
		case SAVETOMEM_BMP1: 
			return g_pMtGdiApi->GdiSaveBMP( &g_pMtFsApi->pGdata->transfData.nuBmpMem1,
										0, 
										0,
										g_pMtFsApi->pGdata->transfData.nuBmpMem1.bmpWidth,
										g_pMtFsApi->pGdata->transfData.nuBmpMem1.bmpHeight,
										0,
										0 );
		case SAVETOMEM_BMP2:
			break;
		case SHOWFROMMEM_BMP1:
			return g_pMtGdiApi->GdiDrawBMP( 0, 
											0, 
											g_pMtFsApi->pGdata->uiSetData.nScreenWidth,
											g_pMtFsApi->pGdata->uiSetData.nScreenHeight,
											&g_pMtFsApi->pGdata->transfData.nuBmpMem1, 
											g_pMtFsApi->pGdata->transfData.nBmpLTx,
											g_pMtFsApi->pGdata->transfData.nBmpLTy );
		case SHOWFROMMEM_BMP2:
			break;
		case SHOW_3D_BMP:
			//	nSrcy = g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight - g_pMtFsApi->pGdata->uiSetData.nSkyHeight;
			//	if( nSrcy < 0 )
			//	{
			//		nSrcy = 0;
			//	}
			//	if( g_pMtFsApi->pGdata->uiSetData.nSkyHeight > _sky_gradient_len )
			{
				nuINT curr_offset = s_MaxSkyHeight - g_pMtFsApi->pGdata->uiSetData.nSkyHeight;
				nuINT _sky_gradient_len = g_SKY_GRADIENT_LEN;
				nuINT bmpheight = g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmp3D.bmpHeight;
				/*__android_log_print(ANDROID_LOG_INFO, "navi", "sky bmpheight %d", bmpheight);
				__android_log_print(ANDROID_LOG_INFO, "navi", "sky bmpWidth %d", g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmp3D.bmpWidth);
				__android_log_print(ANDROID_LOG_INFO, "navi", "sky Screen %d", g_pMtFsApi->pGdata->uiSetData.nScreenWidth);
__android_log_print(ANDROID_LOG_INFO, "navi", "sky _sky_gradient_len %d, curr_offset %d, s_MaxSkyHeight %d", _sky_gradient_len, curr_offset, s_MaxSkyHeight);*/
				g_pMtGdiApi->GdiDrawBMP( 0, 
										 0, 
										 g_pMtFsApi->pGdata->uiSetData.nScreenWidth,
										 bmpheight - _sky_gradient_len - curr_offset,
										 &g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmp3D, 
										 0,
										 curr_offset );

				nuBOOL bRet = g_pMtGdiApi->GdiBmpGradient( 0, 
											 bmpheight - _sky_gradient_len - curr_offset,
											 g_pMtFsApi->pGdata->uiSetData.nScreenWidth,
											 _sky_gradient_len,
											 &g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmp3D,
											 0,
					                         bmpheight - _sky_gradient_len, 
											 0 );

				return bRet;
			}
		case PAINT_NUROBMP:
			g_pMtGdiApi->GdiPaintBMP( 0, 
									  0, 
									0, 
									0, 
									&g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmpBg );
			break;
		case PAINT_NUROCROSSBMP:
			if( g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmpCrossBg.bmpHasLoad )
			{
				g_pMtGdiApi->GdiPaintBMP( 0, 
					0, 
					0, 
					0, 
					&g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmpCrossBg );
			}
			else
			{
				g_pMtGdiApi->GdiPaintBMP( 0, 
					0, 
					0, 
					0, 
					&g_pMtFsApi->pDrawInfo->drawInfoCommon.nuBmpBg );
			}
			break;
		case SAVE_SHOW_MAP_TO_BMP1:
			return g_pMtGdiApi->GdiSaveBMP( &g_pMtFsApi->pGdata->transfData.nuBmpMem1,
										0, 
										0,
										g_pMtFsApi->pGdata->transfData.nBmpWidth,
										g_pMtFsApi->pGdata->transfData.nBmpHeight,
										0,
										0 );
		case SAVE_SHOW_MAP_TO_BMP2:
			return g_pMtGdiApi->GdiSaveBMP( &g_pMtFsApi->pGdata->transfData.nuBmpMem2,
										0, 
										0,
										g_pMtFsApi->pGdata->transfData.nBmpWidth,
										g_pMtFsApi->pGdata->transfData.nBmpHeight,
										0,
										0 );
		case SHOW_MAP_SCREEN:
			return g_pMtGdiApi->GdiFlush(  g_pMtFsApi->pGdata->uiSetData.nScreenLTx,
									   g_pMtFsApi->pGdata->uiSetData.nScreenLTy,
									   g_pMtFsApi->pGdata->transfData.nBmpWidth,
									   g_pMtFsApi->pGdata->transfData.nBmpHeight,
									   0,
									   0 );
		case SHOW_BMP1_SCREEN:
			return g_pMtGdiApi->GdiFlush(  g_pMtFsApi->pGdata->uiSetData.nScreenLTx,
										g_pMtFsApi->pGdata->uiSetData.nScreenLTy,
										g_pMtFsApi->pGdata->transfData.nBmpWidth,
										g_pMtFsApi->pGdata->transfData.nBmpHeight,
										g_pMtFsApi->pGdata->transfData.nBmpLTx,
										g_pMtFsApi->pGdata->transfData.nBmpLTy );
		case SAVE_ZOOM_MAP:
			return g_pMtGdiApi->GdiSaveBMP( &g_pMtFsApi->pGdata->zoomScreenData.nuZoomBmp,
										0,
										0,
										g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpWidth,
										g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpHeight,
										0,
										0 );
		case SHOW_ZOOM_MAP_TO_NURODC:
			return g_pMtGdiApi->GdiDrawBMP( g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.left,
										g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.top,
										g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.right - g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.left,
										g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.bottom - g_pMtFsApi->pGdata->zoomScreenData.rtZoomScreen.top,
										&g_pMtFsApi->pGdata->zoomScreenData.nuZoomBmp,
										0,
										0 );
		case SHOW_DEFAULT:
		default:
			return g_pMtGdiApi->GdiFlush(	g_pMtFsApi->pGdata->uiSetData.nScreenLTx,
										g_pMtFsApi->pGdata->uiSetData.nScreenLTy,
										g_pMtFsApi->pGdata->uiSetData.nScreenWidth,
										g_pMtFsApi->pGdata->uiSetData.nScreenHeight,
										0, 
										0 );
	}
	return nuTRUE;
}

MATHTOOL_API nuINT LibMT_SelectObject(nuBYTE nDrawType, nuBYTE nShowMode, const nuVOID* lpSet, nuLONG *pOut)
{
	return g_pGdiObject->SelectObject(nDrawType, nShowMode, lpSet, pOut);
}
MATHTOOL_API nuBOOL LibMT_DrawObject(nuINT nObjType, PNUROPOINT pPt, nuINT nCount, nuLONG nExtend)
{
	return g_pGdiObject->DrawObject(nObjType, pPt, nCount, nExtend);
}
MATHTOOL_API nuVOID LibMT_Bmp2Dto3D(nuLONG& x, nuLONG &y, nuBYTE nMapType)
{
	nuLONG z = 0;
	g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(x, y, z);
//Old	g_pGdiObject->Bmp2Dto3D(x, y, nMapType);
}
MATHTOOL_API nuVOID LibMT_Bmp3Dto2D(nuLONG& x, nuLONG &y, nuBYTE nMapType)
{
	nuLONG z = 0;
	g_pObj3DDraw->m_obj3DCtrl.Screen3Dto2D(x, y, z);
//nuLONG	g_pGdiObject->Bmp3Dto2D(x, y, nMapType);
}
MATHTOOL_API nuLONG LibMT_Get3DThick(nuLONG thick, nuLONG y)
{
	return g_pObj3DDraw->m_obj3DCtrl.Get3DThick(thick, y);
	return g_pGdiObject->m_viewTransf.Get3DThick(thick, y);
}
MATHTOOL_API nuVOID LibMT_Get3DParam(PCTRL_3D_PARAM pCtrl3DParam)
{
	if( NULL != pCtrl3DParam && NULL != g_pObj3DDraw )
	{
		pCtrl3DParam->bScale	= g_pObj3DDraw->m_obj3DCtrl.m_bScale;
		pCtrl3DParam->nLookAtx	= g_pObj3DDraw->m_obj3DCtrl.m_nViewX;
		pCtrl3DParam->nLookAty	= g_pObj3DDraw->m_obj3DCtrl.m_nViewY;
		pCtrl3DParam->nLookAtz	= g_pObj3DDraw->m_obj3DCtrl.m_nViewZ;
		pCtrl3DParam->nLookTox	= g_pObj3DDraw->m_obj3DCtrl.m_nViewX;
		pCtrl3DParam->nLookToy	= g_pObj3DDraw->m_obj3DCtrl.m_nDy;
		pCtrl3DParam->nLookToz	= 0;
		pCtrl3DParam->nScaleC	= g_pObj3DDraw->m_obj3DCtrl.m_nScaleC;
		pCtrl3DParam->nScaleD	= g_pObj3DDraw->m_obj3DCtrl.m_nScaleD;	
		pCtrl3DParam->nDz		= g_pObj3DDraw->m_obj3DCtrl.m_nDz;
	}
}
