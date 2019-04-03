// FileETG.h: interface for the CFileICT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEICT_H__INCLUDED_)
#define AFX_FILEICT_H__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

typedef struct TAG_SubHighwayCrossRoad
{
	nuLONG  BlockIdx;
	nuLONG  RoadIdx;
}SubHighwayCrossRoad, *pSubHighwayCrossRoad;

typedef struct TAG_HighwayCrossRoad
{
	nuLONG  HighwayID;
	nuLONG  HghwayCrossID;
	nuLONG  HighwayCrossPointX;
	nuLONG  HighwayCrossPointY;
	nuLONG  BKAddr;
}HighwayCrossRoad, *pHighwayCrossRoad;

typedef struct TAG_ICT_HEAD
{
	nuLONG HighCrossPointCount;
	nuLONG Start_BKAddr;
	nuBYTE Reserve[192];
}ICT_HEAD, *pICT_HEAD;

class CFileICT  
{
public:
	CFileICT();
	virtual ~CFileICT();

	nuBOOL	Initialize();
	nuVOID	Free();
	
	nuBOOL  SearchICT(nuLONG lBlockID, nuLONG lRoadIdx, NUROPOINT ptCar, NUROPOINT &ptEtagPoint, nuLONG &HighwayID, nuLONG &HghwayCrossID);
	nuBOOL  SearchSubRoadID(nuLONG lBlockID, nuLONG lRoadIdx, nuLONG Addr);
	nuFILE	*m_pFile;
	nuBOOL   m_bSendEtag;
	ICT_HEAD m_ICT_HEAD;
	HighwayCrossRoad **m_ppHighwayCrossRoad;
	NUROPOINT *m_pptICT;
};

#endif 