// nuroArchive7z.cpp: implementation of the nuroArchive7z class.
//
//////////////////////////////////////////////////////////////////////

#include "NuroArchive7z.h"
nuroArchive7z::nuroArchive7z()
{
    LookToRead_CreateVTable(&lookStream, False);
    LookToRead_Init(&lookStream);      
    allocImp.Alloc = SzAlloc;
    allocImp.Free = SzFree;    
    allocTempImp.Alloc = SzAllocTemp;
    allocTempImp.Free = SzFreeTemp;  
    nuMemset(&archiveStream_fp, 0, sizeof(archiveStream_fp));
    nuMemset(&archiveStream_mem, 0, sizeof(archiveStream_mem));
    buffer = 0;
    m_load = nuFALSE;
    m_type = 0;
}

nuroArchive7z::~nuroArchive7z()
{
    UnloadPack();
}
nuVOID nuroArchive7z::Prepare()
{
#if LZMA_7Z_USE_CRCCALC
    CrcGenerateTable();
#endif
}
nuUINT	nuroArchive7z::LoadPack(nuFILE* fp, nuINT type)
{
    UnloadPack();
    m_type = type;
    SRes res;
    if (LOAD_PACK_TYPE_MEMORY == m_type)
    {
        CFileCloseGuard fcg(fp);
        nuFseek(fp, 0, NURO_SEEK_END);
        nuSIZE length = nuFtell(fp);
        nuFseek(fp, 0, NURO_SEEK_SET);
        buffer = (nuUCHAR*)nuMalloc(length);
        if (!buffer)
        {
            return 0;
        }
        if (nuFread(buffer, 1, length, fp)!=length)
        {
            nuFree(buffer);
            buffer = 0;
            return 0;
        }
        nuroSeekInStream_mem_INIT(&archiveStream_mem, buffer, length);
        lookStream.realStream = &archiveStream_mem.s;
    }
    else
    {        
        nuroSeekInStream_file_INIT(&archiveStream_fp, fp);
        lookStream.realStream = &archiveStream_fp.s;
    }
    SzArEx_Init(&db);
    res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
    if (res != SZ_OK)
    {
        return 0;
    }
    m_load = nuTRUE;
    return db.db.NumFiles;
}
nuVOID	nuroArchive7z::UnloadPack()
{    
    if (m_load)
    {
        SzArEx_Free(&db, &allocImp);
        if (LOAD_PACK_TYPE_MEMORY == m_type)
        {
            if (buffer)
            {
                archiveStream_mem.buffer = 0;
                nuFree(buffer);
                buffer = 0;
            }
        }
        else
        {
            if (archiveStream_fp.fp)
            {
                nuFclose(archiveStream_fp.fp);
                archiveStream_fp.fp = 0;
            }
        }
        m_load = nuFALSE;
    }
}
nuVOID*	nuroArchive7z::FindFile(const nuCHAR* filepath)
{
    if (!m_load || !db.db.NumFiles)
    {
        return 0;
    }
//     UInt32 i;
//     for (i = 0; i < db.db.NumFiles; i++)
//     {
//         CSzFileItem *f = db.db.Files + i;
//         if (!strcmp(f->Name, filepath+1))
//         {
//             return (nuVOID*)(i+1);
//         }
//     }
    nuINT low = 0;
    nuINT high = db.db.NumFiles - 1;
    nuINT mid = 0;
    nuINT cmpres = 0;
    while (low<=high)
    {
        mid=low+((high-low)>>1);
        cmpres = nuStrcmp(db.db.Files[mid].Name, filepath+1);
        if (!cmpres)
        {
            return (nuVOID*)(mid+1);
        }
        if (cmpres > 0)
        {
            high = mid - 1;
        }
        else
        {
            low = mid+1;
        }
    }
    return 0;
}
nuUINT	nuroArchive7z::GetFileLength(nuVOID* file)
{
    if (!m_load)
    {
        return 0;
    }
    if (file)
    {
        UInt32 i = (UInt32)file - 1;
        return db.db.Files[i].Size;
    }
    return 0;
}
nuBOOL	nuroArchive7z::LoadFileData(nuVOID* file, nuUCHAR* buffer, nuUINT size)
{
    if (!m_load)
    {
        return nuFALSE;
    }
    if (!file)
    {
        return nuFALSE;
    }
    UInt32 i = (UInt32)file - 1;
    CSzFileItem *f = db.db.Files + i;
    if (f->IsDir)
    {
        return nuFALSE;
    }
    UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
    Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
    nuSIZE outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
    SRes res;
    nuSIZE offset;
    nuSIZE outSizeProcessed;
    res = SzAr_Extract(&db, &lookStream.s, i,
        &blockIndex, &outBuffer, &outBufferSize,
        &offset, &outSizeProcessed,
        &allocImp, &allocTempImp);
    if (res != SZ_OK || outSizeProcessed != size)
    {
        if (outBuffer)
        {
            IAlloc_Free(&allocImp, outBuffer);
        }
       return nuFALSE;
    }
    nuMemcpy(buffer, outBuffer, outSizeProcessed);
    IAlloc_Free(&allocImp, outBuffer);
    return nuTRUE;
}
