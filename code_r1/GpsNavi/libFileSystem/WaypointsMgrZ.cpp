// WaypointMgrZ.cpp: implementation of the CWaypointMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#include "WaypointsMgrZ.h"
//#include "nuApiInterFace.h"

#define FILE_WAYPOINTS_NAME		nuTEXT("Setting\\LastWaypoints.dat")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaypointsMgrZ::CWaypointsMgrZ()
{
	m_pWaypointsList	= NULL;
	m_nMaxCount			= 0;
	m_nNowCount			= 0;
}

CWaypointsMgrZ::~CWaypointsMgrZ()
{
	Free();
}

bool CWaypointsMgrZ::Initialize()
{
	long nTmp;
	if( !nuReadConfigValue("WAYPOINTSCOUNT", &nTmp) )
	{
		nTmp = 5;
	}
	m_nMaxCount	= nuLOWORD(nTmp);
	if( !nuReadConfigValue("MARKETFIXDIS", &nTmp) )
	{
		nTmp = 500;
	}
	m_nMaxFixedDis = nuLOWORD(nTmp);
	//
	m_pWaypointsList = new WAYPOINTS_NODE[m_nMaxCount];//(PWAYPOINTS_NODE)m_pMmApi->MM_GetStaticMemMass(m_nMaxCount * sizeof(WAYPOINTS_NODE));
	if( m_pWaypointsList == NULL )
	{
		m_nMaxCount = 0;
		return false;
	}
	m_nNowCount = 0;

	ReadWaypointsFile();

	return true;
}

void CWaypointsMgrZ::Free()
{
	if( m_pWaypointsList )
	{
		delete[] m_pWaypointsList;
		m_pWaypointsList = NULL;
		m_nMaxCount	= 0;
		m_nNowCount	= 0;
	}
}

bool CWaypointsMgrZ::ReadWaypointsFile()
{
	if( m_pWaypointsList == NULL )
	{
		return false;
	}
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_WAYPOINTS_NAME);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return false;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&m_nNowCount, sizeof(nuWORD), 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return false;
	}
	long nDis = 0;
	if( m_nNowCount > m_nMaxCount )
	{
		nDis = sizeof(WAYPOINTS_NODE) * (m_nNowCount - m_nMaxCount);
		m_nNowCount = m_nMaxCount;
	}
	if( nuFseek( pFile, nDis, NURO_SEEK_CUR ) != 0 ||
		nuFread(m_pWaypointsList, sizeof(WAYPOINTS_NODE), m_nNowCount, pFile) != m_nNowCount )
	{
		m_nNowCount = 0;
	}
	nuFclose(pFile);
	return true;
}

bool CWaypointsMgrZ::AmendWaypointsFile(int nIdx)
{
	if( m_pWaypointsList == NULL || nIdx >= m_nNowCount )
	{
		return false;
	}
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_WAYPOINTS_NAME);
	nuFILE* pFile;
	if( nIdx == 0 )
	{
		pFile = nuTfopen(tsFile, NURO_FS_WB);
		if( pFile == NULL )
		{
			return false;
		}
		m_pWaypointsList[nIdx].bRunPass = 1;
		if( nuFwrite(&m_nNowCount, sizeof(nuWORD), 1, pFile) != 1 ||
			nuFwrite(m_pWaypointsList, sizeof(WAYPOINTS_NODE), m_nNowCount, pFile) != m_nNowCount )
		{
			nuFclose(pFile);
			return false;
		}
	}
	else
	{
		if( m_pWaypointsList[nIdx].bRunPass )
		{
			return false;
		}
		pFile = nuTfopen(tsFile, NURO_FS_OB);
		if( pFile == NULL )
		{
			return false;
		}
		m_pWaypointsList[nIdx].bRunPass = 1;
		if( nuFseek(pFile, 2 + sizeof(WAYPOINTS_NODE) * nIdx, NURO_SEEK_SET) != 0 ||
			nuFwrite(&m_pWaypointsList[nIdx], 2, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return false;
		}
	}
	nuFclose(pFile);
	return true;
}

