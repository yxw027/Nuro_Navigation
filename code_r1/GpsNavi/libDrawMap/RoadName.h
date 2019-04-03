// RoadName.h: interface for the CRoadName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROADNAME_H__BC6F5138_7057_4294_BB64_A78E248E0CEB__INCLUDED_)
#define AFX_ROADNAME_H__BC6F5138_7057_4294_BB64_A78E248E0CEB__INCLUDED_

#include "NuroClasses.h"
#include "NuroCommonStructs.h"
#include "NuroConstDefined.h"

	typedef struct tagRDNAMENODE
	{
		nuBYTE   	rdNameLen;
		nuBYTE	    rdType;
		nuBYTE	    rdAlign;
		nuBYTE	    rdReady;//
		nuWORD	    rdDwIdx;
		nuWORD	    rdDis;
		nuDWORD  	rdNameAddr;
		nuroPOINT	ptStart;
		nuroPOINT	ptEnd;
		nuroRECT	rect;
		nuBYTE		nFw;
		nuBYTE		nFh;
	nuroRECT	mapRect;
	nuroPOINT	ptMapStart;
	}RDNAMENODE, *PRDNAMENODE, *LPRDNAMENODE;

class CRoadName  
{
public:
	typedef struct tagRoad_Icon_Node
	{
		nuBYTE      byIconType;
		nuWORD      nIconIndex;
	}Road_Icon_Node;

public:
	CRoadName();
	virtual ~CRoadName();

	nuBOOL Initialize(nuWORD nNameCount, PROADSETTING	pRoadSetting, nuBYTE nMapType = MAP_DEFAULT);
	nuVOID Free();

	nuBOOL ResetRdNameList();
	nuBOOL AddRoadName(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
					   nuroPOINT *pPt, nuINT idx1, nuINT idx2);

	nuBOOL DrawRoadName();
	nuBOOL CheckRect(nuroRECT *lpRect);

protected:
	nuBOOL AddRoadNameCH(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
					     nuroPOINT *pPt, nuINT idx1, nuINT idx2);
	nuBOOL AddRoadName3D(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
					     nuroPOINT *pPt, nuINT idx1, nuINT idx2);
	nuBOOL AddRoadNameCN(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
					     nuroPOINT *pPt, nuINT idx1, nuINT idx2);
	nuBOOL AddRoadNameEN(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
					     nuroPOINT *pPt, nuINT idx1, nuINT idx2);
	nuBOOL DrawRoadNameCH();
	nuBOOL DrawRoadName3D();
	nuBOOL DrawRoadNameCN();
	nuBOOL DrawRoadNameCN_DX();
	nuBOOL DrawRoadNameEN();
	nuINT  CheckName(nuWORD nDwIdx, nuDWORD nNameAddr, nuLONG* pDis);
	nuBOOL CheckRectCN(nuroRECT *lpRect, nuBYTE nAlign, nuBYTE nNameLen, nuBYTE nFtSize);
	nuroRECT* ColNameRectCN(nuroRECT *lpRect, nuBYTE nAlign, nuBYTE nNameNum, nuBYTE nFtSize);
	//Louis new function
	nuBOOL DrawRoadNameCHEx();

	//------ Add by XiaoQin @ 2012.10.26 ------
	nuBOOL DrawRoadIcon(nuLONG sx, nuLONG sy, nuBYTE byIconType, nuWCHAR* wsRdNum);
	nuBOOL DrawRoadIconTW(nuLONG sx, nuLONG sy, nuBYTE byIconType, nuWCHAR* wsRdNum);

	//-------add by daniel 20121211--------------------------------//
	nuBYTE DrawRoadNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount);
	//-------add by daniel 20121211--------------------------------//

protected:
	LPRDNAMENODE	m_pRdNameList;
	nuWORD			m_nRdNameCount;
	nuWORD			m_nRdNameIndex;
	PROADSETTING	m_pRoadSetting;
	nuBYTE			m_nMapType;
	nuBYTE			m_nMapDataType;
	nuBYTE			m_nAddRoadFontSize;
	//
	nuWORD          m_nRdIconIdx;
	Road_Icon_Node* m_pRdIconList;
	nuBYTE          m_byPrevScaleIdx;
	nuBYTE          m_b3DMode;
	nuBYTE			m_nMapDirectMode;
	nuSHORT         m_nOldMapAngle;
};



#endif // !defined(AFX_ROADNAME_H__BC6F5138_7057_4294_BB64_A78E248E0CEB__INCLUDED_)
