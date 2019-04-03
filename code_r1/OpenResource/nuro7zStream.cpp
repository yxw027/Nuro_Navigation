
#include "Nuro7zStream.h"
#include "NuroArchiveInc.h"
extern "C"
{    
    SRes nuroSeekInStream_file_Read(void *p, void *buf, size_t *size)
    {
        size_t originalSize = *size;
        nuroSeekInStream_fp* sfp = (nuroSeekInStream_fp*)p;
        *size = nuFread(buf, 1, originalSize, sfp->fp);
        if (*size == originalSize)
            return 0;
        return -1;
    }
    SRes nuroSeekInStream_file_Seek(void *p, Int64 *pos, ESzSeek origin)
    {
        int moveMethod;
        int res;
        nuroSeekInStream_fp* sfp = (nuroSeekInStream_fp*)p;
        switch (origin)
        {
        case SZ_SEEK_SET: moveMethod = NURO_SEEK_SET; break;
        case SZ_SEEK_CUR: moveMethod = NURO_SEEK_CUR; break;
        case SZ_SEEK_END: moveMethod = NURO_SEEK_END; break;
        default: return 1;
        }
        res = nuFseek(sfp->fp, (long)*pos, moveMethod);
        *pos = nuFtell(sfp->fp);
        return res;
    }
    void nuroSeekInStream_file_INIT(nuroSeekInStream_fp *p, nuFILE* fp)
    {
        p->fp = fp;
        p->s.Read = nuroSeekInStream_file_Read;
        p->s.Seek = nuroSeekInStream_file_Seek;
    }    
    SRes nuroSeekInStream_mem_Read(void *p, void *buf, size_t *size)
    {
        nuroSeekInStream_mem* sm = (nuroSeekInStream_mem*)p;
        size_t originalSize = *size;
        size_t lesssize = sm->size - sm->tag;
        if (lesssize < originalSize)
        {
            originalSize = lesssize;
        }
        if (originalSize)
        {
            nuMemcpy(buf, sm->buffer+sm->tag, originalSize);
        }
        sm->tag += originalSize;
        if (*size == originalSize)
            return 0;
        return -1;
    }
    SRes nuroSeekInStream_mem_Seek(void *p, Int64 *pos, ESzSeek origin)
    {
        nuroSeekInStream_mem* sm = (nuroSeekInStream_mem*)p;
        switch (origin)
        {
        case SZ_SEEK_SET: 
            if (*pos<0)
            {
                sm->tag = 0;
                *pos = sm->tag;
                return 1;
            }
            else if (*pos > sm->size)
            {
                sm->tag = sm->size;
                *pos = sm->tag;
                return 1;
            }
            sm->tag = *pos;
            *pos = sm->tag;
            return 0;
        case SZ_SEEK_CUR: 
            if ( (*pos+sm->tag)<0)
            {
                sm->tag = 0;
                *pos = sm->tag;
                return 1;
            }
            else if ((*pos+sm->tag) > sm->size)
            {
                sm->tag = sm->size;
                *pos = sm->tag;
                return 1;
            }
            sm->tag += *pos;
            *pos = sm->tag;
            return 0;
        case SZ_SEEK_END:
            if ( (*pos+sm->size)<0)
            {
                sm->tag = 0;
                *pos = sm->tag;
                return 1;
            }
            else if ((*pos+sm->size) > sm->size)
            {
                sm->tag = sm->size;
                *pos = sm->tag;
                return 1;
            }
            sm->tag = *pos+sm->size;
            *pos = sm->tag;
            return 0;
        default: 
            return 1;
        }
        return 1;
    }
    void nuroSeekInStream_mem_INIT(nuroSeekInStream_mem *p, unsigned char* buffer, nuUINT size)
    {
        p->buffer = buffer;
        p->size = size;
        p->tag = 0;
        p->s.Read = nuroSeekInStream_mem_Read;
        p->s.Seek = nuroSeekInStream_mem_Seek;
    }
};
