// Ctrl_Tmc.cpp: implementation of the CCtrl_Tmc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfxRoute.h"
#include "Ctrl_Tmc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl_Tmc::CCtrl_Tmc()
{
	InitClass();
#ifdef _DEBUG
	TmcAlocCount = 0;
	TmcFreeCount = 0;
#endif
}

CCtrl_Tmc::~CCtrl_Tmc()
{
#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Tmc_MemUesd %d\n", TmcAlocCount - TmcFreeCount);
			fclose(pfile);
		}
	#endif
#endif
	CloseClass();
}
nuBOOL CCtrl_Tmc::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	TMCItemB = NULL;
	TMCFile = NULL;
	return nuTRUE;
}
nuVOID CCtrl_Tmc::CloseClass()
{
	ReleaseStruct();
}
nuBOOL CCtrl_Tmc::ReadTMC(TMC_ROUTER_DATA *TMCPassData, nuSHORT TMCEventCount, nuBOOL *bTMCRTB)
{
/*	FILE *fileo = NULL, *filetest = NULL;
	fileo = fopen("D:\\TMCNodeID.txt","w");
	filetest = fopen("D:\\TMCNodeTest.txt","w");
	if(fileo == NULL || filetest == NULL)
	{
		return nuFALSE;
	}*/
	//nuMemset(FName, 0, sizeof(nuTCHAR) * 512);
	nuLONG starttime = nuGetTickCount();
	nuULONG TmcAddr = 0;
	nuLONG TMCEnventRoadCount = 0, i = 0, j = 0;
	nuLONG count = 0;
	TMCNodeIDCount = 0;
	TMCFile = nuTfopen(FName, NURO_FS_RB);
	if(TMCFile == NULL) {return nuFALSE;}
	
	for(i = 0; i < TMCEventCount; i++)
	{
		if(TMCPassData[i].bTMCPass)
		{
			TMCEnventRoadCount += TMCPassData[i].sTMCNodeCount + 1;//?[1?O?n?[TNodeID
		}
	}
	TMCNodeIDCount = TMCEnventRoadCount;
	m_ppTMCNodeIDList = (StructTMCNodeIDList**)g_pRtMMApi->MM_AllocMem(sizeof(StructTMCNodeIDList) * TMCEnventRoadCount);
	if(m_ppTMCNodeIDList == NULL)
	{
		return nuFALSE;
	}
	else
	{
#ifdef _DEBUG
		TmcAlocCount++;
#endif
	}
	for(i = 0; i < TMCEventCount; i++)
	{
		if(!TMCPassData[i].bTMCPass)
		{
			continue;
		}
		TMCItemB = (StructTMCItemB**)g_pRtMMApi->MM_AllocMem(sizeof(StructTMCItemB) * TMCPassData[i].sTMCNodeCount);
		//new StructTMCItemB[TMCPassData[i].TMCNodeCount];
		if(TMCItemB == NULL)
		{
			ReleaseStruct(); 
			return nuFALSE;
		}
		else
		{
			#ifdef _DEBUG
					TmcAlocCount++;
			#endif
		}
		if(NULL != nuFseek(TMCFile,TMCPassData[i].lTMCLocationAddr, NURO_SEEK_SET))
		{
			ReleaseStruct(); 
			return nuFALSE;
		}
		if(TMCPassData[i].sTMCNodeCount != nuFread(*TMCItemB,sizeof(StructTMCItemB),TMCPassData[i].sTMCNodeCount, TMCFile))
		{
			ReleaseStruct();
			return nuFALSE;
		}
		for(j = 0; j < TMCPassData[i].sTMCNodeCount; j++)
		{
			(*m_ppTMCNodeIDList)[count].Weighting = TMCPassData[i].sTMCRouteWeighting;
			(*m_ppTMCNodeIDList)[count].Time	  = TMCPassData[i].sTMCRouteTime;
			(*m_ppTMCNodeIDList)[count].NodeID    = (*TMCItemB)[j].FNodeID;
			(*m_ppTMCNodeIDList)[count].RTBID     = (*TMCItemB)[j].FRTBID;
			if((*m_ppTMCNodeIDList)[count].RTBID >= 20)
			{
				ReleaseStruct();
				return nuFALSE;
			}
			bTMCRTB[(*m_ppTMCNodeIDList)[count].RTBID] = nuTRUE;
			/*fprintf(fileo,"TMCWeighting:%d, ",TMCPassData[i].TMCWeighting);
			fprintf(fileo,"TMCTime:%d, ",TMCPassData[i].TMCTime);
			fprintf(fileo,"FNodeID:%d, ",(*TMCItemB)[j].FNodeID);
			fprintf(fileo,"FRTBID:%d\n",(*TMCItemB)[j].FRTBID);
			fprintf(filetest,"Weighting:%d, ",(*m_ppTMCNodeIDList)[count].Weighting);
			fprintf(filetest,"Time:%d, ",(*m_ppTMCNodeIDList)[count].Time);
			fprintf(filetest,"NodeID:%d, ",(*m_ppTMCNodeIDList)[count].NodeID);
			fprintf(filetest,"RTBID:%d\n",(*m_ppTMCNodeIDList)[count].RTBID);*/
			count++;
			if(count >= TMCEnventRoadCount)//?ɪ??N?O?????D
			{
				ReleaseStruct();
				return nuFALSE;
			}
		}
		(*m_ppTMCNodeIDList)[count].Weighting = TMCPassData[i].sTMCRouteWeighting;
		(*m_ppTMCNodeIDList)[count].Time	  = TMCPassData[i].sTMCRouteTime;
		(*m_ppTMCNodeIDList)[count].NodeID    = (*TMCItemB)[j - 1].TNodeID;
		(*m_ppTMCNodeIDList)[count].RTBID     = (*TMCItemB)[j - 1].TRTBID;
		/*fprintf(fileo,"TMCWeighting:%d, ",TMCPassData[i].TMCWeighting);
		fprintf(fileo,"TMCTime:%d, ",TMCPassData[i].TMCTime);
		fprintf(fileo,"TNodeID:%d, ",(*TMCItemB)[j - 1].TNodeID);
		fprintf(fileo,"TRTBID:%d\n",(*TMCItemB)[j - 1].TRTBID);
		fprintf(filetest,"Weighting:%d, ",(*m_ppTMCNodeIDList)[count].Weighting);
		fprintf(filetest,"Time:%d, ",(*m_ppTMCNodeIDList)[count].Time);
		fprintf(filetest,"NodeID:%d, ",(*m_ppTMCNodeIDList)[count].NodeID);
		fprintf(filetest,"RTBID:%d\n",(*m_ppTMCNodeIDList)[count].RTBID);*/
		count++;
		g_pRtMMApi->MM_FreeMem((nuPVOID*)TMCItemB);
		#ifdef _DEBUG
				TmcFreeCount++;
		#endif
		TMCItemB = NULL;
	}
	/*fclose(fileo);
	fileo = NULL;*/
	B_Table_shellSort();

	//fprintf(filetest,"\nAfter Sort\n");
	count = 0;
	/*for(i = 0; i < TMCEventCount; i++)
	{
		for(j = 0; j < TMCPassData[i].TMCNodeCount - 1; j++)
		{
			fprintf(filetest,"Weighting:%d, ",(*m_ppTMCNodeIDList)[count].Weighting);
			fprintf(filetest,"Time:%d, ",(*m_ppTMCNodeIDList)[count].Time);
			fprintf(filetest,"NodeID:%d, ",(*m_ppTMCNodeIDList)[count].NodeID);
			fprintf(filetest,"RTBID:%d\n",(*m_ppTMCNodeIDList)[count].RTBID);
			count++;
		}
		fprintf(filetest,"Weighting:%d, ",(*m_ppTMCNodeIDList)[count].Weighting);
		fprintf(filetest,"Time:%d, ",(*m_ppTMCNodeIDList)[count].Time);
		fprintf(filetest,"NodeID:%d, ",(*m_ppTMCNodeIDList)[count].NodeID);
		fprintf(filetest,"RTBID:%d\n",(*m_ppTMCNodeIDList)[count].RTBID);
	}
	fprintf(filetest,"\n------------TotalTime :%d-----------\n",nuGetTickCount() - starttime);
	fclose(filetest);
	filetest = NULL;*/
	return nuTRUE;
}
nuBOOL CCtrl_Tmc::ReadTMC_BTable(nuULONG i,nuULONG Index)
{	
/*	(*TMCData)[i].TMCItemB = (StructTMCItemB*)g_pRtMMApi->MM_AllocMem(sizeof(StructTMCItemB) * (*TMCData)[i].TMCItemA.LocationCodeNum);
	if((*TMCData)[i].TMCItemB == NULL){return nuFALSE;}
	if(NULL != nuFseek(TMCFile,Index,SEEK_SET)) {return nuFALSE;}		
	if(NULL == nuFread((*TMCData)[i].TMCItemB,sizeof(StructTMCItemB), (*TMCData)[i].TMCItemA.LocationCodeNum, TMCFile)){return nuFALSE;}*/
	return nuTRUE;
}
nuBOOL CCtrl_Tmc::ReleaseTMC_BTable(nuULONG i)
{	
/*	if((*TMCData)[i].TMCItemB != NULL){g_pRtMMApi->MM_FreeMem((nuPVOID*)(*TMCData)[i].TMCItemB);}*/
	return nuTRUE;
}
nuVOID CCtrl_Tmc::ReleaseStruct()
{
	if(TMCFile != NULL)
	{
		if(NULL != DataCnt){DataCnt = NULL;}
		if(NULL != TrafficCnt){TrafficCnt = NULL;}
		if(NULL != TMCDataAddr){TMCDataAddr = NULL;}
		if(NULL != TrafficDataAddr){TrafficDataAddr = NULL;}
		if(NULL != TMCReserve){TMCReserve = NULL;}
		if(NULL != m_ppTMCNodeIDList)
		{
	#ifdef _DEBUG
			TmcFreeCount++;
	#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)m_ppTMCNodeIDList); 
			m_ppTMCNodeIDList = NULL;
		}
		
		nuFclose(TMCFile);
		TMCFile = NULL;
	}
}
/*nuBOOL CCtrl_Tmc::B_Table_CountingSort()//?Ƨ?B Table???j?M?i?H?ϥΡA?ثe???k?u?????Ʀr?ഫ?A?|??��?Ω?struct?��?
{	
	nuLONG i = 0, j = 0, Min = 0, Max = (*m_ppTMCNodeIDList)[0].RTBID*1048576+(*m_ppTMCNodeIDList)[0].NodeID;
	nuLONG z = 0, range = 0, *count = NULL;
	for(i = 0; i < TMCNodeIDCount; i++)
	{
		if((*m_ppTMCNodeIDList)[i].RTBID*1048576+(*m_ppTMCNodeIDList)[i].NodeID > Max)
		{
			Max = (*m_ppTMCNodeIDList)[i].RTBID*1048576+(*m_ppTMCNodeIDList)[i].NodeID;
		}
		else if((*m_ppTMCNodeIDList)[i].RTBID*1048576+(*m_ppTMCNodeIDList)[i].NodeID < Min)
		{
			Min = (*m_ppTMCNodeIDList)[i].RTBID*1048576+(*m_ppTMCNodeIDList)[i].NodeID;
		}
	}

	range = Max - Min + 1;
	count = new nuLONG[range];// malloc(range * sizeof(int));
	if(count == NULL)
	{
		return nuFALSE;
	}

	for(i = 0; i < range; i++)
	{
		count[i] = 0;
	}

    for(i = 0; i < TMCNodeIDCount; i++)
	{
	    count[ (*m_ppTMCNodeIDList)[i].RTBID*1048576+(*m_ppTMCNodeIDList)[i].NodeID - Min ]++;
	}

	for(i = Min; i <= Max; i++)
	{
		for(j = 0; j < count[ i - Min ]; j++)
		{
			m_ppTMCNodeIDList[z++].RTBID*1048576+m_ppTMCNodeIDList[z++].NodeID = i;
		}
	}
	delete []count;
	return nuTRUE;
}*/

