// FilePn.cpp: implementation of the CFilePn class.
//
//////////////////////////////////////////////////////////////////////

#include "FilePn.h"
#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePn::CFilePn()
{

}

CFilePn::~CFilePn()
{
	Free();
}

nuBOOL CFilePn::Initialize()
{
	return nuTRUE;
}

nuVOID CFilePn::Free()
{
}

nuBOOL CFilePn::GetPoiName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuPWORD pBufLen)
{
	if( nDwIdx == -1 || pNameBuf == NULL || nNameAddr == -1 )
	{
		return nuFALSE;
	}
	nuFILE *pFile = NULL;
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuBYTE	nNameLen = 0;
	nuTcscpy(sFile, nuTEXT(".PN"));
	if( !LibFS_FindFileWholePath(nDwIdx, sFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	pFile = nuTfopen(sFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	if( nuFseek(pFile, nNameAddr, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&nNameLen, 1, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
//	if( nNameLen > *pBufLen )
//	{
//		nuFclose(pFile);
//		return nuFALSE;
//	}
	nuMemset(pNameBuf, 0, *pBufLen);
	if( nuFread(pNameBuf, 1, nNameLen, pFile) != nNameLen )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	*pBufLen = nNameLen;
	nuFclose(pFile);
	return nuTRUE;
}
