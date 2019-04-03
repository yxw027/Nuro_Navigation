
#include "libNaviControl.h"

CINaviControl* g_iNaviControl = NULL;

NAVICTRL_API nuPVOID LibNC_InitModule(nuPVOID lpOutNC)
{
    /*if( NULL == lpOutNC )
    {
        return NULL;
    }*/
	if( NULL == g_iNaviControl )
	{
		g_iNaviControl = new CINaviControl();
	}
	return g_iNaviControl;
}

NAVICTRL_API nuVOID LibNC_FreeModule()
{
	if( NULL != g_iNaviControl )
	{
		delete g_iNaviControl;
		g_iNaviControl = NULL;
	}
}





