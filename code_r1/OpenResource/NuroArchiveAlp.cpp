// nuroArchiveAlp.cpp: implementation of the nuroArchiveAlp class.
//
//////////////////////////////////////////////////////////////////////

#include "NuroArchiveAlp.h"
#include "nurolzma.h"

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



nuroArchiveAlp::nuroArchiveAlp()
{
    nuMemset(&_packHead, 0, sizeof(_packHead));
    _packInfo = 0;
    prepareCryptTable(cryptTable, 2);
    _packData = 0;
    _fp = 0;
    m_type = 0;
}
nuroArchiveAlp::~nuroArchiveAlp()
{
    UnloadPack();
}
nuVOID nuroArchiveAlp::prepareCryptTable(nuUINT* cryptTable, nuUINT n)
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
nuUINT nuroArchiveAlp::HashString(const nuCHAR *lpszFileName, nuUINT dwCryptIndex)
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
nuUINT nuroArchiveAlp::LoadPack(nuFILE* fp, nuINT type)
{
    UnloadPack();
    m_type = type;
    if (1 != nuFread(&_packHead, sizeof(_packHead), 1, fp))
    {
        nuFclose(fp);
        UnloadPack();
        return 0;
    }
    if (1000 != _packHead.version)
    {
        nuFclose(fp);
        UnloadPack();
        return 0;
    }
    if (_packHead.total && _packHead.indexsize)
    {
        if (LOAD_PACK_TYPE_MEMORY == m_type)
        {
            CFileCloseGuard fcg(fp);
            size_t len = nuFgetlen(fp);
            _packData = (nuUCHAR*) nuMalloc(len);
            if (!_packData)
            {
                UnloadPack();
                return 0;
            }
            nuFseek(fp, 0, NURO_SEEK_SET);
            if (len != nuFread(_packData, 1, len, fp))
            {
                UnloadPack();
                return 0;
            }
            _packInfo = _packData + _packHead.indexpos;
        }
        else
        {
            _fp = fp;
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
    }
    return _packHead.total;
    return 0;
}

nuVOID nuroArchiveAlp::UnloadPack()
{
    if (LOAD_PACK_TYPE_MEMORY == m_type)
    {
        if (_packData)
        {
            nuFree(_packData);
            _packData = 0;
        }
        _packInfo = 0;
    }
    else
    {
        if (_packInfo)
        {
            nuFree(_packInfo);
            _packInfo = 0;
        }
        if (_fp)
        {
            nuFclose(_fp);
            _fp = 0;
        }
    }
    nuMemset(&_packHead, 0, sizeof(_packHead));
}

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

nuVOID* nuroArchiveAlp::FindFile(const nuCHAR* filepath)
{
	/***************把字符串全部转换成大写***************/
	nuCHAR str[64] = {0};
	nuStrcpy(str, filepath);
	/*nuINT i = 0; 
	while( str[i] != '\0' )
	{
		if( str[i] >= 'a' && str[i] <= 'z' )
		{
			str[i] = str[i] - 32;
		}
		i++;
	}*/
	/******************add @ 2011.05.18******************/
    if ( !_packInfo )
    {
        return 0;
    }
    ALPackNode* node = (ALPackNode*)_packInfo;
    nuUINT total = _packHead.total; 
    nuUINT H1 = HashString(str, 0);
    nuUINT H2 = HashString(str, 1); //>>16;
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

nuUINT nuroArchiveAlp::GetFileLength(nuVOID* file0)
{
    if (!file0)
    {
        return 0; 
    }
    ALPackNode* file = (ALPackNode*)file0; 
    return file->fileSize; 
}

#define IN_BUF_SIZE 256*1024

nuBOOL nuroArchiveAlp::LoadFileData(nuVOID* file0, nuUCHAR* buffer, nuUINT size)
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
    if (LOAD_PACK_TYPE_MEMORY == m_type)
    {
        ALPackFileInfo fileInfo;
        nuMemcpy(&fileInfo, _packData+_packHead.datapos+file->dataPos, sizeof(fileInfo));
        nuUCHAR* data =  _packData+_packHead.datapos+file->dataPos+sizeof(fileInfo)+fileInfo.dataDiffPos;
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
            nuMemcpy(buffer, data, filesize);
            return nuTRUE;
        }
        else if( 0x0010 == (fileInfo.flag & 0x00F0 ) && 0x0000 == (fileInfo.flag & 0x000F) )
        {       
            nurolzma::SizeT inProcessed = fileInfo.cmpsSize;
            nurolzma::SizeT outProcessed = fileInfo.fileSize;
            nurolzma::ELzmaStatus status;
#if 1
            nurolzma::SRes res = nurolzma::LzmaDecode(buffer, &outProcessed, data, &inProcessed, fileInfo.props
                , LZMA_PROPS_SIZE, nurolzma::LZMA_FINISH_ANY, &status, &nurolzma::g_Alloc);
#else
            nurolzma::CLzmaDec state;
            LzmaDec_Construct(&state);
            if( SZ_OK != nurolzma::LzmaDec_Allocate(&state, fileInfo.props, LZMA_PROPS_SIZE, &nurolzma::g_Alloc) )
            {
                return nuFALSE;
            }
            nurolzma::LzmaDec_Init(&state); 
            nurolzma::SRes res = nurolzma::LzmaDec_DecodeToBuf(&state, buffer, &outProcessed,
                data, &inProcessed, nurolzma::LZMA_FINISH_ANY, &status);
            nurolzma::LzmaDec_Free(&state, &nurolzma::g_Alloc);
#endif
            if (res != SZ_OK)
            {
                return nuFALSE;
            }
            return nuTRUE;
        }
    }
    else
    {
        nuFseek(_fp, _packHead.datapos + file->dataPos, NURO_SEEK_SET);
        ALPackFileInfo fileInfo;
        if( 1 != nuFread(&fileInfo, sizeof(fileInfo), 1, _fp) )
        {
            return nuFALSE;
        }
        nuFseek(_fp, fileInfo.dataDiffPos, NURO_SEEK_CUR);
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
            if( 1 != nuFread(buffer, filesize, 1, _fp) )
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
            size_t inPos = 0, inSize = 0, outPos = 0;
            size_t readTotal = 0;
            for( ;; )
            {
                if( inPos == inSize )
                {
                    inSize = IN_BUF_SIZE;
                    if (inSize > (fileInfo.cmpsSize-readTotal))
                    {
                        inSize = fileInfo.cmpsSize-readTotal;
                    }
                    inSize = nuFread(inBuf, 1, inSize, _fp);
                    readTotal += inSize;
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
 //               nuDWORD t1 = nuGetTickCount();
                nurolzma::SRes res = nurolzma::LzmaDec_DecodeToBuf(&state, buffer + outPos, &outProcessed,
                    inBuf + inPos, &inProcessed, finishMode, &status);
//                 nuDWORD t2 = nuGetTickCount();
//                 nuDWORD diff = t2-t1;
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
    }
    return nuFALSE;
}

