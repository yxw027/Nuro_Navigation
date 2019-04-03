// MapFileSeaZ.h: interface for the CMapFileSeaZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILESEAZ_H__2BD6BEDD_8A76_4C89_B443_2E3246ADE832__INCLUDED_)
#define AFX_MAPFILESEAZ_H__2BD6BEDD_8A76_4C89_B443_2E3246ADE832__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileSeaZ : public CFileMapBaseZ
{
public:
	CMapFileSeaZ();
	virtual ~CMapFileSeaZ();

	nuBOOL  Initialize();
	nuVOID  Free();

	nuPVOID LoadSeaData(nuroRECT rtMap, nuLONG nScaleValue);
	nuVOID	FreeSeaData();

protected:
	nuBOOL  ColScaleIndex(nuLONG nScaleValue);
	nuBOOL  ColSeaNeedData(nuFILE *pFile);
	nuVOID  RelSeaBlkUnused(nuBOOL bAll = nuTRUE);
	nuBOOL  ReadSeaData(nuFILE *pFile);

protected:
	nuLONG			m_nScaleValue;
	nuLONG			m_nScaleIdx;
	SEAFILEHEAD		m_fileHead;
	PSEASCALEDATA	m_pScaleData;
	SEADATA			m_seaData;
	BLOCKIDARRAY	m_BlockIdLoad;
};

#endif // !defined(AFX_MAPFILESEAZ_H__2BD6BEDD_8A76_4C89_B443_2E3246ADE832__INCLUDED_)
