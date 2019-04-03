// FileCCD.h: interface for the CFileCCD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECCD_H__DBE323E3_AA7B_4A87_AECA_26605A27EAEB__INCLUDED_)
#define AFX_FILECCD_H__DBE323E3_AA7B_4A87_AECA_26605A27EAEB__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

class CFileCCD  
{
public:
	CFileCCD();
	virtual ~CFileCCD();

	nuBOOL  Initialize();
	nuVOID  Free();

	nuPVOID GetDataCCD(const nuroRECT& rtRange, nuWORD nMapIdx = 0);
	nuUINT	GetCCDName(nuWORD nMapIdx, nuLONG nNameAddr, nuWCHAR* pNameBuf, nuWORD nLen);

protected:
	nuINT   ColCCDBlockIDList(const nuroRECT& rtRange, nuDWORD* pCCDBlkIDList);
	nuDWORD nuCCDPointToBlockID(nuLONG x, nuLONG y);
	nuBOOL  RelCCDBlockUsed(PCCDBLOCK pCCDBlock);

public:
	CCDDATA	m_ccdData;
};

#endif // !defined(AFX_FILECCD_H__DBE323E3_AA7B_4A87_AECA_26605A27EAEB__INCLUDED_)
