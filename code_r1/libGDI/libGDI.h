#ifndef __NURO_GDI_20091001
#define __NURO_GDI_20091001

#include "NuroDefine.h"
#include "NuroCommonApi.h"
//#include "CanvasMgrZK.h"

#ifdef GDI_EXPORTS
#define GDI_API extern "C" __declspec(dllexport)
#else
#define GDI_API extern "C"
#endif

GDI_API nuPVOID	LibGDI_InitModule (nuPVOID lpVoidIn);
GDI_API nuVOID	LibGDI_FreeModule();

/////////////////////////////////////////////////////////////////////////////////
GDI_API nuBOOL	LibGDI_Initialize(PNURO_SCREEN pScreen);
GDI_API nuBOOL	LibGDI_ReInitialize(nuWORD nWidth, nuWORD nHeight);

GDI_API nuVOID	LibGDI_Free();

GDI_API PNURO_CANVAS  LibGDI_GetCanvas(nuINT nCvsIdx);

GDI_API nuBOOL	LibGDI_SetLeftTop(PNURO_CANVAS pCvs, nuINT *pLeft, nuINT* pTop);

GDI_API nuBOOL  LibGDI_LoadFontInfo(nuTCHAR* fontfile);

GDI_API nuVOID  LibGDI_InitCanvas(PNURO_CANVAS pCvs);
GDI_API nuBOOL	LibGDI_CreateCanvas(PNURO_CANVAS pCvs, nuWORD nWidth, nuWORD nHeight);
GDI_API nuVOID	LibGDI_DeleteCanvas(PNURO_CANVAS pCvs);
GDI_API nuBOOL	LibGDI_FlushCanvas( nuINT nScreenX, nuINT nScreenY, nuINT nWidth, nuINT nHeight
                                   , PNURO_CANVAS pSrcCvs, nuINT nSrcX, nuINT nSrcY );

GDI_API nuINT       LibGDI_SetDrawMode(NURO_CANVAS* canvas, nuINT mode);
GDI_API nuINT	    LibGDI_SetTextBkMode(PNURO_CANVAS pCvs, nuINT nMode);
GDI_API nuCOLORREF  LibGDI_SetTextBkColor(PNURO_CANVAS pCvs, nuCOLORREF nBkColor);
GDI_API nuCOLORREF  LibGDI_SetTextColor(PNURO_CANVAS pCvs, nuCOLORREF nTextColor);
GDI_API nuCOLORREF  LibGDI_SetTextEdgeColor(NURO_CANVAS* canvas, nuCOLORREF color);
GDI_API nuBOOL      LibGDI_SetTextUseEdgeColor(NURO_CANVAS* canvas, nuBOOL use);
GDI_API nuBOOL	    LibGDI_SetPen(PNURO_CANVAS pCvs, PNUROPEN pPen, PNUROPEN oldPen);
GDI_API nuBOOL	    LibGDI_SetBrush(PNURO_CANVAS pCvs, PNUROBRUSH pBrush, PNUROBRUSH oldBrush);
GDI_API nuBOOL	    LibGDI_SetLogfont(PNURO_CANVAS pCvs, PNUROFONT pLogfont, PNUROFONT oldFont);

GDI_API nuVOID      LibGDI_Set3DPara(nuPVOID Get3DThickFuc);

GDI_API nuINT	LibGDI_FillRect( PNURO_CANVAS pCvs, nuINT x, nuINT y, nuINT w, nuINT h);
GDI_API nuINT	LibGDI_Polyline( PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount);
GDI_API nuINT	LibGDI_PolygonEx( PNURO_CANVAS pCvs, const NUROPOINT* pPoint, nuINT nCount, nuINT nMode);
GDI_API nuINT	LibGDI_Ellipse( PNURO_CANVAS pCvs, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom);
GDI_API nuINT	LibGDI_TextOut( PNURO_CANVAS pCvs, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount,nuINT Parent);
GDI_API nuINT	LibGDI_DrawText( PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,nuINT Parent);
GDI_API nuINT	LibGDI_DrawTextNew( PNURO_CANVAS pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,nuINT Parent);

GDI_API nuBOOL	LibGDI_PolyDashLine(NURO_CANVAS* pCvs, const NUROPOINT* pt, nuINT ptCount, nuINT nDashLen);
GDI_API nuBOOL  LibGDI_Polyline3D(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount); 
GDI_API nuBOOL	LibGDI_PolygonBMP(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuBOOL bHasPenEdge); 
GDI_API nuBOOL	LibGDI_DrawBmp(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY);
GDI_API nuBOOL	LibGDI_DrawPng(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , nuINT PngID, nuINT nSrcX, nuINT nSrcY);
GDI_API nuBOOL	LibGDI_DrawPng2(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY);
GDI_API nuBOOL	LibGDI_DrawBmp2(PNURO_CANVAS pCvs,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH);
GDI_API nuBOOL	LibGDI_ZoomBmp(PNURO_BMP pDesBmp,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
								, const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH);
GDI_API nuBOOL	LibGDI_SaveBmp(PNURO_BMP pBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const NURO_CANVAS* pCvs, nuINT nSrcX, nuINT nSrcY);
GDI_API nuBOOL	LibGDI_PaintBmp(PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY);
//add by chang 2012.10.30
GDI_API nuBOOL  LibGDI_LoadUIPng(PPNGCTRLSTRU &p_png,nuINT &PngIndex,nuTCHAR* pngfile,nuINT pngId);
GDI_API nuBOOL  LibGDI_CleanUIPng(nuINT PngIndex,nuBOOL bCleanAll);

//Added @ 2012.11.12
GDI_API nuBOOL  LibGDI_BmpGradient(PNURO_CANVAS pCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
								   const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode);
GDI_API nuBOOL  LibGDI_ShowStartUpbmp();

#endif
