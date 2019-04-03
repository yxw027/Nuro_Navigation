// GdiApiTrans.cpp: implementation of the CGdiApiTrans class.
//
//////////////////////////////////////////////////////////////////////

#include "GdiApiTrans.h"
//#include "DrawIcon.h"
#include "CPngCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define POIFILE	(nuTCHAR*)nuTEXT("media\\POI_16_565.bmp")
#define SMALL_POIFILE (nuTCHAR*)nuTEXT("media\\small_poi_16_565.bmp")
#define NICFILE	(nuTCHAR*)nuTEXT("media\\NUICON16.NIC")
#define NURO_POI_SIZE_16	16

//PNURO_API_GDI     g_pGdiApiForNew = NULL;
NURO_API_GDI		g_GdiApiForNew;

static nuFILE*		g_fpIcon = NULL;
static nuBYTE*		g_pIconInfo = NULL;
static NURO_BMP	    g_PoiBMP16;	
static NURO_BMP     g_smallPoiBmp; //Added by XiaoQin @2012.11.02

PNURO_CANVAS     CGdiApiTrans::g_pCanvas = NULL;
CPngCtrlLU		 m_pngctrl;


CGdiApiTrans::CGdiApiTrans()
{

}

CGdiApiTrans::~CGdiApiTrans()
{
	nuGdiDelConfig();
}

//************************************Load Config************************************//
nuBOOL CGdiApiTrans::nuGdiLoadConfig()
{
    nuGdiDelConfig();
	nuTCHAR modulePath[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, modulePath, NURO_MAX_PATH);
	nuTCHAR tmpFile[NURO_MAX_PATH] = {0};
	
    //load poi bmp
    nuMemset(&g_PoiBMP16, 0, sizeof(NURO_BMP));
	nuTcscpy(tmpFile, modulePath);
	nuTcscat(tmpFile, POIFILE);	
    if( !nuGdiLoadNuroBMP(&g_PoiBMP16, tmpFile) )
    {
        nuGdiDelConfig();
        return nuFALSE;
    }
    g_PoiBMP16.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;

	//------------ Load Small POI Bitmap -----------------
	nuMemset(&g_smallPoiBmp, 0, sizeof(NURO_BMP));
	nuTcscpy(tmpFile, modulePath);
	nuTcscat(tmpFile, SMALL_POIFILE);	
	if( !nuGdiLoadNuroBMP(&g_smallPoiBmp, tmpFile) )
	{
		nuGdiDelNuroBMP(&g_smallPoiBmp);
		nuMemset(&g_smallPoiBmp, 0, sizeof(NURO_BMP));
	}
	g_smallPoiBmp.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
	//----------------------------------------------------

    //load icon 
	nuTcscpy(tmpFile, modulePath);
	nuTcscat(tmpFile, NICFILE);
    g_fpIcon = nuTfopen(tmpFile, NURO_FS_RB);
    if( g_fpIcon )
    {
        NUROICONFILEINFO iconFileInfo;
        if(nuFread(&iconFileInfo, sizeof(NUROICONFILEINFO), 1, g_fpIcon) == 1)
        {
            g_pIconInfo = (nuBYTE*)nuMalloc(sizeof(NUROICONFILEINFO) + iconFileInfo.nIconNum * sizeof(NUROICONINFO));
            if( NULL == g_pIconInfo )
            {
                nuGdiDelConfig();
                return nuFALSE;
            }
            nuFseek(g_fpIcon, 0, NURO_SEEK_SET);
            if( nuFread( g_pIconInfo, sizeof(NUROICONFILEINFO) + iconFileInfo.nIconNum * sizeof(NUROICONINFO), 
				         1, g_fpIcon) != 1 )
            {
                nuGdiDelConfig();
                return nuFALSE;
            }
        }
    }

	//png?B?z ???N?狾???Png?伀????_??
	nuTCHAR PngFileName[255] = {0};
	nuINT i = 0, pngid = 0;
	m_pngctrl.InitPngCtrl(100);//???l???q?n???s?]?w?L
	for(i=0;i<100;i++)
	{
		//#ifndef ANDROID
         #ifdef WINCE
		if(i<10)
		{
			wsprintf(tmpFile,L"%sMedia\\PIC\\PNG\\POI_000%ld.png",modulePath,i);
		}
		else
		{
			wsprintf(tmpFile,L"%sMedia\\PIC\\PNG\\POI_00%ld.png",modulePath,i);
		}
		#endif
		m_pngctrl.load_png(tmpFile,pngid,i);
	}
    return nuTRUE;
}

nuVOID CGdiApiTrans::nuGdiDelConfig()
{
	m_pngctrl.RelPngCtrl();

	nuGdiDelNuroBMP(&g_PoiBMP16);
    nuMemset(&g_PoiBMP16, 0, sizeof(NURO_BMP));

	nuGdiDelNuroBMP(&g_smallPoiBmp);
	nuMemset(&g_smallPoiBmp, 0, sizeof(NURO_BMP));
    
    if( NULL != g_pIconInfo )
    {
        nuFree(g_pIconInfo);
        g_pIconInfo = NULL;
    }
    
    if( NULL != g_fpIcon )
    {
        nuFclose(g_fpIcon);
        g_fpIcon = NULL;
    }
}

