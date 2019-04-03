// BlockDwCtrlZ.cpp: implementation of the CBlockDwCtrlZ class.
//
//////////////////////////////////////////////////////////////////////

#include "BlockDwCtrlZ.h"
#include "BlockDw.h"
#include "MapFileRdwZ.h"
#include "libFileSystem.h"
#include "MapFilePdwZ.h"
#include "MapFileFhZ.h"
#include "MapDataConstructZ.h"

#include "NuroClasses.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockDwCtrlZ::CBlockDwCtrlZ()
{
	m_pBlockDw = NULL;
	nuMemset(&m_dwBlkDataSmall, 0, sizeof(m_dwBlkDataSmall));
}

CBlockDwCtrlZ::~CBlockDwCtrlZ()
{
	Free();
}

nuBOOL CBlockDwCtrlZ::Initialize(class CBlockDw* pBlockDw)
{
	m_pBlockDw = pBlockDw;
	m_dwBlkDataSmall.nBlkCount = 0;
	return nuTRUE;
}

nuVOID CBlockDwCtrlZ::Free()
{
	FreeDwDataSmall();
}

nuVOID CBlockDwCtrlZ::FreeDwDataSmall(nuBOOL bAll /* = nuTRUE */)
{
	nuBYTE nDeleted = 0;
	nuBYTE i;
	for( i = 0; i < m_dwBlkDataSmall.nBlkCount; ++i)
	{
		if( bAll || !m_dwBlkDataSmall.pDwList[i].nCover )
		{
			++nDeleted;
			FreeBlock(&m_dwBlkDataSmall.pDwList[i]);
		}//删除不需要的Block资料
	}
	//
	if( nDeleted > 0 && nDeleted < m_dwBlkDataSmall.nBlkCount )
	{
		//整理资料把中间删除的Block节点移到数组的最后面
		nuINT nIdxDel = -1;
		for( i = 0; i < m_dwBlkDataSmall.nBlkCount; ++i )
		{
			if( m_dwBlkDataSmall.pDwList[i].nCover )
			{
				if( nIdxDel != -1 )
				{
					m_dwBlkDataSmall.pDwList[nIdxDel ++] = m_dwBlkDataSmall.pDwList[i];
				}
			}
			else
			{
				if( nIdxDel == -1 )
				{
					nIdxDel = i;
				}
			}
		}
	}
	//
	m_dwBlkDataSmall.nBlkCount -= nDeleted;
}
nuVOID CBlockDwCtrlZ::FreeBlock(PDWBLOCKNODE pDwBlkNode)
{
	if( pDwBlkNode->nDataFrom == DW_DATA_FROM_READ )
	{
		m_pMmApi->MM_RelDataMemMass(&pDwBlkNode->nPdwMemIdx);
		m_pMmApi->MM_RelDataMemMass(&pDwBlkNode->nRdwMemIdx);
		m_pMmApi->MM_RelDataMemMass(&pDwBlkNode->nBLdwMemIdx);
		m_pMmApi->MM_RelDataMemMass(&pDwBlkNode->nBAdwMemIdx);
	}
	else
	{
		pDwBlkNode->nPdwMemIdx	= -1;
		pDwBlkNode->nRdwMemIdx	= -1;
		pDwBlkNode->nBLdwMemIdx = -1;
		pDwBlkNode->nBAdwMemIdx = -1;
	}
	pDwBlkNode->nDataFrom = DW_DATA_FROM_NO;
	pDwBlkNode->nCover = 0;
	/*
	pDwBlkNode->nPdwBlkAddr	= -1;
	pDwBlkNode->nRdwBlkAddr	= -1;
	pDwBlkNode->nBLdwBlkAddr= -1;
	pDwBlkNode->nBAdwBlkAddr= -1;
	pDwBlkNode->nBlkID = 0;
	pDwBlkNode->nDwIdx = -1;
	*/
}

