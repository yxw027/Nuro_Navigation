// SearchPYz.cpp: implementation of the CSearchPYz class.
//
//////////////////////////////////////////////////////////////////////
#include "SearchPYz.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PY_SEARCH_MODE_NO					0x00//错误的查询
#define PY_SEARCH_MODE_RESTART				0x01//重新A区从0位置开始搜索
#define PY_SEARCH_MODE_FORWARD				0x02//单音字符增加，向前搜索
#define PY_SEARCH_MODE_BACKWARD				0x03//单音字符减少，需要向前搜索
#define PY_SEARCH_MODE_KEEP					0x04//单音字符没有变化，不需要搜索


CSearchPYz::CSearchPYz()
{
	m_nSHMemIdx = (nuWORD)-1;
}

CSearchPYz::~CSearchPYz()
{

}

nuBOOL CSearchPYz::Initialize()
{
	m_nSHMemIdx = (nuWORD)-1;
	if( !m_filePy.Initialize() )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CSearchPYz::Free()
{
	
}

nuUINT CSearchPYz::SetSHMode(nuUINT nMode)
{
	m_searchSet.nMode = nMode;
	m_filePy.SetFileMode(m_searchSet.nMode);
	return 1;
}

nuUINT CSearchPYz::SetSHRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( nSetMode == SEARCH_DY_SET_MAPIDX )
	{
		m_searchSet.nMapIdx = nuLOWORD(nData);
		if( !m_filePy.SetMapIdx(m_searchSet.nMapIdx) )
		{
			return 0;
		}
		
		PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMemMass(sizeof(PYSEARCHDATA), &m_nSHMemIdx);
		}
		if( pShData == NULL )
		{
			return 0;
		}
		pShData->nTotalFound	= 0;
		pShData->nFoundDataMode	= PY_FOUNDDATA_MODE_DEFAULT;
		pShData->nKeptCount		= 0;
		pShData->nCityID		= (nuWORD)(-1);
		pShData->nTownID		= (nuWORD)(-1);
		pShData->nCountOfCity	= 0;
		pShData->nCountOfTown	= 0;
		
		m_bReSearchPinyin	= nuTRUE;
		nuMemset(m_pAStartIdx, 0, sizeof(m_pAStartIdx));
		m_nASIdxCount = 0;
		nuMemset(m_sName, 0, sizeof(m_sName));
		m_nNameNum	= 0;
		nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	}
	else if( nSetMode == SEARCH_DY_SET_CITYIDX )
	{
		PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			return 0;
		}
		if( pShData->nCityID == nData )
		{
			return 1;
		}
		pShData->nCityID = (nuWORD)nData;
		if( nData == (nuDWORD)(-1) )
		{
			pShData->nTownID = -1;
		}
		else
		{
			SearchPY_City();
		}
		m_bReSearchPinyin	= nuTRUE;
	}
	else if( nSetMode == SEARCH_DY_SET_TOWNIDX )
	{
		PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL || pShData->nTownID == (nuWORD)nData )
		{
			return 0;
		}
		if( pShData->nTownID == (nuWORD)nData )
		{
			return 1;
		}
		pShData->nTownID = (nuWORD)nData;
		if( nData != (nuDWORD)(-1) )
		{
			SearchPY_Town();
		}
		m_bReSearchPinyin	= nuTRUE;
	}
	return 1;
}

