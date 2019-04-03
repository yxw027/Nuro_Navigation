// nuroResource.cpp: implementation of the nuroResource class.
//
//////////////////////////////////////////////////////////////////////

#include "Resources.h"
#include "nurolzma.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum COMPRESSTYPE
{
	COMPRESS_TYPE_NONE = 0,
	COMPRESS_TYPE_LZMA
};
struct ALPackFileInfo
{
	nuUINT fileSize;
	nuUINT cmpsSize;
	nuUINT dataDiffPos;
	nuUSHORT nameSize;
	/*
	flag & 0x8000	exist tag:
		0x0000	none
		0x8000	is exist	
	flag & 0x4000	extend tag:
		0x0000	none
		0x4000	is extend
	flag & 0x00F0	compressed type:
		0x0000 none
		0x0010 The file is compressed using LZMA
	flag & 0x000F	encrypted type: 
		0x0000	none
		0x0001	The file is encrypted using rc4
		*/
	nuUSHORT flag;
	nuUCHAR props[8];//[0/1/2/3/4]LZMA-props
};
struct ALPackNode
{
	nuUINT dataPos;
	nuUINT fileSize;
	nuUINT H1;
	nuUINT H2;
};

class CFileCloseGuard
{
public:
	CFileCloseGuard(nuFILE* fp) : file(fp)
	{
	}
	~CFileCloseGuard()
	{
		if (file)
		{
			nuFclose(file);
		}
	}
private:
	CFileCloseGuard();
	nuFILE* file;
};

class CMemFreeGuard
{
public:
	CMemFreeGuard(nuVOID* p) : _p(p)
	{
	}
	~CMemFreeGuard()
	{
		if (_p)
		{
			nuFree(_p);
		}
	}
private:
	CMemFreeGuard();
	nuVOID* _p;
};

CResources::CResources()
{
	nuMemset(&_packHead, 0, sizeof(_packHead));
	nuMemset(&_packName, 0, sizeof(_packName));
	_packInfo = 0;
	prepareCryptTable(cryptTable, 2);
}

CResources::~CResources()
{
	UnloadPack();
}

nuBOOL CResources::Initialize()
{
	return nuTRUE;
}

nuVOID CResources::prepareCryptTable(nuUINT* cryptTable, nuUINT n)
{ 
	nuUINT dwHih, dwLow,seed = 0x00100001,index1 = 0,index2 = 0, i;
	for( index1 = 0; index1 < 0x100; ++index1 )
	{
		for( index2 = index1, i = 0; i < n; ++i, index2 += 0x100 )
		{ 
			seed = (seed * 125 + 3) % 0x2AAAAB;
			dwHih= (seed & 0xFFFF) << 0x10;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			dwLow= (seed & 0xFFFF);
			cryptTable[index2] = (dwHih | dwLow); 
		} 
	} 
}

