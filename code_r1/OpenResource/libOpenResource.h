
#ifndef __NURO_OPEN_RESOURCE_20151025
#define __NURO_OPEN_RESOURCE_20151025

#include "IOpenResource.h"
#include "libOpenResource.h"
#ifdef OPENRESOURCE_EXPORTS
#define OPENRES_API extern "C" __declspec(dllexport)
#else
#define OPENRES_API extern "C"
#endif

OPENRES_API nuBOOL LibOPR_InitModule(nuPVOID lpOutRS);
OPENRES_API nuVOID  LibOPR_FreeModule();

OPENRES_API OPR_PACK* LibRS_LoadPack(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen);
OPENRES_API nuVOID LibRS_UnloadPack(OPR_PACK* nPackID);
OPENRES_API OPR_PACK_FILE*  LibRS_FindFile(OPR_PACK* nPackID, const nuCHAR* ptsFile);
OPENRES_API nuUINT  LibRS_GetFileLength(OPR_PACK* nPackID, OPR_PACK_FILE* file);
OPENRES_API nuBOOL  LibRS_LoadFileData(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen);
OPENRES_API nuBOOL LibRS_LoadBmp(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap);
OPENRES_API nuVOID LibRS_UnLoadBmp(NURO_BMP& bitmap);
#endif

