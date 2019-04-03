
#include "GdiFontZK.h"
#include "NuroConstDefined.h"

#ifdef NURO_FONT_WAY

CGdiFontZK::CGdiFontZK()
{ 
    _fpFont = NULL;
    _pFontInfo = NULL;
    nuMemset(_FTList, 0, sizeof(_FTList));
    _filename[0] = 0;    
    fontbuffer = 0;
    usedHead = 0;
    usedEnd = 0;
    freeHead = 0;
}

CGdiFontZK::~CGdiFontZK()
{
    Destroy();
}

nuBOOL CGdiFontZK::Init()
{
    return FontBuffer_Init();
    //return nuTRUE;
}

nuVOID CGdiFontZK::Destroy()
{
    FontBuffer_Destroy();
    UnloadFontInfo();
}

nuUINT CGdiFontZK::nuFontAutoType(nuUINT fontH)
{
    nuUINT i = 0;
    for (i=0;i<_FTList[255];i++)
    {
        if (fontH<=_FTList[i])
        {
            return _FTList[i];
        }
    }
    return (i==0)?0:_FTList[i-1];
}

nuBOOL CGdiFontZK::nuFontRectCountEx(nuWCHAR* wStr, 
                                     nuINT nCount,  
                                     nuUINT nUniFontSize, 
                                     nuINT* pW, 
                                     nuINT* pH,
                                     nuBOOL fix)
{
    nuUINT nUniFontType = nuFontAutoType(nUniFontSize);
    nuINT i=0, j=0;
    nuBYTE* p = 0;
    *pW=0, *pH=0;
    nuINT nBlockNum = ((NUROFONTFILEINFO*)_pFontInfo)->nBlockNum;
    NUROFONTBLOCKINFO* pInfo = (NUROFONTBLOCKINFO*)(_pFontInfo+sizeof(NUROFONTFILEINFO));
    if (nUniFontSize == nUniFontType || !fix)
    {
        for(i=0; i<nCount; ++i)
        {		
            for(j=0; j<nBlockNum; ++j)
            {
                if(nUniFontType == pInfo[j].nType)
                {
                    if (!(*pH))
                    {
                        *pH = pInfo[j].nY;
                    }
                    if(wStr[i]>=pInfo[j].nStartUni && wStr[i]<(pInfo[j].nStartUni+pInfo[j].nUniCount))
                    {
                        if (pInfo[j].nReserve1)
                        {
                            p = (nuBYTE*)pInfo[j].nReserve2;
                            *pW += p[wStr[i]-pInfo[j].nStartUni];
                        }
                        else
                        {
                            *pW += pInfo[j].nX;
                        }
                        break;
                    }
                }
            }
        }	
        return nuTRUE;
    }
    for( i=0; i<nCount; ++i )
    {
        for(j=0; j<nBlockNum; ++j)
        {
            if(nUniFontType == pInfo[j].nType)
            {
                if (!(*pH))
                {
                    *pH = ( pInfo[j].nY*nUniFontSize+(nUniFontType>>1))/nUniFontType;
                }
                if(wStr[i]>=pInfo[j].nStartUni && wStr[i]<(pInfo[j].nStartUni+pInfo[j].nUniCount))
                {//find it
                    if (pInfo[j].nReserve1)
                    {
                        p = (nuBYTE*)pInfo[j].nReserve2;
                        *pW += ( (p[wStr[i]-pInfo[j].nStartUni])*nUniFontSize+(nUniFontType>>1))/nUniFontType;
                    }
                    else
                    {
                        *pW += ( pInfo[j].nX*nUniFontSize+(nUniFontType>>1))/nUniFontType;
                    }
                    break;
                }
            }
        }
    }	
    return nuTRUE;
}
nuBOOL CGdiFontZK::LoadFontInfo(const nuTCHAR *fontfile)
{
    if (!nuTcscmp(_filename, fontfile))
    {
        return nuTRUE;
    }
    UnloadFontInfo();
    nuUINT len = nuTcslen(fontfile);
    if (len >= C_FONTFILENAME_MAX)
    {
        return nuFALSE;
    }
    _fpFont = nuTfopen(fontfile, NURO_FS_RB);
    if(!_fpFont)
    {
        return nuFALSE;
    }
    
    NUROFONTFILEINFO fileInfo;
    nuFread(&fileInfo, sizeof(fileInfo), 1, _fpFont);
    if (!fileInfo.nBlockNum)
    {
        UnloadFontInfo();
        return nuFALSE;
    }
    nuUINT nInfoCount = fileInfo.nBlockNum;
    
    _pFontInfo = (nuBYTE*)nuMalloc(sizeof(NUROFONTFILEINFO)+sizeof(NUROFONTBLOCKINFO)*nInfoCount);
    if(!_pFontInfo)
    {
        UnloadFontInfo();
        return nuFALSE;
    }
    nuMemset(_pFontInfo, 0, sizeof(NUROFONTFILEINFO)+sizeof(NUROFONTBLOCKINFO)*nInfoCount);
    
    nuFseek(_fpFont, 0, NURO_SEEK_SET);
    nuFread(_pFontInfo, sizeof(NUROFONTFILEINFO), 1, _fpFont);		
    nuBYTE* p = _pFontInfo + sizeof(NUROFONTFILEINFO);
    nuUINT i = 0;
    for(i=0; i<nInfoCount; i++)
    {
        if(nuFread(p, sizeof(NUROFONTBLOCKINFO), 1, _fpFont) != 1)
        {
            UnloadFontInfo();
            return nuFALSE;
        }
        p += sizeof(NUROFONTBLOCKINFO) ;
    }
    NUROFONTBLOCKINFO* pfbi = (NUROFONTBLOCKINFO*)(_pFontInfo + sizeof(NUROFONTFILEINFO));
    nuBYTE* pExt = NULL;
    nuMemset(_FTList, 0, sizeof(_FTList));
    nuUINT fc=0;
    for(i=0; i<nInfoCount; i++)
    {
        if (!i)
        {
            _FTList[i] = pfbi[i].nType;
        }
        else
        {
            for (fc=0;fc<i;fc++)
            {
                if (_FTList[fc] == 0)
                {
                    _FTList[fc] = pfbi[i].nType;
                    break;
                }
                if (_FTList[fc] == pfbi[i].nType)
                {
                    break;
                }
            }
            if (fc==i)
            {
                _FTList[fc] = pfbi[i].nType;
            }
        }
        if (pfbi[i].nReserve1)
        {
            pExt = (nuBYTE*)nuMalloc(pfbi[i].nUniCount);
            if (!pExt)
            {
                UnloadFontInfo();
                return nuFALSE;
            }
            nuFseek(_fpFont, pfbi[i].nReserve2, NURO_SEEK_SET);
            if (nuFread(pExt, pfbi[i].nUniCount, 1, _fpFont)!=1)
            {
                nuFree(pExt);
                pExt = NULL;
                UnloadFontInfo();
                return nuFALSE;
            }
            pfbi[i].nReserve2 = (nuUINT)pExt;
        }
    }
    _FTList[255] = fc+1;
    nuUINT fc2 = 0;
    nuUINT fc3 = 0;
    nuBYTE ttt = 0;
    for (fc2=_FTList[255]-1; fc2>0;fc2--)
    {
        for (fc3=0; fc3<fc2; fc3++)
        {
            if (_FTList[fc3]>_FTList[fc3+1])
            {
                ttt = _FTList[fc3];
                _FTList[fc3] = _FTList[fc3+1];
                _FTList[fc3+1] = ttt;
            }
        }
    }
    nuTcscpy(_filename, fontfile);
    return nuTRUE;
}
nuVOID CGdiFontZK::UnloadFontInfo()
{
    if(_fpFont != NULL)
    {
        nuFclose(_fpFont);
        _fpFont = NULL;
    }
    if(_pFontInfo != NULL)
    {
        nuINT nBlockNum = ((NUROFONTFILEINFO*)_pFontInfo)->nBlockNum;
        NUROFONTBLOCKINFO* pfbi = (NUROFONTBLOCKINFO*)(_pFontInfo + sizeof(NUROFONTFILEINFO));
        nuBYTE* pExt = NULL;
        for(nuINT _i=0; _i<nBlockNum; ++_i)
        {
            if (pfbi[_i].nReserve1)
            {
                nuFree((nuBYTE*)pfbi[_i].nReserve2);
                pfbi[_i].nReserve2 = 0;
                pfbi[_i].nReserve1 = 0;
            }
        }
        nuFree(_pFontInfo);
        _pFontInfo = NULL;
    }    
    nuMemset(_FTList, 0, sizeof(_FTList));
    _filename[0] = 0;
}
nuBOOL CGdiFontZK::LoadWordInfo(nuroU32 nUniCode, nuroU32 nUniFontType, nuroFontInfo& info)
{
    nuBYTE *pfontBuff = 0;
    NUROFONTBLOCKINFO *pInfo = 0;
    if (!FontBuffer_LoadFontInfo(nUniCode, nUniFontType, _fpFont
        , _pFontInfo, &pInfo, &pfontBuff))
    {
        return nuFALSE;
    } 
    info.stepx = pInfo->nX;
    info.stepy = 0;
    info.height = pInfo->nY;
    if (pInfo->nReserve1)
    {
        nuBYTE* p = (nuBYTE*)pInfo->nReserve2;
        info.stepx = p[nUniCode-pInfo->nStartUni];
    }
    info.startx = 0;
    info.starty = 0;
    info.bufferheight = info.height;
    info.bufferwidth = info.stepx;
    info.buffer = pfontBuff;
    info.bufferbytesperline = info.stepx;
    return nuTRUE;
}

