// FileNameMgr.cpp: implementation of the CFileNameMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "FileNameMgr.h"
#include "NuroModuleApiStruct.h"
#include "NuroConstDefined.h"
#include "FileMpnZ.h"

extern PMEMMGRAPI	g_pFsMmApi;
//Library
//#include "LoadMM.h"

#define MAPPATH						nuTEXT("Map\\")
#define FILE_PATH_					nuTEXT("Media\\File\\")
#define FILE_USERCFG				nuTEXT("Setting\\UserConfig.dat")
#define FILE_USERCFG_TMP			nuTEXT("Setting\\UserConfigTmp.dat")
#define FILE_MAPCFG_DAY				nuTEXT("Setting\\MapConfigD.dat")
#define FILE_MAPCFG_DAY2			nuTEXT("Setting\\MapConfigD_2.dat")
#define FILE_MAPCFG_NIGHT			nuTEXT("Setting\\MapConfigN.dat")
#define FILE_MAPCFG_NIGHT2			nuTEXT("Setting\\MapConfigN_2.dat")
#define FILE_EEYTETYPE_TEXT			nuTEXT("\\EEyeType.txt")
#define MAPMAJORPATH				nuTEXT("Map\\Major\\")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileNameMgr::CFileNameMgr()
{
	m_sAppPath		= NULL;
	m_pMapNameInfo	= NULL;
	m_nMapCount		= 0;
}

CFileNameMgr::~CFileNameMgr()
{
	Free();
}
nuBOOL CFileNameMgr::Initialize()
{
__android_log_print(ANDROID_LOG_INFO, "navi", "CFileNameMgr::Initialize size %d", sizeof(nuTCHAR)*NURO_MAX_PATH);
	m_sAppPath = (nuTCHAR*)g_pFsMmApi->MM_GetStaticMemMass(sizeof(nuTCHAR)*NURO_MAX_PATH);
	if( m_sAppPath == NULL )
	{
__android_log_print(ANDROID_LOG_INFO, "navi", "CFileNameMgr::Initialize Fail");
		return nuFALSE;
	}
	nuGetModulePath(NULL, m_sAppPath, NURO_MAX_PATH);
	return ReadMapPath();
//	return SearchMapFolder(m_sAppPath);
}
nuVOID CFileNameMgr::Free()
{
	m_sAppPath		= NULL;
	m_pMapNameInfo	= NULL;
	m_nMapCount		= 0;
}
/*
nuBOOL CFileNameMgr::SearchMapFolder(const nuTCHAR* sAppPath)
{
#ifdef NURO_OS_WINDOWS
	if( sAppPath == NULL )
	{
		return nuFALSE;
	}
	nuTCHAR sFilePath[NURO_MAX_PATH];
	nuTcscpy(sFilePath, sAppPath);
	nuTcscat(sFilePath, MAPPATH);
	nuTcscat(sFilePath, nuTEXT("?*"));
	NURO_FILE_FIND_DATA fileData;
	nuHANDLE hFile = nuFindFirstFile(sFilePath, &fileData);
	if( hFile == NULL )
	{
		return nuFALSE;
	}
	PMAPNAMEINFO pMapNameInfo;
	nuBOOL bFind = NUTRUE;
	while( bFind )
	{
		if( nuTcscmp( fileData.cFileName, nuTEXT(".") ) == 0 ||
			nuTcscmp( fileData.cFileName, nuTEXT("..") ) == 0	||
			!(fileData.dwFileAttributes & NURO_FILE_ATTRIBUTE_DIRECTORY) )
		{
			bFind = nuFindNextFile(hFile, &fileData);
			continue;
		}
		pMapNameInfo = (PMAPNAMEINFO)g_pFsMmApi->MM_GetStaticMemMass((nuDWORD)sizeof(MAPNAMEINFO));
		if( pMapNameInfo == NULL )
		{
			break;
		}
		nuMemset(pMapNameInfo, 0, sizeof(MAPNAMEINFO));
		pMapNameInfo->nMapID = m_nMapCount;//Temporary
		nuTcsncpy(pMapNameInfo->sMapName, fileData.cFileName, MAPNAMEMAXLEN);
		m_nMapCount ++;
		if( m_pMapNameInfo == NULL )
		{
			m_pMapNameInfo = pMapNameInfo;
		}
		bFind = nuFindNextFile(hFile, &fileData);
	}
	nuFindClose(hFile);
#endif
	return nuTRUE;
}
*/
nuBOOL CFileNameMgr::FindFileWholePath(nuLONG nMapIdx, nuTCHAR *sWholePath, nuWORD num)
{
	if( sWholePath == NULL || m_pMapNameInfo == NULL || nMapIdx >= m_nMapCount )
	{
		return nuFALSE;
	}
	nuTCHAR files[NURO_MAX_PATH] = {0};
	nuTcscpy(files, m_sAppPath);
	nuTcscat(files, MAPPATH);
	if( nMapIdx >= 0 )
	{
		nuTcscat(files, m_pMapNameInfo[nMapIdx].sMapName);
		nuTcscat(files, nuTEXT("\\"));
		nuTcscat(files, m_pMapNameInfo[nMapIdx].sMapName);
		if(nuTcscmp(sWholePath, nuTEXT(".TMC")) == 0)
		{
			nuTcscat(files, nuTEXT("_CC"));	
		}
		nuTcscat(files, sWholePath);
		nuTcscpy(sWholePath, files);
		return nuTRUE;
	}	
	else
	{
		nuTcscat(files, nuTEXT("Major\\"));
		nuTcscat(files, nuTEXT("TW"));
		nuTcscat(files, sWholePath);
		nuTcscpy(sWholePath, files);
		return nuTRUE;
	}
	/*
#ifdef NURO_OS_WINDOWS
	if( sWholePath == NULL || m_pMapNameInfo == NULL || nMapIdx >= m_nMapCount )
	{
		return nuFALSE;
	}
	nuTCHAR files[NURO_MAX_PATH];
	nuTcscpy(files, m_sAppPath);
	nuTcscat(files, MAPPATH);
	if( nMapIdx >= 0 )
	{
		nuTcscat(files, m_pMapNameInfo[nMapIdx].sMapName);
		nuTcscat(files, nuTEXT("\\"));
		nuTcscat(files, m_pMapNameInfo[nMapIdx].sMapName);
		nuTcscat(files, sWholePath);
		nuTcscpy(sWholePath, files);
		return nuTRUE;
	}
	else
	{
		nuTcscat(files, nuTEXT("*"));
		nuTcscat(files, sWholePath);
		NURO_FILE_FIND_DATA fileData;
		nuHANDLE hFile = nuFindFirstFile(files, &fileData);
		if( hFile == NULL )
		{
			return nuFALSE;
		}
		nuBOOL bFind = NUTRUE;
		while( bFind && !(fileData.dwFileAttributes & NURO_FILE_ATTRIBUTE_ARCHIVE) )
		{
			bFind = nuFindNextFile(hFile, &fileData);
		}
		nuFindClose(hFile);
		if( bFind )
		{
			nuTcscpy(sWholePath, m_sAppPath);
			nuTcscat(sWholePath, MAPPATH);
			nuTcscat(sWholePath, fileData.cFileName);
			return nuTRUE;
		}
		else
		{
			return nuFALSE;
		}
	}
#else
	nuTCHAR files[NURO_MAX_PATH];
	nuTcscpy(files, m_sAppPath);
	nuTcscat(files, MAPPATH);
	if( nMapIdx >= 0 ) 
	{
		nuTcscat(files, nuTEXT("TW\\TW"));
	}
	else
	{
		nuTcscat(files, nuTEXT("TW"));
	}
	nuTcscat(files, sWholePath);
	nuTcscpy(sWholePath, files);
#endif
	*/
	return nuTRUE;
}

