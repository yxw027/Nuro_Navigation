// nurofontengine.cpp: implementation of the nuroFontEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "nurofontengine.h"
#include "NuroConstDefined.h"

nuroFontEngine::nuroFontEngine()
{
    _library = 0;
    _face = 0;
    _height= 0;
    _baseheight= 0;
//    _setW= 0;
    _setH= 0;
    _angle=0;
    
    fontbuffer = 0;
    usedHead = 0;
    usedEnd = 0;
    freeHead = 0;
}

nuroFontEngine::~nuroFontEngine()
{
    Destroy();
}
nuBOOL nuroFontEngine::Init()
{
    if (FT_Init_FreeType(&_library))
    {
        return nuFALSE;
    }
    if (!FontBuffer_Init())
    {
        return nuFALSE;
    }    
    memset(&_params, 0, sizeof(_params));
    _params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    _params.gray_spans = nuroFTRenderBuffer::RasterCallback;    
    _params.user = &_ftRenderBuffer;
    return nuTRUE;
}
void nuroFontEngine::Destroy()
{
    FontBuffer_Destroy();
    if (_face)
    {
        FT_Done_Face(_face);
        _face = 0;
    }
    if (_library)
    {
        FT_Done_FreeType(_library);
        _library = 0;
    }
}

nuBOOL nuroFontEngine::LoadFontInfo(const nuTCHAR* name)
{
	nuFILE* fp = nuTfopen(name, NURO_FS_RB);
	if (!fp)
	{
		return nuFALSE;
	}
	if (nuro_FT_Open_Face(_library, 0, &_face, fp))
	{
        nuFclose(fp);
		return nuFALSE;
	}
    return nuTRUE;
}
nuBOOL nuroFontEngine::SetSize(nuroU32 w, nuroU32 h, nuINT angle)
{
    if (h>FontBufferData_FT_SIZELIMIT)
    {
        h = FontBufferData_FT_SIZELIMIT;
    }
    nuroU32 h0 = h;
    //if (_setW != w || _setH != h)
    if (_setH != h)
    {
        while (h>0)
        {
            //if (FT_Set_Pixel_Sizes(_face, w, h))
            if (FT_Set_Pixel_Sizes(_face, 0, h))
            {
                return nuFALSE;
            }
            _height = (nuroU16)_face->size->metrics.height>>6;
            if (_height>h0)
            {
                --h;
            }
            else
            {
                _baseheight = (nuroU16)_face->size->metrics.ascender>>6;
//                _setW = w;
                _setH = h0;
                if (_angle != angle)
                {
                    FT_Matrix    matrix;
                    matrix.xx = (FT_Fixed)( nuCos( angle ) * 0x10000L );
                    matrix.xy = (FT_Fixed)( -nuSin( angle) * 0x10000L );
                    matrix.yx = (FT_Fixed)( nuSin( angle) * 0x10000L );
                    matrix.yy = (FT_Fixed)( nuCos( angle) * 0x10000L );
                    FT_Set_Transform( _face, &matrix, 0);
                    _angle = angle;
                }
                return nuTRUE;
            }
        }
        return nuFALSE;
    }
    if (_angle != angle)
    {
        FT_Matrix    matrix;
        matrix.xx = (FT_Fixed)( nuCos( angle ) * 0x10000L );
        matrix.xy = (FT_Fixed)( -nuSin( angle) * 0x10000L );
        matrix.yx = (FT_Fixed)( nuSin( angle) * 0x10000L );
        matrix.yy = (FT_Fixed)( nuCos( angle) * 0x10000L );
        FT_Set_Transform( _face, &matrix, 0);
        _angle = angle;
    }
    return nuTRUE;
}

