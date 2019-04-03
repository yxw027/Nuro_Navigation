// MapFilePdwZ.cpp: implementation of the CMapFilePdwZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFilePdwZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFilePdwZ::CMapFilePdwZ()
{
	m_pFile = NULL;
	m_nMapIdx = -1;
}

CMapFilePdwZ::~CMapFilePdwZ()
{

}

nuVOID CMapFilePdwZ::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuBOOL CMapFilePdwZ::OpenFile()
{
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, nuTEXT(".pdw"));
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

nuBOOL CMapFilePdwZ::ReadPdwBlk(nuWORD nDwIdx, nuDWORD nBlkAddr, nuPWORD pMemIdx)
{
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
	DWPOIHEAD dwPoiHead;
	if( nuFread(&dwPoiHead, sizeof(DWPOIHEAD), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuLONG nSize = sizeof(DWPOIHEAD) + dwPoiHead.nPOICount * sizeof(DWPOIDATA);
	if( m_pMmApi->MM_GetDataMemMass(nSize, pMemIdx) == NULL )
	{
		return nuFALSE;
	}
	nuBOOL bres = nuTRUE;
	m_pMmApi->MM_LockDataMemMove();
	nuPBYTE pMem = (nuPBYTE)m_pMmApi->MM_GetDataMassAddr(*pMemIdx);
	nuMemcpy(pMem, &dwPoiHead, sizeof(DWPOIHEAD));
	pMem	+= sizeof(DWPOIHEAD);
	nSize	-= sizeof(DWPOIHEAD);
	if( nuFread(pMem, nSize, 1, m_pFile) != 1 )
	{
		bres = nuFALSE;
		m_pMmApi->MM_RelDataMemMass(pMemIdx);
	}
	m_pMmApi->MM_FreeDataMemMove();
	return bres;
}
