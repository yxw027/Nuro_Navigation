// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: GDataMgr.cpp,v 1.39 2010/10/11 05:54:22 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/10/11 05:54:22 $
// $Locker:  $
// $Revision: 1.39 $
// $Source: /home/nuCode/libNaviCtrl/GDataMgr.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "GDataMgr.h"
#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"
#include "nuTTSdefine.h"

extern GDIAPI			g_gdiApi;
extern MATHTOOLAPI		g_mathtoolApi;
extern FILESYSAPI		g_fileSysApi;
extern MEMMGRAPI		g_memMgrApi;
extern DRAWINFOMAPAPI	g_drawIfmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _ZK_GD_CENTER_SCALE				20


#define _CROSS_ZOOM_DIS				200
#define _CROSS_ZOOM_SCALE			100
#define _3D_VIEW_ANGLE_STEP			0

CGDataMgr::CGDataMgr()
{

}

CGDataMgr::~CGDataMgr()
{
	Free();
}

nuBOOL CGDataMgr::Initialize()
{
	m_nSpeedWarningTimer = 0;
	m_nMinuteTimer		= 0;
	m_nNowCrossNearType = CROSSNEARTYPE_DEFAULT;
	m_nNowCrossZoomType	= CROSSNEARTYPE_DEFAULT;
	m_bCrossZoomMap		= nuFALSE;
	m_nOldScaleIdx		= 0;
	m_nSplitScreenTye	= SPLIT_SCREEN_AUTO_CLOSE;
	nuLONG tmp			= 0;
	if( !nuReadConfigValue("SPEEDAUTOZOOM", &m_nSpeedAutoZoom) )
	{
		m_nSpeedAutoZoom	= 80;
	}
	if( !nuReadConfigValue("CARPOSITION", &tmp) )
	{
		tmp = 4;
	}
	m_nCarPosition = nuLOWORD(tmp);

	if( nuReadConfigValue(("MAPCENTERY"), &tmp) )
	{
		m_nCenterScale = (nuINT)(_ZK_GD_CENTER_SCALE * nuCos( tmp / 100000 ) + 0.5);
		if( m_nCenterScale < 0 )
		{
			m_nCenterScale = -m_nCenterScale;
		}
		else if( m_nCenterScale == 0 )
		{
			m_nCenterScale = 1;
		}
	}
	else
	{
		m_nCenterScale = _ZK_GD_CENTER_SCALE;
	}

	if( !nuReadConfigValue("CROSSZOOMDIS", &tmp) )
	{
		tmp = _CROSS_ZOOM_DIS;
	}
	m_nCrossZoomDis = (nuWORD)tmp;
	if( !nuReadConfigValue("CROSSZOOMANGLE", &tmp) )
	{
		tmp = _3D_VIEW_ANGLE_STEP;
	}
	m_n3DAngleStep = (nuBYTE)tmp;
	if( !nuReadConfigValue("CROSSZOOMSCALE", &tmp) )
	{
		tmp = _CROSS_ZOOM_SCALE;
	}
	m_nCrossZoomScale = (nuWORD)tmp;
	if( !nuReadConfigValue("ROTATEMAPANGLE", &m_nRotateMapAngle) )
	{
		 m_nRotateMapAngle = 25;
	}
	ZoomMapInit();
	m_nRoadIdx = -2;
	m_bOverSpeed = nuFALSE;
	return nuTRUE;
}

nuVOID CGDataMgr::Free()
{
	if( g_fileSysApi.pGdata != NULL )
	{
		FreeMemBitmap();
	}
	ZoomMapFree();
}

#define MIN_ANGLE_ROTATE_MAP				2
#define MAX_ANGLE_ROTATE_MAP				15


#define TIME_SPEEDCOUNT					5
nuBOOL CGDataMgr::SetUp(PACTIONSTATE pAcState)
{
	if( g_fileSysApi.pGdata == NULL )
	{
		return nuFALSE;
	}
//	g_fileSysApi.pGdata->uiSetData.nMapDirectMode = MAP_DIRECT_NORTH;
	nuBOOL bSetBmpSize	 = nuFALSE;
	nuBOOL bSetBmpCenter = nuFALSE;
	//Screen size changed, Create memory bitmap
	nuBOOL bResetMemBmpSize = nuFALSE;
	if( pAcState->bResetScreenSize )
	{
		/*DIB bitmap*/
#ifdef _USE_DDB_BITMAP
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth		= g_fileSysApi.pGdata->uiSetData.nScreenWidth + SCREEN_HORIZONTAL_EXTEND;
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight + SCREEN_VERTICAL_EXTEND;
#else
		//g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpBitCount	= 24;
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth		= g_fileSysApi.pGdata->uiSetData.nScreenWidth + SCREEN_HORIZONTAL_EXTEND;
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight + SCREEN_VERTICAL_EXTEND;
#endif
		//
		if( !g_gdiApi.GdiInitBMP(g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth, g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight) )
		{
			return nuFALSE;
		}
		bSetBmpSize	= nuTRUE;
	}
	else//Judge whether it needs to reset BmpSize
	{
		/* 20081124
		if( pAcState->nBmpSizeMode != g_fileSysApi.pGdata->transfData.nBmpSizeMode )
		{
			g_fileSysApi.pGdata->transfData.nBmpSizeMode = pAcState->nBmpSizeMode;
			bSetBmpSize = nuTRUE;
		}
		*/
	}
	//the size of the map's bitmap has changed, so need to reset the BmpSize and BmpCenter
	nuBOOL bSet3DParam = nuFALSE;
	if( bSetBmpSize || pAcState->bResetMemBmpSize/* || pAcState->bResetMapCenter*/)
	{
		//Set BmpSize, BmpExtended size and ZoomBmp size
		pAcState->bDrawZoom = g_fileSysApi.pGdata->zoomScreenData.bZoomScreen;
		SetMapBitmapSize();
		//need know the Carcentermode for resetting the BmpCenterX Y.
		if( pAcState->nCarCenterMode != CAR_CENTER_MODE_REMAIN )//Have new CarCenterMode
		{
			g_fileSysApi.pGdata->transfData.nCarCenterMode = pAcState->nCarCenterMode;
		}
		pAcState->bDrawZoom = (~pAcState->bDrawZoom) & g_fileSysApi.pGdata->zoomScreenData.bZoomScreen;
		bResetMemBmpSize = nuTRUE;
		bSetBmpCenter = nuTRUE;
		bSet3DParam	= nuTRUE;
	}
	else//Judge whether there is a new car center mode.
	{
		if( pAcState->nCarCenterMode != CAR_CENTER_MODE_REMAIN &&
			pAcState->nCarCenterMode != g_fileSysApi.pGdata->transfData.nCarCenterMode )
		{
			g_fileSysApi.pGdata->transfData.nCarCenterMode = pAcState->nCarCenterMode;
			bSetBmpCenter = nuTRUE;
		}
	}
	//
	if( bResetMemBmpSize )
	{
		AllocMemBitmap();
		pAcState->bNeedReloadMap = 1;
	}
	//
	//
	if( pAcState->bDrawBmp1 )
	{
		SetBmpExtend(pAcState->nBmpSizeMode);
		SetBmpCenter();
	}
	//////////////////////////////////////////////////////////////////////////////////////
	//Set Map info
	nuBOOL bSetMapSize = pAcState->bResetScale;
	if( pAcState->bResetMapCenter )//Reset the map center
	{
		g_fileSysApi.pGdata->transfData.nMapCenterX	= pAcState->ptNewMapCenter.x;
		g_fileSysApi.pGdata->transfData.nMapCenterY	= pAcState->ptNewMapCenter.y;
		if( !pAcState->bGlideMoveDraw )
		{
			bSetMapSize = nuTRUE;
		}
	}
	if( pAcState->bResetIconCarPos )//Reset the car position, It is GPS or simulation drawing.
	{
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarIcon, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
	}
	if( pAcState->bResetRealCarPos )//GPS drawing, reset the real car position.
	{
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarRaw, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarFixed, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
	}
	if( bSetMapSize )
	{
		SetMapSize( g_fileSysApi.pGdata->uiSetData.nScaleValue );
	}
	if( bSet3DParam /*|| pAcState->bResetScale*/ )//3D Parameter
	{
		g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_KEEP, 0);
	}
	//Set angle infomation
//	nuBOOL bSetAngleParameter = nuFALSE;
	if( (pAcState->nDrawMode & 0xF0) == DRAWMODE_GPS || 
		(pAcState->nDrawMode & 0xF0) == DRAWMODE_SIM )
	{
		if( pAcState->nAngle >= 0 )
		{
			pAcState->nAngle %= 360;
			//-----------added by daniel 20120131-------------------//
			if((pAcState->nDrawMode & 0xF0) == DRAWMODE_GPS)
			{
				g_fileSysApi.pGdata->carInfo.nCarAngleRaw = pAcState->nAngle;
			}
			//-----------added by daniel 20120131-------------------//
			g_fileSysApi.pGdata->carInfo.nCarAngle = pAcState->nAngle;//Set Car angle
			if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_CAR &&
				g_fileSysApi.pGdata->carInfo.nMapAngle != pAcState->nAngle )
			{
				nuSHORT nDif = g_fileSysApi.pGdata->carInfo.nMapAngle - pAcState->nAngle;
				nDif = NURO_ABS(nDif);
				if( nDif > 180 )
				{
					nDif = 360 - nDif;
				}

				if( nDif >= m_nRotateMapAngle && pAcState->nDrawMode != DRAWMODE_SIM_START )
				{
					//modify for fixroad
					nuSHORT nDiff = g_fileSysApi.pGdata->carInfo.nMapAngle - pAcState->nAngle;
					if( nDiff > 0 )
					{
						if( nDiff < 180 )
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle -= m_nRotateMapAngle;
						}
						else
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle += m_nRotateMapAngle;
						}
					}
					else
					{
						nDiff = NURO_ABS(nDiff);
						if( nDiff < 180 )
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle += m_nRotateMapAngle;
						}
						else
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle -= m_nRotateMapAngle;
						}
					}
					if( g_fileSysApi.pGdata->carInfo.nMapAngle < 0 )
					{
						g_fileSysApi.pGdata->carInfo.nMapAngle += 360;
					}
					else if( g_fileSysApi.pGdata->carInfo.nMapAngle >= 360 )
					{
						g_fileSysApi.pGdata->carInfo.nMapAngle = g_fileSysApi.pGdata->carInfo.nMapAngle % 360;
					}
					pAcState->bNeedExtendRotate = 1;
