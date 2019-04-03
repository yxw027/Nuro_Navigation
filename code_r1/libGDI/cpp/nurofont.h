#ifndef _NURO_FONT_H_FKLASDFNNM09ALPVZ624JH
#define _NURO_FONT_H_FKLASDFNNM09ALPVZ624JH

#include "NuroDefine.h"
#include "nurotypes.h"

#define NURO_FONT_ENGINE_USE_FREETYPE

const nuSIZE C_FONTBUFFER_SIZE = 60;

struct nuroFontInfo
{
    nuroU16 height;
    nuroS16 stepx;
    nuroS16 stepy;
    nuroS16 startx;
    nuroS16 starty;
    nuroU16 bufferwidth;
    nuroU16 bufferheight;
    nuroU16 bufferbytesperline;
    nuroU8* buffer;
};

#endif
