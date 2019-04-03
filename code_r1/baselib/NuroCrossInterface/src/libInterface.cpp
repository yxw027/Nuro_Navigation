
#include "../libInterface.h"
#include "../../NuroClib/NuroClib.h"

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
#define NURO_DRAW_WAY
#define NURO_FONT_WAY
#endif

#include <stdio.h>

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
#include <unistd.h>
extern nuSHORT* gRgb565Screen;
static handler_flush_proc s_flush_function_pointer;
#endif

nuVOID  nuCanvasInit(NURO_CANVAS* canvas)
{
    if (canvas)
    {
        //memset(canvas, 0, sizeof(NURO_CANVAS)); 
        nuMemset(&canvas->sysDetail, 0, sizeof(canvas->sysDetail));
    }
}

nuBOOL  nuCanvasCreate(PNURO_CANVAS pCanvas, nuWORD nWidth, nuWORD nHeight, PSYSDIFF_SCREEN pSysDiffScreen)
{
	if( !pCanvas		|| 
		!pSysDiffScreen	|| 
		!nWidth	||
		!nHeight )
	{
		return nuFALSE;
	}
	if( pCanvas->isCreated )
	{
		return nuTRUE;
	}
#ifdef NURO_OS_WINDOWS
	pCanvas->sysDetail.hMemDC	= CreateCompatibleDC(pSysDiffScreen->hScreenDC);
	
    if( NULL == pCanvas->sysDetail.hMemDC )
	{
		return nuFALSE;
	}
#endif
#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    // TODO
	pCanvas->sysDetail.hMemDC	= (nuVOID *)~0UL;
#endif
	
    pCanvas->bitmap.bmpWidth	= nWidth;
    pCanvas->bitmap.bmpHeight   = nHeight;

    nWidth = ( (nWidth + 1) / 2 ) * 2;//just 565
    pCanvas->bitmap.bytesPerLine = nWidth<<1;
	pCanvas->bitmap.bmpBuffLen	= (nWidth<<1) * nHeight;
    pCanvas->bitmap.bmpBufferType = NURO_BMP_BUFFERTYPE_OUT;

    pCanvas->bitmap.bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
    pCanvas->drawMode = NURO_BMP_TYPE_COPY;
	
#ifdef NURO_OS_WINDOWS
	nuBYTE biTmp[70];
	nuDWORD *pColor;
	BITMAPINFO* bi = (BITMAPINFO*)biTmp;
	bi->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth		= nWidth;
	bi->bmiHeader.biHeight		= -nHeight; // If the height is negative, the bitmap is a top-down DIB and its origin is the upper left corner.
	bi->bmiHeader.biPlanes		= 1;
	bi->bmiHeader.biBitCount	= 16;
	bi->bmiHeader.biCompression = BI_BITFIELDS;
	pColor = (nuDWORD*)(&bi->bmiColors[0]);
	pColor[0] = 0xF800;
	pColor[1] = 0x07E0;
	pColor[2] = 0x001F;//565
	bi->bmiHeader.biSizeImage		= 0;
	bi->bmiHeader.biClrUsed			= 0;
	bi->bmiHeader.biClrImportant	= 0;
	pCanvas->sysDetail.hBitmap = CreateDIBSection( pSysDiffScreen->hScreenDC, 
													bi, 
													DIB_RGB_COLORS, 
													(void**)(&pCanvas->bitmap.pBmpBuff), 
													NULL, 
													0 );
	if( NULL == pCanvas->sysDetail.hBitmap )
	{
		return nuFALSE;
	}
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    // TODO
    
    pCanvas->sysDetail.hBitmap = (void*)~0UL;
    pCanvas->bitmap.pBmpBuff = (unsigned char *)nuMalloc(nWidth * nHeight * 2);
	
    if( NULL == pCanvas->bitmap.pBmpBuff )
	{
		return nuFALSE;
	}
#endif

#ifdef NURO_OS_WINDOWS
	pCanvas->sysDetail.hOldBitmap = (HBITMAP)SelectObject(pCanvas->sysDetail.hMemDC, pCanvas->sysDetail.hBitmap);
#endif
#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    // TODO
    pCanvas->sysDetail.hOldBitmap = (void *)~0UL;
#endif
    pCanvas->isCreated = 1;
	return nuTRUE;
}