nuPVOID CBlockDwCtrlZ::LoadDwDataSmall(nuroRECT rtMap, nuDWORD nDataMode)
{
	BLOCKIDARRAY blockIDlist = {0};
	nuLONG blockID = 0;
	nuLONG nDwCount = LibFS_GetMapCount();
	nuRectToBlockIDArray(rtMap, &blockIDlist);
	nuINT nTotalCount = (blockIDlist.nYend - blockIDlist.nYstart + 1) * (blockIDlist.nXend - blockIDlist.nXstart + 1);
	nuINT nNowBlkCount = 0;
	//test
	if( nTotalCount > MAX_DW_BLOCKS_SMALL )
	{
		nTotalCount = MAX_DW_BLOCKS_SMALL;
	}
	PBLKINFONEED pBlkInfoNeed = new BLKINFONEED[nTotalCount];
	if( pBlkInfoNeed == NULL )
	{
		return NULL;
	}
//	nuroRECT rtBlock;
//	nuMemset(pBlkInfoNeed, 0, sizeof(BLKINFONEED)*nTotalCount);
	//整理资料，确定需要load的BlockID和DwIdx
	for( nuLONG i = blockIDlist.nYstart; i <= blockIDlist.nYend; ++i )
	{
		for( nuLONG j = blockIDlist.nXstart; j <= blockIDlist.nXend; ++j )
		{
			if( nNowBlkCount >= nTotalCount )
			{
				break;
			}
			BLK_OUT_DATA blkOut;
			if( m_pBlockDw->m_pFileFh->BlockIDToMap(nuLOWORD(j), nuLOWORD(i), &blkOut) )
			{
				blockID = i*XBLOCKNUM + j;
				for(nuINT k = 0; k < blkOut.nMapFixed; ++k)
				{
					pBlkInfoNeed[nNowBlkCount].nDwIdx	= blkOut.pBlkFixData[k].nMapIdx;
					pBlkInfoNeed[nNowBlkCount].nBlkIdx	= blkOut.pBlkFixData[k].nBlkIdx;
					pBlkInfoNeed[nNowBlkCount].nBlockID = blockID;
					pBlkInfoNeed[nNowBlkCount].nMode	= 0;
					++nNowBlkCount;
					if( nNowBlkCount >= nTotalCount )
					{
						break;
					}
				}
			}
		}
	}
	/* 2009.06.16
	for( nuINT k = 0; k < nDwCount; ++k )
	{
		for( nuLONG i = blockIDlist.nYstart; i <= blockIDlist.nYend; ++i )
		{
			for( nuLONG j = blockIDlist.nXstart; j <= blockIDlist.nXend; ++j )
			{
				if( nNowBlkCount >= nTotalCount )
				{
					break;
				}
				blockID = i*XBLOCKNUM + j;
				if( m_pBlockDw->m_fileFB.BlockIDInMap(blockID, k) )
				{
					pBlkInfoNeed[nNowBlkCount].nDwIdx	= k;
					pBlkInfoNeed[nNowBlkCount].nBlockID = blockID;
					pBlkInfoNeed[nNowBlkCount].nMode	= 0;
					++nNowBlkCount;
				}
			}
		}
	}//looking for the DW needed.
	*/
	//
	ColBlockInfoSmall(pBlkInfoNeed, nNowBlkCount);
	delete []pBlkInfoNeed;
	pBlkInfoNeed=NULL;
	//把Block资料按DwIdx按小到大排序
	SortBlkNodeByDwIdx(m_dwBlkDataSmall.pDwList, m_dwBlkDataSmall.nBlkCount);
	//Read Block Data;
	if( !ReadDWDataSmall(nDataMode) )
	{
		return NULL;
	}
	SortBlkNodeByBlkID(m_dwBlkDataSmall.pDwList, m_dwBlkDataSmall.nBlkCount);
	return &m_dwBlkDataSmall;
}

