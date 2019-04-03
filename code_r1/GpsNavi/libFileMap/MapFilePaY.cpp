
#include "MapFilePaY.h"

CMapFilePaY::CMapFilePaY()
{
}

CMapFilePaY::~CMapFilePaY()
{
	Close();
}

nuBOOL CMapFilePaY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuMemset(tcsPath, 0, sizeof(tcsPath));
	nuTcscpy(tcsPath, nuTEXT(".PA"));
	if (!m_pFsApi->FS_FindFileWholePath(nMapID, tcsPath, NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CMapFilePaY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFilePaY::Close()
{
	file.Close();
}
nuUINT CMapFilePaY::GetLength()
{
	return file.GetLength();
}
nuBOOL CMapFilePaY::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len)
{
	return file.ReadData(addr, pBuffer, len);
}
nuBOOL CMapFilePaY::GetPaInfo(nuUINT infoAddr, SEARCH_PA_INFO *pPaInfo)
{
	if (!pPaInfo || infoAddr==(nuUINT)-1)
	{
		return nuFALSE;
	}
	nuMemset(pPaInfo, 0, sizeof(SEARCH_PA_INFO));
	nuBYTE l1 = 0;
	nuUINT lAll = infoAddr;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//phone
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->phone, NURO_MIN(l1, 62), 1)!=1)
		{
			return nuFALSE;
		}
	}

	lAll += 1+l1;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//name
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->nameext, NURO_MIN(l1, 254), 1)!=1)
		{
			return nuFALSE;
		}
	}

	lAll += 1+l1;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//fex
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->fex, NURO_MIN(l1, 62), 1)!=1)
		{
			return nuFALSE;
		}
	}

	lAll += 1+l1;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//addr
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->addr, NURO_MIN(l1, 254), 1)!=1)
		{
			return nuFALSE;
		}
	}

	lAll += 1+l1;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//web
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->web, NURO_MIN(l1, 254), 1)!=1)
		{
			return nuFALSE;
		}
	}

	lAll += 1+l1;
	file.Seek(lAll, NURO_SEEK_SET);
	if (file.Read(&l1, 1, 1)!=1)//open time
	{
		return nuFALSE;
	}
	if (l1==(nuBYTE)-1)
	{
		l1 = 0;
	}
	if (l1)
	{
		if (file.Read(pPaInfo->time, NURO_MIN(l1, 254), 1)!=1)
		{
			return nuFALSE;
		}
	}

	return nuTRUE;
}