#include "FileICT.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"

extern PMEMMGRAPI	g_pFsMmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileICT::CFileICT()
{

}

CFileICT::~CFileICT()
{
	Free();
}

nuBOOL CFileICT::Initialize()
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuLONG nMapIdx = 0;	
	m_pptICT = NULL;
	m_pFile = NULL;
	m_ppHighwayCrossRoad = NULL;
	nuTcscpy(tsFile, nuTEXT(".ICT"));
	nuMemset(&m_ICT_HEAD, 0, sizeof(m_ICT_HEAD));
	if( !LibFS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile == NULL )
	{
		return nuFALSE;
	}
	if(1 != nuFread(&m_ICT_HEAD, sizeof(ICT_HEAD), 1, m_pFile))
	{
		nuFclose(m_pFile);
		return nuFALSE;
	}
	/*if(NULL != nuFseek(m_pFile, m_ICT_HEAD.Start_BKAddr, NURO_SEEK_SET))
	{
		nuFclose(m_pFile);
		return nuFALSE;
	}*/
	m_ppHighwayCrossRoad = (HighwayCrossRoad**)g_pFsMmApi->MM_AllocMem(sizeof(TAG_HighwayCrossRoad) * m_ICT_HEAD.HighCrossPointCount);
	if(NULL == m_ppHighwayCrossRoad)
	{
		nuFclose(m_pFile);
		return nuFALSE;
	}
	if(m_ICT_HEAD.HighCrossPointCount != nuFread((*m_ppHighwayCrossRoad), sizeof(TAG_HighwayCrossRoad), m_ICT_HEAD.HighCrossPointCount, m_pFile))
	{
		nuFclose(m_pFile);
		return nuFALSE;
	}
	return nuTRUE;
}
nuVOID CFileICT::Free()
{
	if(NULL != m_pFile)
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
	if(NULL != m_ppHighwayCrossRoad)
	{
		g_pFsMmApi->MM_FreeMem((nuPVOID*)m_ppHighwayCrossRoad);
		m_ppHighwayCrossRoad = NULL;
	}
}
nuBOOL CFileICT::SearchSubRoadID(nuLONG lBlockID, nuLONG lRoadIdx, nuLONG Addr)
{
	nuLONG TmpBlockID = 0, TmpRoadID = 0;
	if(NULL != nuFseek(m_pFile, Addr, NURO_SEEK_SET))
	{
		return nuFALSE;
	}
	if(1 != nuFread(&TmpBlockID, sizeof(nuLONG), 1, m_pFile))
	{
		return nuFALSE;
	}
	if(lBlockID != TmpBlockID)
	{
		return nuFALSE;
	}
	if(1 != nuFread(&TmpRoadID, sizeof(nuLONG), 1, m_pFile))
	{
		return nuFALSE;
	}
	if(lRoadIdx != TmpRoadID)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CFileICT::SearchICT(nuLONG lBlockID, nuLONG lRoadIdx, NUROPOINT ptCar, NUROPOINT &ptEtagPoint, nuLONG &HighwayID, nuLONG &HghwayCrossID)
{
	for(nuINT i = 0; i < m_ICT_HEAD.HighCrossPointCount; i++)
	{
		nuLONG dx = NURO_ABS(ptCar.x - (*m_ppHighwayCrossRoad)[i].HighwayCrossPointX);
		nuLONG dy = NURO_ABS(ptCar.y - (*m_ppHighwayCrossRoad)[i].HighwayCrossPointY);
		if(nuSqrt(dx*dx+dy*dy) <= 500)
		{
			if(SearchSubRoadID(lBlockID, lRoadIdx, (*m_ppHighwayCrossRoad)[i].BKAddr))
			{
				HighwayID = (*m_ppHighwayCrossRoad)[i].HighwayID;
				HghwayCrossID = (*m_ppHighwayCrossRoad)[i].HghwayCrossID;
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}
