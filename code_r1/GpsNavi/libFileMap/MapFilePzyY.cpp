
#include "MapFilePzyY.h"

CMapFilePzyY::CMapFilePzyY()
{
	totalcount = 0;
}

CMapFilePzyY::~CMapFilePzyY()
{
	Close();
}

nuBOOL CMapFilePzyY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID==(nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".pzy"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&totalcount, sizeof(totalcount), 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFilePzyY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFilePzyY::Close()
{
	file.Close();
	totalcount = 0;
}
nuUINT CMapFilePzyY::GetLength()
{
	return file.GetLength();
}
nuBOOL CMapFilePzyY::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len)
{
	return file.ReadData(addr, pBuffer, len);
}
nuUINT CMapFilePzyY::GetPzyInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen)
{
	nuUINT nRes = 0;
	if ((nuWORD)-1==nCityID)
	{
		nRes = file.GetLength();
		file.Seek(sizeof(totalcount)+totalcount*sizeof(SEARCH_SZY_CITYINFO), NURO_SEEK_SET);
		nRes -= file.Tell();
	}
	else
	{
		if (nCityID>=totalcount)
		{
			return 0;
		}
		file.Seek(sizeof(totalcount)+nCityID*sizeof(SEARCH_SZY_CITYINFO), NURO_SEEK_SET);
		SEARCH_SZY_CITYINFO ci;
		if (file.Read(&ci, sizeof(ci), 1)!=1)
		{
			return 0;
		}
		if (nCityID!=ci.nCityCode)
		{
			return 0;
		}
		nRes = sizeof(SEARCH_SZY_STROKE)*ci.nWordCount;
		file.Seek(ci.nStartAddr, NURO_SEEK_SET);
	}
	if (nRes>nMaxLen)
	{
		return 0;
	}
	if (file.Read(pBuffer, nRes, 1)!=1)
	{
		return 0;
	}
	return nRes;
}