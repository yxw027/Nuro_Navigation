// MapFileMdwZ.h: interface for the CMapFileMdwZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILEMDWZ_H__A6300FBC_275F_462E_BB29_0166CBA58C3A__INCLUDED_)
#define AFX_MAPFILEMDWZ_H__A6300FBC_275F_462E_BB29_0166CBA58C3A__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileMdwZ : public CFileMapBaseZ
{
public:
	
public:
	CMapFileMdwZ();
	virtual ~CMapFileMdwZ();

	nuBOOL  initialize();
	nuVOID  free();

public:
	nuPVOID LoadMdwData(const nuroRECT& rtMap);
	nuVOID  FreeMdwData();

protected:
	nuBOOL  ColMdwBlkNeedList(nuLONG *pBlkId, nuLONG *pBlkAddr, nuDWORD nBlkCount, const BLOCKIDARRAY& blkArray);
	nuVOID  RelMdwBlkNotUsed(nuBOOL bAll = nuFALSE);
	nuBOOL  ReadMdwBlk(nuFILE *pFile);

protected:
	BLOCKIDARRAY	m_BlockIdLoad;
	MDWDATA			m_mdwData;
};

#endif // !defined(AFX_MAPFILEMDWZ_H__A6300FBC_275F_462E_BB29_0166CBA58C3A__INCLUDED_)
