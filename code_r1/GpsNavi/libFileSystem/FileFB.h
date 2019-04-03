// FileFB.h: interface for the CFileFB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEFB_H__E177069F_66A3_4C46_B7BE_A5F2CDBB2829__INCLUDED_)
#define AFX_FILEFB_H__E177069F_66A3_4C46_B7BE_A5F2CDBB2829__INCLUDED_

#include "NuroDefine.h"

class CFileFB  
{
public:
	typedef struct tagBLOCKIDSEG
	{
		nuDWORD nFromID;
		nuDWORD nToID;
	}BLOCKIDSEG, FAR *PBLOCKIDSEG, FAR **PPBLOCKIDSEG;

	typedef struct tagDWBLOCKIDSEG
	{
		nuroRECT	rtBoundary;//top is less than Bottom.
		nuDWORD		nSegCount;
		PBLOCKIDSEG	pBlockIDSeg;
	}DWBLOCKIDSEG, FAR  *PDWBLOCKIDSEG, FAR **PPDWBLOCKIDSEG;

public:
	CFileFB();
	virtual ~CFileFB();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL FindBlockID(nuDWORD nBlockID, nuPWORD pDwIdx);
	nuBOOL PtInBoundary(nuroPOINT pt);

	nuBOOL BlockIDInMap(nuDWORD nBlockID, nuWORD nMapIdx);

protected:
	nuroRECT			m_rtBoundary;
	PDWBLOCKIDSEG		m_pDwBlockIDSeg;
	nuWORD				m_nDwCount;
};

#endif // !defined(AFX_FILEFB_H__E177069F_66A3_4C46_B7BE_A5F2CDBB2829__INCLUDED_)
