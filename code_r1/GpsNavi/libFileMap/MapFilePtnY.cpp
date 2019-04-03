
#include "MapFilePtnY.h"

CMapFilePtnY::CMapFilePtnY()
{
	m_nTp1Count = 0;
}

CMapFilePtnY::~CMapFilePtnY()
{
	Close();
}

nuBOOL CMapFilePtnY::Open(long mapid)
{
	if (mapid==(long)-1)
	{
		return nuFALSE;
	}
	Close();
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuTcscpy(tcsPath,nuTEXT(".ptn"));
	if (!m_pFsApi->FS_FindFileWholePath(mapid,tcsPath,NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&m_nTp1Count, 1, 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFilePtnY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFilePtnY::Close()
{
	file.Close();
	m_nTp1Count = 0;
}

nuBYTE CMapFilePtnY::GetTP1Num()
{
	return m_nTp1Count;
}

nuUINT CMapFilePtnY::GetTP1Name(nuWORD idx, nuBYTE* pBuffer, nuUINT nMaxLen)
{
	nuUINT pos = 0;
	if (nMaxLen<EACH_TYPE_NAME_LEN || !pBuffer)
	{
		return 0;
	}
	file.Seek(1+4*idx, NURO_SEEK_SET);
	if (file.Read(&pos, 4, 1)!=1)
	{
		return 0;
	}
	file.Seek(pos, NURO_SEEK_SET);
	if (file.Read(pBuffer, EACH_TYPE_NAME_LEN, 1)!=1)
	{
		return 0;
	}
	return EACH_TYPE_NAME_LEN;
}
nuUINT CMapFilePtnY::GetTP1NameAll(nuBYTE* pBuffer, nuUINT nMaxLen)
{
	nuUINT pos = 0;
	nuBYTE count = nMaxLen/EACH_TYPE_NAME_LEN;
	if (count>m_nTp1Count)
	{
		count = m_nTp1Count;
	}
	for (nuBYTE i=0; i<count; i++)
	{
		file.Seek(1+4*i, NURO_SEEK_SET);
		if (file.Read(&pos, 4, 1)!=1)
		{
			return i;
		}
		file.Seek(pos, NURO_SEEK_SET);
		if (file.Read(pBuffer, EACH_TYPE_NAME_LEN, 1)!=1)
		{
			return i;
		}
		pBuffer += EACH_TYPE_NAME_LEN;
	}
	return count;
}
nuUINT CMapFilePtnY::GetTP2NameAll(nuWORD TP1Idx, nuBYTE* pBuffer, nuUINT nMaxLen)
{
	nuUINT pos = 0;
	if (!pBuffer)
	{
		return 0;
	}
	file.Seek(1+4*TP1Idx, NURO_SEEK_SET);
	if (file.Read(&pos, 4, 1)!=1)
	{
		return 0;
	}
	nuBYTE num = 0;
	file.Seek(pos+EACH_TYPE_NAME_LEN, NURO_SEEK_SET);
	if (file.Read(&num, sizeof(num), 1)!=1)
	{
		return 0;
	}
	if (nuUINT(num*EACH_TYPE_NAME_LEN) > nMaxLen)
	{
		num = nMaxLen/EACH_TYPE_NAME_LEN;
	}
	nuUINT pos2 = 0;
	nuBYTE *pNameBuffer = (nuBYTE*)pBuffer;
	for (int i=0; i<num; i++)
	{
		if (!file.ReadData(pos+EACH_TYPE_NAME_LEN+1+(i<<2), &pos2, 4))
		{
			return i;
		}
		if (!file.ReadData(pos2, pNameBuffer, EACH_TYPE_NAME_LEN))
		{
			return i;
		}
		pNameBuffer += EACH_TYPE_NAME_LEN;
	}
	return num;
}

