#include "CTMCCollation.h"
#include "CKDTMCFileCtrl.h"
#include "CTMCServerFileCtrl.h"
#include <stdio.h>
#include <string.h>

#define MAX_BaseDataCnt      32771
typedef struct tagGBTHeaderStruct
{
	nuBYTE MessageCutFlag;
	nuBYTE DataTypeValue;
	nuBYTE Index;
	nuBYTE GBTCount:6;
	nuBYTE FlushTag:2;
	nuBYTE Reserve;
	nuBYTE ReserveA:4;
	nuBYTE ServiceType:4;
	nuBYTE ReserveB;
	nuBYTE ReserveC;
	nuBYTE MapXLowPart;
	nuBYTE MapYLowPart;
	nuBYTE MapYHighPart:4;
	nuBYTE MapXHighPart:4;
}GBTHeaderStruct,*pGBTHeaderStruct;

typedef struct tagGBTMainHeaderStruct
{
	nuBYTE MessageCutFlag;
	nuBYTE DataTypeValue;
	nuBYTE Index;
	nuBYTE ItemIndex:6;
	nuBYTE FlushTag:2;
	nuBYTE TimeHrHigh:3;//?H?????Ѯɨ?(?p??)
	nuBYTE TimeSupport:1;
	nuBYTE DataType:4;
	nuBYTE TimeMin:6;//?H?????Ѯɨ?(??)
	nuBYTE TimeHrLow:2;//?H?????Ѯɨ?(?p??)
}GBTMainHeaderStruct,*pGBTMainHeaderStruct;

typedef struct tagGBTMainStruct
{
	GBTMainHeaderStruct myGBTMain;
	tagGBTMainStruct *NextGBTMain;
}GBTMainStruct,*pGBTMainStruct;

typedef struct tagGBTBaseStruct
{
	nuBYTE MessageCutFlag;
	nuBYTE GBTBaseValue;
	nuBYTE GBTBaseDataCount_High:7;
	nuBYTE DSL:1;
	nuBYTE GBTBaseDataCount_Low:8;
	nuBYTE GBTBaseData[MAX_BaseDataCnt];
}GBTBaseStruct,*pGBTBaseStruct;

typedef struct tagGBTStruct
{
	GBTHeaderStruct GBTHeader;
	GBTMainStruct	*GBTMain;
	GBTBaseStruct	GBTBase;
	tagGBTStruct	*nextGBT;
}GBTStruct,*pGBTStruct;

typedef struct tagGBTBaseDataStruct
{
	nuBYTE VertexCount_High:4;
	nuBYTE undefineA:1;
	nuBYTE InfoNumFlag:1;
	nuBYTE VertexSpeedType:1;
	nuBYTE InfoType:1;
	nuBYTE VertexCount_Low;
	nuBYTE StartVertexNum_High:4;
	nuBYTE VertexClass:3;
	nuBYTE undefineB:1;
	nuBYTE StartVertexNum_Low;
}GBTBaseDataStruct,*pGBTBaseDataStruct;

typedef struct tagGBTBaseDataTrafficSpeedStruct
{
	nuBYTE VertexSpeed:3;
	nuBYTE ExtendSymbol:3;//?X?i?Х?
	nuBYTE RoadCondition:2;//?D?????q???p
}GBTBaseDataTrafficSpeedStruct,*pGBTBaseDataTrafficSpeedStruct;

typedef struct tagGBTBaseDataTrafficNoSpeedStruct
{
	nuBYTE RoadConditionB:2;
	nuBYTE ExtendSymbolB:2;
	nuBYTE RoadConditionA:2;
	nuBYTE ExtendSymbolA:2;
}GBTBaseDataTrafficNoSpeedStruct,*pGBTBaseDataTrafficNoSpeedStruct;

CKDTMCFileCtrl			cKDTMC;
CTMCServerFileCtrl		cTMCServerFile;

CTMCCollation::CTMCCollation()
{
	bLoadTMCMappingOK = nuFALSE;
	bLoadServerTMCOK = nuFALSE;
	tickcount = 0;
	pRoadIDSortBuffer=NULL;
	pNodeIDFTSortBuffer=NULL;
	m_pUse_RoadIDSortBuffer = NULL;
	m_pUse_NodeIDFTSortBuffer = NULL;
}