//					pAcState->bSetAngle = nuTRUE;
				}
				else if( nDif >= MIN_ANGLE_ROTATE_MAP )
				{
					g_fileSysApi.pGdata->carInfo.nMapAngle = pAcState->nAngle;
				}
				pAcState->bSetAngle = nuTRUE;
			}//Set Map angle
//@Zhikun 2009.08.19			pAcState->nAngle = g_fileSysApi.pGdata->carInfo.nMapAngle;
		}
		if( g_fileSysApi.pGdata->carInfo.nRotateAngle != 0 )
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle = 0;
			pAcState->bSetAngle = nuTRUE;
		}
//		if( pAcState->bDrawZoom )
		{
			pAcState->bSetAngle = nuTRUE;
		}
	}
	else if( pAcState->nDrawMode == DRAWMODE_POWERON )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle = pAcState->nAngle;
		pAcState->bSetAngle = nuTRUE;
	}
	else if( pAcState->nDrawMode == DRAWMODE_UI )//UI rorate map
	{
		if( pAcState->nAngle >= 0 )
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle = pAcState->nAngle;
			pAcState->bSetAngle = nuTRUE;
		}
	}
	if( pAcState->bSetAngle )
	{
		SetAngleParameter();
	}
	if( pAcState->nDrawMode != DRAWMODE_STOP )
	{
		g_fileSysApi.pGdata->carInfo.nShowSpeed = pAcState->nSpeed;
		if( pAcState->nDrawMode == DRAWMODE_GPS )
		{
			g_fileSysApi.pGdata->carInfo.nCarSpeed	= pAcState->nSpeed;
			g_fileSysApi.pGdata->carInfo.nMiddleSpeed = (g_fileSysApi.pGdata->carInfo.nMiddleSpeed * TIME_SPEEDCOUNT 
				+ g_fileSysApi.pGdata->carInfo.nCarSpeed + TIME_SPEEDCOUNT) / (TIME_SPEEDCOUNT + 1);
//			g_fileSysApi.pGdata->carInfo.nShowSpeed = g_fileSysApi.pGdata->carInfo.nMiddleSpeed;
		}
	}
	return nuTRUE;
}

nuBOOL CGDataMgr::FixUp(PACTIONSTATE pAcState)
{
	if( pAcState->bResetMapCenter )//Reset the map center
	{
		g_fileSysApi.pGdata->transfData.nMapCenterX	= pAcState->ptNewMapCenter.x;
		g_fileSysApi.pGdata->transfData.nMapCenterY	= pAcState->ptNewMapCenter.y;
	}
	//
	if( pAcState->bResetIconCarPos )
	{
		g_fileSysApi.pGdata->carInfo.ptCarIcon.x		= pAcState->ptNewMapCenter.x;
		g_fileSysApi.pGdata->carInfo.ptCarIcon.y		= pAcState->ptNewMapCenter.y;
	}
	if( pAcState->bResetRealCarPos )
	{
		g_fileSysApi.pGdata->carInfo.ptCarFixed.x		= pAcState->ptNewMapCenter.x;
		g_fileSysApi.pGdata->carInfo.ptCarFixed.y		= pAcState->ptNewMapCenter.y;
	}
	return nuTRUE;
}

nuVOID CGDataMgr::SetMapBitmapSize(nuINT nLtExd, nuINT nUpExd, nuINT nRtExd, nuINT nDnExd)
{
	switch( g_fileSysApi.pGdata->uiSetData.nScreenType )
	{
		case SCREEN_TYPE_TWO://1:1
			g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth/2;
			g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		 	g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
			g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
			g_fileSysApi.pGdata->zoomScreenData.bZoomScreen	= 1;
			break;
		case SCREEN_TYPE_THREE://2:1
			g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth*2/3;
			g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		 	g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
			g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
			g_fileSysApi.pGdata->zoomScreenData.bZoomScreen		= 1;
			break;
		case SCREEN_TYPE_ONE://whole screen
		case SCREEN_TYPE_DEFAULT://NO Split screen
		default:
			g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth;
			g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		 	g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
			g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
			g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
			g_fileSysApi.pGdata->zoomScreenData.bZoomScreen	= 0;
			break;
	}

	g_fileSysApi.pGdata->transfData.nBmpWidthEx		= g_fileSysApi.pGdata->uiSetData.nScreenWidth  + SCREEN_HORIZONTAL_EXTEND;//g_fileSysApi.pGdata->transfData.nBmpWidth  + SCREEN_HORIZONTAL_EXTEND;
	g_fileSysApi.pGdata->transfData.nBmpHeightEx	= g_fileSysApi.pGdata->uiSetData.nScreenHeight + SCREEN_VERTICAL_EXTEND;//g_fileSysApi.pGdata->transfData.nBmpHeight + SCREEN_VERTICAL_EXTEND;
//	SetBmpExtend(g_fileSysApi.pGdata->transfData.nBmpSizeMode);
	/*
	if( g_fileSysApi.pGdata->transfData.nBmpSizeMode == BMPSIZE_EXTEND_NO )
	{
		g_fileSysApi.pGdata->transfData.nBmpLTx	= 0;
		g_fileSysApi.pGdata->transfData.nBmpLTy	= 0;
	}
	else
	{
		if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_LEFT )
		{
			g_fileSysApi.pGdata->transfData.nBmpLTx		= nLtExd;
			g_fileSysApi.pGdata->transfData.nBmpWidthEx	+= nLtExd;
		}
		if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_UP )
		{
			g_fileSysApi.pGdata->transfData.nBmpLTy		= nUpExd;
			g_fileSysApi.pGdata->transfData.nBmpHeightEx	+= nUpExd;
		}
		if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_RIGHT )
		{
			g_fileSysApi.pGdata->transfData.nBmpWidthEx	+= nRtExd;
		}
		if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_DOWN )
		{
			g_fileSysApi.pGdata->transfData.nBmpHeightEx	+= nDnExd;
		}
	}
	*/
}

nuVOID CGDataMgr::SetBmpExtend(nuBYTE nBmpSizeMode /* = BMPSIZE_EXTEND_NO */)
{
	g_fileSysApi.pGdata->transfData.nBmpLTx		= 0;
	g_fileSysApi.pGdata->transfData.nBmpLTy		= 0;
	if( g_fileSysApi.pGdata->transfData.nBmpSizeMode != nBmpSizeMode )
	{
		g_fileSysApi.pGdata->transfData.nBmpSizeMode = nBmpSizeMode;
	}
	if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_LEFT )
	{
		g_fileSysApi.pGdata->transfData.nBmpLTx		= SCREEN_HORIZONTAL_EXTEND;
	}
	if( g_fileSysApi.pGdata->transfData.nBmpSizeMode & BMPSIZE_EXTEND_UP )
	{
		g_fileSysApi.pGdata->transfData.nBmpLTy		= SCREEN_VERTICAL_EXTEND;
	}
}

nuVOID CGDataMgr::SetMapSize(nuLONG nScale)
{
	g_fileSysApi.pGdata->uiSetData.nScaleValue = nScale;
	nuDWORD nWidth = /*g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.*/nScale * COORCONSTANT * MAPSCREEN;
	if( /*g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.*/nScale <= 20000 )
	{
		nuFLOAT fLen = (nuFLOAT)(g_fileSysApi.pGdata->drawInfoMap.nScalePixels * nuCos( g_fileSysApi.pGdata->transfData.nMapCenterY / 100000 ));
		fLen = NURO_ABS(fLen);
		g_fileSysApi.pGdata->transfData.nMapWidth = (nuDWORD)(nWidth/fLen);
	}
	else
	{
		g_fileSysApi.pGdata->transfData.nMapWidth = nWidth * _ZK_GD_CENTER_SCALE / 
			( g_fileSysApi.pGdata->drawInfoMap.nScalePixels * m_nCenterScale);
	}
	
	nWidth = /*g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.*/nScale * COORCONSTANT * MAPSCREEN / 
		g_fileSysApi.pGdata->drawInfoMap.nScalePixels;
	g_fileSysApi.pGdata->transfData.nMapHeight = (nuDWORD)(nWidth);

	/* 屏幕宽度的值应该随屏幕y坐标的不同而不同。在一个屏幕范围所对应的地图
	 * 范围内，nMapWidth的值应该有很多个，到底有多少个应该和所要显示的地图的
	 * 南北宽度和所处的纬度有一定的关系。
	 */
}

nuVOID CGDataMgr::SetAngleParameter()
{
	{
		nuLONG nShowAngle = g_fileSysApi.pGdata->carInfo.nMapAngle + g_fileSysApi.pGdata->carInfo.nRotateAngle;
		while( nShowAngle < 0 )
		{
			nShowAngle += 360;
		}
		//compensate the map angle
		nShowAngle %= 360;
		/*
		if( nShowAngle < 90 )
		{
			nShowAngle += 2;
		}
		else if( nShowAngle < 180 )
		{
			nShowAngle += 1;
		}
		else if( nShowAngle < 267 )
		{
			nShowAngle += 4;
		}
		else
		{
			nShowAngle += 0;
		}
		*/
		g_fileSysApi.pGdata->transfData.nCosConst = (nuLONG)(LENCONST * nuSin(nShowAngle) + 0.5);
		g_fileSysApi.pGdata->transfData.nSinConst = (nuLONG)(LENCONST * nuCos(nShowAngle) + 0.5);
	}
}

nuVOID CGDataMgr::CalMapShowInfo(PACTIONSTATE pAcState)
{
	if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale >= 
		(nuDWORD)g_fileSysApi.pGdata->uiSetData.nBglStartScaleValue)
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = BSD_MODE_BGL;
	}
	else if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale >= 
		(nuDWORD)g_fileSysApi.pGdata->uiSetData.nBsdStartScaleVelue )
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = BSD_MODE_5k;
	}
	else
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = BSD_MODE_COMMON;
	}
	if( g_fileSysApi.pGdata->zoomScreenData.bZoomScreen && ( 
		/*!g_fileSysApi.pGdata->uiSetData.bBsdDrawMap  || */
		(pAcState->nDrawMode & 0xF0) == DRAWMODE_GPS || 
		(pAcState->nDrawMode & 0xF0) == DRAWMODE_SIM ) ) 
	{
		pAcState->bDrawZoom = 1;
	}
	CalShowRect(&g_fileSysApi.pGdata->transfData.nuShowMapSize);
	nuRectToBlockIDArray(g_fileSysApi.pGdata->transfData.nuShowMapSize, &g_fileSysApi.pGdata->transfData.blockIDShowArray);
}

