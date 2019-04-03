// IOpenResource.cpp: implementation of the CIOpenResource class.
//
//////////////////////////////////////////////////////////////////////

#include "IOpenResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOpenResource::CIOpenResource()
{

}

CIOpenResource::~CIOpenResource()
{

}

nuUINT CIOpenResource::IOprInit(const OPR_INIT_PARAM& opriParam)
{
    return 1;
}

nuVOID CIOpenResource::IOprFree()
{

}

OPR_PACK* CIOpenResource::IOprLoadPack(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen)
{
    return (OPR_PACK*)m_resZip.LoadPack(ptsPackage, nLoadType, nTotalLen);
}

nuVOID CIOpenResource::IOprUnloadPack(OPR_PACK* nPackID)
{
    m_resZip.UnloadPack((nuroArchive*)nPackID);
}

OPR_PACK_FILE* CIOpenResource::IOprFindFile(OPR_PACK* nPackID, const nuCHAR* ptsFile)
{
    return m_resZip.FindFile((nuroArchive*)nPackID, ptsFile);
}

nuUINT CIOpenResource::IOprGetFileLength(OPR_PACK* nPackID, OPR_PACK_FILE* file)
{
    return m_resZip.GetFileLength((nuroArchive*)nPackID, file);
}

nuBOOL CIOpenResource::IOprLoadFileData(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen)
{
    return m_resZip.LoadFileData((nuroArchive*)nPackID, file, pDataBuffer, nBufLen);
}

nuBOOL CIOpenResource::IOprLoadNuroBmp(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap)
{
    return m_resZip.LoadNuroBMP((nuroArchive*)nPackID, &bitmap, file);
}

nuVOID CIOpenResource::IOprUnloadNuroBmp(NURO_BMP& bitmap)
{
    m_resZip.UnloadNuroBMP(&bitmap);
}