nuBOOL CGdiApiTrans::nuGdiInit(NURO_API_GDI* lpInNewGdi, nuPVOID lpOutGDIApi)
{
	//g_pGdiApiForNew = lpInNewGdi;
	g_GdiApiForNew = *lpInNewGdi;

	g_pCanvas = NULL;
	
	nuGdiLoadConfig();

	if( NULL != lpOutGDIApi )
	{
		PGDIAPI pGdiApi = (PGDIAPI)lpOutGDIApi;

        pGdiApi->GdiInitDC	= nuGdiInitDC;
        pGdiApi->GdiFreeDC	= nuGdiFreeDC;
        pGdiApi->GdiInitBMP	= nuGdiInitBMP;
        pGdiApi->GdiFreeBMP	= nuGdiFreeBMP;
        pGdiApi->GdiLoadNuroBMP	  = nuGdiLoadNuroBMP;
        pGdiApi->GdiCreateNuroBMP = nuGdiCreateNuroBMP;
        pGdiApi->GdiDelNuroBMP    = nuGdiDelNuroBMP;

        pGdiApi->GdiSetPen	 = nuGdiSetPen;
        pGdiApi->GdiDelPen	 = nuGdiDelPen;
        pGdiApi->GdiSetBrush = nuGdiSetBrush;
        pGdiApi->GdiDelBrush = nuGdiDelBrush;

        pGdiApi->GdiSetFont	 = nuGdiSetFont;
        pGdiApi->GdiDelFont	 = nuGdiDelFont;
		
        pGdiApi->GdiFlush	 = nuGdiFlush;
        pGdiApi->GdiDrawLine = nuGdiDrawLine;
        pGdiApi->GdiPolyline = nuGdiPolyline;
        pGdiApi->GdiPolygon	 = nuGdiPolygon;
        pGdiApi->GdiEllipse	 = nuGdiEllipse;
        pGdiApi->GdiSetTextColor = nuGdiSetTextColor;
        pGdiApi->GdiSetBKColor	 = nuGdiSetBKColor;
        pGdiApi->GdiSetBkMode	 = nuGdiSetBkMode;
        pGdiApi->GdiExtTextOutW	 = nuGdiExtTextOutW;
        pGdiApi->GdiDrawTextW	 = nuGdiDrawTextW;
        pGdiApi->GdiDrawIcon		= nuGdiDrawIcon;
		pGdiApi->GdiDrawTargetFlagIcon = nuGdiDrawTargetFlagIcon;
        pGdiApi->GdiDrawIconExtern	= nuGdiDrawIconExtern;
        pGdiApi->GdiDrawIconSp	    = nuGdiDrawIconSp;
        pGdiApi->GdiDashLine	    = nuGdiDashLine;
        pGdiApi->GdiSaveBMP	 = nuGdiSaveBMP;
        pGdiApi->GdiDrawBMP	 = nuGdiDrawBMP;
        pGdiApi->GdiDrawPNG	 = nuGdiDrawPNG;
        pGdiApi->GdiDrawPNG2 = nuGdiDrawPNG2;
        pGdiApi->GdiPaintBMP = nuGdiPaintBMP;
        pGdiApi->GdiSelectCanvasBMP	= nuGdiSelectCanvasBMP;
        pGdiApi->GdiDrawPoi	    = nuGdiDrawPoi;
        pGdiApi->GdiSetROP2	    = nuGdiSetROP2;
        pGdiApi->GdiPolygonEx	= nuGdiPolygonEx;
        pGdiApi->GdiPolylineEx	= nuGdiPolylineEx;
        
        pGdiApi->GdiExtTextOutWEx = nuGdiExtTextOutWEx;
        pGdiApi->GdiDrawTextWEx   = nuGdiDrawTextWEx;
        pGdiApi->GdiSet3DPara	  = nuGdiSet3DPara;
        pGdiApi->Gdi3DPolyline	  = nuGdi3DPolyline;
        
        pGdiApi->GdiGetCanvasBmp  = nuGdiGetCanvasBmp;
        pGdiApi->GdiDrawIconEx    = nuGdiDrawIconEx;
        pGdiApi->GdiBmpGradient   = nuGdiBmpGradient;
	pGdiApi->GdiZoomBmp       = nuGdiZoomBmp;
	}
	g_pCanvas = g_GdiApiForNew.GDI_GetCanvas(-1);

	return nuTRUE;
}

nuVOID CGdiApiTrans::nuGdiFree()
{
	g_GdiApiForNew.GDI_Free();
}

nuBOOL CGdiApiTrans::nuGdiInitDC(nuHDC hDC, nuINT nSW, nuINT nSH)
{
	return nuTRUE;
}

nuVOID CGdiApiTrans::nuGdiFreeDC()
{

}
nuBOOL CGdiApiTrans::nuGdiInitBMP(nuUINT nWith, nuUINT nHeight)
{		
    return nuTRUE;
}

