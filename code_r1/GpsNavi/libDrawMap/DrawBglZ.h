// DrawBglZ.h: interface for the CDrawBglZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWBGLZ_H__E61DFC5B_016B_49F6_A527_EB74E6F8EA16__INCLUDED_)
#define AFX_DRAWBGLZ_H__E61DFC5B_016B_49F6_A527_EB74E6F8EA16__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"
#include "MapFileBglZ.h"

#define _USE_BGL_DRAW

#define _MAX_BGL_CLASS_COUNT			15

class CDrawBglZ 
{
public:
	typedef struct tagRDICONNODE
	{
		nuroRECT	rect;
		nuBYTE      byIconType;
		nuWORD      nIconIndex;
	}RDICONNODE, *PRDICONNODE;

	CDrawBglZ();
	virtual ~CDrawBglZ();
	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL	LoadData(const nuroRECT& rtMap, long nScale);
	nuVOID	FreeData();

	nuBOOL	DrawBgl();
	nuBOOL	ColBglData();

	nuBOOL  AddBglRdIconInfo(nuLONG sx, nuLONG sy, RNEXTEND rnExtend);
	nuBOOL  DrawBglRdIcon();
	nuBOOL  DrawBglRdIconTW();
	nuBOOL  ResetRdIconList();

protected:
	CMapFileBglZ	m_fileBgl;
	PBGLDATA		m_pBglData;
	nuWORD          m_nBglRdIconIdx;
	PRDICONNODE     m_pRdIconList;
};

#endif // !defined(AFX_DRAWBGLZ_H__E61DFC5B_016B_49F6_A527_EB74E6F8EA16__INCLUDED_)