nuUINT CWaypointsMgrZ::SetWaypoints(nuWORD idx, long x, long y, nuWCHAR* pwsName /* = NULL */, short nAngle /* = -1 */)
{
	if( m_pWaypointsList == NULL )
	{
		return 0;
	}
	if( idx )
	{
		nAngle = -1;
	}
	SEEKEDROADDATA roadData;
	if( !m_pFsApi->FS_SeekForRoad(x, y, m_nMaxFixedDis, nAngle, &roadData) )
	{
		return 0;
	}
	if( idx >= m_nNowCount )
	{
		if( m_nNowCount < m_nMaxCount )
		{
			idx = m_nNowCount;
			++m_nNowCount;
		}
		else
		{
			idx = m_nNowCount - 1;
		}
	}
	m_pWaypointsList[idx].bRunPass	= 0;
	m_pWaypointsList[idx].nMapIdx	= roadData.nDwIdx;
	m_pWaypointsList[idx].nBlkIdx	= roadData.nBlkIdx;
	m_pWaypointsList[idx].nRoadIdx	= roadData.nRoadIdx;
	m_pWaypointsList[idx].nCityID	= roadData.nCityID;
	m_pWaypointsList[idx].nTownID	= roadData.nTownID;
	m_pWaypointsList[idx].ptRaw.x	= x;
	m_pWaypointsList[idx].ptRaw.y	= y;
	m_pWaypointsList[idx].ptFixed.x = roadData.ptMapped.x;
	m_pWaypointsList[idx].ptFixed.y = roadData.ptMapped.y;
	nuMemset( m_pWaypointsList[idx].wsName, 0, sizeof(m_pWaypointsList[idx].wsName) );
	if( pwsName )
	{
		nuWcsncpy(m_pWaypointsList[idx].wsName, pwsName, WAYPOINTS_NAME_MAX_NUM-1);
	}
	else
	{
		RNEXTEND rnEx;
		m_pFsApi->FS_GetRoadName( roadData.nDwIdx,
								  roadData.nNameAddr,
								  roadData.nRoadClass,
								  m_pWaypointsList[idx].wsName,
								  sizeof(m_pWaypointsList[idx].wsName),
								  &rnEx );
	}
	if( nAngle >= 0 )
	{
		m_ptNextCoor = roadData.ptNextCoor;
	}
	return 1;
}

nuUINT CWaypointsMgrZ::DltWaypoints(nuWORD idx, short nAngle /* = -1 */)
{
	if( m_nNowCount == 0 )
	{
		return 0;
	}
	else if( idx >= m_nNowCount )
	{
		m_nNowCount = 0;
	}
	else if( idx == m_nNowCount - 1 )
	{
		--m_nNowCount;
	}
	else
	{
		if( idx == 0 )
		{
			SetWaypoints(0, m_pWaypointsList[1].ptRaw.x, m_pWaypointsList[1].ptRaw.y, m_pWaypointsList[1].wsName, nAngle);
			idx = 1;
		}
		--m_nNowCount;
		while( idx < m_nNowCount )
		{
			m_pWaypointsList[idx] = m_pWaypointsList[idx+1];
			++idx;
		}
	}
	return 1;
}

nuUINT CWaypointsMgrZ::GetWaypoints(PPARAM_GETWAYPOINTS pParam)
{
	if( pParam && m_pWaypointsList )
	{
		pParam->nNowCount		= m_nNowCount;
		pParam->ptNextCoor		= m_ptNextCoor;
		pParam->pWaypointsList	= m_pWaypointsList;
		return 1;
	}
	else
	{
		return 0;
	}
}

nuUINT CWaypointsMgrZ::KptWaypoints(long x, long y)
{
	nuUINT nRes = -1;
	if( m_pWaypointsList )
	{
		for(int i = 0; i < m_nNowCount; ++i)
		{
			if( !m_pWaypointsList[i].bRunPass &&
				m_pWaypointsList[i].ptFixed.x == x && 
				m_pWaypointsList[i].ptFixed.y == y )
			{
				nRes = i;
				break;
			}
		}
	}
	if( nRes != -1 )
	{
		AmendWaypointsFile(nRes);
	}
	return nRes;
}
