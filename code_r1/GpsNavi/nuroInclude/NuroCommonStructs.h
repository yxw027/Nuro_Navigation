#ifndef __NURO_COMMON_STRUCTS_20080121
#define __NURO_COMMON_STRUCTS_20080121

/* Map Config structs and define. */
typedef struct tagCOMMONSETTING
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
}COMMONSETTING, *PCOMMONSETTING, *PPCOMMONSETTING;
typedef struct tagNAVILINESETTING
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
}NAVILINESETTING, *PNAVILINESETTING, **PPNAVILINESETTING;
typedef struct tagSCALETITLE
{
	nuDWORD	nScale;
	nuBYTE	nSupportBirdView;
	nuBYTE	nRoadSampling;
	nuBYTE	nBgLineSampling;
	nuBYTE	nBgAreaSampling;
}SCALETITLE, *PSCALETITLE, **PPSCALETITLE;
typedef struct tagPOISETTING
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
}POISETTING, *PPOISETTING, **PPPOISETTING;
typedef const POISETTING*	PCPOISETTING;
typedef struct tagROADSETTING
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
	nuBYTE	nFontHeight;
	nuBYTE	nFontColorR;
	nuBYTE	nFontColorG;
	nuBYTE	nFontColorB;//Text color
	nuBYTE	nFontRimColorR;
	nuBYTE	nFontRimColorG;
	nuBYTE	nFontRimColorB;
	nuBYTE	nFontWidth;//---------------------------Below new 
	nuBYTE	nBannerColorR;
	nuBYTE	nBannerColorG;
	nuBYTE	nBannerColorB;
	nuBYTE	nBannerRimColorR;
	nuBYTE	nBannerRimColorG;
	nuBYTE	nBannerRimColorB;
	nuBYTE	nBannerRimWidth;
	nuBYTE	nReserve;
	nuBYTE	nCenterLineWidth3D; // for 3d road
	nuBYTE	nRimLineWidth3D;
	nuBYTE  nReserve2;
	nuBYTE  nReserve3;
}ROADSETTING, *PROADSETTING, **PPROADSETTING;
typedef const ROADSETTING*	PCROADSETTING;
typedef struct tagBGLINESETTING
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
	nuBYTE	nFontHeight;
	nuBYTE	nFontColorR;
	nuBYTE	nFontColorG;
	nuBYTE	nFontColorB;
	nuBYTE	nFontBgColorR;
	nuBYTE	nFontBgColorG;
	nuBYTE	nFontBgColorB;
	nuBYTE	nFontWidth;
}BGLINESETTING, *PBGLINESETTING, *PPBGLINESETTING;
typedef const BGLINESETTING*	PCBGLINESETTING;
typedef struct tagBGAREASETTING
{
	nuBYTE	nShowArea;
	nuBYTE	nShowName;
	nuBYTE	nSupportUserFinger;
	nuBYTE	nCoverColorR;
	nuBYTE	nCoverColorG;
	nuBYTE	nCoverColorB;
	nuBYTE	nFontBlod;
	nuBYTE	nFontBgTransparent;
	nuBYTE	nFontHeight;
	nuBYTE	nFontColorR;
	nuBYTE	nFontColorG;
	nuBYTE	nFontColorB;
	nuBYTE	nFontBgColorR;
	nuBYTE	nFontBgColorG;
	nuBYTE	nFontBgColorB;
	nuBYTE	nFontWidth;
}BGAREASETTING, *PBGAREASETTING, **PPBGAREASETTING;
typedef const BGAREASETTING*	PCBGAREASETTING;
typedef struct tagSCALESETTING
{
	SCALETITLE			scaleTitle;
	PPOISETTING			pPoiSetting;
	PROADSETTING		pRoadSetting;
	PBGLINESETTING		pBgLineSetting;
	PBGAREASETTING		pBgAreaSetting;
}SCALESETTING, *PSCALESETTING, *PPSCALESETTING;
typedef struct tagMAPCONFIG
{
	COMMONSETTING		commonSetting;
	NAVILINESETTING		naviLineSetting;
	SCALESETTING		zoomSetting;
	SCALESETTING		scaleSetting;
	nuLONG*				pScaleList;
}MAPCONFIG, *PMAPCONFIG, **PPMAPCONFIG;
typedef const MAPCONFIG*	CPMAPCONFIG;

#endif