nuVOID CGDataMgr::CalShowRect(PNURORECT lpRect)
{
	NUROPOINT Pt[4] = {0};
	NURORECT rect	= {0};
	nuLONG n		= 0;
	g_mathtoolApi.MT_BmpToMap( 0, 
						 -120, 
						 &(Pt[0].x),
						 &(Pt[0].y) );
	g_mathtoolApi.MT_BmpToMap( 0, 
						 g_fileSysApi.pGdata->transfData.nBmpHeightEx, 
						 &(Pt[1].x),
						 &(Pt[1].y));
	g_mathtoolApi.MT_BmpToMap( g_fileSysApi.pGdata->transfData.nBmpWidthEx,
						 -120, 
						 &(Pt[2].x), 
						 &(Pt[2].y));
	g_mathtoolApi.MT_BmpToMap( g_fileSysApi.pGdata->transfData.nBmpWidthEx, 
						 g_fileSysApi.pGdata->transfData.nBmpHeightEx,
						 &(Pt[3].x), 
						 &(Pt[3].y) );
	if( NURO_ABS( Pt[0].x - Pt[3].x ) >= NURO_ABS( Pt[1].x - Pt[2].x ) )
	{
		rect.left   = Pt[0].x;
		rect.right  = Pt[3].x;
		rect.top    = Pt[2].y;
		rect.bottom = Pt[1].y;
	}
	else
	{
		rect.left   = Pt[1].x;
		rect.right  = Pt[2].x;
		rect.top    = Pt[0].y;
		rect.bottom = Pt[3].y;
	}
	if( rect.left > rect.right )
	{
		n = rect.left;
		rect.left  = rect.right;
		rect.right = n;
	}
	if(  rect.top > rect.bottom )
	{
		n = rect.bottom;
		rect.bottom = rect.top;
		rect.top = n;
	}
	if( rect.left < -XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.left = -XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.right > XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.right = XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.top < -YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.top = -YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.bottom > YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.bottom = YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	lpRect->left	= rect.left;
	lpRect->top		= rect.top;
	lpRect->right	= rect.right;
	lpRect->bottom	= rect.bottom;
}

nuVOID CGDataMgr::CalZoomShowRect(PNURORECT lpRect)
{
	NUROPOINT Pt[4] = {0};
	NURORECT rect   = {0};
	nuLONG n		= 0;
	g_mathtoolApi.MT_ZoomBmpToMap( 0, 
							 0, 
							 &(Pt[0].x),
							 &(Pt[0].y) );
	g_mathtoolApi.MT_ZoomBmpToMap( 0, 
							 g_fileSysApi.pGdata->zoomScreenData.nZoomBmpHeight, 
							 &(Pt[1].x),
							 &(Pt[1].y));
	g_mathtoolApi.MT_ZoomBmpToMap( g_fileSysApi.pGdata->zoomScreenData.nZoomBmpWidth,
							 0, 
							 &(Pt[2].x), 
							 &(Pt[2].y));
	g_mathtoolApi.MT_ZoomBmpToMap( g_fileSysApi.pGdata->zoomScreenData.nZoomBmpWidth, 
							 g_fileSysApi.pGdata->zoomScreenData.nZoomBmpHeight,
							 &(Pt[3].x), 
							 &(Pt[3].y) );
	if( NURO_ABS( Pt[0].x - Pt[3].x ) >= NURO_ABS( Pt[1].x - Pt[2].x ) )
	{
		rect.left   = Pt[0].x;
		rect.right  = Pt[3].x;
		rect.top    = Pt[2].y;
		rect.bottom = Pt[1].y;
	}
	else
	{
		rect.left   = Pt[1].x;
		rect.right  = Pt[2].x;
		rect.top    = Pt[0].y;
		rect.bottom = Pt[3].y;
	}
	if( rect.left > rect.right )
	{
		n = rect.left;
		rect.left  = rect.right;
		rect.right = n;
	}
	if(  rect.top> rect.bottom )
	{
		n = rect.bottom;
		rect.bottom = rect.top;
		rect.top = n;
	}
	/*
	if( rect.left < -1800*EACHBLOCKSIZE )
	{
		rect.left = -1800*EACHBLOCKSIZE;
	}
	if( rect.right > 1800*EACHBLOCKSIZE )
	{
		rect.right = 1800*EACHBLOCKSIZE;
	}
	if( rect.top > 900*EACHBLOCKSIZE )
	{
		rect.top = 900*EACHBLOCKSIZE;
	}
	if( rect.bottom < -900*EACHBLOCKSIZE )
	{
		rect.bottom = -900*EACHBLOCKSIZE;
	}
	*/
	lpRect->left	= rect.left;
	lpRect->top		= rect.top;
	lpRect->right	= rect.right;
	lpRect->bottom	= rect.bottom;
}

nuBOOL CGDataMgr::AllocMemBitmap()
{
	FreeMemBitmap();
	//allocate memory from static memory in MemMgr.dll
	/* Not been used now
	*/
	nuINT nColorLen = (NURO_BMP_BITCOUNT+7)/8;
	g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth	= g_fileSysApi.pGdata->transfData.nBmpWidthEx;
	g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight = g_fileSysApi.pGdata->transfData.nBmpHeightEx;
	//g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpBitCount	= NURO_BMP_BITCOUNT;
	g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpBuffLen	= g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth * 
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight * nColorLen;
	g_fileSysApi.pGdata->transfData.nuBmpMem1.pBmpBuff = (nuPBYTE)g_memMgrApi.MM_GetStaticMemMass(g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpBuffLen);
	
	//***2011.01.10
	g_fileSysApi.pGdata->transfData.nuBmpMem1.bytesPerLine = g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth * 2;
	//***2011.01.10

	//..
	//g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpType		= NURO_BMP_TYPE_COPY;
	g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpReserve	= NURO_BMP_DIB_BUFFER;
	//g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHasLoad	= 1;
	//
	g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth;
	g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpHeight = g_fileSysApi.pGdata->uiSetData.nScreenHeight;
	//g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpBitCount	= NURO_BMP_BITCOUNT;
	g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpBuffLen = g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpWidth *
		g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpHeight * nColorLen;
	//g_fileSysApi.pGdata->transfData.nuBmpMem2.pBmpBuff = (nuPBYTE)g_memMgrApi.MM_GetStaticMemMass(g_fileSysApi.pGdata->transfData.nuBmpMem2.nBuffLen);
	g_fileSysApi.pGdata->transfData.nuBmpMem2.pBmpBuff = (nuPBYTE)g_memMgrApi.MM_GetStaticMemMass(g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpBuffLen);
	//..
	//g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpType		= NURO_BMP_TYPE_COPY;
	g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpReserve	= NURO_BMP_DIB_BUFFER;
	//g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpHasLoad	= 1;
	//

	//***2011.01.10
	g_fileSysApi.pGdata->transfData.nuBmpMem2.bytesPerLine = g_fileSysApi.pGdata->transfData.nuBmpMem2.bmpWidth * 2;
	//***2011.01.10

	return nuTRUE;
}

nuVOID CGDataMgr::FreeMemBitmap()
{
	//Free memory from static memory in MemMgr.dll
	g_memMgrApi.MM_RelStaticMemMassAfter(g_fileSysApi.pGdata->transfData.nuBmpMem1.pBmpBuff);
}

nuVOID CGDataMgr::SetBmpCenter()
{
	g_fileSysApi.pGdata->transfData.nBmpCenterX = g_fileSysApi.pGdata->transfData.nBmpLTx;
	g_fileSysApi.pGdata->transfData.nBmpCenterY = g_fileSysApi.pGdata->transfData.nBmpLTy;
	if( g_fileSysApi.pGdata->transfData.nCarCenterMode == CAR_CENTER_MODE_ONE_HALF_XY )
	{
		g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpWidth/2;
		g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	}
	else if( g_fileSysApi.pGdata->transfData.nCarCenterMode == CAR_CENTER_MODE_X_ONE_Y_THREE )
	{
		g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpWidth/4;
		g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight*3/4;
	}
	else
	{
		if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight/2;
		}
		else
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight*(m_nCarPosition-1)/m_nCarPosition;
		}
	}
}
/*
nuVOID CGDataMgr::MapRectToBlockIDArray(const PNURORECT lpRect, PBLOCKIDARRAY pBlockIDArray)
{
	pBlockIDArray->nXstart	= (lpRect->left + XBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nXend	= (lpRect->right + XBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nYstart	= (lpRect->top + YBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nYend	= (lpRect->bottom + YBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nXstart--;//Extend the number of the blocks
	pBlockIDArray->nYstart--;
	if( pBlockIDArray->nXstart < 0 )	pBlockIDArray->nXstart = 0;
	if( pBlockIDArray->nYstart < 0 )	pBlockIDArray->nYstart = 0;
}
*/

nuBOOL CGDataMgr::PointScreenToBmp(nuLONG &x, nuLONG &y)
{
	//Sceen point to Bitmap points.
	x -= g_fileSysApi.pGdata->uiSetData.nScreenLTx;
	y -= g_fileSysApi.pGdata->uiSetData.nScreenLTy;
	//
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		if( y < g_fileSysApi.pGdata->uiSetData.nSkyHeight )
		{
//			return nuFALSE;
			y = g_fileSysApi.pGdata->uiSetData.nSkyHeight - g_fileSysApi.pGdata->uiSetData.nScreenLTy;
		}
	}
	//if bitmap is extended, extend to point axis
	x += g_fileSysApi.pGdata->transfData.nBmpLTx;
	y += g_fileSysApi.pGdata->transfData.nBmpLTy;
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp3Dto2D(x, y, MAP_DEFAULT);
	}
	return nuTRUE;
}

nuBOOL CGDataMgr::GetScreenCenter(nuLONG &x, nuLONG &y)
{
	x = g_fileSysApi.pGdata->transfData.nBmpLTx + g_fileSysApi.pGdata->transfData.nBmpWidth / 2;
	y = g_fileSysApi.pGdata->transfData.nBmpLTy + g_fileSysApi.pGdata->transfData.nBmpHeight / 2;
	return nuTRUE;
}

