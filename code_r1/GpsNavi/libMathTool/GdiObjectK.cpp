// GdiObjectK.cpp: implementation of the CGdiObject class.
//
//////////////////////////////////////////////////////////////////////

#include "GdiObjectK.h"
#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"
#include "Object3DDrawZK.h"

extern	PFILESYSAPI		g_pMtFsApi;
extern	PGDIAPI			g_pMtGdiApi;

extern CObject3DDrawZK*		g_pObj3DDraw;

#define _3D_LINE_CENTER_WIDTH				2


#define MAPLINE_SOLID						0
#define MAPLINE_DASH						1


#define SCREEN_3D_BOTTOM_EXLEN					10
#define NVARROWLINE_RIMWIDTH					2
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGdiObjectK::CGdiObjectK()
{

}

CGdiObjectK::~CGdiObjectK()
{
	Free();
}

nuBOOL CGdiObjectK::Initialize()
{
	nuLONG nTmp = 0;
	if( !nuReadConfigValue("REDARROWLINEW", &nTmp) )
	{
		nTmp = NVARROWLINE_RIMWIDTH;
	}
	m_nRedArrowLintW = (nuBYTE)nTmp;
	if( !nuReadConfigValue("GONAVI", &nTmp) )
	{
		nTmp = 0;
	}
	if( !nuReadConfigValue("ARROWCOLOR", &m_nArrowColor))
	{
		m_nArrowColor = 16711680;//255 * 2^16 
	}
	/*if( !nuReadConfigValue("ARROWCOLOR_RED", &m_nArrowColorR))
	{
		m_nArrowColorR = 255;
	}
	if( !nuReadConfigValue("ARROWCOLOR_GREEN", &m_nArrowColorG))
	{
		m_nArrowColorG = 0;
	}
	if( !nuReadConfigValue("ARROWCOLOR_BLUE", &m_nArrowColorB))
	{
		m_nArrowColorB = 0;
	}
	if( !nuReadConfigValue("ARROWCOLORRIM_RED", &m_nArrowRimColorR))
	{
		m_nArrowRimColorR = 0;
	}
	if( !nuReadConfigValue("ARROWCOLORRIM_GREEN", &m_nArrowRimColorG))
	{
		m_nArrowRimColorG = 0;
	}
	if( !nuReadConfigValue("ARROWCOLORRIM_BLUE", &m_nArrowRimColorB))
	{
		m_nArrowRimColorB = 0;
	}*/
	
	if( !nuReadConfigValue("DISOFTWOARROWS", &m_nTwoArrowsDis))
	{
		m_nTwoArrowsDis = 30;//255 * 2^16 
	}
	if( !nuReadConfigValue("ARROWLISTWIDTH", &m_lArrowListWidth))
	{
		m_lArrowListWidth = 0;//255 * 2^16 
	}	
	m_nGoNavi	= (nuBYTE)nTmp;
	return nuTRUE;
}

nuVOID CGdiObjectK::Free()
{
}

