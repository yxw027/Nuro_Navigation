
#include "SearchY.h"
#include "NuroDefine.h"

CSearchY::CSearchY()
{
    m_nLoadBufferIdx = -1;
    m_nLoadBufferMaxLen = 0;
	g_nSKBMMIdx = -1;
	nuMemset(&m_pos,0,sizeof(m_pos));
}

CSearchY::~CSearchY()
{
    Free();
}

nuBOOL CSearchY::Init(nuWORD nIdx, nuUINT nLen)
{
    if (!m_pMmApi || !m_pFsApi)
    {
        return nuFALSE;
    }
    if (!m_pMmApi->MM_GetDataMassAddr(nIdx) || !nLen)
    {
        return nuFALSE;
    }
    m_nLoadBufferIdx = nIdx;
    m_nLoadBufferMaxLen = nLen;
    m_searchInfo.nMapID = -1;
    m_searchInfo.nCityID = -1;
    m_searchInfo.nTownID = -1;
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    if (!SetMapId(0))
    {
        return nuFALSE;
    }
    return nuTRUE;
}

nuVOID CSearchY::Free()
{
	/*if(g_nSKBMMIdx!=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&g_nSKBMMIdx);
		g_nSKBMMIdx=-1;
	}*/
	ResetAllSearch();
}
nuVOID CSearchY::ResetAllSearch()
{
    QuitSearchPoi();
    QuitSearchRoad();
}
nuBOOL CSearchY::SetMapId(nuLONG mapid)
{
    ResetAllSearch();
    m_searchInfo.nCityID = -1;
    m_searchInfo.nTownID = -1;
    m_searchInfo.nMapID = mapid;
    m_nLB_CTLen = m_SearchArea.LoadCBInfo(m_nLoadBufferIdx, m_nLoadBufferMaxLen);
    if (!m_nLB_CTLen)
    {
        return nuFALSE;
    }
    return nuTRUE;
}
nuBOOL CSearchY::SetCityId(nuWORD resIdx)
{
    if (m_searchInfo.nMapID == (nuLONG)-1)
    {
        return nuFALSE;
    }
    m_SearchPoi.SetSearchArea(NULL);
    m_searchInfo.nCityID = resIdx;
    m_searchInfo.nTownID = -1;
    return nuTRUE;
}
nuBOOL CSearchY::SetTownId(nuWORD resIdx)
{
    if (m_searchInfo.nMapID == (nuLONG)-1	|| m_searchInfo.nCityID == (nuWORD)-1)
    {
        return nuFALSE;
    }
    m_searchInfo.nTownID = resIdx;
    return nuTRUE;
}
nuBOOL CSearchY::SetPoiTP1(nuUINT resIdx)
{
    if (!StartSearchPoi())
    {
        return nuFALSE;
    }
    m_searchInfo.nTP1ID = resIdx;
    m_searchInfo.nTP2ID = -1;
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    m_SearchPoi.SetPoiKeyDanyin(NULL);
    m_SearchPoi.SetPoiKeyPhone(NULL);
    m_SearchPoi.SetPoiCarFac(-1);
    m_SearchPoi.SetPoiSPType(-1);
    return nuTRUE;
}
/*nuBOOL CSearchY::SetPoiTP1(nuUINT resIdx,nuUINT word)
{
    if (!StartSearchPoi(word))
    {
        return nuFALSE;
    }
    m_searchInfo.nTP1ID = resIdx;
    m_searchInfo.nTP2ID = -1;
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    m_SearchPoi.SetPoiKeyDanyin(NULL);
    m_SearchPoi.SetPoiKeyPhone(NULL);
    m_SearchPoi.SetPoiCarFac(-1);
    m_SearchPoi.SetPoiSPType(-1);
    return nuTRUE;
}*/

