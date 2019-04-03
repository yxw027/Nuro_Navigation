
#include "SearchRoad.h"
#include "SearchArea.h"
#include "MapFileSzyY.h"
#include "MapFileRncY.h"
#include "MapFileRnY.h"
#include "MapFileBhY.h"
#include "MapFilePdnY.h"
#include "MapFileRdwY.h"
#include "MapFileRtY.h"
#include "MapFileRefY.h"

CSearchRoad::CSearchRoad()
{
	m_bKickNumber = nuFALSE;
	m_bIsFromBeging = nuFALSE;
	
	m_nListNameIdx   = -1;
	m_nSearchResIdx = -1;
	m_nTownIdx   = -1;
	
	m_nDYWordIdx=0;
	m_bDYData = nuFALSE;
	m_bSort = nuFALSE;
	m_nTempMap = -1;
	m_nTempCity = -1;
	bLoadAfter = nuTRUE;
    Free();
}

CSearchRoad::~CSearchRoad()
{
    Free();
}

nuBOOL CSearchRoad::Init(nuWORD nMMIdx, nuUINT nMMMaxLen, nuUINT nMMIdleAddr, CSearchArea* pSearchArea)
{
    if (nMMIdx==(nuWORD)-1 || nMMIdleAddr>=nMMMaxLen || !pSearchArea || !nMMIdleAddr)
    {
        return nuFALSE;
    }
    m_pSearchArea = pSearchArea;
    if (!m_pMmApi->MM_GetDataMassAddr(nMMIdx))
    {
        return nuFALSE;
    }
    ClearSZYInfo();
    m_nMMIdx = nMMIdx;
    m_nMMMaxLen = nMMMaxLen;
    m_nMMCTLen = nMMIdleAddr;
    m_bRun = nuTRUE;
    return nuTRUE;
}

nuVOID CSearchRoad::Free()
{    
	m_bKickNumber = nuFALSE;
	m_bIsFromBeging = nuFALSE;

    m_bRun = nuFALSE;
   
    m_nMMIdx = -1;
    m_nMMMaxLen = 0;
    m_nMMCTLen = 0;
    m_pSearchArea = NULL;
    m_nSzyWord = -1;
    nuMemset(m_nSearchRoadKeyName, 0 , sizeof(m_nSearchRoadKeyName));	
    nuMemset(m_nSearchRoadKeyDanyin, 0 , sizeof(m_nSearchRoadKeyDanyin));
    m_nSzyMin = -1;
    m_nSzyMax = -1;
    m_nSzyZhuyin = -1;

	if(m_nListNameIdx !=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&m_nListNameIdx);
	}
	if(m_nSearchResIdx !=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&m_nSearchResIdx);
	}
	if(m_nTownIdx !=(nuWORD)-1)
	{
		m_pMmApi->MM_RelDataMemMass(&m_nTownIdx);
	}
}

nuUINT CSearchRoad::FlushSZYInfo()
{
     ClearSZYInfo();
    if (m_searchInfo.nMapID==(nuLONG)-1 || m_nMMIdx==(nuWORD)-1)
    {
        ClearSZYInfo();
        return 0;
    }
    if (m_nMMSzyLen
        && m_searchInfo.nMapID == m_nSzySearchMapID
        && m_searchInfo.nCityID == m_nSzySearchCityID)
    {
        return m_nMMSzyLen;
    }
    ClearSZYInfo();
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nMMCTLen;

    CMapFileSzyY szyfile; 
    if (!szyfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    m_nMMSzyLen = szyfile.GetSzyInfo_CITY(m_searchInfo.nCityID, p, m_nMMMaxLen-m_nMMCTLen);
    if (!m_nMMSzyLen)
    {
        ClearSZYInfo();
        return 0;
    }
	if(!bLoadAfter)
	{

		if(m_searchInfo.nMapID!=m_nTempMap || m_searchInfo.nCityID!=m_nTempCity)
		{
			m_nTempMap=m_searchInfo.nMapID;
			m_nTempCity=m_searchInfo.nCityID;
			if (!FlushRNInfo())
			{
				return 0;
			}
		}

	}	
    m_nSzyLoadNum = m_nMMSzyLen/sizeof(SEARCH_SZY_STROKE);
    m_nMMSzyLen = ((m_nMMSzyLen+3)>>2)<<2;
    m_nSzySearchMapID = m_searchInfo.nMapID;
    m_nSzySearchCityID = m_searchInfo.nCityID;
    return m_nMMSzyLen;
}

nuVOID CSearchRoad::ClearSZYInfo()
{
    m_nMMSzyLen = 0;
    m_nSzyLoadNum = 0;
    m_nSzySearchCityID = -1;
    m_nSzySearchMapID = -1;
    ResetSZSearch();
}

nuVOID CSearchRoad::ResetSZSearch()
{
    m_nSzySearchTag = 0;
    m_nSzySearchLastCount = 0;

    m_nSzyMinT = -1;
    m_nSzyMaxT = -1;
    m_nSzyZhuyinT = -1;
    
    m_nSzySearchTownID = -1;
    m_nSzyResTotal = -1;
    nuMemset(m_nSzyWordbuffer, 0, sizeof(m_nSzyWordbuffer));

    
    ClearRNInfo();
}

nuBOOL CSearchRoad::SetRoadSZSLimit(nuUINT nMin, nuUINT nMax)
{
    m_nSzyMin = nMin;
    m_nSzyMax = nMax;
    m_nSzyZhuyin = -1;
	if(nMin==0 && nMax ==0)
	{
		bLoadAfter = nuFALSE;

		if(m_searchInfo.nMapID!=m_nTempMap || m_searchInfo.nCityID!=m_nTempCity)
		{
			m_nTempMap=m_searchInfo.nMapID;
			m_nTempCity=m_searchInfo.nCityID;
			if (!FlushSZYInfo())
			{
				return 0;
			}
			if (!FlushRNInfo())
			{
				return 0;
			}
			return nuTRUE;
		}
	}
    return nuTRUE;
}
nuUINT CSearchRoad::GetRoadSZTotal()
{
    return GetRoadSZList(NURO_SEARCH_ALL
        , -1
        , NULL
        , -1);
}

nuINT CSearchRoad::GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
    if (NURO_SEARCH_LAST == type)
    {
        GetRoadSZTotal();
    }
    if (!FlushSZYInfo())
    {
        return 0;
    }
    if (m_searchInfo.nTownID != m_nSzySearchTownID
        || m_nSzyMinT != m_nSzyMin
        || m_nSzyMaxT != m_nSzyMax
        || m_nSzyZhuyinT != m_nSzyZhuyin)
    {
        ResetSZSearch();
        m_nSzySearchTownID = m_searchInfo.nTownID;
        m_nSzyMinT = m_nSzyMin;
        m_nSzyMaxT = m_nSzyMax;
        m_nSzyZhuyinT = m_nSzyZhuyin;
    }
    
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nMax = NURO_MIN(nMax, nMaxLen/EACH_SZY_NAME_LEN);
    if (type==NURO_SEARCH_LAST)
    {
        if (!m_nSzyResTotal || m_nSzyResTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = m_nSzyResTotal%nMax;
        if (!realMax)
        {
            realMax = nMax;
        }
        realTag = m_nSzyResTotal-realMax;
    }
    else
    {
        if (!TranslateSS(type, nMax, m_nSzySearchTag, m_nSzySearchLastCount, &realTag, &realMax))
        {
            return 0;
        }
    }
    nuUINT realCount = 0;
    nuWORD *pResBuffer = (nuWORD*)pBuffer;
    if (type == NURO_SEARCH_ALL)
    {
        if ((nuUINT)-1 != m_nSzyResTotal)
        {
            return m_nSzyResTotal;
        }
        realMax = NURO_SEARCH_BUFFER_MAX_ROADSZ;
    }
    if ((nuUINT)-1==m_nSzyResTotal)
    {
        m_nSzyResTotal = SearchRoadSZ();
        if (type == NURO_SEARCH_ALL)
        {
            return m_nSzyResTotal;
        }
    }
    while (realCount<realMax && realTag<m_nSzyResTotal)
    {
        *pResBuffer = m_nSzyWordbuffer[realTag];
        ++pResBuffer;
        ++realCount;
        ++realTag;
    }
    if (!realCount)
    {
        return 0;
    }
    m_nSzySearchTag = realTag;
    m_nSzySearchLastCount = realCount;
    return m_nSzySearchLastCount;
}
nuWORD CSearchRoad::SearchRoadSZ()
{
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nMMCTLen;
    nuWORD realTag = 0;
    nuWORD realCount = 0;
    SEARCH_SZY_STROKE *pStroke = (SEARCH_SZY_STROKE*)p;
    nuWORD i=0;
    nuBOOL bNext = nuFALSE;
    for ( ;realTag<m_nSzyLoadNum && realCount<NURO_SEARCH_BUFFER_MAX_ROADSZ; ++realTag)
    {
        if (m_nSzySearchTownID!=(nuWORD)-1)
        {
            if (pStroke[realTag].nTownID != m_nSzySearchTownID)
            {
                continue;
            }
        }
        if ( (m_nSzyZhuyinT!=(nuWORD)-1 && pStroke[realTag].nZhuyin==m_nSzyZhuyinT)
            || (m_nSzyZhuyinT==(nuWORD)-1 && pStroke[realTag].nStrokes>=m_nSzyMinT 
            && pStroke[realTag].nStrokes<=m_nSzyMaxT) )			
        {
            if (realCount)
            {
                if (pStroke[realTag].nWord == m_nSzyWordbuffer[realCount-1])
                {
                    continue;
                }
                if ((nuWORD)-1==m_nSzySearchCityID)
                {
                    bNext = nuFALSE;
                    for (i=0; i<(realCount-1); ++i)
                    {
                        if (pStroke[realTag].nWord == m_nSzyWordbuffer[i])
                        {
                            bNext = nuTRUE;
                            break;
                        }
                    }
                    if (bNext)
                    {
                        continue;
                    }
                }
            }
            m_nSzyWordbuffer[realCount] = pStroke[realTag].nWord;
            ++realCount;
        }
    }    
    return realCount;
}

nuBOOL CSearchRoad::SetRoadSZZhuyin(nuWORD zhuyin)
{

	if(zhuyin==0)
	{
		bLoadAfter = nuFALSE;

		if(m_searchInfo.nMapID!=m_nTempMap || m_searchInfo.nCityID!=m_nTempCity)
		{
			m_nTempMap=m_searchInfo.nMapID;
			m_nTempCity=m_searchInfo.nCityID;
			if (!FlushSZYInfo())
			{
				return 0;
			}
			if (!FlushRNInfo())
			{
				return 0;
			}
			return nuTRUE;
		}
	}

    m_nSzyMin = -1;
    m_nSzyMax = -1;
    m_nSzyZhuyin = zhuyin;
    return nuTRUE;	
}
nuBOOL CSearchRoad::SetRoadSZ(nuUINT nResIdx)
{
    m_nSzyWord = -1;
    nuMemset(m_nSearchRoadKeyName, 0 , sizeof(m_nSearchRoadKeyName));	
    nuMemset(m_nSearchRoadKeyDanyin, 0 , sizeof(m_nSearchRoadKeyDanyin));
    if (nResIdx>=m_nSzySearchLastCount)
    {
        return nuFALSE;
    }
    m_nSzyWord = m_nSzyWordbuffer[m_nSzySearchTag + nResIdx - m_nSzySearchLastCount];
    if (m_nSzyWord!=(nuWORD)-1)
    {
        return nuTRUE;
    }
    else
    {
        return nuFALSE;
    }
}
nuBOOL CSearchRoad::SetRoadKeyDanyin(nuWORD *pDanyin)
{
	m_bSort = nuTRUE;
	m_bDYData = nuTRUE;
	if(m_nSearchResIdx == (nuWORD)-1)
	{
		nuPBYTE pData = NULL;
		pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)SH_WORD_DATA_LIMIT*sizeof(DYWORD), &m_nSearchResIdx);
		m_nDYWordIdx=0;
		nuMemset(pData,0,SH_WORD_DATA_LIMIT*sizeof(DYWORD));
	}
	else
	{
		m_nDYWordIdx=0;
		DYWORD *m_SearchRes  = (DYWORD*)m_pMmApi->MM_GetDataMassAddr(m_nSearchResIdx);
		nuMemset(m_SearchRes,0,SH_WORD_DATA_LIMIT*sizeof(DYWORD));
	}	

	nuMemset(m_nSearchRoadKeyDanyinT, 0, sizeof(m_nSearchRoadKeyDanyinT));
    nuMemset(m_nSearchRoadKeyDanyin, 0, sizeof(m_nSearchRoadKeyDanyin));
    if (!pDanyin || !pDanyin[0])
    {
        return nuFALSE;
    }
    m_nSzyWord = -1;
    nuMemset(m_nSearchRoadKeyName, 0, sizeof(m_nSearchRoadKeyName));
    nuWcsncpy(m_nSearchRoadKeyDanyin, pDanyin, (EACH_ROAD_NAME_LEN>>1) -1);
    return nuTRUE;
}
nuBOOL CSearchRoad::SetRoadKeyDanyin2(nuWORD *pDanyin)
{
	//bLoadAfter=nuFALSE;
	if(pDanyin==0)
	{
		//return 0;
		bLoadAfter = nuFALSE;

		if(m_searchInfo.nMapID!=m_nTempMap || m_searchInfo.nCityID!=m_nTempCity)
		{
			m_nTempMap=m_searchInfo.nMapID;
			m_nTempCity=m_searchInfo.nCityID;
			if (!FlushSZYInfo())
			{
				return 0;
			}
			if (!FlushRNInfo())
			{
				return 0;
			}
			return nuTRUE;
		}
	}
	m_bSort = nuFALSE;
	m_bDYData = nuTRUE;
	if(m_nSearchResIdx == (nuWORD)-1)
	{
		nuPBYTE pData = NULL;
		pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)SH_WORD_DATA_LIMIT*sizeof(DYWORD), &m_nSearchResIdx);
		m_nDYWordIdx=0;
	}
	nuMemset(m_nSearchRoadKeyDanyinT, 0, sizeof(m_nSearchRoadKeyDanyinT));
	nuMemset(m_nSearchRoadKeyDanyin, 0, sizeof(m_nSearchRoadKeyDanyin));
	if (!pDanyin || !pDanyin[0])
	{
		return nuFALSE;
	}
	m_nSzyWord = -1;
	nuMemset(m_nSearchRoadKeyName, 0, sizeof(m_nSearchRoadKeyName));
	nuWcsncpy(m_nSearchRoadKeyDanyin, pDanyin, (EACH_ROAD_NAME_LEN>>1) -1);
	return nuTRUE;
}
nuBOOL CSearchRoad::SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging /*= nuFALSE*/)
{
	 m_nNameIdx = 0;

    nuMemset(m_nSearchRoadKeyName, 0, sizeof(m_nSearchRoadKeyName));
    if (!pKeyWord)
    {
        return nuFALSE;
    }
    m_nSzyWord = -1;
	m_bIsFromBeging = bIsFromBeging;
    nuMemset(m_nSearchRoadKeyDanyin, 0, sizeof(m_nSearchRoadKeyDanyin));
    nuWcsncpy(m_nSearchRoadKeyName, pKeyWord, (EACH_ROAD_NAME_LEN>>1) -1);
    return nuTRUE;
}
nuUINT CSearchRoad::FlushRNInfo()
{
    if (m_searchInfo.nMapID==(nuLONG)-1 || m_nMMIdx==(nuWORD)-1)
    {
        ClearRNInfo();
        return 0;
    }
    if (m_nMMRNLen
        && m_searchInfo.nMapID == m_nRNCSearchMapID)
    {
        return m_nMMRNLen;
    }
    ClearRNInfo();
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    p = p + m_nMMCTLen + m_nMMSzyLen;
    CMapFileRncY rncfile;
    CMapFileRnY rnfile;
    if (!rncfile.Open(m_searchInfo.nMapID) || !rnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    m_nMMRNLen = rnfile.GetLength();
    m_nMMRNCLen = rncfile.GetLength();
    if (!m_nMMRNLen || !m_nMMRNCLen
        || (m_nMMMaxLen-m_nMMCTLen-m_nMMSzyLen)<(m_nMMRNLen+m_nMMRNCLen)
        || !rnfile.ReadData(0, p, m_nMMRNLen))
    {
        ClearRNInfo();
        return 0;
    }
    m_nMMRNLen = ((m_nMMRNLen+3)>>2)<<2;
    p += m_nMMRNLen;
    if(bLoadAfter)
    {
	    if (!rncfile.ReadData(0, p, m_nMMRNCLen))
	    {
	        ClearRNInfo();
	        return 0;
	    }
    }
    m_nMMRNCLen = ((m_nMMRNCLen+3)>>2)<<2;
    m_nRNCSearchMapID = m_searchInfo.nMapID;
    return m_nMMRNLen;
}

nuVOID	CSearchRoad::ClearRNInfo()
{
	if(bLoadAfter)
	{
		m_nMMRNCLen = 0;
		m_nMMRNLen = 0;
	}
	m_nRNCSearchMapID = -1;
    m_nRNCSearchCityID = -1;
    m_nRNCSearchTownID = -1;
    ResetRNSearch();
}
nuUINT	CSearchRoad::GetRoadName(nuUINT addr, nuWORD *&pBuffer, nuUINT nMaxLen, nuBYTE *pStrokes)
{
    if (addr>=m_nMMRNLen)
    {
        return 0;
    }
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    p += m_nMMCTLen+m_nMMSzyLen+addr;
    nuBYTE nLenR = *p;
    nuBYTE nLen = *p;
    p += 4;
    if (!nMaxLen)
    {
        pBuffer = (nuWORD*)p;
        if (nLen>(EACH_ROAD_NAME_LEN-2))
        {
            nLen = ((EACH_ROAD_NAME_LEN-2)>>1)<<1;
        }
    }
    else if (pBuffer)
    {
        if (nMaxLen>2)
        {
            if (nLen>(nMaxLen-2))
            {
                nLen = ((nMaxLen-2)>>1)<<1;
            }
            nuMemcpy(pBuffer, p, nLen);
            nuMemset((nuBYTE*)pBuffer+nLen, 0, 2 );
        }
        else
        {
            nLen = 0;
        }
    }
    if (pStrokes)
    {
        p += nLenR;
        p += *p+1;
        *pStrokes = *p;
    }
    return nLen>>1;
}
nuUINT	CSearchRoad::GetRoadDanyin(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen)
{
    if (!m_nMMRNLen || nMaxLen<2)
    {
        return 0;
    }
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nMMCTLen+m_nMMSzyLen+addr;
    nuBYTE nLen = *p;
    p += 4+nLen;
    nLen = *p;
    if (nLen>(nMaxLen-2))
    {
        nLen = ((nMaxLen-2)>>1)<<1;
    }
    p += 1;
    nuMemcpy(pBuffer, p, nLen);
    nuMemset((nuBYTE*)pBuffer+nLen, 0, 2);
    return nLen>>1;
}
nuUINT CSearchRoad::GetRoadTotal()
{
    return GetRoadNameList(NURO_SEARCH_ALL
        , -1
        , NULL
        , -1);
}
nuBOOL CSearchRoad::SetNoName(nuBOOL bRes)
{
	if(bRes)
	{
		if(m_nListNameIdx == (nuWORD)-1)
		{
			nuPBYTE pData = NULL;
			pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)SH_WORD_DATA_LIMIT*sizeof(ListName), &m_nListNameIdx);
		}
		//nuMemset(m_Town,0,sizeof(m_Town)); //Prosper add 20130108
		if(m_nTownIdx == (nuWORD)-1)
		{
			nuPBYTE pData = NULL;
			pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)SH_WORD_DATA_LIMIT*sizeof(nuINT), &m_nTownIdx);
		}
		//nuMemset(m_Town,0,sizeof(nuINT)*NURO_SEARCH_BUFFER_MAX);
