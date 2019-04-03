// SearchPYz.h: interface for the CSearchPYz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHPYZ_H__657C4B51_BFA3_4963_86C8_BA2D18AE17E3__INCLUDED_)
#define AFX_SEARCHPYZ_H__657C4B51_BFA3_4963_86C8_BA2D18AE17E3__INCLUDED_

#include "SearchBaseZK.h"
#include "FilePinyinZ.h"

#define PY_MAX_STR_COUNT						16

#define PY_MAX_KEPT_RECORD						1000
#define PY_MAX_CITY_RECORD						200
#define PY_MAX_TOWN_RECORD						100

#define PY_FOUNDDATA_MODE_DEFAULT				0x00//还没有搜索结果
#define PY_FOUNDDATA_MODE_FIND_ALL				0x01//满足要求的所有结果都已找到
#define PY_FOUNDDATA_MODE_FIND_PART				0x02//实际找了一部分


#define PY_ID_INSERT_FAILURE			0x00
#define PY_ID_INSERT_SAME				0x01
#define PY_ID_INSERT_SUCCESS			0xFF
class CSearchPYz : public CSearchBaseZK
{
public:
	typedef struct tagPYSEARCHDATA
	{
		nuBYTE	nFoundDataMode;
		nuBYTE	nPageMode;
		nuBYTE  nMinStroke;
		nuBYTE	nMaxStroke;
		nuWORD  nCityID;//-1表示全找
		nuWORD  nTownID;//-1表示全找
		nuDWORD	nTotalFound;//All for 全省
		nuDWORD nCountOfCity;//某个City下的个数
		nuDWORD	nCountOfTown;//某个Town下的个数
		nuDWORD nKeptCount;
		nuDWORD pIndexList[PY_MAX_KEPT_RECORD];
		nuDWORD pCityIdxList[PY_MAX_CITY_RECORD];
		nuDWORD pTownIdxList[PY_MAX_TOWN_RECORD];
  }PYSEARCHDATA, *PPYSEARCHDATA;
public:
	CSearchPYz();
	virtual ~CSearchPYz();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuUINT SetSHMode(nuUINT nMode);
	nuUINT SetSHRegion(nuBYTE nSetMode, nuDWORD nData);
	nuUINT SetSHString(const nuCHAR *pBuff, nuBYTE nBufLen);

	nuUINT GetPyDataHead(PSH_HEADER_FORUI pShHeader);
	nuUINT GetPyDataPage(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx);
	nuUINT GetPyDataPinyin(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin);
	nuUINT GetPyOneDetail(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx);
	nuUINT GetPyCityTown(PSH_CITYTOWN_FORUI pCityTown);
protected:
	nuBOOL SearchPY_B(nuBYTE nShMode);
	nuBOOL SearchPY_C(nuBYTE nShMode);
	nuVOID SearchPY_B_STRCMP(nuCHAR *pstr, nuBYTE nstrlen, nuDWORD idx, PPYSEARCHDATA pShData);

	nuBOOL SearchPY_City();
	nuBOOL SearchPY_Town();

	nuVOID SearchPY_Pinyin(nuDWORD *pIdxList, nuINT nCount);

	nuBYTE InsertCityTownID(PSH_CITYTOWN_FORUI pCityTown, nuWORD nID);
protected:
	CFilePinyinZ	m_filePy;
	nuDWORD	m_pAStartIdx[PY_MAX_STR_COUNT];
	nuBYTE	m_nASIdxCount;
	nuBOOL	m_bReSearchPinyin;
	nuBYTE	m_sName[PY_MAX_STR_COUNT*sizeof(nuWCHAR)];
	nuWORD	m_nNameNum;
	nuWORD	m_nSHMemIdx;

};

#endif // !defined(AFX_SEARCHPYZ_H__657C4B51_BFA3_4963_86C8_BA2D18AE17E3__INCLUDED_)
