// MapFileFhZ.cpp: implementation of the CMapFileFhZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileFhZ.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileFhZ::CMapFileFhZ()
{
	m_nMapCount = 0;
	m_pFhHead	= NULL;
	m_nMemIdx	= (nuWORD)(-1);
}

CMapFileFhZ::~CMapFileFhZ()
{
	Free();
}

nuBOOL CMapFileFhZ::Initialize()
{
	if( !ReadFhFileHeader() || !ReadFhFileBody() )
	{
		return nuFALSE;
	}
	m_rtBoundary.left	= XBLOCKNUM;
	m_rtBoundary.top	= YBLOCKNUM;
	m_rtBoundary.right	= 0;
	m_rtBoundary.bottom	= 0;
	for(nuWORD i = 0; i < m_nMapCount; ++i)
	{
		if( m_rtBoundary.left > m_pFhHead[i].XBlockStart )
		{
			m_rtBoundary.left = m_pFhHead[i].XBlockStart;
		}
		if( m_rtBoundary.right < m_pFhHead[i].XBlockEnd )
		{
			m_rtBoundary.right = m_pFhHead[i].XBlockEnd;
		}
		if( m_rtBoundary.top > m_pFhHead[i].YBlockStart )
		{
			m_rtBoundary.top = m_pFhHead[i].YBlockStart;
		}
		if( m_rtBoundary.bottom < m_pFhHead[i].YBlockEnd )
		{
			m_rtBoundary.bottom = m_pFhHead[i].YBlockEnd;
		}
	}
	m_rtBoundary.left	-= XBLOCKNUM/2;
	m_rtBoundary.right	-= XBLOCKNUM/2;
	m_rtBoundary.top	-= YBLOCKNUM/2;
	m_rtBoundary.bottom	-= YBLOCKNUM/2;
	m_rtBoundary.left	*= EACHBLOCKSIZE;
	m_rtBoundary.top	*= EACHBLOCKSIZE;
	m_rtBoundary.right	*= EACHBLOCKSIZE;
	m_rtBoundary.bottom *= EACHBLOCKSIZE;
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, nuTEXT(".bnd"));
	if( m_pFsApi->FS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile != NULL )
		{
			NURORECT rtBnd;
			if( nuFread(&rtBnd, sizeof(nuroRECT), 1, pFile) == 1 )
			{
				m_rtBoundary = rtBnd;
			}
			nuFclose(pFile);
		}
	}
	return nuTRUE;
}

nuVOID CMapFileFhZ::Free()
{
	if( m_pMmApi )
	{
	m_pMmApi->MM_RelDataMemMass(&m_nMemIdx);
}

}

nuBOOL CMapFileFhZ::ReadFhFileHeader()
{
	m_nMapCount = m_pFsApi->FS_GetMapCount();
	if( m_nMapCount == 0 )
	{
		return nuFALSE;
	}
	m_pFhHead = (PFHHEADER)m_pMmApi->MM_GetStaticMemMass(m_nMapCount * sizeof(FHHEADER));
	if( m_pFhHead == NULL )
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE *pFile = NULL;
	nuBOOL bRes = nuTRUE;
	m_nXStEdSize	= 0;
	m_nBlkUsedSize	= 0;
	m_nBlkStateSize	= 0;
	for(nuWORD i = 0; i < m_nMapCount; i++)
	{
		nuTcscpy(tsFile, nuTEXT(".FH"));
		if( !m_pFsApi->FS_FindFileWholePath(i, tsFile, NURO_MAX_PATH) )
		{
			bRes = nuFALSE;
			break;
		}
		if( pFile != NULL )
		{
			nuFclose(pFile);
		}
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			bRes = nuFALSE;
			break;
		}
		if( nuFseek(pFile, sizeof(FHVERSION), NURO_SEEK_SET) != 0	||
			nuFread(&m_pFhHead[i], sizeof(FHHEADER), 1, pFile) != 1 )
		{
			bRes = nuFALSE;
			break;
		}
		m_nXStEdSize	+= m_pFhHead[i].YBlockLines * sizeof(FHXSTARTEND);
		m_nBlkUsedSize	+= m_pFhHead[i].YBlockLines * 2;
		m_nBlkStateSize += (m_pFhHead[i].nBlockCount + 7) / 8;
	}
	if( pFile != NULL )
	{
		nuFclose(pFile);
	}
	return bRes;
}

