#include "IOpenResource.h"
#include "libOpenResource.h"
#include "NuroModuleApiStruct.h"

CIOpenResource* g_pIOpenResource = NULL;
OPENRES_API nuBOOL LibOPR_InitModule(nuPVOID lpOutRS)
{
	if( NULL == g_pIOpenResource )
	{
		g_pIOpenResource = new CIOpenResource();
	}
	else
	{
		return nuFALSE;
	}
	POPENRSAPI pRsApi  = (POPENRSAPI)lpOutRS;  
   	pRsApi->RS_LoadPack  		= LibRS_LoadPack;
	pRsApi->RS_UnloadPack       	= LibRS_UnloadPack;
	pRsApi->RS_FindFile         	= LibRS_FindFile;
	pRsApi->RS_GetFileLength	= LibRS_GetFileLength;
	pRsApi->RS_LoadFileData	        = LibRS_LoadFileData;
	pRsApi->RS_LoadBmp          	= LibRS_LoadBmp;
	pRsApi->RS_UnLoadBmp       	= LibRS_UnLoadBmp;
    	return nuTRUE;
}
OPENRES_API nuVOID  LibOPR_FreeModule()
{
    if( NULL != g_pIOpenResource )
    {
        delete g_pIOpenResource;
        g_pIOpenResource = NULL;
    }
}

OPENRES_API OPR_PACK* LibRS_LoadPack(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen)
{
	return (OPR_PACK*)g_pIOpenResource->IOprLoadPack(ptsPackage, nLoadType, nTotalLen);
}
OPENRES_API nuVOID LibRS_UnloadPack(OPR_PACK* nPackID)
{
	g_pIOpenResource->IOprUnloadPack(nPackID);
}
OPENRES_API OPR_PACK_FILE*  LibRS_FindFile(OPR_PACK* nPackID, const nuCHAR* ptsFile)
{
    return g_pIOpenResource->IOprFindFile(nPackID, ptsFile);
}
OPENRES_API nuUINT  LibRS_GetFileLength(OPR_PACK* nPackID, OPR_PACK_FILE* file)
{
    return g_pIOpenResource->IOprGetFileLength(nPackID, file);
}

OPENRES_API nuBOOL  LibRS_LoadFileData(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen)
{
    return g_pIOpenResource->IOprLoadFileData(nPackID, file, pDataBuffer, nBufLen);
}

OPENRES_API nuBOOL LibRS_LoadBmp(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap)
{
    nuBOOL Ret = g_pIOpenResource->IOprLoadNuroBmp(nPackID, file, bitmap);
    if(Ret)
    {
	bitmap.bmpHasLoad = 1;
    }
    return Ret;
}

OPENRES_API nuVOID LibRS_UnLoadBmp(NURO_BMP& bitmap)
{
    g_pIOpenResource->IOprUnloadNuroBmp(bitmap);
}
