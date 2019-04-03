
#include "../libMethods.h"
#include "../../NuroDefine.h"

nuBOOL nuGetSRECT(nuroSRECT &rect, const nuroSRECT &fatherRect, const nuroSRECT &localRect, nuUINT nType)
{
	switch( nType )
	{
	case RTALIGNTYPE_00://Absolute coordinates
		rect.left	= localRect.left;
		rect.top	= localRect.top;
		rect.right	= localRect.right;
		rect.bottom	= localRect.bottom;
		break;
	case RTALIGNTYPE_01://To Left top 
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= localRect.right + fatherRect.left;
		rect.bottom	= localRect.bottom + fatherRect.top;
		break;
	case RTALIGNTYPE_02://To Right top
		rect.left	= fatherRect.right - localRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= localRect.bottom + fatherRect.top;
		break;
	case RTALIGNTYPE_03://To Left bottom
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= fatherRect.bottom - localRect.top;
		rect.right	= localRect.right + fatherRect.left;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_04://To right bottom
		rect.left	= fatherRect.right - localRect.left;
		rect.top	= fatherRect.bottom - localRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_05://To left right top
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= localRect.bottom + fatherRect.top;
		break;
	case RTALIGNTYPE_06://To left right bottom
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= fatherRect.bottom - localRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_07://To left top bottom
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= localRect.right + fatherRect.left;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_08://To right top bottom
		rect.left	= fatherRect.right - localRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_09://To left top right bottom
		rect.left	= localRect.left + fatherRect.left;
		rect.top	= localRect.top + fatherRect.top;
		rect.right	= fatherRect.right - localRect.right;
		rect.bottom	= fatherRect.bottom - localRect.bottom;
		break;
	case RTALIGNTYPE_10:
		rect.left	= localRect.left + fatherRect.left;
		rect.right  = localRect.right + fatherRect.left;
		rect.top	= (fatherRect.top + fatherRect.bottom) / 2 - localRect.top;
		rect.bottom = (fatherRect.top + fatherRect.bottom) / 2 + localRect.bottom;
		break;
	case RTALIGNTYPE_11:
		rect.left	= fatherRect.right - localRect.left;
		rect.right  = fatherRect.right - localRect.right;
		rect.top	= (fatherRect.top + fatherRect.bottom) / 2 - localRect.top;
		rect.bottom = (fatherRect.top + fatherRect.bottom) / 2 + localRect.bottom;
		break;
	case RTALIGNTYPE_12:
		rect.left	= (fatherRect.left + fatherRect.right) / 2 - localRect.left;
		rect.right	= (fatherRect.left + fatherRect.right) / 2 + localRect.left;
		rect.top	= fatherRect.top + localRect.top;
		rect.bottom = fatherRect.top + localRect.bottom;
		break;
	case RTALIGNTYPE_13:
		rect.left	= (fatherRect.left + fatherRect.right) / 2 - localRect.left;
		rect.right	= (fatherRect.left + fatherRect.right) / 2 + localRect.left;
		rect.top	= fatherRect.bottom - localRect.top;
		rect.bottom = fatherRect.bottom + localRect.bottom;
		break;
	default:
		rect.left	= localRect.left;
		rect.top	= localRect.top;
		rect.right	= localRect.right;
		rect.bottom	= localRect.bottom;
		break;
	}
		
	rect.left = NURO_MAX(rect.left, fatherRect.left);
	rect.left = NURO_MIN(rect.left, fatherRect.right);
	
	rect.right = NURO_MAX(rect.right, fatherRect.left);
	rect.right = NURO_MIN(rect.right, fatherRect.right);
	
	rect.top = NURO_MAX(rect.top, fatherRect.top);
	rect.top = NURO_MIN(rect.top, fatherRect.bottom);
	
	rect.bottom = NURO_MAX(rect.bottom, fatherRect.top);
	rect.bottom = NURO_MIN(rect.bottom, fatherRect.bottom);
	
	return nuTRUE;
}

nuVOID nuNormalizeRect(LPNURORECT pRect)
{
	if( pRect != NULL )
	{
		nuLONG temp;
		if( pRect->left > pRect->right )
		{
			temp = pRect->left;
			pRect->left = pRect->right;
			pRect->right = temp;
		}
		if( pRect->top > pRect->bottom)
		{
			temp = pRect->top;
			pRect->top = pRect->bottom;
			pRect->bottom = temp;
		}
	}
}
/* Judge if a point is in a rectangle. */
nuBOOL nuPointInRect(LPNUROPOINT pPt, LPNURORECT pRect)
{
	return (( pPt->x >= pRect->left && pPt->x <= pRect->right ) & 
		( pPt->y >= pRect->top && pPt->y <= pRect->bottom ));
}
nuBOOL nuPtInSRect(const NUROPOINT& pt, const NUROSRECT& rect)
{
	return (( pt.x >= rect.left && pt.x <= rect.right ) & 
		( pt.y >= rect.top && pt.y <= rect.bottom ));
}