nuBOOL CSearchY::SetPoiTP2(nuUINT resIdx)
{
    if (m_searchInfo.nTP1ID == (nuBYTE)-1)
    {
        return nuFALSE;
    }
    m_searchInfo.nTP2ID	= resIdx;
    return nuTRUE;
}
nuBOOL CSearchY::SetPoiKeyDanyin(nuWCHAR *pDanyin,nuBOOL bComplete)
{
    if (!StartSearchPoi())
    {
        return nuFALSE;
    }
    return m_SearchPoi.SetPoiKeyDanyin(pDanyin,bComplete);
}
nuBOOL CSearchY::SetPoiKeyDanyin2(nuWCHAR *pDanyin,nuBOOL bComplete)
{
	if (pDanyin==0 )       //Prosper add for load before input
	{
		if(StartSearchPoi())
			return m_SearchPoi.SetPoiKeyDanyin2(pDanyin,bComplete);
	}
	if (!StartSearchPoi())
	{
		return nuFALSE;
	}
	return m_SearchPoi.SetPoiKeyDanyin2(pDanyin,bComplete);
}
nuBOOL CSearchY::SetPoiKeyName(nuWCHAR *pName, nuBOOL bFuzzy)
{
	if (!StartSearchPoi())
    {
        return nuFALSE;
    }
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    if (!pName || !pName[0])
    {
        return nuFALSE;
    }
	m_SearchPoi.SetKeyNameFuzzy(bFuzzy);
    m_SearchPoi.SetPoiKeyDanyin(NULL);
    m_SearchPoi.SetPoiKeyPhone(NULL);
    m_SearchPoi.SetPoiCarFac(-1);
    m_SearchPoi.SetPoiSPType(-1);
#ifndef _USE_PANA_MODE_
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
#endif
    if (pName)
    {
        nuUINT len = nuWcslen(pName);
        if (!len || len>=256)
        {
            return nuFALSE;
        }
        nuMemcpy(m_searchInfo.name, pName, len*2);
    }
    return nuTRUE;
}
nuBOOL CSearchY::SetPoiKeyNameP(nuWCHAR *pName, nuBOOL bFuzzy)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetPoiKeyName(pName, bFuzzy);
    }

    return nuFALSE;
}
nuBOOL CSearchY::SetPoiKeyPhone(nuWCHAR *pPhone)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetPoiKeyPhone(pPhone);
    }
    return nuFALSE;
}
nuBOOL CSearchY::SetSearchArea(NURORECT *pRect)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetSearchArea(pRect);
    }
    return nuFALSE;
}
nuBOOL CSearchY::SetRoadSZSLimit(nuUINT nMin, nuUINT nMax)
{
#ifdef _USE_SEARCH_ROAD_
	if (nMin==0 && nMax==0) //Prosper add for load before input
	{
		if(StartSearchRoad())
			return m_SearchRoad.SetRoadSZSLimit(nMin, nMax);
	}
	if (StartSearchRoad())
    {
        return m_SearchRoad.SetRoadSZSLimit(nMin, nMax);
    }
#endif
    return nuFALSE;
}
nuBOOL CSearchY::SetRoadSZZhuyin(nuWORD zhuyin)
{
#ifdef _USE_SEARCH_ROAD_
    if (zhuyin==0)			//Prosper add for load before input
    {
		if(StartSearchRoad())
			return m_SearchRoad.SetRoadSZZhuyin(0);
    }
	else
	{
		if(StartSearchRoad())
		return m_SearchRoad.SetRoadSZZhuyin(zhuyin);
	}
	
#endif
    return nuFALSE;
    
}
nuBOOL CSearchY::SetPoiPZZhuyin(nuWORD zhuyin)
{
#ifdef _USE_SEARCH_ROAD_
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetPoiPZZhuyin(zhuyin);
    }
#endif
    return nuFALSE;
    
}
nuBOOL CSearchY::SetRoadSZ(nuUINT nResIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.SetRoadSZ(nResIdx);
#endif
    return nuFALSE;
}
nuBOOL CSearchY::SetPoiPZ(nuUINT nResIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchPoi.SetPoiPZ(nResIdx);
#endif
    return nuFALSE;
}
nuBOOL CSearchY::SetRoadKeyDanyin(nuWORD *pDanyin)
{
#ifdef _USE_SEARCH_ROAD_
    if (StartSearchRoad())
    {
        return m_SearchRoad.SetRoadKeyDanyin(pDanyin);
    }
#endif
    return nuFALSE;
}
nuBOOL CSearchY::SetRoadKeyDanyin2(nuWORD *pDanyin)
{
#ifdef _USE_SEARCH_ROAD_
	if (pDanyin==0 )       //Prosper add for load before input
	{
		if(StartSearchRoad())
			return m_SearchRoad.SetRoadKeyDanyin2(pDanyin);
	}
	if (StartSearchRoad())
	{
		return m_SearchRoad.SetRoadKeyDanyin2(pDanyin);
	}
#endif
	return nuFALSE;
}
nuBOOL CSearchY::SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging/* = nuFALSE*/)
{
#ifdef _USE_SEARCH_ROAD_
    if (StartSearchRoad())
    {
        return m_SearchRoad.SetRoadKeyName(pKeyWord, bIsFromBeging);
    }
#endif
    return nuFALSE;
}
nuBOOL CSearchY::SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.SetRoadName(nResIdx, bRealIdx);
#endif
    return nuFALSE;
}

nuVOID CSearchY::SetStrokeLimIdx(nuUINT nResIdx)
{
    m_nStrokeLimIdx = nResIdx;
}

nuUINT CSearchY::GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.GetPoiCarFacAll(pBuffer, nMaxLen);
    }
    return 0;
}
nuBOOL CSearchY::SetPoiCarFac(nuUINT nResIdx)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetPoiCarFac(nResIdx);
    }
    return nuFALSE;
}
nuUINT CSearchY::GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.GetPoiSPTypeAll(pBuffer, nMaxLen);
    }
    return 0;
}
nuBOOL CSearchY::SetPoiSPType(nuUINT nResIdx)
{
    if (StartSearchPoi())
    {
        return m_SearchPoi.SetPoiSPType(nResIdx);
    }
    return nuFALSE;
}
nuUINT	CSearchY::GetCityTotal()
{
    return m_SearchArea.GetCityTotal();
}
nuUINT	CSearchY::GetTownTotal()
{
    return m_SearchArea.GetTownTotal();
}
nuUINT	CSearchY::GetPoiTotal()
{
    return m_SearchPoi.GetPoiTotal();
}
/*nuUINT	CSearchY::GetPoiTotal(nuUINT temp)
{
    return m_SearchPoi.GetPoiTotal(-1);
}*/
nuUINT	CSearchY::GetTP1Total()
{
    return m_SearchPoi.GetTP1Total();
}
nuUINT	CSearchY::GetRoadSZTotal()
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadSZTotal();
#else
    return 0;
