// FileRn2O.h: interface for the CFileRn2O class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERN2O_H__08B5154D_5CE0_42E2_BCF3_F607B28440F6__INCLUDED_)
#define AFX_FILERN2O_H__08B5154D_5CE0_42E2_BCF3_F607B28440F6__INCLUDED_

#include "FileMapBaseZ.h"

class CFileRn2O  : public CFileMapBaseZ
{
public:
	CFileRn2O();
	virtual ~CFileRn2O();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL    SetFileMode(nuUINT nMode);
	nuBOOL    SetMapIdx(nuUINT nMapIdx);

	nuPVOID GetRdALength(nuDWORD *pCount);
	nuPVOID GetRdBOrCString(nuDWORD *pCount);
	nuPVOID GetRdDData(nuDWORD *pCount);
	nuPVOID GetRdStroke(nuDWORD *pCount);

	nuBOOL    GetRdOneData(PSH_ITEM_FORUI pItem, nuDWORD nIdx);
	nuBOOL    GetRdOneData_GETNAME(PSH_ITEM_FORUI pItem, nuDWORD nIdx);
	nuBOOL    GetRdCityTown(PSH_CITYTOWN_FORUI pCityTown);
	nuBOOL    GetSPDRoadDetail(PSH_ROAD_DETAIL pOneRoad,nuDWORD nIdx);
	nuBOOL    GetSPDDrNumCoor(nuDWORD nIdx,nuDWORD nDoorNum,nuroPOINT* point);
	nuBOOL    GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n);

	nuDWORD GetCityCount();
protected:
	nuBOOL    OpenFile(const nuTCHAR* ptsFileEx);
	nuVOID    closeFile();

protected:
	RN2_HEADER m_rn2Header;
	nuWORD     m_nRnAMemIdx;
	nuWORD     m_nRnBorCMemIdx;
	nuWORD     m_nRnDMemIdx;
	nuWORD     m_nRnEMemIdx;
	nuWORD     m_nMapIdx;
	nuUINT     m_nMode;
	nuFILE*    m_pFile;
	//DYWORD     *m_SearchRes;	
	nuINT		m_nDYWordIdx;
	nuWORD		m_nNm_SearchResMemIdx;

};

#endif // !defined(AFX_FILERN2O_H__08B5154D_5CE0_42E2_BCF3_F607B28440F6__INCLUDED_)
