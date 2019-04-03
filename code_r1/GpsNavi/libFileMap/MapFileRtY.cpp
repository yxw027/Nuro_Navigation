
#include "MapFileRtY.h"

CMapFileRtY::CMapFileRtY()
{
	rtcount = 0;
}

CMapFileRtY::~CMapFileRtY()
{
	Close();
}
nuBOOL CMapFileRtY::Open(nuLONG nMapID)
{
	Close();
	if (nMapID == (nuLONG)-1)
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".rt"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	if (!file.Open(tsFile, NURO_FS_RB))
	{
		return nuFALSE;
	}
	file.Seek(2, NURO_SEEK_SET);
	if (file.Read(&rtcount, sizeof(rtcount), 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CMapFileRtY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileRtY::Close()
{
	file.Close();
	rtcount = 0;
}
nuWORD CMapFileRtY::GetRtNum()
{
	return rtcount;
}
nuUINT CMapFileRtY::GetRtIdxInfo(nuUINT rtIdx, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (rtIdx>=rtcount)
	{
		return 0;
	}
	if (nMaxLen<sizeof(SEARCH_RTINFO_M))
	{
		return 0;
	}
	SEARCH_RTINFO_M RTIM = {0};
	//SEARCH_RTINFO_M *prtim_ = (SEARCH_RTINFO_M*)pBuffer;
	SEARCH_RTINFO rti = {0};
	nuUINT addr = -1;
	GetRtBasePos(rtIdx, &RTIM);
	file.ReadData(4+(rtcount<<2)+(rtIdx<<2), &addr, 4);
	file.ReadData(addr, &rti, sizeof(rti));
	RTIM.a1count = rti.TotalNodeCount;
	RTIM.a1addr = addr+sizeof(rti);
	RTIM.a2count = rti.RtConnectStruCount;
	RTIM.a2addr = rti.RtConnectStruStartAddr;
	RTIM.a3count = rti.TotalRoadCount;
	RTIM.a3addrM = sizeof(SEARCH_RTINFO_M);
	nuUINT len = RTIM.a3count*sizeof(SEARCH_RTINFO_A3);
	if ((len+sizeof(SEARCH_RTINFO_M))>nMaxLen)
	{
		return 0;
	}
	if (!file.ReadData(rti.RtIndexStruStartAddr, (nuBYTE*)pBuffer+sizeof(SEARCH_RTINFO_M), len))
	{
		return 0;
	}
	nuMemcpy(pBuffer, &RTIM, sizeof(SEARCH_RTINFO_M));
	return len+sizeof(SEARCH_RTINFO_M);
}
nuBOOL CMapFileRtY::GetRtA1Info(nuUINT addr, nuUINT a1idx, SEARCH_RTINFO_A1 *pa1)
{
	if (!pa1)
	{
		return nuFALSE;
	}
	return file.ReadData(addr+a1idx*sizeof(SEARCH_RTINFO_A1), pa1, sizeof(SEARCH_RTINFO_A1));
}
nuBOOL CMapFileRtY::GetRtA2Info(nuUINT addr, nuUINT a2idx, SEARCH_RTINFO_A2 *pa2)
{
	if (!pa2)
	{
		return nuFALSE;
	}
	return file.ReadData(addr+a2idx*sizeof(SEARCH_RTINFO_A2), pa2, sizeof(SEARCH_RTINFO_A2));
}
nuUINT CMapFileRtY::GetRtAddr(nuWORD idx)
{
	return 0;
}
nuBOOL CMapFileRtY::GetRtBasePos(nuWORD idx, SEARCH_RTINFO_M *prtim)
{
	if (!prtim || idx>=rtcount)
	{
		return nuFALSE;
	}
	return file.ReadData(4+12*rtcount+12*idx, prtim, 8);
}