nuINT CGdiObjectK::SelectObject(nuBYTE nDrawType, nuBYTE nShowMode, const nuVOID* lpSet, nuLONG *pOut)
{
	NUROPEN		nuPen	= {0};
	NUROBRUSH	nuBrush = {0};
	NUROFONT	nuLogFont = {0};
	nuCOLORREF col = {0};
	if( nDrawType == DRAW_TYPE_ROADRIM )//Road Rim line
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowRimLine )
		{
			if( !pRoadSet->nShowCenterLine || !pRoadSet->nShowName)
			{
				return  DRAW_OBJ_SKIP;
			}
			else 
			{
				return DRWA_OBJ_ONLYCOLNAME;
			}
		}
		nuPen.nRed	= pRoadSet->nRimLineColorR;
		nuPen.nGreen= pRoadSet->nRimLineColorG;
		nuPen.nBlue	= pRoadSet->nRimLineColorB;
		if( nShowMode )//3D mode
		{
			if( g_pMtFsApi->pGdata->uiSetData.nBsdDrawMap == BSD_MODE_COMMON &&
				pRoadSet->nShowCenterLine && 
				pRoadSet->nCenterLineWidth >= _3D_LINE_CENTER_WIDTH )
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nRimLineWidth;
				g_pMtGdiApi->GdiSetPen(&nuPen);
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				*pOut = pRoadSet->nRimLineWidth;
				/*
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nRimLineWidth;
				nuSetGdiPen(&nuPen);
				*/
				if( pRoadSet->nRimLineType == MAPLINE_DASH )
				{
					return DRAW_OBJ_3D_DASH;
				}
				else
				{
					return DRAW_OBJ_3D_BIRDVIEW;
				}
			}
			else
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nRimLineWidth;
				g_pMtGdiApi->GdiSetPen(&nuPen);
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				*pOut = pRoadSet->nRimLineWidth;
				if( pRoadSet->nRimLineType == MAPLINE_DASH )
				{
					return DRAW_OBJ_3D_DASH;
				}
				else
				{
					return DRAW_OBJ_3D_LINE;
				}
			}
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pRoadSet->nRimLineWidth;
			if( nuPen.nWidth == 0 )
			{
				nuPen.nWidth = 1;
			}
			g_pMtGdiApi->GdiSetPen(&nuPen);
			if( pRoadSet->nRimLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_2D_DASH;
			}
			else
			{
				return DRAW_OBJ_2D_LINE;
			}
		}
	}
	else if( nDrawType == DRAW_TYPE_ROADCENTER )//Road center line
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pRoadSet->nCenterLineColorR;
		nuPen.nGreen= pRoadSet->nCenterLineColorG;
		nuPen.nBlue	= pRoadSet->nCenterLineColorB;
		if( nShowMode )//3D mode
		{
			/**/
			if( g_pMtFsApi->pGdata->uiSetData.nBsdDrawMap == BSD_MODE_COMMON &&
				pRoadSet->nCenterLineWidth >= _3D_LINE_CENTER_WIDTH )
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nCenterLineWidth;
				g_pMtGdiApi->GdiSetPen(&nuPen);
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				*pOut = pRoadSet->nCenterLineWidth;
				if( pRoadSet->nCenterLineType == MAPLINE_DASH )
				{
					return DRAW_OBJ_3D_DASH;
				}
				else
				{
					return DRAW_OBJ_3D_BIRDVIEW;
				}
			}
			else
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nCenterLineWidth;
				if( nuPen.nWidth == 0 )
				{
					nuPen.nWidth = 1;
				}
				g_pMtGdiApi->GdiSetPen(&nuPen);
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				*pOut = pRoadSet->nCenterLineWidth;
				if( pRoadSet->nCenterLineType == MAPLINE_DASH )
				{
					return DRAW_OBJ_3D_DASH;
				}
				else
				{
					return DRAW_OBJ_3D_LINE;
				}
			}
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pRoadSet->nCenterLineWidth;
			if( nuPen.nWidth == 0 )
			{
				nuPen.nWidth = 1;
			}
			g_pMtGdiApi->GdiSetPen(&nuPen);
			if( pRoadSet->nCenterLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_2D_DASH;
			}
			else
			{
				return DRAW_OBJ_2D_LINE;
			}
		}
	}
	else if( nDrawType == DRAW_TYPE_BGLINERIM )
	{
		PCBGLINESETTING pBgLineSet = (PCBGLINESETTING)lpSet;
		if( !pBgLineSet->nShowRimLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pBgLineSet->nRimLineColorR;
		nuPen.nGreen= pBgLineSet->nRimLineColorG;
		nuPen.nBlue	= pBgLineSet->nRimLineColorB;
		nuPen.nStyle= NURO_PS_SOLID;
		nuPen.nWidth= pBgLineSet->nRimLineWidth;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		if( nShowMode )//3D mode
		{
			if( pBgLineSet->nRimLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_3D_DASH;
			}
			else
			{
				return DRAW_OBJ_3D_LINE;
			}
		}
		else
		{
			if( pBgLineSet->nRimLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_2D_DASH;
			}
			else
			{
				return DRAW_OBJ_2D_LINE;
			}
		}
	}
	else if( nDrawType == DRAW_TYPE_BGLINECENTER )
	{
		PCBGLINESETTING pBgLineSet = (PCBGLINESETTING)lpSet;
		if( !pBgLineSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pBgLineSet->nCenterLineColorR;
		nuPen.nGreen= pBgLineSet->nCenterLineColorG;
		nuPen.nBlue	= pBgLineSet->nCenterLineColorB;
		nuPen.nStyle= NURO_PS_SOLID;
		nuPen.nWidth= pBgLineSet->nCenterLineWidth;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		if( nShowMode )//3D mode
		{
			if( pBgLineSet->nCenterLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_3D_DASH;
			}
			else
			{
				return DRAW_OBJ_3D_LINE;
			}
		}
		else
		{
			if( pBgLineSet->nCenterLineType == MAPLINE_DASH )
			{
				return DRAW_OBJ_2D_DASH;
			}
			else
			{
				return DRAW_OBJ_2D_LINE;
			}
		}
	}
	else if( nDrawType == DRAW_TYPE_BGAREA )
	{
		PCBGAREASETTING pBgAreaSet = (PCBGAREASETTING)lpSet;
		if( !pBgAreaSet->nShowArea )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed		= pBgAreaSet->nCoverColorR;
		nuPen.nGreen	= pBgAreaSet->nCoverColorG;
		nuPen.nBlue		= pBgAreaSet->nCoverColorB;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nWidth	= 1;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		nuBrush.nRed	= pBgAreaSet->nCoverColorR;
		nuBrush.nGreen	= pBgAreaSet->nCoverColorG;
		nuBrush.nBlue	= pBgAreaSet->nCoverColorB;
		g_pMtGdiApi->GdiSetBrush(&nuBrush);
		if( nShowMode )
		{
			return DRAW_OBJ_3D_AREA;
		}
		else
		{
			return DRAW_OBJ_2D_AREA;
		}
	}
	else if( nDrawType == DRAW_TYPE_ZOOMFAKE_LINE )
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowRimLine && !pRoadSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed		= pRoadSet->nRimLineColorR;//pRoadSet->nCenterLineColorR;
		nuPen.nGreen	= pRoadSet->nRimLineColorG;//pRoadSet->nCenterLineColorG;
		nuPen.nBlue		= pRoadSet->nRimLineColorB;//pRoadSet->nCenterLineColorB;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nWidth	= 1;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		if( nShowMode )
		{
			return DRAW_OBJ_3D_LINE;
		}
		else
		{
			return DRAW_OBJ_2D_LINE;
		}
	}
	else if( nDrawType == DRAW_TYPE_RNFONT )
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		nuLogFont.lfHeight	= pRoadSet->nFontHeight;
		nuLogFont.lfWidth	= pRoadSet->nFontWidth;
		nuLogFont.lfWeight	= pRoadSet->nFontBold;
		nuLogFont.lfEscapement	= *pOut;
		nuTcscpy(nuLogFont.lfFaceName, nuTEXT("Arial"));
		g_pMtGdiApi->GdiSetFont(&nuLogFont);

		col = nuRGB( pRoadSet->nFontColorR, pRoadSet->nFontColorG, pRoadSet->nFontColorB);
		g_pMtGdiApi->GdiSetTextColor(col);
		return DRAW_OBJ_ROADNAME;
	}
	else if( nDrawType == DRAW_TYPE_RNFONTANDBANNER )
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		nuLogFont.lfHeight	= pRoadSet->nFontHeight;
		nuLogFont.lfWidth	= pRoadSet->nFontWidth;
		nuLogFont.lfWeight	= pRoadSet->nFontBold;
		nuLogFont.lfEscapement	= *pOut;
		nuTcscpy(nuLogFont.lfFaceName, nuTEXT("Arial"));
		g_pMtGdiApi->GdiSetFont(&nuLogFont);

		nuPen.nRed		= pRoadSet->nBannerRimColorR;
		nuPen.nGreen	= pRoadSet->nBannerRimColorG;
		nuPen.nBlue		= pRoadSet->nBannerRimColorB;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nWidth	= 1;
		g_pMtGdiApi->GdiSetPen(&nuPen);

		nuBrush.nRed	= pRoadSet->nBannerColorR;
		nuBrush.nGreen	= pRoadSet->nBannerColorG;
		nuBrush.nBlue	= pRoadSet->nBannerColorB;
		g_pMtGdiApi->GdiSetBrush(&nuBrush);

		col = nuRGB( pRoadSet->nFontColorR, pRoadSet->nFontColorG, pRoadSet->nFontColorB);
		g_pMtGdiApi->GdiSetTextColor(col);
		return DRAW_OBJ_ROADNAME;
	}
	else if( nDrawType == DRAW_TYPE_POI_FONT )
	{
		PCPOISETTING pPoiSet = (PCPOISETTING)lpSet;
		nuPen.nRed		= pPoiSet->nFontColorR;
		nuPen.nGreen	= pPoiSet->nFontColorG;
		nuPen.nBlue		= pPoiSet->nFontColorB;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nWidth	= 1;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		
		nuBrush.nRed	= pPoiSet->nFontColorR;
		nuBrush.nGreen	= pPoiSet->nFontColorG;
		nuBrush.nBlue	= pPoiSet->nFontColorB;
		g_pMtGdiApi->GdiSetBrush(&nuBrush);
		//
		nuLogFont.lfHeight	= pPoiSet->nFontSize;
		nuLogFont.lfWidth	= 0;
		nuLogFont.lfWeight	= pPoiSet->nFontBold;
		nuLogFont.lfEscapement	= *pOut;
		nuTcscpy(nuLogFont.lfFaceName, nuTEXT("Arial"));
		g_pMtGdiApi->GdiSetFont(&nuLogFont);

		col = nuRGB( pPoiSet->nFontColorR, pPoiSet->nFontColorG, pPoiSet->nFontColorB);
		g_pMtGdiApi->GdiSetTextColor(col);
		*pOut = pPoiSet->nFontSize;
		return DRAW_OBJ_POINAME;
	}
	else if( nDrawType == DRAW_TYPE_NAVI_ROAD )
	{
		PNAVILINESETTING pNaviLine = (PNAVILINESETTING)lpSet;
		nuPen.nRed	= pNaviLine->nRouteLineColorR;
		nuPen.nGreen= pNaviLine->nRouteLineColorG;
		nuPen.nBlue	= pNaviLine->nRouteLineColorB;
		if( nShowMode )//3D mode
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pNaviLine->nRouteLineWidth;
			nuBrush.nRed	= nuPen.nRed;
			nuBrush.nGreen	= nuPen.nGreen;
			nuBrush.nBlue	= nuPen.nBlue;
			*pOut = pNaviLine->nRouteLineWidth;
			g_pMtGdiApi->GdiSetBrush(&nuBrush);
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_3D_NAVILINE;
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pNaviLine->nRouteLineWidth;// +((pNaviLine->nRouteLineWidth + 1) & 0x0001);
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_2D_NAVILINE;
		}
	}
	else if( nDrawType == DRAW_TYPE_NAVI_ARROW )
	{
		PNAVILINESETTING pNaviLine = (PNAVILINESETTING)lpSet;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nRed		= pNaviLine->nRouteArrowRimColorR;
		nuPen.nGreen	= pNaviLine->nRouteArrowRimColorG;
		nuPen.nBlue		= pNaviLine->nRouteArrowRimColorB;
		nuPen.nWidth	= 1;
		nuBrush.nRed	= pNaviLine->nRouteArrowColorR;
		nuBrush.nGreen	= pNaviLine->nRouteArrowColorG;
		nuBrush.nBlue	= pNaviLine->nRouteArrowColorB;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		g_pMtGdiApi->GdiSetBrush(&nuBrush);
		if( nShowMode )
		{
			return DRAW_OBJ_3D_NAVIARROW;
		}
		else
		{
			return DRAW_OBJ_2D_NAVIARROW;
		}
	}
	else if( nDrawType == DRAW_TYPE_ZOOM_ROADRIM )//Zoom Road Rim line
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowRimLine && !pRoadSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pRoadSet->nRimLineColorR;
		nuPen.nGreen= pRoadSet->nRimLineColorG;
		nuPen.nBlue	= pRoadSet->nRimLineColorB;
		if( nShowMode )//3D mode
		{
			if( pRoadSet->nShowCenterLine && pRoadSet->nCenterLineWidth >= _3D_LINE_CENTER_WIDTH )
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = 1;
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				*pOut = pRoadSet->nRimLineWidth;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				g_pMtGdiApi->GdiSetPen(&nuPen);
				/*
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nRimLineWidth;
				nuSetGdiPen(&nuPen);
				*/
				return DRAW_OBJ_ZOOM_3D_BIRDVIEW;
			}
			else
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nRimLineWidth;
				g_pMtGdiApi->GdiSetPen(&nuPen);
				return DRAW_OBJ_ZOOM_3D_LINE;
			}
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pRoadSet->nRimLineWidth;
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_ZOOM_2D_LINE;
		}
	}
	else if( nDrawType == DRAW_TYPE_ZOOM_ROADCENTER )//Road center line
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pRoadSet->nCenterLineColorR;
		nuPen.nGreen= pRoadSet->nCenterLineColorG;
		nuPen.nBlue	= pRoadSet->nCenterLineColorB;
		if( nShowMode )//3D mode
		{
			if( pRoadSet->nShowCenterLine && pRoadSet->nCenterLineWidth >= _3D_LINE_CENTER_WIDTH )
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = 1;
				nuBrush.nRed	= nuPen.nRed;
				nuBrush.nGreen	= nuPen.nGreen;
				nuBrush.nBlue	= nuPen.nBlue;
				*pOut = pRoadSet->nCenterLineWidth;
				g_pMtGdiApi->GdiSetBrush(&nuBrush);
				g_pMtGdiApi->GdiSetPen(&nuPen);
				/*
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nCenterLineWidth;
				nuSetGdiPen(&nuPen);
				*/
				return DRAW_OBJ_ZOOM_3D_BIRDVIEW;
			}
			else
			{
				nuPen.nStyle = NURO_PS_SOLID;
				nuPen.nWidth = pRoadSet->nCenterLineWidth;
				g_pMtGdiApi->GdiSetPen(&nuPen);
				return DRAW_OBJ_ZOOM_3D_LINE;
			}
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pRoadSet->nCenterLineWidth;
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_ZOOM_2D_LINE;
		}
		
	}
	else if( nDrawType == DRAW_TYPE_ZOOM_ROADCLINE )
	{
		PCROADSETTING pRoadSet = (PCROADSETTING)lpSet;
		if( !pRoadSet->nShowCenterLine )
		{
			return DRAW_OBJ_SKIP;
		}
		nuPen.nRed	= pRoadSet->nRimLineColorR;
		nuPen.nGreen= pRoadSet->nRimLineColorG;
		nuPen.nBlue	= pRoadSet->nRimLineColorB;
		nuPen.nStyle = NURO_PS_SOLID;
		nuPen.nWidth = 1;
		g_pMtGdiApi->GdiSetPen(&nuPen);
		if( nShowMode )//3D mode
		{
			return DRAW_OBJ_3D_DASH;
		}
		else
		{
			return DRAW_OBJ_2D_DASH;
		}
	}
	else if( nDrawType == DRAW_TYPE_ZOOM_NAVI_ROAD )
	{
		
		PNAVILINESETTING pNaviLine = (PNAVILINESETTING)lpSet;
		nuPen.nRed	= pNaviLine->nRouteLineColorR;
		nuPen.nGreen= pNaviLine->nRouteLineColorG;
		nuPen.nBlue	= pNaviLine->nRouteLineColorB;
		if( nShowMode )//3D mode
		{
			nuPen.nStyle	= NURO_PS_SOLID;
			nuPen.nWidth	= 1;
			nuBrush.nRed	= nuPen.nRed;
			nuBrush.nGreen	= nuPen.nGreen;
			nuBrush.nBlue	= nuPen.nBlue;
			*pOut = pNaviLine->nRouteLineWidth;
			g_pMtGdiApi->GdiSetBrush(&nuBrush);
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_ZOOM_3D_BIRDVIEW;
		}
		else
		{
			nuPen.nStyle = NURO_PS_SOLID;
			nuPen.nWidth = pNaviLine->nRouteLineWidth;
			g_pMtGdiApi->GdiSetPen(&nuPen);
			return DRAW_OBJ_ZOOM_2D_LINE;
		}
	}
	else
	{
		return DRAW_OBJ_SKIP;
	}
}