nuBOOL nuroFontEngine::nuFontRectCountEx(nuWCHAR* wStr, nuINT nCount, nuUINT nUniFontType, nuINT* pW, nuINT* pH, nuBOOL fix)
{
    *pW = 0;
    *pH = 0;
    if (!SetSize(0, nUniFontType, 0))
    {
        return nuFALSE;
    }
    *pH = _height;
    if (nCount<=0)
    {
        nCount = nuWcslen(wStr);
    }
    nuroFontInfo info = {0};
    while (nCount)
    {
        --nCount;
        if (FontBuffer_LoadFontInfo(wStr[nCount], nUniFontType, 0, &info, nuFALSE))
        {
            *pW += info.stepx;
        }
    }
    //*pW += 1;
    return nuTRUE;
}
nuroS16 nuroFontEngine::DrawUniFont(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode,
                                   nuINT r, 
                                   nuINT g, 
                                   nuINT b, 
                                   nuUINT nUniFontType, nuINT angle)
{
    if(!pBmp || !pRect || !nUniCode)
    {
        return 0;
    }
    nuroFontInfo info = {0};
    if (nUniCode == ' ')//space
    {        
        if (!FontBuffer_LoadFontInfo(nUniCode, nUniFontType, angle, &info, nuFALSE))
        {
            return 0;
        }
        pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
        pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
        return info.starty;
    }
    if (!FontBuffer_LoadFontInfo(nUniCode, nUniFontType, angle, &info, nuTRUE))
    {
        return 0;
    }
    if (!info.bufferwidth || !info.bufferheight)
    {
        pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
        pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
        return info.stepy;
    }
    nuINT k, j, nX, nY;
    nuINT t, realR, realG, realB;
    nY = pRect->top+info.starty;

    nuINT hEnd = info.bufferheight;
    if ((pRect->top+hEnd)>pRect->bottom)
    {
        hEnd = pRect->bottom - pRect->top;
    }
    nuINT wEnd = info.bufferwidth;
    if ((pRect->left+wEnd)>pRect->right)
    {
        wEnd = pRect->right - pRect->left;
    }
    nuWORD oldColor;
    nuWORD* oldColorPtr = 0;
    for(k=0; k<hEnd; ++k)
    {
        nX = pRect->left+info.startx;
        for(j=0; j<wEnd; ++j)
        {
            t = info.buffer[k*info.bufferbytesperline+j];
            if (!(t^0xff))
            {
                CGdiMethodZK::nuBMPSetPixel565(pBmp, nX, nY, (r<<11) | (g<<5) | (b), NURO_BMP_TYPE_COPY);
            }
            else if(t)
            {
                oldColorPtr = CGdiMethodZK::nuBMPGetPixel16Ptr(pBmp, nX, nY);
                if (oldColorPtr)
                {
                    oldColor = *oldColorPtr;
                    if (oldColor == pBmp->bmpTransfColor)
                    {
                        realR = (r*t)>>8;
                        realG = (g*t)>>8;
                        realB = (b*t)>>8;
                    }
                    else
                    {
                        realR = oldColor >> 11;
                        realG = (oldColor >> 5) & 0x3f;
                        realB = oldColor & 0x1f;
                        realR += ((r-realR)*t)>>8;
                        realG += ((g-realG)*t)>>8;
                        realB += ((b-realB)*t)>>8;
                    }
                    *oldColorPtr = (realR<<11) | (realG<<5) | (realB);
                }
            }
            ++nX;
        }
        ++nY;
    }
    pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
    pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
    return info.stepy;
}
nuroS16 nuroFontEngine::DrawUniFontEdge(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode,
                        nuUINT nUniFontType, nuINT angle, nuINT r, nuINT g, nuINT b)
{
    if(!pBmp || !pRect || !nUniCode)
    {
        return 0;
    }
    nuroFontInfo info = {0};
    if (nUniCode == ' ')//space
    {        
        if (!FontBuffer_LoadFontInfo(nUniCode, nUniFontType, angle, &info, nuFALSE))
        {
            return 0;
        }
        pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
        pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
        return info.starty;
    }
    if (!FontBuffer_LoadEdgeInfo(nUniCode, nUniFontType, angle, &info))
    {
        return 0;
    }
    if (!info.bufferwidth || !info.bufferheight)
    {
        pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
        pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
        return info.stepy;
    }
    nuINT k, j, nX, nY;
    nuINT t, realR, realG, realB;
    nY = pRect->top+info.starty;//-1;
    nuINT hEnd = info.bufferheight;
    if ((pRect->top+hEnd)>pRect->bottom)
    {
        hEnd = pRect->bottom - pRect->top;
    }
    nuINT wEnd = info.bufferwidth;
    if ((pRect->left+wEnd)>pRect->right)
    {
        wEnd = pRect->right - pRect->left;
    }
    nuWORD oldColor;
    nuWORD* oldColorPtr = 0;
    for(k=0; k<hEnd; ++k)
    {
        nX = pRect->left+info.startx;
        for(j=0; j<wEnd; ++j)
        {
            t = info.buffer[k*info.bufferbytesperline+j];
            if (!(t^0xff))
            {
                CGdiMethodZK::nuBMPSetPixel565(pBmp, nX, nY, (r<<11) | (g<<5) | (b), NURO_BMP_TYPE_COPY);
            }
            else if(t)
            {
                oldColorPtr = CGdiMethodZK::nuBMPGetPixel16Ptr(pBmp, nX, nY);
                if (oldColorPtr)
                {
                    oldColor = *oldColorPtr;
                    if (oldColor == pBmp->bmpTransfColor)
                    {
                        realR = (r*t)>>8;
                        realG = (g*t)>>8;
                        realB = (b*t)>>8;
                    }
                    else
                    {
                        realR = oldColor >> 11;
                        realG = (oldColor >> 5) & 0x3f;
                        realB = oldColor & 0x1f;
                        realR += ((r-realR)*t)>>8;
                        realG += ((g-realG)*t)>>8;
                        realB += ((b-realB)*t)>>8;
                    }
                    *oldColorPtr = (realR<<11) | (realG<<5) | (realB);
                }
            }
            ++nX;
        }
        ++nY;
    }
    pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
    pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
    return info.stepy;
}