//		nuMemset(wsListName,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1 * EACH_ROAD_NAME_LEN>>1);
		nuMemset(m_nSearchRoadKeyDanyinT,0,sizeof(m_nSearchRoadKeyDanyinT));
		nuMemset(m_nSearchRoadKeyNameT,0,sizeof(m_nSearchRoadKeyNameT));
		m_nNameIdx = 0;
		m_bKickNumber = nuTRUE;
	}
	else
	{
		//nuMemset(m_Town,0,sizeof(nuINT)*NURO_SEARCH_BUFFER_MAX);
//		nuMemset(wsListName,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1 * EACH_ROAD_NAME_LEN>>1);
		
		nuMemset(m_nSearchRoadKeyDanyinT,0,sizeof(m_nSearchRoadKeyDanyinT));
		nuMemset(m_nSearchRoadKeyNameT,0,sizeof(m_nSearchRoadKeyNameT));
		m_bKickNumber = nuFALSE;
	}
	return nuTRUE;
}
nuINT CSearchRoad::GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{

    if (NURO_SEARCH_LAST==type
        || NURO_SEARCH_PRE_SC==type
        || NURO_SEARCH_NEXT_SC==type)
    {
        GetRoadTotal();
    }
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nuUINT nEach = nMax;
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    if (type==NURO_SEARCH_LAST)
    {
        if (!m_nResTotal || m_nResTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = m_nResTotal%nEach;
        if (!realMax)
        {
            realMax = nMax;
        }
        realTag = m_nResTotal-realMax;
    }
    else if (type==NURO_SEARCH_PRE_SC)
    {
        if (!m_nResTotal || m_nResTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = nMax;
        nuWORD nIdx = m_nRNCSearchTag - m_nRNCSearchLastCount + m_nStrokeLimIdx;
        if (!nIdx || !m_nRNCSearchLastCount)
        {
            realTag = 0;
        }
        else
        {
            nuBOOL s1 = nuFALSE;
            nuWORD i = nIdx-1;
            while (i)
            {
                if (m_SearchRoadRes[i].strokes<m_SearchRoadRes[nIdx].strokes)
                {
                    if (s1)
                    {
                        realTag = i+1;
                        break;
                    }
                    else
                    {
                        nIdx = i;
                        s1 = nuTRUE;
                    }
                }
                --i;
            }
            if (!i)
            {
                realTag = 0;
            }
            realTag -= realTag%nEach;
        }
    }
    else if (type==NURO_SEARCH_NEXT_SC)
    {
        if (!m_nResTotal || m_nResTotal==(nuUINT)-1)
        {
            return 0;
        }
        if (m_nRNCSearchTag)
        {
            nuWORD nIdx = m_nRNCSearchTag - m_nRNCSearchLastCount + m_nStrokeLimIdx;
            realMax = nMax;
            
            nuWORD i = nIdx+1;
            while (i<m_nResTotal)
            {
                if (m_SearchRoadRes[i].strokes>m_SearchRoadRes[nIdx].strokes)
                {
                    realTag = i;
                    break;
                }
                ++i;
            }
            if (i==m_nResTotal)
            {
                return 0;
            }
            realTag -= realTag%nEach;
        }
        else
        {
            realTag = 0;
        }
    }
    else
    {
    	if(bLoadAfter)
    	{
	        if (!FlushRNInfo())
	        {
	            return 0;
	        }
    	}
        if ( m_searchInfo.nCityID	!= m_nRNCSearchCityID
            || m_searchInfo.nTownID != m_nRNCSearchTownID
            || m_nSzyWordT		!= m_nSzyWord
            || nuWcscmp(m_nSearchRoadKeyDanyin, m_nSearchRoadKeyDanyinT)
            || nuWcscmp(m_nSearchRoadKeyName, m_nSearchRoadKeyNameT))
        {
            ResetRNSearch();
        }
        m_nRNCSearchCityID = m_searchInfo.nCityID;
        m_nRNCSearchTownID = m_searchInfo.nTownID;
        m_nSzyWordT = m_nSzyWord;
        nuMemcpy(m_nSearchRoadKeyNameT, m_nSearchRoadKeyName, sizeof(m_nSearchRoadKeyNameT));
        nuMemcpy(m_nSearchRoadKeyDanyinT, m_nSearchRoadKeyDanyin, sizeof(m_nSearchRoadKeyDanyinT));
        if (m_nCityIDMax==(nuWORD)-1)
        {
            if (m_searchInfo.nCityID==(nuWORD)-1)
            {
                m_nCityIDTag = 0;
                m_nCityIDMax = m_nCityTotal-1;
            }
            else
            {
                m_nCityIDTag = m_searchInfo.nCityID;
                m_nCityIDMax = m_searchInfo.nCityID;
            }
        }
        if (type==NURO_SEARCH_ALL)
        {
            m_nRNCSearchTag = 0;
            m_nRNCSearchLastCount = 0;
            if (m_nResTotal!=(nuUINT)-1)
            {
                return m_nResTotal;
            }
            realMax = NURO_SEARCH_BUFFER_MAX_ROAD;
        }
        else
        {
            if (!TranslateSS(type, nMax, m_nRNCSearchTag, m_nRNCSearchLastCount, &realTag, &realMax))
            {
                return 0;
            }
        }
    }
    nuUINT realCount = 0;
    NURO_SEARCH_NORMALRES *pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
    if (type!=NURO_SEARCH_ALL && !pSearchRes)
    {
        return 0;
    }
    nuWORD *pName = NULL;
    nuUINT _bufferLim = NURO_SEARCH_BUFFER_MAX_ROAD;
    if (m_nResTotal!=(nuUINT)-1)
    {
        _bufferLim = m_nResTotal;
    }
    if (_bufferLim>(realTag+realMax))
    {
        _bufferLim = realTag+realMax;
    }
    while (realTag<_bufferLim)
    {
        if (m_SearchRoadRes[realTag].type != SEARCH_BT_ROAD)
        {
		
            if(bLoadAfter)
            	SearchNextRN(realTag);
            else
            	SearchNextRN2(realTag);
            if (m_SearchRoadRes[realTag].type != SEARCH_BT_ROAD)
            {
                break;
            }
        }
        if (type != NURO_SEARCH_ALL)
        {
			ListName *wsListName = (ListName*)m_pMmApi->MM_GetDataMassAddr(m_nListNameIdx);
			if(!m_bKickNumber )
			{
            pName = pSearchRes->name;
            nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
            GetRoadName(m_SearchRoadRes[realTag].nameAddr
                , pName
                , sizeof(pSearchRes->name)
                , NULL);
            pSearchRes->strokes = m_SearchRoadRes[realTag].strokes;
			}
			else
			{
				nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
				pName = pSearchRes->name;
				pSearchRes->strokes = m_SearchRoadRes[realTag].strokes;
				nuMemcpy(&pSearchRes->name , wsListName[realTag].wsListName2,sizeof(pSearchRes->name));
			}
            ++pSearchRes;
        }
        ++realCount;
        ++realTag;
    }
    if (type == NURO_SEARCH_ALL)
    {
        m_nResTotal = realTag;
        if (!m_nResTotal)
        {
            return m_nResTotal;
        }
#ifdef _SEARCH_ROAD_MAINROADMODE_
        FillMainRoad(m_nResTotal);
        //????
        nuUINT times = m_nMainRoadTotal - 1; 
        nuUINT i = 0;
        nuUINT j = 0;
        nuUINT tag2 = 0;
        nuUINT last = 0;
		SEARCH_MAIN_ROAD tmp = {0};
        nuUINT end = m_nResTotal;
        for (j=0; j<times; j++)
        {
            tag2 = 0;
            last = times-j;
            for(i=1;i<=last;i++)
            {
                if(m_SearchRoadRes[m_MainRoad[tag2].tag].strokes
                    <m_SearchRoadRes[m_MainRoad[i].tag].strokes)
                {
                    tag2 = i;
                }
                else if (m_SearchRoadRes[m_MainRoad[tag2].tag].strokes
                    ==m_SearchRoadRes[m_MainRoad[i].tag].strokes)
                {
                    if (m_SearchRoadRes[m_MainRoad[tag2].tag].nameAddr
                        <=m_SearchRoadRes[m_MainRoad[i].tag].nameAddr)
                    {
                        tag2 = i;
                    }
                }
            }
            if (tag2!=last)
            {
                tmp = m_MainRoad[tag2];
                m_MainRoad[tag2] = m_MainRoad[last];
                m_MainRoad[last] = tmp;
            }
        }
#else
        QueueBUfferRoad(m_SearchRoadRes, m_nResTotal, 2);
#endif
        return m_nResTotal;
    }
    
    if (!realCount)
    {
        return 0;
    }
    m_nRNCSearchTag = realTag;
    m_nRNCSearchLastCount = realCount;
    if (NURO_SEARCH_NEXT_SC==type
        || NURO_SEARCH_PRE_SC==type)
    {
        nuUINT nRes = m_nRNCSearchLastCount;
        nuUINT tmp = (m_nRNCSearchTag-m_nRNCSearchLastCount)/nEach;
        nRes |= (tmp<<16);
        return nRes;
    }

	
    return m_nRNCSearchLastCount;
}

nuINT	CSearchRoad::GetRoadFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuBOOL bCheck = nuTRUE;
    nuUINT keyLen = 0;
    if (!pKey)
    {
        bCheck = nuFALSE;
    }
    else
    {
        keyLen = nuWcslen(pKey);
        if (!keyLen)
        {
            bCheck = nuFALSE;
        }
    }
    if ((nuUINT)-1==m_nResTotal)
    {
        GetRoadTotal();
    }
    if (!m_nResTotal||(nuUINT)-1==m_nResTotal)
    {
        return 0;
    }
    nuWORD i = 0;
    nuUINT resLen = 0;
    nuUINT nRes = 0;
    nuUINT nMax = nMaxLen/sizeof(NURO_SEARCH_FILTERRES);
    NURO_SEARCH_FILTERRES *p = (NURO_SEARCH_FILTERRES*)pBuffer;
    nuUINT total = m_nResTotal;
#ifdef _SEARCH_ROAD_MAINROADMODE_
    total = m_nMainRoadTotal;
#endif
    nuWORD tag = 0;
    nuWORD *pName = NULL;
    for (i=0; i<total&&nRes<nMax; i++)
    {
        tag = i;
#ifdef _SEARCH_ROAD_MAINROADMODE_
        tag = m_MainRoad[i].tag;
#endif
        resLen = GetRoadName(m_SearchRoadRes[tag].nameAddr
            , pName
            , 0
            , NULL);
        if (!resLen)
        {
            continue;
        }
        if (bCheck)
        {
            if (!nuWcsFuzzyJudge(pName, (nuWORD*)pKey, resLen, keyLen, 0))
            {
                continue;
            }
        }
        p->idx = i;
        p->strokes = m_SearchRoadRes[tag].strokes;
        nuMemcpy(p->name, pName, EACH_ROAD_NAME_LEN);
        ++p;
        ++nRes;
    }
    return nRes;
}
nuBOOL CSearchRoad::RoadNameSort(nuVOID *pWord,nuINT nLens)
{
	ListName *wsListName = (ListName*)m_pMmApi->MM_GetDataMassAddr(m_nListNameIdx);
	nuWCHAR wsName[EACH_ROAD_NAME_LEN>>1]={0};
	nuMemcpy(wsName,pWord,sizeof(nuWCHAR)*nLens);
	for(nuINT i=0;i < m_nNameIdx ;i++)
	{
		if(!nuWcsncmp (wsListName[i].wsListName2, wsName,nuWcslen(wsListName[i].wsListName2)))
		{
			return nuFALSE;
		}
	}
	nuMemcpy(wsListName[m_nNameIdx++].wsListName2,wsName,sizeof(wsName));
	return nuTRUE;
}

nuBOOL CSearchRoad::CheckRoadNoNumber(nuVOID *pWord,nuINT nLens)
{
	ListName *wsListName = (ListName*)m_pMmApi->MM_GetDataMassAddr(m_nListNameIdx);
	if(m_nNameIdx == NURO_SEARCH_SORT_NAME)
	{
        return nuFALSE;
	}
	nuWCHAR wsRoadToken[]={27573,36335,34903,36947 ,0}; //琿隔刁笵
	

	nuWCHAR wsName[EACH_ROAD_NAME_LEN>>1]={0};
	nuMemcpy(wsName,pWord,sizeof(nuWCHAR)*nLens);
	for(nuINT i=0;i<nLens;i++)
	{
		if(wsName[i]==wsRoadToken[0] || wsName[i] == wsRoadToken[1] || wsName[i]==wsRoadToken[2] || wsName[i]==wsRoadToken[3])
		{
			for(nuINT j=i+1;j<nLens;j++)
			{
				if(wsName[j] >48  && wsName[j] <58)
				{
					wsName[j] = 0;
					nuBOOL bRes = RoadNameSort(wsName,nLens);
					if(bRes)
					{
						nuMemcpy(pWord,wsName,sizeof(nuWCHAR)*nLens);
						return nuFALSE;
					}

					return nuTRUE;
				}
			}
		}
	}
	nuMemcpy(wsListName[m_nNameIdx++].wsListName2,wsName,sizeof(wsName));
	
	return nuFALSE;
}
nuUINT CSearchRoad::SearchNextRN(nuUINT tag)
{
	nuDWORD t1 = nuGetTickCount();
    nuBYTE* pRnc = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!pRnc)
    {
        return -1;
    }
    if (m_nCityIDTag>m_nCityIDMax)
    {
        return 0;
    }
    nuUINT nRes = 0;
    nuUINT nResLen = 0;
    nuUINT danyinLen = nuWcslen(m_nSearchRoadKeyDanyinT);
    nuUINT keyLen = nuWcslen(m_nSearchRoadKeyNameT);
    pRnc += m_nMMCTLen + m_nMMSzyLen + m_nMMRNLen;
    SEARCH_RNC_HEADER *pRncHeader = (SEARCH_RNC_HEADER*)pRnc;
    SEARCH_RNC_CITYINFO *pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pRncHeader->nCTInfoStartAddr+4);
    while (pCityInfo->nCityCode != m_nCityIDTag)
    {
        if (pCityInfo->nCityCode > m_nCityIDMax)
        {
            return 0;
        }
        pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pCityInfo->nNextCityAddr);
    }
    SEARCH_RNC_TOWNINFO *pTownInfo = NULL;
    nuUINT *pNameAddr = NULL;
    nuWORD nTownTag = 0;
    nuWORD nRoadTag = 0;    
    nuWORD name2[EACH_BUFFER_NAME_LEN] = {0};
    nuWORD *pName = NULL;
    nuINT *m_Town =(nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nTownIdx);
	DYWORD *m_SearchRes  = (DYWORD*)m_pMmApi->MM_GetDataMassAddr(m_nSearchResIdx);

	nNEXT_WORD_COUNT = 0;
	nuMemset(m_wsNEXT_WORD_Data,0,sizeof(m_wsNEXT_WORD_Data));

	nuINT nLimit = 0;
	if(!m_bSort)
	{
		nLimit = 4;
	}
	else
	{
		nLimit = SH_WORD_DATA_LIMIT;
	}

	while (m_nCityIDTag<=m_nCityIDMax && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
    {
        nTownTag = 0;
        pTownInfo 
            = (SEARCH_RNC_TOWNINFO*)((nuBYTE*)pCityInfo	+ sizeof(SEARCH_RNC_CITYINFO));
        while (nTownTag<pCityInfo->nTownCount && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
        {
            if (m_nRNCSearchTownID!=(nuWORD)-1 && pTownInfo[nTownTag].nTownCode!=m_nRNCSearchTownID)
            {
                ++nTownTag;
                continue;
            }
            pNameAddr = (nuUINT*)(pRnc + pTownInfo[nTownTag].nRNStartAddr);
            nRoadTag=0;
            while ( nRoadTag<pTownInfo[nTownTag].nRoadCount && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
            {
                if (tag)
                {//?占?同town 同name ??同nameaddr??息
                    if ( m_nSASTDATA < _SNST_COL_LIMIT_
                        && (pNameAddr[nRoadTag]-m_SearchRoadRes[tag-1].nameAddr) < _SAST_JUDGE_RN_DIS_
                                && IsSameRoadName(pNameAddr[nRoadTag], m_SearchRoadRes[tag-1].nameAddr) 
                        && m_SearchRoadRes[tag-1].cityID == pCityInfo->nCityCode
                        && m_SearchRoadRes[tag-1].townID == pTownInfo[nTownTag].nTownCode)
                    {
                        m_SASTDATA[m_nSASTDATA].addr1 = m_SearchRoadRes[tag-1].nameAddr;
                        m_SASTDATA[m_nSASTDATA].addr2 = pNameAddr[nRoadTag];
                        ++m_nSASTDATA;
                        ++nRoadTag;
                        continue;
                    }
                }
                if (danyinLen)
                {
                    nResLen = GetRoadDanyin(pNameAddr[nRoadTag], name2, EACH_ROAD_NAME_LEN);
					
                    if ((!nResLen) || 
						(-1 == nuWcsKeyMatch(name2, m_nSearchRoadKeyDanyinT, nResLen, danyinLen)))
                    {
                        ++nRoadTag;
                        continue;
                    }                    
                }
				//Prosper 2011 12.07
				#ifndef VALUE_EMGRTX 
				nuINT nDanyinIdx = nuWcsKeyMatch(name2, m_nSearchRoadKeyDanyinT, nResLen, danyinLen);
				nuINT n= nDanyinIdx+danyinLen;

				if(n<=nResLen)
				{
					Sort_KEY_index(name2[n]);
                }
				#endif
                nResLen = GetRoadName(pNameAddr[nRoadTag]
                    , pName
                    , 0
                    , &m_SearchRoadRes[tag].strokes);
                if (nResLen)
                {
                    if ( (!danyinLen&&!keyLen&&m_nSzyWordT==(nuWORD)-1)
                        || danyinLen
                        || (pName[0]==m_nSzyWordT)
                        || ( m_nSzyWordT == (nuWORD)-1     && 
						     nuFALSE       == m_bIsFromBeging&&
							 nuWcsFuzzyJudge(pName, m_nSearchRoadKeyNameT, nResLen, keyLen, _SEARCH_ROAD_FJUDGE_ENDWORD_))
						|| ( m_nSzyWordT == (nuWORD)-1 && 
						     nuTRUE        == m_bIsFromBeging &&
						     0 == nuWcsncmp(pName, m_nSearchRoadKeyNameT, nuWcslen(m_nSearchRoadKeyNameT))))
                    {
						if(nuWcslen(m_nSearchRoadKeyNameT)>0)
						{
							nuINT  nDanyinIdx = 0, nNameDiffPos = 0;
							nuBOOL bres = nuWcsFuzzyJudge_O_FORSEARCHROAD(pName, m_nSearchRoadKeyNameT
								, nResLen, keyLen, nDanyinIdx, nNameDiffPos);

							nuINT nData_Len = nResLen;
							nuINT nKEYData_Len = nuWcslen(m_nSearchRoadKeyNameT);
							nuINT nWordIdx = nDanyinIdx+nNameDiffPos+1;
							if((nWordIdx)<nData_Len)
							{
								if(nNEXT_WORD_COUNT<NEXT_WORD_MAXCOUNT)
								{
									nuBOOL bSame = nuFALSE;
									for(nuINT j=0;j<nNEXT_WORD_COUNT;j++)
									{
										if( m_wsNEXT_WORD_Data[j] == pName[nWordIdx])
										{
											bSame = nuTRUE;
											break;
										}
									}
									if(!bSame)
									m_wsNEXT_WORD_Data[nNEXT_WORD_COUNT++]= pName[nWordIdx];
								}
							}
						}

						nuBOOL bRes = nuFALSE;
						if(m_bKickNumber)
						{

							bRes=CheckRoadNoNumber(pName,nResLen);
							if(bRes)
							{
								nRoadTag++;
								continue;
							}
							m_Town[m_nNameIdx-1]=pTownInfo[nTownTag].nTownCode;
							//m_SearchRes[m_nNameIdx-1]townID=pTownInfo[nTownTag].nTownCode;
						}
	#ifndef VALUE_EMGRTX 
						if(m_bDYData && m_nDYWordIdx < nLimit )
						{		
						nuBOOL  bSameName=nuFALSE;
						nuWCHAR wsName[EACH_BUFFER_NAME_LEN-1]={0};
						nuMemcpy(wsName,pName,2*nResLen);
						
						if(m_SearchRes!=NULL)
						{
						for ( nuUINT l = 0; l < m_nDYWordIdx && m_nDYWordIdx < nLimit; l++ )
						{
							nuBOOL bFIND = nuTRUE;
							for(nuINT k=0;k < danyinLen; k++)
							{
								if(m_SearchRes[l].name[k]!=wsName[nDanyinIdx+k])
								{
									bFIND = nuFALSE;
									break;
								}								
							}
							if(bFIND)
							{
								++m_SearchRes[l].nFindCount;
								bSameName = nuTRUE;						    
								break;
							}
						}
						if(!bSameName && m_nDYWordIdx < nLimit)
						{
							m_SearchRes[m_nDYWordIdx].nFindCount = 1;
							for(nuINT k=0;k<danyinLen;k++)
							{
								m_SearchRes[m_nDYWordIdx].name[k]=pName[nDanyinIdx+k];
							}
							//nuMemcpy(m_SearchRes[m_nDYWordIdx].name,pName[nDanyinIdx],2*danyinLen);
							m_SearchRes[m_nDYWordIdx].townID=pTownInfo[nTownTag].nTownCode;
							m_nDYWordIdx++;
						}
						
						}
						}
						#endif
                        m_SearchRoadRes[tag].nameAddr	= pNameAddr[nRoadTag];
                        m_SearchRoadRes[tag].mapID		= m_nRNCSearchMapID;
                        m_SearchRoadRes[tag].cityID		= pCityInfo->nCityCode;
                        m_SearchRoadRes[tag].townID		= pTownInfo[nTownTag].nTownCode;
                        m_SearchRoadRes[tag].type		= SEARCH_BT_ROAD;
                        ++tag;
                        ++nRes;
                    }
                }
                ++nRoadTag;
            }
            ++nTownTag;
        }
        ++m_nCityIDTag;
        //?蚩舜???实??每??只????一??CITY?惴??
//         if (nRes)
//         {
//             break;
//         }
        if (m_nCityIDTag<=m_nCityIDMax)
        {
            pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pCityInfo->nNextCityAddr);
        }
    }
	//if(m_SearchRes!=NULL)
	if(m_bKickNumber)
	{
		NAME_Order(nRes);
    }
	if(m_bDYData)
	{
		m_bDYData=nuFALSE;
	}

    return nRes;
}

nuUINT CSearchRoad::SearchNextRN2(nuUINT tag)
{
	nuDWORD t1 = nuGetTickCount();
    nuBYTE* pRnc = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!pRnc)
    {
        return -1;
    }
    if (m_nCityIDTag>m_nCityIDMax)
    {
        return 0;
    }
    nuUINT nRes = 0;
    nuUINT nResLen = 0;
    nuUINT danyinLen = nuWcslen(m_nSearchRoadKeyDanyinT);
    nuUINT keyLen = nuWcslen(m_nSearchRoadKeyNameT);
    pRnc += m_nMMCTLen + m_nMMSzyLen + m_nMMRNLen;
  
	nuUINT pNameAddr = 0;// = NULL;
    nuWORD nTownTag = 0;
    nuWORD nRoadTag = 0;    
    nuWORD name2[EACH_BUFFER_NAME_LEN] = {0};
    nuWORD *pName = NULL;
	nuDWORD n1= 0;
	nuINT *m_Town =(nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nTownIdx);
	DYWORD *m_SearchRes  = (DYWORD*)m_pMmApi->MM_GetDataMassAddr(m_nSearchResIdx);

	CMapFileRncY rncfile;

	if (!rncfile.Open(m_searchInfo.nMapID) )
	{
		return 0;
	}

	nuDWORD nStartAddr = 0;
	nuINT m_nTownCount =rncfile.GetTownCount(m_searchInfo.nCityID,nStartAddr);

	nNEXT_WORD_COUNT = 0;
	nuMemset(m_wsNEXT_WORD_Data,0,sizeof(m_wsNEXT_WORD_Data));


	nuINT nLimit = 0;
	if(!m_bSort)
	{
		nLimit = 4;
	}
	else
	{
		nLimit = SH_WORD_DATA_LIMIT;
	}
	if(1)//while (m_nCityIDTag<=m_nCityIDMax && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
    {
        nTownTag = 0;
		SEARCH_RNC_TOWNINFO *pTownInfo;
		pTownInfo = new SEARCH_RNC_TOWNINFO[m_nTownCount];

		nuMemset(pTownInfo,0,sizeof(SEARCH_RNC_TOWNINFO)*m_nTownCount);
		rncfile.ReadData(nStartAddr,pTownInfo,sizeof(SEARCH_RNC_TOWNINFO)*m_nTownCount);
  
		while (nTownTag<m_nTownCount && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
        {
            if (m_nRNCSearchTownID!=(nuWORD)-1 && pTownInfo[nTownTag].nTownCode!=m_nRNCSearchTownID)
            {
                ++nTownTag;
                continue;
            }
			
			rncfile.ReadData(pTownInfo[nTownTag].nRNStartAddr,&pNameAddr,4);
           
			nuUINT *pNameAddr =new nuUINT[pTownInfo[nTownTag].nRoadCount];
			nuMemset(pNameAddr,0,sizeof(nuUINT)*pTownInfo[nTownTag].nRoadCount);
			rncfile.ReadData(pTownInfo[nTownTag].nRNStartAddr,pNameAddr,sizeof(nuUINT)*pTownInfo[nTownTag].nRoadCount);

            nRoadTag=0;
            while ( nRoadTag<pTownInfo[nTownTag].nRoadCount && tag<NURO_SEARCH_BUFFER_MAX_ROAD)
            {
				//rncfile.ReadData(pTownInfo[nTownTag].nRNStartAddr+nTownTag*4,&pNameAddr,4);
                if (tag)
                {//?占?同town 同name ??同nameaddr??息
                    if ( m_nSASTDATA < _SNST_COL_LIMIT_
                        && ((pNameAddr[nRoadTag])-m_SearchRoadRes[tag-1].nameAddr) < _SAST_JUDGE_RN_DIS_
                                && IsSameRoadName((pNameAddr[nRoadTag]), m_SearchRoadRes[tag-1].nameAddr) 
                        && m_SearchRoadRes[tag-1].cityID == m_searchInfo.nCityID
                        && m_SearchRoadRes[tag-1].townID == pTownInfo[nTownTag].nTownCode)
                    {
                        m_SASTDATA[m_nSASTDATA].addr1 = m_SearchRoadRes[tag-1].nameAddr;
                        m_SASTDATA[m_nSASTDATA].addr2 = pNameAddr[nRoadTag];
                        ++m_nSASTDATA;
                        ++nRoadTag;
                        continue;
                    } 
              }
                if (danyinLen)
                {
                    nResLen = GetRoadDanyin(pNameAddr[nRoadTag], name2, EACH_ROAD_NAME_LEN);
					
                    if ((!nResLen) || 
						(-1 == nuWcsKeyMatch(name2, m_nSearchRoadKeyDanyinT, nResLen, danyinLen)))
                    {
                        ++nRoadTag;
                        continue;
                    }                    
                }
				//Prosper 2011 12.07
				#ifndef VALUE_EMGRTX 
				nuINT nDanyinIdx = nuWcsKeyMatch(name2, m_nSearchRoadKeyDanyinT, nResLen, danyinLen);
				nuINT n= nDanyinIdx+danyinLen;

				if(n<=nResLen)
				{
					Sort_KEY_index(name2[n]);
                }
				#endif
                nResLen = GetRoadName(pNameAddr[nRoadTag]
                    , pName
                    , 0
                    , &m_SearchRoadRes[tag].strokes);
                if (nResLen)
                {
                    if ( (!danyinLen&&!keyLen&&m_nSzyWordT==(nuWORD)-1)
                        || danyinLen
                        || (pName[0]==m_nSzyWordT)
                        || ( m_nSzyWordT == (nuWORD)-1     && 
						     nuFALSE       == m_bIsFromBeging&&
							 nuWcsFuzzyJudge(pName, m_nSearchRoadKeyNameT, nResLen, keyLen, _SEARCH_ROAD_FJUDGE_ENDWORD_))
						|| ( m_nSzyWordT == (nuWORD)-1 && 
						     nuTRUE        == m_bIsFromBeging &&
						     0 == nuWcsncmp(pName, m_nSearchRoadKeyNameT, nuWcslen(m_nSearchRoadKeyNameT))))
                    {
						if(nuWcslen(m_nSearchRoadKeyNameT)>0)
						{
							nuINT  nDanyinIdx = 0, nNameDiffPos = 0;
							nuBOOL bres = nuWcsFuzzyJudge_O_FORSEARCHROAD(pName, m_nSearchRoadKeyNameT
								, nResLen, keyLen, nDanyinIdx, nNameDiffPos);

							nuINT nData_Len = nResLen;
							nuINT nKEYData_Len = nuWcslen(m_nSearchRoadKeyNameT);
							nuINT nWordIdx = nDanyinIdx+nNameDiffPos+1;
							if((nWordIdx)<nData_Len)
							{
								if(nNEXT_WORD_COUNT<NEXT_WORD_MAXCOUNT)
								{
									nuBOOL bSame = nuFALSE;
									for(nuINT j=0;j<nNEXT_WORD_COUNT;j++)
									{
										if( m_wsNEXT_WORD_Data[j] == pName[nWordIdx])
										{
											bSame = nuTRUE;
											break;
										}
									}
									if(!bSame)
									m_wsNEXT_WORD_Data[nNEXT_WORD_COUNT++]= pName[nWordIdx];
								}
							}
						}
						nuBOOL bRes = nuFALSE;
						if(m_bKickNumber)
						{

							bRes=CheckRoadNoNumber(pName,nResLen);
							if(bRes)
							{
								nRoadTag++;
								continue;
							}
							m_Town[m_nNameIdx-1]=pTownInfo[nTownTag].nTownCode;
							//m_SearchRes[m_nNameIdx-1]townID=pTownInfo[nTownTag].nTownCode;
						}

	#ifndef VALUE_EMGRTX 
						if(m_bDYData && m_nDYWordIdx < nLimit)
						{		
						nuBOOL  bSameName=nuFALSE;
						nuWCHAR wsName[EACH_BUFFER_NAME_LEN-1]={0};
						nuMemcpy(wsName,pName,2*nResLen);
						
						if(m_SearchRes!=NULL)
						{
						
						for ( nuUINT l = 0; l < m_nDYWordIdx && m_nDYWordIdx < nLimit; l++ )
						{
							nuBOOL bFIND = nuTRUE;
							for(nuINT k=0;k < danyinLen; k++)
							{
								if(m_SearchRes[l].name[k]!=wsName[nDanyinIdx+k])
								{
									bFIND = nuFALSE;
									break;
								}								
							}
							if(bFIND)
							{
								++m_SearchRes[l].nFindCount;
								bSameName = nuTRUE;						    
								break;
							}
						}
						if(!bSameName && m_nDYWordIdx < nLimit)
						{
							m_SearchRes[m_nDYWordIdx].nFindCount = 1;
							for(nuINT k=0;k<danyinLen;k++)
							{
								m_SearchRes[m_nDYWordIdx].name[k]=pName[nDanyinIdx+k];
							}
							//nuMemcpy(m_SearchRes[m_nDYWordIdx].name,pName[nDanyinIdx],2*danyinLen);
							m_SearchRes[m_nDYWordIdx].townID=pTownInfo[nTownTag].nTownCode;
							m_nDYWordIdx++;
						}
						
						}
						}
						#endif
                        m_SearchRoadRes[tag].nameAddr	= pNameAddr[nRoadTag];
                        m_SearchRoadRes[tag].mapID		= m_nRNCSearchMapID;
                        m_SearchRoadRes[tag].cityID		= m_searchInfo.nCityID;//pCityInfo->nCityCode;
                        m_SearchRoadRes[tag].townID		= pTownInfo[nTownTag].nTownCode;
                        m_SearchRoadRes[tag].type		= SEARCH_BT_ROAD;
                        ++tag;
                        ++nRes;
                    }
                }
                ++nRoadTag;
            }
			
			if(pNameAddr != NULL)
			{
				delete[] pNameAddr;
				pNameAddr = NULL;
			}
            ++nTownTag;
        }
		if(pTownInfo != NULL)
		{
			delete[] pTownInfo;
			pTownInfo = NULL;
		}
        ++m_nCityIDTag;
        //?蚩舜???实??每??只????一??CITY?惴??
    }
	//if(m_SearchRes!=NULL)
	if(m_bKickNumber)
	{
		NAME_Order(nRes);
    }
	if(m_bDYData)
	{
		m_bDYData=nuFALSE;
	}
	rncfile.Close();

    return nRes;
}
nuBOOL CSearchRoad::IsSameRoadName(nuUINT addr1, nuUINT addr2)
{
    if (addr1 == addr2)
    {
        return nuTRUE;
    }
    nuWORD *pName1 = NULL;
    nuWORD *pName2 = NULL;
    nuUINT l1 = GetRoadName(addr1, pName1, 0);
    nuUINT l2 = GetRoadName(addr2, pName2, 0);
    if (!l1 || !l2 || l1!=l2)
    {
        return nuFALSE;
    }
    for (nuUINT i=0; i<l1; i++)
    {
        if (pName1[i]!=pName2[i])
        {
            return nuFALSE;
        }
    }
    return nuTRUE;
}
nuBOOL CSearchRoad::IsSameMainRoad(nuUINT addr1, nuUINT addr2)
{
	nuWCHAR wName1[64] = {0};
	nuWCHAR wName2[64] = {0};
    nuWORD *pName1 = NULL;
    nuWORD *pName2 = NULL;
	nuLONG l1=0;
	nuLONG l2=0;
	l1 = GetRoadName(addr1, pName1, 0);
	l2 = GetRoadName(addr2, pName2, 0);
    if ( !l1 || !l2 )
    {
        return nuFALSE;
    }
	nuMemcpy(wName1,pName1,64);
	nuMemcpy(wName2,pName2,64);
	wName1[l1] = 0;
	wName2[l2] = 0;
    for (nuUINT i=0; i<(EACH_ROAD_NAME_LEN>>1); ++i)
    {//??????弄??息
		if( wName1[i] == _SEARCH_ROAD_FJUDGE_ENDWORD_ ||
			wName1[i] == _SEARCH_ROAD_FJUDGE_ENDWORD1_ )
		{
			wName1[i] = 0;
		}
		if( wName2[i] == _SEARCH_ROAD_FJUDGE_ENDWORD_ ||
			wName2[i] == _SEARCH_ROAD_FJUDGE_ENDWORD1_ )
		{
			wName2[i] = 0;
		}
	}
	if( nuWcscmp( wName1,wName2 ) == 0 )
	{//?榷?????路??息
		return nuTRUE;
	}
	return nuFALSE;
}

nuUINT	CSearchRoad::FillMainRoad(nuUINT total)
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    if (!total)
    {
        m_nMainRoadTotal = 0;
    }
    nuUINT nRes = 0;
    nuUINT i = 0;
    nuUINT tag = 0;
    while (i<total)
    {
        if (!i)
        {
            m_MainRoad[nRes].tag = i;
            m_MainRoad[nRes].total = 1;
            ++nRes;
        }
        else
        {
            tag = m_MainRoad[nRes-1].tag;
            if (m_SearchRoadRes[i].cityID == m_SearchRoadRes[tag].cityID
                && m_SearchRoadRes[i].townID == m_SearchRoadRes[tag].townID
                && IsSameMainRoad(m_SearchRoadRes[i].nameAddr, m_SearchRoadRes[tag].nameAddr))
            {
                ++m_MainRoad[nRes-1].total;
            }
            else
            {
                m_MainRoad[nRes].tag = i;
                m_MainRoad[nRes].total = 1;
                ++nRes;
            }
        }
        ++i;
    }
    m_nMainRoadTotal = nRes;
    return nRes;
#else
    return 0;
#endif
}
nuUINT	CSearchRoad::GetRoadTotal_MR()
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    m_nMRSearchTag = 0;
    m_nMRSearchLastCount = 0;
    if ((nuUINT)-1==m_nMainRoadTotal)
    {
        GetRoadTotal();
        if ((nuUINT)-1==m_nMainRoadTotal)
        {
            return 0;
        }
    }
    return m_nMainRoadTotal;