nuBOOL CGdiObjectK::DrawObject(nuINT nObjType, PNUROPOINT pPt, nuINT nCount, nuLONG nExtend)
{
	if( nObjType <= DRAW_OBJ_SKIP/* || pPt == NULL || nCount < 2 */)
	{
		return nuFALSE;
	}
	switch( nObjType )
	{
		case DRAW_OBJ_2D_LINE:
			g_pMtGdiApi->GdiPolyline(pPt, nCount);
			break;
		case DRAW_OBJ_3D_LINE:
//old			m_viewTransf._3DPolyline(pPt, nCount);
			g_pObj3DDraw->Poly3DLine(pPt, nCount);
			break;
		case DRAW_OBJ_3D_BIRDVIEW:
//			m_viewTransf._3DPolyline(pPt, nCount, nExtend);
//old			m_viewTransf._3DBirdViewPolyline(pPt, nCount, nExtend);
			g_pObj3DDraw->Poly3DLine(pPt, nCount, nExtend);
			break;
		case DRAW_OBJ_2D_DASH:
			g_pMtGdiApi->GdiDashLine(pPt, nCount, nExtend);
			break;
		case DRAW_OBJ_3D_DASH:
//old			m_viewTransf._3DDashline(pPt, nCount, nExtend);
			g_pObj3DDraw->Dash3DLine(pPt, nCount, nExtend);
			break;
		case DRAW_OBJ_2D_AREA:
			g_pMtGdiApi->GdiPolygon(pPt, nCount);
			break;
		case DRAW_OBJ_3D_AREA:
//old			m_viewTransf._3DPolygon(pPt, nCount);
			g_pObj3DDraw->Poly3DGon(pPt, nCount);
			break;
		case DRAW_OBJ_2D_NAVIARROW:
		case DRAW_OBJ_3D_NAVIARROW:
		case DRAW_OBJ_ZOOM_2D_NAVIARROW:
		case DRAW_OBJ_ZOOM_3D_NAVIARROW:
			//DrawNaviTrigon(pPt, nCount, 10, nObjType);
			DrawNaviTrigon(pPt, nCount, 7, nObjType);
			break;
		case DRAW_OBJ_2D_NAVILINE:
			g_pMtGdiApi->GdiPolyline(pPt, nCount);
			break;
		case DRAW_OBJ_3D_NAVILINE:
//			m_viewTransf._3DPolyline(pPt, nCount, nExtend);
//old			m_viewTransf._3DBirdViewPolyline(pPt, nCount, nExtend);
			g_pObj3DDraw->Poly3DLine(pPt, nCount, nExtend);
			break;
		case DRAW_OBJ_2D_ARROWLINE:
		case DRAW_OBJ_ZOOM_2D_ARROWLINE:
			DrawNavi2DArrowLine(pPt, nCount, nuLOWORD(nExtend), nObjType);
			break;
		case DRAW_OBJ_3D_ARROWLINE:
			Draw3DIndicateArrow(pPt, nCount, nuLOWORD(nExtend), nObjType);
			break;
		case DRAW_OBJ_ZOOM_3D_ARROWLINE:
			nuroBRUSH brush;
			nuroPEN	 pen;
			brush.nRed		= 0;
			brush.nGreen	= 0;
			brush.nBlue		= 0;
			g_pMtGdiApi->GdiSetBrush(&brush);
			pen.nRed		= 0;
			pen.nGreen		= 0;
			pen.nBlue		= 0;
			pen.nWidth		= 0;
			pen.nStyle		= NURO_PS_NULL;
			g_pMtGdiApi->GdiSetPen(&pen);
			if( m_nRedArrowLintW != 0 )
			{
				DrawNavi3DArrowLine(pPt, nCount, nuLOWORD(nExtend) + m_nRedArrowLintW * 2, nObjType, nuTRUE);
			}
			brush.nRed		= 255;
			brush.nGreen	= 0;
			brush.nBlue		= 0;
			g_pMtGdiApi->GdiSetBrush(&brush);
			pen.nRed		= 255;
			pen.nGreen		= 0;
			pen.nBlue		= 0;
			pen.nWidth		= 0;
			pen.nStyle		= NURO_PS_NULL;
			g_pMtGdiApi->GdiSetPen(&pen);
			DrawNavi3DArrowLine(pPt, nCount, nuLOWORD(nExtend), nObjType, nuFALSE);
			break;
		case DRAW_OBJ_SQUARE:
			DrawBgSquare(nObjType);
			break;
		//Below for Zoom Screen only
		case DRAW_OBJ_ZOOM_2D_LINE:
			g_pMtGdiApi->GdiPolyline(pPt, nCount);
			break;
		case DRAW_OBJ_ZOOM_3D_LINE:
			m_zoomTransf._3DPolyline(pPt, nCount);
			break;
		case DRAW_OBJ_ZOOM_3D_BIRDVIEW:
			m_zoomTransf._3DPolyline(pPt, nCount, nExtend);
			break;
		default:
			return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CGdiObjectK::SetViewParam()
{
	nuLONG nRes = 0;
	if( m_nGoNavi )
	{
		if( 1 == g_pMtFsApi->pGdata->uiSetData.nScaleIdx )
		{
			nRes = m_viewTransf.SetParam( g_pMtFsApi->pGdata->transfData.nBmpWidth, 
				g_pMtFsApi->pGdata->transfData.nBmpHeight, 
				2.0, 
				0.95, 
				0.10,
				3,
				2 );
		}
		else if( 2 == g_pMtFsApi->pGdata->uiSetData.nScaleIdx )
		{
			nRes = m_viewTransf.SetParam( g_pMtFsApi->pGdata->transfData.nBmpWidth, 
				g_pMtFsApi->pGdata->transfData.nBmpHeight, 
				1.2, 
				0.80, 
				0.10,
				2,
				1 );
		}
		else
		{
			nRes = m_viewTransf.SetParam( g_pMtFsApi->pGdata->transfData.nBmpWidth, 
				g_pMtFsApi->pGdata->transfData.nBmpHeight, 
				1.0, 
				0.70, 
				0.10,
				3,
				1 );
		}
	}
	else
	{
		
		nRes = m_viewTransf.SetParam( g_pMtFsApi->pGdata->transfData.nBmpWidth, 
			g_pMtFsApi->pGdata->transfData.nBmpHeight );
	}
	if( nRes <= 0 )
	{
		return nuFALSE;
	}
	g_pMtFsApi->pGdata->uiSetData.nSkyHeight = (nuWORD)nRes;
	if( g_pMtFsApi->pGdata->zoomScreenData.bZoomScreen )
	{
		g_pMtFsApi->pGdata->zoomScreenData.nZoomSkyHeight = (nuWORD)
			m_zoomTransf.SetParam( g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpWidth,
								   g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpHeight,
								   1.0, 
								   0.75,
								   0.2 );
	}
	return nuTRUE;
}

nuBOOL CGdiObjectK::Poly3Dline(PNUROPOINT pPt, nuINT nCount, nuLONG nLineWidth, class CViewTransf *p3DTransf)
{
	return nuTRUE;
}

nuVOID CGdiObjectK::Bmp2Dto3D(nuLONG &x, nuLONG &y, nuBYTE nMapType)
{
	if( nMapType == MAP_ZOOM )
	{
		m_zoomTransf.Screen2Dto3D(x, y);
	}
	else
	{
		m_viewTransf.Screen2Dto3D(x, y);
	}
}
nuVOID CGdiObjectK::Bmp3Dto2D(nuLONG &x, nuLONG &y, nuBYTE nMapType)
{
	if( nMapType == MAP_ZOOM )
	{
		m_zoomTransf.Screen3Dto2D(x, y);
	}
	else
	{
		m_viewTransf.Screen3Dto2D(x, y);
	}
}

#define TRIAL1				2
#define DISOFTWOARROWS		30
nuBOOL CGdiObjectK::DrawNaviTrigon(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjType )
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuLONG dis	= 0;
	nuLONG disx = 0;
	nuLONG disy = 0;
	nuLONG nDisGone = 0, nLen = 0, width = 0;
	nuroPOINT arrayPt[4] = {0};
	nuroPOINT tmpPt = {0}, ptCenter = {0}/*, p1, p2*/;
	nuroRECT screen = {0};
	nuLONG nMapMode = 0;
	if( nObjType >= DRAW_OBJ_ZOOM )
	{
		screen.left		= 0;
		screen.top		= 0;
		screen.right	= g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpWidth;
		screen.bottom	= g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpHeight;
		if( g_pMtFsApi->pGdata->zoomScreenData.b3DMode )
		{
			//screen.top	+= DISOFTWOARROWS*2;
			screen.top += g_pMtFsApi->pGdata->uiSetData.nSkyHeight - 10;
		}
	}
	else
	{
		screen.left		= 0;
		screen.top		= 0;
		screen.right	= g_pMtFsApi->pGdata->transfData.nBmpWidthEx;
		screen.bottom	= g_pMtFsApi->pGdata->transfData.nBmpHeightEx;
		if( g_pMtFsApi->pGdata->uiSetData.b3DMode )
		{
			//screen.top += DISOFTWOARROWS*2;
			screen.top += g_pMtFsApi->pGdata->uiSetData.nSkyHeight - 10;
		}
	}
	for(nuLONG i = 1; i < nCount; i++)
	{
		disx = pPt[i].x - pPt[i-1].x;
		disy = pPt[i].y - pPt[i-1].y;
		dis = (nuLONG)nuSqrt(disx*disx + disy*disy);
		if( dis == 0 || nDisGone + dis < m_nTwoArrowsDis )
		{
			nDisGone += dis;
			continue;
		}
		nLen = m_nTwoArrowsDis - nDisGone;
		nDisGone += dis;
		while( nDisGone >= m_nTwoArrowsDis )
		{
			tmpPt.x = pPt[i-1].x + (nLen)*disx/dis;
			tmpPt.y = pPt[i-1].y + (nLen)*disy/dis;
			if( g_pMtFsApi->pGdata->uiSetData.b3DMode )
			{
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(tmpPt.x, tmpPt.y, nMapMode);
			}
			if( nuPtInRect(tmpPt, screen) )
			{
				ptCenter = tmpPt;
				width = nWidth;
				arrayPt[0].x = ptCenter.x + ((width-TRIAL1) * disx / dis);
				arrayPt[0].y = ptCenter.y + ((width-TRIAL1) * disy / dis);
				arrayPt[2].x = ptCenter.x - ((width-m_lArrowListWidth) * disy / dis);
				arrayPt[2].y = ptCenter.y + ((width-m_lArrowListWidth) * disx / dis);
				arrayPt[1].x = ptCenter.x + ((width-m_lArrowListWidth) * disy / dis);
				arrayPt[1].y = ptCenter.y - ((width-m_lArrowListWidth) * disx / dis);
				if( nObjType == DRAW_OBJ_ZOOM_3D_NAVIARROW )
				{
					m_zoomTransf._3DPolygon(arrayPt, 3);
				}
				else if( nObjType == DRAW_OBJ_3D_NAVIARROW )
				{
					//m_viewTransf._3DPolygon(arrayPt, 3);
					g_pMtGdiApi->GdiPolygon(arrayPt, 3);
				}
				else
				{
					g_pMtGdiApi->GdiPolygon(arrayPt, 3);
				}
			}
			if( g_pMtFsApi->pGdata->uiSetData.b3DMode )
			{
				nDisGone -= m_nTwoArrowsDis*2;
				nLen += m_nTwoArrowsDis*2;
			}
			else
			{
				nDisGone -= m_nTwoArrowsDis*2;
				nLen += m_nTwoArrowsDis*2;
			}
		}
	}
	return nuTRUE;
}
/*****************************
* 画3D红色路口指示箭头函数
*****************************/
nuBOOL CGdiObjectK::DrawNavi3DArrowLine(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType, nuBOOL bRim /* = nuTRUE */)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuLONG x = 0, bottom = 0;
	CViewTransf *pVT = NULL;
	if( nObjeType >= DRAW_OBJ_ZOOM )
	{
		x = g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpWidth / 2;
		bottom = g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpHeight + SCREEN_3D_BOTTOM_EXLEN;
		pVT = &m_zoomTransf;
	}
	else
	{
		x = g_pMtFsApi->pGdata->transfData.nBmpWidthEx / 2;
		bottom = g_pMtFsApi->pGdata->transfData.nBmpHeight + SCREEN_3D_BOTTOM_EXLEN;
		pVT = &m_viewTransf;
	}
	nuLONG z = 0;
//old	pVT->Screen3Dto2D(x, bottom);
	g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(x, bottom, z);
	nuLONG nLen = nWidth, dx = 0, dy = 0;
	nuLONG dxy = 0;
	nuBOOL bLastOutScreen = nuFALSE; 
	nuBOOL bNowOutScreen  = nuFALSE;
	nuroPOINT	pt1 = {0}, pt2 = {0}, newPt1 = {0}, newPt2 = {0};
	nuroPOINT	ptList[4] = {0}, p1 = {0}, p2 = {0};
	nuroRECT	rt = {0};

	dx = pPt[1].x - pPt[0].x;
	dy = pPt[1].y - pPt[0].y;
	dxy = (nuLONG)nuSqrt(dx*dx+dy*dy);
	while( dxy < 2 )
	{
		pPt = &pPt[1];
		nCount--;
		if( nCount < 2 )
		{
			return nuFALSE;
		}
		dx = pPt[1].x - pPt[0].x;
		dy = pPt[1].y - pPt[0].y;
		dxy = (nuLONG)nuSqrt(dx*dx + dy*dy);
	}
	pt1 = pPt[0];
	if( bRim )//extend the pt[0] of Rimline
	{
		pt1.x = pPt[0].x - dx * m_nRedArrowLintW/dxy;
		pt1.y = pPt[0].y - dy * m_nRedArrowLintW/dxy;
	}
	if( pt1.y > bottom )
	{
		bLastOutScreen = nuTRUE;
	}
	else
	{
		newPt1 = pt1;
		bLastOutScreen = nuFALSE;
//old		pVT->Screen2Dto3D(newPt1.x, newPt1.y);
		g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(newPt1.x, newPt1.y, z);
	}
	for(nuLONG i = 1; i < nCount; i++)
	{
		pt2 = pPt[i];
		if( pt2.y > bottom )
		{
			bNowOutScreen = nuTRUE;
		}
		else
		{
			bNowOutScreen = nuFALSE;
		}
		if( !bLastOutScreen || !bNowOutScreen )
		{
			if( bLastOutScreen )
			{ 
				newPt1.x = pt2.x - (pt2.y - bottom)*(pt2.x - pt1.x) / (pt2.y - pt1.y );//pt2.y != pt1.y, it is always sure!
				newPt1.y = bottom;
				pt1 = newPt1;
//Old				pVT->Screen2Dto3D(newPt1.x, newPt1.y);
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(newPt1.x, newPt1.y, z);
			}
			newPt2 = pt2;
			if( bNowOutScreen )
			{
				newPt2.x = pt1.x - (pt1.y - bottom)*(pt1.x - pt2.x) / (pt1.y - pt2.y);//pt2.y != pt2.y
				newPt2.y = bottom;
				pt2 = newPt2;
			}
//Old			pVT->Screen2Dto3D(newPt2.x, newPt2.y);
			g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(newPt2.x, newPt2.y, z);
			dx = newPt2.x - newPt1.x;
			dy = newPt2.y - newPt1.y;
			dxy = (nuLONG)nuSqrt(dx*dx+dy*dy);
			if( dxy != 0 )
			{
				//
				p1.x = pt2.x - nWidth/2;
				p2.x = pt2.x + (nWidth+1)/2;
				p1.y = p2.y = pt2.y;
//old				pVT->Screen2Dto3D(p1.x, p1.y);
//old				pVT->Screen2Dto3D(p2.x, p2.y);
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p1.x, p1.y, z);
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p2.x, p2.y, z);
				nLen = p2.x - p1.x;
				//
				ptList[2].x = newPt2.x + (dy*nLen/(2*dxy));
				ptList[2].y = newPt2.y - (dx*nLen/(2*dxy));
				ptList[3].x = newPt2.x - (dy*nLen/(2*dxy));
				ptList[3].y = newPt2.y + (dx*nLen/(2*dxy));
				p1.x = pt1.x - nWidth/2;
				p2.x = pt1.x + (nWidth+1)/2;
				p1.y = p2.y = pt1.y;
//				pVT->Screen2Dto3D(p1.x, p1.y);
//				pVT->Screen2Dto3D(p2.x, p2.y);
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p1.x, p1.y, z);
				g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p2.x, p2.y, z);
				nLen = p2.x - p1.x;
				ptList[0].x = newPt1.x - (dy*nLen/(2*dxy));
				ptList[0].y = newPt1.y + (dx*nLen/(2*dxy));
				ptList[1].x = newPt1.x + (dy*nLen/(2*dxy));
				ptList[1].y = newPt1.y - (dx*nLen/(2*dxy));
				if( i > 1 )//Draw circle in connections
				{
					rt.left = newPt1.x - (nLen-1)/2;
					rt.top	= newPt1.y - (nLen-1)/2;
					rt.right	= newPt1.x + (nLen+2)/2;
					rt.bottom	= newPt1.y + (nLen+2)/2;
					g_pMtGdiApi->GdiEllipse(rt.left, rt.top, rt.right, rt.bottom);
				}
				g_pMtGdiApi->GdiPolygon(ptList, 4);
				//Draw Trangle Arrow
				if( i == nCount - 1 )//
				{
					p1.x = pt2.x - nWidth/2;
					p2.x = pt2.x + (nWidth+1)/2;
					p1.y = p2.y = pt2.y;
//					pVT->Screen2Dto3D(p1.x, p1.y);
//					pVT->Screen2Dto3D(p2.x, p2.y);
					g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p1.x, p1.y, z);
					g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(p2.x, p2.y, z);
					nLen = p2.x - p1.x;
					ptList[2].x = newPt2.x + (dx*nLen*3/(4*dxy));
					ptList[2].y = newPt2.y + (dy*nLen*3/(4*dxy));
					if( bRim )//extend the pt[0] of Rimline
					{
						newPt2.x = newPt2.x - dx*m_nRedArrowLintW/dxy;
						newPt2.y = newPt2.y - dy*m_nRedArrowLintW/dxy;
					}
					ptList[0].x = newPt2.x + (dy*nLen/(dxy));
					ptList[0].y = newPt2.y - (dx*nLen/(dxy));
					ptList[1].x = newPt2.x - (dy*nLen/(dxy));
					ptList[1].y = newPt2.y + (dx*nLen/(dxy));
					g_pMtGdiApi->GdiPolygon(ptList, 3);
					break;
				}
			}
		}
		pt1 = pPt[i];
		newPt1 = newPt2;
		bLastOutScreen = bNowOutScreen;
	}
	return nuTRUE;
}