nuBOOL CBlockDwCtrlZ::ReadDWDataSmall(nuDWORD nDataMode)
{
	//在已经读入的Block中寻找
	nuBYTE nHasLoad = 0;
	nuBYTE i = 0;
	for( i = 0; i < m_dwBlkDataSmall.nBlkCount; ++i )
	{
		if( m_dwBlkDataSmall.pDwList[i].nDataFrom != DW_DATA_FROM_NO )
		{
			++nHasLoad;
			continue;
		}

		PBLOCKDATA pBlkData = m_pBlockDw->m_pDwDataList[m_dwBlkDataSmall.pDwList[i].nDwIdx].pBlockList;
		while( pBlkData != NULL )
		{
			if( pBlkData->nBlockID == m_dwBlkDataSmall.pDwList[i].nBlkID )
			{
				m_dwBlkDataSmall.pDwList[i].nPdwMemIdx = pBlkData->nPdwMassIdx;
				m_dwBlkDataSmall.pDwList[i].nRdwMemIdx = pBlkData->nRdwMassIdx;
				m_dwBlkDataSmall.pDwList[i].nDataFrom = DW_DATA_FROM_COPY;
				++nHasLoad;
				break;
			}
			pBlkData = pBlkData->pNext;
		}
	}
	if( nHasLoad == m_dwBlkDataSmall.nBlkCount )
	{
		return nuTRUE;
	}
	//**Read blocks left from files
	CMapFileBhZ fileBh;
	BHADDRESS bhAddr = {0};
	nuDWORD nBlkCount = 0;
	nuWORD nOldDwIdx = -1;
	nuDWORD* pBlockIDList = NULL;
	//Read Blocs Address from bh file
	for( i = 0; i < m_dwBlkDataSmall.nBlkCount; ++i )
	{
		if( m_dwBlkDataSmall.pDwList[i].nDataFrom != DW_DATA_FROM_NO )
		{
			continue;
		}
		if( nOldDwIdx != m_dwBlkDataSmall.pDwList[i].nDwIdx )
		{
			nBlkCount = fileBh.GetBlockCount(m_dwBlkDataSmall.pDwList[i].nDwIdx);
			if( nBlkCount == 0 )
			{
				continue;
			}
			/*2009.06.16
			if( pBlockIDList != NULL )
			{
				delete []pBlockIDList;
			}
			pBlockIDList = new nuDWORD[nBlkCount];
			if( pBlockIDList == NULL )
			{
				continue;
			}
			if( !fileBh.ReadBlockIDList(pBlockIDList, nBlkCount) )
			{
				delete []pBlockIDList;
				pBlockIDList = NULL;
				continue;
			}
			nOldDwIdx = m_dwBlkDataSmall.pDwList[i].nDwIdx;
			*/
		}
		//2009.06.16
		if( fileBh.ReadBlockAddr( m_dwBlkDataSmall.pDwList[i].nBlkIdx, 
			&bhAddr, 
			nuLOBYTE(nuLOWORD(nDataMode)) ) )
		{
			m_dwBlkDataSmall.pDwList[i].nPdwBlkAddr = bhAddr.nPdwAddr;
			m_dwBlkDataSmall.pDwList[i].nRdwBlkAddr = bhAddr.nRdwAddr;
			m_dwBlkDataSmall.pDwList[i].nBLdwBlkAddr= bhAddr.nBLdwAddr;
			m_dwBlkDataSmall.pDwList[i].nBAdwBlkAddr= bhAddr.nBAdwAddr;
		}
		/*2009.06.16
		for(nuDWORD j = 0; j < nBlkCount; ++j)
		{
			if( pBlockIDList[j] == m_dwBlkDataSmall.pDwList[i].nBlkID )
			{
				if( fileBh.ReadBlockAddr(j, &bhAddr, nuLOBYTE(nuLOWORD(nDataMode))) )
				{
					m_dwBlkDataSmall.pDwList[i].nPdwBlkAddr = bhAddr.nPdwAddr;
					m_dwBlkDataSmall.pDwList[i].nRdwBlkAddr = bhAddr.nRdwAddr;
					m_dwBlkDataSmall.pDwList[i].nBLdwBlkAddr= bhAddr.nBLdwAddr;
					m_dwBlkDataSmall.pDwList[i].nBAdwBlkAddr= bhAddr.nBAdwAddr;
				}
				break;
			}
		}
		*/
	}
	fileBh.CloseFile();
	if( pBlockIDList != NULL )
	{
		delete []pBlockIDList;
		pBlockIDList = NULL;
	}
	//Read dw data..
	CMapFilePdwZ filePdw;
	CMapFileRdwZ fileRdw;
	for( i = 0; i < m_dwBlkDataSmall.nBlkCount; ++i )
	{
		if( m_dwBlkDataSmall.pDwList[i].nDataFrom != DW_DATA_FROM_NO )
		{
			continue;
		}
		if( m_dwBlkDataSmall.pDwList[i].nRdwBlkAddr != -1 )
		{
			fileRdw.ReadRdwBlk(m_dwBlkDataSmall.pDwList[i].nDwIdx, 
				m_dwBlkDataSmall.pDwList[i].nRdwBlkAddr, 
				&m_dwBlkDataSmall.pDwList[i].nRdwMemIdx);
		}
		if( m_dwBlkDataSmall.pDwList[i].nPdwBlkAddr != -1 )
		{
			filePdw.ReadPdwBlk(m_dwBlkDataSmall.pDwList[i].nDwIdx,
				m_dwBlkDataSmall.pDwList[i].nPdwBlkAddr,
				&m_dwBlkDataSmall.pDwList[i].nPdwMemIdx );
		}
		m_dwBlkDataSmall.pDwList[i].nDataFrom = DW_DATA_FROM_READ;
	}
	return nuTRUE;
}

