// MapFileSeaZ.cpp: implementation of the CMapFileSeaZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileSeaZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileSeaZ::CMapFileSeaZ()
{
	nuMemset(&m_seaData, 0, sizeof(SEADATA));
	nuMemset(&m_fileHead, 0, sizeof(SEAFILEHEAD));
	nuMemset(&m_BlockIdLoad, 0, sizeof(BLOCKIDARRAY));
	m_pScaleData = NULL;
	m_nScaleValue = 0;
	m_nScaleValue = -1;
	m_nScaleIdx = -1;
}

CMapFileSeaZ::~CMapFileSeaZ()
{
	Free();
}

nuBOOL CMapFileSeaZ::Initialize()
{
	return nuTRUE;
}

nuVOID CMapFileSeaZ::Free()
{
	FreeSeaData();
	if( m_pScaleData != NULL )
	{
		delete []m_pScaleData;
		m_pScaleData = NULL;
	}
}

nuPVOID CMapFileSeaZ::LoadSeaData(nuroRECT rtMap, nuLONG nScaleValue)
{
	nuBOOL bReload = nuFALSE;
	if( m_pScaleData == NULL )
	{
		bReload = nuTRUE;
	}
	else if( m_nScaleValue != nScaleValue )
	{
		bReload = ColScaleIndex(nScaleValue);
	}
	if( m_pScaleData != NULL )
	{
		/**/
		rtMap.left	-= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.right	+= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.top	-= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.bottom+= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		
	}
	if( !bReload )
	{
		BLOCKIDARRAY blockIDArray = {0};
		nuRectToBlockIDs(rtMap, &blockIDArray, m_pScaleData[m_nScaleIdx].nBlockSize);
		if( blockIDArray.nXend	 != m_BlockIdLoad.nXend		||
			blockIDArray.nYend	 != m_BlockIdLoad.nYend		||
			blockIDArray.nXstart != m_BlockIdLoad.nXstart	||
			blockIDArray.nYstart != m_BlockIdLoad.nYstart )
		{
			bReload = nuTRUE;
		}
		else
		{
			for (nuBYTE i = 0; i < m_seaData.nBlkCount; ++i)
			{
				if( m_pMmApi->MM_GetDataMassAddr(m_seaData.pSeaList[i].nMemIdx) == NULL )
				{
					bReload = nuTRUE;
				}
			}
		}
		if( !bReload )
		{
			return &m_seaData;
		}
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".sea"));
	if( !m_pFsApi->FS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( !pFile )
	{
		return NULL;
	}
	//Read FileHead
	if( m_pScaleData == NULL )
	{
		if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFread(&m_fileHead, sizeof(SEAFILEHEAD), 1, pFile) != 1 )
		{
			FCLOSE_RETURN(pFile, NULL);
		}
		m_pScaleData = new SEASCALEDATA[m_fileHead.nScaleCnt];
		if( m_pScaleData == NULL )
		{
			FCLOSE_RETURN(pFile, NULL);
		}
		if( nuFread(m_pScaleData, sizeof(SEASCALEDATA), m_fileHead.nScaleCnt, pFile) != m_fileHead.nScaleCnt )
		{
			delete []m_pScaleData;
			m_pScaleData = NULL;
			FCLOSE_RETURN(pFile, NULL);
		}
		ColScaleIndex(nScaleValue);
		/**/
		rtMap.left	-= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.right	+= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.top	-= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		rtMap.bottom+= m_pScaleData[m_nScaleIdx].nBlockSize/2;
		
	}
	nuRectToBlockIDs(rtMap, &m_BlockIdLoad, m_pScaleData[m_nScaleIdx].nBlockSize);
	//Read SeaData
	if(	!ColSeaNeedData(pFile) )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	if( !ReadSeaData(pFile) )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	m_seaData.seaScaleData = m_pScaleData[m_nScaleIdx];
	FCLOSE_RETURN(pFile, &m_seaData);
}

