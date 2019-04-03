// NaviLineCollect.h: interface for the CNaviLineCollect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVILINECOLLECT_H__476CD959_C113_4762_94DE_CB80C9C7C724__INCLUDED_)
#define AFX_NAVILINECOLLECT_H__476CD959_C113_4762_94DE_CB80C9C7C724__INCLUDED_

#include "NuroDefine.h"
#include "nuRouteData.h"

typedef struct tagNAVISUBNODE
{
	nuDWORD			nSubIdx;
	tagNAVISUBNODE	*pNext;
}NAVISUBNODE, *PNAVISUBNODE;

typedef struct tagNRBCNODE
{
	PNAVIRTBLOCKCLASS	pNRBC;
	PNAVISUBNODE		pNaviSubList;
	tagNRBCNODE*		pNext;
}NRBCNODE, *PNRBCNODE;

typedef struct tagNFTCNODE
{
	PNAVIFROMTOCLASS	pNFTC;
	PNRBCNODE			pNRBCList;
	tagNFTCNODE*		pNext;
}NFTCNODE, *PNFTCNODE;

class CNaviLineCollect  
{
public:
	CNaviLineCollect();
	virtual ~CNaviLineCollect();
public:
	PNFTCNODE	m_pNFTCList;

};

#endif // !defined(AFX_NAVILINECOLLECT_H__476CD959_C113_4762_94DE_CB80C9C7C724__INCLUDED_)
