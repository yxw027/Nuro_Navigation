
#include "nuBMPlib3.h"
#include "GdiMethodZK.h"

const nuINT __FLOAT_SHIFT = 12L;
const nuINT __FLOAT_SHIFT_STEP = (1<<__FLOAT_SHIFT);
const nuINT __FLOAT_SHIFT_MASK = (__FLOAT_SHIFT_STEP-1);
#define NURO_FILLGON_PTNUM_DEF	20
#define NURO_MAXFIXDIS			500000

nuUSHORT nuQuickSqrt(nuULONG a)
{
    nuULONG rem = 0;
    nuULONG root = 0;
    nuULONG divisor = 0;
    for(nuINT i=0; i<16; ++i)
    {
        root <<= 1;
        rem = ((rem << 2) + (a >> 30));
        a <<= 2;
        divisor = (root<<1) + 1;
        if(divisor <= rem)
        {
            rem -= divisor;
            ++root;
        }
    }
    return (nuUSHORT)(root);
}
nuVOID nuBMPSetPixel565_Alpha(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD color565, nuINT alpha, nuINT nMode)
{
    if( !nMode || !alpha || pBmp==NULL || pBmp->pBmpBuff== NULL || !NURO_PTINBMP(pBmp, x, y) )
    {
        return;
    }
    nuWORD *pColor16 = (nuWORD*)(pBmp->pBmpBuff + pBmp->bytesPerLine*y + (x<<1));
	if (nMode<NURO_BMP_TYPE_MIXX)
	{
		alpha = (alpha * nMode) >> 4;
	}
    if(alpha<255)
    {
        nuINT realR = nu565RVALUE(*pColor16);
        nuINT realG = nu565GVALUE(*pColor16);
        nuINT realB = nu565BVALUE(*pColor16);
        realR += ( ((nuINT)nu565RVALUE(color565) - realR)*alpha ) >> 8;
        realG += ( ((nuINT)nu565GVALUE(color565) - realG)*alpha ) >> 8;
        realB += ( ((nuINT)nu565BVALUE(color565) - realB)*alpha ) >> 8;
        *pColor16 = nuRGB24TO16_565(realR, realG, realB);
        return;
    }
    *pColor16 = color565;
    return;
}
nuVOID nuBMPDrawLineHor565(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuWORD color565, nuINT nMode)
{
    for(nuINT nCount=x1; nCount<x2; ++nCount)
    {
        nuBMPSetPixel565_Alpha(pBmp, nCount, y, color565, 255, nMode);
    }
}
nuVOID nuBMPDrawLineVer565(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuWORD color565, nuINT nMode)
{
    for(nuINT nCount=y1; nCount<y2; ++nCount)
    {
        nuBMPSetPixel565_Alpha(pBmp, x, nCount, color565, 255, nMode);
    }
}
nuVOID nuBMPSetPixel565Shift(PNURO_BMP pBmp, nuINT x, nuINT y, nuWORD color565, nuINT floatvalue, nuINT nMode)
{
    if( !nMode || !floatvalue || pBmp==NULL || pBmp->pBmpBuff== NULL || !NURO_PTINBMP(pBmp, x, y) )
    {
        return;
    }
    nuWORD *pColor16 = (nuWORD*)(pBmp->pBmpBuff + pBmp->bytesPerLine*y + (x<<1));
	if (nMode < NURO_BMP_TYPE_MIXX)
	{
		floatvalue = (floatvalue*nMode)>>4;
	}
    if(floatvalue<__FLOAT_SHIFT_MASK)
    {
        nuINT realR = nu565RVALUE(*pColor16);
        nuINT realG = nu565GVALUE(*pColor16);
        nuINT realB = nu565BVALUE(*pColor16);
        realR += ( ((nuINT)nu565RVALUE(color565) - realR)*floatvalue ) >> __FLOAT_SHIFT;
        realG += ( ((nuINT)nu565GVALUE(color565) - realG)*floatvalue ) >> __FLOAT_SHIFT;
        realB += ( ((nuINT)nu565BVALUE(color565) - realB)*floatvalue ) >> __FLOAT_SHIFT;
        *pColor16 = nuRGB24TO16_565(realR, realG, realB);
        return;
    }
    *pColor16 = color565;
    return;
}
nuVOID nuBMPDrawLineHorShift(PNURO_BMP pBmp, nuINT fix1, nuINT fix2, nuINT y, nuWORD color565, nuINT nMode)
{
    nuINT d2 = fix1 & __FLOAT_SHIFT_MASK;
    fix1 = fix1 >> __FLOAT_SHIFT;
    nuBMPSetPixel565Shift(pBmp, fix1, y, color565, __FLOAT_SHIFT_MASK-d2, nMode);
    d2 = fix2 & __FLOAT_SHIFT_MASK;
    fix2 = fix2 >> __FLOAT_SHIFT;
    nuBMPSetPixel565Shift(pBmp, fix2+1, y, color565, d2, nMode);
    if (fix1 < fix2)
    {
        ++fix1;
        for(nuINT nCount=fix1; nCount<=fix2; ++nCount)
        {
            nuBMPSetPixel565Shift(pBmp, nCount, y, color565, __FLOAT_SHIFT_MASK, nMode);
        }
    }
}
nuVOID nuBMPDrawLineVerShift(PNURO_BMP pBmp, nuINT x, nuINT fix1, nuINT fix2, nuWORD color565, nuINT nMode)
{
    nuINT d2 = fix1 & __FLOAT_SHIFT_MASK;
    fix1 = fix1 >> __FLOAT_SHIFT;
    nuBMPSetPixel565Shift(pBmp, x, fix1, color565, __FLOAT_SHIFT_MASK-d2, nMode);
    d2 = fix2 & __FLOAT_SHIFT_MASK;
    fix2 = fix2 >> __FLOAT_SHIFT;
    nuBMPSetPixel565Shift(pBmp, x, fix2+1, color565, d2, nMode);
    if (fix1 < fix2)
    {
        ++fix1;
        for(nuINT nCount=fix1; nCount<=fix2; ++nCount)
        {
            nuBMPSetPixel565Shift(pBmp, x, nCount, color565, __FLOAT_SHIFT_MASK, nMode);
        }
    }
}

