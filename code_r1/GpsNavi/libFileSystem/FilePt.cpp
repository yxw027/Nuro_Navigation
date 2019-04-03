// FilePt.cpp: implementation of the CFilePt class.
//
//////////////////////////////////////////////////////////////////////

#include "FilePt.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"

extern PMEMMGRAPI	g_pFsMmApi;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePt::CFilePt()
{
	m_nPtCount	= 0;
	m_pPtData	= NULL;
}

CFilePt::~CFilePt()
{
	Free();
}

nuBOOL CFilePt::Initialize()
{
	m_nDWIdx = -1;
	return nuTRUE;//ReadPtFile();
}

nuVOID CFilePt::Free()
{
	m_nPtCount	= 0;
	if(m_pPtData != NULL)
	{
		nuFree(m_pPtData);
		m_pPtData	= NULL;
	}	
}

nuBOOL CFilePt::ReadPtFile(nuLONG nDWIdx)
{
	if(m_nDWIdx != nDWIdx)
	{
		m_nDWIdx = nDWIdx;
	}
	else
	{
		return nuTRUE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE* pFile = NULL;
	nuBOOL bReadDefault = nuFALSE;
	nuTcscpy(tsFile, nuTEXT("tmp.PT"));
	Free();
	if( !LibFS_FindFileWholePath(nDWIdx, tsFile, NURO_MAX_PATH) )
	{
		bReadDefault = nuTRUE;
	}
	else
	{
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			bReadDefault = nuTRUE;
		}
	}
	if( bReadDefault )
	{
		nuTcscpy(tsFile, nuTEXT(".PT"));
		if( !LibFS_FindFileWholePath(nDWIdx, tsFile, NURO_MAX_PATH) )
		{
			return nuFALSE;
		}
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(&m_nPtCount, 4, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if(NULL == m_pPtData)
	{
		m_pPtData = (PPTNODE)nuMalloc(sizeof(PTNODE)*m_nPtCount);//g_pFsMmApi->MM_GetStaticMemMass(sizeof(PTNODE)*m_nPtCount);
	}
	if( m_pPtData == NULL )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFread(m_pPtData, sizeof(PTNODE), m_nPtCount, pFile);
	nuFclose(pFile);
	return nuTRUE;
}

const nuPVOID CFilePt::GetPtDataAddr(nuLONG nDWIdx, nuLONG* pCount)
{
	*pCount = m_nPtCount;
	return  m_pPtData;
}
const nuPVOID CFilePt::GetPtNodeByType(nuLONG nDWIdx, nuBYTE nType1, nuBYTE nType2)
{
	ReadPtFile(nDWIdx);
	if( m_pPtData == NULL )
	{
		return NULL;
	}
	for(nuLONG i = 0; i < m_nPtCount; i++)
	{
		if( m_pPtData[i].nTypeID1 == nType1 && m_pPtData[i].nTypeID2 == nType2 )
		{
			return &m_pPtData[i];
		}
	}
	return NULL;
}

nuBOOL CFilePt::GetInfoBySourceType(nuLONG nSTypeID1, nuLONG nSTypeID2, nuLONG* pTypeID1, nuLONG *pTypeID2, 
									nuLONG *pDisPlay, nuLONG *pIcon)
{
	/*
	if( m_pPtData == NULL )
	{
		return nuFALSE;
	}
	for(nuLONG i = 0; i < m_nPtCount; i++)
	{
		if( m_pPtData[i].nSourceTypeID1 == nSTypeID1 && m_pPtData[i].nSourceTypeID2 == nSTypeID2 )
		{
			if( pTypeID1 != NULL )
			{
				*pTypeID1 = m_pPtData[i].nTypeID1;
			}
			if( pTypeID2 != NULL )
			{
				*pTypeID2 = m_pPtData[i].nTypeID2;
			}
			if( pDisPlay != NULL )
			{
				*pDisPlay = m_pPtData[i].nDisplay;
			}
			if( pIcon != NULL )
			{
				*pIcon = m_pPtData[i].nIcon;
			}
			return nuTRUE;
		}
	}
	*/
	return nuFALSE;
}

nuBOOL CFilePt::GetInfoByNuroType(nuLONG nTypeID1, nuLONG nTypeID2, nuLONG *pSTypeID1, nuLONG *pSTypeID2, 
								  nuLONG *pDisPlay, nuLONG *pIcon)
{
	/*
	if( m_pPtData == NULL )
	{
		return nuFALSE;
	}
	for(nuLONG i = 0; i < m_nPtCount; i++)
	{
		if( m_pPtData[i].nTypeID1 == nTypeID1 && m_pPtData[i].nTypeID2 == nTypeID2 )
		{
			if( pSTypeID1 != NULL )
			{
				*pSTypeID1 = m_pPtData[i].nSourceTypeID1;
			}
			if( pSTypeID2 != NULL )
			{
				*pSTypeID2 = m_pPtData[i].nSourceTypeID2;
			}
			if( pDisPlay != NULL )
			{
				*pDisPlay = m_pPtData[i].nDisplay;
			}
			if( pIcon != NULL )
			{
				*pIcon = m_pPtData[i].nIcon;
			}
			return nuTRUE;
		}
	}
	*/
	return nuFALSE;
}

nuBOOL CFilePt::SavePtFile(nuINT DwIdx)
{
	if( m_pPtData == NULL )
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT("tmp.PT"));
	if( !LibFS_FindFileWholePath(DwIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_OB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFwrite(&m_nPtCount, 4, 1, pFile) != 1 ||
		nuFwrite(m_pPtData, sizeof(PTNODE), m_nPtCount, pFile) != (nuSIZE)m_nPtCount )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}

nuBOOL CFilePt::SavePtFileCoverForDFT(nuINT DwIdx)
{
	// ReRead .pt
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE* pFile = NULL;

	nuTcscpy(tsFile, nuTEXT(".PT"));
	if( !LibFS_FindFileWholePath(DwIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	
	if( pFile == NULL )
	{
		return nuFALSE;
	}

	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	if( nuFread(&m_nPtCount, 4, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	if(NULL == m_pPtData)
	{
		m_pPtData = (PPTNODE)g_pFsMmApi->MM_GetStaticMemMass(sizeof(PTNODE)*m_nPtCount);
	}
	if( m_pPtData == NULL )
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	nuFread(m_pPtData, sizeof(PTNODE), m_nPtCount, pFile);
	nuFclose(pFile);
	// ----------------

	SavePtFile(DwIdx);
	return nuTRUE;
}