nuBOOL CBlockDwCtrlZ::ColBlockInfoSmall(PBLKINFONEED pBlkInfoNeed, nuINT nCount)
{
	nuINT i = 0;
	for(i = 0; i < nCount; ++i)
	{
		//在上次Load的资料中如果有本次需要的Block资料，标记这些Block节点
		for( nuBYTE j = 0; j < m_dwBlkDataSmall.nBlkCount; ++j )
		{
			if( m_dwBlkDataSmall.pDwList[j].nDwIdx == pBlkInfoNeed[i].nDwIdx &&
				m_dwBlkDataSmall.pDwList[j].nBlkID == pBlkInfoNeed[i].nBlockID )
			{
				m_dwBlkDataSmall.pDwList[j].nCover = 1;
				pBlkInfoNeed[i].nMode	= 1;
				break;
			}
		}
	}
	FreeDwDataSmall(nuFALSE);
	for(i = 0; i < nCount && m_dwBlkDataSmall.nBlkCount < MAX_DW_BLOCKS_SMALL; ++i)
	{
		//把需要新load的Block加到数组中
		if( !pBlkInfoNeed[i].nMode )
		{
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nDwIdx = pBlkInfoNeed[i].nDwIdx;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBlkID = pBlkInfoNeed[i].nBlockID;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBlkIdx= pBlkInfoNeed[i].nBlkIdx;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nPdwBlkAddr	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nRdwBlkAddr	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBLdwBlkAddr	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBAdwBlkAddr	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nPdwMemIdx		= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nRdwMemIdx		= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBLdwMemIdx	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nBAdwMemIdx	= -1;
			m_dwBlkDataSmall.pDwList[m_dwBlkDataSmall.nBlkCount].nDataFrom		= DW_DATA_FROM_NO;
			++m_dwBlkDataSmall.nBlkCount;
		}
	}
	return nuTRUE;
}

