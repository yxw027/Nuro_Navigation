#ifndef _CALL_MANAGER_LOAD_OPEN_RESOURCE
#define _CALL_MANAGER_LOAD_OPEN_RESOURCE

#include "NuroClasses.h"

#ifdef USE_DYM_DLL
#define _USE_OPENRESOURCE_DLL
#else
#define _USE_POENRESOURCE_LIB
#endif

#ifdef _USE_OPENRESOURCE_DLL
class CLoadOpenResource : public CLoadDllZK 
{
#else

#	ifndef __cplusplus
		extern nuPVOID LibOPR_InitModule(nuPVOID lpParam);
		extern nuVOID  LibOPR_FreeModule();
#	else
		extern "C" nuPVOID LibOPR_InitModule(nuPVOID lpParam);
		extern "C" nuVOID  LibOPR_FreeModule();
#	endif

class CLoadOpenResource 
{
#endif

#ifdef _USE_POENRESOURCE_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "OpenResource.lib")
#endif
#endif

public:
    CLoadOpenResource()
#ifdef _USE_OPENRESOURCE_DLL
    :CLoadDllZK( nuTEXT("OpenResource"), "LibOPR_InitModule", "LibOPR_FreeModule")
#endif
    {
    }
    virtual ~CLoadOpenResource()
    {
        FreeOpenResource();
    }
    //
    nuPVOID  LoadOpenResource(const nuTCHAR* ptzDllPath = NULL, nuPVOID pParam = NULL)
    {
#ifdef _USE_OPENRESOURCE_DLL
        return LoadDll(ptzDllPath, pParam);
#else
        return LibOPR_InitModule(pParam);
#endif
    }
    nuVOID   FreeOpenResource()
    {
#ifdef _USE_OPENRESOURCE_DLL
        FreeDll();
#else
        LibOPR_FreeModule();
#endif
    }
};

#endif
