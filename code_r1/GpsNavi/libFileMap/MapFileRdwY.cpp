
#include "MapFileRdwY.h"

CMapFileRdwY::CMapFileRdwY()
{

}

CMapFileRdwY::~CMapFileRdwY()
{

}
nuBOOL CMapFileRdwY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".rdw"));
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
nuBOOL CMapFileRdwY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileRdwY::Close()
{
	file.Close();
}

nuBOOL CMapFileRdwY::GetBlkInfo(nuUINT addr, SEARCH_RDW_BLKINFO* pbi)
{
	if (!pbi || addr==(nuUINT)-1)
	{
		return nuFALSE;
	}
	file.Seek(addr, NURO_SEEK_SET);
	if (file.Read(pbi, sizeof(SEARCH_RDW_BLKINFO), 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CMapFileRdwY::GetFileData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen)
{
	return file.ReadData(nDataAddr, pBuffer, nLen);
}

nuBOOL CMapFileRdwY::GetRoadPos(nuUINT nBlkAddr, nuUINT nRoadIdx, NUROPOINT *pos, SEARCH_RDW_BLKINFO *pBlkInfo)
{
	if (!pos)
	{
		return nuFALSE;
	}
	SEARCH_RDW_BLKINFO *pBlkInfoR = NULL;
	SEARCH_RDW_BLKINFO blkinfo_;
	if (pBlkInfo)
	{
		pBlkInfoR = pBlkInfo;
	}
	else
	{
		if (!GetBlkInfo(nBlkAddr, &blkinfo_))
		{
			return nuFALSE;
		}
		pBlkInfoR = &blkinfo_;
	}
	if (nRoadIdx==(nuUINT)-1)
	{
		return nuFALSE;
	}
	if (nRoadIdx>=pBlkInfoR->RoadCount)
	{
		return nuFALSE;
	}
	SEARCH_RDW_A1 a1;
	nuUINT seek = nBlkAddr+sizeof(SEARCH_RDW_BLKINFO)+sizeof(SEARCH_RDW_A1)*nRoadIdx;
	NUROSPOINT pt;
	if (file.ReadData(seek, &a1, sizeof(a1)))
	{
		seek = nBlkAddr+sizeof(SEARCH_RDW_BLKINFO)
			+ sizeof(SEARCH_RDW_A1)*pBlkInfoR->RoadCount
			+ sizeof(SEARCH_RDW_A2)*pBlkInfoR->RoadCommonCount
			+ sizeof(NUROSPOINT)*(a1.VertexAddCount+(a1.VertexCount>>1));
		if (file.ReadData(seek, &pt, sizeof(pt)))
		{
			pos->x = pt.x;
			pos->y = pt.y;
			return nuTRUE;
		}
	}
	return nuFALSE;
}
// nuWORD CMapFileRdwY::GetRoadTownID(nuUINT nBlkAddr, nuUINT nRoadIdx)
// {
// 	SEARCH_RDW_BLKINFO blkinfo;
// 	if (!GetBlkInfo(nBlkAddr, &blkinfo))
// 	{
// 		return -1;
// 	}
// 	if (nRoadIdx==(nuUINT)-1)
// 	{
// 		return -1;
// 	}
// 	if (nRoadIdx>=blkinfo.RoadCount)
// 	{
// 		return -1;
// 	}
// 	nuWORD ncommidx = -1;
// 	nuUINT seek = sizeof(blkinfo)+sizeof(SEARCH_RDW_A1)*nRoadIdx;
// 	file.Seek(nBlkAddr+seek, NURO_SEEK_SET);
// 	if (file.Read(&ncommidx, 2, 1)!=1)
// 	{
// 		return -1;
// 	}
// 	if (ncommidx>=blkinfo.RoadCommonCount)
// 	{
// 		return -1;
// 	}
// 	file.Seek(nBlkAddr+sizeof(blkinfo)+sizeof(SEARCH_RDW_A1)*blkinfo.RoadCount+sizeof(SEARCH_RDW_A2)*ncommidx
// 		, NURO_SEEK_SET);
// 	SEARCH_RDW_A2 A2;
// 	if (file.Read(&A2, sizeof(A2), 1)!=1)
// 	{
// 		return -1;
// 	}
// 	return A2.TownID;
// }
nuUINT CMapFileRdwY::GetRoadNameAddr(nuUINT nBlkAddr, nuUINT nRoadIdx, SEARCH_RDW_BLKINFO *pBlkInfo)
{
	SEARCH_RDW_BLKINFO *pBlkInfoR = NULL;
	SEARCH_RDW_BLKINFO blkinfo_;
	if (pBlkInfo)
	{
		pBlkInfoR = pBlkInfo;
	}
	else
	{
		if (!GetBlkInfo(nBlkAddr, &blkinfo_))
		{
			return nuFALSE;
		}
		pBlkInfoR = &blkinfo_;
	}
	if (nRoadIdx>=pBlkInfoR->RoadCount)
	{
		return -1;
	}
	nuWORD ncommidx = -1;
	nuUINT seek = sizeof(SEARCH_RDW_BLKINFO)+sizeof(SEARCH_RDW_A1)*nRoadIdx;
	file.Seek(nBlkAddr+seek, NURO_SEEK_SET);
	if (file.Read(&ncommidx, 2, 1)!=1)
	{
		return -1;
	}
	if (ncommidx>=pBlkInfoR->RoadCommonCount)
	{
		return -1;
	}
	file.Seek(nBlkAddr+sizeof(SEARCH_RDW_BLKINFO)+sizeof(SEARCH_RDW_A1)*pBlkInfoR->RoadCount+sizeof(SEARCH_RDW_A2)*ncommidx
		, NURO_SEEK_SET);
	SEARCH_RDW_A2 A2;
	if (file.Read(&A2, sizeof(A2), 1)!=1)
	{
		return -1;
	}
	return A2.RoadNameAddr;
}
nuBOOL CMapFileRdwY::GetRoadA2Info(nuUINT nBlkAddr, nuUINT nRoadIdx, SEARCH_RDW_A2 &A2, SEARCH_RDW_BLKINFO *pBlkInfo)
{
	SEARCH_RDW_BLKINFO *pBlkInfoR = NULL;
	SEARCH_RDW_BLKINFO blkinfo_;
	if (pBlkInfo)
	{
		pBlkInfoR = pBlkInfo;
	}
	else
	{
		if (!GetBlkInfo(nBlkAddr, &blkinfo_))
		{
			return nuFALSE;
		}
		pBlkInfoR = &blkinfo_;
		if (nRoadIdx>=pBlkInfoR->RoadCount)
		{
			return nuFALSE;
		}
	}
	nuWORD ncommidx = -1;
	nuUINT seek = sizeof(SEARCH_RDW_BLKINFO)+sizeof(SEARCH_RDW_A1)*nRoadIdx;
	file.Seek(nBlkAddr+seek, NURO_SEEK_SET);
	if (file.Read(&ncommidx, 2, 1)!=1)
	{
		return nuFALSE;
	}
	if (ncommidx>=pBlkInfoR->RoadCommonCount)
	{
		return nuFALSE;
	}
	file.Seek(nBlkAddr+sizeof(SEARCH_RDW_BLKINFO)+sizeof(SEARCH_RDW_A1)*pBlkInfoR->RoadCount+sizeof(SEARCH_RDW_A2)*ncommidx
		, NURO_SEEK_SET);
	if (file.Read(&A2, sizeof(A2), 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}