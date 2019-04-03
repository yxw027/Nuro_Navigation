// ViewTransf.cpp: implementation of the CViewTransf class.
//
//////////////////////////////////////////////////////////////////////

#include "ViewTransf.h"
#include "NuroModuleApiStruct.h"

extern PGDIAPI				g_pMtGdiApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewTransf::CViewTransf()
{
	nSCx	= 160;
	nSCy	= 240;
	D		= 240;
	A		= 80*240;
	K		= 80;
	YCut	= 300;
	KxN		= 3;
	KxD		= 1;
	Height	= 240;
	m_nLineTop	= D;
}

CViewTransf::~CViewTransf()
{

}

nuBOOL CViewTransf::Initialize()
{
	return nuTRUE;
}

nuVOID CViewTransf::Free()
{

}

nuLONG CViewTransf::SetParam(nuLONG nSW, nuLONG nSH, nuDOUBLE fDScale /* = 1.0 */, 
						     nuDOUBLE fYTscale /* = 0.75 */, nuDOUBLE fYBscale /* = 0.15 */, nuLONG kxn /* = 3 */, 
						     nuLONG kxd /* = 1 */)
{
	if( fYTscale > fDScale || fYBscale > fYTscale )
	{
		return -1;
	}
	nSCx	= nSW/2;
	nSCy	= nSH;
	Height	= nSH;
	KxN		= kxn;
	KxD		= kxd;
	/* y' = D - A / (y + k);*/
	D = (nuLONG)(Height*fDScale);//infinitude y to this value, above the screen.
	/*fYTscale表示原来的Y=0，压缩到哪里的公式*/
//	K = (nuLONG)(Height*(fDScale - fYTscale)/(fYTscale - fYBscale));
//	A = (nuLONG)(Height*Height*(fDScale - fYBscale)*(fDScale - fYTscale)/(fYTscale - fYBscale));
	/*fYBscale表示变换前后Y值不变所在屏幕高度的比值*/
	K = (nuLONG)(Height*(fDScale - fDScale*fYBscale - fYTscale + fYBscale*fYBscale)/(fYTscale - fYBscale));
	A = (nuLONG)(Height*Height*(fDScale - fYTscale)*(1 - fYBscale)*(fDScale - fYBscale)/(fYTscale - fYBscale));
	/*屏幕坐标中y值比这个值大的点*/
	nuLONG tx = nSCx;
	nuLONG ty = Height + 20;
	Screen3Dto2D(tx, ty);
	tx = nSCy + K - 2;
	YCut = tx < ty ? tx : ty;

	m_nLineTop = (nuLONG)(Height);

	/*Sky height*/
	tx = nSCx;
	ty = 0;
	Screen2Dto3D(tx, ty);
	return ty;
}

nuVOID CViewTransf::Screen2Dto3D(nuLONG& x, nuLONG& y)
{
	if( y < m_nLineTop )
	{
		X_STOC(x);
		Y_STOC(y);
		Descartes2Dto3D(x, y);
		X_CTOS(x);
		Y_CTOS(y);
	}
}

nuVOID CViewTransf::Screen3Dto2D(nuLONG& x, nuLONG& y)
{
	X_STOC(x);
	Y_STOC(y);
	Descartes3Dto2D(x, y);
	X_CTOS(x);
	Y_CTOS(y);
}

nuLONG CViewTransf::Get3DExtendXLen(nuLONG xLen, nuLONG y)
{
	y = Height - y;
	if( y >= D || xLen <= 0 )
	{
		return 0;
	}
	else
	{
		return KxN * xLen * (m_nLineTop - y) / (m_nLineTop * KxD);
	}
}

nuLONG CViewTransf::Get3DThick(nuLONG nThick, nuLONG y)
{
	/*
	y = Height - y;
	if( y >= D || nThick <= 0 )
	{
		return 0;
	}
	else
	{
		return KxN * nThick * (m_nLineTop - y) / (m_nLineTop * KxD);
	}
	*/
	if( y < Height - D )
	{
		return 0;
	}
	y += D - Height;
	return (KxN * nThick * y) / (D * KxD);
	
}
/**/
nuBOOL CViewTransf::_3DPolyline(nuroPOINT *pPt, nuLONG nCount)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return nuFALSE;
	}
	nuLONG nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	if( pt1.y > YCut )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > YCut )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
				if( nListCount > 1 )
				{
					g_pMtGdiApi->GdiPolyline(ptList, nListCount);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		g_pMtGdiApi->GdiPolyline(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return nuTRUE;
}
nuBOOL CViewTransf::_3DDashline(nuroPOINT *pPt, nuLONG nCount, nuINT nDashLine)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return nuFALSE;
	}
	nuLONG nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	if( pt1.y > YCut )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > YCut )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
				if( nListCount > 1 )
				{
					g_pMtGdiApi->GdiDashLine(ptList, nListCount, nDashLine);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		g_pMtGdiApi->GdiDashLine(ptList, nListCount, nDashLine);
	}
	delete []ptList;
	ptList=NULL;
	return nuTRUE;
}
//

