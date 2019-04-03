#if !defined(AFX_MAPFILEPIY_H__85BE286A_E8E9_4FED_9081_E599FF34CBE4__INCLUDED_)
#define AFX_MAPFILEPIY_H__85BE286A_E8E9_4FED_9081_E599FF34CBE4__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFilePiY : public CFileMapBaseZ  
{
public:
	CMapFilePiY();
	virtual ~CMapFilePiY();	
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();

	nuUINT	GetPoiNum();
	nuUINT	GetCityNum();
	nuUINT	GetBStartAddr();
	nuBOOL	GetCityInfo(nuWORD nCityID, SEARCH_PI_CITYINFO* pCityInfo);
	nuBOOL	GetPoiInfo_PPIdx(nuUINT nPPIdx, SEARCH_PI_POIINFO *pInfo);
	nuUINT	GetPoiInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen);
	nuBOOL	GetPoiInfo_CEX(nuUINT nCityStartAddr, nuWORD nRoadIdx, SEARCH_PI_POIINFO *pInfo);
private:
	nuUINT m_nPoiCount;
	nuUINT m_nCityCount;
	nuUINT m_nBStartAddr;
	nuCFile file;
};

#endif // !defined(AFX_MAPFILEPIY_H__85BE286A_E8E9_4FED_9081_E599FF34CBE4__INCLUDED_)