nuVOID nuBMPDrawLineHorShift_W(PNURO_BMP pBmp, nuINT x1, nuINT x2, nuINT y, nuWORD color565, nuINT w, nuINT nMode)
{
    if (w & 0x01)
    {
        nuINT y1 = y - (w>>1);
        nuINT y2 = y + (w>>1)+1;
        nuINT xstart = NURO_MIN(x1, x2);
        nuINT xend = NURO_MAX(x1, x2);
        for (nuINT x=xstart; x<=xend; ++x)
        {
            nuBMPDrawLineVer565(pBmp, x, y1, y2, color565, nMode);
        }
    }
    else
    {
        nuINT fixy = y << __FLOAT_SHIFT;
        nuINT fixdw = (w-1) << (__FLOAT_SHIFT-1);
        nuINT fix1 = fixy - fixdw;
        nuINT fix2 = fixy + fixdw;
        nuINT xstart = NURO_MIN(x1, x2);
        nuINT xend = NURO_MAX(x1, x2);
        for (nuINT x=xstart; x<=xend; ++x)
        {
            nuBMPDrawLineVerShift(pBmp, x, fix1, fix2, color565, nMode);
        }
    }    
}
nuVOID nuBMPDrawLineVerShift_W(PNURO_BMP pBmp, nuINT x, nuINT y1, nuINT y2, nuWORD color565, nuINT w, nuINT nMode)
{
    if (w & 0x01)
    {
        nuINT x1 = x - (w>>1);
        nuINT x2 = x + (w>>1)+1;
        nuINT ystart = NURO_MIN(y1, y2);
        nuINT yend = NURO_MAX(y1, y2);
        for (nuINT y=ystart; y<=yend; ++y)
        {
            nuBMPDrawLineHor565(pBmp, x1, x2, y, color565, nMode);
        }
    }
    else
    {
        nuINT fixx = x << __FLOAT_SHIFT;
        nuINT fixdw = (w-1) << (__FLOAT_SHIFT-1);
        nuINT fix1 = fixx - fixdw;
        nuINT fix2 = fixx + fixdw;
        nuINT ystart = NURO_MIN(y1, y2);
        nuINT yend = NURO_MAX(y1, y2);
        for (nuINT y=ystart; y<=yend; ++y)
        {
            nuBMPDrawLineHorShift(pBmp, fix1, fix2, y, color565, nMode);
        }
    }
}
nuVOID nuBMPSetLineContent(FILLGON_LINE_CONTENT& lc, const NUROPOINT& pt0, const NUROPOINT& pt1
                         , nuINT* miny, nuINT* maxy)
{

    if (pt1.y<pt0.y)
    {
        lc.x0 = (pt1.x<<__FLOAT_SHIFT);
        lc.y0 = (pt1.y<<__FLOAT_SHIFT);
        lc.x1 = (pt0.x<<__FLOAT_SHIFT);
        lc.y1 = (pt0.y<<__FLOAT_SHIFT);
    }
    else
    {
        lc.x0 = (pt0.x<<__FLOAT_SHIFT);
        lc.y0 = (pt0.y<<__FLOAT_SHIFT);
        lc.x1 = (pt1.x<<__FLOAT_SHIFT);
        lc.y1 = (pt1.y<<__FLOAT_SHIFT);
    }
    if (miny && *miny > lc.y0)
    {
        *miny = lc.y0;
    }
    if (maxy && *maxy < lc.y1)
    {
        *maxy = lc.y1;
    }
    nuINT dx = lc.x1 - lc.x0;
    nuINT dy = lc.y1 - lc.y0;
	lc.hlinetag = 0;
    if ( 0 == dy)
    {
		lc.hlinetag = 1;
    }
    else
    {
        lc.dxdy = dx/(dy/__FLOAT_SHIFT_STEP);
        if (lc.dxdy > __FLOAT_SHIFT_STEP
            || lc.dxdy < -__FLOAT_SHIFT_STEP)
        {
			lc.dydx = dy/(dx/__FLOAT_SHIFT_STEP);
        }
    }
    lc.crosstag = 0;
}
nuVOID nuBMPSetLineContentShift(FILLGON_LINE_CONTENT& lc, const NUROPOINT& pt0, const NUROPOINT& pt1, 
								nuINT* miny, nuINT* maxy)
{
    if (pt1.y<pt0.y)
    {
        lc.x0 = (pt1.x);
        lc.y0 = (pt1.y);
        lc.x1 = (pt0.x);
        lc.y1 = (pt0.y);
    }
    else
    {
        lc.x0 = (pt0.x);
        lc.y0 = (pt0.y);
        lc.x1 = (pt1.x);
        lc.y1 = (pt1.y);
    }
    if (miny && *miny > lc.y0)
    {
        *miny = lc.y0;
    }
    if (maxy && *maxy < lc.y1)
    {
        *maxy = lc.y1;
    }
    nuINT dx = lc.x1 - lc.x0;
    nuINT dy = lc.y1 - lc.y0;
	lc.hlinetag = 0;
    if ( 0 == dy)
    {
		lc.hlinetag = 1;
    }
    else
    {
		lc.dxdy = nuINT ( ((float)dx)/dy*__FLOAT_SHIFT_STEP );
        if (lc.dxdy > __FLOAT_SHIFT_STEP
            || lc.dxdy < -__FLOAT_SHIFT_STEP)
        {
			lc.dydx = nuINT ( ((float)dy)/dx*__FLOAT_SHIFT_STEP );
        }
    }
    lc.crosstag = 0;
}
nuVOID nuBMPFillGonShift(PNURO_BMP pBmp, FILLGON_LINE_CONTENT* lines, nuINT* pPolyInts, nuINT miny, nuINT maxy
					   , nuINT total, nuWORD color565, nuINT nMode)
{
    nuINT tagline;
    nuINT tagy;
	nuINT ints;	
	nuINT ints_minus1;
	nuINT nCount1;
	nuINT nCount2;
    nuINT tmpPolyInt;	
    nuINT xend = 0;
    nuINT x, y, d2;
    miny = (miny>>__FLOAT_SHIFT)<<__FLOAT_SHIFT;
    maxy = ( (maxy>>__FLOAT_SHIFT)<<__FLOAT_SHIFT ) + __FLOAT_SHIFT_STEP;

	for (tagy = miny; (tagy <= maxy); tagy+=__FLOAT_SHIFT_STEP)
    {
       ints = 0;
        for (tagline = 0; tagline < total; ++tagline)
        {
            switch (lines[tagline].crosstag)
            {
            case 0:
				if (tagy >= lines[tagline].y0)
                {
					if (lines[tagline].hlinetag)
                    {
						if (lines[tagline].x0<lines[tagline].x1)
                        {
                            nuBMPDrawLineHorShift(pBmp, lines[tagline].x0, lines[tagline].x1, tagy>>__FLOAT_SHIFT, color565, nMode);
                        }
                        else
                        {
                            nuBMPDrawLineHorShift(pBmp, lines[tagline].x1, lines[tagline].x0, tagy>>__FLOAT_SHIFT, color565, nMode);
                        }
                        lines[tagline].crosstag = 2;
                    }
                    else		//into
                    {
							lines[tagline].crosstag = 1;
							lines[tagline].crosstag_x = lines[tagline].x0
								+ (( (tagy-lines[tagline].y0)*lines[tagline].dxdy ) >>__FLOAT_SHIFT);
							pPolyInts[ints] = lines[tagline].crosstag_x;
							nuBMPSetPixel565Shift(pBmp, pPolyInts[ints]>>__FLOAT_SHIFT, tagy>>__FLOAT_SHIFT
								, color565, __FLOAT_SHIFT_MASK, nMode);
							++ints;
                    }
                }
				break;
            case 1:
                if (tagy >= lines[tagline].y1)//out
                {
                    lines[tagline].crosstag = 2;
                }
                else		//in
                {
                    lines[tagline].crosstag_x += lines[tagline].dxdy;
                    pPolyInts[ints] = lines[tagline].crosstag_x;
                    ++ints;
                    
                }
                break;
            case 2:
            default:
                break;
            }
        }
        if (ints<2)
        {
            continue;
        }
        ints_minus1 = ints - 1;
        nCount1 = 0;
        nCount2 = 0;
        tmpPolyInt = 0;
		for (nCount1 = 0; nCount1 < ints_minus1; ++nCount1)
        {
            for (nCount2 = nCount1 + 1; nCount2 < ints; ++nCount2)
            {
                if (pPolyInts[nCount2] > pPolyInts[nCount1])
                {
                    tmpPolyInt = pPolyInts[nCount1];
                    pPolyInts[nCount1] = pPolyInts[nCount2];
                    pPolyInts[nCount2] = tmpPolyInt;
                }
            }
        }
        for (tmpPolyInt = 1; tmpPolyInt < ints; tmpPolyInt += 2)
        {
            nuBMPDrawLineHorShift(pBmp, pPolyInts[tmpPolyInt], pPolyInts[tmpPolyInt - 1], tagy>>__FLOAT_SHIFT, color565, nMode);
        }
    }

    for (tagline=0; tagline<total; ++tagline)
    {
		if (lines[tagline].hlinetag)
		{
			continue;
		}
        if ( lines[tagline].dxdy >__FLOAT_SHIFT_STEP)
        {
            xend = (lines[tagline].x1>>__FLOAT_SHIFT);
            x = (lines[tagline].x0>>__FLOAT_SHIFT)+1;
            y = lines[tagline].y0
                + ( ( ( (x<<__FLOAT_SHIFT)-lines[tagline].x0)*lines[tagline].dydx)>>__FLOAT_SHIFT);
            for (; x<xend; ++x)
            {
                if (y > maxy)
                {
                    break;
                }
                if (y >= miny)
                {
					d2 = y & __FLOAT_SHIFT_MASK;
                    nuBMPSetPixel565Shift(pBmp, x, y>>__FLOAT_SHIFT, color565, __FLOAT_SHIFT_MASK-d2, nMode);
                    nuBMPSetPixel565Shift(pBmp, x, (y>>__FLOAT_SHIFT)+1, color565, d2, nMode);
                }
                y += lines[tagline].dydx;
            }
        }
        else if ( lines[tagline].dxdy < -__FLOAT_SHIFT_STEP)
        {	
			xend = (lines[tagline].x1>>__FLOAT_SHIFT)+1;
            x = (lines[tagline].x0>>__FLOAT_SHIFT);
            y = lines[tagline].y0
                + ((((x<<__FLOAT_SHIFT)-lines[tagline].x0)*lines[tagline].dydx)>>__FLOAT_SHIFT);
            for (; x>xend; --x)
            {
                if (y > maxy)
                {
                    break;
                }
                if (y >= miny)
                {
					d2 = y & __FLOAT_SHIFT_MASK;
                    nuBMPSetPixel565Shift(pBmp, x, y>>__FLOAT_SHIFT, color565, __FLOAT_SHIFT_MASK-d2, nMode);
                    nuBMPSetPixel565Shift(pBmp, x, (y>>__FLOAT_SHIFT)+1, color565, d2, nMode);
                }
                y -= lines[tagline].dydx;
            }
            
        }
    }
}
nuVOID nuBMPDrawLine_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT dw, nuWORD color565, nuINT nMode)
{
    if( !CGdiMethodZK::nuLineLimitToRectZ( x1, x2, y1, y2, -40, pBmp->bmpWidth -1 + 40, -40, pBmp->bmpHeight - 1 + 40) )
		//if( !nuLineLimitToRectZ( x1, x2, y1, y2, -dw, pBmp->bmpWidth - 1 + dw, -dw, pBmp->bmpHeight - 1 + dw ) )
    {
        return ;
    }
    nuINT deltax,   deltay,   start,   finish;
    deltax   =   NURO_ABS(x2   -   x1);
    deltay   =   NURO_ABS(y2   -   y1);
    if (!deltax)
    {
        nuBMPDrawLineVerShift_W(pBmp, x1, y1, y2, color565, dw, nMode);
        return;
    }
    if (!deltay)
    {
        nuBMPDrawLineHorShift_W(pBmp, x1, x2, y1, color565, dw, nMode);
        return;
    }
    nuINT dydx = ( (deltay<<__FLOAT_SHIFT) / deltax);
    if (!dydx)
    {
        nuBMPDrawLineHorShift_W(pBmp, x1, x2, (y1+y2)>>1, color565, dw, nMode);
        return;
    }
    nuINT dxdy = ( (deltax<<__FLOAT_SHIFT) / deltay);
    if (!dxdy)
    {
        nuBMPDrawLineVerShift_W(pBmp, (x1+x2)>>1, y1, y2, color565, dw, nMode);
        return;
    }
    if( deltax >= deltay)
    {
        dw *= nuQuickSqrt((1<<(__FLOAT_SHIFT<<1))+dydx*dydx);
        nuINT dw0 = dw;//-(1<<__FLOAT_SHIFT);
        nuINT bdw = (dw0>>1);
        nuINT arrowparam = 1;
        nuINT dy_start = 0;
        nuINT dy_end = 0;
        if(y2 > y1)
        {
            dydx   =   -dydx;
            arrowparam = -arrowparam;
        }
        if(x2   <   x1)
        {
            start   =   x2;
            finish   =   x1;
            dy_start   =   y2<<__FLOAT_SHIFT;
            dy_end = y1 << __FLOAT_SHIFT;
        }
        else
        {
            start   =   x1;
            finish   =   x2;
            dy_start   =   y1 << __FLOAT_SHIFT;
            dy_end = y2 << __FLOAT_SHIFT;
            dydx   =   -dydx;
            arrowparam = -arrowparam;
        }

        nuINT ex = (bdw>>__FLOAT_SHIFT);
        nuINT up = 0;
        nuINT down = 0;
        nuINT dy =dy_start - dydx*ex;
        nuINT yy0 = 0;
        if (arrowparam>0)
        {
            yy0 = dy_start + dxdy*ex;
        }
        else
        {
            yy0 = dy_start - dxdy*ex;
        }
        nuINT yy1 = 0;
        nuINT yy1_set = 0;
        for (nuINT x  = -ex; x<=(deltax+ex); ++x, dy += dydx)
        {
            up = dy-bdw;
            down = dy+bdw;
            if (arrowparam > 0)
            {
                if (x <= ex)
                {
                    if ( yy0 > down)
                    {
                        yy0 -= dxdy;
                        continue;
                    }
                    if (  up < yy0)
                    {
                        up = yy0;
                    }
                    yy0 -= dxdy;
                }
                if (x >= (deltax-ex))
                {
                    if (!yy1_set)
                    {
                        yy1 = dy_end - dxdy*(x-deltax);
                        yy1_set =1;
                    }
                    else
                    {
                        yy1 -= dxdy;
                    }
                    if ( yy1 < up)
                    {
                        continue;
                    }
                    if ( down > yy1)
                    {
                        down = yy1;
                    }
                }
            }
            else
            {
                if (x <= ex)
                {
                    if ( yy0 < up)
                    {
                        yy0 += dxdy;
                        continue;
                    }
                    if (  down > yy0)
                    {
                        down = yy0;
                    }
                    yy0 += dxdy;
                }
                if (x >= (deltax-ex))
                {
                    if (!yy1_set)
                    {
                        yy1 = dy_end + dxdy*(x-deltax);
                        yy1_set =1;
                    }
                    else
                    {
                        yy1 += dxdy;
                    }
                    if ( yy1 > down)
                    {
                        continue;
                    }
                    if ( up < yy1)
                    {
                        up = yy1;
                    }
                }
            }
            nuBMPDrawLineVerShift(pBmp
                , start+x
                , up
                , down
                , color565
                , nMode);
        }
    }
    else
    {
        dw *= nuQuickSqrt((1<<(__FLOAT_SHIFT<<1))+dxdy*dxdy);
        nuINT dw0 = dw;//-(1<<__FLOAT_SHIFT);
        nuINT bdw = (dw0>>1);
        nuINT arrowparam = 1;
        nuINT dx_start = 0;
        nuINT dx_end = 0;
        if(x2   >   x1)
        {
            dxdy   =   -dxdy;
            arrowparam = -arrowparam;
        }
        if(y2   <   y1)
        {
            start   =   y2;
            finish   =   y1;
            dx_start   =   x2<<__FLOAT_SHIFT;
            dx_end = x1 << __FLOAT_SHIFT;
        }
        else
        {
            start   =   y1;
            finish   =   y2;
            dx_start   =   x1<<__FLOAT_SHIFT;
            dx_end = x2 <<__FLOAT_SHIFT;
            dxdy   =   -dxdy;
            arrowparam = -arrowparam;
        }
        nuINT ey = (bdw>>__FLOAT_SHIFT);
        nuINT left = 0;
        nuINT right = 0;
        nuINT dx =dx_start - dxdy*ey;
        nuINT xx0 = 0;
        if (arrowparam>0)
        {
            xx0 = dx_start + dydx*ey;
        }
        else
        {
            xx0 = dx_start - dydx*ey;
        }
        nuINT xx1 = 0;
        nuINT xx1_set = 0;
        for (nuINT y  = -ey; y<=(deltay+ey); ++y, dx += dxdy)
        {
            left = dx-bdw;
            right = dx+bdw;
            if (arrowparam > 0)
            {
                if (y <= ey)
                {
                    if ( xx0 > right)
                    {
                        xx0 -= dydx;
                        continue;
                    }
                    if (  left < xx0)
                    {
                        left = xx0;
                    }
                    xx0 -= dydx;
                }
                if (y >= (deltay-ey))
                {
                    if (!xx1_set)
                    {
                        xx1 = dx_end - dydx*(y-deltay);
                        xx1_set =1;
                    }
                    else
                    {
                        xx1 -= dydx;
                    }
                    if ( xx1 < left)
                    {
                        continue;
                    }
                    if ( right > xx1)
                    {
                        right = xx1;
                    }
                }
            }
            else
            {
                if (y <= ey)
                {
                    if ( xx0 < left)
                    {
                        xx0 += dydx;
                        continue;
                    }
                    if (  right > xx0)
                    {
                        right = xx0;
                    }
                    xx0 += dydx;
                }

                if (y >= (deltay-ey))
                {
                    if (!xx1_set)
                    {
                        xx1 = dx_end + dydx*(y-deltay);
                        xx1_set =1;
                    }
                    else
                    {
                        xx1 += dydx;
                    }
                    if ( xx1 > right)
                    {
                        continue;
                    }
                    if ( left < xx1)
                    {
                        left = xx1;
                    }
                }
            }
            nuBMPDrawLineHorShift(pBmp
                , left
                , right
                , start+y
                , color565
                , nMode);
        }
    }
}
#include "SolidCircleData.h"
nuVOID nuBMPFillCircle_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuUINT D, nuWORD color565, nuINT nMode)
{
    if (D>=2 && D<=SolidCircleData_StartPosAndLineCount[0])
    {
        nuINT a = (D)<<1;
        nuUINT start = SolidCircleData_StartPosAndLineCount[a];
        nuUINT lines = SolidCircleData_StartPosAndLineCount[a+1];
		
		
        const nuUINT8* pSolidCircleData = &SolidCircleData[start];
		//draw center line
        nuINT xleft = x1-*pSolidCircleData+1;
        nuINT xright = x1+*pSolidCircleData-1;
        if (*pSolidCircleData)
        {
            nuBMPDrawLineHor565(pBmp, xleft, xright+1, y1, color565, nMode);
        }
        ++pSolidCircleData;
        while ( *pSolidCircleData )
        {
            --xleft;
            ++xright;
            nuBMPSetPixel565_Alpha(pBmp, xleft, y1, color565, (nuINT)*pSolidCircleData, nMode);
            nuBMPSetPixel565_Alpha(pBmp, xright, y1, color565, (nuINT)*pSolidCircleData, nMode);
            ++pSolidCircleData;
        }
        ;
		//draw other lines
        nuINT ytop = y1;
        nuINT ybottom = y1;
        while (--lines)
        {
            --ytop;
            ++ybottom;
            ++pSolidCircleData;
            xleft = x1-*pSolidCircleData+1;
            xright = x1+*pSolidCircleData-1;
            if (*pSolidCircleData)
            {
                nuBMPDrawLineHor565(pBmp, xleft, xright+1, ytop, color565, nMode);
                nuBMPDrawLineHor565(pBmp, xleft, xright+1, ybottom, color565, nMode);
            }
            ++pSolidCircleData;
            while ( *pSolidCircleData )
            {
                --xleft;
                ++xright;
                nuBMPSetPixel565_Alpha(pBmp, xleft, ytop, color565, (nuINT)*pSolidCircleData, nMode);
                nuBMPSetPixel565_Alpha(pBmp, xright, ytop, color565, (nuINT)*pSolidCircleData, nMode);
                nuBMPSetPixel565_Alpha(pBmp, xleft, ybottom, color565, (nuINT)*pSolidCircleData, nMode);
                nuBMPSetPixel565_Alpha(pBmp, xright, ybottom, color565, (nuINT)*pSolidCircleData, nMode);
                ++pSolidCircleData;
            }
        }
    }
    else
    {
        CGdiMethodZK::nuBMP565FillCircleZ(pBmp, x1, y1, D, color565, nMode);
    }
}
nuVOID nuBMPPolyLine_New(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuINT thick, nuWORD color565, nuINT nMode)
{
    if (pBmp && pPoint && n)
    {
		
        for (nuINT i = 0; i < (n-1); i++)
        {
            if (i==0)
            {
                if(thick>2)
                {
                    nuBMPFillCircle_New(pBmp, pPoint[i].x, pPoint[i].y, thick, color565, nMode);
                }
            }
            nuBMPDrawLine_New(pBmp, pPoint[i].x, pPoint[i].y, pPoint[i+1].x, pPoint[i+1].y, thick, color565, nMode);
            if (thick>2)
            {
                nuBMPFillCircle_New(pBmp, pPoint[i+1].x, pPoint[i+1].y, thick, color565, nMode);
            }
        }
    }
}
class nuroSectionAlloc
{
public:	
	struct nuroMemBlock 
	{
		nuUINT	        total;
		nuUINT8*	freepos;
		nuUINT	        freesize;
		nuroMemBlock*	next;
	};
#define NSA_DEF_BLOCK_SIZE  8192
#define NSA_DEF_BLOCK_FREE_SIZE  (8192 - sizeof(nuroMemBlock))
	nuroSectionAlloc()
	{
		blocks = 0;
	}
	~nuroSectionAlloc()
	{
		nuroMemBlock* block = 0;
		while (blocks)
		{
			block = blocks->next;
			nuFree(blocks);
			blocks = block;
		}
	}
	nuroMemBlock* AddBlock(nuSIZE size)
	{
		nuroMemBlock* block = 0;
		if (size > NSA_DEF_BLOCK_FREE_SIZE)
		{
			size = (size+sizeof(nuroMemBlock)+8191) & (~8191);
		}
		else
		{
			size = NSA_DEF_BLOCK_SIZE;
		}
		block = (nuroMemBlock*)nuMalloc(size);
		if (block)
		{
			block->total = size;
			block->freesize = size - sizeof(nuroMemBlock);
			block->freepos = (nuUINT8*)block + sizeof(nuroMemBlock);
			block->next = blocks;
			blocks = block;
		}
		return block;
	}
	nuVOID* Malloc(nuSIZE size)
	{
		size = ((size+3) & 0xfffffffc);
		if (!blocks || (blocks->freesize < size))
		{
			if (!AddBlock(size))
			{
				return 0;
			}
		}
		if (!blocks)
		{
			return 0;
		}
		nuVOID* p = blocks->freepos;
		blocks->freepos += size;
		blocks->freesize -= size;
		return p;
	}
	nuVOID Free(nuVOID* p)
	{//no action		
	}
	nuroMemBlock* blocks;
};
struct nuroPointListNode
{
	NUROPOINT  point;
	nuroPointListNode* pre;
	nuroPointListNode* next;
};
class nuroPointList
{
public:
	nuroPointList()
	{
		nodeBegin = 0;
		nodeEnd = 0;
		total = 0;
	}
	~nuroPointList()
	{
		while (nodeBegin)
		{
			nodeEnd = nodeBegin->next;
			DestroyNode(nodeBegin);
			nodeBegin = nodeEnd;
		}
	}
	nuroSectionAlloc memAlloc;
	nuroPointListNode* nodeBegin;
	nuroPointListNode* nodeEnd;
	nuINT total;
	nuroPointListNode* CreateNode()
	{
		nuroPointListNode* p = (nuroPointListNode*)memAlloc.Malloc(sizeof(nuroPointListNode));
		if (!p)
		{
			return 0;
		}
		p->next = 0;
		p->pre = 0;
		//nuMemset(p, 0, sizeof(nuroPointListNode));
		return (nuroPointListNode*)p;
	}
	nuVOID DestroyNode(nuroPointListNode* node)
	{
		memAlloc.Free(node);
	}
	nuVOID AddPoint(const NUROPOINT& point)
	{
		nuroPointListNode* node = CreateNode();
		if (!node)
		{
			return;
		}
		node->point = point;
		if (nodeEnd)
		{
			nodeEnd->next = node;
			node->pre = nodeEnd;
			nodeEnd = node;
		}
		else
		{
			nodeBegin = nodeEnd = node;
		}
		++total;
	}
	nuVOID InsertPoint(nuroPointListNode* pos, const NUROPOINT& point)
	{
		if (!pos)
		{
			AddPoint(point);
			return;
		}
		nuroPointListNode* node = CreateNode();
		if (!node)
		{
			return;
		}
		node->point = point;
		if (pos->pre)
		{
			node->pre = pos->pre;
			pos->pre->next = node;
		}
		else
		{
			nodeBegin = node;
		}
		pos->pre = node;
		node->next = pos;
		++total;
	}
	nuVOID RemovePoint(nuroPointListNode* node)
	{
		if (!node)
		{
			return;
		}
		if (nodeBegin == node)
		{
			if (nodeEnd == node)
			{
				nodeBegin = 0;
				nodeEnd = 0;
			}
			else
			{
				nodeBegin = node->next;
				node->next->pre = 0;
			}
		}
		else
		{
			if (nodeEnd == node)
			{
				nodeEnd = node->pre;
				nodeEnd->next = 0;
			}
			else
			{
				node->pre->next = node->next;
				node->next->pre = node->pre;
			}
		}
		DestroyNode(node);
		--total;
		return;
	}
};