nuVOID CBlockDwCtrlZ::SortBlkNodeByDwIdx(PDWBLOCKNODE pDwBlkNode, nuBYTE nCount)
{
	for(nuBYTE i = 0; i < nCount; ++i)
	{
		for(nuBYTE j = i+1; j < nCount; ++j)
		{
			if( pDwBlkNode[i].nDwIdx > pDwBlkNode[j].nDwIdx )
			{
				DWBLOCKNODE blkNode = pDwBlkNode[i];
				pDwBlkNode[i] = pDwBlkNode[j];
				pDwBlkNode[j] = blkNode;
			}
		}
	}
}

nuVOID CBlockDwCtrlZ::SortBlkNodeByBlkID(PDWBLOCKNODE pDwBlkNode, nuBYTE nCount)
{
	for(nuBYTE i = 0; i < nCount; ++i)
	{
		for(nuBYTE j = i+1; j < nCount; ++j)
		{
			if( pDwBlkNode[i].nBlkID > pDwBlkNode[j].nBlkID )
			{
				DWBLOCKNODE blkNode = pDwBlkNode[i];
				pDwBlkNode[i] = pDwBlkNode[j];
				pDwBlkNode[j] = blkNode;
			}
		}
	}
}

nuBOOL CBlockDwCtrlZ::SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut)
{
	return nuTRUE;
}

nuUINT CBlockDwCtrlZ::SeekForCcdInfo(nuroRECT rtMap, PCCDINFO pCcdInfo)
{
	if( NULL == pCcdInfo )
	{
		return 0;
	}
	nuLONG dx = pCcdInfo->ptCarNextCoor.x - pCcdInfo->ptCarMapped.x;
	nuLONG dy = pCcdInfo->ptCarNextCoor.y - pCcdInfo->ptCarMapped.y;
	pCcdInfo->nCcdRdCount = 0;
	pCcdInfo->nDisTotal	= (nuSHORT)nuSqrt(dx*dx + dy*dy);
	if( pCcdInfo->nDisTotal > _CCD_CHECK_MAX_DIS )
	{
		return 1;
	}
	PDWBLKDATASMALL pDwData = (PDWBLKDATASMALL)LoadDwDataSmall(rtMap, DW_DATA_MODE_SMALL | DW_DATA_MODE_RDW);
	if( NULL == pDwData )
	{
		return 0;
	}
	CCDROAD ccdRoad  	= {0};
	ccdRoad.nCcdMapIdx	= pCcdInfo->nCarMapIdx;
	ccdRoad.nCcdBlkIdx	= pCcdInfo->nCarBlkIdx;
	ccdRoad.nCcdRdIdx	= pCcdInfo->nCarRdIdx;
	ccdRoad.ptFirst		= pCcdInfo->ptCarMapped;
	ccdRoad.ptEnd		= pCcdInfo->ptCarNextCoor;
	ccdRoad.nRoadClass	= pCcdInfo->nRoadClass;
	dx = pCcdInfo->ptCarNextCoor.x - pCcdInfo->ptCarMapped.x;
	dy = pCcdInfo->ptCarNextCoor.y - pCcdInfo->ptCarMapped.y;
	nuSHORT nAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
	while( pCcdInfo->nDisTotal < _CCD_CHECK_MAX_DIS )
	{
		if( SeekForCcdRoad( ccdRoad.nCcdMapIdx, 
							ccdRoad.nCcdRdIdx,
							ccdRoad.nCcdBlkIdx,
							nAngle, 
							ccdRoad.nRoadClass,
							ccdRoad.ptEnd,
							pDwData,
							&pCcdInfo->pCcdRoad[ pCcdInfo->nCcdRdCount ] ) )
		{
			ccdRoad	= pCcdInfo->pCcdRoad[ pCcdInfo->nCcdRdCount ];
			++(pCcdInfo->nCcdRdCount);
			if( _CCD_CHECK_MAX_ROADS == pCcdInfo->nCcdRdCount )
			{
				break;
			}
			dx = ccdRoad.ptEnd.x - ccdRoad.ptFirst.x;
			dy = ccdRoad.ptEnd.y - ccdRoad.ptFirst.y;
			nAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
			pCcdInfo->nDisTotal	+= (nuSHORT)nuSqrt(dx*dx + dy*dy);
		}
		else
		{
			break;
		}
	}
	FreeDwDataSmall();
	return 1;
}

