// FileMapDanyinZ.h: interface for the CFileMapDanyinZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMAPDANYINZ_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_)
#define AFX_FILEMAPDANYINZ_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_

#include "FileMapBaseZ.h"

class CFileMapDanyinZ : public CFileMapBaseZ
{
public:
	CFileMapDanyinZ();
	virtual ~CFileMapDanyinZ();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL  SetMapID(nuWORD nMapIdx);

	nuPVOID GetDanyinA(nuDWORD *pCount);
	nuPVOID GetDanyinB(nuDWORD *pCount); //Prosper Add 20130102, Load NameBuffer
	nuPVOID GetDanyinB2(nuDWORD *pCount);
	nuPVOID GetDanyinDE(nuWORD *pDCount, nuWORD *pECount);
	nuBOOL  GetOneDYDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
	
	//Prosper 2012.12 
	nuBOOL  GetOneDYData(PDYNODEFORUI pDyNode, nuDWORD nIdx);
	nuBOOL  GetOneDYData_GETNAME(PDYNODEFORUI pDyNode, nuDWORD nIdx);
	nuBOOL  GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n);//, nuBOOL bReSort);
	nuBOOL  GetOneDYData_WORD_KEY(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n,nuVOID* pPoiKeyName);
	nuBOOL  GetOneDYData_PAGE(PDYNODEFORUI pDyNode, nuDWORD nIdx, nuVOID* pPoiKeyName, nuINT nStartPos);
	nuINT   GetOneDYData_PAGE_START(nuVOID* pPoiKeyName);
	nuINT   GetWordIndx(nuVOID* pPoiKeyName); //prosper add 20130416

	DYWORD  *m_SearchRes; //[NURO_SEARCH_BUFFER_MAX_POI];	
	nuINT	m_nDYWordIdx;
	
	//prosper
	nuUINT  SetSearchMode(nuINT nType); //Set Load Mode ; -1 : Dy  ; 0 : Name
	nuBOOL  ReadData(nuUINT addr, nuVOID *p, nuUINT len);
	nuVOID  CloseNewFile();
	nuBOOL  OpenNewFile(nuWORD nMapIdx);
	//nuVOID	SortByStroke(nuDWORD *plist,nuINT nCount);
	//nuBOOL	NAME_Order(nuINT n);
	//nuINT	TransCode(nuINT code);

protected:
	nuVOID  CloseFile();
	nuBOOL  OpenFile();
protected:
	nuFILE*		m_pFile;
	nuFILE*		m_pFile2;
	nuWORD		m_nMapIdx;
	nuWORD		m_nDanYinAMemIdx;
	nuWORD		m_nDanYinBMemIdx;
	nuWORD		m_nDanYinB2MemIdx;
	nuWORD		m_nDanYinDEMemIdx;
	DANYINHEAD	m_dyHead;
	nuBOOL		m_bNeedReadB2;
	nuBOOL		m_bNeedReadDE;
	nuINT		m_nDYSearchMode;
};

#endif // !defined(AFX_FILEMAPDANYINZ_H__2D27E803_77A5_4DC3_8B56_A1A1A13F2ABF__INCLUDED_)
