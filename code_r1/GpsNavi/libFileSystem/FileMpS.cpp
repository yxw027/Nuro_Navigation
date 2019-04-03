
#include "FileMpS.h"
#include "NuroMapFormat.h"
#include "MapDataConstructZ.h"
#include "NuroModuleApiStruct.h"

CFileMpS::CFileMpS()
{
	m_nMapIdx   = -1;
	m_pFile     = NULL;
	m_pPoiInfo  = NULL; 
}
CFileMpS::~CFileMpS()
{
	Free();
	CloseFile();
}
nuVOID CFileMpS::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}
nuBOOL CFileMpS::OpenFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".MP"));
	m_nMapIdx = 0;
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
nuBOOL CFileMpS::ChangePoiInfo(/*InPut*/NUROPOINT PoiCoor ,/*output*/POI_Info_Data& PoiInfoData)
{
	nuBOOL bRes = nuFALSE;
	if( OpenFile() == nuFALSE )
	{
		CloseFile();
		return nuFALSE;
	}
	MP_Header MPHeader;
	if( 1 != nuFread(&MPHeader, sizeof(MP_Header), 1, m_pFile))
	{
		CloseFile();
		return nuFALSE;
	}
	m_pPoiInfo = (POI_Info**)m_pMmApi->MM_AllocMem(sizeof(POI_Info));
	if(m_pPoiInfo == NULL)
	{
		Free();
		CloseFile();
		return nuFALSE;
	}
	nuMemset((*m_pPoiInfo), 0, sizeof(POI_Info) * MPHeader.nPOICount);
    if(MPHeader.nPOICount != nuFread((*m_pPoiInfo), sizeof(POI_Info), MPHeader.nPOICount, m_pFile))
	{	
		Free();
		CloseFile();
		return nuFALSE;
	}
	nuINT nMid = 0;
	for(nuINT i = 0; i < MPHeader.nPOICount; i++)
	{
		if (NURO_ABS((*m_pPoiInfo)[i].POICoorX - PoiCoor.x) <= 3 
			&& NURO_ABS((*m_pPoiInfo)[i].POICoorY - PoiCoor.y) <= 3)
		{
				nMid = i;
				bRes = nuTRUE;
		}
	}
	if( 0 != nuFseek(m_pFile, (*m_pPoiInfo)[nMid].POIRoadInfoAddr, NURO_SEEK_SET))
	{	
		CloseFile();
		return nuFALSE;
	}
	nuMemset(&PoiInfoData, 0, sizeof(POI_Info_Data));
	if( 1 != nuFread(&PoiInfoData, sizeof(POI_Info_Data), 1, m_pFile))
	{	
		Free();
		CloseFile();
		return nuFALSE;
	}
    
	CloseFile();
	Free();
	return bRes;
}

nuVOID CFileMpS::Free()
{
	m_pMmApi->MM_FreeMem((nuPVOID*)m_pPoiInfo);
	m_pPoiInfo == NULL;
}
