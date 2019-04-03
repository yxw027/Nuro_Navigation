
#include "libGDI.h"
#include "CanvasMgrZK.h"
#include "GdiMethodZK.h"
static CCanvasMgrZK*		g_pCvsMgrZK = NULL;
static CGdiMethodZK*		g_pGdiMethodsZK = NULL;

nuBOOL		b_UseingGDI=nuFALSE;
nuBOOL		b_ReBool=nuFALSE;
nuINT		b_ReINT=0;
nuCOLORREF	b_ReCOLORREF;

GDI_API nuPVOID LibGDI_InitModule(nuPVOID lpVoidIn)
{
	b_UseingGDI=nuFALSE;
	g_pCvsMgrZK = new CCanvasMgrZK();
	if( !g_pCvsMgrZK )
	{
		return 0;
	}
    CGdiBaseZK::s_apiGdi.GDI_Initialize		= LibGDI_Initialize;
    CGdiBaseZK::s_apiGdi.GDI_ReInitialize   = LibGDI_ReInitialize;
    CGdiBaseZK::s_apiGdi.GDI_Free		= LibGDI_Free;
    CGdiBaseZK::s_apiGdi.GDI_GetCanvas = LibGDI_GetCanvas;
    CGdiBaseZK::s_apiGdi.GDI_SetLeftTop = LibGDI_SetLeftTop;
    CGdiBaseZK::s_apiGdi.GDI_LoadFontInfo = LibGDI_LoadFontInfo;
    CGdiBaseZK::s_apiGdi.GDI_InitCanvas = LibGDI_InitCanvas;
    CGdiBaseZK::s_apiGdi.GDI_FlushCanvas = LibGDI_FlushCanvas;
    CGdiBaseZK::s_apiGdi.GDI_CreateCanvas = LibGDI_CreateCanvas;
    CGdiBaseZK::s_apiGdi.GDI_DeleteCanvas = LibGDI_DeleteCanvas;
    CGdiBaseZK::s_apiGdi.GDI_SetDrawMode = LibGDI_SetDrawMode;
    CGdiBaseZK::s_apiGdi.GDI_SetTextBkMode = LibGDI_SetTextBkMode;
    CGdiBaseZK::s_apiGdi.GDI_SetTextBkColor = LibGDI_SetTextBkColor;
    CGdiBaseZK::s_apiGdi.GDI_SetTextColor = LibGDI_SetTextColor;
    CGdiBaseZK::s_apiGdi.GDI_SetTextEdgeColor = LibGDI_SetTextEdgeColor;
    CGdiBaseZK::s_apiGdi.GDI_SetTextUseEdgeColor = LibGDI_SetTextUseEdgeColor;
    CGdiBaseZK::s_apiGdi.GDI_SetPen = LibGDI_SetPen;
    CGdiBaseZK::s_apiGdi.GDI_SetBrush = LibGDI_SetBrush;
    CGdiBaseZK::s_apiGdi.GDI_SetLogfont = LibGDI_SetLogfont;
    CGdiBaseZK::s_apiGdi.GDI_FillRect = LibGDI_FillRect;
    CGdiBaseZK::s_apiGdi.GDI_Polyline = LibGDI_Polyline;
    CGdiBaseZK::s_apiGdi.GDI_PolygonEx = LibGDI_PolygonEx;
    CGdiBaseZK::s_apiGdi.GDI_Ellipse = LibGDI_Ellipse;
    CGdiBaseZK::s_apiGdi.GDI_TextOut = LibGDI_TextOut;
    CGdiBaseZK::s_apiGdi.GDI_DrawText = LibGDI_DrawText;
	CGdiBaseZK::s_apiGdi.GDI_DrawTextNew = LibGDI_DrawTextNew;
    CGdiBaseZK::s_apiGdi.GDI_PolyDashLine = LibGDI_PolyDashLine;
    CGdiBaseZK::s_apiGdi.GDI_Polyline3D = LibGDI_Polyline3D;
    CGdiBaseZK::s_apiGdi.GDI_PolygonBMP = LibGDI_PolygonBMP;
    CGdiBaseZK::s_apiGdi.GDI_DrawBmp = LibGDI_DrawBmp;
    CGdiBaseZK::s_apiGdi.GDI_DrawPng = LibGDI_DrawPng;
    CGdiBaseZK::s_apiGdi.GDI_DrawPng2 = LibGDI_DrawPng2;
    CGdiBaseZK::s_apiGdi.GDI_DrawBmp2 = LibGDI_DrawBmp2;
    CGdiBaseZK::s_apiGdi.GDI_ZoomBmp = LibGDI_ZoomBmp;
    CGdiBaseZK::s_apiGdi.GDI_SaveBmp = LibGDI_SaveBmp;

    CGdiBaseZK::s_apiGdi.GDI_PaintBmp = LibGDI_PaintBmp;

	CGdiBaseZK::s_apiGdi.GDI_Set3DPara = LibGDI_Set3DPara;
	CGdiBaseZK::s_apiGdi.GDI_BmpGradient = LibGDI_BmpGradient;
	CGdiBaseZK::s_apiGdi.GDI_LoadUIPng = LibGDI_LoadUIPng;
	CGdiBaseZK::s_apiGdi.GDI_CleanUIPng = LibGDI_CleanUIPng;
	CGdiBaseZK::s_apiGdi.GDI_ShowStartUpBmp = LibGDI_ShowStartUpbmp;
	return &CGdiBaseZK::s_apiGdi;
}
GDI_API nuVOID LibGDI_FreeModule()
{
	if( g_pCvsMgrZK )
	{
		delete g_pCvsMgrZK;
		g_pCvsMgrZK = 0;
	}
}
GDI_API nuBOOL	LibGDI_Initialize(PNURO_SCREEN pScreen)
{

    return g_pCvsMgrZK->Initialize(pScreen);

}

