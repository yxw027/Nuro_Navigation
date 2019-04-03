// MapFileMdwZ.cpp: implementation of the CMapFileMdwZ class.
//
//////////////////////////////////////////////////////////////////////
#include "NuroConstDefined.h"
#include "MapFileMdwZ.h"
#include "MapDataConstructZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileMdwZ::CMapFileMdwZ()
{
//	m_nNowMdwBlkLoaden	= 0;
//	nuMemset(m_pMdwBlkNeed, 0, sizeof(m_pMdwBlkNeed));
	nuMemset(&m_BlockIdLoad, 0, sizeof(m_BlockIdLoad));
	nuMemset(&m_mdwData, 0, sizeof(m_mdwData));
}

CMapFileMdwZ::~CMapFileMdwZ()
{
	free();
}

nuBOOL CMapFileMdwZ::initialize()
{
	return nuTRUE;
}

nuVOID CMapFileMdwZ::free()
{
	RelMdwBlkNotUsed(nuTRUE);
}

nuVOID CMapFileMdwZ::FreeMdwData()
{
	RelMdwBlkNotUsed(nuTRUE);
}

nuPVOID CMapFileMdwZ::LoadMdwData(const nuroRECT& rtMap)
{
	//
	BLOCKIDARRAY blockIDArray = {0};
	nuRectToBlockIDs(rtMap, &blockIDArray, MDW_BLOCK_SIZE);
	/*
	if(  blockIDArray.nXend		== m_BlockIdLoad.nXend		&&
		 blockIDArray.nXstart	== m_BlockIdLoad.nXstart	&&
		 blockIDArray.nYend		== m_BlockIdLoad.nYend		&&
		 blockIDArray.nYstart	== m_BlockIdLoad.nYstart	)
	{
		return &m_mdwData;
	}
	*/
	m_BlockIdLoad	= blockIDArray;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".MDW"));
	if( !m_pFsApi->FS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return NULL;
	}
	nuDWORD nBlockCount = 0;
	if( nuFseek(pFile, 4, NURO_SEEK_SET) != 0 )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	if( nuFread(&nBlockCount, 4, 1, pFile) != 1 )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	nuLONG *pBlkId = new nuLONG[nBlockCount];
	if( pBlkId == NULL )
	{
		FCLOSE_RETURN(pFile, NULL);
	}
	nuLONG *pAddr = new nuLONG[nBlockCount];
	if( pAddr == NULL )
	{
		delete []pBlkId;
		pBlkId=NULL;
		FCLOSE_RETURN(pFile, NULL);
	}
	nuLONG nClsCnt = 0;
	if( nuFread(pBlkId, 4, nBlockCount, pFile) != nBlockCount	||
		nuFread(pAddr, 4, nBlockCount, pFile) != nBlockCount	||
		nuFseek(pFile, nBlockCount*8, NURO_SEEK_CUR ) != 0		||
		nuFread(&nClsCnt, 4, 1, pFile) != 1						||
		!ColMdwBlkNeedList(pBlkId, pAddr, nBlockCount, blockIDArray) )
	{
		delete []pAddr;
		pAddr=NULL;
		delete []pBlkId;
		pBlkId=NULL;
		FCLOSE_RETURN(pFile, NULL);
	}
	//Read mdw Blocks
	delete []pAddr;
	pAddr=NULL;
	delete []pBlkId;
	pBlkId=NULL;
	ReadMdwBlk(pFile);
	m_mdwData.nClassCnt = nuLOBYTE(nuLOWORD(nClsCnt));
	FCLOSE_RETURN(pFile, &m_mdwData);
}

