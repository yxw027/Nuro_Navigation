// MapFileRdwZ.cpp: implementation of the CMapFileRdwZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileRdwZ.h"
#include "MapDataConstructZ.h"
#include "MapFileBhZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileRdwZ::CMapFileRdwZ()
{
	m_nMapIdx = -1;
	m_pFile = NULL;
}

CMapFileRdwZ::~CMapFileRdwZ()
{
	CloseFile();
}

nuVOID CMapFileRdwZ::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuBOOL CMapFileRdwZ::OpenFile()
{
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, nuTEXT(".Rdw"));
	if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileRdwZ::ReadRdwBlk(nuWORD nDwIdx, nuDWORD nBlkAddr, nuPWORD pMemIdx)
{
	if( nDwIdx == (nuWORD)(-1) || nBlkAddr == (nuDWORD)(-1) || pMemIdx == NULL )
	{
		return nuFALSE;
	}
	if( m_pFile == NULL || m_nMapIdx != nDwIdx )
	{
		m_nMapIdx = nDwIdx;
		if( !OpenFile() )
		{
			return nuFALSE;
		}
	}
	if( nuFseek(m_pFile, nBlkAddr, NURO_SEEK_SET) != 0 )
	{
		return nuFALSE;
	}
	DWROADHEAD dwRoadHead;
	if( nuFread(&dwRoadHead, sizeof(DWROADHEAD), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuLONG nSize;
	nuPBYTE pMem;
	nSize = sizeof(DWROADHEAD) + dwRoadHead.nRoadCount * sizeof(DWROADDATA) + 
		dwRoadHead.nRoadCommonCount * sizeof(DWROADCOMMON) + 
		dwRoadHead.nVertexCount * sizeof(NUROSPOINT);//Col Rdw Block size;
	pMem = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize+dwRoadHead.nRoadCount*sizeof(NUROSRECT), pMemIdx);
	if( pMem == NULL )
	{
		return nuFALSE;
	}
	nuMemcpy(pMem, &dwRoadHead, sizeof(DWROADHEAD));
	pMem += sizeof(DWROADHEAD);
	if( nuFread(pMem, nSize-sizeof(DWROADHEAD), 1, m_pFile) != 1 )
	{
		m_pMmApi->MM_RelDataMemMass(pMemIdx);
		return nuFALSE;
	}
	pMem -= sizeof(DWROADHEAD);
	DWROADBLK roadBlk;
	CMapDataConstructZ::ColRdwBlock(pMem, &roadBlk);
	for(nuDWORD n = 0; n < roadBlk.pDwRoadHead->nRoadCount; n++)
	{
		roadBlk.pRoadRect[n].left = roadBlk.pRoadRect[n].right = 
			roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].x;
		roadBlk.pRoadRect[n].top = roadBlk.pRoadRect[n].bottom = 
			roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].y;
		for(nuDWORD j = 1; j < roadBlk.pDwRoadData[n].nVertexCount; j++)
		{
			if( roadBlk.pRoadRect[n].left > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x )
			{
				roadBlk.pRoadRect[n].left = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x;
			}
			else if( roadBlk.pRoadRect[n].right < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x )
			{
				roadBlk.pRoadRect[n].right = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x;
			}
			//
			if( roadBlk.pRoadRect[n].top > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y )
			{
				roadBlk.pRoadRect[n].top = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y;
			}
			else if( roadBlk.pRoadRect[n].bottom < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y )
			{
				roadBlk.pRoadRect[n].bottom = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CMapFileRdwZ::GetRoadSegData(nuWORD nDwIdx, PROAD_SEG_DATA pRoadSegData)
{
	if( pRoadSegData == NULL || pRoadSegData->nCountUsed == 0 || pRoadSegData->pRoadSeg == NULL)
	{
		return nuFALSE;
	}
	CMapFileBhZ		fileBh;
	if( !fileBh.GetBlockCount(nDwIdx) )
	{
		return nuFALSE;
	}
	if( m_pFile == NULL || m_nMapIdx != nDwIdx )
	{
		m_nMapIdx = nDwIdx;
		if( !OpenFile() )
		{
			return nuFALSE;
		}
	}
	BHADDRESS bhAddress;
	DWROADHEAD dwRoadHead;
	DWROADDATA dwRoadData;
	DWROADCOMMON dwRoadCommon;
	for( nuBYTE i = 0; i < pRoadSegData->nCountUsed; ++i )
	{
		pRoadSegData->pRoadSeg[i].nNameAddress = -1;
		if( !fileBh.ReadBlockAddr(pRoadSegData->pRoadSeg[i].nBlockIdx, &bhAddress, DW_DATA_MODE_RDW) ||
			bhAddress.nRdwAddr == -1 )
		{
			continue;
		}
		if( nuFseek(m_pFile, bhAddress.nRdwAddr, NURO_SEEK_SET)	!= 0 		||
			nuFread(&dwRoadHead, sizeof(DWROADHEAD), 1, m_pFile) != 1		||
			pRoadSegData->pRoadSeg[i].nRoadIdx >= dwRoadHead.nRoadCount			||
			nuFseek(m_pFile, sizeof(DWROADDATA) * pRoadSegData->pRoadSeg[i].nRoadIdx, NURO_SEEK_CUR) != 0 || 
			nuFread(&dwRoadData, sizeof(DWROADDATA), 1, m_pFile) != 1 )
		{
			continue;
		}
		bhAddress.nRdwAddr += sizeof(DWROADHEAD) + sizeof(DWROADDATA) * dwRoadHead.nRoadCount +
			sizeof(DWROADCOMMON) * dwRoadData.nRoadCommonIdx;
		if( nuFseek(m_pFile, bhAddress.nRdwAddr, NURO_SEEK_SET) != 0 ||
			nuFread(&dwRoadCommon, sizeof(DWROADCOMMON), 1, m_pFile) != 1 )
		{
			continue;
		}
		pRoadSegData->pRoadSeg[i].nNameAddress = dwRoadCommon.nRoadNameAddr;
	}
	return nuTRUE;
}
