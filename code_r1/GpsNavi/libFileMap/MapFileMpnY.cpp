
#include "MapFileMpnY.h"

CMapFileMpnY::CMapFileMpnY()
{
	maptotal = 0;
}

CMapFileMpnY::~CMapFileMpnY()
{
	Close();
}
nuBOOL CMapFileMpnY::Open()
{
	if (IsOpen())
	{
		return nuTRUE;
	}
	nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
	nuTcscpy(tcsPath,nuTEXT(".mpn"));
	if (!m_pFsApi->FS_FindFileWholePath(-1,tcsPath,NURO_MAX_PATH))
	{
		return nuFALSE;
	}
	if (!file.Open(tcsPath, NURO_FS_RB))
	{
		return nuFALSE;
	}
	if (file.Read(&maptotal, 4, 1)!=1)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CMapFileMpnY::IsOpen()
{
	return file.IsOpen();
}
nuVOID CMapFileMpnY::Close()
{
	file.Close();
}
nuUINT CMapFileMpnY::GetMapNum() const
{
	return maptotal;
}
nuUINT CMapFileMpnY::GetMapName(nuUINT nStartID, nuUINT nNum, nuVOID* pBuffer, nuUINT nBufferLen)
{
	if (!pBuffer || nStartID>=maptotal)
	{
		return 0;
	}
	nNum = NURO_MIN(nNum, nBufferLen/EACH_MAP_NAME_LEN);
	if ((nStartID+nNum) > maptotal)
	{
		nNum = maptotal - nStartID;
	}
	if (!nNum)
	{
		return 0;
	}
	SEARCH_MPN_INFO info = {0};
	nuBYTE *pNameBuffer = (nuBYTE*)pBuffer;
	file.Seek(sizeof(info)*nStartID, NURO_SEEK_SET);
	for (nuUINT i=0; i<nNum; i++)
	{
		if (file.Read(&info, sizeof(info), 1)!=1)
		{
			break;
		}
		nuMemcpy(pNameBuffer, info.realName, sizeof(info.realName));
		pNameBuffer += sizeof(info.realName);
	}
	return pNameBuffer - (nuBYTE*)pBuffer;
}
