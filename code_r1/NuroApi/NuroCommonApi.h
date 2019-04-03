#ifndef _NURO_COMMON_API_20100415
#define _NURO_COMMON_API_20100415

#include "NuroDefine.h"
//#include "NuroEngineStructs.h"

typedef nuBOOL (*GDI_InitializeProc)(PNURO_SCREEN pScreen);
typedef nuBOOL (*GDI_ReInitializeProc)(nuWORD nWidth, nuWORD nHeight);
typedef nuVOID (*GDI_FreeProc)();

typedef PNURO_CANVAS (*GDI_GetCanvasProc)(nuINT nCvsIdx);
typedef nuBOOL (*GDI_SetLeftTopProc)(PNURO_CANVAS pCvs, nuINT *pLeft, nuINT* pTop);

typedef nuBOOL  (*GDI_LoadFontInfoProc)(nuTCHAR* fontfile);

typedef nuVOID  (*GDI_InitCanvasProc)(PNURO_CANVAS pCvs);
typedef nuBOOL	(*GDI_FlushCanvasProc)( nuINT nScreenX, nuINT nScreenY, nuINT nWidth, nuINT nHeight
                                       , PNURO_CANVAS pSrcCvs, nuINT nSrcX, nuINT nSrcY );
typedef nuBOOL (*GDI_CreateCanvasProc)(PNURO_CANVAS pCvs, nuWORD nWidth, nuWORD nHeight);
typedef nuVOID (*GDI_DeleteCanvasProc)(PNURO_CANVAS pCvs);

typedef nuINT   (*GDI_SetDrawModeProc)(NURO_CANVAS* canvas, nuINT mode);
typedef nuINT (*GDI_SetTextBkModeProc)(PNURO_CANVAS pCvs, nuINT nMode);
typedef nuCOLORREF (*GDI_SetTextBkColorProc)(PNURO_CANVAS pCvs, nuCOLORREF nBkColor);
typedef nuCOLORREF (*GDI_SetTextColorProc)(PNURO_CANVAS pCvs, nuCOLORREF nTextColor);
typedef nuCOLORREF  (*GDI_SetTextEdgeColorProc)(NURO_CANVAS* canvas, nuCOLORREF color);
typedef nuBOOL      (*GDI_SetTextUseEdgeColorProc)(NURO_CANVAS* canvas, nuBOOL use);
typedef nuBOOL (*GDI_SetPenProc)(PNURO_CANVAS pCvs, PNUROPEN pPen, PNUROPEN oldPen);
typedef nuBOOL (*GDI_SetBrushProc)(PNURO_CANVAS pCvs, PNUROBRUSH pBrush, PNUROBRUSH oldBrush);
typedef nuBOOL (*GDI_SetLogfontProc)(PNURO_CANVAS pCvs, PNUROFONT pLogfont, PNUROFONT oldFont);

typedef nuINT (*GDI_FillRectProc)(PNURO_CANVAS pCvs, nuINT x, nuINT y, nuINT w, nuINT h);
typedef nuINT (*GDI_PolylineProc)(PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount);
typedef nuINT (*GDI_PolygonExProc)( PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount, int nMode);
typedef nuINT (*GDI_EllipseProc)( PNURO_CANVAS pCvs, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom);
typedef nuINT (*GDI_TextOutProc)( PNURO_CANVAS pCvs, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount,int Parent);
typedef nuINT (*GDI_DrawTextProc)( PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,int Parent);
typedef nuINT (*GDI_DrawTextNewProc)( PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,int Parent);

typedef nuBOOL	(*GDI_PolyDashLineProc)(NURO_CANVAS* pCvs, const NUROPOINT* pt, int ptCount, int nDashLen);
typedef nuBOOL  (*GDI_Polyline3DProc)(NURO_CANVAS* pCvs, const NUROPOINT* pPt, int nCount); 
typedef nuBOOL	(*GDI_PolygonBMPProc)(NURO_CANVAS* pCvs, const NUROPOINT* pPt, int nCount, PNURO_BMP pBmp, nuBOOL bHasPenEdge); 
typedef nuBOOL (*GDI_DrawBmpProc)( PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                  , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY);
typedef nuBOOL	(*GDI_DrawPngProc)(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , nuINT PngID, nuINT nSrcX, nuINT nSrcY);
typedef nuBOOL	(*GDI_DrawPng2Proc)(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY);
typedef nuBOOL (*GDI_DrawBmp2Proc)( PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                  , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nWidthSrc, nuINT nHeightSrc);
typedef nuBOOL (*GDI_ZoomBmpProc)(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
			                      , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nWidthSrc, nuINT nHeightSrc);