bool CrossXLU(NUROPOINT StartPt,NUROPOINT TargetPt, nuINT TargeX, nuINT y0, nuINT y1,NUROPOINT &RePt)//?^?ǭȪ??ܥi?θ??Ƽ?
{
	if((StartPt.x<TargeX && TargeX<TargetPt.x) || (TargetPt.x<TargeX && TargeX<StartPt.x))
	{
		RePt.x=TargeX;
		RePt.y=StartPt.y + (TargeX-StartPt.x)*(TargetPt.y-StartPt.y)/(TargetPt.x-StartPt.x);
		if(y0<=RePt.y && RePt.y<=y1) return true;
	}
	return false;
}
bool CrossYLU(NUROPOINT StartPt,NUROPOINT TargetPt, nuINT TargeY, nuINT x0, nuINT x1,NUROPOINT &RePt)//?^?ǭȪ??ܥi?θ??Ƽ?
{
	if((StartPt.y<TargeY && TargeY<TargetPt.y) || (TargetPt.y<TargeY && TargeY<StartPt.y))
	{
		RePt.x=StartPt.x + (TargeY-StartPt.y)*(TargetPt.x-StartPt.x)/(TargetPt.y-StartPt.y);
		RePt.y=TargeY;
		if(x0<=RePt.x && RePt.x<=x1) return true;
	}
	return false;
}
nuVOID SortPtArrayLU(NUROPOINT StartPt,NUROPOINT TargetPt,long PtCount,PNUROPOINT &RePt)//?^?ǭȪ??ܥi?θ??Ƽ?
{
	long i,j;
	NUROPOINT tmpPt;
	if(PtCount>1)
	{
		return;
	}
	if(StartPt.x==TargetPt.x)
	{
		//x?ۦP?N??y?Ӥ???
		if(StartPt.y<TargetPt.y)
		{
			for(i=0;i<PtCount;i++)
			{
				for(j=0;j<i;j++)
				{
					if(RePt[i-j-1].y>RePt[i].y)
					{
						tmpPt=RePt[i-j-1];
						RePt[i-j-1]=RePt[i];
						RePt[i]=tmpPt;
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			for(i=0;i<PtCount;i++)
			{
				for(j=0;j<i;j++)
				{
					if(RePt[i-j-1].y<RePt[i].y)
					{
						tmpPt=RePt[i-j-1];
						RePt[i-j-1]=RePt[i];
						RePt[i]=tmpPt;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		//x?i?H?????j?p
		if(StartPt.x<TargetPt.x)
		{
			for(i=0;i<PtCount;i++)
			{
				for(j=0;j<i;j++)
				{
					if(RePt[i-j-1].x>RePt[i].x)
					{
						tmpPt=RePt[i-j-1];
						RePt[i-j-1]=RePt[i];
						RePt[i]=tmpPt;
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			for(i=0;i<PtCount;i++)
			{
				for(j=0;j<i;j++)
				{
					if(RePt[i-j-1].x<RePt[i].x)
					{
						tmpPt=RePt[i-j-1];
						RePt[i-j-1]=RePt[i];
						RePt[i]=tmpPt;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

long Get4CrossLU(NUROPOINT StartPt,NUROPOINT TargetPt, nuINT x0, nuINT y0, nuINT x1, nuINT y1,PNUROPOINT RePt)//?^?ǭȪ??ܥi?θ??Ƽ?
{
	long recount;
	recount=0;
	if(CrossXLU(StartPt,TargetPt,x0,y0,y1,RePt[recount])) recount++;
	if(CrossXLU(StartPt,TargetPt,x1,y0,y1,RePt[recount])) recount++;
	if(CrossYLU(StartPt,TargetPt,y0,x0,x1,RePt[recount])) recount++;
	if(CrossYLU(StartPt,TargetPt,y1,x0,x1,RePt[recount])) recount++;
	SortPtArrayLU(StartPt,TargetPt,recount,RePt);
	//???s?ƦC?^??array????
	return recount;
}

long GetFixLineDisLU(NUROPOINT NowPt, NUROPOINT LinePt1, NUROPOINT LinePt2 )
{
	NUROPOINT FixedPt;
	float k = 1;
	long DisToPt1;
	long DisToPt2;
	long ReDis;
	ReDis=NURO_MAXFIXDIS;

	if(!(	(LinePt1.x<NowPt.x && NowPt.x<LinePt2.x) ||
			(LinePt2.x<NowPt.x && NowPt.x<LinePt1.x) ||
			(LinePt1.y<NowPt.y && NowPt.y<LinePt2.y) ||
			(LinePt2.y<NowPt.y && NowPt.y<LinePt1.y)	) )
	{
		return ReDis;
	}


	if( LinePt1.x == LinePt2.x )
	{
		if( LinePt1.y == LinePt2.y )
		{
			FixedPt.x = LinePt1.x;
			FixedPt.y = LinePt1.y;
		}
		else
		{
			FixedPt.x = LinePt1.x;
			FixedPt.y = NowPt.y;
			if( FixedPt.y > NURO_MAX(LinePt1.y, LinePt2.y) ) FixedPt.y = NURO_MAX(LinePt1.y, LinePt2.y);
			if(	FixedPt.y < NURO_MIN(LinePt1.y, LinePt2.y) ) FixedPt.y = NURO_MIN(LinePt1.y, LinePt2.y);
		}
	}
	else
	{
		if( LinePt1.y == LinePt2.y )
		{
			FixedPt.x = NowPt.x;
			FixedPt.y = LinePt1.y;
			if( FixedPt.x > NURO_MAX(LinePt1.x, LinePt2.x) ) FixedPt.x = NURO_MAX(LinePt1.x, LinePt2.x);
			if(	FixedPt.x < NURO_MIN(LinePt1.x, LinePt2.x) ) FixedPt.x = NURO_MIN(LinePt1.x, LinePt2.x);
		}
		else
		{
			k=(float)((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt.x	=	NowPt.x + ((LinePt2.y - LinePt1.y)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt.y	=	NowPt.y - ((LinePt2.x - LinePt1.x)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));

			if( FixedPt.x > NURO_MAX(LinePt1.x, LinePt2.x) || FixedPt.x < NURO_MIN(LinePt1.x, LinePt2.x)    )
			{
				DisToPt1 = NURO_ABS( FixedPt.x - LinePt1.x );
				DisToPt2 = NURO_ABS( FixedPt.x - LinePt2.x );
				if( DisToPt1 > DisToPt2 )
				{
					FixedPt.x = LinePt2.x;
					FixedPt.y = LinePt2.y;
				}
				else
				{
					FixedPt.x = LinePt1.x;
					FixedPt.y = LinePt1.y;
				}
			}
		}
	}

	double dx, dy, dnum;
	dx = (FixedPt.x - NowPt.x);
	dy = (FixedPt.y - NowPt.y);
	dnum=(dx*dx)+(dy*dy);
	ReDis = (long)nuSqrt( dnum );
	return ReDis;
}
bool GetNearBoundPtLU(NUROPOINT StartPt,NUROPOINT TargetPt, nuINT x0, nuINT y0, nuINT x1, nuINT y1,PNUROPOINT RePt)
{
	NUROPOINT TmpPt;
	long MaxDis,Dis;
	MaxDis=NURO_MAXFIXDIS;
	Dis=NURO_MAXFIXDIS;
	TmpPt.x=x0;
	TmpPt.y=y0;
	Dis=GetFixLineDisLU(TmpPt, StartPt, TargetPt );
	if(MaxDis>Dis)
	{
		RePt[0]=TmpPt;
		MaxDis=Dis;
	}
	TmpPt.x=x0;
	TmpPt.y=y1;
	Dis=GetFixLineDisLU(TmpPt, StartPt, TargetPt );
	if(MaxDis>Dis)
	{
		RePt[0]=TmpPt;
		MaxDis=Dis;
	}
	TmpPt.x=x1;
	TmpPt.y=y0;
	Dis=GetFixLineDisLU(TmpPt, StartPt, TargetPt );
	if(MaxDis>Dis)
	{
		RePt[0]=TmpPt;
		MaxDis=Dis;
	}
	TmpPt.x=x1;
	TmpPt.y=y1;
	Dis=GetFixLineDisLU(TmpPt, StartPt, TargetPt );
	if(MaxDis>Dis)
	{
		RePt[0]=TmpPt;
		MaxDis=Dis;
	}
	if(	MaxDis==NURO_MAXFIXDIS)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool ClipGonLU(const NUROPOINT *pPoint, nuINT n, nuINT x0, nuINT y0, nuINT x1, nuINT y1, nuroPointList& l)
{
	if ( 1 )
	{
		if ((pPoint[0].x == pPoint[n - 1].x) && (pPoint[0].y == pPoint[n - 1].y))
		{
			--n;
		}
		if (n<3)
		{
			return false;
		}
		for (nuINT i=0; i<n; ++i)
		{
			l.AddPoint(pPoint[i]);
		}
		
		nuINT flag0 = 0;
		nuINT flag1 = 0;
		long AddCount,Addindex;
		NUROPOINT AddPt[4];
		nuroPointListNode* node;
		nuroPointListNode* tmpnode;
		NUROPOINT lastPoint;
		NUROPOINT tmppt;
		
		lastPoint = l.nodeEnd->point;
		node = l.nodeBegin;
		while (node)//?W???I
		{
			AddCount=0;
			AddCount=Get4CrossLU(lastPoint,node->point,x0,y0,x1,y1,AddPt);

			if(AddCount==0)
			{
				if(GetNearBoundPtLU(lastPoint,node->point, x0, y0, x1, y1,AddPt))
				{
					l.InsertPoint(node, AddPt[0]);					
				}
			}
			else
			{
				for(Addindex=0;Addindex<AddCount;Addindex++)
				{
					l.InsertPoint(node, AddPt[AddCount-Addindex-1]);					
				}
			}
			lastPoint = node->point;
			node = node->next;
		}
		if (l.total<3)
		{
			return false;
		}

/*
		node = l.nodeBegin->next;
		lastPoint = l.nodeBegin->point;
		while (node)//?R?I
		{
			if(lastPoint.x==node->point.x && lastPoint.y==node->point.y)//?P?I?ư? ?o��?ӫܦX?z?a
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
				continue;
			}

			if(node->point.x<x0 || node->point.x>x1 || node->point.y<y0 || node->point.y>y1)
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
*/
		node = l.nodeBegin;
		while (node)//?R?I
		{
			if(node->point.x<x0 || node->point.x>x1 || node->point.y<y0 || node->point.y>y1)
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		if (l.total<3)
		{
			return false;
		}

		node = l.nodeBegin;
		lastPoint = l.nodeEnd->point;
		while (node)//?R?I
		{
			if(lastPoint.x==node->point.x && lastPoint.y==node->point.y)//?P?I?ư? ?o��?ӫܦX?z?a
			{
				lastPoint = node->point;
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
				continue;
			}
			lastPoint = node->point;
			node = node->next;
		}
		if (l.total<3)
		{
			return false;
		}
	}
	if (l.total<3)
	{
		return false;
	}

	return true;
}
bool ClipGon(const NUROPOINT *pPoint, nuINT n, nuINT x0, nuINT y0, nuINT x1, nuINT y1, nuroPointList& l)
{
	//Louis ???V?sAPI?B?@
//	return ClipGonLU(pPoint,n,x0,y0,x1,y1,l);

	if ( 1 )
	{
		if ((pPoint[0].x == pPoint[n - 1].x) && (pPoint[0].y == pPoint[n - 1].y))
		{
			--n;
		}
		if (n<3)
		{
			return false;
		}
		for (nuINT i=0; i<n; ++i)
		{
			l.AddPoint(pPoint[i]);
		}
		
		nuINT flag0 = 0;
		nuINT flag1 = 0;
		nuroPointListNode* node;
		nuroPointListNode* tmpnode;
		NUROPOINT lastPoint;
		NUROPOINT tmppt;
		
		//x1 clip
		lastPoint = l.nodeBegin->point;
		flag0 = 0;
		if (lastPoint.x > x1)
		{
			flag0 = 1;
		}
		node = l.nodeBegin->next;
		tmppt.x = x1;
		while (node)
		{
			flag1 = 0;
			if (node->point.x > x1)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.y = lastPoint.y + (x1-lastPoint.x)*(node->point.y-lastPoint.y)/(node->point.x-lastPoint.x);
				l.InsertPoint(node, tmppt);					
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		{
			node = l.nodeBegin;
			flag1 = 0;
			if (node->point.x > x1)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.y = lastPoint.y + (x1-lastPoint.x)*(node->point.y-lastPoint.y)/(node->point.x-lastPoint.x);
				l.InsertPoint(node, tmppt);
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		
		//x0 clip
		if (l.total < 3)
		{
			return false;
		}
		lastPoint = l.nodeBegin->point;
		flag0 = 0;
		if (lastPoint.x < x0)
		{
			flag0 = 1;
		}
		node = l.nodeBegin->next;
		tmppt.x = x0;
		while (node)
		{
			flag1 = 0;
			if (node->point.x < x0)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.y = lastPoint.y + (x0-lastPoint.x)*(node->point.y-lastPoint.y)/(node->point.x-lastPoint.x);
				l.InsertPoint(node, tmppt);		
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		{
			node = l.nodeBegin;
			flag1 = 0;
			if (node->point.x < x0)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.y = lastPoint.y + (x0-lastPoint.x)*(node->point.y-lastPoint.y)/(node->point.x-lastPoint.x);
				l.InsertPoint(node, tmppt);	
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		
		//y1 clip
		if (l.total < 3)
		{
			return false;
		}
		lastPoint = l.nodeBegin->point;
		flag0 = 0;
		if (lastPoint.y > y1)
		{
			flag0 = 1;
		}
		node = l.nodeBegin->next;
		tmppt.y = y1;
		while (node)
		{
			flag1 = 0;
			if (node->point.y > y1)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.x = lastPoint.x + (y1-lastPoint.y)*(node->point.x-lastPoint.x)/(node->point.y-lastPoint.y);
				l.InsertPoint(node, tmppt);				
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		{
			node = l.nodeBegin;
			flag1 = 0;
			if (node->point.y > y1)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.x = lastPoint.x + (y1-lastPoint.y)*(node->point.x-lastPoint.x)/(node->point.y-lastPoint.y);
				l.InsertPoint(node, tmppt);
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}

		//y0 clip
		if (l.total < 3)
		{
			return false;
		}
		lastPoint = l.nodeBegin->point;
		flag0 = 0;
		if (lastPoint.y < y0)
		{
			flag0 = 1;
		}
		node = l.nodeBegin->next;
		tmppt.y = y0;
		while (node)
		{
			flag1 = 0;
			if (node->point.y < y0)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.x = lastPoint.x + (y0-lastPoint.y)*(node->point.x-lastPoint.x)/(node->point.y-lastPoint.y);
				l.InsertPoint(node, tmppt);				
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		{
			node = l.nodeBegin;
			flag1 = 0;
			if (node->point.y < y0)
			{
				flag1 = 1;
			}
			if (flag0 != flag1)//new point
			{
				tmppt.x = lastPoint.x + (y0-lastPoint.y)*(node->point.x-lastPoint.x)/(node->point.y-lastPoint.y);
				l.InsertPoint(node, tmppt);
			}
			lastPoint = node->point;
			flag0 = flag1;
			if (flag1)//remove tmpnode
			{
				tmpnode = node;
				node = node->next;
				l.RemovePoint(tmpnode);
			}
			else
			{
				node = node->next;
			}
		}
		if (l.total < 3)
		{
			return false;
		}
		return true;
	}
	return true;
}
nuVOID nuBMPFillGon_Detail(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuWORD color,nuINT nMode, nuroSectionAlloc& alloc)
{
    nuINT total = n-1;
    if ((pPoint[0].x != pPoint[n - 1].x) || (pPoint[0].y != pPoint[n - 1].y))
    {
        ++total;
    }
    if ( total <= NURO_FILLGON_PTNUM_DEF)
    {
        FILLGON_LINE_CONTENT pLines[NURO_FILLGON_PTNUM_DEF];
        nuINT pPolyInts[NURO_FILLGON_PTNUM_DEF];
        nuINT i = 0;
        nuINT k = 0;
        nuINT miny = pPoint[0].y<<__FLOAT_SHIFT;
        nuINT maxy = pPoint[0].y<<__FLOAT_SHIFT;
        if (total >= n)
        {
            nuBMPSetLineContent(pLines[i], pPoint[0], pPoint[n - 1], &miny, &maxy);
            ++i;
        }
        for (k=1; k<n; ++i, ++k)
        {
            nuBMPSetLineContent(pLines[i], pPoint[k], pPoint[k-1], &miny, &maxy);
        }
        if (miny<0)
        {
            miny = 0;
        }
        if (maxy>(pBmp->bmpHeight<<__FLOAT_SHIFT))
        {
            maxy = (pBmp->bmpHeight<<__FLOAT_SHIFT);
        }
        nuBMPFillGonShift(pBmp, pLines, pPolyInts, miny, maxy, total, color, nMode);
    }
    else
    {        
        FILLGON_LINE_CONTENT* pLines = (FILLGON_LINE_CONTENT*)alloc.Malloc((sizeof(FILLGON_LINE_CONTENT)+sizeof(nuINT))*total);
        if (!pLines)
        {
            return;
        }
        nuINT* pPolyInts = (nuINT*)&pLines[total];
        nuINT i = 0;
        nuINT k = 0;
        nuINT miny = pPoint[0].y<<__FLOAT_SHIFT;
        nuINT maxy = pPoint[0].y<<__FLOAT_SHIFT;
        if (total >= n)
        {
            nuBMPSetLineContent(pLines[i], pPoint[0], pPoint[n - 1], &miny, &maxy);
            ++i;
        }
        for (k=1; k<n; ++i, ++k)
        {
            nuBMPSetLineContent(pLines[i], pPoint[k], pPoint[k-1], &miny, &maxy);
        }
        if (miny<0)
        {
            miny = 0;
        }
        if (maxy>(pBmp->bmpHeight<<__FLOAT_SHIFT))
        {
            maxy = (pBmp->bmpHeight<<__FLOAT_SHIFT);
        }
        nuBMPFillGonShift(pBmp, pLines, pPolyInts, miny, maxy, total, color, nMode);
        if (pLines)
        {
            alloc.Free(pLines);
        }
    }
    return;
}
nuVOID nuBMPFillGon_New(PNURO_BMP pBmp, const NUROPOINT *pPoint, nuINT n, nuWORD color,nuINT nMode)
{
	nuroPointList l;
	if (!ClipGon(pPoint, n, 0, 0, pBmp->bmpWidth, pBmp->bmpHeight, l))
	{
		return;
	}
	NUROPOINT* pts = (NUROPOINT*)l.memAlloc.Malloc(l.total * sizeof(NUROPOINT));
	//NUROPOINT* pts = (NUROPOINT*)nuMalloc(l.total * sizeof(NUROPOINT));
	if (pts)
	{
		nuroPointListNode* node = l.nodeBegin;
		NUROPOINT* ptstag = pts;
		while (node)
		{
			*ptstag = node->point;
			++ptstag;
			node = node->next;
		}
		nuBMPFillGon_Detail(pBmp, pts, l.total, color, nMode, l.memAlloc);
		//nuBMPFillGon_Detail(pBmp, pts, l.total, color, nMode);
		//nuFree(pts);
		l.memAlloc.Free(pts);
	}
	//nuBMPFillGon_Detail(pBmp, pPoint, n, color, nMode);
}
nuVOID nuBMPDraw3DLine_New(PNURO_BMP pBmp, nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuWORD color565, nuINT dw, nuINT dw2,nuINT nMode)
{
	if (!dw2)
	{
		nuBMPDrawLine_New(pBmp, x1, y1, x2, y2, dw, color565, nMode);
		return;
	}
    nuINT deltax, deltay;
    if (y1 > y2)
    {
        nuINT tmp = x1;
        x1 = x2;
        x2 = tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    nuINT ar = 1;
    deltax = (x2 - x1);
    if (x1 > x2)
    {
        ar = -1;
        deltax = -deltax;
    }
    deltay = y2 - y1;
    float dd = (float)(deltay*deltay+deltax*deltax);
    if (!dd)
    {
        return;
    }
	dd = __FLOAT_SHIFT_STEP*nuro::math::QuickRSqrtF(dd);
    nuINT wx = (nuINT)((deltay*dw)*dd);
    nuINT wy = (nuINT)((deltax*dw)*dd);
	dw += dw2;
    nuINT wx2 = (nuINT)((deltay*dw)*dd);
    nuINT wy2 = (nuINT)((deltax*dw)*dd);
    wx >>= 1;
    wx2 >>= 1;
    wy >>= 1;
    wy2 >>= 1;
    x1 <<= __FLOAT_SHIFT;
    y1 <<= __FLOAT_SHIFT;
    x2 <<= __FLOAT_SHIFT;
    y2 <<= __FLOAT_SHIFT;
    nuINT pPolyInts[4];
    NUROPOINT pt[4];
    FILLGON_LINE_CONTENT pLines[4];
    pt[0].x = x1 - wx;
    pt[1].x = x1 + wx;
    pt[2].x = x2 + wx2;
    pt[3].x = x2 - wx2;
    if (ar>0)
    {
        pt[0].y = y1 + wy;
        pt[1].y = y1 - wy;
        pt[2].y = y2 - wy2;
        pt[3].y = y2 + wy2;
    }
    else
    {
        pt[0].y = y1 - wy;
        pt[1].y = y1 + wy;
        pt[2].y = y2 + wy2;
        pt[3].y = y2 - wy2;
    }
    nuINT miny = pt[0].y;
    nuINT maxy = pt[0].y;
    nuBMPSetLineContentShift(pLines[0], pt[3], pt[0], &miny, &maxy);
    nuBMPSetLineContentShift(pLines[1], pt[0], pt[1], &miny, &maxy);
    nuBMPSetLineContentShift(pLines[2], pt[1], pt[2], &miny, &maxy);
    nuBMPSetLineContentShift(pLines[3], pt[2], pt[3], &miny, &maxy);
    if (miny<0)
    {
        miny = 0;
    }
    if (maxy>(pBmp->bmpHeight<<__FLOAT_SHIFT))
    {
        maxy = (pBmp->bmpHeight<<__FLOAT_SHIFT);
    }
    nuBMPFillGonShift(pBmp, pLines, pPolyInts, miny, maxy, 4, color565, nMode);
	return;
}
