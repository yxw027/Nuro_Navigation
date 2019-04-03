// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: GDataMgr.h,v 1.9 2010/09/03 07:01:46 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/09/03 07:01:46 $
// $Locker:  $
// $Revision: 1.9 $
// $Source: /home/nuCode/libNaviCtrl/GDataMgr.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDATAMGR_H__461D3AB3_98DD_43F9_9F01_6F71CB61B00B__INCLUDED_)
#define AFX_GDATAMGR_H__461D3AB3_98DD_43F9_9F01_6F71CB61B00B__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroConstDefined.h"

#define ANGLE_TYPE_CAR						0x01
#define ANGLE_TYPE_ROTATE					0x02

#define MAPANGLE_SETTYPE_PROGRESS			0x01
#define MAPANGLE_SETTYPE_ONCE				0x02

//Split screen type
#define SPLIT_SCREEN_AUTO_CLOSE				0x01
#define SPLIT_SCREEN_AUTO_OPEN				0x02

//for SplitScreenProc();
#define SPLIT_PROC_TYPE_AUTO				0x00
#define SPLIT_PROC_TYPE_HAND				0x01

#define SPLIT_PROC_RES_NO					0x00
#define SPLIT_PROC_RES_SPLITTING			0x01
#define SPLIT_PROC_RES_SPLITCOVER			0x02

class CGDataMgr
{
public:
	CGDataMgr();
	virtual ~CGDataMgr();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL SetUp(PACTIONSTATE pAcState);
	nuBOOL FixUp(PACTIONSTATE pAcState);

	nuBOOL PointScreenToBmp(nuLONG& x, nuLONG& y);
	nuBOOL GetScreenCenter(nuLONG& x, nuLONG& y);

	nuBOOL SetAngle(nuLONG nAngle, nuBYTE nAngleType = ANGLE_TYPE_ROTATE );

	nuBOOL ZoomSet(nuINT nScale);

	nuBOOL SetDrawInfoMap(PACTIONSTATE pAcState);

	nuVOID OverSpeedCheck(nuWORD nWarningSlice);

	nuBOOL SetCrossNear( PACTIONSTATE pAcState, nuBYTE nCrossNearType = CROSSNEARTYPE_DEFAULT, 
		                 nuBYTE nCrossZoomtype = CROSSNEARTYPE_DEFAULT );
	nuBOOL SetCrossNearEx( PACTIONSTATE pAcState, nuBYTE nCrossNearType = CROSSNEARTYPE_DEFAULT, 
		                   nuBYTE nCrossZoomtype = CROSSNEARTYPE_DEFAULT );

	nuVOID CalMapShowInfo(PACTIONSTATE pAcState);

	nuVOID SpeedAutoZoom(PACTIONSTATE pAcState, nuBYTE nIndex = 0xFF);

	nuVOID ResetMapAngle(nuSHORT nAngle, nuBYTE nType = MAPANGLE_SETTYPE_ONCE);

	nuBOOL CarFakeMove(nuLONG nFakeDis, PACTIONSTATE pAcState = NULL);
	nuBOOL CarFakeMove(nuLONG nFakeDis, nuBYTE nTotalTimes, nuBYTE nNowIdx, PACTIONSTATE pAcState = NULL);
	//
	nuBOOL MapSetup(PACTIONSTATE pAcState);
	nuBOOL CalMapLoadInfo(PACTIONSTATE pAcState, nuLONG nSW = 0, nuLONG nSH = 0);
	nuVOID MapBmpCenterSet(nuBYTE nCenterMode = CAR_CENTER_MODE_REMAIN, nuWORD x = 0, nuWORD y = 0);
	//
	nuBOOL ZoomMapInit();
	nuVOID ZoomMapFree();
	nuBOOL ZoomMapSetup(nuroPOINT& ptMapCenter, nuSHORT nAngleDx, nuSHORT nAngleDy, nuLONG nPicID);

	nuUINT SplitScreenProc(nuBYTE nSpProcType = SPLIT_PROC_TYPE_AUTO, nuBYTE nNowCrossNearType = 0);
	nuVOID SetAngleParameter();
protected:
	nuVOID MapBmpScaleSet();
	nuVOID MapBmpSizeSet();
	nuBOOL GetMapShowSize(nuLONG nScreenShowW, nuLONG nScreenShowH, nuroRECT& rtMapSize);
	nuVOID MapAngleSet(PACTIONSTATE pAcState);

	//
	nuBOOL GetZoomMapShowSize(nuLONG nWidth, nuLONG nHeight, nuroRECT& rtMapSize);

	nuLONG CrossZoomProc(PACTIONSTATE pAcState, nuBYTE nCrossZoomtype = CROSSNEARTYPE_DEFAULT);


protected:
	nuVOID SetBmpExtend(nuBYTE nBmpSizeMode = BMPSIZE_EXTEND_NO);
	nuVOID SetMapBitmapSize(nuINT nLtExd = 0, nuINT nUpExd = 0, nuINT nRtExd = 0, nuINT nDnExd = 0);
	nuVOID SetMapSize(nuLONG nScale);
	
	nuVOID CalZoomShowRect(PNURORECT lpRect);
	nuVOID CalShowRect(PNURORECT lpRect);
//	nuVOID MapRectToBlockIDArray(const PNURORECT lpRect, PBLOCKIDARRAY pBlockIDArray);
	nuBOOL AllocMemBitmap();
	nuVOID FreeMemBitmap();
	nuVOID SetBmpCenter();

	nuLONG	m_nSpeedAutoZoom;
	nuINT	m_nSpeedWarningTimer;
	nuDWORD	m_nMinuteTimer;
	nuBYTE	m_nOldScaleIdx;
	nuBYTE	m_nNowCrossNearType;
	nuBYTE	m_nNowCrossZoomType;
	nuBYTE	m_nSplitScreenTye;
	nuBOOL	m_bCrossZoomMap;
	nuBOOL	m_bCrossViewChanging;
	nuLONG	m_nOldScale;

	nuWORD	m_nCrossZoomDis;
	nuWORD	m_nCrossZoomScale;
	nuBYTE	m_n3DAngleStep;

private:
	nuWORD	m_nOldBmpCenterX;
	nuWORD	m_nOldBmpCenterY;

	nuWORD	m_nCarPosition;

	nuINT		m_nCenterScale;
public:
	nuLONG m_nRotateMapAngle;
	nuLONG m_nRoadIdx;
	nuBOOL m_bOverSpeed;
};

#endif // !defined(AFX_GDATAMGR_H__461D3AB3_98DD_43F9_9F01_6F71CB61B00B__INCLUDED_)
