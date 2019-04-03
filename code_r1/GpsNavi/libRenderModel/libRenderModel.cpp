// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: libRenderModel.cpp,v 1.13 2010/04/13 09:16:50 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/13 09:16:50 $
// $Locker:  $
// $Revision: 1.13 $
// $Source: /home/nuCode/libRenderModel/libRenderModel.cpp,v $
/////////////////////////////////////////////////////////////////////////
#include "libRenderModel.h"
#include "EglRender.h"
#include "3drManeger.h"
#include "NuroModuleApiStruct.h"

CEglRender  g_cEglRender;
C3drManeger g_c3drManeger;

nuPVOID g_pApiForRM = nuNULL;

RENDERMODEL_API nuBOOL LibRM_InitRenderModel(PAPI_RENDERMODEL pApiRM, nuPVOID pApiForRM)
{
	if (nuNULL != pApiRM)
	{
		pApiRM->RM_AddModel		= LibRM_AddModel;
		pApiRM->RM_DelModel		= LibRM_DelModel;
		pApiRM->RM_StartRenderModel	= LibRM_StartRenderModel;
		pApiRM->RM_SetViewOption	= LibRM_SetViewOption;
		pApiRM->RM_DrawBitmap		= LibRM_DrawBitmap;
	}
    	g_pApiForRM = pApiForRM;
	return nuTRUE;
}

RENDERMODEL_API nuVOID LibRM_FreeRenderModel()
{
}

RENDERMODEL_API nuUINT	LibRM_StartRenderModel(PPARAMETER_START_OPTION pParamStart)
{
	if(!g_cEglRender.Init(pParamStart->sysDiff))
	{
		return 0;
	}
        if (!g_c3drManeger.Init(g_pApiForRM, &g_cEglRender))
	{
		return 0;
	}
	return 1;
}

RENDERMODEL_API nuUINT	LibRM_AddModel( nuLONG nID, nuLONG x, nuLONG y, char *pszModelName, nuLONG rx, nuLONG ry, nuLONG ty, 
									    nuINT height, nuINT dCenD, float fs, float fz, PMATHTOOLAPI pMT )
{
    return g_c3drManeger.Add3dr(nID, x, y, pszModelName) ? 1 : 0;
}

RENDERMODEL_API nuUINT	LibRM_DelModel(nuLONG nID, nuLONG x, nuLONG y)
{
    g_c3drManeger.Del3dr(nID);
    return 1;
}

RENDERMODEL_API nuUINT	LibRM_SetViewOption(PPARAMETER_VIEW_OPTION pParamView)
{
    if (!g_cEglRender.SetViewOption(pParamView))
    {
        return 0;
    }
    return 1;
}

RENDERMODEL_API nuUINT	LibRM_DrawBitmap(PPARAMETER_BITMAP pParamBitmap)
{
    g_c3drManeger.Draw3dr();
    return g_cEglRender.GetBitmapData(pParamBitmap) ? 1 : 0;
}
