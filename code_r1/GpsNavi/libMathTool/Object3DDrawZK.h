// Object3DDrawZK.h: interface for the CObject3DDrawZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT3DDRAWZK_H__E1EECBE1_4DAB_4B91_835C_2E0FB240217A__INCLUDED_)
#define AFX_OBJECT3DDRAWZK_H__E1EECBE1_4DAB_4B91_835C_2E0FB240217A__INCLUDED_

#include "Object3DCtrlZK.h"
#include "NuroModuleApiStruct.h"

class CObject3DDrawZK  
{
public:
	CObject3DDrawZK();
	virtual ~CObject3DDrawZK();
	
	nuBOOL  Initialize(PGDIAPI pGdiApi);
	nuUINT	Poly3DLine(nuroPOINT *pPt, nuLONG nCount);//线条没有粗细变化
	nuUINT	Poly3DLine(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth);//线条有粗细变化
	nuUINT	Poly3DGon(nuroPOINT *pPt, nuLONG nCount);
	nuUINT	Dash3DLine(nuroPOINT *pPt, nuLONG nCount, nuINT nDashLine);

public:
	CObject3DCtrlZK		m_obj3DCtrl;
	
	PGDIAPI	m_pGdiApi;
};

#endif // !defined(AFX_OBJECT3DDRAWZK_H__E1EECBE1_4DAB_4B91_835C_2E0FB240217A__INCLUDED_)
