#include "CTMCServerFileCtrl.h"

CTMCServerFileCtrl::CTMCServerFileCtrl()
{
	iServerTMCFileSize=0;
	pTMCServerData = NULL;
	bReleaseServerFileOK = nuFALSE;
}

CTMCServerFileCtrl::~CTMCServerFileCtrl()
{
	if(pTMCServerData)
	{
		delete [] pTMCServerData;
		pTMCServerData=NULL;
	}
	iServerTMCFileSize=0;
}

nuBOOL CTMCServerFileCtrl::ReadTMCServerFile(nuTCHAR *tsFilePath)
{
	nuFILE *pFTMC=NULL;
	iServerTMCFileSize=0;

	pFTMC=nuTfopen(tsFilePath,NURO_FS_RB);
	if(pFTMC==NULL)
	{
		return nuFALSE;
	}
	
	if(nuFseek(pFTMC, 0, NURO_SEEK_END))
	{
		nuFclose(pFTMC);
		pFTMC=NULL;
		return nuFALSE;
	}
	//取得檔案大小 單位byte
	iServerTMCFileSize = nuFtell(pFTMC);
	//移動指標回檔案起始

	nuFseek(pFTMC,0, NURO_SEEK_SET);
	pTMCServerData=new nuBYTE[iServerTMCFileSize];
	if(pTMCServerData==NULL)
	{
		iServerTMCFileSize=0;
		nuFclose(pFTMC);
		pFTMC=NULL;
		return nuFALSE;
	}
	nuMemset(pTMCServerData,0,sizeof(nuBYTE)*iServerTMCFileSize);

	if(iServerTMCFileSize!=nuFread(pTMCServerData,1,iServerTMCFileSize,pFTMC))
	{
		iServerTMCFileSize=0;
		delete [] pTMCServerData;
		pTMCServerData=NULL;
		nuFclose(pFTMC);
		pFTMC=NULL;
		return nuFALSE;
	}

	nuFclose(pFTMC);
	pFTMC=NULL;
	return nuTRUE;
}
nuBOOL CTMCServerFileCtrl::ReleaseServerFileStru()
{
	if(pTMCServerData)
	{
		delete [] pTMCServerData;
		pTMCServerData = NULL;
		bReleaseServerFileOK = nuTRUE;
	}
    return bReleaseServerFileOK;
}

