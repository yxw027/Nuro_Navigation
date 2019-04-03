#include "FIleKmDL.h"
#include "MapDataConstructZ.h"
#include "NuroModuleApiStruct.h"
#include <wchar.h>
#ifdef _DEBUG
#include <assert.h>
#endif

CFileKMDL::CFileKMDL()
{
	m_lKMCount     = 0;
	m_pFile        = NULL;
	m_pKmInfoData  = NULL;

}

CFileKMDL::~CFileKMDL()
{
	Free();
	CloseFile();
}

nuVOID CFileKMDL::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}
nuBOOL CFileKMDL::Initial()
{
	return ReadKM();
}

nuBOOL CFileKMDL::OpenFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".KM"));
	if( !m_pFsApi->FS_FindFileWholePath(0, tsFile, NURO_MAX_PATH) )
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
nuBOOL CFileKMDL::ReadKM()
{
	if(OpenFile() == nuFALSE)
	{
		CloseFile();
		return nuFALSE;
	}
	else
	{
		if( 1 != nuFread(&m_lKMCount, sizeof(nuLONG), 1, m_pFile) || m_lKMCount <= 0)
		{
			CloseFile();
			return nuFALSE;
		}
		
		m_pKmInfoData = (KMInfoData*)m_pMmApi->MM_GetStaticMemMass(sizeof(KMInfoData) * m_lKMCount);
		if(m_pKmInfoData == NULL)
		{
			Free();
			CloseFile();
			return nuFALSE;
		}
		
		if( 0 != nuFseek(m_pFile, sizeof(nuLONG), NURO_SEEK_SET))
		{
			Free();
			CloseFile();
			return nuFALSE;
		} 

		if( m_lKMCount != nuFread(m_pKmInfoData, sizeof(KMInfoData), m_lKMCount, m_pFile) )
		{
			Free();
			CloseFile();
			return nuFALSE;
		}
		if(m_pKmInfoData[0].nOneWay > 2 || m_pKmInfoData[0].nOneWay < 0 || m_pKmInfoData[0].nClass > 14 || m_pKmInfoData[0].nClass < 0 )
		{
			Free();
			CloseFile();
			return nuFALSE;
		}
	}
	CloseFile();
	return nuTRUE;
}

nuBOOL CFileKMDL::ColKMBlockIndex()
{
	return nuTRUE;
}

