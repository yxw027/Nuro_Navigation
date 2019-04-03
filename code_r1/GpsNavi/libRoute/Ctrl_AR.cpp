
#include "stdAfxRoute.h"

//#include "Ctrl_UONE_RTB.h"
#include "Ctrl_AR.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
CCtrl_AR::CCtrl_AR()
{
	InitClass();
#ifdef _DEBUG
	ARFreeCount = 0;
	ARAlocCount = 0;
#endif
}

CCtrl_AR::~CCtrl_AR()
{
	CloseClass();
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"AR_MemUesd %d\n", ARAlocCount - ARFreeCount);
		fclose(pfile);
	}
#endif
#endif
}
nuVOID CCtrl_AR::InitARStruct()
{
	ARFile = NULL;
}
nuBOOL CCtrl_AR::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	C_ARdata = NULL;
	B_ARdata = NULL;
	A_ARdata = NULL;
	return nuTRUE;
}
nuVOID CCtrl_AR::CloseClass()
{
	ReleaseStruct();
}

nuVOID CCtrl_AR::ReleaseStruct()
{
	if(NULL != C_ARdata) 
	{
	#ifdef _DEBUG
		ARFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*) C_ARdata), 
		C_ARdata = NULL;
	}
	if(NULL != B_ARdata) 
	{
	#ifdef _DEBUG
		ARFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*) B_ARdata),  
		B_ARdata = NULL;
	}
	if(NULL != A_ARdata) 
	{
	#ifdef _DEBUG
		ARFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*) A_ARdata),  
		A_ARdata = NULL;
	}
	if(NULL != RTIDCount) {RTIDCount = NULL;}
	if(ARFile != NULL) {nuFclose(ARFile); ARFile = NULL;}

}

nuBOOL CCtrl_AR::ReadAR_Head()
{
	ARFile  = nuTfopen(FName, NURO_FS_RB);
	if(ARFile == NULL){ return nuFALSE; }
	
	if(NULL != nuFseek(ARFile,0,NURO_SEEK_SET)) {return nuFALSE;}//fseek在成功執行時會回傳NULL
	if(1 != nuFread(&RTIDCount, sizeof(nuUSHORT), 1, ARFile)) {ReleaseStruct(); return nuFALSE;}

	A_ARdata = (structAR_A**)g_pRtMMApi->MM_AllocMem(RTIDCount*sizeof(structAR_A));
	if(A_ARdata == NULL){return nuFALSE;}
#ifdef _DEBUG
	ARAlocCount++;
#endif
	if(RTIDCount != nuFread(*A_ARdata, sizeof(structAR_A), RTIDCount, ARFile)) {ReleaseStruct(); return nuFALSE;}

	nuUINT B_size = 0;
	for(nuLONG i = 0; i < RTIDCount; i++)
	{
		B_size += (*A_ARdata)[i].NearbyCount;//Calculate size of B struct 
	}
	B_ARdata = (structAR_B**)g_pRtMMApi->MM_AllocMem(B_size*sizeof(structAR_B));
	if(B_ARdata == NULL){return nuFALSE;}
#ifdef _DEBUG
	ARAlocCount++;
#endif
	if(B_size != nuFread(*B_ARdata, sizeof(structAR_B), B_size, ARFile)) {ReleaseStruct(); return nuFALSE;}

	return nuTRUE;
}

nuBOOL CCtrl_AR::ReadAR_Node(nuULONG C_add, nuULONG B_add)
{
	nuULONG i = 0, j = 0;
	C_ARdata = (structAR_C**)g_pRtMMApi->MM_AllocMem((*B_ARdata)[B_add].NodeCount*sizeof(structAR_C));
	if(C_ARdata == NULL){return nuFALSE;}	
#ifdef _DEBUG
	ARAlocCount++;
#endif
	if(NULL != nuFseek(ARFile,(*B_ARdata)[B_add].C_address,NURO_SEEK_SET)) {ReleaseStruct(); return nuFALSE;}//fseek在成功執行時會回傳NULL
	if((*B_ARdata)[B_add].NodeCount != nuFread(*C_ARdata, sizeof(structAR_C), (*B_ARdata)[B_add].NodeCount, ARFile)) {ReleaseStruct(); return nuFALSE;}
	AR_Nearby.Count = (*B_ARdata)[B_add].NodeCount;
	for(i = 0; i < AR_Nearby.Count; i++)
	{
		for(j = 0; j < AR_Nearby.Count; j++)
		{
			if((*C_ARdata)[i].FNodeID < (*C_ARdata)[j].FNodeID)
			{
				structAR_C temp = (*C_ARdata)[i];
				(*C_ARdata)[i] = (*C_ARdata)[j];
				(*C_ARdata)[j] = temp;
			}
		}
	}
	//不需要用了，先清除記憶體
	if(NULL != B_ARdata) 
	{
	#ifdef _DEBUG
		ARFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*) B_ARdata),  
		B_ARdata = NULL;
	}
	if(NULL != A_ARdata) 
	{
	#ifdef _DEBUG
		ARFreeCount++;
	#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*) A_ARdata),  
		A_ARdata = NULL;
	}
	if(ARFile != NULL) {nuFclose(ARFile); ARFile = NULL;}
	return nuTRUE;
}