nuBOOL CMapFileMdwZ::ColMdwBlkNeedList(nuLONG *pBlkId, nuLONG *pBlkAddr, nuDWORD nBlkCount, const BLOCKIDARRAY& blkArray)
{
	nuLONG nBlockID =0;
	nuLONG nIdxList[MAX_MDW_BLKS_LOADEN] = {0};
	nuINT nNeedAdd = 0;
	//blocks collection
	nuINT i =0;
	for(i = blkArray.nYstart; i <= blkArray.nYend; ++i)
	{
		for(nuINT j = blkArray.nXstart; j <= blkArray.nXend; ++j)
		{
			if( nNeedAdd == MAX_MDW_BLKS_LOADEN )
			{
				break;
			}
			nBlockID = i * MDW_X_BLOCK_NUM + j;
			for( nuDWORD k = 0; k < nBlkCount; ++k )
			{
				if( pBlkId[k] == nBlockID )
				{
					nuBYTE n;
					for(n = 0; n < m_mdwData.nBlkCount; ++n)
					{
						if( m_mdwData.pMdwNode[n].nBlkID == (nuDWORD)nBlockID )
						{
							m_mdwData.pMdwNode[n].nCover	= 1;
							break;
						}
					}
					if( n == m_mdwData.nBlkCount && pBlkAddr[k] != -1 )
					{
						nIdxList[nNeedAdd++] = k;
					}
					break;
				}
			}
		}
	}
	//remove not used block
	RelMdwBlkNotUsed();
	for(i = 0; i < nNeedAdd && m_mdwData.nBlkCount < MAX_MDW_BLKS_LOADEN; ++i)
	{
		m_mdwData.pMdwNode[m_mdwData.nBlkCount].nCover	= 0;
		m_mdwData.pMdwNode[m_mdwData.nBlkCount].nBlkID	= pBlkId[nIdxList[i]];
		m_mdwData.pMdwNode[m_mdwData.nBlkCount].nMemIdx	= -1;
		m_mdwData.pMdwNode[m_mdwData.nBlkCount].nBlkAddr= pBlkAddr[nIdxList[i]];
		++m_mdwData.nBlkCount;
	}
	return nuTRUE;
}

nuVOID CMapFileMdwZ::RelMdwBlkNotUsed(nuBOOL bAll /* = nuFALSE */)
{
	nuBYTE nDeleteAdd = 0;
	nuBYTE i;
	for(i = 0; i < m_mdwData.nBlkCount; ++i )
	{
		if( bAll || !m_mdwData.pMdwNode[i].nCover )
		{
			++nDeleteAdd;
			m_pMmApi->MM_RelDataMemMass(&m_mdwData.pMdwNode[i].nMemIdx);
			m_mdwData.pMdwNode[i].nCover = 0;
		}
	}
	if( nDeleteAdd < m_mdwData.nBlkCount )
	{
		nuINT jDeleted = -1;
		for(i = 0; i < m_mdwData.nBlkCount; ++i)
		{
			if( m_mdwData.pMdwNode[i].nCover )//Loaden
			{
				if( jDeleted >= 0)
				{
					m_mdwData.pMdwNode[jDeleted++] = m_mdwData.pMdwNode[i];
				}
			}
			else
			{
				if( jDeleted < 0 )
				{
					jDeleted = i;
				}
			}
			m_mdwData.pMdwNode[i].nCover	= 0;
		}
		m_mdwData.nBlkCount	-= nDeleteAdd;
	}
	else
	{
		m_mdwData.nBlkCount = 0;
	}
}

nuBOOL CMapFileMdwZ::ReadMdwBlk(nuFILE *pFile)
{
	MDWHEAD mdwHead  = {0};
	nuLONG nSize = 0;
	for( nuBYTE i = 0; i < m_mdwData.nBlkCount; ++i )
	{
		if( m_pMmApi->MM_GetDataMassAddr(m_mdwData.pMdwNode[i].nMemIdx) == NULL )
		{
			if( nuFseek(pFile, m_mdwData.pMdwNode[i].nBlkAddr, NURO_SEEK_SET) != 0 ||
				nuFread(&mdwHead, sizeof(MDWHEAD), 1, pFile) != 1 )
			{
				continue;
			}
			nSize = sizeof(MDWHEAD) + mdwHead.nRoadCount * sizeof(MDWROAD) + 
				mdwHead.nVertexCount * sizeof(MDWPOINT);
			nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize, &m_mdwData.pMdwNode[i].nMemIdx);
			if( pData == NULL )
			{
				continue;
			}
			nuMemcpy(pData, &mdwHead, sizeof(MDWHEAD));
			pData += sizeof(MDWHEAD);
			nSize -= sizeof(MDWHEAD);
			if( nuFread(pData, nSize, 1, pFile) != 1 )
			{
				m_pMmApi->MM_RelDataMemMass(&m_mdwData.pMdwNode[i].nMemIdx);
				continue;
			}
			//test
//			pData -= sizeof(MDWHEAD);
//			MDWBLKCOL mdwBlk;
//			CMapDataConstructZ::ColMdwBlock(pData, &mdwBlk);
		}//Read Blk data
	}
	return nuTRUE;
}