nuVOID CGdiApiTrans::nuGdiFreeBMP()
{

}

PNURO_BMP CGdiApiTrans::nuGdiGetCanvasBmp()
{
	return &g_pCanvas->bitmap;
}

PNURO_CANVAS CGdiApiTrans::nuGdiGetCanvas()
{
	return g_GdiApiForNew.GDI_GetCanvas(-1);
}

nuBOOL CGdiApiTrans::nuGdiLoadNuroBMP(PNURO_BMP pNuroBmp, const nuTCHAR *tsFile)
{
	if( pNuroBmp == NULL )
	{
		return nuFALSE;
	}
    nuFILE* fp = nuTfopen(tsFile, NURO_FS_RB);
    if( fp == NULL )
    {
        return nuFALSE;
    }
    NURO_BITMAPFILEHEADER bmpFileHead;
    nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));

    if( nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1 )
    {
        nuFclose(fp);
        return nuFALSE;
    }
    NURO_BITMAPINFOHEADER bmpInfoHead;
    nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
    if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1 )
    {
        nuFclose(fp);
        return nuFALSE;
    }
	if( !pNuroBmp->bmpBitCount )
    {
        pNuroBmp->bmpBitCount = 16;
    }

    nuUINT DataSizePerLine = (bmpInfoHead.biWidth * 16+31)/32*4;

    nuINT arrTag = (bmpInfoHead.biHeight>0)?0:1;

    bmpInfoHead.biHeight = NURO_ABS(bmpInfoHead.biHeight);

    nuUINT DataSizePerLineNew = DataSizePerLine;//(((nuWORD)(bmpInfoHead.biWidth))* 16+7)/8;
    pNuroBmp->bytesPerLine = DataSizePerLineNew;
    pNuroBmp->bmpBuffLen   = pNuroBmp->bytesPerLine * ((nuWORD)bmpInfoHead.biHeight);
    nuBYTE* pBuff = (nuBYTE*)nuMalloc(pNuroBmp->bmpBuffLen);
    if( pBuff == NULL )
    {
        nuFree(pBuff);
        nuFclose(fp);
        return nuFALSE;
    }
    nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);
    if( nuFread(pBuff, pNuroBmp->bmpBuffLen, 1, fp) != 1 )
    {
        nuFree(pBuff);
        nuFclose(fp);
        return nuFALSE;
    }
	if( !arrTag )
    {
        nuINT i = 0;
        nuINT iEnd = bmpInfoHead.biHeight >> 1;
        nuBYTE *pBuffTag = pBuff;
        nuBYTE *pBuffTag2 = pBuffTag + (bmpInfoHead.biHeight - 1) * DataSizePerLine;      
        nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
        if( pLine == NULL )
        {
            nuFree(pLine);
            nuFclose(fp);
            return nuFALSE;
        }
		nuMemset(pLine, 0, DataSizePerLine);
        for( i = 0; i < iEnd; ++i )
        {
            nuMemcpy(pLine, pBuffTag, DataSizePerLine);
            nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLine);
            nuMemcpy(pBuffTag2, pLine, DataSizePerLine);
            pBuffTag += DataSizePerLine;
            pBuffTag2 -= DataSizePerLine;
        }
        nuFree(pLine);
    }
	nuFclose(fp);
    pNuroBmp->pBmpBuff	= pBuff;
    pNuroBmp->bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
    pNuroBmp->bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
    pNuroBmp->bmpHasLoad  = 1;
	pNuroBmp->bytesPerLine = pNuroBmp->bmpWidth * 2;
	pNuroBmp->bmpTransfColor = NURO_BMP_TRANSCOLOR_565;

	return nuTRUE;
}

//**************************************************************************
//**************************************************************************
nuBOOL CGdiApiTrans::nuGdiCreateNuroBMP(PNURO_BMP pNuroBmp)
{
	if( pNuroBmp == NULL )
    {
        return nuTRUE;
    }
    nuGdiDelNuroBMP(pNuroBmp);
    pNuroBmp->bmpBitCount = NURO_BMP_BITCOUNT;
    pNuroBmp->bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;

	pNuroBmp->bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
	
	nuINT nColSize = 2;//(pNuroBmp->bmpBitCount + 7)/8;


	nuBYTE biTmp[70] = {0};
	nuINT realBmpWidth = ((pNuroBmp->bmpWidth+1)/2) * 2;//just 565
	NURO_BITMAPINFO* bi = (NURO_BITMAPINFO*)biTmp;
	bi->bmiHeader.biSize = sizeof(NURO_BITMAPINFOHEADER);
	bi->bmiHeader.biWidth	= realBmpWidth;
	bi->bmiHeader.biHeight	= -pNuroBmp->bmpHeight;//If the height is negative, the bitmap is a top-down DIB and its origin is the upper left corner.
	bi->bmiHeader.biPlanes	= 1;
	bi->bmiHeader.biBitCount	= pNuroBmp->bmpBitCount;
	pNuroBmp->bmpBuffLen = nColSize * realBmpWidth * pNuroBmp->bmpHeight;
	nuDWORD *pColor;
	if( bi->bmiHeader.biBitCount == 16 )
	{
		bi->bmiHeader.biCompression = NURO_BI_BITFIELDS;
		pColor = (nuDWORD*)(&bi->bmiColors[0]);
		pColor[0] = 0xF800;
		pColor[1] = 0x07E0;
		pColor[2] = 0x001F;//565
	}
	else
	{
		bi->bmiHeader.biCompression = NURO_BI_RGB;
	}
	bi->bmiHeader.biSizeImage		= 0;
	bi->bmiHeader.biClrUsed			= 0;
	bi->bmiHeader.biClrImportant	= 0;

	pNuroBmp->bytesPerLine = pNuroBmp->bmpWidth * 2;

	//pNuroBmp->bmpHasLoad = 1;

    return nuTRUE;
}