GDI_API nuBOOL	LibGDI_ReInitialize(nuWORD nWidth, nuWORD nHeight)
{

	return g_pCvsMgrZK->ReInitialize(nWidth, nHeight);
}

GDI_API nuVOID	LibGDI_Free()
{
    g_pCvsMgrZK->Free();
}
GDI_API PNURO_CANVAS  LibGDI_GetCanvas(nuINT nCvsIdx)
{

	return g_pCvsMgrZK->GetCanvas(nCvsIdx);
}
GDI_API nuBOOL	LibGDI_SetLeftTop(PNURO_CANVAS pCvs, nuINT *pLeft, nuINT* pTop)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->SetCvsLeftTop(pCvs, pLeft, pTop);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;

//    return g_pCvsMgrZK->SetCvsLeftTop(pCvs, pLeft, pTop);
}
GDI_API nuBOOL  LibGDI_LoadFontInfo(nuTCHAR* fontfile)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->LoadFontInfo(fontfile);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//    return g_pCvsMgrZK->LoadFontInfo(fontfile);
}

GDI_API nuVOID  LibGDI_InitCanvas(PNURO_CANVAS pCvs)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	g_pCvsMgrZK->CvsInit(pCvs);
	b_UseingGDI	=	nuFALSE;
//    g_pCvsMgrZK->CvsInit(pCvs);

// #ifdef NURO_DEBUG
// 	nuTCHAR tPath[256];
// 	nuGetModulePath(NULL, tPath, sizeof (tPath));
// 	nuTcscat(tPath, nuTEXT("log.txt"));
// 	FILE *fp = fopen(tPath, "a+");
// 	fprintf(fp, "\n LibGDI_InitCanvas \n");
// 	fclose(fp);
// #endif 


}
GDI_API nuBOOL LibGDI_CreateCanvas(PNURO_CANVAS pCvs, nuWORD nWidth, nuWORD nHeight)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsCreate(pCvs, nWidth, nHeight);
	b_UseingGDI	=	nuFALSE;

