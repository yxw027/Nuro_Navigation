#include "FileMapTMCLU.h"

#define MAXVDNUM	NURO_MAX_FILE_PATH*2
#define MAXVDEVENTNUM	NURO_MAX_FILE_PATH*2
CFileMapTMCLU cTMC;
#define FILE_XMLW  _T("//My Documents//traffic_way.txt")

CFileMapTMCLU::CFileMapTMCLU(nuVOID)
{
	bVDInit=nuFALSE;

	nVDCount=0;
	pVDData = NULL;
	pVDData = new UNITVD[MAXVDNUM];

	nVDEventCount=0;
	pVDevent = NULL;
	pVDevent = new EVENTVD[MAXVDEVENTNUM];

	if(pVDData && pVDevent)
	{
		bVDInit=nuTRUE;
		nuMemset(pVDData,NULL,sizeof(UNITVD)*MAXVDNUM);
		nuMemset(pVDevent,NULL,sizeof(EVENTVD)*MAXVDEVENTNUM);
		ReadVD();
	}
}

CFileMapTMCLU::~CFileMapTMCLU(nuVOID)
{
	nVDCount=0;
	if(pVDData)
	{
		delete [] pVDData;
		pVDData=NULL;
	}
	nVDEventCount=0;
	if(pVDevent)
	{
		delete [] pVDevent;
		pVDevent=NULL;
	}
}

nuBOOL CFileMapTMCLU::ReadVD()
{
	nuFILE * pFile;
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuCHAR TmpWord;
	nuCHAR TempStr[NURO_MAX_PATH*2];
	nuCHAR *pChr1;
	nuCHAR *pChr2;

	nuLONG lLen;
	nuBOOL bStart;

	if(!bVDInit)
	{
		return nuFALSE;
	}

	nVDCount	=	0;//總數重記
	nuMemset(pVDData,NULL,sizeof(UNITVD)*MAXVDNUM);

	nVDEventCount=0;
	nuMemset(pVDevent,NULL,sizeof(EVENTVD)*MAXVDEVENTNUM);

	lLen=0;
	bStart=nuFALSE;

	//Get Data File Name

	
	#ifdef WINCE
		nuTcscpy(tsFile,FILE_XMLW);
	#else
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile, FILE_CSC_VD);
	#endif

	//Open VD File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		switch(TmpWord)
		{
			case '<':
				bStart=nuTRUE;
				lLen=0;
				break;
			case '>':
				bStart=nuFALSE;
				break;
			default:
				if(bStart)
				{
					if(lLen<((NURO_MAX_PATH*2)-1))
					{
						TempStr[lLen]=TmpWord;
						lLen++;
					}
					else
					{
						bStart=nuFALSE;
						lLen=0;
					}
				}
				break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{
			if(nuStrstr(TempStr,"node enName"))//找到node描述子句
			{
				nVDCount++;
				if(nVDCount>MAXVDNUM)
				{
					nVDCount--;
					continue;
				}
				//取得ID
				pChr1=nuStrstr(TempStr,"id=");
				pChr2=nuStrstr(pChr1+4," ");
				if(pChr1==NULL || pChr2==NULL || pChr2-1-(pChr1+4)>16)
				{
					nVDCount--;
				}
				nuMemcpy(pVDData[nVDCount-1].VDID,pChr1+4,pChr2-1-(pChr1+4));
				//取得速度
				pChr1=nuStrstr(TempStr,"speed=");
				pChr2=nuStrstr(pChr1+7," ");
				if(pChr1==NULL || pChr2==NULL)
				{
					nVDCount--;
				}

				pVDData[nVDCount-1].VDSpeed=nuStrtol(pChr1+7,NULL,10);
				//取得名稱
				pChr1=nuStrstr(TempStr,"name=");
				pChr2=nuStrstr(pChr1+6," ");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+6)>32)
				{
					nVDCount--;
				}
				nuMemcpy(pVDData[nVDCount-1].VDName,pChr1+6,pChr2-1-(pChr1+6));
			}


			if(nuStrstr(TempStr,"rEvent happenTime"))//找到event描述子句
			{
				nVDEventCount++;
				if(nVDEventCount>MAXVDEVENTNUM)
				{
					nVDEventCount--;
					continue;
				}
				//取得事件描述
				pChr1=nuStrstr(TempStr,"status=");
				pChr2=nuStrstr(pChr1+8,"id");
				if(pChr1==NULL || pChr2==NULL || pChr2-2-(pChr1+8)>256)
				{
					nVDEventCount--;
				}
				nuMemcpy(pVDevent[nVDEventCount-1].Event,pChr1+8,pChr2-2-(pChr1+8));

				//取得事件ID
				pChr1=nuStrstr(TempStr,"id=");
				pChr2=nuStrstr(pChr1+6,"/");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+4)>32)
				{
					nVDEventCount--;
				}
				nuMemcpy(pVDevent[nVDEventCount-1].EventID,pChr1+4,pChr2-1-(pChr1+4));
			}
			

			if(nVDCount<=MAXVDNUM)
			{
//				if(nuStrstr(TempStr,"nEvent eventType=") && NULL==nuStrstr(TempStr,"PREWORK"))//找到事件描述子句 排除經常性工作
				if(nuStrstr(TempStr,"nEvent eventType=") )//找到事件描述子句 排除經常性工作
				{
					pVDData[nVDCount-1].EventCount++;
					switch(pVDData[nVDCount-1].EventCount)
					{
						case 1:
							pChr1=nuStrstr(TempStr,"id=");
							pChr2=nuStrstr(pChr1+4,"/");
							nuMemcpy(pVDData[nVDCount-1].Event1,pChr1+4,pChr2-1-(pChr1+4));
							break;
						case 2:
							pChr1=nuStrstr(TempStr,"id=");
							pChr2=nuStrstr(pChr1+4,"/");
							nuMemcpy(pVDData[nVDCount-1].Event2,pChr1+4,pChr2-1-(pChr1+4));
							break;
						case 3:
							pChr1=nuStrstr(TempStr,"id=");
							pChr2=nuStrstr(pChr1+4,"/");
							nuMemcpy(pVDData[nVDCount-1].Event3,pChr1+4,pChr2-1-(pChr1+4));
							break;
						case 4:
							pChr1=nuStrstr(TempStr,"id=");
							pChr2=nuStrstr(pChr1+4,"/");
							nuMemcpy(pVDData[nVDCount-1].Event4,pChr1+4,pChr2-1-(pChr1+4));
							break;
					}
					if(pVDData[nVDCount-1].EventCount>4)
					{
						pVDData[nVDCount-1].EventCount=4;
					}
				}
			}
			lLen=0;
		}
	}

	nuFclose(pFile);
	pFile=NULL;

