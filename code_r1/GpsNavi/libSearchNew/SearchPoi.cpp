
#include "SearchPoi.h"
#include "MapFilePtnY.h"
#include "MapFilePnY.h"
#include "MapFilePaY.h"
#include "MapFilePiY.h"
#include "MapFileBhY.h"
#include "MapFilePdwY.h"
#include "MapFileCarY.h"
#include "MapFileSPIY.h"
#include "NuroClasses.h"
#include "MapFilePzyY.h"

CSearchPoi::CSearchPoi()
{
    m_bRun = nuFALSE;
    m_bSA = nuFALSE;
	nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    nuMemset(m_nSearchPhone, 0, sizeof(m_nSearchPhone));
    nuMemset(&m_sAreaRect, 0, sizeof(m_sAreaRect));
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));

	//Prosper 20111207 
#ifndef VALUE_EMGRT 
	nKeyCount=0;
	nuMemset(KEY_Data,0,sizeof(KEY_Data));
	m_bNameSort = nuTRUE;
	m_bSort = nuFALSE;
#else
	m_bNameSort = nuFALSE;
#endif
	nNEXT_WORD_COUNT = 0;
	nuMemset(m_wsNEXT_WORD_Data,0,sizeof(m_wsNEXT_WORD_Data));

    m_nCarFac = -1;
    m_nSPType = -1;
	m_nDY_countIdx = -1 ;
	m_nsort_countsIdx = -1;
    Free();
	m_bComCompare   = nuFALSE;
	m_bKeyNameFuzzy = nuTRUE; 
	m_bIndexSort = nuFALSE;   //Prosper add ,for WordIdx_Sort

	m_nWordStartIdxMemIdx =-1;
	m_nWordLensMemIdx =-1;
	m_nCityIDLoad = -1;
	m_bSetAreaCenterPos = nuFALSE;
	//m_nWordStartIdx = NULL;
	//m_nWordLens = NULL;
	
}

CSearchPoi::~CSearchPoi()
{
    Free();
}

nuBOOL CSearchPoi::Init(nuWORD nIdx, nuUINT nLen, nuUINT nIdleAddr)
{
    if (!m_pMmApi || !m_pFsApi || !nIdleAddr)
    {
        return nuFALSE;
    }
    if (!m_pMmApi->MM_GetDataMassAddr(nIdx) || !nLen)
    {
        return nuFALSE;
    }
    ClearSearchInfo();
	ClearPZYInfo();
    m_nLoadBufferIdx = nIdx;
    m_nLoadBufferMaxLen = nLen;
    m_nLB_CTLen = nIdleAddr;   
    m_bRun = nuTRUE;
	if(m_nWordStartIdxMemIdx == (nuWORD)-1)
	{
		nuLONG nSize = NURO_SEARCH_BUFFER_MAX_DYPOI* sizeof(nuINT);
		nuINT* m_nWordStartIdx = (nuINT*)m_pMmApi->MM_GetDataMemMass(nSize,&m_nWordStartIdxMemIdx);
		nuMemset(m_nWordStartIdx,0,sizeof(NURO_SEARCH_BUFFER_MAX_DYPOI));

	}
	if(m_nWordLensMemIdx == (nuWORD)-1)
	{
		nuLONG nSize = NURO_SEARCH_BUFFER_MAX_DYPOI* sizeof(nuINT);
		nuINT* m_nWordLens = (nuINT*)m_pMmApi->MM_GetDataMemMass(nSize,&m_nWordLensMemIdx);
		nuMemset(m_nWordLens,0,sizeof(NURO_SEARCH_BUFFER_MAX_DYPOI));
	}
	
	
	return nuTRUE;
}

nuVOID CSearchPoi::Free()
{
    m_bRun = nuFALSE;
    ClearSearchInfo();
	m_nPzyMin = -1;
    m_nPzyMax = -1;
    m_nPzyZhuyin = -1;
    m_nLoadBufferIdx = -1;
    m_nLoadBufferMaxLen = 0;
    m_nLB_CTLen = 0;
	nFirstSort=-1;
	m_nCityID = -1;
	m_nMAPID = -1;
#ifdef VALUE_EMGRT
	/*if(pPoiInfoCN)
	{
		delete[] pPoiInfoCN;
		pPoiInfoCN = NULL;
	}*/
#endif
	if( m_nWordStartIdxMemIdx != (nuWORD)-1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nWordStartIdxMemIdx);
	}
	if( m_nWordLensMemIdx != (nuWORD)-1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nWordLensMemIdx);
	}

	if( m_nDY_countIdx != (nuWORD)-1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nDY_countIdx);
	}
	if( m_nsort_countsIdx != (nuWORD)-1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nsort_countsIdx);
	}

}
//-----------------------------------------------------------------------------------------------------
nuBOOL CSearchPoi::SetPoiPZZhuyin(nuWORD zhuyin)
{
	if(m_nPzyWord >0)nuMemset(m_searchInfo.name, 0 , sizeof(m_searchInfo.name));	

	ClearSearchInfo();
    ClearPZYInfo();
	m_nPzyMin = -1;
    m_nPzyMax = -1;
    m_nPzyZhuyin = zhuyin;

    return nuTRUE;	
}

nuUINT CSearchPoi::GetPoiPZTotal()
{
    return GetPoiPZList(NURO_SEARCH_ALL
        , -1
        , NULL
        , -1);
}
nuINT CSearchPoi::GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
    if (NURO_SEARCH_LAST == type)
    {
        GetPoiPZTotal();
    }
    if (!FlushPZYInfo())
    {
        return 0;
    }
    if (m_searchInfo.nTownID != m_nPzySearchTownID
        || m_nPzyMinT != m_nPzyMin
        || m_nPzyMaxT != m_nPzyMax
        || m_nPzyZhuyinT != m_nPzyZhuyin)
    {
        ResetPZSearch();
        m_nPzySearchTownID = m_searchInfo.nTownID;
        m_nPzyMinT = m_nPzyMin;
        m_nPzyMaxT = m_nPzyMax;
        m_nPzyZhuyinT = m_nPzyZhuyin;
    }
    
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nMax = NURO_MIN(nMax, nMaxLen/EACH_SZY_NAME_LEN);
    if (type==NURO_SEARCH_LAST)
    {
        if (!m_nPzyResTotal || m_nPzyResTotal==(nuUINT)-1)
        {
            return 0;
        }
        realMax = m_nPzyResTotal%nMax;
        if (!realMax)
        {
            realMax = nMax;
        }
        realTag = m_nPzyResTotal-realMax;
    }
    else
    {
        if (!TranslateSS(type, nMax, m_nPzySearchTag, m_nPzySearchLastCount, &realTag, &realMax))
        {
            return 0;
        }
    }
    nuUINT realCount = 0;
    nuWORD *pResBuffer = (nuWORD*)pBuffer;
    if (type == NURO_SEARCH_ALL)
    {
        if ((nuUINT)-1 != m_nPzyResTotal)
        {
            return m_nPzyResTotal;
        }
        realMax = NURO_SEARCH_BUFFER_MAX_ROADSZ;
    }
    if ((nuUINT)-1==m_nPzyResTotal)
    {
        m_nPzyResTotal = SearchPoiPZ();
        if (type == NURO_SEARCH_ALL)
        {
            return m_nPzyResTotal;
        }
    }
    while (realCount<realMax && realTag<m_nPzyResTotal)
    {
        *pResBuffer = m_nPzyWordbuffer[realTag];
        ++pResBuffer;
        ++realCount;
        ++realTag;
    }
    if (!realCount)
    {
        return 0;
    }
    m_nPzySearchTag = realTag;
    m_nPzySearchLastCount = realCount;
    return m_nPzySearchLastCount;
}
nuWORD CSearchPoi::SearchPoiPZ()
{
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nLB_CTLen;
    nuWORD realTag = 0;
    nuWORD realCount = 0;
    SEARCH_SZY_STROKE *pStroke = (SEARCH_SZY_STROKE*)p;
    nuWORD i=0;
    nuBOOL bNext = nuFALSE;
    for ( ;realTag<m_nPzyLoadNum && realCount<NURO_SEARCH_BUFFER_MAX_ROADSZ; ++realTag)
    {
        if (m_nPzySearchTownID!=(nuWORD)-1)
        {
            if (pStroke[realTag].nTownID != m_nPzySearchTownID)
            {
                continue;
            }
        }
        if ( (m_nPzyZhuyinT!=(nuWORD)-1 && pStroke[realTag].nZhuyin==m_nPzyZhuyinT)
            || (m_nPzyZhuyinT==(nuWORD)-1 && pStroke[realTag].nStrokes>=m_nPzyMinT 
            && pStroke[realTag].nStrokes<=m_nPzyMaxT) )			
        {
            if (realCount)
            {
                if (pStroke[realTag].nWord == m_nPzyWordbuffer[realCount-1])
                {
                    continue;
                }
                if ((nuWORD)-1==m_nPzySearchCityID)
                {
                    bNext = nuFALSE;
                    for (i=0; i<(realCount-1); ++i)
                    {
                        if (pStroke[realTag].nWord == m_nPzyWordbuffer[i])
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
            m_nPzyWordbuffer[realCount] = pStroke[realTag].nWord;
            ++realCount;
        }
    }    
    return realCount;
}

nuVOID CSearchPoi::ResetPZSearch()
{
	m_nPzyWord=0;
    m_nPzySearchTag = 0;
    m_nPzySearchLastCount = 0;

    m_nPzyMinT = -1;
    m_nPzyMaxT = -1;
    m_nPzyZhuyinT = -1;
    
    m_nPzySearchTownID = -1;
    m_nPzyResTotal = -1;
    nuMemset(m_nPzyWordbuffer, 0, sizeof(m_nPzyWordbuffer)); 	
   // nuMemset(m_SearchRes, 0, sizeof(m_SearchRes));
    
   // ClearPZYInfo();
}
nuVOID CSearchPoi::ClearPZYInfo()
{
    m_nMMPzyLen = 0;
    m_nPzyLoadNum = 0;
    m_nPzySearchCityID = -1;
    m_nPzySearchMapID = -1;
    ResetPZSearch();
	ClearSearchInfo();
}
nuUINT CSearchPoi::FlushPZYInfo()
{
     ClearPZYInfo();
    if (m_searchInfo.nMapID==(nuLONG)-1 || m_nLoadBufferIdx ==(nuWORD)-1)
    {
        ClearPZYInfo();
        return 0;
    }
    if (m_nMMPzyLen
        && m_searchInfo.nMapID == m_nPzySearchMapID
        && m_searchInfo.nCityID == m_nPzySearchCityID)
    {
        return m_nMMPzyLen;
    }
    ClearPZYInfo();
    nuBYTE *p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!p)
    {
        return 0;
    }
    p += m_nLB_CTLen ;
    CMapFilePzyY pzyfile; 
    if (!pzyfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    m_nMMPzyLen = pzyfile.GetPzyInfo_CITY(m_searchInfo.nCityID, p, m_nLoadBufferMaxLen -m_nLB_CTLen );
    if (!m_nMMPzyLen)
    {
        ClearPZYInfo();
        return 0;
    }
    m_nPzyLoadNum = m_nMMPzyLen/sizeof(SEARCH_SZY_STROKE);
    m_nMMPzyLen = ((m_nMMPzyLen+3)>>2)<<2;
    m_nPzySearchMapID = m_searchInfo.nMapID;
    m_nPzySearchCityID = m_searchInfo.nCityID;
    return m_nMMPzyLen;
}

nuBOOL CSearchPoi::SetPoiPZ(nuUINT nResIdx) //prosper Set 選取的首字
{
    m_nPzyWord = 0;

   

    if (nResIdx>=m_nPzySearchLastCount)
    {
        return nuFALSE;
    }
    m_nPzyWord = m_nPzyWordbuffer[m_nPzySearchTag + nResIdx - m_nPzySearchLastCount];

    if (m_nPzyWord!=(nuWORD)0)
    {
        return nuTRUE;
    }
    else
    {
        return nuFALSE;
    }
}

//------------------------------------------------------------------------------------------------
nuBOOL CSearchPoi::GetPoiMoreInfo(nuUINT nResIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx)
{
    if (!pPaInfo)
    {
        return nuFALSE;
    }
    nuMemset(pPaInfo, 0, sizeof(pPaInfo));
    nuWORD nIdx = nResIdx;
    if (!bRealIdx)
    {
        nIdx = m_nSearchTagPos + nResIdx - m_nLastGetCount;
    }
    /*
    if (m_nLB_PALen)
    {
        nuBYTE* pPaFile = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
        if (!pPaFile)
        {
            return nuFALSE;
        }
        if (m_nSearchPhone[0])
        {
            pPaFile += m_nLB_CTLen + m_nLB_PNLen + m_SearchRes[nIdx].extAddr;
        }
        else
        {
            pPaFile += m_nLB_CTLen + m_SearchRes[nIdx].extAddr;
        }
        nuBYTE len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->phone, pPaFile, NURO_MIN(62, len));
            pPaFile += len;
        }
        len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->nameext, pPaFile, NURO_MIN(254, len));
            pPaFile += len;
        }
        len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->fex, pPaFile, NURO_MIN(62, len));
            pPaFile += len;
        }
        len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->addr, pPaFile, NURO_MIN(254, len));
            pPaFile += len;
        }
        len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->web, pPaFile, NURO_MIN(254, len));
            pPaFile += len;
        }
        len = *pPaFile;
        ++pPaFile;
        if ((nuBYTE)-1==len)
        {
            len = 0;
        }
        if (len)
        {
            nuMemcpy(pPaInfo->time, pPaFile, NURO_MIN(254, len));
        }
        return nuTRUE;
    }
    else
    */
    {
        CMapFilePaY pafile;
        if (!pafile.Open(m_SearchBufferInfo.nMapID))
        {
            return nuFALSE;
        }
        return pafile.GetPaInfo(m_SearchRes[nIdx].extAddr, pPaInfo);
    }
}

nuBOOL CSearchPoi::GetPoiPos(nuUINT nResIdx, NUROPOINT* pos, nuBOOL bRealIdx)
{
    if (!pos)
    {
        return nuFALSE;
    }
    nuWORD nIdx = nResIdx;
    if (!bRealIdx)
    {
        nIdx = m_nSearchTagPos + nResIdx - m_nLastGetCount;
    }
    pos->x = m_SearchRes[nIdx].x;
    pos->y = m_SearchRes[nIdx].y;
    return nuTRUE;
}
nuWORD CSearchPoi::GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx)
{
    nuWORD nIdx = nResIdx;
    if (!bRealIdx)
    {
        nIdx = m_nSearchTagPos + nResIdx - m_nLastGetCount;
    }
    return m_SearchRes[nIdx].cityID;
}
nuWORD CSearchPoi::GetPoiTownID(nuUINT nResIdx, nuBOOL bRealIdx)
{
    nuWORD nIdx = nResIdx;
    if (!bRealIdx)
    {
        nIdx = m_nSearchTagPos + nResIdx - m_nLastGetCount;
    }
    return m_SearchRes[nIdx].townID;
}

nuUINT CSearchPoi::GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen)
{
    CMapFilePtnY ptnfile;
    if (!ptnfile.Open(m_searchInfo.nMapID))//-1
    {
        return 0;
    }
    return ptnfile.GetTP1NameAll((nuBYTE*)pBuffer, nMaxLen);
}

nuUINT  CSearchPoi::GetPoiNearDis(nuUINT nResIdx, nuBOOL bRealIdx/*=0*/)
{
    nuWORD nIdx = nResIdx;
    if (!bRealIdx)
    {
        nIdx = m_nSearchTagPos + nResIdx - m_nLastGetCount;
    }
    return m_SearchRes[nIdx].nDistance;
}

nuUINT CSearchPoi::GetPoiTP2All(nuVOID *pBuffer, nuUINT nMaxLen)
{
    CMapFilePtnY ptnfile;
    if (!ptnfile.Open(m_searchInfo.nMapID))//-1
    {
        return 0;
    }
    return ptnfile.GetTP2NameAll(m_searchInfo.nTP1ID, (nuBYTE*)pBuffer, nMaxLen);
}

nuUINT CSearchPoi::GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    CMapFileCarY carfile;
    if (carfile.Open(m_searchInfo.nMapID))
    {
        return carfile.GetCarFacNameAll(pBuffer, nMaxLen);
    }
    return 0;
}