nuBOOL nuroFontEngine::DrawTextOut(PNURO_BMP pBmp,
                                 nuINT X, 
                                 nuINT Y,
                                 nuWCHAR *wStr, 
                                 nuUINT nCount, 
                                 nuUINT nUniFontType,
                                 nuCOLORREF color,
                                 nuBOOL bUseEdgeColor,
                                 nuCOLORREF edgeColor,
                                 nuINT bkMode,
                                 nuCOLORREF bkColor,
                                 nuINT angle)
{
    nuUINT i = 0;
    nuINT startX = X;
    nuINT startY = Y;
    nuINT FontR, FontG, FontB;  
    if (!SetSize(0, nUniFontType, angle))
    {
        return nuFALSE;
    }
//     if(bkMode != NURO_TRANSPARENT)
//     {
//         nuINT w=0, h=0;
//         nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);
//         nuBMPFillRect(pBmp, X, Y, w, h, 0xff, NURO_BMP_TYPE_COPY);
//     }
    NURORECT rect;
    if (bUseEdgeColor)
    {
        FontR=nuGetRValue(edgeColor)>>3;
        FontG=nuGetGValue(edgeColor)>>2;
        FontB=nuGetBValue(edgeColor)>>3;        
        for(i=0; (i<nCount)&&(startX<pBmp->bmpWidth); ++i)
        {
            if (!wStr[i])
            {
                break;
            }
            rect.left = startX;
            rect.top = startY;
            rect.right	= pBmp->bmpWidth;
            rect.bottom	= pBmp->bmpHeight;
            startY -= DrawUniFontEdge(pBmp
                , &rect
                , wStr[i]
                , nUniFontType, angle
                , FontR
                , FontG
                , FontB);
            startX = rect.right;
        }
    }
    startX = X;
    startY = Y;
    FontR=nuGetRValue(color)>>3;
    FontG=nuGetGValue(color)>>2;
    FontB=nuGetBValue(color)>>3;
    for(i=0; (i<nCount)&&(startX<pBmp->bmpWidth); ++i)
    {
        if (!wStr[i])
        {
            break;
        }
        rect.left = startX;
        rect.top = startY;
        rect.right	= pBmp->bmpWidth;
        rect.bottom	= pBmp->bmpHeight;
        startY -= DrawUniFont(pBmp
            , &rect
            , wStr[i]
            , FontR
            , FontG
            , FontB
            , nUniFontType, angle);
        startX = rect.right;
    }
    return nuTRUE;
}
nuINT	nuroFontEngine::DrawTextOut2(PNURO_BMP pBmp,
                                 PNURORECT pRect,
                                 nuUINT nFormat, 
                                 nuWCHAR* wStr, 
                                 nuINT nCount,  
                                 nuUINT nUniFontType,
                                 nuCOLORREF color,
                                 nuBOOL bUseEdgeColor, 
                                 nuCOLORREF edgeColor,
                                 nuINT bkMode,
                                 nuCOLORREF bkColor)
{	
    nuINT w=0, h=0;
    nuINT i=0;
    if(nCount<0)
    {
        nCount = nuWcslen(wStr);
    }
    if (!SetSize(0, nUniFontType, 0))
    {
        return nuFALSE;
    }
    
	nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);

	// ?p???i?e?X???r?? Modified by Damon 20101024
	nuBOOL bIsDrawPoint = nuFALSE;
	if (w/*-nCount*/ > pRect->right-pRect->left) // - nCount ?????h????
	{
		nuINT wPoint=0, hPoint=0;
		bIsDrawPoint = nuTRUE;
		nuFontRectCountEx((nuWCHAR *)L"...", 3, nUniFontType, &wPoint, &hPoint);

		while(w > ((pRect->right-pRect->left)-wPoint) &&
			  nCount > 0)
		{
			nCount--;
			nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);
		}
	}
	// --------------

    if(nFormat&NURO_DT_CALCRECT)
    {
        pRect->right = pRect->left + w;
        pRect->bottom = pRect->top + h;
        return h;
    }
    nuINT realW = pRect->right- pRect->left;
    nuINT realH = pRect->bottom - pRect->top;
    nuINT startX = pRect->left;
    nuINT startY = pRect->top;
    if(realW > w)
    {	
        if(nFormat&NURO_DT_RIGHT)
        {
            startX = pRect->right - w;
        }
        else if(nFormat&NURO_DT_CENTER)
        {
            startX += (realW-w)>>1;
        }
        else
        {}
        realW = w;
    }
    if(realH > h)
    {
        if(nFormat&NURO_DT_BOTTOM)
        {
            startY = pRect->bottom - h;
        }
        else if(nFormat&NURO_DT_VCENTER)
        {
            startY += (realH-h)>>1;
        }
        else
        {
			startY -= 7;
		}
        realH = h;
    }
    if(bkMode != NURO_TRANSPARENT)
    {
        CGdiMethodZK::nuBMPFillRect(pBmp, startX, startY, realW, realH
            , bkColor, NURO_BMP_TYPE_COPY);
    }
    NURORECT rect;
    nuINT FontR, FontG, FontB;
    nuINT realstartX = startX;
    nuINT realstartY = startY;
    if (bUseEdgeColor)
    {
        FontR=nuGetRValue(edgeColor)>>3;
        FontG=nuGetGValue(edgeColor)>>2;
        FontB=nuGetBValue(edgeColor)>>3;
        for(i=0; (i<nCount)&&(startX<pBmp->bmpWidth); ++i)
        {
            if (!wStr[i])
            {
                break;
            }
            rect.left = realstartX;
            rect.top = realstartY;
            rect.right	= pBmp->bmpWidth;
            rect.bottom	= pBmp->bmpHeight;

			realstartY -= DrawUniFontEdge(pBmp, 
										  &rect,
										  wStr[i],
										  nUniFontType, 
										  0,
										  FontR,
										  FontG,
                    				      FontB);
			realstartX = rect.right;
        }
		// Added by Damon For ect....
		if (nuTRUE == bIsDrawPoint)
		{
			for(i=0; i<3; ++i)
			{
				rect.left = realstartX;
				rect.top = realstartY;
				rect.right	= pBmp->bmpWidth;
				rect.bottom	= pBmp->bmpHeight;

				realstartY -= DrawUniFontEdge(pBmp, 
											  &rect,
											  '.',
										      nUniFontType, 
											  0,
											  FontR,
											  FontG,
											  FontB);
				realstartX = rect.right;
			}
		}
		// -------------------------------------
    }
    FontR=nuGetRValue(color)>>3;
    FontG=nuGetGValue(color)>>2;
    FontB=nuGetBValue(color)>>3;
    realstartX = startX;
    realstartY = startY;
    for(i=0; i<nCount; ++i)
    {
        if (!wStr[i])
        {
            continue;
        }
        rect.left = realstartX;
        rect.top = realstartY;
        rect.right = pRect->right;
        rect.bottom = pRect->bottom;
        if(rect.left>=pBmp->bmpWidth || rect.top>=pBmp->bmpHeight)
        {
            break;
        }
		realstartY -= DrawUniFont(pBmp,
								  &rect,

								  wStr[i],
								  FontR,
								  FontG,
								  FontB,
								  nUniFontType,
								  0);
		realstartX = rect.right;
    }
	// Added by Damon For ect....
	if (nuTRUE == bIsDrawPoint)
	{
		for(i=0; i<3; ++i)
		{
			rect.left = realstartX;
			rect.top = realstartY;
			rect.right	= pBmp->bmpWidth;
			rect.bottom	= pBmp->bmpHeight;

			realstartY -= DrawUniFont(pBmp,
									  &rect,
								      '.',
									  FontR,
									  FontG,
									  FontB,
									  nUniFontType,
									  0);
			realstartX = rect.right;
		}
	}
	// ------------------------------------
    return realH;
}

