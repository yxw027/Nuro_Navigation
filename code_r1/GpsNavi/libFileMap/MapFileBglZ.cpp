// MapFileBglZ.cpp: implementation of the CMapFileBglZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileBglZ.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileBglZ::CMapFileBglZ()
{
	nuMemset(&m_bglData, 0, sizeof(BGLDATA));
	m_nScaleValue = 0;
	nuMemset(&m_BlockIdLoad, 0, sizeof(BLOCKIDARRAY));
	nuMemset(&m_fileHead, 0, sizeof(BGLFILEHEAD));
}

CMapFileBglZ::~CMapFileBglZ()
{
}

nuBOOL CMapFileBglZ::Initialize()
{
	for( nuBYTE i = 0; i < MAX_BGL_BLKS_LOADEN; ++i )
	{
		m_bglData.pBglNode[i].nMemIdx = -1;
	}
	return nuTRUE;
}

nuVOID CMapFileBglZ::Free()
{
	FreeBglData();
}

nuPVOID CMapFileBglZ::LoadBglData(nuroRECT rtMap, nuLONG nScaleValue, nuLONG nBGLStartScale /* = 5000 */)
{
	BLOCKIDARRAY blockIDArray = {0};
	nuBOOL bReload = nuFALSE;
	if( m_nScaleValue != nScaleValue )
	{
		bReload = nuTRUE;
		m_nScaleValue = nScaleValue;
		nuLONG nOldBlkSize = m_bglData.nBlkSize;
		if( m_nScaleValue > BGL_SCALE_50K_START )
		{
			m_bglData.nBlkSize = BGL_BLOCK_SIZE;
			m_pTcsFile = _BGL_50K_NAME;
		}
		else if(m_nScaleValue >= nBGLStartScale )
		{
			m_bglData.nBlkSize = BGL_BLOCK_SIZE_1;
			m_pTcsFile = _BGL_5k_NAME;
		}
		else
		{
			FreeBglData();
			return NULL;
		}
		if( nOldBlkSize != m_bglData.nBlkSize )
		{
			m_fileHead.nTotalCount = 0;
		}
		rtMap.left	-= m_bglData.nBlkSize / 2;
		rtMap.top	-= m_bglData.nBlkSize / 2;
		rtMap.right += m_bglData.nBlkSize / 2;
		rtMap.bottom+= m_bglData.nBlkSize / 2;
	}
	else
	{
		rtMap.left	-= m_bglData.nBlkSize / 2;
		rtMap.top	-= m_bglData.nBlkSize / 2;
		rtMap.right += m_bglData.nBlkSize / 2;
		rtMap.bottom+= m_bglData.nBlkSize / 2;
		nuRectToBlockIDs(rtMap, &blockIDArray, m_bglData.nBlkSize);
		if( blockIDArray.nXend	 != m_BlockIdLoad.nXend		||
			blockIDArray.nYend	 != m_BlockIdLoad.nYend		||
			blockIDArray.nXstart != m_BlockIdLoad.nXstart	||
			blockIDArray.nYstart != m_BlockIdLoad.nYstart )
		{
			bReload = nuTRUE;
		}
		else
		{
			for (nuBYTE i = 0; i < m_bglData.nBlkCount; ++i)
			{
				if( !m_pMmApi->MM_GetDataMassAddr(m_bglData.pBglNode[i].nMemIdx) )
				{
					bReload = nuTRUE;
					break;
				}
			}
		}
	}
	if( !bReload )
	{
		m_bglData.bDataChanged = 0;
		return &m_bglData;
	}
	m_bglData.bDataChanged = 1;
	//
	nuRectToBlockIDs(rtMap, &m_BlockIdLoad, m_bglData.nBlkSize);
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	if( !m_pFsApi->FS_GetFilePath(FILE_NAME_PATH, tsFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	nuTcscat(tsFile, m_pTcsFile);
	/*
	if( !m_pFsApi->FS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	*/
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return NULL;
	}
	if( m_fileHead.nTotalCount == 0 )
	{
		if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
		{
			FCLOSE_RETURN(pFile, NULL);
		}
		if( nuFread(&m_fileHead, sizeof(BGLFILEHEAD), 1, pFile) != 1 )
		{
			FCLOSE_RETURN(pFile, NULL);
		}
		m_bglData.nDivisor = m_fileHead.nDivisor;
	}
	else
	{
		if( nuFseek(pFile, sizeof(BGLFILEHEAD), NURO_SEEK_SET) != 0 )
		{
			FCLOSE_RETURN(pFile, NULL);
		}
	}
	nuLONG *pBlockID = new nuLONG[m_fileHead.nTotalCount];
	if( pBlockID == NULL )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	nuLONG *pBlockAddr = new nuLONG[m_fileHead.nTotalCount];
	if( pBlockAddr == NULL )
	{
		delete []pBlockID;
		pBlockID=NULL;
		FCLOSE_RETURN(pFile, NULL);
	}
	nuDWORD nClassCnt = 0;
	if( nuFread(pBlockID, 4, m_fileHead.nTotalCount, pFile) != m_fileHead.nTotalCount	||
		nuFread(pBlockAddr, 4, m_fileHead.nTotalCount, pFile) != m_fileHead.nTotalCount	||
		nuFseek(pFile, 8*m_fileHead.nTotalCount, NURO_SEEK_CUR) != 0					||
		nuFread(&nClassCnt, 4, 1, pFile) != 1 ||
		!ColBglNeedData(pBlockID, pBlockAddr)  )
	{
		delete []pBlockAddr;
		pBlockAddr=NULL;
		delete []pBlockID;
		pBlockID=NULL;
		FCLOSE_RETURN(pFile, NULL);
	}
	delete []pBlockAddr;
	pBlockAddr=NULL;
	delete []pBlockID;
	pBlockID=NULL;
	m_bglData.nClsCnt = (nuBYTE)nClassCnt;
	ReadBglData(pFile);
	FCLOSE_RETURN(pFile, &m_bglData);
}

nuBOOL CMapFileBglZ::ColBglNeedData(nuLONG *pBlkId, nuLONG *pBlkAddr)
{
	nuLONG nBlockID = 0;
	nuLONG nXBlkNum = nuGetXBlkNum(m_bglData.nBlkSize);
	nuLONG nListIdx[MAX_BGL_BLKS_LOADEN] = {0};
	nuLONG nNeedAdd = 0;
	nuLONG i = 0;
	for(i = m_BlockIdLoad.nYstart; i <= m_BlockIdLoad.nYend; ++i)
	{
		for(nuLONG j = m_BlockIdLoad.nXstart; j <= m_BlockIdLoad.nXend; ++j)
		{
			if( nNeedAdd == MAX_BGL_BLKS_LOADEN )
			{
				break;
			}
			nBlockID = i*nXBlkNum + j;
			for( nuDWORD k = 0; k < m_fileHead.nTotalCount; ++k )
			{
				if( nBlockID == pBlkId[k] )
				{
					if( pBlkAddr[k] <= 0 )
					{
						break;
					}
					nuBYTE n;
					for(n = 0; n < m_bglData.nBlkCount; ++n)
					{
						if( m_bglData.pBglNode[n].nBlkID == (nuDWORD)nBlockID )
						{
							m_bglData.pBglNode[n].nCover = 1;
							break;
						}
					}
					if( n == m_bglData.nBlkCount && pBlkAddr[k] != -1 )
					{
						nListIdx[nNeedAdd++] = k;
					}
					break;
				}
			}
		}
	}
	RelBglBlkUnused(nuFALSE);
	for( i = 0; i < nNeedAdd && m_bglData.nBlkCount < MAX_BGL_BLKS_LOADEN; ++i )
	{
		m_bglData.pBglNode[m_bglData.nBlkCount].nCover		= 0;
		m_bglData.pBglNode[m_bglData.nBlkCount].nBlkID		= pBlkId[ nListIdx[i] ];
		m_bglData.pBglNode[m_bglData.nBlkCount].nMemIdx		= -1;
		m_bglData.pBglNode[m_bglData.nBlkCount].nBlkAddress	= pBlkAddr[nListIdx[i]];
		++(m_bglData.nBlkCount);
	}
	return nuTRUE;
}

nuVOID CMapFileBglZ::RelBglBlkUnused(nuBOOL bAll /* = nuTRUE */)
{
	nuBYTE nDeleteAdd = 0;
	nuBYTE i;
	for(i = 0; i < m_bglData.nBlkCount; ++i )
	{
		if( bAll || !m_bglData.pBglNode[i].nCover )
		{
			++nDeleteAdd;
			m_pMmApi->MM_RelDataMemMass(&m_bglData.pBglNode[i].nMemIdx);
			m_bglData.pBglNode[i].nCover = 0;
		}
	}
	if( nDeleteAdd != 0 )
	{
		if( nDeleteAdd < m_bglData.nBlkCount )
		{
			nuINT jDeleted = -1;
			for(i = 0; i < m_bglData.nBlkCount; ++i)
			{
				if( m_bglData.pBglNode[i].nCover )//Loaden
				{
					m_bglData.pBglNode[i].nCover = 0;
					if( jDeleted >= 0)
					{
						m_bglData.pBglNode[jDeleted++] = m_bglData.pBglNode[i];
					}
				}
				else
				{
					if( jDeleted < 0 )
					{
						jDeleted = i;
					}
				}
			}
			m_bglData.nBlkCount	-= nDeleteAdd;
		}
		else
		{
			m_bglData.nBlkCount = 0;
		}
	}
}

nuBOOL CMapFileBglZ::ReadBglData(nuFILE *pFile)
{
	BGLBLKHEAD bglBlkHead  = {0};
	nuLONG nSize = 0;
	for(nuBYTE i = 0; i < m_bglData.nBlkCount; ++i)
	{
		m_bglData.pBglNode[i].nCover = 0;
		if( m_pMmApi->MM_GetDataMassAddr(m_bglData.pBglNode[i].nMemIdx) != NULL )
		{
			continue;
		}
		if( nuFseek(pFile, m_bglData.pBglNode[i].nBlkAddress, NURO_SEEK_SET) != 0 ||
			nuFread(&bglBlkHead, sizeof(BGLBLKHEAD), 1, pFile) != 1 ||
			bglBlkHead.nBglCount == 0 )
		{
			continue;
		}
		nSize = sizeof(BGLBLKHEAD) + sizeof(BGLBLK) * bglBlkHead.nBglCount + 
			sizeof(BGLPOINT) * bglBlkHead.nVertexCount;
		nuBYTE *pData = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(nSize, &m_bglData.pBglNode[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		nuMemcpy(pData, &bglBlkHead, sizeof(BGLBLKHEAD));
		pData += sizeof(BGLBLKHEAD);
		nSize -= sizeof(BGLBLKHEAD);
		if( nuFread(pData, nSize, 1, pFile) != 1 )
		{
			m_pMmApi->MM_RelDataMemMass(&m_bglData.pBglNode[i].nMemIdx);
			continue;
		}
	}
	return nuTRUE;
}

nuVOID CMapFileBglZ::FreeBglData()
{
	RelBglBlkUnused(nuTRUE);
	m_nScaleValue	= 0;
	m_bglData.nBlkSize = 0;
	nuMemset(&m_BlockIdLoad, 0, sizeof(BLOCKIDARRAY));
}
