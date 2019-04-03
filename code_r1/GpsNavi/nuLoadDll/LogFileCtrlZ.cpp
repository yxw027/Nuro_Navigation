// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LogFileCtrlZ.cpp,v 1.2 2009/08/31 01:12:40 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/31 01:12:40 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/nuLoadDll/LogFileCtrlZ.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "LogFileCtrlZ.h"

#ifdef _WINDOWS
//#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFileCtrlZ::CLogFileCtrlZ()
{
	m_pLogFile = NULL;
}

CLogFileCtrlZ::~CLogFileCtrlZ()
{
	CloseLogFile();
}

nuVOID CLogFileCtrlZ::CloseLogFile()
{
	if( m_pLogFile != NULL )
	{
		nuFclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

nuVOID CLogFileCtrlZ::OpenLogFile(nuTCHAR *pTsFile)
{
	if( m_pLogFile == NULL )
	{
		nuTCHAR file[NURO_MAX_PATH];
		nuGetModulePath(NULL, file, NURO_MAX_PATH);
		nuTcscat(file, pTsFile);
		m_pLogFile = nuTfopen(file, NURO_FS_AB);
	}
}

nuVOID CLogFileCtrlZ::OutString(const nuTCHAR* pTsStr)
{
#ifdef _WINDOWS
	if( m_pLogFile != NULL )
	{
		_ftprintf((FILE*)m_pLogFile, nuTEXT("%s\r\n"), pTsStr);
//		fwprintf((nuFILE*)m_pLogFile, L"%s\r\n", pTsStr);
	}
#endif
}

nuVOID CLogFileCtrlZ::OutNumber(nuLONG nNum)
{
#ifdef _WINDOWS
	if( m_pLogFile != NULL )
	{
		fwprintf((FILE*)m_pLogFile, L"%d\r\n", nNum);
	}
#endif
}

nuVOID CLogFileCtrlZ::SaveTicks()
{
	m_nLastTicks = nuGetTickCount();
}

nuVOID CLogFileCtrlZ::OutTicksPast()
{
#ifdef _WINDOWS
	if( m_pLogFile != NULL )
	{
		fwprintf((FILE*)m_pLogFile, L"%d\r\n", nuGetTickCount()-m_nLastTicks);
	}
#endif
}

nuVOID CLogFileCtrlZ::ClearLofFile(nuTCHAR *pTsFile)
{
	if( m_pLogFile == NULL )
	{
		nuTCHAR file[NURO_MAX_PATH];
		#ifndef ANDROID		
		GetModuleFileName(NULL, file, NURO_MAX_PATH);
		for( nuINT i = nuTcslen(file) - 1; i >= 0; i-- )
		{
			if( file[i] == '\\' )
			{
				file[i+1] = '\0';
				break;
			}
		}
		#endif
		nuTcscat(file, pTsFile);
		m_pLogFile = nuTfopen(file, NURO_FS_WB);
	}
}
