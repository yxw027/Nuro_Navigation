
#include "MainCall.h"


#ifdef CALLMANAGER_EXPORTS
#define CALLMANAGER_API extern "C" __declspec(dllexport)
#else
#define CALLMANAGER_API extern "C"
#endif

extern class CMainCall     *g_cmMainCall;
CALLMANAGER_API nuPVOID GCM_InitModule(nuPVOID pParam);
CALLMANAGER_API nuVOID  GCM_FreeModule();
