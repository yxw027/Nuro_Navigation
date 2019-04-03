// GdiFontZK.h: interface for the CGdiFontZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIFONTZK_H__F7FF924E_7365_4ABA_AAF3_BD114B276266__INCLUDED_)
#define AFX_GDIFONTZK_H__F7FF924E_7365_4ABA_AAF3_BD114B276266__INCLUDED_

#include "NuroDefine.h"
#include "GdiMethodZK.h"
#include "nurocolor.h"
#include "nurofont.h"

const nuSIZE C_FONTBUFFER_DATALIMIT = 3600;
const nuSIZE C_FONTFILENAME_MAX = 128;

typedef struct tagNUROFONTBLOCKINFO
{
    nuBYTE nType;
    nuBYTE nX;
    nuBYTE nY;
    nuBYTE nReserve1;
    nuWORD nStartUni;
    nuWORD nUniCount;
    nuUINT nStartPos;
    nuUINT nReserve2;
}NUROFONTBLOCKINFO;

typedef struct tagNUROFONTFILEINFO
{
    nuBYTE nuro[4];
    nuUINT version;
    nuUINT nFileLen;
    nuBYTE nBlockNum;
    nuBYTE nReserve[3];
}NUROFONTFILEINFO;

typedef struct tagNUROFONTFILEHEAD
{
    NUROFONTFILEINFO fileInfo;
    NUROFONTBLOCKINFO fontBlockInfo[256];//最多255区 最后1区为终止区
}NUROFONTFILEHEAD;// sizeof(NUROFONTFILEINFO)+sizeof(NUROFONTINFO)×256 

struct FontBufferData
{
    nuUINT unicode;
    nuUINT type;
    nuBYTE buffer[C_FONTBUFFER_DATALIMIT];
    NUROFONTBLOCKINFO* info;
    FontBufferData* pre;
    FontBufferData* next;
};

class CGdiFontZK  
{
public:
    CGdiFontZK();
    virtual ~CGdiFontZK();
    nuBOOL Init();
    nuVOID Destroy();
    nuBOOL LoadFontInfo(const nuTCHAR *fontfile);
    nuVOID UnloadFontInfo();
    nuBOOL DrawTextOut(PNURO_BMP pBmp, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount, nuUINT nUniFontType,
                       nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor);
    nuINT  DrawTextOut2(PNURO_BMP pBmp, PNURORECT pRect, nuUINT nFormat, nuWCHAR* wStr, nuINT nCount, nuUINT nUniFontType, 
                        nuCOLORREF color, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor, nuINT bkMode, nuCOLORREF bkColor);
    nuBOOL DrawTextOutRotary(PNURO_BMP pBmp, nuINT X, nuINT Y, nuFLOAT startscalex, nuFLOAT startscaley, 
		                     long RotaryAngle, nuWCHAR *wStr, nuUINT nCount, nuUINT nUniFontType,
                             nuCOLORREF fontcolor, nuBOOL bUseEdgeColor, nuCOLORREF edgeColor);
protected:
    nuBOOL nuTextOutBitmap8D2(nuWCHAR* str , nuINT count, nuUINT nUniFontType, NrBitmap8D2& dstbmp, nuINT& width, nuINT& height);
    nuUINT nuFontAutoType(nuUINT fontH);
    nuBOOL nuFontRectCountEx(nuWCHAR* wStr, nuINT nCount, nuUINT nUniFontSize, nuINT* pW, nuINT* pH, nuBOOL fix=true);
    nuBOOL LoadWordInfo(nuroU32 nUniCode, nuroU32 nUniFontType, nuroFontInfo& info);
    nuUINT DrawUniFont(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode, nuINT FontR, nuINT FontG, nuINT FontB
        , nuUINT nUniFontType, nuBOOL bUseEdgeColor, nuINT EdgeR, nuINT EdgeG, nuINT EdgeB);
    nuUINT DrawUniFont2(PNURO_BMP pBmp, NURORECT* pRect, nuUINT nUniCode, nuINT FontR, nuINT FontG, nuINT FontB
        , nuUINT nUniFontType, nuBOOL bUseEdgeColor, nuINT EdgeR, nuINT EdgeG, nuINT EdgeB);
    FontBufferData* fontbuffer;
    FontBufferData* usedHead;
    FontBufferData* usedEnd;
    FontBufferData* freeHead;
    nuBOOL FontBuffer_Init();
    nuVOID FontBuffer_Destroy();
    nuBOOL FontBuffer_GetBuffer(nuUINT word, nuUINT type, FontBufferData** fbd);
    nuVOID FontBuffer_Active(FontBufferData* fbd);
    nuBOOL FontBuffer_LoadFontInfo(nuUINT word, nuUINT nUniFontType, nuFILE* fpFontData, 
		                           const nuBYTE* pFontInfo, NUROFONTBLOCKINFO** info, nuBYTE** buffer);
    nuFILE *_fpFont;
    nuBYTE *_pFontInfo;
    nuBYTE _FTList[256];
    nuTCHAR _filename[C_FONTFILENAME_MAX];    
};


#endif // !defined(AFX_GDIFONTZK_H__F7FF924E_7365_4ABA_AAF3_BD114B276266__INCLUDED_)
