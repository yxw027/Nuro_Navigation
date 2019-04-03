#ifndef __NURO_INTERFACE_H_20091001
#define __NURO_INTERFACE_H_20091001

#include "../Inc/NuroTypes.h"
#include "NuroBMP.h"

#ifdef NURO_OS_WINDOWS
#include <windows.h>
#endif

typedef nuVOID (*handler_flush_proc)(nuPVOID buf, nuINT x, nuINT y);

/* Nuro pen struct */
#define NURO_PS_SOLID            0
#define NURO_PS_DASH             1       /* -------  */
#define NURO_PS_DOT              2       /* .......  */
#define NURO_PS_DASHDOT          3       /* _._._._  */
#define NURO_PS_DASHDOTDOT       4       /* _.._.._  */
#define NURO_PS_NULL             5
#define NURO_PS_INSIDEFRAME      6
#define NURO_PS_USERSTYLE        7
#define NURO_PS_ALTERNATE        8
typedef struct nuroPEN
{
	nuBYTE	nStyle;
	nuBYTE	nWidth;
	nuBYTE  nRed;
	nuBYTE	nGreen;
	nuBYTE	nBlue;
	nuBYTE	nReserve[3];
}NUROPEN, *PNUROPEN;
typedef const NUROPEN*	PCNUROPEN;

/* Nuro brush struct */
typedef struct nuroBRUSH
{
	nuBYTE	nRed;
	nuBYTE	nGreen;
	nuBYTE	nBlue;
	nuBYTE	nReserve;
}NUROBRUSH, *PNUROBRUSH;
typedef const NUROBRUSH*	PCNUROBRUSH;

/* Nuro font struct */
#define NURO_LF_FACESIZE			32
typedef struct nuroFONT
{
	nuLONG		lfHeight;
	nuLONG		lfWidth;
	nuLONG		lfEscapement;
	nuLONG		lfWeight;
	nuTCHAR		lfFaceName[NURO_LF_FACESIZE];
}NUROFONT, *PNUROFONT;
typedef struct nuroLOGFONT
{
	nuCOLORREF	color;
	nuLONG      lfHeight;
	nuLONG      lfWidth;
	nuLONG      lfEscapement;
	nuLONG      lfWeight;
	nuTCHAR     lfFaceName[NURO_LF_FACESIZE];
}NUROLOGFONT, *PNUROLOGFONT;
typedef const NUROFONT*	PCNUROFONT;

typedef struct tagSYSDIFF_CANVAS
{
	HDC			hMemDC;
	HBITMAP		hBitmap;
	HBITMAP		hOldBitmap;
#ifndef NURO_FONT_WAY
	HFONT		hFont;
	HFONT		hOldFont;
#endif
#ifndef NURO_DRAW_WAY
	HPEN		hPen;
	HPEN		hOldPen;
	HBRUSH		hBrush;
	HBRUSH		hOldBrush;
#endif
}SYSDIFF_CANVAS, *PSYSDIFF_CANVAS;

typedef struct tagSYSDIFF_SCREEN
{
#ifdef NURO_OS_WINDOWS
	HWND		hWnd;
	HDC			hScreenDC;
#endif
#ifdef NURO_OS_LINUX
    nuPVOID     hScreenBuff;
#endif
}SYSDIFF_SCREEN, *PSYSDIFF_SCREEN;

typedef struct tagNURO_CANVAS
{
	nuBYTE			isCreated;
	nuBYTE			cvsType;
	nuWORD			nReserve;

	nuINT			cvsLeft;
	nuINT			cvsTop;

	nuBOOL          textUseEdgeColor;   
	nuINT           drawMode;
	nuINT			textBkMode;
	nuCOLORREF      textColor;
	nuCOLORREF		textBkColor;
	nuCOLORREF      textEdgeColor;
	NUROPEN			pen;
	NUROFONT		font;
	NUROBRUSH		brush;

	NURO_BMP		bitmap;
	SYSDIFF_CANVAS	sysDetail;
}NURO_CANVAS, *PNURO_CANVAS;

typedef struct tagNURO_SCREEN
{
	nuINT			nScreenLeft;
	nuINT			nScreenTop;
	nuINT			nScreenWidth;
	nuINT			nScreenHeight;
	SYSDIFF_SCREEN	sysDiff;
}NURO_SCREEN, *PNURO_SCREEN;

#ifdef __cplusplus
extern "C"
{
#endif
	//Gdi interface

    nuVOID  nuCanvasInit(NURO_CANVAS* canvas);
    nuBOOL  nuCanvasCreate(NURO_CANVAS* canvas, nuWORD nWidth, nuWORD nHeight, SYSDIFF_SCREEN* sysDiffScreen);
    nuVOID  nuCanvasDestroy(NURO_CANVAS* canvas);
    nuBOOL  nuCanvasFlush( NURO_SCREEN* pDesCvs, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
		                   PNURO_CANVAS pSrcCvs, nuINT nSrcX, nuINT nSrcY );
    
    nuVOID  nuCanvasSetTextBkMode(NURO_CANVAS* canvas);
    nuVOID  nuCanvasSetTextBkColor(NURO_CANVAS* canvas);
    nuVOID  nuCanvasSetTextColor(NURO_CANVAS* canvas);    
    nuBOOL  nuCanvasSetPen(NURO_CANVAS* canvas);
    nuBOOL  nuCanvasSetFont(NURO_CANVAS* canvas);
    nuBOOL  nuCanvasSetBrush(NURO_CANVAS* canvas);

    nuINT	nuCanvasFillRect(NURO_CANVAS* canvas, nuINT x, nuINT y, nuINT w, nuINT h);
    nuINT	nuCanvasPolyline(NURO_CANVAS* canvas, const NUROPOINT* pPoint, nuINT nCount);
    nuINT	nuCanvasPolygon(NURO_CANVAS* canvas, const NUROPOINT* pPoint, nuINT nCount); 
    nuINT	nuCanvasEllipse(NURO_CANVAS* canvas, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom);   
    nuINT	nuCanvasTextOut(NURO_CANVAS* canvas, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount);
    nuINT	nuCanvasDrawText(NURO_CANVAS* canvas, nuWCHAR* wStr, nuINT nCount, NURORECT* pRect, nuUINT nFormat);

	nuBOOL  nuCheckSysDiffScreen(PSYSDIFF_SCREEN pSysDiff);
	nuHWND  nuGetSysDiffScreenWnd(PSYSDIFF_SCREEN pSysDiff);
	nuHDC   nuGetSysDiffScreenDC(PSYSDIFF_SCREEN pSysDiff);
    nuBOOL nuSetFlushCallback(handler_flush_proc p_fn);

#ifdef __cplusplus
}
#endif

#endif