//把比對放在這裡，以免哪天不用時方便抽離
nuBOOL CCtrl_AR::AR_NearbyCity_Compare(nuULONG FMap, nuULONG TMap, nuULONG FRtb, nuULONG TRtb, nuULONG *C_add, nuULONG *B_add)
{
	nuULONG MaxID = RTIDCount;
	nuULONG MinID = 0, MidID = 0;
	nuULONG Flag = 0;
	nuULONG NearbyCnt = 0;
	nuULONG i = 0, j = 0 , k = 0;

	while(1)
	{
		MidID = (MaxID + MinID) / 2;
		if(FMap > (*A_ARdata)[MidID].FMapID){MinID = MidID;}
		else if(FMap < (*A_ARdata)[MidID].FMapID){MaxID = MidID;}
		else 
		{
			if(FRtb > (*A_ARdata)[MidID].FAreaID){MinID = MidID;}
			else if(FRtb < (*A_ARdata)[MidID].FAreaID){MaxID = MidID;}
			else
			{
				 AR_Nearby.FMapID = (*A_ARdata)[MidID].FMapID;
				 AR_Nearby.FRTBID = (*A_ARdata)[MidID].FAreaID;
				 Flag = MidID;
				 break;
			}
		}

		if((MaxID - MinID) <= 1)
		{
			if(MaxID == RTIDCount && FMap == (*A_ARdata)[MaxID].FMapID &&
				FRtb == (*A_ARdata)[MaxID].FAreaID)
			{
				 AR_Nearby.FMapID = (*A_ARdata)[MaxID].FMapID;
				 AR_Nearby.FRTBID = (*A_ARdata)[MaxID].FAreaID;
				 Flag = MaxID;
				 break;
			}
			else if(MinID == 0 && FMap == (*A_ARdata)[MinID].FMapID &&
				FRtb == (*A_ARdata)[MinID].FAreaID)
			{
				 AR_Nearby.FMapID = (*A_ARdata)[MinID].FMapID;
				 AR_Nearby.FRTBID = (*A_ARdata)[MinID].FAreaID;
				 Flag = MinID;
				 break;
			}
			return nuFALSE;
		}
	}

	for(i = 0; i < Flag; i++)
	{
		NearbyCnt += (*A_ARdata)[i].NearbyCount;//To find address of B struct
	}
	for(i = NearbyCnt; i < (*A_ARdata)[Flag].NearbyCount + NearbyCnt; i++)
	{
		if((*B_ARdata)[i].TMapID == TMap && (*B_ARdata)[i].TAreaID == TRtb)
		{
			*C_add = (*B_ARdata)[i].C_address;
			*B_add = i;
			return nuTRUE;
		}
	}
	
	return nuFALSE;
}

/*nuBOOL CCtrl_AR::AR_Enter_RT(UONERtBCtrl *g_pRtB_F)//將SRT資料中的TNodeID和RTBID填入AR的資料，以便之後擴散直接使用
{
	for(nuULONG i = 0; i < g_pRtB_F->RtBHeader.TotalNodeCount; i++)
	{
		for(nuULONG j = 0; j < (*g_pRtB_F->RtBMain)[j].ConnectCount; j++)
		{
			if((*g_pRtB_F->RtBConnect)[j].TRtBID == 4095 && (*g_pRtB_F->RtBConnect)[j].TNodeID != 1040000)
			{
				AR_Node_Compare((*g_pRtB_F->RtBConnect)[j].TNodeID, (*g_pRtB_F->RtBConnect)[j].TRtBID);
			}
		}
	}
	return nuTRUE;
}*/

nuBOOL CCtrl_AR::AR_Node_Compare(nuULONG FNodeID, nuULONG FRTBID)
{
	nuULONG MaxID = AR_Nearby.Count;
	nuULONG MinID = 0, MidID = 0;
	nuBOOL Found = nuFALSE;
	ARtrans_C.NodeID = NULL;
	ARtrans_C.RTBID  = NULL;

	while(1)
	{
		MidID = (MaxID + MinID) / 2;
		if(FNodeID > (*C_ARdata)[MidID].FNodeID){ MinID = MidID;}
		else if(FNodeID < (*C_ARdata)[MidID].FNodeID){ MaxID = MidID;}
		else
		{
			ARtrans_C.NodeID = (*C_ARdata)[MidID].TNodeID;
			ARtrans_C.RTBID  = (*C_ARdata)[MidID].TRTBID;
			return nuTRUE;
		}
		if((MaxID - MinID) <= 1)
		{
			if(MaxID == AR_Nearby.Count && FNodeID == (*C_ARdata)[MaxID].FNodeID)
			{
				ARtrans_C.NodeID = (*C_ARdata)[MaxID].TNodeID;
				ARtrans_C.RTBID  = (*C_ARdata)[MaxID].TRTBID;
				return nuTRUE;
			}
			else if(MinID == 0 && FNodeID == (*C_ARdata)[MinID].FNodeID)
			{
				ARtrans_C.NodeID = (*C_ARdata)[MinID].TNodeID;
				ARtrans_C.RTBID  = (*C_ARdata)[MinID].TRTBID;
				return nuTRUE;
			}
			break;
		}	
	}
	return nuFALSE;
}