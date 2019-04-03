// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ReadObjModelZK.cpp,v 1.3 2009/09/18 02:07:30 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/09/18 02:07:30 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libRenderModel/ReadObjModelZK.cpp,v $
/////////////////////////////////////////////////////////////////////////

#ifdef _USE_OPENGLES
#include "ReadObjModelZK.h"
#include "OpenGLProcessZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReadObjModelZK::CReadObjModelZK()
{
	m_nNowObjCount		= 0;
	memset(m_stuObj3DList, 0, sizeof(m_stuObj3DList));
	m_pfTrangel			= NULL;
	m_nNowTrangelCount	= 0;
}

CReadObjModelZK::~CReadObjModelZK()
{
	Free();
}

bool CReadObjModelZK::Initialize(class COpenGLProcessZK* pOpenGL)
{
	m_nNowObjCount	= 0;
	memset(m_stuObj3DList, 0, sizeof(m_stuObj3DList));

	m_pfTrangel = new float[_ZK_MAX_TRANGEL_COUNT];
	if( NULL == m_pfTrangel )
	{
		return false;
	}
	m_nNowTrangelCount	= 0;
	m_pOpenGL	= pOpenGL;
	return true;
}

void CReadObjModelZK::Free()
{
	if( NULL != m_pfTrangel )
	{
		delete[] m_pfTrangel;
		m_pfTrangel = NULL;
	}
}

UINT CReadObjModelZK::AddModel(long x, long y, char *pszObjName)
{
	if( _ZK_OBJ_MAX_COUNT == m_nNowObjCount )
	{
		return 0;
	}
	m_stuObj3DList[m_nNowObjCount].nX	= x;
	m_stuObj3DList[m_nNowObjCount].nY	= y;
	++m_nNowObjCount;
	return 1;
}

bool CReadObjModelZK::ReadObjFile(const TCHAR* tzFieName)
{
	if( NULL == tzFieName /*|| _ZK_OBJ_MAX_COUNT == m_nNowObjCount*/ )
	{
		return false;
	}
	TCHAR tzFile[MAX_PATH];
	_tcscpy(tzFile, s_tzPath);
	_tcscat(tzFile, tzFieName);
	FILE*	pfObj = _tfopen( tzFile, TEXT("r") );
	if( NULL == pfObj )
	{
		return false;
	}
	//read .obj file... 
	char szLine[180];
//	float f1, f2, f3;
//	float fVList[1000][3];
	while( !feof(pfObj) && fgets(szLine, 180, pfObj) != NULL )
	{
		if( szLine[0] == 'v' )//vertex, normal or texture
		{
			if( szLine[1] == ' ' )//vertex data
			{
			}
			else if( szLine[1] == 't' )//texture data
			{
				;
			}
			else if( szLine[1] == 'n' )//normal data
			{
				;
			}
		}
		else if( szLine[0] == 'f')
		{
			;
		}
	}
	
	fclose(pfObj);
	return true;
}
#endif
