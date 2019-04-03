#include "stdAfxRoute.h"
#include "Ctrl_CMP.h"


	CCMPCtrl::CCMPCtrl()
	{
		InitClass();
	}
	CCMPCtrl::~CCMPCtrl()
	{
		CloseClass();
	}
	nuBOOL CCMPCtrl::InitClass()
	{
		m_pCMPFile = NULL;
		CMPHeader.DataCnt = 0;
		pCMPMainData = NULL;
		nuMemset(CMPHeader.Reserve, 0, sizeof(nuCHAR) * 196);		
		return nuTRUE;
	}
	nuVOID CCMPCtrl::CloseClass()
	{
		Release();
	}
	nuBOOL CCMPCtrl::ReadFile()
	{	
		nuTCHAR	FName[NURO_MAX_PATH] = {0};
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		g_pRtFSApi->FS_FindFileWholePath(-1,tsFile,NURO_MAX_PATH);
		for(nuINT i = nuTcslen(tsFile); i >= 0; i--)
		{
			if(tsFile[i] == 0x5c)
			{
				tsFile[i+1] = 0;
				break;
			}
		}
		nuTcscat(tsFile, nuTEXT("China.CMP"));
		nuTcscpy(FName, tsFile);
		m_pCMPFile  = nuTfopen(FName,NURO_FS_RB);
		if(m_pCMPFile == NULL)
		{
			return nuFALSE;
		}
		if(1 != nuFread(&(CMPHeader),sizeof(CMPHeaderStru),1,m_pCMPFile))
		{	
			Release();	
			return nuFALSE;	
		}
		pCMPMainData = new CMPMainDataStru[CMPHeader.DataCnt];
		if(pCMPMainData == NULL)
		{
			Release();	
			return nuFALSE;
		}
		nuMemset(pCMPMainData, 0, sizeof(CMPMainDataStru) * CMPHeader.DataCnt);
		if(CMPHeader.DataCnt != nuFread(pCMPMainData,sizeof(CMPMainDataStru),CMPHeader.DataCnt,m_pCMPFile))
		{
			Release();
			return nuFALSE;
		}
		//nuFclose(m_pCMPFile);
		//m_pCMPFile = NULL;
		return nuTRUE;
	}
	nuVOID CCMPCtrl::Release()
	{
		if(m_pCMPFile != NULL)
		{
			nuFclose(m_pCMPFile);
			m_pCMPFile = NULL;
		}
		if(pCMPMainData != NULL)
		{
			delete [] pCMPMainData;
			pCMPMainData = NULL;
		}
	}