// MapFileCarY.cpp: implementation of the CMapFileCarY class.
//
//////////////////////////////////////////////////////////////////////
#include "MapFileCarY.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileCarY::CMapFileCarY()
{
	typeCount = 0;
}

CMapFileCarY::~CMapFileCarY()
{
	Close();
	typeCount = 0;
}

nuBOOL CMapFileCarY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuMemset(tcsPath, 0, sizeof(tcsPath));
	nuTcscpy(tcsPath, nuTEXT(".CAR"));
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

nuBOOL CMapFileCarY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFileCarY::Close()
{
	file.Close();
}

nuUINT CMapFileCarY::GetTypeNum()
{
	return typeCount;
}

nuUINT CMapFileCarY::GetLength()
{
	return file.GetLength();
}

nuBOOL CMapFileCarY::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len)
{
	return file.ReadData(addr, pBuffer, len);
}

nuUINT CMapFileCarY::GetCarFacNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
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