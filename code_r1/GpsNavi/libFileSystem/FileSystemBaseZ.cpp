// FileSystemBaseZ.cpp: implementation of the CFileSystemBaseZ class.
//
//////////////////////////////////////////////////////////////////////

#include "FileSystemBaseZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PMEMMGRAPI	CFileSystemBaseZ::m_pMmApi	= NULL;
PFILESYSAPI	CFileSystemBaseZ::m_pFsApi	= NULL;

CFileSystemBaseZ::CFileSystemBaseZ()
{

}

CFileSystemBaseZ::~CFileSystemBaseZ()
{

}

nuBOOL CFileSystemBaseZ::Initialize(PAPISTRUCTADDR pApiAddr)
{
	m_pMmApi	= pApiAddr->pMmApi;
	m_pFsApi	= pApiAddr->pFsApi;
	return true;
}

nuVOID CFileSystemBaseZ::Free()
{
	;
}

nuUINT CFileSystemBaseZ::GetConstStr(int nIdx, nuWCHAR *wsBuff, nuWORD nWsNum)
{
	nuTCHAR sFile[NURO_MAX_PATH] = {0}, sNum[10] = {0};
	nuItot(m_pFsApi->pGdata->uiSetData.nLanguage, sNum, 10);
	nuTcscpy(sFile, m_pFsApi->pGdata->pTsPath);
	nuTcscat(sFile, nuTEXT("Media\\File\\"));
	nuTcscat(sFile, sNum);
	nuTcscat(sFile, nuTEXT("\\ConstStr.txt"));
	nuFILE* pFile = nuTfopen(sFile, NURO_FS_RB);
	if( !pFile )
	{
		return 0;
	}
	nuUINT nNum = 0;
	if( nuFseek(pFile, 2, NURO_SEEK_SET) == 0 )
	{
		nNum = nuFreadLineIndexW(nIdx, wsBuff, nWsNum, pFile);
	}
	nuFclose(pFile);
	return nWsNum;
}