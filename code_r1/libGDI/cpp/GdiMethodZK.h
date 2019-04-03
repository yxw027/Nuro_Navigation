// GdiMethodZK.h: interface for the CGdiMethodZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIMETHODZK_H__58B38FEC_D5C0_43A4_AAA2_7865C37581D4__INCLUDED_)
#define AFX_GDIMETHODZK_H__58B38FEC_D5C0_43A4_AAA2_7865C37581D4__INCLUDED_

#include "NuroDefine.h"
#include "nurocolor.h"

typedef nuLONG(*Get3DThickProc)(nuLONG thick, nuLONG y);

#define MOVESCALE	10

#define NURO_USE_BMPLIB3
#define NURO_USE_BMPLIB3_3DLINE
#define NURO_USE_BMPLIB3_FILLGON

#define NURO_DRAW_WAY
#define NURO_FONT_WAY

#define NURO_USE_LINE_MIX
#define NURO_USE_AALINE

#define DASHLEN				10
#define TMPDASHLIST			5

#define NURO_PTINBMP(pBmp, x, y) ( x>=0 && x<pBmp->bmpWidth && y>=0 && y<pBmp->bmpHeight)

#define nuRGB24TO16_565(r,g,b)  ((nuWORD)( ((((nuWORD)(b))&0xf8)>>3) | ((((nuWORD)(g))&0xfc)<<3) | ((((nuWORD)(r))&0xf8)<<8) ))
#define nuRGB24TO16_555(r,g,b)  ((nuWORD)( ((((nuWORD)(b))&0xf8)>>3) | ((((nuWORD)(g))&0xf8)<<2) | ((((nuWORD)(r))&0xf8)<<7) ))

#define nuCOLOR24TO16_565(color)	(nuRGB24TO16_565( (color)&0xFF, ((color)>>8)&0xFF, ((color)>>16)&0xFF ))
#define nuCOLOR24TO16_555(color)	(nuRGB24TO16_555( (color)&0xFF, ((color)>>8)&0xFF, ((color)>>16)&0xFF ))

#define nuCOLOR16_565TO24(color16_565)	    ( nuRGB( (color16_565)>>8, ((color16_565)&0x07F0)>>3, ((color16_565)&0x001F)<<3 ) )
#define nuHVCOLOR16_565TO24(color16_565)    ( nuRGB( ((color16_565)&0x001F)<<3, ((color16_565)&0x07F0)>>3, (color16_565)>>8) )
#define nuCOLOR16_555TO24(color16_555)	    ( nuRGB( (((color16_555)& 0x7C00)>>7)|0x7, (((color16_555)& 0x03E0)>>2)|0x7, (((color16_555)& 0x001F)<<3)|0x7 ) )

#define nu565RVALUE(color565)		( (nuINT)( (color565)>>8 ) )
#define nu565GVALUE(color565)		( (nuINT)( (((color565)& 0x07E0)>>3)|0x3 ) )
#define nu565BVALUE(color565)		( (nuINT)( (((color565)& 0x001F)<<3)|0x7 ) )

#define BMP565_COLOR_MIX(color1, nN1, color2, nN2, nD) ( nuRGB24TO16_565( \
    ((nu565RVALUE(color1) * nN1 + nu565RVALUE(color2) * nN2) / nD), \
    ((nu565GVALUE(color1) * nN1 + nu565GVALUE(color2) * nN2) / nD), \
    ((nu565BVALUE(color1) * nN1 + nu565BVALUE(color2) * nN2) / nD)  \
) )

class NrBitmap8D2
{
private:
    nuINT width_;
    nuINT height_;
    nuINT bytesperline_;
    nuINT type_;
    nuBYTE* buffer_;
public:
    NrBitmap8D2();
    ~NrBitmap8D2();
    nuVOID Destroy();
    nuBOOL Create(nuINT width, nuINT height);
    nuBOOL CreateFromBuffer(nuINT width, nuINT height, nuVOID* buffer);
    nuBOOL CopyTo(NrBitmap8D2* dstbmp, nuINT dstx, nuINT dsty, nuINT srcx, nuINT srcy, nuINT srcw, nuINT srch);
    
    nuBOOL RenderTest(NURO_BMP* dstbmp, nuINT x0, nuINT y0, nuroColor& color);
    
