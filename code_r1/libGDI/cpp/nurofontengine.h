// nurofontengine.h: interface for the nuroFontEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROFONTENGINE_H__EB69AAFC_5A2F_4822_9A0C_86A9225E38FC__INCLUDED_)
#define AFX_NUROFONTENGINE_H__EB69AAFC_5A2F_4822_9A0C_86A9225E38FC__INCLUDED_
#include "NuroDefine.h"
#include "nurotypes.h"
// #include "nuBMPlib.h"
// #include "nuBMPlib2.h"
#include "nurocolor.h"
#include "nurofont.h"
#include "nuroftface.h"
#include "GdiMethodZK.h"

struct FontBufferData_FT_BufferInfo
{   
    nuroS16 startx;
    nuroS16 starty;
    nuroU16 bufferwidth;
    nuroU16 bufferheight;
    nuroU16 bufferbytesperline;
    nuBYTE* buffer;
};
const nuSIZE FontBufferData_FT_BUFFERSIZE = 10000;
const nuSIZE FontBufferData_FT_SIZELIMIT  = 85;
struct FontBufferData_FT
{
    nuUINT unicode;
    nuUINT type;
    nuINT  angle;
    nuUINT render;    
    FT_Glyph glyph;
    nuroS16 stepx;
    nuroS16 stepy;
    FT_BBox bbox;
    nuUINT edgestart;
    FontBufferData_FT_BufferInfo wordinfo;
    FontBufferData_FT_BufferInfo edgeinfo;
    nuBYTE buffer[FontBufferData_FT_BUFFERSIZE];
    FontBufferData_FT* pre;
    FontBufferData_FT* next;
};

// class nuroHashMethod
// {
// public:
//     enum
//     {
//         _HASH1_0 = 1315423911
//     };
//     static nuroU32 Hash1(nuroU32 hash, nuroU16 *str, nuroU32 len) 
//     { 
//         while (len)
//         {
//             --len;
//             hash ^= ((hash << 5) + str[len] + (hash >> 2));
//         }
//         return hash;
//     }
// };

class nuroFTRenderBuffer
{
public:
    static nuVOID RasterCallback(nuINT y, nuINT count, const FT_Span * spans, nuVOID * user) 
    {
        nuroFTRenderBuffer* r1 = (nuroFTRenderBuffer*)user;
        y = r1->y0 - y;
        nuINT x = 0;
        nuBYTE* p = r1->buffer + r1->w*y;
        for (nuINT i=0; i<count; ++i)
        {
            x = r1->x0 + spans[i].x;
            nuMemset(p+x, spans[i].coverage, spans[i].len);
        }
    }
public:
    nuroFTRenderBuffer() {}
    nuINT x0, y0;
    nuINT w, h;
    nuBYTE* buffer;
};
class nuroFontEngine  
{
public:
    nuroFontEngine();
    virtual ~nuroFontEngine();
    
    nuBOOL Init();
    nuVOID Destroy();
    nuBOOL LoadFontInfo(const nuTCHAR* name);
    nuBOOL SetSize(nuroU32 w, nuroU32 h, nuINT angle);
    nuBOOL nuFontRectCountEx(nuWCHAR* wStr, nuINT nCount, nuUINT nUniFontType, nuINT* pW, nuINT* pH, nuBOOL fix=nuTRUE);
    nuroS16 DrawUniFont(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode,
        nuINT FontR, nuINT FontG, nuINT FontB, nuUINT nUniFontType, nuINT angle);
    nuroS16 DrawUniFontEdge(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode,
        nuUINT nUniFontType, nuINT angle, nuINT EdgeR, nuINT EdgeG, nuINT EdgeB);
    nuBOOL DrawTextOut(PNURO_BMP pBmp, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount, nuUINT nUniFontType,
        nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor, nuINT angle=0);
    nuINT	DrawTextOut2(PNURO_BMP pBmp, PNURORECT pRect, nuUINT nFormat, nuWCHAR* wStr,  nuINT nCount, nuUINT nUniFontType,
        nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor);    
	nuINT	DrawTextOutNew(PNURO_BMP pBmp, PNURORECT pRect, nuUINT nFormat, nuWCHAR* wStr,  nuINT nCount, nuUINT nUniFontType,
		nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor);    
protected:
    FT_Library _library;
    FT_Face _face;
    nuroU16 _height;//字号最大高度
    nuroU16 _baseheight;//基线高度
   // nuroU32 _setW;
    nuroU32 _setH;
    nuroS32 _angle;
    
protected:
    FontBufferData_FT* fontbuffer;
    FontBufferData_FT* usedHead;
    FontBufferData_FT* usedEnd;
    FontBufferData_FT* freeHead;
    nuBOOL FontBuffer_Init();
    nuVOID FontBuffer_Destroy();
    nuBOOL FontBuffer_GetBuffer(nuUINT word, nuUINT type, nuINT angle, FontBufferData_FT** fbd);
    nuVOID FontBuffer_Active(FontBufferData_FT* fbd);
    nuBOOL FontBuffer_LoadFontInfo(nuUINT word, nuUINT nUniFontType, nuINT angle, nuroFontInfo* info, nuBOOL render);
    nuBOOL FontBuffer_LoadEdgeInfo(nuUINT word, nuUINT nUniFontType, nuINT angle, nuroFontInfo* info);
    nuroFTRenderBuffer _ftRenderBuffer;
    nuBOOL LoadGlyph(nuUINT word, FontBufferData_FT* fbd);
    nuVOID RenderWord(FT_Library &library, FontBufferData_FT* fbd, nuroU16 baseHeight);
    nuVOID RenderEdge(FT_Library &library, FontBufferData_FT* fbd, nuroU16 baseHeight);    
    FT_Raster_Params _params;
};

#endif // !defined(AFX_NUROFONTENGINE_H__EB69AAFC_5A2F_4822_9A0C_86A9225E38FC__INCLUDED_)
