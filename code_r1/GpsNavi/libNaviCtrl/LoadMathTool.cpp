// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMathTool.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadMathTool.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LoadMathTool.h"

#ifdef _USE_MATHTOOL_DLL
#include "NuroDefine.h"
#else
#include "../libMathTool/libMathTool.h"
#endif

#ifdef _USE_MATHTOOL_LIB
#ifdef NURO_OS_LINUX
#else
#pragma comment(lib, "MathTool.lib")
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadMathTool::CLoadMathTool()
{
	m_hInst	= NULL;
}

CLoadMathTool::~CLoadMathTool()
{

}

nuBOOL CLoadMathTool::InitModule(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpInFS, nuPVOID lpOutMT)
{
	m_hInst = NULL;
#ifdef _USE_MATHTOOL_DLL
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuGetModulePath(NULL, sFile, NURO_MAX_PATH);
#ifdef NURO_OS_LINUX
	nuTcscat(sFile, nuTEXT("Setting\\libMathTool.so"));
#else
	nuTcscat(sFile, nuTEXT("Setting\\MathTool.dll"));
#endif
	m_hInst = nuLoadLibrary(sFile);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	MT_InitMathToolProc MT_InitMathTool = (MT_InitMathToolProc)nuGetProcAddress(m_hInst, "LibMT_InitMathTool");
	if( MT_InitMathTool == NULL || !MT_InitMathTool(lpInMM, lpInGDI, lpInFS, lpOutMT) )
	{
		return nuFALSE;
	}
#else
	m_hInst = (nuHINSTANCE)(-1);
	if( !LibMT_InitMathTool(lpInMM, lpInGDI, lpInFS, lpOutMT) )
	{
		return nuFALSE;
	}
#endif
	return nuTRUE;
}

nuVOID CLoadMathTool::UnLoadModule()
{
#ifdef _USE_MATHTOOL_DLL
	if( m_hInst != NULL )
	{
		MT_FreeMathToolProc MT_FreeMathTool = (MT_FreeMathToolProc)nuGetProcAddress(m_hInst, "LibMT_FreeMathTool");
		if( MT_FreeMathTool != NULL )
		{
			MT_FreeMathTool();
		}
		nuFreeLibrary(m_hInst);
	}
#else
	if ( m_hInst != NULL )
	{
		LibMT_FreeMathTool();
	}
#endif
	m_hInst = NULL;
}



