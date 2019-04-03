
#include "MapFileRnP.h"

CMapFileRnP::CMapFileRnP()
{
	
}

CMapFileRnP::~CMapFileRnP()
{
	Close();
}

nuBOOL CMapFileRnP::Open(nuLONG nMapID)
{
	Close();
	if (nMapID==(nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".rn0"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuUINT CMapFileRnP::GetLength()
{
	return file.GetLength();
}
nuBOOL CMapFileRnP::ReadData(nuUINT addr, nuVOID *p, nuUINT len)
{
	return file.ReadData(addr, p, len);
}
nuBOOL CMapFileRnP::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileRnP::Close()
{
	file.Close();
}

nuBOOL CMapFileRnP::GetRoadName(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen)
{
	if (nMaxLen<EACH_ROAD_NAME_LEN)
	{
		return nuFALSE;
	}
	file.Seek(addr, NURO_SEEK_SET);
	nuBYTE nLen = 0;
	if (file.Read(&nLen, sizeof(nLen), 1)!=1)
	{
		return nuFALSE;
	}
	if (!nLen)
	{
		return nuFALSE;
	}
	if (nLen>(EACH_ROAD_NAME_LEN-2))
	{
		nLen = (EACH_ROAD_NAME_LEN-2);
	}
	file.Seek(3, NURO_SEEK_CUR);
	if (file.Read(pBuffer, nLen, 1)!=1)
	{
		return nuFALSE;
	}
	((nuWORD*)pBuffer)[nLen>>1] = 0;
	if (pResLen)
	{
		*pResLen = nLen;
	}
	return nuTRUE;
}

nuBOOL CMapFileRnP::GetRoadDanyin(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen)
{
	if (nMaxLen<EACH_ROAD_NAME_LEN)
	{
		return nuFALSE;
	}
	file.Seek(addr, NURO_SEEK_SET);
	nuBYTE nLen = 0;
	if (file.Read(&nLen, sizeof(nLen), 1)!=1)
	{
		return nuFALSE;
	}
	file.Seek(3+nLen, NURO_SEEK_CUR);
	if (file.Read(&nLen, sizeof(nLen), 1)!=1)
	{
		return nuFALSE;
	}
	if (!nLen)
	{
		return nuFALSE;
	}
	if (nLen>(EACH_ROAD_NAME_LEN-2))
	{
		nLen = (EACH_ROAD_NAME_LEN-2);
	}
	if (file.Read(pBuffer, nLen, 1)!=1)
	{
		return nuFALSE;
	}
	((nuWORD*)pBuffer)[nLen>>1] = 0;
	if (pResLen)
	{
		*pResLen = nLen;
	}
	return nuTRUE;
}