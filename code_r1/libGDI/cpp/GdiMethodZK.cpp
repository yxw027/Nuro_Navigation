// GdiMethodZK.cpp: implementation of the CGdiMethodZK class.
//
//////////////////////////////////////////////////////////////////////

#include "GdiMethodZK.h"
#include "nuRotaryClip.h"

#ifdef NURO_USE_BMPLIB3
#include "nuBMPlib3.h"
#endif

#ifdef NURO_USE_BMPLIB3_3DLINE
#include "nuBMPlib3.h"
#endif

NrBitmap8D2::NrBitmap8D2()
{
    width_ = 0;
    height_ = 0;
    bytesperline_ = 0;
    buffer_ = 0;
}
NrBitmap8D2::~NrBitmap8D2()
{
    Destroy();
}
nuVOID NrBitmap8D2::Destroy()
{
    width_ = 0;
    height_ = 0;
    bytesperline_ = 0;
    if (1==type_)
    {
        nuFree(buffer_);
    }
    buffer_ = 0;
}
nuBOOL NrBitmap8D2::Create(nuINT width, nuINT height)
{
    if (buffer_)
    {
        Destroy();
    }
    buffer_ = (nuBYTE*) nuMalloc(width*height);
    if (!buffer_)
    {
        return false;
    }
    type_ = 1;
    width_ = width;
    bytesperline_ = width_;
    height_ = height;
    return nuTRUE;
}
nuBOOL NrBitmap8D2::CreateFromBuffer(nuINT width, nuINT height, nuVOID* buffer)
{
    if (buffer_)
    {
        Destroy();
    }
    width_ = width;
    bytesperline_ = width_;
    height_ = height;
    buffer_ = (nuBYTE*)buffer;
    return nuTRUE;
}
nuBOOL NrBitmap8D2::CopyTo(NrBitmap8D2* dstbmp, nuINT dstx, nuINT dsty, nuINT srcx, nuINT srcy, nuINT srcw, nuINT srch)
{
    if (!buffer_ || !dstbmp || !dstbmp->buffer_)
    {
        return false;
    }
    if (!CGdiMethodZK::nrAreaCoverClip(dstbmp->width(), dstbmp->height(), width_, height_, dstx, dsty, srcx, srcy, srcw, srch))
    {
        return false;
    }
    nuBYTE* pDstLineData = dstbmp->LineData(dsty);
    nuBYTE* pSrcLineData = this->LineData(srcy);
    nuINT y = 0;
    for (y=0; y<srch; ++y)
    {
        nuMemcpy(&pDstLineData[dstx], &pSrcLineData[srcx], srcw);
        pDstLineData += dstbmp->bytesperline();
        pSrcLineData += bytesperline_;
    }
    return nuTRUE;
}
nuBOOL NrBitmap8D2::RenderTest(NURO_BMP* dstbmp, nuINT x0, nuINT y0, nuroColor& color)
{
    nuINT x=0;
    nuINT y=0;
    nuBYTE bytevalue=0;
    for (y=0; y<height_; ++y)
    {
        for (x=0; x<width_; ++x)
        {
            bytevalue = this->GetPixel(x, y);
            if (bytevalue)
            {
                CGdiMethodZK::nuBMPSetPixel565(dstbmp, x0+x, y0+y, nuRGB24TO16_565(color.RValue(), color.GValue(), color.BValue()), NURO_BMP_TYPE_COPY);
            }
        }
    }
    return nuTRUE;
}
nuBOOL NrBitmap8D2::RenderOut(NURO_BMP* dstbmp, nuINT x0, nuINT y0, nuroColor& color, nuBOOL useEdge, nuroColor& edgecolor)
{
    nuINT x=0;
    nuINT y=0;
    nuBYTE bytevalue=0;
    nuBYTE* linedata = LineData(0);
    for (y=0; y<height_; ++y)
    {
        for (x=0; x<width_; ++x)
        {
            bytevalue = linedata[x];
            if (bytevalue)
            {
                nuCOLORREF oldColor = CGdiMethodZK::nuBMPGetPixel(dstbmp, x0+x, y0+y);
                nuINT t_edge = bytevalue&0x0f;
                nuINT t = bytevalue>> 4;
                nuINT realR = nuGetRValue(oldColor);
                nuINT realG = nuGetGValue(oldColor);
                nuINT realB = nuGetBValue(oldColor);
                if (useEdge && t_edge)
                {                    
                    realR = (edgecolor.RValue()*t_edge+realR*(15-t_edge) )/15;
                    realG = (edgecolor.GValue()*t_edge+realG*(15-t_edge) )/15;
                    realB = (edgecolor.BValue()*t_edge+realB*(15-t_edge) )/15;
                }
                if (t)
                {
                    realR = (color.RValue()*t+realR*(15-t) )/15;
                    realG = (color.GValue()*t+realG*(15-t) )/15;
                    realB = (color.BValue()*t+realB*(15-t) )/15;
                }
                CGdiMethodZK::nuBMPSetPixel565(dstbmp, x0+x, y0+y, nuRGB24TO16_565(realR, realG, realB), NURO_BMP_TYPE_COPY);
            }
        }
        linedata += bytesperline_;
    }
    return nuTRUE;
}
nuBOOL NrBitmap8D2::RenderOutZoom0(NURO_BMP* pDesBmp, nuINT desx, nuINT desy, float scalex, float scaley
                                 , nuroColor& color, nuBOOL useEdge, nuroColor& edgecolor)
{
    
    if (scalex<=0
        || scaley<=0
        || desx>=pDesBmp->bmpWidth
        || desy>=pDesBmp->bmpHeight)
    {
        return false;
    }
    nuINT desw = (nuINT)(width_ * scalex);
    nuINT desh = (nuINT)(height_* scaley);
    nuLONG xrIntFloat_16= (width_<<16)/desw+1;
    nuLONG yrIntFloat_16= (height_<<16)/desh+1;
    nuWORD* pDstLine = 0;
    nuBYTE* pSrcLine = 0;    
    nuLONG y_16 = 0;
    nuWORD transColor = nuLOWORD(pDesBmp->bmpTransfColor);//nuLOWORD(NURO_BMP_TRANSCOLOR_565);
    long bytesWidthDes = CGdiMethodZK::nuBMP565BytesPerLine(pDesBmp);
    long bytesWidthSrc = bytesperline_;
    for (nuLONG y=0; y<desh; ++y)
    {
        if ( CGdiMethodZK::nuBMP565InY(pDesBmp, desy+y) )
        {
            pDstLine = CGdiMethodZK::nuBMP565LineData(pDesBmp, desy+y);
            pSrcLine = LineData(y_16>>16);
            nuLONG x_16 = 0;
            for (nuLONG x=0; x<desw; ++x)
            {
                if ( CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
                {
                    
                    if (pSrcLine[x_16>>16])
                    {
                        nuCOLORREF oldColor = CGdiMethodZK::nuBMPGetPixel(pDesBmp, desx+x, desy+y);
                        nuINT t_edge = pSrcLine[x_16>>16]&0x0f;
                        nuINT t = pSrcLine[x_16>>16]>> 4;
                        nuINT realR = nuGetRValue(oldColor);
                        nuINT realG = nuGetGValue(oldColor);
                        nuINT realB = nuGetBValue(oldColor);
                        if (useEdge && t_edge)
                        {                            
                            realR = (edgecolor.RValue()*t_edge+realR*(15-t_edge) )/15;
                            realG = (edgecolor.GValue()*t_edge+realG*(15-t_edge) )/15;
                            realB = (edgecolor.BValue()*t_edge+realB*(15-t_edge) )/15;
                        }
                        if (t)
                        {
                            ++t;
                            realR = (color.RValue()*t+realR*(16-t) )/16;
                            realG = (color.GValue()*t+realG*(16-t) )/16;
                            realB = (color.BValue()*t+realB*(16-t) )/16;
                        }
                        CGdiMethodZK::nuBMPSetPixel565(pDesBmp, desx+x, desy+y, nuRGB24TO16_565(realR, realG, realB), NURO_BMP_TYPE_COPY);
                    }
                }
                x_16 += xrIntFloat_16;
            }
        }
        y_16 += yrIntFloat_16;
    }
    return nuTRUE;
}
nuVOID NrBitmap8D2::Bilinear2Color(nuBYTE* PColor0,nuBYTE* PColor1,unsigned long u_16,unsigned long v_16
                                 , nuWORD* result, nuroColor& colorDefault
                                 , nuroColor& color, nuBOOL useEdge, nuroColor& edgecolor)
{
    unsigned long pm3_16=(u_16>>8)*(v_16>>8);
    unsigned long pm2_16=u_16 - pm3_16;
    unsigned long pm1_16=v_16 - pm3_16;
    unsigned long pm0_16=(1<<16)-pm1_16-pm2_16-pm3_16;
    nuINT realR = colorDefault.RValue();
    nuINT realG = colorDefault.GValue();
    nuINT realB = colorDefault.BValue();
    nuINT t_edge = 0;
    nuINT t = 0;
    if (useEdge)
    {
        t_edge = (PColor0[0]&0x0f) * pm0_16
            + (PColor0[1]&0x0f) * pm2_16
            + (PColor1[0]&0x0f) * pm1_16
            + (PColor1[1]&0x0f) * pm3_16;
        t_edge = t_edge >> 16;
        if (t_edge)
        {
            realR = (edgecolor.RValue()*t_edge+realR*(15-t_edge) )/15;
            realG = (edgecolor.GValue()*t_edge+realG*(15-t_edge) )/15;
            realB = (edgecolor.BValue()*t_edge+realB*(15-t_edge) )/15;
        }
    }    
    t = (PColor0[0]>>4) * pm0_16
        + (PColor0[1]>>4) * pm2_16
        + (PColor1[0]>>4) * pm1_16
        + (PColor1[1]>>4) * pm3_16;
    t = t>>16;
    if (t)
    {
        ++t;
        realR = (color.RValue()*t+realR*(16-t) )/16;
        realG = (color.GValue()*t+realG*(16-t) )/16;
        realB = (color.BValue()*t+realB*(16-t) )/16;
    }
    *result = nuRGB24TO16_565(realR, realG, realB);
}
nuVOID NrBitmap8D2::Bilinear2BorderColor(const long x_16,const long y_16,nuWORD* result, nuroColor& colorDefault
                                       , nuroColor& color, nuBOOL useEdge, nuroColor& edgecolor)
{
    
    nuroColor color2;
    nuINT a = sizeof(nuroColor);
    
    long x=(x_16>>16);
    long y=(y_16>>16);
    unsigned long u_16=((unsigned short)(x_16));
    unsigned long v_16=((unsigned short)(y_16));
    
    nuBYTE bytevalue[4];
    bytevalue[0] = GetPixel(x, y);
    bytevalue[1] = GetPixel(x+1, y);
    bytevalue[2] = GetPixel(x, y+1);
    bytevalue[3] = GetPixel(x+1, y+1);
    
    Bilinear2Color(&bytevalue[0],&bytevalue[2],u_16,v_16,result, colorDefault
        , color, useEdge, edgecolor);
}
nuBOOL NrBitmap8D2::RenderOutZoomBilinear2(NURO_BMP* pDesBmp
                                         , nuINT desx
                                         , nuINT desy
                                         , float scalex
                                         , float scaley
                                         , nuroColor& color
                                         , nuBOOL useEdge
                                         , nuroColor& edgecolor)
{
    nuINT desw = (nuINT)(width_ * scalex);
    nuINT desh = (nuINT)(height_* scaley);
    
    long xrIntFloat_16=(width_<<16)/desw+1; 
    long yrIntFloat_16=(height_<<16)/desh+1;
    const long csDErrorX=-(1<<15)+(xrIntFloat_16>>1);
    const long csDErrorY=-(1<<15)+(yrIntFloat_16>>1);
    
    long border_y0 = -csDErrorY/yrIntFloat_16+1;
    if (border_y0>=desh)
        border_y0 = desh;
    long border_x0 = -csDErrorX/xrIntFloat_16+1;
    if (border_x0>=desw ) 
        border_x0=desw;
    long border_y1=(((height_-2)<<16)-csDErrorY)/yrIntFloat_16+1;
    if (border_y1<border_y0)
        border_y1=border_y0;
    long border_x1=(((width_-2)<<16)-csDErrorX)/xrIntFloat_16+1; 
    if (border_x1<border_x0) 
        border_x1=border_x0;
    
    nuWORD* pDesLine = (nuWORD*)pDesBmp->pBmpBuff;
    long bytesWidthDes = CGdiMethodZK::nuBMP565BytesPerLine(pDesBmp);
    long bytesWidthSrc = bytesperline_;
    long srcy_16=csDErrorY;
    long y = 0;
    long x = 0;
    nuroColor tmpcolor;
    ((nuBYTE*&)pDesLine) += bytesWidthDes*desy;
    for (y=0;y<border_y0;++y)
    {
        if (CGdiMethodZK::nuBMP565InY(pDesBmp, desy+y))
        {
            long srcx_16=csDErrorX;
            for (x=0;x<desw;++x)
            {
                if ( !CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
                {
                    srcx_16+=xrIntFloat_16;
                    continue;
                }
                tmpcolor.SetValue(pDesLine[x+desx], nuroColor::COLORTYPE_RGB565);
                Bilinear2BorderColor(srcx_16,srcy_16,&pDesLine[x+desx], tmpcolor, color, useEdge, edgecolor);
                srcx_16+=xrIntFloat_16;
            }
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
    for (y=border_y0; y<border_y1; ++y)
    {
        if (!CGdiMethodZK::nuBMP565InY(pDesBmp, desy+y))
        {
            srcy_16+=yrIntFloat_16;
            ((nuBYTE*&)pDesLine)+=bytesWidthDes;
            continue;
        }
        long srcx_16=csDErrorX;
        for (x=0; x<border_x0; ++x)
        {
            if ( CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
            {
                tmpcolor.SetValue(pDesLine[x+desx], nuroColor::COLORTYPE_RGB565);
                Bilinear2BorderColor(srcx_16,srcy_16,&pDesLine[x+desx], tmpcolor, color, useEdge, edgecolor);
            }
            srcx_16+=xrIntFloat_16;
        }
        unsigned long v_16=(srcy_16 & 0xFFFF);
        nuBYTE* PSrcLineColor= LineData(srcy_16>>16);
        for (x=border_x0; x<border_x1; ++x)
        {
            if ( CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
            {
                nuBYTE* PColor0=&PSrcLineColor[srcx_16>>16];
                nuBYTE* PColor1=PColor0+bytesWidthSrc;
                tmpcolor.SetValue(pDesLine[x+desx], nuroColor::COLORTYPE_RGB565);
                Bilinear2Color(PColor0, PColor1, (srcx_16 & 0xFFFF),v_16,&pDesLine[x+desx], tmpcolor
                    , color, useEdge, edgecolor);
                
            }
            srcx_16+=xrIntFloat_16;
        }
        for (x=border_x1;x<desw;++x)
        {
            if ( CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
            {
                tmpcolor.SetValue(pDesLine[x+desx], nuroColor::COLORTYPE_RGB565);
                Bilinear2BorderColor(srcx_16,srcy_16,&pDesLine[x+desx], tmpcolor
                    , color, useEdge, edgecolor);
            }
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
    for (y=border_y1;y<desh;++y)
    {
        if (CGdiMethodZK::nuBMP565InY(pDesBmp, desy+y))
        {
            long srcx_16=csDErrorX;
            for (x=0;x<desw;++x)
            {
                if ( CGdiMethodZK::nuBMP565InX(pDesBmp, desx+x) )
                {
                    tmpcolor.SetValue(pDesLine[x+desx], nuroColor::COLORTYPE_RGB565);
                    Bilinear2BorderColor(srcx_16,srcy_16,&pDesLine[x+desx], tmpcolor
                        , color, useEdge, edgecolor);                   
                }
                srcx_16+=xrIntFloat_16;
            }
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
    return nuTRUE;
}
nuVOID NrBitmap8D2::RotaryBilinear2Line(nuWORD* pDstLine,long dst_border_x0,long dst_in_x0,long dst_in_x1,long dst_border_x1
                                      , long srcx0_16,long srcy0_16,long Ax_16,long Ay_16                                 
                                      , nuroColor& color
                                      , nuBOOL useEdge
                                      , nuroColor& edgecolor)
{
    nuroColor tmpcolor;
    for (long x=dst_border_x0;x<dst_border_x1;++x)
    {
        tmpcolor.SetValue(pDstLine[x], nuroColor::COLORTYPE_RGB565);
        Bilinear2BorderColor(srcx0_16,srcy0_16,&pDstLine[x], tmpcolor
            , color, useEdge, edgecolor);
        srcx0_16+=Ax_16;
        srcy0_16+=Ay_16;
    }
}
nuVOID NrBitmap8D2::RenderOutRotaryBilinear2(const PNURO_BMP pDstBmp
                                           , long centerx
                                           , long centery
                                           , long centernewx
                                           , long centernewy  
                                           , long RotaryAngle
                                           , double ZoomX
                                           , double ZoomY                     
                                           , nuroColor& color
                                           , nuBOOL useEdge
                                           , nuroColor& edgecolor)
{    
    double tmprZoomXY=1.0/(ZoomX*ZoomY);
    double rZoomX=tmprZoomXY*ZoomY;
    double rZoomY=tmprZoomXY*ZoomX;
    double sinA = nuSin(RotaryAngle);
    double cosA = nuCos(RotaryAngle);
    long Ax_16=(long)(rZoomX*cosA*(1<<16)); 
    long Ay_16=(long)(rZoomX*sinA*(1<<16)); 
    long Bx_16=(long)(-rZoomY*sinA*(1<<16));
    long By_16=(long)(rZoomY*cosA*(1<<16));
    long Cx_16=(long)((-centernewx*rZoomX*cosA+centernewy*rZoomY*sinA+centerx)*(1<<16));
    long Cy_16=(long)((-centernewx*rZoomX*sinA-centernewy*rZoomY*cosA+centery)*(1<<16)); 
    TRotaryClipData rcData;
    rcData.Ax_16=Ax_16;
    rcData.Bx_16=Bx_16;
    rcData.Cx_16=Cx_16;
    rcData.Ay_16=Ay_16;
    rcData.By_16=By_16;
    rcData.Cy_16=Cy_16;
    rcData.dst_width=pDstBmp->bmpWidth;
    rcData.dst_height=pDstBmp->bmpHeight;
    rcData.src_width=width_;
    rcData.src_height=height_;
    if (!rcData.inti_clip(centernewx,centernewy,1)) 
    {
        return;
    }
    nuLONG bytesWidthDst = CGdiMethodZK::nuBMP565BytesPerLine(pDstBmp);
    nuWORD* pDstLine= (nuWORD*)pDstBmp->pBmpBuff;
    ((nuBYTE*&)pDstLine)+=(bytesWidthDst*rcData.out_dst_down_y);
    while (nuTRUE) //to down
    {
        long y=rcData.out_dst_down_y;
        if (y>=pDstBmp->bmpHeight) break;
        if (y>=0)
        {
            RotaryBilinear2Line(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                rcData.out_dst_x1_in,rcData.out_dst_x1_boder
                ,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16
                , color, useEdge, edgecolor);
        }
        if (!rcData.next_clip_line_down()) break;
        ((nuBYTE*&)pDstLine)+=bytesWidthDst;
    }    
    pDstLine= (nuWORD*)pDstBmp->pBmpBuff;
    ((nuBYTE*&)pDstLine)+=(bytesWidthDst*rcData.out_dst_up_y);
    while (rcData.next_clip_line_up()) //to up 
    {
        long y=rcData.out_dst_up_y;
        if (y<0) break;
        ((nuBYTE*&)pDstLine)-=bytesWidthDst;
        if (y<pDstBmp->bmpHeight)
        {
            RotaryBilinear2Line(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                rcData.out_dst_x1_in,rcData.out_dst_x1_boder
                ,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16
                , color, useEdge, edgecolor);
        }
    }
}
nuVOID NrBitmap8D2::FillColor(nuBYTE color8)
{
    if (color8)
    {
        return;
    }
    nuMemset(buffer_, 0, bytesperline_*height_);
    return;
}

CGdiMethodZK::CGdiMethodZK()
{
    
}

CGdiMethodZK::~CGdiMethodZK()
{
    
}

nuBOOL CGdiMethodZK::Initialize()
{
    return nuTRUE;
}

nuVOID CGdiMethodZK::Free()
{
    ;
}
nuVOID CGdiMethodZK::BmpLineTCopy(nuWORD* pClrDes, nuWORD* pClrSrc, nuINT nPixelNum, nuINT nMode)
{
    for(nuINT i = 0; i < nPixelNum; ++i)
    {
//        BmpSetPixel(&pClrDes[i], pClrSrc[i], nMode);
        BmpSetPixel(pClrDes, *pClrSrc, nMode);
        ++pClrDes;
        ++pClrSrc;
    }
}

nuBOOL CGdiMethodZK::nrAreaCoverClip(nuINT dstwidth, nuINT dstheight, nuINT srcwidth, nuINT srcheight
                                    , nuINT& dstx, nuINT& dsty, nuINT& srcx, nuINT& srcy, nuINT& srcw, nuINT& srch)
{    
    if ( srcx>=srcwidth
        || srcy>=srcheight 
        || (dstx>=dstwidth)
        || (dsty>=dstheight)
        || srcw<=0 
        || srch<=0  )
    {
        return false;
    }
    if (srcx<0)
    {
        srcw += srcx;
        if (srcw<=0)
        {
            return false;
        }
        srcx = 0;
    }
    if ( (srcx+srcw)>srcwidth )
    {
        srcw = srcwidth - srcx;
    }
    if (srcy<0)
    {
        srch += srcy;
        if (srch<=0)
        {
            return false;
        }
        srcy = 0;
    }
    if ( (srcy+srch)>srcheight )
    {
        srch = srcheight - srcy;
    }
    if ( ((dstx+srcw)<0 )
        || ((dsty+srch)<0) )
    {
        return false;
    }
    if (dstx<0)
    {
        srcx -= dstx;
        if (srcx>=srcwidth)
        {
            return false;
        }
        srcw += dstx;
        dstx = 0;
    }
    if ( (dstx+srcw)>dstwidth )
    {
        srcw = dstwidth-dstx;
        if (srcw<=0)
        {
            return false;
        }
    }    
    if (dsty<0)
    {
        srcy -= dsty;
        if (srcy>=srcheight)
        {
            return false;
        }
        srch += dsty;
        dsty = 0;
    }
    if ( (dsty+srch)>dstheight )
    {
        srch = dstheight-dsty;
        if (srch<=0)
        {
            return false;
        }
    }
    return nuTRUE;
}

nuBOOL CGdiMethodZK::nuGMCopyBmp( PNURO_BMP pDesBmp, 
                   nuINT nDesX, 
                   nuINT nDesY, 
                   nuINT nWidth, 
                   nuINT nHeight, 
                   const NURO_BMP* pSrcBmp, 
                   nuINT nSrcX, 
                   nuINT nSrcY,
                   nuINT mode)
{
    if( nWidth <= 0		|| 
        nHeight <= 0	|| 
        !CGdiMethodZK::BmpDrawCheck( pDesBmp,
        nDesX,
        nDesY,
        nWidth,
        nHeight,
        pSrcBmp,
        nSrcX,
        nSrcY) ) 
    {
        return false;
    }
    if( mode != NURO_BMP_TYPE_COPY)
    {
        return CGdiMethodZK::BmpTCopy( pDesBmp, 
            nDesX,
            nDesY,
            nWidth,
            nHeight,
            pSrcBmp,
            nSrcX,
            nSrcY,
            mode);
    }
    else
    {
        return CGdiMethodZK::BmpCopy( pDesBmp, 
									   nDesX,
                                       nDesY, 
                                       nWidth,
                                       nHeight,
                                       pSrcBmp,
                                       nSrcX,
                                       nSrcY );
    }
}

nuBOOL CGdiMethodZK::nuGMPaintBmp( PNURO_BMP pDesBmp,
                    nuINT nDesX,
                    nuINT nDesY,
                    nuINT nWidth,
                    nuINT nHeight,
                    const PNURO_BMP pSrcBmp,
                    nuINT nSrcX, 
                    nuINT nSrcY ,
                    nuINT mode)
{
    /*if( nWidth <= 0		|| 
        nHeight <= 0	|| 
        !CGdiMethodZK::BmpDrawCheck( pDesBmp,
        nDesX,
        nDesY,
        nWidth,
        nHeight,
        pSrcBmp,
        nSrcX,
        nSrcY) ) 
    {	
        return false;
    }*/
    if( mode != NURO_BMP_TYPE_COPY)
    {
        return CGdiMethodZK::BmpTPaint( pDesBmp, 
            nDesX,
            nDesY,
            nWidth,
            nHeight,
            pSrcBmp,
            nSrcX,
            nSrcY,
            mode);
    }
    else
    {
        return CGdiMethodZK::BmpPaint( pDesBmp, 
            nDesX,
            nDesY, 
            nWidth,
            nHeight,
            pSrcBmp,
            nSrcX,
            nSrcY );
    }
}
nuBOOL CGdiMethodZK::BmpCopy( PNURO_BMP pDesBmp, 
							 nuINT nDesX, 
							 nuINT nDesY, 
							 nuINT width, 
							 nuINT height, 
							 const NURO_BMP* pSrcBmp, 
							 nuINT nSrcX, 
							 nuINT nSrcY )
{
// 	nuBYTE* sr1;
// 	nuBYTE*	de1;
	nuWORD i1,i2;
	nuINT usew = width*2;
	nuINT maxdh = nDesY+height;
	nuINT maxsh = nSrcY+height;
	nuBYTE* sbase = pSrcBmp->pBmpBuff + nSrcY * pSrcBmp->bytesPerLine + (nSrcX<<1);
	nuBYTE* dbase = pDesBmp->pBmpBuff + nDesY * pDesBmp->bytesPerLine + (nDesX<<1);
	for(i1=nDesY,i2=nSrcY; (i1<maxdh)&&(i2<maxsh); ++i1,++i2)
	{
// 		sr1 = sbase + i2*pSrcBmp->bytesPerLine;
// 		de1 = dbase + i1*pDesBmp->bytesPerLine;
		nuMemcpy(dbase, sbase, usew);
        sbase += pSrcBmp->bytesPerLine;
        dbase += pDesBmp->bytesPerLine;
    }
	return nuTRUE;
}

nuBOOL CGdiMethodZK::BmpTCopy( PNURO_BMP pDesBmp, 
							  nuINT nDesX, 
							  nuINT nDesY, 
							  nuINT width, 
							  nuINT height, 
							  const NURO_BMP* pSrcBmp, 
							  nuINT nSrcX, 
							  nuINT nSrcY,
                              nuINT mode)
{
#ifdef _NURO_TEST
	CFileLogZK logFile;
	logFile.LogOpen(TEXT("test.log"), TEXT("ab+"));
	DWORD nTick = GetTickCount();
#endif
	nuWORD i1,i2,j1,j2;
	nuWORD *pColor16Src = (nuWORD*)pSrcBmp->pBmpBuff + ((nSrcY * pSrcBmp->bytesPerLine)>>1) + nSrcX;
	nuWORD *pColor16Des = (nuWORD*)pDesBmp->pBmpBuff + ((nDesY * pDesBmp->bytesPerLine)>>1) + nDesX;
	nuINT maxdh = nDesY+height;
	nuINT maxsh = nSrcY+height;
	nuINT maxdw = nDesX+width;
	nuINT maxsw = nSrcX+width;
    nuINT nextdw = (pDesBmp->bytesPerLine>>1) - width;
    nuINT nextsw = (pSrcBmp->bytesPerLine>>1) - width;
    if (NURO_BMP_TYPE_MIXX == mode)
    {
        nuINT count = 0;
        for(i1=nDesY,i2=nSrcY; (i1<maxdh)&&(i2<maxsh); ++i1,++i2)
        {
            count = 0;
            for(j1=nDesX,j2=nSrcX;(j1<maxdw)&&(j2<maxsw);++j1,++j2)
            {
                if( pColor16Src[count] != pSrcBmp->bmpTransfColor )
                {
                    ++count;
                }
                else
                {
                    if (count)
                    {
                        nuMemcpy(pColor16Des, pColor16Src, count<<1);
                        pColor16Des += count;
                        pColor16Src += count;
                        count = 0;
                    }
                    ++pColor16Des;
                    ++pColor16Src;
                }
            }
            if (count)
            {
                nuMemcpy(pColor16Des, pColor16Src, count<<1);
                pColor16Des += count;
                pColor16Src += count;
                count = 0;
            }
            pColor16Src += nextsw;
            pColor16Des += nextdw;
	    }
    }
    else
    {
        for(i1=nDesY,i2=nSrcY; (i1<maxdh)&&(i2<maxsh); ++i1,++i2)
        {
            for(j1=nDesX,j2=nSrcX;(j1<maxdw)&&(j2<maxsw);++j1,++j2)
            {
                /*            if( pColor16Src[j2] != pSrcBmp->bmpTransfColor )*/
                if( *pColor16Src != pSrcBmp->bmpTransfColor )
                {
                    /*				BmpSetPixel(&pColor16Des[j1], pColor16Src[j2], mode);*/
                    BmpSetPixel(pColor16Des, *pColor16Src, mode);
                }
                ++pColor16Des;
                ++pColor16Src;
            }
            pColor16Src += nextsw;
            pColor16Des += nextdw;
            //         pColor16Des = (nuWORD*)( (nuBYTE*)pColor16Des + pDesBmp->bytesPerLine);
            //         pColor16Src = (nuWORD*)( (nuBYTE*)pColor16Src + pSrcBmp->bytesPerLine);
	    }
    }
#ifdef _NURO_TEST
	nTick = GetTickCount() - nTick;
	fwprintf(logFile.m_pFile, L"T Bmp Times = %d ms\r\n", nTick);
	logFile.LogClose();
#endif
	return nuTRUE;
}

nuBOOL CGdiMethodZK::BmpPaint( PNURO_BMP pDesBmp, 
							  nuINT nDesX, 
							  nuINT nDesY, 
							  nuINT nWidth, 
							  nuINT nHeight, 
							  const PNURO_BMP pSrcBmp, 
							  nuINT nSrcX, 
							  nuINT nSrcY )
{
	nuINT nDesYto = nDesY + nHeight;
	nDesYto  = NURO_MIN( nDesYto,  pDesBmp->bmpHeight );
	
	if( nDesY < 0 )//????Y???磩畎??Y??????????埭芞?葍?眳??
	{
		nDesY	+= ( (-nDesY) / pSrcBmp->bmpHeight) * pSrcBmp->bmpHeight;
		nHeight += (nDesY / pSrcBmp->bmpHeight) * pSrcBmp->bmpHeight;
	}
	if( nDesY >= nDesYto )
	{
		return false;
	}
	if( nDesX < 0 )//????X???磩畎??X??????????埭芞????眳??
	{
		nDesX	+= ( (-nDesX) / pSrcBmp->bmpWidth ) * pSrcBmp->bmpWidth;
		nWidth	+= ( nDesX / pSrcBmp->bmpWidth) * pSrcBmp->bmpWidth;
	}
	if( nWidth > pDesBmp->bmpWidth - nDesX )//????????妏?????幙??醴??芞?醴???
	{
		nWidth = pDesBmp->bmpWidth - nDesX;
	}
	if( nWidth <= 0 )
	{
		return false;
	}
	nuINT iSrc = 0;
	if( nDesY < 0 )//????Y????????????????0????????埭芞Y??????????
	{
		iSrc = -nDesY; 
		nDesY = 0;
	}
	nuINT nFirstLine;
	nuINT nFirstSrcX;
	if( nDesX < 0 )
	{
		nFirstLine = pSrcBmp->bmpWidth + nDesX;
		nFirstSrcX = -nDesX;
	}
	else
	{
		nFirstLine = pSrcBmp->bmpWidth;
		nFirstSrcX = 0;
	}
	while( nDesY < nDesYto )
	{
		if( iSrc >= pSrcBmp->bmpHeight )
		{
			iSrc = 0;
		}
		nuINT nTmpW = nWidth;
		nuWORD *pClrDes = (nuWORD*)(pDesBmp->pBmpBuff + nDesY * pDesBmp->bytesPerLine);
		nuWORD *pClrSrc = (nuWORD*)(pSrcBmp->pBmpBuff + iSrc * pSrcBmp->bytesPerLine);
		//......
		if( nFirstLine >= nTmpW )
		{
			nuMemcpy(pClrDes, pClrSrc + nFirstSrcX, 2 * nTmpW);
		}
		else
		{
			nuMemcpy(pClrDes, pClrSrc + nFirstSrcX, 2 * nFirstLine);
			nTmpW	-= nFirstLine;
			pClrDes += nFirstLine;
			while( nTmpW > pSrcBmp->bmpWidth )
			{
				nuMemcpy(pClrDes, pClrSrc, 2 * pSrcBmp->bmpWidth);
				pClrDes += pSrcBmp->bmpWidth;
				nTmpW	-= pSrcBmp->bmpWidth;
			}
			nuMemcpy(pClrDes, pClrSrc, 2 * nTmpW);
		}
		++iSrc;
		++nDesY;
	}
	return nuTRUE;
}

nuBOOL CGdiMethodZK::BmpTPaint( PNURO_BMP pDesBmp, 
							   nuINT nDesX, 
							   nuINT nDesY, 
							   nuINT nWidth, 
							   nuINT nHeight, 
							   const PNURO_BMP pSrcBmp, 
							   nuINT nSrcX, 
							   nuINT nSrcY,
                               nuINT mode)
{
	nuINT nDesYto = nDesY + nHeight;
	nDesYto  = NURO_MIN( nDesYto,  pDesBmp->bmpHeight );
	
	if( nDesY < 0 )//????Y???磩畎??Y??????????埭芞?葍?眳??
	{
		nDesY	+= ( (-nDesY) / pSrcBmp->bmpHeight) * pSrcBmp->bmpHeight;
		nHeight += (nDesY / pSrcBmp->bmpHeight) * pSrcBmp->bmpHeight;
	}
	if( nDesY >= nDesYto )
	{
		return false;
	}
	if( nDesX < 0 )//????X???磩畎??X??????????埭芞????眳??
	{
		nDesX	+= ( (-nDesX) / pSrcBmp->bmpWidth ) * pSrcBmp->bmpWidth;
		nWidth	+= ( nDesX / pSrcBmp->bmpWidth) * pSrcBmp->bmpWidth;
	}
	if( nWidth > pDesBmp->bmpWidth - nDesX )//????????妏?????幙??醴??芞?醴???
	{
		nWidth = pDesBmp->bmpWidth - nDesX;
	}
	if( nWidth <= 0 )
	{
		return false;
	}
	nuINT iSrc = 0;
	if( nDesY < 0 )//????Y????????????????0????????埭芞Y??????????
	{
		iSrc = -nDesY; 
		nDesY = 0;
	}
	nuINT nFirstLine;
	nuINT nFirstSrcX;
	if( nDesX < 0 )
	{
		nFirstLine = pSrcBmp->bmpWidth + nDesX;
		nFirstSrcX = -nDesX;
	}
	else
	{
		nFirstLine = pSrcBmp->bmpWidth;
		nFirstSrcX = 0;
	}
	while( nDesY < nDesYto )
	{
		if( iSrc >= pSrcBmp->bmpHeight )
		{
			iSrc = 0;
		}
		nuINT nTmpW = nWidth;
        nuWORD *pClrDes = (nuWORD*)(pDesBmp->pBmpBuff + nDesY * pDesBmp->bytesPerLine);
		nuWORD *pClrSrc = (nuWORD*)(pSrcBmp->pBmpBuff + iSrc * pSrcBmp->bytesPerLine);

// 		nuWORD *pClrDes = (nuWORD*)pDesBmp->pBmpBuff;
// 		pClrDes += nDesY * pDesBmp->bmpWidth;
// 		nuWORD *pClrSrc = (nuWORD*)pSrcBmp->pBmpBuff;
// 		pClrSrc += iSrc * pSrcBmp->bmpWidth;
		//......
		if( nFirstLine >= nTmpW )
		{
			nuMemcpy(pClrDes, pClrSrc + nFirstSrcX, 2 * nTmpW);
		}
		else
		{
			BmpLineTCopy(pClrDes, pClrSrc + nFirstSrcX, nFirstLine, mode);
			nTmpW	-= nFirstLine;
			pClrDes += nFirstLine;
			while( nTmpW > pSrcBmp->bmpWidth )
			{
				BmpLineTCopy(pClrDes, pClrSrc, pSrcBmp->bmpWidth, mode);
				pClrDes += pSrcBmp->bmpWidth;
				nTmpW	-= pSrcBmp->bmpWidth;
			}
			BmpLineTCopy(pClrDes, pClrSrc, nTmpW, mode);
		}
		++iSrc;
		++nDesY;
	}
	return nuTRUE;
}

nuBOOL CGdiMethodZK::BmpDrawCheck( PNURO_BMP pDesBmp, 
                                 nuINT &nDesX, 
                                 nuINT &nDesY, 
                                 nuINT &width, 
                                 nuINT &height, 
                                 const NURO_BMP* pSrcBmp, 
                                 nuINT &nSrcX, 
                                 nuINT &nSrcY )
{
    /*??????芞?????牮???????(nDesX,nDesY)??(nSrcX,nSrcY)??弇??講*/
    nuWORD dw = pDesBmp->bmpWidth;
    nuWORD dh = pDesBmp->bmpHeight;
    nuWORD sw = pSrcBmp->bmpWidth;
    nuWORD sh = pSrcBmp->bmpHeight;
    /*??尨醴??芞????猁???????搧?????*/
    if( nDesX < 0 )
    {
        width = width + nDesX;
        if( width <= 0 )
        {
            return false;
        }
        nSrcX = nSrcX - nDesX;
        nDesX = 0;
    }
    if( nDesY < 0 )
    {
        height	= height + nDesY;
        if( height <= 0 )
        {
            return false;
        }
        nSrcY	= nSrcY - nDesY;
        nDesY	= 0;
    }
    
    if( nSrcX < 0 )
    {
        width = width + nSrcX;
        if( width <= 0 )
        {
            return false;
        }
        nDesX = nDesX - nSrcX;
        nSrcX = 0;
    }
    if( nSrcY < 0 )
    {
        height	= height + nSrcY;
        if( height <= 0 )
        {
            return false;
        }
        nDesY	= nDesY - nSrcY;
        nSrcY	= 0;
    }
    
    if( nDesX + width >= dw )//?珝?????
    {
        width = width - ((nDesX + width) - dw);
        if( width <= 0 )
        {
            return false;
        }
    }
    if( nDesY + height >= dh )
    {
        height = height - ((nDesY + height) - dh);
        if( height <= 0)
        {
            return false;
        }
    }
    
    if( nSrcX + width >= sw )
    {
        width = width - ((nSrcX + width) -sw);
        if( width <= 0 )
        {
            return false;
        }
    }
    if( nSrcY + height >= sh )
    {
        height = height - ((nSrcY + height) - sh);
        if( height <= 0 )
        {
            return false;
        }
    }
    return nuTRUE;
}
nuVOID CGdiMethodZK::nuBMPSetPixel565_BMP(PNURO_BMP pBmp, nuINT x, nuINT y, PNURO_BMP pBmp2, nuINT nMode)
{
    nuWORD *pColor16 = nuNULL;
    nuWORD color565 = nuBMPGetPixel16(pBmp2, x%pBmp2->bmpWidth, y%pBmp2->bmpHeight);
    if ( !pBmp || !pBmp2 || pBmp->pBmpBuff== nuNULL || pBmp2->pBmpBuff== nuNULL || !NURO_PTINBMP(pBmp, x, y) )
    {
        return;
    }
    pColor16 = &nuBMP565LineData(pBmp, y)[x];
    if(nMode == NURO_BMP_TYPE_COPY)
    {
        *pColor16 = color565;
        return;
    }
    if(color565 ==  pBmp->bmpTransfColor
        || nMode == NURO_BMP_TYPE_MIX0)
    {
        return;
    }
    if(nMode<0x10)
    {
        nuBYTE realR = (nu565RVALUE(color565)*nMode+nu565RVALUE(*pColor16)*(16-nMode))/16;
        nuBYTE realG = (nu565GVALUE(color565)*nMode+nu565GVALUE(*pColor16)*(16-nMode))/16;
        nuBYTE realB = (nu565BVALUE(color565)*nMode+nu565BVALUE(*pColor16)*(16-nMode))/16;
        *pColor16 = nuRGB24TO16_565(realR,realG,realB);
        return;
    }
    switch(nMode)
    {
    case NURO_BMP_TYPE_MIXX:
        *pColor16 = color565;
        break;
    case NURO_BMP_TYPE_AND:
        *pColor16 = color565 & (*pColor16);
        break;
    case NURO_BMP_TYPE_OR:
        *pColor16 = color565 | (*pColor16);
        break;
    case NURO_BMP_TYPE_XOR:
        *pColor16 = color565 ^ (*pColor16);
        break;
    case NURO_BMP_TYPE_NBK:
        *pColor16 = ~(*pColor16);
        break;
    case NURO_BMP_TYPE_ANDNBK:
        *pColor16 = color565 & (~(*pColor16));
        break;
    case NURO_BMP_TYPE_ORNBK:
        *pColor16 = color565 | (~(*pColor16));
        break;
    case NURO_BMP_TYPE_XORNBK:
        *pColor16 = color565 ^ (~(*pColor16));
        break;
    default:
        *pColor16 = color565;
        break;			
    }
    return;
}

nuVOID CGdiMethodZK::nuBMPDrawLineHor(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuCOLORREF color, nuINT nMode)
{
    nuINT nCount = 0;
    nuWORD color565 = nuCOLOR24TO16_565(color);	
    if ( !pBmp )
    {
        return;
    }	
    if (x1 == x2)
    {
        nuBMPSetPixel565(pBmp, x1, y, color565, nMode);
        return;
    }	
    if ( y>=0 && y<pBmp->bmpHeight && !(x1<0 && x2<0) && !( x1>=pBmp->bmpWidth &&  x2>=pBmp->bmpWidth) )
    {
        if (x1<0)
        {
            x1 = 0;
        }
        else if (x1>pBmp->bmpWidth)
        {
            x1 = pBmp->bmpWidth;
        }
        if (x2<0)
        {
            x2 = 0;
        }
        else if (x2>pBmp->bmpWidth)
        {
            x2 = pBmp->bmpWidth;
        }
        nuINT xMin = NURO_MIN(x1,x2);
        nuINT xMax = NURO_MAX(x1,x2);
        for(nCount=xMin; nCount<xMax; ++nCount)
        {
            nuBMPSetPixel565(pBmp, nCount, y, color565, nMode);
        }
    }
    return;
}

nuVOID CGdiMethodZK::nuBMPDrawLineVer(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuCOLORREF color, nuINT nMode)
{
    nuINT nCount = 0;
    nuWORD color565 = nuCOLOR24TO16_565(color);
    
    if ( !pBmp )
    {
        return;
    }
    
    if (y1 == y2)
    {
        nuBMPSetPixel565(pBmp, x, y1, color565, nMode);
        return;
    }
    
    if (x >= 0 && x < pBmp->bmpWidth && !(y1<0 && y2<0) 
        && !( y1>=pBmp->bmpHeight &&  y2>=pBmp->bmpHeight) )
    {
        if (y1<0)
        {
            y1 = 0;
        }
        else if (y1>pBmp->bmpHeight)
        {
            y1 = pBmp->bmpHeight;
        }
        if (y2<0)
        {
            y2 = 0;
        }
        else if (y2>pBmp->bmpHeight)
        {
            y2 = pBmp->bmpHeight;
        }
        nuINT yMin = NURO_MIN(y1,y2);
        nuINT yMax = NURO_MAX(y1,y2);
        for(nCount=yMin; nCount<yMax; ++nCount)
        {
            nuBMPSetPixel565(pBmp, x, nCount, color565, nMode);
        }
    }
    return;
}

nuVOID CGdiMethodZK::nuBMPDrawLineHor_BMP(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, PNURO_BMP pBmp2, nuINT nMode)
{
    nuWORD nCount = 0;	
    if( !pBmp || !pBmp2)
    {
        return;
    }
    
    if(x1 == x2)
    {
        nuBMPSetPixel565_BMP(pBmp, x1, y, pBmp2, nMode);
        return;
    }
    
    if ( y>=0 && y<pBmp->bmpHeight && !(x1<0 && x2<0) && !( x1>=pBmp->bmpWidth &&  x2>=pBmp->bmpWidth) )
    {
        if (x1<0)
        {
            x1 = 0;
        }
        else if (x1>pBmp->bmpWidth)
        {
            x1 = pBmp->bmpWidth;
        }
        if (x2<0)
        {
            x2 = 0;
        }
        else if (x2>pBmp->bmpWidth)
        {
            x2 = pBmp->bmpWidth;
        }
        nuINT xMin = NURO_MIN(x1,x2);
        nuINT xMax = NURO_MAX(x1,x2);
        for(nCount=xMin; nCount<xMax; ++nCount)
        {
            nuBMPSetPixel565_BMP(pBmp, nCount, y, pBmp2, nMode);
        }
    }
    return;
}
nuVOID CGdiMethodZK::nuBMPDrawLineVer_BMP(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, PNURO_BMP pBmp2, nuINT nMode)
{
    nuWORD nCount = 0;
    if( !pBmp || !pBmp2)
    {
        return;
    }
    
    if(y1 == y2)
    {
        nuBMPSetPixel565_BMP(pBmp, x, y1, pBmp2, nMode);
        return;
    }
    
    if (x >= 0 && x < pBmp->bmpWidth && !(y1<0 && y2<0) 
        && !( y1>=pBmp->bmpHeight &&  y2>=pBmp->bmpHeight) )
    {
        if (y1<0)
        {
            y1 = 0;
        }
        else if (y1>pBmp->bmpHeight)
        {
            y1 = pBmp->bmpHeight;
        }
        if (y2<0)
        {
            y2 = 0;
        }
        else if (y2>pBmp->bmpHeight)
        {
            y2 = pBmp->bmpHeight;
        }
        nuINT yMin = NURO_MIN(y1,y2);
        nuINT yMax = NURO_MAX(y1,y2);
        for(nCount=yMin; nCount<yMax; ++nCount)
        {
            nuBMPSetPixel565_BMP(pBmp, x, nCount, pBmp2, nMode);
        }
    }
    return;
}
nuVOID CGdiMethodZK::nuBMPDrawLine1(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2
                                    , nuCOLORREF color, nuINT nMode)
{
    nuINT dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
    nuWORD color565 = nuCOLOR24TO16_565(color);
    if(pBmp == nuNULL)
    {
        return;
    }
    if ((x1<0 && x2<0)
        || (x1>=pBmp->bmpWidth && x2>=pBmp->bmpWidth)
        || (y1<0 && y2<0)
        || (y1>=pBmp->bmpHeight && y2>=pBmp->bmpHeight) )
    {
        return ;
    }
    dx = NURO_ABS(x2-x1);
    dy = NURO_ABS(y2-y1);
    if( dy==0 && dx==0 )
    {
        nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
        return;
    }
    
    if( dy == 0 )
    {
        nuBMPDrawLineHor(pBmp, x1, x2, y1, color, nMode);
        return;
    }
    
    if( dx == 0 )
    {
        nuBMPDrawLineVer(pBmp, x1, y1, y2, color, nMode);
        return;
    }
    
    if (dy <= dx)
    {
        d = (dy << 1) - dx;
        incr1 = dy << 1;
        incr2 = (dy - dx) << 1;
        if (x1 > x2)
        {
            x = x2;
            y = y2;
            ydirflag = (-1);
            xend = x1;
        }
        else
        {
            x = x1;
            y = y1;
            ydirflag = 1;
            xend = x2;
        }
        nuBMPSetPixel565(pBmp, x, y, color565, nMode);
        if (((y2 - y1) * ydirflag) > 0)
        {
            // ????眻?萰?訇?妠???0
            while (x < xend)
            {
                ++x;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    ++y;
                    d += incr2;
                }
                nuBMPSetPixel565(pBmp, x, y, color565, nMode);
            }
        }
        else
        {
            // ????眻?萰?訇??苤?睇?????0
            while (x < xend)
            {
                ++x;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    --y;
                    d += incr2;
                }
                nuBMPSetPixel565(pBmp, x, y, color565, nMode);
            }
        }
    }
    else
    {
        d = (dx << 1) - dy;
        incr1 = dx << 1;
        incr2 = (dx - dy) << 1;
        if (y1 > y2)
        {
            y = y2;
            x = x2;
            yend = y1;
            xdirflag = (-1);
        }
        else
        {
            y = y1;
            x = x1;
            yend = y2;
            xdirflag = 1;
        }
        nuBMPSetPixel565(pBmp, x, y, color565, nMode);
        if (((x2 - x1) * xdirflag) > 0)
        {
            while (y < yend)
            {
                ++y;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    ++x;
                    d += incr2;
                }
                nuBMPSetPixel565(pBmp, x, y, color565, nMode);
            }
        }
        else
        {
            while (y < yend)
            {
                ++y;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    --x;
                    d += incr2;
                }
                nuBMPSetPixel565(pBmp, x, y, color565, nMode);
            }
        }
    }
    return;
}

nuVOID CGdiMethodZK::nuBMPDrawLine1Ex(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuCOLORREF color, nuINT nMode)
{
    nuINT deltax,   deltay,   start,   finish;   
    double dx,   dy,   dydx;   //   fractional   parts   
    double d1,   d2;  
    nuWORD color565 = nuCOLOR24TO16_565(color);
    deltax   =   NURO_ABS(x2   -   x1);   //   Calculate   deltax   and   deltay   for   initialisation   
    deltay   =   NURO_ABS(y2   -   y1);   
    
    if( deltay==0 && deltax==0 )
    {
        nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
        return;
    }
    
    if( deltay == 0 )
    {
        nuBMPDrawLineHor(pBmp, x1, x2, y1, color, nMode);
        return;
    }
    
    if( deltax == 0 )
    {
        nuBMPDrawLineVer(pBmp, x1, y1, y2, color, nMode);
        return;
    }
    
    if( deltax > deltay) //   then   begin   //   horizontal   or   vertical   
    {   
        if(y2 > y1) //   determine   rise   and   run   
            dydx   =   -((double)deltay   /   deltax);   
        else   
            dydx   =   (double)deltay   /   deltax;   
        if(x2   <   x1)   
        {   
            start   =   x2;   //   right   to   left   
            finish   =   x1;   
            dy   =   y2;   
        }   
        else   
        {   
            start   =   x1;   //   left   to   right   
            finish   =   x2;   
            dy   =   y1;   
            dydx   =   -dydx;   //   inverse   slope   
        }   
        
        for   (nuINT   x   =   start;   x   <=   finish;   ++x)   
        {   
            d1 = (nuINT)dy;
            d2 = dy-d1;
            color565 = nuRGB24TO16_565(
                nuGetRValue(color)*(1-d2)+nuGetRValue(nuBMPGetPixel(pBmp, x, (nuINT)d1))*d2,
                nuGetGValue(color)*(1-d2)+nuGetGValue(nuBMPGetPixel(pBmp, x, (nuINT)d1))*d2,
                nuGetBValue(color)*(1-d2)+nuGetBValue(nuBMPGetPixel(pBmp, x, (nuINT)d1))*d2);
            nuBMPSetPixel565(pBmp, x, (nuINT)d1, color565, NURO_BMP_TYPE_COPY);
            color565 = nuRGB24TO16_565(
                nuGetRValue(color)*d2+nuGetRValue(nuBMPGetPixel(pBmp, x, (nuINT)d1+1))*(1-d2),
                nuGetGValue(color)*d2+nuGetGValue(nuBMPGetPixel(pBmp, x, (nuINT)d1+1))*(1-d2),
                nuGetBValue(color)*d2+nuGetBValue(nuBMPGetPixel(pBmp, x, (nuINT)d1+1))*(1-d2));
            nuBMPSetPixel565(pBmp, x, (nuINT)d1+1, color565, NURO_BMP_TYPE_COPY);
            dy   =   dy   +   dydx;   //   next   point   
        }   
    }   
    else   
    {   
        if(x2   >   x1) //   determine   rise   and   run   
            dydx   =   -((double)deltax   /   deltay);   
        else   
            dydx   =   (double)deltax   /   deltay;   
        
        if(y2   <   y1)   
        {   
            start   =   y2;   //   right   to   left   
            finish   =   y1;   
            dx   =   x2;   
        }   
        else   
        {   
            start   =   y1;   //   left   to   right   
            finish   =   y2;   
            dx   =   x1;   
            dydx   =   -dydx;   //   inverse   slope   
        }   
        
        for(nuINT   y   =   start;   y   <=   finish;   ++y)   
        {   
            
            d1 = (nuINT)dx;
            d2 = dx-d1;  
            color565 = nuRGB24TO16_565(
                nuGetRValue(color)*(1-d2)+nuGetRValue(nuBMPGetPixel(pBmp, (nuINT)d1,   y))*d2,
                nuGetGValue(color)*(1-d2)+nuGetGValue(nuBMPGetPixel(pBmp, (nuINT)d1,   y))*d2,
                nuGetBValue(color)*(1-d2)+nuGetBValue(nuBMPGetPixel(pBmp, (nuINT)d1,   y))*d2 );
            nuBMPSetPixel565(pBmp, (nuINT)d1,   y, color565, NURO_BMP_TYPE_COPY);
            color565 = nuRGB24TO16_565(
                nuGetRValue(color)*d2+(1-d2)*nuGetRValue(nuBMPGetPixel(pBmp, (nuINT)d1+1,   y)),
                nuGetGValue(color)*d2+(1-d2)*nuGetGValue(nuBMPGetPixel(pBmp, (nuINT)d1+1,   y)),
                nuGetBValue(color)*d2+(1-d2)*nuGetBValue(nuBMPGetPixel(pBmp, (nuINT)d1+1,   y)) );
            nuBMPSetPixel565(pBmp, (nuINT)d1+1,   y, color565, NURO_BMP_TYPE_COPY);
            dx   =   dx   +   dydx;   
        }   
    } 
}

nuVOID CGdiMethodZK::nuBMPDrawEllipse1(PNURO_BMP pBmp, nuINT xx1, nuINT yy1, nuINT xx2, nuINT yy2
                                       , nuCOLORREF color, nuINT nMode)
{
    nuINT xx,yy,x1,x2,x3,x4,y1,y2,y3,y4,pk;
    nuINT xc,yc,rx,ry,rx2,ry2,tworx2,twory2,px,py;
    nuWORD color565 = nuCOLOR24TO16_565(color);
    xc = (xx1 + xx2) >> 1;
    yc = (yy1 + yy2) >> 1;
    if(xx1 > xx2)
        rx = (xx1 - xx2) >> 1;
    else
        rx = (xx2 - xx1) >> 1;
    if(yy1 > yy2)
        ry = (yy1 - yy2) >> 1;
    else
        ry = (yy2 - yy1) >> 1;
    rx2 = rx * rx;
    ry2 = ry * ry;
    tworx2 = rx2<<1;
    twory2 = ry2<<1;
    xx = 0;
    yy = ry;
    px = 0;
    py = tworx2 * yy;
    pk = ry2 - (rx2*ry) + (rx2>>2); 
    x1 = xc , y1 = yc + ry;
    x2 = xc , y2 = yc + ry;
    x3 = xc , y3 = yc - ry;
    x4 = xc , y4 = yc - ry;
    nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
    nuBMPSetPixel565(pBmp, x2, y2, color565, nMode);
    nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
    nuBMPSetPixel565(pBmp, x4, y4, color565, nMode);
    while(px < py)
    {	
        ++xx;
        px+= twory2;
        ++x1,--x2,++x3,--x4;
        if(pk < 0)
            pk+= ry2 + px;
        else
        {	
            --yy;
            --y1,--y2,++y3,++y4;
            py -= tworx2;
            pk+= ry2 + px - py;
        }
        nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
        nuBMPSetPixel565(pBmp, x2, y2, color565, nMode);
        nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
        nuBMPSetPixel565(pBmp, x4, y4, color565, nMode);
    }
    pk = (nuINT)nuSqrt(((ry2*(xx*2+1)*(xx*2+1))>>2) + rx2*(yy-1)*(yy-1) - rx2*ry2);
    while(yy>0)
    {	
        --yy;
        --y1,--y2,++y3,++y4;
        py-=tworx2;
        if(pk>0)
        {	pk+=rx2 - py;
        }
        else
        {	
            ++xx;
            ++x1,--x2,++x3,--x4;
            px+=twory2;
            pk+=rx2-py+px;
        }
        nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
        nuBMPSetPixel565(pBmp, x2, y2, color565, nMode);
        nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
        nuBMPSetPixel565(pBmp, x4, y4, color565, nMode);
    }
}
nuVOID CGdiMethodZK::nuBMPDrawCircle1(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT radius, nuCOLORREF color, nuINT nMode)
{
    nuINT xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
    nuWORD color565 = nuCOLOR24TO16_565(color);
    if(pBmp == nuNULL)
    {
        return;
    }
    xx = 0;
    yy = radius;
    x1 = x , y1 = y + radius;
    x2 = x , y2 = y + radius;
    x3 = x , y3 = y - radius;
    x4 = x , y4 = y - radius;
    x5 = x + radius, y5 = y;
    x6 = x - radius, y6 = y;
    x7 = x + radius, y7 = y;
    x8 = x - radius, y8 = y;
    pk = 1 - radius;
    
    nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
    nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
    nuBMPSetPixel565(pBmp, x5, y5, color565, nMode);
    nuBMPSetPixel565(pBmp, x7, y7, color565, nMode);
    
    while(xx < yy)
    {	
        ++xx;
        ++x1,--x2,++x3,--x4;
        ++y5,++y6,--y7,--y8;
        if(pk < 0)
        {
            pk+= (xx<<1) + 1;
        }
        else
        {
            --yy;
            --y1,--y2,++y3,++y4;
            --x5,++x6,--x7,++x8;
            pk+= 2*(xx - yy) + 1;
        }
        nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
        nuBMPSetPixel565(pBmp, x2, y2, color565, nMode);
        nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
        nuBMPSetPixel565(pBmp, x4, y4, color565, nMode);
        nuBMPSetPixel565(pBmp, x5, y5, color565, nMode);
        nuBMPSetPixel565(pBmp, x6, y6, color565, nMode);
        nuBMPSetPixel565(pBmp, x7, y7, color565, nMode);
        nuBMPSetPixel565(pBmp, x8, y8, color565, nMode);
    }
}
// ????珨??????峈thick??眻?葍?
nuVOID CGdiMethodZK::nuBMPDrawLine(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2
                                   , nuINT thick, nuCOLORREF color, nuINT nMode)
{
#ifdef NURO_USE_AALINE
    nuBMPDrawLineEx(pBmp, x1, y1, x2, y2, thick, color, nMode);
    return ;
#endif
    
    if(pBmp == nuNULL)
    {
        return;
    }
    if (!thick)
    {
        return;
    }
    if (thick==1)
    {
        nuBMPDrawLine1(pBmp,x1, y1, x2, y2, color, nMode);
        return ;
    }
    if ((x1<0 && x2<0)
        || (x1>=pBmp->bmpWidth && x2>=pBmp->bmpWidth)
        || (y1<0 && y2<0)
        || (y1>=pBmp->bmpHeight && y2>=pBmp->bmpHeight) )
    {
        return ;
    }
    nuINT dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
    nuINT wid;
    nuINT wstart;
    dx = NURO_ABS(x2-x1);
    dy = NURO_ABS(y2-y1);
    if( !dy && !dx)
    {
        nuBMPFillCircle(pBmp, x1, y1, (thick-1)>>1, color, nMode);
        return;
    }
    if( !dy )
    {
        nuBMPFillRect(pBmp, NURO_MIN(x1,x2), y1-((thick)>>1), dx, thick, color, nMode);
        return;
    }
    if( !dx )
    {
        nuBMPFillRect(pBmp, x1-((thick)>>1), NURO_MIN(y1,y2), thick, dy, color, nMode);
        return;
    }
    if (dy <= dx)
    {
        wid = thick + thick * dy / (dx<<1);
        if (wid == 0)
        {
            wid = 1;
        }
        d = (dy << 1) - dx;
        incr1 = dy << 1;
        incr2 = (dy - dx) << 1;
        if (x1 > x2)
        {
            x = x2;
            y = y2;
            ydirflag = (-1);
            xend = x1;
        }
        else
        {
            x = x1;
            y = y1;
            ydirflag = 1;
            xend = x2;
        }
        if (((y2 - y1) * ydirflag) > 0)
        {
            // ????眻?萰?訇?妠???0
            while (x < xend)
            {
                wstart = y - (wid >> 1);
                nuBMPDrawLineVer(pBmp, x, wstart, wstart + wid, color, nMode);
                ++x;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    ++y;
                    d += incr2;
                }
            }
        }
        else
        {
            while (x < xend)
            {
                wstart = y - (wid >> 1);
                nuBMPDrawLineVer(pBmp, x, wstart, wstart + wid, color, nMode);
                ++x;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    --y;
                    d += incr2;
                }
            }
        }
        
    }
    else
    {
        wid = thick + thick * dx / (dy<<1);
        if (wid == 0)
        {
            wid = 1;
        }
        d = (dx << 1) - dy;
        incr1 = dx << 1;
        incr2 = (dx - dy) << 1;
        if (y1 > y2)
        {
            y = y2;
            x = x2;
            yend = y1;
            xdirflag = (-1);
        }
        else
        {
            y = y1;
            x = x1;
            yend = y2;
            xdirflag = 1;
        }
        if (((x2 - x1) * xdirflag) > 0)
        {
            while (y < yend)
            {
                wstart = x - (wid >> 1);
                nuBMPDrawLineHor(pBmp, wstart, wstart + wid, y, color, nMode);
                ++y;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    ++x;
                    d += incr2;
                }
            }
        }
        else
        {
            while (y < yend)
            {
                wstart = x - (wid >> 1);
                nuBMPDrawLineHor(pBmp, wstart, wstart + wid, y, color, nMode);
                ++y;
                if (d < 0)
                {
                    d += incr1;
                }
                else
                {
                    --x;
                    d += incr2;
                }
            }
        }
    }
}

// ????珨??????峈thick??眻?葍?
nuVOID CGdiMethodZK::nuBMPDrawLineEx(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode)
{
    nuBMPDrawLineExL(pBmp,x1,y1,x2,y2,thick,color,nMode);
    return;
    nuINT dx, dy;
    nuINT wid,halfwid;
    nuINT start,finish,dydx,d1,d2,tempX,tempY;
    nuWORD color565;
    nuCOLORREF ColorNow;
    
    if(pBmp == nuNULL){
        return;
    }
    
    if (!thick){
        return;
    }
    
    if( !nuLineLimitToRectZ(x1, x2, y1, y2, 0, pBmp->bmpWidth-1, 0, pBmp->bmpHeight-1) )
    {
        return ;
    }
    
    dx = NURO_ABS(x2-x1);
    dy = NURO_ABS(y2-y1);
    
    //Louis ???w???s?d??
    if(dx>2000) dx=2000;
    if(dy>2000) dy=2000;
    
    if( !dy && !dx )
    {//?I
        nuBMPFillCircle(pBmp, x1, y1, (thick-1)>>1, color, nMode);
        return;
    }
    /**/
    if( !dy )
    {
        nuBMPFillRect(pBmp, NURO_MIN(x1,x2), y1-((thick)>>1), dx, thick, color, nMode);
        return;
    }
    if( !dx )
    {
        nuBMPFillRect(pBmp, x1-((thick)>>1), NURO_MIN(y1,y2), thick, dy, color, nMode);
        return;
    }
    
    if( dy <= dx ){ //   then   begin   //   horizontal   or   vertical
        wid = thick + thick * dy / (dx<<1);//y?b?e??
        halfwid = wid >> 1;
        
        if(y2 > y1) //   determine   rise   and   run   
            dydx   =   -((dy<<MOVESCALE)   /   dx);   
        else   
            dydx   =   (dy<<MOVESCALE)   /   dx;   
        
        if(x2   <   x1){   
            start   =   x2;   //   right   to   left   
            finish   =   x1;   
            dy   =   y2<<MOVESCALE;   
        }   
        else{   
            start   =   x1;   //   left   to   right   
            finish   =   x2;   
            dy   =   y1<<MOVESCALE;   
            dydx   =   -dydx;   //   inverse   slope   
        }   
        
        for   (nuINT   x   =   start;   x   <=   finish;   ++x)
        {   
            d1 = (dy>>MOVESCALE)<<MOVESCALE;
            d2 = dy-d1;
            
            tempX=x;
            tempY=(d1>>MOVESCALE)-halfwid;
            if( tempX>=0				&&
                tempX<pBmp->bmpWidth	&&
                tempY>=0				&&
                tempY<pBmp->bmpHeight	)
            {
                ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                color565 = nuRGB24TO16_565(
                    (nuGetRValue(color)*((1<<MOVESCALE)-d2)+nuGetRValue(ColorNow)*d2)>>MOVESCALE,
                    (nuGetGValue(color)*((1<<MOVESCALE)-d2)+nuGetGValue(ColorNow)*d2)>>MOVESCALE,
                    (nuGetBValue(color)*((1<<MOVESCALE)-d2)+nuGetBValue(ColorNow)*d2)>>MOVESCALE);
                nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
            }
            
            if((tempY+1)<(tempY+wid))
            {
                nuBMPDrawLineVer(pBmp, tempX, tempY+1, tempY+wid, color, nMode);
            }
            
            tempX=x;
            tempY=tempY+wid;
            if( tempX>=0				&&
                tempX<pBmp->bmpWidth	&&
                tempY>=0				&&
                tempY<pBmp->bmpHeight	)
            {
                ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                color565 = nuRGB24TO16_565(
                    (nuGetRValue(color)*d2+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                    (nuGetGValue(color)*d2+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                    (nuGetBValue(color)*d2+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE);
                nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
            }
            dy   =   dy   +   dydx;   //   next   point   
        }   
    }   
    else   
    {   
        wid = thick + thick * dx / (dy<<1);//y?b?e??
        halfwid = wid >> 1;
        
        if(x2 > x1) //   determine   rise   and   run   
            dydx   =   -((dx<<MOVESCALE)   /   dy);   
        else   
            dydx   =   (dx<<MOVESCALE)   /   dy;   
        
        if(y2   <   y1)   
        {   
            start   =   y2;   //   right   to   left   
            finish   =   y1;   
            dx   =   x2<<MOVESCALE;   
        }   
        else   
        {   
            start   =   y1;   //   left   to   right   
            finish   =   y2;   
            dx   =   x1<<MOVESCALE;   
            dydx   =   -dydx;   //   inverse   slope   
        }   
        
        for(nuINT   y   =   start;   y   <=   finish;   ++y)   
        {   
            d1 = (dx>>MOVESCALE)<<MOVESCALE;
            d2 = dx-d1;
            
            tempX=(d1>>MOVESCALE)-halfwid;
            tempY=y;
            if( tempX>=0				&&
                tempX<pBmp->bmpWidth	&&
                tempY>=0				&&
                tempY<pBmp->bmpHeight	)
            {
                ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                color565 = nuRGB24TO16_565(
                    (nuGetRValue(color)*((1<<MOVESCALE)-d2)+nuGetRValue(ColorNow)*d2)>>MOVESCALE,
                    (nuGetGValue(color)*((1<<MOVESCALE)-d2)+nuGetGValue(ColorNow)*d2)>>MOVESCALE,
                    (nuGetBValue(color)*((1<<MOVESCALE)-d2)+nuGetBValue(ColorNow)*d2)>>MOVESCALE);
                nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
            }
            
            if((tempX+1)<(tempX+wid))
            {
                nuBMPDrawLineHor(pBmp, tempX+1, tempX+wid, tempY, color, nMode);
            }
            
            tempX=tempX+wid;
            tempY=y;
            if( tempX>=0				&&
                tempX<pBmp->bmpWidth	&&
                tempY>=0				&&
                tempY<pBmp->bmpHeight	)
            {
                ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                color565 = nuRGB24TO16_565(
                    (nuGetRValue(color)*d2+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                    (nuGetGValue(color)*d2+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                    (nuGetBValue(color)*d2+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE);
                nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
            }
            dx   =   dx   +   dydx;   
        }   
    } 
}

//?峔荈i???狤鷑有B?z???sAPI
nuVOID CGdiMethodZK::nuBMPDrawLineExL(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode)
{
    nuINT dx, dy;
    nuINT wid,halfwid;
    nuINT start,finish,dydx,d2;//,d1,tempX,tempY
    nuWORD color565;
    nuCOLORREF ColorNow;
    
    nuINT S_UP,S_Down,E_UP,E_Down;
    
    if(pBmp == nuNULL)
    {
        return;
    }
    
    if (!thick)
    {
        return;
    }
    
    if( !nuLineLimitToRectZ(x1, x2, y1, y2, 0, pBmp->bmpWidth-1, 0, pBmp->bmpHeight-1) )
    {
        return ;
    }
    
    dx = NURO_ABS(x2-x1);
    dy = NURO_ABS(y2-y1);
    
    //Louis ???w???s?d??
    if(dx>2000) dx=2000;
    if(dy>2000) dy=2000;
    
    
    if( !dy && !dx )
    {//?I
        nuBMPFillCircle(pBmp, x1, y1, (thick-1)>>1, color, nMode);
        return;
    }
    
    if( dy <= dx ){ //   then   begin   //   horizontal   or   vertical
        if(nulCos(nulAtan2(dy,dx))==0)
        {
            wid = thick* (1<<MOVESCALE);//?e?? 1024???@????
        }
        else
        {
            wid = (thick* (1<<MOVESCALE)*nuGetlBase()) /nulCos(nulAtan2(dy,dx));//?e?? 1024???@????
        }
        
        halfwid = wid >> 1;
        
        S_UP	=	halfwid;
        E_UP	=	halfwid;
        S_Down	=	-halfwid;
        E_Down	=	-halfwid;
        
        if(y2 > y1) //   determine   rise   and   run   
            dydx   =   -((dy<<MOVESCALE)   /   dx);   
        else
            dydx   =   (dy<<MOVESCALE)   /   dx;   
        
        if(x2   <   x1){   
            start   =   x2;   //   right   to   left   
            finish   =   x1;   
            dy   =   y2<<MOVESCALE;   
        }
        else{   
            start   =   x1;   //   left   to   right   
            finish   =   x2;   
            dy   =   y1<<MOVESCALE;   
            dydx   =   -dydx;   //   inverse   slope   
        }
        
        S_UP	=	dy+S_UP;
        S_Down	=	dy+S_Down;
        E_UP	=	dy+E_UP;
        E_Down	=	dy+E_Down;
        
        nuINT SNum,ENum,N_UpNum,N_DownNum;
        nuINT S_Int,E_Int;
        nuINT L_U,L_D,R_U,R_D;
        
        //        for   (nuINT   x   =   start;   x   <=   finish;   x++){
        for   (nuINT   x   =   start;   x   <   finish;   ++x){
            E_UP	=	dydx+E_UP;
            E_Down	=	dydx+E_Down;
            
            //?w?X?怜??怬C???[?\?d??
            //?繫????A?????珔i???B?z
            if(E_UP<S_UP)		SNum=S_UP;
            else				SNum=E_UP;
            if(E_Down<S_Down)	ENum=E_Down;
            else				ENum=S_Down;
            
            S_Int = SNum>>MOVESCALE;
            E_Int = ENum>>MOVESCALE;
            for(nuINT y=S_Int;y>=E_Int;y--)
            {
                N_UpNum=((y+1)<<MOVESCALE);
                N_DownNum=(y<<MOVESCALE);
                
                //?w?q?W???P?U?????|????
                if(S_UP<N_UpNum)		L_U=S_UP;
                else if(S_Down>N_UpNum)	L_U=S_Down;
                else					L_U=N_UpNum;
                
                if(S_UP<N_DownNum)			L_D=S_UP;
                else if(S_Down>N_DownNum)	L_D=S_Down;
                else						L_D=N_DownNum;
                
                if(E_UP<N_UpNum)		R_U=E_UP;
                else if(E_Down>N_UpNum)	R_U=E_Down;
                else					R_U=N_UpNum;
                
                if(E_UP<N_DownNum)			R_D=E_UP;
                else if(E_Down>N_DownNum)	R_D=E_Down;
                else						R_D=N_DownNum;
                
                d2=((L_U-L_D)+(R_U-R_D))/2;
                
                if( x>=0				&&
                    x<pBmp->bmpWidth	&&
                    y>=0				&&
                    y<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, x, y);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*d2+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                        (nuGetGValue(color)*d2+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                        (nuGetBValue(color)*d2+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, x, y, color565, NURO_BMP_TYPE_COPY);
                }
            }
            
            S_UP	=	dydx+S_UP;
            S_Down	=	dydx+S_Down;
        }
    }
    else
    {
        if(nulCos(nulAtan2(dx,dy))==0)
        {
            wid = thick* (1<<MOVESCALE);//?e?? 1024???@????
        }
        else
        {
            wid = (thick* (1<<MOVESCALE)*nuGetlBase()) /nulCos(nulAtan2(dx,dy));//?e?? 1024???@????
        }
        halfwid = wid >> 1;
        
        S_UP	=	halfwid;
        E_UP	=	halfwid;
        S_Down	=	-halfwid;
        E_Down	=	-halfwid;
        
        if(x2 > x1) //   determine   rise   and   run   
            dydx   =   -((dx<<MOVESCALE)   /   dy);   
        else
            dydx   =   (dx<<MOVESCALE)   /   dy;   
        
        if(y2   <   y1)
        {   
            start   =   y2;   //   right   to   left   
            finish   =   y1;   
            dx   =   x2<<MOVESCALE;   
        }
        else
        {   
            start   =   y1;   //   left   to   right   
            finish   =   y2;   
            dx   =   x1<<MOVESCALE;   
            dydx   =   -dydx;   //   inverse   slope   
        }
        
        S_UP	=	dx+S_UP;
        S_Down	=	dx+S_Down;
        E_UP	=	dx+E_UP;
        E_Down	=	dx+E_Down;
        
        nuINT SNum,ENum,N_UpNum,N_DownNum;
        nuINT S_Int,E_Int;
        nuINT L_U,L_D,R_U,R_D;
        
        //        for   (nuINT   y   =   start;   y   <=   finish;   y++){
        for   (nuINT   y   =   start;   y   <   finish;   ++y)
        {
            E_UP	=	dydx+E_UP;
            E_Down	=	dydx+E_Down;
            
            //?w?X?怜??怬C???[?\?d??
            //?繫????A?????珔i???B?z
            if(E_UP<S_UP)		SNum=S_UP;
            else				SNum=E_UP;
            if(E_Down<S_Down)	ENum=E_Down;
            else				ENum=S_Down;
            
            S_Int = SNum>>MOVESCALE;
            E_Int = ENum>>MOVESCALE;
            
            for(nuINT x=S_Int; x>=E_Int; --x)
            {
                N_UpNum=((x+1)<<MOVESCALE);
                N_DownNum=(x<<MOVESCALE);
                
                //?w?q?W???P?U?????|????
                if(S_UP<N_UpNum)		L_U=S_UP;
                else if(S_Down>N_UpNum)	L_U=S_Down;
                else					L_U=N_UpNum;
                
                if(S_UP<N_DownNum)			L_D=S_UP;
                else if(S_Down>N_DownNum)	L_D=S_Down;
                else						L_D=N_DownNum;
                
                if(E_UP<N_UpNum)		R_U=E_UP;
                else if(E_Down>N_UpNum)	R_U=E_Down;
                else					R_U=N_UpNum;
                
                if(E_UP<N_DownNum)			R_D=E_UP;
                else if(E_Down>N_DownNum)	R_D=E_Down;
                else						R_D=N_DownNum;
                
                d2=((L_U-L_D)+(R_U-R_D))/2;
                
                if( x>=0				&&
                    x<pBmp->bmpWidth	&&
                    y>=0				&&
                    y<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, x, y);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*d2+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                        (nuGetGValue(color)*d2+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE,
                        (nuGetBValue(color)*d2+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d2))>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, x, y, color565, NURO_BMP_TYPE_COPY);
                }
            }
            
            S_UP	=	dydx+S_UP;
            S_Down	=	dydx+S_Down;
        }
    }
}

nuVOID CGdiMethodZK:: nuBMPDrawLineYT3DEx(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick0, nuINT thickD, nuCOLORREF color, nuINT nMode)
{   
    if(!pBmp)
    {
        return;
    }
    nuINT dx, dy,dx2, dy2;
    nuINT wid,halfwid;
    nuINT start,finish,dydx,difdydx,d1,d2,tempX,tempY,d12,d22,tempX2,tempY2;
    nuWORD color565;
    nuCOLORREF ColorNow; 
    
    if(!thick0)
    {
        if (thickD)
        {
            ++thick0;
            --thickD;
        }
        else
        {
            return;
        }
    }
    if (thickD<1)
    {
        nuBMP565FillCircleZ(pBmp, x1, y1, thick0-1, nuCOLOR24TO16_565(color), nMode);
        if (thick0>3)
        {
            nuBMP565DrawLineZ(pBmp, x1, y1, x2, y2, thick0-1, nuCOLOR24TO16_565(color), nMode);
        }
        else
        {
            nuBMPDrawLineEx(pBmp, x1, y1, x2, y2, thick0, color, nMode);
        }		
        nuBMP565FillCircleZ(pBmp, x2, y2, thick0-1, nuCOLOR24TO16_565(color), nMode);
    }
    if((x1<0 && x2<0)								||
        (x1>=pBmp->bmpWidth && x2>=pBmp->bmpWidth)	||
        (y1<0 && y2<0)								||
        (y1>=pBmp->bmpHeight && y2>=pBmp->bmpHeight) )
    {
        return ;
    }
    
    dx = NURO_ABS(x2-x1);
    dy = NURO_ABS(y2-y1);
    
    //Louis ???w???s?d??
    if(dx>2000) dx=2000;
    if(dy>2000) dy=2000;
    
    if( !dy && !dx){//?I
        nuBMPFillCircle(pBmp, x1, y1, thick0>>1, color, nMode);
        return;
    }
    // 	if (!dy)
    // 	{
    // 		return;
    // 	}
    // 	if (!dx)
    // 	{
    // 		return ;
    // 	}
    if (y1<y2)
    {
        nuBMPFillCircle(pBmp, x1, y1, (thick0-2)>>1, color, nMode);
        nuBMPFillCircle(pBmp, x2, y2, (thick0+thickD-2)>>1, color, nMode);
    }
    else
    {
        nuBMPFillCircle(pBmp, x2, y2, (thick0-2)>>1, color, nMode);
        nuBMPFillCircle(pBmp, x1, y1, (thick0+thickD-2)>>1, color, nMode);
    }
    if( dy <= dx )
    { //   then   begin   //   horizontal   or   vertical
        //?o?怐??B?z?|?J?????D ?]???妦茈D?n?O?]??y???雂珜y????
        //???O?o?荍@?峎O?Hx???D ?o?侇|?y?????D ?搨n?h?@?I???U?~??
        wid = thick0 + thick0 * dy / (dx<<1);//y?b?e??
        halfwid = wid >> 1;
        difdydx=(thickD<<(MOVESCALE-1))/dx;
        
        if(y2 > y1)
        { //   determine   rise   and   run
            dydx   =   (dy<<MOVESCALE)   /   (x2-x1);  
            
            start   =   x1;   //   left   to   right   
            finish   =   x2;   
            dy   =   y1<<MOVESCALE;   
            dy2	=	dy;
        }
        else
        {   
            dydx   =   (dy<<MOVESCALE)   /   (x1-x2);   
            
            start   =   x2;   //   right   to   left   
            finish   =   x1;   
            dy   =   y2<<MOVESCALE;
            dy2	=	dy;
        }
        
        if(start   <   finish)
        {   
            for   (nuINT   x   =   start;   x   <=   finish;   ++x){   
                d1 = (dy>>MOVESCALE)<<MOVESCALE;
                d2 = dy-d1;
                
                d12 = (dy2>>MOVESCALE)<<MOVESCALE;
                d22 = dy2-d12;
                
                tempX=x;
                tempY=(d1>>MOVESCALE)-halfwid;
                tempX2=x;
                tempY2=(d12>>MOVESCALE)-halfwid+wid;
                
                if( tempX>=0				&&
                    tempX<pBmp->bmpWidth	&&
                    tempY>=0				&&
                    tempY<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*((1<<MOVESCALE)-d2)+nuGetRValue(ColorNow)*d2)>>MOVESCALE,
                        (nuGetGValue(color)*((1<<MOVESCALE)-d2)+nuGetGValue(ColorNow)*d2)>>MOVESCALE,
                        (nuGetBValue(color)*((1<<MOVESCALE)-d2)+nuGetBValue(ColorNow)*d2)>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
                }
                
                if((tempY+1)<tempY2)
                {
                    nuBMPDrawLineVer(pBmp, tempX, tempY+1, tempY2, color, nMode);
                }
                
                if( tempX2>=0				&&
                    tempX2<pBmp->bmpWidth	&&
                    tempY2>=0				&&
                    tempY2<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, tempX2, tempY2);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*d22+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                        (nuGetGValue(color)*d22+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                        (nuGetBValue(color)*d22+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, tempX2, tempY2, color565, NURO_BMP_TYPE_COPY);
                }
                dy   =   dy   +   dydx-difdydx;   //   next   point   
                dy2   =   dy2   +   dydx+difdydx;   //   next   point   
            }   
        }   
        else{
            
            for   (nuINT   x   =   start;   x   >   finish;   --x)
            {   
                d1 = (dy>>MOVESCALE)<<MOVESCALE;
                d2 = dy-d1;
                
                d12 = (dy2>>MOVESCALE)<<MOVESCALE;
                d22 = dy2-d12;
                
                tempX=x;
                tempY=(d1>>MOVESCALE)-halfwid;
                tempX2=x;
                tempY2=(d12>>MOVESCALE)-halfwid+wid;
                
                if( tempX>=0				&&
                    tempX<pBmp->bmpWidth	&&
                    tempY>=0				&&
                    tempY<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*((1<<MOVESCALE)-d2)+nuGetRValue(ColorNow)*d2)>>MOVESCALE,
                        (nuGetGValue(color)*((1<<MOVESCALE)-d2)+nuGetGValue(ColorNow)*d2)>>MOVESCALE,
                        (nuGetBValue(color)*((1<<MOVESCALE)-d2)+nuGetBValue(ColorNow)*d2)>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
                }
                
                if((tempY+1)<tempY2)
                {
                    nuBMPDrawLineVer(pBmp, tempX, tempY+1, tempY2, color, nMode);
                }
                
                if( tempX2>=0				&&
                    tempX2<pBmp->bmpWidth	&&
                    tempY2>=0				&&
                    tempY2<pBmp->bmpHeight	)
                {
                    ColorNow=nuBMPGetPixel(pBmp, tempX2, tempY2);
                    color565 = nuRGB24TO16_565(
                        (nuGetRValue(color)*d22+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                        (nuGetGValue(color)*d22+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                        (nuGetBValue(color)*d22+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE);
                    nuBMPSetPixel565(pBmp, tempX2, tempY2, color565, NURO_BMP_TYPE_COPY);
                }
                dy   =   dy   -   dydx-difdydx;   //   next   point   
                dy2   =   dy2   -   dydx+difdydx;   //   next   point   
            }   
        }  
}   
else   
{
    wid = thick0 + thick0 * dx / (dy<<1);//y?b?e??
    halfwid = wid >> 1;
    difdydx=(thickD<<(MOVESCALE-1))/dy;
    
    if(x2 > x1) //   determine   rise   and   run   
        dydx   =   -((dx<<MOVESCALE)   /   dy);   
    else   
        dydx   =   (dx<<MOVESCALE)   /   dy;   
    
    
    if(y2   <   y1)   
    {   
        start   =   y2;   //   right   to   left   
        finish   =   y1;   
        dx   =   x2<<MOVESCALE;   
        dx2	= dx;
    }   
    else   
    {   
        start   =   y1;   //   left   to   right   
        finish   =   y2;   
        dx   =   x1<<MOVESCALE;   
        dx2	= dx;
        dydx   =   -dydx;   //   inverse   slope   
    }   
    
    for(nuINT   y   =   start;   y   <=   finish;   ++y)   
    {   
        d1 = (dx>>MOVESCALE)<<MOVESCALE;
        d2 = dx-d1;
        
        d12 = (dx2>>MOVESCALE)<<MOVESCALE;
        d22 = dx2-d12;
        
        tempX=(d1>>MOVESCALE)-halfwid;
        tempY=y;
        tempX2=(d12>>MOVESCALE)-halfwid+wid;
        tempY2=y;
        if( tempX>=0				&&
            tempX<pBmp->bmpWidth	&&
            tempY>=0				&&
            tempY<pBmp->bmpHeight	)
        {
            ColorNow=nuBMPGetPixel(pBmp, tempX, tempY);
            color565 = nuRGB24TO16_565(
                (nuGetRValue(color)*((1<<MOVESCALE)-d2)+nuGetRValue(ColorNow)*d2)>>MOVESCALE,
                (nuGetGValue(color)*((1<<MOVESCALE)-d2)+nuGetGValue(ColorNow)*d2)>>MOVESCALE,
                (nuGetBValue(color)*((1<<MOVESCALE)-d2)+nuGetBValue(ColorNow)*d2)>>MOVESCALE);
            nuBMPSetPixel565(pBmp, tempX, tempY, color565, NURO_BMP_TYPE_COPY);
        }
        
        if((tempX+1)<tempX2)
        {
            nuBMPDrawLineHor(pBmp, tempX+1, tempX2, tempY, color, nMode);
        }
        
        if( tempX2>=0				&&
            tempX2<pBmp->bmpWidth	&&
            tempY2>=0				&&
            tempY2<pBmp->bmpHeight	)
        {
            ColorNow=nuBMPGetPixel(pBmp, tempX2, tempY2);
            color565 = nuRGB24TO16_565(
                (nuGetRValue(color)*d22+nuGetRValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                (nuGetGValue(color)*d22+nuGetGValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE,
                (nuGetBValue(color)*d22+nuGetBValue(ColorNow)*((1<<MOVESCALE)-d22))>>MOVESCALE);
            nuBMPSetPixel565(pBmp, tempX2, tempY2, color565, NURO_BMP_TYPE_COPY);
        }
        dx   =   dx   +   dydx-difdydx;   
        dx2   =   dx2   +   dydx+difdydx;   
    }   
} 
}

nuVOID CGdiMethodZK::nuBMPDrawEllipse(PNURO_BMP pBmp, nuINT xx1, nuINT yy1, nuINT xx2, nuINT yy2
                                      , nuINT thick, nuCOLORREF color, nuINT nMode)
{
    if(pBmp == nuNULL || thick < 1)
    {
        return;
    }
    nuINT thickP2 = thick>>1;
    if(thick%2 == 0)
    {
        nuBMPDrawEllipse(pBmp, xx1, yy1, xx2, yy2, thick-1, color, nMode);
        nuBMPDrawEllipse1(pBmp, xx1-thickP2, yy1-thickP2, xx2+thickP2, yy2+thickP2, color, nMode);
    }
    if(thick == 1)
    {
        nuBMPDrawEllipse1(pBmp, xx1, yy1, xx2, yy2, color, nMode);
        return;
    }
    nuINT xx,yy,x1,x2,x3,x4,y1,y2,y3,y4,pk;
    nuINT xc,yc,rx,ry,rx2,ry2,tworx2,twory2,px,py;
    xc = (xx1 + xx2) >> 1;
    yc = (yy1 + yy2) >> 1;
    if(xx1 > xx2)
        rx = (xx1 - xx2) >> 1;
    else
        rx = (xx2 - xx1) >> 1;
    if(yy1 > yy2)
        ry = (yy1 - yy2) >> 1;
    else
        ry = (yy2 - yy1) >> 1;
    rx2 = rx * rx;
    ry2 = ry * ry;
    tworx2 = rx2<<1;
    twory2 = ry2<<1;
    xx = 0;
    yy = ry;
    px = 0;
    py = tworx2 * yy;
    pk = ry2 - (rx2*ry) + (rx2>>2); 
    x1 = xc , y1 = yc + ry;
    x2 = xc , y2 = yc + ry;
    x3 = xc , y3 = yc - ry;
    x4 = xc , y4 = yc - ry;
    nuBMPFillCircle(pBmp, x1, y1, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x2, y2, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x3, y3, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x4, y4, thickP2, color, nMode);
    while(px < py)
    {	
        ++xx;
        px+= twory2;
        ++x1,--x2,++x3,--x4;
        if(pk < 0)
            pk+= ry2 + px;
        else
        {	
            --yy;
            --y1,--y2,++y3,++y4;
            py -= tworx2;
            pk+= ry2 + px - py;
        }
        nuBMPFillCircle(pBmp, x1, y1, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x2, y2, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x3, y3, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x4, y4, thickP2, color, nMode);
    }
    pk = (nuINT)nuSqrt(((ry2*(xx*2+1)*(xx*2+1))>>2) + rx2*(yy-1)*(yy-1) - rx2*ry2);
    while(yy>0)
    {	
        --yy;
        --y1,--y2,++y3,++y4;
        py-=tworx2;
        if(pk>0)
        {	pk+=rx2 - py;
        }
        else
        {	
            ++xx;
            ++x1,--x2,++x3,--x4;
            px+=twory2;
            pk+=rx2-py+px;
        }
        nuBMPFillCircle(pBmp, x1, y1, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x2, y2, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x3, y3, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x4, y4, thickP2, color, nMode);
    }
}
nuVOID CGdiMethodZK::nuBMPDrawCircle(PNURO_BMP pBmp, nuINT x, nuINT y
                                     , nuINT radius, nuINT thick, nuCOLORREF color, nuINT nMode)
{
    if(pBmp == nuNULL)
    {
        return;
    }
    nuINT thickP2 = thick>>1;
    if(thick%2 == 0)
    {
        nuBMPDrawCircle(pBmp, x, y, radius, thick-1, color, nMode);
        nuBMPDrawCircle1(pBmp, x, y, radius+thickP2, color, nMode);
    }
    
    if(thick == 1)
    {
        nuBMPDrawCircle1(pBmp, x, y, radius, color, nMode);
        return;
    }
    nuINT xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
    
    if(pBmp == nuNULL)
    {
        return;
    }
    
    xx = 0;
    yy = radius;
    x1 = x , y1 = y + radius;
    x2 = x , y2 = y + radius;
    x3 = x , y3 = y - radius;
    x4 = x , y4 = y - radius;
    x5 = x + radius, y5 = y;
    x6 = x - radius, y6 = y;
    x7 = x + radius, y7 = y;
    x8 = x - radius, y8 = y;
    pk = 1 - radius;
    
    nuBMPFillCircle(pBmp, x1, y1, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x3, y3, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x5, y5, thickP2, color, nMode);
    nuBMPFillCircle(pBmp, x7, y7, thickP2, color, nMode);
    while(xx < yy)
    {	
        ++xx;
        ++x1,--x2,++x3,--x4;
        ++y5,++y6,--y7,--y8;
        if(pk < 0)
            pk+= 2*xx + 1;
        else
        {	
            --yy;
            --y1,--y2,++y3,++y4;
            --x5,++x6,--x7,++x8;
            pk+= 2*(xx - yy) + 1;
        }
        nuBMPFillCircle(pBmp, x1, y1, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x2, y2, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x3, y3, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x4, y4, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x5, y5, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x6, y6, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x7, y7, thickP2, color, nMode);
        nuBMPFillCircle(pBmp, x8, y8, thickP2, color, nMode);
    }
}

nuVOID CGdiMethodZK::nuBMPDrawRectLine(PNURO_BMP pBmp, nuINT left, nuINT top, nuINT w, nuINT h
                                       , nuINT thick, nuCOLORREF color, nuINT nMode)
{
    if( pBmp==nuNULL || w <1 || h<1 )
    {
        return;
    }
    nuBMPDrawLine(pBmp, left, top, left, top+h, thick, color, nMode);
    nuBMPDrawLine(pBmp, left, top, left+w, top, thick, color, nMode);
    nuBMPDrawLine(pBmp, left+w, top, left+w, top+h, thick, color, nMode);
    nuBMPDrawLine(pBmp, left, top+h, left+w, top+h, thick, color, nMode);
}
nuVOID CGdiMethodZK::nuBMPFillGon(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n
                                  , nuCOLORREF color, nuCOLORREF edgeColor, nuINT nMode)
{
    nuINT i;
    nuINT y;
    nuINT miny, maxy;
    nuINT ind1, ind2;
    nuINT ints;
    nuINT* pPolyInts = nuNULL;
    nuINT* pPolyIntsTag = nuNULL;
    nuINT tmpPolyInt = 0;
    nuINT nCount1 = 0;
    nuINT nCount2 = 0;
    PNUROPOINT tmpPoint = nuNULL;
    
    if (pBmp==nuNULL || n<3)
    {
        return;
    }
    
    
    nuINT inc = 0;
    if( (pPoint[0].x != pPoint[n-1].x) || (pPoint[0].y != pPoint[n-1].y))
    {
        inc = 1;
    }
    
    pPolyInts = (nuINT*)nuMalloc(sizeof(nuINT) * n);
    if (pPolyInts == nuNULL)
    {
        return;
    }
    pPolyIntsTag = (nuINT*)nuMalloc(sizeof(nuINT) * n);
    if (pPolyIntsTag == nuNULL)
    {
        nuFree(pPolyInts);
        return;
    }
    
    miny = pPoint[0].y;
    maxy = pPoint[0].y;
    for (i = 1; (i < n); ++i)
    {
        if (pPoint[i].y < miny)
        {
            miny = pPoint[i].y;
        }
        if (pPoint[i].y > maxy)
        {
            maxy = pPoint[i].y;
        }
    }
    if (miny < 0)
    {
        miny = 0;
    }
    if (maxy >= pBmp->bmpHeight)
    {
        maxy = pBmp->bmpHeight - 1;
    }
    
    nuWORD edgeColor565 =  nuCOLOR24TO16_565(edgeColor);
    for (y = miny; (y <= maxy); ++y)
    {
        ints = 0;
        for (i = 0; (i < n); ++i)
        {
            if (!i)
            {
                if (inc)
                {
                    ind1 = n - 1;
                    ind2 = 0;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                ind1 = i - 1;
                ind2 = i;
            }
            
            if( (y<NURO_MIN(pPoint[ind1].y, pPoint[ind2].y)) || (y>NURO_MAX(pPoint[ind1].y, pPoint[ind2].y)) )
            {
                continue;
            }
            if( (y==pPoint[ind1].y)&&(y==pPoint[ind2].y))
            {
                {
                    nuBMPDrawLineHor(pBmp, pPoint[ind1].x, pPoint[ind2].x, y,edgeColor, nMode);
                }
            }
            else if(y==pPoint[ind1].y)
            {
                pPolyInts[ints] = pPoint[ind1].x;
                if(y>pPoint[ind2].y)
                {
                    pPolyIntsTag[ints] = -1;
                }
                else
                {
                    pPolyIntsTag[ints] = 1;
                }
                ++ints;
            }
            else if(y==pPoint[ind2].y)
            {
                pPolyInts[ints] = pPoint[ind2].x;
                if(y>pPoint[ind1].y)
                {
                    pPolyIntsTag[ints] = -1;
                }
                else
                {
                    pPolyIntsTag[ints] = 1;
                }
                ++ints;
            }
            else
            {
                pPolyInts[ints]
                    = (y - pPoint[ind1].y) * (pPoint[ind2].x - pPoint[ind1].x) / (pPoint[ind2].y - pPoint[ind1].y)
                    + pPoint[ind1].x;
                
                pPolyIntsTag[ints] = 0;
                ++ints;
            }
        }
        
        for(nCount1=0; nCount1<ints-1; ++nCount1)
        {
            for(nCount2=nCount1+1; nCount2<ints; ++nCount2)
            {
                if(pPolyInts[nCount2] > pPolyInts[nCount1])
                {
                    tmpPolyInt = pPolyInts[nCount1];
                    pPolyInts[nCount1] = pPolyInts[nCount2];
                    pPolyInts[nCount2] = tmpPolyInt;
                    
                    tmpPolyInt = pPolyIntsTag[nCount1];
                    pPolyIntsTag[nCount1] = pPolyIntsTag[nCount2];
                    pPolyIntsTag[nCount2] = tmpPolyInt;
                }
            }
        }
        nuINT lastTP = 0;
        nuINT lastIO = -1;
        nuINT lastX = 0;
        for (i=0; i<ints; ++i)
        {
            nuBMPSetPixel565(pBmp, pPolyInts[i], y, edgeColor565, nMode);
            if(i==0)
            {
                lastX	= pPolyInts[i];
                lastTP	= pPolyIntsTag[i];
                if(lastTP == 0)
                {
                    lastIO = -lastIO;
                }
                continue;
            }
            if(lastTP == 0)
            {
                if(lastIO == 1 && lastX-pPolyInts[i]>1)
                {
                    nuBMPDrawLineHor(pBmp, lastX, pPolyInts[i]+1, y,color, nMode);
                }
                lastX	= pPolyInts[i];
                lastTP	= pPolyIntsTag[i];
                if(lastTP == 0)
                {
                    lastIO = -lastIO;
                }
            }
            else
            {
                if(pPolyIntsTag[i] == 0)
                {
                    lastIO = -lastIO;
                }
                else
                {
                    if(lastTP != pPolyIntsTag[i])
                    {
                        lastIO = -lastIO;
                    }
                    lastX	= pPolyInts[i];
                    lastTP	= 0;
                }
            }
        }
}
nuFree(pPolyInts);
nuFree(pPolyIntsTag);
}

nuVOID CGdiMethodZK::nuBMPFillGon_BMP(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n
                                      , PNURO_BMP pBmp2, nuCOLORREF* pEdgeColor, nuINT nMode)
{
    nuINT i;
    nuINT y;
    nuINT miny, maxy;
    nuINT ind1, ind2;
    nuINT ints;
    nuINT* pPolyInts = nuNULL;
    nuINT* pPolyIntsTag = nuNULL;
    nuINT tmpPolyInt = 0;
    nuINT nCount1 = 0;
    nuINT nCount2 = 0;
    PNUROPOINT tmpPoint = nuNULL;
    
    if (pBmp==nuNULL || n<3)
    {
        return;
    }
    
    nuINT inc = 0;
    if( (pPoint[0].x != pPoint[n-1].x) || (pPoint[0].y != pPoint[n-1].y))
    {
        inc = 1;
    }
    
    pPolyInts = (nuINT*)nuMalloc(sizeof(nuINT) * n);
    if (pPolyInts == nuNULL)
    {
        return;
    }
    pPolyIntsTag = (nuINT*)nuMalloc(sizeof(nuINT) * n);
    if (pPolyIntsTag == nuNULL)
    {
        nuFree(pPolyInts);
        return;
    }
    miny = pPoint[0].y;
    maxy = pPoint[0].y;
    for (i = 1; (i < n); ++i)
    {
        if (pPoint[i].y < miny)
        {
            miny = pPoint[i].y;
        }
        if (pPoint[i].y > maxy)
        {
            maxy = pPoint[i].y;
        }
    }
    if (miny < 0)
    {
        miny = 0;
    }
    if (maxy >= pBmp->bmpHeight)
    {
        maxy = pBmp->bmpHeight - 1;
    }
    nuWORD edgeColor565 = 0;
    nuBOOL bUseEdgeColor = nuFALSE;
    if (pEdgeColor)
    {
        edgeColor565 =  nuCOLOR24TO16_565(*pEdgeColor);
        bUseEdgeColor = nuTRUE;
    }
    
    for (y = miny; (y <= maxy); ++y)
    {
        ints = 0;
        for (i = 0; (i < n); ++i)
        {
            if (!i)
            {
                if (inc)
                {
                    ind1 = n - 1;
                    ind2 = 0;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                ind1 = i - 1;
                ind2 = i;
            }
            
            if( (y<NURO_MIN(pPoint[ind1].y, pPoint[ind2].y)) || (y>NURO_MAX(pPoint[ind1].y, pPoint[ind2].y)) )
            {
                continue;
            }
            if( (y==pPoint[ind1].y)&&(y==pPoint[ind2].y))
            {
                {
                    if (bUseEdgeColor)
                    {
                        nuBMPDrawLineHor(pBmp, pPoint[ind1].x, pPoint[ind2].x, y,*pEdgeColor, nMode);
                    }
                    else
                    {
                        nuBMPDrawLineHor_BMP(pBmp, pPoint[ind1].x, pPoint[ind2].x, y,pBmp2, nMode);
                    }
                }
            }
            else if(y==pPoint[ind1].y)
            {
                pPolyInts[ints] = pPoint[ind1].x;
                if(y>pPoint[ind2].y)
                {
                    pPolyIntsTag[ints] = -1;
                }
                else
                {
                    pPolyIntsTag[ints] = 1;
                }
                ++ints;
            }
            else if(y==pPoint[ind2].y)
            {
                pPolyInts[ints] = pPoint[ind2].x;
                if(y>pPoint[ind1].y)
                {
                    pPolyIntsTag[ints] = -1;
                }
                else
                {
                    pPolyIntsTag[ints] = 1;
                }
                ++ints;
            }
            else
            {
                pPolyInts[ints]
                    = (y - pPoint[ind1].y) * (pPoint[ind2].x - pPoint[ind1].x) / (pPoint[ind2].y - pPoint[ind1].y)
                    + pPoint[ind1].x;
                
                pPolyIntsTag[ints] = 0;
                ++ints;
            }
        }
        
        for(nCount1=0; nCount1<ints-1; ++nCount1)
        {
            for(nCount2=nCount1+1; nCount2<ints; ++nCount2)
            {
                if(pPolyInts[nCount2] > pPolyInts[nCount1])
                {
                    tmpPolyInt = pPolyInts[nCount1];
                    pPolyInts[nCount1] = pPolyInts[nCount2];
                    pPolyInts[nCount2] = tmpPolyInt;
                    
                    tmpPolyInt = pPolyIntsTag[nCount1];
                    pPolyIntsTag[nCount1] = pPolyIntsTag[nCount2];
                    pPolyIntsTag[nCount2] = tmpPolyInt;
                }
            }
        }
        nuINT lastTP = 0;
        nuINT lastIO = -1;
        nuINT lastX = 0;
        for (i=0; i<ints; ++i)
        {
            if (bUseEdgeColor)
            {
                nuBMPSetPixel565(pBmp, pPolyInts[i], y, edgeColor565, nMode);
            }
            else
            {
                nuBMPSetPixel565_BMP(pBmp, pPolyInts[i], y, pBmp2, nMode);
            }
            if(i==0)
            {
                lastX	= pPolyInts[i];
                lastTP	= pPolyIntsTag[i];
                if(lastTP == 0)
                {
                    lastIO = -lastIO;
                }
                continue;
            }
            if(lastTP == 0)
            {
                if (lastIO == 1 && lastX-pPolyInts[i]>1)
                {
                    nuBMPDrawLineHor_BMP(pBmp, lastX, pPolyInts[i]+1, y,pBmp2, nMode);
                }
                lastX	= pPolyInts[i];
                lastTP	= pPolyIntsTag[i];
                if (lastTP == 0)
                {
                    lastIO = -lastIO;
                }
            }
            else
            {
                if(pPolyIntsTag[i] == 0)
                {
                    lastIO = -lastIO;
                }
                else
                {
                    if(lastTP != pPolyIntsTag[i])
                    {
                        lastIO = -lastIO;
                    }
                    lastX	= pPolyInts[i];
                    lastTP	= 0;
                }
            }
        }
}
nuFree(pPolyInts);
nuFree(pPolyIntsTag);
}
nuVOID CGdiMethodZK::nuBMPFillEllipse(PNURO_BMP pBmp, nuINT a1, nuINT b1, nuINT a2, nuINT b2, nuCOLORREF color, nuINT nMode)
{
    if(pBmp == nuNULL || (a1==a2 && b1==b2))
    {	
        return;
    }
    nuWORD color565 = nuCOLOR24TO16_565(color);
    nuINT xx,yy,x1,x2,x3,x4,y1,y2,y3,y4,pk;
    nuINT xc,yc,rx,ry,rx2,ry2,tworx2,twory2,px,py;
    nuINT xx1,xx2,xx3,xx4;
    long width= pBmp->bmpWidth-1;
    long height=pBmp->bmpHeight-1;
    nuBOOL b;
    xc = (a1 + a2) >> 1;
    yc = (b1 + b2) >> 1;
    if(a1 > a2)
        rx = (a1 - a2) >> 1;
    else
        rx = (a2 - a1) >> 1;
    if(b1 > b2)
        ry = (b1 - b2) >> 1;
    else
        ry = (b2 - b1) >> 1;
    rx2 = rx * rx;
    ry2 = ry * ry;
    tworx2 = rx2<<1;
    twory2 = ry2<<1;
    xx = 0;
    yy = ry;
    px = 0;
    py = tworx2 * yy;
    pk = ry2 - (rx2*ry) + (rx2>>2); 
    x1 = xc , y1 = yc + ry;
    x2 = xc , y2 = yc + ry;
    x3 = xc , y3 = yc - ry;
    x4 = xc , y4 = yc - ry;
    nuBMPSetPixel565(pBmp, x1,y1,color565, nMode);
    nuBMPSetPixel565(pBmp, x3,y3,color565, nMode);
    while(px < py)
    {	
        ++xx;
        px+= twory2;
        ++x1,--x2,++x3,--x4;
        if(pk < 0)
        {	
            b = nuFALSE;
            pk+= ry2 + px;
            nuBMPSetPixel565(pBmp, x1,y1,color565, nMode);
            nuBMPSetPixel565(pBmp, x2,y2,color565, nMode);
            nuBMPSetPixel565(pBmp, x3,y3,color565, nMode);
            nuBMPSetPixel565(pBmp, x4,y4,color565, nMode);
        }
        else
        {	
            --yy;
            b = nuTRUE;
            --y1,--y2,++y3,++y4;
            py -= tworx2;
            pk+= ry2 + px - py;
        }
        xx1=x1,xx2=x2,xx3=x3,xx4=x4;
        if(xx1 < 0 || xx2 > width)
            b = nuFALSE;
        if(xx1 > width)
            xx1=xx3=width;
        if(xx2 < 0)
            xx2=xx4=0;
        if(b)
        {	
            if(y1>=0&&y1<=height)
            {
                while(xx2<xx1)
                {
                    nuBMPSetPixel565(pBmp, xx2,y2,color565, nMode);
                    ++xx2;
                }
            }
            if(y3>=0&&y3<=height)
            {	
                while(xx4<xx3)
                {	
                    nuBMPSetPixel565(pBmp, xx4,y4,color565, nMode);
                    ++xx4;
                }
            }
        }
    }
    pk = (nuINT)nuSqrt(((ry2*(xx*2+1)*(xx*2+1))>>2) + rx2*(yy-1)*(yy-1) - rx2*ry2);
    while(yy>0)
    {
        b = nuTRUE;
        --yy;
        --y1,--y2,++y3,++y4;
        py-=tworx2;
        if(pk>0)
        {
            pk+=rx2 - py;
        }
        else
        {	
            ++xx;
            ++x1,--x2,++x3,--x4;
            px+=twory2;
            pk+=rx2-py+px;
        }
        xx1=x1,xx2=x2,xx3=x3,xx4=x4;
        if(xx1 < 0 || xx2 > width)
            b = nuFALSE;
        if(xx1 > width)
            xx1=xx3=width;
        if(xx2 < 0)
            xx2=xx4=0;
        if(b)
        {	
            if(y1>=0&&y1<=height)
            {	
                while(xx2<xx1)
                {	
                    nuBMPSetPixel565(pBmp, xx2,y2,color565, nMode);
                    ++xx2;
                }
            }
            if(y3>=0&&y3<=height)
            {	
                while(xx4<xx3)
                {	
                    nuBMPSetPixel565(pBmp,xx4,y4,color565, nMode);
                    ++xx4;
                }
            }
        }
    }
}
nuVOID CGdiMethodZK::nuBMPFillCircle(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT radius, nuCOLORREF color, nuINT nMode)
{
    if(pBmp == nuNULL)
    {	
        return;
    }
    nuWORD color565 = nuCOLOR24TO16_565(color);
    if (radius < 1)
    {
        nuBMPSetPixel565(pBmp, x, y, color565, nMode);
        return;
    }
    nuINT xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
    nuINT xx1,xx2,xx3,xx4,xx5,xx6,xx7,xx8;
    nuWORD width = pBmp->bmpWidth - 1;
    nuWORD height = pBmp->bmpHeight - 1;
    
    xx = 0;
    yy = radius;
    x1 = x, y1 = y + radius;
    x2 = x, y2 = y + radius;
    x3 = x, y3 = y - radius;
    x4 = x, y4 = y - radius;
    x5 = x + radius, y5 = y;
    x6 = x - radius, y6 = y;
    x7 = x + radius, y7 = y;
    x8 = x - radius, y8 = y;
    pk = 1 - radius;
    
    nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
    nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
    
    xx5 = x5;
    xx6 = x6;
    if(xx5 >= 0 && xx6 <= width)
    {
        if(xx5 > width)
            xx5=xx7= width;
        if(xx6 < 0)
            xx6=xx8= 0;
        if(y5>=0 && y5<=height)
        {	
            do
            {	
                nuBMPSetPixel565(pBmp, xx6, y6, color565, nMode);
                ++xx6;
            }while(xx6 < xx5);
        }
    }
    
    nuBOOL b1,b2;
    while(xx < yy)
    {
        ++xx;
        ++x1, --x2, ++x3, --x4;
        ++y5, ++y6, --y7, --y8;
        b2 = nuTRUE;
        if(pk < 0)
        {	
            b1 = nuFALSE;
            pk += (xx<<1) + 1;
            nuBMPSetPixel565(pBmp, x1, y1, color565, nMode);
            nuBMPSetPixel565(pBmp, x2, y2, color565, nMode);
            nuBMPSetPixel565(pBmp, x3, y3, color565, nMode);
            nuBMPSetPixel565(pBmp, x4, y4, color565, nMode);
        }
        else
        {
            --yy;
            b1 = nuTRUE;
            --y1,--y2,++y3,++y4;
            --x5,++x6,--x7,++x8;
            pk += 2*(xx - yy) + 1;
        }
        xx1=x1,xx2=x2,xx3=x3,xx4=x4,xx5=x5,xx6=x6,xx7=x7,xx8=x8;
        if(xx1 < 0)
        {	
            xx1 = xx3 = 0;
            b1 = nuFALSE;
        }
        if(xx1 > width)
            xx1 = xx3 = width;
        if(xx2 < 0)
            xx2 = xx4 = 0;
        if(xx2 > width)
        {
            xx2=xx4= width;
            b1 = nuFALSE;
        }
        if(xx5 < 0)
        {
            xx5=xx7= 0;
            b2 = nuFALSE;
        }
        if(xx5 > width)
            xx5=xx7= width;
        if(xx6 < 0)
            xx6 = xx8 = 0;
        if(xx6 > width)
        {
            xx6 = xx8 = width;
            b2 = nuFALSE;
        }
        if(b1)
        {	
            if(y1 >= 0 && y1 <= height)
            {	
                do
                {	
                    nuBMPSetPixel565(pBmp, xx2, y2, color565, nMode);
                    ++xx2;
                }while(xx2<xx1);
            }
            if(y3>=0 && y3<=height)
            {
                do
                {
                    nuBMPSetPixel565(pBmp, xx4, y4, color565, nMode);
                    ++xx4;
                }while(xx4 < xx3);
            }
        }
        if(b2)
        {
            if(y5>=0&&y5<=height)
            {
                do
                {
                    nuBMPSetPixel565(pBmp, xx6, y6, color565, nMode);
                    ++xx6;
                }while(xx6 < xx5);
            }
            if(y7>=0 && y7<=height)
            {
                do
                {
                    nuBMPSetPixel565(pBmp, xx8, y8, color565, nMode);
                    ++xx8;
                }while(xx8<xx7);
            }
        }
    }
}

nuVOID CGdiMethodZK::nuBMPFillRect(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT w, nuINT h, nuCOLORREF color, nuINT nMode)
{
    if(pBmp == nuNULL)
        return;
    if(w<=0 || h<=0)
    {
        return;
    }
    if(x<0)
    {
        w = w+x;
        x = 0;
    }
    if(y<0)
    {
        h = h+y;
        y = 0;
    }
    if(w<=0 || h<=0)
    {
        return;
    }
    if(x+w > pBmp->bmpWidth)
    {
        w = pBmp->bmpWidth-x;
    }
    if(y+h > pBmp->bmpHeight)
    {
        h = pBmp->bmpHeight - y;
    }
    if(w<=0 || h<=0)
    {
        return;
    }
    nuBMPDrawLineHor(pBmp, x, x+w, y, color, nMode);
    nuINT i = 0;	
    if (nMode==NURO_BMP_TYPE_COPY)
    {
        nuWORD *pColor16StartD = nuNULL;
        nuWORD *pColor16StartS = &nuBMP565LineData(pBmp, y)[x];
        for(i=1; i<h; ++i)
        {
            pColor16StartD = &nuBMP565LineData(pBmp, y+i)[x];
            nuMemcpy(pColor16StartD, pColor16StartS, w*2);
        }
    }
    else
    {
        for(i=1; i<h; ++i)
        {
            nuBMPDrawLineHor(pBmp, x, x+w, y+i, color, nMode);
        }
    }
}


nuVOID CGdiMethodZK::nuBMPPolyLine(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuINT thick, nuCOLORREF color, nuINT nMode)
{
#ifdef NURO_USE_BMPLIB3
    nuBMPPolyLine_New(pBmp, pPoint, n, thick, nuCOLOR24TO16_565(color), NURO_BMP_TYPE_COPY);
#else
    /* remarked by Zhikun on 20090304*/
    nuINT i;
    for (i = 0; (i < n-1); ++i)
    {
        if(i==0)
        {
            nuBMPFillCircle(pBmp, pPoint[i].x, pPoint[i].y, (thick-1)>>1, color, nMode);
        }
        nuBMPDrawLine(pBmp, pPoint[i].x, pPoint[i].y, pPoint[i+1].x, pPoint[i+1].y, thick, color, nMode);
        nuBMPFillCircle(pBmp, pPoint[i+1].x, pPoint[i+1].y, (thick-1)>>1, color, nMode);
    }
#endif
}



//*** New Draw way, added by zhikun ************************************************

nuBOOL CGdiMethodZK::nuBMP565PolyLineZ(PNURO_BMP pBmp, const NUROPOINT* ptList, nuINT nCount, nuINT thick, nuWORD nColor565)
{
#ifdef NURO_USE_BMPLIB3
    nuBMPPolyLine_New(pBmp, ptList, nCount, thick, nColor565, NURO_BMP_TYPE_COPY);
#else
    nuINT i = 0, j = 0;
    //	nuINT dx1, dx2, dy1, dy2, dxy1, dxy2;
    for(j = 1; j < nCount; ++j)
    {
    /*
    if( j != nCount - 1 )
    {
    dx1 = ptList[j].x - ptList[i].x;
    dy1 = ptList[j].y - ptList[i].y;
    
      dx2 = ptList[j+1].x - ptList[j].x;
      dy2 = ptList[j+1].y - ptList[j].y;
      dxy1 = dx1*dy2 - dx2*dy1;
      dxy2 = dx1*dx2 + dy1*dy2;
      dxy1 = NURO_ABS(dxy1);
      dxy2 = NURO_ABS(dxy2);
      
        if( (dxy1<<6) <= dxy2 )
        {
        continue;
        }
        }
        */
        if( thick > 1 )
        {
            nuBMP565FillCircleZ( pBmp, 
                ptList[i].x, 
                ptList[i].y, 
                thick, 
                nColor565,
                0 );
        }
        
        nuBMP565DrawLineZ( pBmp, 
            ptList[i].x, 
            ptList[i].y,
            ptList[j].x,
            ptList[j].y,
            thick,
            nColor565,
            0 );
        i = j;
    }
    if( thick > 1 )
    {
        /**/
        nuBMP565FillCircleZ( pBmp, 
            ptList[j-1].x, 
            ptList[j-1].y, 
            thick, 
            nColor565,
            0 );
    }
#endif
    return nuTRUE;
}


nuBOOL CGdiMethodZK::nuBMP565DrawLineZ(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, 
                                       nuWORD nColor565, nuINT nMode)
{
    if( !nuLineLimitToRectZ( x1, x2, y1, y2, -thick, pBmp->bmpWidth - 1 + thick, -thick, pBmp->bmpHeight - 1 + thick ) )
    {
        return nuFALSE;
    }
    nuINT dx = x2 - x1;
    nuINT dy = y2 - y1;
    if( dx == 0 && dy == 0)
    {
        return nuTRUE;//nuBMP565FillCircleZ(pBmp, x1, y1, thick, nColor565, nMode);
    }
    else if( dx == 0 )
    {
        //		nuBMPFillRect(pBmp, x1-((thick)>>1), NURO_MIN(y1,y2), thick, dy, nColor565, nMode);
        //		return nuTRUE;
        return nuBMP565DrawVerticalLineZ(pBmp, x1, y1, y2, thick, nColor565, nMode);
    }
    else if( dy == 0 )
    {
        //		nuBMPFillRect(pBmp, NURO_MIN(x1,x2), y1-((thick)>>1), dx, thick, nColor565, nMode);
        //		return nuTRUE;
        return nuBMP565DrawHorizontalLineZ(pBmp, x1, x2, y1, thick, nColor565, nMode);
    }
    //	--thick;
    nuINT dxy = (nuINT)(nuSqrt(dx*dx + dy*dy));
    nuINT nWidth = thick;
    
    nuINT x, y, nLastValue = -1000, nRemainder = 0, nSameCount/*, nSameMin*/;
    nuINT nThick1 ,nThick2;
    nuINT nNumB, nNumC, nDeno;
    nuINT xEx, yEx;
    nuWORD* pColor16;
    if( NURO_ABS(dx) >= NURO_ABS(dy) )//??????????X?廗?羃迭?
    {
        if( dx < 0 )//?????????磩畎???傽??????硐???
        {
            x  = x1;
            y  = y1;
            x1 = x2;
            y1 = y2;
            x2 = x;
            y2 = y;
            dx = -dx;
            dy = -dy;
        }
        thick = (((thick * dxy)<<1) + dx) / (dx<<1);
        nThick1 = (thick)>>1;
        nThick2 = (thick)>>1;
        if( !(thick & 0x01) )
        {
            --nThick1;
        }
        //
        xEx = (NURO_ABS(dy) * nWidth + dxy) / (dxy<<1);
        yEx = dx * dxy;
        yEx = (dy * dy * nWidth + yEx) / (yEx<<1);
        
        nuBOOL bMixUp, bMixDown;
        nuINT yTop, yBottom, yStart, yEnd;
        nuINT j = 0;
        nuINT xAdd = dx + (xEx);
        nuINT xEnd = dx - xEx;
        nuINT yAdd;
        nSameCount = (dx + NURO_ABS(dy) - 1) / (NURO_ABS(dy));
        for( nuINT i = -xEx; i <= xAdd; ++i, ++j )
        {
            x = x1 + i;
            if( x < 0 || x >= pBmp->bmpWidth )
            {
                continue;
            }
            yAdd = (i * dy) << 1;
            if( yAdd >= 0 )
            {
                nRemainder = (yAdd + dx);
            }
            else
            {
                nRemainder = (yAdd - dx);
            }
            y = y1 + nRemainder / (dx<<1);
            nRemainder %= (dx<<1);
            
            bMixDown = bMixUp = nuTRUE;
            if( i < 0 )
            {
                //				nSameCount = 1;
                j = 0;
                //				bMixDown = bMixUp = nuFALSE;
            }
            else if( nLastValue != y )
            {
                if( i == 0 )
                {
                    nRemainder = dx * dy / NURO_ABS(dy);
                }
                nuINT nScc;
                
                if( dy > 0 )
                {
                    nScc = ((dx <<1) + (NURO_ABS(dy)<<1) - 1 - nRemainder)/(NURO_ABS(dy) << 1);
                }
                else
                {
                    nScc = ((dx <<1) + (NURO_ABS(dy)<<1) - 1 + nRemainder)/(NURO_ABS(dy) << 1);
                }
                /*
                if( nSameCount < nSameMin )
                {
                nSameCount = nSameMin;
                }
                */
                j = nSameCount - nScc;
                nLastValue = y;
            }
            yStart = y - nThick1;
            yEnd = y + nThick2;
            
            if( i <= xEx )
            {
                if( (i*dy) <= 0 )
                {
                    yTop = y1 - ( ((dx * i)<<1) - (dy) ) / (dy<<1);
                }
                else
                {
                    yTop = y1 - ( ((dx * i)<<1) + (dy) ) / (dy<<1);
                }
                if( dy > 0 )
                {
                    if( yTop > yEnd )
                    {
                        yTop = yEnd;
                    }
                    if( yTop > yStart )
                    {
                        yStart = yTop;
                        //						bMixUp = nuFALSE;
                    }
                }
                else
                {
                    if( yTop < yStart )
                    {
                        yTop = yStart;
                    }
                    if( yTop < yEnd )
                    {
                        yEnd = yTop;
                        //						bMixDown = nuFALSE;
                    }
                }
            }
            if( i >= xEnd )
            {
                if( i <= dx )
                {
                    yBottom = y2 + ( dx * (dx - i)) / dy;
                }
                else
                {
                    yBottom = y2 + ( dx * (dx - i)) / dy;
                }
                if( dy > 0 )
                {
                    if( yBottom < yEnd )
                    {
                        yEnd = yBottom;
                        bMixDown = nuFALSE;
                    }
                }
                else
                {
                    if( yBottom > yStart )
                    {
                        yStart = yBottom;
                        bMixUp = nuFALSE;
                    }
                }
            }
            
            nuINT yFrom = (yStart > 0) ? yStart : 0;
            nuINT yTo = (yEnd < pBmp->bmpHeight ) ? yEnd : (pBmp->bmpHeight-1);
            while( yFrom <= yTo )
            {
                nuBMP565LineData(pBmp, yFrom)[x] = nColor565;
                ++yFrom;
            }
#ifdef NURO_USE_LINE_MIX
            --yStart;
            ++yEnd;
            if( bMixUp && yStart >= 0 && yStart < pBmp->bmpHeight )
            {
                if( dy > 0 )
                {
                    nNumC = nSameCount - j;
                    nNumB = j + 1;
                    nDeno = nSameCount + 1;
                    
                }
                else
                {
                    nNumC = j + 1;
                    nNumB = nSameCount - j;
                    nDeno = nSameCount + 1;
                }
                pColor16 = &nuBMP565LineData(pBmp, yStart)[x];
                *pColor16 = BMP565_COLOR_MIX(*pColor16, nNumB, nColor565, nNumC, nDeno);
            }
            if( bMixDown && yEnd >= 0 && yEnd < pBmp->bmpHeight )
            {
                if( dy > 0 )
                {
                    nNumC = j + 1;
                    nNumB = nSameCount - j;
                    nDeno = nSameCount + 1;
                }
                else
                {
                    nNumC = nSameCount - j;
                    nNumB = j + 1;
                    nDeno = nSameCount + 1;
                }
                pColor16 = &nuBMP565LineData(pBmp, yEnd)[x];
                *pColor16 = BMP565_COLOR_MIX(*pColor16, nNumB, nColor565, nNumC, nDeno);
            }
#endif
}
}
else//??????????y?廗????
{
    
    if( dy < 0 )//?????????磩畎???傽??????硐???
    {
        x  = x1;
        y  = y1;
        x1 = x2;
        y1 = y2;
        x2 = x;
        y2 = y;
        dx = -dx;
        dy = -dy;
    }
    thick = (((thick * dxy)<<1) + dy) / (dy<<1);
    nThick1 = (thick)>>1;
    nThick2 = (thick)>>1;
    if( !(thick & 0x01) )
    {
        --nThick1;
    }
    //
    yEx = (NURO_ABS(dx) * nWidth + dxy) / (dxy<<1);
    
    nuBOOL bMixLeft, bMixRight;
    nuINT xLeft, xRight, xStart, xEnd;
    nuINT j = 0;
    nuINT yAdd = dy + (yEx);
    nuINT yEnd = dy - yEx;
    nuINT xAdd;
    nSameCount = (dy + NURO_ABS(dx) - 1)/NURO_ABS(dx);
    for( nuINT i = -yEx; i < yAdd; ++i, ++j )
    {
        y = y1 + i;
        if( y < 0 || y >= pBmp->bmpHeight )
        {
            continue;
        }
        
        xAdd = (i * dx) << 1;
        if( xAdd >= 0 )
        {
            nRemainder = (xAdd + (dy));
        }
        else
        {
            nRemainder = (xAdd - (dy));
        }
        x = x1 + nRemainder / (dy<<1);
        nRemainder %= (dy<<1);
        bMixLeft = bMixRight = nuTRUE;
        if( i < 0 )
        {
            //				nSameCount = 1;
            j = 0;
            //				bMixLeft = bMixRight = nuFALSE;
        }
        else if( nLastValue != x )
        {
            if( i == 0 )
            {
                nRemainder = dx * dy / NURO_ABS(dx);
            }
            nuINT nScc;
            
            if( dx > 0 )
            {
                nScc = ((dy <<1) + (NURO_ABS(dx)<<1) - 1 - nRemainder)/(NURO_ABS(dx) << 1);
            }
            else
            {
                nScc = ((dy <<1) + (NURO_ABS(dx)<<1) - 1 + nRemainder)/(NURO_ABS(dx) << 1);
            }
            j = 0; nSameCount = nScc;
            nLastValue = x;
        }
        xStart = x - nThick1;
        xEnd = x + nThick2;
        if( /*yEx > 1 &&*/ i <= yEx )
        {
            if( i * dx <= 0 )
            {
                xLeft = x1 - (((dy * i)<<1) - dx ) / (dx<<1);
            }
            else
            {
                xLeft = x1 - (((dy * i)<<1) + dx ) / (dx<<1);
            }
            if( dx > 0 )
            {
                if( xLeft > xEnd )
                {
                    xLeft = xEnd;
                }
                if( xLeft > xStart )
                {
                    xStart = xLeft;
                    //						bMixLeft = nuFALSE;
                }
            }
            else
            {
                if( xLeft < xStart )
                {
                    xLeft = xStart;
                }
                if( xLeft < xEnd )
                {
                    xEnd = xLeft;
                    //						bMixRight = nuFALSE;
                }
            }
        }
        if( /*yEx > 1 &&*/ i >= yEnd )
        {
            if( i <= dy )
            {
                xRight = x2 + ( dy * (dy - i)) / dx;
            }
            else
            {
                xRight = x2 + ( dy * (dy - i)) / dx;
            }
            if( dx > 0 )
            {
                if( xRight < xEnd )
                {
                    xEnd = xRight;
                    bMixRight = nuFALSE;
                }
            }
            else
            {
                if( xRight > xStart )
                {
                    xStart = xRight;
                    bMixLeft = nuFALSE;
                }
            }
        }
        
        nuINT xFrom = (xStart > 0) ? xStart : 0;
        nuINT xTo = (xEnd < pBmp->bmpWidth ) ? xEnd : (pBmp->bmpWidth-1);
        pColor16 = &nuBMP565LineData(pBmp, y)[xFrom];
        while( xFrom <= xTo )
        {
            *pColor16 = nColor565;
            ++pColor16;
            ++xFrom;
        }
#ifdef NURO_USE_LINE_MIX
        --xStart;
        ++xEnd;
        
        if( bMixLeft && xStart >= 0 && xStart < pBmp->bmpWidth )
        {
            if( dx > 0 )
            {
                nNumC = nSameCount - j;
                nNumB = j + 1;
                nDeno = nSameCount + 1;
                
            }
            else
            {
                nNumC = j + 1;
                nNumB = nSameCount - j;
                nDeno = nSameCount + 1;
            }
            pColor16 = &nuBMP565LineData(pBmp, y)[xStart];
            *pColor16 = BMP565_COLOR_MIX(*pColor16, nNumB, nColor565, nNumC, nDeno);
        }
        if( bMixRight && xEnd >= 0 && xEnd < pBmp->bmpWidth )
        {
            if( dx > 0 )
            {
                nNumC = j + 1;
                nNumB = nSameCount - j;
                nDeno = nSameCount + 1;
            }
            else
            {
                nNumC = nSameCount - j;
                nNumB = j + 1;
                nDeno = nSameCount + 1;
            }
            pColor16 = &nuBMP565LineData(pBmp, y)[xEnd];
            *pColor16 = BMP565_COLOR_MIX(*pColor16, nNumB, nColor565, nNumC, nDeno);
        }
#endif
}
}
return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565DrawVerticalLineZ(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuINT thick, 
                                               nuWORD nColor565, nuINT nMode)
{
    nuINT y;
    if( y1 > y2 )
    {
        y = y1;
        y1 = y2;
        y2 = y;
    }
    if( y1 < 0 )
    {
        y1 = 0;
    }
    if( y2 >= pBmp->bmpHeight )
    {
        y2 = pBmp->bmpHeight - 1;
    }
    if( y1 > y2 )
    {
        return nuFALSE;
    }
    //
    nuINT xMin = x - (thick>>1);
    nuINT xMax = x + (thick>>1);
    if( thick & 0x01 )
    {
        --xMin;
    }
    ++xMax;
    if( xMin >= pBmp->bmpWidth || xMax <= 0 )
    {
        return nuFALSE;
    }
    for(y = y1; y <= y2; ++y)
    {
        nuWORD* pColor16 = nuBMP565LineData(pBmp, y);
        nuINT i = xMin;
        if( i >= 0 )
        {
            /*mix*/
            pColor16 += i;
            //Setcolor....
#ifdef NURO_USE_LINE_MIX
            *pColor16	= nuRGB24TO16_565(((nu565RVALUE(*pColor16)>>1) + (nu565RVALUE(nColor565)>>1)),
                ((nu565GVALUE(*pColor16)>>1) + (nu565GVALUE(nColor565)>>1)),
                ((nu565BVALUE(*pColor16)>>1) + (nu565BVALUE(nColor565)>>1)));
#endif
            ++pColor16;
            ++i;
        }
        else
        {
            i = 0;
        }
        while(i < xMax && i < pBmp->bmpWidth )
        {
            *pColor16 = nColor565;
            ++i;
            ++pColor16;
        }
#ifdef NURO_USE_LINE_MIX
        if( i < pBmp->bmpWidth )
        {
            //setcolor
            *pColor16	= nuRGB24TO16_565(((nu565RVALUE(*pColor16)>>1) + (nu565RVALUE(nColor565)>>1)),
                ((nu565GVALUE(*pColor16)>>1) + (nu565GVALUE(nColor565)>>1)),
                ((nu565BVALUE(*pColor16)>>1) + (nu565BVALUE(nColor565)>>1)));
        }
#endif
    }
    return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565DrawHorizontalLineZ(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuINT thick, 
                                                 nuWORD nColor565, nuINT nMode)
{
    nuINT x;
    if( x1 > x2 )
    {
        x = x1;
        x1 = x2;
        x2 = x;
    }
    if( x1 < 0 )
    {
        x1 = 0;
    }
    if( x2 >= pBmp->bmpWidth )
    {
        x2 = pBmp->bmpWidth - 1;
    }
    if( x1 > x2 )
    {
        return nuFALSE;
    }
    nuINT yMin = y - ((thick)>>1);
    nuINT yMax = y + ((thick)>>1);
    if( (thick&0x01) )
    {
        --yMin;
    }
    ++yMax;
    if( yMin >= pBmp->bmpHeight || yMax < 0 )
    {
        return nuFALSE;
    }
    nuWORD* pColor16;
    if( yMin >= 0 )
    {
        /*Mix*/
#ifdef NURO_USE_LINE_MIX
        pColor16 = &nuBMP565LineData(pBmp, yMin)[x1];
        for(nuINT i = x1; i <= x2; ++i)
        {
            *pColor16 = nuRGB24TO16_565(((nu565RVALUE(*pColor16)>>1) + (nu565RVALUE(nColor565)>>1)),
                ((nu565GVALUE(*pColor16)>>1) + (nu565GVALUE(nColor565)>>1)),
                ((nu565BVALUE(*pColor16)>>1) + (nu565BVALUE(nColor565)>>1)));
            ++pColor16;
        }
#endif
        ++yMin;
    }
    else
    {
        yMin = 0;
    }
    if( yMax < pBmp->bmpHeight )
    {
        /*Mix*/
#ifdef NURO_USE_LINE_MIX
        pColor16 = &nuBMP565LineData(pBmp, yMax)[x1];
        for(nuINT i = x1; i <= x2; ++i)
        {
            *pColor16 = nuRGB24TO16_565(((nu565RVALUE(*pColor16)>>1) + (nu565RVALUE(nColor565)>>1)),
                ((nu565GVALUE(*pColor16)>>1) + (nu565GVALUE(nColor565)>>1)),
                ((nu565BVALUE(*pColor16)>>1) + (nu565BVALUE(nColor565)>>1)));
            ++pColor16;
        }
#endif
        --yMax;
    }
    else
    {
        yMax = pBmp->bmpHeight - 1;
    }
    for( y = yMin; y <= yMax; ++y )
    {
        pColor16 = &nuBMP565LineData(pBmp, y)[x1];
        for(nuINT i = x1; i <= x2; ++i)
        {
            *pColor16 = nColor565;
            ++pColor16;
        }
    }
    return nuTRUE;
}



nuBOOL CGdiMethodZK::nuBMP565FillCircleZ(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode)
{
    if( diameter < 2 )
    {
        return nuFALSE;
    }
    if( !(diameter & 0x01) )
    {
        return nuBMP565FillCircleD2Z(pBmp, x, y, diameter, nColor565, nMode);
    }
    else
    {
        return nuBMP565FillCircleD1Z(pBmp, x, y, diameter, nColor565, nMode);
    }
    nuBYTE	nFillMode;
    nuWORD* pColor16;
    nuBYTE	nBit0 = (nuBYTE)(diameter&0x01);
    if( x >= 0 && x < pBmp->bmpWidth && y >= 0 && y < pBmp->bmpHeight )
    {
        nuBMP565LineData(pBmp, y)[x] = nColor565;
    }
    long nDD;
    long nRR1 = ((diameter-1)*(diameter-1))>>2;
    long nRR3 = ((diameter+1)*(diameter+1))>>2;
    long nRR5 = ((diameter+3)*(diameter+3))>>2;
    long nRdisD, nRdisB, nDeno;
    if( diameter > 1 )
    {
        nuINT nSum;
        nuINT nRadius = (diameter + 1)>>1;//??埴?警蓂???襤踾??
        nuINT xR1, xR2, xL1, xL2, yB1, yB2, yT1, yT2;
        for(nuINT i = 0; i <= nRadius; ++i)
        {
            long nii = i*i;
            for(nuINT j = i; j <= nRadius; ++j)
            {
                /**/
                if( j == 0 )
                {
                    continue;
                }
                //
                xR1 = x + j;
                xL1 = x - j;// j != 0
                if( i != j )
                {
                    xR2 = x + i;
                    if( i != 0 )
                    {
                        xL2 = x - i;
                    }
                    else
                    {
                        xL2 = -1;
                    }
                }
                else
                {
                    xR2 = -1;
                    xL2 = -1;
                }
                yB1 = y + i;
                if( i != 0 )
                {
                    yT1 = y - i;
                }
                else
                {
                    yT1 = -1;
                }
                if( i != j )
                {
                    yB2 = y + j;
                    yT2 = y - j;//j != 0
                }
                else
                {
                    yB2 = -1;
                    yT2 = -1;
                }
                nSum = i+j;
                nFillMode = 0;
                if( nSum < nRadius )
                {
                    nFillMode = 1;
                }
                else
                {
                    nDD = nii + j*j;
                    if( nDD <= nRR1 )
                    {
                        nFillMode = 1;
                    }
                    else if( nDD < nRR3 )
                    {
                        nFillMode = 2;
                    }
                    else if( nDD < nRR5 )
                    {
#ifdef NURO_USE_LINE_MIX
                        nFillMode = 3;
#endif
                    }
                }
                if( nFillMode == 1 )
                {
                    if( xR1 >= 0 && xR1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yB1)[xR1] = nColor565;
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yT1)[xR1] = nColor565;
                        }
                    }
                    if( xR2 >= 0 && xR2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yB2)[xR2] = nColor565;
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yT2)[xR2] = nColor565;
                        }
                    }
                    if( xL1 >= 0 && xL1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yB1)[xL1] = nColor565;
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yT1)[xL1] = nColor565;
                        }
                    }
                    if( xL2 >= 0 && xL2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yB2)[xL2] = nColor565;
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            nuBMP565LineData(pBmp, yT2)[xL2] = nColor565;
                        }
                    }
                }
                else if( nFillMode == 2 )
                {
                    nRdisD = nRR3 - nDD;
                    nRdisB = diameter - nRdisD;
                    nDeno = (diameter<<1);
                    nRdisD += diameter;
                    if( xR1 >= 0 && xR1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB1)[xR1];
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT1)[xR1];
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xR2 >= 0 && xR2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB2)[xR2];
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT2)[xR2];
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xL1 >= 0 && xL1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB1)[xL1];
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT1)[xL1] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xL2 >= 0 && xL2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB2)[xL2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT2)[xL2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                }
                else if( nFillMode == 3 )
                {
                    nRdisD = nRR5 - nDD;
                    nRdisB = nDD - nRR3;
                    nDeno = nRR5 - nRR3;
                    nRdisB += nDeno;
                    nDeno = nDeno<<1;
                    if( xR1 >= 0 && xR1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB1)[xR1] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT1)[xR1] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xR2 >= 0 && xR2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB2)[xR2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT2)[xR2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xL1 >= 0 && xL1 < pBmp->bmpWidth )
                    {
                        if( yB1 >= 0 && yB1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB1)[xL1] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT1 >= 0 && yT1 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT1)[xL1] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                    if( xL2 >= 0 && xL2 < pBmp->bmpWidth )
                    {
                        if( yB2 >= 0 && yB2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yB2)[xL2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                        if( yT2 >= 0 && yT2 < pBmp->bmpHeight )
                        {
                            pColor16 = &nuBMP565LineData(pBmp, yT2)[xL2] ;
                            *pColor16 = BMP565_COLOR_MIX(*pColor16, nRdisB, nColor565, nRdisD, nDeno);
                        }
                    }
                }
}
}
}
return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565FillCircleD1Z(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode)
{
    if( diameter <= 0 )
    {
        return nuFALSE;
    }
    nuBYTE	nFillMode;
    nuWORD* pColor16;
    nuBYTE	nBit0 = (nuBYTE)(diameter&0x01);
    nuINT nRadius = diameter>>1;
    if( x >= 0 && x < pBmp->bmpWidth && y >= 0 && y < pBmp->bmpHeight ) 
    {
        nuBMP565LineData(pBmp, y)[x] = nColor565;
    }
    if ( diameter == 1 )
    {
        return nuTRUE;
    }
    //
    long nDD;
    long nRR = nRadius * nRadius;
    long nRR2 = (diameter + 2)*(diameter + 2) / 4;//(nRadius + 2)*(nRadius + 2);
    long nClrBg, nClrFg, nClrTotal = (nRR2 - nRR);
    NUROPOINT ptList[8];
    nuINT nNowPt;
    nuINT nXEnd = nRadius + 1;
    for(nuINT i = 1; i <= nXEnd; ++i)
    {
        long nSum;
        long nii = i * i;
        for(nuINT j = 0; j <= i; ++j)
        {
            nSum = i + j;
            if( nSum <= nRadius )
            {
                nFillMode = 1;
            }
            /*
            else if( i == nXEnd && j == 0)
            {
            nFillMode = 2;
            nClrBg = nClrTotal / 2;
            nClrFg = nClrTotal - nClrBg;
            }
            */
            else
            {
                nDD = nii + j*j;
                if( nDD <= nRR )
                {
                    nFillMode = 1;
                }
                else if( nDD < nRR2 )
                {
                    nFillMode = 2;
                    //					nClrFg = (nRR2 - nDD)>>1;//nClrTotal - nClrBg;
                    //					nClrBg = nClrTotal - nClrFg;//(nDD - nRR)/2;
                    nClrBg = (nDD - nRR);
                    nClrFg = nClrTotal - nClrBg;
                }
                else
                {
                    continue;
                }
            }
            //...
            ptList[0].x = x + i;
            ptList[0].y = y + j;
            nNowPt = 1;
            if( i != j && j != 0 )
            {
                ptList[nNowPt].x = x + j;
                ptList[nNowPt].y = y + i;
                ++ nNowPt;
            }
            ptList[nNowPt].x = x + j;
            ptList[nNowPt].y = y - i;
            ++ nNowPt;
            if( i != j && j != 0 )
            {
                ptList[nNowPt].x = x + i;
                ptList[nNowPt].y = y - j;
                ++ nNowPt;
            }
            ptList[nNowPt].x = x - i;
            ptList[nNowPt].y = y - j;
            ++ nNowPt;
            if( i != j && j != 0 )
            {
                ptList[nNowPt].x = x - j;
                ptList[nNowPt].y = y - i;
                ++ nNowPt;
            }
            //
            ptList[nNowPt].x = x - j;
            ptList[nNowPt].y = y + i;
            ++ nNowPt;
            if( i != j && j != 0 )
            {
                ptList[nNowPt].x = x - i;
                ptList[nNowPt].y = y + j;
                ++ nNowPt;
            }
            //
            if( nFillMode == 1 )
            {
                for(nuINT k = 0; k < nNowPt; ++k)
                {
                    if( ptList[k].x >= 0 && ptList[k].x < pBmp->bmpWidth &&
                        ptList[k].y >= 0 && ptList[k].y < pBmp->bmpHeight )
                    {
                        nuBMP565LineData(pBmp, ptList[k].y)[ptList[k].x] = nColor565;
                    }
                }
            }
            else
            {
                for(nuINT k = 0; k < nNowPt; ++k)
                {
                    if( ptList[k].x >= 0 && ptList[k].x < pBmp->bmpWidth &&
                        ptList[k].y >= 0 && ptList[k].y < pBmp->bmpHeight )
                    {
                        pColor16 =  &nuBMP565LineData(pBmp, ptList[k].y)[ptList[k].x];
                        *pColor16 = BMP565_COLOR_MIX(*pColor16, nClrBg, nColor565, nClrFg, nClrTotal);
                    }
                }
            }
        }
}
return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565FillCircleD2Z(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode)
{
    if( diameter <= 0 )
    {
        return nuFALSE;
    }
    nuBYTE	nFillMode;
    nuWORD* pColor16;
    nuINT nRadius = diameter>>1;
    //
    long nDD;
    long nRR = nRadius * nRadius;
    long nRR2 = (diameter + 3)*(diameter + 3) / 4;//(nRadius + 2)*(nRadius + 2);
    long nClrBg, nClrFg, nClrTotal = (nRR2 - nRR)/2;
    NUROPOINT ptList[8];
    nuINT nNowPt;
    nuINT nXEnd = nRadius + 1;
    for(nuINT i = 1; i <= nXEnd; ++i)
    {
        long nSum;
        long nii = i * i;
        for(nuINT j = 1; j <= i; ++j)
        {
            nSum = i + j;
            if( nSum < nRadius )
            {
                nFillMode = 1;
            }
            else
            {
                nDD = nii + j*j;
                if( nDD <= nRR )
                {
                    nFillMode = 1;
                }
                else if( nDD < nRR2 )
                {
                    nFillMode = 2;
                    //					nClrBg = (nDD - nRR);
                    //					nClrFg = nClrTotal - nClrBg;
                    nClrBg = (nDD - nRR)/2;
                    nClrFg = nClrTotal - nClrBg;
                }
                else
                {
                    continue;
                }
            }
            //
            ptList[0].x = x + i;
            ptList[0].y = y + j;
            ptList[1].x = ptList[0].x;
            ptList[1].y = y - j + 1;
            ptList[2].x = x - i + 1;
            ptList[2].y = ptList[0].y;
            ptList[3].x = ptList[2].x;
            ptList[3].y = ptList[1].y;
            nNowPt = 4;
            if( i != j )
            {
                ptList[4].x = x + j;
                ptList[4].y = y + i;
                ptList[5].x = ptList[4].x;
                ptList[5].y = y - i + 1;
                ptList[6].x = x - j + 1;
                ptList[6].y = ptList[4].y;
                ptList[7].x = ptList[6].x;
                ptList[7].y = ptList[5].y;
                nNowPt = 8;
            }
            
            if( nFillMode == 1 )
            {
                for(nuINT k = 0; k < nNowPt; ++k)
                {
                    if( ptList[k].x >= 0 && ptList[k].x < pBmp->bmpWidth &&
                        ptList[k].y >= 0 && ptList[k].y < pBmp->bmpHeight )
                    {
                        nuBMP565LineData(pBmp, ptList[k].y)[ptList[k].x] = nColor565;
                    }
                }
            }
            else
            {
                for(nuINT k = 0; k < nNowPt; ++k)
                {
                    if( ptList[k].x >= 0 && ptList[k].x < pBmp->bmpWidth &&
                        ptList[k].y >= 0 && ptList[k].y < pBmp->bmpHeight )
                    {
                        pColor16 =  &nuBMP565LineData(pBmp, ptList[k].y)[ptList[k].x];
                        *pColor16 = BMP565_COLOR_MIX(*pColor16, nClrBg, nColor565, nClrFg, nClrTotal);
                    }
                }
            }
        }
    }
    return nuTRUE;
}

nuBOOL CGdiMethodZK::nuLineLimitToRectZ(nuINT &x1, nuINT& x2, nuINT& y1, nuINT& y2, nuINT nL, nuINT nR, nuINT nT, nuINT nB)
{
    if( (x1 <= nL && x2 <= nL)	|| 
        (x1 >= nR && x2 >= nR)	||
        (y1 <= nT && y2 <= nT)	||
        (y1 >= nB && y2 >= nB)	)
    {
        return nuFALSE;
    }
    nuINT nXmin, nXmax, nYmin, nYmax;
    //nuroRECT rtBound = {nL, nT, nR, nB};
    if( x1 > x2 )
    {
        nXmin = x2;
        nXmax = x1;
    }
    else
    {
        nXmin = x1;
        nXmax = x2;
    }
    if( y1 > y2 )
    {
        nYmin = y2;
        nYmax = y1;
    }
    else
    {
        nYmin = y1;
        nYmax = y2;
    }
    if( nXmin >= nL && nXmax <= nR && nYmin >= nT && nYmax <= nB )
    {
        return nuTRUE;
    }//?葍庤?????????羶?郇???
    //	nuINT nJx1, nJx2, nJx3, nJx4, nJy1, nJy2, nJy3, nJy4;
    //	DEBUGSTRING(nuTEXT("LineLimitToRectZ IN"));
    NUROPOINT ptTmpList[6];
    nuINT nListCount = 0;
    nuINT dx = x2 - x1;
    nuINT dy = y2 - y1;
    /**/
    nuBOOL bBothOut = nuTRUE;
    if( (x1 > nL && x1 < nR && y1 > nT && y1 < nB) ||
        (x2 > nL && x2 < nR && y2 > nT && y2 < nB) )
    {
        bBothOut = nuFALSE;
    }
    if( nL > nXmin && nL < nXmax )
    {
        ptTmpList[nListCount].x = nL;
        ptTmpList[nListCount].y = y1 + (nL - x1) * dy / dx;
        if( ptTmpList[nListCount].y >= nT	 && 
            ptTmpList[nListCount].y <= nB	 &&
            ptTmpList[nListCount].y >= nYmin && 
            ptTmpList[nListCount].y <= nYmax )
        {
            ++nListCount;
        }
    }
    if( nR > nXmin && nR < nXmax )
    {
        ptTmpList[nListCount].x = nR;
        ptTmpList[nListCount].y = y1 + (nR - x1) * dy / dx;
        if( ptTmpList[nListCount].y >= nT	 && 
            ptTmpList[nListCount].y <= nB	 &&
            ptTmpList[nListCount].y >= nYmin && 
            ptTmpList[nListCount].y <= nYmax )
        {
            ++nListCount;
        }
    }
    if( nT > nYmin && nT < nYmax )
    {
        ptTmpList[nListCount].y = nT;
        ptTmpList[nListCount].x = x1 + (nT - y1) * dx / dy;
        if( ptTmpList[nListCount].x >= nL &&
            ptTmpList[nListCount].x <= nR &&
            ptTmpList[nListCount].x >= nXmin && 
            ptTmpList[nListCount].x <= nXmax )
        {
            nuINT i = 0;
            while( i < nListCount )
            {
                if( ptTmpList[i].x == ptTmpList[nListCount].x &&
                    ptTmpList[i].y == ptTmpList[nListCount].y )
                {
                    break;
                }
                ++i;
            }
            if( i == nListCount )
            {
                ++nListCount;	
            }
        }
    }
    if( nB > nYmin && nB < nYmax )
    {
        ptTmpList[nListCount].y = nB;
        ptTmpList[nListCount].x = x1 + (nB - y1) * dx / dy;
        if( ptTmpList[nListCount].x >= nL &&
            ptTmpList[nListCount].x <= nR &&
            ptTmpList[nListCount].x >= nXmin && 
            ptTmpList[nListCount].x <= nXmax )
        {
            nuINT i = 0;
            while( i < nListCount )
            {
                if( ptTmpList[i].x == ptTmpList[nListCount].x &&
                    ptTmpList[i].y == ptTmpList[nListCount].y )
                {
                    break;
                }
                ++i;
            }
            if( i == nListCount )
            {
                ++nListCount;	
            }
        }
    }
    if( nListCount == 0  )
    {
        //		DEBUGSTRING(nuTEXT("LineLimitToRectZ OUT"));
        return nuFALSE;
    }
    if( nListCount == 1 )
    {
        if( x1 >= nL && x1 <= nR && y1 >= nT && y1 <= nB )
        {
            x2 = ptTmpList[0].x;
            y2 = ptTmpList[0].y;
        }
        else
        {
            x1 = ptTmpList[0].x;
            y1 = ptTmpList[0].y;
        }
    }
    /**/
    else if( bBothOut && nListCount == 2 )
    {
        x1 = ptTmpList[0].x;
        y1 = ptTmpList[0].y;
        x2 = ptTmpList[1].x;
        y2 = ptTmpList[1].y;
    }
    
    else
    {
        ptTmpList[nListCount].x = x1;
        ptTmpList[nListCount].y = y1;
        ++nListCount;
        ptTmpList[nListCount].x = x2;
        ptTmpList[nListCount].y = y2;
        ++nListCount;
        NUROPOINT point;
        nuINT i;
        nuBOOL bSetFirst = nuFALSE;
        if( NURO_ABS(dy) > NURO_ABS(dx) )
        {
            for(i = 0; i < nListCount; ++i)
            {
                for(nuINT j = i+1; j < nListCount; ++j)
                {
                    if( ptTmpList[i].y > ptTmpList[j].y )
                    {
                        point = ptTmpList[i];
                        ptTmpList[i] = ptTmpList[j];
                        ptTmpList[j] = point;
                    }
                }
            }
            for( i = 0; i < nListCount; ++i )
            {
                if( ptTmpList[i].y >= nT && ptTmpList[i].y <= nB )
                {
                    x2 = ptTmpList[i].x;
                    y2 = ptTmpList[i].y;
                    if( !bSetFirst )
                    {
                        x1 = x2;
                        y1 = y2;
                        bSetFirst = nuTRUE;
                    }
                }
            }
        }
        else
        {
            for(i = 0; i < nListCount; ++i)
            {
                for(nuINT j = i+1; j < nListCount; ++j)
                {
                    if( ptTmpList[i].x > ptTmpList[j].x )
                    {
                        point = ptTmpList[i];
                        ptTmpList[i] = ptTmpList[j];
                        ptTmpList[j] = point;
                    }
                }
            }
            for( i = 0; i < nListCount; ++i )
            {
                if( ptTmpList[i].x >= nL && ptTmpList[i].x <= nR )
                {
                    x2 = ptTmpList[i].x;
                    y2 = ptTmpList[i].y;
                    if( !bSetFirst )
                    {
                        x1 = x2;
                        y1 = y2;
                        bSetFirst = nuTRUE;
                    }
                }
            }
        }
    }
    //	DEBUGSTRING(nuTEXT("LineLimitToRectZ OUT"));
    return nuTRUE;
}

nuBOOL CGdiMethodZK::nuBMP565Poly3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, const NUROPOINT *ptList, nuINT nCount, nuINT thick, nuWORD nColor565)
{
    
    nuINT i = 0, j = 0;
    for(j = 1; j < nCount; ++j)
    {
#ifdef NURO_USE_BMPLIB3_3DLINE
		nuBMPFillCircle_New(pBmp, ptList[i].x, ptList[i].y
			, g_pGet3DThick(thick, ptList[i].y), nColor565, NURO_BMP_TYPE_COPY);
#else
        nuBMP565FillCircleZ( pBmp, 
            ptList[i].x, 
            ptList[i].y, 
            g_pGet3DThick(thick, ptList[i].y), 
            nColor565,
            0 );
#endif
        nuBMP565Draw3DLineZ(pBmp, g_pGet3DThick,  
            ptList[i].x, 
            ptList[i].y,
            ptList[j].x,
            ptList[j].y,
            thick,
            nColor565,
            0 );
        i = j;
    }
#ifdef NURO_USE_BMPLIB3_3DLINE
	nuBMPFillCircle_New(pBmp, ptList[j-1].x, ptList[j-1].y
		, g_pGet3DThick(thick, ptList[j-1].y), nColor565, NURO_BMP_TYPE_COPY);
#else
    nuBMP565FillCircleZ( pBmp, 
        ptList[j-1].x, 
        ptList[j-1].y, 
        g_pGet3DThick(thick, ptList[j-1].y), 
        nColor565,
        0 );
#endif
    return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565DrawVertical3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, nuINT x, nuINT y1, nuINT y2, nuINT thick, 
                                                 nuWORD nColor565, nuINT nMode)
{
    nuINT y;
    if( y1 > y2 )
    {
        y = y1;
        y1 = y2;
        y2 = y;
    }
    if( y1 < 0 )
    {
        y1 = 0;
    }
    if( y2 >= pBmp->bmpHeight )
    {
        y2 = pBmp->bmpHeight - 1;
    }
    if( y1 > y2 )
    {
        return nuFALSE;
    }
    //
    nuINT aDy = y2 - y1;
    nuINT nMinThick = g_pGet3DThick(thick, y1);
    nuINT nMaxThick = g_pGet3DThick(thick, y2);
    nuINT nRank = nMaxThick - nMinThick + 1;
    nuINT xMin = x - (nMaxThick>>1);
    nuINT xMax = x + (nMaxThick>>1);
    nuINT nResidue = 0, yDis, nCount;
    nuINT nDeno, nLeftBg, nRightBg;
    nuBOOL bLeft;
    nCount = nRank;
    yDis = (aDy + nResidue) / nCount;
    nResidue = (aDy + nResidue) % nCount;
    nDeno = (yDis<<1) + 1;
    if( nMaxThick & 0x01 )
    {
        --xMin;
        bLeft		= nuFALSE;
        nLeftBg		= yDis;
        nRightBg	= 0;
    }
    else
    {
        bLeft		= nuTRUE;
        nLeftBg		= 0;
        nRightBg	= yDis;
    }
    ++xMax;
    if( xMin >= pBmp->bmpWidth || xMax <= 0 )
    {
        return nuFALSE;
    }
    nuINT yAdd = 0;
    --nRank;
    for(y = y2; y >= y1; --y, ++yAdd)
    {
        if( nRank && yAdd == yDis )
        {
            yDis = (aDy + nResidue) / nCount;
            nResidue = (aDy + nResidue) % nCount;
            nDeno = (yDis<<1) + 1;
            if( bLeft )
            {
                if( xMax > x )
                {
                    --xMax;
                    nRightBg = 0;
                }
                bLeft = nuFALSE;
            }
            else
            {
                if( xMin < x )
                {
                    ++xMin;
                    nLeftBg = 0;
                }
                bLeft = nuTRUE;
            }
            yAdd = 0;
            --nRank;
        }
        ++nLeftBg;
        ++nRightBg;
        nuWORD* pColor16 = nuBMP565LineData(pBmp, y);
        nuINT i = xMin;
        if( i >= 0 )
        {
            pColor16 += i;
#ifdef NURO_USE_LINE_MIX
            if( nLeftBg < nDeno )
            {
                nuINT nC = nDeno - nLeftBg;
                *pColor16 = BMP565_COLOR_MIX(*pColor16, nLeftBg, nColor565, nC, nDeno);
            }
#endif
            ++pColor16;
            ++i;
        }
        else
        {
            i = 0;
        }
        while(i < xMax && i < pBmp->bmpWidth )
        {
            *pColor16 = nColor565;
            ++i;
            ++pColor16;
        }
#ifdef NURO_USE_LINE_MIX
        if( i < pBmp->bmpWidth )
        {
            if( nRightBg < nDeno )
            {
                nuINT nC = nDeno - nRightBg;
                *pColor16 = BMP565_COLOR_MIX(*pColor16, nRightBg, nColor565, nC, nDeno);
            }
        }
#endif
    }
    return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565Fill3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, const NUROPOINT *ptList, nuINT nCount, 
                                         nuWORD nColor565, nuINT nType, nuINT nMode)
{
    nuWORD* pColor16;
    if( nType == 1 )
    {
        nuINT dx1 = ptList[2].x - ptList[1].x;
        nuINT dy1 = ptList[2].y - ptList[1].y;
        nuINT dx2 = ptList[0].x - ptList[1].x;
        nuINT dy2 = ptList[0].y - ptList[1].y;
        nuINT DX1 = 0, DX2 = 0, X1, X2, Y1, Y2;
        X1 = X2 = ptList[1].x;
        Y1 = Y2 = ptList[1].y;
        nuINT y1, y2;
        for(nuINT x = ptList[1].x; x <= ptList[3].x; ++x)
        {
            if( x >= 0 && x < pBmp->bmpWidth )
            {
                if( dx1 == 0 )
                {
                    y1 = Y1;
                }
                else
                {
                    if( dy1 > 0 )
                    {
                        y1 = Y1 + (DX1 * dy1 + (dx1>>1)) / dx1;
                    }
                    else
                    {
                        y1 = Y1 + (DX1 * dy1 - (dx1>>1)) / dx1;
                    }
                }
                if( dx2 == 0 )
                {
                    y2 = Y2;
                }
                else
                {
                    if( dy2 > 0 )
                    {
                        y2 = Y2 + (DX2 * dy2 + (dx2>>1))/ dx2;
                    }
                    else
                    {
                        y2 = Y2 + (DX2 * dy2 - (dx2>>1))/ dx2;
                    }
                }
                nuINT i = NURO_MAX(y1, 0);
                nuINT yEnd = NURO_MIN(y2, (pBmp->bmpHeight-1));
                while( i <= yEnd )
                {
                    nuBMP565LineData(pBmp, i)[x] = nColor565;
                    ++i;
                }
            }
            if( x == ptList[2].x )
            {
                X1 = ptList[2].x;
                Y1 = ptList[2].y;
                dx1 = ptList[3].x - ptList[2].x;
                dy1 = ptList[3].y - ptList[2].y;
                DX1 = 0;
            }
            if( x == ptList[0].x )
            {
                X2 = ptList[0].x;
                Y2 = ptList[0].y;
                dx2 = ptList[3].x - ptList[0].x;
                dy2 = ptList[3].y - ptList[0].y;
                DX2 = 0;
            }
            ++DX1;
            ++DX2;
        }
    }
    else if( nType == 2)
    {
        nuINT dx1 = ptList[1].x - ptList[0].x;
        nuINT dy1 = ptList[1].y - ptList[0].y;
        nuINT dx2 = ptList[3].x - ptList[0].x;
        nuINT dy2 = ptList[3].y - ptList[0].y;
        nuINT DX1 = 0, DX2 = 0, X1, X2, Y1, Y2;
        X1 = X2 = ptList[0].x;
        Y1 = Y2 = ptList[0].y;
        nuINT y1, y2;
        for(nuINT x = ptList[0].x; x <= ptList[2].x; ++x)
        {
            if( x >= 0 && x < pBmp->bmpWidth )
            {
                if( dx1 == 0 )
                {
                    y1 = Y1;
                }
                else
                {
                    if( dy1 > 0 )
                    {
                        y1 = Y1 + (DX1 * dy1 + (dx1>>1)) / dx1;
                    }
                    else
                    {
                        y1 = Y1 + (DX1 * dy1 - (dx1>>1)) / dx1;
                    }
                }
                if( dx2 == 0 )
                {
                    y2 = Y2;
                }
                else
                {
                    if( dy2 > 0 )
                    {
                        y2 = Y2 + (DX2 * dy2 + (dx2>>1))/ dx2;
                    }
                    else
                    {
                        y2 = Y2 + (DX2 * dy2 - (dx2>>1))/ dx2;
                    }
                }
                //				++y1;
                nuINT i = NURO_MAX(y1, 0);
                nuINT yEnd = NURO_MIN(y2, (pBmp->bmpHeight-1));
                while( i <= yEnd )
                {
                    nuBMP565LineData(pBmp, i)[x] = nColor565;
                    ++i;
                }
            }
            if( x == ptList[1].x )
            {
                X1 = ptList[1].x;
                Y1 = ptList[1].y;
                dx1 = ptList[2].x - ptList[1].x;
                dy1 = ptList[2].y - ptList[1].y;
                DX1 = 0;
            }
            if( x == ptList[3].x )
            {
                X2 = ptList[3].x;
                Y2 = ptList[3].y;
                dx2 = ptList[2].x - ptList[3].x;
                dy2 = ptList[2].y - ptList[3].y;
                DX2 = 0;
            }
            ++DX1;
            ++DX2;
        }
    }
    else if( nType == 3 )
    {
        nuINT dx1 = ptList[0].x - ptList[1].x;
        nuINT dy1 = ptList[0].y - ptList[1].y;
        nuINT dx2 = ptList[2].x - ptList[1].x;
        nuINT dy2 = ptList[2].y - ptList[1].y;
        nuINT DY1 = 0, DY2 = 0, X1, X2, Y1, Y2;
        X1 = X2 = ptList[1].x;
        Y1 = Y2 = ptList[1].y;
        nuINT x1, x2;
        for(nuINT y = ptList[1].y; y <= ptList[3].y; ++y)
        {
            if( y >= 0 && y < pBmp->bmpHeight )
            {
                if( dy1 == 0 )
                {
                    x1 = X1;
                }
                else
                {
                    if( dx1 > 0 )
                    {
                        x1 = X1 + (DY1 * dx1 + (dy1>>1)) / dy1;
                    }
                    else
                    {
                        x1 = X1 + (DY1 * dx1 - (dy1>>1)) / dy1;
                    }
                }
                if( dy2 == 0 )
                {
                    x2 = X2;
                }
                else
                {
                    if( dx2 > 0 )
                    {
                        x2 = X2 + (DY2 * dx2 + (dy2>>1)) / dy2;
                    }
                    else
                    {
                        x2 = X2 + (DY2 * dx2 - (dy2>>1)) / dy2;
                    }
                }
                nuINT i = NURO_MAX(x1, 0);
                nuINT xEnd = NURO_MIN(x2, (pBmp->bmpWidth-1));
                pColor16 = &nuBMP565LineData(pBmp, y)[i];
                while( i <= xEnd )
                {
                    *pColor16 = nColor565;
                    ++pColor16;
                    ++i;
                }
            }
            if( y == ptList[0].y )
            {
                X1 = ptList[0].x;
                Y1 = ptList[0].y;
                dx1 = ptList[3].x - ptList[0].x;
                dy1 = ptList[3].y - ptList[0].y;
                DY1 = 0;
            }
            if( y == ptList[2].y )
            {
                X2 = ptList[2].x;
                Y2 = ptList[2].y;
                dx2 = ptList[3].x - ptList[2].x;
                dy2 = ptList[3].y - ptList[2].y;
                DY2 = 0;
            }
            ++DY1;
            ++DY2;
        }
    }
    else if( nType == 4 )
    {
        nuINT dx1 = ptList[3].x - ptList[0].x;
        nuINT dy1 = ptList[3].y - ptList[0].y;
        nuINT dx2 = ptList[1].x - ptList[0].x;
        nuINT dy2 = ptList[1].y - ptList[0].y;
        nuINT DY1 = 0, DY2 = 0, X1, X2, Y1, Y2;
        X1 = X2 = ptList[0].x;
        Y1 = Y2 = ptList[0].y;
        nuINT x1, x2;
        for(nuINT y = ptList[0].y; y <= ptList[2].y; ++y)
        {
            if( y >= 0 && y < pBmp->bmpHeight )
            {
                if( dy1 == 0 )
                {
                    x1 = X1;
                }
                else
                {
                    if( dx1 > 0 )
                    {
                        x1 = X1 + (DY1 * dx1 + (dy1>>1)) / dy1;
                    }
                    else
                    {
                        x1 = X1 + (DY1 * dx1 - (dy1>>1)) / dy1;
                    }
                }
                if( dy2 == 0 )
                {
                    x2 = X2;
                }
                else
                {
                    if( dx2 > 0 )
                    {
                        x2 = X2 + (DY2 * dx2 + (dy2>>1)) / dy2;
                    }
                    else
                    {
                        x2 = X2 + (DY2 * dx2 - (dy2>>1)) / dy2;
                    }
                }
                nuINT i = NURO_MAX(x1, 0);
                nuINT xEnd = NURO_MIN(x2, (pBmp->bmpWidth-1));
                pColor16 = &nuBMP565LineData(pBmp, y)[i];
                while( i <= xEnd )
                {
                    *pColor16 = nColor565;
                    ++pColor16;
                    ++i;
                }
            }
            if( y == ptList[3].y )
            {
                X1 = ptList[3].x;
                Y1 = ptList[3].y;
                dx1 = ptList[2].x - ptList[3].x;
                dy1 = ptList[2].y - ptList[3].y;
                DY1 = 0;
            }
            if( y == ptList[1].y )
            {
                X2 = ptList[1].x;
                Y2 = ptList[1].y;
                dx2 = ptList[2].x - ptList[1].x;
                dy2 = ptList[2].y - ptList[1].y;
                DY2 = 0;
            }
            ++DY1;
            ++DY2;
        }
    }
    return nuTRUE;
}
nuBOOL CGdiMethodZK::nuBMP565Draw3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, 
                                         nuWORD nColor565, nuINT nMode)
{
    nuINT nMaxThick = g_pGet3DThick(thick, NURO_MAX(y1, y2));
#ifdef NURO_USE_BMPLIB3_3DLINE
	if (y1 == y2)
	{
		nuBMPDrawLine_New(pBmp, x1, y1, x2, y2, nMaxThick, nColor565, NURO_BMP_TYPE_COPY);
		return nuTRUE;
	}
	nuINT nMinThick = 0;
#else
    nuINT nMinThick = g_pGet3DThick(thick, NURO_MIN(y1, y2));
    if( nMaxThick == nMinThick )
    {
        if( !nMinThick ) ++nMinThick;
        return nuBMP565DrawLineZ(pBmp, x1, y1, x2, y2, nMinThick, nColor565, nMode);
    }
#endif
    if( !nuLineLimitToRectZ( x1, 
        x2, 
        y1, 
        y2, 
        -nMaxThick, 
        pBmp->bmpWidth - 1 + nMaxThick,
        -nMaxThick, 
        pBmp->bmpHeight - 1 + nMaxThick ) )
    {
        return nuFALSE;
    }
    nMaxThick = g_pGet3DThick(thick, NURO_MAX(y1, y2));
    nMinThick = g_pGet3DThick(thick, NURO_MIN(y1, y2));
#ifdef NURO_USE_BMPLIB3_3DLINE
	nuBMPDraw3DLine_New(pBmp, x1, y1, x2, y2
		, nColor565, nMinThick, nMaxThick-nMinThick, NURO_BMP_TYPE_COPY);
	return nuTRUE;
#endif
    //	if( nMaxThick > 0 )	--nMaxThick;
    //	if( nMinThick > 0 )	--nMinThick;
    if( nMaxThick == nMinThick )
    {
        if( !nMaxThick ) ++nMaxThick;
        return nuBMP565DrawLineZ(pBmp, x1, y1, x2, y2, nMaxThick, nColor565, nMode);
    }
    nuINT dx = x2 - x1;
    nuINT dy = y2 - y1;
    if( dx == 0 && dy == 0)
    {
        return nuTRUE;//nuBMP565FillCircleZ(pBmp, x1, y1, thick, nColor565, nMode);
    }
    else if( dx == 0 )
    {
        return nuBMP565DrawVertical3DLineZ(pBmp, g_pGet3DThick, x1, y1, y2, thick, nColor565, nMode);
    }
    else if( dy == 0 )
    {
        return nuBMP565DrawHorizontalLineZ(pBmp, x1, x2, y1, nMinThick, nColor565, nMode);
    }
    nuroPOINT ptList[4];
    nuINT dxy = (nuINT)(nuSqrt(dx*dx + dy*dy));
    nuINT x, y, nEx1, nEx2, nEy1, nEy2, aDx, aDy, nLen;
    nuBOOL bOddNum;
    if( NURO_ABS(dx) >= NURO_ABS(dy) )//??????????X?廗?羃迤??????腄?
    {
        if( dx < 0 )//?????????磩畎頑1 < x2;
        {
            x  = x1;
            y  = y1;
            x1 = x2;
            y1 = y2;
            x2 = x;
            y2 = y;
            dx = -dx;
            dy = -dy;
        }
        aDy = NURO_ABS(dy);
        aDx = NURO_ABS(dx);
        if( dy > 0 )
        {
            nLen = nMinThick>>1;
            if( nMinThick & 0x01 )
            {
                bOddNum = nuTRUE;
            }
            else
            {
                bOddNum = nuFALSE;
            }
            nEx1 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy1 = (dx * nLen + (dxy>>1)) / (dxy);
            ptList[0].x = x1 - nEx1;
            ptList[0].y = y1 + nEy1;
            //
            if( !bOddNum && nLen )//髒??
            {
                --nLen;
                nEx1 = (dy * nLen + (dxy>>1) ) / (dxy);
                nEy1 = (dx * nLen + (dxy>>1) ) / (dxy);
            }
            ptList[1].x = x1 + nEx1;
            ptList[1].y = y1 - nEy1;
            //
            nLen = nMaxThick>>1;
            if( nMaxThick & 0x01 )
            {
                bOddNum = nuTRUE;
            }
            else
            {
                bOddNum = nuFALSE;
            }
            nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
            ptList[3].x = x2 - nEx2;
            ptList[3].y = y2 + nEy2;
            //
            if( !bOddNum && nLen )//髒??
            {
                --nLen;
                nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
                nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
            }
            ptList[2].x = x2 + nEx2;
            ptList[2].y = y2 - nEy2;
            //
            if( ptList[0].x != ptList[3].x ||
                ptList[0].y != ptList[3].y || 
                ptList[1].x != ptList[2].x ||
                ptList[1].y != ptList[2].y )
            {
                nuBMP565Fill3DLineZ(pBmp, g_pGet3DThick, ptList, 4, nColor565, 2, 0);
                nuBMP565DrawLineZ(pBmp, ptList[1].x, ptList[1].y, ptList[2].x, ptList[2].y, 1, nColor565, 0);
            }
            nuBMP565DrawLineZ(pBmp, ptList[0].x, ptList[0].y, ptList[3].x, ptList[3].y, 1, nColor565, 0);
        }
        else
        {
            nLen = nMaxThick>>1;
            if( nMaxThick & 0x01 )
            {
                bOddNum = nuTRUE;
            }
            else
            {
                bOddNum = nuFALSE;
            }
            nEx1 = (dy * nLen - (dxy>>1)) / (dxy);
            nEy1 = (dx * nLen + (dxy>>1)) / (dxy);
            ptList[0].x = x1 - nEx1;
            ptList[0].y = y1 + nEy1;
            if( !bOddNum && nLen )
            {
                --nLen;
                nEx1 = (dy * nLen - (dxy>>1)) / (dxy);
                nEy1 = (dx * nLen + (dxy>>1)) / (dxy);
            }
            ptList[1].x = x1 + nEx1;
            ptList[1].y = y1 - nEy1;
            //
            nLen = nMinThick>>1;
            if( nMinThick & 0x01 )
            {
                bOddNum = nuTRUE;
            }
            else
            {
                bOddNum = nuFALSE;
            }
            nEx2 = (dy * nLen - (dxy>>1)) / (dxy);
            nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
            ptList[3].x = x2 - nEx2;
            ptList[3].y = y2 + nEy2;
            if( !bOddNum && nLen )
            {
                --nLen;
                nEx2 = (dy * nLen - (dxy>>1)) / (dxy);
                nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
            }
            ptList[2].x = x2 + nEx2;
            ptList[2].y = y2 - nEy2;
            if( ptList[0].x != ptList[3].x ||
                ptList[0].y != ptList[3].y || 
                ptList[1].x != ptList[2].x ||
                ptList[1].y != ptList[2].y )
            {
                nuBMP565Fill3DLineZ(pBmp, g_pGet3DThick, ptList, 4, nColor565, 1, 0);
                nuBMP565DrawLineZ(pBmp, ptList[1].x, ptList[1].y, ptList[2].x, ptList[2].y, 1, nColor565, 0);
            }
            nuBMP565DrawLineZ(pBmp, ptList[0].x, ptList[0].y, ptList[3].x, ptList[3].y, 1, nColor565, 0);
        }
}
else
{
    if( dy < 0 )//?????????磩畎頓1 < y2;
    {
        x  = x1;
        y  = y1;
        x1 = x2;
        y1 = y2;
        x2 = x;
        y2 = y;
        dx = -dx;
        dy = -dy;
    }
    aDy = NURO_ABS(dy);
    aDx = NURO_ABS(dx);
    if( dx > 0 )
    {
        nLen = nMinThick>>1;
        if( nMinThick & 0x01 )
        {
            bOddNum = nuTRUE;
        }
        else
        {
            bOddNum = nuFALSE;
        }
        nEx1 = (dy * nLen + (dxy>>1)) / (dxy);
        nEy1 = (dx * nLen + (dxy>>1)) / (dxy);
        ptList[1].x = x1 + nEx1;
        ptList[1].y = y1 - nEy1;
        if( !bOddNum && nLen )
        {
            --nLen;
            nEx1 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy1 = (dx * nLen + (dxy>>1)) / (dxy);
        }
        ptList[0].x = x1 - nEx1;
        ptList[0].y = y1 + nEy1;
        //
        nLen = nMaxThick>>1;
        if( nMaxThick & 0x01 )
        {
            bOddNum = nuTRUE;
        }
        else
        {
            bOddNum = nuFALSE;
        }
        nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
        nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
        ptList[2].x = x2 + nEx2;
        ptList[2].y = y2 - nEy2;
        if( !bOddNum && nLen )
        {
            --nLen;
            nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy2 = (dx * nLen + (dxy>>1)) / (dxy);
        }
        ptList[3].x = x2 - nEx2;
        ptList[3].y = y2 + nEy2;
        if( ptList[0].x != ptList[3].x ||
            ptList[0].y != ptList[3].y || 
            ptList[1].x != ptList[2].x ||
            ptList[1].y != ptList[2].y )
        {
            nuBMP565Fill3DLineZ(pBmp, g_pGet3DThick, ptList, 4, nColor565, 3, 0);
            nuBMP565DrawLineZ(pBmp, ptList[1].x, ptList[1].y, ptList[2].x, ptList[2].y, 1, nColor565, 0);
        }
        nuBMP565DrawLineZ(pBmp, ptList[0].x, ptList[0].y, ptList[3].x, ptList[3].y, 1, nColor565, 0);
    }
    else
    {
        nLen = nMinThick>>1;
        nEx1 = (dy * nLen + (dxy>>1)) / (dxy);
        nEy1 = (dx * nLen - (dxy>>1)) / (dxy);
        ptList[1].x = x1 + nEx1;
        ptList[1].y = y1 - nEy1;
        if( !(nMinThick&01) && nLen )
        {
            --nLen;
            nEx1 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy1 = (dx * nLen - (dxy>>1)) / (dxy);
        }
        ptList[0].x = x1 - nEx1;
        ptList[0].y = y1 + nEy1;
        nLen = nMaxThick>>1;
        nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
        nEy2 = (dx * nLen - (dxy>>1)) / (dxy);
        ptList[2].x = x2 + nEx2;
        ptList[2].y = y2 - nEy2;
        if( !(nMaxThick&0x01) && nLen )
        {
            --nLen;
            nEx2 = (dy * nLen + (dxy>>1)) / (dxy);
            nEy2 = (dx * nLen - (dxy>>1)) / (dxy);
        }
        ptList[3].x = x2 - nEx2;
        ptList[3].y = y2 + nEy2;
        
        if( ptList[0].x != ptList[3].x ||
            ptList[0].y != ptList[3].y || 
            ptList[1].x != ptList[2].x ||
            ptList[1].y != ptList[2].y )
        {
            nuBMP565Fill3DLineZ(pBmp, g_pGet3DThick, ptList, 4, nColor565, 4, 0);
            nuBMP565DrawLineZ(pBmp, ptList[1].x, ptList[1].y, ptList[2].x, ptList[2].y, 1, nColor565, 0);
        }
        nuBMP565DrawLineZ(pBmp, ptList[0].x, ptList[0].y, ptList[3].x, ptList[3].y, 1, nColor565, 0);
    }
}
return nuTRUE;
}
nuVOID CGdiMethodZK::nuBMPBilinear2_Fast(nuWORD* PColor0
									   , nuWORD* PColor1
									   , unsigned long u_16
									   , unsigned long v_16
									   , nuWORD* result
									   , nuWORD colorDefault
									   , nuINT mode)
{
    unsigned long pm3_16=(u_16>>8)*(v_16>>8);
    unsigned long pm2_16=u_16 - pm3_16;
    unsigned long pm1_16=v_16 - pm3_16;
    unsigned long pm0_16=(1<<16)-pm1_16-pm2_16-pm3_16;
    
    if (PColor0[0]== (NURO_BMP_TRANSCOLOR_565))
    {
        pm0_16 = 0;
    }
    if (PColor0[1] == (NURO_BMP_TRANSCOLOR_565))
    {
        pm2_16 = 0;
    }    
    if (PColor1[0] == (NURO_BMP_TRANSCOLOR_565))
    {
        pm1_16 = 0;
    }
    if (PColor1[1] == (NURO_BMP_TRANSCOLOR_565))
    {
        pm3_16 = 0;
    }
    nuWORD color = nuRGB24TO16_565(
		(pm0_16*nu565RVALUE(PColor0[0])+pm2_16*nu565RVALUE(PColor0[1])+pm1_16*nu565RVALUE(PColor1[0])+pm3_16*nu565RVALUE(PColor1[1]))>>16
        , (pm0_16*nu565GVALUE(PColor0[0])+pm2_16*nu565GVALUE(PColor0[1])+pm1_16*nu565GVALUE(PColor1[0])+pm3_16*nu565GVALUE(PColor1[1]))>>16
        , (pm0_16*nu565BVALUE(PColor0[0])+pm2_16*nu565BVALUE(PColor0[1])+pm1_16*nu565BVALUE(PColor1[0])+pm3_16*nu565BVALUE(PColor1[1]))>>16 
		);
//     nuWORD colorBuffer[4];
//     colorBuffer[0] = PColor0[0];
//     if (colorBuffer[0] == nuLOWORD(NURO_BMP_TRANSCOLOR_565))
//     {
//         colorBuffer[0] = colorDefault;
//     }
//     colorBuffer[1] = PColor0[1];
//     if (colorBuffer[1] == nuLOWORD(NURO_BMP_TRANSCOLOR_565))
//     {
//         colorBuffer[1] = colorDefault;
//     }    
//     colorBuffer[2] = PColor1[0];
//     if (colorBuffer[2] == nuLOWORD(NURO_BMP_TRANSCOLOR_565))
//     {
//         colorBuffer[2] = colorDefault;
//     }
//     colorBuffer[3] = PColor1[1];
//     if (colorBuffer[3] == nuLOWORD(NURO_BMP_TRANSCOLOR_565))
//     {
//         colorBuffer[3] = colorDefault;
//     }
//     nuWORD color = nuRGB24TO16_565(
// 		(pm0_16*nu565RVALUE(colorBuffer[0])+pm2_16*nu565RVALUE(colorBuffer[1])+pm1_16*nu565RVALUE(colorBuffer[2])+pm3_16*nu565RVALUE(colorBuffer[3]))>>16
//         , (pm0_16*nu565GVALUE(colorBuffer[0])+pm2_16*nu565GVALUE(colorBuffer[1])+pm1_16*nu565GVALUE(colorBuffer[2])+pm3_16*nu565GVALUE(colorBuffer[3]))>>16
//         , (pm0_16*nu565BVALUE(colorBuffer[0])+pm2_16*nu565BVALUE(colorBuffer[1])+pm1_16*nu565BVALUE(colorBuffer[2])+pm3_16*nu565BVALUE(colorBuffer[3]))>>16 
// 		);
	nuBMPSetPixelData565(result, color, mode);
}
nuVOID CGdiMethodZK::nuBMPBilinear2_Border(PNURO_BMP pBmp
										 , const long x0
										 , const long y0
										 , nuroRECT& rect
// 										 , const long w0
// 										 , const long h0
                                         , const long x_16
										 , const long y_16
										 , nuWORD* result
										 , nuWORD colorDefault
										 , nuINT mode)
{
    long x=(x_16>>16)+x0;
    long y=(y_16>>16)+y0;
    unsigned long u_16=((unsigned short)(x_16));
    unsigned long v_16=((unsigned short)(y_16));    
    nuWORD pixel[4];
//     nuroRECT rect;
//     rect.left = x0;
//     rect.right = x0 + w0;
//     rect.top = y0;
//     rect.bottom = y0 + h0;
    pixel[0]=nuBMPGetPixel16_(pBmp, x, y, colorDefault, rect);
    pixel[1]=nuBMPGetPixel16_(pBmp, x+1, y, colorDefault, rect);
    pixel[2]=nuBMPGetPixel16_(pBmp, x, y+1, colorDefault, rect);
    pixel[3]=nuBMPGetPixel16_(pBmp, x+1, y+1, colorDefault, rect);
    nuBMPBilinear2_Fast(&pixel[0],&pixel[2],u_16,v_16,result, colorDefault, mode);//>>8
}
nuVOID CGdiMethodZK::nuBMPZoom0(PNURO_BMP pDesBmp
                              , nuINT desx
                              , nuINT desy
                              , nuINT desw
                              , nuINT desh
                              , PNURO_BMP pSrcBmp
                              , nuINT srcx
                              , nuINT srcy
                              , nuINT srcw
                              , nuINT srch)
{
    /*
    if (desw<=0
        || desh<=0
        || desx>=pDesBmp->bmpWidth
        || desy>=pDesBmp->bmpHeight
        || (desx+desw)<=0
        || (desy+desh)<=0 )
    {
        return;
    }
    nuLONG xrIntFloat_16=(srcw<<16)/desw+1;
    nuLONG yrIntFloat_16=(srch<<16)/desh+1;
    nuWORD* pDstLine = 0;
    nuWORD* pSrcLine = 0;    
    nuLONG y_16 = 0;
    nuWORD transColor = (nuWORD)pSrcBmp->bmpTransfColor;
    long bytesWidthDes = nuBMP565BytesPerLine(pDesBmp);
    long bytesWidthSrc = nuBMP565BytesPerLine(pSrcBmp);
    for (nuLONG y=0; y<desh; ++y)
    {
        if ( nuBMP565InY(pDesBmp, desy+y) )
        {
            pDstLine = nuBMP565LineData(pDesBmp, desy+y);
            pSrcLine = nuBMP565LineData(pSrcBmp, srcy+(y_16>>16));
            nuLONG x_16 = 0;
            for (nuLONG x=0; x<desw; ++x)
            {
                if ( nuBMP565InX(pDesBmp, desx+x) )
                {
                    if(pSrcLine[srcx+(x_16>>16)] != transColor)
                    {
                        pDstLine[desx+x] = pSrcLine[srcx+(x_16>>16)];
                    }
                }
                x_16 += xrIntFloat_16;
            }
        }
        y_16 += yrIntFloat_16;
    }*/
    if (desw<=0
        || desh<=0
        || desx>=pDesBmp->bmpWidth
        || desy>=pDesBmp->bmpHeight
        || (desx+desw)<=0
        || (desy+desh)<=0 )
    {
        return;
    }
    nuLONG xrIntFloat_16=(srcw<<16)/desw+1;
    nuLONG yrIntFloat_16=(srch<<16)/desh+1;
    nuWORD* pDstLine = 0;
    nuWORD* pSrcLine = 0;    
    nuLONG y_16 = 0;
    nuWORD transColor = (nuWORD)pSrcBmp->bmpTransfColor;
    long bytesWidthDes = nuBMP565BytesPerLine(pDesBmp)/2;
    long bytesWidthSrc = nuBMP565BytesPerLine(pSrcBmp)/2;
    nuLONG x_16 = 0;
    pDstLine = nuBMP565LineData(pDesBmp, desy);
    nuINT ytag = 0;
    pSrcLine = nuBMP565LineData(pSrcBmp, srcy+(y_16>>16));
    nuWORD* pDstTag = 0;
    nuWORD* pSrcTag = 0;
    nuINT xtag = 0;
    for (nuLONG y=0; y<desh; ++y)
    {
        x_16 = 0;
        pDstTag = pDstLine + desx;
        xtag = 0;
        pSrcTag = pSrcLine + srcx;
        for (nuLONG x=0; x<desw; ++x)
        {
            if(*pSrcTag != transColor)
            {
                *pDstTag = *pSrcTag;
            }
            ++pDstTag;
            x_16 += xrIntFloat_16;
            if ((x_16>>16)>xtag)
            {
                ++xtag;
                ++pSrcTag;
            }
        }
        y_16 += yrIntFloat_16;
        pDstLine += bytesWidthDes;
        if ((y_16>>16)>ytag)
        {
            ++ytag;
            pSrcLine += bytesWidthSrc;
        }
    }
}
nuVOID CGdiMethodZK::nuBMPZoom_Bilinear2(PNURO_BMP pDesBmp
                                       , nuINT desx
                                       , nuINT desy
                                       , nuINT desw
                                       , nuINT desh
                                       , PNURO_BMP pSrcBmp
                                       , nuINT srcx
                                       , nuINT srcy
                                       , nuINT srcw
                                       , nuINT srch
									   , nuINT mode)
{
    long xrIntFloat_16=((srcw)<<16)/desw+1; 
    long yrIntFloat_16=((srch)<<16)/desh+1;
    const long csDErrorX=-(1<<15)+(xrIntFloat_16>>1);
    const long csDErrorY=-(1<<15)+(yrIntFloat_16>>1);
    
    long border_y0 = -csDErrorY/yrIntFloat_16+1;
    if (border_y0>=desh)
        border_y0 = desh;
    long border_x0 = -csDErrorX/xrIntFloat_16+1;
    if (border_x0>=desw ) 
        border_x0=desw;
    long border_y1=(((srch-2)<<16)-csDErrorY)/yrIntFloat_16+1;
    if (border_y1<border_y0)
        border_y1=border_y0;
    long border_x1=(((srcw-2)<<16)-csDErrorX)/xrIntFloat_16+1; 
    if (border_x1<border_x0) 
        border_x1=border_x0;
    
    nuWORD* pDesLine = (nuWORD*)pDesBmp->pBmpBuff;
    long bytesWidthDes = nuBMP565BytesPerLine(pDesBmp);
    long bytesWidthSrc = nuBMP565BytesPerLine(pSrcBmp);
    long srcy_16=csDErrorY;
    long y = 0;
    long x = 0;
    nuroRECT rect;
    rect.left = srcx;
	rect.right = srcx + srcw;
	rect.top = srcy;
	rect.bottom = srcy + srch;
    ((nuBYTE*&)pDesLine) += bytesWidthDes*desy;
    for (y=0;y<border_y0;++y)
    {
        if (nuBMP565InY(pDesBmp, desy+y))
        {
            long srcx_16=csDErrorX;
            for (x=0;x<desw;++x)
            {
                if ( !nuBMP565InX(pDesBmp, desx+x) )
                {
                    srcx_16+=xrIntFloat_16;
                    continue;
                }
                nuBMPBilinear2_Border(pSrcBmp, srcx, srcy, rect, srcx_16,srcy_16,&pDesLine[x+desx], pDesLine[x+desx], mode);
                srcx_16+=xrIntFloat_16;
            }
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
    for (y=border_y0; y<border_y1; ++y)
    {
        if (!nuBMP565InY(pDesBmp, desy+y))
        {
            srcy_16+=yrIntFloat_16;
            ((nuBYTE*&)pDesLine)+=bytesWidthDes;
            continue;
        }
        long srcx_16=csDErrorX;
        for (x=0; x<border_x0; ++x)
        {
            if ( nuBMP565InX(pDesBmp, desx+x) )
            {
                nuBMPBilinear2_Border(pSrcBmp, srcx, srcy, rect, srcx_16,srcy_16,&pDesLine[x+desx], pDesLine[x+desx], mode);
            }
            srcx_16+=xrIntFloat_16;
        }
        unsigned long v_8=(srcy_16 & 0xFFFF);
        nuWORD* PSrcLineColor= (nuWORD*) (pSrcBmp->pBmpBuff+bytesWidthSrc*(srcy+(srcy_16>>16))) ;
        for (x=border_x0; x<border_x1; ++x)
        {
            if ( nuBMP565InX(pDesBmp, desx+x) )
            {
                nuWORD* PColor0=&PSrcLineColor[srcx+(srcx_16>>16)];
                nuWORD* PColor1=(nuWORD*)((nuBYTE*)(PColor0)+bytesWidthSrc);
                nuBMPBilinear2_Fast(PColor0,PColor1,(srcx_16 & 0xFFFF),v_8,&pDesLine[x+desx], pDesLine[x+desx], mode);
            }
            srcx_16+=xrIntFloat_16;
        }
        for (x=border_x1;x<desw;++x)
        {
            if ( nuBMP565InX(pDesBmp, desx+x) )
            {
                nuBMPBilinear2_Border(pSrcBmp, srcx, srcy, rect, srcx_16,srcy_16,&pDesLine[x+desx], pDesLine[x+desx], mode);
            }
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
    for (y=border_y1;y<desh;++y)
    {
        if (nuBMP565InY(pDesBmp, desy+y))
        {
            long srcx_16=csDErrorX;
            for (x=0;x<desw;++x)
            {
                if ( nuBMP565InX(pDesBmp, desx+x) )
                {
                    nuBMPBilinear2_Border(pSrcBmp, srcx, srcy, rect, srcx_16,srcy_16,&pDesLine[x+desx], pDesLine[x+desx], mode);
                }
                srcx_16+=xrIntFloat_16;
            }
        }
        srcy_16+=yrIntFloat_16;
        ((nuBYTE*&)pDesLine)+=bytesWidthDes;
    }
}
nuVOID CGdiMethodZK::nuBMPRotary_BilInear_CopyLine(nuWORD* pDstLine,long dst_border_x0,long dst_in_x0,long dst_in_x1,long dst_border_x1
                                                 , const PNURO_BMP pSrcBmp
                                                 , const long x0, const long y0, nuroRECT& rect//, const long w0, const long h0
                                                 , long srcx0_16,long srcy0_16,long Ax_16,long Ay_16, nuINT mode)
{
    long x;
    for (x=dst_border_x0;x<dst_border_x1;++x)
    {
        nuBMPBilinear2_Border(pSrcBmp, x0, y0, rect, srcx0_16, srcy0_16, &pDstLine[x], pDstLine[x], mode);
        srcx0_16+=Ax_16;
        srcy0_16+=Ay_16;
    }
//     for (x=dst_border_x0;x<dst_in_x0;++x)
//     {
//         nuBMPBilinear2_Border(pSrcBmp, x0, y0, rect, srcx0_16, srcy0_16, &pDstLine[x], pDstLine[x], mode);
//         srcx0_16+=Ax_16;
//         srcy0_16+=Ay_16;
//     }
//     for (x=dst_in_x0;x<dst_in_x1;++x)
//     {
//         nuBMPBilinear2_Border(pSrcBmp, x0, y0, rect, srcx0_16, srcy0_16, &pDstLine[x], pDstLine[x], mode);
//         srcx0_16+=Ax_16;
//         srcy0_16+=Ay_16;
//     }
//     for (x=dst_in_x1;x<dst_border_x1;++x)
//     {
//         nuBMPBilinear2_Border(pSrcBmp, x0, y0, rect, srcx0_16, srcy0_16, &pDstLine[x], pDstLine[x], mode);
//         srcx0_16+=Ax_16;
//         srcy0_16+=Ay_16;
//     }
}
nuVOID CGdiMethodZK::nuBMPRotaryBilInear(const PNURO_BMP pDstBmp,const PNURO_BMP pSrcBmp
                                       , long srcx, long srcy, long srcw, long srch, float centerScaleX, float centerScaleY
                                       , long RotaryAngle,double ZoomX,double ZoomY,long move_x,long move_y, nuINT mode)
{    
    double tmprZoomXY=1.0/(ZoomX*ZoomY); 
    double rZoomX=tmprZoomXY*ZoomY;
    double rZoomY=tmprZoomXY*ZoomX;
    double sinA = nuSin(RotaryAngle);
    double cosA = nuCos(RotaryAngle);
    long Ax_16=(long)(rZoomX*cosA*(1<<16));
    long Ay_16=(long)(rZoomX*sinA*(1<<16));
    long Bx_16=(long)(-rZoomY*sinA*(1<<16));
    long By_16=(long)(rZoomY*cosA*(1<<16));
    
    long centerx = (long)(srcw * centerScaleX);
    long centery = (long)(srch * centerScaleY);
    
    long Cx_16=(long)((-(move_x)*rZoomX*cosA+(move_y)*rZoomY*sinA+centerx)*(1<<16));
    long Cy_16=(long)((-(move_x)*rZoomX*sinA-(move_y)*rZoomY*cosA+centery)*(1<<16));
    
	nuroRECT rect;
    rect.left = srcx;
	rect.right = srcx + srcw;
	rect.top = srcy;
	rect.bottom = srcy + srch;
    
    TRotaryClipData rcData;
    rcData.Ax_16=Ax_16;
    rcData.Bx_16=Bx_16;
    rcData.Cx_16=Cx_16;
    rcData.Ay_16=Ay_16;
    rcData.By_16=By_16;
    rcData.Cy_16=Cy_16;
    rcData.dst_width=pDstBmp->bmpWidth;
    rcData.dst_height=pDstBmp->bmpHeight;
    rcData.src_width=srcw;
    rcData.src_height=srch;
    if (!rcData.inti_clip(move_x, move_y,1)) return;
    
    nuLONG bytesWidthDst = nuBMP565BytesPerLine(pDstBmp);
    nuWORD* pDstLine= (nuWORD*)pDstBmp->pBmpBuff;
    ((nuBYTE*&)pDstLine)+=(bytesWidthDst*rcData.out_dst_down_y);
    while (nuTRUE) //to down
    {
        long y=rcData.out_dst_down_y;
        if (y>=pDstBmp->bmpHeight) break;
        if (y>=0)
        {
            nuBMPRotary_BilInear_CopyLine(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                rcData.out_dst_x1_in,rcData.out_dst_x1_boder
                ,pSrcBmp, srcx, srcy, rect//srcw, srch
                ,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16, mode);
        }
        if (!rcData.next_clip_line_down()) break;
        ((nuBYTE*&)pDstLine)+=bytesWidthDst;
    }
    
    pDstLine= (nuWORD*)pDstBmp->pBmpBuff;
    ((nuBYTE*&)pDstLine)+=(bytesWidthDst*rcData.out_dst_up_y);
    while (rcData.next_clip_line_up()) //to up 
    {
        long y=rcData.out_dst_up_y;
        if (y<0) break;
        ((nuBYTE*&)pDstLine)-=bytesWidthDst;
        if (y<pDstBmp->bmpHeight)
        {
            nuBMPRotary_BilInear_CopyLine(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                rcData.out_dst_x1_in,rcData.out_dst_x1_boder
                ,pSrcBmp, srcx, srcy, rect//srcw, srch
                ,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16, mode);
        }
    }
}