nuBOOL CGdiObjectK::DrawNavi2DArrowLine(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuLONG dx = 0, dy = 0, dxy = 0;
	nuroPOINT pt1 = {0};
	nuroPEN pen = {0};
	nuroBRUSH brush = {0};
	//Draw Rim Line
	pen.nStyle	= NURO_PS_NULL;
	pen.nRed	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorR;
	pen.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorG;
	pen.nBlue	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorB;
	if( m_nRedArrowLintW != 0 )
	{
		pen.nWidth	= m_nRedArrowLintW * 2 + nWidth;
		pen.nStyle	= NURO_PS_SOLID;
		g_pMtGdiApi->GdiSetPen(&pen);
		g_pMtGdiApi->GdiPolyline(pPt, nCount);
	}
	//Draw Trangel Arrow
	pen.nWidth		= m_nRedArrowLintW;
	g_pMtGdiApi->GdiSetPen(&pen);
	brush.nRed		= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorR;//m_nArrowColorR;//m_nArrowColor / 65536;
	brush.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorG;//m_nArrowColorG;//m_nArrowColor % 65536 / 256;
	brush.nBlue		= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorB;//m_nArrowColorB;//m_nArrowColor % 256;
	g_pMtGdiApi->GdiSetBrush(&brush);
	pt1 = pPt[nCount-1];
	for(nuLONG i = nCount-2; i >= 0; i-- )
	{
		dx = pt1.x - pPt[i].x;
		dy = pt1.y - pPt[i].y;
		dxy = (nuLONG)nuSqrt(dx*dx + dy*dy);
		if( dxy > m_nRedArrowLintW )
		{
			break;
		}
	}
	if( dxy > m_nRedArrowLintW )
	{
		nuroPOINT ptList[3];
		ptList[0].x = pt1.x + 3*dx*nWidth/(2*dxy);
		ptList[0].y = pt1.y + 3*dy*nWidth/(2*dxy);
		pt1.x = pt1.x - dx * (m_nRedArrowLintW+1)/dxy;
		pt1.y = pt1.y - dy * (m_nRedArrowLintW+1)/dxy;
		ptList[1].x = pt1.x + 3*dy*nWidth/(2*dxy);
		ptList[1].y = pt1.y - 3*dx*nWidth/(2*dxy);
		ptList[2].x = pt1.x - 3*dy*nWidth/(2*dxy);
		ptList[2].y = pt1.y + 3*dx*nWidth/(2*dxy);
		g_pMtGdiApi->GdiPolygon(ptList, 3);
	}
	//Draw Center Line
	pen.nWidth	= nuLOBYTE(nWidth);
	pen.nRed	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorR;//m_nArrowColorR;//m_nArrowColor / 65536;;
	pen.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorG;//m_nArrowColorG;//m_nArrowColor % 65536 / 256;
	pen.nBlue	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorB;//m_nArrowColorB;//m_nArrowColor % 256;
	g_pMtGdiApi->GdiSetPen(&pen);
	dx = pPt[1].x - pPt[0].x;
	dy = pPt[1].y - pPt[0].y;
	dxy = (nuLONG)nuSqrt(dx*dx+dy*dy);
	if( dxy > m_nRedArrowLintW )
	{
		pt1.x = pPt[0].x + dx*m_nRedArrowLintW/dxy;
		pt1.y = pPt[0].y + dy*m_nRedArrowLintW/dxy;
		pPt[0] = pt1;
	}
	g_pMtGdiApi->GdiPolyline(pPt, nCount);
	return nuTRUE;
}

