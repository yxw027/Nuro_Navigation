
#include "../nuCFile.h"
#include "../../NuroDefine.h"

nuCFile::nuCFile()
{
	fp = NULL;
}

nuCFile::~nuCFile()
{
	Close();
}

nuBOOL nuCFile::Open(const nuTCHAR *fileName, nuINT mode)
{
	fp = nuTfopen(fileName, mode);
	if (!fp)
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL nuCFile::IsOpen()
{
	if (fp)
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuINT  nuCFile::Seek(nuLONG nDis, nuINT nType)
{
	return nuFseek(fp, nDis, nType);
}

nuSIZE	nuCFile::Write(const void* pBuff, nuSIZE size, nuSIZE count)
{
	return nuFwrite(pBuff, size, count, fp);
}

nuSIZE	nuCFile::Read(void* pBuff, nuSIZE size, nuSIZE count)
{
	return nuFread(pBuff, size, count, fp);
}

void nuCFile::Close()
{
	if (fp)
	{
		nuINT res = nuFclose(fp);
        NURO_UNUSED(res);
		fp = NULL;
	}	
}

nuSIZE	nuCFile::Tell()
{
	return nuFtell(fp);
}

nuSIZE	nuCFile::GetLength()
{
	return nuFgetlen(fp);
}

nuSIZE	nuCFile::ReadLineW(nuWCHAR* pBuff,  nuSIZE nMaxLen)
{
	return nuFreadLine(pBuff, nMaxLen, fp);
}

nuSIZE	nuCFile::ReadLineA(nuCHAR* pBuff,  nuSIZE nMaxLen)
{
	return nuFreadLineA(pBuff, nMaxLen, fp);
}

nuBOOL nuCFile::ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen)
{
	Seek(nDataStartAddr, NURO_SEEK_SET);

	if (Read(pBuffer, nLen, 1)!=1)
	{
		return nuFALSE;
	}

	return nuTRUE;
}
