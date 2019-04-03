 
#include "libSearchNew.h"
#include "SearchY.h"
#include "SearchDanyinZ.h"
#include "nuCLogData.h"
#include "MapFileBhY.h"
#include "SearchPYz.h"
#include "SearchSPDNumO.h"
#include "NuroSearchSDK.h"

#include "NuroCore.h"


#include "SearchNewDanyinP.h"

static PMEMMGRAPI_SH	g_pShMM = NULL;
static PFILESYSAPI	g_pShFS = NULL;
static nuWORD g_nMMIdx = -1;
static nuUINT g_nMMLen = 0;
static CSearchY *g_pSearch = NULL;
static CSearchDanyinZ *g_pSearchDY = NULL;
nuCLogData *g_pLogData = NULL;
static nuCLogData &g_LogData = *g_pLogData ;
static CSearchPYz*	g_pSearchPY = NULL;
static CSearchSPDNumO* g_pSearchSPD = NULL;

//@zhikun 2009.07.20
static nuUINT	g_shType = SH_BEGIN_TYPE_NO;

#include "FileMapNewDanyinP.h"
static CSearchNewDanyinP *g_pSearchNewDY = NULL;
nuINT nCityID=-1;
nuINT m_nMapID= -1 ,m_nCityID = -1,m_nTownID= -1;

SEARCHNEW_API nuBOOL LibSH_InitSearchZ(PAPISTRUCTADDR pApiAddr)
{	
	if (!pApiAddr || !pApiAddr->pMmApi || !pApiAddr->pFsApi)
	{
		return nuFALSE;
	}
	g_pShMM	= pApiAddr->pMm_SHApi;
	g_pShFS	= pApiAddr->pFsApi;
	CFileMapBaseZ::InitData(pApiAddr);
	CSearchBaseZ::InitData(pApiAddr);
	CSearchBaseZK::InitData(pApiAddr);
	g_pLogData = new nuCLogData ;
	if( NULL == g_pLogData )
	{
		return nuFALSE;
	}
    
    pApiAddr->pShApi->SH_Stroke_GetNextWord = LibSH_Stroke_GetNextWordData;
    pApiAddr->pShApi->SH_Smart_SecondWords = LibSH_ZhuYinForSmart_SecondWords;
	pApiAddr->pShApi->SH_Smart_OtherWords = LibSH_ZhuYinForSmart_OtherWords;
	pApiAddr->pShApi->SH_Stroke_Poi_GetNextWord = LibSH_Stroke_Poi_GetNextWordData;
    pApiAddr->pShApi->SH_Smart_Poi_SecondWords = LibSH_ZhuYinForSmart_Poi_SecondWords;
	pApiAddr->pShApi->SH_Smart_Poi_OtherWords = LibSH_ZhuYinForSmart_Poi_OtherWords;
    pApiAddr->pShApi->SH_Smart_GetPoiList = LibSH_Smart_GetPoiList;
    


	pApiAddr->pShApi->SH_StartSearch = LibSH_StartSearch;
	pApiAddr->pShApi->SH_EndSearch	= LibSH_EndSearch;
	pApiAddr->pShApi->SH_SetMapId	= LibSH_SetMapId;
	pApiAddr->pShApi->SH_SetCityId	= LibSH_SetCityId;
	pApiAddr->pShApi->SH_SetTownId	= LibSH_SetTownId;
	pApiAddr->pShApi->SH_SetPoiTP1	= LibSH_SetPoiTP1;
	pApiAddr->pShApi->SH_SetPoiTP2	= LibSH_SetPoiTP2;
	pApiAddr->pShApi->SH_SetPoiKeyDanyin = LibSH_SetPoiKeyDanyin;
	pApiAddr->pShApi->SH_SetPoiKeyName	= LibSH_SetPoiKeyName;
	pApiAddr->pShApi->SH_SetPoiKeyPhone	= LibSH_SetPoiKeyPhone;
	pApiAddr->pShApi->SH_SetSearchArea	= LibSH_SetSearchArea;
	pApiAddr->pShApi->SH_SetRoadSZSLimit = LibSH_SetRoadSZSLimit;
	pApiAddr->pShApi->SH_SetRoadSZZhuyin = LibSH_SetRoadSZZhuyin;
	pApiAddr->pShApi->SH_SetPoiPZZhuyin = LibSH_SetPoiPZZhuyin;//prosper
	pApiAddr->pShApi->SH_SetRoadSZ = LibSH_SetRoadSZ;
	pApiAddr->pShApi->SH_SetPoiPZ = LibSH_SetPoiPZ;
	pApiAddr->pShApi->SH_SetRoadKeyDanyin = LibSH_SetRoadKeyDanyin;
	pApiAddr->pShApi->SH_SetRoadKeyName = LibSH_SetRoadKeyName;
	pApiAddr->pShApi->SH_SetRoadName = LibSH_SetRoadName;
	pApiAddr->pShApi->SH_GetCityTotal = LibSH_GetCityTotal;
	pApiAddr->pShApi->SH_GetTownTotal = LibSH_GetTownTotal;
	pApiAddr->pShApi->SH_GetPoiTotal = LibSH_GetPoiTotal;

	
	pApiAddr->pShApi->SH_GetTP1Total = LibSH_GetTP1Total;
	pApiAddr->pShApi->SH_GetRoadSZTotal = LibSH_GetRoadSZTotal;
	pApiAddr->pShApi->SH_GetPoiPZTotal = LibSH_GetPoiPZTotal;//prosper
	pApiAddr->pShApi->SH_GetRoadTotal = LibSH_GetRoadTotal;
	pApiAddr->pShApi->SH_GetRoadCrossTotal = LibSH_GetRoadCrossTotal;
	pApiAddr->pShApi->SH_GetMapNameAll = LibSH_GetMapNameAll;
	pApiAddr->pShApi->SH_GetCityNameAll = LibSH_GetCityNameAll;
	pApiAddr->pShApi->SH_GetTownNameAll = LibSH_GetTownNameAll;
	pApiAddr->pShApi->SH_GetPoiTP1All = LibSH_GetPoiTP1All;
	pApiAddr->pShApi->SH_GetPoiList = LibSH_GetPoiList;
	pApiAddr->pShApi->SH_GetRoadSZList = LibSH_GetRoadSZList;
	pApiAddr->pShApi->SH_GetPoiPZList = LibSH_GetPoiPZList; //prosper
	pApiAddr->pShApi->SH_GetRoadNameList = LibSH_GetRoadNameList;
	pApiAddr->pShApi->SH_GetRoadCrossList = LibSH_GetRoadCrossList;
	pApiAddr->pShApi->SH_GetPoiCityName	= LibSH_GetPoiCityName;
	pApiAddr->pShApi->SH_GetPoiTownName	= LibSH_GetPoiTownName;
	pApiAddr->pShApi->SH_GetPoiByPhone	= LibSH_GetPoiByPhone;
	pApiAddr->pShApi->SH_GetPoiMoreInfo = LibSH_GetPoiMoreInfo;
	pApiAddr->pShApi->SH_GetPoiPos = LibSH_GetPoiPos;
	pApiAddr->pShApi->SH_GetPoiTP2All = LibSH_GetPoiTP2All;
	pApiAddr->pShApi->SH_GetRoadDoorInfo = LibSH_GetRoadDoorInfo;
	pApiAddr->pShApi->SH_GetRoadCrossPos = LibSH_GetRoadCrossPos;
	pApiAddr->pShApi->SH_GetRoadCrossTownName = LibSH_GetRoadCrossTownName;
	pApiAddr->pShApi->SH_GetRoadCrossCityName = LibSH_GetRoadCrossCityName;
	pApiAddr->pShApi->SH_GetRoadPos = LibSH_GetRoadPos;
	pApiAddr->pShApi->SH_GetRoadTownName = LibSH_GetRoadTownName;
	pApiAddr->pShApi->SH_GetRoadCityName = LibSH_GetRoadCityName;
	
	pApiAddr->pShApi->SH_GetRoadTotal_MR = LibSH_GetRoadTotal_MR;
	pApiAddr->pShApi->SH_GetRoadNameList_MR = LibSH_GetRoadNameList_MR;
	pApiAddr->pShApi->SH_SetRoadName_MR = LibSH_SetRoadName_MR;
	pApiAddr->pShApi->SH_GetRoadNameALL_MR = LibSH_GetRoadNameALL_MR;
	
	pApiAddr->pShApi->SH_GetFilterResAll = LibSH_GetFilterResAll;

 	pApiAddr->pShApi->SH_SHDYStart		= LibSH_SHDYStart;
 	pApiAddr->pShApi->SH_SHDYStop		= LibSH_SHDYStop;
 	pApiAddr->pShApi->SH_SHDYSetRegion	= LibSH_SHDYSetRegion;
 	pApiAddr->pShApi->SH_SHDYSetDanyin	= LibSH_SHDYSetDanyin;
 	pApiAddr->pShApi->SH_SHDYGetHead	= LibSH_SHDYGetHead;
 	pApiAddr->pShApi->SH_SHDYGetPageData	= LibSH_SHDYGetPageData;
 	pApiAddr->pShApi->SH_SHDYGetPageStroke	= LibSH_SHDYGetPageStroke;
 	pApiAddr->pShApi->SH_SHDYGetOneDetail	= LibSH_SHDYGetOneDetail;
 	pApiAddr->pShApi->SH_SHDYCityTownName	= LibSH_SHDYCityTownName;

	pApiAddr->pShApi->SH_SetStrokeLimIdx  = LibSH_SetStrokeLimIdx;
	pApiAddr->pShApi->SH_GetPoiCarFacAll  = LibSH_GetPoiCarFacAll;
	pApiAddr->pShApi->SH_SetPoiCarFac     = LibSH_SetPoiCarFac;
	pApiAddr->pShApi->SH_CollectNextWordAll = LibSH_CollectNextWordAll;

	pApiAddr->pShApi->SH_LDCreateDataFile = LibSH_LDCreateDataFile;
	pApiAddr->pShApi->SH_LDOpenFile       = LibSH_LDOpenFile;
	pApiAddr->pShApi->SH_LDCloseFile      = LibSH_LDCloseFile;
	pApiAddr->pShApi->SH_LDGetFileHeader  = LibSH_LDGetFileHeader;
	pApiAddr->pShApi->SH_LDAddRecord      = LibSH_LDAddRecord;
	pApiAddr->pShApi->SH_LDDelRecord      = LibSH_LDDelRecord;
	pApiAddr->pShApi->SH_LDClearRecord    = LibSH_LDClearRecord;
	pApiAddr->pShApi->SH_LDGetRecord      = LibSH_LDGetRecord;

 	pApiAddr->pShApi->SH_GetPoiSPTypeAll  = LibSH_GetPoiSPTypeAll;
 	pApiAddr->pShApi->SH_SetPoiSPType = LibSH_SetPoiSPType;
 	pApiAddr->pShApi->SH_GetChildRoadTotal_MR = LibSH_GetChildRoadTotal_MR;
 	pApiAddr->pShApi->SH_GetRoadDoorTotal = LibSH_GetRoadDoorTotal;
    pApiAddr->pShApi->SH_GetMapInfo       = LibSH_GetMapInfo;

    pApiAddr->pShApi->SH_StartSKBSearch   = LibSH_StartSKBSearch;
    pApiAddr->pShApi->SH_StopSKBSearch    = LibSH_StopSKBSearch;
    pApiAddr->pShApi->SH_GetSKBAll        = LibSH_GetSKBAll;

	pApiAddr->pShApi->SH_ShBeginZ			= LibSH_ShBeginZ;
	pApiAddr->pShApi->SH_ShEndZ				= LibSH_ShEndZ;
	pApiAddr->pShApi->SH_ShSetModeZ			= LibSH_ShSetModeZ;
	pApiAddr->pShApi->SH_ShSetRegionZ		= LibSH_ShSetRegionZ;
	pApiAddr->pShApi->SH_ShSetStringZ		= LibSH_ShSetStringZ;
	pApiAddr->pShApi->SH_ShGetHeadZ			= LibSH_ShGetHeadZ;
	pApiAddr->pShApi->SH_ShGetPageDataZ		= LibSH_ShGetPageDataZ;
	pApiAddr->pShApi->SH_ShGetPagePinyinZ	= LibSH_ShGetPagePinyinZ;
	pApiAddr->pShApi->SH_ShGetOneDetailZ	= LibSH_ShGetOneDetailZ;
	pApiAddr->pShApi->SH_ShCityTownNameZ	= LibSH_ShCityTownNameZ;
	pApiAddr->pShApi->SH_ShGetRoadDetailZ	= LibSH_ShGetRoadDetailZ;
	pApiAddr->pShApi->SH_ShGetDrNumCoorZ	= LibSH_ShGetDrNumCoorZ;
	pApiAddr->pShApi->SH_GetPoiNearDis      = LibSH_GetPoiNearDis;

	pApiAddr->pShApi->SH_GetPoiCityID       = LibSH_GetPoiCityID;
	pApiAddr->pShApi->SH_SetPoiKeyNameP		= LibSH_SetPoiKeyNameP;//
	pApiAddr->pShApi->SH_GetRoadTownID =LibSH_GetRoadTownID;

	//Prosper add for VoiceSearch 201211
	pApiAddr->pShApi->SH_VoicePoiKeySearch	= LibSH_VoicePoiKeySearch;
	pApiAddr->pShApi->SH_VoicePoiSetMap		= LibSH_VoicePoiSetMap;
	pApiAddr->pShApi->SH_VoicePoiPosName	= LibSH_VoicePoiPosName;
	pApiAddr->pShApi->SH_VoicePoiEnd		= LibSH_VoicePoiEnd;
	pApiAddr->pShApi->SH_VoiceRoadName		= LibSH_VoiceRoadName;

	//Prosper add for key_poi Search 201212 
	pApiAddr->pShApi->SH_DYGetWordData    = LibSH_SHDYGetWordData;
	pApiAddr->pShApi->SH_DYSetDanyinKeyname = LibSH_SHDYSetDanyinKeyname;
	pApiAddr->pShApi->SH_DYGetPageData_WORD   = LibSH_SHDYGetPageData_WORD;
	pApiAddr->pShApi->SH_ShGetList = LibSH_ShGetList;
	pApiAddr->pShApi->SH_ShGetNextWords = LibSH_ShGetNextWords;
	pApiAddr->pShApi->SH_ShSetNoName = LibSH_SHSetNoName;

	pApiAddr->pShApi->SH_SHDYSetSearchMode = LibSH_SHDYSetSearchMode ;
	pApiAddr->pShApi->SH_SHDYSetKey = LibSH_SHDYSetKey;
	
	pApiAddr->pShApi->SH_GetRoadSKB =LibSH_GetRoadSKB; //prosper 2011226
	pApiAddr->pShApi->SH_GetPoiSKB =LibSH_GetPoiSKB;   //prosper 20111226

	pApiAddr->pShApi->SH_GetRoadWordData = LibSH_GetRoadWordData; //Prosper 20130103

	//pApiAddr->pShApi->SH_ShGetSKB = LibSH_ShGetSKB;
	pApiAddr->pShApi->SH_ShGetWordData=LibSH_ShGetWordData;
	pApiAddr->pShApi->SH_SHDYSetDanyin2	= LibSH_SHDYSetDanyin2;
	pApiAddr->pShApi->SH_SetRoadKeyDanyin2 = LibSH_SetRoadKeyDanyin2;
	pApiAddr->pShApi->SH_SetPoiKeyDanyin2 = LibSH_SetPoiKeyDanyin2;
	
	pApiAddr->pShApi->SH_GetDYPoiTotal = LibSH_GetDYPoiTotal;
#ifdef  ZENRIN
	pApiAddr->pShApi->SH_GetDYDataCOUNT = LibSH_GetDYDataCOUNT;
#endif
#ifndef VALUE_EMGRT
	pApiAddr->pShApi->SH_ShGetDoorInfo  = LibSH_ShGetDoorInfo;
	pApiAddr->pShApi->SH_ShGetDoorInfoData  = LibSH_ShGetDoorInfoData;
#endif

	pApiAddr->pShApi->SH_NEWSEARCH_SDK = LibSH_NEWSEARCH_SDK;
	return nuTRUE;
}

