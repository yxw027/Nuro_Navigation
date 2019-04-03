// MapFb.cpp: implementation of the CMapFb class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFb.h"
#include "libDrawMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFb::CMapFb()
{
	m_nIdxMapHead	= -1;
	m_nIdxMapBody	= -1;
}

CMapFb::~CMapFb()
{
	Free();
}

nuBOOL CMapFb::Initialize()
{
	m_nIdxMapHead	= -1;
	m_nIdxMapBody	= -1;
	return nuTRUE;
}
nuVOID CMapFb::Free()
{
	g_pDMLibMM->MM_RelDataMemMass(&m_nIdxMapHead);
	g_pDMLibMM->MM_RelDataMemMass(&m_nIdxMapBody);
}

nuBOOL CMapFb::ReadMapFb()
{
	nuBOOL bReadHead = nuFALSE, bReadBody = nuFALSE;
	if( g_pDMLibMM->MM_GetDataMassAddr(m_nIdxMapHead) == NULL )
	{
		bReadHead = nuTRUE;
	}
	if( g_pDMLibMM->MM_GetDataMassAddr(m_nIdxMapBody) == NULL )
	{
		bReadBody = nuTRUE;
	}
	if( !bReadHead && !bReadBody )
	{
		return nuTRUE;
	}
	nuFILE *pfile = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuDWORD nSize = 0;
	PBSD_MAPFB_HEAD pMapFbHead = NULL;
	nuTcscpy(tsFile, nuTEXT("Map.Fb"));
	if( !g_pDMLibFS->FS_FindFileWholePath(-1, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	pfile = nuTfopen(tsFile, NURO_FS_RB);
	if( pfile == NULL )
	{
		return nuFALSE;
	}
	if( bReadHead )
	{
		if( nuFseek(pfile, 0, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pfile);
			return nuFALSE;
		}
		nuWORD nMapCount = 0;
		if( nuFread(&nMapCount, sizeof(nuWORD), 1, pfile) != 1 )
		{
			nuFclose(pfile);
			return nuFALSE;
		}
		nSize = 4 + nMapCount * sizeof(BSD_MAPFB_ID_NODE);
		if( g_pDMLibMM->MM_GetDataMemMass(nSize, &m_nIdxMapHead) == NULL )
		{
			nuFclose(pfile);
			return nuFALSE;
		}
		pMapFbHead = ColMapFbHead(m_nIdxMapHead);
		pMapFbHead->nMapCount	= nMapCount;
		if( nuFread(pMapFbHead->pMapIDNodeList, sizeof(BSD_MAPFB_ID_NODE), nMapCount, pfile) != nMapCount )
		{
			g_pDMLibMM->MM_RelDataMemMass(&m_nIdxMapHead);
			nuFclose(pfile);
			return nuFALSE;
		}
	}
	if( bReadBody )
	{
		pMapFbHead = ColMapFbHead(m_nIdxMapHead);
		nSize = 2 + pMapFbHead->nMapCount * sizeof(BSD_MAPFB_ID_NODE);
		if( nuFseek( pfile, nSize, NURO_SEEK_SET ) != 0 )
		{
			nuFclose(pfile);
			return nuFALSE;
		}
		nuDWORD nFileLen = nuFgetlen(pfile);
		nSize = nFileLen - nSize;
		PBSD_MAPFB_BODY pMapFbBody = (PBSD_MAPFB_BODY)g_pDMLibMM->MM_GetDataMemMass(nSize, &m_nIdxMapBody);
		if( pMapFbBody == NULL )
		{
			nuFclose(pfile);
			return nuFALSE;
		}
		if( nuFread(&pMapFbBody, 1, nSize, pfile) != nSize )
		{
			g_pDMLibMM->MM_RelDataMemMass(&m_nIdxMapBody);
			nuFclose(pfile);
			return nuFALSE;
		}
	}
	nuFclose(pfile);
	return nuTRUE;
}


PBSD_MAPFB_HEAD CMapFb::ColMapFbHead(nuWORD idx)
{
	PBSD_MAPFB_HEAD pMapFbHead = (PBSD_MAPFB_HEAD)g_pDMLibMM->MM_GetDataMassAddr(idx);
	if( pMapFbHead != NULL )
	{
		nuBYTE *pBuff = (nuBYTE*)pMapFbHead;
		pMapFbHead->pMapIDNodeList = (PBSD_MAPFB_ID_NODE)(pBuff + 4);
	}
	return pMapFbHead;
}
PBSD_MAPFB_BODY CMapFb::ColMapFbBody(nuWORD idx)
{
	PBSD_MAPFB_BODY pMapFbBody = (PBSD_MAPFB_BODY)g_pDMLibMM->MM_GetDataMassAddr(idx);
	if( pMapFbBody != NULL )
	{
		nuBYTE *pBuff = (nuBYTE*)pMapFbBody;
		pBuff += 4;
		pMapFbBody->pBsdMapFbScaleHead = pBuff;
	}
	return pMapFbBody;
}