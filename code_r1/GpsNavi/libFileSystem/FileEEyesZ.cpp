// FileEEyesZ.cpp: implementation of the CFileEEyesZ class.
//
//////////////////////////////////////////////////////////////////////

#include "FileEEyesZ.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileEEyesZ::CFileEEyesZ()
{
	m_pFile		= NULL;
	m_nMapIdx	= -1;
	nuMemset(&m_fileHead, 0, sizeof(m_fileHead));
}

CFileEEyesZ::~CFileEEyesZ()
{
	CloseFile();
}

nuBOOL CFileEEyesZ::OpenFile()
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".EL"));
	if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile == NULL )
	{
		return nuFALSE;
	}
	if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&m_fileHead.nBlkCount, 4, 1, m_pFile) != 1 )\
	{
		CloseFile();
		return nuFALSE;
	}
	m_fileHead.pIDList = new nuDWORD[m_fileHead.nBlkCount];
	if( m_fileHead.pIDList == NULL )
	{
		CloseFile();
		return nuFALSE;
	}
	if( nuFread(m_fileHead.pIDList, 4, m_fileHead.nBlkCount, m_pFile) != m_fileHead.nBlkCount )//Read CCD BlockID list
	{ 
		CloseFile();
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CFileEEyesZ::CloseFile()
{
	if( m_pFile )
	{
		nuFclose(m_pFile);
		m_pFile		= NULL;
		m_nMapIdx	= -1;
	}
	if( m_fileHead.pIDList )
	{
		delete []m_fileHead.pIDList;
		m_fileHead.pIDList = NULL;
		m_fileHead.nBlkCount = 0;
	}
}

nuBOOL CFileEEyesZ::ReadEEyeBlk(EEYES_BLK& eEyesBlk, nuWORD nDwIdx)
{
	if( m_pFile == NULL || m_nMapIdx != nDwIdx )
	{
		CloseFile();
		m_nMapIdx = nDwIdx;
		if( !OpenFile() )
		{
			return nuFALSE;
		}
	}
	nuBOOL bFind = nuFALSE;
	for(nuDWORD i = 0; i < m_fileHead.nBlkCount; ++i )
	{
		if( m_fileHead.pIDList[i] == eEyesBlk.nBlkID )
		{
			eEyesBlk.nBlkIdx = i;
			eEyesBlk.nDwIdx  = nDwIdx;
			bFind = nuTRUE;
			break;
		}
	}
	if( !bFind )
	{
		return nuFALSE;
	}
	nuLONG nAddress = 0;
	nuDWORD nCount = 0;
	nuLONG nSize = 4 + (m_fileHead.nBlkCount + eEyesBlk.nBlkIdx) * 4;
	if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0			||
		nuFread(&nAddress, sizeof(nuLONG), 1, m_pFile) != 1	||
		nuFseek(m_pFile, nAddress, NURO_SEEK_SET) != 0		||
		nuFread(&nCount, sizeof(nuDWORD), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nSize = sizeof(nuDWORD) + nCount * sizeof(E_EYE_NODE);
	nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize, &eEyesBlk.nMemIdx);
	if( pData == NULL )
	{
		return nuFALSE;
	}
	nuMemcpy(pData, &nCount, sizeof(nuDWORD));
	pData += sizeof(nuDWORD);
	if( nuFread(pData, nSize - sizeof(nuDWORD), 1, m_pFile) != 1 )
	{
		m_pMmApi->MM_RelDataMemMass(&eEyesBlk.nMemIdx);
		return nuFALSE;
	}
	return nuTRUE;
}

nuUINT CFileEEyesZ::GetEEyeTypeName(nuBYTE nType, nuPVOID pBuff, nuUINT nBuffLen)
{
	nuTCHAR tsFile[NURO_MAX_PATH];
	if( !m_pFsApi->FS_GetFilePath(FILE_NAME_EEYETYPE_TEXT, tsFile, sizeof(tsFile)) )
	{
		return 0;
	}
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( !pFile )
	{
		return 0;
	}
	nuUINT nNum = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 )
	{
		nNum = nuFreadLineIndexW(nType, (nuWCHAR*)pBuff, nBuffLen/sizeof(nuWCHAR), pFile);
	}
	nuFclose(pFile);
	return nNum;
}