typedef nuBOOL (*GDI_SaveBmpProc)( PNURO_BMP pBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                  , const NURO_CANVAS* pCvs, nuINT nSrcX, nuINT nSrcY );
typedef nuBOOL (*GDI_PaintBmpProc)( PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                   , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY);

typedef nuVOID (*GDI_Set3DParaProc)(nuPVOID Get3DThickFuc);
typedef nuBOOL (*GDI_LoadUIPngProc)(PPNGCTRLSTRU &p_png,nuINT &PngIndex,nuTCHAR* pngfile,nuINT pngId);
typedef nuBOOL (*GDI_CleanUIPngProc)(nuINT PngIndex,nuBOOL bCleanAll);
typedef nuBOOL (*GDI_BmpGradientProc)(PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
								      const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode);
typedef nuBOOL (*GDI_ShowStartUpBmpProc)();

typedef struct tagNURO_API_GDI
{
    GDI_InitializeProc		GDI_Initialize;
	GDI_ReInitializeProc    GDI_ReInitialize;
    GDI_FreeProc		GDI_Free;
    GDI_GetCanvasProc GDI_GetCanvas;
    GDI_SetLeftTopProc GDI_SetLeftTop;
    GDI_LoadFontInfoProc GDI_LoadFontInfo;
    GDI_InitCanvasProc GDI_InitCanvas;
    GDI_FlushCanvasProc GDI_FlushCanvas;
    GDI_CreateCanvasProc GDI_CreateCanvas;
    GDI_DeleteCanvasProc GDI_DeleteCanvas;
    GDI_SetDrawModeProc GDI_SetDrawMode;
    GDI_SetTextBkModeProc GDI_SetTextBkMode;
    GDI_SetTextBkColorProc GDI_SetTextBkColor;
    GDI_SetTextColorProc GDI_SetTextColor;
    GDI_SetTextEdgeColorProc GDI_SetTextEdgeColor;
    GDI_SetTextUseEdgeColorProc GDI_SetTextUseEdgeColor;
    GDI_SetPenProc GDI_SetPen;
    GDI_SetBrushProc GDI_SetBrush;
    GDI_SetLogfontProc GDI_SetLogfont;
    GDI_FillRectProc GDI_FillRect;
    GDI_PolylineProc GDI_Polyline;
    GDI_PolygonExProc GDI_PolygonEx;
	GDI_PolygonExProc GDI_PolygonExX;
    GDI_EllipseProc GDI_Ellipse;
    GDI_TextOutProc GDI_TextOut;
    GDI_DrawTextProc GDI_DrawText;
	GDI_DrawTextNewProc GDI_DrawTextNew;
    GDI_PolyDashLineProc GDI_PolyDashLine;
    GDI_Polyline3DProc GDI_Polyline3D;
	GDI_Polyline3DProc GDI_Polyline3DX;
    GDI_PolygonBMPProc GDI_PolygonBMP;
    GDI_DrawBmpProc GDI_DrawBmp;
	GDI_DrawPngProc GDI_DrawPng;
    GDI_DrawPng2Proc GDI_DrawPng2;
	GDI_DrawBmp2Proc GDI_DrawBmp2;
	GDI_ZoomBmpProc  GDI_ZoomBmp;
    GDI_SaveBmpProc GDI_SaveBmp;
    GDI_PaintBmpProc GDI_PaintBmp;	
	GDI_Set3DParaProc GDI_Set3DPara;
	GDI_BmpGradientProc GDI_BmpGradient;
	GDI_LoadUIPngProc GDI_LoadUIPng;
	GDI_CleanUIPngProc GDI_CleanUIPng;
	GDI_ShowStartUpBmpProc GDI_ShowStartUpBmp;
}NURO_API_GDI, *PNURO_API_GDI;

#endif