nuVOID  nuCanvasDestroy(PNURO_CANVAS pCanvas)
{
	if( pCanvas && pCanvas->isCreated)
    {
#ifndef NURO_DRAW_WAY
        if( NULL != pCanvas->sysDetail.hPen )
        {
            SelectObject(pCanvas->sysDetail.hMemDC, pCanvas->sysDetail.hOldPen);
            DeleteObject(pCanvas->sysDetail.hPen);
            pCanvas->sysDetail.hPen = 0;
            pCanvas->sysDetail.hOldPen = 0;
        }        
        if( NULL != pCanvas->sysDetail.hBrush )
        {
            SelectObject(pCanvas->sysDetail.hMemDC, pCanvas->sysDetail.hOldBrush);
            DeleteObject(pCanvas->sysDetail.hBrush);
            pCanvas->sysDetail.hBrush = 0;
            pCanvas->sysDetail.hOldBrush = 0;
        }
#endif
#ifndef NURO_FONT_WAY
        if( pCanvas->sysDetail.hFont )
        {
            SelectObject(pCanvas->sysDetail.hMemDC, pCanvas->sysDetail.hOldFont);
            DeleteObject(pCanvas->sysDetail.hFont);
            pCanvas->sysDetail.hFont = 0;
            pCanvas->sysDetail.hOldFont = 0;
        }
#endif

#ifdef NURO_OS_WINDOWS
		if( pCanvas->sysDetail.hOldBitmap )
		{
			SelectObject(pCanvas->sysDetail.hMemDC, pCanvas->sysDetail.hOldBitmap);
			pCanvas->sysDetail.hOldBitmap = 0;
		}
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
            // TODO
#endif

#ifdef NURO_OS_WINDOWS
		if( pCanvas->sysDetail.hBitmap )
		{
			DeleteObject(pCanvas->sysDetail.hBitmap);
			pCanvas->sysDetail.hBitmap = 0;
			pCanvas->bitmap.pBmpBuff	= 0;
		}
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
        // TODO
        if( pCanvas->bitmap.pBmpBuff )
        {
#ifdef ANDROID
            if ((void *)pCanvas->bitmap.pBmpBuff != (void *)gRgb565Screen)
            {
                nuFree(pCanvas->bitmap.pBmpBuff);
                pCanvas->bitmap.pBmpBuff = 0;
            }
#else
            nuFree(pCanvas->bitmap.pBmpBuff);
            pCanvas->bitmap.pBmpBuff = 0;
#endif
        }
#endif

#ifdef NURO_OS_WINDOWS
        if( pCanvas->sysDetail.hMemDC )
		{
			DeleteDC(pCanvas->sysDetail.hMemDC);
			pCanvas->sysDetail.hMemDC = 0;
		}
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
            // TODO
#endif
        pCanvas->isCreated = 0;
	}
    nuCanvasInit(pCanvas);
}
nuBOOL  nuCanvasFlush(	PNURO_SCREEN pDesCvs,
                      nuINT nDesX,
                      nuINT nDesY,
                      nuINT nWidth,
                      nuINT nHeight,
                      PNURO_CANVAS pSrcCvs,
                      nuINT nSrcX,
                      nuINT nSrcY )
{
#ifdef NURO_OS_WINDOWS
    if( BitBlt( pDesCvs->sysDiff.hScreenDC, 
        nDesX, 
        nDesY, 
        nWidth, 
        nHeight, 
        pSrcCvs->sysDetail.hMemDC, 
        nSrcX, 
        nSrcY,
        SRCCOPY ) )
    {
        return nuTRUE;
    }
    else
    {
        return nuFALSE;
    }
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    // TODO

#ifndef ANDROID

    if (s_flush_function_pointer)
    {
        s_flush_function_pointer(pSrcCvs->bitmap.pBmpBuff, nWidth, nHeight);
    }

#else
    
    // fastten speed
    //if ((void *)gRgb565Screen != (void *)pSrcCvs->bitmap.pBmpBuff)
    {
        nuMemcpy(gRgb565Screen, pSrcCvs->bitmap.pBmpBuff, sizeof(nuSHORT) * nWidth * nHeight);
    //    nuFree(pSrcCvs->bitmap.pBmpBuff);
    //    pSrcCvs->bitmap.pBmpBuff = (nuBYTE *)gRgb565Screen;
    }
    
    if (s_flush_function_pointer)
    {
        s_flush_function_pointer(pSrcCvs->bitmap.pBmpBuff, nWidth, nHeight);
    }

#endif

    return nuTRUE;
#endif
}