nuUINT CGdiFontZK::DrawUniFont(PNURO_BMP pBmp, 
                               NURORECT* pRect, 
                               nuUINT nUniCode, 
                               nuINT FontR, 
                               nuINT FontG, 
                               nuINT FontB, 
                               nuUINT nUniFontType, 
                               nuBOOL bUseEdgeColor, 
                               nuINT EdgeR,
                               nuINT EdgeG,
                               nuINT EdgeB)
{
    if(!pBmp || !pRect || !nUniCode)
    {
        return 0;
    }
    nuroFontInfo info = {0};
    if (!LoadWordInfo(nUniCode, nUniFontType, info))
    {
        return 0;
    }
    nuCOLORREF oldColor;
    nuINT k, j, nX, nY;
    nuINT t, t_edge, realR, realG, realB;
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
    for(k=0; k<hEnd; ++k)
    {
        nX = pRect->left+info.startx;
        for(j=0; j<wEnd; ++j)
        {
            t = info.buffer[k*info.bufferbytesperline+j];
            if(t)
            {
                oldColor = CGdiMethodZK::nuBMPGetPixel(pBmp, nX, nY);
                t_edge = t&0x0f;
                t >>= 4;
                realR = nuGetRValue(oldColor);
                realG = nuGetGValue(oldColor);
                realB = nuGetBValue(oldColor);
                if (bUseEdgeColor &&  t_edge&&(t^0xf) )
                {
                    realR += ((EdgeR-realR)*t_edge)>>4;
                    realG += ((EdgeG-realG)*t_edge)>>4;
                    realB += ((EdgeB-realB)*t_edge)>>4;
                }
                if (t)
                {
                    realR += ((FontR-realR)*t)>>4;
                    realG += ((FontG-realG)*t)>>4;
                    realB += ((FontB-realB)*t)>>4;
                }
                CGdiMethodZK::nuBMPSetPixel565(pBmp, nX, nY, nuRGB24TO16_565(realR, realG, realB), NURO_BMP_TYPE_COPY);
            }
            ++nX;
        }
        ++nY;
    }
    pRect->right = NURO_MIN(pRect->right, pRect->left+info.stepx);
    pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+info.height);
    return pRect->bottom - pRect->top;
}

