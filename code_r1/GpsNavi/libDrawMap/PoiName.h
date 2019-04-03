// PoiName.h: interface for the CPoiName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINAME_H__56E94E13_0554_459A_BF27_55CCBA53EED6__INCLUDED_)
#define AFX_POINAME_H__56E94E13_0554_459A_BF27_55CCBA53EED6__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroCommonStructs.h"
#include "DrawSpecialPoiZ.h"
  
#define POI_SHOW_NAME			    0x01
#define POI_SHOW_ICON			    0x02

#define ADD_RES_NEW				    0x01
#define ADD_RES_FULL			    0x02
#define ADD_RES_FAIL			    0x00

#define _POI_MAX_COUNTS_            1000
#define _POI_PRIORITY_FILE_NAME_    nuTEXT("map\\poipriority.txt")

class CPoiName 
{
public:
	typedef struct tagPOINODE
	{
		nuBYTE  	nType1;
		nuBYTE  	nType2;
		nuBYTE  	nNameLen;
		nuBYTE  	nMode;//bit0: if Drawname; bit1: if Draw Icon;
		nuBYTE  	nPoiType; //DWPOIDATE.nReserve, when nReserve[0] !=0 ,this poi as special.
		nuBYTE  	nIconType;
		nuWORD      nPriority; //Poi Priority
		nuWORD  	nDwIdx;
		nuDWORD 	nNameAddr;
		nuDWORD 	nIcon;
		nuroPOINT	ptPos;
		nuroRECT	rtIcon;
		nuroRECT	rtNAME;
	}POINODE, *PPOINODE;

	typedef struct tagPOI_PRIORITY
	{
		nuBYTE	byTypeID1;
		nuBYTE	byTypeID2;
		nuWORD  nPriority;
	}POI_PRIORITY, *PPOI_PRIORITY;

	typedef struct tagPOI_PRIORITY_INFO
	{
		nuWORD           nPoiCount;
		nuWORD           nPriorityCount;
		PPOI_PRIORITY    pPoiPriority;
	}POI_PRIORITY_INFO, *PPOI_PRIORITY_INFO;

public:
	CPoiName();
	virtual ~CPoiName();

	nuBOOL Initialize(class CRoadName* pRoadName, PPOISETTING	pPoiSetting, nuBYTE nMapType = MAP_DEFAULT );
	nuVOID Free();

	nuINT AddPoiShowName(nuBYTE nType1, nuBYTE nType2, nuBYTE nNameLen, nuWORD nDwIdx, 
		                 nuDWORD nNameAddr, nuBYTE nPoiType, nuWORD cityID, nuDWORD nIcon, nuroPOINT ptPos);//
	nuINT AddPoiNode(nuBYTE nType1, nuBYTE nType2, nuBYTE nNameLen, nuWORD nDwIdx, 
			         nuDWORD nNameAddr, nuBYTE nPoiType, nuWORD cityID, nuDWORD nIcon, nuroPOINT ptPos);

	//--- Added by XiaoQin @ 2012.10.09 ---
	nuINT AddPoiNodeEx( nuBYTE nType1, nuBYTE nType2, nuBYTE nNameLen, nuWORD nDwIdx, nuDWORD nNameAddr, 
		                nuBYTE nPoiType, nuWORD cityID, nuDWORD nIcon, nuroPOINT ptPos, nuINT nPriority );
	//-------------------------------------
	nuBOOL DrawPoi();

	// --- Added by Dengxu @ 2013 01 02
	nuBOOL DrawPoiIcon_DX();

	nuVOID ResetPoiList();

protected:
	nuBOOL TypeIsFull(nuBYTE nType1, nuBYTE nType2);
	nuBOOL CheckPoiRect(nuroRECT, const NUROPOINT& pt, nuBYTE* pByMode);
	nuBOOL IsPoiOverlaped(nuLONG x, nuLONG y);
	nuBOOL IsPoiOverlaped(POINODE* pNode);

	//--- Added by XiaoQin @ 2012.10.09 ---
	nuBOOL SortPoiNode();
	nuBOOL SelectPoiNode();
	nuBOOL ReadPoiPriorityFile();
	nuINT  GetPriorityByType(nuBYTE byType1, nuBYTE byType2);
	nuUINT ReadScaleCount(nuBYTE *pData, nuUINT nDataLen);
	nuUINT DataAnalyse(nuBYTE *pData, nuUINT nDataLen, nuWORD nScaleIdx);
	nuBOOL IsLineEnd(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i);
	nuBOOL IsOtherType2(nuBYTE byType1, nuBYTE byType2, nuWORD nScaleIdx, nuWORD nPOICount);
	nuBOOL PoiIconIsFull(nuDWORD nIconID);
	nuVOID ReadPTNode();
	//-------------------------------------

protected:
	PPOINODE	        m_pPoiList;
	nuWORD		        m_nPoiCount;
	PPOINODE            m_pformerList;
	nuWORD              m_pformerCount;
	nuWORD              m_nMaxPoiCount;
	nuWORD		        m_nPoiIndex;
	nuBYTE		        m_nMapType;
	class CRoadName*	m_pRoadName;
	PPOISETTING	        m_pPoiSetting;
	CDrawSpecialPoiZ	m_drawSpPoi;
	//
	nuWORD              m_nScaleCount;
	PPOI_PRIORITY_INFO  m_pPriorityInfo;
	nuBOOL              m_bPriority;
	nuLONG              m_nMaxNumofSameIcon;
	nuLONG              m_nPoiTotalTypeCount;
	nuWORD              m_nPoiType1Count;
	nuWORD*             m_pPoiType2CountList;
};

#endif // !defined(AFX_POINAME_H__56E94E13_0554_459A_BF27_55CCBA53EED6__INCLUDED_)

