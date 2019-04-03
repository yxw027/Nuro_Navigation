// CanvasMgrZK.cpp: implementation of the CCanvasMgrZK class.
//
//////////////////////////////////////////////////////////////////////

#include "CanvasMgrZK.h"
#include "nuRotaryClip.h"
#include "NuroClasses.h"

#ifdef NURO_USE_BMPLIB3_FILLGON
#include "nuBMPlib3.h"
#endif

#ifdef NURO_FONT_ENGINE_USE_FREETYPE
	nuroFontEngine  m_fontmgr;
	nuroFontEngine m_fontmgr2;
#endif


#define STARTUP_BMP_FILE_PATH   nuTEXT("Media\\Nuroui\\nuro_startup.bmp")

PNURO_CANVAS CCanvasMgrZK::GetCanvas(nuINT nCvsIdx)
{
    if( nCvsIdx < 0 || m_nCvsCount == 0 )
    {
        return &m_cvsGdi;
    }
    else if( nCvsIdx >= m_nCvsCount )
    {
        return m_pCvsList[m_nCvsCount - 1].pNuroCvs;
    }
    else
    {
        return m_pCvsList[nCvsIdx].pNuroCvs;
    }
}

nuBOOL CCanvasMgrZK::SetCvsLeftTop(PNURO_CANVAS pCvs, nuINT *pLeft, nuINT* pTop)
{
    if( nuNULL == pCvs )
    {
        pCvs = &m_cvsGdi;
    }
    nuINT nOldL = pCvs->cvsLeft;
    nuINT nOldT = pCvs->cvsTop;
    pCvs->cvsLeft	= *pLeft;
    pCvs->cvsTop	= *pTop;
    *pLeft	= nOldL;
    *pTop	= nOldT;
    return nuTRUE;
}

CCanvasMgrZK::CCanvasMgrZK()
{
    CvsInit(&m_cvsGdi);
    CvsInit(&m_cvsBuffer);
    nuMemset(&m_Screen, 0, sizeof(m_Screen));
    nuMemset(m_pCvsList, 0, sizeof(m_pCvsList));
    m_nCvsCount	= 0;
    m_bFlush	= nuFALSE;
}

CCanvasMgrZK::~CCanvasMgrZK()
{
    Free();
}

nuBOOL CCanvasMgrZK::Initialize(PNURO_SCREEN pScreen)
{
    if( nuNULL == pScreen )
    {
        return nuFALSE;
    }
    m_Screen = *pScreen;
    if( !CvsCreate(&m_cvsBuffer, pScreen->nScreenWidth, pScreen->nScreenHeight) )
    {
        return nuFALSE;
    }

#ifndef _USE_CANVAS_SINGLE
    m_cvsBuffer.cvsType = NURO_CANVAS_TYPE_SINGLE;
#endif
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
    m_fontmgr.Init();
    m_fontmgr2.Init();
#else
    m_fontmgr.Init();
#endif
	
	//在這裡進行PNG的初始化動做吧 Louis 20120828
	m_pngctrl.InitPngCtrl(10);//創建10個結構(以目前的系統來說 預先創建的結構應該要有數百個才行)

    return nuTRUE;
}

nuBOOL CCanvasMgrZK::ReInitialize(nuWORD nWidth, nuWORD nHeight)
{
	m_Screen.nScreenWidth  = nWidth;
	m_Screen.nScreenHeight = nHeight;

	CvsDestroy(&m_cvsBuffer);
	if( !CvsCreate(&m_cvsBuffer, m_Screen.nScreenWidth, m_Screen.nScreenHeight) )
	{
		return nuFALSE;
	}

	CvsDestroy(NULL);
	if( !CvsCreate(NULL, m_Screen.nScreenWidth, m_Screen.nScreenHeight) )
	{
		return nuFALSE;
	}

	return nuTRUE;
}

nuVOID CCanvasMgrZK::Free()
{
	//在這裡解構PNG的資料 Louis20120828
	m_pngctrl.RelPngCtrl();//解構記憶體
	
     CvsDestroy(&m_cvsBuffer);
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
     m_fontmgr.Destroy();
     m_fontmgr2.Destroy();
#else
     m_fontmgr.Destroy();
#endif
}

nuBOOL CCanvasMgrZK::LoadFontInfo(nuTCHAR* fontfile)
{
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
	m_fontmgr.LoadFontInfo(fontfile);
    return m_fontmgr2.LoadFontInfo(fontfile);
#else
    return m_fontmgr.LoadFontInfo(fontfile);
#endif
}

