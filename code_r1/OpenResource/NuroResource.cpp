// nuroResource.cpp: implementation of the nuroResource class.
//
//////////////////////////////////////////////////////////////////////

#include "NuroResource.h"
#include "NuroArchive7z.h"
#include "NuroArchiveAlp.h"
nuroResource::nuroResource()
{
    m_CommonPack = 0;
}
nuroResource::~nuroResource()
{
   if (m_CommonPack)
   {
       delete m_CommonPack;
       m_CommonPack = 0;
   }
}
nuBOOL nuroResource::Initialize()
{
    nuroArchive7z::Prepare();
    return nuTRUE;
}
nuUINT	nuroResource::LoadPack(const nuTCHAR* packname, nuINT type)
{
    UnloadPack();
    nuUINT total = 0;
    m_CommonPack = LoadPack(packname, type, total);
    if (!m_CommonPack)
    {
        return 0;
    }
    return total;
}
nuVOID	nuroResource::UnloadPack()
{
    if (m_CommonPack)
    {
        UnloadPack(m_CommonPack);
        m_CommonPack = 0;
    }
}
nuVOID*	nuroResource::FindFile(const nuCHAR* filepath)
{
    return FindFile(m_CommonPack, filepath);
}
nuUINT	nuroResource::GetFileLength(nuVOID* file)
{
    return GetFileLength(m_CommonPack, file);
}
nuBOOL	nuroResource::LoadFileData(nuVOID* file, nuUCHAR* buffer, nuUINT size)
{
    return LoadFileData(m_CommonPack, file, buffer, size);
}
nuBOOL	nuroResource::LoadNuroBMP(NURO_BMP* pNuroBmp, nuVOID* file)
{
    return LoadNuroBMP(m_CommonPack, pNuroBmp, file);
}
nuroArchive* nuroResource::LoadPack(const nuTCHAR* packname, nuINT type, nuUINT &total)
{
    nuroArchive* res = 0;
    nuFILE* fp = nuTfopen(packname, NURO_FS_RB);
    if (!fp)
    {
        return 0;
    }
    nuCHAR head[4] = {0};
    if (1 != nuFread(head, 4, 1, fp) )
    {
        nuFclose(fp);
        return 0;
    }
    nuFseek(fp, 0, NURO_SEEK_SET);
    if (!nuStrncmp(head, "ALP", 3))
    {
        res = new nuroArchiveAlp();
        total = res->LoadPack(fp, type);
        if (!total)
        {
            delete res;
			res=NULL;
	        nuFclose(fp);
            return 0;
        }
        return res;
    }
    else if (!nuStrncmp(head, "7z", 2))
    {
        res = new nuroArchive7z();
        total = res->LoadPack(fp, type);
        if (!total)
        {
            delete res;
			res=NULL;
	        nuFclose(fp);
            return 0;
        }
        nuFclose(fp);
        return res;
    }
	nuFclose(fp);
    return 0;
}
nuVOID	nuroResource::UnloadPack(nuroArchive* resource)
{
    delete resource;
	resource=NULL;
}
nuVOID*	nuroResource::FindFile(nuroArchive* resource, const nuCHAR* filepath)
{
    return resource->FindFile(filepath);
}
nuUINT	nuroResource::GetFileLength(nuroArchive* resource, nuVOID* file)
{
    return resource->GetFileLength(file);
}
nuBOOL	nuroResource::LoadFileData(nuroArchive* resource, nuVOID* file, nuUCHAR* buffer, nuUINT size)
{
    return resource->LoadFileData(file, buffer, size);
}
nuBOOL	nuroResource::LoadNuroBMP(nuroArchive* resource, NURO_BMP* pNuroBmp, nuVOID* file)
{
 //   nuDWORD t1 = nuGetTickCount();
    //	if( !pNuroBmp || pNuroBmp->bmpHasLoad )
    if( !pNuroBmp )
    {
        return nuFALSE;
    }
    nuSIZE filesize = GetFileLength(resource, file);
    if( !filesize )
    {
        return nuFALSE;
    }
    nuUCHAR* buffer = (nuUCHAR*)nuMalloc(filesize);
    if (!buffer)
    {
        return nuFALSE;
    }
    CMemFreeGuard mfg(buffer);
    if( !LoadFileData(resource, file, buffer, filesize) )
    {
        return nuFALSE;
    }    
//     nuDWORD t2 = nuGetTickCount();
    //pNuroBmp->bmpBitCount = 16; 
    
    NURO_BITMAPFILEHEADER* bmpFileHead = (NURO_BITMAPFILEHEADER*)buffer;    
    NURO_BITMAPINFOHEADER* bmpInfoHead = (NURO_BITMAPINFOHEADER*)(buffer + sizeof(NURO_BITMAPFILEHEADER));
    
    nuINT arrTag = (bmpInfoHead->biHeight > 0) ? 0:1;
    bmpInfoHead->biHeight  = NURO_ABS(bmpInfoHead->biHeight); 
    nuUINT DataSizePerLine = (bmpInfoHead->biWidth * 16 + 31) / 32 * 4; 
    pNuroBmp->bmpBuffLen   = DataSizePerLine * ((nuWORD)bmpInfoHead->biHeight);
    //
    nuBYTE* pBuff          = (nuBYTE*)nuMalloc(pNuroBmp->bmpBuffLen);	
    if( !pBuff )
    {
        return nuFALSE;
    }
    
    //	pNuroBmp->bmpBitCount = 16;
    
    nuMemcpy(pBuff, buffer + bmpFileHead->bfOffBits, pNuroBmp->bmpBuffLen);
    
    if( !arrTag )
    {
        nuINT i=0;
        nuINT iEnd = bmpInfoHead->biHeight >> 1;
        nuBYTE *pBuffTag = pBuff;
        nuBYTE *pBuffTag2 = pBuffTag + (bmpInfoHead->biHeight-1)*DataSizePerLine;      
        nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
        if( pLine == NULL )
        {
            nuFree(pBuff);
            return nuFALSE;
        }
        for( i = 0; i < iEnd; ++i )
        {
            nuMemcpy(pLine, pBuffTag, DataSizePerLine);
            nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLine);
            nuMemcpy(pBuffTag2, pLine, DataSizePerLine);
            pBuffTag += DataSizePerLine;
            pBuffTag2 -= DataSizePerLine;
        }
        nuFree(pLine);
    }	
