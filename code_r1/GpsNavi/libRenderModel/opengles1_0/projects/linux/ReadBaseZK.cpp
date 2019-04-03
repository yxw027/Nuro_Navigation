// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ReadBaseZK.cpp,v 1.3 2009/11/25 08:15:24 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/11/25 08:15:24 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libRenderModel/ReadBaseZK.cpp,v $
/////////////////////////////////////////////////////////////////////////

#ifdef _USE_OPENGLES
#include "ReadBaseZK.h"
#include "nuApiInterFace.h"
#include "nuClib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const TCHAR _ZK_C_3D_MODEL_FOLDER[] = nuTEXT("MODEL/");
TCHAR CReadBaseZK::s_tzPath[];

CReadBaseZK::CReadBaseZK()
{

}

CReadBaseZK::~CReadBaseZK()
{

}

void CReadBaseZK::InitStatic()
{
	nuGetModuleFileName(NULL, s_tzPath, _ZK_3D_MODEL_FOLDER_PATH_NM);
	for(int i = nuTcslen(s_tzPath) - 1; i >= 0; i--)
	{
		if( s_tzPath[i] == '\\' )
		{
			s_tzPath[i+1] = '\0';
			break;
		}
	}
	nuTcscat(s_tzPath, _ZK_C_3D_MODEL_FOLDER);
}

#endif
