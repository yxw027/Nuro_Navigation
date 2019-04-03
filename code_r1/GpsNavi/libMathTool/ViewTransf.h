// ViewTransf.h: interface for the CViewTransf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWTRANSF_H__C0928A16_7BC6_49BE_BD34_4997D9E633ED__INCLUDED_)
#define AFX_VIEWTRANSF_H__C0928A16_7BC6_49BE_BD34_4997D9E633ED__INCLUDED_

#include "NuroDefine.h"

class CViewTransf  
{
#define X_STOC(x)				(x -= nSCx) 
#define Y_STOC(y)				(y = nSCy - y)
#define X_CTOS(x)				(x += nSCx)
#define Y_CTOS(y)				(y = nSCy - y)
public:
	CViewTransf();
	virtual ~CViewTransf();

	nuBOOL Initialize();
	nuVOID Free();

	nuLONG SetParam( nuLONG nSW, nuLONG nSH, nuDOUBLE fDScale = 1.0, 
				     nuDOUBLE fYTscale = 0.75, nuDOUBLE fYBscale = 0.15, nuLONG kxn = 3,
				     nuLONG kxd = 1 );
	nuVOID Screen2Dto3D(nuLONG &x, nuLONG &y);
	nuVOID Screen3Dto2D(nuLONG &x, nuLONG &y);

	nuBOOL _3DPolyline(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth);
	nuBOOL _3DPolyline(nuroPOINT *pPt, nuLONG nCount);
	nuBOOL _3DDashline(nuroPOINT *pPt, nuLONG nCount, nuINT nDashLine);
	nuBOOL _3DPolygon(nuroPOINT *pPt, nuLONG nCount);
	nuBOOL _3DBirdViewPolyline(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth);
	nuLONG Get3DThick(nuLONG nThick, nuLONG y);
protected:
	nuVOID ScreenToDescartes(nuLONG &x, nuLONG &y)
	{
		x = x + nSCx;
		y = nSCy - y;
	}
	nuVOID DescartesToScreen(nuLONG &x, nuLONG &y)
	{
		x = x - nSCx;
		y = nSCy - y;
	}
	nuVOID Descartes2Dto3D(nuLONG &x, nuLONG &y)
	{
		y = D - A/(y + K);
		x = KxN * x * (D - y) / (D * KxD);
	}
	nuVOID Descartes3Dto2D(nuLONG &x, nuLONG &y)
	{
		x = x * D * KxD / (D - y ) / KxN;
		y = A / (D - y) - K;
	}
	nuLONG Get3DExtendXLen(nuLONG xLen, nuLONG y);
public:
	nuLONG	nSCx, nSCy;//the base point of the transformation. screen axis
	nuLONG	D, A, K;//y' = D - A/(y+k); formula of transformation.
	nuLONG	Height;//the height of the Screen;
	nuLONG	YCut;//the point's y value above YCut will been cut off. this is screen axis coordinate.
	nuLONG	KxN, KxD;

	nuLONG	m_nLineTop;//For Get3DExtendXLen();

};

#endif // !defined(AFX_VIEWTRANSF_H__C0928A16_7BC6_49BE_BD34_4997D9E633ED__INCLUDED_)
