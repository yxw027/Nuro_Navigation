
#include "../BmpMethodsZK.h"

//#include "nuroClib.h"
//#include "libInterface.h"

// #ifdef _NURO_OS_WIN
// #pragma pack(push) 
// #pragma pack(1)
// #endif
// 
// __ATTRIBUTE_PRE struct nutagBITMAPFILEHEADER {
//     nuWORD		bfType;
//     nuDWORD	bfSize;
//     nuWORD		bfReserved1;
//     nuWORD		bfReserved2;
//     nuDWORD	bfOffBits;
// }__ATTRIBUTE_PACKED__;
// typedef struct nutagBITMAPFILEHEADER NURO_BITMAPFILEHEADER;
// 
// __ATTRIBUTE_PRE struct nutagBITMAPINFOHEADER{
//     nuDWORD	biSize;
//     nuINT		biWidth;
//     nuINT		biHeight;
//     nuWORD		biPlanes;
//     nuWORD		biBitCount;
//     nuDWORD	biCompression;
//     nuDWORD	biSizeImage;
//     nuINT		biXPelsPerMeter;
//     nuINT		biYPelsPerMeter;
//     nuDWORD	biClrUsed;
//     nuDWORD	biClrImportant;
// }__ATTRIBUTE_PACKED__;
// typedef struct nutagBITMAPINFOHEADER NURO_BITMAPINFOHEADER;
// 
// __ATTRIBUTE_PRE struct nutagRGBQUAD {
//     nuBYTE    rgbBlue;
//     nuBYTE    rgbGreen;
//     nuBYTE    rgbRed;
//     nuBYTE    rgbReserved;
// }__ATTRIBUTE_PACKED__;
// typedef struct nutagRGBQUAD NURO_RGBQUAD;
// 
// struct nutagBITMAPINFO {
//     NURO_BITMAPINFOHEADER	bmiHeader;
//     NURO_RGBQUAD			bmiColors[1];
// }__ATTRIBUTE_PACKED__;
// typedef struct nutagBITMAPINFO NURO_BITMAPINFO;
// 
// 
// #ifdef _NURO_OS_WIN
// #pragma pack(pop) 
// #endif


void CBmpMethodsZK::BmpInit(PNURO_BMP pBmp)
{
    if (pBmp)
    {
        nuMemset(pBmp, 0, sizeof(NURO_BMP));
        pBmp->bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
        pBmp->bmpBufferType  = NURO_BMP_BUFFERTYPE_OUT;
    }
}

nuVOID CBmpMethodsZK::BmpDestroy(PNURO_BMP pNuroBmp)
{
    if (pNuroBmp && pNuroBmp->pBmpBuff && pNuroBmp->bmpBuffLen)
    {
        if (pNuroBmp->bmpBufferType == NURO_BMP_BUFFERTYPE_NEW)
        {
            delete pNuroBmp->pBmpBuff;
        }
        else if (pNuroBmp->bmpBufferType == NURO_BMP_BUFFERTYPE_MALLOC)
        {
            nuFree(pNuroBmp->pBmpBuff);
        }
    }
    BmpInit(pNuroBmp);
}

CBmpLoader::CBmpLoader(NURO_BMP* bitmap):m_bitmap(bitmap)
{
    
}
CBmpLoader::~CBmpLoader()
{
    
}