// #ifdef NURO_DEBUG
// 	nuTCHAR tPath[256];
// 	nuGetModulePath(NULL, tPath, sizeof (tPath));
// 	nuTcscat(tPath, nuTEXT("log.txt"));
// 	FILE *fp = fopen(tPath, "a+");
// 	fprintf(fp, "\n LibGDI_CreateCanvas\n");
// 	fclose(fp);
// #endif


	return b_ReBool;
//	return g_pCvsMgrZK->CvsCreate(pCvs, nWidth, nHeight);
}
GDI_API nuVOID LibGDI_DeleteCanvas(PNURO_CANVAS pCvs)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	g_pCvsMgrZK->CvsDestroy(pCvs);
	b_UseingGDI	=	nuFALSE;
//	g_pCvsMgrZK->CvsDestroy(pCvs);
}
GDI_API nuBOOL LibGDI_FlushCanvas( nuINT nScreenX,  nuINT nScreenY, nuINT nWidth, nuINT nHeight,
                                  PNURO_CANVAS pSrcCvs, nuINT nSrcX, nuINT nSrcY )
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsFlush( nScreenX, nScreenY, nWidth, nHeight, pSrcCvs, nSrcX, nSrcY );
	b_UseingGDI	=	nuFALSE;

// #ifdef NURO_DEBUG
// 	nuTCHAR tPath[256];
// 	nuGetModulePath(NULL, tPath, sizeof (tPath));
// 	nuTcscat(tPath, nuTEXT("log.txt"));
// 	FILE *fp = fopen(tPath, "a+");
// 	fprintf(fp, "\n LibGDI_FlushCanvas \n");
// 	fclose(fp);
// #endif 


	return b_ReBool;
//    return g_pCvsMgrZK->CvsFlush( nScreenX, nScreenY, nWidth, nHeight, pSrcCvs, nSrcX, nSrcY );
}
GDI_API nuINT   LibGDI_SetDrawMode(NURO_CANVAS* canvas, nuINT mode)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT	=	g_pCvsMgrZK->CvsSetDrawMode(canvas, mode);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//    return g_pCvsMgrZK->CvsSetDrawMode(canvas, mode);
}
GDI_API nuINT	LibGDI_SetTextBkMode(PNURO_CANVAS pCvs, nuINT nMode)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT	=	 g_pCvsMgrZK->CvsSetTextBkMode(pCvs, nMode);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//    return g_pCvsMgrZK->CvsSetTextBkMode(pCvs, nMode);
}
GDI_API nuCOLORREF LibGDI_SetTextBkColor(PNURO_CANVAS pCvs, nuCOLORREF nBkColor)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI		=	nuTRUE;
	b_ReCOLORREF	=	 g_pCvsMgrZK->CvsSetTextBkColor(pCvs, nBkColor);
	b_UseingGDI		=	nuFALSE;
	return b_ReCOLORREF;
//	return g_pCvsMgrZK->CvsSetTextBkColor(pCvs, nBkColor);
}
GDI_API nuCOLORREF LibGDI_SetTextColor(PNURO_CANVAS pCvs, nuCOLORREF nTextColor)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI		=	nuTRUE;
	b_ReCOLORREF	=	 g_pCvsMgrZK->CvsSetTextColor(pCvs, nTextColor);
	b_UseingGDI		=	nuFALSE;
	return b_ReCOLORREF;
//	return g_pCvsMgrZK->CvsSetTextColor(pCvs, nTextColor);
}
GDI_API nuCOLORREF  LibGDI_SetTextEdgeColor(NURO_CANVAS* canvas, nuCOLORREF color)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI		=	nuTRUE;
	b_ReCOLORREF	=	 g_pCvsMgrZK->CvsSetTextEdgeColor(canvas, color);
	b_UseingGDI		=	nuFALSE;
	return b_ReCOLORREF;