    nuBOOL RenderOut(NURO_BMP* dstbmp
        , nuINT x0
        , nuINT y0
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuBOOL RenderOutZoom0(NURO_BMP* pDesBmp
        , nuINT desx
        , nuINT desy
        , nuFLOAT scalex
        , nuFLOAT scaley
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuBOOL RenderOutZoomBilinear2(NURO_BMP* pDesBmp
        , nuINT desx
        , nuINT desy
        , nuFLOAT scalex
        , nuFLOAT scaley
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuVOID Bilinear2Color(nuBYTE* PColor0
        , nuBYTE* PColor1
        , nuDWORD u_16
        , nuDWORD v_16
        , nuWORD* result
        , nuroColor& colorDefault
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuVOID Bilinear2BorderColor(const nuLONG x_16
        , const nuLONG y_16
        , nuWORD* result
        , nuroColor& colorDefault
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuVOID RotaryBilinear2Line(nuWORD* pDstLine
        , nuLONG dst_border_x0
        , nuLONG dst_in_x0
        , nuLONG dst_in_x1
        , nuLONG dst_border_x1
        , nuLONG srcx0_16
        , nuLONG srcy0_16
        , nuLONG Ax_16
        , nuLONG Ay_16                                 
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    nuVOID RenderOutRotaryBilinear2(const PNURO_BMP pDstBmp
        , nuLONG centerx
        , nuLONG centery
        , nuLONG centernewx
        , nuLONG centernewy   
        , nuLONG RotaryAngle
        , double ZoomX
        , double ZoomY                    
        , nuroColor& color
        , nuBOOL useEdge
        , nuroColor& edgecolor);
    inline nuINT width()
    {
        return width_;
    }
    inline nuINT height()
    {
        return height_;
    }
    inline nuINT bytesperline()
    {
        return bytesperline_;
    }
    inline nuBYTE* buffer()
    {
        return buffer_;
    }
    inline nuBYTE* LineData(nuINT y)
    {
        return &buffer_[bytesperline_*y];
    }
    inline nuBYTE GetPixel(nuINT x, nuINT y)
    {
        if (x<0 
            || x>=width_
            || y<0
            || y>=height_)
        {
            return 0;
        }
        return buffer_[bytesperline_*y + x];
    }
    inline nuVOID SetPixel(nuINT x, nuINT y, nuBYTE color8)
    {
        buffer_[bytesperline_*y + x] = color8;
    }
    nuVOID FillColor(nuBYTE color8);
};

class CGdiMethodZK  
{
public:
    CGdiMethodZK();
    virtual ~CGdiMethodZK();
    
    nuBOOL Initialize();
    nuVOID Free();
    
    //inline static nuVOID BmpSetPixel(nuWORD* pClrDes, nuWORD nClrSrc, nuINT nMode)
    inline static nuVOID BmpSetPixel(nuWORD* pClrDes, nuINT nClrSrc, nuINT nMode)
    {
        if (nMode == NURO_BMP_TYPE_MIXX )
        {
            *pClrDes = nClrSrc;
        }
        else if( nMode <= NURO_BMP_TYPE_MIXX )
        {
//             nuBYTE realR = (nu565RVALUE(nClrSrc) * nMode + nu565RVALUE(*pClrDes) * (16-nMode)) / 16;
//             nuBYTE realG = (nu565GVALUE(nClrSrc) * nMode + nu565GVALUE(*pClrDes) * (16-nMode)) / 16;
//             nuBYTE realB = (nu565BVALUE(nClrSrc) * nMode + nu565BVALUE(*pClrDes) * (16-nMode)) / 16;
//             *pClrDes = nuRGB24TO16_565(realR, realG, realB);
            //nuWORD oldColor16 = *pClrDes;
            nuINT oldColor16 = *pClrDes;
            nuINT realR = oldColor16 >> 11;
            nuINT realG = oldColor16 & 0x07E0;
            nuINT realB = oldColor16 & 0x1f;
            realR += (((nClrSrc>>11)-realR)*nMode)>>4;
            realG += (((nClrSrc&0x07E0)-realG)*nMode)>>4;
            realB += (((nClrSrc&0x1f)-realB)*nMode)>>4;
            *pClrDes = (realR<<11) | (realG&0x07E0) | (realB);
        }
        else if( NURO_BMP_TYPE_AND == nMode )
        {
            *pClrDes &= nClrSrc;
        }
        else if( NURO_BMP_TYPE_OR == nMode )
        {
            *pClrDes |= nClrSrc;
        }
        else if( NURO_BMP_TYPE_XOR == nMode )
        {
            *pClrDes ^= nClrSrc;
        }
        else
        {
            *pClrDes = nClrSrc;
        }
    }
    inline static nuVOID BmpLineTCopy(nuWORD* pClrDes, nuWORD* pClrSrc, nuINT nPixelNum, nuINT nMode);    
    static nuBOOL nrAreaCoverClip(nuINT dstwidth, nuINT dstheight, nuINT srcwidth, nuINT srcheight
        , nuINT& dstx, nuINT& dsty, nuINT& srcx, nuINT& srcy, nuINT& srcw, nuINT& srch);
    static nuBOOL nuGMCopyBmp(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
        , const NURO_BMP* pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT mode);
    static nuBOOL nuGMPaintBmp(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
        , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT mode);
    static nuBOOL BmpCopy(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT width, nuINT height
        , const NURO_BMP* pSrcBmp, nuINT nSrcX, nuINT nSrcY );
    static nuBOOL BmpTCopy( PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT width, nuINT height
        , const NURO_BMP* pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT mode);
    static nuBOOL BmpPaint( PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
        , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY );
    static nuBOOL BmpTPaint( PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT width, nuINT height
        , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT mode);    
    static nuBOOL BmpDrawCheck( PNURO_BMP pDesBmp, nuINT &nDesX, nuINT &nDesY, nuINT &width, nuINT &height
        , const NURO_BMP* pSrcBmp, nuINT &nSrcX, nuINT &nSrcY);
    static inline nuLONG  nuBMP565BytesPerLine(PNURO_BMP pBmp)
    {    
        return pBmp->bytesPerLine;
    }
    static inline nuBOOL nuBMP565InY(PNURO_BMP pBmp, nuINT y)
    {
        return (y>=0) && (y<pBmp->bmpHeight);
     }
    static inline nuBOOL nuBMP565InX(PNURO_BMP pBmp, nuINT x)
    {
        return (x>=0) && (x<pBmp->bmpWidth);
    }
    static inline nuBOOL nuPtInRect(nuINT x, nuINT y, const NURORECT& rect)
    {
        return (x >= rect.left) && (x < rect.right) && (y >= rect.top) && (y < rect.bottom);
    }
    static inline nuWORD* nuBMP565LineData(PNURO_BMP pBmp, nuINT y)
    {
        return (nuWORD*)(pBmp->pBmpBuff + pBmp->bytesPerLine*y);
    }
    static inline nuWORD* nuBMPGetPixel16Ptr(PNURO_BMP pBmp, nuINT x, nuINT y)
    {
//         if( pBmp == nuNULL || !NURO_PTINBMP(pBmp, x, y) )
//         {
//             return 0;
//         }
        if( !NURO_PTINBMP(pBmp, x, y) )
        {
            return 0;
        }
        return nuBMP565LineData(pBmp, y)+x;
    }
    static inline nuCOLORREF nuBMPGetPixel(PNURO_BMP pBmp, nuINT x, nuINT y)
    {
//         if( pBmp == nuNULL || !NURO_PTINBMP(pBmp, x, y) )
//         {
//             return 0;
//         }
        if( !NURO_PTINBMP(pBmp, x, y) )
        {
            return 0;
        }
        return (nuCOLORREF)nuCOLOR16_565TO24(nuBMP565LineData(pBmp, y)[x]);
    }
    static inline nuWORD nuBMPGetPixel16(PNURO_BMP pBmp, nuINT x, nuINT y)
    {
//         if( pBmp == nuNULL || !NURO_PTINBMP(pBmp, x, y) )
//         {
//             return 0;
//         }
        if( !NURO_PTINBMP(pBmp, x, y) )
        {
            return 0;
        }
        return nuBMP565LineData(pBmp, y)[x];
    }
    static inline nuWORD nuBMPGetPixel16_(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD colorDefault, nuroRECT& rect)
    {    
//         if( pBmp == NULL || !nuPtInRect(x, y, rect) )
//         {
//             return colorDefault;
//         }    
        if(!nuPtInRect(x, y, rect) )
        {
            return colorDefault;
        }
        nuWORD *pColor16 = NULL;   
        nuWORD color = nuBMP565LineData(pBmp, y)[x];
        if (color == pBmp->bmpTransfColor)
        {
            return colorDefault;
        }
        return color;
    }
    static inline nuVOID nuBMPSetPixel(PNURO_BMP pBmp, nuINT x, nuINT y, nuCOLORREF color, nuINT nMode)
    {
//         if( pBmp == nuNULL || !NURO_PTINBMP(pBmp, x, y) )
//         {
//             return;
//         }
        if( !NURO_PTINBMP(pBmp, x, y) )
        {
            return;
        }
//        nuBMPSetPixel565(pBmp, x, y, nuCOLOR24TO16_565(color), nMode);
        nuBMPSetPixelData565(nuBMP565LineData(pBmp, y)+x, nuCOLOR24TO16_565(color),nMode);
    }
    static inline nuVOID nuBMPSetPixel565(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD color565, nuINT nMode)
    {
//         if( pBmp==nuNULL || pBmp->pBmpBuff== nuNULL || !NURO_PTINBMP(pBmp, x, y) )
//         {
//             return;
//         }
        if( !NURO_PTINBMP(pBmp, x, y) )
        {
            return;
        }
        nuBMPSetPixelData565(nuBMP565LineData(pBmp, y)+x, color565, nMode);
    }
    static inline nuVOID nuBMPSetPixelData565(nuWORD* pColor16, nuWORD color565, nuINT nMode)
    {
        if(nMode == NURO_BMP_TYPE_COPY)
        {
            *pColor16 = color565;
            return;
        }
        if(nMode == NURO_BMP_TYPE_MIX0)
        {
            return;
        }
        if(nMode<0x10)
        {
//             nuBYTE realR = (nu565RVALUE(color565)*nMode+nu565RVALUE(*pColor16)*(16-nMode))/16;
//             nuBYTE realG = (nu565GVALUE(color565)*nMode+nu565GVALUE(*pColor16)*(16-nMode))/16;
//             nuBYTE realB = (nu565BVALUE(color565)*nMode+nu565BVALUE(*pColor16)*(16-nMode))/16;
//             *pColor16 = nuRGB24TO16_565(realR,realG,realB);
//             return;
            nuWORD oldColor16 = *pColor16;
            nuINT realR = oldColor16 >> 11;
            nuINT realG = oldColor16 & 0x07E0;
            nuINT realB = oldColor16 & 0x1f;                
            realR += (((color565>>11)-realR)*nMode)>>4;
            realG += (((color565&0x07E0)-realG)*nMode)>>4;
            realB += (((color565&0x1f)-realB)*nMode)>>4;
            *pColor16 = (realR<<11) | (realG&0x07E0) | (realB);
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
    static nuVOID nuBMPDrawLineHor(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawLineVer(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPSetPixel565_BMP(PNURO_BMP pBmp, nuINT x, nuINT y, PNURO_BMP pBmp2, nuINT nMode);
    static nuVOID nuBMPDrawLineHor_BMP(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, PNURO_BMP pBmp2, nuINT nMode);
    static nuVOID nuBMPDrawLineVer_BMP(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, PNURO_BMP pBmp2, nuINT nMode);
    static nuVOID nuBMPDrawLine1(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawLine1Ex(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawEllipse1(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawCircle1(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT radius, nuCOLORREF color, nuINT nMode);    
    static nuVOID nuBMPDrawRhombic(PNURO_BMP pBmp, nuINT cx, nuINT cy, nuINT dx, nuINT dy, nuINT w, nuINT h, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawLine(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawLineEx(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawLineYT3DEx(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick0, nuINT thickD, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawEllipse(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawCircle(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT radius, nuINT thick, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPDrawRectLine(PNURO_BMP pBmp, nuINT left, nuINT top, nuINT w, nuINT h, nuINT thick, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPFillGon(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuCOLORREF color, nuCOLORREF edgeColor, nuINT nMode);
    static nuVOID nuBMPFillGon_BMP(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, PNURO_BMP pBmp2, nuCOLORREF* pEdgeColor, nuINT nMode);
    static nuVOID nuBMPFillEllipse(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPFillCircle(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT radius, nuCOLORREF color, nuINT nMode);
    static nuVOID nuBMPFillRect(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT w, nuINT h, nuCOLORREF color, nuINT nMode);    
    static nuVOID nuBMPPolyLine(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuINT thick, nuCOLORREF color, nuINT nMode);
    //zhikun    
    static nuBOOL nuBMP565PolyLineZ(PNURO_BMP pBmp, const NUROPOINT* ptList, nuINT nCount, nuINT thick, nuWORD nColor565);
    static nuBOOL nuBMP565DrawLineZ(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565DrawVerticalLineZ(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuINT thick, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565DrawHorizontalLineZ(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuINT thick, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565FillCircleZ(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565FillCircleD1Z(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565FillCircleD2Z(PNURO_BMP pBmp, nuINT x, nuINT y, nuINT diameter, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuLineLimitToRectZ(nuINT &x1, nuINT& x2, nuINT& y1, nuINT& y2, nuINT nL, nuINT nR, nuINT nT, nuINT nB);
    //3D Line
    static nuBOOL nuBMP565Poly3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, const NUROPOINT *ptList, nuINT nCount, nuINT thick, nuWORD nColor565);
    static nuBOOL nuBMP565DrawVertical3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, nuINT x, nuINT y1, nuINT y2, nuINT thick, nuWORD nColor565, nuINT nMode);
    static nuBOOL nuBMP565Fill3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, const NUROPOINT *ptList, nuINT nCount, nuWORD nColor565, nuINT nType, nuINT nMode);
    static nuBOOL nuBMP565Draw3DLineZ(PNURO_BMP pBmp, Get3DThickProc g_pGet3DThick, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuWORD nColor565, nuINT nMode);
    //Louis
    static nuVOID nuBMPDrawLineExL(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT thick, nuCOLORREF color, nuINT nMode);
    
    static inline nuVOID nuBMPBilinear2_Fast(nuWORD* PColor0
        , nuWORD* PColor1
        , nuDWORD u_8
        , nuDWORD v_8
        , nuWORD* result
        , nuWORD colorDefault
        , nuINT mode);
    static inline nuVOID nuBMPBilinear2_Border(PNURO_BMP pBmp
        , const nuLONG x0
        , const nuLONG y0
        , nuroRECT& rect
        //      , const nuLONG w0
        // 		, const nuLONG h0
        , const nuLONG x_16
        , const nuLONG y_16
        , nuWORD* result
        , nuWORD colorDefault
        , nuINT mode);
    static nuVOID nuBMPZoom0(PNURO_BMP pDesBmp, nuINT desx, nuINT desy, nuINT desw, nuINT desh
        , PNURO_BMP pSrcBmp, nuINT srcx, nuINT srcy, nuINT srcw, nuINT srch);
    static nuVOID nuBMPZoom_Bilinear2(PNURO_BMP pDesBmp
        , nuINT desx
        , nuINT desy
        , nuINT desw
        , nuINT desh
        , PNURO_BMP pSrcBmp
        , nuINT srcx
        , nuINT srcy
        , nuINT srcw
        , nuINT srch
        , nuINT mode);
    static nuVOID nuBMPRotary_BilInear_CopyLine(nuWORD* pDstLine,nuLONG dst_border_x0
        , nuLONG dst_in_x0,nuLONG dst_in_x1,nuLONG dst_border_x1
        , const PNURO_BMP pSrcBmp, const nuLONG x0, const nuLONG y0, nuroRECT& rect//, const nuLONG w0, const nuLONG h0
        , nuLONG srcx0_16,nuLONG srcy0_16,nuLONG Ax_16,nuLONG Ay_16, nuINT mode);
    static nuVOID nuBMPRotaryBilInear(const PNURO_BMP pDstBmp,const PNURO_BMP pSrcBmp, nuLONG srcx, nuLONG srcy
        , nuLONG srcw, nuLONG srch, nuFLOAT centerScaleX, nuFLOAT centerScaleY, nuLONG RotaryAngle
        , double ZoomX,double ZoomY,nuLONG move_x,nuLONG move_y, nuINT mode);
};

#endif // !defined(AFX_GDIMETHODZK_H__58B38FEC_D5C0_43A4_AAA2_7865C37581D4__INCLUDED_)