nuBOOL CGDataMgr::SetAngle(nuLONG nAngle, nuBYTE nAngleType/* = ANGLE_TYPE_ROTATE*/)
{
	if( nAngleType == ANGLE_TYPE_ROTATE )
	{
		if( nAngle == 0 )
		{
//			g_fileSysApi.pGdata->carInfo.nRotateAngle	= g_fileSysApi.pGdata->carInfo.nCarAngle - g_fileSysApi.pGdata->carInfo.nMapAngle;
			g_fileSysApi.pGdata->carInfo.nRotateAngle	= 0;
			g_fileSysApi.pGdata->carInfo.nMapAngle		= g_fileSysApi.pGdata->carInfo.nCarAngle;
		}
		else if( nAngle == 360 )
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle	= 0;//90 - g_fileSysApi.pGdata->carInfo.nMapAngle;
			g_fileSysApi.pGdata->carInfo.nMapAngle		= 90;
		}
		else
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle	= (g_fileSysApi.pGdata->carInfo.nRotateAngle + nAngle) % 360;
		}
		return nuTRUE;
	}
	else if( nAngleType == ANGLE_TYPE_CAR )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CGDataMgr::ZoomSet(nuINT nScale)
{
	nuDWORD nWidth = nScale * COORCONSTANT * MAPSCREEN;
	nuDOUBLE fLen = g_fileSysApi.pGdata->drawInfoMap.nScalePixels * nuCos(g_fileSysApi.pGdata->transfData.nMapCenterY / 100000);
	fLen = NURO_ABS(fLen);
	g_fileSysApi.pGdata->transfData.nMapWidth = (nuDWORD)(nWidth/fLen);
	nWidth = nScale * COORCONSTANT * MAPSCREEN / g_fileSysApi.pGdata->drawInfoMap.nScalePixels;
	g_fileSysApi.pGdata->transfData.nMapHeight = (nuDWORD)(nWidth);

//	g_fileSysApi.pGdata->transfData.nBmpLTx = 0;
//	g_fileSysApi.pGdata->transfData.nBmpLTy = 0;
	return nuTRUE;
}

nuBOOL CGDataMgr::SetDrawInfoMap(PACTIONSTATE pAcState)
{
	//Get car point in bitmap2
	if( pAcState->bResetIconCarPos || pAcState->bResetMapCenter )
	{
		g_mathtoolApi.MT_MapToBmp( g_fileSysApi.pGdata->carInfo.ptCarIcon.x, 
			g_fileSysApi.pGdata->carInfo.ptCarIcon.y,
			&g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x,
			&g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y );
		g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x -= g_fileSysApi.pGdata->transfData.nBmpLTx;
		g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y -= g_fileSysApi.pGdata->transfData.nBmpLTy;
		if( g_fileSysApi.pGdata->uiSetData.b3DMode )
		{
			g_mathtoolApi.MT_Bmp2Dto3D( g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x, 
				g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y, 
				MAP_DEFAULT );
		}
		//calculate the car angle
		nuLONG nDis = g_fileSysApi.pGdata->drawInfoMap.nScaleValue * 5;
		nuLONG nx = g_fileSysApi.pGdata->carInfo.ptCarIcon.x + (nuLONG)( nDis * nuCos(g_fileSysApi.pGdata->carInfo.nCarAngle) );
		nuLONG ny = g_fileSysApi.pGdata->carInfo.ptCarIcon.y + (nuLONG)( nDis * nuSin(g_fileSysApi.pGdata->carInfo.nCarAngle) );
		g_mathtoolApi.MT_MapToBmp(nx, ny, &nx, &ny);
		if( g_fileSysApi.pGdata->uiSetData.b3DMode )
		{
			g_mathtoolApi.MT_Bmp2Dto3D(nx, ny, MAP_DEFAULT);
		}
		g_fileSysApi.pGdata->drawInfoMap.nCarAngleDx = nx - g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.x;
		g_fileSysApi.pGdata->drawInfoMap.nCarAngleDy = ny - g_fileSysApi.pGdata->drawInfoMap.ptCarInScreen.y;
		//Cross Zoom
		if( g_fileSysApi.pGdata->uiSetData.bNavigation && g_fileSysApi.pGdata->zoomScreenData.bSetZoomData )
		{
			g_mathtoolApi.MT_ZoomMapToBmp( g_fileSysApi.pGdata->carInfo.ptCarIcon.x,
				g_fileSysApi.pGdata->carInfo.ptCarIcon.y,
				&g_fileSysApi.pGdata->zoomScreenData.nCarInCrossX,
				&g_fileSysApi.pGdata->zoomScreenData.nCarInCrossY );
		}
	}
	//Get Map Center point, 
	g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.x = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
	g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.y = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
	if( g_fileSysApi.pGdata->uiSetData.b3DMode )
	{
		g_mathtoolApi.MT_Bmp2Dto3D( g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.x, g_fileSysApi.pGdata->drawInfoMap.ptMapCenterInScreen.y, MAP_DEFAULT);
	}
	//Set Scale value
	g_fileSysApi.pGdata->drawInfoMap.nScaleValue = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
	//Weather Back to Car button
	return nuTRUE;
}

#define OVER_SPEED_MAX_NUM					5
nuVOID CGDataMgr::OverSpeedCheck(nuWORD nWarningSlice)
{
	nuLONG bSpeedWarning = nuFALSE;
	m_bOverSpeed = nuFALSE;
	if(g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed <= 10)
	{
		return;
	}
	if(g_fileSysApi.pGdata->carInfo.nCarSpeed > (g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed*10))//
	{//禬硉璶冀厨
		bSpeedWarning = nuTRUE;
	}
	else
	{
		bSpeedWarning = nuFALSE;
	}
	if(bSpeedWarning)
	{
		if(g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx != m_nRoadIdx)
		{//禬硉琿隔ぃ冀厨
			m_nRoadIdx = g_fileSysApi.pGdata->drawInfoMap.roadCenterOn.nRoadIdx;
			nuWORD lNowTime = g_fileSysApi.pGdata->timerData.nHour * 60 * 60 + g_fileSysApi.pGdata->timerData.nMinute * 60 + g_fileSysApi.pGdata->timerData.nSecond;
			if(NURO_ABS(lNowTime - m_nMinuteTimer) > 15)
			{//竒冀厨15ずぃ冀厨added by daniel for avx pana 20110927
				m_nMinuteTimer = lNowTime;
			}
			else 
			{
				return;
			}
		}
		else
		{
			return ;
		}
	}
	else
	{
		m_nRoadIdx = -2;//繦獽倒ぉ把计⊿Τ疭瞇竡琌磷秨瞷ΤRoadIndex絪腹暗﹍て
		return ;
	}
	/*if( g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed != 0 &&
		g_fileSysApi.pGdata->carInfo.nCarSpeed > (g_fileSysApi.pGdata->drawInfoMap.roadCarOn.nLimitSpeed*10) )
	{
		m_nSpeedWarningTimer ++;
	}
	else
	{
		m_nSpeedWarningTimer = 0;
	}
	nuDWORD nNowTime = g_fileSysApi.pGdata->timerData.nHour * 60 * 60 + g_fileSysApi.pGdata->timerData.nMinute * 60 + g_fileSysApi.pGdata->timerData.nSecond;
	if( m_nSpeedWarningTimer > OVER_SPEED_MAX_NUM && (nNowTime - m_nMinuteTimer) >= nWarningSlice)*/
	{
		m_bOverSpeed = nuTRUE;
		if( g_fileSysApi.pUserCfg->nSpeedWarningVoice )
		{//冀厨
			g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 4, 0, 0, 0, NULL);
		}
		else
		{
			g_fileSysApi.pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_OVERSPEED, 0, NULL);
		}
		//m_nSpeedWarningTimer = 0;
		//m_nMinuteTimer		 = nNowTime;
	}
}

nuBOOL CGDataMgr::SetCrossNear(PACTIONSTATE pAcState, 
							 nuBYTE nCrossNearType /* = CROSSNEARTYPE_DEFAULT */, 
							 nuBYTE nCrossZoomtype /* = CROSSNEARTYPE_DEFAULT */ )
{
	//---------------------------------------------------------------------------//
	nuBOOL bRes = nuFALSE;
	nuBOOL bReLoadMapCfg = nuFALSE;
	nuLONG nScale = -1;
	if( g_fileSysApi.pUserCfg->bCrossAutoZoom /*&&
		m_nNowCrossZoomType != nCrossZoomtype*/ )
	{
		m_nNowCrossZoomType = nCrossZoomtype;
		switch (m_nNowCrossZoomType)
		{
			case CROSSZOOM_TYPE_IN:
			case CROSSZOOM_TYPE_OUT:
				if( DRAWMODE_GPS != (pAcState->nDrawMode&0xF0) && 
					DRAWMODE_SIM != pAcState->nDrawMode  )
				{
					return nuFALSE;
				}
				//
				if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 300 )
				{
					if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType >= 200 )
					{
						if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 3 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 3;
							bRes = nuTRUE;
							bReLoadMapCfg = nuTRUE;
						}
						else
						{
							if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 230 )
							{
								if( 58 != g_fileSysApi.pGdata->uiSetData.nScaleValue )
								{
									nScale = 58;
									bRes = nuTRUE;
								}
							}
							else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 265 )
							{
								if( 66 != g_fileSysApi.pGdata->uiSetData.nScaleValue )
								{
									nScale = 66;
									bRes = nuTRUE;
								}
							}
						}
					}
					else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType >= 100 )
					{
						if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 2 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
							bRes = nuTRUE;
							bReLoadMapCfg = nuTRUE;
						}
						else
						{
							if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 130 )
							{
								if( g_fileSysApi.pGdata->uiSetData.nScaleValue != 34 )
								{
									nScale = 34;
									bRes = nuTRUE;
								}
							}
							else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 165)
							{
								if( g_fileSysApi.pGdata->uiSetData.nScaleValue != 42 )
								{
									nScale = 42;
									bRes = nuTRUE;
								}
							}
						}
					}
					else
					{
						if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 1 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 1;
							bRes = nuTRUE;
							bReLoadMapCfg = nuTRUE;
						}
					}
				}
				else
				{
					if( m_bCrossZoomMap )
					{
						if( g_fileSysApi.pGdata->drawInfoNavi.nDisToBeforeCross <= 50 )
						{
							if( g_fileSysApi.pGdata->drawInfoNavi.nDisToBeforeCross > 25 &&
								1 == g_fileSysApi.pGdata->uiSetData.nScaleIdx &&
								35 != g_fileSysApi.pGdata->uiSetData.nScaleValue )
							{
								nScale = 35;
								bRes = nuTRUE;
							}
						}
						if( g_fileSysApi.pGdata->drawInfoNavi.nDisToBeforeCross <= 75 )
						{
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
							bRes = nuTRUE;
							bReLoadMapCfg = nuTRUE;
						}
						else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToBeforeCross <= 100 )
						{
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 3;
							bRes = nuTRUE;
							bReLoadMapCfg = nuTRUE;
						}
						else
						{
							if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != m_nOldScaleIdx )
							{
								g_fileSysApi.pGdata->uiSetData.nScaleIdx = m_nOldScaleIdx;
								bRes = nuTRUE;
								bReLoadMapCfg = nuTRUE;
							}
							m_bCrossZoomMap = nuFALSE;
						}
					}
					break;
				}
				break;
			/*
			case CROSSZOOM_TYPE_IN:
				if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 300 )
				{
					if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType >= 200 )
					{
						if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 2 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
							bRes = nuTRUE;
						}
					}
					else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType >= 100 )
					{
						if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 1 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 1;
							bRes = nuTRUE;
						}
					}
					else
					{
						
						if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 0 )
						{
							if( !m_bCrossZoomMap )
							{
								m_bCrossZoomMap = nuTRUE;
								m_nOldScaleIdx	= g_fileSysApi.pGdata->uiSetData.nScaleIdx;
							}
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = 0;
							bRes = nuTRUE;
						}
					}
				}
				break;
			case CROSSZOOM_TYPE_OUT:
				if( m_bCrossZoomMap )
				{
					if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 50 )
					{
						g_fileSysApi.pGdata->uiSetData.nScaleIdx = 1;
						bRes = nuTRUE;
					}
					else if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= 100 )
					{
						g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
						bRes = nuTRUE;
					}
					else
					{
						if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != m_nOldScaleIdx )
						{
							g_fileSysApi.pGdata->uiSetData.nScaleIdx = m_nOldScaleIdx;
							bRes = nuTRUE;
						}
						m_bCrossZoomMap = nuFALSE;
					}
				}
				break;
			*/
			default:
				if( m_bCrossZoomMap )
				{
					if ( g_fileSysApi.pGdata->uiSetData.nScaleIdx != m_nOldScaleIdx )
					{
						g_fileSysApi.pGdata->uiSetData.nScaleIdx = m_nOldScaleIdx;
						bRes = nuTRUE;
						bReLoadMapCfg = nuTRUE;
					}
					m_bCrossZoomMap = nuFALSE;
				}
				break;
		}
		if( bReLoadMapCfg )
		{
			g_fileSysApi.FS_ReLoadMapConfig();
		}
	}
	
	if( SplitScreenProc(SPLIT_PROC_TYPE_AUTO, nCrossNearType) != SPLIT_PROC_RES_NO )
	{
		bRes = nuTRUE;
	}
	if( bRes )
	{
		SetMapBitmapSize();
		AllocMemBitmap();
		SetBmpCenter();
		if( -1 == nScale )
		{
			nScale = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
		}
		SetMapSize( nScale );
		g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_SCALE, 0);
		m_nNowCrossNearType = nCrossNearType;
		pAcState->bNeedReloadMap = 1;
	}
	return bRes;
}

