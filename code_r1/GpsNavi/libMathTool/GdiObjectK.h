// GdiObjectK.h: interface for the CGdiObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIOBJECT_H__BA3AB5F3_C078_4AC7_A95C_308B528DBB75__INCLUDED_)
#define AFX_GDIOBJECT_H__BA3AB5F3_C078_4AC7_A95C_308B528DBB75__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "ViewTransf.h"

class CGdiObjectK  
{
public:
	CGdiObjectK();
	virtual ~CGdiObjectK();

	nuBOOL Initialize();
	nuVOID Free();

	nuINT  SelectObject(nuBYTE nDrawType, nuBYTE nShowMode, const nuVOID* lpSet, nuLONG *pOut);

	nuBOOL DrawObject(nuINT nObjType, PNUROPOINT pPt, nuINT nCount, nuLONG nExtend);

	nuBOOL SetViewParam();

	nuBOOL DrawBgSquare(nuINT nObjType);

	nuVOID Bmp2Dto3D(nuLONG &x, nuLONG &y, nuBYTE nMapType);
	nuVOID Bmp3Dto2D(nuLONG &x, nuLONG &y, nuBYTE nMapType);
protected:
	nuBOOL Poly3Dline(PNUROPOINT pPt, nuINT nCount, nuLONG nLineWidth, class CViewTransf *p3DTransf);
	nuBOOL DrawNaviTrigon(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjType);
	nuBOOL DrawNavi2DArrowLine(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType);
	nuBOOL DrawNavi3DArrowLine(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType, nuBOOL bRim = nuTRUE);

	nuBOOL Draw3DIndicateArrow(PNUROPOINT pPt, nuINT nCount, nuWORD nWidth, nuINT nObjeType);
public:
	CViewTransf		m_viewTransf;
	CViewTransf		m_zoomTransf;
	nuBYTE			m_nRedArrowLintW;
	nuBYTE			m_nGoNavi;
	nuLONG			m_nArrowColor;
// 	nuLONG			m_nArrowColorR;
// 	nuLONG			m_nArrowColorG;
// 	nuLONG			m_nArrowColorB;
// 	nuLONG			m_nArrowRimColorR;
// 	nuLONG			m_nArrowRimColorG;
// 	nuLONG			m_nArrowRimColorB;

	nuLONG			m_nTwoArrowsDis;
	nuLONG			m_lArrowListWidth;
	
};

#endif // !defined(AFX_GDIOBJECT_H__BA3AB5F3_C078_4AC7_A95C_308B528DBB75__INCLUDED_)
