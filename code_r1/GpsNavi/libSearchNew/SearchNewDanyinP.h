// SearchNewDanyinP.h: interface for the CSearchNewDanyinP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SearchNewDanyinP_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_)
#define AFX_SearchNewDanyinP_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_

#include "SearchBaseZK.h"
#include "FileMapNewDanyinP.h"
#include "MapFileSKB.h"

#define NDY_CANDYWORD_LIMIT                     300
#define DY_MAX_WCHAR_NUM						20
#define NewDY_MAX_WCHAR_NUM						24

#define DY_MAX_ADDRESS_NUM						(DY_A_NAME_LEN/sizeof(nuWCHAR))

#define NDY_MAX_KEPT_RECORD_LIMIT				2000
#define NDY_MAX_KEPT_RECORD						3500 //prosper add , 1000->2500
#define NDY_MAX_CITY_RECORD						200
#define NDY_MAX_TOWN_RECORD						100

#define DY_FOUNDDATA_MODE_DEFAULT				0x00//还没有搜索结果
#define DY_FOUNDDATA_MODE_FIND_ALL				0x01//满足要求的所有结果都已找到
#define DY_FOUNDDATA_MODE_FIND_PART				0x02//实际找了一部分
class CSearchNewDanyinP : public CSearchBaseZK
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
    nuDWORD pIndexList[NDY_MAX_KEPT_RECORD];
	nuDWORD pCityIdxList[NDY_MAX_CITY_RECORD];
    nuDWORD pTownIdxList[NDY_MAX_TOWN_RECORD];
	nuDWORD pLenList[NDY_MAX_KEPT_RECORD];
  }DYSEARCHDATA, *PDYSEARCHDATA;
  typedef struct tagNDY_NAMELEN
  {
	  nuDWORD Len;
  }NDY_NAMELEN, *PNDY_NAMELEN;

public:
  CSearchNewDanyinP();
  virtual ~CSearchNewDanyinP();
  nuBOOL Initialize();
  nuVOID Free();
  nuUINT  SetSearchRegion(nuBYTE nSetMode, nuDWORD nData);
  nuUINT	SetSearchDanyin(nuWCHAR *pWsSH, nuBYTE nWsNum);
  nuUINT	SetSearchDanyin2(nuWCHAR *pWsSH, nuBYTE nWsNum); //20130130 Prosper add , more faster
  nuUINT	GetDYDataHead(PDYCOMMONFORUI pDyCommonForUI);
  nuUINT  GetDYDataPage(PNDYDETAILFORUI pDyDataForUI, nuINT &nCount);
  nuUINT  GetDYDataStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
  nuUINT	GetDyDataDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
  nuUINT  GetDyCityTownName(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
  
  nuBOOL  VoicePoiSetMap(nuINT nIdx);
  nuBOOL  VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx);
  nuBOOL  VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
  nuBOOL  VoicePoiEnd();
  
  //add by Prosper 20121216
  nuUINT  GetDYGetWordData(PDYWORDFORUI pDyData); 
  nuBOOL  SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging); 
  nuUINT  GetDYDataPage_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
  nuUINT  SetSearchKEY(nuWCHAR *pKeyWord, nuBYTE bIsFromBeging,nuBOOL);
  nuUINT  SetSearchMode(nuINT nType);

  nuINT	  GetPoiTotal(); 
  nuUINT  GetDYALLData(nuPVOID buffer,nuINT &nCount);
  nuUINT  GetDYData_COUNT(); //ZENRIN

  nuINT GetTownNameAll(nuINT nID,nuVOID *pBuffer, nuUINT nMaxLen);
  nuINT GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
 
  nuBOOL	GetNEXTAll(nuVOID* pKEYBuf, nuUINT& nCount) ;
  nuWCHAR	m_wsNEXT_WORD_Data[NEXT_WORD_MAXCOUNT];
  nuINT		nNEXT_WORD_COUNT ;

  nuVOID    StopSKBSearch();
  nuINT     GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount);
  nuINT    GetSKBInBuffer(nuCHAR* pBuffer, nuINT nMaxLen);
  nuBOOL   JudgeSKB(nuCHAR* strDes, nuCHAR* strSrc, nuINT nDesLen, nuINT nSrcLen, nuWORD townID);
  nuUINT SetAreaCenterPos(nuroPOINT pos);