nuBOOL CMapFileFhZ::ReadFhFileBody()
{
	if( m_nMapCount == 0 )
	{
		return nuFALSE;
	}
	nuBYTE* pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMemIdx);
	if( pData == NULL )
	{
		nuDWORD nSize = m_nBlkStateSize + m_nBlkUsedSize + m_nXStEdSize;
		pData = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(nSize, &m_nMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE *pFile = NULL;
	nuBOOL bRes = nuTRUE;
	nuWORD i;
	PFHXSTARTEND pXStEd = (PFHXSTARTEND)pData;
	nuPWORD pBlkUsed = (nuPWORD)(pData + m_nXStEdSize);
	nuPBYTE pBlkState = (nuPBYTE)(pData + m_nXStEdSize + m_nBlkUsedSize);
	for(i = 0; i < m_nMapCount; i++)
	{
		nuTcscpy(tsFile, nuTEXT(".FH"));
		if( !m_pFsApi->FS_FindFileWholePath(i, tsFile, NURO_MAX_PATH) )
		{
			bRes = nuFALSE;
			break;
		}
		if( pFile != NULL )
		{
			nuFclose(pFile);
		}
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			bRes = nuFALSE;
			break;
		}
		if( nuFseek(pFile, sizeof(FHVERSION)+sizeof(FHHEADER), NURO_SEEK_SET) != 0 ||
			nuFread(pXStEd, sizeof(FHXSTARTEND), m_pFhHead[i].YBlockLines, pFile) != m_pFhHead[i].YBlockLines ||
			nuFread(pBlkState, (m_pFhHead[i].nBlockCount + 7) / 8, 1, pFile) != 1 )
		{
			bRes = nuFALSE;
			break;
		}
		nuDWORD nBlkByteIdx = 0;
		nuBYTE nState = 1;
		for(nuWORD j = 0; j < m_pFhHead[i].YBlockLines; ++j)
		{
			pBlkUsed[j] = 0;
			nuINT nXCount = pXStEd[j].nXEnd - pXStEd[j].nXStart + 1;
			for(nuINT k = 0; k < nXCount; ++k)
			{
				if( (pBlkState[nBlkByteIdx] & nState) )
				{
					++pBlkUsed[j];
				}
				nState <<= 1;
				if( !nState )
				{
					nState = 1;
					++nBlkByteIdx;
				}
			}
		}
		pXStEd		+= m_pFhHead[i].YBlockLines;
		pBlkUsed	+= m_pFhHead[i].YBlockLines;
		pBlkState	+= ((m_pFhHead[i].nBlockCount + 7) / 8);
	}
	if( pFile != NULL )
	{
		nuFclose(pFile);
	}
	return bRes;
}

nuBOOL CMapFileFhZ::BlockIDToMap(nuWORD nXBlock, nuWORD nYBlock, PBLK_OUT_DATA pBlkOut)
{
	nuBYTE* pByte = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMemIdx);
	if( pByte == NULL )
	{
		return nuFALSE;
	}
	PFHXSTARTEND pXStEd = (PFHXSTARTEND)pByte;
	nuPWORD pBlkUsed = (nuPWORD)(pByte + m_nXStEdSize);
	nuPBYTE pBlkState = (nuPBYTE)(pByte + m_nXStEdSize + m_nBlkUsedSize);

	pBlkOut->nMapFixed = 0;
	nuBOOL bFind = nuFALSE;
	for (nuWORD i = 0; i < m_nMapCount; ++i )
	{
		if( nXBlock >= m_pFhHead[i].XBlockStart	&&
			nXBlock <= m_pFhHead[i].XBlockEnd	&& 
			nYBlock >= m_pFhHead[i].YBlockStart	&&
			nYBlock <= m_pFhHead[i].YBlockEnd )
		{
			nuWORD nYIdx = nYBlock - m_pFhHead[i].YBlockStart;
			if( nXBlock >= pXStEd[nYIdx].nXStart && nXBlock <= pXStEd[nYIdx].nXEnd )
			{
				nuDWORD nBlkBefor = 0, nBlkNeed = 0;
				nuWORD j;
				for(j = 0; j < nYIdx; ++j )
				{
					if( pXStEd[j].nXEnd >= pXStEd[j].nXStart )
					{
						nBlkBefor += (pXStEd[j].nXEnd - pXStEd[j].nXStart + 1);
						nBlkNeed  += pBlkUsed[j];
					}
				}
				nuDWORD nBlkIdx = nBlkBefor + (nXBlock - pXStEd[nYIdx].nXStart);
				if( pBlkState[nBlkIdx/8] & ((nuBYTE)( 1 << (nBlkIdx%8) )) )
				{
					nBlkIdx = nBlkBefor/8;
					nuBYTE nState = (nuBYTE)(1 << (nBlkBefor%8));
					for( j = pXStEd[j].nXStart; j < nXBlock; ++j )
					{
						if( pBlkState[nBlkIdx] & nState )
						{
							++nBlkNeed;
						}
						nState <<= 1;
						if( !nState )
						{
							nState = 1;
							++nBlkIdx;
						}
					}
					bFind = nuTRUE;
					pBlkOut->pBlkFixData[pBlkOut->nMapFixed].nMapIdx = i;
					pBlkOut->pBlkFixData[pBlkOut->nMapFixed].nBlkIdx = nBlkNeed;
					++pBlkOut->nMapFixed;
					if( pBlkOut->nMapFixed == MAX_MAP_COUNT_OF_ONE_BLK )
					{
						break;
					}
				}
			}
		}
		pXStEd		+= m_pFhHead[i].YBlockLines;
		pBlkUsed	+= m_pFhHead[i].YBlockLines;
		pBlkState	+= (m_pFhHead[i].nBlockCount + 7) / 8;
	}
	return bFind;
}

nuBOOL CMapFileFhZ::PtInBoundary(const nuroPOINT& point)
{
      if( point.x > m_rtBoundary.left-EACHBLOCKSIZE        &&
            point.x < m_rtBoundary.right+EACHBLOCKSIZE      &&
            point.y > m_rtBoundary.top-EACHBLOCKSIZE        &&
            point.y < m_rtBoundary.bottom+EACHBLOCKSIZE          )
      {
            return nuTRUE;
      }
      return nuFALSE;
}
