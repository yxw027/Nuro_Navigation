
#ifndef __NURO_NAVICTRL_20110128
#define __NURO_NAVICTRL_20110128

#include "INaviControl.h"

#ifdef NAVICTRL_EXPORTS
#define NAVICTRL_API extern "C" __declspec(dllexport)
#else
#ifdef _USE_NAVICONTROL_DLL
#define NAVICTRL_API extern "C" __declspec(dllimport)
#else
#define NAVICTRL_API extern "C"
#endif
#endif

NAVICTRL_API nuPVOID  LibNC_InitModule(nuPVOID lpOutNC);
NAVICTRL_API nuVOID   LibNC_FreeModule();

#endif