nuBOOL CSearchPoi::SetPoiCarFac(nuUINT nResIdx)
{
	m_bComCompare = nuFALSE;
	
    m_nCarFac = nResIdx;
    if (m_nCarFac==(nuUINT)-1)
    {
        return nuFALSE;
    }
    m_bSA = nuFALSE;
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    nuMemset(m_nSearchPhone , 0, sizeof(m_nSearchPhone));
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
    m_nSPType = -1;
    return nuTRUE;
}
nuUINT CSearchPoi::GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
    CMapFileSPIY spifile;
    if (spifile.Open(m_searchInfo.nMapID))
    {
        return spifile.GetSPTypeNameAll(pBuffer, nMaxLen);
    }
    return 0;
}
nuBOOL CSearchPoi::SetPoiSPType(nuUINT nResIdx)
{
	m_bComCompare = nuFALSE;

    m_nSPType = nResIdx;
    if (m_nSPType==(nuUINT)-1)
    {
        return nuFALSE;
    }
    m_bSA = nuFALSE;
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    nuMemset(m_nSearchPhone , 0, sizeof(m_nSearchPhone));
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
    m_nCarFac = -1;
    return nuTRUE;
}
nuBOOL CSearchPoi::GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo)
{
    if (!pPhone || !pInfo || m_searchInfo.nMapID==(nuLONG)-1)
    {
        return nuFALSE;
    }
    nuUINT len = nuWcslen(pPhone);
    if (len>31)
    {
        len = 31;
    }
    CMapFilePiY pifile;
    CMapFilePaY pafile;
    if (!pifile.Open(m_searchInfo.nMapID) || !pafile.Open(m_searchInfo.nMapID))
    {
        return nuFALSE;
    }
    nuINT cmp = 0;
    nuBOOL bFind = nuFALSE;
    nuUINT totalPoi = pifile.GetPoiNum();
    if (!totalPoi)
    {
        return nuFALSE;
    }
	SEARCH_PI_POIINFO piInfo = {0};
	SEARCH_PA_INFO paInfo = {0};
    nuUINT start = 0;
    nuUINT end = totalPoi-1;
    nuUINT center = 0;
    while (start<=end)
    {
        center = (start+end)>>1;
        if (pifile.GetPoiInfo_PPIdx(center, &piInfo) 
            && pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
        {
            cmp = nuWcsncmp(pPhone, paInfo.phone, len);
            if (!cmp)
            {
                bFind = nuTRUE;
                goto ENDTAG;
            }
            else if (cmp>0)
            {
                start = center+1;
            }
            else
            {
                end = center-1;
            }
        }
        else
        {
            break;
        }
    }
    
ENDTAG:
    if (!bFind)
    {
        return nuFALSE;
    }
    CMapFilePnY pnfile;
    if (!pnfile.Open(m_searchInfo.nMapID))
    {
        return nuFALSE;
    }
    if (!pnfile.GetName(piInfo.nNameAddr, pInfo->name, EACH_POI_NAME_LEN))
    {
        return nuFALSE;
    }
    pInfo->nMapID = m_searchInfo.nMapID;
    pInfo->nCityID = piInfo.nCityID;
    pInfo->nTownID = piInfo.nTownID;
    pInfo->nTP1ID = piInfo.nTP1ID;
    pInfo->nTP2ID = piInfo.nTP2ID;
    pInfo->nX = piInfo.nX;
    pInfo->nY = piInfo.nY;
    nuMemcpy(&pInfo->moreinfo, &paInfo, sizeof(paInfo));
    return nuTRUE;
}

nuVOID CSearchPoi::SetKeyNameFuzzy(const nuBOOL c_bFuzzy)
{
	m_bKeyNameFuzzy = c_bFuzzy;
}

nuINT	CSearchPoi::GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{


    if (m_nSearchPhone[0])
    {
        return GetPoiList_PHONE(type, nMax, pBuffer, nMaxLen);
    }
    if (m_searchInfo.nMapID==(nuLONG)-1)
    {
        ClearSearchInfo();
        return 0;
    }
	
    nuUINT realTag = 0;
    nuUINT realMax = 0;	
    nuUINT realCount = 0;
    nuUINT nEach = nMax;
	//nuMemset(m_nDY_count, 0, sizeof(m_nDY_count));
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    NURO_SKT skt = NURO_SKT_NAME;
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
        nuWORD nIdx = m_nSearchTagPos - m_nLastGetCount + m_nStrokeLimIdx;
        if (!nIdx || !m_nLastGetCount)
        {
            realTag = 0;
        }
        else
        {
            nuBOOL s1 = nuFALSE;
            nuWORD i = nIdx-1;
            while (i)
            {
                if (m_SearchRes[i].strokes<m_SearchRes[nIdx].strokes)
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
        if (m_nSearchTagPos)
        {
            nuWORD nIdx = m_nSearchTagPos - m_nLastGetCount + m_nStrokeLimIdx;
            realMax = nMax;
            
            nuWORD i = nIdx+1;
            while (i<m_nResTotal)
            {
                if (m_SearchRes[i].strokes>m_SearchRes[nIdx].strokes)
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
        if (m_bSA)
        {
            if (!LoadSearchInfo_AREA())
            {
                return 0;
            }
        }
        else
        {
            if (m_bSA != m_bSAT
                || m_searchInfo.nMapID != m_SearchBufferInfo.nMapID
                || m_searchInfo.nCityID	!= m_SearchBufferInfo.nCityID )
            {
                ClearSearchInfo();
            }
            else if (m_nCarFac != m_nCarFacT
                || m_nSPType != m_nSPTypeT
                || m_searchInfo.nTownID != m_SearchBufferInfo.nTownID
                || m_searchInfo.nTP1ID	!= m_SearchBufferInfo.nTP1ID
                || m_searchInfo.nTP2ID	!= m_SearchBufferInfo.nTP2ID
                || nuWcscmp(m_searchInfo.name, m_SearchBufferInfo.name)
                || nuWcscmp(m_nSearchPoiKeyDanyin, m_nSearchPoiKeyDanyinT)
                || nuWcscmp(m_nSearchPhone, m_nSearchPhoneT)
				|| nuWcscmp(m_nSearchPoiKeyName, m_nSearchPoiKeyNameT)) //prosper 06
            {
                ClearSearchInfo();
                //ResetPoiSearch(); //竘賸陔腔刲坰源宒GetPoiList_PHONE 樟創load隙懂腔訧蹋褫夔竘帤眭渣昫
            }
            m_bSAT = m_bSA;
            m_nCarFacT = m_nCarFac;
            m_nSPTypeT = m_nSPType;
            nuMemcpy(&m_SearchBufferInfo, &m_searchInfo, sizeof(m_SearchBufferInfo));
            nuMemcpy(m_nSearchPoiKeyDanyinT, m_nSearchPoiKeyDanyin, sizeof(m_nSearchPoiKeyDanyinT));
            nuMemcpy(m_nSearchPhoneT, m_nSearchPhone, sizeof(m_nSearchPhone));
			nuMemcpy(m_nSearchPoiKeyNameT, m_nSearchPoiKeyName, sizeof(m_nSearchPhone));//prosper 06
	
            if (m_nCityIDMax==(nuWORD)-1)
            {
                if (m_searchInfo.nCityID==(nuWORD)-1)
                {
                    m_nCityIDTag = 0;
                    m_nCityIDMax = m_nCityTotal;
                }
                else
                {
                    m_nCityIDTag = m_searchInfo.nCityID;
                    m_nCityIDMax = m_searchInfo.nCityID;
                }
            }
        }
        if (type==NURO_SEARCH_ALL)
        {
            m_nSearchTagPos = 0;
            m_nLastGetCount = 0;
            if ((m_nResTotal!=(nuUINT)-1))
            {
                return m_nResTotal;
            }
            else
            {
                realMax = NURO_SEARCH_BUFFER_MAX_POI;
            }
        }
        else
        {
            if (!TranslateSS(type, nMax, m_nSearchTagPos, m_nLastGetCount, &realTag, &realMax))
            {
                return 0;
            }
        }
        if (m_bSA)
        {
            skt = NURO_SKT_AREA;
        }
        else if (m_SearchBufferInfo.nTP1ID != (nuBYTE)-1)
        {
            skt = NURO_SKT_TYPE;
        }
        else if ( m_nSearchPoiKeyDanyin[0] || m_SearchBufferInfo.name[0])
        {
            skt = NURO_SKT_NAME;
        }
        else if (m_nSearchPhone[0])
        {
            skt = NURO_SKT_PHONE;
        }
        else if (m_nCarFac != (nuUINT)-1)
        {
            skt = NURO_SKT_CARFAC;
        }
        else if (m_nSPType != (nuUINT)-1)
        {
            skt = NURO_SKT_SPI;
        }
        else
        {
            skt = NURO_SKT_NAME;
        }
    }
    NURO_SEARCH_NORMALRES* pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
    if (type!=NURO_SEARCH_ALL && !pSearchRes)
    {
        return 0;
    }
    CMapFilePnY pnfile;
    if (!pnfile.Open(m_SearchBufferInfo.nMapID))
    {
        return 0;
    }

    nuUINT _bufferLim = NURO_SEARCH_BUFFER_MAX_POI;
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
        if (m_SearchRes[realTag].type!=SEARCH_BT_POI)
        {
            SearchNextPoi(skt, realTag);
			if(skt==NURO_SKT_AREA || skt==NURO_SKT_NAME)
			{
				_bufferLim=m_nResTotal;
			}
            if (m_SearchRes[realTag].type!=SEARCH_BT_POI)
            {
                break;
            }
        }
        if (!m_SearchRes[realTag].name[0])
        {
            if (!GetPoiName(realTag, (nuBYTE*)(m_SearchRes[realTag].name), EACH_POI_NAME_LEN, pnfile))
            {
                break;
            }
        }

		if (type != NURO_SEARCH_ALL && (m_nSearchPoiKeyDanyin[0])&&nFirstSort<0 && m_bSort)
        {
			m_bSort = nuFALSE;
			if(nFirstSort == (nuINT)-1)
			{
				if( m_nDY_countIdx != (nuWORD)-1 )
				{
					m_pMmApi->MM_RelDataMemMass(&m_nDY_countIdx);
				}
				if( m_nsort_countsIdx != (nuWORD)-1 )
				{
					m_pMmApi->MM_RelDataMemMass(&m_nsort_countsIdx);
				}
				nuUINT *m_nDY_count = (nuUINT*)m_pMmApi->MM_GetDataMemMass(m_nResTotal*sizeof(nuUINT),&m_nDY_countIdx);
				nuUINT *m_nsort_counts = (nuUINT*)m_pMmApi->MM_GetDataMemMass(m_nResTotal*sizeof(nuUINT),&m_nsort_countsIdx);
				for(nuUINT nIdx = 0; nIdx < m_nResTotal; nIdx++)
				{
					 m_nDY_count[nIdx] = (nuUINT)m_SearchRes[nIdx].nFindCount+1;    //比較用ARRAY
					 m_nsort_counts[nIdx] = (nuUINT)m_SearchRes[nIdx].nFindCount+1; //排序好ARRAY
				}
				nuINT nChangtemp = 0; //BUBBLE
				
				for(nuUINT nSortIdx = 0; nSortIdx < m_nResTotal; nSortIdx++)
				{
					for(nuUINT i = nSortIdx; i < m_nResTotal ; i++)
					{
						if(m_nsort_counts[nSortIdx]<m_nsort_counts[i])
						{
							nChangtemp = m_nsort_counts[nSortIdx];
							m_nsort_counts[nSortIdx] = m_nsort_counts[i];
							m_nsort_counts[i] = nChangtemp;
						} 
					}
				
				}
					nFirstSort++;
					
				for(nuUINT nNameIdx = 0; nNameIdx < m_nResTotal; nNameIdx++)
				{
      				for( nuUINT i = 0; i < realMax; i++)
					{
						if(m_nDY_count[i]==m_nsort_counts[nFirstSort])
						{ //如果為MAX的表名稱丟入
							nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
							nuMemcpy(pSearchRes->name, m_SearchRes[i].name, EACH_BUFFER_NAME_LEN-2);
							pSearchRes->strokes = m_SearchRes[i].strokes;
							++pSearchRes;
							nFirstSort++;
							
					
							
							realTag++;      //不確定會不會影響 暫時加入
							realCount++;
							m_nDY_count[i] = 0;
							break; //進入下個判斷
						}
					}
				}
				if( m_nDY_countIdx != (nuWORD)-1 )
				{
					m_pMmApi->MM_RelDataMemMass(&m_nDY_countIdx);
				}
				if( m_nsort_countsIdx != (nuWORD)-1 )
				{
					m_pMmApi->MM_RelDataMemMass(&m_nsort_countsIdx);
				}
			}
			break;
		}		
        if (type != NURO_SEARCH_ALL)
        {
            nuMemset(pSearchRes, 0, sizeof(NURO_SEARCH_NORMALRES));
            nuMemcpy(pSearchRes->name, m_SearchRes[realTag].name, EACH_BUFFER_NAME_LEN-2);
            pSearchRes->strokes = m_SearchRes[realTag].strokes;
            ++pSearchRes;
        }
        ++realTag;
        ++realCount;
    }
    if (type == NURO_SEARCH_ALL)
    {
        m_nResTotal = realTag;

		nuINT queueType = 2;

		if (NURO_SKT_NAME == skt)
		{
			if (nuTRUE == m_bComCompare)
			{
				//queueType = 5;
				queueType = 8;
			}
			else
			{
				queueType = 7;
			}
		}
		else if (NURO_SKT_AREA == skt)
		{
			queueType = 6;
		}

        if (m_nResTotal)
        {
			if(m_nResTotal<NURO_SEARCH_BUFFER_MAX_POI)
			{
				QueueBUffer(m_SearchRes, m_nResTotal, queueType);
				if(m_bNameSort)
				{
					SortByStroke(m_nResTotal); //ADD BY PROSPER 
				}
			}
        }
		if(m_bSetAreaCenterPos && m_nResTotal>0 &&  skt == NURO_SKT_AREA)
		{
			for(int nIdx2=0;nIdx2<m_nResTotal-1;nIdx2++)
				for(int nIdx=0;nIdx<m_nResTotal-1;nIdx++)
				{
					SEARCH_BUFFER tmp_SearchRes= {0};
					if(m_SearchRes[nIdx].nDistance > m_SearchRes[nIdx+1].nDistance)  
					{
							tmp_SearchRes		=	m_SearchRes[nIdx];
							m_SearchRes[nIdx]	=	m_SearchRes[nIdx+1];
							m_SearchRes[nIdx+1]  =   tmp_SearchRes;
					}
				}
			//m_bSetAreaCenterPos =false;
		}
        return m_nResTotal;
    }
    if (!realCount)
    {
        return 0;
    }
    m_nSearchTagPos = realTag;
    m_nLastGetCount = realCount;
    if (NURO_SEARCH_NEXT_SC==type
        || NURO_SEARCH_PRE_SC==type)
    {
        nuUINT nRes = m_nLastGetCount;
        nuUINT tmp = (m_nSearchTagPos-m_nLastGetCount)/nEach;
        nRes |= (tmp<<16);
        return nRes;
    }
    return m_nLastGetCount;
}

nuINT CSearchPoi::GetPoiList_PHONE(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen)
{
    if (m_searchInfo.nMapID==(nuLONG)-1)
    {
        ClearSearchInfo();
        return 0;
    }
    nuUINT realTag = 0;
    nuUINT realMax = 0;
    nuUINT realCount = 0;
    nuUINT nEach = nMax;
    nMax = NURO_MIN(nMax, nMaxLen/sizeof(NURO_SEARCH_NORMALRES));
    if (m_bSA != m_bSAT
        || m_searchInfo.nMapID != m_SearchBufferInfo.nMapID
        || m_searchInfo.nCityID	!= m_SearchBufferInfo.nCityID
        || m_nCarFac != m_nCarFacT
        || m_nSPType != m_nSPTypeT)
    {
        ClearSearchInfo();
    }
    else if (m_searchInfo.nTownID != m_SearchBufferInfo.nTownID
        || m_searchInfo.nTP1ID	!= m_SearchBufferInfo.nTP1ID
        || m_searchInfo.nTP2ID	!= m_SearchBufferInfo.nTP2ID
        || nuWcscmp(m_searchInfo.name, m_SearchBufferInfo.name)
        || nuWcscmp(m_nSearchPoiKeyDanyin, m_nSearchPoiKeyDanyinT)
        || nuWcscmp(m_nSearchPhone, m_nSearchPhoneT))
    {
        ClearSearchInfo();
    }
    if (type==NURO_SEARCH_ALL)
    {
        m_nSearchTagPos = 0;
        m_nLastGetCount = 0;
        if (!m_nResTotal || m_nResTotal==(nuUINT)-1)
        {
            m_bSAT = m_bSA;
            m_nCarFacT = m_nCarFac;
            m_nSPTypeT = m_nSPType;
            nuMemcpy(&m_SearchBufferInfo, &m_searchInfo, sizeof(m_SearchBufferInfo));
            nuMemcpy(m_nSearchPoiKeyDanyinT, m_nSearchPoiKeyDanyin, sizeof(m_nSearchPoiKeyDanyinT));
            nuMemcpy(m_nSearchPhoneT, m_nSearchPhone, sizeof(m_nSearchPhone));      
            FindPhoneMatch();
        }
        return m_nResTotal;
    }
    else
    {
        if (!pBuffer)
        {
            return 0;
        }
        nuMemset(pBuffer, 0, nMaxLen);
        if (m_nResTotal==(nuUINT)-1)
        {
            GetPoiTotal();
        }
        if (!m_nResTotal)
        {
            return 0;
        }
        switch(type)
        {
        case NURO_SEARCH_NEW:
            realTag = 0;
            realMax = nMax;
            break;
        case NURO_SEARCH_LAST:
            realMax = m_nResTotal%nEach;
            if (!realMax)
            {
                realMax = nMax;
            }
            realTag = m_nResTotal-realMax;
            break;
        case NURO_SEARCH_PRE:
            if (!m_nSearchTagPos || !m_nLastGetCount || m_nSearchTagPos<=m_nLastGetCount)
            {
                return 0;
            }
            realTag = m_nSearchTagPos - m_nLastGetCount;
            realTag = (realTag<=nMax)?0:(realTag-nMax);
            realMax = nMax;
            break;
        case NURO_SEARCH_NEXT:
            realTag = m_nSearchTagPos;
            realMax = nMax;
            break;
        case NURO_SEARCH_RELOAD:
            if (!m_nSearchTagPos 
                || !m_nLastGetCount 
                || m_nSearchTagPos<m_nLastGetCount 
                || nMax<m_nLastGetCount)
            {
                return 0;
            }
            else
            {
                realTag = m_nSearchTagPos - m_nLastGetCount;
                realMax = m_nLastGetCount;
            }
            break;
        default:
            return 0;
        }
        CMapFilePiY pifile;
        if (!pifile.Open(m_searchInfo.nMapID))
        {
            return 0;
        }
		SEARCH_PI_POIINFO piInfo = {0};
        NURO_SEARCH_NORMALRES* pSearchRes = (NURO_SEARCH_NORMALRES*)pBuffer;
        CMapFilePnY pnfile;
        if (!pnfile.Open(m_SearchBufferInfo.nMapID))
        {
            return 0;
        }        
        nuUINT _bufferLim = NURO_SEARCH_BUFFER_MAX_POI;
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
            if (m_SearchRes[realTag].type!=SEARCH_BT_POI)
            {
                if (!pifile.GetPoiInfo_PPIdx(m_firstPos+realTag, &piInfo))
                {
                    break;
                }
                FillResBuffer(realTag, &piInfo, 0);
                if (!GetPoiName(realTag, (nuBYTE*)(m_SearchRes[realTag].name), EACH_POI_NAME_LEN, pnfile))
                {
                    break;
                }
				
            }
            nuMemcpy(pSearchRes->name, m_SearchRes[realTag].name, EACH_BUFFER_NAME_LEN-2);
            pSearchRes->strokes = m_SearchRes[realTag].strokes;
            ++pSearchRes;
            ++realTag;
            ++realCount;
        }
        if (!realCount)
        {
            return 0;
        }
        m_nSearchTagPos = realTag;
        m_nLastGetCount = realCount;
        return m_nLastGetCount;
    }
}


nuINT CSearchPoi::GetPoiFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen)
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
        GetPoiTotal();
    }
    if (!m_nResTotal)
    {
        return 0;
    }
    nuWORD i = 0;
    CMapFilePnY pnfile;
    if (!pnfile.Open(m_SearchBufferInfo.nMapID))
    {
        return 0;
    }
    nuUINT resLen = 0;
    nuUINT nRes = 0;
    nuUINT nMax = nMaxLen/sizeof(NURO_SEARCH_FILTERRES);
    NURO_SEARCH_FILTERRES *p = (NURO_SEARCH_FILTERRES*)pBuffer;
    for (i=0; i<m_nResTotal&&nRes<nMax; ++i)
    {
        if (!m_SearchRes[i].name[0])
        {
            resLen = pnfile.GetName(m_SearchRes[i].nameAddr, m_SearchRes[i].name, EACH_POI_NAME_LEN);
        }
        else
        {
            resLen = nuWcslen(m_SearchRes[i].name);
        }
        if (!resLen)
        {
            continue;
        }
        if (bCheck)
        {
            if (!nuWcsFuzzyJudge(m_SearchRes[i].name, (nuWORD*)pKey, resLen, keyLen, 0))
            {
                continue;
            }
        }
        p->idx = i;
        p->strokes = m_SearchRes[i].strokes;
        nuMemcpy(p->name, m_SearchRes[i].name, EACH_POI_NAME_LEN);
        ++p;
        ++nRes;
    }
    return nRes;
}
nuBOOL CSearchPoi::LoadSearchInfo_AREA()
{
    if (m_bSA != m_bSAT
        || m_searchInfo.nMapID	!= m_SearchBufferInfo.nMapID)
    {
        ClearSearchInfo();
        nuBYTE* pBuffer = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
        if (!pBuffer || !m_nLB_CTLen)
        {
            return nuFALSE;
        }
        pBuffer+=m_nLB_CTLen;
        if (!m_nBHIdxLen)
        {
            CMapFileBhY bhfile;
            if (!bhfile.Open(m_searchInfo.nMapID))
            {
                ClearSearchInfo();
                return nuFALSE;
            }
            m_nBHIdxLen
                = bhfile.GetBHIdxInfo(pBuffer, m_nLoadBufferMaxLen-m_nLB_CTLen);
            if (!m_nBHIdxLen)
            {
                ClearSearchInfo();
                return nuFALSE;
            }
        }
    }
    else if (m_sAreaRectT.bottom	!= m_sAreaRect.bottom
        || m_sAreaRectT.left	!= m_sAreaRect.left
        || m_sAreaRectT.right	!= m_sAreaRect.right
        || m_sAreaRectT.top		!= m_sAreaRect.top
        || m_searchInfo.nTP1ID != m_SearchBufferInfo.nTP1ID
        || m_searchInfo.nTP2ID != m_SearchBufferInfo.nTP2ID
        || nuWcscmp(m_searchInfo.name, m_SearchBufferInfo.name))
    {
        ResetPoiSearch();
    }
    m_bSAT = m_bSA;
    nuMemcpy(&m_SearchBufferInfo, &m_searchInfo, sizeof(m_SearchBufferInfo));
    nuMemcpy(&m_sAreaRectT, &m_sAreaRect, sizeof(m_sAreaRect));
    return nuTRUE;
}
nuBOOL CSearchPoi::LoadSearchInfo_CITY(nuWORD cityID, nuBOOL bLoadPN)
{
    if (cityID	!= m_nCityIDLoad)
    {
        nuBYTE* pBuffer = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
        if (!pBuffer || !m_nLB_CTLen)
        {
            return nuFALSE;
        }
        pBuffer+=m_nLB_CTLen;
        CMapFilePiY pifile;
        if (!pifile.Open(m_searchInfo.nMapID))
        {
            return nuFALSE;
        }
        m_nLB_PILen = pifile.GetPoiInfo_CITY(cityID, pBuffer, m_nLoadBufferMaxLen-m_nLB_CTLen);
        if (!m_nLB_PILen)
        {
            return nuFALSE;
        }
        m_nLoadPoiCount = m_nLB_PILen / sizeof(SEARCH_PI_POIINFO);
        m_nLB_PILen2 = m_nLB_PILen;
		m_nLoadPoiCount2 = m_nLoadPoiCount;
		m_nLB_PNLen = 0;
        if (bLoadPN)
        {
            pBuffer+=m_nLB_PILen;		
            CMapFilePnY pnfile;
            if (!pnfile.Open(m_searchInfo.nMapID))
            {
                return nuFALSE;
            }
            m_nLB_PNLen = pnfile.GetPoiInfo_CITY(cityID
                , pBuffer, m_nLoadBufferMaxLen-m_nLB_CTLen-m_nLB_PILen, &m_nLoadPnBaseAddr);
			m_nLB_PNLen2 = m_nLB_PNLen;
			m_nLoadPnBaseAddr2 = m_nLoadPnBaseAddr;
            if (!m_nLB_PNLen)
            {
                return nuFALSE;
            }
        }
    }
    m_nCityIDLoad = cityID;
    return nuTRUE;
}
/*
nuBOOL CSearchPoi::LoadSearchInfo_PHONE(nuWORD cityID)
{
    if (cityID==(nuWORD)-1)
    {
        return nuFALSE;
    }
    nuBYTE* pBuffer = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pBuffer || !m_nLB_CTLen)
    {
        return nuFALSE;
    }
    pBuffer += m_nLB_CTLen + m_nLB_PNLen;
    if (m_SearchBufferInfo.nMapID!=m_nLB_PAMapID || !m_nLB_PALen)
    {
        m_nLB_PALen = 0;
        m_nCityIDLoad = -1;
        CMapFilePaY pafile;
        if (!pafile.Open(m_SearchBufferInfo.nMapID))
        {
            return nuFALSE;
        }
        m_nLB_PALen = pafile.GetLength();
        if ((m_nLoadBufferMaxLen-m_nLB_CTLen-m_nLB_PNLen)<m_nLB_PALen)
        {
            m_nLB_PALen = 0;
        }
        if (!pafile.ReadData(0, pBuffer, m_nLB_PALen))
        {
            m_nLB_PALen = 0;
        }
        m_nLB_PAMapID = m_SearchBufferInfo.nMapID;
        m_nLB_PALen = ((m_nLB_PALen+3)>>2)<<2;
    }
    pBuffer += m_nLB_PALen;
    if (cityID!=m_nCityIDLoad)
    {
        m_nLoadPoiCount = 0;
        m_nCityIDLoad = -1;
        CMapFilePiY pifile;
        if (!pifile.Open(m_searchInfo.nMapID))
        {
            return nuFALSE;
        }
        m_nLB_PILen = pifile.GetPoiInfo_CITY(cityID
            , pBuffer, m_nLoadBufferMaxLen-m_nLB_CTLen-m_nLB_PNLen-m_nLB_PALen);
        m_nLoadPoiCount = m_nLB_PILen / sizeof(SEARCH_PI_POIINFO);
        m_nCityIDLoad = cityID;
        m_nLB_PILen = ((m_nLB_PILen+3)>>2)<<2;
    }
    return nuTRUE;
}
*/

