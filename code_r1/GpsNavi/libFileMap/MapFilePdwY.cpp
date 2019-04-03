
#include "MapFilePdwY.h"

CMapFilePdwY::CMapFilePdwY()
{

}

CMapFilePdwY::~CMapFilePdwY()
{
	Close();
}

nuBOOL CMapFilePdwY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".pdw"));
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
nuVOID CMapFilePdwY::Close()
{
	file.Close();
}
nuBOOL CMapFilePdwY::IsOpen()
{
	return file.IsOpen();
}
nuUINT CMapFilePdwY::GetBlkPoiNum(nuUINT blkaddr)
{
	if (blkaddr == (nuUINT)-1)
	{
		return -1;
	}
	nuUINT num = 0;
	if (file.ReadData(blkaddr, &num, 4))
	{
		return num;
	}
	return 0;
}
nuUINT CMapFilePdwY::GetBlkPoiInfo(nuUINT blkaddr, nuVOID *pBuffer, nuUINT nMaxLen)
{
	nuUINT num = GetBlkPoiNum(blkaddr);
	if (!num)
	{
		return 0;
	}
	num *= sizeof(SEARCH_PDW_A1);
	if (num>nMaxLen)
	{
		return 0;
	}
	if (file.ReadData(blkaddr+4, pBuffer, num))
	{
		return num;
	}
	return 0;
}
nuBOOL CMapFilePdwY::GetBlkPoiInfo_ByIdx(nuUINT blkaddr, nuUINT idx, SEARCH_PDW_A1 *pInfo)
{
	return file.ReadData(blkaddr+idx*sizeof(SEARCH_PDW_A1), pInfo, sizeof(SEARCH_PDW_A1));
}