#define	SQUAREW			40
nuBOOL CGdiObjectK::DrawBgSquare(nuINT nObjType)
{
	nuBYTE b3Dmode = 0;
	nuLONG nWidth = 0, nHeight = 0;
	nuLONG nTop = 0, nLeft = 0;
	CViewTransf *pVf = NULL;
	if( nObjType >= DRAW_OBJ_ZOOM )
	{
		b3Dmode = g_pMtFsApi->pGdata->zoomScreenData.b3DMode;
		nWidth	= g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpWidth;
		nHeight	= g_pMtFsApi->pGdata->zoomScreenData.nZoomBmpHeight;
		nTop	= 0;
		nLeft	= 0;
		pVf		= &m_zoomTransf;
	}
	else
	{
		b3Dmode = g_pMtFsApi->pGdata->uiSetData.b3DMode;
		nWidth	= g_pMtFsApi->pGdata->transfData.nBmpWidth;
		nHeight	= g_pMtFsApi->pGdata->transfData.nBmpHeight;
		nLeft	= g_pMtFsApi->pGdata->transfData.nBmpLTx;
		nTop	= g_pMtFsApi->pGdata->transfData.nBmpLTy;
		pVf		= &m_viewTransf;
	}
	if( !b3Dmode )
	{
		return nuFALSE;
	}
	nuroPEN pen = {0};
	pen.nRed	= g_pMtFsApi->pMapCfg->naviLineSetting.nBgGroundColorR;
	pen.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nBgGroundColorG;
	pen.nBlue	= g_pMtFsApi->pMapCfg->naviLineSetting.nBgGroundColorB;
	pen.nStyle	= NURO_PS_SOLID;
	pen.nWidth	= 1;
	g_pMtGdiApi->GdiSetPen(&pen);
	nuroPOINT pt[2] = {0};
	nuLONG i = 0;
	for( i = 1; i <= (nWidth / SQUAREW); i++ )
	{
		pt[0].x = SQUAREW * i;
		pt[0].y = nTop;
		pt[1].x = SQUAREW * i;
		pt[1].y = nTop + nHeight;
//		pVf->_3DPolyline(pt, 2);
		g_pObj3DDraw->Poly3DLine(pt, 2);
	}
	for( i = 1; i <= (nHeight / SQUAREW); i++ )
	{
		pt[0].y = SQUAREW * i;
		pt[0].x = nLeft;
		pt[1].y = SQUAREW * i;
		pt[1].x = nLeft + nWidth;
//		pVf->_3DPolyline(pt, 2);
		g_pObj3DDraw->Poly3DLine(pt, 2);
	}
	return nuTRUE;
}

