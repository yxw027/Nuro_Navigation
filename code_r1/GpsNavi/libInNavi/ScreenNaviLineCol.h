// ScreenNaviLineCol.h: interface for the CScreenNaviLineCol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENNAVILINECOL_H__646A1E71_9B91_483E_B1A3_3FFDDF8A624B__INCLUDED_)
#define AFX_SCREENNAVILINECOL_H__646A1E71_9B91_483E_B1A3_3FFDDF8A624B__INCLUDED_

#include "NuroDefine.h"

typedef struct tagSCREENCOVERIDX
{
	nuLONG	FTIdx;
	nuLONG	RtBIdx;
	nuDWORD	SubIdx;
	tagSCREENCOVERIDX* pNext;
}SCREENCOVERIDX, *PSCREENCOVERIDX;

class CScreenNaviLineCol  
{
public:
	CScreenNaviLineCol();
	virtual ~CScreenNaviLineCol();

	nuBOOL  AddNode(nuLONG FTIdx, nuLONG RtBIdx, nuLONG SubIdx);
	nuVOID  DelList();

public:
	PSCREENCOVERIDX		m_pNaviLineHead;
	PSCREENCOVERIDX		m_pNaviLineEnd;
};

#endif // !defined(AFX_SCREENNAVILINECOL_H__646A1E71_9B91_483E_B1A3_3FFDDF8A624B__INCLUDED_)