nuVOID CGdiApiTrans::nuGdiDelNuroBMP(PNURO_BMP pNuroBmp)
{
	if( pNuroBmp == NULL && !pNuroBmp->bmpHasLoad )
    {
        return;
    }
    if( pNuroBmp->pBmpBuff != NULL )
    {
		nuFree(pNuroBmp->pBmpBuff);   
        pNuroBmp->pBmpBuff	= NULL;
    }
    pNuroBmp->bmpHasLoad = 0;
}

//return is the NUROBITMAP* which is canvas had.
NURO_BMP* CGdiApiTrans::nuGdiSelectCanvasBMP(NURO_BMP* pNuroBmp)
{
	PNURO_BMP pBmp = &g_pCanvas->bitmap;
	if( pNuroBmp == NULL )
    {
        return NULL;
    }
	//g_pCanvas->bitmap = *pNuroBmp;
    return pBmp;
}
nuBOOL CGdiApiTrans::nuGdiFlush(nuINT nStartX, nuINT nStartY, nuINT nWidth, nuINT nHeight, nuINT nSourceX, nuINT nSourceY)
{
	return g_GdiApiForNew.GDI_FlushCanvas(nStartX, nStartY, nWidth, nHeight, NULL, nSourceX, nSourceY);
}

nuCOLORREF CGdiApiTrans::nuGdiSetTextColor(nuCOLORREF color)
{
	return g_GdiApiForNew.GDI_SetTextColor(NULL, color);
}
nuCOLORREF CGdiApiTrans::nuGdiSetBKColor(nuCOLORREF color)
{
	return g_GdiApiForNew.GDI_SetTextBkColor(NULL, color);		
}
nuINT CGdiApiTrans::nuGdiSetBkMode(nuINT nMode)
{
	return g_GdiApiForNew.GDI_SetTextBkMode(NULL, nMode);
}
nuINT	CGdiApiTrans::nuGdiSetROP2(nuINT nMode)
{
	return 1;
}
nuBOOL CGdiApiTrans::nuGdiSetPen(PCNUROPEN pNuroPen)
{
	return g_GdiApiForNew.GDI_SetPen(NULL, (PNUROPEN)pNuroPen, NULL);
}
nuVOID CGdiApiTrans::nuGdiDelPen()
{
	
}
nuBOOL CGdiApiTrans::nuGdiSetBrush(PCNUROBRUSH pNuroBrush)
{
	return g_GdiApiForNew.GDI_SetBrush(NULL, (PNUROBRUSH)pNuroBrush, NULL);
}
nuVOID CGdiApiTrans::nuGdiDelBrush()
{	

}
nuBOOL CGdiApiTrans::nuGdiSetFont(PNUROFONT pNuroFont)
{
	return g_GdiApiForNew.GDI_SetLogfont(NULL, pNuroFont, NULL);
}
nuVOID CGdiApiTrans::nuGdiDelFont()
{
	
}
nuBOOL CGdiApiTrans::nuGdiDrawLine(nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT wMin, nuINT wAddMax, nuINT nMode)
{
	return nuTRUE;
	/*if( &g_pCanvas->bitmap )
    {
        nuCOLORREF color = nuRGB(g_pCanvas->pen.nRed, g_pCanvas->pen.nGreen, g_pCanvas->pen.nBlue);
        g_pGdiMethodsZK->nuBMPDrawLineYT3DEx(&g_pCanvas->bitmap, x1, y1, x2, y2, wMin, wAddMax, color, nMode);
        return nuTRUE;
    }
    return nuFALSE;*/
}

