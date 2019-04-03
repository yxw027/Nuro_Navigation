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

#include "NuroDefine.h"
#include "NuroRenderModel.h"
#include "NuroModuleApiStruct.h"
//it must be after nuDefine.h
#ifdef _USRDLL
#ifdef RENDERMODEL_EXPORTS
#define RENDERMODEL_API extern "C" __declspec(dllexport)
#else
#define RENDERMODEL_API extern "C" __declspec(dllimport)
#endif
#else
#define RENDERMODEL_API extern "C" 
#endif

RENDERMODEL_API nuBOOL	LibRM_InitRenderModel(PAPI_RENDERMODEL pApiRM, nuPVOID pApiForRM);
RENDERMODEL_API nuVOID	LibRM_FreeRenderModel();

RENDERMODEL_API nuUINT	LibRM_StartRenderModel(PPARAMETER_START_OPTION pParamStart);
RENDERMODEL_API nuUINT	LibRM_AddModel( nuLONG nID, nuLONG x, nuLONG y, char *pszModelName, nuLONG rx, nuLONG ry, 
									    nuLONG ty, nuINT height, nuINT dCenD, nuFLOAT fs, nuFLOAT fz, PMATHTOOLAPI pMT );
RENDERMODEL_API nuUINT	LibRM_DelModel(nuLONG nID, nuLONG x, nuLONG y);
RENDERMODEL_API nuUINT	LibRM_SetViewOption(PPARAMETER_VIEW_OPTION pParamView);
RENDERMODEL_API nuUINT	LibRM_DrawBitmap(PPARAMETER_BITMAP pParamBitmap);

#endif