SEARCHNEW_API nuVOID LibSH_FreeSearchZ()
{
	if( g_pLogData )
	{
		delete g_pLogData;
		g_pLogData = NULL;
	}
	LibSH_EndSearch();
	CSearchBaseZ::FreeData();
	CFileMapBaseZ::FreeData();
}

SEARCHNEW_API nuBOOL LibSH_StartSearch(nuWORD nMMIdx, nuUINT nMMLen)
{

	//LibSH_EndSearch();
	if (!g_pSearch)
	{
		g_pSearch = new CSearchY;
		if (!g_pSearch)
		{
			return nuFALSE;
		}
	}
	g_pShMM->MM_CollectDataMem(0);
	nuBYTE *pMM = (nuBYTE*)g_pShMM->MM_GetDataMemMass(nMMLen, &g_nMMIdx);
	if (!pMM)
	{
		LibSH_EndSearch();
		return nuFALSE;
	}
	if (!g_pSearch->Init(g_nMMIdx, nMMLen))
	{
		LibSH_EndSearch();
		return nuFALSE;
	}
	g_nMMLen = nMMLen;
	
				
	return nuTRUE;
}
SEARCHNEW_API nuVOID LibSH_EndSearch()
{
	g_nMMLen = 0;
	LibSH_SHDYStop();
	if (g_pSearch)
	{
		delete g_pSearch;
		g_pSearch = NULL;
	}
	if (g_nMMIdx!=(nuWORD)-1)
	{
		g_pShMM->MM_RelDataMemMass(&g_nMMIdx);
		g_nMMIdx = -1;
	}
}

