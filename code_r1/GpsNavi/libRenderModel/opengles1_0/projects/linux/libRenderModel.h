// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: libRenderModel.h,v 1.7 2010/04/13 09:16:50 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/13 09:16:50 $
// $Locker:  $
// $Revision: 1.7 $
// $Source: /home/nuCode/libRenderModel/libRenderModel.h,v $
/////////////////////////////////////////////////////////////////////////
#ifndef NURO_RENDER_MODEL_20090820
#define NURO_RENDER_MODEL_20090920


#include "nuDefine.h"
#include "nuroRenderModel.h"
//it must be after nuDefine.h
#ifdef _USRDLL
#ifdef RENDERMODEL_EXPORTS
#define RENDERMODEL_API extern "C" __declspec(dllexport)
#else
#define RENDERMODEL_API extern "C" __declspec(dllimport)
#endif
#else
#define RENDERMODEL_API 
#endif

RENDERMODEL_API bool	LibRM_InitRenderModel(PAPI_RENDERMODEL pApiRM, PAPI_FOR_RM pApiForRM);
RENDERMODEL_API void	LibRM_FreeRenderModel();

RENDERMODEL_API nuUINT	LibRM_StartRenderModel(PPARAMETER_START_OPTION pParamStart);
RENDERMODEL_API nuUINT	LibRM_AddModel(long nID, long x, long y, char *pszModelName
                                       , long rx, long ry, long ty, int height, int dCenD, float fs, float fz
                                       , PMATHTOOLAPI pMT);
RENDERMODEL_API nuUINT	LibRM_DelModel(long nID, long x, long y);
RENDERMODEL_API nuUINT	LibRM_SetViewOption(PPARAMETER_VIEW_OPTION pParamView);
RENDERMODEL_API nuUINT	LibRM_DrawBitmap(PPARAMETER_BITMAP pParamBitmap);

#endif