#ifndef _NURO_API_OPEN_RESOURCE_20101210
#define _NURO_API_OPEN_RESOURCE_20101210

#include "NuroDefine.h"

typedef nuVOID          OPR_PACK;
typedef nuVOID          OPR_PACK_FILE;

#define _NURO_LOAD_PACKAGE_MEMORY   0
#define _NURO_LOAD_PACKAGE_FILE     1

class CGApiOpenResource
{
public:
    CGApiOpenResource()
    {
    }
    virtual ~CGApiOpenResource()
    {
    }
public:
    typedef struct tagOPR_INIT_PARAM
    {
    }OPR_INIT_PARAM, *POPR_INIT_PARAM;
    virtual nuUINT          IOprInit(const OPR_INIT_PARAM& opriParam) = 0;
    virtual nuVOID          IOprFree() = 0;
    enum emLoadPackType
    {
        LOAD_PACK_MEMORY = 0,
        LOAD_PACK_FILE
    };
    virtual OPR_PACK*       IOprLoadPack(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen) = 0;
    virtual nuVOID          IOprUnloadPack(OPR_PACK* nPackID) = 0;
    virtual OPR_PACK_FILE*  IOprFindFile(OPR_PACK* nPackID, const nuCHAR* ptsFile) = 0;
    virtual nuUINT          IOprGetFileLength(OPR_PACK* nPackID, OPR_PACK_FILE* file) = 0;
    virtual nuBOOL          IOprLoadFileData(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen) = 0;
    virtual nuBOOL          IOprLoadNuroBmp(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap) = 0;
    virtual nuVOID          IOprUnloadNuroBmp(NURO_BMP& bitmap) = 0;
};

#endif