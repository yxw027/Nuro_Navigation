// SearchSPDNumO.h: interface for the CSearchSPDNumO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHSPDNUMO_H__4081D9CE_0F1A_4ACC_9EC6_F002DED65F63__INCLUDED_)
#define AFX_SEARCHSPDNUMO_H__4081D9CE_0F1A_4ACC_9EC6_F002DED65F63__INCLUDED_

#include "SearchBaseZK.h"
#include "FileRn2O.h"

#define SPD_MAX_STR_COUNT						16

#define SPD_MAX_KEPT_RECORD						1000
#define SPD_MAX_CITY_RECORD						200
#define SPD_MAX_TOWN_RECORD						100

#define SPD_FOUNDDATA_MODE_DEFAULT				0x00//还没有搜索结果
#define SPD_FOUNDDATA_MODE_FIND_ALL				0x01//满足要求的所有结果都已找到
#define SPD_FOUNDDATA_MODE_FIND_PART			0x02//实际找了一部分

#define SPD_ID_INSERT_FAILURE			0x00
#define SPD_ID_INSERT_SAME				0x01
#define SPD_ID_INSERT_SUCCESS			0xFF

class CSearchSPDNumO  : public CSearchBaseZK
{
public:
	typedef struct tagSPDSEARCHDATA
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
		nuDWORD pIndexList[SPD_MAX_KEPT_RECORD];
		nuDWORD pCityIdxList[SPD_MAX_CITY_RECORD];
		nuDWORD pTownIdxList[SPD_MAX_TOWN_RECORD];
  }SPDSEARCHDATA, *PSPDSEARCHDATA;
public:
	CSearchSPDNumO();
	virtual ~CSearchSPDNumO();

	nuBOOL Initialize();
	nuVOID Free();

	nuUINT SetSHMode(nuUINT nMode);
	nuUINT SetSHRegion(nuBYTE nSetMode,nuDWORD nData);
	nuUINT SetSHString(const nuCHAR* pBuff,nuBYTE nBufLen);

	nuUINT GetSPDDataHead(PSH_HEADER_FORUI pShHeader);
	nuUINT GetSPDDataPage(PSH_PAGE_FORUI pPageData,nuDWORD nItemIdx);
	nuUINT GetSPDRoadDetail(PSH_ROAD_DETAIL pOneRoad, nuDWORD nIdx);
	nuUINT GetSPDDrNumCoor(nuDWORD nIdx,nuDWORD nDoorNum,nuroPOINT* point);
	nuUINT GetSPDCityTown(PSH_CITYTOWN_FORUI pCityTown);
	nuUINT GetSPDDataStroke(PSH_PAGE_FORUI pPageData, nuBYTE nStroke);
	nuUINT GetSPDDataList(nuWORD& nMax,nuVOID* pBuffer, nuUINT nMaxLen);
	nuBOOL NextWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort);
	nuUINT GetRoadSKB(void* pSKBBuf); //Prosper 2011.12.07
	nuUINT GetDYGetWordData(PDYWORDFORUI pDyData);
#ifndef VALUE_EMGRT
	nuBOOL GetDoorNumberInfo(SH_ROAD_DETAIL  &DoorinfoS);
	nuBOOL GetDoorNumberInfoData(nuVOID* pBuffer);
#endif

protected:
	nuBOOL   SearchSPD_City();
	nuBOOL   SearchSPD_Town();
	nuBOOL   SearchSPD_B(nuBYTE nShMode);
	nuBOOL   SearchSPD_C(nuBYTE nShMode);
	nuBOOL   SearchSPD_B_Nuro(nuBYTE nShMode);
	nuBOOL   SearchSPD_C_Nuro(nuBYTE nShMode);
	nuVOID   SearchSPD_B_STRCMP(nuWCHAR *pstr, nuBYTE nstrlen, nuDWORD idx, PSPDSEARCHDATA pShData);
	nuVOID   SearchSPD_Stroke(nuDWORD *pIdxList, nuINT nCount);
	nuVOID   SearchSPD_C_STRCMP(nuWCHAR *pstr,nuBYTE nstrlen, nuDWORD idx, PSPDSEARCHDATA pShData);
	nuBYTE   InsertCityTownID(PSH_CITYTOWN_FORUI pCityTown, nuWORD nID );
	nuBOOL	 NAME_Order(nuINT n);  //Prosper add 20130123,Name Sort
	nuINT	 TransCode(nuINT code);//Prosper add 20130123,Name Sort
	
protected:
	CFileRn2O    m_fileSpd;
	nuDWORD      m_pAStartIdx[SPD_MAX_STR_COUNT]; //记录当前记录的物理位置
	nuBYTE       m_nASIdxCount;
	nuBOOL         m_bReSearchSpd;
	nuBYTE       m_sName[SPD_MAX_STR_COUNT*sizeof(nuWCHAR)];
	nuWORD       m_nNameNum;
	nuWORD       m_nSHMemIdx;
	nuBOOL		 m_bKickNumber;

	NURO_SEARCH_SDN_LIST *n_pNORMALRES;
	nuINT		m_nDataIdx;
	nuBOOL		m_bNextName;
	nuINT	    m_nWordStartIdx[SH_POI_WORD_DATA_LIMIT];
	//nuDWORD	    m_nWordIdx[SH_POI_WORD_DATA_LIMIT];
	nuWORD		m_nSearchName[SH_NAME_FORUI_NUM];
	//nuINT		m_nTotal;
	
	typedef struct tag_Code				//Unicode 锣 Big5 ┮惠挡篶 
	{

		nuINT	nBig;
		nuINT	nUniCode;    

	}Trans_DATA;
	Trans_DATA *data;
	nuINT	nSortLimt;
	typedef struct tag_NAME				//Unicode 锣 Big5 ┮惠挡篶 
	{
		nuWCHAR name[32];
	
	}Trans_DATANAME,*PTrans_DATANAME;
	//Trans_DATANAME   *wsListName;
#ifndef VALUE_EMGRT
	SEARCH_PDN_NODE_DI *m_Doorinfo;
	nuINT m_ndoor;
#endif
	nuINT m_nWordEnd[SH_POI_WORD_DATA_LIMIT];
	nuWORD	m_nwsListNameMemIdx;
	
	//nuBOOL		 m_bKEYNAME;
};

#endif // !defined(AFX_SEARCHSPDNUMO_H__4081D9CE_0F1A_4ACC_9EC6_F002DED65F63__INCLUDED_)