nuVOID CGdiApiTrans::nuGdiFillRect(nuINT x, nuINT y, nuINT w, nuINT h)
{
	g_GdiApiForNew.GDI_FillRect(NULL, x, y, w, h);		
}
//
nuBOOL CGdiApiTrans::nuGdiPolyline(const NUROPOINT* pPt, nuINT nCount)
{
	return g_GdiApiForNew.GDI_Polyline(NULL, pPt, nCount);		
}
nuBOOL CGdiApiTrans::nuGdiPolylineEx(const NUROPOINT* pPt, nuINT nCount, nuINT nMode)
{
	return g_GdiApiForNew.GDI_Polyline(NULL, pPt, nCount);
}
nuBOOL CGdiApiTrans::nuGdiPolygon(const NUROPOINT* pPt, nuINT nCount)
{
	return nuGdiPolygonEx(pPt, nCount, NURO_BMP_TYPE_COPY);
}
nuBOOL CGdiApiTrans::nuGdiPolygonEx(const NUROPOINT* pPt, nuINT nCount, nuINT nMode)
{
	return g_GdiApiForNew.GDI_PolygonEx(NULL, pPt, nCount, nMode);
}
//
nuBOOL CGdiApiTrans::nuGdiPolygonBMPEx(const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuINT nMode, nuBOOL bHasPenEdge)
{
	return nuTRUE;
}
nuBOOL CGdiApiTrans::nuGdiEllipse(nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom)
{
	return g_GdiApiForNew.GDI_Ellipse(NULL, nLeft, nTop, nRight, nBottom);
}
nuBOOL CGdiApiTrans::nuGdiEllipseEx(nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom, nuINT nMode)
{
	return g_GdiApiForNew.GDI_Ellipse(NULL, nLeft, nTop, nRight, nBottom);
}
nuBOOL CGdiApiTrans::nuGdiExtTextOutW(nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount)
{
	return nuGdiExtTextOutWEx(X, Y, wStr, nCount, nuFALSE, 0);	
}
nuBOOL CGdiApiTrans::nuGdiExtTextOutWEx(nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor)
{
	g_pCanvas->textUseEdgeColor = bUseEdgeColor;
	g_pCanvas->textEdgeColor = edgeColor;
	
	return g_GdiApiForNew.GDI_TextOut(NULL, X, Y, wStr, nCount,0);			

}
nuBOOL CGdiApiTrans::nuGdiExtTextOutWRotaryEx( nuINT X,
								 nuINT Y, 
							 	 nuFLOAT startscalex,
								 nuFLOAT startscaley,
								 nuLONG RotaryAngle,
							 	 nuWCHAR *wStr,
								 nuUINT nCount,
								 nuBOOL bUseEdgeColor,
                                 nuCOLORREF edgeColor )
{
	return g_GdiApiForNew.GDI_TextOut(NULL, X, Y, wStr, nCount,0);
}

nuINT CGdiApiTrans::nuGdiDrawTextW(nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat)
{
	return nuGdiDrawTextWEx(wStr, nCount, pRect, nFormat, nuFALSE, 0);
}
nuINT CGdiApiTrans::nuGdiDrawTextWEx( nuWCHAR* wStr, 
					    nuINT nCount, 
					    PNURORECT pRect, 
					    nuUINT nFormat, 
					    nuBOOL bUseEdgeColor, 
					    nuCOLORREF edgeColor )
{
	g_pCanvas->textUseEdgeColor = bUseEdgeColor;
	g_pCanvas->textEdgeColor    = edgeColor;
	return g_GdiApiForNew.GDI_DrawText(NULL, wStr, nCount, pRect, nFormat,0);
}
nuBOOL CGdiApiTrans::nuGdiDashLine(const NUROPOINT* pPt, nuINT nCount, nuINT nDashLen)
{
	return g_GdiApiForNew.GDI_PolyDashLine(NULL, pPt, nCount, nDashLen);
}
nuBOOL CGdiApiTrans::nuGdi3DPolyline(const NUROPOINT* pPt, nuINT nCount)
{
	return g_GdiApiForNew.GDI_Polyline3D(NULL, pPt, nCount);
}
nuBOOL CGdiApiTrans::nuGdiDrawBMP( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
	const NURO_BMP* pNuroBmp, nuINT nSrcX, nuINT nSrcY )
{
	g_GdiApiForNew.GDI_SetDrawMode(NULL, NURO_BMP_TYPE_USE_TRANSCOLOR);
	return g_GdiApiForNew.GDI_DrawBmp(NULL, nDesX, nDesY, nWidth, nHeight, (NURO_BMP*)pNuroBmp, nSrcX, nSrcY);
}

nuBOOL CGdiApiTrans::nuGdiDrawPNG( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
	nuINT PngID, nuINT nSrcX, nuINT nSrcY )
{
	g_GdiApiForNew.GDI_SetDrawMode(NULL, NURO_BMP_TYPE_USE_TRANSCOLOR);
	return g_GdiApiForNew.GDI_DrawPng(NULL, nDesX, nDesY, nWidth, nHeight, PngID, nDesX, nDesY);
}
nuBOOL CGdiApiTrans::nuGdiDrawPNG2( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
	PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY )
{
	g_GdiApiForNew.GDI_SetDrawMode(NULL, NURO_BMP_TYPE_USE_TRANSCOLOR);
	return g_GdiApiForNew.GDI_DrawPng2(NULL, nDesX, nDesY, nWidth, nHeight, p_png, nDesX, nDesY);
}

nuBOOL CGdiApiTrans::nuGdiDrawBMPTagColor( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
	const NURO_BMP* pNuroBmp, nuINT nSrcX, nuINT nSrcY, nuCOLORREF color, nuINT nMode )
{
	return nuTRUE;
}
nuBOOL CGdiApiTrans::nuGdiSaveBMP( PNURO_BMP pNuroBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, 
	nuINT nHeight, nuINT nSrcX, nuINT nSrcY )
{
	return g_GdiApiForNew.GDI_SaveBmp(pNuroBmp, nDesX, nDesY, nWidth, nHeight, NULL, nSrcX, nSrcY);
}

