// MapConfig.h: interface for the CMapConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPCONFIG_H__12D550B0_4444_4879_AA44_F32A41E49816__INCLUDED_)
#define AFX_MAPCONFIG_H__12D550B0_4444_4879_AA44_F32A41E49816__INCLUDED_

#include "NuroDefine.h"
#include "NuroCommonStructs.h"

class CMapConfig  
{
public:
	//Structs in MapConfig.dat file
	typedef struct tagNAVISTRU
	{
		nuBYTE	nRouteLineWidth;
		nuBYTE	nRouteLineColorR;
		nuBYTE	nRouteLineColorG;
		nuBYTE	nRouteLineColorB;
		nuBYTE	nRouteArrowType;
		nuBYTE	nRouteArrowColorR;
		nuBYTE	nRouteArrowColorG;
		nuBYTE	nRouteArrowColorB;
		nuBYTE	nRouteArrowRimColorR;
		nuBYTE	nRouteArrowRimColorG;
		nuBYTE	nRouteArrowRimColorB;
		nuBYTE	nBgGroundColorR;
		nuBYTE	nBgGroundColorG;
		nuBYTE	nBgGroundColorB;
		nuBYTE	nReserve[2];
	}NAVISTRU, *PNAVISTRU, **PPNAVISTRU;

	typedef struct tagCOMMONSETSTRU
	{
		nuBYTE	nMaxRoadNameCount;
		nuBYTE	nMaxPOINameCount;
		nuBYTE	nScaleLayerCount;
		nuBYTE	nStartScale;
		nuBYTE	nPOIType1Count;
		nuBYTE	nRoadTypeCount;
		nuBYTE	nBgLineTypeCount;
		nuBYTE	nBgAreaTypeCount;
		nuWORD	nFixedRoadDis;
		nuWORD	nMapDisplayCount;
	}COMMONSETSTRU, *PCOMMONSETSTRU, *PPCOMMONSETSTRU;

	typedef struct tagSCALEHEAD
	{
		nuDWORD	nScale;
		nuBYTE	nSupportBirdView;
		nuBYTE	nRoadSampling;
		nuBYTE	nBgLineSampling;
		nuBYTE	nBgAreaSampling;
	}SCALEHEAD, *PSCALEHEAD, **PPSCALEHEAD;

	typedef struct tagPOISETSTRU
	{
		nuBYTE	nShowIcon;
		nuBYTE	nShowName;
		nuBYTE	nSupportUserFinger;
		nuBYTE	nFontBold;
		nuBYTE	nFontBgTransparent;
		nuBYTE	nFontSize;
		nuBYTE	nFontColorR;
		nuBYTE	nFontColorG;
		nuBYTE	nFontColorB;
		nuBYTE	nFontBgColorR;
		nuBYTE	nFontBgColorG;
		nuBYTE	nFontBgColorB;
	}POISETSTRU, *PPOISETSTRU, **PPPOISETSTRU;

	typedef struct tagLINESETSTRU
	{
		nuBYTE	nShowCenterLine;
		nuBYTE	nShowRimLine;
		nuBYTE	nShowName;
		nuBYTE	nSupportUserFinger;
		nuBYTE	nCenterLineWidth;
		nuBYTE	nCenterLineType;
		nuBYTE	nCenterLineColorR;
		nuBYTE	nCenterLineColorG;
		nuBYTE	nCenterLineColorB;
		nuBYTE	nRimLineWidth;
		nuBYTE	nRimLineType;
		nuBYTE	nRimLineColorR;
		nuBYTE	nRimLineColorG;
		nuBYTE	nRimLineColorB;
		nuBYTE	nFontBold;
		nuBYTE	nFontBgTransparent;
		nuBYTE	nFontSize;
		nuBYTE	nFontColorR;
		nuBYTE	nFontColorG;
		nuBYTE	nFontColorB;
		nuBYTE	nFontBgColorR;
		nuBYTE	nFontBgColorG;
		nuBYTE	nFontBgColorB;
		nuBYTE	nReserve;
	}LINESETSTRU, *PLINESETSTRU, **PPLINESETSTRU;

	typedef struct tagAREASETSTRU
	{
		nuBYTE	nShowArea;
		nuBYTE	nShowName;
		nuBYTE	nSupportUserFinger;
		nuBYTE	nCoverColorR;
		nuBYTE	nCoverColorG;
		nuBYTE	nCoberColorB;
		nuBYTE	nFontBlod;
		nuBYTE	nFontBgTransparent;
		nuBYTE	nFontSize;
		nuBYTE	nFontColorR;
		nuBYTE	nFontColorG;
		nuBYTE	nFontColorB;
		nuBYTE	nFontBgColorR;
		nuBYTE	nFontBgColorG;
		nuBYTE	nFontBgColorB;
		nuBYTE	nReserve;
	}AREASETSTRU, *PAREASETSTRU, *PPAREASETSTRU;

public:
	CMapConfig();
	virtual ~CMapConfig();

	nuBOOL Initialize(nuBYTE bNightDay, nuBYTE nScaleIndx = 0);
	nuVOID Free();

	nuBOOL ReadMapSetting(nuBYTE nScaleIdx, nuBYTE bNightDay, nuBOOL bReadAll = nuFALSE);
	CPMAPCONFIG GetConstMapConfig() { return m_pMapConfig; }

public:
	PMAPCONFIG		m_pMapConfig;

protected:
	nuBYTE			m_nScaleIdx;
	nuBYTE			m_bNightDay;
};

#endif // !defined(AFX_MAPCONFIG_H__12D550B0_4444_4879_AA44_F32A41E49816__INCLUDED_)
