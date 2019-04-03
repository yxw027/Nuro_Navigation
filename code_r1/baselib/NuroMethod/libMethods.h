#ifndef __NURO_METHODS_H_20091001
#define __NURO_METHODS_H_20091001

#include "../NuroDefine.h"
//#include "../NuroConstDefined.h"

//NURO STRUCT-------------------
#define RTALIGNTYPE_00			0
#define RTALIGNTYPE_01			1
#define RTALIGNTYPE_02			2
#define RTALIGNTYPE_03			3
#define RTALIGNTYPE_04			4
#define RTALIGNTYPE_05			5
#define RTALIGNTYPE_06			6
#define RTALIGNTYPE_07			7
#define RTALIGNTYPE_08			8
#define RTALIGNTYPE_09			9
#define RTALIGNTYPE_10			10
#define RTALIGNTYPE_11			11
#define RTALIGNTYPE_12			12
#define RTALIGNTYPE_13			13

#ifndef NURO_LONG_MIN
#define NURO_LONG_MIN	(-2147483647L-1)
#endif
#ifndef NURO_LONG_MAX
#define NURO_LONG_MAX	2147483647L
#endif

#define NURO_MAX_PTINPOLYGON_LIM 256

#define NURO_ISSPACE(x)		(((x)==0x20)?1:0)
#define NURO_ISDIGIT(x)		(((x)>=0x30 && (x)<=0x39)?1:0)
#define NURO_ISALPHA(x)		((((x)>=0x41 && (x)<=0x5A)||((x)>=0x61 && (x)<=0x7A))?1:0)	
#define NURO_ISUPPER(x)		(((x)>=0x41 && (x)<=0x5A)?1:0)
#define NURO_ISLOWER(x)		(((x)>=0x61 && (x)<=0x7A)?1:0)


#ifdef __cplusplus
extern "C"
{
#endif
	
	nuBOOL	nuGetSRECT(nuroSRECT &rect, const nuroSRECT &fatherRect, const nuroSRECT &localRect, nuUINT nType);
	nuVOID	nuNormalizeRect(LPNURORECT pRect);
	nuBOOL	nuPointInRect(LPNUROPOINT pPt, LPNURORECT pRect);
	nuBOOL	nuPtInSRect(const NUROPOINT& pt, const NUROSRECT& rect);
	nuBOOL	nuPtInRect(const NUROPOINT& pt, const NURORECT& rect);
	nuBOOL	nuPtInPolygon(const NUROPOINT& pt, const NUROPOINT* pPt, nuINT nCount);
	nuBOOL  nuSPtInRect(const NUROSPOINT& pt, const NURORECT& rect);
	nuBOOL	nuRectCoverRect(LPNURORECT pRect1, LPNURORECT pRect2);
	nuBOOL	nuPtFixToLineSeg(const nuroPOINT& NowPt, const nuroPOINT& LinePt1, const nuroPOINT& LinePt2, nuroPOINT *FixedPt, nuLONG *Dis);
	nuVOID	nuBlockIDtoRect(nuDWORD nBlockID, LPNURORECT lpRect);
	nuVOID	nuDegreeToMeter(nuFLOAT fDegree, nuLONG &meter);
	nuVOID	nuMeterToDegree(nuLONG  meter, nuFLOAT &fDegree);
	nuVOID	nuRectToBlockIDArray(const NURORECT& rect, PBLOCKIDARRAY pBlockIDArray);
	nuVOID	nuRectToBlockIDs(const NURORECT &rect, PBLOCKIDARRAY pBlockIDArray, nuLONG nBlockSize);
	nuLONG	nuGetXBlkNum(nuLONG nBlockSize);
	nuVOID	nuBlockIDtoRectEx(nuDWORD nBlockID, NURORECT& rect, nuLONG nBlockSize);
	nuBOOL	nuBlockIDArrayEqual(const BLOCKIDARRAY& block1, const BLOCKIDARRAY& block2);
	nuBOOL	nuPointFix(nuroPOINT ptCenter,nuroPOINT &ptFix,nuroRECT ScreenRt);
	nuLONG	nuDis(nuDOUBLE lat1, nuDOUBLE lng1, nuDOUBLE lat2, nuDOUBLE lng2);

#ifdef __cplusplus
}
#endif


#endif