nuUINT CGdiFontZK::DrawUniFont2(	PNURO_BMP pBmp, 
                                NURORECT* pRect, 
                                nuUINT nUniCode, 
                                nuINT FontR, 
                                nuINT FontG, 
                                nuINT FontB, 
                                nuUINT nUniFontType,
                                nuBOOL bUseEdgeColor, 
                                nuINT EdgeR,
                                nuINT EdgeG,
                                nuINT EdgeB)
{
    nuINT FONTBASE = nuFontAutoType(nUniFontType);
    if (nUniFontType == (nuUINT)FONTBASE)
    {
        return DrawUniFont(pBmp, 
            pRect, 
            nUniCode, 
            FontR, 
            FontG, 
            FontB, 
            nUniFontType, 
            bUseEdgeColor, 
            EdgeR,
            EdgeG,
            EdgeB);
    }
    if(!pBmp || !pRect || !nUniCode)
    {
        return 0;
    }
    nuINT NewFntX,NewFntY,UniSize;
    nuCOLORREF oldColor;
    nuINT k, j, nX, nY;
    nuINT nowvalue,t, t_edge, realR, realG, realB;
    nuINT UnitL,UnitH,FntL,FntH;
    nuroRECT	Fntrect,LimitValue;
    nuroFontInfo info = {0};
    if (!LoadWordInfo(nUniCode, FONTBASE, info))
    {
        return 0;
    }
    nuWORD nRealW = info.bufferwidth;
    NewFntX=(nRealW*nUniFontType+(FONTBASE>>1))/FONTBASE;
    NewFntY=(info.bufferheight*nUniFontType+(FONTBASE>>1))/FONTBASE;
    if(nUniCode)
    {
        if( nUniFontType == 0)
        {
            nUniFontType = 24;
        }
        UniSize=FONTBASE*FONTBASE/nUniFontType;
        Fntrect.left=0;
        Fntrect.top=0;
        Fntrect.right=0;
        Fntrect.bottom=0;
        
        for(k=0; k<NewFntY; ++k)
        {
            Fntrect.left=0;
            Fntrect.top=Fntrect.bottom;
            Fntrect.right=Fntrect.left+UniSize;
            Fntrect.bottom=Fntrect.top+UniSize;
            
            for(j=0; j<NewFntX; ++j)
            {
                Fntrect.right=Fntrect.left+UniSize;
                LimitValue=Fntrect;
                t_edge = 0;//?r?????N???⪺?{??0x00~0x0F
                t = 0;//?r??
                
                //?i?????ƾ??z
                for(FntL=(Fntrect.left/FONTBASE);FntL<((Fntrect.right/FONTBASE)+1);FntL++)
                {
                    UnitL=FntL*FONTBASE;
                    if(UnitL<=Fntrect.left)
                    {
                        if((UnitL+FONTBASE)<=Fntrect.right)
                        {
                            UnitL=FONTBASE-(Fntrect.left-UnitL);
                        }
                        else
                        {
                            UnitL=Fntrect.right-Fntrect.left;
                        }
                    }
                    else if(UnitL<Fntrect.right)
                    {
                        if(FONTBASE+UnitL<=Fntrect.right)
                        {
                            UnitL=FONTBASE;
                        }
                        else
                        {
                            UnitL=Fntrect.right-UnitL;
                        }
                    }
                    else
                    {
                        continue;
                    }                    
                    for(FntH=(Fntrect.top/FONTBASE);FntH<((Fntrect.bottom/FONTBASE)+1);FntH++)
                    {
                        UnitH=FntH*FONTBASE;
                        if(UnitH<=Fntrect.top)
                        {
                            if((UnitH+FONTBASE)<=Fntrect.bottom)
                            {
                                UnitH=FONTBASE-(Fntrect.top-UnitH);
                            }
                            else
                            {
                                UnitH=Fntrect.bottom-Fntrect.top;
                            }
                        }
                        else if(UnitH<Fntrect.bottom)
                        {
                            if(FONTBASE+UnitH<=Fntrect.bottom)
                            {
                                UnitH=FONTBASE;
                            }
                            else
                            {
                                UnitH=Fntrect.bottom-UnitH;
                            }
                        }
                        else
                        {
                            continue;
                        }
                        nowvalue = info.buffer[FntH*nRealW+FntL];
                        t_edge += (nowvalue&0x0f)*UnitL*UnitH;//?r?????N???⪺?{??0x00~0x0F
                        t += (nowvalue>>4)*UnitL*UnitH;//?r??
                    }
                }
                t_edge=t_edge/(UniSize*UniSize);
                t=t/(UniSize*UniSize);
                Fntrect.left=Fntrect.right;
                
                if((t||t_edge) && (pRect->left+j)<pRect->right && (pRect->top+k)<pRect->bottom)
                {
                    nX = pRect->left+j;                                            
                    nY = pRect->top+k;
                    oldColor = CGdiMethodZK::nuBMPGetPixel(pBmp, nX, nY);
                    //???o????
                    realR = nuGetRValue(oldColor);
                    realG = nuGetGValue(oldColor);
                    realB = nuGetBValue(oldColor);
                    if (bUseEdgeColor && t_edge)
                    {                        
                        realR += ((EdgeR-realR)*t_edge)>>4;
                        realG += ((EdgeG-realG)*t_edge)>>4;
                        realB += ((EdgeB-realB)*t_edge)>>4;
                    }
                    if (t)
                    {
                        realR += ((FontR-realR)*t)>>4;
                        realG += ((FontG-realG)*t)>>4;
                        realB += ((FontB-realB)*t)>>4;
                    }
                    CGdiMethodZK::nuBMPSetPixel565(pBmp, 
                        nX, 
                        nY, 
                        nuRGB24TO16_565(realR, realG, realB), 
                        NURO_BMP_TYPE_COPY);
                }
            }
        }
    }
    pRect->right = NURO_MIN(pRect->right, pRect->left+NewFntX);
    pRect->bottom = NURO_MIN(pRect->bottom, pRect->top+NewFntY);
    return pRect->bottom - pRect->top;
}
nuBOOL CGdiFontZK::DrawTextOut( PNURO_BMP pBmp,
                                nuINT X, 
                                nuINT Y,
                                nuWCHAR *wStr, 
                                nuUINT nCount, 
                                nuUINT nUniFontType,
                                nuCOLORREF color,
                                nuBOOL bUseEdgeColor,
                                nuCOLORREF edgeColor,
                                nuINT bkMode,
                                nuCOLORREF bkColor)
{
    nuUINT i = 0;
    nuINT startX = X;
    nuINT startY = Y;
    nuINT FontR, FontG, FontB;
    nuINT EdgeR, EdgeG, EdgeB;
    
    if(bkMode != NURO_TRANSPARENT)
    {
        nuINT w=0, h=0;
        nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);
        CGdiMethodZK::nuBMPFillRect(pBmp, X, Y, w, h, bkColor, NURO_BMP_TYPE_COPY);
    }
    NURORECT rect;
    FontR=nuGetRValue(color);
    FontG=nuGetGValue(color);
    FontB=nuGetBValue(color);
    EdgeR=nuGetRValue(edgeColor);
    EdgeG=nuGetGValue(edgeColor);
    EdgeB=nuGetBValue(edgeColor);
    
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
        DrawUniFont2(pBmp
            , &rect
            , wStr[i]
            , FontR
            , FontG
            , FontB
            , nUniFontType
            , bUseEdgeColor
            , EdgeR
            , EdgeG
            , EdgeB);
        startX = rect.right;
    }
    return nuTRUE;
}
nuINT CGdiFontZK::DrawTextOut2(PNURO_BMP pBmp,
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
    nuFontRectCountEx(wStr, nCount, nUniFontType, &w, &h);
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
        {}
        realH = h;
    }
    if(bkMode != NURO_TRANSPARENT)
    {
        CGdiMethodZK::nuBMPFillRect(pBmp, startX, startY, realW, realH
            , bkColor, NURO_BMP_TYPE_COPY);
    }
    NURORECT rect;
    nuINT FontR, FontG, FontB;
    nuINT EdgeR, EdgeG, EdgeB;
    FontR=nuGetRValue(color);
    FontG=nuGetGValue(color);
    FontB=nuGetBValue(color);
    EdgeR=nuGetRValue(edgeColor);
    EdgeG=nuGetGValue(edgeColor);
    EdgeB=nuGetBValue(edgeColor);
    for(i=0; i<nCount; ++i)
    {
        if (!wStr[i])
        {
            continue;
        }
        rect.left = startX;
        rect.top = startY;
        rect.right = pRect->right;
        rect.bottom = pRect->bottom;
        if(rect.left>=pBmp->bmpWidth || rect.top>=pBmp->bmpHeight)
        {
            break;
        }
        if (DrawUniFont2(pBmp
            , &rect
            , wStr[i]
            , FontR
            , FontG
            , FontB
            , nUniFontType
            , bUseEdgeColor
            , EdgeR
            , EdgeG
            , EdgeB))
        {
            startX = rect.right;
        }
    }
    return realH;
}
nuBOOL	CGdiFontZK::DrawTextOutRotary(PNURO_BMP pBmp,
                                             nuINT X,
                                             nuINT Y, 
                                             nuFLOAT startscalex,
                                             nuFLOAT startscaley,
                                             long RotaryAngle,
                                             nuWCHAR *wStr,
                                             nuUINT nCount,
                                             nuUINT nUniFontType,
                                             nuCOLORREF fontcolor,
                                             nuBOOL bUseEdgeColor,
                                             nuCOLORREF edgeColor)
{
    NrBitmap8D2 b1;
    nuINT w = 0;
    nuINT h = 0;
    nuUINT h0 = nuFontAutoType(nUniFontType);
    nuINT count =  NURO_MIN((nuUINT)nCount, nuWcslen(wStr));
    if (!nuTextOutBitmap8D2(wStr, count, h0, b1, w, h))
    {
        return nuFALSE;
    }
    if (!w || !h)
    {
        return 0;
    }
    double zoom = (double)nUniFontType / h0;
    nuroColor color(fontcolor, nuroColor::COLORTYPE_RGBA32);
    nuroColor color2(edgeColor, nuroColor::COLORTYPE_RGBA32);
    b1.RenderOutRotaryBilinear2(pBmp
        , long(b1.width() * startscalex)
        , long(b1.height() * startscaley)
        , X
        , Y
        , RotaryAngle 
        , zoom
        , zoom
        , color, bUseEdgeColor, color2);
    return nuTRUE;
}
nuBOOL CGdiFontZK::nuTextOutBitmap8D2(nuWCHAR* str
                                    , nuINT count
                                    , nuUINT nUniFontType
                                    , NrBitmap8D2& dstbmp, nuINT& width, nuINT& height)
{
    if (!nuFontRectCountEx(str, count, nUniFontType, &width, &height, nuFALSE))
    {
        return nuFALSE;
    }
    if (dstbmp.width()<width
        || dstbmp.height()<height)
    {
        if (!dstbmp.Create(width, height))
        {
            return nuFALSE;
        }
    }
    dstbmp.FillColor(0);
    
    nuINT ncount = 0;
    NrBitmap8D2 tmpbmp;
    nuINT startx = 0;
    unsigned short nRealW=0;
    unsigned short nRealH=0;
    nuBYTE* p=0;
    nuroFontInfo info = {0};
    for (ncount=0; ncount<count; ++ncount)
    {
        if (!LoadWordInfo(str[ncount], nUniFontType, info))
        {
            return 0;
        }
        nRealH = info.bufferheight;
        nRealW = info.bufferwidth;
        tmpbmp.CreateFromBuffer(nRealW, nRealH, info.buffer);
        tmpbmp.CopyTo(&dstbmp, startx, 0, 0, 0, nRealW, nRealH);
        startx += nRealW;
    }
    return nuTRUE;
}