nuBOOL CGdiApiTrans::nuGdiPaintBMP(nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const PNURO_BMP nuBitmap)
{
	if( nWidth == 0 ) 
	{
		nWidth = g_pCanvas->bitmap.bmpWidth;
	}
	if( nHeight == 0 )
	{
		nHeight = g_pCanvas->bitmap.bmpHeight;
	}
	return g_GdiApiForNew.GDI_PaintBmp(NULL, nDesX, nDesY, nWidth, nHeight, nuBitmap, 0, 0);
}

//************************************Draw Icon************************************/
//#include "DrawIcon.h"
static NURO_BMP  nuIconBMP   = {0};
static nuBYTE    iconBuff[32*1024] = {0};

nuBOOL CGdiApiTrans::nuGdiDrawIcon(nuINT x, nuINT y, nuLONG Icon)
{
#ifdef NURO_OS_WINDOWS
#ifdef NURO_USE_DDB_BITMAP
 
	HICON IconImage = {0};
    IconImage = (HICON)LoadImage( g_hInstImage, MAKEINTRESOURCE(Icon), IMAGE_ICON, 0, 0, 0 );
	if( IconImage == NULL )
	{
		return nuFALSE;
	}
	DrawIconEx( NULL, x, y, IconImage, 0, 0, 0, NULL, DI_NORMAL );
    DestroyIcon(IconImage);
    return nuTRUE;
#endif
#endif

#ifdef NURO_USE_DIB_BITMAP
    if( g_pIconInfo == NULL )
    {
        return nuFALSE;
    }
    NUROICONFILEINFO* pIconFileInfo = (NUROICONFILEINFO*)g_pIconInfo;
    NUROICONINFO* pIconInfo = (NUROICONINFO*)(g_pIconInfo+sizeof(NUROICONFILEINFO));
    nuUINT i = 0;
    for( i = 0; i < pIconFileInfo->nIconNum; ++i )
    {
        if( pIconInfo->ID == Icon )
        {
            nuMemset(&nuIconBMP, 0, sizeof(NURO_BMP));
			nuIconBMP.bmpBitCount = 16;
            nuIconBMP.bmpHasLoad = 1;
            nuIconBMP.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
            nuIconBMP.bmpWidth = pIconInfo->nW;
            nuIconBMP.bmpHeight = pIconInfo->nH;
            nuIconBMP.bmpBuffLen= pIconInfo->nW * pIconInfo->nH * 2;

			//*****2011.01.12*****//
			nuIconBMP.bmpTransfColor = NURO_BMP_TRANSCOLOR_565; 
			nuIconBMP.bytesPerLine   = nuIconBMP.bmpWidth * 2;
			//********************//

            nuFseek(g_fpIcon, pIconInfo->nStartPos, NURO_SEEK_SET);
            if( nuFread(iconBuff, nuIconBMP.bmpBuffLen, 1, g_fpIcon) == 1 )
            {
                nuIconBMP.pBmpBuff = iconBuff;
                nuIconBMP.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
#ifdef VALUE_EMGRT
                if( Icon == 10252 || Icon == 10253 || Icon == 10255 )
                {
                    nuIconBMP.bmpType = NURO_BMP_TYPE_MIXA;
                }
#endif
                nuGdiDrawBMP(x, y, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight, &nuIconBMP, 0, 0);
            }
            break;
        }
        ++pIconInfo;
    }
    if( i == pIconFileInfo->nIconNum )
    {
        return nuFALSE;
    }
    return nuTRUE;
#endif
    return nuFALSE;
}

