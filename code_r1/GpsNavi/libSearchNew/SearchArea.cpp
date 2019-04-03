
#include "SearchArea.h"
#include "MapFileCbZ.h"

CSearchArea::CSearchArea()
{
    Init();
}

CSearchArea::~CSearchArea()
{
    Free();
}

nuBOOL CSearchArea::Init()
{
    FreeCBInfo();
    return nuTRUE;
}

nuVOID CSearchArea::Free()
{
    FreeCBInfo();
}

nuUINT CSearchArea::LoadCBInfo(nuWORD nMMIdx, nuUINT nMaxLen)
{
    nuLONG nMapIdx = m_searchInfo.nMapID;
    if (nMapIdx!=m_nST_MapID)
    {
        FreeCBInfo();
    }
    nuBYTE *pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    //	每次加载 都保证读取最新数据
    // 	if (pCBInfo && m_nCBInfoLen)
    // 	{
    // 		return m_nCBInfoLen;
    // 	}
    pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(nMMIdx);
    if (!pCBInfo)
    {
        return 0;
    }
    CMapFileCbZ cbfile;
    if (!cbfile.Open(nMapIdx))
    {
        return 0;
    }
    m_nCBInfoLen = cbfile.GetLength();
    if (m_nCBInfoLen>nMaxLen)
    {
        FreeCBInfo();
        return 0;
    }
    if (!cbfile.ReadData(0, pCBInfo, m_nCBInfoLen))
    {
        FreeCBInfo();
        return 0;
    }
    m_nCBInfoLen = ((m_nCBInfoLen+3)>>2)<<2;
    m_nCityTotal = *((nuUINT*)pCBInfo);
    m_nTownTotal = *((nuUINT*)pCBInfo+1);
    m_nFirstTownIdxBuffer[0] = 0;
    m_nCBInfoIdx = nMMIdx;
    m_nST_MapID = nMapIdx;
    return m_nCBInfoLen;
}

nuVOID CSearchArea::FreeCBInfo()
{
    m_nTownTotal = 0;
    m_nCityTotal = 0;
    m_nCBInfoLen = 0;
    m_nCBInfoIdx = -1;
    m_nST_MapID = -1;
    nuMemset(m_nFirstTownIdxBuffer, 0xff, sizeof(m_nFirstTownIdxBuffer));
}

nuUINT CSearchArea::GetCityTotal()
{
    return m_nCityTotal;
}
nuUINT CSearchArea::GetTownTotal()
{
    return m_nTownTotal;
}

nuUINT CSearchArea::GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    return 0;
}
nuUINT CSearchArea::GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    if (!pBuffer)
    {
        return 0;
    }
    nuUINT nNum = nMaxLen/EACH_CITY_NAME_LEN;
    if (!nNum)
    {
        return 0;
    }
    if (nNum>m_nCityTotal)
    {
        nNum = m_nCityTotal;
    }
    nuBYTE* pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    if (!pCBInfo)
    {
        return 0;
    }
    nuMemcpy(pBuffer, pCBInfo+8, EACH_CITY_NAME_LEN*nNum);
    return nNum;
}
nuUINT CSearchArea::GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    nuUINT count = nMaxLen/EACH_TOWN_NAME_LEN;
    nuBYTE *pNameBuffer = (nuBYTE*)pBuffer;
    nuBYTE* pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    if (!pCBInfo)
    {
        return 0;
    }
    nuUINT *pAddrByCT = (nuUINT*)(pCBInfo+m_nCBInfoLen-(m_nTownTotal<<2));
    if (m_nFirstTownIdxBuffer[m_searchInfo.nCityID]==(nuWORD)-1)
    {
        m_nFirstTownIdxBuffer[m_searchInfo.nCityID]
            = SearchTownFirst(m_searchInfo.nCityID, pCBInfo, m_nTownTotal);
    }
    
    nuWORD nTownIdx = m_nFirstTownIdxBuffer[m_searchInfo.nCityID];
    if (nTownIdx==(nuWORD)-1)
    {
        return 0;
    }
	SEARCH_CB_TOWNINFO tf = {0};
    nuMemset(&tf, 0, sizeof(tf));
    nuUINT nRes = 0;
    while (nTownIdx<m_nTownTotal && nRes<count)
    {
        if (!GetTownInfo(pCBInfo, pAddrByCT[nTownIdx], &tf))
        {
            break;
        }
        if (tf.nCityCode == m_searchInfo.nCityID)
        {
            nuMemcpy(pNameBuffer, tf.wsTownName, EACH_TOWN_NAME_LEN);
            pNameBuffer += EACH_TOWN_NAME_LEN;
            ++nTownIdx;
            ++nRes;
        }
        else
        {
            break;
        }
    }
    return nRes;
    
}

