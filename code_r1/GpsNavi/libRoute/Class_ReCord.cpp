
#include "Class_ReCord.h"
//#include "CDontToHere.h"
#include "NuroModuleApiStruct.h"
#include "RouteBase.h"
#include "AllSystemDataBase.h"

nuBOOL ReCord::ReCord_Write(ROUTINGDATA *NaviData, nuLONG RunDataNumber)
{
	/*nuBOOL bNRBCFirst = nuTRUE, bNFTCFirst = nuTRUE;
	NAVIRTBLOCKCLASS *tempNRBC = NULL;
	NAVIFROMTOCLASS *tempNFTC  = NULL;
	if(NaviData == NULL || NaviData->NFTC == NULL || NaviData->NFTCCount == NULL)
	{
		return nuFALSE;
	}
	
	nuTCHAR  Path[100] = {0};
	ReCord_Path(RunDataNumber, Path);

	ReCording = nuTfopen(Path,NURO_FS_WB);
	if(ReCording == NULL)
	{
		return nuFALSE;
	}
	ROUTINGDATA *routingdata = NULL;
	routingdata = NaviData;
	nuFwrite(routingdata,sizeof(ROUTINGDATA),1,ReCording);
	while(routingdata->NFTC != NULL)
	{
		if(1 != nuFwrite(routingdata->NFTC,sizeof(NAVIFROMTOCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
		if(1 != nuFwrite(&routingdata->NFTC->FCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
		if(1 != nuFwrite(&routingdata->NFTC->TCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}

		while(routingdata->NFTC->NRBC)
		{
			if(1 != nuFwrite(routingdata->NFTC->NRBC,sizeof(NAVIRTBLOCKCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFwrite(&routingdata->NFTC->NRBC->FInfo,sizeof(PTFIXTOROADSTRU),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFwrite(&routingdata->NFTC->NRBC->FInfo.FixCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(routingdata->NFTC->NRBC->NodeIDCount != nuFwrite(routingdata->NFTC->NRBC->NodeIDList,sizeof(nuDWORD),routingdata->NFTC->NRBC->NodeIDCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFwrite(&routingdata->NFTC->NRBC->TInfo,sizeof(PTFIXTOROADSTRU),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFwrite(&routingdata->NFTC->NRBC->TInfo.FixCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}

			for(nuULONG k = 0; k < routingdata->NFTC->NRBC->NSSCount; k++)
			{
				if(1 != nuFwrite(&routingdata->NFTC->NRBC->NSS[k],sizeof(NAVISUBCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(routingdata->NFTC->NRBC->NSS[k].ArcPtCount != nuFwrite(routingdata->NFTC->NRBC->NSS[k].ArcPt,sizeof(NUROPOINT),routingdata->NFTC->NRBC->NSS[k].ArcPtCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFwrite(&routingdata->NFTC->NRBC->NSS[k].Bound,sizeof(NURORECT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFwrite(&routingdata->NFTC->NRBC->NSS[k].EndCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(routingdata->NFTC->NRBC->NSS[k].CrossCount != nuFwrite(routingdata->NFTC->NRBC->NSS[k].NCC,sizeof(NAVICROSSCLASS),routingdata->NFTC->NRBC->NSS[k].CrossCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFwrite(&routingdata->NFTC->NRBC->NSS[k].StartCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}	
			}
			if(bNRBCFirst == nuTRUE)
			{
				tempNRBC = routingdata->NFTC->NRBC;
				bNRBCFirst = nuFALSE;
			}
			routingdata->NFTC->NRBC = routingdata->NFTC->NRBC->NextRTB;				
		}
		if(bNFTCFirst == nuTRUE)
		{
			tempNFTC = routingdata->NFTC;
			bNFTCFirst = nuFALSE;
		}
		routingdata->NFTC = routingdata->NFTC->NextFT;
	}
	nuFclose(ReCording);
	NaviData->NFTC = tempNFTC;
	NaviData->NFTC->NRBC = tempNRBC;*/
	nuTCHAR  Path[100] = {0};
	ReCord_Path(RunDataNumber, Path);

	ReCording = nuTfopen(Path,NURO_FS_WB);
	if(ReCording == NULL)
	{
		return nuFALSE;
	}
	/*ROUTINGDATA *routingdata;
	routingdata = NaviData;
	nuFwrite(&routingdata->NFTC->FCoor,sizeof(NUROPOINT),1,ReCording);
	nuFwrite(&routingdata->NFTC->TCoor,sizeof(NUROPOINT),1,ReCording);*/
	nuFclose(ReCording);
	return nuTRUE;
}