nuBOOL CGdiFontZK::FontBuffer_Init()
{
    if (!fontbuffer)
    {
        fontbuffer = (FontBufferData*) nuMalloc(sizeof(FontBufferData) * C_FONTBUFFER_SIZE);
        if (fontbuffer)
        {
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
nuVOID CGdiFontZK::FontBuffer_Destroy()
{
    if(fontbuffer)
    {
        nuFree(fontbuffer);
        fontbuffer = 0;
        usedHead = 0;
        usedEnd = 0;
        freeHead = 0;
    }
}
nuBOOL CGdiFontZK::FontBuffer_GetBuffer(nuUINT word, nuUINT type, FontBufferData** fbd)
{	
    if(!usedHead)
    {
        *fbd = freeHead;
        return nuFALSE;
    }
    FontBufferData* tagbuffer = usedEnd;
    while (tagbuffer)
    {
        if(tagbuffer->unicode == word && tagbuffer->type == type)
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
        //nuMemset(freeHead, 0, sizeof(FontBufferData));
    }
    *fbd = freeHead;
    return nuFALSE;
}
nuVOID CGdiFontZK::FontBuffer_Active(FontBufferData* fbd)
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
nuBOOL CGdiFontZK::FontBuffer_LoadFontInfo(nuUINT word, nuUINT nUniFontType, nuFILE* fpFontData
                                         , const nuBYTE* pFontInfo, NUROFONTBLOCKINFO** info, nuBYTE** buffer)
{
    if (!FontBuffer_Init())
    {
        return nuFALSE;
    }
    FontBufferData* fbd = 0;
    if (!FontBuffer_GetBuffer(word, nUniFontType, &fbd))// && fbd
    {
        nuINT i=0;
        nuINT nBlockNum = ((NUROFONTFILEINFO*)pFontInfo)->nBlockNum;
        NUROFONTBLOCKINFO* pInfo = (NUROFONTBLOCKINFO*)(pFontInfo+sizeof(NUROFONTFILEINFO));
        for(i=0; i<nBlockNum; ++i)
        {
            if(nUniFontType == pInfo[i].nType
                && word>=pInfo[i].nStartUni
                && word<(nuUINT)(pInfo[i].nStartUni+pInfo[i].nUniCount) )
            {
                pInfo = &pInfo[i];
                break;
            }
        }
        if(i == nBlockNum)
        {
            return nuFALSE;
        }
        nuSIZE fontdatalen = pInfo->nX*pInfo->nY;
        if (fontdatalen>C_FONTBUFFER_DATALIMIT)
        {
            return nuFALSE;
        }
        nuFseek(fpFontData, pInfo->nStartPos+(word-pInfo->nStartUni)*pInfo->nX*pInfo->nY, NURO_SEEK_SET);
        if (nuFread(fbd->buffer, fontdatalen, 1, fpFontData) != 1)
        {
            return nuFALSE;
        }
        fbd->unicode = word;
        fbd->info = pInfo;
        fbd->type = pInfo->nType;
    }
    *info = fbd->info;
    *buffer = fbd->buffer;
    FontBuffer_Active(fbd);
    return nuTRUE;
}
#endif //NURO_FONT_WAY