#ifndef _NURO_BITMAP_LINE4_
    nuUINT DataSizePerLineNew = ( ( (nuWORD)(bmpInfoHead->biWidth) ) * 16 + 7 ) / 8;
    if( DataSizePerLineNew != DataSizePerLine )
    {
        nuINT i=0;
        nuINT iEnd = bmpInfoHead->biHeight-1;
        nuBYTE *pBuffTag = pBuff + DataSizePerLineNew;
        nuBYTE *pBuffTag2 = pBuff + DataSizePerLine;
        for( i = 0; i < iEnd; ++i )
        {
            nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLineNew);
            pBuffTag  += DataSizePerLineNew;
            pBuffTag2 += DataSizePerLine;
        }
        pNuroBmp->bmpBuffLen  = DataSizePerLineNew *((nuWORD)bmpInfoHead->biHeight);
    }
    DataSizePerLine = DataSizePerLineNew;
#endif
    pNuroBmp->pBmpBuff	        = pBuff;
    pNuroBmp->bmpWidth	        = (nuWORD)bmpInfoHead->biWidth;
    pNuroBmp->bmpHeight	        = (nuWORD)bmpInfoHead->biHeight;
    pNuroBmp->bmpBufferType     = NURO_BMP_BUFFERTYPE_MALLOC;
    pNuroBmp->bmpTransfColor    = NURO_BMP_TRANSCOLOR_565;
    pNuroBmp->bytesPerLine      = DataSizePerLine;
/*    nuDWORD t3 = nuGetTickCount();*/
    return nuTRUE;
}
nuVOID    nuroResource::UnloadNuroBMP(NURO_BMP* pNuroBmp)
{
    if( pNuroBmp != NULL && pNuroBmp->pBmpBuff != NULL )
    {
        nuFree(pNuroBmp->pBmpBuff);
        pNuroBmp->pBmpBuff = NULL;
    }
}
