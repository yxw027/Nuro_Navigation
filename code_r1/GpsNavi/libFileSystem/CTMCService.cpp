#include "CTMCService.h"
#include "CTMCCollation.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"


CTMCService::CTMCService()
{
	tmpTMCCollation = NULL;
	nCount = 0;
}

CTMCService::~CTMCService()
{
    Release_TMC_Data();
}


nuBOOL CTMCService::Initial_TMC_Event_Data(nuBYTE nDWIdx)
{
	//add by chang;
	__android_log_print(ANDROID_LOG_INFO, "TMC", "Initial_TMC_Event_Data...");
	if (NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		 return nuFALSE;
	}

	nuTCHAR sFile[NURO_MAX_PATH] = {0};

	if( !LibFS_FindFileWholePath(nDWIdx, sFile, NURO_MAX_PATH) )
	{
		tmpTMCCollation[nDWIdx].ReleaseServerTMCFile();
		return nuFALSE;
	}
	for(nuINT i = nuTcslen(sFile); i >= 0; i--)
	{
		if(sFile[i] == 0x5c)
		{
			sFile[i] = 0;
			break;
		}
	}

    	nuTcscat(sFile, nuTEXT(".DAT"));
	if( !tmpTMCCollation[nDWIdx].LoadServerTMCFile(sFile))
	{
		tmpTMCCollation[nDWIdx].ReleaseServerTMCFile();
		return nuFALSE;
	}

	nuMemset(sFile, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	nuTcscpy(sFile, nuTEXT(".TMC"));
	if( !LibFS_FindFileWholePath(nDWIdx, sFile, NURO_MAX_PATH) )
	{
		tmpTMCCollation[nDWIdx].ReleaseTMCMappingFile();
		return nuFALSE;
	}
	if( !tmpTMCCollation[nDWIdx].LoadTMCMappingFile(sFile))
	{
		tmpTMCCollation[nDWIdx].ReleaseTMCMappingFile();
		return nuFALSE;
	}

	if( !tmpTMCCollation[nDWIdx].CombineData())
	{
		tmpTMCCollation[nDWIdx].ReleaseServerTMCFile();
		tmpTMCCollation[nDWIdx].ReleaseTMCMappingFile();
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "TMC", "Initial_TMC_Event_Data Finished...");
	return nuTRUE;
}

nuVOID CTMCService::Release_TMC_Event_Data(nuBYTE nDWIdx)
{
	if (NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return ;
	}
	tmpTMCCollation[nDWIdx].ReleaseTMCMappingFile();
	tmpTMCCollation[nDWIdx].ReleaseServerTMCFile();
}

nuBOOL CTMCService::Initial_TMC_Data(nuINT iCount)
{
	//add by xiaochang;2014-11-8
	nCount = iCount;
	//////////////////////////////
	tmpTMCCollation = new CTMCCollation[nCount];
	if(tmpTMCCollation == NULL)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuVOID CTMCService::Release_TMC_Data()
{
	for(nuUINT i =0; i < nCount;i++)
	{
		Release_TMC_Event_Data(i);
	}

	if(tmpTMCCollation != NULL)
	{
		delete [] tmpTMCCollation;
		tmpTMCCollation = NULL;
	}
}
nuUINT CTMCService::GetTMCTrafficInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID)
{
	if(NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return 0;
	}
	return tmpTMCCollation[nDWIdx].GetTMCTraffic(BlockID, RoadID);
}

nuUINT CTMCService::GetTMCTrafficInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1,nuUINT NodeID2)
{
	if(NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return 0;
	}
	return tmpTMCCollation[nDWIdx].GetTMCTrafficByNodeID(NodeID1, NodeID2);
}
nuUINT CTMCService::GetTMCDirInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID)
{
	if(NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return 0;
	}
	return tmpTMCCollation[nDWIdx].GetTMCDir(BlockID, RoadID);
}
nuUINT CTMCService::GetTMCDirInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1,nuUINT NodeID2)
{
	if(NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return 0;
	}
	return tmpTMCCollation[nDWIdx].GetTMCDirByNodeID(NodeID1, NodeID2);
}

nuBOOL CTMCService::Copy_TMC_Event_Data(nuBYTE nDWIdx)
{
	tmpTMCCollation[nDWIdx].Copy_TMC_Event_Data();
	return nuTRUE;
}

TMCRoadInfoStru* CTMCService::GetTMCDataInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID)
{
	if(NULL == tmpTMCCollation || nDWIdx < 0 || nDWIdx >= nCount)
	{
		return 0;
	}
	return tmpTMCCollation[nDWIdx].GetTMCData(BlockID, RoadID);
}