nuBOOL ReCord::ReCord_Read(ROUTINGDATA *NaviData, nuLONG RunDataNumber) 
{
/*	NAVIFROMTOCLASS *tempNFTC = NULL;
	NAVIFROMTOCLASS *NFTC[5]  = {NULL};
	NAVIRTBLOCKCLASS *NRBC[100] = {NULL};

	nuLONG NFTCCounter = 0;
	nuLONG NRBCCounter = 0;
	routingdata = new ROUTINGDATA;
	if(routingdata == NULL){return nuFALSE;}
	nuBOOL bFirstNRBC = nuTRUE; 

	nuTCHAR  Path[100] = {0};
	ReCord_Path(RunDataNumber, Path);

	ReCording = nuTfopen(Path,NURO_FS_RB);
	if(ReCording == NULL)
	{
		return nuFALSE;
	}
	if(1 != nuFread(routingdata,sizeof(ROUTINGDATA),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
	nuLONG j = 0;
	for( j = 0 ; j < routingdata->NFTCCount; j++)//?إ߸??ưO????
	{
		//?إ߸??Ƶ??c  Create NFTC Structure
		if(!g_pCRDC->Create_NaviFromToClass(tempNFTC)){nuFclose(ReCording);	return nuFALSE;	}
		NFTC[j] = tempNFTC;
		tempNFTC = tempNFTC->NextFT;
	} 
	do{	
		routingdata->NFTC = NFTC[NFTCCounter];
		if(1 != nuFread(routingdata->NFTC,sizeof(NAVIFROMTOCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
		if(1 != nuFread(&routingdata->NFTC->FCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
		if(1 != nuFread(&routingdata->NFTC->TCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
		while(routingdata->NFTC->NRBC != NULL)
		{
			//?إ߰϶�c	Create NRBC Structure 
			routingdata->NFTC->NRBC = NULL;
			if(!g_pCRDC->Create_NaviRtBlockClass(routingdata->NFTC->NRBC)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFread(routingdata->NFTC->NRBC,sizeof(NAVIRTBLOCKCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFread(&routingdata->NFTC->NRBC->FInfo,sizeof(PTFIXTOROADSTRU),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFread(&routingdata->NFTC->NRBC->FInfo.FixCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			//?إ?NodeID?}?C Create NodeID Array 
			if(routingdata->NFTC->NRBC->NSSCount != NULL)
			{
				if(!g_pCRDC->Create_NodeIDList(routingdata->NFTC->NRBC, routingdata->NFTC->NRBC->NodeIDCount)){nuFclose(ReCording); return nuFALSE;}
			}
			if(routingdata->NFTC->NRBC->NodeIDCount != nuFread(routingdata->NFTC->NRBC->NodeIDList,sizeof(nuDWORD),routingdata->NFTC->NRBC->NodeIDCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFread(&routingdata->NFTC->NRBC->TInfo,sizeof(PTFIXTOROADSTRU),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			if(1 != nuFread(&routingdata->NFTC->NRBC->TInfo.FixCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			//?إ߹D?�c  Create NSS Array Structure
			routingdata->NFTC->NRBC->NSS = NULL;
			if(routingdata->NFTC->NRBC->NSSCount != NULL)
			{
				if(!g_pCRDC->Create_NaviSubClass(routingdata->NFTC->NRBC,routingdata->NFTC->NRBC->NSSCount)){nuFclose(ReCording); return nuFALSE;}
			}
			for(nuULONG k = 0; k < routingdata->NFTC->NRBC->NSSCount; k++)
			{
				if(1 != nuFread(&routingdata->NFTC->NRBC->NSS[k],sizeof(NAVISUBCLASS),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				//?إ߮y?а}?C  Create Coordinate Array
				routingdata->NFTC->NRBC->NSS[k].ArcPt = NULL;
				if(!g_pCRDC->Create_ArcPt(routingdata->NFTC->NRBC->NSS[k],routingdata->NFTC->NRBC->NSS[k].ArcPtCount)){nuFclose(ReCording); return nuFALSE;}
				if(routingdata->NFTC->NRBC->NSS[k].ArcPtCount != nuFread(routingdata->NFTC->NRBC->NSS[k].ArcPt,sizeof(NUROPOINT),routingdata->NFTC->NRBC->NSS[k].ArcPtCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFread(&routingdata->NFTC->NRBC->NSS[k].Bound,sizeof(NURORECT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFread(&routingdata->NFTC->NRBC->NSS[k].EndCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
				//?إߧø??}?C???c Create NCC Array Structure
				routingdata->NFTC->NRBC->NSS[k].NCC = NULL;
				if(!g_pCRDC->Create_CrossData(routingdata->NFTC->NRBC->NSS[k],routingdata->NFTC->NRBC->NSS[k].CrossCount)){nuFclose(ReCording); return nuFALSE;}
				if(routingdata->NFTC->NRBC->NSS[k].CrossCount != nuFread(routingdata->NFTC->NRBC->NSS[k].NCC,sizeof(NAVICROSSCLASS),routingdata->NFTC->NRBC->NSS[k].CrossCount,ReCording)){nuFclose(ReCording); return nuFALSE;}
				if(1 != nuFread(&routingdata->NFTC->NRBC->NSS[k].StartCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
			}
			NRBC[NRBCCounter] = routingdata->NFTC->NRBC;
			NRBCCounter++;
			routingdata->NFTC->NRBC = routingdata->NFTC->NRBC->NextRTB;
		}
		NFTC[NFTCCounter] = routingdata->NFTC;
		NFTCCounter++;
		routingdata->NFTC = routingdata->NFTC->NextFT;
	}while(routingdata->NFTC != NULL);
	nuFclose(ReCording);
	ReCording = NULL;

	for(j = 0; j + 1 < NFTCCounter; j++)
	{
		NFTC[j]->NextFT  = NFTC[j + 1];
	}

	for(j = 0; j + 1 < NRBCCounter; j++)
	{
		NRBC[j]->NextRTB = NRBC[j + 1];
	}
	NaviData->NFTCCount = routingdata->NFTCCount;
	NaviData->NFTC = NFTC[0];
	NaviData->NFTC->NRBC = NRBC[0];
	
	delete routingdata;
	routingdata = NULL;*/
	return nuTRUE;
}
nuVOID ReCord::ReCord_Path(nuLONG RunDataNumber, nuTCHAR *FilePath)
{
	nuTCHAR  Path[100] = {0};
	nuTCHAR  datapath[15] = {'U','I','d','a','t','a','\\','\0'};
	nuTCHAR  DemoNumber[3] = {0};
	nuTCHAR  DemoName[5] = {'D','e','m','o','\0'};
	nuGetModulePath(NULL, Path, NURO_MAX_PATH);
	DemoNumber[2] = '\0';
	DemoNumber[1] = 0x30 + RunDataNumber;
	DemoNumber[0] = '_';
	nuTcscat( Path, datapath);
	nuTcscat( Path, DemoName);
	nuTcscat( Path, DemoNumber);
	nuTcscat( Path, nuTEXT(".dat"));
	nuTcscpy( FilePath, Path);
}