nuBOOL nuPtInRect(const NUROPOINT& pt, const NURORECT& rect)
{
	return (( pt.x >= rect.left && pt.x <= rect.right ) & 
		( pt.y >= rect.top && pt.y <= rect.bottom ));
}
nuBOOL nuPtInPolygon(const NUROPOINT& pt, const NUROPOINT* pPt, nuINT nCount)
{
	int inc = 0;
	if (pPt[0].x!=pPt[nCount-1].x || pPt[0].y!=pPt[nCount-1].y)
	{
		inc = 1;
	}
	int ind1, ind2;
	int i = 0;
	int ints = 0;
	int tagx = 0;
	for (i=0; i<nCount; i++)
	{
		if (!i)
		{
			if (inc)
			{
				ind1 = nCount - 1;
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
		if (pt.y < NURO_MIN(pPt[ind1].y, pPt[ind2].y) 
			|| pt.y > NURO_MAX(pPt[ind1].y, pPt[ind2].y) )
		{
			continue;
		}
		if( (pt.y==pPt[ind1].y)&&(pt.y==pPt[ind2].y))
		{
			if (pt.x >= NURO_MIN(pPt[ind1].x, pPt[ind2].x) 
			&& pt.x <= NURO_MAX(pPt[ind1].x, pPt[ind2].x))
			{
				return nuTRUE;
			}
		}
		else if(pt.y==pPt[ind1].y)
		{
			if (pPt[ind1].x==pt.x)
			{
				return nuTRUE;
			}
		}
		else if(pt.y==pPt[ind2].y)
		{
			if (pPt[ind2].x==pt.x)
			{
				return nuTRUE;
			}
		}
		else
		{
			tagx
				= (pt.y - pPt[ind1].y) * (pPt[ind2].x - pPt[ind1].x) / (pPt[ind2].y - pPt[ind1].y)
				+ pPt[ind1].x;
			if (tagx==pt.x)
			{
				return nuTRUE;
			}
			if (tagx>pt.x)
			{
				++ints;
				}
			}
		}
	return ints&0x01;
}
nuBOOL  nuSPtInRect(const NUROSPOINT& pt, const NURORECT& rect)
{
	return (( pt.x >= rect.left && pt.x <= rect.right ) & 
		( pt.y >= rect.top && pt.y <= rect.bottom ));
}
/* Judge whether a rectangle covers another one. */
nuBOOL nuRectCoverRect(LPNURORECT pRect1, LPNURORECT pRect2)
{
	nuBOOL crossX = nuFALSE;
	nuBOOL crossY = nuFALSE;
	crossX = ( pRect1->left >= pRect2->left  && pRect1->left <= pRect2->right )    ||
		( pRect1->right >= pRect2->left && pRect1->right <= pRect2->right )   ||
		( pRect2->left >= pRect1->left  && pRect2->left <= pRect1->right )    ||
		( pRect2->right >= pRect1->left && pRect2->right <= pRect1->right ) ;
	crossY = ( pRect1->top >= pRect2->top    && pRect1->top <= pRect2->bottom )    ||
		( pRect1->bottom >= pRect2->top && pRect1->bottom <= pRect2->bottom ) ||
		( pRect2->top >= pRect1->top    && pRect2->top <= pRect1->bottom )    ||
		( pRect2->bottom >= pRect1->top  && pRect2->bottom <= pRect1->bottom ) ;
	return (crossX & crossY);
}
nuBOOL nuPtFixToLineSeg(const nuroPOINT& NowPt, const nuroPOINT& LinePt1, const nuroPOINT& LinePt2, nuroPOINT *FixedPt, nuLONG *Dis)
{
//	nuDOUBLE k = 1.;
	nuLONG DisToPt1;
	nuLONG DisToPt2;
	if( LinePt1.x == LinePt2.x )
	{
		if( LinePt1.y == LinePt2.y )
		{
			*FixedPt = LinePt1;
		}
		else
		{
			FixedPt->x = LinePt1.x;
			FixedPt->y = NowPt.y;
			if( FixedPt->y > NURO_MAX(LinePt1.y, LinePt2.y) ) FixedPt->y = NURO_MAX(LinePt1.y, LinePt2.y);
			if(	FixedPt->y < NURO_MIN(LinePt1.y, LinePt2.y) ) FixedPt->y = NURO_MIN(LinePt1.y, LinePt2.y);
		}
	}
	else
	{
		if( LinePt1.y == LinePt2.y )
		{
			FixedPt->x = NowPt.x;
			FixedPt->y = LinePt1.y;
			if( FixedPt->x > NURO_MAX(LinePt1.x, LinePt2.x) ) FixedPt->x = NURO_MAX(LinePt1.x, LinePt2.x);
			if(	FixedPt->x < NURO_MIN(LinePt1.x, LinePt2.x) ) FixedPt->x = NURO_MIN(LinePt1.x, LinePt2.x);
		}
		else
		{
		/************************************************************************ 
		点到直线的垂足点的计算公式
		x = ( k^2 * pt1.x + k * (point.y - pt1.y ) + point.x ) / ( k^2 + 1) ，
		y = k * ( x - pt1.x) + pt1.y;
		k 是直线的斜率，pt1是直线上的一点 point是直线外的点
			************************************************************************/
			/*
			k = ( (nuFLOAT)(LinePt2.y - LinePt1.y) / (nuFLOAT)(LinePt2.x - LinePt1.x) );
			FixedPt->x =(nuLONG)( ( k*k*LinePt1.x + k*(NowPt.y-LinePt1.y) + NowPt.x ) / ( k*k +1 ) );
			FixedPt->y =(nuLONG)( k * ( FixedPt->x - LinePt1.x ) + LinePt1.y );
			*/
			/*
			k=((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt->x	=	NowPt.x + (nuLONG)(k*(nuFLOAT)(LinePt2.y - LinePt1.y));
			FixedPt->y	=	NowPt.y - (nuLONG)(k*(nuFLOAT)(LinePt2.x - LinePt1.x));
			*/
//			k=((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt->x	=	NowPt.x + ((LinePt2.y - LinePt1.y)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			FixedPt->y	=	NowPt.y - ((LinePt2.x - LinePt1.x)*((NowPt.y-LinePt1.y)*(LinePt2.x - LinePt1.x)-(NowPt.x-LinePt1.x)*(LinePt2.y - LinePt1.y)))/((LinePt2.x - LinePt1.x)*(LinePt2.x - LinePt1.x)+(LinePt2.y - LinePt1.y)*(LinePt2.y - LinePt1.y));
			
			if( FixedPt->x > NURO_MAX(LinePt1.x, LinePt2.x) || 
				FixedPt->x < NURO_MIN(LinePt1.x, LinePt2.x) ||
				FixedPt->y > NURO_MAX(LinePt1.y, LinePt2.y) || 
				FixedPt->y < NURO_MIN(LinePt1.y, LinePt2.y) )
			{
				DisToPt1 = ( FixedPt->x - LinePt1.x );
				DisToPt2 = ( FixedPt->x - LinePt2.x );
				DisToPt1 = NURO_ABS(DisToPt1);
				DisToPt2 = NURO_ABS(DisToPt2);
				if( DisToPt1 > DisToPt2 )
				{
					*FixedPt = LinePt2;
				}
				else
				{
					*FixedPt = LinePt1;
				}
			}
		}
	}
	
	nuLONG dx, dy, dnum;
	dx = (FixedPt->x - NowPt.x) / COORCONSTANT;
	dy = (FixedPt->y - NowPt.y) / COORCONSTANT;
	dnum=(dx*dx)+(dy*dy);
	*Dis = (nuLONG)nuSqrt( dnum );
	return nuTRUE;
}
nuLONG nuGetXBlkNum(nuLONG nBlockSize)
{
	return((HALF_MAX_LONGITUDE*2)/nBlockSize);
}
nuVOID nuBlockIDtoRectEx(nuDWORD nBlockID, NURORECT& rect, nuLONG nBlockSize)
{
	int nXnum = (HALF_MAX_LONGITUDE*2)/nBlockSize;
	rect.left = nBlockID % nXnum;
	rect.top = nBlockID / nXnum;
	rect.left = rect.left * nBlockSize - HALF_MAX_LONGITUDE;
	rect.top = rect.top * nBlockSize - HALF_MAX_LATITUDE;
	rect.right = rect.left + nBlockSize;
	rect.bottom = rect.top + nBlockSize;
}
nuVOID nuBlockIDtoRect(nuDWORD nBlockID, LPNURORECT lpRect)
{
	nuLONG x, y;
	x = nBlockID % XBLOCKNUM;
	y = nBlockID / XBLOCKNUM;
	x = (x - XBLOCKNUM / 2) * EACHBLOCKSIZE;
	y = (y - YBLOCKNUM / 2) * EACHBLOCKSIZE;
	lpRect->left	= x;
	lpRect->top		= y;
	lpRect->right	= x + EACHBLOCKSIZE;
	lpRect->bottom	= y + EACHBLOCKSIZE;
}
nuVOID nuDegreeToMeter(nuFLOAT fDegree, nuLONG &meter)
{
	nuLONG nD, nF;
	nD = (nuLONG)(fDegree/100);
	nF = (nuLONG)((fDegree-(nD*100))*EACHBLOCKSIZE*BLOCKCOUNTOFEACHDEGREE/60);
	meter	= nD * EACHBLOCKSIZE * BLOCKCOUNTOFEACHDEGREE + nF;
}
nuVOID	nuMeterToDegree(nuLONG  meter, nuFLOAT &fDegree)
{
	fDegree = (nuFLOAT)(meter*1.0/(EACHBLOCKSIZE*BLOCKCOUNTOFEACHDEGREE));
}
nuVOID nuRectToBlockIDArray(const NURORECT& rect, PBLOCKIDARRAY pBlockIDArray)
{
	pBlockIDArray->nXstart	= (rect.left - MAXLENOFROADOUTBLOCK + XBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nXend	= (rect.right+ MAXLENOFROADOUTBLOCK + XBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nYstart	= (rect.top - MAXLENOFROADOUTBLOCK + YBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nYend	= (rect.bottom + MAXLENOFROADOUTBLOCK + YBLOCKNUM*EACHBLOCKSIZE/2)/EACHBLOCKSIZE;
	pBlockIDArray->nXstart -= 1;//Extend the number of the blocks
	pBlockIDArray->nYstart -= 1;
	pBlockIDArray->nXend += 1;//Extend the number of the blocks
	pBlockIDArray->nYend += 1;
	if( pBlockIDArray->nXstart < 0 )	pBlockIDArray->nXstart = 0;
	if( pBlockIDArray->nYstart < 0 )	pBlockIDArray->nYstart = 0;
	if( pBlockIDArray->nXend >= XBLOCKNUM ) pBlockIDArray->nXend = XBLOCKNUM - 1;
	if( pBlockIDArray->nYend >= YBLOCKNUM ) pBlockIDArray->nYend = YBLOCKNUM - 1;
}
nuVOID nuRectToBlockIDs(const NURORECT &rect, PBLOCKIDARRAY pBlockIDArray, nuLONG nBlockSize)
{
	pBlockIDArray->nXstart	= (rect.left + HALF_MAX_LONGITUDE)/nBlockSize;
	pBlockIDArray->nXend	= (rect.right + HALF_MAX_LONGITUDE)/nBlockSize;
	pBlockIDArray->nYstart	= (rect.top + HALF_MAX_LATITUDE)/nBlockSize;
	pBlockIDArray->nYend	= (rect.bottom + HALF_MAX_LATITUDE)/nBlockSize;
	
	if( pBlockIDArray->nXstart < 0 )	pBlockIDArray->nXstart = 0;
	if( pBlockIDArray->nYstart < 0 )	pBlockIDArray->nYstart = 0;
}
nuBOOL nuBlockIDArrayEqual(const BLOCKIDARRAY& block1, const BLOCKIDARRAY& block2)
{
	if( block1.nXstart  != block2.nXstart	||
		block1.nXend	!= block2.nXend		||
		block1.nYstart	!= block2.nYstart	||
		block1.nYend	!= block2.nYend	)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL nuPointFix(nuroPOINT ptCenter,nuroPOINT &ptFix,nuroRECT ScreenRt)
{
	if ( !nuPointInRect( &ptCenter, &ScreenRt))
	{
		return nuFALSE;
	}
	nuroPOINT ptMid = {0, 0};
	while(1)
	{
		ptMid.x = ( ptCenter.x + ptFix.x ) / 2;
		ptMid.y = ( ptCenter.y + ptFix.y ) / 2;
		if ( nuPointInRect( &ptMid, &ScreenRt))
		{
			break;
		}
		else
		{
			ptFix = ptMid;
		}
	}
	return nuTRUE;
}
#define EARTH_RADIUS  6378.137
nuLONG nuDis(nuDOUBLE lat1, nuDOUBLE lng1, nuDOUBLE lat2, nuDOUBLE lng2)
{
	nuDOUBLE  x1 = EARTH_RADIUS*nuCosJ(lat1)*nuCosJ(lng1) ;
	nuDOUBLE	y1 = EARTH_RADIUS*nuCosJ(lat1)*nuSinJ(lng1) ;
	nuDOUBLE  z1 = EARTH_RADIUS* nuSinJ(lat1) ;
	nuDOUBLE  x2 = EARTH_RADIUS*nuCosJ(lat2)*nuCosJ(lng2) ;
	nuDOUBLE	y2 = EARTH_RADIUS*nuCosJ(lat2)*nuSinJ(lng2) ;
	nuDOUBLE  z2 = EARTH_RADIUS* nuSinJ(lat2) ;
	nuDOUBLE dis = nuSqrt( (x1-x2)*(x1-x2) +(y1- y2)*(y1- y2) + (z1- z2)*(z1- z2));
	return (nuLONG)dis;
}
