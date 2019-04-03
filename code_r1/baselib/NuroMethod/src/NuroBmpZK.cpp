// NuroBmpZK.cpp: implementation of the CNuroBmpZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../NuroBmpZK.h"
#include "../../NuroDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNuroBmpZK::CNuroBmpZK()
{
    nuMemset(&m_bmp, 0, sizeof(NURO_BMP));
    m_bmp.bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
    m_bmp.bmpBufferType  = NURO_BMP_BUFFERTYPE_OUT;
}

CNuroBmpZK::~CNuroBmpZK()
{
    FreeNuroBmp();
}

nuBOOL CNuroBmpZK::LoadNuroBmp(const nuCHAR* ptsBmpName)
{
    FreeNuroBmp();
    nuFILE* fp = nuFopen(ptsBmpName, NURO_FS_RB);
    if(fp == NULL)
    {
        return false;
    }
    NURO_BITMAPFILEHEADER bmpFileHead;
    nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));
    if(nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return false;
    }
    NURO_BITMAPINFOHEADER bmpInfoHead;
    nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
    if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return false;
    }
    nuUINT DataSizePerLine= (bmpInfoHead.biWidth * 16+31)/32*4;
    nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
    if(pLine == NULL)
    {
        nuFclose(fp);
        return false;
    }
    nuMemset(pLine, 0, DataSizePerLine);
    int arrTag = (bmpInfoHead.biHeight>0)?0:1;
    bmpInfoHead.biHeight = NURO_ABS(bmpInfoHead.biHeight);
    
    nuUINT DataSizePerLineNew = DataSizePerLine;//(((nuWORD)(bmpInfoHead.biWidth))* 16+7)/8;
    
    m_bmp.bytesPerLine = DataSizePerLineNew;
    m_bmp.bmpBuffLen   = m_bmp.bytesPerLine*((nuWORD)bmpInfoHead.biHeight);
    nuBYTE* pBuff = (nuBYTE*)nuMalloc(m_bmp.bmpBuffLen);
    if(pBuff == NULL)
    {
        nuFree(pLine);
        nuFclose(fp);
        return false;
    }
    nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);
    if(nuFread(pBuff, m_bmp.bmpBuffLen, 1, fp) != 1)
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
    m_bmp.pBmpBuff	= pBuff;
    m_bmp.bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
    m_bmp.bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
    m_bmp.bmpBufferType = NURO_BMP_BUFFERTYPE_MALLOC;
    m_bmp.bmpTransfColor    = NURO_BMP_TRANSCOLOR_565;
    return true;
}

nuBOOL CNuroBmpZK::LoadNuroBmp(const nuWCHAR* ptsBmpName)
{
    FreeNuroBmp();
    nuFILE* fp = nuWfopen(ptsBmpName, NURO_FS_RB);
    if(fp == NULL)
    {
        return false;
    }
    NURO_BITMAPFILEHEADER bmpFileHead;
    nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));
    if(nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return false;
    }
    NURO_BITMAPINFOHEADER bmpInfoHead;
    nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
    if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1)
    {
        nuFclose(fp);
        return false;
    }
    nuUINT DataSizePerLine= (bmpInfoHead.biWidth * 16+31)/32*4;
    nuBYTE* pLine = (nuBYTE*)nuMalloc(DataSizePerLine);
    if(pLine == NULL)
    {
        nuFclose(fp);
        return false;
    }
    nuMemset(pLine, 0, DataSizePerLine);
    int arrTag = (bmpInfoHead.biHeight>0)?0:1;
    bmpInfoHead.biHeight = NURO_ABS(bmpInfoHead.biHeight);
    
    nuUINT DataSizePerLineNew = DataSizePerLine;//(((nuWORD)(bmpInfoHead.biWidth))* 16+7)/8;
    
    m_bmp.bytesPerLine = DataSizePerLineNew;
    m_bmp.bmpBuffLen   = m_bmp.bytesPerLine*((nuWORD)bmpInfoHead.biHeight);
    nuBYTE* pBuff = (nuBYTE*)nuMalloc(m_bmp.bmpBuffLen);
    if(pBuff == NULL)
    {
        nuFree(pLine);
        nuFclose(fp);
        return false;
    }
    nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);
    if(nuFread(pBuff, m_bmp.bmpBuffLen, 1, fp) != 1)
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
    m_bmp.pBmpBuff	= pBuff;
    m_bmp.bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
    m_bmp.bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
    m_bmp.bmpBufferType = NURO_BMP_BUFFERTYPE_MALLOC;
    m_bmp.bmpTransfColor    = NURO_BMP_TRANSCOLOR_565;
    return true;
}