#else
    return 0;
#endif
}
nuUINT CSearchRoad::GetChildRoadTotal(nuUINT nMainRoadIdx, nuBOOL bRealIdx)
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    if ((nuUINT)-1==m_nMainRoadTotal)
    {
        return 0;
    }
    nuUINT maintag = nMainRoadIdx;
    if (!bRealIdx)
    {
        maintag = m_nMRSearchTag + nMainRoadIdx - m_nMRSearchLastCount;
    }
    return m_MainRoad[maintag].total;
#else
    return 0;
#endif
}
nuUINT	CSearchRoad::GetRoadNameList_MR(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    if (!pBuffer)
    {
        return 0;
    }
    if ((nuUINT)-1==m_nMainRoadTotal)
    {
        GetRoadTotal();
        if ((nuUINT)-1==m_nMainRoadTotal)
        {
            return 0;
        }
    }
    if (!m_nMainRoadTotal || type==NURO_SEARCH_ALL)
    {
        return m_nMainRoadTotal;
    }
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nuUINT nEach = nMax;
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    if (type==NURO_SEARCH_LAST)
    {
        realMax = m_nMainRoadTotal%nEach;
        if (!realMax)
        {
            realMax = nMax;
        }
        realTag = m_nMainRoadTotal-realMax;
    }
    else if (type==NURO_SEARCH_PRE_SC)
    {
        realMax = nMax;
        nuWORD nIdx = m_nMRSearchTag - m_nMRSearchLastCount + m_nStrokeLimIdx;
        if (!nIdx || !m_nMRSearchLastCount)
        {
            realTag = 0;
        }
        else
        {
            nuBOOL s1 = nuFALSE;
            nuWORD i = nIdx-1;
            while (i)
            {
                if (m_SearchRoadRes[m_MainRoad[i].tag].strokes<m_SearchRoadRes[m_MainRoad[nIdx].tag].strokes)
                {
                    if (s1)
                    {
                        realTag = i+1;
                        realTag -= realTag%nEach;
                        break;
                    }
                    else
                    {
                        nIdx = i;
                        s1 = nuTRUE;
                    }
                }
                --i;
            }
            if (!i)
            {
                realTag = 0;
            }
        }
    }
    else if (type==NURO_SEARCH_NEXT_SC)
    {
        if (!m_nMRSearchTag)
        {
            realTag = 0;
        }
        else
        {
            nuWORD nIdx = m_nMRSearchTag - m_nMRSearchLastCount + m_nStrokeLimIdx;
            realMax = nMax;	
            nuWORD i = nIdx+1;
            while (i<m_nMainRoadTotal)
            {
                if (m_SearchRoadRes[m_MainRoad[i].tag].strokes>m_SearchRoadRes[m_MainRoad[nIdx].tag].strokes)
                {
                    realTag = i;
                    realTag -= realTag%nEach;
                    break;
                }
                ++i;
            }
            if (i==m_nMainRoadTotal)
            {
                return 0;
            }
        }
    }
    else
    {
        if (!TranslateSS(type, nMax, m_nMRSearchTag, m_nMRSearchLastCount, &realTag, &realMax))
        {
            return 0;
        }
    }
    nuUINT realCount = 0;
    NURO_SEARCH_NORMALRES *pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
    nuUINT tag = 0;
    nuWORD *pName = NULL;
    while (realCount<realMax && realTag<m_nMainRoadTotal)
    {
        tag = m_MainRoad[realTag].tag;
        if (m_SearchRoadRes[tag].type != SEARCH_BT_ROAD)
        {
            break;
        }
        pName = pSearchRes->name;
        nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
        GetRoadName(m_SearchRoadRes[tag].nameAddr
            , pName
            , sizeof(pSearchRes->name)
            , NULL);
        pSearchRes->strokes = m_SearchRoadRes[tag].strokes;
        ++pSearchRes;
        ++realCount;
        ++realTag;
    }
    if (!realCount)
    {
        return 0;
    }
    m_nMRSearchTag = realTag;
    m_nMRSearchLastCount = realCount;
    if (NURO_SEARCH_NEXT_SC==type
        || NURO_SEARCH_PRE_SC==type)
    {
        nuUINT nRes = m_nMRSearchLastCount;
        nuUINT tmp = (m_nMRSearchTag-m_nMRSearchLastCount)/nEach;
        nRes |= (tmp<<16);
        return nRes;
    }
    return m_nMRSearchLastCount;
#else
    return 0;
#endif
}
nuUINT	CSearchRoad::GetRoadNameALL_MR(nuUINT nMainRoadIdx, nuBYTE* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx)
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    if (!pBuffer)
    {
        return 0;
    }
    if ((nuUINT)-1==m_nMainRoadTotal)
    {
        GetRoadTotal();
        if ((nuUINT)-1==m_nMainRoadTotal)
        {
            return 0;
        }
    }
    nuUINT maintag = nMainRoadIdx;
    if (!bRealIdx)
    {
        maintag = m_nMRSearchTag + nMainRoadIdx - m_nMRSearchLastCount;
    }
    nuUINT nMax = m_MainRoad[maintag].total;
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    NURO_SEARCH_NORMALRES *pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
    nuUINT tag = m_MainRoad[maintag].tag;
    nuUINT i = 0;
    nuWORD *pName = NULL;
    while (i<nMax)
    {
        if (tag>=m_nResTotal)
        {
            break;
        }
        pName = pSearchRes->name;
        nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
        if (!GetRoadName(m_SearchRoadRes[tag].nameAddr
            , pName
            , sizeof(pSearchRes->name)
            , NULL))
        {
            break;
        }
        pSearchRes->strokes = m_SearchRoadRes[tag].strokes;
        ++pSearchRes;
        ++tag;
        ++i;
    }
    return i;