nuBOOL CBmpLoader::Load(const nuCHAR* ptsBmpName)
{
    if (!m_bitmap)
    {
        return nuFALSE;
    }
    nuFILE* fp = nuFopen(ptsBmpName, NURO_FS_RB);
    if(fp == NULL)
    {
        return nuFALSE;
    }
    NURO_BITMAPFILEHEADER bmpFileHead;
    nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));
    if(nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    
    NURO_BITMAPINFOHEADER bmpInfoHead;
    nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
    if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    nuUINT DataSizePerLine= (bmpInfoHead.biWidth * 16+31)/32*4;
    nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
    if(pLine == NULL)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    nuMemset(pLine, 0, DataSizePerLine);
    int arrTag = (bmpInfoHead.biHeight>0)?0:1;
    bmpInfoHead.biHeight = NURO_ABS(bmpInfoHead.biHeight);
    
    nuUINT DataSizePerLineNew = DataSizePerLine;//(((nuWORD)(bmpInfoHead.biWidth))* 16+7)/8;
    
    m_bitmap->bytesPerLine = DataSizePerLineNew;
    m_bitmap->bmpBuffLen   = m_bitmap->bytesPerLine*((nuWORD)bmpInfoHead.biHeight);
    nuBYTE* pBuff = (nuBYTE*)nuMalloc(m_bitmap->bmpBuffLen);
    if(pBuff == NULL)
    {
        nuFree(pLine);
        nuFclose(fp);
        return false;
    }
    nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);    
    if(nuFread(pBuff, m_bitmap->bmpBuffLen, 1, fp) != 1)
    {
        nuFree(pBuff);
        nuFclose(fp);
        return false;
    }
    if(!arrTag)
    {
        nuINT i=0;
        nuINT iEnd = bmpInfoHead.biHeight >> 1;
        nuBYTE *pBuffTag = pBuff;
        nuBYTE *pBuffTag2 = pBuffTag + (bmpInfoHead.biHeight-1)*DataSizePerLineNew;
        for(i=0; i<iEnd; ++i)
        {
            nuMemcpy(pLine, pBuffTag, DataSizePerLineNew);
            nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLineNew);
            nuMemcpy(pBuffTag2, pLine, DataSizePerLineNew);
            pBuffTag += DataSizePerLineNew;
            pBuffTag2 -= DataSizePerLineNew;
        }
    }
    nuFree(pLine);
    nuFclose(fp);
    m_bitmap->pBmpBuff	= pBuff;
    m_bitmap->bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
    m_bitmap->bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
    m_bitmap->bmpBufferType = NURO_BMP_BUFFERTYPE_MALLOC;
    return true;
}

nuBOOL CBmpLoader::Load(const nuWCHAR* ptsBmpName)
{
    if (!m_bitmap)
    {
        return nuFALSE;
    }
    nuFILE* fp = nuWfopen(ptsBmpName, NURO_FS_RB);
    if(fp == NULL)
    {
        return nuFALSE;
    }
    NURO_BITMAPFILEHEADER bmpFileHead;
    nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));
    if(nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    
    NURO_BITMAPINFOHEADER bmpInfoHead;
    nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
    if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    nuUINT DataSizePerLine= (bmpInfoHead.biWidth * 16+31)/32*4;
    nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
    if(pLine == NULL)
    {
        nuFclose(fp);
        return nuFALSE;
    }
    nuMemset(pLine, 0, DataSizePerLine);
    int arrTag = (bmpInfoHead.biHeight>0)?0:1;
    bmpInfoHead.biHeight = NURO_ABS(bmpInfoHead.biHeight);
    
    nuUINT DataSizePerLineNew = DataSizePerLine;//(((nuWORD)(bmpInfoHead.biWidth))* 16+7)/8;
    
    m_bitmap->bytesPerLine = DataSizePerLineNew;
    m_bitmap->bmpBuffLen   = m_bitmap->bytesPerLine*((nuWORD)bmpInfoHead.biHeight);
    nuBYTE* pBuff = (nuBYTE*)nuMalloc(m_bitmap->bmpBuffLen);
    if(pBuff == NULL)
    {
        nuFree(pLine);
        nuFclose(fp);
        return false;
    }
    nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);    
    if(nuFread(pBuff, m_bitmap->bmpBuffLen, 1, fp) != 1)
    {
        nuFree(pBuff);
        nuFclose(fp);
        return false;
    }
    if(!arrTag)
    {
        nuINT i=0;
        nuINT iEnd = bmpInfoHead.biHeight >> 1;
        nuBYTE *pBuffTag = pBuff;
        nuBYTE *pBuffTag2 = pBuffTag + (bmpInfoHead.biHeight-1)*DataSizePerLineNew;
        for(i=0; i<iEnd; ++i)
        {
            nuMemcpy(pLine, pBuffTag, DataSizePerLineNew);
            nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLineNew);
            nuMemcpy(pBuffTag2, pLine, DataSizePerLineNew);
            pBuffTag += DataSizePerLineNew;
            pBuffTag2 -= DataSizePerLineNew;
        }
    }
    nuFree(pLine);
    nuFclose(fp);
    m_bitmap->pBmpBuff	= pBuff;
    m_bitmap->bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
    m_bitmap->bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
    m_bitmap->bmpBufferType = NURO_BMP_BUFFERTYPE_MALLOC;
    return true;
}

