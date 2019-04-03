
#include "MapFileCbZ.h"

CMapFileCbZ::CMapFileCbZ()
{
	nuMemset(&cbHeader, 0, sizeof(cbHeader));
}

CMapFileCbZ::~CMapFileCbZ()
{
	Close();
}

nuBOOL CMapFileCbZ::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".cb"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if( file.Read(&cbHeader, sizeof(cbHeader), 1) != 1 )
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileCbZ::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileCbZ::Close()
{
	file.Close();
	nuMemset(&cbHeader, 0, sizeof(cbHeader));
}
nuUINT CMapFileCbZ::GetLength()
{
	return file.GetLength();
}
nuUINT CMapFileCbZ::GetCityNum() const
{
	return cbHeader.nCityCount;
}

nuUINT CMapFileCbZ::GetTownNum() const
{
	return cbHeader.nTownCount;
}

nuUINT CMapFileCbZ::GetCityName(nuUINT nStartCID, nuUINT nNum, nuVOID* pBuffer, nuUINT nBufferLen)
{
	if (!pBuffer || nStartCID>=cbHeader.nCityCount)
	{
		return 0;
	}
	nNum = NURO_MIN(nNum, nBufferLen/EACH_CITY_NAME_LEN);
	if( cbHeader.nCityCount < nStartCID + nNum )
	{
		nNum = nuLOWORD(cbHeader.nCityCount - nStartCID);
	}
	if (!nNum)
	{
		return 0;
	}
	if (file.Seek(nStartCID*EACH_CITY_NAME_LEN+sizeof(cbHeader), NURO_SEEK_SET))
	{
		return 0;
	}
	nuMemset(pBuffer, 0, nBufferLen);
	return file.Read(pBuffer, EACH_CITY_NAME_LEN, nNum);
}

nuUINT CMapFileCbZ::GetTownInfo_CTIdx(nuVOID* pBuffer, nuUINT nBufferLen)
{
	nuUINT len = cbHeader.nTownCount*4;
	if (!pBuffer || nBufferLen<len)
	{
		return 0;
	}
	if (file.Seek(-(nuLONG)len, NURO_SEEK_END))
	{
		return 0;
	}
	if (file.Read(pBuffer, len, 1)!=1)
	{
		return 0;
	}
	return len;
}

nuWORD CMapFileCbZ::GetTownInfo_CityCode(nuUINT nTownAddr)
{
	nuWORD id = 0;
	if (file.Seek(nTownAddr, NURO_SEEK_SET))
	{
		return -1;
	}
	else if (file.Read(&id, sizeof(id), 1) == 1)
	{
		return id;
	}
	else
	{
		return -1;
	}	
}
nuWORD CMapFileCbZ::GetTownInfo_TownCode(nuUINT nTownAddr)
{
	nuWORD id = 0;
	if (file.Seek(nTownAddr+2, NURO_SEEK_SET))
	{
		return -1;
	}
	else if (file.Read(&id, sizeof(id), 1) == 1)
	{
		return id;
	}
	else
	{
		return -1;
	}
}
nuBOOL CMapFileCbZ::GetTownInfo(nuUINT nTownAddr, SEARCH_CB_TOWNINFO* pTownInfo )
{
	if (!pTownInfo)
	{
		return nuFALSE;
	}
	else if (file.Seek(nTownAddr, NURO_SEEK_SET))
	{
		return nuFALSE;
	}
	else if (file.Read(pTownInfo, sizeof(SEARCH_CB_TOWNINFO), 1) == 1)
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

nuBOOL CMapFileCbZ::ReadData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen)
{
	return file.ReadData(nDataAddr, pBuffer, nLen);
}