// DrawMajorPoiZK.h: interface for the CDrawMajorPoiZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWMAJORPOIZK_H__C271156F_BFD2_478D_8D2A_0F8378F68542__INCLUDED_)
#define AFX_DRAWMAJORPOIZK_H__C271156F_BFD2_478D_8D2A_0F8378F68542__INCLUDED_

#include "NuroDefine.h"
#include "FileMapBaseZ.h"

class CDrawMajorPoiZK : CFileMapBaseZ
{
public:
	typedef struct tagMAJOR_POI_HEADER
	{
		nuDWORD	nPoiCount;
		nuDWORD	nNameAddrSt;
	}MAJOR_POI_HEADER, *PMAJOR_POI_HEADER;

	typedef struct tagMAJOR_POI_NODE
	{
		nuroPOINT	point;
		nuBYTE		nClass;
		nuBYTE		nNameLen;
		nuWORD		nReserved;
	}MAJOR_POI_NODE, *PMAJOR_POI_NODE;

public:
	CDrawMajorPoiZK();
	virtual ~CDrawMajorPoiZK();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL	ReadMajorPoi();
	nuBOOL	ColMajorPoi(nuPVOID lpVoid);
	nuVOID	FreeMajorPoi();

protected:
	inline nuBOOL PoiCover(nuroRECT* ptRect, nuINT& nNowCount, nuroRECT* pRect);
	nuVOID LoadIconBmp(nuBYTE nDayNight);

protected:
	nuWORD		m_nMemIdx;
	NURO_BMP	m_bmpIcon;
	nuBYTE		m_nDayNight;
};

#endif // !defined(AFX_DRAWMAJORPOIZK_H__C271156F_BFD2_478D_8D2A_0F8378F68542__INCLUDED_)