nuBOOL CSearchArea::GetTownInfo(nuBYTE *pCBInfo, nuUINT addr, SEARCH_CB_TOWNINFO *pTI)
{
    if (pTI)
    {
        nuMemcpy(pTI, pCBInfo + addr, sizeof(SEARCH_CB_TOWNINFO));
        return nuTRUE;
    }
    return nuFALSE;
}
nuWORD CSearchArea::GetTownInfo_CC(nuBYTE *pCBInfo, nuUINT addr)
{
    SEARCH_CB_TOWNINFO *pTI = (SEARCH_CB_TOWNINFO*)(pCBInfo + addr);
    return pTI->nCityCode;
}
nuWORD CSearchArea::GetTownInfo_TC(nuBYTE *pCBInfo, nuUINT addr)
{
    SEARCH_CB_TOWNINFO *pTI = (SEARCH_CB_TOWNINFO*)(pCBInfo + addr);
    return pTI->nTownCode;
}
nuBOOL CSearchArea::GetCityNameByID(nuWORD cityID, nuVOID *pBuffer, nuUINT nMaxLen)
{
    if ((nuWORD)-1==cityID)
    {
        return nuFALSE;
    }
    if (nMaxLen<EACH_CITY_NAME_LEN)
    {
        return nuFALSE;
    }
    nuBYTE* pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    if (!pCBInfo)
    {
        return nuFALSE;
    }
    pCBInfo += 8 + EACH_CITY_NAME_LEN*cityID;
    nuMemcpy(pBuffer, pCBInfo, EACH_CITY_NAME_LEN);
    return nuTRUE;
}
nuBOOL CSearchArea::GetTownNameByID(nuWORD cityID
                                  , nuWORD townID
                                  , nuVOID *pBuffer
                                  , nuUINT nMaxLen)
{
    if (cityID==(nuWORD)-1 || townID==(nuWORD)-1)
    {
        return nuFALSE;
    }
    if (nMaxLen<EACH_TOWN_NAME_LEN)
    {
        return nuFALSE;
    }
	SEARCH_CB_TOWNINFO tf = {0};
    nuBYTE* pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    if (!pCBInfo)
    {
        return nuFALSE;
    }
    nuUINT *pAddrByCT = (nuUINT*)(pCBInfo+m_nCBInfoLen-(m_nTownTotal<<2));
    
    if (m_nFirstTownIdxBuffer[cityID]==(nuWORD)-1)
    {
        m_nFirstTownIdxBuffer[cityID] 
            = SearchTownFirst(cityID, pCBInfo, m_nTownTotal);
        if (m_nFirstTownIdxBuffer[cityID]==(nuWORD)-1)
        {
            return nuFALSE;
        }
    }
    if (m_nFirstTownIdxBuffer[cityID]+townID >= (nuWORD)m_nTownTotal)
    {
        return nuFALSE;
    }
    if (GetTownInfo(pCBInfo, pAddrByCT[m_nFirstTownIdxBuffer[cityID]+townID], &tf))
    {
        if (tf.nTownCode!=townID)
        {
            return nuFALSE;
        }
        nuMemcpy(pBuffer, tf.wsTownName, EACH_TOWN_NAME_LEN);
        return nuTRUE;
    }
    return nuFALSE;
}