nuUINT CResources::HashString(const nuCHAR *lpszFileName, nuUINT dwCryptIndex)
{ 
	nuUCHAR *key = (nuUCHAR *)lpszFileName;
	nuUINT seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
	nuINT ch;
	while( *key != 0 )
	{ 
		//ch = toupper(*key++);
		ch = (*key++);
		seed1 = cryptTable[(dwCryptIndex<< 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
	}
	return seed1;
}

nuBOOL CResources::LoadNBC(NURO_BMP* pNuroBmp, const nuTCHAR* filename)
{
	if( !pNuroBmp )
	{
		return nuFALSE;
	}
	nuFILE* fp = nuTfopen(filename, NURO_FS_RB);
    if( !fp )
    {
        return nuFALSE;
    }
	NuroBmpContent nbc;
	if( 1 != nuFread(&nbc, sizeof(nbc), 1, fp) )
    {
        nuFclose(fp);
        return nuFALSE; 
    }
	if( NURO_BMP_CONTENT_TYPE_565 == nbc.type )
	{
//		pNuroBmp->bmpBitCount = 16; 
		nuBYTE* buffer = (nuBYTE*)nuMalloc(nbc.size);
		if( !buffer )
		{
			nuFclose(fp); 
			return nuFALSE;
		}
		if( 1 != nuFread(buffer, nbc.size, 1, fp) )
		{
			nuFclose(fp);
			nuFree(buffer);
			return nuFALSE;
		}
		if( NURO_BMP_CONTENT_COMPRESS_TYPE_LZMA == nbc.compresstype )
		{
			nuBYTE* data = (nuBYTE*)nuMalloc(nbc.compresssize);
			if( !data )
			{
				nuFclose(fp);
				nuFree(buffer);
				return nuFALSE;
			}
			nuUINT sizein = nbc.size;
			nuUINT sizeout = nbc.compresssize;
			if( SZ_OK != nurolzma::Lzma86_Decode(data, &sizeout, buffer, &sizein) )
			{
				nuFclose(fp);
				nuFree(buffer);
				nuFree(data);
				return nuFALSE;
			}
//			pNuroBmp->bmpBitCount = NURO_BMP_BITCOUNT;
//			pNuroBmp->bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
			pNuroBmp->bmpBuffLen  = nbc.compresssize;
			pNuroBmp->pBmpBuff	= data;
			pNuroBmp->bmpWidth	= nbc.width;
			pNuroBmp->bmpHeight	= nbc.height;
//			pNuroBmp->bmpHasLoad = 1;			
			nuFclose(fp);
			nuFree(buffer); 
			return nuTRUE;
		}
	}
	nuFclose(fp);
	return nuFALSE;
}

nuUINT CResources::LoadPack(const nuTCHAR* packname)
{
	UnloadPack();
	nuFILE* fp = nuTfopen(packname, NURO_FS_RB);
	if (!fp)
	{
		return 0;
	}
	CFileCloseGuard fcg(fp);	
	if (1 != nuFread(&_packHead, sizeof(_packHead), 1, fp))
	{
		UnloadPack();
		return 0;
	}
	if (1000 != _packHead.version)
	{
		UnloadPack();
		return 0;
	}
	if (_packHead.total && _packHead.indexsize)
	{
		_packInfo = (nuUCHAR*) nuMalloc(_packHead.indexsize);
		if (!_packInfo)
		{
			UnloadPack();
			return 0;
		}
		nuFseek(fp, _packHead.indexpos, NURO_SEEK_SET);
		if (_packHead.indexsize != nuFread(_packInfo, 1, _packHead.indexsize, fp))
		{
			UnloadPack();
			return 0;
		}
	}
	nuTcsncpy(_packName, packname, 256);
	return _packHead.total;
	return 0;
}

nuVOID CResources::UnloadPack()
{
	
	if (_packInfo)
	{
		nuFree(_packInfo);
		_packInfo = 0;
	}
	nuMemset(&_packHead, 0, sizeof(_packHead));
	nuMemset(&_packName, 0, sizeof(_packName));
}
extern nuINT ListNodeCmp(const ALPackNode& a, nuUINT H1, nuUINT H2);
/*
nuINT ListNodeCmp(const ALPackNode& a, nuUINT H1, nuUINT H2)
{
	if (a.H1 < H1)
	{
		return -1;
	}
	if (a.H1 > H1)
	{
		return 1;
	}
	if (a.H2 < H2)
	{
		return -1;
	}
	if (a.H2 > H2)
	{
		return 1;
	}
	return 0;
}
*/

nuVOID* CResources::FindFile(const nuCHAR* filepath)
{
	if ( !_packInfo )
	{
		return 0;
	}
	ALPackNode* node = (ALPackNode*)_packInfo;
	nuUINT total = _packHead.total; 
	nuUINT H1 = HashString(filepath, 0);
	nuUINT H2 = HashString(filepath, 1); //>>16;
	nuUINT left, right, middle;
	left = 0;
	right = total - 1;
	nuINT cmpres = 0;
	while( left <= right )
	{
		middle = (left+right) >> 1;
		cmpres = ListNodeCmp(node[middle], H1, H2);
		switch (cmpres)
		{
		case 0:
			return &node[middle];
		case 1:
			right = middle - 1;
			break;
		case -1:
		default:
			left = middle + 1;
			break;
		}
	}
	return 0;
}

nuUINT CResources::GetFileLength(nuVOID* file0)
{
	if (!file0)
	{
		return 0; 
	}
	ALPackNode* file = (ALPackNode*)file0; 
	return file->fileSize; 
}

#define IN_BUF_SIZE 256*1024

nuBOOL CResources::LoadFileData(nuVOID* file0, nuUCHAR* buffer, nuUINT size)
{
	if (!_packInfo || !file0 || !buffer)
	{
		return nuFALSE;
	}
	ALPackNode* file = (ALPackNode*)file0; 
	nuUINT filesize = GetFileLength(file);
	if (filesize > size)
	{
		return nuFALSE;
	}
	nuFILE* fp = nuTfopen(_packName, NURO_FS_RB);
	if (!fp)
	{
		return nuFALSE;
	}
	CFileCloseGuard fcg(fp);
	nuFseek(fp, _packHead.datapos + file->dataPos, NURO_SEEK_SET);
	ALPackFileInfo fileInfo;
	if( 1 != nuFread(&fileInfo, sizeof(fileInfo), 1, fp) )
	{
		return nuFALSE;
	}
	nuFseek(fp, fileInfo.dataDiffPos, NURO_SEEK_CUR);
	if( !(fileInfo.flag & 0x8000) ) //is delete
	{
		return nuFALSE;
	}
	if( fileInfo.flag & 0x4000 ) //is extend
	{
		return nuFALSE;
	}
	if( 0x0000 == (fileInfo.flag & 0x00FF) )
	{
		if( 1 != nuFread(buffer, filesize, 1, fp) )
		{
			return nuFALSE;
		}
		return nuTRUE;
	}
	else if( 0x0010 == (fileInfo.flag & 0x00F0 ) && 0x0000 == (fileInfo.flag & 0x000F) )
	{
		nurolzma::Byte* inBuf = (nurolzma::Byte*)nuMalloc(IN_BUF_SIZE);
		if( !inBuf )
		{
			return nuFALSE;
		}
		CMemFreeGuard mfg(inBuf);
		nurolzma::UInt64 unpackSize = (nurolzma::UInt64)filesize;
		nurolzma::UInt64 returnsize = unpackSize;
		nurolzma::CLzmaDec state;
		LzmaDec_Construct(&state);
		if( SZ_OK != nurolzma::LzmaDec_Allocate(&state, fileInfo.props, LZMA_PROPS_SIZE, &nurolzma::g_Alloc) )
		{
			return nuFALSE;
		}
		nurolzma::LzmaDec_Init(&state);
		nuSIZE inPos = 0, inSize = 0, outPos = 0;
		for( ;; )
		{
			if( inPos == inSize )
			{
				inSize = IN_BUF_SIZE;
				inSize = nuFread(inBuf, 1, inSize, fp);
				if( !inSize )
				{
					nurolzma::LzmaDec_Free(&state, &nurolzma::g_Alloc);
					return nuFALSE;
				}
				inPos = 0;
			}
			nurolzma::SizeT inProcessed = inSize - inPos;
			nurolzma::SizeT outProcessed = (nurolzma::SizeT)(returnsize - outPos);
			nurolzma::ELzmaFinishMode finishMode = nurolzma::LZMA_FINISH_ANY;
			nurolzma::ELzmaStatus status;		
			nurolzma::SRes res = nurolzma::LzmaDec_DecodeToBuf(&state, buffer + outPos, &outProcessed,
				inBuf + inPos, &inProcessed, finishMode, &status);
			inPos += inProcessed;
			outPos += outProcessed;
			unpackSize -= outProcessed;
			if( !unpackSize )
			{
				nurolzma::LzmaDec_Free(&state, &nurolzma::g_Alloc);
				if (res != SZ_OK)
				{
					return nuFALSE;
				}
				return nuTRUE;
			}
			if( !inProcessed && !outProcessed )
			{
				nurolzma::LzmaDec_Free(&state, &nurolzma::g_Alloc);
				return nuFALSE;
			}
		}
		nurolzma::LzmaDec_Free(&state, &nurolzma::g_Alloc);
		return nuFALSE;
	}
	return nuFALSE;
}

nuBOOL CResources::LoadNuroBMP(NURO_BMP* pNuroBmp, nuVOID* file)
{
//	if( !pNuroBmp || pNuroBmp->bmpHasLoad )
	if( !pNuroBmp )
	{
		return nuFALSE;
	}
	nuSIZE filesize = GetFileLength(file);
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
	if( !LoadFileData(file, buffer, filesize) )
	{
		return nuFALSE;
	}

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

	return nuTRUE;
}

nuVOID CResources::UnloadNuroBMP(NURO_BMP* pNuroBmp)
{
	if( pNuroBmp != NULL && pNuroBmp->pBmpBuff != NULL )
	{
        nuFree(pNuroBmp->pBmpBuff);
        pNuroBmp->pBmpBuff = NULL;
	}
}
