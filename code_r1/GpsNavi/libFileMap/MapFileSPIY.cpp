// MapFileSPIY.cpp: implementation of the CMapFileSPIY class.
//
//////////////////////////////////////////////////////////////////////

#include "MapFileSPIY.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileSPIY::CMapFileSPIY()
{
	typeCount = 0;
}

CMapFileSPIY::~CMapFileSPIY()
{
	Close();
	typeCount = 0;
}

nuBOOL CMapFileSPIY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuMemset(tcsPath, 0, sizeof(tcsPath));
	nuTcscpy(tcsPath, nuTEXT(".SPI"));
	if (!m_pFsApi->FS_FindFileWholePath(nMapID, tcsPath, NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&typeCount, sizeof(typeCount), 1)!=1)
	{
        Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileSPIY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFileSPIY::Close()
{
	file.Close();
}

nuUINT CMapFileSPIY::GetTypeNum()
{
	return typeCount;
}

nuUINT CMapFileSPIY::GetLength()
{
	return file.GetLength();
}

nuBOOL CMapFileSPIY::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len)
{
	return file.ReadData(addr, pBuffer, len);
}

nuUINT CMapFileSPIY::GetSPTypeNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	nuUINT nLoad = nMaxLen/EACH_TYPE_NAME_LEN;
	nuBYTE* pNameBuffer = (nuBYTE*)pBuffer;
	if (nLoad>typeCount)
	{
		nLoad = typeCount;
	}
	file.Seek(4, NURO_SEEK_SET);
	nuUINT i = 0;
	while (i<nLoad)
	{
		if (file.Read(pNameBuffer, EACH_TYPE_NAME_LEN, 1)!=1)
		{
			return 0;
		}
		file.Seek(8, NURO_SEEK_CUR);
		pNameBuffer+=EACH_TYPE_NAME_LEN;
		++i;
	}
	return i;
}