nuBOOL CGdiApiTrans::nuGdiDrawTargetFlagIcon(nuINT x, nuINT y, nuLONG Icon)
{
#ifdef NURO_OS_WINDOWS
#ifdef NURO_USE_DDB_BITMAP
 
	HICON IconImage = {0};
    IconImage = (HICON)LoadImage( g_hInstImage, MAKEINTRESOURCE(Icon), IMAGE_ICON, 0, 0, 0 );
	if( IconImage == NULL )
	{
		return nuFALSE;
	}
	DrawIconEx( NULL, x, y, IconImage, 0, 0, 0, NULL, DI_NORMAL );
    DestroyIcon(IconImage);
    return nuTRUE;
#endif
#endif

#ifdef NURO_USE_DIB_BITMAP
    if( g_pIconInfo == NULL )
    {
        return nuFALSE;
    }
    NUROICONFILEINFO* pIconFileInfo = (NUROICONFILEINFO*)g_pIconInfo;
    NUROICONINFO* pIconInfo = (NUROICONINFO*)(g_pIconInfo+sizeof(NUROICONFILEINFO));
    nuUINT i = 0;
    for( i = 0; i < pIconFileInfo->nIconNum; ++i )
    {
        if( pIconInfo->ID == Icon )
        {
            nuMemset(&nuIconBMP, 0, sizeof(NURO_BMP));
			nuIconBMP.bmpBitCount = 16;
            nuIconBMP.bmpHasLoad = 1;
            nuIconBMP.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
            nuIconBMP.bmpWidth = pIconInfo->nW;
            nuIconBMP.bmpHeight = pIconInfo->nH;
            nuIconBMP.bmpBuffLen= pIconInfo->nW * pIconInfo->nH * 2;

			//*****2011.01.12*****//
			nuIconBMP.bmpTransfColor = NURO_BMP_TRANSCOLOR_565; 
			nuIconBMP.bytesPerLine   = nuIconBMP.bmpWidth * 2;
			//********************//

            nuFseek(g_fpIcon, pIconInfo->nStartPos, NURO_SEEK_SET);
            if( nuFread(iconBuff, nuIconBMP.bmpBuffLen, 1, g_fpIcon) == 1 )
            {
                nuIconBMP.pBmpBuff = iconBuff;
                nuIconBMP.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
#ifdef VALUE_EMGRT
                if( Icon == 10252 || Icon == 10253 || Icon == 10255 )
                {
                    nuIconBMP.bmpType = NURO_BMP_TYPE_MIXA;
                }
#endif
                nuGdiDrawBMP(x-(nuIconBMP.bmpWidth/2), y-nuIconBMP.bmpHeight, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight, &nuIconBMP, 0, 0);
            }
            break;
        }
        ++pIconInfo;
    }
    if( i == pIconFileInfo->nIconNum )
    {
        return nuFALSE;
    }
    return nuTRUE;
#endif
    return nuFALSE;
}


nuBOOL CGdiApiTrans::nuGdiDrawIconEx(nuINT x, nuINT y, nuLONG Icon, nuBYTE nbmpType, nuBYTE nPos)
{
#ifdef NURO_USE_DIB_BITMAP
    if( NULL == g_pIconInfo )
    {
        return nuFALSE;
    }
    NUROICONFILEINFO* pIconFileInfo = (NUROICONFILEINFO*)g_pIconInfo;
    NUROICONINFO* pIconInfo = (NUROICONINFO*)( g_pIconInfo + sizeof(NUROICONFILEINFO) );
    nuUINT i = 0;
    for( i = 0; i < pIconFileInfo->nIconNum; ++i )
    {
        if( pIconInfo->ID == Icon )
        {
            nuMemset(&nuIconBMP, 0, sizeof(NURO_BMP));
            nuIconBMP.bmpBitCount = 16;
            nuIconBMP.bmpHasLoad = 1;
            nuIconBMP.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
			nuIconBMP.bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
            nuIconBMP.bmpWidth = pIconInfo->nW;
            nuIconBMP.bmpHeight = pIconInfo->nH;
            nuIconBMP.bmpBuffLen = pIconInfo->nW * pIconInfo->nH * 2;
            nuFseek(g_fpIcon, pIconInfo->nStartPos, NURO_SEEK_SET);
            if( nuFread(iconBuff, nuIconBMP.bmpBuffLen, 1, g_fpIcon) == 1 )
            {
                nuIconBMP.pBmpBuff = iconBuff;
                nuIconBMP.bmpType = nbmpType;
                switch ( nPos )
                {
                case ICONALIGN_LEFTTOP:
                    break;
                case ICONALIGN_LEFTCENTER:
                    y = y - nuIconBMP.bmpHeight / 2;
                    break;
                case ICONALIGN_LEFTBOTTOM:
                    y = y - nuIconBMP.bmpHeight;
                    break;
                case ICONALIGN_RIGHTTOP:
                    x = x - nuIconBMP.bmpWidth;
                    break;
                case ICONALIGN_RIGHTCENTER:
                    x = x - nuIconBMP.bmpWidth;
                    y = y - nuIconBMP.bmpHeight / 2;
                    break;
                case ICONALIGN_RIGHTBOTTOM:
                    x = x - nuIconBMP.bmpWidth;
                    y = y - nuIconBMP.bmpHeight;
                    break;
                case ICONALIGN_CENTERCENTER:
                    x = x - nuIconBMP.bmpWidth / 2;
                    y = y - nuIconBMP.bmpHeight / 2;
                    break;
                case ICONALIGN_CENTERTOP:
                    x = x - nuIconBMP.bmpWidth / 2;
                    break;
                case ICONALIGN_CENTERBOTTOM:
                    x = x - nuIconBMP.bmpWidth / 2;
                    y = y - nuIconBMP.bmpHeight;
                }
				//*****2011.01.11*****//
				nuIconBMP.bytesPerLine = nuIconBMP.bmpWidth * 2;

                nuGdiDrawBMP(x, y, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight, &nuIconBMP, 0, 0);
            }
            break;
        }
        ++pIconInfo;
    }
    if( i == pIconFileInfo->nIconNum )
    {
        return nuFALSE;
    }
    return nuTRUE;   
#endif
    return nuFALSE;
}
nuBOOL CGdiApiTrans::nuGdiDrawIconSp(nuINT x, nuINT y, nuLONG Icon, nuBYTE dayOrNight)
{
	return nuTRUE;
}
nuBOOL CGdiApiTrans::nuGdiDrawIconExtern(nuINT x, nuINT y, nuLONG Icon ,nuLONG nRotationAngle)
{
	return nuTRUE;
}

