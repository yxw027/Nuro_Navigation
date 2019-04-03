// MapFileSKB.cpp: implementation of the CMapFileSKB class.
//
//////////////////////////////////////////////////////////////////////
#include "MapFileSKB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFileSKB::CMapFileSKB()
{

}

CMapFileSKB::~CMapFileSKB()
{
    Close();
}
nuBOOL CMapFileSKB::Open(nuLONG nMapID)
{
    Close();
    if (nMapID == (nuLONG)-1)
    {
        return nuFALSE;
    }
    nuTCHAR tcsPath[NURO_MAX_PATH] = {0};
    nuMemset(tcsPath, 0, sizeof(tcsPath));
    nuTcscpy(tcsPath, nuTEXT(".SKB"));
    if (!m_pFsApi->FS_FindFileWholePath(nMapID, tcsPath, NURO_MAX_PATH))
    {
        return nuFALSE;
    }
    if (!file.Open(tcsPath, NURO_FS_RB))
    {
        return nuFALSE;
    }
    if (file.Read(&cityCount, sizeof(cityCount), 1)!=1)
    {
        Close();
        return nuFALSE;
    }
	return nuTRUE;
}
nuBOOL CMapFileSKB::GetCityInfo(nuWORD cityID, SEARCH_SKB_CITYINFO& info)
{
    if (!IsOpen() || cityID>=cityCount)
    {
        return nuFALSE;
    }

    return file.ReadData(4+sizeof(SEARCH_SKB_CITYINFO)*cityID, 
        &info, sizeof(SEARCH_SKB_CITYINFO));
}
nuBOOL CMapFileSKB::ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen)
{
    return file.ReadData(nDataStartAddr, pBuffer, nLen);
}
nuBOOL CMapFileSKB::IsOpen()
{
    return file.IsOpen();
}
nuVOID CMapFileSKB::Close()
{
    file.Close();
}