SEARCHNEW_API nuBOOL LibSH_GetMapInfo(long mapid, SEARCH_MAP_INFO &mapinfo)
{
    CMapFileBhY bhfile;
    if (!bhfile.Open(mapid))
    {
        return nuFALSE;
    }
    return bhfile.GetHeadInfo(mapinfo);
}
SEARCHNEW_API nuBOOL LibSH_SetMapId(long resIdx)
{
	LibSH_SHDYStop();
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetMapId(resIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetCityId(nuWORD resIdx)
{
	nCityID = resIdx;
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetCityId(resIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetTownId(nuWORD resIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetTownId(resIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetPoiTP1(nuUINT resIdx)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetPoiTP1(resIdx);
	}
	return nuFALSE;
}

SEARCHNEW_API nuBOOL LibSH_SetPoiTP2(nuUINT resIdx)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetPoiTP2(resIdx);
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyDanyin( nuWCHAR *pDanyin,nuBOOL bComplete)
{
	if ( g_pSearch )
	{
		return g_pSearch->SetPoiKeyDanyin(pDanyin, bComplete );
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyDanyin2( nuWCHAR *pDanyin,nuBOOL bComplete)
{
	if ( g_pSearch )
	{
		return g_pSearch->SetPoiKeyDanyin2(pDanyin, bComplete );
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyName(nuWCHAR *pName, nuBOOL bFuzzy)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetPoiKeyName(pName, bFuzzy);
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyNameP(nuWCHAR *pName, nuBOOL bFuzzy)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetPoiKeyNameP(pName, bFuzzy);
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyPhone(nuWCHAR *pPhone)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetPoiKeyPhone(pPhone);
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetSearchArea(NURORECT *pRect)
{
	if (g_pSearch)
	{
		return g_pSearch->SetSearchArea(pRect);
	}
	return nuFALSE;
}
SEARCHNEW_API nuBOOL LibSH_SetRoadSZSLimit(nuUINT nMin, nuUINT nMax)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetRoadSZSLimit(nMin, nMax);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadSZZhuyin(nuWORD zhuyin)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetRoadSZZhuyin(zhuyin);
}
SEARCHNEW_API nuBOOL LibSH_SetPoiPZZhuyin(nuWORD zhuyin)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->SetPoiPZZhuyin(zhuyin);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadSZ(nuUINT nResIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetRoadSZ(nResIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetPoiPZ(nuUINT nResIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetPoiPZ(nResIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyDanyin(nuWORD *pKeyWord)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetRoadKeyDanyin(pKeyWord);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyDanyin2(nuWORD *pKeyWord)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetRoadKeyDanyin2(pKeyWord);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging /*= nuFALSE*/)
{
	if (!g_pSearch)
	{
		__android_log_print(ANDROID_LOG_INFO, "ID", "g_pSearch null la");	
		return nuFALSE;
	}
	return g_pSearch->SetRoadKeyName(pKeyWord, bIsFromBeging);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetRoadName(nResIdx, bRealIdx);
}

SEARCHNEW_API nuVOID LibSH_SetStrokeLimIdx(nuUINT nResIdx)
{
	if (!g_pSearch)
	{
		return;
	}
	g_pSearch->SetStrokeLimIdx(nResIdx);
}

SEARCHNEW_API nuUINT LibSH_GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiCarFacAll(pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_SetPoiCarFac(nuUINT nResIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetPoiCarFac(nResIdx);
}
SEARCHNEW_API nuUINT LibSH_GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiSPTypeAll(pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_SetPoiSPType(nuUINT nResIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetPoiSPType(nResIdx);
}
SEARCHNEW_API nuUINT LibSH_CollectNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetNextWordAll(pHeadWords, pBuffer, nMaxWords);
}
SEARCHNEW_API nuUINT LibSH_GetCityTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetCityTotal();
}
SEARCHNEW_API nuUINT LibSH_GetTownTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetTownTotal();
}
SEARCHNEW_API nuUINT LibSH_GetPoiTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiTotal();
}
/*
SEARCHNEW_API nuUINT LibSH_GetPoiCount(nuUINT poi)
{
	return g_pSearch->GetPoiTotal(poi);
}*/

SEARCHNEW_API nuUINT LibSH_GetTP1Total()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetTP1Total();
}
SEARCHNEW_API nuUINT LibSH_GetRoadSZTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadSZTotal();
}
SEARCHNEW_API nuUINT LibSH_GetPoiPZTotal() //prosper
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiPZTotal();
}
SEARCHNEW_API nuUINT LibSH_GetRoadTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadTotal();
}
SEARCHNEW_API nuUINT LibSH_GetRoadCrossTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCrossTotal();
}
SEARCHNEW_API nuINT LibSH_GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetMapNameAll(pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT LibSH_GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (g_pSearchNewDY != NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "ID", "g_pSearchNewDY GetCityNameAll ");
		return g_pSearchNewDY->GetCityNameAll(pBuffer, nMaxLen);
	}
	else
	{
		if (!g_pSearch)
		{
			__android_log_print(ANDROID_LOG_INFO, "ID", "g_pSearch GetCityNameAll null");
			return nuFALSE;
		}	
		return g_pSearch->GetCityNameAll(pBuffer, nMaxLen);
	}
}
SEARCHNEW_API nuINT LibSH_GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (g_pSearchNewDY != NULL)
	{
		return g_pSearchNewDY->GetTownNameAll(m_nCityID,pBuffer, nMaxLen);
	}
	else
	{
		if (!g_pSearch)
		{
			return nuFALSE;
		}
		return g_pSearch->GetTownNameAll(pBuffer, nMaxLen);
	}
}
SEARCHNEW_API nuINT  LibSH_GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiTP1All(pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_GetPoiTP2All(nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiTP2All(pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_GetPoiList(NURO_SEARCH_TYPE st, nuWORD nMaxPoi, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiNameList(st, nMaxPoi, (nuBYTE*)pBuffer, nMaxLen);
}
// Prosper
SEARCHNEW_API nuINT  LibSH_Smart_GetPoiList(NURO_SEARCH_TYPE st, nuWORD nMaxPoi, nuVOID* pBuffer, nuUINT nMaxLen,nuUINT word)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiNameList(st, nMaxPoi, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiMoreInfo(nuUINT nPoiListIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->GetPoiMoreInfo(nPoiListIdx, pPaInfo, bRealIdx);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiPos(nuUINT nPoiListIdx, NUROPOINT* pos, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->GetPoiPos(nPoiListIdx, pos, bRealIdx);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiCityName(nResIdx, pBuffer, nMaxLen, bRealIdx);
}
SEARCHNEW_API long LibSH_GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx) // Added by Damon 20100309
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiCityID(nResIdx, bRealIdx);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiTownName(nResIdx, pBuffer, nMaxLen, bRealIdx);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetPoiByPhone(pPhone, pInfo);
}
SEARCHNEW_API nuINT  LibSH_GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->GetRoadSZList(type, nMax, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen) //prosper
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return 	g_pSearch->GetPoiPZList(type, nMax, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadNameList(type, nMax, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuUINT LibSH_GetRoadDoorTotal()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadDoorTotal();
}
SEARCHNEW_API nuBOOL LibSH_GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadDoorInfo(pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCrossList(type, nMax, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCrossPos(nResIdx, pos);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCrossTownName(nResIdx, pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCrossCityName(nResIdx, pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadPos(NUROPOINT* pos)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadPos(pos);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadTownName(nuWORD *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadTownName(pBuffer, nMaxLen);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadCityName(nuWORD *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadCityName(pBuffer, nMaxLen);
}
SEARCHNEW_API nuUINT LibSH_GetRoadTotal_MR()
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetMainRoadTotal();
}
SEARCHNEW_API nuUINT LibSH_GetChildRoadTotal_MR(nuUINT nMainRoadIdx, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetChildRoadTotal(nMainRoadIdx, bRealIdx);
}
SEARCHNEW_API nuINT LibSH_GetRoadNameList_MR(NURO_SEARCH_TYPE type
										   , nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadNameList_MR(type, nMax, pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT LibSH_GetRoadNameALL_MR(nuUINT nMainRoadIdx
										  , nuVOID* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetRoadNameALL_MR(nMainRoadIdx, pBuffer, nMaxLen, bRealIdx);
}
SEARCHNEW_API nuBOOL LibSH_SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->SetRoadName_MR(nMainRoadIdx, nRoadIdx, bRealIdx);
}

SEARCHNEW_API nuINT LibSH_GetFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen)
{
	if (!g_pSearch)
	{
		return nuFALSE;
	}
	return g_pSearch->GetFilterResAll(pKey, pBuffer, nMaxLen);
}

SEARCHNEW_API nuBOOL  LibSH_StartSKBSearch(nuUINT nType)
{
	if( g_pSearch )
	{
		return g_pSearch->StartSKBSearch(nType);
	}
	else
	{
		return nuFALSE;
	}
}
SEARCHNEW_API nuVOID  LibSH_StopSKBSearch()
{
	if( g_pSearch )
	{
		g_pSearch->StopSKBSearch();
	}
	else if(g_pSearchNewDY)
	{
		g_pSearchNewDY->StopSKBSearch();
	}	
}
SEARCHNEW_API nuINT   LibSH_GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount)
{
	if( g_pSearch )
	{
		return g_pSearch->GetSKBAll(keyStr, pBuffer, nMaxLen, resCount);
	}
	else if(g_pSearchNewDY)
	{
		return g_pSearchNewDY->GetSKBAll(keyStr, pBuffer, nMaxLen, resCount);
	}
	else
	{
		return 0;
	}
}

SEARCHNEW_API nuUINT LibSH_SHDYStart()
{
	/*if (g_pSearch)
	{
		delete g_pSearch;
		g_pSearch = NULL;
	}
	if (g_nMMIdx!=(nuWORD)-1)
	{
		g_pShMM->MM_RelDataMemMass(&g_nMMIdx);
		g_nMMIdx = -1;
	}
	g_pShMM->MM_CollectDataMem(0);*/
	if( g_pSearchDY == NULL )
	{
		g_pSearchDY = new CSearchDanyinZ();
		if( g_pSearchDY == NULL )
		{
			return 0;
		}
		if( !g_pSearchDY->Initialize() )
		{
			delete g_pSearchDY;
			g_pSearchDY = NULL;
			return 0;
		}
	}
	return 1;
}
SEARCHNEW_API nuUINT LibSH_SHDYStop()
{
	if( g_pSearchDY != NULL )
	{
		delete g_pSearchDY;
		g_pSearchDY = NULL;
		if (g_nMMLen)
		{
			return (nuUINT)LibSH_StartSearch(-1, g_nMMLen);
		}
		return 1;
	}
	return 0;
}

SEARCHNEW_API nuUINT LibSH_SearchBeginZ(nuUINT nType)
{
	return 0;
}

SEARCHNEW_API nuUINT LibSH_SHDYSetRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchRegion(nSetMode, nData);
}
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchDanyin(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyin2(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchDanyin2(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHDYGetHead(PDYCOMMONFORUI pDyHead)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataHead(pDyHead);
}
SEARCHNEW_API nuUINT LibSH_SHDYGetPageData(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataPage(pDyDataForUI, nItemIdx);
}
SEARCHNEW_API nuUINT LibSH_SHDYGetPageStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataStroke(pDyDataForUI, nStroke);
}
SEARCHNEW_API nuUINT LibSH_SHDYGetOneDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDyDataDetail(pDyDetailForUI, nIdx);
}
SEARCHNEW_API nuUINT LibSH_SHDYCityTownName(PDYCITYTOWNNAME pDyCTname, long nCityID)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDyCityTownName(pDyCTname, nCityID);
}
SEARCHNEW_API nuUINT LibSH_SHDYSetSearchMode(nuINT nMode) 
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	
	return g_pSearchDY->SetSearchMode(nMode);
}

//@zhikun 2009.07.13 for the whole search
SEARCHNEW_API nuUINT LibSH_ShBeginZ(nuUINT nType)
{
	if( g_shType == nType )
	{
		return 0;
	}
	g_shType = nType;
	if (g_pSearch)
	{
		delete g_pSearch;
		g_pSearch = NULL;
	}
	if (g_nMMIdx!=(nuWORD)-1)
	{
		g_pShMM->MM_RelDataMemMass(&g_nMMIdx);
		g_nMMIdx = -1;
	}
	g_pShMM->MM_CollectDataMem(0);
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY == NULL )
		{
			g_pSearchPY = new CSearchPYz();
			if( g_pSearchPY == NULL )
			{
				return 0;
			}
			if( !g_pSearchPY->Initialize() )
			{
				delete g_pSearchPY;
				g_pSearchPY = NULL;
				return 0;
			}
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		//Special door number...
		if( g_pSearchSPD == NULL )
		{
			g_pSearchSPD = new CSearchSPDNumO();
			if( g_pSearchSPD == NULL )
			{
				return 0;
			}
			if( !g_pSearchSPD->Initialize() )
			{
				delete g_pSearchSPD;
				g_pSearchSPD = NULL;
				return 0;
			}
		}
	}
	return 1;
}
SEARCHNEW_API nuUINT LibSH_ShEndZ()
{
	if( g_shType == SH_BEGIN_TYPE_SPDOORNUM || 
		g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			delete g_pSearchPY;
			g_pSearchPY = NULL;
		}
		/* Special door number;*/
		if(  g_pSearchSPD )
		{
			delete g_pSearchSPD;
			g_pSearchSPD = NULL;
		}
		
		g_shType = SH_BEGIN_TYPE_NO;
		if (g_nMMLen)
		{
			return (nuUINT)LibSH_StartSearch(-1, g_nMMLen);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShSetModeZ(nuUINT nShMode)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->SetSHMode(nShMode);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->SetSHMode(nShMode);
		}
	}
	return 0;
}

SEARCHNEW_API nuUINT LibSH_ShSetInput(nuUINT nInputType)
{
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShSetRegionZ(nuBYTE nSetMode, nuDWORD nData)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->SetSHRegion(nSetMode, nData);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->SetSHRegion(nSetMode,nData);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShSetStringZ(nuCHAR *pBuff, nuWORD nBuffLen)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->SetSHString(pBuff, nuLOBYTE(nBuffLen));
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->SetSHString(pBuff,nuLOBYTE(nBuffLen));
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetHeadZ(PSH_HEADER_FORUI pShHeader)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->GetPyDataHead(pShHeader);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDDataHead(pShHeader);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetPageDataZ(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->GetPyDataPage(pPageData, nItemIdx);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDDataPage(pPageData,nItemIdx);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetPagePinyinZ(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->GetPyDataPinyin(pPageData, nPinyin);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDDataStroke(pPageData,nPinyin);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetOneDetailZ(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->GetPyOneDetail(pOneDetail, nIdx);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		;
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShCityTownNameZ(PSH_CITYTOWN_FORUI pCityTown)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		if( g_pSearchPY )
		{
			return g_pSearchPY->GetPyCityTown(pCityTown);
		}
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDCityTown(pCityTown);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetRoadDetailZ(PSH_ROAD_DETAIL pOneRoad, nuDWORD nIdx)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		;
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDRoadDetail(pOneRoad,nIdx);
		}
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_ShGetDrNumCoorZ(nuDWORD nIdx, nuDWORD nDoorNum, nuroPOINT* point)
{
	if( g_shType == SH_BEGIN_TYPE_PY )
	{
		;
	}
	else if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetSPDDrNumCoor(nIdx,nDoorNum,point);
		}
	}
	return 0;
}
SEARCHNEW_API nuINT  LibSH_ShGetList(nuWORD& nMax,nuVOID* pBuffer, nuUINT nMaxLen)
{
	return g_pSearchSPD->GetSPDDataList(nMax,pBuffer,nMaxLen);
}
SEARCHNEW_API nuINT  LibSH_ShGetNextWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	return g_pSearchSPD->NextWords( pRoadBuf, RoadBufLen,pWordBuf,WordLen, n_iCount,wKeyWord,  candylen, ReSort);
}
SEARCHNEW_API nuUINT  LibSH_ShGetSKB(nuVOID* pWordBuf)
{
	return g_pSearchSPD->GetRoadSKB( pWordBuf);
}
SEARCHNEW_API nuUINT LibSH_ShGetWordData(PDYWORDFORUI pDyData)
{
	if( g_pSearchSPD == NULL )
	{
		return 0;
	}
	return g_pSearchSPD->GetDYGetWordData(pDyData);
}
SEARCHNEW_API nuUINT LibSH_GetPoiNearDis(nuUINT nResIdx, 
										 nuBOOL   bRealIdx)
{
	return g_pSearch->GetPoiNearDis(nResIdx, bRealIdx);
}
//@zhikun 2009.07.13 for Pinyin search
/*
SEARCHNEW_API nuUINT LibSH_PYSetModeZ(nuUINT nMode)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->SetSHMode(nMode);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYSetRegionZ(nuBYTE nSetMode, nuDWORD nData)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->SetSHRegion(nSetMode, nData);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYSetStringZ(nuCHAR *pBuff, nuWORD nBuffLen)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->SetSHString(pBuff, nuLOBYTE(nBuffLen));
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYGetHeadZ(PSH_HEADER_FORUI pShHeader)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->GetPyDataHead(pShHeader);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYGetPageDataZ(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->GetPyDataPage(pPageData, nItemIdx);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYGetPagePinyinZ(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->GetPyDataPinyin(pPageData, nPinyin);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYGetOneDetailZ(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->GetPyOneDetail(pOneDetail, nIdx);
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_PYCityTownNameZ(PSH_CITYTOWN_FORUI pCityTown)
{
	if( g_pSearchPY )
	{
		return g_pSearchPY->GetPyCityTown(pCityTown);
	}
	return 0;
}
*/

SEARCHNEW_API nuBOOL LibSH_LDCreateDataFile(nuTCHAR *pFileName
										  , nuUINT nMax, nuUINT nEachRecordLen, nuINT mode)
{
	return nuCLogData::Create(pFileName, nMax, nEachRecordLen, mode);
}
SEARCHNEW_API nuBOOL LibSH_LDOpenFile(nuTCHAR *pFileName)
{
	return g_LogData.Open(pFileName);
}
SEARCHNEW_API nuVOID LibSH_LDCloseFile()
{
	g_LogData.Close();
}
SEARCHNEW_API nuBOOL LibSH_LDGetFileHeader(LOGDATAFILEHEADER *fileHeader)
{
	return g_LogData.GetHeader(fileHeader);
}
SEARCHNEW_API nuBOOL LibSH_LDAddRecord(nuVOID *pBuffer, nuUINT nLen)
{
	return g_LogData.AddRecord(pBuffer, nLen);
}
SEARCHNEW_API nuVOID LibSH_LDDelRecord(nuUINT idx)
{
	g_LogData.DelRecord(idx);
}
SEARCHNEW_API nuVOID LibSH_LDClearRecord()
{
	g_LogData.ClearRecord();
}
SEARCHNEW_API nuUINT LibSH_LDGetRecord(nuUINT sIdx, nuUINT nMax
									   , nuVOID *pBuffer, nuUINT nBufferLen)
{
	return g_LogData.GetRecord(sIdx, nMax, pBuffer, nBufferLen);
}
 
SEARCHNEW_API nuVOID LibSH_Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort)
{	
  g_pSearch->Stroke_GetNextWordData((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}

SEARCHNEW_API nuVOID LibSH_ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort)
{
	g_pSearch->ZhuYinForSmart_SecondWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,candylen,ReSort);
}

SEARCHNEW_API nuBOOL LibSH_ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort)
{
	return g_pSearch->ZhuYinForSmart_OtherWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}
SEARCHNEW_API nuVOID LibSH_Stroke_Poi_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort)
{	
  g_pSearch->Stroke_Poi_GetNextWordData((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}

SEARCHNEW_API nuVOID LibSH_ZhuYinForSmart_Poi_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort)
{
	g_pSearch->ZhuYinForSmart_Poi_SecondWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,candylen,ReSort);
}