CTMCCollation::~CTMCCollation()
{
	if(pRoadIDSortBuffer)
	{
		delete [] pRoadIDSortBuffer;
		pRoadIDSortBuffer=NULL;
	}
	if(pNodeIDFTSortBuffer)
	{
		delete [] pNodeIDFTSortBuffer;
		pNodeIDFTSortBuffer=NULL;
	}
	if(m_pUse_NodeIDFTSortBuffer)
	{
		delete [] m_pUse_NodeIDFTSortBuffer;
		m_pUse_NodeIDFTSortBuffer=NULL;
	}
	if(m_pUse_RoadIDSortBuffer)
	{
		delete [] m_pUse_RoadIDSortBuffer;
		m_pUse_RoadIDSortBuffer=NULL;
	}
}

nuBOOL CTMCCollation::LoadTMCMappingFile(nuTCHAR *tsFilePath)
{
	if(bLoadTMCMappingOK)	cKDTMC.ReleaseTMCStru();
	bLoadTMCMappingOK=cKDTMC.ReadTMCFile(tsFilePath);
	return bLoadTMCMappingOK;
}

nuBOOL CTMCCollation::LoadServerTMCFile(nuTCHAR *tsFilePath)
{
	if(bLoadServerTMCOK) cTMCServerFile.ReleaseServerFileStru();
	bLoadServerTMCOK=cTMCServerFile.ReadTMCServerFile(tsFilePath);
	return bLoadServerTMCOK;
}

nuBOOL CTMCCollation::CombineData()
{
__android_log_print(ANDROID_LOG_INFO, "TMC", "CombineData ...");
	/*while(m_bCopy)
	{
		nuSleep(50);
	}*/
	nuINT i = 0, j = 0, k = 0, l = 0;
    	nuINT GBTBaseDataCount = 0;
	nuINT nSpeed = 0, nMeshID = 0, nLinkID = 0, nRoadClass = 0, nTrafficEvent = 0;
	pGBTHeaderStruct		pGBTHeader;
	pGBTMainHeaderStruct	pGBTMainHeader;
	pGBTBaseStruct			pGBTBase;
	pGBTBaseDataStruct		pGBTBaseData;
	//?T?{?U?�p?A?P?_?O?_?B?z?ǰt
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return nuFALSE;
	}

	for(i = 0; i < cTMCServerFile.iServerTMCFileSize;)
	{
		pGBTHeader=(pGBTHeaderStruct)&(cTMCServerFile.pTMCServerData[i]);
		i = i + sizeof(GBTHeaderStruct);
		nMeshID = (((pGBTHeader->MapYHighPart<<8)+pGBTHeader->MapYLowPart)>>3)*10000+(((pGBTHeader->MapXHighPart<<8)+pGBTHeader->MapXLowPart)>>3)*100+(((pGBTHeader->MapYHighPart<<8)+pGBTHeader->MapYLowPart)&0x07)*10+(((pGBTHeader->MapXHighPart<<8)+pGBTHeader->MapXLowPart)&0x07);
			
		for(j=0;j<pGBTHeader->GBTCount;j++)
		{
			pGBTMainHeader=(pGBTMainHeaderStruct)&(cTMCServerFile.pTMCServerData[i]);
			i=i+sizeof(GBTMainHeaderStruct);

			pGBTBase=(pGBTBaseStruct)&(cTMCServerFile.pTMCServerData[i]);
			GBTBaseDataCount=(pGBTBase->GBTBaseDataCount_High<<8)+pGBTBase->GBTBaseDataCount_Low;
			i = i + 4 + GBTBaseDataCount;
			l = 0, k = 0;
			pGBTBaseDataTrafficNoSpeedStruct test1;
			pGBTBaseDataTrafficSpeedStruct test2;
			for(l = 0;l < GBTBaseDataCount; l=l+k)
			{
				pGBTBaseData = (pGBTBaseDataStruct)&(pGBTBase->GBTBaseData[l]);
				nRoadClass = pGBTBaseData->VertexClass + 1;
				l =l+4;
				if(pGBTBaseData->InfoType==0) //?????p??t
				{
					for(k=0; k<(pGBTBaseData->VertexCount_High<<8)+pGBTBaseData->VertexCount_Low; k++)
					{
						test2=(pGBTBaseDataTrafficSpeedStruct)&(pGBTBase->GBTBaseData[k+l]);
						nLinkID=(pGBTBaseData->StartVertexNum_High<<8)+pGBTBaseData->StartVertexNum_Low+k;
						nTrafficEvent=test2->RoadCondition;
						MergeData(nMeshID,nRoadClass,nLinkID,nTrafficEvent);
						switch(test2->ExtendSymbol)
						{
							case 0://?t?פ??T?w
								l=l;
								break;
							case 1://??H??
								l=l;
								break;
							case 2://??H??+?X?i1
								l++;
								break;
							case 3://??H??+?X?i1+?X?i2
								l+=2;
								nSpeed=(pGBTBase->GBTBaseData[k+l]*8)+test2->VertexSpeed;
								break;
							case 4://??H??+?X?i2
								l++;
								nSpeed=(pGBTBase->GBTBaseData[k+l]*8)+test2->VertexSpeed;
								break;
							case 5://?H???׶?
								l++;
								break;
							case 6://???w?q
							case 7://???w?q
							default:
								l=l;
								break;
						}
					}
				}
				else //?u?????p?S???t??
				{
					for(k=0; k<(pGBTBaseData->VertexCount_High<<8)+pGBTBaseData->VertexCount_Low; k++)
					{
						test1 = (pGBTBaseDataTrafficNoSpeedStruct)&(pGBTBase->GBTBaseData[k+l]);
						nLinkID = (pGBTBaseData->StartVertexNum_High<<8)+pGBTBaseData->StartVertexNum_Low+k;
						nTrafficEvent = test1->RoadConditionA;
						MergeData(nMeshID,nRoadClass,nLinkID,nTrafficEvent);

						switch(test1->ExtendSymbolA)
						{
							case 0://?S???X?i
								l=l;
								break;
							case 1://??H??+?X?i1
								l++;
								break;
							case 2://?L??????
								l=l;
								break;
							case 3://?H???׶?(?L??)
								l=l;
								break;
							default:
								l=l;
								break;
						}
					}
				}
			}
		}
	}

	ShellSort();