#else
    return 0;
#endif
}
nuBOOL	CSearchRoad::SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx)
{
#ifdef _SEARCH_ROAD_MAINROADMODE_
    if ((nuUINT)-1==m_nMainRoadTotal)
    {
        return nuFALSE;
    }
    nuUINT maintag = nMainRoadIdx;
    if (!bRealIdx)
    {
        maintag = m_nMRSearchTag + nMainRoadIdx - m_nMRSearchLastCount;
    }
    maintag = m_MainRoad[maintag].tag;
    if (maintag>=m_nResTotal || (maintag+nRoadIdx)>=m_nResTotal)
    {
        return nuFALSE;
    }
    m_nSearchRoadIdx = maintag+nRoadIdx;
    m_nSearchRoadCityID = m_SearchRoadRes[m_nSearchRoadIdx].cityID;
    m_nSearchRoadNameAddr = m_SearchRoadRes[m_nSearchRoadIdx].nameAddr;
    m_nSearchRoadTownID = m_SearchRoadRes[m_nSearchRoadIdx].townID;
    m_nSearchRoadDoorPos = -1;
    m_nSearchRoadDoorNum = -1;
    LoadRoadPos();
    return nuTRUE;
#else
    return nuFALSE;
#endif
}
nuBOOL CSearchRoad::SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx)
{
	nuINT *m_Town =(nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nTownIdx);
    nuUINT nPos = nResIdx;
    if (!bRealIdx)
    {
        nPos = m_nRNCSearchTag + nResIdx - m_nRNCSearchLastCount;
    }
    m_nSearchRoadCityID = m_SearchRoadRes[nPos].cityID;
    m_nSearchRoadNameAddr = m_SearchRoadRes[nPos].nameAddr;
	if(m_bKickNumber)
	m_nSearchRoadTownID = 	m_Town[nPos];
	else
    m_nSearchRoadTownID = m_SearchRoadRes[nPos].townID;
    m_nSearchRoadDoorPos = -1;
    m_nSearchRoadDoorNum = -1;
    m_nSearchRoadIdx = nPos;
    //LoadRoadPos();
    return nuTRUE;
}
nuVOID CSearchRoad::ResetRNSearch()
{
    m_nCityIDTag = -1;
    m_nCityIDMax = -1;
    
    m_nRNCSearchTownID = -1;
    
    m_nSzyWordT = -1;
    nuMemset(m_nSearchRoadKeyNameT, 0 , sizeof(m_nSearchRoadKeyNameT));	
    nuMemset(m_nSearchRoadKeyDanyinT, 0 , sizeof(m_nSearchRoadKeyDanyinT));

    m_nSearchRoadNameAddr = -1;
    m_nSearchRoadCityID = -1;
    m_nSearchRoadTownID = -1;
    m_nSearchRoadIdx = -1;
    m_nSearchRoadDoorPos = -1;
    m_nSearchRoadDoorNum = -1;
    m_nSearchRoadIdx = -1;
    
    m_nResTotal = -1;
    m_nRNCSearchTag = 0;
    m_nRNCSearchLastCount = 0;
    m_nMainRoadTotal = -1;
    m_nMRSearchTag = 0;
    m_nMRSearchLastCount = 0;
    nuMemset(m_SearchRoadRes, 0, sizeof(m_SearchRoadRes));
    nuMemset(m_SASTDATA, 0, sizeof(m_SASTDATA));
    m_nSASTDATA = 0;
    
	nKeyCount=0;                            //Prosper 20121226
	nuMemset(KEY_Data,0,sizeof(KEY_Data));

	DYWORD *m_SearchRes  = (DYWORD*)m_pMmApi->MM_GetDataMassAddr(m_nSearchResIdx);
	if(m_SearchRes)
	{
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*SH_WORD_DATA_LIMIT);
		m_nDYWordIdx=0;
	
	}
    ClearCrossInfo();
}

