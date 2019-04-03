
#include "libCallManager.h"

class CMainCall     *g_cmMainCall = NULL;
CALLMANAGER_API nuPVOID GCM_InitModule(nuPVOID pParam)
{
    g_cmMainCall = new CMainCall();
    return g_cmMainCall;
}
CALLMANAGER_API nuVOID  GCM_FreeModule()
{
    if( NULL != g_cmMainCall )
    {
        delete g_cmMainCall;
        g_cmMainCall = NULL;
    }
}