nuBOOL CGDataMgr::SetCrossNearEx(PACTIONSTATE pAcState, 
							   nuBYTE nCrossNearType /* = CROSSNEARTYPE_DEFAULT */, 
							   nuBYTE nCrossZoomtype /* = CROSSNEARTYPE_DEFAULT */)
{
	//---------------------------------------------------------------------------//
	nuBOOL bRes = nuFALSE;
	nuBOOL bSplit = nuFALSE;
	nuLONG nScale = CrossZoomProc(pAcState, nCrossZoomtype);
	if( -1 != nScale )
	{
		bRes = nuTRUE;
	}
	/*if( SplitScreenProc(SPLIT_PROC_TYPE_AUTO, nCrossNearType) != SPLIT_PROC_RES_NO )
	{
		bSplit = nuTRUE;
		bRes = nuTRUE;
	}*/
	if( bRes )
	{
		SetMapBitmapSize();
		AllocMemBitmap();
		SetBmpCenter();
		if( bSplit )
		{
			g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_KEEP, 0);
		}
		if( -1 == nScale )
		{
			nScale = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
		}
		SetMapSize( nScale );
		m_nNowCrossNearType = nCrossNearType;
		pAcState->bNeedReloadMap = 1;
	}
	return bRes;
}

nuLONG CGDataMgr::CrossZoomProc(PACTIONSTATE pAcState, nuBYTE nCrossZoomtype /* = CROSSNEARTYPE_DEFAULT */)
{
	nuLONG nScale = -1;
	if( g_fileSysApi.pUserCfg->bCrossAutoZoom  &&  g_fileSysApi.pUserCfg->nZoomAutoMode == 1)//revised by daniel 20111025//эも笆ゑㄒへぃ癸隔暗罽
	{
		m_nNowCrossZoomType = nCrossZoomtype;
		switch (m_nNowCrossZoomType)
		{
			case CROSSZOOM_TYPE_IN:
			case CROSSZOOM_TYPE_OUT:
				if( DRAWMODE_GPS != (pAcState->nDrawMode&0xF0) && 
					DRAWMODE_SIM != pAcState->nDrawMode  )
				{
					return -1;
				}
				if( g_fileSysApi.pGdata->drawInfoNavi.nDisToCrossZoomType <= m_nCrossZoomDis )
				{
					if( !m_bCrossZoomMap )
					{
						m_bCrossZoomMap = nuTRUE;
						m_nOldScale = g_fileSysApi.pGdata->uiSetData.nScaleValue;
						if( g_fileSysApi.pGdata->uiSetData.nScaleValue > m_nCrossZoomScale )
						{
							nScale = m_nCrossZoomScale;
						}
					}
					m_bCrossViewChanging = g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_STEP_IN, m_n3DAngleStep);
					if( !m_bCrossViewChanging )
					{
						if(g_fileSysApi.pGdata->uiSetData.nScaleValue != 400000 && -1 == nScale && g_fileSysApi.pGdata->uiSetData.nScaleValue > 25 )
						{
							nScale = g_fileSysApi.pGdata->uiSetData.nScaleValue - 5;
							if( nScale < 25 )
							{
								nScale = 25;
							}
						}
					}
				}
				else
				{
					if( m_bCrossZoomMap &&
						g_fileSysApi.pGdata->drawInfoNavi.nDisToBeforeCross >= 25 )
					{
						nuLONG nSMin = NURO_MIN(m_nCrossZoomScale, m_nOldScale);
						if( g_fileSysApi.pGdata->uiSetData.nScaleValue < nSMin )
						{
							nScale = g_fileSysApi.pGdata->uiSetData.nScaleValue + 5;
							if( nScale > nSMin )
							{
								nScale = nSMin;
							}
						}
						if( -1 == nScale )
						{
							m_bCrossViewChanging = g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_STEP_OUT, m_n3DAngleStep);
							if( !m_bCrossViewChanging )
							{
								m_bCrossZoomMap = nuFALSE;
								nScale = m_nOldScale;
							}
						}
					}
				}
				break;
			default:
				if( m_bCrossZoomMap )
				{
					nScale = m_nOldScale;
					g_mathtoolApi.MT_Set3DParam(_3D_PARAM_TYPE_MAX, 0);
					m_bCrossZoomMap = nuFALSE;
				}
				break;
		}
		if( -1 != nScale )
		{
			nuINT i = 0;
			for(  ; i < g_fileSysApi.pMapCfg->commonSetting.nScaleLayerCount - 1; ++i )
			{
				if( nScale >= g_fileSysApi.pMapCfg->pScaleList[i] &&
					nScale < g_fileSysApi.pMapCfg->pScaleList[i+1] )
				{
					break;
				}
			}
			if( i != g_fileSysApi.pGdata->uiSetData.nScaleIdx )
			{
				g_fileSysApi.pGdata->uiSetData.nScaleIdx = i;
				g_fileSysApi.FS_ReLoadMapConfig();
			}
		}
	}
	return nScale;
}

nuVOID CGDataMgr::SpeedAutoZoom(PACTIONSTATE pAcState, nuBYTE nIndex /*= 0xFF*/)
{
	if( m_bCrossZoomMap )
	{
		return;
	}
	nuBOOL bReset = nuFALSE;
//#define AVX_PANA
#ifdef AVX_PANA /*Revised by Daniel 20110628*/
	nuLONG nSpeed = -1;
	if((pAcState->nDrawMode & 0xF0) == DRAWMODE_GPS)
	{ 
		nSpeed = g_fileSysApi.pGdata->carInfo.nMiddleSpeed;
	}
	else if((pAcState->nDrawMode & 0xF0) == DRAWMODE_SIM)
	{
		nSpeed = g_fileSysApi.pUserCfg->nSimSpeed;
	}
	if(g_fileSysApi.pUserCfg->bCrossAutoZoom  && nSpeed >= 0)
	{
		if ( 0xFF == nIndex)
		{
			if( nSpeed < 30)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScaleIdx != 1)
				{
					bReset = nuTRUE;
					g_fileSysApi.pGdata->uiSetData.nScaleIdx = 1;
				}
			}
			else if( nSpeed < 60)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScaleIdx != 2)
				{
					bReset = nuTRUE;
					g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
				}
			}
			else if( nSpeed < 80)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScaleIdx != 3)
				{
					bReset = nuTRUE;
					g_fileSysApi.pGdata->uiSetData.nScaleIdx = 3;
				}
			}
			else if( nSpeed < 100)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScaleIdx != 4)
				{
					bReset = nuTRUE;
					g_fileSysApi.pGdata->uiSetData.nScaleIdx = 4;
				}
			}
			else if( nSpeed >= 100)
			{
				if(g_fileSysApi.pGdata->uiSetData.nScaleIdx != 5)
				{
					bReset = nuTRUE;
					g_fileSysApi.pGdata->uiSetData.nScaleIdx = 5;
				}
			}
		}
		else
		{
			if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != nIndex )
			{
				g_fileSysApi.pGdata->uiSetData.nScaleIdx = nIndex;
				bReset = nuTRUE;
			}
		}
	}
#else
	if ( 0xFF == nIndex)
	{
		if( g_fileSysApi.pGdata->carInfo.nMiddleSpeed > m_nSpeedAutoZoom )
		{
			if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 4 )
			{
				g_fileSysApi.pGdata->uiSetData.nScaleIdx = 4;
				pAcState->bResetScale	= 1;
				bReset = nuTRUE;
			}
		}
		else if( g_fileSysApi.pGdata->carInfo.nMiddleSpeed > 40 )
		{
			if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 3 )
			{
				g_fileSysApi.pGdata->uiSetData.nScaleIdx = 3;
				pAcState->bResetScale	= 1;
				bReset = nuTRUE;
			}
		}
		else 
		{
			if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != 2 )
			{
				g_fileSysApi.pGdata->uiSetData.nScaleIdx = 2;
				pAcState->bResetScale	= 1;
				bReset = nuTRUE;
			}
		}
	}
	else
	{
		if( g_fileSysApi.pGdata->uiSetData.nScaleIdx != nIndex )
		{
			g_fileSysApi.pGdata->uiSetData.nScaleIdx = nIndex;
			pAcState->bResetScale	= 1;
			bReset = nuTRUE;
		}
	}
#endif
	if( bReset )
	{
		pAcState->bResetScale	= 1;
		g_fileSysApi.FS_ReLoadMapConfig();
		SetMapSize( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale );
	}
}