__android_log_print(ANDROID_LOG_INFO, "TMC", "CombineData finished");
	//CreateTMCFindMap();
	return nuTRUE;
}


nuBOOL CTMCCollation::MergeData(nuINT MeshID,nuINT RoadClass,nuINT LinkID,nuINT TrafficEvent)
{
	for(nuUINT i=0; i<cKDTMC.TMCStru.KDTMCHeader.DataCnt; i++)
	{
		if(cKDTMC.TMCStru.pKDTMCMain[i].MeshID==MeshID && cKDTMC.TMCStru.pKDTMCMain[i].RoadClass==RoadClass && cKDTMC.TMCStru.pKDTMCMain[i].LinkID==LinkID)
		{
			for(nuUINT j=0; j<cKDTMC.TMCStru.pKDTMCMain[i].LocationCodeNum; j++)
			{
				cKDTMC.TMCStru.pKDTMCNode[cKDTMC.TMCStru.pKDTMCMain[i].LocationVertexAddr+j].TrafficEvent=TrafficEvent;
			}
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuUINT CTMCCollation::GetTMCTraffic(nuUINT BlockID,nuUINT RoadID)
{
/*	nuUINT va = FindFromTMCFindMap(BlockID,RoadID);
	if (va != 4)
	{
		return va;
	}*/
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCTraffic...");

	//Copy_TMC_Event_Data();
	
	nuUINT iLow,iHigh,iMid;
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return 0;
	}
	if(m_pUse_RoadIDSortBuffer)
	{
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID<BlockID )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID==BlockID )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID<RoadID )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID==RoadID)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].TrafficEvent;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}
	}
	else
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount2;i++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[i].BlockID==BlockID && cKDTMC.TMCStru2.pKDTMCNode[i].RoadID==RoadID)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].TrafficEvent;
			}
		}
	}
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCTraffic finished");
	return 0;
}

