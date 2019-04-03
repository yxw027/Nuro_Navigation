#ifndef __NURO_INLINE_METHODS_20091004
#define __NURO_INLINE_METHODS_20091004

#include "NuroDefine.h"

inline nuVOID NURO_CANVAS_INIT(PNURO_CANVAS pCvs)	
{
	nuMemset(pCvs, 0, sizeof(NURO_CANVAS));
}

inline nuVOID NURO_BMP_INIT(PNURO_BMP pBmp, nuWORD nW, nuWORD nH)
{
	nuMemset(pBmp, 0, sizeof(NURO_BMP));
	pBmp->bmpWidth	= nW;
	pBmp->bmpHeight	= nH;
}

inline nuBOOL nuRectInRect(const NURORECT& rt1, const NURORECT& rt2)
{
	if( rt1.left	>= rt2.left		&&
		rt1.right	<= rt2.right	&&
		rt1.top		>= rt2.top		&&
		rt1.bottom	<= rt2.bottom	)
	{
		return nuTRUE;
	}
	return nuFALSE;
}

inline nuBOOL nuRectCoverRect(const NURORECT& rt1, const NURORECT& rt2)
{
    return nuFALSE;
}


#endif