nuBOOL ReCord::ReCord_ReadCoor(NUROPOINT *FCoor, NUROPOINT*TCoor, nuLONG RunDataNumber)
{
	/*nuTCHAR  Path[100] = {0};
	ReCord_Path(RunDataNumber,Path);
	ReCording = nuTfopen(Path,NURO_FS_RB);
	if(ReCording == NULL)
	{
		return nuFALSE;
	}
	
	if(NULL != nuFseek(ReCording,sizeof(NAVIFROMTOCLASS) + sizeof(ROUTINGDATA),NURO_SEEK_SET)){nuFclose(ReCording); return nuFALSE;}
	if(1 != nuFread(FCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
	if(1 != nuFread(TCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
	nuFclose(ReCording);*/

	nuTCHAR  Path[100] = {0};
	ReCord_Path(RunDataNumber,Path);
	ReCording = nuTfopen(Path,NURO_FS_RB);
	if(ReCording == NULL)
	{
		return nuFALSE;
	}
	
	if(1 != nuFread(FCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
	if(1 != nuFread(TCoor,sizeof(NUROPOINT),1,ReCording)){nuFclose(ReCording); return nuFALSE;}
	//nuFwrite(FCoor,sizeof(NUROPOINT),1,ReCording);
	//nuFwrite(TCoor,sizeof(NUROPOINT),1,ReCording);
	nuFclose(ReCording);
	return nuTRUE;
}