nuBOOL CCanvasMgrZK::LoadUIPng(PPNGCTRLSTRU &p_png, nuINT &PngIndex,nuTCHAR* pngfile/* = NULL*/,nuINT pngId/* = 0*/)
{
	if (NULL == pngfile || 0 == pngId)
	{
		return nuFALSE;
	}
	if (!m_pngctrl.load_png(pngfile,PngIndex,pngId))
	{
		return nuFALSE;
	}
	if (!m_pngctrl.Get_PngData_id(pngId,p_png))
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CCanvasMgrZK::CleanUIPng(nuINT PngIndex,nuBOOL bCleanAll)
{
	m_pngctrl.Clean_Png(PngIndex,bCleanAll);
	return nuTRUE;
}

nuVOID CCanvasMgrZK::CheckCanvas(NURO_CANVAS** canvas)
{        
    if( !*canvas )
    {
        *canvas = &m_cvsGdi;
    }
}
nuVOID CCanvasMgrZK::CvsInit(PNURO_CANVAS pCvs)
{
    CheckCanvas(&pCvs);   
    pCvs->isCreated = 0;
    pCvs->cvsType = 0;
    pCvs->nReserve = 0;        
    pCvs->cvsLeft = 0;
    pCvs->cvsTop = 0;        
    pCvs->textUseEdgeColor = false;   
    pCvs->drawMode = NURO_BMP_TYPE_COPY;
    pCvs->textBkMode = NURO_TRANSPARENT;
    pCvs->textColor = 0;
    pCvs->textBkColor = 0;
    pCvs->textEdgeColor = 0;
    memset(&pCvs->pen, 0, sizeof(pCvs->pen));
    memset(&pCvs->font, 0, sizeof(pCvs->font));
    memset(&pCvs->brush, 0, sizeof(pCvs->brush));
    CBmpMethodsZK::BmpInit(&pCvs->bitmap);
    nuCanvasInit(pCvs);
}   
nuBOOL CCanvasMgrZK::CvsCreate(PNURO_CANVAS pCvs, nuWORD nWidth, nuWORD nHeight)
{
    CheckCanvas(&pCvs);
    if( !nuCanvasCreate(pCvs, nWidth, nHeight, &m_Screen.sysDiff) )
    {
        return nuFALSE;
    }
	CvsSetDrawMode(pCvs, NURO_BMP_TYPE_COPY);
    CvsSetTextBkColor(pCvs, 0);
    CvsSetTextColor(pCvs, 0XFFFFFFFF);
    CvsSetTextEdgeColor(pCvs, 0);
    CvsSetTextUseEdgeColor(pCvs, nuTRUE);
	NUROPEN pen;
	nuMemset(&pen, 0, sizeof(pen));
	pen.nBlue = 0;
	pen.nGreen = 0;
	pen.nRed = 0xff;
	pen.nStyle = NURO_PS_SOLID;
	pen.nWidth = 1;
    CvsSetPen(pCvs, pen, 0);
	NUROBRUSH brush;
	nuMemset(&brush, 0, sizeof(brush));
	brush.nGreen = 0;
	brush.nRed = 0;
	brush.nBlue = 0xff;
    CvsSetBrush(pCvs, brush, 0);
	NUROFONT font;
	nuMemset(&font, 0, sizeof(font));
	font.lfHeight = 16;
	CvsSetFont(pCvs, font,0);
    return nuTRUE;
}

nuVOID CCanvasMgrZK::CvsDestroy(PNURO_CANVAS pCvs)
{
    CheckCanvas(&pCvs);
    nuCanvasDestroy(pCvs);
    CBmpMethodsZK::BmpDestroy(&pCvs->bitmap);
}
nuINT CCanvasMgrZK::CvsSetDrawMode(NURO_CANVAS* canvas, nuINT mode)
{
    CheckCanvas(&canvas);
    nuINT oldMode = canvas->drawMode;
    canvas->drawMode = mode;
    return oldMode;
}
nuINT CCanvasMgrZK::CvsSetTextBkMode(NURO_CANVAS* canvas, nuINT mode)
{
    CheckCanvas(&canvas);
    nuINT oldMode = canvas->textBkMode;
    canvas->textBkMode = mode;
#ifdef NURO_FONT_WAY
#else
    nuCanvasSetTextBkMode(canvas);
#endif
    return oldMode;
}
nuCOLORREF CCanvasMgrZK::CvsSetTextBkColor(NURO_CANVAS* canvas, nuCOLORREF color)
{
    CheckCanvas(&canvas);
    nuCOLORREF oldColor = canvas->textBkColor;
    canvas->textBkColor = color;
#ifdef NURO_FONT_WAY
#else 
    nuCanvasSetTextBkColor(canvas);
#endif
    return oldColor;
}
nuCOLORREF CCanvasMgrZK::CvsSetTextColor(NURO_CANVAS* canvas, nuCOLORREF color)
{    
    CheckCanvas(&canvas);
    nuCOLORREF oldColor = canvas->textColor;
    canvas->textColor = color;
#ifdef NURO_FONT_WAY
#else
    nuCanvasSetTextColor(canvas);
#endif
    return oldColor;
}
nuCOLORREF CCanvasMgrZK::CvsSetTextEdgeColor(NURO_CANVAS* canvas, nuCOLORREF color)
{
    CheckCanvas(&canvas);
    nuCOLORREF oldColor = canvas->textEdgeColor;
    canvas->textEdgeColor = color;
    return oldColor;
}
nuBOOL CCanvasMgrZK::CvsSetTextUseEdgeColor(NURO_CANVAS* canvas, nuBOOL use)
{
    CheckCanvas(&canvas);
    nuBOOL old = canvas->textUseEdgeColor;
    canvas->textUseEdgeColor = use;
    return old;
}
nuBOOL CCanvasMgrZK::CvsSetPen(NURO_CANVAS* canvas, NUROPEN& pen, NUROPEN* oldPen)
{
    CheckCanvas(&canvas);
    if (oldPen)
    {
        *oldPen = canvas->pen;
    }
    canvas->pen = pen;
#ifdef NURO_DRAW_WAY
    return nuTRUE;
#else
    return nuCanvasSetPen(canvas);
#endif
}
nuBOOL CCanvasMgrZK::CvsSetFont(NURO_CANVAS* canvas, NUROFONT& font, NUROFONT* oldFont)
{
    CheckCanvas(&canvas);
    if (oldFont)
    {
        *oldFont = canvas->font;
    }
    canvas->font = font;
#ifdef NURO_FONT_WAY
    return nuTRUE;
#else
    return nuCanvasSetFont(canvas);
#endif
}
nuBOOL CCanvasMgrZK::CvsSetBrush(NURO_CANVAS* canvas, NUROBRUSH& brush, NUROBRUSH* oldBrush)
{
    CheckCanvas(&canvas);
    if (oldBrush)
    {
        *oldBrush = canvas->brush;
    }
    canvas->brush = brush;
#ifdef NURO_DRAW_WAY
    return nuTRUE;
#else
    return nuCanvasSetBrush(canvas);
#endif
}
nuINT	CCanvasMgrZK::CvsFillRect(NURO_CANVAS* canvas, nuINT x, nuINT y, nuINT w, nuINT h)
{
    CheckCanvas(&canvas);
#ifdef NURO_DRAW_WAY
    nuCOLORREF realBColor = nuRGB(canvas->brush.nRed, canvas->brush.nGreen, canvas->brush.nBlue);
    CGdiMethodZK::nuBMPFillRect(&canvas->bitmap, x, y, w, h, realBColor, NURO_BMP_TYPE_COPY);
    return 1;
#else
    return nuCanvasFillRect(canvas, x, y, w, h);
#endif
}

nuINT CCanvasMgrZK::CvsPolyline(NURO_CANVAS* pCvs, const NUROPOINT* pPoint, nuINT nCount)
{
    CheckCanvas(&pCvs);
#ifdef NURO_DRAW_WAY
    nuCOLORREF realPColor = nuRGB(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
    if (pCvs->pen.nStyle == NURO_PS_SOLID)
    {
        if( pCvs->pen.nWidth > 2 )
        {
            nuWORD nColor565 = nuRGB24TO16_565(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
            CGdiMethodZK::nuBMP565PolyLineZ( &pCvs->bitmap, pPoint, nCount, pCvs->pen.nWidth-1, nColor565);
        }
        else
        {
            CGdiMethodZK::nuBMPPolyLine(	&pCvs->bitmap, pPoint, nCount, pCvs->pen.nWidth, realPColor, pCvs->drawMode);
        }
    }
    return 0;
#else
    return nuCanvasPolyline(pCvs, pPoint, nCount);
#endif
}
nuVOID CCanvasMgrZK::CvsSet3DParam(Get3DThickProc Get3DThickFuc)
{
    m_Get3DThickFuc = Get3DThickFuc;
}
nuINT	CCanvasMgrZK::CvsPolygonEx(NURO_CANVAS* pCvs, const NUROPOINT* pPoint, nuINT nCount, nuINT nMode)
{
    CheckCanvas(&pCvs);
	// Added by Damon For Mode Must Change
	nuINT  nOldMode  = pCvs->drawMode;
	pCvs->drawMode = nMode;
	// -----------------------------------
#ifdef NURO_DRAW_WAY
    nuCOLORREF realPColor = 0;
    if(pCvs->pen.nStyle == NURO_PS_SOLID)
    {
        realPColor = nuRGB(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
    }
    else
    {
        realPColor = nuRGB(pCvs->brush.nRed, pCvs->brush.nGreen, pCvs->brush.nBlue);
    }
    nuCOLORREF realBColor
        = nuRGB(pCvs->brush.nRed, pCvs->brush.nGreen, pCvs->brush.nBlue);

#ifdef NURO_USE_BMPLIB3_FILLGON
    if(pCvs->drawMode)
    {
        nuBMPFillGon_New(&pCvs->bitmap, pPoint, nCount, nuCOLOR24TO16_565(realBColor), pCvs->drawMode);
    }
    if (pCvs->pen.nStyle == NURO_PS_SOLID && pCvs->pen.nWidth>=1)
    {
        CvsPolyline(pCvs, pPoint, nCount);
        if( (pPoint[0].x != pPoint[nCount-1].x) || (pPoint[0].y != pPoint[nCount-1].y))
        {
            NUROPOINT tmpPoint[2];
            tmpPoint[0] = pPoint[0];
            tmpPoint[1] = pPoint[nCount-1];
            CvsPolyline(pCvs, tmpPoint, 2);
        }
    }
	pCvs->drawMode = nOldMode;

    return nuTRUE;
#else
    CGdiMethodZK::nuBMPFillGon(&pCvs->bitmap, pPoint, nCount, realBColor, realPColor, pCvs->drawMode);
    if (pCvs->pen.nStyle == NURO_PS_SOLID && pCvs->pen.nWidth>=1)
    {
        CvsPolyline(pCvs, pPoint, nCount);
        if( (pPoint[0].x != pPoint[nCount-1].x) || (pPoint[0].y != pPoint[nCount-1].y))
        {
            NUROPOINT tmpPoint[2];
            tmpPoint[0] = pPoint[0];
            tmpPoint[1] = pPoint[nCount-1];
            CvsPolyline(pCvs, tmpPoint, 2);
        }
    }
    else
    {
        CGdiMethodZK::nuBMPPolyLine(&pCvs->bitmap, pPoint, nCount, 1, realPColor, pCvs->drawMode);
        if( (pPoint[0].x != pPoint[nCount-1].x) || (pPoint[0].y != pPoint[nCount-1].y))
        {
            NUROPOINT tmpPoint[2];
            tmpPoint[0] = pPoint[0];
            tmpPoint[1] = pPoint[nCount-1];
            CGdiMethodZK::nuBMPPolyLine(&pCvs->bitmap, tmpPoint, 2, 1, realPColor, pCvs->drawMode);
        }
    }
    return 0;
#endif
#else
    return nuCanvasPolygon(pCvs, pPoint, nCount);
#endif
}

nuINT	CCanvasMgrZK::CvsEllipse(NURO_CANVAS* pCvs, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom)
{
    CheckCanvas(&pCvs);
#ifdef NURO_DRAW_WAY
    nuCOLORREF realBColor
        = nuRGB(pCvs->brush.nRed, pCvs->brush.nGreen, pCvs->brush.nBlue);   
    CGdiMethodZK::nuBMPFillEllipse(&pCvs->bitmap, nLeft, nTop, nRight, nBottom, realBColor, pCvs->drawMode);
    if(  pCvs->pen.nStyle == NURO_PS_SOLID)
    {
        nuCOLORREF realPColor
            = nuRGB(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
        CGdiMethodZK::nuBMPDrawEllipse(&pCvs->bitmap, nLeft, nTop, nRight, nBottom
            , pCvs->pen.nWidth, realPColor, pCvs->drawMode);
    }
    return 0;
#else
    return nuCanvasEllipse(pCvs, nLeft, nTop, nRight, nBottom);
#endif
}


nuINT CCanvasMgrZK::CvsTextOut(NURO_CANVAS* pCvs, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount,nuINT parent)
{
    CheckCanvas(&pCvs);
#ifdef NURO_FONT_WAY
#   ifdef NURO_FONT_ENGINE_USE_FREETYPE
	if(parent==0)
	{
		return m_fontmgr.DrawTextOut(&pCvs->bitmap, X, Y, wStr, nCount, pCvs->font.lfHeight, pCvs->textColor
				, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}
	else
	{
		return m_fontmgr2.DrawTextOut(&pCvs->bitmap, X, Y, wStr, nCount, pCvs->font.lfHeight, pCvs->textColor
				, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}
#   else
    return m_fontmgr.DrawTextOut(&pCvs->bitmap, X, Y, wStr, nCount, pCvs->font.lfHeight, pCvs->textColor
        , pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
#   endif
#else
    return nuCanvasTextOut(pCvs, X, Y, wStr, nCount);
#endif
}
nuINT	CCanvasMgrZK::CvsDrawText(NURO_CANVAS* pCvs, nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat,nuINT parent)
{
    CheckCanvas(&pCvs);
#ifdef NURO_FONT_WAY
#   ifdef NURO_FONT_ENGINE_USE_FREETYPE
	if(parent==0)
	{
		return m_fontmgr.DrawTextOut2(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
			   ,pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}
	else
	{
		return m_fontmgr2.DrawTextOut2(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
			   ,pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}

#   else
    return m_fontmgr.DrawTextOut2(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
        , pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
#   endif
#else
    return nuCanvasDrawText(pCvs, wStr, nCount, pRect, nFormat);
#endif
}

nuINT CCanvasMgrZK::CvsDrawTextNew(NURO_CANVAS* pCvs, nuWCHAR* wStr, nuINT nCount, NURORECT* pRect, nuUINT nFormat,nuINT parent)
{
	CheckCanvas(&pCvs);
#ifdef NURO_FONT_WAY
#   ifdef NURO_FONT_ENGINE_USE_FREETYPE
	if(parent==0)
	{
		return m_fontmgr.DrawTextOutNew(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
			,pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}
	else
	{
		return m_fontmgr2.DrawTextOutNew(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
			,pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
	}

#   else
	return m_fontmgr.DrawTextOut2(&pCvs->bitmap, pRect, nFormat, wStr, nCount, pCvs->font.lfHeight
		, pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor);
#   endif
#else
	return nuCanvasDrawText(pCvs, wStr, nCount, pRect, nFormat);
#endif
}


nuBOOL CCanvasMgrZK::CvsTextOutRotary(NURO_CANVAS* pCvs, nuINT X,
                                    nuINT Y, 
                                    nuFLOAT centerscalex,
                                    nuFLOAT centerscaley,
                                    nuLONG RotaryAngle,
                                    nuWCHAR *wStr,
                                    nuUINT nCount, nuINT parent)
{    
    CheckCanvas(&pCvs);
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
	if(parent==0)
	{
		return m_fontmgr.DrawTextOut(&pCvs->bitmap, X, Y, wStr, nCount, pCvs->font.lfHeight, pCvs->textColor
        , pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor, RotaryAngle);
	}
	else
	{
	     return m_fontmgr2.DrawTextOut(&pCvs->bitmap, X, Y, wStr, nCount, pCvs->font.lfHeight, pCvs->textColor
        , pCvs->textUseEdgeColor, pCvs->textEdgeColor, pCvs->textBkMode, pCvs->textBkColor, RotaryAngle);
	}
#else
    return m_fontmgr.DrawTextOutRotary(&pCvs->bitmap, X, Y, centerscalex, centerscaley, RotaryAngle
        , wStr, nCount, pCvs->font.lfHeight, pCvs->textColor, pCvs->textUseEdgeColor, pCvs->textEdgeColor);
#endif
}
nuBOOL CCanvasMgrZK::CvsPolyDashLine(NURO_CANVAS* pCvs, const NUROPOINT* pt, nuINT ptCount, nuINT nDashLen)
{
    CheckCanvas(&pCvs);
	//
	int nTmpDashList = TMPDASHLIST;
    if( pt == nuNULL || ptCount < 2 )
    {
        return nuFALSE;
    }
    if( nuNULL == pCvs )
    {
        pCvs = &m_cvsGdi;
    }
    if( nDashLen < 10 )
    {
        nDashLen = DASHLEN;
    }
	//---------- add for 弊賜剞盄----------//
	if( ptCount > 2 && ptCount < 20 )
	{
		nDashLen = 4;
		nTmpDashList = 3;
	}
	//-------------------------------------//
    nuLONG dx, dy, dis = 0, tmpDis, ddis, discopy;
    nuLONG lenLeft = 0;
    nuroPOINT pPt[TMPDASHLIST], tmpPt;
    nuLONG ptIdx = 0;
    nuBOOL bDrawOrSkip = nuFALSE;
    nuroPOINT pPtTmp[2];
    for(nuLONG i = 1; i < ptCount; ++i)
    {
        pPtTmp[0] = pt[i-1];
        pPtTmp[1] = pt[i];
        CGdiMethodZK::nuLineLimitToRectZ( (nuINT&)pPtTmp[0].x
            , (nuINT&)pPtTmp[1].x
            , (nuINT&)pPtTmp[0].y
            , (nuINT&)pPtTmp[1].y
            , 0
            , pCvs->bitmap.bmpWidth
            , 0
            , pCvs->bitmap.bmpHeight );
        tmpPt = pPtTmp[0];
        pPt[ptIdx] = tmpPt;
        ptIdx++;
        if( ptIdx >= nTmpDashList )
        {
            if( bDrawOrSkip )
            {
                CvsPolyline(pCvs, pPt, ptIdx);
            }
            pPt[0]		= tmpPt;
            ptIdx		= 1;
            bDrawOrSkip = !bDrawOrSkip;
            lenLeft		= 0;
        }
        dx = pPtTmp[1].x - pPtTmp[0].x;
        dy = pPtTmp[1].y - pPtTmp[0].y;
        discopy = dis = nulSqrt(dx, dy);//(nuLONG)(nuSqrt(dx*dx+dy*dy));
        ddis = 0;
        while( discopy != 0 && lenLeft+dis >= nDashLen )
        {
            tmpDis  = nDashLen - lenLeft;
            ddis += tmpDis;
            tmpPt.x = dx*ddis/discopy + pPtTmp[0].x;
            tmpPt.y = dy*ddis/discopy + pPtTmp[0].y;
            if( bDrawOrSkip )
            {
                pPt[ptIdx] = tmpPt;
                ++ptIdx;
                CvsPolyline(pCvs, pPt, ptIdx);
            }
            pPt[0]		= tmpPt;
            ptIdx		= 1;
            bDrawOrSkip = !bDrawOrSkip;
            lenLeft		= 0;
            dis			-= tmpDis;
        }
        lenLeft += dis;
    }
    return nuTRUE;
}

nuBOOL CCanvasMgrZK::CvsPolyline3D(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount)
{
    CheckCanvas(&pCvs);
    nuWORD nColor565 = nuRGB24TO16_565(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
    CGdiMethodZK::nuBMP565Poly3DLineZ(&pCvs->bitmap, m_Get3DThickFuc, pPt, nCount, pCvs->pen.nWidth, nColor565);
    return nuTRUE;
}

nuBOOL CCanvasMgrZK::CvsPolygonBMP(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuBOOL bHasPenEdge)
{
    CheckCanvas(&pCvs);
    nuCOLORREF realPColor = 0;
    if (bHasPenEdge && pCvs->pen.nStyle == NURO_PS_SOLID)
    {
        realPColor = nuRGB(pCvs->pen.nRed, pCvs->pen.nGreen, pCvs->pen.nBlue);
        CGdiMethodZK::nuBMPFillGon_BMP(&pCvs->bitmap, pPt, nCount, pBmp, &realPColor, pCvs->drawMode);
        if (pCvs->pen.nWidth>1)
        {
            CvsPolyline(pCvs, pPt, nCount);
            if (pPt[0].x != pPt[nCount-1].x || pPt[0].y != pPt[nCount-1].y)
            {
                NUROPOINT tmpPoint[2];
                tmpPoint[0] = pPt[0];
                tmpPoint[1] = pPt[nCount-1];
                CvsPolyline(pCvs, tmpPoint, 2);
            }
        }
    }
    else
    {
        CGdiMethodZK::nuBMPFillGon_BMP(&pCvs->bitmap, pPt, nCount, pBmp, nuNULL, pCvs->drawMode);
    }
    return nuTRUE;
}

nuBOOL CCanvasMgrZK::CvsDrawBMP(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                , NURO_BMP* bitmap, nuINT nSrcX, nuINT nSrcY)
{
    CheckCanvas(&canvas);
    return CGdiMethodZK::nuGMCopyBmp(&canvas->bitmap, nDesX, nDesY, nWidth, nHeight
         , bitmap, nSrcX, nSrcY, canvas->drawMode);
}

nuBOOL CCanvasMgrZK::CvsDrawPNG2(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY)
{
	nuBOOL b_Return;
	b_Return=nuFALSE;
    CheckCanvas(&canvas);
	NURORECT m_rect;
	b_Return=nuTRUE;
	m_pngctrl.SetMemDCColorType(MEMDCCOLORTYPE_16_565);
	m_rect.left		=	0;
	m_rect.right	=	canvas->bitmap.bmpWidth;
	m_rect.top		=	0;
	m_rect.bottom	=	canvas->bitmap.bmpHeight;
	m_pngctrl.Draw_PNG_EX2(p_png,(nuBYTE*)canvas->bitmap.pBmpBuff,m_rect,nSrcX,nSrcY,PNG_DRAWMODE_LT);
    return b_Return;
}


nuBOOL CCanvasMgrZK::CvsDrawPNG(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                                , nuINT PngID, nuINT nSrcX, nuINT nSrcY)
{
	nuBOOL b_Return;
	b_Return=nuFALSE;
    CheckCanvas(&canvas);
	
	nuTCHAR PngFileName[255];
	nuINT pngid=-1;
	NURORECT m_rect;
	
#ifndef ANDROID
#ifdef WINCE
	if(PngID<10)
	{
		wsprintf(PngFileName,L"D:\\NuroWork\\Yulon\\LCMM\\app\\NuroWnds\\Media\\PIC\\PNG\\POI_000%ld.png",PngID);
	}
	else
	{
		wsprintf(PngFileName,L"D:\\NuroWork\\Yulon\\LCMM\\app\\NuroWnds\\Media\\PIC\\PNG\\POI_00%ld.png",PngID);
	}
#endif
   #endif
	//#endif
	nuFILE *pfile;
	if(!m_pngctrl.check_if_png(PngFileName,&pfile))
	{
		return b_Return;
	}
	b_Return=nuTRUE;
	m_pngctrl.load_png(PngFileName,pngid,-1);

	PNGCTRLSTRU *ppng=NULL;

	m_pngctrl.Get_PngData_idx(pngid,ppng);


	m_pngctrl.SetMemDCColorType(MEMDCCOLORTYPE_16_565);
	m_rect.left		=	0;
	m_rect.right	=	canvas->bitmap.bmpWidth;
	m_rect.top		=	0;
	m_rect.bottom	=	canvas->bitmap.bmpHeight;
	m_pngctrl.Draw_PNG_EX(pngid,(nuBYTE*)canvas->bitmap.pBmpBuff,m_rect,nSrcX,nSrcY,PNG_DRAWMODE_LT);
	m_pngctrl.Clean_Png(pngid,false);
    return b_Return;
}

nuBOOL CCanvasMgrZK::CvsDrawBmp2(NURO_CANVAS* canvas,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH)
{
	CheckCanvas(&canvas);
	CGdiMethodZK::nuBMPZoom_Bilinear2(&canvas->bitmap, nDesX, nDesY, nWidth, nHeight
		, pBmp, nSrcX, nSrcY, nSrcW, nSrcH, canvas->drawMode);
	return nuTRUE;
}
nuBOOL CCanvasMgrZK::CvsZoomBmp(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
								const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH)
{
	CGdiMethodZK::nuBMPZoom_Bilinear2(pDesBmp, nDesX, nDesY, nWidth, nHeight
		, pSrcBmp, nSrcX, nSrcY, nSrcW, nSrcH, NURO_BMP_TYPE_COPY);
	return nuTRUE;
}
nuBOOL CCanvasMgrZK::CvsPaintBMP(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT width, nuINT height, NURO_BMP* bitmap, nuINT nSrcX, nuINT nSrcY)
{
    CheckCanvas(&canvas);
    return CGdiMethodZK::nuGMPaintBmp(&canvas->bitmap, nDesX, nDesY, width, height
        , bitmap, nSrcX, nSrcY, canvas->drawMode);
}
nuBOOL CCanvasMgrZK::CvsSaveBMP(NURO_BMP* bitmap,nuINT nDesX, nuINT nDesY, nuINT width, nuINT height, const NURO_CANVAS* canvas, nuINT nSrcX, nuINT nSrcY)
{
    CheckCanvas((NURO_CANVAS**)(&canvas));
    return CGdiMethodZK::nuGMCopyBmp(bitmap, nDesX, nDesY, width, height
        , &canvas->bitmap, nSrcX, nSrcY, NURO_BMP_TYPE_COPY);
}

nuVOID CCanvasMgrZK::LockScreen()
{
    while( m_bFlush )
    {
        nuSleep(25);
    }
    m_bFlush = nuTRUE;
}

nuVOID CCanvasMgrZK::UnlockScreen()
{
    m_bFlush = nuFALSE;
}

nuBOOL CCanvasMgrZK::CvsFlush( nuINT nScreenX, 
                              nuINT nScreenY, 
                              nuINT nWidth, 
                              nuINT nHeight, 
                              PNURO_CANVAS pSrcCvs, 
                              nuINT nSrcX, 
                              nuINT nSrcY )
{    
    CCanvasMgrAutoLock lock(*this);
    if( nuNULL == pSrcCvs )
    {
        pSrcCvs = &m_cvsGdi;
    }
    if (!nWidth || !nHeight)
    {
        nWidth = m_Screen.nScreenWidth;
        nHeight = m_Screen.nScreenHeight;
    }
    //if( NURO_CANVAS_TYPE_SINGLE == pSrcCvs->cvsType )
    {
        return nuCanvasFlush( &m_Screen, 
            nScreenX+m_Screen.nScreenLeft,
            nScreenY+m_Screen.nScreenTop,
            nWidth,
            nHeight,
            pSrcCvs,
            nSrcX,
            nSrcY );
    }
    nuINT nCvsIdx = PushCanvas(pSrcCvs);
    if( -1 == nCvsIdx )
    {
        return nuFALSE;
    }
    while( nCvsIdx < m_nCvsCount )
    {
        CGdiMethodZK::nuGMCopyBmp( &m_cvsBuffer.bitmap,
            m_pCvsList[nCvsIdx].pNuroCvs->cvsLeft,
            m_pCvsList[nCvsIdx].pNuroCvs->cvsTop,
            m_pCvsList[nCvsIdx].pNuroCvs->bitmap.bmpWidth,
            m_pCvsList[nCvsIdx].pNuroCvs->bitmap.bmpHeight,
            &m_pCvsList[nCvsIdx].pNuroCvs->bitmap,
            0,
            0,
            NURO_BMP_TYPE_MIXX);
        ++nCvsIdx;
    }
    return nuCanvasFlush( &m_Screen, 
        nScreenX,
        nScreenY,
        nWidth,
        nHeight,
        &m_cvsBuffer,
        nSrcX,
        nSrcY );
}

nuINT CCanvasMgrZK::PushCanvas(PNURO_CANVAS pCvs)
{
    if( nuNULL == pCvs )
    {
        return -1;
    }
    for(nuWORD i = 0; i < m_nCvsCount; ++i )
    {
        if( pCvs == m_pCvsList[i].pNuroCvs )
        {
            return i;
        }
    }
    if( _GDI_CANVAS_MAX_COUNT <= m_nCvsCount )
    {
        return -1;
    }
    m_pCvsList[m_nCvsCount].pNuroCvs = pCvs;
    ++ m_nCvsCount;
    return m_nCvsCount - 1;
}

nuINT CCanvasMgrZK::PopCanvas(const PNURO_CANVAS pCvs)
{
    if( nuNULL == pCvs || 0 == m_nCvsCount )
    {
        return 0;
    }
    nuUINT nFind = 0;
    for(nuWORD i = 0; i < m_nCvsCount; ++i)
    {
        if( nFind )
        {
            m_pCvsList[ i - 1 ] = m_pCvsList[i];
        }
        else
        {
            if( pCvs == m_pCvsList[i].pNuroCvs )
            {
                nFind = 1;
            }
        }
    }
    --m_nCvsCount;
    return nFind;
}

nuBOOL CCanvasMgrZK::CvsBmpGradient(NURO_CANVAS* canvas, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
									const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode)
{
	CheckCanvas(&canvas);
	if( NULL == pBmp || 
		NULL == canvas->bitmap.pBmpBuff ||
		nWidth < 0 ||
		nHeight < 0 )
	{
		return nuFALSE;
	}
	nuWORD* pDesClr = (nuWORD*)canvas->bitmap.pBmpBuff;
	nuWORD*	pSrcClr = (nuWORD*)pBmp->pBmpBuff;
	nuWORD* pDesC;
	nuWORD* pSrcC;
	int nDLine, nDIdx, nSLine, nSIdx, k;
	for(int i = 0; i < nHeight; ++i)
	{
		nDLine = nDesY + i;
		nSLine = nSrcY + i;
		if( nDLine < 0 || nSLine < 0  || nDLine >= canvas->bitmap.bmpHeight ||
			nSLine >= pBmp->bmpHeight )
		{
			continue;
		}
		k = nHeight - i;
		pDesC = pDesClr + nDLine * canvas->bitmap.bmpWidth;
		pSrcC = pSrcClr + nSLine * pBmp->bmpWidth;
		for(int j = 0; j < nWidth; ++j)
		{
			nDIdx = j + nDesX;
			nSIdx = j + nSrcX;
			if( nDIdx < 0 || nSIdx < 0 || nDIdx >= canvas->bitmap.bmpWidth )
			{
				continue;
			}
			if( nSIdx >= pBmp->bmpWidth )
			{
				nSIdx -= pBmp->bmpWidth;
			}
			nuBYTE nR = (nu565RVALUE(pDesC[nDIdx]) * i + nu565RVALUE(pSrcC[nSIdx]) * k) / nHeight;
			nuBYTE nG = (nu565GVALUE(pDesC[nDIdx]) * i + nu565GVALUE(pSrcC[nSIdx]) * k) / nHeight;
			nuBYTE nB = (nu565BVALUE(pDesC[nDIdx]) * i + nu565BVALUE(pSrcC[nSIdx]) * k) / nHeight;
			pDesC[nDIdx] = nuCOLOR24TO16_565(nuRGB(nR, nG, nB));
		}
	}
}
nuBOOL CCanvasMgrZK::CvsShowStartupBmp()
{
	/* show startup bmp */

	NURO_BMP spbmp;
	nuMemset(&spbmp,NULL,sizeof(NURO_BMP));
	nuTCHAR path[256] = {0};
	nuGetModulePath(NULL,path,256);
	nuTcscat(path,STARTUP_BMP_FILE_PATH);
	if(!CvsLoadNuroBMP(&spbmp,path))
	{
		return nuFALSE;
	}
	spbmp.bmpType = NURO_BMP_TYPE_COPY;
	CvsDrawBMP(NULL,0,0,spbmp.bmpWidth,spbmp.bmpHeight,&spbmp,0,0);
	CvsFlush(0,0,spbmp.bmpWidth,spbmp.bmpHeight,NULL,0,0);
	CvsDelNuroBMP(&spbmp);
	return nuTRUE;
}
nuBOOL CCanvasMgrZK::CvsLoadNuroBMP(PNURO_BMP pNuroBmp, const nuTCHAR *tsFile)
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
nuVOID CCanvasMgrZK::CvsDelNuroBMP(PNURO_BMP pNuroBmp)
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