//  nuBYTE  *nALLDanyinLen;
//  PDY_D_NODE *m_IDInfo;
protected:
  nuBOOL SearchDY_A(nuBYTE nShMode);
  nuBOOL SearchDY_B(nuBYTE nShMode);
  nuVOID SearchDY_A_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData);
  nuVOID SearchDY_A_WSCMP2(nuWCHAR *pWsDY, nuDWORD idx, nuDWORD nStartlen,PDYSEARCHDATA pShData);
  nuBOOL SearchDY_City();
  nuBOOL SearchDY_Town();
  nuVOID SearchDY_Stroke(nuDWORD *pIdxList, nuINT nCount);
  
  nuVOID SearchDY_MapStart();
  nuVOID SearchDY_B_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData);
  
  nuINT	 TransCode(nuINT code);
  nuBOOL NAME_Order(nuINT n);

  

protected:
  CFileMapNewDanyinP	m_fileNewDY;
  nuWORD	m_nShDataMemIdx;
  nuWORD	m_nNDYMemLenIdx;
  nuWORD	m_nNDYIDMemLenIdx;
//  nuWORD	m_nwsListNameIdx;
  //[0]:存第1个单音的起始的A区Index；[1]:存第1、2个单音的起始的A区Index；[2]:存第1、2、3个单音的起始A区Index；
  nuDWORD	m_pAStartIdx[DY_MAX_ADDRESS_NUM];
  nuBYTE	m_nASIdxCount;
  nuINT	    m_nWordStartIdx[NDY_MAX_KEPT_RECORD];
  nuINT	    m_nCandyWordStartIdx[NDY_MAX_KEPT_RECORD];
  //nuINT	    m_nIdx[2000];
  nuINT	    m_nIdx[NDY_MAX_KEPT_RECORD]; //
  nuINT	    m_nWord[NDY_MAX_KEPT_RECORD]; //prosper add for faster search

  nuBOOL	bFindFirst;
  nuINT		m_nIndex;
  nuBOOL    m_bNew;

  nuBYTE	m_nNameNum;
  nuWCHAR	m_wsName[NewDY_MAX_WCHAR_NUM];
  nuBOOL	m_bReSearchStroke;
  nuINT     m_nDYSearchMode; //?

  nuWCHAR  m_nSearchPoiKeyName[NewDY_MAX_WCHAR_NUM];
  nuWCHAR  m_nSearchPoiDanyinName[NewDY_MAX_WCHAR_NUM];
 
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
	
	}Trans_DATANAME;
	//Trans_DATANAME   *wsListName;
	nuBOOL m_bSort;
	nuBOOL m_bSortCity;
	//#ifdef ZENRIN 
		Trans_DATANAME   *wsTempName;
		Trans_DATANAME	 *m_wsNameList;
		nuINT			 *nSameCity;
		nuINT			 *nSameTown;
		nuINT			 nSameNameIdx;
	//#endif
		nuDWORD nTime;
		nuDWORD g_time;
		nuDWORD* nNDYLEN;
		nuINT m_nTotalLen;
		nuINT m_nCityID;
		nuINT m_nTownID;

        nuWORD  m_SKBCityIDT;
	nuWORD  m_SKBTownIDT;
	nuUINT  m_SKBLoadLen;
	nuUINT  m_SKBResT;
	SEARCH_SKB_CITYINFO infoT;
	nuCHAR    m_SKBKeyStrT[128];
	nuBYTE  m_SKBBuffer[128];
	nuLONG    m_SKBMapIDT;
	nuWORD m_nSKBROADMMIdx;
	nuWORD m_nSKBROADMMIdx2;
	nuWORD m_nSKBPOIMMIdx;
	nuWORD m_nSKBPOIMMIdx2;
	nuBOOL bFindALL;
	nuINT    m_nCityTownID[128];
	nuBOOL m_bSetAreaCenterPos;
	NUROPOINT m_TypeCenterPos;
	nuBOOL m_bFUZZY;
	// Added by damon 20100202
	#define LONEFFECTCONST						1.113
	#define LATEFFECTCONST						1.109
	#define EFFECTBASE							1.000
	#define DEGBASE								100000

	double getLatEffect(nuroPOINT pt)
	{
		return (nuCos(pt.y/DEGBASE)*((pt.y*10/DEGBASE)%10) + nuCos(pt.y/DEGBASE+1)*(10-((pt.y*10/DEGBASE)%10)))/10;
	}
	
};

#endif // !defined(AFX_SearchNewDanyinP_H__38804BE2_48AC_4230_8EA2_8C733260283C__INCLUDED_)