nuVOID CGDataMgr::ResetMapAngle(nuSHORT nAngle, nuBYTE nType /* = MAPANGLE_SETTYPE_ONCE */)
{
	if( nType == MAPANGLE_SETTYPE_PROGRESS )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle += nAngle;
	}
	else if( nType == MAPANGLE_SETTYPE_ONCE )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle = nAngle;
	}
	//
	if( g_fileSysApi.pGdata->carInfo.nMapAngle < 0 )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle += 360;
	}
	else if( g_fileSysApi.pGdata->carInfo.nMapAngle >= 360 )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle = g_fileSysApi.pGdata->carInfo.nMapAngle % 360;
	}
	SetAngleParameter();
	CalShowRect(&g_fileSysApi.pGdata->transfData.nuShowMapSize);
}

nuBOOL CGDataMgr::CarFakeMove(nuLONG nFakeDis, PACTIONSTATE pAcState)
{
	nuLONG nDx = 0, nDy = 0;
	nuroPOINT ptIcon = {0};
	g_mathtoolApi.MT_MapToBmp(g_fileSysApi.pGdata->carInfo.ptCarIcon.x,
		g_fileSysApi.pGdata->carInfo.ptCarIcon.y,
		&(ptIcon.x),
		&(ptIcon.y) );
	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
	{
		nFakeDis = nFakeDis * g_fileSysApi.pGdata->drawInfoMap.nScalePixels / g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
		nDx = (nuLONG)(nFakeDis * nuCos(g_fileSysApi.pGdata->carInfo.nCarAngle));
		nDy = (nuLONG)(nFakeDis * nuSin(g_fileSysApi.pGdata->carInfo.nCarAngle));
	}
	else
	{
		nDy = nFakeDis * g_fileSysApi.pGdata->drawInfoMap.nScalePixels / g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale;
	}
	g_fileSysApi.pGdata->transfData.nBmpLTx		+= (nuSHORT)nDx;
	g_fileSysApi.pGdata->transfData.nBmpLTy		-= (nuSHORT)nDy;
	//
//	if( pAcState )
	{
		g_mathtoolApi.MT_BmpToMap(ptIcon.x + nDx,
				ptIcon.y - nDy,
				&g_fileSysApi.pGdata->carInfo.ptCarIcon.x,
				&g_fileSysApi.pGdata->carInfo.ptCarIcon.y );
	}
	return nuTRUE;
}

nuBOOL CGDataMgr::CarFakeMove(nuLONG nFakeDis, nuBYTE nTotalTimes, nuBYTE nNowIdx, PACTIONSTATE pAcState /* = NULL */)
{
	if( nFakeDis == 0 || nTotalTimes == nNowIdx )
	{
		return nuFALSE;
	}
	nuLONG nDx = 0, nDy = 0;
	nuroPOINT ptIcon;
	g_mathtoolApi.MT_MapToBmp(g_fileSysApi.pGdata->carInfo.ptCarIcon.x,
		g_fileSysApi.pGdata->carInfo.ptCarIcon.y,
		&(ptIcon.x),
		&(ptIcon.y) );
	if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_NORTH )
	{
		nDx = (nuLONG)(nFakeDis * nuCos(g_fileSysApi.pGdata->carInfo.nCarAngle));
		nDy = (nuLONG)(nFakeDis * nuSin(g_fileSysApi.pGdata->carInfo.nCarAngle));
		if( nDx >= 0 )
		{
			nDx = (nDx + nNowIdx) / nTotalTimes;
		}
		else
		{
			nDx = (nDx - nNowIdx) / nTotalTimes;
		}
		if( nDy >= 0 )
		{
			nDy = (nDy + nNowIdx) / nTotalTimes;
		}
		else
		{
			nDy = (nDy - nNowIdx) / nTotalTimes;
		}
	}
	else
	{
		nDy = (nFakeDis + nNowIdx)/ nTotalTimes;
	}
	g_fileSysApi.pGdata->transfData.nBmpLTx		+= (nuSHORT)nDx;
	g_fileSysApi.pGdata->transfData.nBmpLTy		-= (nuSHORT)nDy;
	//
	//	if( pAcState )
	{
		g_mathtoolApi.MT_BmpToMap(ptIcon.x + nDx,
			ptIcon.y - nDy,
			&g_fileSysApi.pGdata->carInfo.ptCarIcon.x,
			&g_fileSysApi.pGdata->carInfo.ptCarIcon.y );
	}
	return nuTRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
nuBOOL CGDataMgr::MapSetup(PACTIONSTATE pAcState)//Scale value and map rectangle
{
	if( g_fileSysApi.pGdata == NULL )
	{
		return nuFALSE;
	}
	//Maybe it been used in PC.
	if( pAcState->bResetScreenSize )
	{
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth + SCREEN_HORIZONTAL_EXTEND;
		g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight + SCREEN_VERTICAL_EXTEND;
		if( !g_gdiApi.GdiInitBMP(g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpWidth, 
			g_fileSysApi.pGdata->transfData.nuBmpMem1.bmpHeight) )
		{
			return nuFALSE;
		}
		pAcState->bResetMemBmpSize = 1;
	}//Set up when the size of the real sceen has changed.
	if( pAcState->bResetMemBmpSize )
	{
		//
		MapBmpSizeSet();
		//need know the Carcentermode for resetting the BmpCenterX Y.
		if( pAcState->nCarCenterMode == CAR_CENTER_MODE_REMAIN )//Have new CarCenterMode
		{
			pAcState->nCarCenterMode = g_fileSysApi.pGdata->transfData.nCarCenterMode;
		}//it is necessary!
	}
	if( pAcState->bDrawBmp1 )
	{
		SetBmpExtend();
		MapBmpCenterSet(pAcState->nCarCenterMode);
	}
	if( pAcState->bResetScale ||
		(pAcState->bResetMapCenter && !pAcState->bGlideMoveDraw) )
	{
		MapBmpScaleSet();
	}
	//Set Map info
	if( pAcState->bResetMapCenter )//Reset the map center
	{
		g_fileSysApi.pGdata->transfData.nMapCenterX	= pAcState->ptNewMapCenter.x;
		g_fileSysApi.pGdata->transfData.nMapCenterY	= pAcState->ptNewMapCenter.y;
	}
	if( pAcState->bResetIconCarPos )//Reset the car position, It is GPS or simulation drawing.
	{
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarIcon, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
	}
	if( pAcState->bResetRealCarPos )//GPS drawing, reset the real car position.
	{
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarRaw, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
		nuMemcpy(&g_fileSysApi.pGdata->carInfo.ptCarFixed, &pAcState->ptNewMapCenter, sizeof(NUROPOINT));
	}
	MapAngleSet(pAcState);
	//
	if( pAcState->nDrawMode != DRAWMODE_STOP )
	{
		g_fileSysApi.pGdata->carInfo.nShowSpeed = pAcState->nSpeed;
		if( pAcState->nDrawMode == DRAWMODE_GPS )
		{
			g_fileSysApi.pGdata->carInfo.nCarSpeed	= pAcState->nSpeed;
			g_fileSysApi.pGdata->carInfo.nMiddleSpeed = (g_fileSysApi.pGdata->carInfo.nMiddleSpeed * TIME_SPEEDCOUNT 
				+ g_fileSysApi.pGdata->carInfo.nCarSpeed + TIME_SPEEDCOUNT) / (TIME_SPEEDCOUNT + 1);
//			g_fileSysApi.pGdata->carInfo.nShowSpeed = g_fileSysApi.pGdata->carInfo.nMiddleSpeed;
		}
	}
	return nuTRUE;
}//

nuVOID CGDataMgr::MapAngleSet(PACTIONSTATE pAcState)
{
	if( (pAcState->nDrawMode & 0xF0) == DRAWMODE_GPS || 
		(pAcState->nDrawMode & 0xF0) == DRAWMODE_SIM )
	{
		if( pAcState->nAngle >= 0 )
		{
			pAcState->nAngle %= 360;
			g_fileSysApi.pGdata->carInfo.nCarAngle = pAcState->nAngle;//Set Car angle
			if( g_fileSysApi.pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_CAR &&
				g_fileSysApi.pGdata->carInfo.nMapAngle != pAcState->nAngle )
			{
				nuSHORT nDif = g_fileSysApi.pGdata->carInfo.nMapAngle - pAcState->nAngle;
				nDif = NURO_ABS(nDif);
				if( nDif > 180 )
				{
					nDif = 360 - nDif;
				}
				if( nDif >= m_nRotateMapAngle && pAcState->nDrawMode != DRAWMODE_SIM_START )
				{
					nuSHORT nDiff = g_fileSysApi.pGdata->carInfo.nMapAngle - pAcState->nAngle;
					if( nDiff > 0 )
					{
						if( nDiff < 180 )
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle -= m_nRotateMapAngle;
						}
						else
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle += m_nRotateMapAngle;
						}
					}
					else
					{
						nDiff = NURO_ABS(nDiff);
						if( nDiff < 180 )
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle += m_nRotateMapAngle;
						}
						else
						{
							g_fileSysApi.pGdata->carInfo.nMapAngle -= m_nRotateMapAngle;
						}
					}
					if( g_fileSysApi.pGdata->carInfo.nMapAngle < 0 )
					{
						g_fileSysApi.pGdata->carInfo.nMapAngle += 360;
					}
					else if( g_fileSysApi.pGdata->carInfo.nMapAngle >= 360 )
					{
						g_fileSysApi.pGdata->carInfo.nMapAngle = g_fileSysApi.pGdata->carInfo.nMapAngle % 360;
					}
					pAcState->bNeedExtendRotate = 1;
					//					pAcState->bSetAngle = nuTRUE;
				}
				else if( nDif >= MIN_ANGLE_ROTATE_MAP )
				{
					g_fileSysApi.pGdata->carInfo.nMapAngle = pAcState->nAngle;
				}
				pAcState->bSetAngle = nuTRUE;
			}//Set Map angle
			pAcState->nAngle = g_fileSysApi.pGdata->carInfo.nMapAngle;
		}
		if( g_fileSysApi.pGdata->carInfo.nRotateAngle != 0 )
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle = 0;
			pAcState->bSetAngle = nuTRUE;
		}
		//		if( pAcState->bDrawZoom )
		{
			pAcState->bSetAngle = nuTRUE;
		}
	}
	else if( pAcState->nDrawMode == DRAWMODE_POWERON )
	{
		g_fileSysApi.pGdata->carInfo.nMapAngle = pAcState->nAngle;
		pAcState->bSetAngle = nuTRUE;
	}
	else if( pAcState->nDrawMode == DRAWMODE_UI )//UI rorate map
	{
		if( pAcState->nAngle >= 0 )
		{
			g_fileSysApi.pGdata->carInfo.nRotateAngle = pAcState->nAngle;
			pAcState->bSetAngle = nuTRUE;
		}
	}
	if( pAcState->bSetAngle )
	{
		SetAngleParameter();
	}
}

