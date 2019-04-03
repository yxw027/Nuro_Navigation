// MapDataConstructZ.h: interface for the CMapDataConstructZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDATACONSTRUCTZ_H__F3C0D707_1D2D_4AC1_9C13_5B99F693F39A__INCLUDED_)
#define AFX_MAPDATACONSTRUCTZ_H__F3C0D707_1D2D_4AC1_9C13_5B99F693F39A__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

class CMapDataConstructZ  
{
public:
	CMapDataConstructZ();
	virtual ~CMapDataConstructZ();

	static nuVOID ColMdwBlock(nuPBYTE pData, PMDWBLKCOL pMdwBlkCol);
	static nuVOID ColBglBlock(nuPBYTE pData, PBGLBLKCOL pBglBlkCol);
	static nuVOID ColSeaBlock(nuPBYTE pData, PSEABLKCOL pSeaBlkCol);
	static nuVOID ColRdwBlock(nuPBYTE pData, PDWROADBLK pDwRoadBlk);
	static nuVOID ColPdwBlock(nuPBYTE pData, PDWPOIBLK pDwPoiBlk);
};

#endif // !defined(AFX_MAPDATACONSTRUCTZ_H__F3C0D707_1D2D_4AC1_9C13_5B99F693F39A__INCLUDED_)
