// DrawMpoiZ.h: interface for the CDrawMpoiZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWMPOIZ_H__9FEC7551_8C80_4533_829A_AB0AD09DE4E5__INCLUDED_)
#define AFX_DRAWMPOIZ_H__9FEC7551_8C80_4533_829A_AB0AD09DE4E5__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

#define  _MAJOR_POI_NAME_NUM			4
class CDrawMpoiZ : CFileMapBaseZ 
{
public:
	typedef struct tagMPOINODE
	{
		nuroPOINT	point;
		nuDWORD		nClass:8;
		nuDWORD		nReserve:24;
		nuWCHAR		nName[_MAJOR_POI_NAME_NUM];
	}MPOINODE, *PMPOINODE;

	typedef struct tagMPOIFILEDATA
	{
		nuDWORD		nPOICount;
		PMPOINODE	pMPOIList;
	}MPOIFILEDATA, *PMPOIFILEDATA;

public:
	CDrawMpoiZ();
	virtual ~CDrawMpoiZ();
	nuBOOL Initialize();
	nuVOID Free();
	nuBOOL ColMpoiData(nuPVOID pVoid);
	nuBOOL ReadMpoiData();

protected:
	nuWORD	m_nMemIdx;
};

#endif // !defined(AFX_DRAWMPOIZ_H__9FEC7551_8C80_4533_829A_AB0AD09DE4E5__INCLUDED_)