//**********************************Draw POI**********************************//
nuUINT CGdiApiTrans::nuGdiDrawPoi(nuINT x, nuINT y, nuLONG Poi, nuUINT nRTPOSStyle, nuBOOL bSmallIcon /* = nuFALSE */)

{
	nuINT realX = x;
    nuINT realY = y;
    if( Poi < 0 || g_PoiBMP16.bmpHasLoad != 1 )
    {
        return 0;
    }
	nuINT nPoiSizeH = 0;
	nuINT nPoiSizeW = 0;
	if( bSmallIcon && g_smallPoiBmp.bmpHasLoad )
	{
		nPoiSizeH = g_smallPoiBmp.bmpHeight;
		nPoiSizeW = g_smallPoiBmp.bmpHeight;
		if( Poi * nPoiSizeW  > g_smallPoiBmp.bmpWidth )
		{
			return 0;
		}
	}
	else
	{
		nPoiSizeH = g_PoiBMP16.bmpHeight;
		nPoiSizeW = g_PoiBMP16.bmpHeight;
		if( Poi * nPoiSizeW  > g_PoiBMP16.bmpWidth )
		{
			return 0;
		}
	}
	//
    switch( nRTPOSStyle )
    {
    case NURO_RTPOS_LT: break;
    case NURO_RTPOS_LC: realY -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_LB: realY -= nPoiSizeH; break;
        
    case NURO_RTPOS_CT: realX -= (nPoiSizeW>>1); break;
    case NURO_RTPOS_CC: realX -= (nPoiSizeW>>1); realY -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_CB: realX -= (nPoiSizeW>>1); realY -= nPoiSizeH; break;
        
    case NURO_RTPOS_RT: realX -= nPoiSizeW; break;
    case NURO_RTPOS_RC: realX -= nPoiSizeW; realY -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_RB: realX -= nPoiSizeW; realY -= nPoiSizeH; break;
    default: break;
    }
   
	//png?B?z
	PNGCTRLSTRU *ppng=NULL;
	m_pngctrl.Get_PngData_id(Poi,ppng);

	nuBOOL bRes = nuFALSE;
    switch( nPoiSizeH )
    {
    case NURO_POI_SIZE_16:
		if(ppng==NULL)
		{
			if( bSmallIcon && g_smallPoiBmp.bmpHasLoad )
			{
				bRes = nuGdiDrawBMP(realX, realY, nPoiSizeW, nPoiSizeH, &g_smallPoiBmp, Poi*nPoiSizeW, 0);
			}
			else
			{
				bRes = nuGdiDrawBMP(realX, realY, nPoiSizeW, nPoiSizeH, &g_PoiBMP16, Poi*nPoiSizeW, 0);
			}
		}
		else
		{
			bRes = nuGdiDrawPNG2(realX, realY, nPoiSizeW, nPoiSizeH, ppng, Poi*nPoiSizeW, 0);
		}
        break;
    default:
		if(ppng==NULL)
		{
			if( bSmallIcon && g_smallPoiBmp.bmpHasLoad )
			{
				bRes = nuGdiDrawBMP(realX, realY, nPoiSizeW, nPoiSizeH, &g_smallPoiBmp, Poi*nPoiSizeW, 0);
			}
			else
			{
				bRes = nuGdiDrawBMP(realX, realY, nPoiSizeW, nPoiSizeH, &g_PoiBMP16, Poi*nPoiSizeW, 0);
			}			
		}
		else
		{
			bRes = nuGdiDrawPNG2(realX, realY, nPoiSizeW, nPoiSizeH, ppng, Poi*nPoiSizeW, 0);
		}
        break;
    }
    
    nPoiSizeW = nPoiSizeW << 16;
    if( bRes )
    {
        return nPoiSizeH + nPoiSizeW;
    }
    else
    {
        return 0;
    }
}

nuVOID CGdiApiTrans::nuGdiSet3DPara(const PFUNC_FOR_GDI pForGDI)
{
	g_GdiApiForNew.GDI_Set3DPara(pForGDI->lpFcGet3DThick); 
}
nuBOOL CGdiApiTrans::nuGdiBmpGradient( nuINT nDesX, 
					                   nuINT nDesY, 
					                   nuINT nWidth, 
					                   nuINT nHeight, 
					                   const NURO_BMP* pBmp, 
					                   nuINT nSrcX, 
					                   nuINT nSrcY,
					                   nuINT nMode )
{
	return g_GdiApiForNew.GDI_BmpGradient(NULL, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY, nMode);
}
nuBOOL CGdiApiTrans::nuGdiZoomBmp(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
			                      , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nWidthSrc, nuINT nHeightSrc)
{
	return g_GdiApiForNew.GDI_ZoomBmp(pDesBmp, nDesX, nDesY, nWidth, nHeight, pSrcBmp, nSrcX, nSrcY, nWidthSrc, nHeightSrc);
}

