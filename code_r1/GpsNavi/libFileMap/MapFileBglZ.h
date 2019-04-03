// MapFileBglZ.h: interface for the CMapFileBglZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILEBGLZ_H__4CA95C1A_6EEF_4E18_A788_CD129E07F838__INCLUDED_)
#define AFX_MAPFILEBGLZ_H__4CA95C1A_6EEF_4E18_A788_CD129E07F838__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

#define _BGL_50K_NAME		nuTEXT("Map\\Major\\China50k.bgl")
#define _BGL_5k_NAME		nuTEXT("Map\\Major\\China5k.bgl")


class CMapFileBglZ : public CFileMapBaseZ
{
public:
	CMapFileBglZ();
	virtual ~CMapFileBglZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuPVOID LoadBglData(nuroRECT rtMap, nuLONG nScaleValue, nuLONG nBGLStartScale = 5000);
	nuVOID	FreeBglData();

protected:
	nuBOOL ColBglNeedData(nuLONG *pBlkId, nuLONG *pBlkAddr);
	nuVOID RelBglBlkUnused(nuBOOL bAll = nuTRUE);
	nuBOOL ReadBglData(nuFILE *pFile);

protected:
	BGLFILEHEAD		m_fileHead;
	nuLONG			m_nScaleValue;
	BLOCKIDARRAY	m_BlockIdLoad;
	BGLDATA			m_bglData;
	nuTCHAR*		m_pTcsFile;
};

#endif // !defined(AFX_MAPFILEBGLZ_H__4CA95C1A_6EEF_4E18_A788_CD129E07F838__INCLUDED_)