nuINT	nuroFontEngine::DrawTextOutNew(PNURO_BMP pBmp, PNURORECT pRect, nuUINT nFormat, nuWCHAR* wStr,  nuINT nCount, nuUINT nUniFontType,
				                   nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor)
{	
	nuINT w=0, h=0;
	nuINT i=0;
	if(nCount<0)
	{
		nCount = nuWcslen(wStr);
	}
	if (!SetSize(0, nUniFontType, 0))
	{
		return nuFALSE;
	}

	// ?p?e?Ҧ???ø?X?????r?d??
	nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);
    // ------------------------

	nuINT nlineCount = (pRect->bottom - pRect->top) / (h+1); // ?p???i???X????
	//add by chang 
	if (nlineCount == 0)
	{
		nlineCount = 1;
	}

	nuUINT   nNowLine      = 0;
	nuUINT   nOffset       = 0;
	nuUINT   nNowTextCount = 0;
	nuBOOL     bNeedNextLine = nuTRUE;

	while(nNowLine < nlineCount && bNeedNextLine)
	{
		nNowTextCount = nCount - nOffset;
		for (nuUINT n = nOffset+1; n<=nCount && nNowLine<(nlineCount-1); ++n)
		{   // ?̫??@?? ???ݭp??
			nuFontRectCountEx(wStr+nOffset, n-nOffset, nUniFontType, &w, &h);

			if (w > (pRect->right-pRect->left))
			{   
				nNowTextCount = n-nOffset-1;
				break;
			}
		}

		if (nOffset+nNowTextCount >= nCount)
		{
			bNeedNextLine = nuFALSE;
		}
		nOffset += nNowTextCount;
		nNowLine++;
	}
	// --------------
	if (nNowLine == 0)
	{
		nNowLine = 1;
	}

	nuINT nRectH     = (pRect->bottom-pRect->top) / nNowLine; // ?p?⥭?�X?Ӫ?????

	nNowLine      = 0;
	nOffset       = 0;
	nNowTextCount = 0;
	bNeedNextLine = nuTRUE;

	NURORECT rectNowText   = {0};

	while(nNowLine < nlineCount && bNeedNextLine)
	{
		rectNowText.left   = pRect->left;
		rectNowText.top    = pRect->top+nNowLine*nRectH;
		rectNowText.right  = pRect->right;
		rectNowText.bottom = rectNowText.top+nRectH;

		nNowTextCount = nCount - nOffset;
		for (nuUINT n = nOffset+1; n<=nCount && nNowLine<(nlineCount-1); ++n)
		{   // ?̫??@?? ???ݭp??
			nuFontRectCountEx(wStr+nOffset, n-nOffset, nUniFontType, &w, &h);

			if (w > (pRect->right-pRect->left))
			{   
				nNowTextCount = n-nOffset-1;
				break;
			}
		}
		
		if (nOffset+nNowTextCount >= nCount)
		{
			bNeedNextLine = nuFALSE;
		}

		DrawTextOut2(pBmp, &rectNowText, nFormat, wStr+nOffset, nNowTextCount, nUniFontType, color, bUseEdgeColor, edgeColor, bkMode, bkColor);
		nOffset += nNowTextCount;
		nNowLine++;
	}
	// --------------

	return nNowLine*(h+1);
}

