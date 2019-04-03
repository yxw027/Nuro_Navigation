// FileMapNewDanyinP.h: interface for the CFileMapNewDanyinP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FileMapNewDanyinP_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_)
#define AFX_FileMapNewDanyinP_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroMapFormat.h"

class CFileMapNewDanyinP : public CFileMapBaseZ
{
public:
	typedef struct tagNAME
	{
		nuWCHAR	wsALLName[NDY_CITYTOWN_NAME_LEN/2];
	}NDYNAME, *PNDYNAME;

	typedef struct tagNDY_CITYDATA
	{
		nuDWORD CityRoadAddr;
		nuDWORD CityPOIAddr;
		nuDWORD nCityDataCount;
	}NDY_CITYDATA, *PNDY_CITYDATA;


	typedef struct tagNDY_NAMELEN
	{
		nuDWORD Len;
	}NDY_NAMELEN, *PNDY_NAMELEN;

	
	typedef struct tagNDYROADINFO
	{
		nuINT nTownID;
		nuWCHAR	wsRoadName[DY_NAME_FORUI_CITYORTOWN_NUM];
	}NDYROADINFO, *PNDYROADINFO;

	CFileMapNewDanyinP();
	virtual ~CFileMapNewDanyinP();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL  SetMapID(nuWORD nMapIdx);
	nuBOOL  LoadData();
	nuBOOL  SetID(nuWORD nCityIdx,nuWORD nTownIdx);
	nuINT   GetIDInfo(nuVOID *data);
	nuBOOL  CheckID(nuDWORD nIdx,nuINT nCityIdx,nuINT nTownIdx);
	
	nuPVOID GetDanyinA(nuDWORD *pCount);
	nuPVOID GetDanyinB(nuDWORD *pCount); //Prosper Add 20130102, Load NameBuffer
	nuPVOID GetDanyinB2(nuDWORD *pCount);
	nuPVOID GetDanyinDE(nuWORD *pDCount, nuWORD *pECount);
	nuBOOL  GetOneDYDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
	
	//Prosper 2012.12 
	nuBOOL  GetOneDYData(PDYNODEFORUI pDyNode, nuDWORD nIdx);
	nuBOOL  GetOneDYData_GETNAME(PDYNODEFORUI pDyNode, nuDWORD nIdx ,nuDWORD nStartLen);
	nuBOOL  GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n,nuVOID* pPoiKeyName,nuINT );//, nuBOOL bReSort);
	nuBOOL  GetOneDYData_PAGE(PDYNODEFORUI pDyNode, nuDWORD nIdx, nuVOID* pPoiKeyName, nuINT nStartPos,nuINT nBeforeLen2);
	nuUINT  GetDYALLData(nuPVOID buffer,nuINT &nCount);
	nuBOOL  CheckRoadSame(nuWORD m_nBeforeLen,nuINT nLen,nuDWORD nIdx);
	nuBOOL  CheckRoadSame(nuWCHAR *wsRoadName,nuINT nLen,nuDWORD nIdx);
	//DYWORD  *m_SearchRes; //[NURO_SEARCH_BUFFER_MAX_POI];	
	nuINT	m_nDYWordIdx;
	
	//prosper
	nuUINT  SetSearchMode(nuINT nType); //Set Load Mode ; -1 : Dy  ; 0 : Name
	nuBOOL  ReadData(nuUINT addr, nuVOID *p, nuUINT len);
	nuVOID  CloseNewFile();
	nuBOOL  OpenNewFile(nuWORD nMapIdx);

	nuINT	GetDanyinLen(nuINT nIdx , nuINT nStartLen,nuBOOL bRes);
	nuINT   GetDanyinLen2(nuVOID *p);
	nuINT  GetTownNameAll(nuINT nCityID, nuVOID *pBuffer, nuUINT nMaxLen);
	nuINT   GetWordIndx(nuVOID* pPoiKeyName); //prosper add 20130416
	nuINT GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
	nuINT GetTownIDAll();

	nuBOOL    StartSKBSearch(nuUINT nType);
    nuVOID    StopSKBSearch();

	nuBOOL	ColCityRealCount(NDY_CITYDATA	*ndyCityDataALL);
    nuINT   GetTownIDInfo(nuVOID *data);
protected:
	nuVOID  CloseFile();
	nuBOOL  OpenFile();
protected:
	nuDWORD*     m_pNameLen;
	nuFILE*		m_pFile;
	nuFILE*		m_pFile2;
	nuFILE*		m_pFileCB;
	nuWORD		m_nMapIdx;
	nuWORD		m_nNewDanYinLenMemIdx;
	nuWORD		m_nNewDanYinCityMemIdx;
	nuWORD		m_nNewDanYinDanYinMemIdx;
	nuWORD		m_nNewDanYinNameMemIdx;
	nuWORD		m_nNewDanYinCityTownMemIdx;
	nuWORD		m_SearchResIdx;


	nuWORD		m_nNewDanYinAMemIdx;
	nuWORD		m_nNewDanYinBMemIdx;
	nuWORD		m_nNewDanYinB2MemIdx;
	nuWORD		m_nNewDanYinDEMemIdx;
	NEWDANYINHEAD	m_ndyHead;
	NDY_CITYDATA	m_ndyCityData;
	nuDWORD		m_nBerforeCityLen;
	nuBOOL		m_bNeedReadB2;
	nuBOOL		m_bNeedReadDE;
	nuINT		m_nDYSearchMode;
	nuWCHAR		m_KeyName[NewDY_B_NAME_LEN/2];
	nuBOOL		m_bCity; //chice city
	nuWORD		m_nCityIdx;
	nuWORD		m_nTownIdx;

	nuDWORD		m_nBerforeCityWORDLen;

	NDYNAME	*wsALLCityName;
	NDYNAME	*wsALLTownName;
	
	SEARCH_CB_HEADER CBHeader;	
	nuINT n_CityTownID[128];

	NDYROADINFO *m_NDYROADINFO;
	nuINT		m_nRoadCount;
	nuWORD		m_nNewDanYinROADINFOIdx;
	nuBOOL		m_bFindPOI;
	nuBOOL		m_bFUZZY;
};

#endif // !defined(AFX_FileMapNewDanyinP_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_)