#ifndef NURO_DRAW_WAY
nuVOID nuCanvasSetTextBkMode(NURO_CANVAS* canvas)
{
    SetBkMode(canvas->sysDetail.hMemDC, canvas->textBkMode);
}
nuVOID  nuCanvasSetTextBkColor(NURO_CANVAS* canvas)
{
    SetBkColor(canvas->sysDetail.hMemDC, canvas->textBkColor);
}
nuVOID  nuCanvasSetTextColor(NURO_CANVAS* canvas)
{
    SetTextColor(canvas->sysDetail.hMemDC, canvas->textColor);
}
nuBOOL  nuCanvasSetPen(NURO_CANVAS* canvas)
{
    HPEN newPen = CreatePen( canvas->pen.nStyle, canvas->pen.nWidth
        , RGB(canvas->pen.nRed, canvas->pen.nGreen, canvas->pen.nBlue) );
    if (!newPen)
    {
        return nuFALSE;
    }
    if( canvas->sysDetail.hPen )
    {
        SelectObject(canvas->sysDetail.hMemDC, canvas->sysDetail.hOldPen);
        DeleteObject(canvas->sysDetail.hPen);
        canvas->sysDetail.hPen = 0;
        canvas->sysDetail.hOldPen = 0;
    }
    canvas->sysDetail.hPen = newPen;
    canvas->sysDetail.hOldPen = (HPEN)SelectObject(canvas->sysDetail.hMemDC, canvas->sysDetail.hPen);
    return nuTRUE;
}
nuBOOL  nuCanvasSetFont(NURO_CANVAS* canvas)
{
    LOGFONT logFont;
    memset(&logFont, 0, sizeof(LOGFONT));
    logFont.lfHeight		= canvas->font.lfHeight;
    logFont.lfWidth			= canvas->font.lfWidth;
    logFont.lfWeight		= canvas->font.lfWeight;
    logFont.lfEscapement	= canvas->font.lfEscapement;
    nuTcscpy(logFont.lfFaceName, canvas->font.lfFaceName);//Face name, can use other facename here
    logFont.lfCharSet		= DEFAULT_CHARSET;
    logFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
    logFont.lfItalic		= 0;
    logFont.lfOrientation	= 0;
    logFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
    logFont.lfPitchAndFamily= DEFAULT_PITCH;
    logFont.lfQuality		= DEFAULT_QUALITY;
    logFont.lfStrikeOut		= 0;
    logFont.lfUnderline		= 0;    
    HFONT newFont = CreateFontIndirect(&logFont);
    if (!newFont)
    {
        return nuFALSE;
    }
    if( canvas->sysDetail.hFont )
    {
        SelectObject(canvas->sysDetail.hMemDC, canvas->sysDetail.hOldFont);
        DeleteObject(canvas->sysDetail.hFont);
        canvas->sysDetail.hFont = 0;
        canvas->sysDetail.hOldFont = 0;
    }
    canvas->sysDetail.hFont = newFont;
    canvas->sysDetail.hOldFont = (HFONT)SelectObject(canvas->sysDetail.hMemDC, canvas->sysDetail.hFont);
    return nuTRUE;
}
nuBOOL  nuCanvasSetBrush(NURO_CANVAS* canvas)
{
    HBRUSH newBrush = CreateSolidBrush( RGB( canvas->brush.nRed,canvas->brush.nGreen,canvas->brush.nBlue) );
    if (!newBrush)
    {
        return nuFALSE;
    }
    if( canvas->sysDetail.hBrush )
    {
        SelectObject(canvas->sysDetail.hMemDC, canvas->sysDetail.hOldBrush);
        DeleteObject(canvas->sysDetail.hBrush);
        canvas->sysDetail.hBrush = 0;
        canvas->sysDetail.hOldBrush = 0;
    }
    canvas->sysDetail.hBrush = newBrush;
    SelectObject( canvas->sysDetail.hMemDC, canvas->sysDetail.hBrush );
    return nuTRUE;
}