SEARCHNEW_API nuBOOL LibSH_ZhuYinForSmart_Poi_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort)
{
	return g_pSearch->ZhuYinForSmart_Poi_OtherWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}

SEARCHNEW_API nuINT LibSH_GetRoadTownID()
{
	return g_pSearch->GetRoadTownID();
}

SEARCHNEW_API nuBOOL LibSH_VoiceRoadName(VOICEROADFORUI data,nuVOID *wChangeName,nuVOID *wCutName,nuroPOINT &point)
{
			
 nuBOOL  bReName=nuFALSE;  
 nuWCHAR wRoadName[SH_VOICE_ROADNAME]={0};
 
 nuBOOL bModeRoad = nuFALSE;
 nuBOOL bRes = g_pSearch->GetMode(data,&wRoadName,bModeRoad);
 nuBOOL bChangMode = nuFALSE;
 nuBOOL bFindPos = nuFALSE;
 if(!bRes)
 {
  return nuFALSE;
 }
 nuINT nDoorNum = 0;
 nuINT nDoorNum2 = 0;
 if(!bModeRoad)
 {
  LibSH_SetRoadKeyName(wRoadName,nuTRUE);
  nuINT nCount = LibSH_GetRoadTotal();
  if(nCount > 0)
  {
   LibSH_SetRoadName(0,-1);
   nuBOOL bNumChange = nuFALSE;
   g_pSearch->GetPosInfo(bNumChange,data.nDoorNum1,data.nDoorNum2,point);
   bFindPos=nuTRUE;
   if(bNumChange)
   {
    nuWCHAR wRoadName2[SH_VOICE_NAME_LEN4]={0};
    nuWcscat(wRoadName2,data.wCITY);
    nuWcscat(wRoadName2,data.wTOWN);
    nuWcscat(wRoadName2,data.wROAD1);
    nuWcscat(wRoadName2,data.wROAD2);
    nuWcscat(wRoadName2,data.wROAD3);
    if(data.nDoorNum2 > 0) 
    {
     nuWCHAR wNum[8] = {0};
     nuWCHAR wNum2[8] = {0};
     nuItow( data.nDoorNum1, wNum, 10 );
     nuItow( data.nDoorNum2, wNum2, 10 );
     nuWcscat(wRoadName2,wNum);
     nuINT nLen=nuWcslen(wRoadName2);
     wRoadName2[nLen]=20043;
     wRoadName2[nLen+1]=0;
     nuWcscat(wRoadName2,wNum2);
     nLen=nuWcslen(wRoadName2);
     wRoadName2[nLen]=34399;
     wRoadName2[nLen+1]=0;
    }
    else
    {
     nuWCHAR wNum[8] = {0};
     nuItow( data.nDoorNum1, wNum, 10 );
     nuWcscat(wRoadName2,wNum);
     nuINT nLen=nuWcslen(wRoadName2);
     wRoadName2[nLen]=34399;
     wRoadName2[nLen+1]=0;
    }
    bReName=nuTRUE;
    nuMemcpy(wChangeName,wRoadName2,sizeof(wRoadName2));
   }
  }
  else
  {
   bModeRoad  = nuTRUE;
   bChangMode = nuTRUE;
  }
 }
 if(bModeRoad)  
 {
   LibSH_ShBeginZ(SH_BEGIN_TYPE_SPDOORNUM);
   LibSH_ShSetModeZ(SH_ENTER_MODE_NAME|SH_ENTER_MODE_NURO);
   LibSH_ShSetRegionZ(SH_REGION_MAP,  0 );
   LibSH_ShSetRegionZ(SH_REGION_CITY, data.nCity);
   LibSH_ShSetRegionZ(SH_REGION_TOWN, data.nTown);

   if(data.nDoorNum2 > 0)
   {
    nuINT nLen=nuWcslen(wRoadName);
    if(bChangMode || wRoadName[nLen-1] == 33256)
    {
     
     nuWCHAR wNum[8] = {0};
     nuWCHAR wNum2[8] = {0};
     nuItow( data.nDoorNum1, wNum, 10 );
     nuItow( data.nDoorNum2, wNum2, 10 );
     nuWcscat(wRoadName,wNum);
     nuINT nLen=nuWcslen(wRoadName);
     wRoadName[nLen]=20043;
     wRoadName[nLen+1]=0;
     nuWcscat(wRoadName,wNum2);
     nLen=nuWcslen(wRoadName);
     wRoadName[nLen]=34399;
     wRoadName[nLen+1]=0;
    }
    
    data.nDoorNum1 = 0;
   }
   LibSH_ShSetStringZ((char*)wRoadName,
                                nuWcslen(wRoadName)*sizeof(nuWCHAR));
   
   /*if(nuWcslen(data.wDOORNUM1))
   {
    nDoorNum = nuWcstol(data.wDOORNUM1,NULL,10);
   }*/
   
   
   bFindPos=LibSH_ShGetDrNumCoorZ(0, data.nDoorNum1, &point);
   

   nuWCHAR wRoadName2[SH_VOICE_NAME_LEN4]={0};
   if(!bFindPos)
   {
    nuWCHAR wKeyName[SH_VOICE_NAME_LEN3]={0};
    nuWcscat(wRoadName2,data.wCITY);
    nuWcscat(wRoadName2,data.wTOWN);
    nuWcscat(wRoadName2,data.wROAD1);
    nuWcscat(wKeyName,data.wROAD1);

    LibSH_ShSetStringZ((char*)wKeyName,
     nuWcslen(wKeyName)*sizeof(nuWCHAR));
    bFindPos=LibSH_ShGetDrNumCoorZ(0, 0, &point);
   }
   LibSH_ShEndZ(); 
   if(bFindPos)
   {
    nuMemcpy(wChangeName,wRoadName2,sizeof(wRoadName2));
    bReName=nuTRUE;
   }
   else
    return nuFALSE;  
  
 }
 if(!bFindPos)
 {
  nuWCHAR wKeyName[SH_VOICE_NAME_LEN3]={0};
  nuWCHAR wRoadName2[SH_VOICE_NAME_LEN4]={0};
  nuWcscat(wRoadName2,data.wCITY);
  nuWcscat(wRoadName2,data.wTOWN);
  nuWcscat(wRoadName2,data.wROAD1);
  nuWcscat(wKeyName,data.wROAD1);
  LibSH_SetCityId(data.nCity);
  LibSH_SetTownId(data.nTown);

  LibSH_SetRoadKeyName(wKeyName,nuTRUE);
  nuINT nCount = LibSH_GetRoadTotal();
  if(nCount > 0)
  {
   LibSH_SetRoadName(0,-1);
   nuBOOL bNumChange = nuFALSE;
   nuINT  nNum=0;
   nuBOOL bFindPos = g_pSearch->GetPosInfo(bNumChange,nNum,0,point);
   if(bFindPos)
   {
    nuMemcpy(wChangeName,wRoadName2,sizeof(wRoadName2));
    bReName=nuTRUE;
   }
   else
   {
    return nuFALSE;
   }
  }
  else
  {
   return nuFALSE;
  }
 }
 if(!bReName)
 {
  nuWCHAR wRoadName2[SH_VOICE_NAME_LEN4]={0};
  nuWcscat(wRoadName2,data.wCITY);
  nuWcscat(wRoadName2,data.wTOWN);
  nuWcscat(wRoadName2,data.wROAD1);
  nuWcscat(wRoadName2,data.wROAD2);
  nuWcscat(wRoadName2,data.wROAD3);
  nuWcscat(wRoadName2,data.wDOORNUM1);
  nuMemcpy(wChangeName,wRoadName2,sizeof(wRoadName2));
 }

 return nuTRUE;
}