nuBOOL nuroFontEngine::FontBuffer_Init()
{
    if (!fontbuffer)
    {
        fontbuffer = (FontBufferData_FT*) nuMalloc(sizeof(FontBufferData_FT) * C_FONTBUFFER_SIZE);
        if (fontbuffer)
        {
            nuMemset(fontbuffer, 0, sizeof(FontBufferData_FT) * C_FONTBUFFER_SIZE);
            freeHead = fontbuffer;
            nuINT tag = C_FONTBUFFER_SIZE-1;
            for(nuINT i=0; i<tag; ++i)
            {
                fontbuffer[i].next = &fontbuffer[i+1];
            }
            fontbuffer[tag].next = 0;
            return nuTRUE;
        }
        FontBuffer_Destroy();
        return nuFALSE;
    }
    return nuTRUE;
}
void nuroFontEngine::FontBuffer_Destroy()
{
    if(fontbuffer)
    {
        FontBufferData_FT* tagbuffer = usedHead;
        while (tagbuffer)
        {
            FT_Done_Glyph(tagbuffer->glyph);
            tagbuffer = tagbuffer->next;
        }
        nuFree(fontbuffer);
        fontbuffer = 0;
        usedHead = 0;
        usedEnd = 0;
        freeHead = 0;
    }
}
nuBOOL nuroFontEngine::FontBuffer_GetBuffer(nuUINT word, nuUINT type, nuINT angle, FontBufferData_FT** fbd)
{
    if(!usedHead)
    {
        *fbd = freeHead;
        return nuFALSE;
    }
    FontBufferData_FT* tagbuffer = usedEnd;
    while (tagbuffer)
    {
        if(tagbuffer->unicode == word && tagbuffer->type == type && tagbuffer->angle == angle)
        {
            *fbd = tagbuffer;
            if(usedHead != usedEnd && tagbuffer != usedEnd)
            {
                if(usedHead == tagbuffer)
                {
                    usedHead = usedHead->next;
                    usedHead->pre = 0;
                }
                else
                {
                    tagbuffer->pre->next = tagbuffer->next;
                    tagbuffer->next->pre = tagbuffer->pre;
                }
                usedEnd->next = tagbuffer;
                tagbuffer->pre = usedEnd;
                usedEnd = tagbuffer;
                usedEnd->next = 0;
            }
            return nuTRUE;
        }
        tagbuffer = tagbuffer->pre;
    }
    if (!freeHead)
    {
        freeHead = usedHead;
        usedHead = usedHead->next;
        usedHead->pre = 0;

        freeHead->next = 0;
        //nuMemset(freeHead, 0, sizeof(FontBufferData_FT));
    }
    *fbd = freeHead;
    return nuFALSE;
}
void nuroFontEngine::FontBuffer_Active(FontBufferData_FT* fbd)
{
    if(fbd != usedEnd)
    {
		if( NULL != freeHead )
		{
	        freeHead = freeHead->next;
		    if(!usedEnd)
		    {
			    usedHead = usedEnd = fbd;
				fbd->next = 0;
				fbd->pre = 0;
			}
			else
			{
				usedEnd->next = fbd;
				fbd->pre = usedEnd;
				usedEnd = fbd;
				usedEnd->next = 0;
			}
		}
    }
}