nuBOOL CGDataMgr::CalMapLoadInfo(PACTIONSTATE pAcState, nuLONG nSW /* = 0 */, nuLONG nSH /* = 0 */)
{
	if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale >= 
		(nuDWORD)g_fileSysApi.pGdata->uiSetData.nBglStartScaleValue )
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = BSD_MODE_BGL;
	}
	else if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale >= 
		(nuDWORD)g_fileSysApi.pGdata->uiSetData.nBsdStartScaleVelue )
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = BSD_MODE_5k;
	}
	else
	{
		g_fileSysApi.pGdata->uiSetData.nBsdDrawMap = 0;
	}//CalMapInfo
	//
	if( nSW == 0 )
	{
		nSW = g_fileSysApi.pGdata->transfData.nBmpWidthEx;
	}
	if( nSH == 0 )
	{
		nSH = g_fileSysApi.pGdata->transfData.nBmpHeightEx;
	}
	nuroRECT rtMapShow;
	BLOCKIDARRAY blockIDArray;
	CalShowRect(&g_fileSysApi.pGdata->transfData.nuShowMapSize);
	//
	GetMapShowSize(nSW, nSH, rtMapShow);
	g_fileSysApi.pGdata->transfData.nuShowMapSize = rtMapShow;
	nuRectToBlockIDArray(rtMapShow, &blockIDArray);//&g_fileSysApi.pGdata->transfData.blockIDShowArray);
	if( !nuBlockIDArrayEqual(blockIDArray, g_fileSysApi.pGdata->transfData.blockIDShowArray) )
	{
		g_fileSysApi.pGdata->transfData.blockIDShowArray = blockIDArray;
		pAcState->bNeedReloadMap	= 1;
	}
	else if( pAcState->bResetScale )
	{
		pAcState->bNeedReloadMap	= 1;
	}//Reset scale, reload map
	else
	{
		pAcState->bNeedReloadMap	= 0;
	}
	//
	return nuTRUE;
}

nuVOID CGDataMgr::MapBmpCenterSet(nuBYTE nCenterMode /* = CAR_CENTER_MODE_REMAIN */, nuWORD x /* = 0 */, nuWORD y /* = 0 */)
{
	if( nCenterMode != CAR_CENTER_MODE_REMAIN )
	{
		g_fileSysApi.pGdata->transfData.nCarCenterMode = nCenterMode;
		if( g_fileSysApi.pGdata->transfData.nCarCenterMode == CAR_CENTER_MODE_ONE_HALF_XY )
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			g_fileSysApi.pGdata->transfData.nBmpCenterY = g_fileSysApi.pGdata->transfData.nBmpHeight/2;
		}
		else if( g_fileSysApi.pGdata->transfData.nCarCenterMode == CAR_CENTER_MODE_X_ONE_Y_THREE )
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpWidth/4;
			g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight*3/4;
		}
		else if( g_fileSysApi.pGdata->transfData.nCarCenterMode == CAR_CENTER_MODE_XY_PARAMETER )
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX = x;
			g_fileSysApi.pGdata->transfData.nBmpCenterY = y;
		}
		else
		{
			g_fileSysApi.pGdata->transfData.nBmpCenterX = g_fileSysApi.pGdata->transfData.nBmpWidth/2;
			g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpHeight*(m_nCarPosition-1)/m_nCarPosition;
		}
		m_nOldBmpCenterX	= g_fileSysApi.pGdata->transfData.nBmpCenterX;
		m_nOldBmpCenterY	= g_fileSysApi.pGdata->transfData.nBmpCenterY;
	}
	else
	{
		g_fileSysApi.pGdata->transfData.nBmpCenterX = m_nOldBmpCenterX;
		g_fileSysApi.pGdata->transfData.nBmpCenterY = m_nOldBmpCenterY;
	}
	g_fileSysApi.pGdata->transfData.nBmpCenterX += g_fileSysApi.pGdata->transfData.nBmpLTx;
	g_fileSysApi.pGdata->transfData.nBmpCenterY += g_fileSysApi.pGdata->transfData.nBmpLTy;
}

nuVOID CGDataMgr::MapBmpSizeSet()
{
	switch( g_fileSysApi.pGdata->uiSetData.nScreenType )
	{
	case SCREEN_TYPE_TWO://1:1
		g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth/2;
		g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
		g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
		g_fileSysApi.pGdata->zoomScreenData.bZoomScreen	= 1;
		break;
	case SCREEN_TYPE_THREE://2:1
		g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth*2/3;
		g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
		g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
		g_fileSysApi.pGdata->zoomScreenData.bZoomScreen	= 1;
		break;
	case SCREEN_TYPE_ONE://whole screen
	case SCREEN_TYPE_DEFAULT:
	default:
		g_fileSysApi.pGdata->transfData.nBmpWidth	= g_fileSysApi.pGdata->uiSetData.nScreenWidth;
		g_fileSysApi.pGdata->transfData.nBmpHeight	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
		g_fileSysApi.pGdata->transfData.rtMapScreen.left		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.top		= 0;
		g_fileSysApi.pGdata->transfData.rtMapScreen.right		= g_fileSysApi.pGdata->transfData.rtMapScreen.left + g_fileSysApi.pGdata->transfData.nBmpWidth;
		g_fileSysApi.pGdata->transfData.rtMapScreen.bottom		= g_fileSysApi.pGdata->transfData.rtMapScreen.top + g_fileSysApi.pGdata->transfData.nBmpHeight;
		g_fileSysApi.pGdata->zoomScreenData.bZoomScreen	= 0;
		break;
	}
	g_fileSysApi.pGdata->transfData.nBmpWidthEx		= g_fileSysApi.pGdata->uiSetData.nScreenWidth  + SCREEN_HORIZONTAL_EXTEND;//g_fileSysApi.pGdata->transfData.nBmpWidth  + SCREEN_HORIZONTAL_EXTEND;
	g_fileSysApi.pGdata->transfData.nBmpHeightEx	= g_fileSysApi.pGdata->uiSetData.nScreenHeight + SCREEN_VERTICAL_EXTEND;//g_fileSysApi.pGdata->transfData.nBmpHeight + SCREEN_VERTICAL_EXTEND;
	AllocMemBitmap();
}


nuVOID CGDataMgr::MapBmpScaleSet()
{
	nuDWORD nWidth = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale * COORCONSTANT * MAPSCREEN;
	/*
	double fLen = g_fileSysApi.pGdata->drawInfoMap.nScalePixels * 
		nuCos( g_fileSysApi.pGdata->transfData.nMapCenterY / 100000 );
	fLen = NURO_ABS(fLen);
	g_fileSysApi.pGdata->transfData.nMapWidth = (nuDWORD)(nWidth/fLen);
	*/
	//
	if( g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale <= 20000 )
	{
		nuFLOAT fLen = (nuFLOAT)(g_fileSysApi.pGdata->drawInfoMap.nScalePixels * nuCos( g_fileSysApi.pGdata->transfData.nMapCenterY / 100000 ));
		fLen = NURO_ABS(fLen);
		g_fileSysApi.pGdata->transfData.nMapWidth = (nuDWORD)(nWidth/fLen);
	}
	else
	{
		g_fileSysApi.pGdata->transfData.nMapWidth = nWidth * m_nCenterScale / 
			( g_fileSysApi.pGdata->drawInfoMap.nScalePixels * _ZK_GD_CENTER_SCALE);
	}
	//
	nWidth = g_fileSysApi.pMapCfg->scaleSetting.scaleTitle.nScale * COORCONSTANT * MAPSCREEN / 
		g_fileSysApi.pGdata->drawInfoMap.nScalePixels;
	g_fileSysApi.pGdata->transfData.nMapHeight = (nuDWORD)(nWidth);
}