SEARCHNEW_API nuBOOL LibSH_VoicePoiSetMap(nuINT nIdx)
{
	if( g_pSearchDY == NULL )
	{
		g_pSearchDY = new CSearchDanyinZ();
		
		if( g_pSearchDY == NULL )
		{
			return 0;
		}
	}
	return g_pSearchDY->VoicePoiSetMap(nIdx);
}
SEARCHNEW_API nuBOOL LibSH_VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	return g_pSearchDY->VoicePoiKeySearch(pDyDetailForUI,nIdx);
}
SEARCHNEW_API nuBOOL LibSH_VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx)
{
	return g_pSearchDY->VoicePoiPosName((nuBYTE*)pCityName,(nuBYTE*)pTownName,nIdx);
}
SEARCHNEW_API nuBOOL LibSH_VoicePoiEnd()
{
	g_pSearchDY->VoicePoiEnd();
	if( g_pSearchDY != NULL )
	{
		delete g_pSearchDY;
		g_pSearchDY = NULL;
	}
	return true;
}
SEARCHNEW_API nuUINT LibSH_SHDYGetWordData(PDYWORDFORUI pDyData)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYGetWordData(pDyData);
}
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyinKeyname(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetPoiKeyName(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHDYGetPageData_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYDataPage_WORD(pDyDataForUI, nItemIdx);
}
SEARCHNEW_API nuBOOL LibSH_SHSetNoName(nuBOOL bRes)
{
	if (g_shType == SH_BEGIN_TYPE_SPDOORNUM)
	{
		return 0;
	}
	return g_pSearch->SetNoName(bRes);
}
SEARCHNEW_API nuUINT LibSH_SHDYSetKey(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->SetSearchKEY(pWsDy, nWsNum);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadSKB(nuVOID* pSKBBuf, nuUINT& WordCont) //Prosper 20111226
{
	return g_pSearch->GetRoadSKB((nuBYTE*)pSKBBuf,  WordCont);
}
SEARCHNEW_API nuBOOL LibSH_GetPoiSKB(nuVOID* pSKBBuf, nuUINT& WordCont)   //Prosper 20111226
{
	return g_pSearch->GetPoiSKB((nuBYTE*)pSKBBuf,  WordCont);
}
SEARCHNEW_API nuBOOL LibSH_GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen)
{
	return g_pSearch->GetRoadWordData(nTotal, (nuBYTE*)pBuffer, nMaxLen);
}
SEARCHNEW_API nuINT LibSH_GetDYPoiTotal()
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetPoiTotal();
}
#ifdef ZENRIN
SEARCHNEW_API nuINT LibSH_GetDYDataCOUNT()
{
	if( g_pSearchDY == NULL )
	{
		return 0;
	}
	return g_pSearchDY->GetDYData_COUNT();
}
#endif
#ifndef VALUE_EMGRT
SEARCHNEW_API nuBOOL LibSH_ShGetDoorInfo(SH_ROAD_DETAIL &data)
{
	if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetDoorNumberInfo(data);
		}
	}
	return 0;
}
SEARCHNEW_API nuINT LibSH_ShGetDoorInfoData(nuVOID* pBuffer)
{
	if( g_shType == SH_BEGIN_TYPE_SPDOORNUM )
	{
		if( g_pSearchSPD )
		{
			return g_pSearchSPD->GetDoorNumberInfoData(pBuffer);
		}
	}
	return 0;
}
#endif

nuLONG g_nSDKSearchMMIdx;
nuWCHAR 	     m_wSKB[128];
nuWCHAR 	     m_wKEY[128];
nuWCHAR 	     m_wPHONEKEY[128];
nuWORD 		     m_nSEARCH_MODE;

nuBOOL		     m_bRoadCross = nuFALSE;
nuBOOL		     m_bReLoadNDY = nuFALSE;
nuINT		     m_nNDYMODE = -1; //
nuINT 		     m_nCrossRoadIdx = -1;

SDK_LISTDATA *NDY_RoadData  = NULL;
nuINT 	     *m_nRoadTownID = NULL;

nuINT 		     m_nTempMapID = -1;
						
