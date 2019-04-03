// DrawMgrZ.h: interface for the CDrawMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWMGRZ_H__5E3D7E2E_C51E_41E0_9ED6_E01974360EA2__INCLUDED_)
#define AFX_DRAWMGRZ_H__5E3D7E2E_C51E_41E0_9ED6_E01974360EA2__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"
#include "LineCollect.h"
#include "DrawReal3DPicZ.h"

//#define _USE_NEW_DRAWMAP				

#define DRAWRD_MODE_COMMON			0x00
#define DRAWRD_MODE_ONLYCENTER		0x01
#define DRAWRD_MODE_FAKELINE		0x02

class CDrawMgrZ  
{
public:
	typedef struct tagCROSSZOOMROADEX
	{
		nuroPOINT	point;
		nuBYTE		nRoadClass;
		nuBYTE		nRdDict;
	}CROSSZOOMROADEX, *PCROSSZOOMROADEX;
public:
	CDrawMgrZ();
	virtual ~CDrawMgrZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL AllocDrawMem();
	nuBOOL FreeDrawMem();

	nuBOOL CollectDrawMem();

	nuBOOL DrawRoad(nuPVOID lpVoid, nuBYTE nDrawMode = DRAWRD_MODE_COMMON);
	nuBOOL DrawBgLine(nuPVOID lpVoid, nuBYTE nDrawMode = DRAWRD_MODE_COMMON);
	nuBOOL DrawBgArea(nuPVOID lpVoid, nuBYTE nDrawMode = DRAWRD_MODE_COMMON);

	nuBOOL ZoomCrossProc(nuPVOID lpVoid);

	nuBOOL DrawTrace(nuPVOID pVoid);
	nuBOOL DrawMemo(nuPVOID lpVoid);
	nuBOOL DrawTrajectory(nuPVOID lpVoid); //Gps走过的点
	nuBOOL DrawEEyesPlayed(nuPVOID lpVoid);

	nuBOOL ColMapRoad(nuPVOID lpVoid);

protected:
	nuBOOL DrawZoomCrossRoad(nuPVOID lpVoid, PDWBLKDATASMALL pDwBlkDataSmall);
	nuBOOL DrawZoomNaviRoad();
	nuBOOL DrawZoomCrossPoi(nuPVOID lpVoid, PDWBLKDATASMALL pDwBlkDataSmall);
//	nuVOID CollectShp(nuPVOID lpVoid);

public:
	nuBOOL		m_bDrawRawCar;

protected:
	CLineCollect		m_zoomRoadCol;
	CDrawReal3DPicZ		m_drawReal3Dpic;
	class CMemoMgrZ*	m_pMemoMgr;
	NURO_BMP			m_bmpEEye;
	PSCREENMAP			m_pScreenMap;

private:
	nuLONG m_nCrossExtendDis;
	nuBYTE				m_bDrawCrossIcon;
	nuBYTE				m_nDottedLineClass;
	nuBYTE				m_nZoomArrowWidth;
};

#endif // !defined(AFX_DRAWMGRZ_H__5E3D7E2E_C51E_41E0_9ED6_E01974360EA2__INCLUDED_)
