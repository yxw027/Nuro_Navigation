
#include "MapFileRnY.h"

CMapFileRnY::CMapFileRnY()
{
	
}

CMapFileRnY::~CMapFileRnY()
{
	Close();
}

nuBOOL CMapFileRnY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID==(nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".rn"));
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
nuUINT CMapFileRnY::GetLength()
{
	return file.GetLength();
}
nuBOOL CMapFileRnY::ReadData(nuUINT addr, nuVOID *p, nuUINT len)
{
	return file.ReadData(addr, p, len);
}
nuBOOL CMapFileRnY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileRnY::Close()
{
	file.Close();
}

nuBOOL CMapFileRnY::GetRoadName(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen)
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
nuBOOL CMapFileRnY::GetRoadDanyin(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen)
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