nuBOOL CBlockDwCtrlZ::SeekForCcdRoad( nuWORD nMapIdx, 
								    nuWORD nRdIdx, 
									nuDWORD nBlkIdx,
									short	nAngle, 
									nuDWORD nRoadClass,
									nuroPOINT& ptNextCoor, 
									PDWBLKDATASMALL pDwData, 
									PCCDROAD pCcdRoad )
{
	DWROADBLK dwRoadBlk = {0};
	nuBYTE* pData = NULL;
	nuroPOINT ptF = {0}, ptE = {0};
	nuroRECT rtBlock = {0};
	nuroSPOINT sptNext = {0};
	nuBOOL bRes = nuFALSE;
	nuSHORT nOldAngle = 0, nNowAngle = 0, nDifOld = 0, nDifNow = 0;
	nuLONG dx = 0, dy = 0;
	for(nuUINT i = 0; i < pDwData->nBlkCount; ++i)
	{
		pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr( pDwData->pDwList[i].nRdwMemIdx );
		if( NULL == pData )
		{
			continue;
		}
		CMapDataConstructZ::ColRdwBlock(pData, &dwRoadBlk);
		nuBlockIDtoRect(pDwData->pDwList[i].nBlkID, &rtBlock);
		sptNext.x = (nuSHORT)(ptNextCoor.x - rtBlock.left);
		sptNext.y = (nuSHORT)(ptNextCoor.y - rtBlock.top);
		nuBYTE OneWay = 0;
		for(nuDWORD j = 0; j < dwRoadBlk.pDwRoadHead->nRoadCount; ++j)
		{
			nuBOOL bFind = nuFALSE;
			if( sptNext.x == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x &&
				sptNext.y == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y )
			{
				OneWay = 1;
				if( 2 == dwRoadBlk.pDwRoadData[j].nOneWay )
				{
					continue;
				}
				bFind = nuTRUE;
				ptF.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x + rtBlock.left;
				ptF.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y + rtBlock.top;
				ptE.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x + rtBlock.left;
				ptE.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y + rtBlock.top;
			}
			else if( sptNext.x == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x &&
					 sptNext.y == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y )
			{
				OneWay = 2;
				if( 1 == dwRoadBlk.pDwRoadData[j].nOneWay )
				{
					continue;
				}
				bFind = nuTRUE;
				ptE.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x + rtBlock.left;
				ptE.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y + rtBlock.top;
				ptF.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x + rtBlock.left;
				ptF.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y + rtBlock.top;
			}
			if( bFind )
			{
				if( bRes )
				{
					dx = ptE.x - ptF.x;
					dy = ptE.y - ptF.y;
					nNowAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
					nDifNow = nNowAngle - nAngle;
					nDifNow = NURO_ABS(nDifNow);
					if( nDifNow > 180 )
					{
						nDifNow = 360 - nDifNow;
					}
					if(nRoadClass != dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType)
					{
						nDifNow += 20;
					}
					if( nDifNow < nDifOld )
					{
						pCcdRoad->nCcdMapIdx = pDwData->pDwList[i].nDwIdx;
						pCcdRoad->nCcdBlkIdx = pDwData->pDwList[i].nBlkIdx;
						pCcdRoad->nRoadClass = dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType;
						pCcdRoad->nCcdRdIdx	 = (nuWORD)j;
						pCcdRoad->ptFirst	 = ptF;
						pCcdRoad->ptEnd		 = ptE;
						pCcdRoad->nOneWay    = OneWay;
						nDifOld		= nDifNow;
						nOldAngle	= nNowAngle;
					}
				}
				else
				{
					pCcdRoad->nCcdMapIdx = pDwData->pDwList[i].nDwIdx;
					pCcdRoad->nCcdBlkIdx = pDwData->pDwList[i].nBlkIdx;
					pCcdRoad->nRoadClass = dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType;
					pCcdRoad->nCcdRdIdx	 = (nuWORD)j;
					pCcdRoad->ptFirst	 = ptF;
					pCcdRoad->ptEnd		 = ptE;
					pCcdRoad->nOneWay    = OneWay;
					dx = ptE.x - ptF.x;
					dy = ptE.y - ptF.y;
					nOldAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
					nDifOld = nOldAngle - nAngle;
					nDifOld = NURO_ABS(nDifOld);
					if( nDifOld > 180 )
					{
						nDifOld = 360 - nDifOld;
					}
					if(nRoadClass != dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType)
					{
						nDifNow += 20;
					}
				}
				bRes = nuTRUE;
			}
		}
	}
	return bRes;
}
nuUINT CBlockDwCtrlZ::SeekForKMInfo(nuroRECT rtMap, PKMINFO pKMInfo)
{
	if( NULL == pKMInfo )
	{
		return 0;
	}
	nuLONG dx = pKMInfo->ptCarNextCoor.x - pKMInfo->ptCarMapped.x;
	nuLONG dy = pKMInfo->ptCarNextCoor.y - pKMInfo->ptCarMapped.y;
	pKMInfo->nKMRdCount = 0;
	pKMInfo->nDisTotal	= (nuSHORT)nuSqrt(dx*dx + dy*dy);
	if( pKMInfo->nDisTotal > _KM_CHECK_MAX_DIS )
	{
		return 1;
	}
	PDWBLKDATASMALL pDwData = (PDWBLKDATASMALL)LoadDwDataSmall(rtMap, DW_DATA_MODE_SMALL | DW_DATA_MODE_RDW);
	if( NULL == pDwData )
	{
		return 0;
	}
	KMROAD KMRoad		= {0};
	KMRoad.nKMMapIdx	= pKMInfo->nCarMapIdx;
	KMRoad.nKMBlkIdx	= pKMInfo->nCarBlkIdx;
	KMRoad.nKMRdIdx		= pKMInfo->nCarRdIdx;
	KMRoad.ptFirst		= pKMInfo->ptCarMapped;
	KMRoad.ptEnd		= pKMInfo->ptCarNextCoor;
	KMRoad.nRoadClass  = pKMInfo->nRoadClass;
	dx = pKMInfo->ptCarNextCoor.x - pKMInfo->ptCarMapped.x;
	dy = pKMInfo->ptCarNextCoor.y - pKMInfo->ptCarMapped.y;
	nuSHORT nAngle = -1;//(nuSHORT)(nuAtan2(dy, dx) * 180/PI);
	while( pKMInfo->nDisTotal < _KM_CHECK_MAX_DIS )
	{
		if( SeekForKMRoad(  KMRoad.nKMMapIdx, 
							KMRoad.nKMRdIdx,
							KMRoad.nKMBlkIdx,
							-1, 
							KMRoad.nRoadClass,
							KMRoad.ptEnd,
							pDwData,
							&pKMInfo->pKMRoad[ pKMInfo->nKMRdCount ] ) )
		{
			KMRoad	= pKMInfo->pKMRoad[ pKMInfo->nKMRdCount ];
			++(pKMInfo->nKMRdCount);
			if( _KM_CHECK_MAX_ROADS == pKMInfo->nKMRdCount )
			{
				break;
			}
			dx = KMRoad.ptEnd.x - KMRoad.ptFirst.x;
			dy = KMRoad.ptEnd.y - KMRoad.ptFirst.y;
			nAngle = (nuSHORT)(nuAtan2(dy, dx) * 180/PI);
			pKMInfo->nDisTotal	+= (nuSHORT)nuSqrt(dx*dx + dy*dy);
		}
		else
		{
			break;
		}
	}
	FreeDwDataSmall();
	return 1;
}

