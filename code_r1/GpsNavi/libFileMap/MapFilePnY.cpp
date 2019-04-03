
#include "MapFilePnY.h"

CMapFilePnY::CMapFilePnY()
{
	m_nCityCount = 0;
}

CMapFilePnY::~CMapFilePnY()
{
	Close();
}

nuBOOL CMapFilePnY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuMemset(tcsPath, 0, sizeof(tcsPath));
	nuTcscpy(tcsPath, nuTEXT(".PN"));
	if (!m_pFsApi->FS_FindFileWholePath(nMapID, tcsPath, NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&m_nCityCount, 4, 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFilePnY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFilePnY::Close()
{
	file.Close();
	m_nCityCount = 0;
}

nuUINT CMapFilePnY::GetLength()
{
	return file.GetLength();
}

nuBOOL CMapFilePnY::ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen)
{
	return file.ReadData(nDataStartAddr, pBuffer, nLen);
}

nuUINT CMapFilePnY::GetCityNum()
{
	return m_nCityCount;
}

nuBOOL   CMapFilePnY::GetCityInfo(nuWORD nCityID, NURO_PN_CITYINFO* pInfo)
{
	if (nCityID>=m_nCityCount || !pInfo)
	{
		return nuFALSE;
	}
	file.Seek(sizeof(NURO_PN_CITYINFO)*nCityID+4, NURO_SEEK_SET);
	if (file.Read(pInfo, sizeof(NURO_PN_CITYINFO), 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuUINT CMapFilePnY::GetStrokes(nuUINT addr)
{
// 	nuBYTE i = 0;
// 	file.Seek(addr, NURO_SEEK_SET);
// 	file.Read(&i, 1, 1);
// 	file.Seek(i, NURO_SEEK_CUR);
// 	file.Read(&i, 1, 1);
// 	file.Seek(i, NURO_SEEK_CUR);
// 	file.Read(&i, 1, 1);
	nuBYTE i = 0;
	nuBYTE buffer[400] = {0};
	file.Seek(addr, NURO_SEEK_SET);
	file.Read(&buffer, 1, 400);
	i = buffer[buffer[0]+1];
	return buffer[buffer[0]+i+2];
}
nuUINT CMapFilePnY::GetName(nuUINT addr, nuVOID *pBuffer, nuUINT nMaxLen, nuBYTE *pStrokes)
{
	if (!pBuffer || nMaxLen<EACH_POI_NAME_LEN)
	{
		return 0;
	}
	nuMemset(pBuffer, 0, nMaxLen);
	nuBYTE len = 0;
	file.ReadData(addr, &len, 1);
	if (len>(nMaxLen-2))
	{
		len = nMaxLen -2;
	}
	file.Read(pBuffer, len, 1);
	if (pStrokes)
	{
		file.Read(&len, 1, 1);
		file.Seek(len, NURO_SEEK_CUR);
		file.Read(pStrokes, 1, 1);
	}
	return len;
}
nuUINT CMapFilePnY::GetPoiInfoLen_City(nuWORD nCityID)
{
	return -1;
}
nuUINT CMapFilePnY::GetPoiInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen, nuUINT* pBaseAddr)
{
	if (!pBuffer || !pBaseAddr || nCityID>=m_nCityCount)
	{
		return 0;
	}
	NURO_PN_CITYINFO info = {0};
	if (!GetCityInfo(nCityID, &info))
	{
		return 0;
	}
	if (info.nCityID!=nCityID)
	{
		return 0;
	}
	nuUINT startPos = info.nStartPos;
	nuUINT len = 0;
	if (nCityID == m_nCityCount-1)
	{
		len = file.GetLength();
		if (len<startPos)
		{
			return 0;
		}
		len = len - startPos;
	}
	else
	{
		if (!GetCityInfo(nCityID+1, &info))
		{
			return 0;
		}
		if (info.nStartPos<startPos)
		{
			return 0;
		}
		len = info.nStartPos - startPos;
	}
	if (!len || len>nMaxLen)
	{
		return 0;
	}
	file.Seek(startPos, NURO_SEEK_SET);
	if (file.Read(pBuffer, len, 1)!=1)
	{
		return 0;
	}

	*pBaseAddr = startPos;
	return len;
}