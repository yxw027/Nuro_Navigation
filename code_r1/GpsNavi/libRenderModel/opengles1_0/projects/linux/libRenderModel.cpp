// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: libRenderModel.cpp,v 1.13 2010/04/13 09:16:50 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/13 09:16:50 $
// $Locker:  $
// $Revision: 1.13 $
// $Source: /home/nuCode/libRenderModel/libRenderModel.cpp,v $
/////////////////////////////////////////////////////////////////////////
#ifdef _USE_OPENGLES

#include "libRenderModel.h"
#include "OpenGLProcessZK.h"
#include "Read3DRModelZK.h"

//for test
#include "nuClib.h"
#include "nuApiInterFace.h"
#include <stdio.h>

class COpenGLProcessZK	g_openGLProcZK;
class CRead3DRModelZK	g_read3DModelZK;

RENDERMODEL_API bool LibRM_InitRenderModel(PAPI_RENDERMODEL pApiRM, PAPI_FOR_RM pApiForRM)
{
    fprintf(stderr, "%s at %s...%d..!!!\n\n\n", __func__, __FILE__, __LINE__);
	if( NULL != pApiRM )
	{
        fprintf(stderr, "%s at %s...%d..!!!\n\n\n", __func__, __FILE__, __LINE__);
		pApiRM->RM_AddModel		= LibRM_AddModel;
		pApiRM->RM_DelModel		= LibRM_DelModel;
		pApiRM->RM_StartRenderModel	= LibRM_StartRenderModel;
		pApiRM->RM_SetViewOption	= LibRM_SetViewOption;
		pApiRM->RM_DrawBitmap		= LibRM_DrawBitmap;
	}
        fprintf(stderr, "%s at %s...%d..!!!\n\n\n", __func__, __FILE__, __LINE__);
	CReadBaseZK::InitStatic();
	return true;
}

RENDERMODEL_API void LibRM_FreeRenderModel()
{
    fprintf(stderr, "InitOpenGLES InitOpenGLES InitOpenGLES at %s...%d..!!!!!!!!!!!!!!!!!\n\n\n", __FILE__, __LINE__);
	g_read3DModelZK.Free();
	g_openGLProcZK.FreeOpenGLES();
}

RENDERMODEL_API nuUINT	LibRM_StartRenderModel(PPARAMETER_START_OPTION pParamStart)
{
	RECT rtScreen;
	rtScreen.left	= pParamStart->rtScreen.left;
	rtScreen.top	= pParamStart->rtScreen.top;
	rtScreen.right	= pParamStart->rtScreen.right;
	rtScreen.bottom	= pParamStart->rtScreen.bottom;

    fprintf(stderr, "%s at %s...%d..!!!!!!!!!!!!!!!!!\n\n\n", __func__, __FILE__, __LINE__);
	
    
    if( !g_openGLProcZK.InitOpenGLES((HWND)pParamStart->hWnd, (HDC)pParamStart->hDC, rtScreen) )
	{
    fprintf(stderr, "%s at %s...%d..!!!!!!!!!!!!!!!!!\n\n\n", __func__, __FILE__, __LINE__);
		return 0;
	}
    fprintf(stderr, "%s at %s...%d..!!!!!!!!!!!!!!!!!\n\n\n", __func__, __FILE__, __LINE__);
	if( !g_read3DModelZK.Initialize( &g_openGLProcZK ) )
	{
		return 0;
	}
	return 1;
}

RENDERMODEL_API nuUINT	LibRM_AddModel(long nID, long x, long y, char *pszModelName
                                        , long rx, long ry, long ty, int height, int dCenD, float fs, float fz
                                        , PMATHTOOLAPI pMT)
{
	return g_read3DModelZK.AddModel(nID, x, y, pszModelName
         , rx, ry, ty, height, dCenD, fs, fz
         , pMT);
}

RENDERMODEL_API nuUINT	LibRM_DelModel(long nID, long x, long y)
{
	return g_read3DModelZK.DelModel(nID, x, y);
}

RENDERMODEL_API nuUINT	LibRM_SetViewOption(PPARAMETER_VIEW_OPTION pParamView)
{
	return g_openGLProcZK.SetViewOption(pParamView);
}

RENDERMODEL_API nuUINT	LibRM_DrawBitmap(PPARAMETER_BITMAP pParamBitmap)
{
    //return 1;
	g_read3DModelZK.DrawModel();
	return g_openGLProcZK.GetBitmapData(pParamBitmap);
}

#endif