#endif
}
nuUINT	CSearchY::GetPoiPZTotal()
{
#ifdef _USE_SEARCH_ROAD_
    //return m_SearchRoad.GetRoadSZTotal();
	return m_SearchPoi.GetPoiPZTotal();
#else
    return 0;
#endif
}
nuUINT	CSearchY::GetRoadTotal()
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadTotal();
#else
    return 0;
#endif
}
nuUINT	CSearchY::GetRoadCrossTotal()
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadCrossTotal();
#else
    return 0;
#endif
}

nuUINT CSearchY::GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords)
{
    if (m_SearchPoi.IsRun())
    {
        return m_SearchPoi.GetNextWordAll(pHeadWords, pBuffer, nMaxWords);
    }
    else if (m_SearchRoad.IsRun())
    {
        return m_SearchRoad.GetNextWordAll(pHeadWords, pBuffer, nMaxWords);
    }
    return 0;
}

int CSearchY::GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetMapNameAll(pBuffer, nMaxLen);
}
int CSearchY::GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetCityNameAll(pBuffer, nMaxLen);
}
int CSearchY::GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetTownNameAll(pBuffer, nMaxLen);
}

int CSearchY::GetFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuMemset(pBuffer, 0, nMaxLen);
    if (m_SearchPoi.IsRun())
    {
        return m_SearchPoi.GetPoiFilterResAll(pKey, pBuffer, nMaxLen);
    }
    else if (m_SearchRoad.IsRun())
    {
        return m_SearchRoad.GetRoadFilterResAll(pKey, pBuffer, nMaxLen);
    }
    else
    {
        return 0;
    }
}
nuBOOL CSearchY::StartSearchPoi()
{
    if (!m_SearchPoi.IsRun())
    {
        QuitSearchRoad();
        StopSKBSearch();
		#ifdef VALUE_EMGRT
		//nuBYTE *pMM = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(30000, &g_nSKBMMIdx);
		#endif
		nuBOOL bRes = nuFALSE;
		bRes= m_SearchPoi.Init(m_nLoadBufferIdx, m_nLoadBufferMaxLen, m_nLB_CTLen);
		#ifdef VALUE_EMGRT
			m_SearchPoi.Load();
		#endif
		__android_log_print(ANDROID_LOG_INFO, "pos", " CSearchY::StartSearchPoi ....... %d %d ",m_pos.x,m_pos.y);
		m_SearchPoi.SetAreaCenterPos(m_pos);
	
		return bRes;
    }
    return nuTRUE;
}

nuVOID CSearchY::QuitSearchPoi()
{
    if (m_SearchPoi.IsRun())
    {
        m_SearchPoi.Free();
    }
}
nuBOOL CSearchY::GetPoiMoreInfo(nuUINT nResIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx)
{
    if (!pPaInfo)
    {
        return nuFALSE;
    }
    nuMemset(pPaInfo, 0, sizeof(SEARCH_PA_INFO));
    return m_SearchPoi.GetPoiMoreInfo(nResIdx, pPaInfo, bRealIdx);
}
nuBOOL CSearchY::GetPoiPos(nuUINT nResIdx, NUROPOINT* pos, nuBOOL bRealIdx)
{
    if (!pos)
    {
        return nuFALSE;
    }
    return m_SearchPoi.GetPoiPos(nResIdx, pos, bRealIdx);
}
nuBOOL CSearchY::GetPoiCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
    if (!pBuffer)
    {
        return nuFALSE;
    }
    return m_SearchArea.GetCityNameByID(m_SearchPoi.GetPoiCityID(nResIdx, bRealIdx)
        , pBuffer
        , nMaxLen);
}
nuLONG CSearchY::GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx) // Added by Damon 20100309
{
	return m_SearchPoi.GetPoiCityID(nResIdx, bRealIdx);
}
nuBOOL CSearchY::GetPoiTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
    if (!pBuffer)
    {
        return nuFALSE;
    }
    return m_SearchArea.GetTownNameByID(m_SearchPoi.GetPoiCityID(nResIdx, bRealIdx)
        , m_SearchPoi.GetPoiTownID(nResIdx)
        , pBuffer
        , nMaxLen);
}

nuUINT CSearchY::GetPoiNearDis(nuUINT nResIdx, nuBOOL bRealIdx)
{
	return m_SearchPoi.GetPoiNearDis(nResIdx, bRealIdx);
}

nuBOOL CSearchY::GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo)
{
    return m_SearchPoi.GetPoiByPhone(pPhone, pInfo);
}

