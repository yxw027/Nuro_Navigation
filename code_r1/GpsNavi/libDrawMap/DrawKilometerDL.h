// DrawKilometerDL.h: interface for the CDrawKilometerDL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWKILOMETERDL_H__E723E1BC_E2B4_4EBF_B68A_18813889DA20__INCLUDED_)
#define AFX_DRAWKILOMETERDL_H__E723E1BC_E2B4_4EBF_B68A_18813889DA20__INCLUDED_

#include "NuroDefine.h"
#include "nuRouteData.h"

#define _KILO_BMP_FILE_PATH_      nuTEXT("Media\\PIC\\")
#define _MAX_KILO_ICON_COUNT_     20

class CDrawKilometerDL  
{
public:
	typedef struct tagKMData
	{
		nuLONG    nScreenX;
		nuLONG    nScreenY;
		nuLONG    nMapX;
		nuLONG    nMapY;
		nuSHORT   nDis;
		nuSHORT   nClass;
		nuLONG	  nBlockidx;
		nuLONG	  nRoadidx;
	}KMDATA, *PKMDATA;

	CDrawKilometerDL();
	virtual ~CDrawKilometerDL();
	//
	nuBOOL  Initialize();
	nuVOID  Free();
	nuBOOL  AddKilometers(nuDWORD nBlockIdx);
	nuUINT  DrawKilometers(nuBYTE byIconType);
	nuUINT  ResetKmList();
	nuUINT  SetKiloFontColor(nuCOLORREF col, NUROFONT nuLogFont, nuBOOL bUISet);
	nuBOOL  GetNearKilo(nuLONG* pDis, nuWCHAR* pwsKilos);
	nuVOID	MappingKm(const nuLONG &KmCount, const NUROPOINT &ptCar, PKMDATA pShowKmData, 
		KMInfoData *pFindKmData, nuLONG &Index, nuLONG &Dis, nuWCHAR *wsKiloNum);
	nuBOOL	JudePriority(nuLONG BlockIdx_1, nuLONG BLockIdx_2, nuLONG RoadIdx_1, nuLONG RoadIdx_2);

private:
	nuUINT  LoadKiloIcon(nuBYTE byIconType);
	nuBOOL  IsIconOverlaped(KMDATA &KmData);

private:
	nuBYTE       m_byIconType;
	NURO_BMP     m_bmpKilo;
	nuUINT       m_nKmCount;
	nuWCHAR      m_wsKMName[5];
	nuLONG		 m_lKiloNum;
	nuDOUBLE	 m_dAddKiloNum;
	PKMDATA      m_pKmDataList;
	nuCOLORREF   m_Color;
	NUROFONT     m_LogFont;
	nuBOOL       m_bUISet;
	nuLONG		 m_nTickCount;
	nuLONG		 m_lRoadidx;
	nuLONG		 m_lBlockidx;
	KMInfoData   *m_pKmData;
	nuLONG		 m_lNearKMCount;

};


#endif // !defined(AFX_DRAWKILOMETERDL_H__E723E1BC_E2B4_4EBF_B68A_18813889DA20__INCLUDED_)