nuUINT CSearchRoad::CollectSameRoad(nuUINT *pNameAddr, nuUINT nMaxLim)
{
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 1;
    }
    p += m_nMMCTLen+m_nMMSzyLen;
    nuBYTE *pName = p+pNameAddr[0];
    nuBYTE nLen = *pName;
    nuBYTE nLen2 = 0;
    
    nuWORD name[EACH_BUFFER_NAME_LEN] = {0};
    nuWORD name2[EACH_BUFFER_NAME_LEN] = {0};
    
    nuBYTE nLenT = nLen;
    if (nLenT>(EACH_ROAD_NAME_LEN-2))
    {
        nLenT = (EACH_ROAD_NAME_LEN-2);
    }
    nuMemcpy(name, pName+4, nLenT);
    nuMemset((nuBYTE*)name+nLenT, 0, 2);
    
    nuUINT nPerSize = 6 + nLen + *(pName+4+nLen);
    
    nuUINT nRes = 1;
    //????前??
    nuBYTE *p2 = pName;
    nuUINT nameaddr = pNameAddr[0];
    while (nRes<nMaxLim)
    {
        p2 -= nPerSize;
        if (p2<p)
        {
            break;
        }
        nameaddr -= nPerSize;
        nLen2 = *p2;
        if (nLen2!=nLen)
        {
            break;
        }
        nLenT = nLen;
        if (nLenT>(EACH_ROAD_NAME_LEN-2))
        {
            nLenT = (EACH_ROAD_NAME_LEN-2);
        }
        nuMemcpy(name2, p2+4, nLenT);
        nuMemset((nuBYTE*)name2+nLenT, 0, 2);
        if (!nuWcscmp(name, name2))
        {
            pNameAddr[nRes] = nameaddr;
            ++nRes;
        }
        else
        {
            break;
        }
    }
    
    //??????缀
    p2 = pName;
    nameaddr = pNameAddr[0];
    while (nRes<nMaxLim)
    {
        p2 += nPerSize;
        if (p2>=(p+m_nMMRNLen))
        {
            break;
        }
        nameaddr += nPerSize;
        nLen2 = *p2;
        if (nLen2!=nLen)
        {
            break;
        }
        nLenT = nLen;
        if (nLenT>(EACH_ROAD_NAME_LEN-2))
        {
            nLenT = (EACH_ROAD_NAME_LEN-2);
        }
        nuMemcpy(name2, p2+4, nLenT);
        nuMemset((nuBYTE*)name2+nLenT, 0, 2);
        if (!nuWcscmp(name, name2))
        {
            pNameAddr[nRes] = nameaddr;
            ++nRes;
        }
        else
        {
            break;
        }
    }
    return nRes;
}

nuUINT CSearchRoad::GetRoadDoorTotal()
{
    if (m_nSearchRoadDoorNum!=(nuWORD)-1)
    {
        return m_nSearchRoadDoorNum;
    }
    m_nSearchRoadDoorNum = 0;
    CMapFilePdnY pdnfile;
    if (!pdnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    nuUINT nameAddrExt[256]={0};
    nameAddrExt[0] = m_nSearchRoadNameAddr;
    nuUINT roadCount = CollectSameRoad(nameAddrExt, 256);
    nuUINT pos = pdnfile.GetCityDoorInfoStartAddr(m_nSearchRoadCityID);
    nuBOOL bFind = nuFALSE;
    
    const nuUINT __nTempNode = 1000;
	SEARCH_PDN_NODE_R pdnNode[__nTempNode] = {0};
    nuUINT realC = 0;
    nuUINT cc = 0;
    nuUINT ccc = 0;
    for (;;)
    {
        realC = pdnfile.ReadData(pos, pdnNode, sizeof(SEARCH_PDN_NODE_R), __nTempNode);
        for (cc=0; cc<realC; ++cc)
        {
            if (pdnNode[cc].CityID>m_nSearchRoadCityID)
            {
                return m_nSearchRoadDoorNum;
            }
            if (pdnNode[cc].TownID==m_nSearchRoadTownID)	
            {
                ccc = 0;
                bFind = nuFALSE;
                while (ccc<roadCount)
                {
                    if (pdnNode[cc].RoadNameAddr==nameAddrExt[ccc])
                    {
                        bFind = nuTRUE;
                        break;
                    }
                    ++ccc;
                }
                if (bFind)
                {
                    m_nSearchRoadDoorNum = pdnNode[cc].doorNoCount;
                    m_nSearchRoadDoorPos = pos;
                    m_nSearchRDN = pdnNode[cc];
			        return m_nSearchRoadDoorNum;
                }
            }
        }
        if (realC!=__nTempNode)
        {
            break;
        }
        pos += sizeof(pdnNode);
    }
    return m_nSearchRoadDoorNum;
}

nuBOOL CSearchRoad::GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen)
{
    if (!pBuffer)
    {
        return nuFALSE;
    }
    if (GetRoadDoorTotal())
    {
        CMapFilePdnY pdnfile;
        if (!pdnfile.Open(m_searchInfo.nMapID))
        {
            return nuFALSE;
        }
        nuUINT len = sizeof(SEARCH_PDN_NODE);
        if (m_nSearchRoadDoorNum>1)
        {
            len += (m_nSearchRoadDoorNum-1)*sizeof(SEARCH_PDN_NODE_DI);
        }
        if (len>nMaxLen)
        {
            return nuFALSE;
        }
        nuMemcpy(pBuffer, &m_nSearchRDN, sizeof(m_nSearchRDN));
        return pdnfile.GetData(m_nSearchRDN.bAddr
            , ((nuBYTE*)pBuffer)+sizeof(SEARCH_PDN_NODE_R)-4
            , m_nSearchRoadDoorNum*sizeof(SEARCH_PDN_NODE_DI));
    }
    return nuFALSE;
}