KMInfoData* CFileKMDL::GetKMDataDL(nuLONG BlockIdx, /*output*/ nuLONG &Counter)
{
	if(m_pKmInfoData == NULL)
	{
		return nuFALSE;
	}
	nuINT nMax   = m_lKMCount - 1; 
	nuINT nMid   = 0; 
	nuINT nMin   = 0;
	nuINT i		 = 0;
	nuBOOL bFind = nuFALSE;
	while (nMin <= nMax) 
	{ 
		nMid = (nMin + nMax) / 2;
		if(m_pKmInfoData[nMid].lBlockIdx < BlockIdx ) 
		{
			nMin = nMid + 1; 
		}
		else if (m_pKmInfoData[nMid].lBlockIdx > BlockIdx ) 
		{
			nMax = nMid - 1; 
		}
		else
		{    
			Counter = 1;
			bFind = nuTRUE;
			break;
		}
	}
	if(!bFind)
	{
		return NULL;
	}
	if(nMid > 0)
	{
		for(i = nMid - 1; i > 0; i--)
		{
			if(m_pKmInfoData[i].lBlockIdx == BlockIdx ) //&& m_pKmInfoData[i].lRoadIdx == RoadData.nRoadIdx)
			{
				nMid--;
			}
			else
			{
				break;
			}
		}
	}	
	if(nMid < m_lKMCount)
	{
		for(i = nMid + 1; i < m_lKMCount; i++)
		{
			if(m_pKmInfoData[i].lBlockIdx == BlockIdx ) //&& m_pKmInfoData[i].lRoadIdx == RoadData.nRoadIdx)
			{
				Counter++;
			}
			else
			{
				break;
			}
		}
	}
	return &m_pKmInfoData[nMid];
}
#define SEEK_KM_UP		1
#define SEEK_KM_DOWN	1
KMInfoData* CFileKMDL::GetKMDataMapping(const SEEKEDROADDATA &RoadData, nuLONG &Counter)
{
	nuLONG Dx = 0, Dy = 0, Dis = 0;
	nuLONG Index = -1, i = 0, ReIndex = -1;
	nuLONG Distance = 0;
	nuLONG lBlockIdx = RoadData.nBlkIdx;
	if(m_pKmInfoData == NULL)
	{
		return NULL;
	}
	if(!SeekKMBlock(lBlockIdx, Index))
	{
		/*for(i = lBlockIdx - SEEK_KM_UP; i < lBlockIdx + SEEK_KM_DOWN; i++)
		{
			if(i == lBlockIdx)
			{	
				continue;
			}
			if(SeekKMBlock(lBlockIdx, Index))
			{
				break;
			}
		}*/
		return NULL;
	}	
	
	if(Index > 0)
	{
		for(i = Index - 1; i > 0; i--)
		{
			if(m_pKmInfoData[i].lBlockIdx == RoadData.nBlkIdx ) //&& m_pKmInfoData[i].lRoadIdx == RoadData.nRoadIdx)
			{
				Index--;
			}
			else
			{
				break;
			}
		}
	}	
	if(Index < m_lKMCount)
	{
		for(i = Index + 1; i < m_lKMCount; i++)
		{
			if(m_pKmInfoData[i].lBlockIdx == RoadData.nBlkIdx ) //&& m_pKmInfoData[i].lRoadIdx == RoadData.nRoadIdx)
			{
				Counter++;
			}
			else
			{
				break;
			}
		}
	}
	
	Dx = (m_pKmInfoData[Index].X - RoadData.ptMapped.x), Dy = (m_pKmInfoData[Index].Y - RoadData.ptMapped.y);
	Dis = (nuLONG)nuSqrt((nuDOUBLE)(Dx * Dx + Dy * Dy));
	ReIndex = Index;
	Counter += Index;
	for(i = Index; i <= Counter; i++)
	{
		Dx = (m_pKmInfoData[i].X - RoadData.ptMapped.x);
		Dy = (m_pKmInfoData[i].Y - RoadData.ptMapped.y);
		Distance = (nuLONG)nuSqrt((nuDOUBLE)(Dx * Dx + Dy * Dy));
		if(m_pKmInfoData[i].nClass != RoadData.nRoadClass)
		{
			//Distance += 300;
			continue;
		}
		if(m_pKmInfoData[i].lRoadIdx != RoadData.nRoadIdx)
		{
			Distance += 30;
			//continue;
		}
		if(Dis > Distance || m_pKmInfoData[Index].nClass != RoadData.nRoadClass)
		{
			Dis = Distance;
			ReIndex = i;
			Index = i;
		}
	}
	return &m_pKmInfoData[ReIndex];
}
nuBOOL CFileKMDL::SeekKMBlock(const nuLONG &lBlockIdx, nuLONG &Index)
{
	nuINT nMax   = m_lKMCount - 1; 
	nuINT nMid   = 0; 
	nuINT nMin   = 0;
	while (nMin <= nMax) 
	{ 
		nMid = (nMin + nMax) / 2;
		if(m_pKmInfoData[nMid].lBlockIdx < lBlockIdx ) 
		{
			nMin = nMid + 1; 
		}
		else if (m_pKmInfoData[nMid].lBlockIdx > lBlockIdx ) 
		{
			nMax = nMid - 1; 
		}
		else
		{   
			Index = nMid;
			return nuTRUE;
		}
	}
	return nuFALSE;
}
nuBOOL CFileKMDL::SeekKMRoad(const nuLONG &lBlockIdx, const nuLONG &lRoadIdx, nuLONG &Index)
{
	nuINT nMax   = m_lKMCount - 1; 
	nuINT nMid   = 0; 
	nuINT nMin   = 0;
	while (nMin <= nMax) 
	{ 
		nMid = (nMin + nMax) / 2;
		if(m_pKmInfoData[nMid].lBlockIdx < lBlockIdx ) 
		{
			nMin = nMid + 1; 
		}
		else if (m_pKmInfoData[nMid].lBlockIdx > lBlockIdx ) 
		{
			nMax = nMid - 1; 
		}
		else
		{   
			if(m_pKmInfoData[nMid].lRoadIdx < lRoadIdx ) 
			{
				nMin = nMid + 1; 
			}
			else if (m_pKmInfoData[nMid].lRoadIdx > lRoadIdx ) 
			{
				nMax = nMid - 1; 
			}
			else
			{   
				Index = nMid;
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}
nuBOOL CFileKMDL::SeekKMData(const NUROPOINT &ptCar, const nuLONG &lBlockIdx, const nuLONG &lRoadIdx, 
							 const nuLONG &lRoadClass, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName)
{
	nuLONG Dx = 0, Dy = 0, Dis = 0;
	nuLONG Index = -1, i = 0, ReIndex = -1;
	nuLONG Distance = 0;
	nuLONG Counter = 0;
	if(m_pKmInfoData == NULL)
	{
		return nuFALSE;
	}
	if(!SeekKMRoad(lBlockIdx, lRoadIdx, Index))
	{
		return nuFALSE;
	}	
	
	if(Index > 0)
	{
		for(i = Index - 1; i > 0; i--)
		{
			if(m_pKmInfoData[i].lBlockIdx == lBlockIdx && m_pKmInfoData[i].lRoadIdx == lRoadIdx)
			{
				Index--;
			}
			else
			{
				break;
			}
		}
	}	
	if(Index < m_lKMCount)
	{
		for(i = Index + 1; i < m_lKMCount; i++)
		{
			if(m_pKmInfoData[i].lBlockIdx == lBlockIdx && m_pKmInfoData[i].lRoadIdx == lRoadIdx)
			{
				Counter++;
			}
			else
			{
				break;
			}
		}
	}
	
	ReIndex = -1;
	Counter += Index;
	Dis = 0xFFFFFFF;
	for(i = Index; i <= Counter; i++)
	{
		if(m_pKmInfoData[i].nClass != lRoadClass)
		{
			continue;
		}
		Dx = (m_pKmInfoData[i].X - ptCar.x);
		Dy = (m_pKmInfoData[i].Y - ptCar.y);
		Distance = (nuLONG)nuSqrt((nuDOUBLE)(Dx * Dx + Dy * Dy));
		if(m_pKmInfoData[i].lRoadIdx != lRoadIdx)
		{
			Distance += 20;
		}
		if(Dis > Distance)//Distance < 100 || 
		{
			Dis = Distance;
			ReIndex = i;
		}
	}
	if(ReIndex >= 0)
	{
		lKMDis = Dis;
		lKMNum = m_pKmInfoData[ReIndex].KmName;
		if(OpenFile() == nuFALSE)
		{
			CloseFile();
			return nuFALSE;
		}
		if(NULL != nuFseek(m_pFile, m_pKmInfoData[ReIndex].NameAddr, NURO_SEEK_SET))
		{
			CloseFile();
			return nuFALSE;
		}
		if( m_pKmInfoData[ReIndex].NameLen / 2 != nuFread(wsKMName, sizeof(nuWCHAR), m_pKmInfoData[ReIndex].NameLen / 2, m_pFile) )
		{
			CloseFile();
			return nuFALSE;
		}
		CloseFile();
#ifdef ZENRIN
		nuFLOAT fKmName = m_pKmInfoData[ReIndex].KmName;
		fKmName /= 100;
		//nuINT Len = swprintf( wsKilometer, nuTEXT("%4.1f"), fKmName);
#else
		nuItow((m_pKmInfoData[ReIndex].KmName / 100), wsKilometer, 10);
#endif
		nuWcscat(wsKMName, (nuWCHAR*)nuTEXT("-"));
		nuWcscat(wsKMName, wsKilometer);
		return nuTRUE;
	}
	return nuFALSE;
}
nuBOOL CFileKMDL::CheckKMInfo(PKMINFO pKMInfo, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName)
{
	nuWCHAR	wsKilo[6] = {0};
	nuBOOL bFind	  = nuFALSE;
	nuLONG lBlockIdx  = pKMInfo->nCarBlkIdx;
	nuLONG lRoadIdx   = pKMInfo->nCarRdIdx;
	nuLONG lRoadClass = pKMInfo->nRoadClass;
	nuLONG lKMRoadCounter = 0;
	nuItow(lBlockIdx, wsKilo,10);
	//nuWcscpy(wsKilo, wsKilometer);
	if(!SeekKMData(pKMInfo->ptCarMapped, lBlockIdx, lRoadIdx, lRoadClass, wsKilometer, lKMNum, lKMDis, wsKMName))
	{
		while(lKMRoadCounter < _KM_CHECK_MAX_ROADS)
		{
			lBlockIdx  = pKMInfo->pKMRoad[lKMRoadCounter].nKMBlkIdx;
			lRoadIdx   = pKMInfo->pKMRoad[lKMRoadCounter].nKMRdIdx;
			lRoadClass = pKMInfo->pKMRoad[lKMRoadCounter].nRoadClass;
			if(SeekKMData(pKMInfo->ptCarMapped, lBlockIdx, lRoadIdx, lRoadClass, wsKilometer, lKMNum, lKMDis, wsKMName))
			{
				return nuTRUE;
			}
			lKMRoadCounter++;
		}
	}
	else
	{
		return nuTRUE;
	}
	return nuFALSE;
}
nuVOID CFileKMDL::Free()
{
	if(m_pKmInfoData)
	{
		m_pMmApi->MM_RelStaticMemMassAfter((nuPBYTE)m_pKmInfoData);
		m_pKmInfoData=NULL;
	}
}