nuUINT CTMCCollation::GetTMCTrafficByNodeID(nuUINT NodeID1,nuUINT NodeID2)
{
//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCTrafficByNodeID...");

	//Copy_TMC_Event_Data();
		
	nuUINT iLow,iHigh,iMid;
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return 0;
	}
	if(m_pUse_NodeIDFTSortBuffer)
	{
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode<NodeID1 )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode==NodeID1 )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode<NodeID2 )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode==NodeID2)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].TrafficEvent;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}

		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode<NodeID2 )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode==NodeID2 )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode<NodeID1 )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode==NodeID1)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].TrafficEvent;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}

	}
	else
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount2;i++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[i].StartNode==NodeID1 && cKDTMC.TMCStru2.pKDTMCNode[i].EndNode==NodeID2)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].TrafficEvent;
			}
			if(cKDTMC.TMCStru2.pKDTMCNode[i].EndNode==NodeID1 && cKDTMC.TMCStru2.pKDTMCNode[i].StartNode==NodeID2)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].TrafficEvent;
			}
		}
	}
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCTrafficByNodeID finished");
	return 0;
}
nuUINT CTMCCollation::GetTMCDir(nuUINT BlockID,nuUINT RoadID)
{
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCDir...");
	//Copy_TMC_Event_Data();
		
	nuUINT iLow,iHigh,iMid;
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return 0;
	}
	if(m_pUse_RoadIDSortBuffer)
	{
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID<BlockID )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID==BlockID )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID<RoadID )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID==RoadID)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].Dir;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}
	}
	else
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount2;i++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[i].BlockID==BlockID && cKDTMC.TMCStru2.pKDTMCNode[i].RoadID==RoadID)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].Dir;
			}
		}
	}
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCDir finished");
	return 0;
}
nuUINT CTMCCollation::GetTMCDirByNodeID(nuUINT NodeID1,nuUINT NodeID2)
{
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCDirByNodeID...");
	//Copy_TMC_Event_Data();
	nuUINT iLow,iHigh,iMid;
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return 0;
	}
	if(m_pUse_NodeIDFTSortBuffer)
	{
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode<NodeID1 )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode==NodeID1 )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode<NodeID2 )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode==NodeID2)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].Dir;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode<NodeID2 )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].StartNode==NodeID2 )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode<NodeID1 )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].EndNode==NodeID1)
				{
					return cKDTMC.TMCStru2.pKDTMCNode[m_pUse_NodeIDFTSortBuffer[iMid]].Dir;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}
	}
	else
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount2;i++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[i].StartNode==NodeID1 && cKDTMC.TMCStru2.pKDTMCNode[i].EndNode==NodeID2)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].Dir;
			}
			if(cKDTMC.TMCStru2.pKDTMCNode[i].EndNode==NodeID1 && cKDTMC.TMCStru2.pKDTMCNode[i].StartNode==NodeID2)
			{
				return cKDTMC.TMCStru2.pKDTMCNode[i].Dir;
			}
		}
	}
	//__android_log_print(ANDROID_LOG_INFO, "TMC", "GetTMCDirByNodeID finished");
	return 0;
}

TMCRoadInfoStru* CTMCCollation::GetTMCData(nuUINT BlockID,nuUINT RoadID)
{
	nuUINT iLow = 0,iHigh = 0,iMid = 0;
	nuUINT i = 0, j = 0, k = 0, Dir = 0;
	nuMemset(RoadInfo, 0, sizeof(TMCRoadInfoStru)*2);
	if(nuFALSE==bLoadTMCMappingOK || nuFALSE==bLoadServerTMCOK)
	{
		return 0;
	}
	if(m_pUse_RoadIDSortBuffer)
	{
		iLow=0;
		iHigh=cKDTMC.TotalNodeDataCount2;
		while(true)
		{
			iMid=(iLow+iHigh)/2;
			if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID<BlockID )
			{
				if(iLow==iMid) break;
				iLow=iMid;
			}
			else if( cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].BlockID==BlockID )
			{
				if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID<RoadID )
				{
					if(iLow==iMid) break;
					iLow=iMid;
				}
				else if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].RoadID==RoadID)
				{
					Dir = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].Dir;
					RoadInfo[0].Dir = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].Dir;
					RoadInfo[0].Traffic = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[iMid]].TrafficEvent;
					for(j= iMid - 1; j > 0;j--)
					{
						if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].BlockID != BlockID || cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].RoadID != RoadID)
						{
							break;
						}
						if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].BlockID == BlockID && cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].RoadID==RoadID)
						{
							if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].Dir != Dir)
							{
								RoadInfo[1].Dir = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].Dir;
								RoadInfo[1].Traffic = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[j]].TrafficEvent;
								return RoadInfo;
							}
						}
					}
					for(k= iMid+1;k < cKDTMC.TotalNodeDataCount2;k++)
					{
						if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].BlockID != BlockID || cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].RoadID != RoadID)
						{
							break;
						}
						if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].BlockID == BlockID && cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].RoadID==RoadID)
						{
							if(cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].Dir != Dir)
							{
								RoadInfo[1].Dir = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].Dir;
								RoadInfo[1].Traffic = cKDTMC.TMCStru2.pKDTMCNode[m_pUse_RoadIDSortBuffer[k]].TrafficEvent;
								return RoadInfo;
							}
						}
					}
					return RoadInfo;
				}
				else
				{
					if(iHigh==iMid) break;
					iHigh=iMid;
				}
			}
			else
			{
				if(iHigh==iMid) break;
				iHigh=iMid;
			}
		}
	}
	else
	{
		for(i=0;i<cKDTMC.TotalNodeDataCount2;i++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[i].BlockID==BlockID && cKDTMC.TMCStru2.pKDTMCNode[i].RoadID==RoadID)
			{
				RoadInfo[0].Dir = cKDTMC.TMCStru2.pKDTMCNode[i].Dir;
				RoadInfo[0].Traffic = cKDTMC.TMCStru2.pKDTMCNode[i].TrafficEvent;
				break;
			}
		}
		for(j=i+1;j<cKDTMC.TotalNodeDataCount2;j++)
		{
			if(cKDTMC.TMCStru2.pKDTMCNode[j].BlockID==BlockID && cKDTMC.TMCStru2.pKDTMCNode[j].RoadID==RoadID)
			{
				RoadInfo[1].Dir = cKDTMC.TMCStru2.pKDTMCNode[i].Dir;
				RoadInfo[1].Traffic = cKDTMC.TMCStru2.pKDTMCNode[i].TrafficEvent;
				break;
			}
		}
		return RoadInfo;
	}
	return 0;
}
nuBOOL CTMCCollation::ReleaseTMCMappingFile()
{
	return cKDTMC.ReleaseTMCStru();
}
nuBOOL CTMCCollation::ReleaseServerTMCFile()
{
	return cTMCServerFile.ReleaseServerFileStru();
}

