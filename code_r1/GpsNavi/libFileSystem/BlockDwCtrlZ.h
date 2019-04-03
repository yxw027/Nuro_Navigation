// BlockDwCtrlZ.h: interface for the CBlockDwCtrlZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKDWCTRLZ_H__8E64BF66_2DCB_435E_82FE_9E351D9E06C5__INCLUDED_)
#define AFX_BLOCKDWCTRLZ_H__8E64BF66_2DCB_435E_82FE_9E351D9E06C5__INCLUDED_

#include "MapFileBhZ.h"

#define MAX_DW_COUNT_LOADEN			8

class CBlockDwCtrlZ  : public CFileMapBaseZ
{
public:
	typedef struct tagBLKINFONEED
	{
		nuDWORD	nBlockID;
		nuDWORD nBlkIdx;
		nuWORD	nDwIdx;
		nuBYTE	nMode;
//		nuBYTE	bBlkExtend;
	}BLKINFONEED, *PBLKINFONEED;
public:
	CBlockDwCtrlZ();
	virtual ~CBlockDwCtrlZ();

	nuBOOL Initialize(class CBlockDw* pBlockDw);
	nuVOID Free();

	nuPVOID LoadDwDataSmall(nuroRECT rtMap, nuDWORD nDataMode);
	nuVOID	FreeDwDataSmall(nuBOOL bAll = nuTRUE);

	nuBOOL  SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut);
	nuUINT  SeekForCcdInfo(nuroRECT rtMap, PCCDINFO pCcdInfo);

	nuUINT SeekForKMInfo(nuroRECT rtMap, PKMINFO pKMInfo);//added by daniel 20121205
	nuBOOL SeekForKMRoad( nuWORD nMapIdx, 
					      nuWORD nRdIdx, 
						  nuDWORD nBlkIdx,
						  nuSHORT	nAngle, 
						  nuDWORD nRoadClass,
						  nuroPOINT& ptNextCoor, 
						  PDWBLKDATASMALL pDwData, 
						  PKMROAD pKMRoad );//added by daniel 20121205

protected:
	nuBOOL ColBlockInfoSmall(PBLKINFONEED pBlkInfoNeed, nuINT nCount);
	nuBOOL ReadDWDataSmall(nuDWORD nDataMode);

	nuVOID FreeBlock(PDWBLOCKNODE pDwBlkNode);
	nuVOID SortBlkNodeByDwIdx(PDWBLOCKNODE pDwBlkNode, nuBYTE nCount);
	nuVOID SortBlkNodeByBlkID(PDWBLOCKNODE pDwBlkNode, nuBYTE nCount);

	nuBOOL SeekForCcdRoad( nuWORD nMapIdx, 
						   nuWORD nRdIdx, 
						   nuDWORD nBlkIdx,
						   nuSHORT nAngle,
						  nuDWORD nRoadClass,
						 nuroPOINT& ptNextCoor, 
						 PDWBLKDATASMALL pDwData, 
						 PCCDROAD pCcdRoad );
protected:
	DWBLKDATASMALL	m_dwBlkDataSmall;

	class CBlockDw*	m_pBlockDw;
};

#endif // !defined(AFX_BLOCKDWCTRLZ_H__8E64BF66_2DCB_435E_82FE_9E351D9E06C5__INCLUDED_)