UNITVD VD;
EVENTVD VDevent;
nuLONG k,l;
for(k=0;k<nVDCount;k++)
{
	GetVDUnit(k,VD);
	for(l=0;l<VD.EventCount;l++)
	{
		switch(l)
		{
			case 0:
				GetVDEVENT(VD.Event1,VDevent);
				break;
			case 1:
				GetVDEVENT(VD.Event2,VDevent);
				break;
			case 2:
				GetVDEVENT(VD.Event3,VDevent);
				break;
			case 3:
				GetVDEVENT(VD.Event4,VDevent);
				break;			
		}
	}
}

	return nuTRUE;
}

nuBOOL CFileMapTMCLU::CleanVDData()
{
	if(!bVDInit)
	{
		return nuFALSE;
	}
	nVDCount=0;
	if(pVDData)
	{
		delete [] pVDData;
		pVDData=NULL;
	}
	nVDEventCount=0;
	if(pVDevent)
	{
		delete [] pVDevent;
		pVDevent=NULL;
	}
	return nuTRUE;
}

nuLONG CFileMapTMCLU::GetVDCount()
{
	return nVDCount;
}

nuBOOL CFileMapTMCLU::GetVDUnit(nuLONG UnitIndex,UNITVD &VD)
{
	if(!bVDInit)
	{
		return nuFALSE;
	}
	if(0<=UnitIndex && UnitIndex<nVDCount)
	{
		nuMemcpy(&VD,&(pVDData[UnitIndex]),sizeof(UNITVD));
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CFileMapTMCLU::GetVDEVENT(nuCHAR *pEvent,EVENTVD &VDevent)
{
	nuLONG i;
	if(!bVDInit)
	{
		return nuFALSE;
	}
	if(pEvent==NULL)
	{
		return nuFALSE;
	}

	for(i=0;i<nVDEventCount;i++)
	{
		if(0==nuMemcmp(pVDevent[i].EventID,pEvent,nuStrlen(pVDevent[i].EventID)))
		{
			nuMemcpy(&VDevent,&(pVDevent[i]),sizeof(UNITVD));
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuBOOL CFileMapTMCLU::GetVD(nuVOID*pTmcBuf,const nuUINT size)
{
	nuMemcpy(pTmcBuf,pVDData,size);
	return nuTRUE;
}