nuBOOL CNuroBmpZK::CreateNuroBmp(const nuWCHAR* ptsBmpName,nuLONG With,nuLONG High)
{
	FreeNuroBmp();

	/* Open the bmp file */
	nuFILE* fp = nuWfopen(ptsBmpName, NURO_FS_RB);
	if(fp == NULL)
	{
		return nuFALSE;
	}

	/* read bmp header */
	NURO_BITMAPFILEHEADER bmpFileHead;
	nuMemset(&bmpFileHead, 0, sizeof(NURO_BITMAPFILEHEADER));
	if(nuFread(&bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, fp) != 1)
	{
		nuFclose(fp);
		return nuFALSE;
	}

	/* read info head */
	NURO_BITMAPINFOHEADER bmpInfoHead;
	nuMemset(&bmpInfoHead, 0, sizeof(NURO_BITMAPINFOHEADER));
	if( nuFread(&bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, fp) != 1)
	{
		nuFclose(fp);
		return nuFALSE;
	}
	bmpInfoHead.biWidth = With;
	bmpInfoHead.biHeight = High;

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

	m_bmp.bytesPerLine = DataSizePerLineNew;
	m_bmp.bmpBuffLen   = m_bmp.bytesPerLine*((nuWORD)bmpInfoHead.biHeight);
	nuBYTE* pBuff = (nuBYTE*)nuMalloc(m_bmp.bmpBuffLen);
	if(pBuff == NULL)
	{
		nuFree(pLine);
		nuFclose(fp);
		return nuFALSE;
	}
	nuFseek(fp, bmpFileHead.bfOffBits, NURO_SEEK_SET);
	if(nuFread(pBuff, m_bmp.bmpBuffLen, 1, fp) != 1)
	{
		nuFree(pBuff);
		nuFclose(fp);
		return nuFALSE;
	}
// 	if(!arrTag)
// 	{
// 		nuINT i=0;
// 		nuINT iEnd = bmpInfoHead.biHeight >> 1;
// 		nuBYTE *pBuffTag = pBuff;
// 		nuBYTE *pBuffTag2 = pBuffTag + (bmpInfoHead.biHeight-1)*DataSizePerLineNew;
// 		for(i=0; i<iEnd; ++i)
// 		{
// 			nuMemcpy(pLine, pBuffTag, DataSizePerLineNew);
// 			nuMemcpy(pBuffTag, pBuffTag2, DataSizePerLineNew);
// 			nuMemcpy(pBuffTag2, pLine, DataSizePerLineNew);
// 			pBuffTag += DataSizePerLineNew;
// 			pBuffTag2 -= DataSizePerLineNew;
// 		}
// 	}
	nuFree(pLine);
	nuFclose(fp);
	m_bmp.pBmpBuff	= pBuff;
	m_bmp.bmpWidth	= (nuWORD)bmpInfoHead.biWidth;
	m_bmp.bmpHeight	= (nuWORD)bmpInfoHead.biHeight;
	m_bmp.bmpBufferType = NURO_BMP_BUFFERTYPE_MALLOC;
	m_bmp.bmpTransfColor    = NURO_BMP_TRANSCOLOR_565;
	return nuTRUE;
}
nuVOID CNuroBmpZK::FreeNuroBmp()
{
    if( NULL != m_bmp.pBmpBuff )
    {
        nuFree(m_bmp.pBmpBuff);
        m_bmp.pBmpBuff  = NULL;
    }
}
