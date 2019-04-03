// nuroArchive.h: interface for the nuroArchive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROARCHIVE_H__726DF570_38C8_448E_8A55_B99A635A572E__INCLUDED_)
#define AFX_NUROARCHIVE_H__726DF570_38C8_448E_8A55_B99A635A572E__INCLUDED_

#define LOAD_PACK_TYPE_MEMORY   0
#define LOAD_PACK_TYPE_FILE     1
#include "NuroArchiveInc.h"
class nuroArchive  
{
public:
    nuroArchive();
    virtual ~nuroArchive();    
    virtual nuUINT	LoadPack(nuFILE* fp, nuINT type);
    virtual nuVOID	UnloadPack();
    virtual nuVOID*	FindFile(const nuCHAR* filepath);
    virtual nuUINT	GetFileLength(nuVOID* file);
    virtual nuBOOL	LoadFileData(nuVOID* file, nuUCHAR* buffer, nuUINT size);
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
#endif // !defined(AFX_NUROARCHIVE_H__726DF570_38C8_448E_8A55_B99A635A572E__INCLUDED_)