nuBOOL nuroFontEngine::FontBuffer_LoadFontInfo(nuUINT word, nuUINT nUniFontType, nuINT angle, nuroFontInfo* info, nuBOOL render)
{
    FontBufferData_FT* fbd = 0;
    if (!FontBuffer_GetBuffer(word, nUniFontType, angle, &fbd))
    {
        fbd->unicode = word;
        fbd->type = nUniFontType;
        fbd->angle = angle;
        LoadGlyph(word, fbd);
    }
    info->height = _height;
    info->stepx = fbd->stepx;
    info->stepy = fbd->stepy;
    if (render)
    {
         if (!(fbd->render&0x01))
        {
            fbd->render |= 0x01;
            RenderWord(_library, fbd, _baseheight);
        }
        FontBufferData_FT_BufferInfo* wordinfo = &fbd->wordinfo;        
        info->buffer = wordinfo->buffer;
        info->startx = wordinfo->startx;
        info->starty = wordinfo->starty;
        info->bufferwidth = wordinfo->bufferwidth;
        info->bufferheight= wordinfo->bufferheight;
        info->bufferbytesperline = wordinfo->bufferbytesperline;
    }
    FontBuffer_Active(fbd);
    return nuTRUE;
}
nuBOOL nuroFontEngine::FontBuffer_LoadEdgeInfo(nuUINT word, nuUINT nUniFontType, nuINT angle, nuroFontInfo* info)
{
    FontBufferData_FT* fbd = 0;
    if (!FontBuffer_GetBuffer(word, nUniFontType, angle, &fbd))
    {
        fbd->unicode = word;
        fbd->type = nUniFontType;
        fbd->angle = angle;
        LoadGlyph(word, fbd);
    }
    info->height = _height;
    info->stepx = fbd->stepx;
    info->stepy = fbd->stepy;
    if (!(fbd->render&0x02))
    {
        fbd->render |= 0x02;
        RenderEdge(_library, fbd, _baseheight);
    }
    FontBufferData_FT_BufferInfo* edgeinfo = &fbd->edgeinfo;
    info->buffer = edgeinfo->buffer;
    info->startx = edgeinfo->startx;
    info->starty = edgeinfo->starty;
    info->bufferwidth = edgeinfo->bufferwidth;
    info->bufferheight= edgeinfo->bufferheight;
    info->bufferbytesperline = edgeinfo->bufferbytesperline;
    FontBuffer_Active(fbd);
    return nuTRUE;
}
nuBOOL nuroFontEngine::LoadGlyph(nuUINT word, FontBufferData_FT* fbd)
{    
    fbd->render = 0;
    if (fbd->glyph)
    {
        FT_Done_Glyph(fbd->glyph);
        fbd->glyph = 0;
    }
    nuMemset(&fbd->wordinfo, 0, sizeof(FontBufferData_FT_BufferInfo)<<1);
    FT_UInt glyph_index = FT_Get_Char_Index(_face, word);
    if (!glyph_index
        || FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_BITMAP)
        || FT_Get_Glyph(_face->glyph, &fbd->glyph))
    {
        fbd->stepx = 0;
        fbd->stepy = 0;
        fbd->render |= 0x03;
        return nuFALSE;
    }
    fbd->edgestart = 0;
    FT_Glyph_Get_CBox(fbd->glyph, FT_GLYPH_BBOX_PIXELS, &fbd->bbox);
    nuINT w = fbd->bbox.xMax-fbd->bbox.xMin;
    nuINT h = fbd->bbox.yMax-fbd->bbox.yMin;
    if (!w || !h)
    {
        fbd->render |= 0x03;
    }
    else
    {
        fbd->edgestart = (w+1)*(h+1);
        if (fbd->edgestart>FontBufferData_FT_BUFFERSIZE)
        {
            fbd->render |= 0x03;//out of buffer
        }
    }
    fbd->stepx = (nuroS16)(_face->glyph->advance.x >> 6);
    fbd->stepy = (nuroS16)(_face->glyph->advance.y >> 6);
    return nuTRUE;
}
void nuroFontEngine::RenderWord(FT_Library &library, FontBufferData_FT* fbd, nuroU16 baseHeight)
{
    FT_Glyph glyph;
    if (FT_Glyph_Copy(fbd->glyph, &glyph))
    {
        return;
    }
    _ftRenderBuffer.w = fbd->bbox.xMax-fbd->bbox.xMin+1;
    _ftRenderBuffer.h = fbd->bbox.yMax-fbd->bbox.yMin+1;
    FontBufferData_FT_BufferInfo* wordinfo = &fbd->wordinfo;
    _ftRenderBuffer.x0 = -fbd->bbox.xMin;
    _ftRenderBuffer.y0 = fbd->bbox.yMax;
    wordinfo->buffer = fbd->buffer;
    _ftRenderBuffer.buffer = wordinfo->buffer;    
    nuMemset(wordinfo->buffer, 0, _ftRenderBuffer.w*_ftRenderBuffer.h);
    wordinfo->startx = - _ftRenderBuffer.x0;
    wordinfo->starty = baseHeight - _ftRenderBuffer.y0;
    wordinfo->bufferwidth = _ftRenderBuffer.w;
    wordinfo->bufferheight= _ftRenderBuffer.h;
    wordinfo->bufferbytesperline = _ftRenderBuffer.w;
    FT_Outline* o = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
    FT_Outline_Render(library, o, &_params);
    FT_Done_Glyph(glyph);
    return;
}
void nuroFontEngine::RenderEdge(FT_Library &library, FontBufferData_FT* fbd, nuroU16 baseHeight)
{
    FT_Glyph glyph;
    if (FT_Glyph_Copy(fbd->glyph, &glyph))
    {
        return;
    }
    FT_Stroker stroker;
    FT_Stroker_New(library, &stroker);
    FT_Stroker_Set(stroker, 1<<6, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);//?W?[?@?h????
    FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);    
    FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
    _ftRenderBuffer.w = bbox.xMax-bbox.xMin+1;
    _ftRenderBuffer.h = bbox.yMax-bbox.yMin+1;
    FontBufferData_FT_BufferInfo* edgeinfo = &fbd->edgeinfo;
    if ( (FontBufferData_FT_BUFFERSIZE<fbd->edgestart)
        || (_ftRenderBuffer.w * _ftRenderBuffer.h) > (nuINT)(FontBufferData_FT_BUFFERSIZE-fbd->edgestart) )
    {
        FT_Stroker_Done(stroker); 
        FT_Done_Glyph(glyph);
        return;
    }
    _ftRenderBuffer.x0 = -bbox.xMin;
    _ftRenderBuffer.y0 = bbox.yMax;
    edgeinfo->buffer = fbd->buffer+fbd->edgestart;
    _ftRenderBuffer.buffer = edgeinfo->buffer;
    nuMemset(edgeinfo->buffer, 0, _ftRenderBuffer.w*_ftRenderBuffer.h);
    edgeinfo->startx = - _ftRenderBuffer.x0;
    edgeinfo->starty = baseHeight - _ftRenderBuffer.y0;
    edgeinfo->bufferwidth = _ftRenderBuffer.w;
    edgeinfo->bufferheight= _ftRenderBuffer.h;
    edgeinfo->bufferbytesperline = _ftRenderBuffer.w;
    FT_Outline* o = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
    FT_Outline_Render(library, o, &_params);
    FT_Stroker_Done(stroker); 
    FT_Done_Glyph(glyph);
    return;
}
