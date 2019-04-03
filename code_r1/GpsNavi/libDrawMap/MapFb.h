// MapFb.h: interface for the CMapFb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFB_H__8C169D4C_933C_4EDD_ADC3_CB3B32A3EE3D__INCLUDED_)
#define AFX_MAPFB_H__8C169D4C_933C_4EDD_ADC3_CB3B32A3EE3D__INCLUDED_

#include "NuroDefine.h"

//*** For for header of Map.bsd
typedef struct tagBSD_MAPFB_ID_NODE
{
	nuDWORD		nMapID;
	nuroRECT	rtMapBoundary;
}BSD_MAPFB_ID_NODE, *PBSD_MAPFB_ID_NODE;

typedef struct tagBSD_MAPFB_HEAD
{
	nuWORD				nMapCount;
	nuWORD				nReseve;
	PBSD_MAPFB_ID_NODE	pMapIDNodeList;
}BSD_MAPFB_HEAD, *PBSD_MAPFB_HEAD;
//*** For for body of Map.bsd

typedef struct tagBSD_BLOCKID_NODE
{
	nuDWORD				nFromID;
	nuDWORD				nToID;
}BSD_BLOCKID_NODE, *PBSD_BLOCKID_NODE;
typedef struct tagBSD_MAPFB_SCALE
{
	nuDWORD				nScaleName:8;
	nuDWORD				nBlockIDNodeCount:24;
	PBSD_BLOCKID_NODE	pBlockIDNodeListHead;
}BSD_MAPFB_SCALE, *PBSD_MAPFB_SCALE;
/* 由于每个 scale的大小不是固定的 所以不能构建成一个结构，要在计算时计算地址偏移
 */
typedef struct tagBSD_MAPFB_BODY
{
	nuDWORD				nScaleCount;
	nuBYTE*				pBsdMapFbScaleHead;
}BSD_MAPFB_BODY, *PBSD_MAPFB_BODY;

class CMapFb  
{
public:
	CMapFb();
	virtual ~CMapFb();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL	ReadMapFb();

protected:

	PBSD_MAPFB_HEAD ColMapFbHead(nuWORD idx);
	PBSD_MAPFB_BODY ColMapFbBody(nuWORD idx);

public:
	nuWORD		m_nIdxMapHead;
	nuWORD		m_nIdxMapBody;
};

#endif // !defined(AFX_MAPFB_H__8C169D4C_933C_4EDD_ADC3_CB3B32A3EE3D__INCLUDED_)