nuWORD CSearchArea::SearchTownFirst(nuWORD nCityIdx, nuBYTE *pCBInfo, nuUINT nTownCount)
{
    nuUINT *pAddrByCT = (nuUINT*)(pCBInfo+m_nCBInfoLen-(m_nTownTotal<<2));
    if (nTownCount==1)
    {
        if (GetTownInfo_CC(pCBInfo, pAddrByCT[0]) == nCityIdx)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }	
    nuUINT startIdx = 0;
    nuUINT endIdx = nTownCount-1;
    nuUINT centerIdx = 0;
    nuWORD nowcityid = -1;
    nowcityid = GetTownInfo_CC(pCBInfo, pAddrByCT[startIdx]);
    if (nowcityid == nCityIdx)
    {
        return startIdx;
    }
    else if (nowcityid > nCityIdx)
    {
        return -1;
    }
    nowcityid = GetTownInfo_CC(pCBInfo, pAddrByCT[endIdx]);
    if (nowcityid == nCityIdx)
    {
        if (startIdx == endIdx-1)
        {
            return endIdx;
        }
        else if (GetTownInfo_CC(pCBInfo, pAddrByCT[endIdx-1]) != nCityIdx)
        {
            return endIdx;
        }
    }
    else if (nowcityid < nCityIdx || nowcityid == (nuWORD)-1)
    {
        return -1;			
    }
    while (endIdx-startIdx>=2)
    {
        centerIdx = (endIdx+startIdx)/2;
        nowcityid = GetTownInfo_CC(pCBInfo, pAddrByCT[centerIdx]);
        if (nowcityid == nCityIdx)
        {
            if (startIdx == centerIdx-1)
            {
                return centerIdx;
            }
            else if (GetTownInfo_CC(pCBInfo, pAddrByCT[centerIdx-1] )!= nCityIdx)
            {
                return centerIdx;
            }
            else
            {
                endIdx = centerIdx;
            }
        }
        else if (nowcityid > nCityIdx)
        {
            endIdx = centerIdx;
        }
        else
        {
            startIdx = centerIdx;
        }
    }
    return -1;
}

nuUINT CSearchArea::GetTownBlkInfo(nuWORD nCityIdx
                                   , nuWORD nTownID
                                   , nuBYTE *pBuffer
                                   , nuUINT nMaxLen)
{
    if (!pBuffer)
    {
        return 0;
    }
    nuBYTE* pCBInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nCBInfoIdx);
    if (!pCBInfo)
    {
        return 0;
    }
    nuUINT *pAddrByCT = (nuUINT*)(pCBInfo+m_nCBInfoLen-(m_nTownTotal<<2));
    if (m_nFirstTownIdxBuffer[nCityIdx]==(nuWORD)-1)
    {
        m_nFirstTownIdxBuffer[nCityIdx] 
            = SearchTownFirst(nCityIdx, pCBInfo, m_nTownTotal);
        if (m_nFirstTownIdxBuffer[nCityIdx]==(nuWORD)-1)
        {
            return 0;
        }
    }
    if (m_nFirstTownIdxBuffer[nCityIdx]+nTownID >= (nuWORD)m_nTownTotal)
    {
        return 0;
    }
    nuUINT townInfoAddr = pAddrByCT[m_nFirstTownIdxBuffer[nCityIdx]+nTownID];
	SEARCH_CB_TOWNINFO ci = {0};
    if (!GetTownInfo(pCBInfo, townInfoAddr, &ci))
    {
        return 0;
    }
    nuUINT nResLen = 4+(ci.nTownBlkInfoCount<<3);
    if (nMaxLen<nResLen)
    {
        return 0;
    }
    nuMemcpy(pBuffer, pCBInfo+townInfoAddr+104, nResLen);
    return nResLen;
}
