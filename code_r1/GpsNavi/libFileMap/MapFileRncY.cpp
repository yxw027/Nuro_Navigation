
#include "MapFileRncY.h"

CMapFileRncY::CMapFileRncY()
{
	nuMemset(&header, 0, sizeof(header));
	citycount = 0;
}

CMapFileRncY::~CMapFileRncY()
{
	Close();
}

nuBOOL CMapFileRncY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID==(nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".rnc"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&header, sizeof(header), 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileRncY::IsOpen()
{
	return file.IsOpen();
}

nuVOID CMapFileRncY::Close()
{
	file.Close();
	nuMemset(&header, 0, sizeof(header));
	citycount = 0;
}

nuUINT CMapFileRncY::GetCityCount()
{
	if (!citycount)
	{
		file.Seek(header.nCTInfoStartAddr, NURO_SEEK_SET);
		if (file.Read(&citycount, sizeof(citycount), 1)!=1)
		{
			return 0;
		}
	}
	return citycount;
}
nuUINT CMapFileRncY::GetTownCount(nuINT nCity , nuDWORD &nStartaddr)
{
	nuUINT towncount=0;
	nuINT  nCount=0;
	nuLONG  nRncCity=0;
	nuLONG nAddr=0;
	
	file.Seek(header.nCTInfoStartAddr, NURO_SEEK_SET);
	
	if (file.Read(&nCount, sizeof(nCount), 1)!=1)
	{
		return 0;
	}
	nAddr+=sizeof(nCount);

	if(nCity > nCount)
	{
		return 0;
	}

	for(nuINT i=0; i<nCount;i++)
	{
		file.Read(&nRncCity, 2, 1);
		nAddr+=2;
		if( nCity != nRncCity)
		{
			nuLONG nTownCount=0;
			file.Read(&nTownCount, 2, 1);
			nAddr+=2;
			file.Seek(4+12*nTownCount, NURO_SEEK_CUR);
			nAddr+=4+12*nTownCount;
			continue;
		}
		if (file.Read(&towncount, 2, 1)!=1)
		{
			return 0;
		}
		nAddr+=2;
		break;
	}
	if (file.Read(&nStartaddr, 4, 1)!=1)
	{
		return 0;
	}
	nAddr+=4;
	SEARCH_RNC_TOWNINFO temp = {0};
	nStartaddr = header.nCTInfoStartAddr+nAddr;;

//	file.Seek(nStartaddr, NURO_SEEK_SET);
		if (file.Read(&temp, sizeof(temp), 1)!=1)
		{
			return 0;
		}
	return towncount;
}
nuUINT CMapFileRncY::GetLength()
{
	return file.GetLength();
}

nuBOOL CMapFileRncY::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT nLen)
{
	return file.ReadData(addr, pBuffer, nLen);
}

nuUINT CMapFileRncY::GetRNCInfo(nuWORD nCityID, nuWORD nTownID, nuBYTE* pBuffer, nuUINT nMaxLen)
{
	if (nCityID>=GetCityCount() || nMaxLen<4)
	{
		return 0;
	}
	file.Seek(header.nCTInfoStartAddr+4, NURO_SEEK_SET);
	SEARCH_RNC_CITYINFO ci = {0};
	if (file.Read(&ci, sizeof(ci), 1)!=1)
	{
		return 0;
	}
	while (ci.nCityCode < nCityID)
	{
		file.Seek(ci.nNextCityAddr, NURO_SEEK_SET);
		if (file.Read(&ci, sizeof(ci), 1)!=1)
		{
			return 0;
		}
	}
	if (ci.nCityCode!=nCityID)
	{
		return 0;
	}
	nuUINT nMMTotalNeed = 0;
	nuBYTE *p = pBuffer;
	SEARCH_RNC_TOWNINFO *pti = NULL;

	if (nTownID != (nuWORD)-1)
	{
		if (nTownID>=ci.nTownCount)
		{
			return 0;
		}
		*((nuWORD*)pBuffer) = 1;		
		nMMTotalNeed = 4 + sizeof(SEARCH_RNC_TOWNINFO);
		if (nMaxLen<nMMTotalNeed)
		{
			return 0;
		}
		p += 4;
		pti = (SEARCH_RNC_TOWNINFO*)p;
		file.Seek(nTownID*sizeof(SEARCH_RNC_TOWNINFO), NURO_SEEK_CUR);
		if (file.Read(pti, sizeof(SEARCH_RNC_TOWNINFO), 1)!=1)
		{
			return 0;
		}
		nMMTotalNeed += pti->nRoadCount * 4;
		if (nMaxLen<nMMTotalNeed)
		{
			return 0;
		}
		p += sizeof(SEARCH_RNC_TOWNINFO);
		*((nuWORD*)(pBuffer+2)) = pti->nRoadCount;
		file.Seek(pti->nRNStartAddr, NURO_SEEK_SET);
		if (file.Read(p, pti->nRoadCount * 4, 1)!=1)
		{
			return 0;
		}
		pti->nRNStartAddr = 0;
		return nMMTotalNeed;
	}
	else
	{
		nuUINT nRoadTotal = 0;	
		*((nuWORD*)pBuffer) = ci.nTownCount;		

		nMMTotalNeed = 4 + sizeof(SEARCH_RNC_TOWNINFO)*ci.nTownCount;
		if (nMaxLen<nMMTotalNeed)
		{
			return 0;
		}
		p+=4;
		pti = (SEARCH_RNC_TOWNINFO*)p;
		if (file.Read(pti, sizeof(SEARCH_RNC_TOWNINFO), ci.nTownCount)!=ci.nTownCount)
		{
			return 0;
		}
		p+=sizeof(SEARCH_RNC_TOWNINFO)*ci.nTownCount;
		for (int i=0; i<ci.nTownCount; i++)
		{
			nMMTotalNeed += (4*pti[i].nRoadCount);
			if (nMaxLen<nMMTotalNeed)
			{
				return 0;
			}
			file.Seek(pti[i].nRNStartAddr, NURO_SEEK_SET);
			if (file.Read(p, 4, pti[i].nRoadCount)!=pti[i].nRoadCount)
			{
				return 0;
			}
			p+=(4*pti[i].nRoadCount);
			pti[i].nRNStartAddr = nRoadTotal;
			nRoadTotal += pti[i].nRoadCount;
		}
		*((nuWORD*)(pBuffer+2)) = nRoadTotal;
		return nMMTotalNeed;
	}
}