nuBOOL CCtrl_Tmc::B_Table_shellSort()//?Ƨ?B Table???j?M?i?H?ϥ?
{
	nuLONG gap = 0;
	nuLONG i = 0, j = 0, k = 0;
	StructTMCNodeIDList temp;
	gap = TMCNodeIDCount / 2;
	while(gap > 0)//?Ƨ?RTBID?MNodeID
	{		
		for(k = 0; k < gap; k++)
		{
			for(i = k + gap; i < TMCNodeIDCount; i += gap)
			{
				for(j = i - gap; j >= k; j -= gap)
				{
					if((*m_ppTMCNodeIDList)[j].RTBID > (*m_ppTMCNodeIDList)[j+gap].RTBID)
					{
						temp = (*m_ppTMCNodeIDList)[j];
						(*m_ppTMCNodeIDList)[j] = (*m_ppTMCNodeIDList)[j+gap];
						(*m_ppTMCNodeIDList)[j+gap] = temp;						
					}
					else if((*m_ppTMCNodeIDList)[j].RTBID == (*m_ppTMCNodeIDList)[j+gap].RTBID)
					{
						if((*m_ppTMCNodeIDList)[j].NodeID > (*m_ppTMCNodeIDList)[j+gap].NodeID)
						{
							temp = (*m_ppTMCNodeIDList)[j];
							(*m_ppTMCNodeIDList)[j] = (*m_ppTMCNodeIDList)[j+gap];
							(*m_ppTMCNodeIDList)[j+gap] = temp;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		gap = gap / 2;
	}	
	return nuTRUE;
}