nuUINT CSearchPYz::SetSHString(const nuCHAR *pBuff, nuBYTE nBufLen)
{
	if( pBuff == NULL || nBufLen == NULL )
	{
		return 0;
	}
	nuBYTE i = 0;
	nuBYTE nCharWide = 1;
	nuWORD nBuffNum = nBufLen;
	if( m_searchSet.nMode & SH_PY_MODE_STR_B )
	{
		for(i = 0; i < m_nNameNum && i < nBufLen; ++i)
		{
			if( m_sName[i] != pBuff[i] )
			{
				break;
			}
		}
	}
	else if( m_searchSet.nMode & SH_PY_MODE_STR_C )
	{
		nCharWide = sizeof(nuWCHAR);
		nBuffNum  = nBufLen / sizeof(nuWCHAR);
		nuWCHAR* wsName = (nuWCHAR*)m_sName;
		nuWCHAR* wsBuff = (nuWCHAR*)pBuff;
		for(i = 0; i < m_nNameNum && i < nBuffNum; ++i)
		{
			if( wsName[i] != wsBuff[i] )
			{
				break;
			}
		}
	}
	else
	{
		return 0;
	}
	nuBYTE nSHMode;// = PY_SEARCH_MODE_RESTART;
	if( m_nNameNum == i && i == nBuffNum )//和上次一样，不用搜索
	{
		nSHMode = PY_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//第一个字符就不一样，重新搜索
	{
		nSHMode = PY_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else
	{
		if( i == m_nNameNum )
		{
			nSHMode = PY_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = PY_SEARCH_MODE_BACKWARD;//DY_SEARCH_MODE_RESTART;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//向后搜索，需要重设搜索的起点
		}
	}
	if( nBuffNum > PY_MAX_STR_COUNT )
	{
		nBufLen = PY_MAX_STR_COUNT * nCharWide;
		nBuffNum = PY_MAX_STR_COUNT;
	}
	m_nNameNum = nBuffNum;
	nuMemcpy(m_sName, pBuff, nBufLen);
	if( m_searchSet.nMode & SH_PY_MODE_STR_B )
	{
		if( !SearchPY_B(nSHMode) )
		{
			return 0;
		}
	}
	else if( m_searchSet.nMode & SH_PY_MODE_STR_C )
	{
		if( !SearchPY_C(nSHMode) )
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

nuBOOL CSearchPYz::SearchPY_B(nuBYTE nShMode)
{
	if( nShMode == PY_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	nuBYTE *pLength = (nuBYTE*)m_filePy.GetPyALength(&nTotalCount);
	nuCHAR *pPinyin = (nuCHAR*)m_filePy.GetPyBorCString(NULL);
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pLength == NULL || pPinyin == NULL || pShData == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;//20090109.
	nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	if( nShMode == PY_SEARCH_MODE_RESTART || nShMode == PY_SEARCH_MODE_BACKWARD )//重新从B区找
	{
		nuDWORD nAIdxStart;
		if( m_nASIdxCount == 0 )
		{
			nAIdxStart = 0;
		}
		else
		{
			nAIdxStart = m_pAStartIdx[m_nASIdxCount-1];
		}
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount	 = 0;
		pShData->nFoundDataMode	= PY_FOUNDDATA_MODE_DEFAULT;
		for(nuDWORD i = nAIdxStart ; i < nTotalCount; ++i)
		{
			SearchPY_B_STRCMP(pPinyin, pLength[i], i, pShData);
			pPinyin += pLength[i];
		}
	}
	else
	{
		//SEARCH in the record list
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx =  pShData->pIndexList[ nOldKeptCount - 1];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode	= PY_FOUNDDATA_MODE_DEFAULT;
		nuDWORD idx = 0;
		for(i = 0 ; i < nOldKeptCount; ++i)
		{
			while( idx < pShData->pIndexList[i] )
			{
				pPinyin += pLength[idx];
				++idx;
			}
			SearchPY_B_STRCMP(pPinyin, pLength[idx], idx, pShData);
		}
		//Search the following strings that hasn't been recorded.
		if( nFoundDataMode == PY_FOUNDDATA_MODE_FIND_PART && idx < nTotalCount )
		{
			pPinyin += pLength[idx];
			++idx;
			while( idx < nTotalCount )
			{
				SearchPY_B_STRCMP(pPinyin, pLength[idx], idx, pShData);
				pPinyin += pLength[idx];
				++idx;
			}
		}
	}
	m_bReSearchPinyin = nuTRUE;
	return nuTRUE;
}

nuBOOL CSearchPYz::SearchPY_C(nuBYTE nShMode)
{
	return nuTRUE;
}

nuVOID CSearchPYz::SearchPY_B_STRCMP(nuCHAR *pstr, nuBYTE nstrlen, nuDWORD idx, PPYSEARCHDATA pShData)
{
	nuBOOL bHaveKept = nuFALSE;
	for(nuBYTE j = 0; j < nstrlen; ++j)
	{
		if( pstr[j] == NULL || nstrlen - j < m_nNameNum )
		{
			break;
		}
		//
		if( pstr[j] == m_sName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum ; ++k )
			{
				if( pstr[k+j] != m_sName[k] )
				{
					break;
				}
			}
			while( m_nASIdxCount < k && m_nASIdxCount < PY_MAX_STR_COUNT )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}
			if( k == m_nNameNum )//找到一个相符合的
			{
				if( k+j < nstrlen /*&& pWsDY[k+j] != NULL*/ )
				{
					AddKeyMask(pstr[k+j]);
				}
				//
				if( !bHaveKept )
				{
					++pShData->nTotalFound;
					if( pShData->nKeptCount < PY_MAX_KEPT_RECORD )
					{
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						pShData->nFoundDataMode	= PY_FOUNDDATA_MODE_FIND_ALL;
					}
					else
					{
						pShData->nFoundDataMode	= PY_FOUNDDATA_MODE_FIND_PART;
					}
					bHaveKept = nuTRUE;
				}
				j += (k - 1);
			}
		}
	}
}

nuBOOL CSearchPYz::SearchPY_City()
{
	nuDWORD nCount = 0;
	PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(&nCount);
	if( pPyD == NULL )
	{
		return nuFALSE;
	}//不能放到取pShData之下，因为这里面会分配内存。
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//注意这里不能和上面的换位置
	pShData->nCountOfCity = 0;
	for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
	{
		if( pPyD[pShData->pIndexList[i]].nCityID == pShData->nCityID )
		{
			pShData->pCityIdxList[ pShData->nCountOfCity ] = pShData->pIndexList[i];
			++pShData->nCountOfCity;
			if( pShData->nCountOfCity >= PY_MAX_CITY_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CSearchPYz::SearchPY_Town()
{
	nuDWORD nCount = 0;
	PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(&nCount);
	if( pPyD == NULL )
	{
		return nuFALSE;
	}//不能放到取pShData之下，因为这里面会分配内存。
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//注意这里不能和上面的换位置
	pShData->nCountOfTown = 0;
	for(nuDWORD i = 0; i < pShData->nCountOfCity; ++i)
	{
		if( pPyD[ pShData->pCityIdxList[i] ].nTownID == pShData->nTownID )
		{
			pShData->pTownIdxList[ pShData->nCountOfTown ] = pShData->pCityIdxList[i];
			++pShData->nCountOfTown;
			if( pShData->nCountOfTown >= PY_MAX_TOWN_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuUINT CSearchPYz::GetPyDataHead(PSH_HEADER_FORUI pShHeader)
{
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShHeader == NULL || pShData == NULL )
	{
		return 0;
	}
	else
	{
		pShHeader->nTotalCount = pShData->nTotalFound;
		pShHeader->pDyKeyMask	= &m_keyMask;
		return 1;
	}
}

nuUINT CSearchPYz::GetPyDataPage(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx)
{
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pPageData == NULL || pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD	*pIdxList=NULL;
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
	}
	else
	{
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
	}
	//
	if( nItemIdx >= nCount )
	{
		return 0;
	}
	//
	if( pPageData->nPageItemCount > DY_NAME_FORUI_MAX_COUNT )
	{
		pPageData->nPageItemCount = DY_NAME_FORUI_MAX_COUNT;
	}
	nuWORD nCountRead = nuWORD(nCount - nItemIdx);
	if( nCountRead > pPageData->nPageItemCount )
	{
		nCountRead = pPageData->nPageItemCount;
	}
	pPageData->nThisPageCount = 0;
	for(nuDWORD i = nItemIdx; i < nCountRead + nItemIdx; ++i)
	{
		if( !m_filePy.GetPyOneData(&pPageData->dyNodeList[pPageData->nThisPageCount], pIdxList[i]) )
		{
			break;
		}
		pPageData->pNodeIdxList[pPageData->nThisPageCount] = pIdxList[i];
		++pPageData->nThisPageCount;
	}
	pPageData->nNowPageItemIdx = (nuWORD)nItemIdx;
	//
	pPageData->nTotalCount = nCount;
	pPageData->nIdxSelected= 0;
	if( m_bReSearchPinyin )
	{
		SearchPY_Pinyin(pIdxList, nCount);
		m_bReSearchPinyin = nuFALSE;
	}
	pPageData->nMinStroke = pShData->nMinStroke;
	pPageData->nMaxStroke = pShData->nMaxStroke;
	return 1;
	
}

nuVOID CSearchPYz::SearchPY_Pinyin(nuDWORD *pIdxList, int nCount)
{
	PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(NULL);
	if( pPyD == NULL )
	{
		return ;
	}
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData != NULL )
	{
		pShData->nMinStroke = pShData->nMaxStroke = pPyD[ pIdxList[0] ].nPinYin;
		for(int i = 1; i < nCount; ++i)
		{
			if( pPyD[ pIdxList[i] ].nPinYin < pShData->nMinStroke )
			{
				pShData->nMinStroke = pPyD[ pIdxList[i] ].nPinYin;
			}
			else if( pPyD[ pIdxList[i] ].nPinYin > pShData->nMaxStroke )
			{
				pShData->nMaxStroke = pPyD[ pIdxList[i] ].nPinYin;
			}	
		}
	}
}

nuUINT CSearchPYz::GetPyDataPinyin(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin)
{
	if( pPageData == NULL )
	{
		return 0;
	}
	PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(NULL);
	if( pPyD == NULL )
	{
		return 0;
	}
	PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD *pIdxList=NULL;
	nuINT nCount = 0;
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
	}
	else
	{
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
	}
	//
	nuINT i = 0;
	if( pPageData->nIdxSelected >= pPageData->nThisPageCount )
	{
		pPageData->nIdxSelected = 0;
	}
	//
	if( nPinyin == pPageData->dyNodeList[pPageData->nIdxSelected].nStroke )
	{
		return 1;
	}//The character hasn't been changed.
	else if( nPinyin > pPageData->dyNodeList[pPageData->nIdxSelected].nStroke)
	{
		nuBOOL bFind = nuFALSE;
		for(i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pPageData->pNodeIdxList[ pPageData->nIdxSelected ] )
			{
				break;
			}
		}
		for(;i < nCount; ++i)
		{
			if( pPyD[ pIdxList[i] ].nPinYin >= nPinyin )
			{
				bFind = nuTRUE;
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}
	}//pinyin
	else
	{
		nuBOOL bFind = nuFALSE;
		for(i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pPageData->pNodeIdxList[ pPageData->nIdxSelected ] )
			{
				break;
			}
		}
		
		for(; i >= 0; --i)
		{
			if( pPyD[ pIdxList[i] ].nPinYin <= nPinyin )
			{
				bFind = nuTRUE;
				nPinyin = pPyD[ pIdxList[i] ].nPinYin;
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}//没有更小的笔划数了
		for(; i > 0; --i)
		{
			if( pPyD[ pIdxList[i - 1] ].nPinYin != nPinyin )
			{
				break;
			}
		}
	}//笔划减少
	nuWORD nNewPageItemIdx = (nuWORD)((i/pPageData->nPageItemCount) * pPageData->nPageItemCount);
	if( pPageData->nNowPageItemIdx != nNewPageItemIdx )
	{
		pPageData->nNowPageItemIdx = nNewPageItemIdx;
		nuWORD nCountRead = nuWORD(nCount - nNewPageItemIdx);
		if( nCountRead > pPageData->nPageItemCount )
		{
			nCountRead = pPageData->nPageItemCount;
		}
		pPageData->nThisPageCount = 0;
		for(nuWORD j = nNewPageItemIdx; j < nCountRead + nNewPageItemIdx; ++j)
		{
			if( !m_filePy.GetPyOneData(&pPageData->dyNodeList[pPageData->nThisPageCount], pIdxList[j]) )
			{
				break;
			}
			pPageData->pNodeIdxList[pPageData->nThisPageCount] = pIdxList[j];
			++pPageData->nThisPageCount;
		}
	}
	pPageData->nIdxSelected	= (nuBYTE)(i % pPageData->nPageItemCount);
	pPageData->nTotalCount	= nCount;
	return 1;
}

nuUINT CSearchPYz::GetPyOneDetail(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx)
{
	if( m_filePy.GetPyOneDetail(pOneDetail, nIdx) )
	{
		return 1;
	}
	return 0;
}

nuUINT CSearchPYz::GetPyCityTown(PSH_CITYTOWN_FORUI pCityTown)
{
	if( pCityTown == NULL )
	{
		return 0;
	}
	pCityTown->nTotalCount = 0;
	if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_ALL )
	{
		nuDWORD nCityCount = m_filePy.GetCityCount();
		while( pCityTown->nTotalCount < SH_NAME_CITYTOWN_MAX_COUNT && pCityTown->nTotalCount < nCityCount )
		{
			pCityTown->pIdList[pCityTown->nTotalCount] = pCityTown->nTotalCount;
			++pCityTown->nTotalCount;
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_USED )
	{
		PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(NULL);
		if( pPyD == NULL )
		{
			return 0;
		}
		PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			return 0;
		}
		nuDWORD i = 0;
		nuWORD nStartID = pCityTown->nIDStart;
		nuWORD nLastID = 0;
		for( i = 0; i < pShData->nKeptCount; ++i )
		{
			InsertCityTownID(pCityTown, pPyD[ pShData->pIndexList[i] ].nCityID);
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_ALL )
	{
		nuDWORD nCount = 0;
		PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(&nCount);
		if( pPyD == NULL )
		{
			return 0;
		}
		for(nuDWORD i = 0; i < nCount; ++i )
		{
			if( pCityTown->nCityID == pPyD[i].nCityID )
			{
				InsertCityTownID(pCityTown, pPyD[i].nTownID);
			}
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_USED )
	{
		PPDY_D_NODE pPyD = (PPDY_D_NODE)m_filePy.GetPyDData(NULL);
		if( pPyD == NULL )
		{
			return 0;
		}
		PPYSEARCHDATA pShData = (PPYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			return 0;
		}
		if( pShData->nCityID == (nuWORD)(-1) )
		{
			return 0;
		}
		nuDWORD i = 0;
		nuWORD nStartID = pCityTown->nIDStart;
		nuWORD nLastID = 0;
		for( i = 0; i < pShData->nCountOfCity; ++i )
		{
			InsertCityTownID(pCityTown, pPyD[ pShData->pCityIdxList[i] ].nTownID);
		}
	}
	pCityTown->nNowCount = pCityTown->nTotalCount;
	if( m_filePy.GetPyCityTown(pCityTown) )
	{
		return 1;
	}
	return 0;
}

nuBYTE CSearchPYz::InsertCityTownID(PSH_CITYTOWN_FORUI pCityTown, nuWORD nID)
{
	if( pCityTown->nTotalCount == SH_NAME_CITYTOWN_MAX_COUNT )
	{
		return PY_ID_INSERT_FAILURE;
	}
	nuINT j;
	for(j = 0; j < pCityTown->nTotalCount; ++j)
	{
		if( nID == pCityTown->pIdList[j] )
		{
			return PY_ID_INSERT_SAME;
		}
		else if( nID < pCityTown->pIdList[j] )
		{
			break;
		}
	}
	++pCityTown->nTotalCount;
	for(nuINT k = pCityTown->nTotalCount - 1; k > j; --k)
	{
		pCityTown->pIdList[k] = pCityTown->pIdList[k-1];
	}
	pCityTown->pIdList[j] = nID;
	return PY_ID_INSERT_SUCCESS;
}