int CSearchY::GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen)
{
    if (!pBuffer)
    {
        return 0;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchPoi.GetPoiTP1All(pBuffer, nMaxLen);
}
int CSearchY::GetPoiTP2All(nuVOID* pBuffer, nuUINT nMaxLen)
{
    if (!pBuffer)
    {
        return 0;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchPoi.GetPoiTP2All(pBuffer, nMaxLen);
}

int	CSearchY::GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{	
    if (!pBuffer)
    {
        return 0;
    }
    return m_SearchPoi.GetPoiNameList(type, nMax, pBuffer, nMaxLen);
}
/*
int	CSearchY::GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen,nuUINT word)
{	
    if (!pBuffer)
    {
        return 0;
    }
    return m_SearchPoi.GetPoiNameList(type, nMax, pBuffer, nMaxLen,word);
}*/
nuBOOL CSearchY::StartSearchRoad()
{
#ifdef _USE_SEARCH_ROAD_
    if (!m_SearchRoad.IsRun())
    {
        QuitSearchPoi();
        StopSKBSearch();
        #ifdef VALUE_EMGRT
	//	nuBYTE *pMM = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(30000, &g_nSKBMMIdx);
        #endif
        return m_SearchRoad.Init(m_nLoadBufferIdx, m_nLoadBufferMaxLen
            , m_nLB_CTLen, &m_SearchArea);
    }
    else
    {
        return nuTRUE;
    }
#endif
    return nuFALSE;
}
nuVOID CSearchY::QuitSearchRoad()
{
#ifdef _USE_SEARCH_ROAD_
    m_SearchRoad.Free();
#endif
}
int CSearchY::GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return 0;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchRoad.GetRoadSZList(type, nMax, pBuffer, nMaxLen);
#else
    return 0;
#endif
}
int CSearchY::GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return 0;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchPoi.GetPoiPZList(type, nMax, pBuffer, nMaxLen);
#else
    return 0;
#endif
}
int CSearchY::GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return 0;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchRoad.GetRoadNameList(type, nMax, pBuffer, nMaxLen);
#else
    return 0;
#endif
}

nuUINT CSearchY::GetRoadDoorTotal()
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadDoorTotal();
#else
    return 0;
#endif
}

nuBOOL CSearchY::GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return nuFALSE;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchRoad.GetRoadDoorInfo(pBuffer, nMaxLen);
#else
    return nuFALSE;
#endif
}
int CSearchY::GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return 0;
    }
    return m_SearchRoad.GetRoadCrossList(type, nMax, pBuffer, nMaxLen);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pos)
    {
        return nuFALSE;
    }
    return m_SearchRoad.GetRoadCrossPos(nResIdx, pos);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadCrossTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return nuFALSE;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetTownNameByID(m_SearchRoad.GetRoadCrossCityID(nResIdx)
        , m_SearchRoad.GetRoadCrossTownID(nResIdx)
        , pBuffer
        , nMaxLen);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadCrossCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return nuFALSE;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetCityNameByID(m_SearchRoad.GetRoadCrossCityID(nResIdx)
        , pBuffer
        , nMaxLen);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadPos(NUROPOINT* pos)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pos)
    {
        return nuFALSE;
    }
    return m_SearchRoad.GetRoadPos(pos);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadTownName(nuWORD *pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return nuFALSE;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetTownNameByID(m_SearchRoad.GetRoadCityID()
        , m_SearchRoad.GetRoadTownID()
        , pBuffer
        , nMaxLen);
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchY::GetRoadCityName(nuWORD *pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    if (!pBuffer)
    {
        return nuFALSE;
    }
    nuMemset(pBuffer, 0, nMaxLen);
    return m_SearchArea.GetCityNameByID(m_SearchRoad.GetRoadCityID()
        , pBuffer
        , nMaxLen);
#else
    return nuFALSE;
#endif
}
nuUINT	CSearchY::GetMainRoadTotal()
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadTotal_MR();
#else
    return 0;
#endif
}
nuUINT CSearchY::GetChildRoadTotal(nuUINT nMainRoadIdx, nuBOOL bRealIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetChildRoadTotal(nMainRoadIdx, bRealIdx);
#else
    return 0;
#endif
}
int	CSearchY::GetRoadNameList_MR(NURO_SEARCH_TYPE type
                                 , nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadNameList_MR(type, nMax, (nuBYTE*)pBuffer, nMaxLen);
#else
    return 0;
#endif
}

nuUINT	CSearchY::GetRoadNameALL_MR(nuUINT nMainRoadIdx, nuVOID* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.GetRoadNameALL_MR(nMainRoadIdx, (nuBYTE*)pBuffer, nMaxLen, bRealIdx);
#else
    return 0;
#endif
}

nuBOOL	CSearchY::SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx)
{
#ifdef _USE_SEARCH_ROAD_
    return m_SearchRoad.SetRoadName_MR(nMainRoadIdx, nRoadIdx, bRealIdx);
#else
    return nuFALSE;
#endif
}


nuVOID CSearchY::Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort)
{
	 m_SearchRoad.Stroke_GetNextWordData((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);	
}

nuVOID CSearchY::ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort)
{	
	m_SearchRoad.ZhuYinForSmart_SecondWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,candylen,ReSort);
}
nuBOOL CSearchY::ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort)
{
	return m_SearchRoad.ZhuYinForSmart_OtherWords((nuBYTE*)pRoadBuf,RoadBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}
nuVOID CSearchY::Stroke_Poi_GetNextWordData(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort)
{
	 m_SearchPoi.Stroke_GetNextWordData((nuBYTE*)pPoiBuf,PoiBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);	
}
nuVOID CSearchY::ZhuYinForSmart_Poi_SecondWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort)
{	
	m_SearchPoi.ZhuYinForSmart_SecondWords((nuBYTE*)pPoiBuf,PoiBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,candylen,ReSort);
}
nuBOOL CSearchY::ZhuYinForSmart_Poi_OtherWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort)
{
	return m_SearchPoi.ZhuYinForSmart_OtherWords((nuBYTE*)pPoiBuf,PoiBufLen,(nuBYTE*)pWordBuf,WordLen,WordCont,(nuBYTE*)candi,candylen,ReSort);
}