nuVOID CTMCCollation::ShellSort()
{
	SortRoadID();
	SortNodeIDFT();
}

nuVOID CTMCCollation::SortRoadID()
{
	nuUINT iLow,iHigh,iMid,RoadID,BlockID;

	if(pRoadIDSortBuffer)
	{
		delete [] pRoadIDSortBuffer;
		pRoadIDSortBuffer=NULL;
	}
	pRoadIDSortBuffer=new nuUINT[cKDTMC.TotalNodeDataCount];
	if(pRoadIDSortBuffer)
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount;i++)
		{
			pRoadIDSortBuffer[i]=i;
		}
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount;i++)
		{
			if(i==0) continue;

			BlockID=cKDTMC.TMCStru.pKDTMCNode[i].BlockID;
			RoadID=cKDTMC.TMCStru.pKDTMCNode[i].RoadID;

			if(cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[i-1]].BlockID<BlockID || (cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[i-1]].BlockID==BlockID && cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[i-1]].RoadID<RoadID))
			{
				 continue;
			}
			else if(cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[0]].BlockID>BlockID || (cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[0]].BlockID==BlockID && cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[0]].RoadID>RoadID))
			{
				memmove(&(pRoadIDSortBuffer[1]),&(pRoadIDSortBuffer[0]),i*sizeof(nuUINT));
				pRoadIDSortBuffer[0]=i;
			}
			else
			{
				iLow=0;
				iHigh=i+1;
				while(true)
				{
					iMid=(iLow+iHigh)/2;
					if(cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[iMid]].BlockID<BlockID )
					{
						if(iLow==iMid) break;
						iLow=iMid;
					}
					else if( cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[iMid]].BlockID==BlockID )
					{
						if(cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[iMid]].RoadID<RoadID )
						{
							if(iLow==iMid) break;
							iLow=iMid;
						}
						else if(cKDTMC.TMCStru.pKDTMCNode[pRoadIDSortBuffer[iMid]].RoadID==RoadID)
						{
							break;
						}
						else
						{
							if(iHigh==iMid) break;
							iHigh=iMid;
						}
					}
					else
					{
						if(iHigh==iMid) break;
						iHigh=iMid;
					}
				}
				memmove(&(pRoadIDSortBuffer[iMid+2]),&(pRoadIDSortBuffer[iMid+1]),(i-(iMid+1))*sizeof(nuUINT));
				pRoadIDSortBuffer[iMid+1]=i;
			}
		}
	}
}