nuUINT CSearchRoad::FlushCrossInfo()
{
    if (m_nRC_RTLen
        && m_nTownBlkInfoLen
        && m_nRCSearchMapID == m_nRNCSearchMapID
        && m_nRCSearchCityID == m_nSearchRoadCityID
        && m_nSearchRoadTownID == m_nRCSearchTownID
        )
    {
        return m_nRC_RTLen;
    }
    ClearCrossInfo();
    //????BLK LIST??息
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    //????RT_AREA ID
    m_nRNCSearchMapID = m_searchInfo.nMapID; //Prosper ADD , 20130705
    nuUINT rta = CMapFileRefY::GetRtIdxEx(m_nRNCSearchMapID, m_nSearchRoadCityID, m_nSearchRoadTownID);
    if (rta==(nuUINT)-1)
    {
        return 0;
    }
    CMapFileRtY rtfile;
    CMapFileBhY bhfile;
    if (!rtfile.Open(m_nRNCSearchMapID) || !bhfile.Open(m_nRNCSearchMapID))
    {
        return 0;
    }
    p += m_nMMCTLen + m_nMMSzyLen + m_nMMRNLen + m_nMMRNCLen;
    m_nTownBlkInfoLen = m_pSearchArea->GetTownBlkInfo(m_nSearchRoadCityID
        , m_nSearchRoadTownID, p, m_nMMMaxLen-m_nMMCTLen-m_nMMSzyLen-m_nMMRNCLen-m_nMMRNLen);
    if (!m_nTownBlkInfoLen)
    {
        ClearCrossInfo();
        return 0;
    }
    m_nTownBlkInfoLen = ((m_nTownBlkInfoLen+3)>>2)<<2;
    p += m_nTownBlkInfoLen;
    m_nRC_RTLen = rtfile.GetRtIdxInfo(rta
        , p
        , m_nMMMaxLen-m_nMMCTLen-m_nMMSzyLen-m_nMMRNCLen-m_nMMRNLen-m_nTownBlkInfoLen);
    if (!m_nRC_RTLen)
    {
        ClearCrossInfo();
        return 0;
    }
    m_nRC_RTLen = ((m_nRC_RTLen+3)>>2)<<2;
    p += m_nRC_RTLen;
    m_nBHIdxLen = bhfile.GetBHIdxInfo(p
        , m_nMMMaxLen-m_nMMCTLen-m_nMMSzyLen-m_nMMRNCLen-m_nMMRNLen-m_nTownBlkInfoLen-m_nRC_RTLen);
    if (!m_nBHIdxLen)
    {
        ClearCrossInfo();
        return 0;
    }
    m_nBHIdxLen = ((m_nBHIdxLen+3)>>2)<<2;
    m_nBHTotal = bhfile.GetBlkNum();
    m_nRCSearchMapID = m_nRNCSearchMapID;
    m_nRCSearchCityID = m_nSearchRoadCityID;
    m_nRCSearchTownID = m_nSearchRoadTownID;
    return m_nRC_RTLen;
}
nuVOID CSearchRoad::ClearCrossInfo()
{
    m_nRC_RTLen = 0;
    m_nTownBlkInfoLen = 0;
    m_nBHIdxLen = 0;
    m_nBHTotal = 0;
    m_nRCSearchMapID = -1;
    m_nRCSearchCityID = -1;
    m_nRCSearchTownID = -1;
    ResetCrossSearch();
}
nuUINT CSearchRoad::BlkIDToIdx(nuUINT nBlkID)
{
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return -1;
    }
    nuUINT *pBHInfo = (nuUINT*)(p+m_nMMCTLen+m_nMMSzyLen+m_nMMRNLen+m_nMMRNCLen+m_nTownBlkInfoLen+m_nRC_RTLen);
    if (!pBHInfo || !m_nBHTotal)
    {
        return -1;
    }
    nuUINT start = 0;
    nuUINT end = m_nBHTotal-1;
    nuUINT center = 0;
    while (start<=end)
    {
        center = (end+start)>>1;
        if (pBHInfo[center]==nBlkID)
        {
            return center;
        }
        else if (pBHInfo[center]>nBlkID)
        {
            if (!center)
            {
                return -1;
            }
            end = center-1;
        }
        else
        {
            start = center+1;
        }
    }
    return -1;
}
nuUINT CSearchRoad::GetRoadCrossTotal()
{
    return GetRoadCrossList(NURO_SEARCH_ALL
        , -1
        , NULL
        , -1);
}
nuINT CSearchRoad::GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
    if (type != NURO_SEARCH_ALL && !pBuffer)
    {
        return 0;
    }
    if (NURO_SEARCH_LAST==type)
    {
        GetRoadCrossTotal();
    }
    if (!FlushCrossInfo())
    {
        return 0;
    }
    if (m_nSearchRoadNameAddr!=m_nRCSearchNameAddr)
    {
        ResetCrossSearch();
        m_nRCSearchNameAddr = m_nSearchRoadNameAddr;
    }
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nuUINT nEach = nMax;
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    if (type==NURO_SEARCH_LAST)
    {
        if (!m_RCTotal || m_RCTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = m_RCTotal%nMax;
        if (!realMax)
        {
            realMax = nMax;
        }
        realTag = m_RCTotal-realMax;
    }
    else if (type==NURO_SEARCH_PRE_SC)
    {
        if (!m_RCTotal || m_RCTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = nMax;
        nuWORD nIdx = m_nRCSearchTag - m_nRCSearchLastCount + m_nStrokeLimIdx;
        if (!nIdx || !m_nRCSearchLastCount)
        {
            realTag = 0;
        }
        else
        {
            nuBOOL s1 = nuFALSE;
            nuWORD i = nIdx-1;
            while (i)
            {
                if (m_SearchCrossRes[i].strokes<m_SearchCrossRes[nIdx].strokes)
                {
                    if (s1)
                    {
                        realTag = i+1;
                        break;
                    }
                    else
                    {
                        nIdx = i;
                        s1 = nuTRUE;
                    }
                }
                --i;
            }
            if (!i)
            {
                realTag = 0;
            }
            realTag -= realTag%nEach;
        }
    }
    else if (type==NURO_SEARCH_NEXT_SC)
    {
        if (!m_RCTotal || m_RCTotal==(nuUINT)-1)
        {
            return 0;
        }
        nuWORD nIdx = m_nRCSearchTag - m_nRCSearchLastCount + m_nStrokeLimIdx;
        realMax = nMax;
        nuWORD i = nIdx+1;
        while (i<m_RCTotal)
        {
            if (m_SearchCrossRes[i].strokes>m_SearchCrossRes[nIdx].strokes)
            {
                realTag = i;
                break;
            }
            ++i;
        }
        if (i==m_RCTotal)
        {
            return 0;
        }
        realTag -= realTag%nEach;
    }
    else if (type == NURO_SEARCH_ALL)
    {
        m_nRCSearchTag = 0;
        m_nRCSearchLastCount = 0;
        if ((nuUINT)-1 != m_RCTotal)
        {
            return m_RCTotal;
        }
        realMax = NURO_SEARCH_BUFFER_MAX_CROSS;
    }
    else
    {
        if (!TranslateSS(type, nMax, m_nRCSearchTag, m_nRCSearchLastCount, &realTag, &realMax))
        {
            return 0;
        }
    }
    nuUINT realCount = 0;
    CMapFileBhY bhfile;
    CMapFileRdwY rdwfile;
    if (!bhfile.Open(m_nRCSearchMapID) || !rdwfile.Open(m_nRCSearchMapID))
    {
        return 0;
    }
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nMMCTLen+m_nMMSzyLen+m_nMMRNLen+m_nMMRNCLen;
    nuUINT nblkl = *((nuUINT*)p);
    SEARCH_CB_BIL *pbil = (SEARCH_CB_BIL*)(p+4);
    nuUINT nsearchc = 0;
    NURO_SEARCH_NORMALRES *pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
    nuWORD *pName = NULL;    
    nuUINT _bufferLim = NURO_SEARCH_BUFFER_MAX_CROSS;
    if (m_RCTotal!=(nuUINT)-1)
    {
        _bufferLim = m_RCTotal;
    }
    if (_bufferLim>(realTag+realMax))
    {
        _bufferLim = realTag+realMax;
    }
    while (realTag<_bufferLim)
    {
        if (m_SearchCrossRes[realTag].type!=SEARCH_BT_ROAD)
        {
            for (;;)
            {
                if ((pbil[m_nBLIdx].fromBlkID+m_nBLIdxM)>pbil[m_nBLIdx].toBlkID)
                {
                    m_nBLIdxM = 0;
                    ++m_nBLIdx;
                    if (m_nBLIdx>=nblkl)
                    {
                        break;
                    }
                }
                nsearchc = 
                    SearchRCBlock(BlkIDToIdx(pbil[m_nBLIdx].fromBlkID+m_nBLIdxM), realTag, bhfile, rdwfile);
                ++m_nBLIdxM;
                if (nsearchc)
                {
                    break;
                }
            }
            if (m_SearchCrossRes[realTag].type!=SEARCH_BT_ROAD)
            {
                break;
            }
        }
        if (type != NURO_SEARCH_ALL)
        {
            pName = pSearchRes->name;
            nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
            GetRoadName(m_SearchCrossRes[realTag].nameAddr
                , pName
                , sizeof(pSearchRes->name)
                , NULL);
            pSearchRes->strokes = m_SearchCrossRes[realTag].strokes;
            ++pSearchRes;
        }
        ++realCount;
        ++realTag;
    }
    if (!realCount)
    {
        return 0;
    }
    if (type == NURO_SEARCH_ALL)
    {
        m_RCTotal = realTag;
        if (m_RCTotal)
        {
            QueueBUfferRoad(m_SearchCrossRes, m_RCTotal, 2);
        }
        return m_RCTotal;
    }
    m_nRCSearchTag = realTag;
    m_nRCSearchLastCount = realCount;
    if (NURO_SEARCH_NEXT_SC==type
        || NURO_SEARCH_PRE_SC==type)
    {
        nuUINT nRes = m_nRCSearchLastCount;
        nuUINT tmp = (m_nRCSearchTag-m_nRCSearchLastCount)/nEach;
        nRes |= (tmp<<16);
        return nRes;
    }
    return m_nRCSearchLastCount;
}

nuUINT	CSearchRoad::SearchRCBlock(nuUINT blkID, nuUINT tag, CMapFileBhY& bhfile, CMapFileRdwY& rdwfile)
{
    nuUINT roadRdwDataAddr = bhfile.GetBHRoadAddrByIdx(blkID);
    if (roadRdwDataAddr==(nuUINT)-1)
    {
        return 0;
    }
    CMapFileRtY rtfile;
	SEARCH_RDW_BLKINFO rbi = {0};
    if (!rtfile.Open(m_nRNCSearchMapID)
        || !rdwfile.GetBlkInfo(roadRdwDataAddr, &rbi))
    {
        return 0;
    }
    nuUINT m_nBRIdx = 0;
    nuUINT nResC = 0;
    nuUINT nRes = 0;
    nuUINT nTag = tag;
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return nuFALSE;
    }
    p += m_nMMCTLen + m_nMMSzyLen + m_nMMRNCLen + m_nMMRNLen + m_nTownBlkInfoLen;
    SEARCH_RTINFO_M *pRtim = (SEARCH_RTINFO_M*)p;
    SEARCH_RTINFO_A3 *pa3_ = (SEARCH_RTINFO_A3*)(p+pRtim->a3addrM);
	SEARCH_RTINFO_A1 F_A1 = {0};
    nuUINT ia3 = 0;
    
    nuUINT node[_CROSS_COL_PT_LIMIT_] = {0};
    nuINT ptc = 0;
    nuUINT nameEx[_CROSS_COL_RN_LIMIT_] ={0};
    nuUINT nNameEx = 0;
    
    nuBYTE i = 0;
    nuBOOL ba1 = nuTRUE;
    nuBOOL ba2 = nuTRUE;
    SEARCH_RDW_A2 a2;
    SEARCH_RDW_A1 *pA1 = NULL;
    SEARCH_RDW_A2 *pA2 = NULL;
    SEARCH_RDW_A2 *pA2Tag = NULL;
    nuBYTE *pLoad = p+m_nRC_RTLen+m_nBHIdxLen;
    nuUINT nLoadLim = m_nMMMaxLen - m_nMMCTLen - m_nMMSzyLen - m_nMMRNCLen
        - m_nMMRNLen - m_nTownBlkInfoLen - m_nRC_RTLen - m_nBHIdxLen;
    nuUINT nRdwA1A2Len = 0;
    nuUINT nameAddr_2 = 0;
    nuUINT idn = 0;
    nRdwA1A2Len	= rbi.RoadCommonCount*sizeof(SEARCH_RDW_A2)+rbi.RoadCount*sizeof(SEARCH_RDW_A1);
    if (nRdwA1A2Len <= nLoadLim)
    {
        rdwfile.GetFileData(roadRdwDataAddr+sizeof(rbi), pLoad, nRdwA1A2Len);
        pA1 = (SEARCH_RDW_A1*)pLoad;
        pA2 = (SEARCH_RDW_A2*)(pLoad+rbi.RoadCount*sizeof(SEARCH_RDW_A1));
    }
    else
    {
        nRdwA1A2Len = 0;
    }
    
    // collect addr
    nameEx[0] = m_nSearchRoadNameAddr;
    ++nNameEx;
    idn = 0;
    //?占?同??同??路??
    while (idn<_SNST_COL_LIMIT_)
    {
        if (nNameEx==_CROSS_COL_RN_LIMIT_)
        {
            break;
        }
        if (m_SASTDATA[idn].addr1==m_nSearchRoadNameAddr)
        {
            nameEx[nNameEx] = m_SASTDATA[idn].addr2;
            ++nNameEx;
        }
        ++idn;
    }
    
    //?占???路?说?    
    nuBOOL btag = nuFALSE;
    while (m_nBRIdx<rbi.RoadCount)
    {
        if (ptc==_CROSS_COL_PT_LIMIT_)
        {
            break;
        }
        if (nRdwA1A2Len)
        {
            if (pA1[m_nBRIdx].RoadCommonIndex >= rbi.RoadCommonCount)
            {
                ++m_nBRIdx;
                continue;
            }
            pA2Tag = &pA2[pA1[m_nBRIdx].RoadCommonIndex];
        }
        else
        {
            if (!rdwfile.GetRoadA2Info(roadRdwDataAddr, m_nBRIdx, a2, &rbi))
            {
                ++m_nBRIdx;
                continue;
            }
            pA2Tag = &a2;
        }
        btag = nuFALSE;
        if (pA2Tag->CityID==m_nSearchRoadCityID	&& pA2Tag->TownID==m_nSearchRoadTownID)
        {
            for (idn=0; idn<nNameEx; ++idn)
            {
                if (pA2Tag->RoadNameAddr == nameEx[idn])
                {
                    btag = nuTRUE;
                    break;
                }
            }
        }
        if (btag)
        {
            ia3 = 0;
            while (ia3<pRtim->a3count)
            {
                if (ptc==_CROSS_COL_PT_LIMIT_)
                    break;
                if (pa3_[ia3].BlockIndex==blkID
                    && pa3_[ia3].RoadIndex==m_nBRIdx)
                {
                    ba1 = nuTRUE;
                    ba2 = nuTRUE;
                    for (i=0; i<ptc&&(ba1||ba2); ++i)
                    {
                        if (ba1 && node[i]==pa3_[ia3].FNodeID && node[i]==pa3_[ia3].FNodeID)
                        {
                            ba1 = nuFALSE;
                        }
                        if (ba2 && node[i]==pa3_[ia3].TNodeID && node[i]==pa3_[ia3].TNodeID)
                        {
                            ba2 = nuFALSE;
                        }
                    }
                    if (ba1)
                    {
                        node[ptc] = pa3_[ia3].FNodeID;
                        ++ptc;
                    }
                    if (ptc<_CROSS_COL_PT_LIMIT_ && ba2)
                    {
                        node[ptc] = pa3_[ia3].TNodeID;
                        ++ptc;
                    }
                    break;
                }
                ++ia3;
            }
        }
        ++m_nBRIdx;
    }
    
    nuUINT iatemp = 0;
    nuBOOL bCross = nuFALSE;
    nuUINT nodeID = 0 ;
    nuUINT bhRoadAddr = 0;
    nuUINT tagtag = 0 ;
    nuWORD *pName = NULL;
    nuUINT nResJ = 0;
    while (ptc && iatemp<pRtim->a3count)
    {
        if ((tag+nRes)>=NURO_SEARCH_BUFFER_MAX_CROSS)
        {
            break;
        }
        bCross = nuFALSE;
        //?卸??欠??嘟?
        for (i=0; i<ptc; ++i)
        {
            if (pa3_[iatemp].FNodeID==node[i] || pa3_[iatemp].TNodeID==node[i])
            {
                nodeID = node[i];
                bCross = nuTRUE;
                break;
            }
        }
        if (bCross)
        {//??取??路??RT-A1/A2??息
            bhRoadAddr = bhfile.GetBHRoadAddrByIdx(pa3_[iatemp].BlockIndex);
            if (!rdwfile.GetRoadA2Info(bhRoadAddr, pa3_[iatemp].RoadIndex, a2)
                || (a2.RoadNameAddr==-1)
                || !rtfile.GetRtA1Info(pRtim->a1addr, nodeID, &F_A1))
            {
                bCross = nuFALSE;
            }
        }        
        if (bCross)
        {//?????欠???指????路同??
            for (idn=0; idn<nNameEx; ++idn)
            {
                if (a2.RoadNameAddr == nameEx[idn])
                {
                    bCross = nuFALSE;
                }
            }
        }
        if (bCross)
        {//?????欠???????同????路
            for (nResJ=0; nResJ<tag+nRes; ++nResJ)
            {
                if ( IsSameRoadName(m_SearchCrossRes[nResJ].nameAddr, a2.RoadNameAddr)          
                    && m_SearchCrossRes[nResJ].cityID==a2.CityID
                    && m_SearchCrossRes[nResJ].townID==a2.TownID)
                {
                    bCross = nuFALSE;
                    break;
                }
            }
        }
        if (bCross)
        {
            tagtag = tag+nRes;
            m_SearchCrossRes[tagtag].cityID = a2.CityID;
            m_SearchCrossRes[tagtag].type = SEARCH_BT_ROAD;
            m_SearchCrossRes[tagtag].nameAddr = a2.RoadNameAddr;
#ifdef VALUE_EMGRT
            m_SearchCrossRes[tagtag].x = F_A1.x;
            m_SearchCrossRes[tagtag].y = F_A1.y;
#else
            m_SearchCrossRes[tagtag].x = F_A1.x + (F_A1.coor_level%4)*65536 +pRtim->baseX;
            m_SearchCrossRes[tagtag].y = F_A1.y + (F_A1.coor_level/4)*65536 +pRtim->baseY;
#endif
            m_SearchCrossRes[tagtag].mapID = m_nRCSearchMapID;
            m_SearchCrossRes[tagtag].townID = a2.TownID;
            if (GetRoadName(m_SearchCrossRes[tagtag].nameAddr
                , pName
                , 0
                , &m_SearchCrossRes[tagtag].strokes))
            {
                ++nRes;
            }
        }
        ++iatemp;
    }
    return nRes;
}

