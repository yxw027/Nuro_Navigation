// nuroArchive7z.h: interface for the nuroArchive7z class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCES7Z_H__B20D0FA9_91D9_44B5_8E43_0F6ABE8A4761__INCLUDED_)
#define AFX_RESOURCES7Z_H__B20D0FA9_91D9_44B5_8E43_0F6ABE8A4761__INCLUDED_

#include "NuroArchive.h"

extern "C" 
{
#include "7z/7zCrc.h"
#include "7z/7zVersion.h"    
#include "7z/7zAlloc.h"
#include "7z/7zExtract.h"
#include "7z/7zIn.h"
#include "Nuro7zStream.h"
};
class nuroArchive7z : public nuroArchive
{
public:
	nuroArchive7z();
	virtual ~nuroArchive7z();

    static nuVOID Prepare();

    virtual nuUINT	LoadPack(nuFILE* fp, nuINT type);
    virtual nuVOID	UnloadPack();
    virtual nuVOID*	FindFile(const nuCHAR* filepath);
    virtual nuUINT	GetFileLength(nuVOID* file);
    virtual nuBOOL	LoadFileData(nuVOID* file, nuUCHAR* buffer, nuUINT size);
    nuroSeekInStream_fp  archiveStream_fp;
    nuroSeekInStream_mem archiveStream_mem;
    nuUCHAR* buffer;
    CLookToRead lookStream;
    CSzArEx db;
    ISzAlloc allocImp;
    ISzAlloc allocTempImp;

    nuBOOL m_load;
    nuINT m_type;
};

#endif // !defined(AFX_RESOURCES7Z_H__B20D0FA9_91D9_44B5_8E43_0F6ABE8A4761__INCLUDED_)