nuBOOL CMapFileSeaZ::ColScaleIndex(nuLONG nScaleValue)
{
	nuBOOL bReload = nuFALSE;
	m_nScaleValue = nScaleValue;
	nuDWORD i;
	for (i = 0; i < m_fileHead.nScaleCnt - 1; ++i)
	{
		if( m_nScaleValue <= (nuLONG)m_pScaleData[i].nScaleValue )
		{
			break;
		}
	}
	if( m_nScaleIdx != (nuLONG)i )
	{
		bReload = nuTRUE;
		m_nScaleIdx = i;
		RelSeaBlkUnused(nuTRUE);
	}
	return bReload;
}
nuBOOL CMapFileSeaZ::ColSeaNeedData(nuFILE *pFile)
{
	//
	//
	nuLONG nBlockID = 0 ;
	nuLONG nXBlkNum = nuGetXBlkNum(m_pScaleData[m_nScaleIdx].nBlockSize);
	SEABLKLISTNODE pBlkList[MAX_SEA_BLKS_LOADEN]={0};
	nuLONG nNeedAdd = 0;
	nuLONG nAddress = m_pScaleData[m_nScaleIdx].nStartAddress;
	nuLONG i = 0;
	for(i = m_BlockIdLoad.nYstart; i <= m_BlockIdLoad.nYend; ++i)
	{
		for(nuLONG j = m_BlockIdLoad.nXstart; j <= m_BlockIdLoad.nXend; ++j)
		{
			if( nNeedAdd == MAX_SEA_BLKS_LOADEN )
			{
				break;
			}
			if( i < m_pScaleData[m_nScaleIdx].nYStart || i > m_pScaleData[m_nScaleIdx].nYEnd ||
				j < m_pScaleData[m_nScaleIdx].nXStart || j > m_pScaleData[m_nScaleIdx].nXEnd )\
			{
				continue;
			}
			nBlockID = i * nXBlkNum + j;
			nuBYTE k;
			for( k = 0; k < m_seaData.nBlkCount; ++k )
			{
				if( m_seaData.pSeaList[k].nBlkID == (nuDWORD)nBlockID )
				{
					m_seaData.pSeaList[k].nCover = 1;
					break;
				}
			}
			if( k == m_seaData.nBlkCount )
			{
				nuLONG index = (i - m_pScaleData[m_nScaleIdx].nYStart) * (m_pScaleData[m_nScaleIdx].nXEnd - m_pScaleData[m_nScaleIdx].nXStart + 1) + 
					(j - m_pScaleData[m_nScaleIdx].nXStart);
				if (nuFseek(pFile, nAddress + sizeof(SEABLKLISTNODE) * index, NURO_SEEK_SET) == 0 &&
					nuFread(&pBlkList[nNeedAdd], sizeof(SEABLKLISTNODE), 1, pFile) == 1 &&
					pBlkList[nNeedAdd].nBlockAddr != -1)
				{
					pBlkList[nNeedAdd].nBlockSize = nBlockID;
					++nNeedAdd;
				}
			}
		}
	}
	RelSeaBlkUnused(nuFALSE);
	for( i = 0; i < nNeedAdd && i < MAX_SEA_BLKS_LOADEN ; ++i )
	{
		if (m_seaData.nBlkCount >= 64)
		{
			break;
		}
		m_seaData.pSeaList[m_seaData.nBlkCount].nBlkAddress	= pBlkList[i].nBlockAddr;
		m_seaData.pSeaList[m_seaData.nBlkCount].nBlkID	= pBlkList[i].nBlockSize;
		m_seaData.pSeaList[m_seaData.nBlkCount].nMemIdx = -1;
		m_seaData.pSeaList[m_seaData.nBlkCount].nCover	= 0;
		++(m_seaData.nBlkCount);
	}
	return nuTRUE;
}

nuBOOL CMapFileSeaZ::ReadSeaData(nuFILE *pFile)
{
	SEABLKHEAD blkHead = {0};
	nuLONG nSize = 0;
	for( nuBYTE i = 0; i < m_seaData.nBlkCount; ++i )
	{
		m_seaData.pSeaList[i].nCover = 0;
		if( m_pMmApi->MM_GetDataMassAddr(m_seaData.pSeaList[i].nMemIdx) != NULL )
		{
			continue;
		}
		if( nuFseek(pFile, m_seaData.pSeaList[i].nBlkAddress, NURO_SEEK_SET) != 0 ||
			nuFread(&blkHead, sizeof(SEABLKHEAD), 1, pFile) != 1 )
		{
			continue;
		}
		nSize = sizeof(SEABLKHEAD) + sizeof(SEABLKDATA) * blkHead.nSeaAreaCount + 
			sizeof(SEAPOINT) * blkHead.nSeaVertexCount;
		nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize, &m_seaData.pSeaList[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		nuMemcpy(pData, &blkHead, sizeof(SEABLKHEAD));
		pData += sizeof(SEABLKHEAD);
		nSize -= sizeof(SEABLKHEAD);
		if( nuFread(pData, nSize, 1, pFile) != 1 )
		{
			m_pMmApi->MM_RelDataMemMass(&m_seaData.pSeaList[i].nMemIdx);
			continue;
		}
	}
	return nuTRUE;
}

nuVOID CMapFileSeaZ::RelSeaBlkUnused(nuBOOL bAll /* = nuTRUE */)
{
	nuBYTE nDeleted = 0;
	nuBYTE i;
	for(i = 0; i < m_seaData.nBlkCount; ++i )
	{
		if( bAll || !m_seaData.pSeaList[i].nCover )
		{
			++nDeleted;
			m_pMmApi->MM_RelDataMemMass(&m_seaData.pSeaList[i].nMemIdx);
			m_seaData.pSeaList[i].nCover = 0;
		}
	}
	if( nDeleted < m_seaData.nBlkCount )
	{
		nuINT iDelete = -1;
		for( i = 0; i < m_seaData.nBlkCount; ++i )
		{
			if( m_seaData.pSeaList[i].nCover )//Loaden
			{
				if( iDelete >= 0 )
				{
					m_seaData.pSeaList[iDelete++] = m_seaData.pSeaList[i];
				}
				m_seaData.pSeaList[i].nCover = 0;
			}
			else
			{
				if( iDelete < 0 )
				{
					iDelete = i;
				}
			}
		}
		m_seaData.nBlkCount -= nDeleted;
	}
	else
	{
		m_seaData.nBlkCount = 0;
	}
}

nuVOID CMapFileSeaZ::FreeSeaData()
{
	RelSeaBlkUnused(nuTRUE);
}