nuBOOL CSearchRoad::GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos)
{
    if (!pos)
    {
        return nuFALSE;
    }
    if (nResIdx>=m_nRCSearchLastCount || nResIdx>=m_nRCSearchTag || m_nRCSearchLastCount>m_nRCSearchTag)
    {
        return nuFALSE;
    }
    nuUINT nPos = m_nRCSearchTag + nResIdx - m_nRCSearchLastCount;
    pos->x = m_SearchCrossRes[nPos].x;
    pos->y = m_SearchCrossRes[nPos].y;
    return nuTRUE;
}
nuWORD CSearchRoad::GetRoadCrossTownID(nuUINT nResIdx)
{
    if (nResIdx>=m_nRCSearchLastCount || nResIdx>=m_nRCSearchTag || m_nRCSearchLastCount>m_nRCSearchTag)
    {
        return -1;
    }
    return m_SearchCrossRes[m_nRCSearchTag + nResIdx - m_nRCSearchLastCount].townID;
}
nuWORD	CSearchRoad::GetRoadCrossCityID(nuUINT nResIdx)
{
    if (nResIdx>=m_nRCSearchLastCount || nResIdx>=m_nRCSearchTag || m_nRCSearchLastCount>m_nRCSearchTag)
    {
        return -1;
    }
    return m_SearchCrossRes[m_nRCSearchTag + nResIdx - m_nRCSearchLastCount].cityID;
}
nuBOOL CSearchRoad::GetRoadPos(NUROPOINT* pos)
{
	LoadRoadPos();
	
    if (m_SearchRoadRes[m_nSearchRoadIdx].x && m_SearchRoadRes[m_nSearchRoadIdx].y)
    {
        pos->x = m_SearchRoadRes[m_nSearchRoadIdx].x;
        pos->y = m_SearchRoadRes[m_nSearchRoadIdx].y;
        return nuTRUE;
    }
    return nuFALSE;
}
nuBOOL CSearchRoad::LoadRoadPos()
{
    if (!FlushCrossInfo())
    {
        return nuFALSE;
    }
	NUROPOINT pos = {0};
    CMapFileBhY bhfile;
    CMapFileRdwY rdwfile;
    if (!bhfile.Open(m_nRCSearchMapID) || !rdwfile.Open(m_nRCSearchMapID))
    {
        return nuFALSE;
    }
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!p)
    {
        return nuFALSE;
    }
    p += m_nMMCTLen+m_nMMSzyLen+m_nMMRNLen+m_nMMRNCLen;
    nuUINT nblkl = *((nuUINT*)p);
    SEARCH_CB_BIL *pbil = (SEARCH_CB_BIL*)(p+4);
    nuUINT nl = 0;
    nuUINT nln = 0;
    nuUINT nroad = 0;
	SEARCH_RDW_BLKINFO rbi = {0};
    nuUINT roadRdwDataAddr =0;
	NURORECT rt={0};
    
    nuBYTE *pLoad = p+m_nTownBlkInfoLen+m_nRC_RTLen+m_nBHIdxLen;
    nuUINT nLoadLim = m_nMMMaxLen - m_nMMCTLen - m_nMMSzyLen
        - m_nMMRNCLen - m_nMMRNLen - m_nTownBlkInfoLen - m_nRC_RTLen - m_nBHIdxLen;
    nuUINT nRdwA1A2Len = 0;
    SEARCH_RDW_A2 *pA2 = NULL;
    SEARCH_RDW_A1 *pA1 = NULL;
    
    for (;;)
    {
        if ((pbil[nl].fromBlkID+nln)>pbil[nl].toBlkID)
        {
            nln = 0;
            ++nl;
            if (nl>=nblkl)
            {
                break;
            }
        }
        roadRdwDataAddr = bhfile.GetBHRoadAddrByIdx(BlkIDToIdx(pbil[nl].fromBlkID+nln));
        nuBlockIDtoRect(pbil[nl].fromBlkID+nln, &rt);
        if (rdwfile.GetBlkInfo(roadRdwDataAddr, &rbi) && rbi.RoadCount)
        {
            nroad = 0;
            nRdwA1A2Len	= rbi.RoadCommonCount*sizeof(SEARCH_RDW_A2)
                +rbi.RoadCount*sizeof(SEARCH_RDW_A1);
            if (nRdwA1A2Len <= nLoadLim)
            {
                rdwfile.GetFileData(roadRdwDataAddr+sizeof(rbi), pLoad, nRdwA1A2Len);
                pA1 = (SEARCH_RDW_A1*)pLoad;
                pA2 = (SEARCH_RDW_A2*)(pLoad+rbi.RoadCount*sizeof(SEARCH_RDW_A1));
                while (nroad<rbi.RoadCount)
                {				
                    if (pA1[nroad].RoadCommonIndex<rbi.RoadCommonCount
                        && pA2[pA1[nroad].RoadCommonIndex].CityID == m_nSearchRoadCityID
                        && pA2[pA1[nroad].RoadCommonIndex].TownID == m_nSearchRoadTownID)
                    {
						nuWORD *pName1 = NULL;
						nuWORD *pName2 = NULL;
						nuUINT l1 = GetRoadName(pA2[pA1[nroad].RoadCommonIndex].RoadNameAddr, pName1, 0);
						nuUINT l2 = GetRoadName(m_nSearchRoadNameAddr, pName2, 0);
                        if( (l1 == l2 ) &&
							(nuMemcmp(pName1,pName2,l1*2) == 0 ) )
						{
							if (rdwfile.GetRoadPos(roadRdwDataAddr, nroad, &pos, &rbi))
							{
								pos.x += rt.left;
								pos.y += rt.top;
								m_SearchRoadRes[m_nSearchRoadIdx].x = pos.x;
								m_SearchRoadRes[m_nSearchRoadIdx].y = pos.y;
								return nuTRUE;
							}
						}
						
                    }
                    ++nroad;
                }
            }
            else
            {
				SEARCH_RDW_A2 roadA2Info = {0};
                while (nroad<rbi.RoadCount)
                {
                    if (rdwfile.GetRoadA2Info(roadRdwDataAddr, nroad, roadA2Info, &rbi)
                        //&& roadA2Info.RoadNameAddr == m_nSearchRoadNameAddr
                        && roadA2Info.CityID == m_nSearchRoadCityID
                        && roadA2Info.TownID == m_nSearchRoadTownID)
                    {
						nuWORD *pName1 = NULL;
						nuWORD *pName2 = NULL;
						nuUINT l1 = GetRoadName(roadA2Info.RoadNameAddr, pName1, 0);
						nuUINT l2 = GetRoadName(m_nSearchRoadNameAddr, pName2, 0);
                        if( (l1 == l2 ) &&
							(nuMemcmp(pName1,pName2,l1*2) == 0 ) )
						{
							if (rdwfile.GetRoadPos(roadRdwDataAddr, nroad, &pos))
							{
								pos.x += rt.left;
								pos.y += rt.top;
								m_SearchRoadRes[m_nSearchRoadIdx].x = pos.x;
								m_SearchRoadRes[m_nSearchRoadIdx].y = pos.y;
								return nuTRUE;
							}
						}
					}
                    ++nroad;
                }
            }
        }
        ++nln;
    }
    return nuFALSE;
}
nuWORD CSearchRoad::GetRoadTownID()
{
    return m_nSearchRoadTownID;
}
nuWORD CSearchRoad::GetRoadCityID()
{
    return m_nSearchRoadCityID;
}
nuVOID CSearchRoad::ResetCrossSearch()
{
    m_nRCSearchNameAddr = -1;
    m_nRCSearchTag = 0;
    m_nRCSearchLastCount = 0;
    m_nBLIdx = 0;
    m_nBLIdxM = 0;
    m_RCTotal = -1;
    nuMemset(m_SearchCrossRes, 0, sizeof(m_SearchCrossRes));
}

