// FilePinyinZ.h: interface for the CFilePinyinZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPINYINZ_H__D15C638A_F728_4AC6_A174_D22D3B91B609__INCLUDED_)
#define AFX_FILEPINYINZ_H__D15C638A_F728_4AC6_A174_D22D3B91B609__INCLUDED_

#include "FileMapBaseZ.h"

class CFilePinyinZ : public CFileMapBaseZ
{
public:
	CFilePinyinZ();
	virtual ~CFilePinyinZ();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL	SetFileMode(nuUINT nMode);
	nuBOOL	SetMapIdx(nuUINT nMapIdx);

	nuPVOID GetPyALength(nuDWORD *pCount);
	nuPVOID GetPyBorCString(nuDWORD *pCount);
	nuPVOID GetPyDData(nuDWORD* pCount);

	nuBOOL GetPyOneData(PSH_ITEM_FORUI pItem, nuDWORD nIdx);
	nuBOOL GetPyOneDetail(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx);
	nuBOOL GetPyCityTown(PSH_CITYTOWN_FORUI pCityTown);

	nuDWORD GetCityCount() { return m_pdyHeader.nCityCount;}
protected:
	nuBOOL	OpenFile(const nuTCHAR* pTsFileEx);
	nuVOID	CloseFile();
protected:
	PDY_HEADER	m_pdyHeader;
	nuWORD		m_nPYAMemIdx;
	nuWORD		m_nPYBorCMemIdx;
	nuWORD		m_nPYDMemIdx;
	nuWORD		m_nMapIdx;
	nuUINT		m_nMode;
	nuFILE*		m_pFile;
};

#endif // !defined(AFX_FILEPINYINZ_H__D15C638A_F728_4AC6_A174_D22D3B91B609__INCLUDED_)
