// MapFilePdnY.cpp: implementation of the CMapFilePdnY class.
//
//////////////////////////////////////////////////////////////////////
#include "MapFilePdnY.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapFilePdnY::CMapFilePdnY()
{
    pdnNum = 0;
    cityNum = 0;
}

CMapFilePdnY::~CMapFilePdnY()
{
    Close();
    pdnNum = 0;
    cityNum = 0;
}

nuBOOL CMapFilePdnY::Open(nuLONG nMapID)
{
    Close();
    if (nMapID == (nuLONG)-1)
    {
        return nuFALSE;
    }
    nuTCHAR tsFile[NURO_MAX_PATH] = {0};
    nuTcscpy(tsFile, nuTEXT(".pdn"));
    if( !m_pFsApi->FS_FindFileWholePath(nMapID, tsFile, NURO_MAX_PATH) )
    {
        return nuFALSE;
    }
    if (!file.Open(tsFile, NURO_FS_RB))
    {
        return nuFALSE;
    }
    if (file.Read(&pdnNum, 4,1)!=1)
    {
        Close();
        return nuFALSE;
    }
    if (file.Read(&cityNum, 4, 1)!=1)
    {
        Close();
        return nuFALSE;
    }
    return nuTRUE;
}
nuBOOL CMapFilePdnY::IsOpen()
{
    return file.IsOpen();
}
nuVOID CMapFilePdnY::Close()
{
    file.Close();
}
nuUINT CMapFilePdnY::GetCityDoorInfoStartAddr(nuWORD nCityID)
{
    if (nCityID>=cityNum)
    {
        return 0;
    }
    nuUINT pos = 0;
    file.Seek(8+(nCityID<<2), NURO_SEEK_SET);
    if (file.Read(&pos, 4, 1)!=1)
    {
        return 0;
    }
    return pos;
}
nuUINT CMapFilePdnY::GetDoorInfo(nuWORD nCityID, nuVOID* pBuffer, nuUINT nMaxLen)
{
    if (nCityID>=cityNum)
    {
        return 0;
    }
    nuUINT pos = 0;
    file.Seek(8+(nCityID<<2), NURO_SEEK_SET);
    if (file.Read(&pos, 4, 1)!=1)
    {
        return 0;
    }
    nuUINT len = 0;
    if (nCityID == (cityNum-1))
    {
        len = file.GetLength();
    }
    else
    {
        file.Seek(8+((nCityID+1)<<2), NURO_SEEK_SET);
        if (file.Read(&len, 4, 1)!=1)
        {
            return 0;
        }
    }
    if (len<pos)
    {
        return 0;
    }
    len -= pos;
    if (!len || len>nMaxLen)
    {
        return 0;
    }
    file.Seek(pos, NURO_SEEK_SET);
    if (file.Read(pBuffer, len, 1)!=1)
    {
        return 0;
    }
    return len;
}

nuUINT CMapFilePdnY::GetCityNum()
{
    return cityNum;
}

nuBOOL CMapFilePdnY::GetData(nuUINT startaddr, nuVOID *pBuffer, nuUINT len)
{
    return file.ReadData(startaddr, pBuffer, len);
}

nuSIZE CMapFilePdnY::ReadData(nuUINT startaddr, nuVOID* pBuff, nuSIZE size, nuSIZE count)
{
    file.Seek(startaddr, NURO_SEEK_SET);
    return file.Read(pBuff, size, count);
}