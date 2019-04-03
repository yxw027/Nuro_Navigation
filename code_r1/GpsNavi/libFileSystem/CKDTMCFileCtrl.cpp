#include "CKDTMCFileCtrl.h"
#include <stdio.h>
#include <string.h>

CKDTMCFileCtrl::CKDTMCFileCtrl()
{
	nuMemset(&TMCStru,NULL,sizeof(KDTMCFileStruct));
	TotalNodeDataCount = 0;
	TotalNodeDataCount2 = 0;
	bReleaseTMCFileOK = nuFALSE;
}

CKDTMCFileCtrl::~CKDTMCFileCtrl()
{
	ReleaseTMCStru();
}

nuBOOL CKDTMCFileCtrl::ReadTMCFile(nuTCHAR *tsFilePath)
{
	/*while(!m_bUseTMC)
	{
		nuSleep(50);
	}
	m_bUseTMC = nuTRUE;*/
	nuFILE *pfKDTMC = NULL;
	pfKDTMC = nuTfopen(tsFilePath,NURO_FS_RB);
	if(pfKDTMC!=NULL)
	{
		//Read KDTMC Header
		if(1!=nuFread(&(TMCStru.KDTMCHeader),sizeof(KDTMCFileHeaderStruct),1,pfKDTMC))
		{
			nuFclose(pfKDTMC);
			pfKDTMC = NULL;
			ReleaseTMCStru();
			m_bUseTMC = nuFALSE;
			return nuFALSE;
		}

		//Read KDTMC Main
		TMCStru.pKDTMCMain = new KDTMCFileMainDataStruct[TMCStru.KDTMCHeader.DataCnt];
		if(TMCStru.pKDTMCMain==NULL)
		{
			nuFclose(pfKDTMC);
			pfKDTMC = NULL;
			ReleaseTMCStru();
			m_bUseTMC = nuFALSE;
			return nuFALSE;
		}
		nuFseek(pfKDTMC,100, NURO_SEEK_SET);
		if(TMCStru.KDTMCHeader.DataCnt!=nuFread(TMCStru.pKDTMCMain,sizeof(KDTMCFileMainDataStruct),TMCStru.KDTMCHeader.DataCnt,pfKDTMC))
		{
			nuFclose(pfKDTMC);
			pfKDTMC = NULL;
			ReleaseTMCStru();
			m_bUseTMC = nuFALSE;
			return nuFALSE;
		}

		//Read KDTMC Node
		TotalNodeDataCount = 0;
		for(int i=0;i<TMCStru.KDTMCHeader.DataCnt;i++)
		{
			TotalNodeDataCount+=TMCStru.pKDTMCMain[i].LocationCodeNum;
			//將原紀錄"檔案位置"的資料轉換為"Index"
			TMCStru.pKDTMCMain[i].LocationVertexAddr=(TMCStru.pKDTMCMain[i].LocationVertexAddr-TMCStru.KDTMCHeader.NodeDataAddr)/sizeof(KDTMCFileNodeIDDataStruct);
		}
		TMCStru.pKDTMCNode = new KDTMCFileNodeIDDataStruct[TotalNodeDataCount];
		if(TMCStru.pKDTMCNode==NULL)
		{
			nuFclose(pfKDTMC);
			pfKDTMC = NULL;
			ReleaseTMCStru();
			m_bUseTMC = nuFALSE;
			return nuFALSE;
		}
		nuFseek(pfKDTMC,TMCStru.KDTMCHeader.NodeDataAddr, NURO_SEEK_SET);
		if(TotalNodeDataCount!=nuFread(TMCStru.pKDTMCNode,sizeof(KDTMCFileNodeIDDataStruct),TotalNodeDataCount,pfKDTMC))
		{
			nuFclose(pfKDTMC);
			pfKDTMC = NULL;
			ReleaseTMCStru();
			m_bUseTMC = nuFALSE;
			return nuFALSE;
		}

		nuFclose(pfKDTMC);
		pfKDTMC = NULL;
		m_bUseTMC = nuFALSE;
		return nuTRUE;
	}
	//m_bUseTMC = nuFALSE;
	return nuFALSE;
}
nuVOID CKDTMCFileCtrl::CopyTMCStru()
{
	/*while(!m_bUseTMC)
	{
		nuSleep(50);
	}
	m_bUseTMC = nuTRUE;*/
	ReleaseTMCStru2();
	memcpy(&TMCStru2.KDTMCHeader, &TMCStru.KDTMCHeader, sizeof(KDTMCFileHeaderStruct));
	TMCStru2.pKDTMCMain = new KDTMCFileMainDataStruct[TMCStru.KDTMCHeader.DataCnt];
	memcpy(TMCStru2.pKDTMCMain, TMCStru.pKDTMCMain, sizeof(KDTMCFileMainDataStruct) * TMCStru.KDTMCHeader.DataCnt);
	TMCStru2.pKDTMCNode = new KDTMCFileNodeIDDataStruct[TotalNodeDataCount];
	memcpy(TMCStru2.pKDTMCNode, TMCStru.pKDTMCNode, sizeof(KDTMCFileNodeIDDataStruct) * TotalNodeDataCount);
	TotalNodeDataCount2 = TotalNodeDataCount;
	//m_bUseTMC = nuFALSE;
}
nuVOID CKDTMCFileCtrl::ReleaseTMCStru2()
{
	if(TMCStru2.pKDTMCNode)
	{
		delete [] TMCStru2.pKDTMCNode;
		TMCStru2.pKDTMCNode = NULL;
	}
	if(TMCStru2.pKDTMCMain)
	{
		delete [] TMCStru2.pKDTMCMain;
		TMCStru2.pKDTMCMain = NULL;
	}
	nuMemset(&TMCStru2,NULL,sizeof(KDTMCFileStruct));
	TotalNodeDataCount2=0;
}
nuBOOL CKDTMCFileCtrl::ReleaseTMCStru()
{
	if(TMCStru.pKDTMCNode)
	{
		delete [] TMCStru.pKDTMCNode;
		TMCStru.pKDTMCNode = NULL;
		bReleaseTMCFileOK = nuTRUE;
	}
	if(TMCStru.pKDTMCMain)
	{
		delete [] TMCStru.pKDTMCMain;
		TMCStru.pKDTMCMain = NULL;
		bReleaseTMCFileOK = nuTRUE;
	}
	nuMemset(&TMCStru,NULL,sizeof(KDTMCFileStruct));

	TotalNodeDataCount=0;
	return bReleaseTMCFileOK;
}


