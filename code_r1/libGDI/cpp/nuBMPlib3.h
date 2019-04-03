#ifndef _NUBMPLIB3_H_FAJLN2M09FDAJLJNZFA
#define _NUBMPLIB3_H_FAJLN2M09FDAJLJNZFA

#include "NuroDefine.h"

struct FILLGON_LINE_CONTENT
{
    nuINT x0;
    nuINT y0;
    nuINT x1;
    nuINT y1;
    nuINT dxdy;
    nuINT dydx;
	nuINT hlinetag;
    nuINT crosstag;
    nuINT crosstag_x;
};
namespace nuro
{
	namespace math
	{
#define _____MAGIC_NUMBER 0x5f375a86 //0x5f3759df
		inline nuFLOAT QuickRSqrtF(nuFLOAT x)
		{
			nuFLOAT xhalf = 0.5f*x;
			nuINT i = *(nuINT*)&x; // get bits for floating value
			i = _____MAGIC_NUMBER- (i>>1); // gives initial guess y0
			x = *(nuFLOAT*)&i; // convert bits back to nuFLOAT
			x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
			//x = x*(1.5f-xhalf*x*x); 
			return x;
		}
		inline nuFLOAT QuickSqrtF(nuFLOAT number) 
		{
			nuINT i;
			nuFLOAT x, y;
			x = number * 0.5F;
			y  = number;
			i  = * ( nuINT * ) &y;
			i  = _____MAGIC_NUMBER - ( i >> 1 );
			y  = * ( nuFLOAT * ) &i;
			y  = y * ( 1.5F - ( x * y * y ) );
			//y  = y * ( 1.5F - ( x * y * y ) );
			return number * y;
		}
	}
}
unsigned short nuQuickSqrt(nuULONG a);
nuVOID nuBMPSetPixel565_Alpha(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD color565, nuINT alpha, nuINT nMode);
nuVOID nuBMPDrawLineHor565(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuWORD color565, nuINT nMode);
nuVOID nuBMPDrawLineVer565(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuWORD color565, nuINT nMode);

nuVOID nuBMPSetPixel565Shift(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD color565, nuINT floatvalue, nuINT nMode);
nuVOID nuBMPDrawLineHorShift(PNURO_BMP pBmp, nuINT fix1, nuINT fix2, nuINT y, nuWORD color565, nuINT nMode);
nuVOID nuBMPDrawLineVerShift(PNURO_BMP pBmp, nuINT x, nuINT fix1, nuINT fix2, nuWORD color565, nuINT nMode);
nuVOID nuBMPDrawLineHorShift_W(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuWORD color565, nuINT w, nuINT nMode);
nuVOID nuBMPDrawLineVerShift_W(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuWORD color565, nuINT w, nuINT nMode);

nuVOID nuBMPSetLineContent(FILLGON_LINE_CONTENT& lc, const NUROPOINT& pt0, const NUROPOINT& pt1, nuINT* miny, nuINT* maxy);
nuVOID nuBMPSetLineContentShift(FILLGON_LINE_CONTENT& lc, const NUROPOINT& pt0, const NUROPOINT& pt1, nuINT* miny, nuINT* maxy);
nuVOID nuBMPFillGonShift(PNURO_BMP pBmp, FILLGON_LINE_CONTENT* lines, nuINT* pPolyInts, nuINT miny, 
						 nuINT maxy, nuINT total, nuWORD color565, nuINT nMode);

nuVOID nuBMPDrawLine_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT dw, nuWORD color565, nuINT nMode);
nuVOID nuBMPFillCircle_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuUINT D, nuWORD color565, nuINT nMode);
nuVOID nuBMPPolyLine_New(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuINT thick, nuWORD color565, nuINT nMode);

nuVOID nuBMPFillGon_New(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuWORD color,nuINT nMode);
nuVOID nuBMPDraw3DLine_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuWORD color565,nuINT dw, nuINT dw2,nuINT nMode);

#endif //_NUBMPLIB3_H_FAJLN2M09FDAJLJNZFA


