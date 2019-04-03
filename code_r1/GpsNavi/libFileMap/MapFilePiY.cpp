
#include "MapFilePiY.h"

CMapFilePiY::CMapFilePiY()
{
	m_nPoiCount = 0;
	m_nCityCount = 0;
	m_nBStartAddr = -1;
}

CMapFilePiY::~CMapFilePiY()
{
	Close();
}

nuBOOL CMapFilePiY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID==(nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuMemset(tcsPath, 0, sizeof(tcsPath));
	nuTcscpy(tcsPath, nuTEXT(".PI"));
	if (!m_pFsApi->FS_FindFileWholePath(nMapID, tcsPath, NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&m_nPoiCount, sizeof(m_nPoiCount), 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	if (file.Read(&m_nCityCount, sizeof(m_nCityCount), 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CMapFilePiY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFilePiY::Close()
{
	file.Close();
	m_nCityCount = 0;
	m_nPoiCount = 0;
	m_nBStartAddr = -1;
}
nuUINT CMapFilePiY::GetPoiNum()
{
	return m_nPoiCount;
}
nuUINT CMapFilePiY::GetCityNum()
{
	return m_nCityCount;
}
nuUINT CMapFilePiY::GetBStartAddr()
{
	if (m_nBStartAddr!=(nuUINT)-1)
	{
		return m_nBStartAddr;
	}
	nuUINT addr = -1;
	if (file.ReadData(8+sizeof(SEARCH_PI_CITYINFO)*m_nCityCount, &addr, 4))
	{
		return addr;
	}
	return -1;
}

nuBOOL CMapFilePiY::GetCityInfo(nuWORD nCityID, SEARCH_PI_CITYINFO* pCityInfo)
{
	if (!pCityInfo)
	{
		return nuFALSE;
	}
	file.Seek(8+sizeof(SEARCH_PI_CITYINFO)*nCityID, NURO_SEEK_SET);
	if (file.Read(pCityInfo, sizeof(SEARCH_PI_CITYINFO), 1) != 1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFilePiY::GetPoiInfo_PPIdx(nuUINT nPPIdx, SEARCH_PI_POIINFO *pInfo)
{
	if (nPPIdx>=m_nPoiCount || !pInfo)
	{
		return nuFALSE;
	}
	nuUINT addr = -1;
	if (!file.ReadData(GetBStartAddr()+(nPPIdx<<3), &addr, 4) || addr==(nuUINT)-1)
	{
		return nuFALSE;
	}
	if (file.ReadData(addr, pInfo, sizeof(SEARCH_PI_POIINFO)))
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuUINT CMapFilePiY::GetPoiInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen)
{
	if (!pBuffer || nCityID>=m_nCityCount)
	{
		return 0;
	}
	SEARCH_PI_CITYINFO info = {0};
	if (!GetCityInfo(nCityID, &info))
	{
		return 0;
	}
	if (info.nCityID!=nCityID)
	{
		return 0;
	}
	nuUINT nRes = info.nPoiCount*sizeof(SEARCH_PI_POIINFO);
	if (!info.nPoiCount || nRes>nMaxLen)
	{
		return 0;
	}
	file.Seek(info.nStartPos, NURO_SEEK_SET);
	if (file.Read(pBuffer, nRes, 1)!=1)
	{
		return 0;
	}
	return nRes;
}

nuBOOL CMapFilePiY::GetPoiInfo_CEX(nuUINT nCityStartAddr, nuWORD nRoadIdx, SEARCH_PI_POIINFO *pInfo)
{
	file.Seek(nCityStartAddr+nRoadIdx*sizeof(SEARCH_PI_POIINFO), NURO_SEEK_SET);
	if (file.Read(pInfo, sizeof(SEARCH_PI_POIINFO), 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}