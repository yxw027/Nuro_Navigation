// DrawReal3DPicZ.h: interface for the CDrawReal3DPicZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWREAL3DPICZ_H__D4270F58_319A_4FF5_A7F6_C120F38094F5__INCLUDED_)
#define AFX_DRAWREAL3DPICZ_H__D4270F58_319A_4FF5_A7F6_C120F38094F5__INCLUDED_

#include "NuroDefine.h"

#define _REAL_3D_FILE	    nuTEXT("Media\\3dPic.NIC")

class CDrawReal3DPicZ  
{
public:
	typedef struct tagREAL3DFILEHEAD
	{
		nuWORD nW;
		nuWORD nH;
		nuUINT version;
		nuUINT nFileLen;
		nuUINT nIconNum;
	}REAL3DFILEHEAD, *PREAL3DFILEHEAD;
	typedef struct tagREAL3DPICNODE
	{
		nuDWORD  ID;
		nuWORD   nW;
		nuWORD   nH;
		nuBYTE   nRes[4];
		nuDWORD  nStartPos;
	}REAL3DPICNODE, *PREAL3DPICNODE;

	typedef struct tagREAL3DHEAD
	{
		REAL3DFILEHEAD fileHead;
		PREAL3DPICNODE p3DPicNodeList;
	}REAL3DHEAD, *PREAL3DHEAD;

public:
	CDrawReal3DPicZ();
	virtual ~CDrawReal3DPicZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL DrawReal3DPic(nuLONG nReal3DPicID, nuINT x = 0, nuINT y = 0);
	nuBOOL Draw3DRealPic(nuLONG nReal3DPicID, nuLONG nDwIdx, nuINT x = 0, nuINT y = 0);

protected:
	nuBOOL Read3DHeader(nuFILE* pFile);

protected:
	REAL3DHEAD	m_real3DHead;
	nuBOOL		m_bRead3DHead;
	nuLONG        m_nUseAlp;
};

#endif // !defined(AFX_DRAWREAL3DPICZ_H__D4270F58_319A_4FF5_A7F6_C120F38094F5__INCLUDED_)