nuBOOL CFileNameMgr::GetFilePath(nuWORD nFileType, nuTCHAR *sWholePath, nuWORD len, nuINT nLanguage /* = -1 */)
{
	if( sWholePath == NULL || m_sAppPath == NULL )
	{
		return nuFALSE;
	}
	nuTCHAR tsSum[10] = {0};
	nuTcscpy(sWholePath, m_sAppPath);
	switch(nFileType)
	{
		case FILE_NAME_PATH:
			break;
		case FILE_NAME_USER_CFG:
			nuTcscat(sWholePath, FILE_USERCFG);
			break;
		case FILE_NAME_USER_CFG_TMP:
			nuTcscat(sWholePath, FILE_USERCFG_TMP);
			break;
		case FILE_NAME_MAP_CFG_DAY:
			nuTcscat(sWholePath, FILE_MAPCFG_DAY);
			break;
		case FILE_NAME_MAP_CFG_DAY2:
			nuTcscat(sWholePath, FILE_MAPCFG_DAY2);
			break;
		case FILE_NAME_MAP_CFG_NIGHT:
			nuTcscat(sWholePath, FILE_MAPCFG_NIGHT);
			break;
		case FILE_NAME_MAP_CFG_NIGHT2:
			nuTcscat(sWholePath, FILE_MAPCFG_NIGHT2);
			break;
		case FILE_NAME_EEYETYPE_TEXT:
			nuTcscat(sWholePath, FILE_PATH_);
			nuItot(nLanguage, tsSum, 10);
			nuTcscat(sWholePath, tsSum);
			nuTcscat(sWholePath, FILE_EEYTETYPE_TEXT);
			break;
		default:
			return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CFileNameMgr::ReadMapPath()
{
	nuWORD nMemIdx = 0;
	nuUINT nBuffLen = 64 * sizeof(MPNDATA) + 4;
	nuPBYTE pData = (nuPBYTE)g_pFsMmApi->MM_GetDataMemMass(nBuffLen, &nMemIdx);
	if( pData == NULL )
	{
		return nuFALSE;
	}
//	nuUINT nReadLen = CFileMpnZ::GetMpnBuff(m_sAppPath, pData, nBuffLen, MPN_DATA_MAPID|MPN_DATA_FILENAME);
	if( CFileMpnZ::GetMpnData(m_sAppPath, pData, nBuffLen) )
	{
		nuPDWORD pMapCount = (nuPDWORD)pData;
		PMPNDATA pMpnData = (PMPNDATA)(pData + 4);
		nuTCHAR tsFile[NURO_MAX_PATH];
		for(nuDWORD i = 0; i < *pMapCount; i++ )
		{
			nuTcscpy(tsFile, m_sAppPath);
			nuTcscat(tsFile, MAPPATH);
			nuTcscat(tsFile, pMpnData[i].tsFoldName);
			nuTcscat(tsFile, nuTEXT("\\"));
			nuTcscat(tsFile, pMpnData[i].tsFoldName);
			nuTcscat(tsFile, nuTEXT(".bh"));
			nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
			if( pFile != NULL )
			{
				nuFclose(pFile);
				PMAPNAMEINFO pMapNameInfo = (PMAPNAMEINFO)g_pFsMmApi->MM_GetStaticMemMass((nuDWORD)sizeof(MAPNAMEINFO));
				if( pMapNameInfo != NULL )
				{
					pMapNameInfo->nMapID = pMpnData[i].nMapID;
					pMapNameInfo->point	= pMpnData[i].point;
					nuTcsncpy(pMapNameInfo->sMapName, pMpnData[i].tsFoldName, MAPNAMEMAXLEN/sizeof(nuTCHAR));
					nuWcsncpy(pMapNameInfo->wMapFriendName, pMpnData[i].wsFriendName, MAPFRIENDNAMELEN/sizeof(nuWCHAR));
					if( m_pMapNameInfo == NULL )
					{
						m_pMapNameInfo = pMapNameInfo;
					}
					m_nMapCount ++;
				}
			}
		}
	}
	if( m_pMapNameInfo == NULL )
	{
		m_nMapCount = 1;
		PMAPNAMEINFO pMapNameInfo = (PMAPNAMEINFO)g_pFsMmApi->MM_GetStaticMemMass((nuDWORD)sizeof(MAPNAMEINFO));
		if( pMapNameInfo != NULL )
		{
			pMapNameInfo->nMapID = 0;
			nuTcscpy(pMapNameInfo->sMapName, nuTEXT("TW"));
			if( m_pMapNameInfo == NULL )
			{
				m_pMapNameInfo = pMapNameInfo;
			}
		}
	}
	g_pFsMmApi->MM_RelDataMemMass(&nMemIdx);
	return nuTRUE;
}
nuLONG CFileNameMgr::MapIDIndex(nuLONG nID, nuBYTE nType)
{
	if( nID < 0 || m_pMapNameInfo == NULL )
	{
		return -1;
	}
	if( nType == MAP_ID_TO_INDEX )
	{
		for(nuLONG i = 0; i < m_nMapCount; ++i)
		{
			if( m_pMapNameInfo[i].nMapID == nID )
			{
				return i;
			}
		}
		return -1;
	}
	else if( nType == MAP_INDEX_TO_ID )
	{
		if( nID >= m_nMapCount )
		{
			return -1;
		}
		else
		{
			return m_pMapNameInfo[nID].nMapID;
		}
	}
	else
	{
		return -1;
	}
}

nuBOOL CFileNameMgr::ReadMapFriendName(nuWORD nMapIdx, nuWCHAR *pWsName, nuWORD nLen)
{
	if( nMapIdx >= m_nMapCount || pWsName == NULL )
	{
		return nuFALSE;
	}
	nuWcsncpy( pWsName, m_pMapNameInfo[nMapIdx].wMapFriendName, NURO_MIN(MAPFRIENDNAMELEN/sizeof(nuWCHAR), nLen/sizeof(nuWCHAR)) );
	pWsName[nLen/sizeof(nuWCHAR)] = 0;
	return nuTRUE;
}