nuINT	nuCanvasFillRect(NURO_CANVAS* canvas, nuINT x, nuINT y, nuINT w, nuINT h)
{
    RECT rect = {x, y, x+w, y+h};
    return FillRect(canvas->sysDetail.hMemDC, &rect, canvas->sysDetail.hBrush);
}
nuINT	nuCanvasPolyline(NURO_CANVAS* canvas, const NUROPOINT* pPoint, nuINT nCount)
{ 
    return Polyline(canvas->sysDetail.hMemDC, (CONST POINT*)pPoint, nCount);
}
nuINT	nuCanvasPolygon(NURO_CANVAS* canvas, const NUROPOINT* pPoint, nuINT nCount)
{
    return Polygon(canvas->sysDetail.hMemDC, (CONST POINT*)pPoint, nCount);
}
nuINT	nuCanvasEllipse(NURO_CANVAS* canvas, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom)
{
    return Ellipse(canvas->sysDetail.hMemDC, nLeft, nTop, nRight, nBottom);
}
nuINT	nuCanvasTextOut(NURO_CANVAS* canvas, nuINT X, nuINT Y, WCHAR *wStr, nuUINT nCount)
{
	return ExtTextOutW(canvas->sysDetail.hMemDC, X, Y, ETO_CLIPPED, NULL, wStr, nCount, NULL);
}
nuINT	nuCanvasDrawText(NURO_CANVAS* canvas, WCHAR* wStr, nuINT nCount, NURORECT* pRect, nuUINT nFormat)
{
	return DrawTextW(canvas->sysDetail.hMemDC, wStr, nCount, (RECT*)pRect, nFormat);
}
#endif

nuBOOL  nuCheckSysDiffScreen(PSYSDIFF_SCREEN pSysDiff)
{
#ifdef NURO_OS_WINDOWS
    if (NULL == pSysDiff->hWnd || NULL == pSysDiff->hScreenDC)
    {
        return nuFALSE;
    }
#endif

#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    // Doing nothing and allways true
    // ...
#endif
	return nuTRUE;
}

nuHWND nuGetSysDiffScreenWnd(PSYSDIFF_SCREEN pSysDiff)
{
#ifdef NURO_OS_WINDOWS
	return pSysDiff->hWnd;
#endif
#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    return (nuHWND)(0x01);
#endif
}

nuHDC  nuGetSysDiffScreenDC(PSYSDIFF_SCREEN pSysDiff)
{
#ifdef NURO_OS_WINDOWS
	return pSysDiff->hScreenDC;
#endif
#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    return (nuHDC)(0x01);
#endif
}

nuBOOL nuSetFlushCallback(handler_flush_proc p_fn)
{
#if defined NURO_OS_LINUX || defined NURO_OS_UCOS
    s_flush_function_pointer = p_fn;
#endif
    return nuTRUE;
}