//    return g_pCvsMgrZK->CvsSetTextEdgeColor(canvas, color);
}
GDI_API nuBOOL LibGDI_SetTextUseEdgeColor(NURO_CANVAS* canvas, nuBOOL use)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsSetTextUseEdgeColor(canvas, use);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//    return g_pCvsMgrZK->CvsSetTextUseEdgeColor(canvas, use);
}
GDI_API nuBOOL LibGDI_SetPen(PNURO_CANVAS pCvs, PNUROPEN pPen, PNUROPEN oldPen)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsSetPen(pCvs, *pPen, oldPen);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsSetPen(pCvs, *pPen, oldPen);
}
GDI_API nuBOOL	LibGDI_SetBrush(PNURO_CANVAS pCvs, PNUROBRUSH pBrush, PNUROBRUSH oldBrush)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsSetBrush(pCvs, *pBrush, oldBrush);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsSetBrush(pCvs, *pBrush, oldBrush);
}
GDI_API nuBOOL	LibGDI_SetLogfont(PNURO_CANVAS pCvs, PNUROFONT pLogfont, PNUROFONT oldFont)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsSetFont(pCvs, *pLogfont, oldFont);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsSetFont(pCvs, *pLogfont, oldFont);
}

GDI_API nuVOID  LibGDI_Set3DPara(nuPVOID Get3DThickFuc)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	g_pCvsMgrZK->CvsSet3DParam((Get3DThickProc)Get3DThickFuc);
	b_UseingGDI	=	nuFALSE;
//	g_pCvsMgrZK->CvsSet3DParam((Get3DThickProc)Get3DThickFuc);
}

GDI_API nuINT	LibGDI_FillRect( PNURO_CANVAS pCvs, 
								 nuINT x, 
								 nuINT y, 
								 nuINT w, 
								 nuINT h)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsFillRect(pCvs, x, y, w, h);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//	return g_pCvsMgrZK->CvsFillRect(pCvs, x, y, w, h);
}
GDI_API nuINT	LibGDI_Polyline(PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	 g_pCvsMgrZK->CvsPolyline(pCvs, pPoint, nCount);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//	return g_pCvsMgrZK->CvsPolyline(pCvs, pPoint, nCount);
}
GDI_API nuINT	LibGDI_PolygonEx(PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount, nuINT nMode)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsPolygonEx(pCvs, pPoint, nCount, nMode);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//	return g_pCvsMgrZK->CvsPolygon(pCvs, pPoint, nCount);
}
GDI_API nuINT	LibGDI_Ellipse(PNURO_CANVAS pCvs, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsEllipse(pCvs, nLeft, nTop, nRight, nBottom);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//	return g_pCvsMgrZK->CvsEllipse(pCvs, nLeft, nTop, nRight, nBottom);
}
GDI_API nuINT	LibGDI_TextOut( PNURO_CANVAS pCvs, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount,nuINT Parent)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsTextOut(pCvs, X, Y, wStr, nCount,Parent);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
//	return g_pCvsMgrZK->CvsTextOut(pCvs, X, Y, wStr, nCount);
}
GDI_API nuINT	LibGDI_DrawText(PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,nuINT Parent)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsDrawText(pCvs, wStr, nCount, pRect, nFormat,Parent);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
}

GDI_API nuINT LibGDI_DrawTextNew( PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,nuINT Parent)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReINT		=	g_pCvsMgrZK->CvsDrawTextNew(pCvs, wStr, nCount, pRect, nFormat,Parent);
	b_UseingGDI	=	nuFALSE;
	return b_ReINT;
}

