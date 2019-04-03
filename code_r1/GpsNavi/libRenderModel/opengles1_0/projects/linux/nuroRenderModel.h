// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: nuroRenderModel.h,v 1.10 2010/04/13 09:16:50 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/13 09:16:50 $
// $Locker:  $
// $Revision: 1.10 $
// $Source: /home/nuCode/libRenderModel/nuroRenderModel.h,v $
/////////////////////////////////////////////////////////////////////////
#ifndef NURO_STRUCT_RENDER_MODEL_20090820
#define NURO_STRUCT_RENDER_MODEL_20090820

#include "nuDefine.h"
#include "nuroModuleApiStruct.h"

typedef struct tagPARAMETER_START_OPTION
{
	nuHINSTANCE	hInst;
	nuHWND		hWnd;
	nuHDC		hDC;
	nuPBYTE		pMemory;
	nuroRECT	rtScreen;
}PARAMETER_START_OPTION, *PPARAMETER_START_OPTION;
typedef nuUINT(*RM_StartRenderModelProc)(PPARAMETER_START_OPTION pParamStart);
typedef nuUINT(*RM_AddModelProc)(long nID, long x, long y, char *pszModelName
                                 , long rx, long ry, long ty, int height, int dCenD, float fs, float fz
                                 , PMATHTOOLAPI pMT);
typedef nuUINT(*RM_DelModelProc)(long nID, long x, long y);
//...
typedef struct tagPARAMETER_VIEW_OPTION
{
	float		fLookAtX;
	float		fLookAtY;
	float		fLookAtZ;
	float		fLookToX;
	float		fLookToY;
	float		fLookToZ;
	float		fUpX;
	float		fUpY;
	float		fUpZ;
	float		fScale;
	float		fAngle;
    long        screenW;
    long        screenH;
}PARAMETER_VIEW_OPTION, *PPARAMETER_VIEW_OPTION;
typedef nuUINT(*RM_SetViewOptionProc)(PPARAMETER_VIEW_OPTION pParamView);
typedef struct tagPARAMETER_BITMAP
{
	nuWORD		nWidth;
	nuWORD		nHeight;
	nuBYTE		nType;
	long		nBuffLen;
	nuPBYTE		pBitsBuff;
}PARAMETER_BITMAP, *PPARAMETER_BITMAP;
typedef nuUINT(*RM_DrawBitmapProc)(PPARAMETER_BITMAP pParamBitmap);

typedef struct tagAPI_RENDERMODEL
{
	RM_StartRenderModelProc		RM_StartRenderModel;
	RM_AddModelProc				RM_AddModel;
	RM_DelModelProc				RM_DelModel;
	//...
	RM_SetViewOptionProc		RM_SetViewOption;
	RM_DrawBitmapProc			RM_DrawBitmap;
}API_RENDERMODEL, *PAPI_RENDERMODEL;


typedef struct tagAPI_FOR_RM
{
	nuPVOID		lpVoid;
}API_FOR_RM, *PAPI_FOR_RM;

#endif