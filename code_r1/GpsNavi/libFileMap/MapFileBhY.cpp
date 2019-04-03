
#include "MapFileBhY.h"

CMapFileBhY::CMapFileBhY():blkNum(0)
{
    nuMemset(&m_head, 0, sizeof(m_head));
}

CMapFileBhY::~CMapFileBhY()
{
	Close();
}

nuBOOL CMapFileBhY::Open(long nMapID)
{
	Close();
	if (nMapID == (long)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".bh"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
    if (file.Read(&m_head, sizeof(m_head), 1)!=1)
    {
        Close();
        return nuFALSE;
    }
/*	file.Seek(28, NURO_SEEK_SET);*/
	if (file.Read(&blkNum, 4, 1)!=1)
	{
		Close();
		return nuFALSE;
	}
// 	if (file.GetLength()<(32+sizeof(SEARCH_BH_INFO)*blkNum))
// 	{
// 		Close();
// 		return nuFALSE;
// 	}
	return nuTRUE;
}

nuVOID CMapFileBhY::Close()
{
	file.Close();
	blkNum = 0;
    nuMemset(&m_head, 0, sizeof(m_head));
// 	if (pBHInfo)
// 	{
// 		nuFree(pBHInfo);
// 		pBHInfo = NULL;
// 	}
}
nuBOOL CMapFileBhY::GetHeadInfo(SEARCH_BH_HEAD &head)
{
    if (!file.IsOpen())
    {
        return nuFALSE;
    }
    nuMemcpy(&head, &m_head, sizeof(head));
    return nuTRUE;
}
// 
// nuBOOL CMapFileBhY::GetBHInfo(nuUINT nBlkID, SEARCH_BH_INFO *pbi)
// {
// 	//don't use
// 	return nuFALSE;
// 
// 	if (!pBHInfo || !pbi)
// 	{
// 		return nuFALSE;
// 	}
// 	//²éÕÒblkÎ»ÖÃ
// 	nuUINT pos = GetBlkIdx(nBlkID);
// 	if (pos==(nuUINT)-1)
// 	{
// 		return nuFALSE;
// 	}
// 	return nuFALSE;
// }
nuUINT CMapFileBhY::GetBHIdxInfo(nuVOID *pBuffer, nuUINT nMaxLen)
{
	nuUINT len = blkNum*4;
	if (nMaxLen<len)
	{
		return 0;
	}
	if (!file.ReadData(32, pBuffer, len))
	{
		return 0;
	}
	return len;
}
// nuUINT CMapFileBhY::GetBHRoadAddr(const nuUINT &nBlkID)
// {
// 	if (!pBHInfo)
// 	{
// 		return -1;
// 	}
// 	return GetBHRoadAddrByIdx(GetBlkIdx(nBlkID));
// }
nuUINT CMapFileBhY::GetBHPoiAddrByIdx(const nuUINT &nBlkIDx)
{
	if (nBlkIDx==(nuUINT)-1)
	{
		return -1;
	}
	nuUINT addr = -1;
	file.Seek(((blkNum+nBlkIDx)<<2)+32,NURO_SEEK_SET);
	if (file.Read(&addr, 4, 1)!=1)
	{
		return -1;
	}
	return addr;
}
nuUINT CMapFileBhY::GetBHRoadAddrByIdx(const nuUINT &nBlkIDx)
{
	if (nBlkIDx==(nuUINT)-1)
	{
		return -1;
	}
	nuUINT addr = -1;
	file.Seek(((3*blkNum+nBlkIDx)<<2)+32,NURO_SEEK_SET);
	if (file.Read(&addr, 4, 1)!=1)
	{
		return -1;
	}
	return addr;
}
// nuUINT CMapFileBhY::GetBlkIdx(const nuUINT &nBlkID)
// {
// 	if (!pBHInfo || !blkNum)
// 	{
// 		return -1;
// 	}
// 	nuUINT start = 0;
// 	nuUINT end = blkNum-1;
// 	nuUINT center = 0;
// 
// 	if (pBHInfo[start]==nBlkID)
// 	{
// 		return start;
// 	}
// 	else if (pBHInfo[end]==nBlkID)
// 	{
// 		return end;
// 	}
// 
// 	while (end-start>1)
// 	{
// 		center = (end+start)>>1;
// 		if (pBHInfo[center]==nBlkID)
// 		{
// 			return center;
// 		}
// 		else if (pBHInfo[center]>nBlkID)
// 		{
// 			end = center;
// 		}
// 		else
// 		{
// 			start = center;
// 		}
// 	}
// 	return -1;
// }

nuUINT CMapFileBhY::GetBlkNum()
{
	return blkNum;
}
