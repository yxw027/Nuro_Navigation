// nuroResource.h: interface for the nuroResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCESBASE_H__B8B6641F_479E_45BB_8F68_FEBE2B590258__INCLUDED_)
#define AFX_RESOURCESBASE_H__B8B6641F_479E_45BB_8F68_FEBE2B590258__INCLUDED_

#include "NuroArchive.h"

class nuroResource
{
public:
	nuroResource();
    virtual ~nuroResource();
	nuBOOL Initialize();
    nuUINT	LoadPack(const nuTCHAR* packname, nuINT type=LOAD_PACK_TYPE_FILE);
    nuVOID	UnloadPack();
    nuVOID*	FindFile(const nuCHAR* filepath);
    nuUINT	GetFileLength(nuVOID* file);
    nuBOOL	LoadFileData(nuVOID* file, nuUCHAR* buffer, nuUINT size);
    nuBOOL	LoadNuroBMP(NURO_BMP* pNuroBmp, nuVOID* file);
    nuroArchive* m_CommonPack;

    nuroArchive*	LoadPack(const nuTCHAR* packname, nuINT type, nuUINT &total);
    nuVOID	UnloadPack(nuroArchive* resource);
    nuVOID*	FindFile(nuroArchive* resource, const nuCHAR* filepath);
    nuUINT	GetFileLength(nuroArchive* resource, nuVOID* file);
    nuBOOL	LoadFileData(nuroArchive* resource, nuVOID* file, nuUCHAR* buffer, nuUINT size);    
    nuBOOL	LoadNuroBMP(nuroArchive* resource, NURO_BMP* pNuroBmp, nuVOID* file);

    nuVOID    UnloadNuroBMP(NURO_BMP* pNuroBmp);
};

#endif // !defined(AFX_RESOURCESBASE_H__B8B6641F_479E_45BB_8F68_FEBE2B590258__INCLUDED_)
