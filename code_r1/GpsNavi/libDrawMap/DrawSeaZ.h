// DrawSeaZ.h: interface for the CDrawSeaZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWSEAZ_H__EDCAB889_51AF_4613_B854_7765F743CA9E__INCLUDED_)
#define AFX_DRAWSEAZ_H__EDCAB889_51AF_4613_B854_7765F743CA9E__INCLUDED_

#include "NuroDefine.h"
#include "MapFileSeaZ.h"

#define _USE_SEA_DRAW

#define SEA_AREA_TYPE					0
#define RIVER_AREA_TYPE					1
#define ISLAND_AREA_TYPE				9
#define SPECIAL_AREA_TYPE				10

class CDrawSeaZ  
{
public:
	CDrawSeaZ();
	virtual ~CDrawSeaZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL LoadData(const nuroRECT& rtMap, nuLONG nScale);
	nuVOID FreeData();

	nuBOOL DrawSeaZ();
	nuBOOL ColSeaData();

protected:
	CMapFileSeaZ	m_fileSea;
	PSEADATA		m_pSeaData;
};

#endif // !defined(AFX_DRAWSEAZ_H__EDCAB889_51AF_4613_B854_7765F743CA9E__INCLUDED_)
