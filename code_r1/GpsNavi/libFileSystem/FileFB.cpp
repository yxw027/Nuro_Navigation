// FileFB.cpp: implementation of the CFileFB class.
//
//////////////////////////////////////////////////////////////////////

#include "FileFB.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"
#include "NuroClasses.h"

extern PMEMMGRAPI	g_pFsMmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileFB::CFileFB()
{
	m_pDwBlockIDSeg = NULL;
	m_nDwCount		= 0;
}

CFileFB::~CFileFB()
{
	Free();
}

nuBOOL CFileFB::Initialize()
{
	m_nDwCount = LibFS_GetMapCount();
	if( m_nDwCount == 0 )
	{
		return nuFALSE;
	}
	m_pDwBlockIDSeg = (PDWBLOCKIDSEG)g_pFsMmApi->MM_GetStaticMemMass(m_nDwCount*sizeof(DWBLOCKIDSEG));
	if( m_pDwBlockIDSeg == NULL )
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE *pFile = NULL;
	nuDWORD nSize = 0;
	for(nuWORD i = 0; i < m_nDwCount; i++)
	{
		m_pDwBlockIDSeg[i].pBlockIDSeg	= NULL;
		m_pDwBlockIDSeg[i].nSegCount	= 0;
		nuMemset(&m_pDwBlockIDSeg[i].rtBoundary, 0, sizeof(NURORECT));
		nuTcscpy(tsFile, nuTEXT(".FB"));
		if( !LibFS_FindFileWholePath(i, tsFile, NURO_MAX_PATH) )
		{
			continue;
		}
		if( pFile != NULL )
		{
			nuFclose(pFile);
		}
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			continue;
		}
		if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
		{
			continue;
		}
		if( nuFread(&m_pDwBlockIDSeg[i].nSegCount, sizeof(nuDWORD), 1, pFile) != 1 )
		{
			continue;
		}
		
		nSize = m_pDwBlockIDSeg[i].nSegCount * sizeof(BLOCKIDSEG);
		m_pDwBlockIDSeg[i].pBlockIDSeg = (PBLOCKIDSEG)g_pFsMmApi->MM_GetStaticMemMass(nSize);
		if( m_pDwBlockIDSeg[i].pBlockIDSeg == NULL )
		{
			continue;
		}
		m_rtBoundary.left	= 0;
		m_rtBoundary.top	= 0;
		m_rtBoundary.right	= 0;
		m_rtBoundary.bottom	= 0;
		nuBOOL bFBound = nuTRUE;
		if( nuFread(m_pDwBlockIDSeg[i].pBlockIDSeg, nSize, 1, pFile) == 1 )
		{
			//Calculate the boundary of the dw
			nuBOOL bFirst = nuTRUE;
			nuroRECT rect = {0};
			for(nuDWORD n = 0; n < m_pDwBlockIDSeg[i].nSegCount; n++)
			{
				for(nuDWORD m = m_pDwBlockIDSeg[i].pBlockIDSeg[n].nFromID; m <= m_pDwBlockIDSeg[i].pBlockIDSeg[n].nToID; m++)
				{
					nuBlockIDtoRect(m, &rect);
					if( bFBound )
					{
						bFBound = nuFALSE;
						m_rtBoundary = rect;
					}
					else
					{
						if( m_rtBoundary.left > rect.left )
						{
							m_rtBoundary.left = rect.left;
						}
						if( m_rtBoundary.right < rect.right )
						{
							m_rtBoundary.right = rect.right;
						}
						if( m_rtBoundary.top > rect.top )
						{
							m_rtBoundary.top = rect.top;
						}
						if( m_rtBoundary.bottom < rect.bottom )
						{
							m_rtBoundary.bottom = rect.bottom;
						}
					}
					if( bFirst )
					{
						m_pDwBlockIDSeg[i].rtBoundary = rect;
						bFirst = nuFALSE;
					}
					else
					{
						if( m_pDwBlockIDSeg[i].rtBoundary.left > rect.left )
						{
							m_pDwBlockIDSeg[i].rtBoundary.left = rect.left;
						}
						if( m_pDwBlockIDSeg[i].rtBoundary.right < rect.right )
						{
							m_pDwBlockIDSeg[i].rtBoundary.right = rect.right;
						}
						if( m_pDwBlockIDSeg[i].rtBoundary.top > rect.top )
						{
							m_pDwBlockIDSeg[i].rtBoundary.top = rect.top;
						}
						if( m_pDwBlockIDSeg[i].rtBoundary.bottom < rect.bottom )
						{
							m_pDwBlockIDSeg[i].rtBoundary.bottom = rect.bottom;
						}
					}
				}
			}
		}
	}
	if( pFile != NULL )
	{
		nuFclose(pFile);
		pFile = NULL;
	}
	nuTcscpy(tsFile, nuTEXT(".bnd"));
	if( LibFS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		pFile = nuTfopen(tsFile, NURO_FS_RB);
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
nuVOID CFileFB::Free()
{
	m_pDwBlockIDSeg	= NULL;
	m_nDwCount		= 0;
}

nuBOOL CFileFB::FindBlockID(nuDWORD nBlockID, nuPWORD pDwIdx)
{
	nuroRECT rect = {0};
	nuBlockIDtoRect(nBlockID, &rect);
	for( nuWORD i = 0; i < m_nDwCount; i ++ )
	{
		if( m_pDwBlockIDSeg[i].pBlockIDSeg == NULL )
		{
			continue;
		}
		if( nuRectCoverRect(&rect, &m_pDwBlockIDSeg[i].rtBoundary) )
		{
			for(nuDWORD j = 0; j < m_pDwBlockIDSeg[i].nSegCount; j++)
			{
				if( nBlockID >= m_pDwBlockIDSeg[i].pBlockIDSeg[j].nFromID &&
					nBlockID <= m_pDwBlockIDSeg[i].pBlockIDSeg[j].nToID )
				{
					*pDwIdx = i;
					return nuTRUE;
				}
			}
		}
	}
	return nuFALSE;
}
nuBOOL CFileFB::BlockIDInMap(nuDWORD nBlockID, nuWORD nMapIdx)
{
	if ( nMapIdx >= m_nDwCount )
	{
		return nuFALSE;
	}
	nuroRECT rect;
	nuBlockIDtoRect(nBlockID, &rect);
	if( m_pDwBlockIDSeg[nMapIdx].pBlockIDSeg == NULL )
	{
		return nuFALSE;
	}
	if( nuRectCoverRect(&rect, &m_pDwBlockIDSeg[nMapIdx].rtBoundary) )
	{
		for(nuDWORD j = 0; j < m_pDwBlockIDSeg[nMapIdx].nSegCount; j++)
		{
			if( nBlockID >= m_pDwBlockIDSeg[nMapIdx].pBlockIDSeg[j].nFromID &&
				nBlockID <= m_pDwBlockIDSeg[nMapIdx].pBlockIDSeg[j].nToID )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuBOOL CFileFB::PtInBoundary(nuroPOINT pt)
{
	/*
	for(nuWORD i = 0; i < m_nDwCount; i++ )
	{
		if( pt.x > m_pDwBlockIDSeg[i].rtBoundary.left	&&
			pt.x < m_pDwBlockIDSeg[i].rtBoundary.right	&&
			pt.y > m_pDwBlockIDSeg[i].rtBoundary.top	&&
			pt.y < m_pDwBlockIDSeg[i].rtBoundary.bottom	)
		{
			return nuTRUE;
		}
	}
	*/
	if( pt.x > m_rtBoundary.left	&&
		pt.x < m_rtBoundary.right	&&
		pt.y > m_rtBoundary.top		&&
		pt.y < m_rtBoundary.bottom	)
	{
		return nuTRUE;
	}
	return nuFALSE;
}