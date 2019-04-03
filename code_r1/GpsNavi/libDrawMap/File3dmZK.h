// File3dmZK.h: interface for the CFile3dmZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE3DMZK_H__D0853757_1656_4214_9998_A2C6897F0FE0__INCLUDED_)
#define AFX_FILE3DMZK_H__D0853757_1656_4214_9998_A2C6897F0FE0__INCLUDED_

#include "NuroDefine.h"

class CFile3dmZK  
{
public:
	CFile3dmZK();
	virtual ~CFile3dmZK();

	nuBOOL	Read3dmCoor(nuTCHAR* ptzName);
	nuVOID	Delete3dmCoor();
	nuBOOL	Read3drName(nuTCHAR* pTz3dm, nuDWORD nIdx3dr, nuCHAR* ps3dr, nuWORD nBuflen);
	
public:
	nuDWORD		m_nTotalCount;
	nuroPOINT*	m_pPtCoor;
};

#endif // !defined(AFX_FILE3DMZK_H__D0853757_1656_4214_9998_A2C6897F0FE0__INCLUDED_)