void CBmpLoader::Save(const nuCHAR* ptsBmpName, NURO_BMP* pBmp)
{
    NURO_BITMAPFILEHEADER bf = { 0 }; 
    NURO_BITMAPINFOHEADER bi = { 0 }; 
    bi.biSize = sizeof( NURO_BITMAPINFOHEADER ); 
    bi.biBitCount = 16;
    bi.biWidth = pBmp->bmpWidth; 
    bi.biHeight = pBmp->bmpHeight; 
    bi.biPlanes = 1; 
    bi.biCompression = NURO_BI_BITFIELDS; 
    bi.biSizeImage = (((( bi.biWidth)*(bi.biBitCount)+31)/32)*4) * bi.biHeight;
    bi.biHeight = -bi.biHeight;
    bf.bfType = 0x4d42; 
    bf.bfSize = sizeof( NURO_BITMAPFILEHEADER ) + sizeof( NURO_BITMAPINFOHEADER ) + bi.biSizeImage+12; 
    bf.bfOffBits = sizeof( NURO_BITMAPFILEHEADER ) + sizeof( NURO_BITMAPINFOHEADER )+12; 
    nuFILE* fp = nuFopen(ptsBmpName, NURO_FS_WB);
    if (fp)
    {
        nuFwrite(&bf, sizeof( NURO_BITMAPFILEHEADER ), 1, fp);
        nuFwrite(&bi, sizeof( NURO_BITMAPINFOHEADER ), 1, fp);
        nuDWORD mask[3] = {0x1f<<11, 0x3f<<5, 0x1f};
        nuFwrite(mask, 12, 1, fp); 
        nuFwrite(pBmp->pBmpBuff, bi.biSizeImage, 1, fp);
        nuFclose(fp);
    }
}

void CBmpLoader::Save(const nuWCHAR* ptsBmpName, NURO_BMP* pBmp)
{
    NURO_BITMAPFILEHEADER bf = { 0 }; 
    NURO_BITMAPINFOHEADER bi = { 0 }; 
    bi.biSize = sizeof( NURO_BITMAPINFOHEADER ); 
    bi.biBitCount = 16;
    bi.biWidth = pBmp->bmpWidth; 
    bi.biHeight = pBmp->bmpHeight; 
    bi.biPlanes = 1; 
    bi.biCompression = NURO_BI_BITFIELDS; 
    bi.biSizeImage = (((( bi.biWidth)*(bi.biBitCount)+31)/32)*4) * bi.biHeight;
    bi.biHeight = -bi.biHeight;
    bf.bfType = 0x4d42; 
    bf.bfSize = sizeof( NURO_BITMAPFILEHEADER ) + sizeof( NURO_BITMAPINFOHEADER ) + bi.biSizeImage+12; 
    bf.bfOffBits = sizeof( NURO_BITMAPFILEHEADER ) + sizeof( NURO_BITMAPINFOHEADER )+12; 
    nuFILE* fp = nuWfopen(ptsBmpName, NURO_FS_WB);
    if (fp)
    {
        nuFwrite(&bf, sizeof( NURO_BITMAPFILEHEADER ), 1, fp);
        nuFwrite(&bi, sizeof( NURO_BITMAPINFOHEADER ), 1, fp);
        nuDWORD mask[3] = {0x1f<<11, 0x3f<<5, 0x1f};
        nuFwrite(mask, 12, 1, fp); 
        nuFwrite(pBmp->pBmpBuff, bi.biSizeImage, 1, fp);
        nuFclose(fp);
    }
}

