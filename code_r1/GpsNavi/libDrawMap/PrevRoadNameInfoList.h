#ifndef __PREVROADNAMEINFOLIST_H__
#define __PREVROADNAMEINFOLIST_H__
#include "NuroDefine.h"
#include "libDrawMap.h"
#include "RoadName.h"
#define MAX_COUNT 50
typedef struct _RoadDTL
{
	nuBYTE   bInUse;
	nuBYTE   bExist;
	RDNAMENODE data;
	nuWCHAR text[10];
}RoadDTL;

class CPrevRoadNameInfoList
{
public:
	CPrevRoadNameInfoList(nuVOID);
	~CPrevRoadNameInfoList(nuVOID);

	int    FindFirstUnused(nuVOID);
	nuBOOL ResetExistTag  (nuVOID);
	nuBOOL ResetAll       (nuVOID);
	nuBOOL RemoveNoneExist(nuVOID);
	nuBOOL RectModify     (nuDWORD dwRoadNameAddr, NURORECT& roadNameRect, nuBYTE rdAlign);
	nuBOOL RectModify     (RDNAMENODE& node);

	nuBOOL ConvertRect_MapToBmp(NURORECT& rect);
	nuBOOL ConvertRect_BmpToMap(NURORECT& rect);

	nuBOOL ConvertRect_BmpToMap(NURORECT& rect1, const NURORECT& rect2);
	nuBOOL ConvertRect_MapToBmp(NURORECT& rect1, const NURORECT& rect2);

	nuBOOL BmpToMap(nuLONG bmpX, nuLONG bmpY, nuLONG& mapX, nuLONG& mapY);
	nuBOOL MapToBmp(nuLONG mapX, nuLONG mapY, nuLONG& bmpX, nuLONG& bmpY);
	nuBOOL GetRoadName(RDNAMENODE& node, nuWCHAR *wsRoadName);

	RoadDTL m_PrevNodes[MAX_COUNT];
};

extern CPrevRoadNameInfoList g_PrevRoadName;
#endif // File PrevRoadNameInfoList.h 
