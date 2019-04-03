// MapFileBhZ.cpp: implementation of the CMapFileBhZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileBhZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileBhZ::CMapFileBhZ()
{
	m_pFile = NULL;
}

CMapFileBhZ::~CMapFileBhZ()
{
	CloseFile();
}

nuBOOL CMapFileBhZ::OpenFile()
{	
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".bh"));
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
nuVOID CMapFileBhZ::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuDWORD CMapFileBhZ::GetBlockCount(nuWORD nMapIdx)
{
	if( m_pFile == NULL || nMapIdx != m_nMapIdx )
	{
		m_nMapIdx = nMapIdx;
		if( !OpenFile() )
		{
			return 0;
		}
		//
		if( nuFseek(m_pFile, sizeof(BHSTATICHEAD) - 4, NURO_SEEK_SET) != 0 ||
			nuFread(&m_nBlkCount, 4, 1, m_pFile) != 1 )
		{
			return 0;
		}
	}//reopen the bh file
	return m_nBlkCount;
}

nuBOOL CMapFileBhZ::ReadBlockIDList(nuDWORD* pBlockID, nuDWORD nCount)
{
	if( !m_pFile )
	{
		return nuFALSE;
	}
	if( nuFseek(m_pFile, sizeof(BHSTATICHEAD), NURO_SEEK_SET) != 0 ||
		nuFread(pBlockID, sizeof(nuDWORD), nCount, m_pFile) != nCount )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileBhZ::ReadBlockAddr(nuDWORD nBlockIdx, PBHADDRESS pBlockAddr, nuBYTE nMode /* = -1 */)
{
	if( !m_pFile )
	{
		return nuFALSE;
	}
	pBlockAddr->nPdwAddr = -1;
	pBlockAddr->nRdwAddr = -1;
	pBlockAddr->nBLdwAddr = -1;
	pBlockAddr->nBAdwAddr = -1;
	nuLONG nSize;
	if( nMode & DW_DATA_MODE_PDW )
	{
		nSize = sizeof(BHSTATICHEAD) + 4 * m_nBlkCount + 4 * nBlockIdx;
		if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
			nuFread(&pBlockAddr->nPdwAddr, 4, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	if( nMode & DW_DATA_MODE_RDW )
	{
		nSize = sizeof(BHSTATICHEAD) + 12 * m_nBlkCount + 4 * nBlockIdx;
		if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
			nuFread(&pBlockAddr->nRdwAddr, 4, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	if( nMode & DW_DATA_MODE_BLDW )
	{
		nSize = sizeof(BHSTATICHEAD) + 24 * m_nBlkCount + 4 * nBlockIdx;
		if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
			nuFread(&pBlockAddr->nBLdwAddr, 4, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	if( nMode & DW_DATA_MODE_BADW )
	{
		nSize = sizeof(BHSTATICHEAD) + 32 * m_nBlkCount + 4 * nBlockIdx;
		if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
			nuFread(&pBlockAddr->nBAdwAddr, 4, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}
