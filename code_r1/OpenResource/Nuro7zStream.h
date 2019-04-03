
#ifndef NURO_7Z_STRAM_H_F8943HKHFSNPZMQ
#define NURO_7Z_STRAM_H_F8943HKHFSNPZMQ

#include "NuroArchiveInc.h"
extern "C"
{
#include "./7z/Types.h"
    typedef struct tagSeekInStream_fp
    {
        ISeekInStream s;
        nuFILE* fp;
    } nuroSeekInStream_fp;

    SRes nuroSeekInStream_file_Read(void *p, void *buf, size_t *size);
    SRes nuroSeekInStream_file_Seek(void *p, Int64 *pos, ESzSeek origin);
    void nuroSeekInStream_file_INIT(nuroSeekInStream_fp *p, nuFILE* fp);

    typedef struct tagSeekInStream_mem
    {
        ISeekInStream s;
        unsigned char* buffer;
        nuUINT tag;
        nuUINT size;
    } nuroSeekInStream_mem;
    
    SRes nuroSeekInStream_mem_Read(void *p, void *buf, size_t *size);
    SRes nuroSeekInStream_mem_Seek(void *p, Int64 *pos, ESzSeek origin);
    void nuroSeekInStream_mem_INIT(nuroSeekInStream_mem *p, unsigned char* buffer, nuUINT size);
};

#endif //NURO_7Z_STRAM_H_F8943HKHFSNPZMQ