nuINT CSearchY::GetRoadTownID()
{
	return m_SearchRoad.GetRoadTownID();
}

nuBOOL CSearchY::GetMode(VOICEROADFORUI& data,nuVOID* wName,nuBOOL &bMode)
{
	nuWCHAR wUserCityBuffer[PROTWONMAXCOUNT*EACH_CITY_NAME_LEN] = {0};
	//nuMemset(wUserCityBuffer, NULL, sizeof(wUserCityBuffer));

	nuINT iCITYGet = GetCityNameAll(wUserCityBuffer, PROTWONMAXCOUNT*EACH_CITY_NAME_LEN); // 

	nuBOOL bCheckCity=nuFALSE;
	nuINT nCityIdx=0;
	for(;nCityIdx<iCITYGet;nCityIdx++)
	{
		if(nuWcsncmp(data.wCITY,wUserCityBuffer+nCityIdx*(EACH_CITY_NAME_LEN/2),3)==0)
		{	
			bCheckCity=nuTRUE;
			break;
		}

	}
	if(!bCheckCity)
	{
		return nuFALSE;
	}


	SetCityId(nCityIdx);
	data.nCity=nCityIdx;

	nuWCHAR wUserTownBuffer[PROTWONMAXCOUNT*EACH_TOWN_NAME_LEN] = {0};
	//nuMemset(wUserTownBuffer, NULL, sizeof(wUserTownBuffer));

	nuINT iTOWNGet =GetTownNameAll(wUserTownBuffer, PROTWONMAXCOUNT*EACH_TOWN_NAME_LEN);    
	nuBOOL bCheckTown=nuFALSE;
	nuINT nTownIdx=0;
	nuINT nTownLen=0;
	nuWCHAR temp[EACH_TOWN_NAME_LEN]={0};

	for(;nTownIdx<iTOWNGet;nTownIdx++)
	{
		nuMemcpy(temp,wUserTownBuffer+nTownIdx*(EACH_TOWN_NAME_LEN/2),EACH_TOWN_NAME_LEN);

		if(nuWcsncmp(data.wTOWN,temp,nuWcslen(temp))==0)
		{
			bCheckTown=nuTRUE;
			nTownLen=nuWcslen(data.wTOWN);
			break;
		}

	}
	if(!bCheckTown)
	{
		data.nTown=-1;
		SetTownId(data.nTown);
		//return nuFALSE;
		//LibSH_SetTownId(nTownIdx);
	}
	else
	{
		SetTownId(nTownIdx);
		data.nTown=nTownIdx;
	}
	nuBOOL  bSpecial=nuFALSE;
	nuBOOL	bDoornumber=nuFALSE;
	nuBOOL	bNumberCombine=nuFALSE;
	nuBOOL  bNeedPlaceName=nuFALSE;
	//nuBOOL  bNumber3=false;

	nuINT nDoorNum  =0;
	nuINT nDoorNum2 =0;
	
	nuWCHAR wROAD[128]={0};
	
	nuWCHAR wENDWORD[]={27573,24055,24324,20043,34399,0};//L"\ACq\ABѧˤ\A7\B8\B9";
	
	nuINT nLen=0;
	nLen=nuWcslen(data.wROAD1);
	if(nLen>0) //place or road 
	{
		nuBOOL bFind = nuFALSE;
		for(nuINT i=0;i<nLen;i++)
		{
			if(data.wROAD1[i] == 36335 || data.wROAD1[i] == 34903)
			{
				bFind = nuTRUE;
				break;
			}	
		}
		if(bFind)
		{
			bSpecial=nuFALSE;
		}		
		nuWcscat(wROAD,data.wROAD1);		
	}
	else
	{
		bSpecial=nuTRUE;
	}

	nLen=nuWcslen(data.wROAD2);
	if(nLen>0)
	{
		//\ABD\BCƦr\AB\D1
		for(int i=0;i<nLen-1;i++)
		{
			if(data.wROAD2[i]<48 ||  data.wROAD2[i]>57)
			{
				bSpecial=true;
			}
		}
		nuWcscat(wROAD,data.wROAD2);
		//nLen=nuWcslen(wROAD);
		//wROAD[nLen]=wENDWORD[1];
	}

	nLen=nuWcslen(data.wROAD3);
	if(nLen>0 && !bNeedPlaceName)
	{
		//\ABD\BCƦr\A7\CB
		for(int i=0;i<nLen-1;i++)
		{
			if(data.wROAD3[i]<48 ||  data.wROAD3[i]>57)
			{
				bSpecial=true;
			}
		}

		nuWcscat(wROAD,data.wROAD3);
		//nLen=nuWcslen(wROAD);
		//wROAD[nLen]=wENDWORD[2];
	}
	
	nLen=nuWcslen(data.wDOORNUM1);
	if(data.wDOORNUM1[nLen-1] == wENDWORD[4] )
	{		
		bDoornumber=true;
		nuBOOL	bNum2=false;
		nuINT j=0;
		nuINT jj=0;

		nuINT nNum1,nNum2;
		nNum1=nNum2=0;
		nuINT i=0;
		for( i = nLen-2 ; nLen > 0;i--)
		{
			if(data.wDOORNUM1[i] > 47 &&  data.wDOORNUM1[i] < 58 )
			{
				if(!bNum2)
				{
					nuINT nNum=(data.wDOORNUM1[i]-48);
					for(int k=0;k<j;k++)
					{
						nNum*=10;
					}
					nNum1+=nNum;
				}
				else
				{
					nuINT nNum=(data.wDOORNUM1[i]-48);
					for(int k=0;k<j;k++)
					{
						nNum*=10;
					}
					nNum2+=nNum;
				}
				j++;
			}
			if( data.wDOORNUM1[i] == wENDWORD[3] )
			{
				bNum2=true;
				j=0;
				continue;
			}
			if(data.wDOORNUM1[i] <48 ||  data.wDOORNUM1[i] > 57)
			{
				break;
			}
		}

		if(!bNum2)
		{
			data.nDoorNum1 = nNum1;
			data.nDoorNum2 = 0;
		}
		else
		{
			data.nDoorNum1 = nNum2;
			data.nDoorNum2 = nNum1;
		}
	}

	nLen=nuWcslen(wROAD);
	wROAD[nLen]=0;
	
	nuMemcpy(wName,wROAD,sizeof(wROAD));
	
	

	bMode = bSpecial;

	return nuTRUE;
}


