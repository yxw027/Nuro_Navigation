// ScreenNaviLineCol.cpp: implementation of the CScreenNaviLineCol class.
//
//////////////////////////////////////////////////////////////////////

#include "ScreenNaviLineCol.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenNaviLineCol::CScreenNaviLineCol()
{
	m_pNaviLineHead	= NULL;
	m_pNaviLineEnd	= NULL;
}

CScreenNaviLineCol::~CScreenNaviLineCol()
{
	DelList();
}

nuBOOL CScreenNaviLineCol::AddNode(nuLONG FTIdx, nuLONG RtBIdx, nuLONG SubIdx)
{
	PSCREENCOVERIDX ptmp = new SCREENCOVERIDX;
	if( ptmp == NULL )
	{
		return nuFALSE;
	}
	ptmp->FTIdx		= FTIdx;
	ptmp->RtBIdx	= RtBIdx;
	ptmp->SubIdx	= SubIdx;
	ptmp->pNext		= NULL;
	if( m_pNaviLineHead == NULL )
	{
		m_pNaviLineEnd = m_pNaviLineHead = ptmp;
	}
	else
	{
		m_pNaviLineEnd->pNext	= ptmp;
		m_pNaviLineEnd	= ptmp;
	}
	return nuTRUE;
}

nuVOID CScreenNaviLineCol::DelList()
{
	while( m_pNaviLineHead != NULL )
	{
		m_pNaviLineEnd = m_pNaviLineHead->pNext;
		delete m_pNaviLineHead;
		m_pNaviLineHead = m_pNaviLineEnd;
	}
}