nuVOID CSearchPoi::ClearSearchInfo()
{
	//Prosper 20111207 
#ifndef VALUE_EMGRT 
	nKeyCount=0;
	nuMemset(KEY_Data,0,sizeof(KEY_Data));
#endif
	//Prosper add 2014.07
    //m_nLoadPoiCount		= 0;
    m_nLoadPnBaseAddr	= 0;
    m_nLB_PILen = 0;
    m_nLB_PNLen = 0;
    //m_nLB_PI_B_Len = 0;
    //m_nLB_PI_B_Total = 0;
    m_nLB_PAMapID = -1;
    //m_nLB_PI_B_MapID = -1;
    //m_nLB_PALen = 0;
    m_nBHIdxLen = 0;
    //Prosper add 2014.07
    //m_nCityIDLoad = -1;
    ResetPoiSearch();
}

nuVOID CSearchPoi::ResetPoiSearch()
{
    m_nCarFacT    = -1;
    m_nSPTypeT    = -1;
    m_SearchBufferInfo.nMapID = -1;
    m_SearchBufferInfo.nCityID = -1;
    m_SearchBufferInfo.nTownID = -1;
    m_SearchBufferInfo.nTP1ID = -1;
    m_SearchBufferInfo.nTP2ID = -1;
    m_bSAT = nuFALSE;
    m_nResTotal = -1;
    m_nCityIDTag = -1;
    m_nCityIDMax = -1;
    m_nSearchTagPos = 0;
    m_nLastGetCount = 0;
    nuMemset(&m_sAreaRectT, 0, sizeof(m_sAreaRectT));
    nuMemset(m_SearchRes, 0, sizeof(m_SearchRes));	
    nuMemset(m_SearchBufferInfo.name, 0, sizeof(m_SearchBufferInfo.name));	
    nuMemset(m_nSearchPoiKeyDanyinT, 0, sizeof(m_nSearchPoiKeyDanyinT));
    nuMemset(m_nSearchPhoneT, 0, sizeof(m_nSearchPhoneT));
    nuMemset(m_nSearchPoiKeyNameT, 0, sizeof(m_nSearchPoiKeyNameT));
	
}

nuBOOL CSearchPoi::SetPoiKeyDanyin( nuWCHAR *pDanyin, nuBOOL bComplete )
{	
	m_bIndexSort = nuFALSE; //Prosper add ,2013.10.30
	m_bSort = nuTRUE;
    nuMemset(m_nSearchPoiKeyDanyin , 0, sizeof(m_nSearchPoiKeyDanyin));
    if (!pDanyin || !pDanyin[0])
    {
        return nuFALSE;
    }
    m_bSA = nuFALSE;

    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    nuMemset(m_nSearchPhone, 0, sizeof(m_nSearchPhone));
    SetPoiCarFac(-1);
    SetPoiSPType(-1);
#ifndef _USE_PANA_MODE_
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
#endif
    nuWcsncpy(m_nSearchPoiKeyDanyin, pDanyin, (EACH_POI_NAME_LEN>>1)-1);
	m_bComCompare = bComplete;
    return nuTRUE;
}
nuBOOL CSearchPoi::SetPoiKeyDanyin2( nuWCHAR *pDanyin, nuBOOL bComplete )
{	
	if(pDanyin==0)     //Prosper add for load before input
	{
		Load();
		return nuTRUE;
	}
	m_bSort = nuFALSE;
	nuMemset(m_nSearchPoiKeyDanyin , 0, sizeof(m_nSearchPoiKeyDanyin));
	if (!pDanyin || !pDanyin[0])
	{
		return nuFALSE;
	}
	m_bSA = nuFALSE;

	nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
	nuMemset(m_nSearchPhone, 0, sizeof(m_nSearchPhone));
	SetPoiCarFac(-1);
	SetPoiSPType(-1);
#ifndef _USE_PANA_MODE_
	m_searchInfo.nTP1ID = -1;
	m_searchInfo.nTP2ID = -1;
#endif
	nuWcsncpy(m_nSearchPoiKeyDanyin, pDanyin, (EACH_POI_NAME_LEN>>1)-1);
	m_bComCompare = bComplete;
	return nuTRUE;
}
nuBOOL CSearchPoi::SetPoiKeyPhone(nuWCHAR *pPhone)
{
	m_bComCompare = nuFALSE;

    nuMemset(m_nSearchPhone , 0, sizeof(m_nSearchPhone));
    if (!pPhone || !pPhone[0])
    {
        return nuFALSE;
    }
    m_bSA = nuFALSE;
	
    nuMemset(m_searchInfo.name, 0, sizeof(m_searchInfo.name));
    nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    SetPoiCarFac(-1);
    SetPoiSPType(-1);
#ifdef _USE_PANA_MODE_
    if (pPhone)
    {
        m_searchInfo.nTP1ID = -1;
        m_searchInfo.nTP2ID = -1;
    }
#else
    m_searchInfo.nTP1ID = -1;
    m_searchInfo.nTP2ID = -1;
#endif
    nuWcsncpy(m_nSearchPhone, pPhone, 31);
    return nuTRUE;
}
nuBOOL CSearchPoi::SetSearchArea(NURORECT *pRect)
{
	m_bComCompare = nuFALSE;

    if (!pRect)
    {
        m_bSA = nuFALSE;
        return nuFALSE;
    }
    m_SearchBufferInfo.nCityID = -1;
    m_SearchBufferInfo.nTownID = -1;
    nuMemcpy(&m_sAreaRect, pRect, sizeof(m_sAreaRect));
    nuLONG temp = 0;
    if (m_sAreaRect.left > m_sAreaRect.right)
    {
        temp = m_sAreaRect.left;
        m_sAreaRect.left = m_sAreaRect.right;
        m_sAreaRect.right = temp;
    }
    if (m_sAreaRect.top > m_sAreaRect.bottom)
    {
        temp = m_sAreaRect.top;
        m_sAreaRect.top = m_sAreaRect.bottom;
        m_sAreaRect.bottom = temp;
    }
    
    nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    m_bSA = nuTRUE;

    SetPoiCarFac(-1);
    SetPoiSPType(-1);
    return nuTRUE;
}

