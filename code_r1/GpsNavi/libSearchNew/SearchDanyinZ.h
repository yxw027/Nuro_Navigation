// SearchDanyinZ.h: interface for the CSearchDanyinZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHDANYINZ_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_)
#define AFX_SEARCHDANYINZ_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_

#include "SearchBaseZK.h"
#include "FileMapDanyinZ.h"

#define DY_MAX_WCHAR_NUM						20

#define DY_MAX_ADDRESS_NUM						(DY_A_NAME_LEN/sizeof(nuWCHAR))

#define DY_MAX_KEPT_RECORD_LIMIT				2000
#define DY_MAX_KEPT_RECORD						2500 //prosper add , 1000->2500
#define DY_MAX_CITY_RECORD						200
#define DY_MAX_TOWN_RECORD						100

#define DY_FOUNDDATA_MODE_DEFAULT				0x00//还没有搜索结果
#define DY_FOUNDDATA_MODE_FIND_ALL				0x01//满足要求的所有结果都已找到
#define DY_FOUNDDATA_MODE_FIND_PART				0x02//实际找了一部分
class CSearchDanyinZ : public CSearchBaseZK
{
public:
  typedef struct tagDYSEARCHDATA
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
    nuDWORD pIndexList[DY_MAX_KEPT_RECORD];
    nuDWORD pCityIdxList[DY_MAX_CITY_RECORD];
    nuDWORD pTownIdxList[DY_MAX_TOWN_RECORD];
  }DYSEARCHDATA, *PDYSEARCHDATA;
public:
  CSearchDanyinZ();
  virtual ~CSearchDanyinZ();
  nuBOOL Initialize();
  nuVOID Free();
  nuUINT  SetSearchRegion(nuBYTE nSetMode, nuDWORD nData);
  nuUINT	SetSearchDanyin(nuWCHAR *pWsSH, nuBYTE nWsNum);
  nuUINT	SetSearchDanyin2(nuWCHAR *pWsSH, nuBYTE nWsNum); //20130130 Prosper add , more faster
  nuUINT	SetSearchDanyin3(nuWCHAR *pWsSH, nuBYTE nWsNum); //
  nuUINT	GetDYDataHead(PDYCOMMONFORUI pDyCommonForUI);
  nuUINT  GetDYDataPage(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
  nuUINT  GetDYDataStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
  nuUINT	GetDyDataDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
  nuUINT  GetDyCityTownName(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
  
  nuBOOL  VoicePoiSetMap(nuINT nIdx);
  nuBOOL  VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx);
  nuBOOL  VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
  nuBOOL  VoicePoiEnd();
  
  //add by Prosper 20121216
  nuUINT  GetDYGetWordData(PDYWORDFORUI pDyData); 
  nuBOOL  SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging); 
  nuUINT  GetDYDataPage_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
  nuUINT  SetSearchKEY(nuWCHAR *pKeyWord, nuBYTE bIsFromBeging);
  nuUINT  SetSearchMode(nuINT nType);

  nuINT	  GetPoiTotal(); 

  nuUINT GetDYData_COUNT(); //ZENRIN

protected:
  nuBOOL SearchDY_A(nuBYTE nShMode);
  nuBOOL SearchDY_B(nuBYTE nShMode);
  nuVOID SearchDY_A_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData);
  nuVOID SearchDY_A_WSCMP2(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData);
  nuBOOL SearchDY_City();
  nuBOOL SearchDY_Town();
  nuVOID SearchDY_Stroke(nuDWORD *pIdxList, nuINT nCount);
  
  nuVOID SearchDY_MapStart();
  nuVOID SearchDY_B_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData);
  
  nuINT	 TransCode(nuINT code);
  nuBOOL NAME_Order(nuINT n);

protected:
  CFileMapDanyinZ	m_fileDY;
  nuWORD	m_nShDataMemIdx;
  //[0]:存第1个单音的起始的A区Index；[1]:存第1、2个单音的起始的A区Index；[2]:存第1、2、3个单音的起始A区Index；
  nuDWORD	m_pAStartIdx[DY_MAX_ADDRESS_NUM];
  nuBYTE	m_nASIdxCount;
  nuINT	    m_nWordStartIdx[DY_MAX_KEPT_RECORD];
  nuINT	    m_nIdx[200]; //
  nuINT	    m_nWord[2500]; //prosper add for faster search

  nuBOOL	bFindFirst;
  nuINT		m_nIndex;
  nuBOOL    m_bNew;

  nuBYTE	m_nNameNum;
  nuWCHAR	m_wsName[DY_MAX_WCHAR_NUM];
  nuBOOL	m_bReSearchStroke;
  nuINT     m_nDYSearchMode; //?

  nuWCHAR  m_nSearchPoiKeyName[DY_MAX_WCHAR_NUM];
  nuWCHAR  m_nSearchPoiDanyinName[DY_MAX_WCHAR_NUM];
 
  nuBOOL m_bKickNumber;
  typedef struct tag_Code				//Unicode 锣 Big5 ┮惠挡篶 
  {

	  nuINT	nBig;
	  nuINT	nUniCode;    

  }Trans_DATA;
  Trans_DATA *data;
  nuINT		m_nWordIdx;
  nuINT		nSortLimt;
  nuBOOL m_bNameSort;
  typedef struct tag_NAME				//Unicode 锣 Big5 ┮惠挡篶 
	{
		nuWCHAR name[40];
	
	}Trans_DATANAME,*PTrans_DATANAME;;
	//Trans_DATANAME   *wsListName;
	nuBOOL m_bSort;
	nuBOOL m_bSortCity;

	//nuINT m_Idx[2500];

	#ifdef ZENRIN 
		Trans_DATANAME   *wsTempName;
		nuINT			 *nSameCity;
		nuINT			 *nSameTown;
		nuINT			 nSameNameIdx;
	#endif

		nuWORD	m_nwsListNameMemIdx;
};

#endif // !defined(AFX_SEARCHDANYINZ_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_)