nuBOOL CGdiObjectK::Draw3DIndicateArrow(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuLONG z = 0;
	nuLONG dx = 0, dy = 0, dxy = 0;
	nuroPOINT pt1 = {0};
	nuroPEN pen = {0};
	nuroBRUSH brush = {0};
	//Draw Rim Line
	pen.nStyle	= NURO_PS_NULL;
	pen.nRed	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorR;
	pen.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorG;
	pen.nBlue	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowRimColorB;
	if( m_nRedArrowLintW != 0 )
	{
		pen.nWidth	= m_nRedArrowLintW * 2 + nWidth;
		pen.nStyle	= NURO_PS_SOLID;
		g_pMtGdiApi->GdiSetPen(&pen);
//		g_pMtGdiApi->GdiPolyline(pPt, nCount);
//old		m_viewTransf._3DBirdViewPolyline(pPt, nCount, pen.nWidth);
		g_pObj3DDraw->Poly3DLine(pPt, nCount, pen.nWidth);
	}
	//Draw Trangel Arrow
	pen.nWidth		= m_nRedArrowLintW;
	g_pMtGdiApi->GdiSetPen(&pen);
	brush.nRed		= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorR;//m_nArrowColorR;//m_nArrowColor / 65536;
	brush.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorG;//m_nArrowColorG;//m_nArrowColor % 65536 / 256;
	brush.nBlue		= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorB;//m_nArrowColorB;//m_nArrowColor % 256;
	g_pMtGdiApi->GdiSetBrush(&brush);
	pt1 = pPt[nCount-1];
	for(nuLONG i = nCount-2; i >= 0; i-- )
	{
		dx = pt1.x - pPt[i].x;
		dy = pt1.y - pPt[i].y;
		dxy = (nuLONG)nuSqrt(dx*dx + dy*dy);
		if( dxy > m_nRedArrowLintW )
		{
			break;
		}
	}
	if( dxy > m_nRedArrowLintW )
	{
//Old		m_viewTransf.Screen2Dto3D(pt1.x, pt1.y);
		g_pObj3DDraw->m_obj3DCtrl.Screen2Dto3D(pt1.x, pt1.y, z);
		short nOldW = nWidth;
//Old		nWidth = (short)m_viewTransf.Get3DThick(nWidth, pt1.y);
		nWidth = (short)g_pObj3DDraw->m_obj3DCtrl.Get3DThick(nWidth, pt1.y);
		nuroPOINT ptList[3];
		ptList[0].x = pt1.x + 3*dx*nWidth/(2*dxy);
		ptList[0].y = pt1.y + 3*dy*nWidth/(2*dxy);
		pt1.x = pt1.x - dx * (m_nRedArrowLintW+1)/dxy;
		pt1.y = pt1.y - dy * (m_nRedArrowLintW+1)/dxy;
		ptList[1].x = pt1.x + 3*dy*nWidth/(2*dxy);
		ptList[1].y = pt1.y - 3*dx*nWidth/(2*dxy);
		ptList[2].x = pt1.x - 3*dy*nWidth/(2*dxy);
		ptList[2].y = pt1.y + 3*dx*nWidth/(2*dxy);
		g_pMtGdiApi->GdiPolygon(ptList, 3);
//		m_viewTransf._3DPolygon(ptList, 3);
		nWidth = nOldW;
	}
	//Draw Center Line
	pen.nWidth	= nuLOBYTE(nWidth);
	pen.nRed	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorR;//m_nArrowColorR;//m_nArrowColor / 65536;
	pen.nGreen	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorG;//m_nArrowColorG;//m_nArrowColor % 65536 / 256;
	pen.nBlue	= g_pMtFsApi->pMapCfg->naviLineSetting.nRouteArrowColorB;//m_nArrowColorB;//m_nArrowColor % 256;
	g_pMtGdiApi->GdiSetPen(&pen);
	/*
	dx = pPt[1].x - pPt[0].x;
	dy = pPt[1].y - pPt[0].y;
	dxy = (nuLONG)nuSqrt(dx*dx+dy*dy);
	if( dxy > m_nRedArrowLintW )
	{
		pt1.x = pPt[0].x + dx*m_nRedArrowLintW/dxy;
		pt1.y = pPt[0].y + dy*m_nRedArrowLintW/dxy;
		pPt[0] = pt1;
	}
	*/
//	g_pMtGdiApi->GdiPolyline(pPt, nCount);
//Old	m_viewTransf._3DBirdViewPolyline(pPt, nCount, pen.nWidth);
	g_pObj3DDraw->Poly3DLine(pPt, nCount, pen.nWidth);
	return nuTRUE;
}

