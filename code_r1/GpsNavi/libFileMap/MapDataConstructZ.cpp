// MapDataConstructZ.cpp: implementation of the CMapDataConstructZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapDataConstructZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapDataConstructZ::CMapDataConstructZ()
{

}

CMapDataConstructZ::~CMapDataConstructZ()
{

}

nuVOID CMapDataConstructZ::ColMdwBlock(nuPBYTE pData, PMDWBLKCOL pMdwBlkCol)
{
	pMdwBlkCol->pMdwHead	= (PMDWHEAD)pData;
	pData += sizeof(MDWHEAD);
	pMdwBlkCol->pMdwRoad	= (PMDWROAD)pData;
	pData += sizeof(MDWROAD) * pMdwBlkCol->pMdwHead->nRoadCount;
	pMdwBlkCol->ptVertext	= (PMDWPOINT)pData;
}

nuVOID CMapDataConstructZ::ColBglBlock(nuPBYTE pData, PBGLBLKCOL pBglBlkCol)
{
	pBglBlkCol->pHead = (PBGLBLKHEAD)pData;
	pData += sizeof(BGLBLKHEAD);
	pBglBlkCol->pData = (PBGLBLK)pData;
	pData += sizeof(BGLBLK) * pBglBlkCol->pHead->nBglCount;
	pBglBlkCol->ptVertext = (PBGLPOINT)pData;
}

nuVOID CMapDataConstructZ::ColSeaBlock(nuPBYTE pData, PSEABLKCOL pSeaBlkCol)
{
	pSeaBlkCol->pSeaHead = (PSEABLKHEAD)pData;
	pData += sizeof(SEABLKHEAD);
	pSeaBlkCol->pSeaData = (PSEABLKDATA)pData;
	pData += sizeof(SEABLKDATA) * pSeaBlkCol->pSeaHead->nSeaAreaCount;
	pSeaBlkCol->ptVertex = (PSEAPOINT)pData;
}

nuVOID CMapDataConstructZ::ColRdwBlock(nuPBYTE pData, PDWROADBLK pDwRoadBlk)
{
	pDwRoadBlk->pDwRoadHead = (PDWROADHEAD)pData;
	pData += sizeof(DWROADHEAD);
	pDwRoadBlk->pDwRoadData = (PDWROADDATA)pData;
	pData += sizeof(DWROADDATA)*pDwRoadBlk->pDwRoadHead->nRoadCount;
	pDwRoadBlk->pDwRoadCommon = (PDWROADCOMMON)pData;
	pData += sizeof(DWROADCOMMON)*pDwRoadBlk->pDwRoadHead->nRoadCommonCount;
	pDwRoadBlk->pSPointCoor = (PNUROSPOINT)pData;
	pData += sizeof(PNUROSPOINT)*pDwRoadBlk->pDwRoadHead->nVertexCount;
	pDwRoadBlk->pRoadRect = (PNUROSRECT)pData;

	/*
	pRoadBlk->pDwRoadHead = (PDWROADHEAD)pByte;
	pByte += sizeof(DWROADHEAD);
	pRoadBlk->pDwRoadData = (PDWROADDATA)pByte;
	pByte += sizeof(DWROADDATA)*pRoadBlk->pDwRoadHead->nRoadCount;
	pRoadBlk->pDwRoadCommon = (PDWROADCOMMON)pByte;
	pByte += sizeof(DWROADCOMMON)*pRoadBlk->pDwRoadHead->nRoadCommonCount;
	pRoadBlk->pSPointCoor = (PNUROSPOINT)pByte;
	pByte += sizeof(PNUROSPOINT)*pRoadBlk->pDwRoadHead->nVertexCount;
	pRoadBlk->pRoadRect = (PNUROSRECT)pByte;
	*/
}

nuVOID CMapDataConstructZ::ColPdwBlock(nuPBYTE pData, PDWPOIBLK pDwPoiBlk)
{
	pDwPoiBlk->pDwPOIHead = (PDWPOIHEAD)pData;
	pData += sizeof(DWPOIHEAD);
	pDwPoiBlk->pDwPOIData = (PDWPOIDATA)pData;
}