nuUINT CSearchPoi::GetTP1Total()
{
	CMapFilePtnY ptnfile;
    if (!ptnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    return ptnfile.GetTP1Num();
}

nuUINT CSearchPoi::GetPoiTotal()
{
    //m_bcheck=m_bcheck^1;
    nFirstSort = -1;  
	return GetPoiNameList(NURO_SEARCH_ALL, -1, NULL, -1);//,20843

}

nuBOOL CSearchPoi::GetPoiName(nuUINT idx, nuBYTE* pNameBuffer, nuUINT nMaxLen, CMapFilePnY &pnfile)
{
    if (m_nLB_PNLen && m_SearchRes[idx].cityID==m_nCityIDLoad )
    {
        nuBYTE *pNameInfo = GetPoiNameInfo(m_SearchRes[idx].nameAddr);
        if (!pNameInfo)
        {
            return nuFALSE;
        }
        nuBYTE nLen = *pNameInfo;
        if (nLen>(EACH_POI_NAME_LEN-2))
        {
            nLen = EACH_POI_NAME_LEN-2;
        }
        nuMemcpy(pNameBuffer, pNameInfo+1, nLen);


        ((nuWORD*)pNameBuffer)[nLen>>1] = 0;
        nuUINT i = pNameInfo[pNameInfo[0]+1];
        m_SearchRes[idx].strokes = pNameInfo[pNameInfo[0]+i+2];
    
	}
    else
    {
        nuUINT namelen = pnfile.GetName(m_SearchRes[idx].nameAddr
            , pNameBuffer
            , nMaxLen
            , &m_SearchRes[idx].strokes);
        return namelen ? nuTRUE:nuFALSE;
    }
    return nuTRUE;
}
nuBYTE *CSearchPoi::GetPoiNameInfo(nuUINT addr)
{
    nuBYTE* pPoiInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPoiInfo)
    {
        return NULL;
    }
    nuBYTE* pPoiNameInfo = NULL;
    #ifdef VALUE_EMGRT
    if (m_nSearchPhone[0])
    {
        pPoiNameInfo = pPoiInfo+m_nLB_CTLen;
    }
    else
    {
    	
        pPoiNameInfo = pPoiInfo+m_nLB_CTLen+m_nLB_PILen2;
    }
    if (addr<m_nLoadPnBaseAddr2 || addr>=m_nLoadPnBaseAddr2+m_nLB_PNLen2)
    {
        return NULL;
    }
    return pPoiNameInfo+addr-m_nLoadPnBaseAddr2;
    #else
    if (m_nSearchPhone[0])
    {
        pPoiNameInfo = pPoiInfo+m_nLB_CTLen;
    }
    else
    {
        pPoiNameInfo = pPoiInfo+m_nLB_CTLen+m_nLB_PILen;
    }
    if (addr<m_nLoadPnBaseAddr || addr>=m_nLoadPnBaseAddr+m_nLB_PNLen)
    {
        return NULL;
    }
    return pPoiNameInfo+addr-m_nLoadPnBaseAddr;
    #endif
}
nuUINT CSearchPoi::SearchNextPoi(NURO_SKT skt, nuUINT tag)
{
    switch (skt)
    {
    case NURO_SKT_TYPE:
		
        return SearchNextPoi_TYPE(tag);
       
    case NURO_SKT_NAME:
		{
#ifndef VALUE_EMGRTX //2013_0613_ prosper add
		return SearchNextPoi_NAME(tag);
#else	
		//2013_0613_prosper add , Load Data Before  input
        return SearchNextPoi_NAME_CN(tag);
#endif
		}
		//return SearchNextPoi_NAME(tag);
    //case NURO_SKT_PHONE:
    //    return SearchNextPoi_PHONE(tag);
    case NURO_SKT_AREA:
        return SearchNextPoi_AREA();
    case NURO_SKT_CARFAC:
        return SearchNextPoi_Car();
    case NURO_SKT_SPI:
        return SearchNextPoi_SPI();
    default:
        return 0;
    }
    return 0;
}
nuBOOL nuWcsFuzzyJudge_O_FORSEARCHPOI(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen
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
nuBOOL nuWcsFuzzyJudge_FORSEARCHPOI(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen
						, nuWORD endWord, nuINT& startpos, nuINT& diffpos)
{
	if (!endWord)
	{
		return nuWcsFuzzyJudge_O_FORSEARCHPOI(pDes, pKey, desLen, keyLen, startpos, diffpos);
	}
	if (desLen<keyLen)
	{
		return nuFALSE;
	}
	nuUINT i = 0;
	startpos = -1;
	diffpos = 0;
    for(i=0; i<desLen; ++i)
	{
		if (pDes[i]==endWord)
		{
			return nuFALSE;
		}
        if(nuWcsSameAs(&pDes[i], &pKey[0]))
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

nuUINT CSearchPoi::SearchNextPoi_NAME(nuUINT tag)
{
    nuUINT res = 0;nuINT z=0;  SEARCH_BUFFER m_SearchRes2;
    nuUINT nIdx = 0;
    nuUINT len = 0;
    nuWORD nameLen = 0;
    nuUINT keyLen = nuWcslen(m_SearchBufferInfo.name);
    nuUINT keyLen2 = nuWcslen(m_nSearchPoiKeyDanyinT);
    nuBYTE *pNameInfo = NULL;
    nuWORD *pName = NULL;
	nuWCHAR  wTempPOIDY[EACH_POI_NAME_LEN] = {0};   // Added by Damon 20100122
	nuWCHAR  wTempPOIName[EACH_POI_NAME_LEN] = {0}; // Added by Damon 20100122
	nuINT  nDanyinIdx = 0; //梓暮等秞植撓跺弇离眈肮 added by ouyang 20100118
	nuINT  nNameDiffPos = 0;
    nuINT j=-1;
	SEARCH_BUFFER ZHUY_IN_DATA = {0};

	
	nuINT* m_nWordStartIdx = (nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nWordStartIdxMemIdx);
	nuINT* m_nWordLens = (nuINT*)m_pMmApi->MM_GetDataMassAddr(m_nWordLensMemIdx);

	SEARCH_PI_POIINFO* pPoiInfo = (SEARCH_PI_POIINFO*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPoiInfo)
    {
        return res;
    }
    pPoiInfo = (SEARCH_PI_POIINFO*) ((nuBYTE*)pPoiInfo+m_nLB_CTLen);	
	nuBOOL bres = nuFALSE;
	nuINT count=0;nuINT x=0;
	
	nNEXT_WORD_COUNT = 0;
	nuMemset(m_wsNEXT_WORD_Data,0,sizeof(m_wsNEXT_WORD_Data));
							

	nuINT nMAX = 4;
	#ifndef VALUE_EMGRT
		nMAX = NURO_SEARCH_BUFFER_MAX_POI ;
	#endif
	if(m_bSort)
	{
		nMAX = NURO_SEARCH_BUFFER_MAX_POI ;
	}
	nuWCHAR  wTempPOIName2[EACH_POI_NAME_LEN] = {0};

	
    while (m_nCityIDTag<=m_nCityIDMax)// && tag<NURO_SEARCH_BUFFER_MAX_POI)
    {
        if (LoadSearchInfo_CITY(m_nCityIDTag))
        {
            nIdx = 0;
            while (nIdx<m_nLoadPoiCount)// && tag<NURO_SEARCH_BUFFER_MAX_POI)
            {
            	#ifdef VALUE_EMGRT
            	if(!m_bSort)
				{
					if(res==4)
						break;
				}
				#endif
                if (m_SearchBufferInfo.nCityID!=(nuWORD)-1
                    && m_SearchBufferInfo.nTownID!=(nuWORD)-1
                    && pPoiInfo[nIdx].nTownID!=m_SearchBufferInfo.nTownID)
                {
                    ++nIdx;
                    continue;
                }
                pNameInfo = GetPoiNameInfo(pPoiInfo[nIdx].nNameAddr);
                if (pNameInfo && *pNameInfo)
                {
					if(!m_nSearchPoiKeyDanyin[0])
					{
						if(tag >= nMAX) 
						{
							break;
						}
					}
                    if (m_nSearchPoiKeyDanyin[0])
                    {
						
                        pName = (nuWORD*)(pNameInfo+2+*pNameInfo);
                        nameLen = (*(pNameInfo+1+*pNameInfo));
						
						
						if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }
						if(tag<nMAX)
						{
							nuMemcpy(m_SearchRes[tag].name, pName, nameLen);
							m_SearchRes[tag].name[nameLen>>1] = 0;
						}
						else
						{
							nuMemcpy(ZHUY_IN_DATA.name, pName, nameLen);
							ZHUY_IN_DATA.name[nameLen>>1] = 0;
						}
						nuWCHAR wsDanyin[EACH_POI_NAME_LEN]={0};
						nuWcscpy(wsDanyin,m_SearchRes[tag].name);
						nuINT Danyin_p=0;
						nuINT nDanyinLen = (nameLen/2);
						for(nuINT Danyin_i=0;Danyin_i<nDanyinLen;Danyin_i++)
						{
							if(wsDanyin[Danyin_i]>47&& wsDanyin[Danyin_i]<58 
								|| wsDanyin[Danyin_i]>64 && wsDanyin[Danyin_i]<91
								|| wsDanyin[Danyin_i]>96 && wsDanyin[Danyin_i]<123
								|| wsDanyin[Danyin_i]>=8544 && wsDanyin[Danyin_i]<=8565 || wsDanyin[Danyin_i]==12295
								|| wsDanyin[Danyin_i]>=0x4E00 /*&& wsDanyin[Danyin_i]<=0x9FA5*/ )
							{
								if(wTempPOIName2[Danyin_i]==12398)
											continue;
								wsDanyin[Danyin_p++] = wsDanyin[Danyin_i];
							}
							else
							{
								continue;						
							}
						}
						wsDanyin[Danyin_p]=0;
						nameLen=Danyin_p;
						nuWcscpy(m_SearchRes[tag].name,wsDanyin);
						m_SearchRes[tag].name[Danyin_p]=0;
						// Added by Damon 20100122
						if (nuTRUE == m_bComCompare)
						{
                          
							if(tag<nMAX)
							{
								nuMemset(wTempPOIDY, NULL, sizeof(wTempPOIDY));
								nuWcsDelSpSign(wTempPOIDY, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen);
								nDanyinIdx = nuWcsKeyMatch(wTempPOIDY, m_nSearchPoiKeyDanyinT, nuWcslen(wTempPOIDY), keyLen2);
							}
							else
							{
								#ifndef VALUE_EMGRT 
								nuMemset(wTempPOIDY, NULL, sizeof(wTempPOIDY));
								nuWcsDelSpSign(wTempPOIDY, ZHUY_IN_DATA.name, EACH_POI_NAME_LEN, nameLen);
								#endif
								nDanyinIdx = nuWcsKeyMatch(wTempPOIDY, m_nSearchPoiKeyDanyinT, nuWcslen(wTempPOIDY), keyLen2);
								//ZHUY_IN_DATA
							}
						}
						else
						{
                            if(tag<nMAX) 
							{
								nDanyinIdx = nuWcsKeyMatch(m_SearchRes[tag].name, m_nSearchPoiKeyDanyinT, nameLen, keyLen2);
							}
							else
							{	
								#ifndef VALUE_EMGRT 
								nDanyinIdx = nuWcsKeyMatch(ZHUY_IN_DATA.name, m_nSearchPoiKeyDanyinT, nameLen, keyLen2);
								#endif
							}
						}
						// -----------------------

						nameLen = *(pNameInfo);

                        if ( nDanyinIdx != -1 )
                        {//植菴撓跺趼睫羲宎眈肮 added by ouyang 20100118
							
							//Prosper 2011.12.07
							#ifndef VALUE_EMGRT 
							nuINT n= nDanyinIdx+keyLen2;

							if(n<nameLen/2)
							{
								if(tag >= nMAX) 
								{
									bool bRes=Sort_KEY_index(ZHUY_IN_DATA.name[n]);
									//if(bRes==TRUE)

									//++nIdx;
									//continue;
								}
								else
								{
									Sort_KEY_index(m_SearchRes[tag].name[n]);
								}
							}
							#endif
							
							if(tag >= nMAX) 
							{
								++nIdx;
								continue;
							}

							if (nuTRUE == m_bComCompare)
							{  // Modified by Damon 20100122	
								nuMemset(wTempPOIName, NULL, sizeof(wTempPOIName));
								#ifdef VALUE_EMGRT
								nuWCHAR  wTempPOIName2[EACH_POI_NAME_LEN] = {0};
								nuMemcpy(wTempPOIName2, pNameInfo+1, nameLen);
								int p=0;
								for(nuINT i=0;i<nameLen;i++)
								{
									if(wTempPOIName2[i]!=32)
									{
										m_SearchRes[tag].name[p++] = wTempPOIName2[i];
									}
								}
								/*for(nuINT i=0;i<nameLen;i++)
								{
									if(wTempPOIName2[i]>47&& wTempPOIName2[i]<58 
										|| wTempPOIName2[i]>64 && wTempPOIName2[i]<91
										|| wTempPOIName2[i]>96 && wTempPOIName2[i]<123
										|| wTempPOIName2[i]>12398 )
									{
										m_SearchRes[tag].name[p++] = wTempPOIName2[i];
									}
								}*/

								#else
								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								#endif
								nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen);
						
								#ifdef VALUE_EMGRT
								nuINT nFindWord= 0;
								for(nuINT j=0,k=0;j<keyLen2;k++)
								{
									if(wTempPOIName[nDanyinIdx+k]==0)
										break;
									else
									{
										m_SearchRes[tag].name[j++]=wTempPOIName[nDanyinIdx+k];
										nFindWord++;
									}
								}
								if(nFindWord!=keyLen2)
								{
									 ++nIdx;
									continue;
								}
								m_SearchRes[tag].name[keyLen2] = 0;
								nuBOOL bSameName = nuFALSE;
								for ( nuUINT l = 0; l < tag; l++ )
								{
									if ( 0 == nuWcsncmp(m_SearchRes[l].name, &wTempPOIName[nDanyinIdx], keyLen2))
									{
										bSameName = nuTRUE;
										++m_SearchRes[l].nFindCount;
										break;
									}
								}
								if (nuTRUE == bSameName)
								{
									++nIdx;
									continue;
								}
								#else
								//prosper add kick  "("、 ")" 、"."
								nuINT nShiftWord=0;
								for(nuINT j=0;j<=nDanyinIdx;j++)
								{
									
									if(wTempPOIName[j]==40||wTempPOIName[j]==41||wTempPOIName[j]==183 || wTempPOIName[j] ==12398)
									{
										nShiftWord++;
									}
								}
									nDanyinIdx+=nShiftWord;

								nuINT nFindWord= 0;
								for(nuINT j=0,k=0;j<keyLen2;k++)
								{
									if(wTempPOIName[nDanyinIdx+k]==0)
										break;
									if(wTempPOIName[nDanyinIdx+k]==40||wTempPOIName[nDanyinIdx+k]==41 || wTempPOIName[nDanyinIdx+k]==183 || wTempPOIName[nDanyinIdx+k] ==12398)
									{
										//k++;
										continue;
									}
									else
									{
										m_SearchRes[tag].name[j++]=wTempPOIName[nDanyinIdx+k];
										nFindWord++;
									}
								}
								if(nFindWord!=keyLen2)
								{
									 ++nIdx;
									continue;
								}
								m_SearchRes[tag].name[keyLen2] = 0;

								nuBOOL bSameName = nuFALSE;
								for ( nuUINT l = 0; l < tag; l++ )
								{
									if ( 0 == nuWcsncmp(m_SearchRes[l].name, &wTempPOIName[nDanyinIdx], keyLen2))
									{
										bSameName = nuTRUE;
										++m_SearchRes[l].nFindCount;
									    
										break;
									}
									


								}
								if (nuTRUE == bSameName)
								{
                                    
									++nIdx;
									continue;
								}
								#endif
                                
								//nuMemcpy(m_SearchRes[tag].name, &wTempPOIName[nDanyinIdx], keyLen2*sizeof(nuWCHAR));
								m_SearchRes[tag].name[keyLen2] = 0;
								m_SearchRes[tag].nFindCount    = 1;
							}
							else
							{  
					          
								if(tag >= nMAX) 
								{
									continue;
								}

								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								m_SearchRes[tag].name[nameLen>>1] = 0;
							}

							if(tag >= nMAX) 
							{
								continue;
							}
							m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
                           
							

							FillResBuffer(tag
                                , &pPoiInfo[nIdx]
                                , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
						
							if(m_bSetAreaCenterPos)
							{
								nuDOUBLE DisX = 0;
								nuDOUBLE DisY = 0;
								nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
								nuDOUBLE x1,x2,y1,y2;
								if(m_SearchRes[tag].x>0&&m_SearchRes[tag].y>0)
								{
									x1=m_SearchRes[tag].x;
									y1=m_SearchRes[tag].y;
									x2=m_TypeCenterPos.x;
									y2=m_TypeCenterPos.y;
									DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
									DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
									nuLONG nowLen = DisX+DisY;
									m_SearchRes[tag].nDistance = nowLen; 
								}
							}
                            ++res;
                            ++tag;
                        }
                    }
                    else if (m_SearchBufferInfo.name[0])
                    {
                        nameLen = (*pNameInfo);
                        if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }
                        nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
                        m_SearchRes[tag].name[nameLen>>1] = 0;

						bres = nuFALSE;
						if (nuTRUE == m_bKeyNameFuzzy)  
						{
							//bres = nuWcsFuzzyJudge(m_SearchRes[tag].name, m_SearchBufferInfo.name, nameLen>>1, keyLen, 0);
							bres = nuWcsFuzzyJudge_FORSEARCHPOI(m_SearchRes[tag].name, m_SearchBufferInfo.name
								, nameLen>>1, keyLen, 0, nDanyinIdx, nNameDiffPos);

							nuINT nData_Len = nuWcslen(m_SearchRes[tag].name);
							nuINT nKEYData_Len = nuWcslen(m_SearchBufferInfo.name);
							nuINT nWordIdx = nDanyinIdx+nNameDiffPos+1;
							if(bres && (nWordIdx)<nData_Len)
							{
								if(nNEXT_WORD_COUNT<NEXT_WORD_MAXCOUNT)
								{
									nuBOOL bSame = nuFALSE;
									for(nuINT j=0;j<nNEXT_WORD_COUNT;j++)
									{
										if( m_wsNEXT_WORD_Data[j] == m_SearchRes[tag].name[nWordIdx])
										{
											bSame = nuTRUE;
											break;
										}
									}
									if(!bSame)
									m_wsNEXT_WORD_Data[nNEXT_WORD_COUNT++]= m_SearchRes[tag].name[nWordIdx];
								}
							}

							if (bres)
							{
								m_SearchRes[tag].nNearDis	   = (nNameDiffPos<<24) | (nDanyinIdx<<16) | nameLen;
							}
						}
						else
						{
							#ifndef VALUE_EMGRT
							nuWCHAR  wTempPOIName[EACH_POI_NAME_LEN] = {0}; // Added by Damon 20100122
							//nuMemset(wTempPOIName, NULL, sizeof(wTempPOIName));
							//nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen>>1);
							#ifdef ZENRIN
							nuWcscpy(wTempPOIName, m_SearchRes[tag].name);
							#else
							nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen>>1);
							#endif
							nDanyinIdx = nuWcsKeyMatch(wTempPOIName, m_SearchBufferInfo.name, nameLen>>1, keyLen);
							if (-1 != nDanyinIdx)
							{
								bres = nuTRUE;
								m_nWordStartIdx[tag]=nDanyinIdx; //Prosper add , search by index
								m_nWordLens[tag]=nameLen;
								m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
							}
							#else
							nuWCHAR  wTempPOIName[EACH_POI_NAME_LEN] = {0}; // Added by Damon 20100122
							nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen>>1);
							nDanyinIdx = nuWcsKeyMatch(wTempPOIName, m_SearchBufferInfo.name, nameLen>>1, keyLen);
							if (-1 != nDanyinIdx)
							{
								bres = nuTRUE;
								m_nWordStartIdx[tag]=nDanyinIdx; //Prosper add , search by index
								m_nWordLens[tag]=nameLen;
								m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
							}

							nuINT nKeyLen=nuWcslen(m_SearchBufferInfo.name);
							nuBOOL bFind = nuFALSE;
							nuINT j=0;
							nuWcscpy(wTempPOIName2,m_SearchRes[tag].name);
							nuINT n=nameLen/2;
							for(nuINT i=0;i<n;i++)
							{	
								if(j==nKeyLen)
								{
									break;
								}
								if(wTempPOIName2[i]>47&& wTempPOIName2[i]<58 
									|| wTempPOIName2[i]>64 && wTempPOIName2[i]<91
									|| wTempPOIName2[i]>96 && wTempPOIName2[i]<123
									|| wTempPOIName2[i]>12398 )
								{
									if(wTempPOIName2[i] == m_SearchBufferInfo.name[j])
									{
										j++;
									}
									else
									{
										j=0;
										if(wTempPOIName2[i] == m_SearchBufferInfo.name[j])
										{
											j++;
										}
									}
								}
								else 
								{
									continue;
								}
							}

							if(j==nKeyLen)
							{
								bFind = nuTRUE;
								bres = nuTRUE;
								m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
							}
							#endif


						}
                        if (bres)
                        {
							#ifdef ZENRIN
								nuBOOL bSameName = nuFALSE;
								for ( nuUINT l = 0; l < res; l++ )
								{
									if ( 0 == nuWcscmp(m_SearchRes[l].name, m_SearchRes[tag].name))
									{
										bSameName = nuTRUE;
										break;
									}
								}
								if (nuTRUE == bSameName)
								{
									nIdx++;
									continue;
								}
							#endif
							//m_SearchRes[tag].nNearDis	   = 0;
                            
								
								
								FillResBuffer(tag
                                , &pPoiInfo[nIdx]
                                , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
                            

								if(m_bSetAreaCenterPos)
							{
								nuDOUBLE DisX = 0;
								nuDOUBLE DisY = 0;
								nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
								nuDOUBLE x1,x2,y1,y2;
								if(m_SearchRes[tag].x>0&&m_SearchRes[tag].y>0)
								{
									x1=m_SearchRes[tag].x;
									y1=m_SearchRes[tag].y;
									x2=m_TypeCenterPos.x;
									y2=m_TypeCenterPos.y;
									DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
									DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
									nuLONG nowLen = DisX+DisY;
									m_SearchRes[tag].nDistance = nowLen; 
								}
							}

							++res;
                            ++tag;
                        }
                    }
					else if(m_nPzyWord) //prosper  比對智慧注音首字 找出符合的比對POI NAME
					{
						nameLen = (*pNameInfo);
						if (nameLen>(EACH_POI_NAME_LEN-2))
						{
							nameLen = EACH_POI_NAME_LEN - 2;
						}
						nuMemcpy(m_SearchRes2.name, pNameInfo+1, nameLen);

						if(m_SearchRes2.name[0]==m_nPzyWord) //若第一個字 與m_nPzyWord同 把DATA加入
						{
							nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
							m_SearchRes[tag].name[nameLen>>1] = 0;
							m_SearchRes[tag].nNearDis	   = 0;
							FillResBuffer(tag, &pPoiInfo[nIdx], *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
							++tag;
						}
						//	++res;
					}
					else if(m_nSearchPoiKeyName[0]) //prosper 06 比對智慧注音關鍵字
					{
						nameLen = (*pNameInfo);
						if (nameLen>(EACH_POI_NAME_LEN-2))
						{
							nameLen = EACH_POI_NAME_LEN - 2;
						}
						nuMemcpy(m_SearchRes2.name, pNameInfo+1, nameLen);
						
						if(0 == nuWcsncmp(m_nSearchPoiKeyName, m_SearchRes2.name, nuWcslen(m_nSearchPoiKeyName))) //若有與關鍵字相同時 把DATA加入
						{
								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								m_SearchRes[tag].name[nameLen>>1] = 0;
								m_SearchRes[tag].nNearDis	   = 0;
								FillResBuffer(tag, &pPoiInfo[nIdx], *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));							
								++tag;
							//	++res;
						}
					}
                    else
                    {
						
                        nameLen = (*pNameInfo);
                        if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }

                          nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
                       
						m_SearchRes[tag].name[nameLen>>1] = 0;
						m_SearchRes[tag].nNearDis	   = 0;
                        
                        

                        FillResBuffer(tag
                            , &pPoiInfo[nIdx]
                            , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
                      
						if(m_bSetAreaCenterPos)
							{
								nuDOUBLE DisX = 0;
								nuDOUBLE DisY = 0;
								nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
								nuDOUBLE x1,x2,y1,y2;
								if(m_SearchRes[tag].x>0&&m_SearchRes[tag].y>0)
								{
									x1=m_SearchRes[tag].x;
									y1=m_SearchRes[tag].y;
									x2=m_TypeCenterPos.x;
									y2=m_TypeCenterPos.y;
									DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
									DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
									nuLONG nowLen = DisX+DisY;
									m_SearchRes[tag].nDistance = nowLen; 
								}
							}	
							
							++tag;
						 
						
						++res;
                        
                    }
                }
                pNameInfo = NULL;
                ++nIdx;
            }
        }
        ++m_nCityIDTag;
        //湖羲森揭褫妗珋藩棒硐刲坰珨跺CITY晞殿隙
//         if (res)
//         {
//             return res;
//         }
    }
 

	if(!m_bIndexSort) ////Prosper add , search by index
	{
		m_bIndexSort =nuTRUE;
		nuINT tempIdx = 0;
		nuINT tempLen = 0;

		SEARCH_BUFFER tempSearchRes = {0};
		for(nuDWORD i = 0; i < res;i++)
		{
			for(nuDWORD ii = 0; ii < res-1;ii++)
			{
				if(m_nWordStartIdx[ii]>m_nWordStartIdx[ii+1])
				{
					
					tempIdx = m_nWordStartIdx[ii];
					m_nWordStartIdx[ii]=m_nWordStartIdx[ii+1];
					m_nWordStartIdx[ii+1]=tempIdx;

					tempLen = m_nWordLens[ii];
					m_nWordLens[ii]=m_nWordLens[ii+1];
					m_nWordLens[ii+1]=tempLen;

					tempSearchRes = m_SearchRes[ii];
					m_SearchRes[ii]=m_SearchRes[ii+1];
					m_SearchRes[ii+1]=tempSearchRes;

				}
			}
		}
			for(nuDWORD i = 0; i < res;i++)
			{
				for(nuDWORD ii = 0; ii < res-1;ii++)
				{
					if(m_nWordStartIdx[ii]==m_nWordStartIdx[ii+1])
					{
						if((m_nWordLens[ii]<=m_nWordLens[ii+1]))
						{
							continue;
						}
						
						tempIdx = m_nWordStartIdx[ii];
						m_nWordStartIdx[ii]=m_nWordStartIdx[ii+1];
						m_nWordStartIdx[ii+1]=tempIdx;

						tempLen = m_nWordLens[ii];
						m_nWordLens[ii]=m_nWordLens[ii+1];
						m_nWordLens[ii+1]=tempLen;

						tempSearchRes = m_SearchRes[ii];
						m_SearchRes[ii]=m_SearchRes[ii+1];
						m_SearchRes[ii+1]=tempSearchRes;

					}
				}
			}

			nuINT nLimit=0;
			if(m_SearchBufferInfo.name[0]==0)  //If Danyin , No Sort
				nLimit=999;
			else 
				nLimit=-1;
			for(nuINT k=32;k>nLimit;k--)
			{
				for(nuDWORD i = 0; i < res;i++)
				{
					for(nuDWORD ii = 0; ii < res-1;ii++)
					{
						if((m_nWordStartIdx[ii]!=m_nWordStartIdx[ii+1])||(m_nWordLens[ii]!=m_nWordLens[ii+1])||m_nWordLens[ii]<k||m_nWordLens[ii+1]<k)
						{
							continue;
						}
						nuINT l1= CheckWordIsNumber(m_SearchRes[ii].name[k]);
						nuINT l2= CheckWordIsNumber(m_SearchRes[ii+1].name[k]);
						if(l1>l2)
						{
							tempIdx = m_nWordStartIdx[ii];
							m_nWordStartIdx[ii]=m_nWordStartIdx[ii+1];
							m_nWordStartIdx[ii+1]=tempIdx;

							tempLen = m_nWordLens[ii];
							m_nWordLens[ii]=m_nWordLens[ii+1];
							m_nWordLens[ii+1]=tempLen;

							tempSearchRes = m_SearchRes[ii];
							m_SearchRes[ii]=m_SearchRes[ii+1];
							m_SearchRes[ii+1]=tempSearchRes;

						}
					}
				}

			}

	}
	
	m_POI_DY_COUNT = m_SearchRes[0].nFindCount+1 ;
	
	m_nResTotal = res;
    return res;
}
nuUINT CSearchPoi::SearchNextPoi_NAME_CN(nuUINT tag)
{
    nuUINT res = 0;nuINT z=0;  SEARCH_BUFFER m_SearchRes2;
    nuUINT nIdx = 0;
    nuUINT len = 0;
    nuWORD nameLen = 0;
    nuUINT keyLen = nuWcslen(m_SearchBufferInfo.name);
    nuUINT keyLen2 = nuWcslen(m_nSearchPoiKeyDanyinT);
    nuBYTE *pNameInfo = NULL;
    nuWORD *pName = NULL;
	nuWCHAR  wTempPOIDY[EACH_POI_NAME_LEN] = {0};   // Added by Damon 20100122
	nuWCHAR  wTempPOIName[EACH_POI_NAME_LEN] = {0}; // Added by Damon 20100122
	nuINT  nDanyinIdx = 0; //梓暮等秞植撓跺弇离眈肮 added by ouyang 20100118
	nuINT  nNameDiffPos = 0;
    nuINT j=-1;
	SEARCH_BUFFER ZHUY_IN_DATA = {0};

    /*SEARCH_PI_POIINFO* pPoiInfo = (SEARCH_PI_POIINFO*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPoiInfo)
    {
        return res;
    }
    pPoiInfo = (SEARCH_PI_POIINFO*) ((nuBYTE*)pPoiInfo+m_nLB_CTLen);	
	*/
	SEARCH_PI_POIINFO* pPoiInfoCN = (SEARCH_PI_POIINFO*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
	if (!pPoiInfoCN)
	{
		return res;
	}
	pPoiInfoCN = (SEARCH_PI_POIINFO*) ((nuBYTE*)pPoiInfoCN+m_nLB_CTLen);
	nuBOOL bres = nuFALSE;
	nuINT count=0;nuINT x=0;
	nuDWORD timer1 = nuGetTickCount();
	nuINT nMAX = 4;
	#ifndef VALUE_EMGRT
		nMAX = NURO_SEARCH_BUFFER_MAX_POI ;
	#endif
	if(m_bSort)
	{
		nMAX = NURO_SEARCH_BUFFER_MAX_POI ;
	}
	nuWCHAR  wTempPOIName2[EACH_POI_NAME_LEN] = {0};
	nuDWORD  nt=0;
	nuDWORD  nt2=0;
	nuDWORD  nt3=0;
	nuDWORD t1,t2;
	nuDWORD t= nuGetTickCount();
    while (m_nCityIDTag<=m_nCityIDMax)// && tag<NURO_SEARCH_BUFFER_MAX_POI)
    {
        if (true)//LoadSearchInfo_CITY(m_nCityIDTag))
        {
            nIdx = 0;
            while (nIdx<m_nLoadPoiCount2)// && tag<NURO_SEARCH_BUFFER_MAX_POI)
            {
				if(!m_bSort)
				{
					if(res==4)
						break;
				}
				if (m_SearchBufferInfo.nCityID!=(nuWORD)-1
                    && m_SearchBufferInfo.nTownID!=(nuWORD)-1
                    && pPoiInfoCN[nIdx].nTownID!=m_SearchBufferInfo.nTownID)
                {
                    ++nIdx;
                    continue;
                }
				t1 = nuGetTickCount();
                pNameInfo = GetPoiNameInfo(pPoiInfoCN[nIdx].nNameAddr);
                t2 = nuGetTickCount();
				nt+=t2-t1;
				if (pNameInfo && *pNameInfo)
                {
					if(!m_nSearchPoiKeyDanyin[0])
					{
						if(tag >= nMAX) 
						{
							break;
						}
					}
                    if (m_nSearchPoiKeyDanyin[0])
                    {
						
                        pName = (nuWORD*)(pNameInfo+2+*pNameInfo);
                        nameLen = (*(pNameInfo+1+*pNameInfo));
                        if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }
						if(tag<nMAX)
						{
							nuMemcpy(m_SearchRes[tag].name, pName, nameLen);
							m_SearchRes[tag].name[nameLen>>1] = 0;

						}
						else
						{
							nuMemcpy(ZHUY_IN_DATA.name, pName, nameLen);
							ZHUY_IN_DATA.name[nameLen>>1] = 0;
						}
						// Added by Damon 20100122
						if (nuTRUE == m_bComCompare)
						{
                          
							if(tag<nMAX)
							{
								nuMemset(wTempPOIDY, NULL, sizeof(wTempPOIDY));
								nuWcsDelSpSign(wTempPOIDY, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen);
								nDanyinIdx = nuWcsKeyMatch(wTempPOIDY, m_nSearchPoiKeyDanyinT, nuWcslen(wTempPOIDY), keyLen2);
								
								//nuMemcpy(wTempPOIDY,m_SearchRes[tag].name,sizeof(m_SearchRes[tag].name));
							
								//nDanyinIdx = nuWcsKeyMatch(pName, m_nSearchPoiKeyDanyinT, nameLen/2, keyLen2);
								nuDWORD	t3= nuGetTickCount();

								if(nDanyinIdx>0)
									int k=0;
								nt2+=t3-t2;
								nt3+=t2-t1;
							}
							else
							{
								#ifndef VALUE_EMGRT 
								nuMemset(wTempPOIDY, NULL, sizeof(wTempPOIDY));
								nuWcsDelSpSign(wTempPOIDY, ZHUY_IN_DATA.name, EACH_POI_NAME_LEN, nameLen);
								#endif
								nDanyinIdx = nuWcsKeyMatch(wTempPOIDY, m_nSearchPoiKeyDanyinT, nuWcslen(wTempPOIDY), keyLen2);
								//ZHUY_IN_DATA
							}
						}
						else
						{
                            if(tag<nMAX) 
							{
								nDanyinIdx = nuWcsKeyMatch(m_SearchRes[tag].name, m_nSearchPoiKeyDanyinT, nameLen, keyLen2);
							}
							else
							{	
								#ifndef VALUE_EMGRT 
								nDanyinIdx = nuWcsKeyMatch(ZHUY_IN_DATA.name, m_nSearchPoiKeyDanyinT, nameLen, keyLen2);
								#endif
							}
						}
						// -----------------------

						nameLen = *(pNameInfo);

                        if ( nDanyinIdx != -1 )
                        {//植菴撓跺趼睫羲宎眈肮 added by ouyang 20100118
							
							//Prosper 2011.12.07
							#ifndef VALUE_EMGRT 
							nuINT n= nDanyinIdx+keyLen2;

							if(n<nameLen/2)
							{
								if(tag >= nMAX) 
								{
									bool bRes=Sort_KEY_index(ZHUY_IN_DATA.name[n]);
									//if(bRes==TRUE)

									//++nIdx;
									//continue;
								}
								else
								{
									Sort_KEY_index(m_SearchRes[tag].name[n]);
								}
							}
							#endif
							
							if(tag >= nMAX) 
							{
								++nIdx;
								continue;
							}

							if (nuTRUE == m_bComCompare )
							{  // Modified by Damon 20100122	
								nuMemset(wTempPOIName, NULL, sizeof(wTempPOIName));
								#ifdef VALUE_EMGRT
								nuWCHAR  wTempPOIName2[EACH_POI_NAME_LEN] = {0};
								nuMemcpy(wTempPOIName2, pNameInfo+1, nameLen);
								int p=0;
								for(nuINT i=0;i<nameLen/2;i++)
								{
									if(wTempPOIName2[i]>47&& wTempPOIName2[i]<58 
										|| wTempPOIName2[i]>64 && wTempPOIName2[i]<91
										|| wTempPOIName2[i]>96 && wTempPOIName2[i]<123
										|| wTempPOIName2[i]>12398 )
									{
										m_SearchRes[tag].name[p++] = wTempPOIName2[i];
									}
								}

								#else
								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								#endif
								nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen);
							


								//nuMemcpy(wTempPOIName,m_SearchRes[tag].name,sizeof(m_SearchRes[tag].name));
								//nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen);
								
						
									
								nuBOOL bSameName = nuFALSE;
								for ( nuUINT l = 0; l < tag; l++ )
								{
									if ( 0 == nuWcsncmp(m_SearchRes[l].name, &wTempPOIName[nDanyinIdx], keyLen2))
									{
										bSameName = nuTRUE;
										++m_SearchRes[l].nFindCount;
									    
										break;
									}
									


								}
								if (nuTRUE == bSameName)
								{
                                    
									++nIdx;
									continue;
								}
                                
								nuMemcpy(m_SearchRes[tag].name, &wTempPOIName[nDanyinIdx], keyLen2*sizeof(nuWCHAR));
								m_SearchRes[tag].name[keyLen2] = 0;
								m_SearchRes[tag].nFindCount    = 1;
								
							}
							else
							{  
					          
								if(tag >= nMAX) 
								{
									continue;
								}

								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								m_SearchRes[tag].name[nameLen>>1] = 0;
							}

							if(tag >= nMAX) 
							{
								continue;
							}
							m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
                           
							FillResBuffer(tag
                                , &pPoiInfoCN[nIdx]
                                , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
						 
								
                            ++res;
                            ++tag;
                        }
                    }
                    else if (m_SearchBufferInfo.name[0])
                    {
                        nameLen = (*pNameInfo);
                        if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }
                        nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
                        m_SearchRes[tag].name[nameLen>>1] = 0;

						bres = nuFALSE;
						if (nuTRUE == m_bKeyNameFuzzy)  
						{
							//bres = nuWcsFuzzyJudge(m_SearchRes[tag].name, m_SearchBufferInfo.name, nameLen>>1, keyLen, 0);
							bres = nuWcsFuzzyJudge_FORSEARCHPOI(m_SearchRes[tag].name, m_SearchBufferInfo.name
								, nameLen>>1, keyLen, 0, nDanyinIdx, nNameDiffPos);
							nuINT nData_Len = nuWcslen(m_SearchRes[tag].name);
							nuINT nKEYData_Len = nuWcslen(m_SearchBufferInfo.name);
							nuINT nWordIdx = nDanyinIdx+nNameDiffPos+1;
							if(bres && (nWordIdx)<nData_Len)
							{
								if(nNEXT_WORD_COUNT<NEXT_WORD_MAXCOUNT)
								{
									nuBOOL bSame = nuFALSE;
									for(nuINT j=0;j<nNEXT_WORD_COUNT;j++)
									{
										if( m_wsNEXT_WORD_Data[j] == m_SearchRes[tag].name[nWordIdx])
										{
											bSame = nuTRUE;
											break;
										}
									}
									if(!bSame)
									m_wsNEXT_WORD_Data[nNEXT_WORD_COUNT++]= m_SearchRes[tag].name[nWordIdx];
								}
							}
						
							if (bres)
							{
								m_SearchRes[tag].nNearDis	   = (nNameDiffPos<<24) | (nDanyinIdx<<16) | nameLen;
							}
						}
						else
						{
							#ifndef VALUE_EMGRT
							nuWCHAR  wTempPOIName[EACH_POI_NAME_LEN] = {0}; // Added by Damon 20100122
							//nuMemset(wTempPOIName, NULL, sizeof(wTempPOIName));
							nuWcsDelSpSign(wTempPOIName, m_SearchRes[tag].name, EACH_POI_NAME_LEN, nameLen>>1);
							nDanyinIdx = nuWcsKeyMatch(wTempPOIName, m_SearchBufferInfo.name, nameLen>>1, keyLen);
							if (-1 != nDanyinIdx)
							{
								bres = nuTRUE;
								m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
							}
							#else
							nuINT nKeyLen=nuWcslen(m_SearchBufferInfo.name);
							nuBOOL bFind = nuFALSE;
							nuINT j=0;
							nuWcscpy(wTempPOIName2,m_SearchRes[tag].name);
							nuINT n=nameLen/2;
							for(nuINT i=0;i<n;i++)
							{	
								if(wTempPOIName2[i]>47&& wTempPOIName2[i]<58 
									|| wTempPOIName2[i]>64 && wTempPOIName2[i]<91
									|| wTempPOIName2[i]>96 && wTempPOIName2[i]<123
									|| wTempPOIName2[i]>12398 )
								{
									if(wTempPOIName2[i] == m_SearchBufferInfo.name[j])
									{
										j++;
									}
									else
									{
										j=0;
									}
								}
								else 
								{
									continue;
								}
								if(j==nKeyLen)
									break;
							}

							if(j==nKeyLen)
							{
								bFind = nuTRUE;
								bres = nuTRUE;
								m_SearchRes[tag].nNearDis	   = (nDanyinIdx<<16) | nameLen;
							}
							#endif


						}
                        if (bres)
                        {
							#ifdef ZENRIN
								nuBOOL bSameName = nuFALSE;
								for ( nuUINT l = 0; l < res; l++ )
								{
									if ( 0 == nuWcscmp(m_SearchRes[l].name, m_SearchRes[tag].name))
									{
										bSameName = nuTRUE;
										break;
									}
								}
								if (nuTRUE == bSameName)
								{
									nIdx++;
									continue;
								}
							#endif
							//m_SearchRes[tag].nNearDis	   = 0;
                            FillResBuffer(tag
                                , &pPoiInfoCN[nIdx]
                                , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
                            
							++res;
                            ++tag;
                        }
                    }
					else if(m_nPzyWord) //prosper  比對智慧注音首字 找出符合的比對POI NAME
					{
						nameLen = (*pNameInfo);
						if (nameLen>(EACH_POI_NAME_LEN-2))
						{
							nameLen = EACH_POI_NAME_LEN - 2;
						}
						nuMemcpy(m_SearchRes2.name, pNameInfo+1, nameLen);

						if(m_SearchRes2.name[0]==m_nPzyWord) //若第一個字 與m_nPzyWord同 把DATA加入
						{
							nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
							m_SearchRes[tag].name[nameLen>>1] = 0;
							m_SearchRes[tag].nNearDis	   = 0;
							FillResBuffer(tag, &pPoiInfoCN[nIdx], *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
							++tag;
						}
						//	++res;
					}
					else if(m_nSearchPoiKeyName[0]) //prosper 06 比對智慧注音關鍵字
					{
						nameLen = (*pNameInfo);
						if (nameLen>(EACH_POI_NAME_LEN-2))
						{
							nameLen = EACH_POI_NAME_LEN - 2;
						}
						nuMemcpy(m_SearchRes2.name, pNameInfo+1, nameLen);
						
						if(0 == nuWcsncmp(m_nSearchPoiKeyName, m_SearchRes2.name, nuWcslen(m_nSearchPoiKeyName))) //若有與關鍵字相同時 把DATA加入
						{
								nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
								m_SearchRes[tag].name[nameLen>>1] = 0;
								m_SearchRes[tag].nNearDis	   = 0;
								FillResBuffer(tag, &pPoiInfoCN[nIdx], *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));							
								++tag;
							//	++res;
						}
					}
                    else
                    {
						
                        nameLen = (*pNameInfo);
                        if (nameLen>(EACH_POI_NAME_LEN-2))
                        {
                            nameLen = EACH_POI_NAME_LEN - 2;
                        }

                          nuMemcpy(m_SearchRes[tag].name, pNameInfo+1, nameLen);
                       
						m_SearchRes[tag].name[nameLen>>1] = 0;
						m_SearchRes[tag].nNearDis	   = 0;
                        
                        

                        FillResBuffer(tag
                            , &pPoiInfoCN[nIdx]
                            , *(pNameInfo+2+*pNameInfo+*(pNameInfo+1+*pNameInfo)));
                        ++tag;
						 
						
						++res;
                        
                    }
                }
                pNameInfo = NULL;
                ++nIdx;
            }
        }
        ++m_nCityIDTag;
        //湖羲森揭褫妗珋藩棒硐刲坰珨跺CITY晞殿隙
//         if (res)
//         {
//             return res;
//         }
    }
 
	nuDWORD tt= nuGetTickCount();
//	nuDWORD t3= t2-t1;
	nuDWORD timer2 = nuGetTickCount();

	m_nResTotal = res;
    return res;
}
nuUINT CSearchPoi::Load()
{
	/*if(pPoiInfoCN != NULL)
	{
		if(pPoiInfoCN[0].nNameAddr>	45036)
	{
		int l=0;

	}
	}*/
	
	
		

	
	

	
	if(m_nMAPID!= m_searchInfo.nMapID || m_nCityID !=m_searchInfo.nCityID)
	{
		m_nMAPID  = m_searchInfo.nMapID;
		m_nCityID = m_searchInfo.nCityID;
		m_nCityIDLoad = -1;
		LoadSearchInfo_CITY(m_searchInfo.nCityID);
	

	}

	return true;
	
	


}
nuUINT CSearchPoi::SearchNextPoi_TYPE(nuUINT tag)
{
    nuUINT res = 0;
    nuUINT resTemp = 0;
    nuUINT nIdx = 0;
    nuBOOL bTP2 = nuFALSE;
    CMapFilePnY pnfile;
    if (true)//m_SearchBufferInfo.nTP2ID!=(nuBYTE)-1)
    {
        bTP2 = nuTRUE;
        if (!pnfile.Open(m_SearchBufferInfo.nMapID))
        {
            return 0;
        }
    }
    SEARCH_PI_POIINFO* pPoiInfo = (SEARCH_PI_POIINFO*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPoiInfo)
    {
        return res;
    }
    pPoiInfo = (SEARCH_PI_POIINFO*) ((nuBYTE*)pPoiInfo+m_nLB_CTLen);
    nuINT keyLen = nuWcslen(m_searchInfo.name);
    while (m_nCityIDTag<=m_nCityIDMax && tag<NURO_SEARCH_BUFFER_MAX_POI)
    {
        resTemp = res;
        nuBOOL bLoadRes = nuFALSE;
        if (bTP2)
        {
            bLoadRes = LoadSearchInfo_CITY(m_nCityIDTag, nuFALSE);
        }
        else
        {
            bLoadRes = LoadSearchInfo_CITY(m_nCityIDTag, nuTRUE);
        }
        if (bLoadRes)
        {
            nIdx = 0;
            while (nIdx<m_nLoadPoiCount && tag<NURO_SEARCH_BUFFER_MAX_POI)
            {
                if (pPoiInfo[nIdx].nTP1ID<m_SearchBufferInfo.nTP1ID)
                {
                    ++nIdx;
                    continue;
                }
               if (pPoiInfo[nIdx].nTP1ID>m_SearchBufferInfo.nTP1ID)
                {
                    break;
                }
                if (m_SearchBufferInfo.nCityID!=(nuWORD)-1
                    && m_SearchBufferInfo.nTownID!=(nuWORD)-1
                    && pPoiInfo[nIdx].nTownID!=m_SearchBufferInfo.nTownID)
                {
                    ++nIdx;
                    continue;
                }
                if (m_SearchBufferInfo.nTP2ID!=(nuBYTE)-1
                    && pPoiInfo[nIdx].nTP2ID!=m_SearchBufferInfo.nTP2ID)
                {
                    ++nIdx;
                    continue;
                }
                FillResBuffer(tag, &pPoiInfo[nIdx]);
                if (!GetPoiName(tag
                    , (nuBYTE*)(m_SearchRes[tag].name)
                    , EACH_POI_NAME_LEN
                    , pnfile) )
                {
                    m_SearchRes[tag].type = 0;
                    ++nIdx;
                    continue;
                }
				if(m_bSetAreaCenterPos)
				{
					nuDOUBLE DisX = 0;
					nuDOUBLE DisY = 0;
					nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
					nuDOUBLE x1,x2,y1,y2;
					if(m_SearchRes[tag].x>0&&m_SearchRes[tag].y>0)
					{
						x1=m_SearchRes[tag].x;
						y1=m_SearchRes[tag].y;
						x2=m_TypeCenterPos.x;
						y2=m_TypeCenterPos.y;
						DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
						DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
						nuLONG nowLen = DisX+DisY;
						m_SearchRes[tag].nDistance = nowLen; 
					}
				}
                if (keyLen)
                {
                    nuUINT len = nuWcslen(m_SearchRes[tag].name);
                    if (!nuWcsFuzzyJudge(m_SearchRes[tag].name, m_searchInfo.name, len, keyLen, 0))
                    {
                        m_SearchRes[tag].type = 0;
                        ++nIdx;
                        continue;
                    }
                }
                ++res;
                ++tag;
                ++nIdx;
            }
        }
        ++m_nCityIDTag;
        //湖羲森揭褫妗珋藩棒硐刲坰珨跺CITY晞殿隙
//         if (res)
//         {
//             return res;
//         }
    }
    return res;	
}
nuUINT CSearchPoi::SetAreaCenterPos(nuroPOINT pos)
{
	__android_log_print(ANDROID_LOG_INFO, "pos", "SetAreaCenterPos .......");
	m_bSetAreaCenterPos = nuTRUE;
	m_TypeCenterPos = pos;
	return true;
}
nuUINT CSearchPoi::SearchNextPoi_AREA()
{
    SEARCH_SPOIINFO *pPoiinfoBuffer 
        = (SEARCH_SPOIINFO*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPoiinfoBuffer)
    {
        return 0;
    }
    pPoiinfoBuffer = (SEARCH_SPOIINFO*)((nuBYTE*)pPoiinfoBuffer+m_nLB_CTLen);	
    CMapFilePdwY pdwfile;
    CMapFileBhY bhfile;
    CMapFilePnY pnfile;
    if (!pdwfile.Open(m_searchInfo.nMapID) 
        || !bhfile.Open(m_searchInfo.nMapID)
        || !pnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
	NURORECT blkbaserect = {0};
    nuUINT blkID = -1;
    nuUINT pdwaddr = -1;
    nuUINT blkpoinum = 0;
    nuUINT blkpoitag = 0;
    nuUINT count = 0;
    m_nResTotal = 0;
    SEARCH_PDW_A1 *pPdwpoiinfo 
        = (SEARCH_PDW_A1*)((nuBYTE*)pPoiinfoBuffer+m_nLB_CTLen+m_nBHIdxLen);
	NUROPOINT poipos = {0};
	BLOCKIDARRAY m_sAreaBlkT = {0};
    nuRectToBlockIDs(m_sAreaRectT, &m_sAreaBlkT, EACHBLOCKSIZE);
    nuUINT xc = m_sAreaBlkT.nXend - m_sAreaBlkT.nXstart + 2;  // Modified by Damon
    nuUINT yc = m_sAreaBlkT.nYend - m_sAreaBlkT.nYstart + 2;  // Modified by Damon
    nuUINT i = 0;
    nuUINT j = 0;
    nuUINT k = 0;

    const nuUINT _nLimitBlkPoiInfoLen = 1000000;
    nuWORD cityTempTag      = -1;
    nuUINT cityPILen        = 0;
    nuUINT addrTempTag      = 0;
    nuBYTE *pNameTempBuffer = NULL;
    nuBYTE *pNameTempReal = NULL;
    nuUINT blkPoiInfoLen = 0;
    nuUINT keylen = nuWcslen(m_searchInfo.name);

	nuroPOINT ptCenter = {0};
    ptCenter.x = (m_sAreaRectT.left + m_sAreaRectT.right)/2;
	ptCenter.y = (m_sAreaRectT.top + m_sAreaRectT.bottom)/2;
	nuDOUBLE LatEffect = getLatEffect(ptCenter);

	nuUINT nMaxDis = NURO_ABS((m_sAreaRectT.right - m_sAreaRectT.left)/2);
	nuBOOL bNearPoiCheck = nuFALSE;

	nuUINT nMinDis = 0;
	for(k=1;k<=5;k++)
	{
		nMaxDis=NURO_ABS((m_sAreaRectT.right - m_sAreaRectT.left)/2)*k/5;
		nMinDis=NURO_ABS((m_sAreaRectT.right - m_sAreaRectT.left)/2)*(k-1)/5;
		if(count>=NURO_SEARCH_BUFFER_MAX_POI) continue;
    for (i=0; i<xc; ++i)
    {
        for (j=0; j<yc; ++j)
        {
            if (count>=NURO_SEARCH_BUFFER_MAX_POI)
            {
				if(!m_bSA)
                break;
            }
            blkID = (m_sAreaBlkT.nYstart + j)*XBLOCKNUM + m_sAreaBlkT.nXstart+i;
            pdwaddr = bhfile.GetBHPoiAddrByIdx(BlkIDToIdx(blkID));
            if (pdwaddr==(nuUINT)-1)
            {
                continue;
            }
            nuBlockIDtoRect(blkID, &blkbaserect);
            blkpoinum = pdwfile.GetBlkPoiNum(pdwaddr);
            if (!blkpoinum)
            {
                continue;
            }
            blkPoiInfoLen =
                pdwfile.GetBlkPoiInfo(pdwaddr, pPdwpoiinfo, m_nLoadBufferMaxLen-m_nLB_CTLen-m_nBHIdxLen);
            if (!blkPoiInfoLen)
            {
                continue;
            }
            if (blkPoiInfoLen>_nLimitBlkPoiInfoLen)
            {
                cityTempTag = -1;
                addrTempTag = 0;
                pNameTempBuffer = NULL;
            }
            for (blkpoitag=0; blkpoitag<blkpoinum; ++blkpoitag)
            {
                if (count>=NURO_SEARCH_BUFFER_MAX_POI)
                {
						if(!m_bSA)
						break;
						//Prosper 2012.08 Max_POI , Sort & Continue Col.
						if(!bNearPoiCheck)
						{
							bNearPoiCheck=true;
							SEARCH_BUFFER temp = {0};
						nuDWORD a1 = nuGetTickCount();
						nuINT times=NURO_SEARCH_BUFFER_MAX_POI-1;
						for (nuINT j=0; j<times; j++)
						{ 
							nuINT tag2 = 0;
							nuINT last = times-j;
							for(i=1;i<=last;i++)
							{ 
								if(m_SearchRes[tag2].nDistance < m_SearchRes[i].nDistance)
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
		
				poipos.x = blkbaserect.left + pPdwpoiinfo[blkpoitag].pos.x+ pPdwpoiinfo[blkpoitag].NaviPos.x; 
    			poipos.y = blkbaserect.top + pPdwpoiinfo[blkpoitag].pos.y + pPdwpoiinfo[blkpoitag].NaviPos.y;

				//算出所在經緯度距離
				nuDOUBLE DisX = 0;
				nuDOUBLE DisY = 0;
				DisX = NURO_ABS(poipos.x-ptCenter.x)*LONEFFECTCONST*LatEffect/EFFECTBASE;
			    DisY = NURO_ABS(poipos.y-ptCenter.y)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5

				//算出距離
				nuLONG nowLen = DisX+DisY; //(long)nuSqrt(DisX*DisX+DisY*DisY);
				if(nMinDis>nowLen) continue;

                if ((m_SearchBufferInfo.nTP1ID==(nuBYTE)-1||m_SearchBufferInfo.nTP1ID == pPdwpoiinfo[blkpoitag].nTP1ID)
                    && (m_SearchBufferInfo.nTP2ID==(nuBYTE)-1||m_SearchBufferInfo.nTP2ID == pPdwpoiinfo[blkpoitag].nTP2ID)
	                    && (nuLONG)nMaxDis >= nowLen/*nuPointInRect(&poipos, &m_sAreaRectT)*/
                    )//point in area
                {
					nuUINT iLen = 0;

                    if (cityTempTag==(nuWORD)-1 || cityTempTag!=pPdwpoiinfo[blkpoitag].nCityID)
                    {
                        pNameTempBuffer = (nuBYTE*)pPdwpoiinfo + NURO_MAX(_nLimitBlkPoiInfoLen, blkPoiInfoLen);
                        cityPILen = pnfile.GetPoiInfo_CITY(pPdwpoiinfo[blkpoitag].nCityID
                            , pNameTempBuffer
                            , m_nLoadBufferMaxLen-m_nLB_CTLen-m_nBHIdxLen - NURO_MAX(_nLimitBlkPoiInfoLen, blkPoiInfoLen)
                            , &addrTempTag);
                        if (!cityPILen)
                        {
                            continue;
                        }
                        cityTempTag = pPdwpoiinfo[blkpoitag].nCityID;
                    }
                    if (pPdwpoiinfo[blkpoitag].nNameAddr<addrTempTag
                        || pPdwpoiinfo[blkpoitag].nNameAddr>=addrTempTag+cityPILen)
                    {
                        continue;
                    }
						
						if(count < NURO_SEARCH_BUFFER_MAX_POI)
						{
                    nuMemset(m_SearchRes[count].name, 0, sizeof(m_SearchRes[count].name));
                    pNameTempReal = pNameTempBuffer + pPdwpoiinfo[blkpoitag].nNameAddr - addrTempTag;
                    nuMemcpy( m_SearchRes[count].name
                        , pNameTempReal+1
                        , NURO_MIN(*pNameTempReal, sizeof(m_SearchRes[count].name)-2) );
                    if (keylen)
                    {
                        if (!nuWcsFuzzyJudge(m_SearchRes[count].name
                            , m_searchInfo.name
                            , nuWcslen(m_SearchRes[count].name)
                            , keylen
                            , 0) )
                        {
                            continue;
                        }
                    }

					m_SearchRes[count].nDistance = nowLen;
                    m_SearchRes[count].cityID	= pPdwpoiinfo[blkpoitag].nCityID;
                    m_SearchRes[count].mapID	= m_SearchBufferInfo.nMapID;
                    m_SearchRes[count].townID	= pPdwpoiinfo[blkpoitag].nTownID;
                    m_SearchRes[count].nameAddr	= pPdwpoiinfo[blkpoitag].nNameAddr;
                    m_SearchRes[count].extAddr	= pPdwpoiinfo[blkpoitag].nMoreInfoAddr;
                    m_SearchRes[count].ex1c		= pPdwpoiinfo[blkpoitag].nTP1ID;
                    m_SearchRes[count].ex2c		= pPdwpoiinfo[blkpoitag].nTP2ID;
                    m_SearchRes[count].x		= poipos.x;
                    m_SearchRes[count].y		= poipos.y;
                    m_SearchRes[count].type		= SEARCH_BT_POI;
                    m_SearchRes[count].strokes
                        = pNameTempReal[pNameTempReal[0]+pNameTempReal[pNameTempReal[0]+1]+2];
                    ++count;
                }
						//Prosper 2012.08 For Near_Search Find nearest pois
					
						if(bNearPoiCheck)
						{
							nuINT i=-1;
							nuBOOL bNeedAdd=nuFALSE;
							if(m_SearchRes[NURO_SEARCH_BUFFER_MAX_POI-1].nDistance < nowLen)
							{
								continue;
							}
							//PROSPER ADD 2015.10.02
							nuBOOL bCheckADD = nuFALSE;
							for(i=0;i<NURO_SEARCH_BUFFER_MAX_POI-1;i++)
							{
								if(m_SearchRes[i].nameAddr == pPdwpoiinfo[blkpoitag].nNameAddr)
								{
									bCheckADD = nuTRUE;
									break;
								}
								if(m_SearchRes[i].nDistance > nowLen)
								{
									break;
								}

							}
							if(bCheckADD)
							{
								break;
							}
							for(i=0;i<NURO_SEARCH_BUFFER_MAX_POI-1;i++)
							{
								if(m_SearchRes[i].nDistance > nowLen) //If More Closer , Add
								{
									bNeedAdd=nuTRUE;
								}
								else
								{
									continue;
								}							
	
								for(int j=NURO_SEARCH_BUFFER_MAX_POI-1;j>i;j--)//Shift 1,Start Form i 
								{
									m_SearchRes[j]=m_SearchRes[j-1];
								}
								break;
							}
							if(!bNeedAdd) //Not Need Added
							{
								continue;
							}
	
							nuMemset(m_SearchRes[i].name, 0, sizeof(m_SearchRes[i].name));
							pNameTempReal = pNameTempBuffer + pPdwpoiinfo[blkpoitag].nNameAddr - addrTempTag;
							nuMemcpy( m_SearchRes[i].name, pNameTempReal+1
							, NURO_MIN(*pNameTempReal, sizeof(m_SearchRes[i].name)-2) );
					
						
							m_SearchRes[i].nNearDis = nowLen;
							m_SearchRes[i].cityID	= pPdwpoiinfo[blkpoitag].nCityID;
							m_SearchRes[i].mapID	= m_SearchBufferInfo.nMapID;
							m_SearchRes[i].townID	= pPdwpoiinfo[blkpoitag].nTownID;
							m_SearchRes[i].nameAddr	= pPdwpoiinfo[blkpoitag].nNameAddr;
							m_SearchRes[i].extAddr	= pPdwpoiinfo[blkpoitag].nMoreInfoAddr;
							m_SearchRes[i].ex1c		= pPdwpoiinfo[blkpoitag].nTP1ID;
							m_SearchRes[i].ex2c		= pPdwpoiinfo[blkpoitag].nTP2ID;
							m_SearchRes[i].x		= poipos.x;
							m_SearchRes[i].y		= poipos.y;
							m_SearchRes[i].type		= SEARCH_BT_POI;
							m_SearchRes[i].strokes
								= pNameTempReal[pNameTempReal[0]+pNameTempReal[pNameTempReal[0]+1]+2];
							//count++;
						}
	                }
	            }
            }
        }
    }
    m_nResTotal = count;
    return m_nResTotal;
}
nuUINT CSearchPoi::SearchNextPoi_Car()
{	
    //婥窒CARfile
    CMapFileCarY carfile;
    CMapFilePnY pnfile;
    if (!carfile.Open(m_searchInfo.nMapID)
        || !pnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    nuUINT nCarFac = carfile.GetTypeNum();
    if (!nCarFac || m_nCarFac>=nCarFac)
    {
        return 0;
    }
    nuBYTE *pCarinfoBuffer 
        = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pCarinfoBuffer)
    {
        return 0;
    }
    pCarinfoBuffer += m_nLB_CTLen;
    if (carfile.GetLength()>m_nLoadBufferMaxLen-m_nLB_CTLen)
    {
        return 0;
    }
    if (!carfile.ReadData(0, pCarinfoBuffer, carfile.GetLength()))
    {
        return 0;
    }
    NURO_CAR_FACTYPE *pCarFac = (NURO_CAR_FACTYPE*)(pCarinfoBuffer+4+sizeof(NURO_CAR_FACTYPE)*m_nCarFac);
    NURO_CAR_DATA *pCarData = (NURO_CAR_DATA*)(pCarinfoBuffer + pCarFac->addr);
    
    nuUINT nData = 0;
    nuUINT count = 0;
    while (nData < pCarFac->count)
    {
        if (count>=NURO_SEARCH_BUFFER_MAX_POI)
        {
            break;
        }
        if (m_searchInfo.nCityID!=(nuWORD)-1)
        {
            if (pCarData[nData].city!=m_searchInfo.nCityID)
            {
                ++nData;
                continue;
            }
            else
            {
                if (m_searchInfo.nTownID!=(nuWORD)-1)
                {
                    if (pCarData[nData].town!=m_searchInfo.nTownID)
                    {
                        ++nData;
                        continue;
                    }
                }
            }
        }
        m_SearchRes[count].cityID = pCarData[nData].city;
        m_SearchRes[count].townID = pCarData[nData].town;
        m_SearchRes[count].x = pCarData[nData].x;
        m_SearchRes[count].y = pCarData[nData].y;
        m_SearchRes[count].extAddr = pCarData[nData].extAddr;
        m_SearchRes[count].mapID = m_searchInfo.nMapID;
        m_SearchRes[count].nameAddr = pCarData[nData].nameAddr;
        m_SearchRes[count].type = SEARCH_BT_POI;
        m_SearchRes[count].strokes	= -1;
        m_SearchRes[count].name[0]	= 0;
        
        pnfile.GetName(pCarData[nData].nameAddr, m_SearchRes[count].name
            , EACH_BUFFER_NAME_LEN<<1, &m_SearchRes[count].strokes);
        ++count;
        ++nData;
    }
    return count;
}
nuUINT CSearchPoi::SearchNextPoi_SPI()
{
    CMapFileSPIY spifile;
    CMapFilePnY pnfile;
    if (!spifile.Open(m_searchInfo.nMapID)
        || !pnfile.Open(m_searchInfo.nMapID))
    {
        return 0;
    }
    nuUINT nSPType = spifile.GetTypeNum();
    if (!nSPType || m_nSPType>=nSPType)
    {
        return 0;
    }
    nuBYTE *pSPIinfoBuffer 
        = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pSPIinfoBuffer)
    {
        return 0;
    }
    pSPIinfoBuffer += m_nLB_CTLen;
    if (spifile.GetLength()>m_nLoadBufferMaxLen-m_nLB_CTLen)
    {
        return 0;
    }
    if (!spifile.ReadData(0, pSPIinfoBuffer, spifile.GetLength()))
    {
        return 0;
    }
    NURO_CAR_FACTYPE *pSPType = (NURO_CAR_FACTYPE*)(pSPIinfoBuffer+4+sizeof(NURO_CAR_FACTYPE)*m_nSPType);
    NURO_CAR_DATA *pSPData = (NURO_CAR_DATA*)(pSPIinfoBuffer + pSPType->addr);
    
    nuUINT nData = 0;
    nuUINT count = 0;
    while (nData < pSPType->count)
    {
        if (count>=NURO_SEARCH_BUFFER_MAX_POI)
        {
            break;
        }
        if (m_searchInfo.nCityID!=(nuWORD)-1)
        {
            if (pSPData[nData].city!=m_searchInfo.nCityID)
            {
                ++nData;
                continue;
            }
            else
            {
                if (m_searchInfo.nTownID!=(nuWORD)-1)
                {
                    if (pSPData[nData].town!=m_searchInfo.nTownID)
                    {
                        ++nData;
                        continue;
                    }
                }
            }
        }
        m_SearchRes[count].cityID = pSPData[nData].city;
        m_SearchRes[count].townID = pSPData[nData].town;
        m_SearchRes[count].x = pSPData[nData].x;
        m_SearchRes[count].y = pSPData[nData].y;
        m_SearchRes[count].extAddr = pSPData[nData].extAddr;
        m_SearchRes[count].mapID = m_searchInfo.nMapID;
        m_SearchRes[count].nameAddr = pSPData[nData].nameAddr;
        m_SearchRes[count].type = SEARCH_BT_POI;
        m_SearchRes[count].strokes	= -1;
        m_SearchRes[count].name[0]	= 0;
        
        pnfile.GetName(pSPData[nData].nameAddr, m_SearchRes[count].name
            , EACH_BUFFER_NAME_LEN<<1, &m_SearchRes[count].strokes);
        ++count;
        ++nData;
    }
    return count;
}
/*
nuBOOL CSearchPoi::nuPhoneJudge(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen)
{
    if (desLen<keyLen)
    {
        return nuFALSE;
    }
    nuUINT i=0;
    for (i=0; i<desLen; ++i)
    {
        if (pDes[i]=='-')
        {
            continue;
        }
        else if (pDes[i] == pKey[0])
        {
            if (keyLen==1)
            {
                return nuTRUE;
            }
            else
            {
                --keyLen;
                ++pKey;
                continue;
            }
        }
        else
        {
            return nuFALSE;
        }
    }
    return nuFALSE;
}
*/
nuINT CSearchPoi::nuPhoneCompare(nuWCHAR* pDes, nuWCHAR* pSrc, nuUINT desLen, nuUINT keyLen)
{
    nuUINT i=0;
    for (i=0; i<desLen; ++i)
    {
        if (pDes[i]=='-')
        {
            continue;
        }
        else if (pDes[i] == pSrc[0])
        {
            if (keyLen==1)
            {
                return 0;
            }
            else
            {
                --keyLen;
                ++pSrc;
                continue;
            }
        }
        else
        {
            return pDes[i] - pSrc[0];
        }
    }
    return - pSrc[0];
}
/*
nuUINT CSearchPoi::SearchNextPoi_PHONE(nuUINT tag)
{
    nuUINT res	= 0;
    nuUINT nIdx	= 0;
    nuWORD phone[32] = {0};
    nuWORD *_pPhoneTag = NULL;
    nuBYTE keylen = nuWcslen(m_nSearchPhone);
    SEARCH_PI_POIINFO *pPoiInfo = NULL;
    SEARCH_PI_POIINFO poiInfo2;
    SEARCH_PI_POIINFO *_pPoiInfoTag = NULL;
    nuBYTE phoneLen = 0;
    nuBYTE* pPaInfo = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!pPaInfo)
    {
        return res;
    }
    pPaInfo = (nuBYTE*)pPaInfo+m_nLB_CTLen;
#ifdef _USE_PANA_MODE_
    if (!m_nLB_PNLen)
    {
        //load all pnfile
        CMapFilePnY pnfile;
        if (pnfile.Open(m_searchInfo.nMapID))
        {
            m_nLB_PNLen = pnfile.GetLength();
            if ( m_nLB_PNLen<=(m_nLoadBufferMaxLen-m_nLB_CTLen) )
            {
                if (pnfile.ReadData(0, pPaInfo, m_nLB_PNLen))
                {
                    m_nLB_PNLen = ((m_nLB_PNLen+3)>>2)<<2;
                }
                else
                {
                    m_nLB_PNLen = 0;
                }
            }
            else
            {
                m_nLB_PNLen = 0;
            }
            pnfile.Close();
        }
    }
#endif
    pPaInfo += m_nLB_PNLen;
    CMapFilePaY pafile;
    CMapFilePiY pifile;
    SEARCH_PA_INFO painfo;
    
    if (m_nCityIDTag<=m_nCityIDMax)
    {
        LoadSearchInfo_PHONE(m_nCityIDTag);
        if (!m_nLB_PALen)
        {
            if (!pafile.Open(m_SearchBufferInfo.nMapID))
            {
                m_nCityIDTag = m_nCityIDMax+1;
                return 0;
            }
        }
    }
    
    while (m_nCityIDTag<=m_nCityIDMax && tag<NURO_SEARCH_BUFFER_MAX_POI)
    {
        if (LoadSearchInfo_PHONE(m_nCityIDTag))
        {
            nIdx = 0;
            SEARCH_PI_CITYINFO picityinfo;
            if (!m_nLB_PILen)
            {
                if (!pifile.Open(m_SearchBufferInfo.nMapID))
                {
                    m_nCityIDTag = m_nCityIDMax+1;
                    return 0;
                }
                
                if (!pifile.GetCityInfo(m_nCityIDTag, &picityinfo))
                {
                    ++m_nCityIDTag;
                    continue;
                }
                m_nLoadPoiCount = picityinfo.nPoiCount;				
            }
            else
            {
                pPoiInfo = (SEARCH_PI_POIINFO*)(pPaInfo+m_nLB_PALen);
            }
            while (nIdx<m_nLoadPoiCount && tag<NURO_SEARCH_BUFFER_MAX_POI)
            {
                if (!m_nLB_PILen)
                {
                    if (!pifile.GetPoiInfo_CEX(picityinfo.nStartPos,nIdx, &poiInfo2))
                    {
                        ++nIdx;
                        continue;
                    }
                    _pPoiInfoTag = &poiInfo2;
                }
                else //彆load善賸poi陓洘
                {
                    _pPoiInfoTag = &pPoiInfo[nIdx];
                }
                phoneLen = 0;
                if (m_nLB_PALen)
                {
                    nuMemcpy(&phoneLen, pPaInfo+_pPoiInfoTag->nOtherInfoAddr, 1);
                    if (phoneLen==(nuBYTE)-1)
                    {
                        phoneLen = 0;
                    }
                    if (phoneLen)
                    {
                        if (phoneLen>62)
                        {
                            phoneLen = 62;
                        }
                        phoneLen = phoneLen>>1;
                        nuMemcpy(phone, pPaInfo+_pPoiInfoTag->nOtherInfoAddr+1, phoneLen<<1);
                        phone[phoneLen] = 0;
                        _pPhoneTag = phone;
                    }
                }
                else
                {
                    pafile.GetPaInfo(_pPoiInfoTag->nOtherInfoAddr, &painfo);
                    phoneLen = nuWcslen(painfo.phone);
                    _pPhoneTag = painfo.phone;
                }
                if (phoneLen)
                {
                    if (nuPhoneJudge(_pPhoneTag, m_nSearchPhoneT, phoneLen, keylen))
                    {
                        m_SearchRes[tag].mapID	= m_SearchBufferInfo.nMapID;
                        m_SearchRes[tag].cityID	= _pPoiInfoTag->nCityID;
                        m_SearchRes[tag].townID = _pPoiInfoTag->nTownID;
                        m_SearchRes[tag].type	= SEARCH_BT_POI;
                        m_SearchRes[tag].ex1c	= (nuBYTE)_pPoiInfoTag->nTP1ID;
                        m_SearchRes[tag].ex2c	= (nuBYTE)_pPoiInfoTag->nTP2ID;
                        m_SearchRes[tag].extAddr= _pPoiInfoTag->nOtherInfoAddr;
                        m_SearchRes[tag].nameAddr	= _pPoiInfoTag->nNameAddr;
                        m_SearchRes[tag].x		= _pPoiInfoTag->nX;
                        m_SearchRes[tag].y		= _pPoiInfoTag->nY;
                        m_SearchRes[tag].strokes= -1;
                        m_SearchRes[tag].name[0] = 0;
                        ++tag;
                        ++res;
                    }
                }
                ++nIdx;
            }
        }
        ++m_nCityIDTag;
        if (res)
        {
            return res;
        }
    }
    return res;
}
*/
nuUINT CSearchPoi::BlkIDToIdx(nuUINT nBlkID)
{
    if (nBlkID==(nuUINT)-1)
    {
        return -1;
    }
    nuBYTE* p = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nLoadBufferIdx);
    if (!p || !m_nBHIdxLen)
    {
        return -1;
    }
    nuUINT start = 0;
    nuUINT end = (m_nBHIdxLen>>2)-1;
    nuUINT center = 0;
    nuUINT* pBHInfo = (nuUINT*)(p+m_nLB_CTLen);
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

nuBOOL CSearchPoi::FillResBuffer(nuUINT tag, SEARCH_PI_POIINFO *pPoiInfo, nuBYTE strokes)
{
    if (!pPoiInfo)
    {
        return nuFALSE;
    }
    m_SearchRes[tag].mapID	    = m_SearchBufferInfo.nMapID;
    m_SearchRes[tag].cityID	    = pPoiInfo->nCityID;
    m_SearchRes[tag].townID	    = pPoiInfo->nTownID;
    m_SearchRes[tag].ex1c	    = (nuBYTE)pPoiInfo->nTP1ID;
    m_SearchRes[tag].ex2c	    = (nuBYTE)pPoiInfo->nTP2ID;
    m_SearchRes[tag].extAddr	= pPoiInfo->nOtherInfoAddr;
    m_SearchRes[tag].nameAddr	= pPoiInfo->nNameAddr;
    m_SearchRes[tag].type	    = SEARCH_BT_POI;
    m_SearchRes[tag].x	  	    = pPoiInfo->nX;
    m_SearchRes[tag].y		    = pPoiInfo->nY;
	m_SearchRes[tag].nFindCount = 0;
    m_SearchRes[tag].strokes    = strokes;
    return nuTRUE;
}
nuUINT CSearchPoi::FindPhoneMatch()
{
    //脤梑忑帣
    m_nResTotal = 0;
    m_firstPos = -1;
    m_lastPos = -1;
    //
    nuINT cmp = 0;
    nuUINT keylen = nuWcslen(m_nSearchPhone);
    CMapFilePiY pifile;
    CMapFilePaY pafile;
    if (!pifile.Open(m_searchInfo.nMapID) 
        || !pafile.Open(m_searchInfo.nMapID) 
        ||!pifile.GetPoiNum())
    {
        return 0;
    }
	SEARCH_PI_POIINFO piInfo = {0};
	SEARCH_PA_INFO paInfo = {0};
    nuUINT start = 0;
    nuUINT end = pifile.GetPoiNum()-1;
    if (!pifile.GetPoiInfo_PPIdx(0, &piInfo))
    {
        return 0;
    }
    cmp = -1;
    if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
    {
        if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
        {
            return 0;
        }
        cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
    }
    if (cmp>0)
    {
        return 0;
    }
    else if (cmp==0)
    {
        m_firstPos = 0;
    }
    if (pifile.GetPoiNum()>1)
    {
        if (!pifile.GetPoiInfo_PPIdx(end, &piInfo))
        {
            return 0;
        }
        cmp = -1;
        if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
        {
            if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
            {
                return 0;
            }
            cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
        }
        if (cmp<0)
        {
            return 0;
        }
        else if (cmp==0)
        {
            m_lastPos = end;
        }
    }
    if (m_firstPos==(nuUINT)-1)
    {
        nuUINT center = 0;
        while (start<end)
        {
            center = (start+end)>>1;
            if (!pifile.GetPoiInfo_PPIdx(center, &piInfo))
            {
                return 0;
            }
            cmp = -1;
            if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
            {
                if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
                {
                    return 0;
                }
                cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
            }
            if (cmp>=0)
            {
                end = center-1;
            }
            else
            {
                start = center+1;
            }
        }
        if (!pifile.GetPoiInfo_PPIdx(start, &piInfo))
        {
            return 0;
        }
        cmp = -1;
        if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
        {
            if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
            {
                return 0;
            }
            cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
        }
        m_firstPos = start;
        if (cmp<0)
        {
            ++m_firstPos;
        }
    }
    if (m_lastPos==(nuUINT)-1)
    {
        start = m_firstPos;
        end = NURO_MIN(pifile.GetPoiNum()-1, start+NURO_SEARCH_BUFFER_MAX_POI-1);
        nuUINT center = 0;
        while (start<end)
        {
            center = (start+end)>>1;
            if (!pifile.GetPoiInfo_PPIdx(center, &piInfo))
            {
                return 0;
            }
            cmp = -1;
            if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
            {
                if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
                {
                    return 0;
                }
                cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
            }
            if (cmp>0)
            {
                end = center-1;
            }
            else
            {
                start = center+1;
            }
        }
        if (!pifile.GetPoiInfo_PPIdx(start, &piInfo))
        {
            return 0;
        }
        cmp = -1;
        if (piInfo.nOtherInfoAddr!=(nuUINT)-1)
        {
            if (!pafile.GetPaInfo(piInfo.nOtherInfoAddr, &paInfo))
            {
                return 0;
            }
            cmp = nuPhoneCompare(paInfo.phone, m_nSearchPhone, nuWcslen(paInfo.phone), keylen);
        }
        m_lastPos = start;
        if (cmp>0)
        {
            --m_lastPos;
        }
    }
    m_nResTotal = m_lastPos - m_firstPos + 1;
    if (m_nResTotal>NURO_SEARCH_BUFFER_MAX_POI)
    {
        m_nResTotal = NURO_SEARCH_BUFFER_MAX_POI;
        m_lastPos = m_firstPos + m_nResTotal - 1;
    }
    return m_nResTotal;
}
nuUINT CSearchPoi::GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords)
{
    nuUINT nRes = 0;
    nuUINT nBufferTag = 0;
    nuWORD nAddWord = 0;
    nuWORD i = 0;
    nuWORD *pTagWord = NULL;
    nuBYTE idx = 0;
    while (nBufferTag<NURO_SEARCH_BUFFER_MAX_POI && m_SearchRes[nBufferTag].type)
    {
        if (nRes==nMaxWords)
        {
            break;
        }
        nAddWord = 0;
        idx = 0;
        if (pHeadWords)
        {
            while (m_SearchRes[nBufferTag].name[idx])
            {
                if (pHeadWords[idx])
                {
                    if (m_SearchRes[nBufferTag].name[idx]==pHeadWords[idx])
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
                    nAddWord = m_SearchRes[nBufferTag].name[idx];
                    break;
                }
            }
        }
        else
        {
            nAddWord = m_SearchRes[nBufferTag].name[0];
        }
        if (nAddWord == _SEARCH_ROAD_FJUDGE_ENDWORD_
            && m_SearchRes[nBufferTag].type == SEARCH_BT_ROAD )
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
nuVOID CSearchPoi::ZhuYinForSmart_SecondWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount, const nuUINT SelectWord, const nuBOOL ReSort)
{												
	nuINT	iTotalNextWordCount=0;				//次字的個數
	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;			//暫存次字陣列
	NURO_SEARCH_NORMALRES *pNORMALRES =(NURO_SEARCH_NORMALRES*)pPoiBuf;//暫存Poi名稱陣列

	nuINT i;
	for (i=0; i<PoiBufLen; i++)
	{
		if (0 == i)															//第一個字放入 m_pWCHAR[0]
		{
			pWord[n_iCount] = pNORMALRES[i].name[SelectWord];
			n_iCount++;
		}
		else if (pNORMALRES[i].name[SelectWord] != pWord[n_iCount-1]) 	//當該字不同與次字陣列[m_iCount-1]的字時
		{
			nuBOOL bisAdd = nuTRUE;
			nuINT j;
			for (j=0; j<n_iCount; j++)                                      //比對字串中前N個字(iCmpIdx) 找出未加入的字 
			{
				if (pNORMALRES[i].name[SelectWord] == pWord[j]) 
				{
					bisAdd = nuFALSE;											//有該字元 則加入的BOOL=FALSE
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

nuBOOL CSearchPoi::ZhuYinForSmart_OtherWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	nuBOOL  LastWord=nuTRUE;			//判斷是否為最後一個字
	nuINT n=candylen;				//KeyWordy字數

	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;
	NURO_SEARCH_NORMALRES *n_pNORMALRES =(NURO_SEARCH_NORMALRES*)pPoiBuf;//暫存Poi名稱陣列

	nuINT i;
	for (i=0; i<PoiBufLen; i++)
	{
		nuWCHAR* wOUTPUTWord = n_pNORMALRES[i].name;		
		if(0 != nuWcsncmp((nuWCHAR*)wKeyWord,wOUTPUTWord, n))		//與KeyWord比較 若不相等 就與Poi名稱陣列的下一組DATA比對
		{
			continue;
		}
		if(0 != n_pNORMALRES[i].name[n] )					//不為最後一個字 	LastWord = nuFALSE;
		{
			LastWord = nuFALSE;
		}
		if(0 == n_pNORMALRES[i].name[n] )					//最後一個字
		{
			continue;
		}
		if (0 == i||0 == n_iCount)								
		{
			pWord[n_iCount] = n_pNORMALRES[i].name[n];			//第一個字放入 pWord[0]
			n_iCount++;
		}
		else if (n_pNORMALRES[i].name[n] != pWord[n_iCount-1])	//當該字不同與次字陣列[n_iCount-1]的字時
		{
			nuBOOL bisAdd = nuTRUE;
			nuINT j;
			for (j=0; j<n_iCount; j++)
			{
				if (n_pNORMALRES[i].name[n] == pWord[j])			//有該字元 則加入的BOOL=FALSE
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
	return LastWord;//判斷是否為最後一個字
}
nuVOID CSearchPoi::Stroke_GetNextWordData(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	nuINT	len=(nuINT)candylen;		//KeyWord長度
	nuINT	iCmpIdx=candylen;			//KeyWordy字數
	nuINT	iTotalNextWordCount=0;		//次字的數量
	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;
	NURO_SEARCH_NORMALRES *pNORMALRES =(NURO_SEARCH_NORMALRES*)pPoiBuf;//暫存Poi名稱陣列

	nuINT i;
	for ( i=0; i<PoiBufLen; i++)
	{
		if (iCmpIdx < (nuINT)nuWcslen(pNORMALRES[i].name))
		{
			if (0 == nuWcsncmp(pNORMALRES[i].name, (nuWCHAR *)wKeyWord, iCmpIdx))	//比對字串中前N個字(iCmpIdx) 找出符合的 進行下述判斷
			{
				nuBOOL bNeedAdd = nuTRUE;	
				nuINT  j=0;
				for (j=0; j<(nuINT)nuWcslen(pWord); j++)				//比對次字字串中的DATA 若沒有此字再加入次字陣列中
				{
					if (pWord[j] == pNORMALRES[i].name[iCmpIdx])		
					{
						bNeedAdd = nuFALSE;									//有該字元 則加入的BOOL=FALSE
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
	WordCont=iTotalNextWordCount;//次字的個數
}
nuBOOL CSearchPoi::SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging /*= nuFALSE*/)
{
    nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
    if (!pKeyWord)
    {
        return nuFALSE;
    }
    m_nPzyWord = 0;
	m_bIsFromBeging = bIsFromBeging;
    nuMemset(m_nSearchPoiKeyDanyin, 0, sizeof(m_nSearchPoiKeyDanyin));
    nuWcsncpy(m_nSearchPoiKeyName, pKeyWord, (EACH_POI_NAME_LEN>>1) -1);
    return nuTRUE;
}
nuBOOL CSearchPoi:: Sort_KEY_index(nuWORD nCode) //PROSPER 2011.1207
{
	nuINT n=-1;
	nuBOOL bSame=nuFALSE;
	if(nKeyCount == (sizeof(KEY_Data)/sizeof(nuWCHAR)))
	{
		return nuFALSE;
	}
	if(nKeyCount==0)
	{
		KEY_Data[nKeyCount]=nCode;
		nKeyCount++;
		return nuFALSE;
	}
	while(n++ < nKeyCount)
	{
		if(nCode == KEY_Data[n])
		{
            bSame = nuTRUE;		  
			break;
		}
	}
	if(!bSame)
	{
		//if(nKeyCount==(KEY_Data/sizeof(nuWCHAR)))
		{
		//	return nuFALSE; // Over
		}
	    KEY_Data[nKeyCount]=nCode;
		nKeyCount++;
	}
	return nuTRUE;

}
nuBOOL CSearchPoi:: GetPoiSKB(nuVOID* pSKBBuf, nuUINT& nCount) //Prosper 2011 12.07
{

	nuMemcpy(pSKBBuf,KEY_Data,sizeof(KEY_Data));
	nCount=nKeyCount;
	return nuTRUE;
}
nuVOID CSearchPoi::SortByStroke(nuINT nTotal)
{
	nuINT n = nTotal;


	if (n == 0 || nuTRUE) //?????Ƨ?
	{
		return;// nuFALSE;
	}

	else
	{
#define  FILE_NAME_SORT			nuTEXT("UIdata\\Sort.bin")
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_SORT);
		nuFILE *TempFile;

		TempFile = nuTfopen(tsFile, NURO_FS_RB);
		if( TempFile == NULL )
		{
			return; //nuFALSE;
		}

		nuFseek(TempFile,0,NURO_SEEK_END);
		nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
		nSortLimt=f_size;
		data=new Trans_DATA[f_size];
		nuFseek(TempFile,0,NURO_SEEK_SET);
		nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);


		nuFclose(TempFile);
		TempFile=NULL;
	}
	//nuWCHAR *temp=new nuWCHAR[NURO_SEARCH_BUFFER_MAX>>1];
	//nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
	SEARCH_BUFFER temp = {0};

	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	nuINT nTemp =0 ;
	for(nIdx2=0;nIdx2<n-1;nIdx2++)
		for(nIdx=0;nIdx<n-1;nIdx++)

		{
			nuINT nLen  = nuWcslen(m_SearchRes[nIdx].name);
			nuINT nLen2 = nuWcslen(m_SearchRes[nIdx+1].name);


			if(nLen > nLen2 )
			{
				//nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
				temp				=	m_SearchRes[nIdx];
				m_SearchRes[nIdx]	=	m_SearchRes[nIdx+1];
				m_SearchRes[nIdx+1]  =   temp;

			
			}			
		}
		/*nuINT k=0;
		for(nuINT j=0;j<n;j++)
		{
			if(nuWcslen(m_SearchRes[j].name)<=4)
			int l=0;
		}*/
		while(k-->0)
		{
			//CHECK NWORDS:5
			//CONDITION1: 
			//CHECK WORDS_CODES (BIG5)

			//CONDITION2: 
			//K==0&&parser==parser2 
			//CHECK WORDS_LEN

			for(nIdx2=0;nIdx2<n-1;nIdx2++)
				for(nIdx=0;nIdx<n-1;nIdx++)
				{
					parser=parser2=0;
					nuINT nLen  = nuWcslen(m_SearchRes[nIdx].name);
					nuINT nLen2 = nuWcslen(m_SearchRes[nIdx+1].name);
					if(nLen < nLen2)
					{
						continue;
					}
					if(nLen>=k && nLen2 >=k )
					{
						parser  = TransCode(m_SearchRes[nIdx].name[k]);
						parser2 = TransCode(m_SearchRes[nIdx+1].name[k]);
					}
					if(parser > parser2)  
					{
						//nuINT nLen  = nuWcslen(m_SearchRes[nIdx].name);
						//nuINT nLen2 = nuWcslen(m_SearchRes[nIdx+1].name);
							//	nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
							temp				=	m_SearchRes[nIdx];
							m_SearchRes[nIdx]	=	m_SearchRes[nIdx+1];
							m_SearchRes[nIdx+1]  =   temp;

					}

				}
		}



		if(data!=NULL)
		{
			delete[] data;
			data = NULL;
		}
		return ;//nuTRUE; 
}
nuINT	CSearchPoi::TransCode(nuINT code)
{	
	//??�??ARRAY unicode ??BIG5
	//Trans_DATA *test=new Trans_DATA[2]({123,456},{156,456});
	//test[0]=(0x456,0x123);

	//nuINT *try1=new nuINT[0](10);

	nuINT idx=-1;
	nuINT nLimit=nSortLimt;
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	

	low	  = 0;
	upper = nLimit-1;

	//?Y???諸???Ƥ??blow~upper?d?? ?N???Τ??? ?????^??-1
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
nuBOOL CSearchPoi:: GetNEXTWORD(nuVOID* pKEYBuf, nuUINT& nCount) //Prosper 2011 12.07
{

	nuMemcpy(pKEYBuf,m_wsNEXT_WORD_Data,sizeof(m_wsNEXT_WORD_Data));
	nCount = nNEXT_WORD_COUNT;
	return nuTRUE;
}
nuINT CSearchPoi::CheckWordIsNumber(nuWORD WordCode)
{
	if(WordCode>=65&&WordCode<=90)  //Sort English Words , upper case
	{
		return WordCode-65+26+21;
	}
	if(WordCode>=97&&WordCode<=122) //Sort English Words , upper case
	{
		return WordCode-97+21;
	}
	nuINT nIdx = 999;
	switch(WordCode)
	{
	case 48:	nIdx=0;  break;
	case 49:	nIdx=1;  break;
	case 50:	nIdx=2;  break;
	case 51:	nIdx=3;  break;
	case 52:	nIdx=4;  break;	 
	case 53:	nIdx=5;  break;	 
	case 54:	nIdx=6;  break;	 
	case 55:	nIdx=7;  break;	 
	case 56:	nIdx=8;  break;	 
	case 57:	nIdx=9;  break;	 

	case 19968: nIdx=11; break;	 
	case 20108: nIdx=12; break;	 
	case 19977: nIdx=13; break;	 
	case 22235: nIdx=14; break;	 
	case 20116: nIdx=15; break;	 
	case 20845: nIdx=16; break;	 
	case 19971: nIdx=17; break;	 
	case 20843: nIdx=18; break;	 
	case 20061: nIdx=19; break;	 
	case 21313: nIdx=20; break;
	default   :          break;

	}
	return nIdx;
}
