// BlockDw.h: interface for the CBlockDw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKDW_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)
#define AFX_BLOCKDW_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"
#include "NuroMapFormat.h"
//#include "FileFB.h"

#define MAX_BLOCKSNODE_LIST					16

class CBlockDw  
{
public:
	typedef struct tagBLOCKSNODE
	{
		nuWORD	nDwIdx;
		nuWORD	nMemMassIdx;
		nuDWORD	nBlkID;
		nuDWORD nBlkIdx;
	}BLOCKSNODE, *PBLOCKSNODE;
	typedef struct tagBLOCKINFO
	{
		nuLONG	nMapIdx;
		nuLONG	nBlockID;
		nuLONG	nBlockIdx;
	}BLOCKINFO, *PBLOCKINFO;
public:
	CBlockDw();
	virtual ~CBlockDw();

	nuBOOL Initialize(class CMapFileFhZ* pFileFh);
	nuVOID Free();
	nuBOOL LoadBlocks(const BLOCKIDARRAY& nBIDLMap, const BLOCKIDARRAY& nBIDLZoom);

	nuBOOL SetBlockID(nuWORD nDwIdx, nuDWORD nBlockID, nuDWORD nBlockIdx = 0, nuBYTE nBlockExtend = 0,
		              nuBYTE nBlockZoom = 0, nuBYTE nBlockZoomExtended = 0);
	nuVOID CollectBlocks();
	nuBOOL ReadBlocks();

	nuPVOID GetScreenMap();

	nuBOOL SeekForNaviPt(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDROADDATA pRoadData);
	nuBOOL PaddingRoad(PSEEKEDROADDATA pRoadData);
	nuBOOL SeekForPoi(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDPOIDATA pPoiData);
	nuBOOL SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, nuBOOL bSetMarket);
protected:
	nuBOOL ReleaseAllBlocks(nuWORD nDwIdx);
	nuBOOL ReleaseBlocksUnused(nuWORD nDwIdx);
	nuBOOL ReadDwBlocks(nuWORD nDwIdx);

	nuBOOL ColRdwBlock(nuWORD nMassIdx, PDWROADBLK pRoadBlk);
	nuBOOL ColPdwBlock(nuWORD nMassIdx, PDWPOIBLK	pPoiBlk);
	nuBOOL ColBdwBgLineBlock(nuWORD nMassIdx, PDWBGLINEBLK pDwBgLineBlk);
	nuBOOL ColBdwBgAreaBlock(nuWORD nMassIdx, PDWBGAREABLK pDwBgAreaBlk);

	nuBOOL ColScreenMap();
	nuVOID RelScreenMap();

	nuVOID SortBlockNodesByDw(PBLOCKSNODE pBlockNode, nuWORD nCount);
	nuBOOL ReadRdwBlocks(PBLOCKSNODE pBlockNode, nuWORD nCount);
	nuVOID RelsRdwBlocks(PBLOCKSNODE pBlockNode, nuWORD nCount);
	nuBOOL SeekBlkForRoad( nuLONG x, nuLONG y, nuLONG nAngle, PSEEKEDROADDATA pRoadData, 
		                   PDWROADBLK pRoadBlk, nuLONG nBlockID );
	nuBOOL SeekBlkForPoi( nuLONG x, nuLONG y, nuLONG nAngle, PSEEKEDPOIDATA pPoiData, PDWPOIBLK pPoiblk, 
		                  nuLONG nBlockID, PBLOCKINFO pBlockInfo, nuDWORD nCoverDis = CENTERPOI_SEEK_MAX_DIS  );
	nuINT GetPoiIndex(nuLONG nLen, PSEEKEDPOIDATA pPoiList, nuWORD nNowCount);
	nuBOOL SeekBlkForRoads(nuBOOL bNameMapping, PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, PDWROADBLK pRoadBlk, 
		                    nuLONG nBlockID, nuWORD nDwIdx, nuDWORD nBlkIdx, nuDWORD nBlkID );
	nuBOOL SeekBlkForRoadsEX(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, PDWROADBLK pRoadBlk, 
							nuLONG nBlockID, nuWORD nDwIdx, nuDWORD nBlkIdx, nuDWORD nBlkID );
public:
	nuWORD		m_nDwCount;
	PDWDATA		m_pDwDataList;
	SCREENMAP	m_screenMap;
//	class CFileFB	m_fileFB;
	class CMapFileFhZ*	m_pFileFh;

private:
	BLOCKIDARRAY	m_blockIDArrayOld;
	nuLONG m_nMaxDisIdx;
	nuLONG m_nMaxDis;
	/*test
		nuINT m_nRdBlocks;
		nuINT m_nPdBlocks;
		nuINT m_nBlBlocks;
		nuINT m_nBaBlocks;
	test*/
	nuWORD			m_LastCarAngle;
	nuLONG			m_lUseTMC;
};

#endif // !defined(AFX_BLOCKDW_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)
