
#include "MapFileRefY.h"

CMapFileRefY::CMapFileRefY()
{
	total = 0;
}

CMapFileRefY::~CMapFileRefY()
{
	Close();
}

nuBOOL CMapFileRefY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".ref"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&total, 4, 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CMapFileRefY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileRefY::Close()
{
	file.Close();
}
nuUINT CMapFileRefY::GetTotalNum()
{
	return total;
}
nuUINT CMapFileRefY::GetRtInfo(const nuWORD &nCityID, const nuWORD &nTownID, nuVOID *pBuffer, nuUINT nMaxLen)
{
	nuUINT len = file.GetLength();
	if (len!=(4+(total<<3)))
	{
		return 0;
	}
	len -= 4;
	if (len>nMaxLen)
	{
		return 0;
	}
	if (!file.ReadData(4, pBuffer, len))
	{
		return 0;
	}
	return len;
}
nuUINT CMapFileRefY::GetRtIdxEx(const nuLONG &nMapID, const nuWORD &nCityID, const nuWORD &nTownID)
{
	CMapFileRefY file;
	if (!file.Open(nMapID))
	{
		return -1;
	}
	nuUINT len = file.GetTotalNum()<<3;
	SEARCH_REF_INFO *p = (SEARCH_REF_INFO*)nuMalloc(len);
	if (!p)
	{
		return -1;
	}
	if (!file.GetRtInfo(nCityID, nTownID, p, len))
	{
		nuFree(p);
		return -1;
	}
	nuUINT start = 0;
	nuUINT end = file.GetTotalNum() -1;
	nuUINT center = 0;
	nuUINT rta = -1;
	if (p[start].cityID==nCityID && p[start].townID==nTownID)
	{
		rta = p[start].rtAreaID;
		nuFree(p);
		return rta;
	}
	if (p[end].cityID==nCityID && p[end].townID==nTownID)
	{
		rta = p[end].rtAreaID;
		nuFree(p);
		return rta;
	}
	while (end-start>1)
	{
		center = (end+start)>>1;
		if (p[center].cityID==nCityID && p[center].townID==nTownID)
		{
			nuUINT rta = p[center].rtAreaID;
			nuFree(p);
			return rta;
		}
		else if ((p[center].cityID>nCityID) || (p[center].cityID==nCityID && p[center].townID>nTownID))
		{
			end = center;
		}
		else
		{
			start = center;
		}
	}
	nuFree(p);
	return -1;
}