nuVOID CTMCCollation::SortNodeIDFT()
{
	nuUINT iLow,iHigh,iMid,StartNode,EndNode;

	if(pNodeIDFTSortBuffer)
	{
		delete [] pNodeIDFTSortBuffer;
		pNodeIDFTSortBuffer=NULL;
	}
	pNodeIDFTSortBuffer=new nuUINT[cKDTMC.TotalNodeDataCount];
	if(pNodeIDFTSortBuffer)
	{
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount;i++)
		{
			pNodeIDFTSortBuffer[i]=i;
		}
		for(nuUINT i=0;i<cKDTMC.TotalNodeDataCount;i++)
		{
			if(i==0) continue;

			StartNode=cKDTMC.TMCStru.pKDTMCNode[i].StartNode;
			EndNode=cKDTMC.TMCStru.pKDTMCNode[i].EndNode;

			if(cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[i-1]].StartNode<StartNode || (cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[i-1]].StartNode==StartNode && cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[i-1]].EndNode<EndNode))
			{
				 continue;
			}
			else if(cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[0]].StartNode>StartNode || (cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[0]].StartNode==StartNode && cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[0]].EndNode>EndNode))
			{
				memmove(&(pNodeIDFTSortBuffer[1]),&(pNodeIDFTSortBuffer[0]),i*sizeof(nuUINT));
				pNodeIDFTSortBuffer[0]=i;
			}
			else
			{
				iLow=0;
				iHigh=i+1;
				while(true)
				{
					iMid=(iLow+iHigh)/2;
					if(cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[iMid]].StartNode<StartNode )
					{
						if(iLow==iMid) break;
						iLow=iMid;
					}
					else if( cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[iMid]].StartNode==StartNode )
					{
						if(cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[iMid]].EndNode<EndNode )
						{
							if(iLow==iMid) break;
							iLow=iMid;
						}
						else if(cKDTMC.TMCStru.pKDTMCNode[pNodeIDFTSortBuffer[iMid]].EndNode==EndNode)
						{
							break;
						}
						else
						{
							if(iHigh==iMid) break;
							iHigh=iMid;
						}
					}
					else
					{
						if(iHigh==iMid) break;
						iHigh=iMid;
					}
				}
				memmove(&(pNodeIDFTSortBuffer[iMid+2]),&(pNodeIDFTSortBuffer[iMid+1]),(i-(iMid+1))*sizeof(nuUINT));
				pNodeIDFTSortBuffer[iMid+1]=i;
			}
		}
	}
}
nuBOOL CTMCCollation::Copy_TMC_Event_Data()
{	
	cKDTMC.CopyTMCStru();
	if(m_pUse_NodeIDFTSortBuffer != NULL)
	{
		delete [] m_pUse_NodeIDFTSortBuffer;
		m_pUse_NodeIDFTSortBuffer = NULL;
	}
	m_pUse_NodeIDFTSortBuffer = new nuUINT[cKDTMC.TotalNodeDataCount2];

	if(m_pUse_RoadIDSortBuffer != NULL)
	{
		delete [] m_pUse_RoadIDSortBuffer;
		m_pUse_RoadIDSortBuffer = NULL;
	}
	m_pUse_RoadIDSortBuffer = new nuUINT[cKDTMC.TotalNodeDataCount2];
	nuMemcpy(m_pUse_NodeIDFTSortBuffer, pNodeIDFTSortBuffer, sizeof(nuUINT) * cKDTMC.TotalNodeDataCount2);
	nuMemcpy(m_pUse_RoadIDSortBuffer, pRoadIDSortBuffer, sizeof(nuUINT) * cKDTMC.TotalNodeDataCount2);
	__android_log_print(ANDROID_LOG_INFO, "TMC", "Copy_TMC_Event_Data");
	return nuTRUE;
}
/*nuBOOL CTMCCollation::CreateTMCFindMap()
{
	if (cKDTMC.TotalNodeDataCount)
	{
		for (int i = 0;i < cKDTMC.TotalNodeDataCount;i++)
		{
			int BlockID      = cKDTMC.TMCStru.pKDTMCNode[i].BlockID;
			int RoadID       = cKDTMC.TMCStru.pKDTMCNode[i].RoadID;
			int TrafficEvent = cKDTMC.TMCStru.pKDTMCNode[i].TrafficEvent;
			int va = RoadID*10 + TrafficEvent;
			m_traffFindMap.insert (pair<int,int>(BlockID,va));
		}
	}
	return nuTRUE;
}
nuUINT CTMCCollation::FindFromTMCFindMap(nuUINT BlockID,nuUINT RoadID)
{
	pair <multimap<int,int>::iterator, multimap<int,int>::iterator> ret;
	ret = m_traffFindMap.equal_range(BlockID);
	for (multimap<int,int>::iterator it=ret.first; it!=ret.second; ++it)
	{
		if (it->second/10 == RoadID)
		{
			return it->second%10;
		}
	}	
	return 4;
}*/
