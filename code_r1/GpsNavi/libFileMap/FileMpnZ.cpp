// FileMpnZ.cpp: implementation of the CFileMpnZ class.
//
//////////////////////////////////////////////////////////////////////
#include "FileMpnZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMpnZ::CFileMpnZ()
{

}

CFileMpnZ::~CFileMpnZ()
{

}
/*
nuUINT CFileMpnZ::GetMpnBuff(nuTCHAR *tsPath, nuPVOID pBuff, nuUINT nBufLen, nuUINT nDataMode)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, tsPath);
	nuTcscat(tsFile, MAP_MPN_PATH);
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return 0;
	}
	nuPBYTE pData = (nuPBYTE)pBuff;
	nuUINT	nMapCount;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(pData, 4, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nMapCount = *((nuUINT*)pData);
	pData += 4;
	nuDWORD nSize = 4;
	if ( nDataMode & MPN_DATA_MAPID )
	{
		nSize += nMapCount * 4;
	}
	if ( nDataMode & MPN_DATA_FILENAME )
	{
		nSize += nMapCount * MPN_FILENAME_LEN;
	}
	if ( nDataMode & MPN_DATA_FRIENDNAME )
	{
		nSize += nMapCount * MPN_FRIENDNAME_LEN;
	}
	if ( nDataMode & MPN_DATA_XY )
	{
		nSize += nMapCount * 8;
	}
	if( nSize > nBufLen )
	{
		nuFclose(pFile);
		return nSize;
	}
	//
	nuLONG nLen;
	nLen = nMapCount * 4;
	if ( nDataMode & MPN_DATA_MAPID )
	{
		if( nuFread(pData, nLen, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nSize;
		}
		pData += nLen;
	}
	else
	{
		if( nuFseek(pFile, nLen, NURO_SEEK_CUR) != 0 )
		{
			nuFclose(pFile);
			return nSize;
		}
	}
	//
	nLen = nMapCount * MPN_FILENAME_LEN;
	if ( nDataMode & MPN_DATA_FILENAME )
	{
		if( nuFread(pData, nLen, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nSize;
		}
		pData += nLen;
	}
	else
	{
		if( nuFseek(pFile, nLen, NURO_SEEK_CUR) != 0 )
		{
			nuFclose(pFile);
			return nSize;
		}
	}
	//
	nLen = nMapCount * MPN_FRIENDNAME_LEN;
	if ( nDataMode & MPN_DATA_FRIENDNAME )
	{
		if( nuFread(pData, nLen, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nSize;
		}
		pData += nLen;
	}
	else
	{
		if( nuFseek(pFile, nLen, NURO_SEEK_CUR) != 0 )
		{
			nuFclose(pFile);
			return nSize;
		}
	}
	//
	nLen = nMapCount * 8;
	if ( nDataMode & MPN_DATA_XY )
	{
		if( nuFread(pData, nLen, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nSize;
		}
		pData += nLen;
	}
	else
	{
		if( nuFseek(pFile, nLen, NURO_SEEK_CUR) != 0 )
		{
			nuFclose(pFile);
			return nSize;
		}
	}
	nuFclose(pFile);
	return nSize;
}
*/
nuUINT CFileMpnZ::GetMpnData(nuTCHAR *tsPath, nuPVOID pBuff, nuUINT& nBufLen)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, tsPath);
	nuTcscat(tsFile, MAP_MPN_PATH);
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
__android_log_print(ANDROID_LOG_INFO, "navi", "GetMpnData Fail %s", tsFile);
		return 0;
	}
	nuUINT	nMapCount = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&nMapCount, 4, 1, pFile) != 1 )
	{
__android_log_print(ANDROID_LOG_INFO, "navi", "GetMpnData nuFseek Fail");
		nuFclose(pFile);
		return 0;
	}
	nuDWORD nSize = 4 + nMapCount * sizeof(MPNDATA);
	if( nSize > nBufLen || pBuff == NULL )
	{
		nuFclose(pFile);
		nBufLen = nSize;
__android_log_print(ANDROID_LOG_INFO, "navi", "nSize > nBufLen || pBuff == NULL Fail");
		return 0;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(pBuff, 1, nSize, pFile) != nSize )
	{
__android_log_print(ANDROID_LOG_INFO, "navi", "nuFread Fail");
		nuFclose(pFile);
		return 0;
	}
	//
	if( 1 != sizeof(nuTCHAR) )
	{
		PMPNDATA pMpn = (PMPNDATA)(((nuBYTE*)pBuff) + 4);
		nuCHAR tsFoldName[MPN_FILENAME_LEN] = {0};
		for(nuUINT i = 0; i < nMapCount; ++i)
		{
			nuMemcpy(tsFoldName, pMpn[i].tsFoldName, MPN_FILENAME_LEN);
			if( 0 == tsFoldName[1] && 0 != tsFoldName[2] )
			{
				continue;
			}//it's wchar version
			nuMemset(pMpn[i].tsFoldName, 0, MPN_FILENAME_LEN);
			for(nuINT j = 0; j < MPN_FILENAME_LEN/sizeof(nuTCHAR); ++j)
			{
				pMpn[i].tsFoldName[j] = tsFoldName[j];
			}
		}
	}
	nuFclose(pFile);
	return nSize;
}
