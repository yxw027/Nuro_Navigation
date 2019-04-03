// FilePt.h: interface for the CFilePt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPT_H__78F35C51_CD9E_4C15_9EBC_ED89CAA1EA43__INCLUDED_)
#define AFX_FILEPT_H__78F35C51_CD9E_4C15_9EBC_ED89CAA1EA43__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

class CFilePt  
{
public:
	CFilePt();
	virtual ~CFilePt();

	nuBOOL Initialize();
	nuVOID Free();

	const nuPVOID GetPtDataAddr(nuLONG nDWIdx, nuLONG* pCount);
	const nuPVOID GetPtNodeByType(nuLONG nDWIdx, nuBYTE nType1, nuBYTE nByte2);
	nuBOOL GetInfoBySourceType(nuLONG nSTypeID1, nuLONG nSTypeID2, nuLONG* pTypeID1, nuLONG* pTypeID2, nuLONG* pDisPlay, nuLONG* pIcon);
	nuBOOL GetInfoByNuroType(nuLONG nTypeID1, nuLONG nTypeID2, nuLONG* pSTypeID1, nuLONG* pSTypeID2, nuLONG* pDisPlay, nuLONG* pIcon);

	nuBOOL SavePtFile(nuINT DwIdx);
	nuBOOL SavePtFileCoverForDFT(nuINT DwIdx);

protected:
	nuBOOL ReadPtFile(nuLONG nDWIdx);

public:
	PPTNODE		m_pPtData;
	nuLONG		m_nPtCount;
	nuLONG		m_nDWIdx;
};

#endif // !defined(AFX_FILEPT_H__78F35C51_CD9E_4C15_9EBC_ED89CAA1EA43__INCLUDED_)