GDI_API nuBOOL	LibGDI_PolyDashLine(NURO_CANVAS* pCvs, const NUROPOINT* pt, nuINT ptCount, nuINT nDashLen)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsPolyDashLine(pCvs, pt, ptCount, nDashLen);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//    return g_pCvsMgrZK->CvsPolyDashLine(pCvs, pt, ptCount, nDashLen);
}
GDI_API nuBOOL  LibGDI_Polyline3D(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsPolyline3D(pCvs, pPt, nCount);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//    return g_pCvsMgrZK->CvsPolyline3D(pCvs, pPt, nCount);
}
GDI_API nuBOOL	LibGDI_PolygonBMP(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuBOOL bHasPenEdge)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsPolygonBMP(pCvs, pPt, nCount, pBmp, bHasPenEdge);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//    return g_pCvsMgrZK->CvsPolygonBMP(pCvs, pPt, nCount, pBmp, bHasPenEdge);
}
GDI_API nuBOOL	LibGDI_DrawBmp(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsDrawBMP(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsDrawBMP(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY);
}

GDI_API nuBOOL	LibGDI_DrawPng(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , nuINT PngID, nuINT nSrcX, nuINT nSrcY)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	true;
	b_ReBool	=	g_pCvsMgrZK->CvsDrawPNG(pCvs, nDesX, nDesY, nWidth, nHeight, PngID, nSrcX, nSrcY);
	b_UseingGDI	=	false;
	return b_ReBool;
}

GDI_API nuBOOL	LibGDI_DrawPng2(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	true;
	b_ReBool	=	g_pCvsMgrZK->CvsDrawPNG2(pCvs, nDesX, nDesY, nWidth, nHeight, p_png, nSrcX, nSrcY);
	b_UseingGDI	=	false;
	return b_ReBool;
}

GDI_API nuBOOL	LibGDI_DrawBmp2(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsDrawBmp2(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY, nSrcW, nSrcH);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsDrawBmp2(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY, nSrcW, nSrcH);
}
GDI_API nuBOOL	LibGDI_ZoomBmp(PNURO_BMP pDesBmp,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
							   , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsZoomBmp(pDesBmp, nDesX, nDesY, nWidth, nHeight, pSrcBmp, nSrcX, nSrcY, nSrcW, nSrcH);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
	//	return g_pCvsMgrZK->CvsDrawBmp2(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY, nSrcW, nSrcH);
}
GDI_API nuBOOL	LibGDI_SaveBmp(PNURO_BMP pBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const NURO_CANVAS* pCvs, nuINT nSrcX, nuINT nSrcY)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CvsSaveBMP(pBmp, nDesX, nDesY, nWidth, nHeight, pCvs, nSrcX, nSrcY);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsSaveBMP(pBmp, nDesX, nDesY, nWidth, nHeight, pCvs, nSrcX, nSrcY);
}

GDI_API nuBOOL	LibGDI_PaintBmp(PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	 g_pCvsMgrZK->CvsPaintBMP(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
//	return g_pCvsMgrZK->CvsPaintBMP(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY);
}
GDI_API nuBOOL  LibGDI_BmpGradient(PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
								   const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	 g_pCvsMgrZK->CvsBmpGradient(pCvs, nDesX, nDesY, nWidth, nHeight, pBmp, nSrcX, nSrcY, nMode);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
}
GDI_API nuBOOL  LibGDI_LoadUIPng(PPNGCTRLSTRU &p_png,nuINT &PngIndex,nuTCHAR* pngfile,nuINT pngId)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->LoadUIPng(p_png,PngIndex,pngfile,pngId);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
}
GDI_API nuBOOL  LibGDI_CleanUIPng(nuINT PngIndex,nuBOOL bCleanAll)
{
	while(b_UseingGDI)
	{
		nuSleep(5);
	}
	b_UseingGDI	=	nuTRUE;
	b_ReBool	=	g_pCvsMgrZK->CleanUIPng(PngIndex,bCleanAll);
	b_UseingGDI	=	nuFALSE;
	return b_ReBool;
}
GDI_API nuBOOL LibGDI_ShowStartUpbmp()
{
	return g_pCvsMgrZK->CvsShowStartupBmp();
}