nuBOOL CBlockDwCtrlZ::SeekForKMRoad( nuWORD nMapIdx, 
								    nuWORD nRdIdx, 
									nuDWORD nBlkIdx,
									nuSHORT	nAngle, 
									nuDWORD nRoadClass,
									nuroPOINT& ptNextCoor, 
									PDWBLKDATASMALL pDwData, 
									PKMROAD pKMRoad )
{
	DWROADBLK dwRoadBlk = {0};
	nuBYTE* pData = NULL;
	nuroPOINT ptF = {0}, ptE = {0};
	nuroRECT rtBlock = {0};
	nuroSPOINT sptNext = {0};
	nuBOOL bRes = nuFALSE;
	//nuSHORT nOldAngle = 0, nNowAngle = 0, nDifOld = 0, nDifNow = 0;
	nuLONG dx = 0, dy = 0;
	for(nuUINT i = 0; i < pDwData->nBlkCount; ++i)
	{
		pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr( pDwData->pDwList[i].nRdwMemIdx );
		if( NULL == pData )
		{
			continue;
		}
		CMapDataConstructZ::ColRdwBlock(pData, &dwRoadBlk);
		nuBlockIDtoRect(pDwData->pDwList[i].nBlkID, &rtBlock);
		sptNext.x = (nuSHORT)(ptNextCoor.x - rtBlock.left);
		sptNext.y = (nuSHORT)(ptNextCoor.y - rtBlock.top);
		for(nuDWORD j = 0; j < dwRoadBlk.pDwRoadHead->nRoadCount; ++j)
		{
			nuBYTE nOneWay = 0;
			nuBOOL bFind = nuFALSE;
			if( sptNext.x == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x &&
				sptNext.y == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y )
			{
				nOneWay = 1;
				if( 2 == dwRoadBlk.pDwRoadData[j].nOneWay )
				{
					continue;
				}
				bFind = nuTRUE;
				ptF.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x + rtBlock.left;
				ptF.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y + rtBlock.top;
				ptE.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x + rtBlock.left;
				ptE.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y + rtBlock.top;
			}
			else if( sptNext.x == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x &&
					 sptNext.y == dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y )
			{
				nOneWay = 2;
				if( 1 == dwRoadBlk.pDwRoadData[j].nOneWay )
				{
					continue;
				}
				bFind = nuTRUE;
				ptE.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].x + rtBlock.left;
				ptE.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx ].y + rtBlock.top;
				ptF.x = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].x + rtBlock.left;
				ptF.y = dwRoadBlk.pSPointCoor[ dwRoadBlk.pDwRoadData[j].nVertexAddrIdx + dwRoadBlk.pDwRoadData[j].nVertexCount - 1 ].y + rtBlock.top;
			}
			if( bFind )
			{				
				if( bRes )
				{
					pKMRoad->nOneWay = nOneWay;
					if(nRoadClass == dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType)
					{
						pKMRoad->nKMMapIdx = pDwData->pDwList[i].nDwIdx;
						pKMRoad->nKMBlkIdx = pDwData->pDwList[i].nBlkIdx;
						pKMRoad->nKMRdIdx	 = (nuWORD)j;
						pKMRoad->ptFirst	 = ptF;
						pKMRoad->ptEnd		 = ptE;
						pKMRoad->nRoadClass = dwRoadBlk.pDwRoadCommon[dwRoadBlk.pDwRoadData[j].nRoadCommonIdx].nRoadType;
					}
				}
				bRes = nuTRUE;
			}
		}
	}
	return bRes;
}