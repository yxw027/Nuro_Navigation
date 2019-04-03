// FilePa.cpp: implementation of the CFilePa class.
//
//////////////////////////////////////////////////////////////////////

#include "FilePa.h"
#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePa::CFilePa()
{

}

CFilePa::~CFilePa()
{

}

nuBOOL CFilePa::GetPa(nuLONG nMapIdx, nuDWORD nAddress, PPOIPANODE pPaNode)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".PA"));
	if( !LibFS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	nuMemset(pPaNode, 0, sizeof(POIPANODE));
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	if( nuFseek(pFile, nAddress, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nPhoneLen, 1, 1, pFile) != 1 ||
		pPaNode->nPhoneLen > sizeof(pPaNode->wsPhone) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsPhone, pPaNode->nPhoneLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nSubBranchLen, 1, 1, pFile) != 1 ||
		pPaNode->nSubBranchLen > sizeof(pPaNode->wsSubBranch) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsSubBranch, pPaNode->nSubBranchLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nFaxLen, 1, 1, pFile) != 1 ||
		pPaNode->nFaxLen > sizeof(pPaNode->wsFax) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsFax, pPaNode->nFaxLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nAddressLen, 1, 1, pFile) != 1 ||
		pPaNode->nAddressLen > sizeof(pPaNode->wsAddress) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsAddress, pPaNode->nAddressLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nWebLen, 1, 1, pFile) != 1 ||
		pPaNode->nWebLen > sizeof(pPaNode->wsWeb) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsWeb, pPaNode->nWebLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&pPaNode->nOpenTimeLen, 1, 1, pFile) != 1 ||
		pPaNode->nOpenTimeLen > sizeof(pPaNode->wsOpenTime) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(pPaNode->wsOpenTime, pPaNode->nOpenTimeLen, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}