nuBOOL CGDataMgr::GetMapShowSize(nuLONG nScreenShowW, nuLONG nScreenShowH, nuroRECT& rtMapSize)
{
	NUROPOINT Pt[4] = {0};
	NURORECT rect   = {0};
	nuLONG n		= 0;
	g_mathtoolApi.MT_BmpToMap( 0, 
		0, 
		&(Pt[0].x),
		&(Pt[0].y) );
	g_mathtoolApi.MT_BmpToMap( 0, 
		nScreenShowH, 
		&(Pt[1].x),
		&(Pt[1].y));
	g_mathtoolApi.MT_BmpToMap( nScreenShowW,
		0, 
		&(Pt[2].x), 
		&(Pt[2].y));
	g_mathtoolApi.MT_BmpToMap( nScreenShowW, 
		nScreenShowH,
		&(Pt[3].x), 
		&(Pt[3].y) );
	if( NURO_ABS( Pt[0].x - Pt[3].x ) >= NURO_ABS( Pt[1].x - Pt[2].x ) )
	{
		rect.left   = Pt[0].x;
		rect.right  = Pt[3].x;
		rect.top    = Pt[2].y;
		rect.bottom = Pt[1].y;
	}
	else
	{
		rect.left   = Pt[1].x;
		rect.right  = Pt[2].x;
		rect.top    = Pt[0].y;
		rect.bottom = Pt[3].y;
	}
	if( rect.left > rect.right )
	{
		n = rect.left;
		rect.left  = rect.right;
		rect.right = n;
	}
	if(  rect.top > rect.bottom )
	{
		n = rect.bottom;
		rect.bottom = rect.top;
		rect.top = n;
	}
	if( rect.left < -XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.left = -XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.right > XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.right = XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.top < -YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.top = -YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.bottom > YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.bottom = YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	rtMapSize = rect;
	return nuTRUE;
}

nuBOOL CGDataMgr::ZoomMapInit()
{
	PZOOMSCREENDATA pZoomdata = &g_fileSysApi.pGdata->zoomScreenData;
	pZoomdata->nuZoomBmp.bmpWidth	= g_fileSysApi.pUserCfg->nZoomBmpWidth;
	pZoomdata->nuZoomBmp.bmpHeight	= g_fileSysApi.pUserCfg->nZoomBmpHeight;
	pZoomdata->nuZoomBmp.bmpReserve	= NURO_BMP_DIB_SECTION;
	pZoomdata->nuZoomBmp.bmpType	= NURO_BMP_TYPE_COPY;
	if( pZoomdata->nuZoomBmp.bmpWidth == 0 )//Useful
	{
		pZoomdata->nuZoomBmp.bmpWidth = g_fileSysApi.pGdata->uiSetData.nScreenWidth / 2;
	}
	if( pZoomdata->nuZoomBmp.bmpHeight == 0 )
	{
		pZoomdata->nuZoomBmp.bmpHeight= g_fileSysApi.pGdata->uiSetData.nScreenHeight / 2;
	}
	if( !g_gdiApi.GdiCreateNuroBMP(&pZoomdata->nuZoomBmp) )
	{
		return nuFALSE;
	}
	pZoomdata->nZoomBmpWidth	= pZoomdata->nuZoomBmp.bmpWidth;
	pZoomdata->nZoomBmpHeight	= pZoomdata->nuZoomBmp.bmpHeight;
	pZoomdata->nZoomBmpCenterX	= pZoomdata->nZoomBmpWidth / 2;
	pZoomdata->nZoomBmpCenterY	= pZoomdata->nZoomBmpHeight / 2;

	pZoomdata->rtZoomScreen.left	= g_fileSysApi.pGdata->uiSetData.nScreenWidth / 2;
	pZoomdata->rtZoomScreen.right	= g_fileSysApi.pGdata->uiSetData.nScreenWidth;
	pZoomdata->rtZoomScreen.top		= 0;
	pZoomdata->rtZoomScreen.bottom	= g_fileSysApi.pGdata->uiSetData.nScreenHeight;
	return nuTRUE;
}
nuVOID CGDataMgr::ZoomMapFree()
{
	if(g_fileSysApi.pGdata)
	{
	g_gdiApi.GdiDelNuroBMP(&g_fileSysApi.pGdata->zoomScreenData.nuZoomBmp);
}
	
}
nuBOOL CGDataMgr::ZoomMapSetup(nuroPOINT& ptMapCenter, nuSHORT nAngleDx, nuSHORT nAngleDy, nuLONG nPicID)
{
	g_fileSysApi.pGdata->zoomScreenData.nZoomMapCenterX = ptMapCenter.x;
	g_fileSysApi.pGdata->zoomScreenData.nZoomMapCenterY = ptMapCenter.y;
	g_fileSysApi.pGdata->zoomScreenData.nPicID	= nPicID;

	//ColZoom MapSize;
	nuDWORD nWidth = g_fileSysApi.pMapCfg->zoomSetting.scaleTitle.nScale * COORCONSTANT * MAPSCREEN;
	double fLen = g_fileSysApi.pGdata->drawInfoMap.nScalePixels * nuCos( g_fileSysApi.pGdata->zoomScreenData.nZoomMapCenterY / 100000 );
	fLen = NURO_ABS(fLen);
	g_fileSysApi.pGdata->zoomScreenData.nZoomMapWidth	= (nuLONG)(nWidth / fLen);
	nWidth = g_fileSysApi.pMapCfg->zoomSetting.scaleTitle.nScale * COORCONSTANT * MAPSCREEN / g_fileSysApi.pGdata->drawInfoMap.nScalePixels;
	g_fileSysApi.pGdata->zoomScreenData.nZoomMapHeight	= (nuLONG)(nWidth);

	//Col Zoom Angle
	if( nAngleDx == 0 && nAngleDy == 0 )
	{
		nAngleDy = 1;
	}
	g_fileSysApi.pGdata->zoomScreenData.nZoomAngleDis = (nuLONG)nuSqrt(nAngleDx*nAngleDx + nAngleDy*nAngleDy);
	g_fileSysApi.pGdata->zoomScreenData.nZoomAngleDy  = nAngleDy;//(nuLONG)(LENCONST * nuSin(nAngle) + 0.5);
	g_fileSysApi.pGdata->zoomScreenData.nZoomAgnleDx  = nAngleDx;//(nuLONG)(LENCONST * nuCos(nAngle) + 0.5);
	//Col Zoom Map Size
	GetZoomMapShowSize( g_fileSysApi.pGdata->zoomScreenData.nuZoomBmp.bmpWidth,
		g_fileSysApi.pGdata->zoomScreenData.nuZoomBmp.bmpHeight,
		g_fileSysApi.pGdata->zoomScreenData.nuZoomMapSize );
	g_fileSysApi.pGdata->zoomScreenData.bSetZoomData = 1;
	return nuTRUE;
}

nuBOOL CGDataMgr::GetZoomMapShowSize(nuLONG nWidth, nuLONG nHeight, nuroRECT& rtMapSize)
{
	NUROPOINT Pt[4] = {0};
	NURORECT rect   = {0};
	nuLONG n		= 0;
	g_mathtoolApi.MT_ZoomBmpToMap( 0, 
		0, 
		&(Pt[0].x),
		&(Pt[0].y) );
	g_mathtoolApi.MT_ZoomBmpToMap( 0, 
		nHeight, 
		&(Pt[1].x),
		&(Pt[1].y));
	g_mathtoolApi.MT_ZoomBmpToMap( nWidth,
		0, 
		&(Pt[2].x), 
		&(Pt[2].y));
	g_mathtoolApi.MT_ZoomBmpToMap( nWidth, 
		nHeight,
		&(Pt[3].x), 
		&(Pt[3].y) );
	if( NURO_ABS( Pt[0].x - Pt[3].x ) >= NURO_ABS( Pt[1].x - Pt[2].x ) )
	{
		rect.left   = Pt[0].x;
		rect.right  = Pt[3].x;
		rect.top    = Pt[2].y;
		rect.bottom = Pt[1].y;
	}
	else
	{
		rect.left   = Pt[1].x;
		rect.right  = Pt[2].x;
		rect.top    = Pt[0].y;
		rect.bottom = Pt[3].y;
	}
	if( rect.left > rect.right )
	{
		n = rect.left;
		rect.left  = rect.right;
		rect.right = n;
	}
	if(  rect.top > rect.bottom )
	{
		n = rect.bottom;
		rect.bottom = rect.top;
		rect.top = n;
	}
	if( rect.left < -XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.left = -XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.right > XBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.right = XBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.top < -YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.top = -YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	if( rect.bottom > YBLOCKNUM * EACHBLOCKSIZE / 2 )
	{
		rect.bottom = YBLOCKNUM * EACHBLOCKSIZE / 2;
	}
	rtMapSize = rect;
	return nuTRUE;
}

nuUINT CGDataMgr::SplitScreenProc(nuBYTE nSpProcType /* = SPLIT_PROC_TYPE_AUTO */, nuBYTE nNowCrossNearType /* = 0 */)
{
	nuUINT nRes = SPLIT_PROC_RES_NO;
	if( nSpProcType == SPLIT_PROC_TYPE_HAND )
	{
		if( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_DISABLE  ||
			g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_ONLYAUTO ||
			( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_AUTOHAND && 
			g_fileSysApi.pGdata->uiSetData.nSplitScreenType == SPLIT_SCREEN_AUTO ) )
		{
			return nRes;
		}
		if( g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_DEFAULT ) 
		{
			g_fileSysApi.pGdata->uiSetData.nSplitScreenType  = SPLIT_SCREEN_NO;
			g_fileSysApi.pGdata->uiSetData.nScreenType		 = SCREEN_TYPE_DEFAULT;
			nRes = SPLIT_PROC_RES_SPLITTING;
		}
		else
		{
			g_fileSysApi.pGdata->uiSetData.nSplitScreenType	= SPLIT_SCREEN_HAND;
			g_fileSysApi.pGdata->uiSetData.nScreenType		= g_fileSysApi.pUserCfg->nScreenType;
			nRes = SPLIT_PROC_RES_SPLITCOVER;
		}
		return nRes;
	}
	else
	{
		if( m_nNowCrossNearType == nNowCrossNearType )
		{
			return nRes;
		}
		m_nNowCrossNearType = nNowCrossNearType;
		if( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_ONLYAUTO ||
			( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_AUTOHAND &&
			( g_fileSysApi.pGdata->uiSetData.nSplitScreenType == SPLIT_SCREEN_NO || 
			  g_fileSysApi.pGdata->uiSetData.nSplitScreenType == SPLIT_SCREEN_AUTO
			  || SPLIT_SCREEN_HAND == g_fileSysApi.pGdata->uiSetData.nSplitScreenType ) ) )
		{
			switch ( m_nNowCrossNearType )
			{
			case CROSSNEARTYPE_SCALE0_IN:
			case CROSSNEARTYPE_SCALE1_IN:
			case CROSSNEARTYPE_PIC_IN:
			case CROSSNEARTYPE_HIGHWAY:
			case CROSSNEARTYPR_PIC_IN_SMALL:
				if( m_nSplitScreenTye != SPLIT_SCREEN_AUTO_OPEN && 
					g_fileSysApi.pGdata->uiSetData.nScreenType != g_fileSysApi.pUserCfg->nScreenType )
				{
					g_fileSysApi.pGdata->uiSetData.nScreenType = g_fileSysApi.pUserCfg->nScreenType;
					m_nSplitScreenTye		= SPLIT_SCREEN_AUTO_OPEN;
					nRes = SPLIT_PROC_RES_SPLITTING;
				}
				break;
			case CROSSNEARTYPE_DEFAULT://Cover from crossnear type
			case CROSSNEARTYPE_COVER:
			case CROSSNEARTYPE_SCALE0_OUT:
			case CROSSNEARTYPE_SCALE1_OUT:
			case CROSSNEARTYPE_PIC_OUT:
			case CROSSNEARTYPE_STOPNAVI:
				m_nSplitScreenTye	= SPLIT_SCREEN_AUTO_CLOSE;
				g_fileSysApi.pGdata->uiSetData.nSplitScreenType = SPLIT_SCREEN_NO;
				if( g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_DEFAULT )
				{
					g_fileSysApi.pGdata->uiSetData.nScreenType = SCREEN_TYPE_DEFAULT;
					nRes = SPLIT_PROC_RES_SPLITCOVER;
				}
				break;
			default:
				break;
			}
		}//Split screen
		else if( g_fileSysApi.pUserCfg->nSplitScreenMode == SPLIT_SCREEN_TYPE_ONLYHAND && 
				 m_nNowCrossNearType == CROSSNEARTYPE_STOPNAVI )
		{
			m_nSplitScreenTye	= SPLIT_SCREEN_AUTO_CLOSE;
			g_fileSysApi.pGdata->uiSetData.nSplitScreenType = SPLIT_SCREEN_NO;
			if( g_fileSysApi.pGdata->uiSetData.nScreenType != SCREEN_TYPE_DEFAULT )
			{
				g_fileSysApi.pGdata->uiSetData.nScreenType = SCREEN_TYPE_DEFAULT;
				nRes = SPLIT_PROC_RES_SPLITCOVER;
			}
		}
	}
	return nRes;
}
