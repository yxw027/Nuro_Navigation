// GpsLogFileZ.cpp: implementation of the CGpsLogFileZ class.
//
//////////////////////////////////////////////////////////////////////

#include "GpsLogFileZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpsLogFileZ::CGpsLogFileZ()
{
	if(!nuReadConfigValue("VENDERTYPE", &m_nVenderType))
	{
		m_nVenderType = 0;
	}
}

CGpsLogFileZ::~CGpsLogFileZ()
{

}
bool CGpsLogFileZ::AppendGpsData(char *psGpsData, int nLen)
{
	if ( psGpsData == NULL || nLen == 0  )
	{
		return false;
	}
	nuFILE *fpLogFile = NULL;

	if(m_nVenderType == _VENDER_ELEAD)
	{
		fpLogFile = nuTfopen(nuTEXT("\\Staticstore\\Navi\\GPSLOG.TXT"), NURO_FS_AB);
	}
	else if(fpLogFile == NULL)
	{
		nuMemset( m_sFilePath, 0, sizeof(m_sFilePath) );
		#ifndef ANDROID 	
		GetModuleFileName(NULL, m_sFilePath, NURO_MAX_PATH);
		#else
		nuGetModulePathA(NULL, m_sFilePath, NURO_MAX_PATH);
		#endif
			
		for(int i = nuTcslen(m_sFilePath) - 1; i >= 0; i--)
		{
			if( m_sFilePath[i] == nuTEXT('\\') )
			{
				m_sFilePath[i+1] = nuTEXT('\0');
				break;
			}
		}
		nuTcscat( m_sFilePath, nuTEXT("GPSLOG.TXT"));
		fpLogFile = nuTfopen(m_sFilePath,NURO_FS_AB);
	}
	if ( fpLogFile == NULL )
	{
		return false;
	}
	if ( nuFwrite( psGpsData, nLen, 1,fpLogFile ) != 1)
	{
		nuFclose( fpLogFile );
		fpLogFile=NULL;
		return false;
	}
	nuFclose( fpLogFile );
	fpLogFile=NULL;
	return true;
}

bool CGpsLogFileZ::InitFile(bool bDelte)
{
	//TCHAR sFilePath[NURO_MAX_PATH];
	nuMemset( m_sFilePath, 0, sizeof(m_sFilePath) );
	#ifndef ANDROID	
	GetModuleFileName(NULL, m_sFilePath, NURO_MAX_PATH);
	for(int i = nuTcslen(m_sFilePath) - 1; i >= 0; i--)
	{
		if( m_sFilePath[i] == nuTEXT('\\') )
		{
			m_sFilePath[i+1] = nuTEXT('\0');
			break;
		}
	}
	#endif
	nuTcscat( m_sFilePath, nuTEXT("GPSLOG.TXT"));
	nuFILE *fpLogFile = nuTfopen(m_sFilePath,NURO_FS_RB);
	if ( (fpLogFile && bDelte)
		|| !fpLogFile)
	{
		if( fpLogFile )
		{
			nuFclose( fpLogFile );
			fpLogFile = NULL;
		}
		fpLogFile =nuTfopen(m_sFilePath,NURO_FS_WB);
		if ( !fpLogFile )
		{
			return false;
		}
		nuFclose( fpLogFile );
		fpLogFile = NULL;
	}
	if( fpLogFile )
	{
		nuFclose( fpLogFile );
		fpLogFile = NULL;
	}
   return true;
}