nuUINT CSearchRoad::GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords)
{
    nuUINT nRes = 0;
    nuUINT nBufferTag = 0;
    nuWORD nAddWord = 0;
    nuWORD i = 0;
    nuWORD *pTagWord = NULL;
    nuBYTE idx = 0;
    nuWORD *pName = NULL;
#ifdef _SEARCH_ROAD_MAINROADMODE_
    // need more deal for efficiency
#endif
    while (nBufferTag<NURO_SEARCH_BUFFER_MAX_ROAD && m_SearchRoadRes[nBufferTag].type)
    {
        if (nRes==nMaxWords)
        {
            break;
        }
        nAddWord = 0;
        idx = 0;
        GetRoadName(m_SearchRoadRes[nBufferTag].nameAddr
            , pName
            , 0
            , NULL);
        if (pHeadWords)
        {
            while (pName[idx])
            {
                if (pHeadWords[idx])
                {
                    if (pName[idx]==pHeadWords[idx])
                    {
                        ++idx;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    nAddWord = pName[idx];
                    break;
                }
            }
        }
        else
        {
            nAddWord = pName[0];
        }
        if (nAddWord == _SEARCH_ROAD_FJUDGE_ENDWORD_
            && m_SearchRoadRes[nBufferTag].type == SEARCH_BT_ROAD )
        {
            nAddWord = 0;	
        }
        if (nAddWord)
        {
            if (!nRes)
            {
                pBuffer[0] = nAddWord;
                ++nRes;
            }
            else
            {
                for (i=0; i<nRes; i++)
                {
                    if (nAddWord==pBuffer[i])
                    {
                        break;
                    }
                }
                if (i==nRes)
                {
                    pBuffer[nRes] = nAddWord;
                    ++nRes;
                }
            }
        }
        ++nBufferTag;
    }
    return nRes;
}

nuUINT CSearchRoad::GetRoadKeyDanyinMatchWords(nuWORD** buffer)
{    
    nuUINT danyinLen = nuWcslen(m_nSearchRoadKeyDanyin);
    if (!danyinLen)
    {
        return 0;
    }
    nuUINT danyinLenSaveLen = danyinLen;
    if (danyinLenSaveLen > 15)
    {
        danyinLenSaveLen = 15;
    }
    if (!danyinLen)
    {
        return 0;
    }
    if (!FlushRNInfo())
    {
        return 0;
    }
    m_danYinMatchWordsAreaTag.nMapID = m_searchInfo.nMapID;
    if ( m_searchInfo.nCityID	== m_danYinMatchWordsAreaTag.nCityID
        && m_searchInfo.nTownID == m_danYinMatchWordsAreaTag.nTownID
        && !nuWcscmp(m_nSearchRoadKeyDanyin, m_danYinMatchWordsTag) )
    {
        if (buffer)
        {
            *buffer = m_danYinMatchWordsBuffer;
        }
        return m_danYinMatchWordsCount;
    }
    m_danYinMatchWordsCount = 0;//reset
    nuMemset(m_danYinMatchWordsBuffer, 0, sizeof(m_danYinMatchWordsBuffer));
    m_danYinMatchWordsAreaTag.nCityID = m_searchInfo.nCityID;
    m_danYinMatchWordsAreaTag.nTownID = m_searchInfo.nTownID;
    nuMemcpy(m_danYinMatchWordsTag, m_nSearchRoadKeyDanyin, sizeof(m_nSearchRoadKeyDanyin));
    nuWORD tag=0;
	nuWORD tagmax=0;
    if (m_searchInfo.nCityID==(nuWORD)-1)
    {
        tag = 0;
        tagmax = m_nCityTotal-1;
    }
    else
    {
        tag = m_searchInfo.nCityID;
        tagmax = m_searchInfo.nCityID;
    }

    nuBYTE* pRnc = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMMIdx);
    if (!pRnc)
    {
        return -1;
    }
    nuUINT nRes = 0;
    pRnc += m_nMMCTLen + m_nMMSzyLen + m_nMMRNLen;

    SEARCH_RNC_HEADER *pRncHeader = (SEARCH_RNC_HEADER*)pRnc;
    SEARCH_RNC_CITYINFO *pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pRncHeader->nCTInfoStartAddr+4);
    while (pCityInfo->nCityCode != tag)
    {
        if (pCityInfo->nCityCode > tagmax)
        {
            return 0;
        }
        pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pCityInfo->nNextCityAddr);
    }

    SEARCH_RNC_TOWNINFO *pTownInfo = NULL;
    nuUINT *pNameAddr = NULL;
    nuWORD nTownTag = 0;
    nuWORD nRoadTag = 0;
    nuWORD name2[EACH_BUFFER_NAME_LEN] = {0};
    nuWORD *pName = NULL;
    nuINT matchpos = 0;
    nuUINT repeattag = 0;
    nuUINT nameLentag = 0;
    nuUINT danyinLentag = 0;
    nuBOOL   repeat = nuFALSE;
    while (tag<=tagmax && nRes<200)
    {
        nTownTag = 0;
        pTownInfo = (SEARCH_RNC_TOWNINFO*)((nuBYTE*)pCityInfo + sizeof(SEARCH_RNC_CITYINFO));
        while (nTownTag<pCityInfo->nTownCount && nRes<200)
        {
            if (m_danYinMatchWordsAreaTag.nTownID!=(nuWORD)-1 
                && pTownInfo[nTownTag].nTownCode!=m_danYinMatchWordsAreaTag.nTownID)
            {
                ++nTownTag;
                continue;
            }
            pNameAddr = (nuUINT*)(pRnc + pTownInfo[nTownTag].nRNStartAddr);
            nRoadTag=0;
            while ( nRoadTag<pTownInfo[nTownTag].nRoadCount && nRes<200)
            {
                nameLentag = GetRoadName(pNameAddr[nRoadTag], pName, 0, 0);
#if 1
                for (nuUINT i=0; i<nameLentag; ++i)
                {
                    if (pName[i] == _SEARCH_ROAD_FJUDGE_ENDWORD_
                        || pName[i] == _SEARCH_ROAD_FJUDGE_ENDWORD1_)
                    {
                        nameLentag = i+1;
                        break;
                    }
                }
#endif
                if (nameLentag<danyinLen)
                {
                    ++nRoadTag;
                    continue;
                }
                danyinLentag = GetRoadDanyin(pNameAddr[nRoadTag], name2, EACH_ROAD_NAME_LEN);
                if ( !danyinLentag || danyinLentag<nameLentag)
                {
                    ++nRoadTag;
                    continue;
                }
                matchpos = nuWcsKeyMatch(name2, m_danYinMatchWordsTag, nameLentag, danyinLen);
                if (-1 == matchpos)
                {
                    ++nRoadTag;
                    continue;
                }
                if (nRes)//check repeat
                {
                    repeat = nuFALSE;
                    for (repeattag=0; repeattag<nRes; ++repeattag)
                    {
                        if (!nuWcsncmp(&m_danYinMatchWordsBuffer[repeattag<<4], pName+matchpos, danyinLenSaveLen))
                        {
                            repeat = nuTRUE;
                            break;
                        }
                    }
                    if (repeat)
                    {
                        ++nRoadTag;
                        continue;
                    }
                }
                nuWcsncpy(&m_danYinMatchWordsBuffer[nRes<<4], &pName[matchpos], danyinLenSaveLen);
                m_danYinMatchWordsBuffer[(nRes<<4)+danyinLenSaveLen] = 0;
                ++nRes;
                ++nRoadTag;
            }
            ++nTownTag;
        }
        ++tag;
        if (tag<=tagmax)
        {
            pCityInfo = (SEARCH_RNC_CITYINFO*)(pRnc+pCityInfo->nNextCityAddr);
        }
    }
    m_danYinMatchWordsCount = nRes;
    return nRes;
}

nuVOID CSearchRoad::Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	nuINT	len=(nuINT)candylen;		//KeyWord????
	nuINT	iCmpIdx=candylen;			//KeyWordy?r??
	nuINT	iTotalNextWordCount=0;		//???r???贫q
	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;
	NURO_SEARCH_NORMALRES *pNORMALRES =(NURO_SEARCH_NORMALRES*)pRoadBuf;//?圈sRoad?W?侔}?C

	nuUINT i;
	for ( i=0; i<RoadBufLen; i++)
	{
		if (iCmpIdx < (nuINT)nuWcslen(pNORMALRES[i].name))
		{
			if (0 == nuWcsncmp(pNORMALRES[i].name, (nuWCHAR *)wKeyWord, iCmpIdx))	//?????r?辘?eN?应r(iCmpIdx) ???X?纽X?? ?i???U?z?P?_
			{
				nuBOOL bNeedAdd = nuTRUE;	
				nuINT  j=0;
				for (j=0; j<(nuINT)nuWcslen(pWord); j++)				//???铴?r?r?辘??DATA ?Y?S?????r?A?[?J???r?}?C??
				{
					if (pWord[j] == pNORMALRES[i].name[iCmpIdx])		
					{
						bNeedAdd = nuFALSE;									//???应r?? ?h?[?J??BOOL=FALSE
						break;
					}
				}
				if (nuTRUE == bNeedAdd)
				{
					pWord[iTotalNextWordCount] = pNORMALRES[i].name[iCmpIdx];
					iTotalNextWordCount++;				
				}
			}
		}
	}
	WordCont=iTotalNextWordCount;//???r???蛹?
}


nuVOID CSearchRoad::ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount, const nuUINT SelectWord, const nuBOOL ReSort)
{												
	nuINT	iTotalNextWordCount=0;				//???r???蛹?
	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;			//?圈s???r?}?C
	NURO_SEARCH_NORMALRES *pNORMALRES =(NURO_SEARCH_NORMALRES*)pRoadBuf;//?圈sRoad?W?侔}?C

	nuUINT i;
	for (i=0; i<RoadBufLen; i++)
	{
		if (0 == i)															//?膜@?应r???J pWCHAR[0]
		{
			pWord[n_iCount] = pNORMALRES[i].name[SelectWord];
			n_iCount++;
		}
		else if (pNORMALRES[i].name[SelectWord] != pWord[n_iCount-1]) 	//???应r???P?P???r?}?C[n_iCount-1]???r??
		{
			nuBOOL bisAdd = nuTRUE;
			nuUINT j;
			for (j=0; j<n_iCount; j++)                                      //?????r?辘?eN?应r(iCmpIdx) ???X???[?J???r 
			{
				if (pNORMALRES[i].name[SelectWord] == pWord[j]) 
				{
					bisAdd = nuFALSE;											//???应r?? ?h?[?J??BOOL=FALSE
					break;
				}
			}
			if (nuTRUE == bisAdd)
			{
				pWord[n_iCount] = pNORMALRES[i].name[SelectWord];
				n_iCount++;
			}
		}
	} 
}

nuBOOL CSearchRoad::ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	nuBOOL  LastWord=nuTRUE;			//?P?_?O?_???太??@?应r
	nuINT n=candylen;				//KeyWordy?r??

	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;
	NURO_SEARCH_NORMALRES *n_pNORMALRES =(NURO_SEARCH_NORMALRES*)pRoadBuf;//?圈sRoad?W?侔}?C

	nuUINT i;
	for (i=0; i<RoadBufLen; i++)
	{
		nuWCHAR* wOUTPUTWord = n_pNORMALRES[i].name;		
		if(0 != nuWcsncmp((nuWCHAR*)wKeyWord,wOUTPUTWord, n))		//?PKeyWord???? ?Y???鄣? ?N?PRoad?W?侔}?C???U?@??DATA????
		{
			continue;
		}
		if(0 != n_pNORMALRES[i].name[n] )					//?????太??@?应r 	LastWord = nuFALSE;
		{
			LastWord = nuFALSE;
		}
		if(0 == n_pNORMALRES[i].name[n] )					//?太??@?应r
		{
			continue;
		}
		if (0 == i||0 == n_iCount)								
		{
			pWord[n_iCount] = n_pNORMALRES[i].name[n];			//?膜@?应r???J pWCHAR[0]
			n_iCount++;
		}
		else if (n_pNORMALRES[i].name[n] != pWord[n_iCount-1])	//???应r???P?P???r?}?C[n_iCount-1]???r??
		{
			nuBOOL bisAdd = nuTRUE;
			nuUINT j;
			for (j=0; j<n_iCount; j++)
			{
				if (n_pNORMALRES[i].name[n] == pWord[j])			//???应r?? ?h?[?J??BOOL=FALSE
				{
					bisAdd = nuFALSE;
					break;
				}
			}
			if (nuTRUE == bisAdd)
			{
				pWord[n_iCount] = n_pNORMALRES[i].name[n];
				n_iCount++;
			}
		}
	}
	return LastWord;//?P?_?O?_???太??@?应r
}
bool CSearchRoad:: Sort_KEY_index(nuWORD nCode) //Prosper 2011.12.07
{
	nuINT  n = -1;
	nuBOOL bSame = false;
	if(nCode<12500)
	{
		return nuFALSE;
	}
	if(nKeyCount == 0)
	{
		KEY_Data[nKeyCount]=nCode;
		nKeyCount++;
		return true;
	}
	while(n++ < nKeyCount)
	{
		if(nCode == KEY_Data[n])
		{
			bSame=true;		  
			break;
		}
	}
	if(!bSame)
	{
		KEY_Data[nKeyCount] = nCode;
		nKeyCount++;
	}
	return true;

}
nuBOOL CSearchRoad:: GetRoadSKB(void* pSKBBuf, nuUINT& nCount) //Prosper 2011.12.07
{

	nuMemcpy(pSKBBuf,KEY_Data,sizeof(KEY_Data));
    nCount=nKeyCount;
	return true;
}
nuBOOL CSearchRoad::GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen)
{
	DYWORD *m_SearchRes = (DYWORD*)m_pMmApi->MM_GetDataMassAddr(m_nSearchResIdx);
	if(nMaxLen==0)
	{
		nTotal = m_nDYWordIdx;
		return nuTRUE;
	}
	else
	{
		if(m_bSort)
		{
		m_bSort = nuFALSE;
		DYWORD temp={0};
		nuINT nIdx=m_nDYWordIdx;
		//Bubble
		for(nuINT ii=0;ii<nIdx-1;ii++)
		{
			nuINT times=nIdx-2;
			for (nuINT j=0; j<times; j++)
			{ 
				nuINT tag2 = 0;
				nuINT last = times-j;
				for(nuINT i=1;i<=last;i++)
				{ 
					if((int)m_SearchRes[tag2].nFindCount > (int)m_SearchRes[i].nFindCount)
					{
						tag2 = i;
					} 
				} 
				if (tag2!=last)
				{
					temp = m_SearchRes[last];
					m_SearchRes[last] = m_SearchRes[tag2];
					m_SearchRes[tag2] = temp;
				} 

			}
		}

		}
		nuMemcpy(pBuffer,m_SearchRes,nMaxLen);
	}
	return true; 
}
nuBOOL	CSearchRoad::NAME_Order(nuINT n)
{

	nuINT *m_Town =(nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nTownIdx);
	n = m_nNameIdx;
	

	if (n==0 || nuTRUE) //???????
	{
		return nuFALSE;
	}

return nuTRUE;
}
nuINT	CSearchRoad::TransCode(nuINT code)
{	
	//?????ARRAY unicode ??BIG5
	//Trans_DATA *test=new Trans_DATA[2]({123,456},{156,456});
	//test[0]=(0x456,0x123);

	//nuINT *try1=new nuINT[0](10);

	nuINT idx=-1;
	nuINT nLimit=nSortLimt;
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//???????
	nuCHAR	*pEdn = NULL;	  //???????	

	low	  = 0;
	upper = nLimit-1;

	//?Y??????????blow~upper?d?? ?N?????? ?????^??-1
	if(code<data[low].nUniCode) 
	{
		return code;
	}

	if(code>data[upper].nUniCode)
	{
		return -1;
	}

	while(low <= upper) 
	{ 
		nuINT mid = (low+upper) / 2;
		lLocation = data[mid].nUniCode;

		if(lLocation < code) 
		{
			low = mid+1; 
		}
		else if(lLocation > code) 
		{	
			upper = mid - 1; 
		}
		else 
		{    
			return data[mid].nBig;
		}
	} 
	return -1; 
}

nuBOOL CSearchRoad:: GetNEXTWORD(nuVOID* pKEYBuf, nuUINT& nCount) //Prosper 2011 12.07
{

	nuMemcpy(pKEYBuf,m_wsNEXT_WORD_Data,sizeof(m_wsNEXT_WORD_Data));
	nCount = nNEXT_WORD_COUNT;
	return nuTRUE;
}
nuBOOL CSearchRoad::nuWcsFuzzyJudge_O_FORSEARCHROAD(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen
						  , nuINT& startpos, nuINT& diffpos)
{
	if (desLen<keyLen)
	{
		return nuFALSE;
	}
	nuUINT i = 0;
	startpos = -1;
	diffpos = 0;
    for (i=0; i<desLen; ++i)
	{
        if (nuWcsSameAs(&pDes[i], &pKey[0]))
		{
			if (-1 == startpos)
			{
				startpos = i;
			}
			if (keyLen==1)
			{
				diffpos = i - startpos;
				return nuTRUE;
			}
			else
			{
				--keyLen;
				++pKey;
			}
		}
		else if ((desLen-i-1)<keyLen)
		{
			return nuFALSE;
		}
	}
	return nuFALSE;
}