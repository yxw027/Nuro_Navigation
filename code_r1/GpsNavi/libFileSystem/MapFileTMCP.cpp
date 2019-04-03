
#include "MapFileTMCP.h"

CMapFileTMCP::CMapFileTMCP()
{
	
}

CMapFileTMCP::~CMapFileTMCP()
{
	Close();
}

nuBOOL CMapFileTMCP::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".tmc"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if( file.Read(&tmcHeader, sizeof(tmcHeader), 1) != 1 )
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileTMCP::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileTMCP::Close()
{
	file.Close();
	nuMemset(&tmcHeader, 0, sizeof(tmcHeader));
}
nuUINT CMapFileTMCP::GetLength()
{
	return file.GetLength();
}


nuBOOL CMapFileTMCP::ReadData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen)
{
	return file.ReadData(nDataAddr, pBuffer, nLen);
}