nuBOOL	CSearchY::GetPosInfo(nuBOOL &bNumChange,nuINT &nDoorNum,nuINT nDoorNum2,nuroPOINT &point)
{
	nuINT iPDNNODECOUNT =   GetRoadDoorTotal();

	if(iPDNNODECOUNT==0 || nDoorNum == 0) // 
	{
		GetRoadPos(&point);
		return true;
	}

	nuBYTE* pbyPdn = NULL;
	if (NULL != pbyPdn)
	{
		delete []pbyPdn;
	}
	pbyPdn = new nuBYTE[sizeof(SEARCH_PDN_NODE)+iPDNNODECOUNT*sizeof(SEARCH_PDN_NODE_DI)];
		
	nuBOOL bHavePdn =  GetRoadDoorInfo(pbyPdn, 
									sizeof(SEARCH_PDN_NODE)+iPDNNODECOUNT*sizeof(SEARCH_PDN_NODE_DI));

	SEARCH_PDN_NODE* psPdn = (SEARCH_PDN_NODE*)pbyPdn;

	SEARCH_PDN_NODE_DI* pPdnNodeDI = psPdn->di;
	nuroPOINT temp	=	{0};
	nuBOOL    bFIND	=	nuFALSE;

	if(nDoorNum > psPdn->DoorRangeMax)
	{
		bNumChange = nuTRUE;
		nDoorNum = psPdn->DoorRangeMax;
	}
	if(nDoorNum < psPdn->DoorRangeMin)
	{
		bNumChange = nuTRUE;
		nDoorNum = psPdn->DoorRangeMin;
	}
	// -------------
		for (nuINT l=0; l < (nuINT)psPdn->doorNoCount; l++)
		{
			
			if(nDoorNum > (nuINT)pPdnNodeDI[l].doorNumber)  //EX:??J??P 9??
			{
				temp = pPdnNodeDI[l].pos;
			}
			if(nDoorNum == (nuINT)pPdnNodeDI[l].doorNumber && nDoorNum2 == (nuINT)pPdnNodeDI[l].doorNumber2)  //EX:??J??P 9??
			{
				point = pPdnNodeDI[l].pos;
				bFIND = true;
				break;
			}
		
		}	
		if(!bFIND)
		{
			point = temp;	
		}
		if (NULL != pbyPdn)
		{
			delete []pbyPdn;
			pbyPdn = NULL;
		}
		return true;
}
nuBOOL CSearchY::SetNoName(nuBOOL bRes)
{
	return m_SearchRoad.SetNoName(bRes);
}
nuBOOL CSearchY::GetRoadSKB(nuVOID* pSKBBuf, nuUINT& nCount)             //Prosper 20111226
{
		return m_SearchRoad.GetRoadSKB((nuBYTE*) pSKBBuf, nCount);
		
}
nuBOOL CSearchY::GetPoiSKB(nuVOID* pSKBBuf, nuUINT& nCount)				//Prosper 20111226
{
	return m_SearchPoi.GetPoiSKB((nuBYTE*) pSKBBuf, nCount);

}
nuBOOL CSearchY::GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen)
{
	return m_SearchRoad.GetRoadWordData(nTotal, (nuBYTE*)pBuffer, nMaxLen);
}
  nuINT     CSearchY::GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount)
    {
		#ifdef NURO_OS_WINCEX  
	/*	FILE *fp = fopen("SDMMC\\GetSKBAll.txt","a+");
		fprintf(fp,"@GetSKBAll ..........\n");
		fclose(fp);
	*/	#endif

        if (m_searchInfo.nCityID==(nuWORD)-1)
        {
            return 0;
        }		
        if (   m_searchInfo.nMapID  != m_SKBMapIDT   //\C8\E7\B9\FB\D0\E8Ҫ\CB\D1\CB\F7
            || m_searchInfo.nCityID != m_SKBCityIDT
            || m_searchInfo.nTownID != m_SKBTownIDT
            || nuStrcmp(keyStr, m_SKBKeyStrT))
        {
            nuBOOL bReload = nuFALSE;
            m_SKBResT = 0;
            if (m_searchInfo.nMapID != m_SKBMapIDT
                || m_searchInfo.nCityID != m_SKBCityIDT)
            {
				if(g_nSKBMMIdx!=(nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&g_nSKBMMIdx);
					g_nSKBMMIdx=-1;
				}
				if(g_nSKBMMIdx2!=(nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&g_nSKBMMIdx2);
					g_nSKBMMIdx2=-1;
				}
                bReload = nuTRUE;
            }
            nuINT keyLen = nuStrlen(keyStr);
            nuMemset(m_SKBBuffer, 0, sizeof(m_SKBBuffer));
            m_SKBMapIDT = m_searchInfo.nMapID;
            m_SKBCityIDT = m_searchInfo.nCityID;
            m_SKBTownIDT = m_searchInfo.nTownID;
			nuMemset(m_SKBKeyStrT, NULL, sizeof(m_SKBKeyStrT));
            nuMemcpy(m_SKBKeyStrT, keyStr, keyLen);
			
            

          
		   nuCHAR* pB1 = NULL;
		   nuCHAR* pB2 = NULL;
			if (bReload)
            {
                m_SKBLoadLen = 0;
                nuUINT pos = 0;
                CMapFileSKB skbfile;
                if (!skbfile.Open(m_SKBMapIDT))
                {
                    return 0;
                }
                if (!skbfile.GetCityInfo(m_SKBCityIDT, infoT))
                {
                    return 0;
                }
                if (m_SearchRoad.IsRun())
                {
                    pos = infoT.roadPos;
                    count = infoT.roadCount;
                }
                else
                {
                    pos = infoT.poiPos;
                    count = infoT.poiCount;
                }
				#ifdef NURO_OS_WINCEX  
				FILE *fp = fopen("SDMMC\\GetSKBAll.txt","a+");
				fprintf(fp,"Before new pSkb1..........\n");
				fclose(fp);
				#endif

      			nuCHAR *pMM = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(infoT.poiCount*sizeof(nuCHAR), &g_nSKBMMIdx);
			    if (!skbfile.ReadData(pos, pMM, count))
                {
                    return 0;
                }
				pB1 = pMM;
     		
				#ifdef NURO_OS_WINCEX  
				fp = fopen("SDMMC\\GetSKBAll.txt","a+");
				fprintf(fp,"After new pSkb1..........\n");
				fclose(fp);
				#endif

                nuUINT i = 0;
                for (; i<count; ++i)
                {
                    m_SKBLoadLen += pB1[i]+2;
                }
				//nuCHAR *p2 = new nuCHAR[m_SKBLoadLen]; // GET DATA
            	#ifdef NURO_OS_WINCEX  
				fp = fopen("SDMMC\\GetSKBAll.txt","a+");
				fprintf(fp,"Before skbfile.ReadData..........\n");
				fclose(fp);
				#endif
				nuCHAR *pMM2 = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(sizeof(nuCHAR)*m_SKBLoadLen, &g_nSKBMMIdx2);
				if (!skbfile.ReadData(pos+count, pMM2, m_SKBLoadLen))
                {
                    m_SKBLoadLen = 0;
                    return 0;
                }
				#ifdef NURO_OS_WINCEX  
				fp = fopen("SDMMC\\GetSKBAll.txt","a+");
				fprintf(fp,"After skbfile.ReadData..........\n");
				fclose(fp);
				#endif
				skbfile.Close();
				#ifdef NURO_OS_WINCEX  
				fp = fopen("SDMMC\\GetSKBAll.txt","a+");
				fprintf(fp,"close skbfile..........\n");
				fclose(fp);
				#endif
            if (!m_SKBLoadLen)
            {
                return 0;
            }
            if (m_SearchRoad.IsRun())
            {
                count = infoT.roadCount;
            }
            else
            {
                count = infoT.poiCount;
            }
            pB2 = pMM2;// + count;
            nuWORD townID;
			
			#ifdef NURO_OS_WINCEX  
			fp = fopen("SDMMC\\GetSKBAll.txt","a+");
			fprintf(fp,"before townID..........\n");
			fclose(fp);
			#endif
            
			nuMemcpy(&townID, pB2+pB1[0], 2);

			#ifdef NURO_OS_WINCEX  
			fp = fopen("SDMMC\\GetSKBAll.txt","a+");
			fprintf(fp,"after townID..........\n");
			fclose(fp);
			#endif
			
			
			#ifdef NURO_OS_WINCEX  
			fp = fopen("SDMMC\\GetSKBAll.txt","a+");
			fprintf(fp,"NEW OK..........\n");
			fclose(fp);
			#endif
			}
			
			#ifdef NURO_OS_WINCEX  
			FILE *fp = fopen("SDMMC\\GetSKBAll.txt","a+");
			fprintf(fp,"Before JudgeSKB..........\n");
			fclose(fp);
			#endif

			nuCHAR *SKB =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(g_nSKBMMIdx);
			nuCHAR *SKB2 =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(g_nSKBMMIdx2);
            if (JudgeSKB(SKB, keyStr, SKB2[0], keyLen, townID))
            {
                ++m_SKBResT;
            }
			
			#ifdef NURO_OS_WINCEX  
			fp = fopen("SDMMC\\GetSKBAll.txt","a+");
			fprintf(fp,"After JudgeSKB..........\n\n\n");
			fclose(fp);
			#endif

            int i = 0;
			pB2 =SKB2 ;
			pB1 =SKB;
            for (; i<count; )
            {
                pB2 += pB1[i]+2;
                ++i;
                nuMemcpy(&townID, pB2+pB1[i], 2);
                if (JudgeSKB(pB2, keyStr, pB1[i], keyLen, townID))
                {
                    ++m_SKBResT;
                }
            }
			/*if(p != NULL)
			{
				delete[] p;
				p = NULL;
			}*/
			
        }
        if (resCount)
        {
            *resCount = m_SKBResT;
        }
        return GetSKBInBuffer(pBuffer, nMaxLen);
    }
   nuBOOL   CSearchY:: JudgeSKB(nuCHAR* strDes, nuCHAR* strSrc, nuINT nDesLen, nuINT nSrcLen, nuWORD townID)
    {
        nuBOOL bInclude = nuFALSE;
        nuBYTE idx = 0;
        if (!nSrcLen && nDesLen)//\C8\E7\B9\FBKEYΪ\BF\D5 \CC\ED\BC\D3\CA\D7\D7\D6
        {
            idx = strDes[0];
            if (idx>=128)
            {
                return bInclude;
            }
            if (m_SKBTownIDT==(nuWORD)-1
                || m_SKBTownIDT==townID)
            {
                bInclude = nuTRUE;
				
                m_SKBBuffer[idx] = 1;
            }
        }
        else
        {
            while (nDesLen > nSrcLen)
            {
				if (_SEARCH_ROAD_FJUDGE_ENDWORD_ == strDes[0])
				{
					break;
				}
                if (!nuStrncmp(strDes, strSrc, nSrcLen))
                {
                    idx = strDes[nSrcLen];
                    if (idx>=128)
                    {
                        return bInclude;
                    }
                    if (m_SKBTownIDT==(nuWORD)-1
                        || m_SKBTownIDT==townID)
                    {
                        bInclude = nuTRUE;
                        m_SKBBuffer[idx] = 1;
                    }
                }
                --nDesLen;
                ++strDes;
            }
        }
        return bInclude;
    }
    nuINT     CSearchY::GetSKBInBuffer(nuCHAR* pBuffer, nuINT nMaxLen)
    {
        nuINT res = 0;
        nuINT i = 0;
        for (; i<128; ++i)
        {
            if (m_SKBBuffer[i] &&
				i)
            {
                if (pBuffer && res<nMaxLen)
                {
                    pBuffer[res] = i;
                    ++res;
                }
            }
        }
        return res;
    }
	nuBOOL    CSearchY::StartSKBSearch(nuUINT nType)
    {
		if (SH_BEGIN_TYPE_ROAD  == nType ||
			SH_BEGIN_TYPE_CROSS == nType)
		{
			if (m_SearchRoad.IsRun())
			{
				m_SearchRoad.ClearSZYInfo();
			}
			else
			{
				if (!StartSearchRoad())
				{
					return nuFALSE;
				}
			}
		}
		else if (SH_BEGIN_TYPE_POI == nType)
		{
			if (m_SearchPoi.IsRun())
			{
				m_SearchPoi.ClearSearchInfo();
			}
			else
			{
				if (!StartSearchPoi())
				{
					return nuFALSE;
				}
			}
        }
        else
        {
            return nuFALSE;
        }
		
        //StopSKBSearch();
        return nuTRUE;
    }

    nuVOID    CSearchY::StopSKBSearch()
    {
        m_SKBResT = 0;
        m_SKBLoadLen = 0;
        m_SKBMapIDT = -1;
        m_SKBCityIDT = -1;
        m_SKBTownIDT = -1;
        nuMemset(&infoT, 0, sizeof(infoT));
        nuMemset(m_SKBKeyStrT, 0, sizeof(m_SKBKeyStrT));
        nuMemset(m_SKBBuffer, 0, sizeof(m_SKBBuffer));

		if(g_nSKBMMIdx!=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&g_nSKBMMIdx);
		g_nSKBMMIdx=-1;
	}
	if(g_nSKBMMIdx2!=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&g_nSKBMMIdx2);
		g_nSKBMMIdx2=-1;
	}
		/*if(pSkb!=NULL)
		{
			delete []pSkb;
			pSkb = NULL;
		}
		if(pSkb1!=NULL)
		{
			delete []pSkb1;
			pSkb1 = NULL;
		}*/
    }
    nuBOOL CSearchY::GetNEXTAll(nuVOID* pSKBBuf, nuUINT& nCount)             //Prosper 20111226
{
	if(m_SearchRoad.IsRun())
		return m_SearchRoad.GetNEXTWORD((nuBYTE*) pSKBBuf, nCount);
	if(m_SearchPoi.IsRun())
		return m_SearchPoi.GetNEXTWORD((nuBYTE*) pSKBBuf, nCount);
}
nuUINT CSearchY::SetAreaCenterPos(nuroPOINT pos)
{
	m_pos = pos;
	return 1;
}