nuBOOL CViewTransf::_3DPolyline(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth)
{
	if( pPt == NULL || nCount < 2 || nLineWidth < 0 )
	{
		return nuFALSE;
	}
	else if( nLineWidth <= 1 )
	{
		return _3DPolyline(pPt, nCount);
	}
	nuroPOINT pt[2] = {0}, newPt[2] = {0};//, ptList[4];
	nuroRECT rt = {0};
	nuLONG nLen0 = 0, nLen1 = nLineWidth, dx = 0, dy = 0, dxy = 0;
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	pt[0] = pPt[0];
	if( pt[0].y > YCut )
	{
		bLastOut = nuTRUE;
	}
	else
	{
		bLastOut = nuFALSE;
		newPt[0] = pt[0];
		Screen2Dto3D(newPt[0].x, newPt[0].y);
		nLen0 = Get3DExtendXLen(nLineWidth, newPt[0].y);
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt[1] = pPt[i];
		if( pt[1].y > YCut )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut || !bNowOut )
		{
			if( bLastOut )
			{
				newPt[0].x = pt[1].x - (pt[1].y - YCut)*(pt[1].x - pt[0].x) / (pt[1].y - pt[0].y );//pt[1].y != pt[0].y, it is always sure!
				newPt[0].y = YCut;
				pt[0] = newPt[0];
				Screen2Dto3D(newPt[0].x, newPt[0].y);
				nLen0 = Get3DExtendXLen(nLineWidth, newPt[0].y);
			}
			newPt[1] = pt[1];
			if( bNowOut )
			{
				newPt[1].x = pt[0].x - (pt[0].y - YCut)*(pt[0].x - pt[1].x) / (pt[0].y - pt[1].y);//pt[1].y != pt[1].y
				newPt[1].y = YCut;
				pt[1] = newPt[1];
			}
			Screen2Dto3D(newPt[1].x, newPt[1].y);
			dx = newPt[1].x - newPt[0].x;
			dy = newPt[1].y - newPt[0].y;
			dxy = (nuLONG)nuSqrt(dx*dx+dy*dy);
			if( dxy != 0 )
			{
				nLen1 = Get3DExtendXLen(nLineWidth, newPt[1].y);
				//
				/*
				if( nLen1 == 0 && nLen0 == 0 )
				{
					Polyline(hDC, newPt, 2);
				}
				else
				*/
				{
					/*
					ptList[2].x = newPt[1].x + (dy*nLen1/(2*dxy));
					ptList[2].y = newPt[1].y - (dx*nLen1/(2*dxy));
					ptList[3].x = newPt[1].x - (dy*nLen1/(2*dxy));
					ptList[3].y = newPt[1].y + (dx*nLen1/(2*dxy));
					
					ptList[0].x = newPt[0].x - (dy*nLen0/(2*dxy));
					ptList[0].y = newPt[0].y + (dx*nLen0/(2*dxy));
					ptList[1].x = newPt[0].x + (dy*nLen0/(2*dxy));
					ptList[1].y = newPt[0].y - (dx*nLen0/(2*dxy));
					*/
					if( i > 1 && nLen0 > 3 )//Draw circle in connections
					{
						rt.left = newPt[0].x - (nLen0-1)/2;
						rt.top	= newPt[0].y - (nLen0-1)/2;
						rt.right	= newPt[0].x + (nLen0)/2;
						rt.bottom	= newPt[0].y + (nLen0)/2;
						g_pMtGdiApi->GdiEllipse(rt.left, rt.top, rt.right, rt.bottom);
					}
					////g_pMtGdiApi->GdiPolygon(ptList, 4);
					if( nLen1 < nLen0 )
					{
						g_pMtGdiApi->GdiDrawLine(newPt[1].x, newPt[1].y, newPt[0].x, newPt[0].y
							,nLen1, nLen0-nLen1, NURO_BMP_TYPE_COPY);
					}
					else
					{
						g_pMtGdiApi->GdiDrawLine(newPt[0].x, newPt[0].y, newPt[1].x, newPt[1].y
							,nLen0, nLen1-nLen0, NURO_BMP_TYPE_COPY);
					}
				}
				nLen0 = nLen1;
			}
		}
		newPt[0] = newPt[1];
		bLastOut = bNowOut;
		pt[0] = pPt[i];
	}
	return nuTRUE;
}

nuBOOL CViewTransf::_3DPolygon(nuroPOINT *pPt, nuLONG nCount)
{
	if( pPt == NULL || nCount < 3 )
	{
		return nuFALSE;
	}
	nuLONG nListCount = nCount*3/2 + 1;
	nuroPOINT *ptList = new nuroPOINT[nListCount];
	if( ptList == NULL )
	{
		return nuFALSE;
	}
	nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	nuBOOL bAddTwoPt = nuFALSE;
	pt2 = pPt[0];
	for(nuINT i = 1; i <= nCount; i++)
	{
		bAddTwoPt = nuFALSE;
		pt1 = pt2;
		if( i == nCount )
		{
			pt2 = pPt[0];
		}
		else
		{
			pt2 = pPt[i];
		}
		if( pt1.y > YCut && pt2.y > YCut )
		{
			continue;
		}
		else if( pt1.y > YCut && pt2.y <= YCut )
		{
			ptNew.y = YCut;
			ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
			pt1 = ptNew;
		}
		else if( pt1.y <= YCut && pt2.y > YCut )
		{
			ptNew.y = YCut;
			ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
			bAddTwoPt = nuTRUE;
		}
		ptList[nListCount] = pt1;
		Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
		nListCount++;
		if( bAddTwoPt )
		{
			ptList[nListCount] = ptNew;
			Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
			nListCount++;
		}
	}
	if( nListCount > 2 )
	{
		g_pMtGdiApi->GdiPolygon(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return nuTRUE;
}

nuBOOL CViewTransf::_3DBirdViewPolyline(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return nuFALSE;
	}
	nuLONG nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	if( pt1.y > YCut )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > YCut )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
				if( nListCount > 1 )
				{
					g_pMtGdiApi->Gdi3DPolyline(ptList, nListCount);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = YCut;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		g_pMtGdiApi->Gdi3DPolyline(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return nuTRUE;
}