SEARCHNEW_API nuBOOL LibSH_NEWSEARCH_SDK(PCALL_BACK_PARAM pParam)
{
    __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ID mode %d %d  %ld",pParam->nCallType,pParam->lCallID, pParam->lDataLen);
	
	switch(pParam->nCallType)
	{
		case _NR_MSG_SDK_SEARCH_SETSEARCHMODE:
		{
			__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETSEARCHMODE .... %d" , pParam->nCallType);
			m_nSEARCH_MODE = (nuINT)pParam->lCallID;
			
			if(m_nSEARCH_MODE == SET_SEARCH_ALL)
			{
				LibSH_EndSearch();
				LibSH_SHNewDYStop(); 
				if(NDY_RoadData)
				{
					delete []NDY_RoadData;
					NDY_RoadData = NULL;
				}
				if(m_nRoadTownID)
				{
					delete []m_nRoadTownID;
					m_nRoadTownID = NULL;
				}
				m_nTempMapID = -1;
				nuBOOL bREs = LibSH_SHNewDYStart();	
				nuroPOINT pos = (nuroPOINT)g_pShFS->pGdata->drawInfoMap.roadCenterOn.ptMapped;
				g_pSearchNewDY->SetAreaCenterPos(pos);
				if(bREs)
				__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_ALL START OK ....");
				else
				__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_ALL START fail ....");
			}
			else
			{
				if( pParam->lCallID != SET_SEARCH_END)
				{
					g_nSDKSearchMMIdx = -1;
					nuUINT nLen = _SDK_MEMSIZE_;

					if (false == LibSH_StartSearch(g_nSDKSearchMMIdx, nLen))
					{
						__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "NEWSEARCH fail");
						return false;
					}
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "NEWSEARCH true");

					//0311
					nuroPOINT pos = (nuroPOINT)g_pShFS->pGdata->drawInfoMap.roadCenterOn.ptMapped;
					LibSH_SetAreaCenterPos(pos);
					__android_log_print(ANDROID_LOG_INFO, "pos", "pos .... %d %d",pos.x,pos.y);
				}			
				else
				{
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "END..........");
					if(NDY_RoadData)
					{
						delete []NDY_RoadData;
						NDY_RoadData = NULL;
					}
					if(m_nRoadTownID)
					{
						delete []m_nRoadTownID;
						m_nRoadTownID = NULL;
					}
					LibSH_EndSearch();
					LibSH_SHNewDYStop(); 
					m_bReLoadNDY = nuFALSE;
					m_bRoadCross = false;
				}
			}
			
		}
		break;
		case _NR_MSG_SDK_SEARCH_SETSEARCHID:
		{
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETSEARCHID %d",pParam->lCallID);
			
			switch(pParam->lCallID)
			{
				
				case SET_SEARCH_STATE_ID:
				{
					if(m_nSEARCH_MODE != SET_SEARCH_ALL)
					{
						LibSH_SetMapId(pParam->lDataLen);
					}
					else
					{
						m_nMapID = pParam->lDataLen;
						m_bReLoadNDY = nuTRUE;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_MAPIDX ,   m_nMapID);
					}
				}
				break;
				case SET_SEARCH_CITY_ID:
				{
					if(m_nSEARCH_MODE != SET_SEARCH_ALL)
					{
						LibSH_SetCityId(pParam->lDataLen);
					}
					else
					{
						m_nCityID = pParam->lDataLen;
						m_bReLoadNDY = nuTRUE;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_CITYIDX ,  m_nCityID);
					}
				}
				break;
				case SET_SEARCH_TOWN_ID:
				{
					LibSH_SetTownId(pParam->lDataLen);
					m_nTownID = pParam->lDataLen;
					m_bReLoadNDY = nuTRUE;

				}
				break;
				case SET_SEARCH_TYPE1_ID:
				{
					LibSH_SetPoiTP1(pParam->lDataLen);
				}
				break;
				case SET_SEARCH_TYPE2_ID:
				{
					LibSH_SetPoiTP2(pParam->lDataLen);
				}
				break;	
				case SET_SEARCH_POI_DIS_ID:
				{
					PMAPCENTERINFO pMapInfo = (PMAPCENTERINFO)pParam->pVOID;
					LibSH_SetMapId(pMapInfo->nFixedFileIdx);
					nuLONG nDis = pParam->lDataLen * 1000;				

					nuroPOINT pos = {0};
					nuMemcpy(&pos,pParam->pVOID,sizeof(nuroPOINT));
					NURORECT rect = {pMapInfo->ptCenterRaw.x - nDis, 
		             		pMapInfo->ptCenterRaw.y - nDis, 
		             		pMapInfo->ptCenterRaw.x + nDis, 
		             		pMapInfo->ptCenterRaw.y + nDis};
				
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "!!! pos %ld %ld",pMapInfo->ptCenterRaw.x,pMapInfo->ptCenterRaw.y);
					LibSH_SetSearchArea(&rect);
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_POIDIS1_ID ok la");
				}
				break;	
				case SET_SEARCH_ROAD_ID:
				{
					if(m_nSEARCH_MODE != SET_SEARCH_ALL)
					{	
						if(pParam->lDataLen>=0)
						{
							m_bRoadCross = nuTRUE;
							LibSH_SetRoadName(pParam->lDataLen, false);
						}
						else //-1
							m_bRoadCross = nuFALSE;
					}

					else //Prosper add 2016.01.25
					{
						
						m_bRoadCross = true;

						//LibSH_SHNewDYStop();						
						g_nSDKSearchMMIdx = -1;
						nuUINT nLen = _SDK_MEMSIZE_;
						__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "LibSH_StartSearch before");
						if (false == LibSH_StartSearch(g_nSDKSearchMMIdx, nLen))
						{
							return false;
						}
						__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "LibSH_StartSearch ok");
						LibSH_SetMapId(m_nMapID);
						LibSH_SetCityId(m_nCityID);
						m_nCrossRoadIdx = pParam->lDataLen;
						__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_ROAD_ID ok");

						
					}
				}
				break;
			}
		}		
		break;
		case _NR_MSG_SDK_SEARCH_GETNAMEALL:
		{
			__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_GETNAMEALL %d",pParam->lCallID);
			
			switch(pParam->lCallID)
			{
				case SET_SEARCH_STATE_ID:
				{
					nuINT nCount = g_pShFS->FS_GetMapCount();
					PMAPNAMEINFO info = (PMAPNAMEINFO)g_pShFS->FS_GetMpnData();
					nuBYTE        m_byBuffer[SDK_PROTWONMAXCOUNT*EACH_CITY_NAME_LEN] = {0};
					for(int j=0;j<nCount;j++)
					{
						nuMemcpy(&m_byBuffer[j*EACH_CITY_NAME_LEN],
							info[j].wMapFriendName,
								sizeof(info[j].wMapFriendName));
					}
					pParam->lDataLen = nCount*EACH_CITY_NAME_LEN;
					//pParam->lCallID  = SET_SEARCH_STATE_ID;			
					pParam->pVOID= &m_byBuffer;
				}	
				break;
				case SET_SEARCH_CITY_ID:
				{
					nuBYTE        m_byBuffer[SDK_PROTWONMAXCOUNT*EACH_CITY_NAME_LEN] = {0};
		 			nuINT nCount = LibSH_GetCityNameAll(m_byBuffer,
 										PROTWONMAXCOUNT*EACH_CITY_NAME_LEN);
					pParam->lDataLen = nCount*EACH_CITY_NAME_LEN;
					//pParam->lCallID  = SET_SEARCH_CITY_ID ;			
					pParam->pVOID= &m_byBuffer;
				}	
				break;
				case SET_SEARCH_TOWN_ID:
				{
					nuBYTE        m_byBuffer[SDK_PROTWONMAXCOUNT*EACH_CITY_NAME_LEN] = {0};
		 			nuINT nCount = LibSH_GetTownNameAll(m_byBuffer,
 										SDK_PROTWONMAXCOUNT*EACH_CITY_NAME_LEN);
					pParam->lDataLen = nCount*EACH_TOWN_NAME_LEN;
					//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
					pParam->pVOID= &m_byBuffer;
				}	
				break;
				case SET_SEARCH_TYPE1_ID:
				{
					nuWCHAR wTemp[SDK_CALTYPETOTALCOUT*EACH_TYPE_NAME_LEN/2] = {0};

					nuINT nCount  = LibSH_GetPoiTP1All(wTemp,
		                                                              SDK_CALTYPETOTALCOUT*EACH_TYPE_NAME_LEN);
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_TYPE1_ID %d",nCount);
					pParam->lDataLen = nCount*EACH_TYPE_NAME_LEN;
					//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
					pParam->pVOID= &wTemp;

				}
				break;
				case SET_SEARCH_TYPE2_ID:
				{
					nuWCHAR wTemp[SDK_CALTYPETOTALCOUT*EACH_TYPE_NAME_LEN/2] = {0};

					nuINT nCount  = LibSH_GetPoiTP2All(wTemp,
		                                                              SDK_CALTYPETOTALCOUT*EACH_TYPE_NAME_LEN);
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_TYPE2_ID %d",nCount);
					pParam->lDataLen = nCount*EACH_TYPE_NAME_LEN;
					//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
					pParam->pVOID= &wTemp;
				}
				break;
				case SET_SEARCH_NEXTWORD_ID:
				{
					nuWCHAR wTemp[NEXT_WORD_MAXCOUNT] = {0};
					nuUINT nCount = 0;
					LibSH_GetNEXTAll(wTemp, nCount);
					pParam->lDataLen = nCount;
					//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
					pParam->pVOID= &wTemp;
				}
				break;
			}	
			
			g_NuroCore->SEARCHSDKCallBack(pParam);
		}
		break;
		case _NR_MSG_SDK_SEARCH_GETCANDIDATEWORD:
		{
			__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_GETCANDIDATEWORD %d ",pParam->lCallID); //m_wSKB[0]
			m_bRoadCross = false;
		switch(m_nSEARCH_MODE)
		{
			case SET_SEARCH_ALL:
			{
				if(!pParam->lCallID)
				{
					if(m_bReLoadNDY)
					{
						LibSH_SHNewDYSetSearchMode(SEARCH_NDY_SET_DANNY);
						m_nNDYMODE = SEARCH_NDY_SET_DANNY;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_TOWNIDX ,  m_nTownID);
						m_bReLoadNDY = nuFALSE;
					}
					LibSH_SHNewDYSetDanyin2(m_wSKB,nuWcslen(m_wSKB));
				}
				else
				{
					if(m_bReLoadNDY)
					{
						LibSH_SHNewDYSetSearchMode(SEARCH_NDY_SET_DANNY);
						m_nNDYMODE = SEARCH_NDY_SET_DANNY;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_TOWNIDX ,  m_nTownID);
						m_bReLoadNDY = nuFALSE;
					}
					LibSH_SHNewDYSetDanyin(m_wSKB,nuWcslen(m_wSKB));
				}				
				DYWORDFORUI data = {0};
				LibSH_SHNewDYGetWordData(&data);

				nuLONG nBufferLen = sizeof(DYWORD)*data.nTotalCount;
				pParam->lDataLen = sizeof(DYWORD)*data.nTotalCount;
				if(data.nTotalCount>0)
				{
					pParam->pVOID= data.Data;
				}
				g_NuroCore->SEARCHSDKCallBack(pParam);
			}
			break;
			case SET_SEARCH_ROAD:
			{
				if(!pParam->lCallID)
				{
					LibSH_SetRoadKeyDanyin2(m_wSKB);
				}
				else
				{
					LibSH_SetRoadKeyDanyin(m_wSKB);
				}			
				
				LibSH_GetRoadTotal();
				nuINT m_nRoadCount = 0;
				LibSH_GetRoadWordData(m_nRoadCount,NULL,0);
				DYWORD *m_pRoadKeyWord = NULL;
				if(m_nRoadCount >0)
				{
					m_pRoadKeyWord = new DYWORD[m_nRoadCount];
					nuLONG nBufferLen = sizeof(DYWORD)*m_nRoadCount;
					nuMemset(m_pRoadKeyWord,NULL,nBufferLen);
			
					LibSH_GetRoadWordData(m_nRoadCount,m_pRoadKeyWord,nBufferLen);
				}
				__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "m_nRoadCount :%d ",m_nRoadCount);
			
				pParam->lDataLen = sizeof(DYWORD)*m_nRoadCount;
				//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
				pParam->pVOID= m_pRoadKeyWord;
				g_NuroCore->SEARCHSDKCallBack(pParam);
				if(m_pRoadKeyWord)
				{
					delete [] m_pRoadKeyWord;
					m_pRoadKeyWord = NULL;
				}
			}
			break;
			case SET_SEARCH_POI:
			{
				nuBOOL bMode = pParam->lCallID;
				if(!pParam->lCallID)
				{
					LibSH_SetPoiKeyDanyin2(m_wSKB,bMode);
				}
				else
				{
					LibSH_SetPoiKeyDanyin(m_wSKB,bMode);
				}			
				
				nuINT m_nPoiCount = 0;
				m_nPoiCount=LibSH_GetPoiTotal();
				DYWORD *m_pPoiKeyWord = NULL;
				if(m_nPoiCount>0)
				{			
					m_pPoiKeyWord = new DYWORD[m_nPoiCount];
					nuLONG nBufferLen = sizeof(DYWORD)*m_nPoiCount;
					nuMemset(m_pPoiKeyWord,NULL,nBufferLen);				
					nuINT k = 0;					
					while(k < m_nPoiCount)
					{
						nuWCHAR  *m_pwListBuffer   = new nuWCHAR[128];
						nuMemset(m_pwListBuffer,NULL,sizeof(nuWCHAR)*128);
						if(k==0)
						{
							LibSH_GetPoiList(NURO_SEARCH_NEW,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
						}
						else					
						{
							LibSH_GetPoiList(NURO_SEARCH_NEXT,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
						}
						nuWcscpy(m_pPoiKeyWord[k].name,m_pwListBuffer);
					
						if(m_pwListBuffer)
						{
							delete []m_pwListBuffer;
							m_pwListBuffer = NULL;
						}
						k++;
					}
				}
				pParam->lDataLen = sizeof(DYWORD)*m_nPoiCount;
				//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
				pParam->pVOID= m_pPoiKeyWord;
				g_NuroCore->SEARCHSDKCallBack(pParam);
				if(m_pPoiKeyWord)
				{
					delete [] m_pPoiKeyWord;
					m_pPoiKeyWord = NULL;
				}
			}
			break;
		}
		}
		break;
		case _NR_MSG_SDK_SEARCH_SETKEY:
		{
			m_bRoadCross = false;
			__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETKEY");
			
			nuBOOL bMode = pParam->lCallID;
			nuMemset(m_wSKB,0,sizeof(m_wSKB));
			//nuMemcpy(m_wSKB,pParam->pVOID,sizeof(m_wSKB));
			g_NuroCore->setSDKKEY(_NR_MSG_SDK_SEARCH_SETKEY,m_wSKB);
			for(int i=0;i<nuWcslen(m_wSKB);i++)
			{
			 	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETKEY m_wSKB: %d %d",i,m_wSKB[i]);	
			}
			switch(m_nSEARCH_MODE)
			{
				case SET_SEARCH_ALL:
				{
					//LibSH_SHNewDYSetKey(wsKEYNAME2,nuWcslen(wsKEYNAME2));

				}
				break;
				case SET_SEARCH_ROAD:
				{			
					 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ROAD KEY %d %d",m_wSKB[0],m_wSKB[1]);
					LibSH_SetRoadKeyDanyin2(m_wSKB);
					nuINT nCount  = LibSH_GetRoadTotal();
					 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ROAD %d ",nCount);
				}
				break;
				case SET_SEARCH_POI:
				{			
					LibSH_SetPoiTP1(-1);
					LibSH_SetPoiKeyDanyin2(m_wSKB, bMode);
					nuINT nCount  = LibSH_GetPoiTotal();
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "POI %d ",nCount);
				}
				break;
			}
			break;

		}
		break;
		case _NR_MSG_SDK_SEARCH_SETCANDIDATEKEY:
		{
			m_bRoadCross = false;	
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY " );
			//nuBOOL bMode = pParam->lCallID;
			nuMemset(m_wKEY,0,sizeof(m_wKEY));
			g_NuroCore->setSDKKEY(_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY,m_wKEY);
			for(int i=0;i<nuWcslen(m_wKEY);i++)
			{
			 	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY m_wKEY: %d %d",i,m_wKEY[i]);	
			}
			 			
			//nuWcscpy(m_wKEY,s_pUI_GET_DATA->SEARCH_SDK_DATA.wsKeyName);

			switch(m_nSEARCH_MODE)
			{
				case SET_SEARCH_ALL:
				{			
					LibSH_SHNewDYSetDanyinKeyname(m_wKEY,nuWcslen(m_wKEY));
					
				}	
				case SET_SEARCH_ROAD:
				{			
					LibSH_SetRoadKeyName(m_wKEY,nuFALSE);
					
				}
				break;
				case SET_SEARCH_POI:
				{			
					LibSH_SetPoiKeyName(m_wKEY,nuFALSE);
				}
				break;
			}
			break;

		}
		break;
		case _NR_MSG_SDK_SEARCH_SETFUZZYKEY:
		{
			m_bRoadCross = false;
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETFUZZYKEY");
			nuMemset(m_wKEY,0,sizeof(m_wKEY));
			g_NuroCore->setSDKKEY(_NR_MSG_SDK_SEARCH_SETFUZZYKEY,m_wKEY);

			for(int i=0;i<nuWcslen(m_wKEY);i++)
			{
			 	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETFUZZYKEY m_wKEY: %d %d",i,m_wKEY[i]);	
			}			
			
			switch(m_nSEARCH_MODE)
			{
				case SET_SEARCH_ROAD:
				{			
					LibSH_SetRoadKeyName(m_wKEY,nuTRUE);
					
				}
				break;
				case SET_SEARCH_POI:
				{			
					LibSH_SetPoiKeyName(m_wKEY,nuTRUE);
				}
				break;
				case SET_SEARCH_ALL:
				{			
					nuDWORD T1 = nuGetTickCount();
					
					if(m_bReLoadNDY)
					{
						LibSH_SHNewDYSetSearchMode(SEARCH_NDY_SET_NAME);
						m_nNDYMODE = SEARCH_NDY_SET_NAME;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_TOWNIDX ,  m_nTownID);
						m_bReLoadNDY = nuFALSE;
					}
					nuDWORD T2 = nuGetTickCount();
					 
                                        __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "LOAD TIME %ld",T2-T1);

					if(nuWcslen(m_wKEY)!=0)
					LibSH_SHNewDYSetKey(m_wKEY,nuWcslen(m_wKEY),true);
					// __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ALL KEY %d %d",m_wKEY[0],m_wKEY[1]);
					
				}
				break;
			}
			break;
		}
		break;
		case _NR_MSG_SDK_SEARCH_SETFUZZYKEY_PART:
		{
			m_bRoadCross = false;
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETFUZZYKEY_PART");
			nuMemset(m_wKEY,0,sizeof(m_wKEY));
			g_NuroCore->setSDKKEY(_NR_MSG_SDK_SEARCH_SETFUZZYKEY_PART,m_wKEY);

			for(int i=0;i<nuWcslen(m_wKEY);i++)
			{
			 	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETFUZZYKEY m_wKEY: %d %d",i,m_wKEY[i]);	
			}			
			
			switch(m_nSEARCH_MODE)
			{
				case SET_SEARCH_ROAD:
				{			
					//LibSH_SetRoadKeyName(m_wKEY,nuTRUE);
					
				}
				break;
				case SET_SEARCH_POI:
				{			
					//LibSH_SetPoiKeyName(m_wKEY,nuTRUE);
				}
				break;
				case SET_SEARCH_ALL:
				{			
					nuDWORD T1 = nuGetTickCount();
					
					if(m_bReLoadNDY)
					{
						LibSH_SHNewDYSetSearchMode(SEARCH_NDY_SET_NAME);
						m_nNDYMODE = SEARCH_NDY_SET_NAME;
						LibSH_SHNewDYSetRegion(SEARCH_DY_SET_TOWNIDX ,  m_nTownID);
						m_bReLoadNDY = nuFALSE;
					}
					nuDWORD T2 = nuGetTickCount();
					 
                                        __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "LOAD TIME %ld",T2-T1);

					if(nuWcslen(m_wKEY)!=0)
					LibSH_SHNewDYSetKey(m_wKEY,nuWcslen(m_wKEY),false);
					// __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ALL KEY %d %d",m_wKEY[0],m_wKEY[1]);
					
				}
				break;
			}
			break;
		}
		break;
		case _NR_MSG_SDK_SEARCH_SETPHONEKEY:
		{
			m_bRoadCross = false;
			__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETPHONEKEY");
			
			nuBOOL bMode = pParam->lCallID;
			nuMemset(m_wPHONEKEY,0,sizeof(m_wPHONEKEY));
			g_NuroCore->setSDKKEY(_NR_MSG_SDK_SEARCH_SETPHONEKEY,m_wPHONEKEY);
			for(int i=0;i<nuWcslen(m_wPHONEKEY);i++)
			{
			 	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_SETPHONEKE m_wKEY: %d %d",i,m_wPHONEKEY[i]);	
			}	
			
			LibSH_SetPoiKeyPhone(m_wPHONEKEY);

		}
		break;
		case _NR_MSG_SDK_SEARCH_GETKEYBOARD:
		{
			m_bRoadCross = false;
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_GETKEYBOARD %d",pParam->lCallID );
			
			if(pParam->lCallID == SET_SEARCH_NEXTWORD_ID)
			{
				nuWCHAR wTemp[NEXT_WORD_MAXCOUNT] = {0};

				nuUINT nCount  = 0;
				LibSH_GetNEXTAll(wTemp,nCount);
				__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "SET_SEARCH_TYPE2_ID %d",nCount);
				pParam->lDataLen = nCount;
				//pParam->lCallID  = SET_SEARCH_TOWN_ID ;			
				pParam->pVOID= &wTemp;
			}
			else
			{
				switch(m_nSEARCH_MODE)
				{
					case SET_SEARCH_ROAD:
					{		
						LibSH_StartSKBSearch(SH_BEGIN_TYPE_ROAD);		
					}
					break;
					case SET_SEARCH_POI:
					{
						LibSH_StartSKBSearch(SH_BEGIN_TYPE_POI);	
					}
					break;				
				}
				char cBuffer[64];
				nuMemset(cBuffer, NULL, sizeof(cBuffer));
				int iRecCount = 0;
				char cKey[64];
				nuMemset(cKey, NULL, sizeof(cKey));
				nuWcstoAsc(cKey, sizeof(cKey), m_wSKB, sizeof(m_wSKB));
				int iGet = LibSH_GetSKBAll(cKey, cBuffer, 64, &iRecCount);
				if(iGet ==0)
				{
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "GetSKBAll fail  ");
					return false;
				}
				LibSH_StopSKBSearch();	
				pParam->lDataLen = sizeof(cBuffer);
				for(int k=0;k<iGet;k++)
				__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "@@@@ value  %d %d",k ,cBuffer[k]);
			
				pParam->pVOID= &cBuffer;
			}
			g_NuroCore->SEARCHSDKCallBack(pParam);
		}
		break;
		case _NR_MSG_SDK_SEARCH_GETLISTALL:
		{
			 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "_NR_MSG_SDK_SEARCH_GETLISTALL %d",m_nSEARCH_MODE);
			
			switch(m_nSEARCH_MODE)
			{
				case SET_SEARCH_ALL:
				{
					if(!m_bRoadCross)
					{
						nuINT nCount = 0;
						NDYDETAILFORUI	*m_temp = new NDYDETAILFORUI[NDY_MAX_KEPT_RECORD];
						nuMemset(m_temp,0,sizeof(NDYDETAILFORUI)*NDY_MAX_KEPT_RECORD);
						//if(m_nNDYMODE == SEARCH_NDY_SET_NAME)
						//nCount = LibSH_SHNewDYGetPoiTotal();
						LibSH_GetNewDYGetALLData(m_temp,nCount);
						 __android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "ALL ROADNAME nCount %d ",nCount);
				
						if(NDY_RoadData)
						{
							delete []NDY_RoadData;
							NDY_RoadData = NULL;
						}
						if(nCount>0)
						{
							NDY_RoadData = new SDK_LISTDATA[nCount];
							nuMemset(NDY_RoadData,NULL,sizeof(SDK_LISTDATA)*nCount);
							if(m_nRoadTownID)
							{
								delete []m_nRoadTownID;
								m_nRoadTownID = NULL;
							}
							m_nRoadTownID = new nuINT[nCount];
							nuMemset(m_nRoadTownID,0,sizeof(nuINT)*nCount);
				
							for(nuINT k = 0; k<nCount;k++)
							{
								NDY_RoadData[k].pos = m_temp[k].point;
								nuWcscpy(NDY_RoadData[k].wsName,m_temp[k].wsName);
								nuWcscpy(NDY_RoadData[k].wsCityName,m_temp[k].wsCityName);
								nuWcscpy(NDY_RoadData[k].wsTownName,m_temp[k].wsTownName);
								nuWcscpy(NDY_RoadData[k].wsOther,m_temp[k].wsOther);
								m_nRoadTownID[k] = m_temp[k].nTownID;
								NDY_RoadData[k].nType = m_temp[k].nType;
								NDY_RoadData[k].nDis=m_temp[k].nDistance;
								//__android_log_print(ANDROID_LOG_INFO, "ID", "m_nRoad[k] %d %d %d",k, m_temp[k].nCityID,m_temp[k].nTownID);
							
							}
						}		
						pParam->lDataLen = nCount*sizeof(SDK_LISTDATA);
						pParam->pVOID = NDY_RoadData;
						pParam->lCallID = m_nSEARCH_MODE ;
				
						g_NuroCore->SEARCHSDKCallBack(pParam);
				
						if(m_temp!=NULL)
						{
							delete[] m_temp;
							m_temp = NULL;
						}
										
					}
					else
					{
						LibSH_SetTownId(m_nRoadTownID[m_nCrossRoadIdx]);
						//__android_log_print(ANDROID_LOG_INFO, "ID", "m_nRoadTownID[m_nCrossRoadIdx] %d",m_nRoadTownID[m_nCrossRoadIdx]);
						
						LibSH_SetRoadKeyName(NDY_RoadData[m_nCrossRoadIdx].wsName,nuTRUE);
						//	__android_log_print(ANDROID_LOG_INFO, "ID", "NAME LEN %d %d", pParam->lDataLen,nuWcslen(NDY_RoadData[pParam->lDataLen].wsName));
						//for(nuINT l=0;l<nuWcslen(NDY_RoadData[m_nCrossRoadIdx].wsName);l++)
	//{						//	__android_log_print(ANDROID_LOG_INFO, "ID", "NAME[]  %d %d", l,NDY_RoadData[m_nCrossRoadIdx].wsName[l]);}							
							nuINT nCountX = LibSH_GetRoadTotal();
							LibSH_SetRoadName(0,false);
							nuINT m_nNDYRoadCross = 0;						
							m_nNDYRoadCross = LibSH_GetRoadCrossTotal();
							
							__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "%d %d ", nCountX,m_nNDYRoadCross);
						nuINT nCount = LibSH_GetRoadCrossTotal();
						__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "cROSS COUNT %d ",nCount);
						if(nCount>0)
						{
						
							SDK_LISTDATA *data = new SDK_LISTDATA[nCount];
							nuMemset(data,NULL,sizeof(SDK_LISTDATA)*nCount);				
							nuINT k = 0;					
							while(k < nCount)
							{
								nuWCHAR  *m_pwListBuffer   = new nuWCHAR[128];
						 		nuMemset(m_pwListBuffer,NULL,sizeof(nuWCHAR)*128);
								nuWCHAR wsTempCity[128]={0};
								nuWCHAR wsTempTown[128]={0};
								nuroPOINT tempPos = {0};
								if(k==0)
								{
									LibSH_GetRoadCrossList(NURO_SEARCH_NEW,1,
										(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
								}
								else					
								{
									LibSH_GetRoadCrossList(NURO_SEARCH_NEXT,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
				
								}
							
								bool b_GetTown = LibSH_GetRoadCrossTownName(0,(nuWORD*)wsTempTown, 
																				 1*EACH_TOWN_NAME_LEN);// ???^Town????
								bool b_GetCity = LibSH_GetRoadCrossCityName(0,(nuWORD*)wsTempCity, 
														 1*EACH_TOWN_NAME_LEN);// ???^city????
				
								LibSH_GetRoadCrossPos(0, &tempPos);
								
								nuWcscpy(data[k].wsName,m_pwListBuffer);
								nuWcscpy(data[k].wsCityName,wsTempCity);
								nuWcscpy(data[k].wsTownName,wsTempTown);
								data[k].pos = tempPos;
				
								if(m_pwListBuffer)
								{
									delete []m_pwListBuffer;
									m_pwListBuffer = NULL;
								}
								k++;
							}
						
				
							pParam->lDataLen = nCount*sizeof(SDK_LISTDATA);
							pParam->pVOID = data;
							pParam->lCallID = m_nSEARCH_MODE ;
				
							g_NuroCore->SEARCHSDKCallBack(pParam);
				
							if(data)
							{
								delete []data;
								data = NULL;
							}
						}
					}
				}		
				break;	
				case SET_SEARCH_POI:
				case SET_SEARCH_PHONE:
				case SET_SEARCH_NEARPOI:
				{
					nuINT nCount = LibSH_GetPoiTotal();
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "################ count %d ",nCount);
					if(nCount>0)
					{
						SDK_LISTDATA * data = new SDK_LISTDATA[nCount];
						 nuMemset(data,NULL,sizeof(SDK_LISTDATA)*nCount);				
						nuINT k = 0;					
						while(k < nCount)
						{
							nuWCHAR  *m_pwListBuffer   = new nuWCHAR[128];
						 	nuMemset(m_pwListBuffer,NULL,sizeof(nuWCHAR)*128);
							nuWCHAR wsTempCity[128]={0};
							nuWCHAR wsTempTown[128]={0};
							nuroPOINT tempPos = {0};
							if(k==0)
							{
								LibSH_GetPoiList(NURO_SEARCH_NEW,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
							}
							else					
							{
								LibSH_GetPoiList(NURO_SEARCH_NEXT,1,
								(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
							}
							bool b_GetTown = LibSH_GetPoiTownName(0,
														        (nuWORD*)wsTempTown, 
			                                                    1*EACH_TOWN_NAME_LEN, 
			                                                    false);// ???^Town????
							bool b_GetCity = LibSH_GetPoiCityName(0,
																(nuWORD*)wsTempCity, 
																1*EACH_TOWN_NAME_LEN,
																false);// ???^city????
							LibSH_GetPoiPos(0, &tempPos, false);

							//if(m_nSEARCH_MODE == SET_SEARCH_NEARPOI)
							
							data[k].nDis=LibSH_GetPoiNearDis(0,false);
							
	__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "Road data[k].nDis %d %d %ld ",tempPos.x,tempPos.y,data[k].nDis);

							nuWcscpy(data[k].wsName,m_pwListBuffer);
							nuWcscpy(data[k].wsCityName,wsTempCity);
							nuWcscpy(data[k].wsTownName,wsTempTown);
							data[k].pos = tempPos;
							SEARCH_PA_INFO pPaInfo = {0};
							LibSH_GetPoiMoreInfo(k, &pPaInfo, true);
							nuMemcpy(data[k].wsOther,pPaInfo.addr,sizeof(pPaInfo.addr));

							if(m_pwListBuffer)
							{
								delete []m_pwListBuffer;
								m_pwListBuffer = NULL;
							}
							k++;
						}
						pParam->lDataLen = nCount*sizeof(SDK_LISTDATA);
						pParam->pVOID = data;
						pParam->lCallID = m_nSEARCH_MODE ;		


						g_NuroCore->SEARCHSDKCallBack(pParam);
		
						if(data)
						{
							delete []data;
							data = NULL;
						}
						
					}
			}	
			break;
			case SET_SEARCH_ROAD:
				{
					if(!m_bRoadCross)
					{
					nuINT nCount = LibSH_GetRoadTotal();
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "Road COUNT %d ",nCount);
					
					if(nCount>0)
					{
						SDK_LISTDATA * data = new SDK_LISTDATA[nCount];
						nuMemset(data,NULL,sizeof(SDK_LISTDATA)*nCount);				
						nuINT k = 0;					
						while(k < nCount)
						{
							nuWCHAR  *m_pwListBuffer   = new nuWCHAR[128];
						 	nuMemset(m_pwListBuffer,NULL,sizeof(nuWCHAR)*128);
							nuWCHAR wsTempCity[128]={0};
							nuWCHAR wsTempTown[128]={0};
							nuroPOINT tempPos = {0};
							if(k==0)
							{
								LibSH_GetRoadNameList(NURO_SEARCH_NEW,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
							}
							else					
							{
								LibSH_GetRoadNameList(NURO_SEARCH_NEXT,1,
								(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);

							}
							LibSH_SetRoadName(0, false);  // 
							

		bool b_GetTown = LibSH_GetRoadTownName((nuWORD*)wsTempTown, 
			                                                     1*EACH_TOWN_NAME_LEN);// ???^Town????
		bool b_GetCity = LibSH_GetRoadCityName((nuWORD*)wsTempCity, 
									     1*EACH_TOWN_NAME_LEN);// ???^city????
		
							LibSH_GetRoadPos(&tempPos);
							
							nuWcscpy(data[k].wsName,m_pwListBuffer);
							nuWcscpy(data[k].wsCityName,wsTempCity);
							nuWcscpy(data[k].wsTownName,wsTempTown);
							data[k].pos = tempPos;

							if(m_pwListBuffer)
							{
								delete []m_pwListBuffer;
								m_pwListBuffer = NULL;
							}
							k++;
						}
						
		
						pParam->lDataLen = nCount*sizeof(SDK_LISTDATA);
						pParam->pVOID = data;
						pParam->lCallID = m_nSEARCH_MODE ;

						g_NuroCore->SEARCHSDKCallBack(pParam);
		
						if(data)
						{
							delete []data;
							data = NULL;
						}
						
					}
				}
				else
				{
					
					nuINT nCount = LibSH_GetRoadCrossTotal();
					__android_log_print(ANDROID_LOG_INFO, "SEARCH_SDK", "cROSS COUNT %d ",nCount);
					if(nCount>0)
					{
						SDK_LISTDATA * data = new SDK_LISTDATA[nCount];
						nuMemset(data,NULL,sizeof(SDK_LISTDATA)*nCount);				
						nuINT k = 0;					
						while(k < nCount)
						{
							nuWCHAR  *m_pwListBuffer   = new nuWCHAR[128];
						 	nuMemset(m_pwListBuffer,NULL,sizeof(nuWCHAR)*128);
							nuWCHAR wsTempCity[128]={0};
							nuWCHAR wsTempTown[128]={0};
							nuroPOINT tempPos = {0};
							if(k==0)
							{
								LibSH_GetRoadCrossList(NURO_SEARCH_NEW,1,
									(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);
							}
							else					
							{
								LibSH_GetRoadCrossList(NURO_SEARCH_NEXT,1,
								(nuBYTE*)m_pwListBuffer,EACH_POI_NAME_LEN);

							}
							//LibSH_SetRoadName(0, false);  // 
							

		bool b_GetTown = LibSH_GetRoadCrossTownName(0,(nuWORD*)wsTempTown, 
			                                                     1*EACH_TOWN_NAME_LEN);// ???^Town????
		bool b_GetCity = LibSH_GetRoadCrossCityName(0,(nuWORD*)wsTempCity, 
									     1*EACH_TOWN_NAME_LEN);// ???^city????
		
							LibSH_GetRoadCrossPos(0, &tempPos);
							
							nuWcscpy(data[k].wsName,m_pwListBuffer);
							nuWcscpy(data[k].wsCityName,wsTempCity);
							nuWcscpy(data[k].wsTownName,wsTempTown);
							data[k].pos = tempPos;

							if(m_pwListBuffer)
							{
								delete []m_pwListBuffer;
								m_pwListBuffer = NULL;
							}
							k++;
						}
						
		
						pParam->lDataLen = nCount*sizeof(SDK_LISTDATA);
						pParam->pVOID = data;
						pParam->lCallID = m_nSEARCH_MODE ;

						g_NuroCore->SEARCHSDKCallBack(pParam);
		
						if(data)
						{
							delete []data;
							data = NULL;
						}
						
					}
				}
				}
				
				break;
			}
									
		}
		break;
	}

	return nuTRUE;
}
SEARCHNEW_API nuINT   LibSH_GetNEXTAll(nuVOID* pkeyStr, nuUINT& WordCont)
{

	if( m_nSEARCH_MODE != SET_SEARCH_ALL )
	{
		 __android_log_print(ANDROID_LOG_INFO, "sdkinfo", "g_pSearch->GetNEXTAll ");
		return g_pSearch->GetNEXTAll(pkeyStr, WordCont);
	}
	else  //if(g_pSearchNewDY)
	{
		 __android_log_print(ANDROID_LOG_INFO, "sdkinfo", "g_pSearchNewDY->GetNEXTAll ");
		return g_pSearchNewDY->GetNEXTAll(pkeyStr, WordCont); 
	}
}
SEARCHNEW_API nuUINT LibSH_SHNewDYStart()
{
	if (g_pSearch)
	{
		delete g_pSearch;
		g_pSearch = NULL;
	}
	if (g_nMMIdx!=(nuWORD)-1)
	{
		g_pShMM->MM_RelDataMemMass(&g_nMMIdx);
		g_nMMIdx = -1;
	}
	g_pShMM->MM_CollectDataMem(0);
	if( g_pSearchNewDY == NULL )
	{
		g_pSearchNewDY = new CSearchNewDanyinP();
		if( g_pSearchNewDY == NULL )
		{
			return 0;
		}
		if( !g_pSearchNewDY->Initialize() )
		{
			delete g_pSearchNewDY;
			g_pSearchNewDY = NULL;
			return 0;
		}
	}
	return 1;
}
SEARCHNEW_API nuUINT LibSH_SHNewDYStop()
{
	if( g_pSearchNewDY != NULL )
	{
		delete g_pSearchNewDY;
		g_pSearchNewDY = NULL;
		if (g_nMMLen)
		{
			return (nuUINT)LibSH_StartSearch(-1, g_nMMLen);
		}
		return 1;
	}
	return 0;
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetSearchMode(nuINT nMode) 
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetSearchMode(nMode);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetSearchRegion(nSetMode, nData);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetSearchDanyin(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyin2(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetSearchDanyin2(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYGetHead(PDYCOMMONFORUI pDyHead)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDYDataHead(pDyHead);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYGetWordData(PDYWORDFORUI pDyData)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDYGetWordData(pDyData);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyinKeyname(nuWCHAR *pWsDy, nuBYTE nWsNum)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetPoiKeyName(pWsDy, nWsNum);
}
SEARCHNEW_API nuUINT LibSH_SHNewDYSetKey(nuWCHAR *pWsDy, nuBYTE nWsNum,nuBOOL bFindALL)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->SetSearchKEY(pWsDy,nuWcslen(pWsDy),bFindALL);
}

					
SEARCHNEW_API nuUINT LibSH_SHNewDYGetPageData_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDYDataPage_WORD(pDyDataForUI, nItemIdx);
}
SEARCHNEW_API nuBOOL LibSH_GetNewDYGetALLData(nuPVOID buffer,nuINT &nCount)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDYALLData(buffer,nCount);
}
SEARCHNEW_API nuINT LibSH_GetNewDYPoiTotal()
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetPoiTotal();
}
#ifdef ZENRIN
SEARCHNEW_API nuINT LibSH_GetNewDYDataCOUNT()
{
#ifdef NDY
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDYData_COUNT();
#endif
	return 0;
}
#endif
SEARCHNEW_API nuUINT LibSH_SHNewDYGetOneDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetDyDataDetail(pDyDetailForUI, nIdx);;
}
SEARCHNEW_API nuUINT LibSH_SHNewDYGetPoiTotal()
{
	if( g_pSearchNewDY == NULL )
	{
		return 0;
	}
	return g_pSearchNewDY->GetPoiTotal();
}
SEARCHNEW_API nuINT LibSH_SetAreaCenterPos(nuroPOINT pos)
{
	if (g_pSearch)
	{
		return 	g_pSearch->SetAreaCenterPos(pos);
	}
	return nuFALSE;
}
