// SearchDanyinZ.cpp: implementation of the CSearchDanyinZ class.
//
//////////////////////////////////////////////////////////////////////
#include "SearchDanyinZ.h"
#include "NuroClasses.h"

//#include "windows.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DY_SEARCH_MODE_NO					0x00//渣昫腔脤戙
#define DY_SEARCH_MODE_RESTART				0x01//笭陔A植0弇离羲宎刲坰
#define DY_SEARCH_MODE_FORWARD				0x02//等秞趼睫崝樓ㄛ砃刲坰
#define DY_SEARCH_MODE_BACKWARD				0x03//等秞趼睫熬屾ㄛ剒猁砃刲坰
#define DY_SEARCH_MODE_KEEP					0x04//等秞趼睫羶衄曹趙ㄛ祥剒猁刲坰

CSearchDanyinZ::CSearchDanyinZ()
{

	
    //wsListName = NULL;
/*#ifndef VALUE_EMGRT */
	m_bNameSort = nuTRUE;
	m_bSort = nuFALSE;
	m_bSortCity = nuFALSE;
// #else
// 	m_bNameSort = nuFALSE;
// #endif

	#ifdef ZENRIN
	nSameNameIdx = 0;
	wsTempName = NULL;
	nSameCity  = NULL;
	nSameTown  = NULL;
	nSameNameIdx = 0;	
	#endif
	m_nwsListNameMemIdx = -1;
}

CSearchDanyinZ::~CSearchDanyinZ()
{
	if(m_nwsListNameMemIdx != (nuWORD)(-1) )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nwsListNameMemIdx);
	}
	Free();
}

nuBOOL CSearchDanyinZ::Initialize()
{
	InitSearchSet();
	if( !m_fileDY.Initialize() )
	{
		return nuFALSE;
	}
	m_nShDataMemIdx = -1;
	m_bReSearchStroke = nuTRUE;
	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_nSearchPoiDanyinName,0,sizeof(m_nSearchPoiDanyinName));
	return nuTRUE;
}

nuVOID CSearchDanyinZ::Free()
{
	if( m_nShDataMemIdx != -1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nShDataMemIdx);
	}	
	m_fileDY.Free();
	/*if(wsListName)
	{
		delete[] wsListName;
		wsListName = NULL;
	}*/
	#ifdef ZENRIN
	if(wsTempName)
	{
		delete[] wsTempName;
		wsTempName = NULL; 
	} 
	if(nSameCity)
	{
		delete[] nSameCity;
		nSameCity = NULL;
	}
	if(nSameTown)
	{
	   delete[] nSameTown;
	   nSameTown= NULL;
	}
	#endif
}

nuUINT CSearchDanyinZ::SetSearchRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( nSetMode == SEARCH_DY_SET_MAPIDX )
	{
		m_searchSet.nMapIdx = nuLOWORD(nData);
		if( !m_fileDY.SetMapID(m_searchSet.nMapIdx) )
		{
			return 0;
		}
		PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
		if( pShData == NULL )
		{
			pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMemMass(sizeof(DYSEARCHDATA), &m_nShDataMemIdx);
		}
		if( pShData == NULL )
		{
			return 0;
		}
		pShData->nTotalFound	= 0;
		pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_DEFAULT;
		pShData->nKeptCount		= 0;
		pShData->nCityID		= (nuWORD)(-1);
		pShData->nTownID		= (nuWORD)(-1);
		pShData->nCountOfCity	= 0;
		pShData->nCountOfTown	= 0;
		
		m_nASIdxCount = 0;
		m_nNameNum	= 0;
		m_bReSearchStroke	= nuTRUE;
		nuMemset(m_pAStartIdx, 0, sizeof(m_pAStartIdx));
		nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
		//		SearchDY_MapStart();
	}
	else if( nSetMode == SEARCH_DY_SET_CITYIDX )
	{
		PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
		if( pShData == NULL || pShData->nCityID == (nuWORD)nData )
		{
			return 0;
		}
		pShData->nCityID = (nuWORD)nData;
		if( nData == (nuDWORD)(-1) )
		{
			pShData->nTownID = -1;
		}
		else
		{
			SearchDY_City();
		}
		m_bReSearchStroke	= nuTRUE;
	}
	else if( nSetMode == SEARCH_DY_SET_TOWNIDX )
	{
		PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
		if( pShData == NULL || pShData->nTownID == (nuWORD)nData )
		{
			return 0;
		}
		pShData->nTownID = (nuWORD)nData;
		if( nData != (nuDWORD)(-1) )
		{
			SearchDY_Town();
		}
		m_bReSearchStroke	= nuTRUE;
	}
	
	return 1;
}

nuUINT CSearchDanyinZ::SetSearchDanyin(nuWCHAR *pWsSH, nuBYTE nWsNum)
{ 
	 m_bSort = nuTRUE;
	if(m_nwsListNameMemIdx==-1)
	{
		nuLONG nSize = DY_MAX_KEPT_RECORD_LIMIT* sizeof(Trans_DATANAME);
		nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);

		//pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize, &m_nwsListNameMemIdx);
		nuMemset(pData,0,nSize);
		//wsListName = new Trans_DATANAME[];	
	}
	//nuMemset(wsListName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD_LIMIT);


	//nuMemset(m_nWordStartIdx,0,sizeof(m_nWordStartIdx));
	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_nSearchPoiDanyinName,0,sizeof(m_nSearchPoiDanyinName));
	if( pWsSH == NULL || nWsNum == NULL )
	{
		return 0;
	}
	nuBYTE i;
	for(i = 0; i < m_nNameNum && i <nWsNum; ++i)
	{
		if( m_wsName[i] != pWsSH[i] )
		{
			break;
		}
	}
	nuBYTE nSHMode = DY_SEARCH_MODE_RESTART;
	if( m_nNameNum == i && i == nWsNum )//睿奻棒珨欴ㄛ祥蚚刲坰
	{
		nSHMode = DY_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//菴珨跺趼睫憩祥珨欴ㄛ笭陔刲坰
	{
		nSHMode = DY_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else
	{
		if( i == m_nNameNum )
		{
			nSHMode = DY_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = DY_SEARCH_MODE_BACKWARD;//DY_SEARCH_MODE_RESTART;
			//m_nASIdxCount = 0;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//砃綴刲坰ㄛ剒猁笭扢刲坰腔萸
			
		}
	}

	if(nSHMode != DY_SEARCH_MODE_KEEP)
	{
		nuMemset(m_nWordStartIdx,-1,sizeof(m_nWordStartIdx));
	}

	nuMemset(m_wsName, 0, sizeof(m_wsName));
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));

	nuWcsncpy(m_wsName, pWsSH, DY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiDanyinName, pWsSH, DY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	m_nNameNum = nWsNum;
	nuDWORD t1 = nuGetTickCount();
	if( !SearchDY_A(nSHMode) )
	{
		return 0;
	}
	nuDWORD t2 = nuGetTickCount();
	nuDWORD t3 = t2-t1;
	return 1;
}
nuUINT CSearchDanyinZ::SetSearchDanyin2(nuWCHAR *pWsSH, nuBYTE nWsNum)
{ 
	 m_bSort = nuFALSE;
	 if(m_nwsListNameMemIdx==-1)
	 {
		 nuLONG nSize = DY_MAX_KEPT_RECORD_LIMIT* sizeof(Trans_DATANAME);
		 nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);
		 nuMemset(pData,0,nSize);

		// nuMemset(0,pData,nSize);
	 }
	 
/*
	if(wsListName == NULL)
	{
		wsListName = new Trans_DATANAME[DY_MAX_KEPT_RECORD_LIMIT];	
	}
	nuMemset(wsListName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD_LIMIT);

*/
	//nuMemset(m_nWordStartIdx,0,sizeof(m_nWordStartIdx));
	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_nSearchPoiDanyinName,0,sizeof(m_nSearchPoiDanyinName));
	if( pWsSH == NULL || nWsNum == NULL )
	{
		return 0;
	}
	nuBYTE i;
	for(i = 0; i < m_nNameNum && i <nWsNum; ++i)
	{
		if( m_wsName[i] != pWsSH[i] )
		{
			break;
		}
	}
	nuBYTE nSHMode = DY_SEARCH_MODE_RESTART;
	if( m_nNameNum == i && i == nWsNum )//睿奻棒珨欴ㄛ祥蚚刲坰
	{
		nSHMode = DY_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//菴珨跺趼睫憩祥珨欴ㄛ笭陔刲坰
	{
		nSHMode = DY_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else
	{
		if( i == m_nNameNum )
		{
			nSHMode = DY_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = DY_SEARCH_MODE_BACKWARD;//DY_SEARCH_MODE_RESTART;
			//m_nASIdxCount = 0;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//砃綴刲坰ㄛ剒猁笭扢刲坰腔萸

		}
	}

	if(nSHMode != DY_SEARCH_MODE_KEEP)
		nuMemset(m_nWordStartIdx,-1,sizeof(m_nWordStartIdx));

	nuMemset(m_wsName, 0, sizeof(m_wsName));
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));

	nuWcsncpy(m_wsName, pWsSH, DY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiDanyinName, pWsSH, DY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	m_nNameNum = nWsNum;
	nuDWORD t1 = nuGetTickCount();
	if( !SearchDY_A(nSHMode) )
	{
		return 0;
	}
	nuDWORD t2 = nuGetTickCount();
	nuDWORD t3 = t2-t1;

	return 1;
}
nuUINT CSearchDanyinZ::SetSearchDanyin3(nuWCHAR *pWsSH, nuBYTE nWsNum)
{ 
	//m_bSort = nuTRUE;
	
	if(m_nwsListNameMemIdx == -1)
	{
		nuLONG nSize = DY_MAX_KEPT_RECORD_LIMIT* sizeof(Trans_DATANAME);
		PTrans_DATANAME wsListName = (PTrans_DATANAME)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);
		nuMemset(wsListName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD_LIMIT);

	//	wsListName = new Trans_DATANAME[DY_MAX_KEPT_RECORD_LIMIT];	
	}
	

	//nuMemset(m_nWordStartIdx,0,sizeof(m_nWordStartIdx));
	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_nSearchPoiDanyinName,0,sizeof(m_nSearchPoiDanyinName));
	if( pWsSH == NULL || nWsNum == NULL )
	{
		return 0;
	}
	nuBYTE i;
	for(i = 0; i < m_nNameNum && i <nWsNum; ++i)
	{
		if( m_wsName[i] != pWsSH[i] )
		{
			break;
		}
	}
	nuBYTE nSHMode = DY_SEARCH_MODE_RESTART;
	if( m_nNameNum == i && i == nWsNum )//睿奻棒珨欴ㄛ祥蚚刲坰
	{
		nSHMode = DY_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//菴珨跺趼睫憩祥珨欴ㄛ笭陔刲坰
	{
		nSHMode = DY_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else
	{
		if( i == m_nNameNum )
		{
			nSHMode = DY_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = DY_SEARCH_MODE_BACKWARD;//DY_SEARCH_MODE_RESTART;
			//m_nASIdxCount = 0;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//砃綴刲坰ㄛ剒猁笭扢刲坰腔萸

		}
	}

	if(nSHMode != DY_SEARCH_MODE_KEEP)
	{
		nuMemset(m_nWordStartIdx,-1,sizeof(m_nWordStartIdx));
	}

	nuMemset(m_wsName, 0, sizeof(m_wsName));
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));

	nuWcsncpy(m_wsName, pWsSH, DY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiDanyinName, pWsSH, DY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	m_nNameNum = nWsNum;
	//nuDWORD t1 = nuGetTickCount();
	if( !SearchDY_A(nSHMode) )
	{
		return 0;
	}
	//nuDWORD t2 = nuGetTickCount();
	//nuDWORD t3 = t2-t1;
	return 1;
}

nuBOOL CSearchDanyinZ::SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging)
{
	m_bSortCity= nuTRUE;
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));//
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	if (!pKeyWord)  
	{
		return nuFALSE; 
	}
	 nuWcscpy(m_nSearchPoiKeyName, pKeyWord);
	 m_bNameSort = nuTRUE;
	return nuTRUE;
}


nuUINT CSearchDanyinZ::SetSearchKEY(nuWCHAR *pWsSH, nuBYTE nWsNum)
{
	m_bSortCity = nuTRUE;
	if(m_nwsListNameMemIdx == -1)
	{
		nuLONG nSize = DY_MAX_KEPT_RECORD_LIMIT* sizeof(Trans_DATANAME);
		PTrans_DATANAME wsListName = (PTrans_DATANAME)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);
		nuMemset(wsListName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD_LIMIT);

		//	wsListName = new Trans_DATANAME[DY_MAX_KEPT_RECORD_LIMIT];	
	}
	/*
	if(wsListName == NULL)
	{
		wsListName = new Trans_DATANAME[DY_MAX_KEPT_RECORD_LIMIT];	
	}
	nuMemset(wsListName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD_LIMIT);
	*/
	if( pWsSH == NULL || nWsNum == NULL )
	{
		return 0;
	}
	nuBYTE i;
	for(i = 0; i < m_nNameNum && i <nWsNum; ++i)
	{
		if( m_wsName[i] != pWsSH[i] )
		{
			break;
		}
	}
	nuBYTE nSHMode = DY_SEARCH_MODE_RESTART;
	if( m_nNameNum == i && i == nWsNum )//睿奻棒珨欴ㄛ祥蚚刲坰
	{
		nSHMode = DY_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//菴珨跺趼睫憩祥珨欴ㄛ笭陔刲坰
	{
		nSHMode = DY_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else
	{
		if( i == m_nNameNum )
		{
			nSHMode = DY_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = DY_SEARCH_MODE_BACKWARD;//DY_SEARCH_MODE_RESTART;
			//m_nASIdxCount = 0;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//砃綴刲坰ㄛ剒猁笭扢刲坰腔萸

		}
	}
	nuMemset(m_wsName, 0, sizeof(m_wsName));
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));

	nuWcsncpy(m_wsName, pWsSH, DY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiKeyName, pWsSH, DY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
	//nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	m_nNameNum = nWsNum;
	if( !SearchDY_B(nSHMode) )
	{
		return 0;
	}/*
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	nuINT nTotal = pShData->nKeptCount;

	for(nuINT i=0;i<nTotal ;i++)
	{
		DYNODEFORUI temp = {0};
		m_fileDY.GetOneDYData2(&temp, pShData->pIndexList[i]);
		nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		
	}
	NAME_Order(nTotal);
	*/
	return 1;
}
nuBOOL CSearchDanyinZ::SearchDY_B(nuBYTE nShMode)
{
	if( nShMode == DY_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount;
	PDYNODE_B_1 pDanyinA = (PDYNODE_B_1)m_fileDY.GetDanyinB(&nTotalCount);//郔疑溫婓pShData眳
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL || pDanyinA == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;//20090109.
	nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	if( nShMode == DY_SEARCH_MODE_RESTART || nShMode == DY_SEARCH_MODE_BACKWARD )//笭陔植A梑
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
		pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_DEFAULT;
		for(nuDWORD i = nAIdxStart ; i < nTotalCount; ++i)
		{
			//			MessageBoxW(NULL, (nuWCHAR*)pDanyinA[i].pName, L"", 0);

			SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[i].nName, i, pShData);
		}
	}
	else
	{
		//珋植悵湔腔List笢梑ㄛList祥寀樟哿婓A腔綴醱樟哿梑
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i;
		nuDWORD nLastIdx =  pShData->pIndexList[ nOldKeptCount - 1];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_DEFAULT;
		for(i = 0 ; i < nOldKeptCount; ++i)
		{
			SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[ pShData->pIndexList[i] ].nName, 
				pShData->pIndexList[i], 
				pShData );
		}
		if(  nFoundDataMode == DY_FOUNDDATA_MODE_FIND_PART )
		{
			for( i = nLastIdx; i < nTotalCount; ++i )
			{
				SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[i].nName, i, pShData);
			}
		}
	}
	m_bReSearchStroke = nuTRUE;
	return nuTRUE;
}
nuBOOL CSearchDanyinZ::SearchDY_A(nuBYTE nShMode)
{
	#ifdef ZENRIN
		nSameNameIdx = 0;
		if(NULL == wsTempName)
		wsTempName = new Trans_DATANAME[DY_MAX_KEPT_RECORD];
		if(NULL == nSameCity)
		nSameCity  = new nuINT [DY_MAX_KEPT_RECORD];
		if(NULL == nSameTown)
		nSameTown  = new nuINT [DY_MAX_KEPT_RECORD];

		nuMemset(wsTempName,0,sizeof(Trans_DATANAME)*DY_MAX_KEPT_RECORD);
		nuMemset(nSameCity,0,sizeof(nuINT)*DY_MAX_KEPT_RECORD);
		nuMemset(nSameTown,0,sizeof(nuINT)*DY_MAX_KEPT_RECORD);


	#endif 
	if( nShMode == DY_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	PDANYIN_A pDanyinA = (PDANYIN_A)m_fileDY.GetDanyinA(&nTotalCount);//郔疑溫婓pShData眳
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL || pDanyinA == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;//20090109.
	nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	if( nShMode == DY_SEARCH_MODE_RESTART || nShMode == DY_SEARCH_MODE_BACKWARD )//笭陔植A梑
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
		pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_DEFAULT;
		for(nuDWORD i = nAIdxStart ; i < nTotalCount; ++i)
		{
			//			MessageBoxW(NULL, (nuWCHAR*)pDanyinA[i].pName, L"", 0);
			SearchDY_A_WSCMP((nuWCHAR*)pDanyinA[i].pName, i, pShData);
		}
	}
	else
	{
		//珋植悵湔腔List笢梑ㄛList祥寀樟哿婓A腔綴醱樟哿梑
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx =  pShData->pIndexList[ nOldKeptCount - 1];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_DEFAULT;
		for(i = 0 ; i < nOldKeptCount; ++i)
		{
			SearchDY_A_WSCMP((nuWCHAR*)pDanyinA[ pShData->pIndexList[i] ].pName, 
				pShData->pIndexList[i], 
				pShData );
		}
		if( nFoundDataMode == DY_FOUNDDATA_MODE_FIND_PART )
		{
			for( i = nLastIdx; i < nTotalCount; ++i )
			{
				SearchDY_A_WSCMP((nuWCHAR*)pDanyinA[i].pName, i, pShData);
			}
		}
	}
	m_bReSearchStroke = nuTRUE;
	return nuTRUE;
}

nuVOID CSearchDanyinZ::SearchDY_A_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData)
{
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	nuBOOL bHaveKept = nuFALSE;
	for(nuBYTE j = 0; j < nMaxNum; ++j)
	{
		if( pWsDY[j] == NULL || nMaxNum - j < m_nNameNum )
		{
			break;
		}
		//
		if( pWsDY[j] == m_wsName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum ; ++k )
			{
				if( pWsDY[k+j] != m_wsName[k] )
				{
					break;
				}
			}
			// Modified by Damon for backword search Error by Damon 20100415
			while( m_nASIdxCount < k && m_nASIdxCount < DY_MAX_ADDRESS_NUM )
			{
				if( (k-1) == m_nASIdxCount )
				{
					m_pAStartIdx[m_nASIdxCount] = idx;
				}
				++m_nASIdxCount;
			}	
			/*while( m_nASIdxCount < k && m_nASIdxCount < DY_MAX_ADDRESS_NUM )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}*/
			//  --------------------------------------------------------------
			if( k == m_nNameNum )//梑善珨跺眈睫磁腔
			{
				if( k+j < nMaxNum /*&& pWsDY[k+j] != NULL*/ )
				{
					AddKeyMask(pWsDY[k+j]);
				}
				//
				if( !bHaveKept )
				{
					

					++pShData->nTotalFound;
					if( pShData->nKeptCount < DY_MAX_KEPT_RECORD ) 
					{
						m_nWordStartIdx[pShData->nKeptCount] = j;
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_ALL;
					}
					else
					{
						pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_PART;
					}
					bHaveKept = nuTRUE;
				}
				j += (k - 1);
			}
		}
	}
}

nuVOID CSearchDanyinZ::SearchDY_A_WSCMP2(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData)
{
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	nuBOOL bHaveKept = nuFALSE;
	nuBYTE k = 0;
	for(nuBYTE j = 0; j < nMaxNum; ++j)
	{
		if( pWsDY[j] == NULL || nMaxNum - j < m_nNameNum )
		{
			break;
		}
		//
		if( pWsDY[j] == m_wsName[k] )
		{
			k++;
			if(k<m_nNameNum) continue;
			// Modified by Damon for backword search Error by Damon 20100415
			while( m_nASIdxCount < k && m_nASIdxCount < DY_MAX_ADDRESS_NUM )
			{
				if( (k-1) == m_nASIdxCount )
				{
					m_pAStartIdx[m_nASIdxCount] = idx;
				}
				++m_nASIdxCount;
			}	
			/*while( m_nASIdxCount < k && m_nASIdxCount < DY_MAX_ADDRESS_NUM )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}*/
			//  --------------------------------------------------------------
			if( j+1 < nMaxNum /*&& pWsDY[k+j] != NULL*/ )
			{
				AddKeyMask(pWsDY[j+1]);
			}
			//
			if( !bHaveKept )
			{
				++pShData->nTotalFound;
				if( pShData->nKeptCount < DY_MAX_KEPT_RECORD )
				{
					pShData->pIndexList[pShData->nKeptCount++] = idx;
					pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_ALL;
				}
				else
				{
					pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_PART;
				}
				bHaveKept = nuTRUE;
			}
			break;
		}
	}
}
nuVOID CSearchDanyinZ::SearchDY_B_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData)
{
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	nuBOOL bHaveKept = nuFALSE;
	for(nuBYTE j = 0; j < nMaxNum; ++j)
	{
		if( pWsDY[j] == NULL || nMaxNum - j < m_nNameNum || bHaveKept)
		{
			break;
		}
		//
		if( pWsDY[j] == m_wsName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum,(k+j)<nMaxNum ; )
			{
				if( pWsDY[k+j] == m_wsName[k] )
				{
					++k; 
					if(k==m_nNameNum)
					break;
				} 
				else
				{
					j++;
				}
 
			}
			//j=nMaxNum;
			if(k!=m_nNameNum)
				return ;
			// Modified by Damon for backword search Error by Damon 20100415

			/*while( m_nASIdxCount < k && m_nASIdxCount < DY_MAX_ADDRESS_NUM )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}*/
			//  --------------------------------------------------------------
			if( k == m_nNameNum )//梑善珨跺眈睫磁腔
			{
				//
				if( !bHaveKept )
				{
					

					++pShData->nTotalFound;
					if( pShData->nKeptCount < DY_MAX_KEPT_RECORD )
					{
						m_nWordStartIdx[pShData->nKeptCount] = j;
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_ALL;
					}
					else
					{
						pShData->nFoundDataMode	= DY_FOUNDDATA_MODE_FIND_PART;
					}
					bHaveKept = nuTRUE;
				}
			//	j += (k - 1);
			}
		}
	}
}

nuUINT CSearchDanyinZ::GetDYDataHead(PDYCOMMONFORUI pDyCommonForUI)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyCommonForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	else
	{
		pDyCommonForUI->nTotalCount = pShData->nTotalFound;
		pDyCommonForUI->pDyKeyMask	= &m_keyMask;
		return 1;
	}
}

nuUINT CSearchDanyinZ::GetDYDataPage(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyDataForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
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
	if( pDyDataForUI->nPageItemCount > DY_NAME_FORUI_MAX_COUNT )
	{
		pDyDataForUI->nPageItemCount = DY_NAME_FORUI_MAX_COUNT;
	}
	nuWORD nCountRead = nuWORD(nCount - nItemIdx);
	if( nCountRead > pDyDataForUI->nPageItemCount )
	{
		nCountRead = pDyDataForUI->nPageItemCount;
	}
	pDyDataForUI->nThisPageCount = 0;

	//PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	nuINT nTotal = pShData->nKeptCount;

	//nuINT n[1000]={0};
	//nuINT nn[1000]={0};
	/*for(nuINT i=0;i<nTotal ;i++)
	{
		DYNODEFORUI temp = {0};
		m_fileDY.GetOneDYData_GETNAME(&temp, pShData->pIndexList[i]);
		nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
	}
	NAME_Order(nTotal);
	*/
	if(false&&m_bSortCity)
	{
		m_bSortCity =nuFALSE;
		nuINT tempIdx=0;
		DYNODEFORUI tempData = {0};
		DYNODEFORUI *temp  = new DYNODEFORUI[nCount];
		nuMemset(temp,0,sizeof(DYNODEFORUI)*nCount);

		for(nuDWORD i = 0; i < nCount;i++)
		{
			m_fileDY.GetOneDYData_GETNAME(&temp[i], pShData->pIndexList[i]);
		}

		for(nuDWORD i = 0; i < nCount;i++)
		{
			for(nuDWORD ii = 0; ii < nCount-1;ii++)
			{
				if(temp[ii].nTownID > temp[ii+1].nTownID)
				{
					tempData = temp[ii];
					temp[ii] = temp[ii+1];
					temp[ii+1] = tempData;

					tempIdx = pShData->pIndexList[ii];
					pShData->pIndexList[ii]=pShData->pIndexList[ii+1];
					pShData->pIndexList[ii+1]=tempIdx;

			
				}
			}
		}

	}

	pIdxList = pShData->pIndexList;

	for(nuDWORD i = nItemIdx; i < nCountRead + nItemIdx; ++i)
	{
		if( !m_fileDY.GetOneDYData(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount], pIdxList[i]) )
		{
			break;
		}
		pDyDataForUI->pNodeIdxList[pDyDataForUI->nThisPageCount] = pIdxList[i];
		++pDyDataForUI->nThisPageCount;
	}
	pDyDataForUI->nNowPageItemIdx = (nuWORD)nItemIdx;
	//
	pDyDataForUI->nTotalCount = nCount;
	pDyDataForUI->nIdxSelected= 0;
	if( m_bReSearchStroke )
	{
		SearchDY_Stroke(pIdxList, nCount);
		m_bReSearchStroke = nuFALSE;
	}
	pDyDataForUI->nMinStroke = pShData->nMinStroke;
	pDyDataForUI->nMaxStroke = pShData->nMaxStroke;
	return 1;
}

nuUINT CSearchDanyinZ::GetDYGetWordData(PDYWORDFORUI pDyData)
{
	m_bNew   = nuFALSE;
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if(pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
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
	
	//
	nuINT j=0,nIdx=0;
	nuINT n_Len=nuWcslen(m_wsName);

	nuINT x=-1; //Just Clear Data 
	m_fileDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x);
	
	nuDWORD t1 = nuGetTickCount();

	if(m_nSearchPoiDanyinName)
	{
		nuINT nLimit = 0;
		
		for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
		{
			nuINT n=0;
			if( !m_fileDY.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&pDyData->Data[nIdx], pIdxList[i],n) )
			{
				if(n==-1)
				{
					m_nWord[i]=-1;
				//	m_nIdx[i]=-1;
					continue;
				}
				if(pDyData->Data[n].nFindCount<200)
				{
					pDyData->Data[n].nFindCount++;
					m_nWord[i]=n;
					//	m_nIdx[i]=n;
				}
				else
				{
					m_nWord[i]=-1;
				//	m_nIdx[i]=-1;
				}
				continue;
			}
			if(pDyData->Data[nIdx].nFindCount<200)
			{
				pDyData->Data[nIdx].nFindCount++;
				m_nWord[i]=nIdx;
				//m_nIdx[i]=nIdx;
			}
			else
			{
				m_nWord[i]=-1;
				//m_nIdx[i]=-1;
			}
			++nIdx;
			if(!m_bSort)
			{
				if(nIdx==4)
					break; 
			}
		}
	}
	pDyData->Data[nIdx].name[0] = 0;

	
	if(m_bSort)
	{
		m_bSort = nuFALSE;

	DYWORD temp={0};
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
				if((int)pDyData->Data[tag2].nFindCount > (int)pDyData->Data[i].nFindCount)
				{
					tag2 = i;
				} 
			} 
			if (tag2!=last)
			{
				temp = pDyData->Data[last];
				pDyData->Data[last] = pDyData->Data[tag2];
				pDyData->Data[tag2] = temp;
			} 

		}
	}

	}
	nuDWORD t2 = nuGetTickCount();
	nuDWORD t3 = t2-t1;
	pDyData->nTotalCount = nIdx;	
	
	return 1;
}
nuUINT CSearchDanyinZ::GetDYDataPage_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	nuDWORD t1 = nuGetTickCount();
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyDataForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
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
	if( pDyDataForUI->nPageItemCount > DY_NAME_FORUI_MAX_COUNT )
	{
		pDyDataForUI->nPageItemCount = DY_NAME_FORUI_MAX_COUNT;
	}
	nuWORD nCountRead = nuWORD(nCount - nItemIdx);
	if( nCountRead > pDyDataForUI->nPageItemCount )
	{
		nCountRead = pDyDataForUI->nPageItemCount;
	}
	pDyDataForUI->nThisPageCount = 0;
	nuMemset(pDyDataForUI->dyNodeList,0,sizeof(pDyDataForUI->dyNodeList));
	
	nuINT n_Len=nuWcslen(m_wsName);
	nuINT nIdx=0;
	nuINT x=-1;
	//m_fileDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x);
	#ifdef ZENRIN
		nSameNameIdx=0;
	#endif
	//DYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	//nuINT n[1000]={0};
	//nuINT nn[1000]={0};

	//PDYWORD m_SearchRes = (PDYWORD)m_pMmApi->MM_GetDataMassAddr(m_nwsListNameMemIdx);
	nuLONG nSize = DY_MAX_KEPT_RECORD_LIMIT* sizeof(Trans_DATANAME);
	PTrans_DATANAME wsListName = (PTrans_DATANAME)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);


	if(m_bNameSort && nuFALSE)
	{
		nuDWORD t1 =nuGetTickCount();
		m_bNameSort = nuFALSE;
	
		for(nuINT i=0;i<nCount ;i++)
		{
			DYNODEFORUI temp = {0};
			m_fileDY.GetOneDYData_GETNAME(&temp, pShData->pIndexList[i]);
			nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		}
		NAME_Order(nCount);

		nuDWORD t2 =nuGetTickCount();
		nuDWORD t3 =t2-t1;
		int l=0;
	}

	if(nuFALSE && m_bSortCity)
	{
		m_bSortCity =nuFALSE;
	nuINT tempIdx=0;
	DYNODEFORUI tempData = {0};
	DYNODEFORUI *temp  = new DYNODEFORUI[nCount];
	nuMemset(temp,0,sizeof(DYNODEFORUI)*nCount);

	for(nuDWORD i = 0; i < nCount;i++)
	{
		m_fileDY.GetOneDYData_GETNAME(&temp[i], pShData->pIndexList[i]);
	}

	for(nuDWORD i = 0; i < nCount;i++)
	{
		for(nuDWORD ii = 0; ii < nCount-1;ii++)
		{
			if(temp[ii].nTownID > temp[ii+1].nTownID)
			{
				tempData = temp[ii];
				temp[ii] = temp[ii+1];
				temp[ii+1] = tempData;

				tempIdx = pShData->pIndexList[ii];
				pShData->pIndexList[ii]=pShData->pIndexList[ii+1];
				pShData->pIndexList[ii+1]=tempIdx;

				tempIdx = m_nWordStartIdx[ii];
				m_nWordStartIdx[ii]=m_nWordStartIdx[ii+1];
				m_nWordStartIdx[ii+1]=tempIdx;
			}
		}
	}

	}

	pIdxList = pShData->pIndexList;
	
	//prosper add 20130416
	nuINT nComIdx=m_fileDY.GetWordIndx(m_nSearchPoiKeyName);
	
	if(!m_bNew)
	{

	for(nuDWORD i = 0; i < nCount;i++)
	{
		//prosper add for faster search 2013 04 16 
		if(m_nWord[i]!=nComIdx || m_nWord[i]==-1)
		{
			continue;
		}

		if( !m_fileDY.GetOneDYData_PAGE(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount],pIdxList[i],m_nSearchPoiKeyName,m_nWordStartIdx[i] ))
		{
			continue;
			//break;
		}
		
		#ifdef ZENRIN
		DYNODEFORUI temp = {0};
		m_fileDY.GetOneDYData_GETNAME(&temp, pIdxList[i]);
		
		nuBOOL bRes=nuFALSE;
		for(nuINT i=0;i<nSameNameIdx;i++)
		{

			if(nuWcscmp(wsTempName[i].name,pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount].wsName)==0)
			{
				if( nSameCity[i]==temp.nCityID && nSameTown[i] ==temp.nTownID)
				{
					bRes=nuTRUE;
					break;
				}
				
			}
		}
		if(bRes)
		{
			nuMemset(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount],0,sizeof(pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount]));
			continue ;
		}
		else
		{
			nuWcscpy(wsTempName[nSameNameIdx].name,pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount].wsName);
			nSameCity[nSameNameIdx]=temp.nCityID;
			nSameTown[nSameNameIdx]=temp.nTownID;
			nSameNameIdx++;
		}
#endif

		nIdx++;
		if(nIdx > nItemIdx)
		{

			pDyDataForUI->pNodeIdxList[pDyDataForUI->nThisPageCount] = pIdxList[i];
			++pDyDataForUI->nThisPageCount;
			if(pDyDataForUI->nThisPageCount == 5)
				break;

		}
	}

	}
	else
	{

		nuDWORD i = 0;
		i+=nItemIdx;
		for(; i < m_nIndex;i++)
	{
		DYNODEFORUI temp = {0};
		m_fileDY.GetOneDYData_GETNAME(&temp, m_nIdx[i]);
		nuWcscpy(pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount].wsCityName,temp.wsCityName);
		nuWcscpy(pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount].wsTownName,temp.wsTownName);
		nuWcscpy(pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount].wsName,temp.wsName);
		nIdx++;
		if(true)
		{

			pDyDataForUI->pNodeIdxList[pDyDataForUI->nThisPageCount] = m_nIdx[i];
			++pDyDataForUI->nThisPageCount;
			if(pDyDataForUI->nThisPageCount == 5)
				break;

		}
	}

	}

	pDyDataForUI->nNowPageItemIdx = (nuWORD)nItemIdx;
	//
	pDyDataForUI->nTotalCount = nCount;
	pDyDataForUI->nIdxSelected= 0;
	
	pDyDataForUI->nMinStroke = pShData->nMinStroke;
	pDyDataForUI->nMaxStroke = pShData->nMaxStroke;
	return 1;
}
#ifdef ZENRIN
nuUINT CSearchDanyinZ::GetDYData_COUNT()
{
	m_nIndex = 0;
	m_bNew   = nuTRUE;
	nuMemset(&m_nIdx,0,sizeof(m_nIdx));

	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
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
	
	
	
	nuINT n_Len=nuWcslen(m_wsName);
	nuINT nIdx=0;
	nuINT x=-1;
	m_fileDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x);
	
	nSameNameIdx=0;
	PTrans_DATANAME wsListName = (PTrans_DATANAME)m_pMmApi->MM_GetDataMassAddr(m_nwsListNameMemIdx);

	if(m_bNameSort && nuFALSE)
	{
		nuDWORD t1 =nuGetTickCount();
		m_bNameSort = nuFALSE;
	
		for(nuINT i=0;i<nCount ;i++)
		{
			DYNODEFORUI temp = {0};
			m_fileDY.GetOneDYData_GETNAME(&temp, pShData->pIndexList[i]);
			nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		}
		NAME_Order(nCount);

		nuDWORD t2 =nuGetTickCount();
		nuDWORD t3 =t2-t1;
		int l=0;
	}

	if(m_bSortCity&&false)
	{
		m_bSortCity =nuFALSE;
	nuINT tempIdx=0;
	DYNODEFORUI tempData = {0};
	DYNODEFORUI *temp  = new DYNODEFORUI[nCount];
	nuMemset(temp,0,sizeof(DYNODEFORUI)*nCount);

	for(nuDWORD i = 0; i < nCount;i++)
	{
		m_fileDY.GetOneDYData_GETNAME(&temp[i], pShData->pIndexList[i]);
	}

	for(nuDWORD i = 0; i < nCount;i++)
	{
		for(nuDWORD ii = 0; ii < nCount-1;ii++)
		{
			if(temp[ii].nTownID > temp[ii+1].nTownID)
			{
				tempData = temp[ii];
				temp[ii] = temp[ii+1];
				temp[ii+1] = tempData;

				tempIdx = pShData->pIndexList[ii];
				pShData->pIndexList[ii]=pShData->pIndexList[ii+1];
				pShData->pIndexList[ii+1]=tempIdx;

				tempIdx = m_nWordStartIdx[ii];
				m_nWordStartIdx[ii]=m_nWordStartIdx[ii+1];
				m_nWordStartIdx[ii+1]=tempIdx;
			}
		}
	}

	}

	pIdxList = pShData->pIndexList;
	nuINT nCOUNT = 0;
	for(nuDWORD i = 0; i < nCount;i++)
	{
		DYNODEFORUI data = {0};		
		if( !m_fileDY.GetOneDYData_PAGE(&data,pIdxList[i],m_nSearchPoiKeyName,m_nWordStartIdx[i] ))
		{
			continue;
			//break;
		}
		#ifdef ZENRIN
		DYNODEFORUI temp = {0};
		m_fileDY.GetOneDYData_GETNAME(&temp, pIdxList[i]);
		
		nuBOOL bRes=nuFALSE;
		for(nuINT i=0;i<nSameNameIdx;i++)
		{

			if(nuWcscmp(wsTempName[i].name,data.wsName)==0)
			{
				if( nSameCity[i]==temp.nCityID && nSameTown[i] ==temp.nTownID)
				{
					bRes=nuTRUE;
					break;
				}
				
			}
		}
		if(bRes)
		{
			//nuMemset(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount],0,sizeof(pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount]));
			continue ;
		}
		else
		{
			m_nIdx[m_nIndex]=pIdxList[i];
			m_nIndex++;
			nuWcscpy(wsTempName[nSameNameIdx].name,data.wsName);
			nSameCity[nSameNameIdx]=temp.nCityID;
			nSameTown[nSameNameIdx]=temp.nTownID;
			nSameNameIdx++;
		}
#endif

		nIdx++;
	}

	return nSameNameIdx;
}
#endif
nuBOOL CSearchDanyinZ::SearchDY_City()
{
	nuDWORD nCount = 0;
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileDY.GetDanyinB2(&nCount);
	if( pDyNodeB2 == NULL )
	{
		return nuFALSE;
	}//祥夔溫善pShData眳狟ㄛ秪峈涴爵醱頗煦饜囀湔﹝
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//蛁砩涴爵祥夔睿奻醱腔遙弇离
	pShData->nCountOfCity = 0;
	for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
	{
		if( pDyNodeB2[pShData->pIndexList[i]].nCityID == pShData->nCityID )
		{
			pShData->pCityIdxList[ pShData->nCountOfCity ] = pShData->pIndexList[i];
			++pShData->nCountOfCity;
			if( pShData->nCountOfCity >= DY_MAX_CITY_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CSearchDanyinZ::SearchDY_Town()
{
	nuDWORD nCount = 0;
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileDY.GetDanyinB2(&nCount);
	if( pDyNodeB2 == NULL )
	{
		return nuFALSE;
	}//郔疑睿綴醱腔弇离祥猁遙
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//
	pShData->nCountOfTown = 0;
	for(nuDWORD i = 0; i < pShData->nCountOfCity; ++i)
	{
		if( pDyNodeB2[ pShData->pCityIdxList[i] ].nTownID == pShData->nTownID )
		{
			pShData->pTownIdxList[ pShData->nCountOfTown ] = pShData->pCityIdxList[i];
			++pShData->nCountOfTown;
			if( pShData->nCountOfTown >= DY_MAX_TOWN_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuUINT CSearchDanyinZ::GetDYDataStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileDY.GetDanyinB2(NULL);
	if( pDyDataForUI == NULL || pShData == NULL || pDyNodeB2 == NULL )
	{
		return 0;
	}
	//
	nuDWORD *pIdxList = NULL;
	nuINT nCount =0;
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
	nuINT i=0;
	if( pDyDataForUI->nIdxSelected >= pDyDataForUI->nThisPageCount )
	{
		pDyDataForUI->nIdxSelected = 0;
	}
	//
	if( nStroke == pDyDataForUI->dyNodeList[pDyDataForUI->nIdxSelected].nStroke )
	{
		return 1;
	}//捩赫羶衄曹趙
	else if( nStroke > pDyDataForUI->dyNodeList[pDyDataForUI->nIdxSelected].nStroke)
	{
		nuBOOL bFind = nuFALSE;
		/*
		*/
		for(i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pDyDataForUI->pNodeIdxList[ pDyDataForUI->nIdxSelected ] )
			{
				break;
			}
		}
		for(;i < nCount; ++i)
		{
			if( pDyNodeB2[ pIdxList[i] ].nStroke >= nStroke )
			{
				bFind = nuTRUE;
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}//羶衄載湮腔捩赫杅賸
	}//捩赫崝樓
	else
	{
		nuBOOL bFind = nuFALSE;
		/**/
		for(i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pDyDataForUI->pNodeIdxList[ pDyDataForUI->nIdxSelected ] )
			{
				break;
			}
		}
		
		for(; i >= 0; --i)
		{
			if( pDyNodeB2[ pIdxList[i] ].nStroke <= nStroke )
			{
				bFind = nuTRUE;
				nStroke = pDyNodeB2[ pIdxList[i] ].nStroke;
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}//羶衄載苤腔捩赫杅賸
		for(; i > 0; --i)
		{
			if( pDyNodeB2[ pIdxList[i - 1] ].nStroke != nStroke )
			{
				break;
			}
		}
	}//捩赫熬屾
	nuWORD nNewPageItemIdx = (nuWORD)((i/pDyDataForUI->nPageItemCount) * pDyDataForUI->nPageItemCount);
	if( pDyDataForUI->nNowPageItemIdx != nNewPageItemIdx )
	{
		pDyDataForUI->nNowPageItemIdx = nNewPageItemIdx;
		nuWORD nCountRead = nuWORD(nCount - nNewPageItemIdx);
		if( nCountRead > pDyDataForUI->nPageItemCount )
		{
			nCountRead = pDyDataForUI->nPageItemCount;
		}
		pDyDataForUI->nThisPageCount = 0;
		for(nuWORD j = nNewPageItemIdx; j < nCountRead + nNewPageItemIdx; ++j)
		{
			if( !m_fileDY.GetOneDYData(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount], pIdxList[j]) )
			{
				break;
			}
			pDyDataForUI->pNodeIdxList[pDyDataForUI->nThisPageCount] = pIdxList[j];
			++pDyDataForUI->nThisPageCount;
		}
	}
	pDyDataForUI->nIdxSelected	= (nuBYTE)(i % pDyDataForUI->nPageItemCount);
	pDyDataForUI->nTotalCount	= nCount;
	return 1;
}

nuVOID CSearchDanyinZ::SearchDY_Stroke(nuDWORD *pIdxList, int nCount)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileDY.GetDanyinB2(NULL);
	if( pShData != NULL && pDyNodeB2 != NULL )
	{
		pShData->nMinStroke = pShData->nMaxStroke = pDyNodeB2[ pIdxList[0] ].nStroke;
		for(int i = 1; i < nCount; ++i)
		{
			if( pDyNodeB2[ pIdxList[i] ].nStroke < pShData->nMinStroke )
			{
				pShData->nMinStroke = pDyNodeB2[ pIdxList[i] ].nStroke;
			}
			else if( pDyNodeB2[ pIdxList[i] ].nStroke > pShData->nMaxStroke )
			{
				pShData->nMaxStroke = pDyNodeB2[ pIdxList[i] ].nStroke;
			}	
		}
	}
}

nuUINT CSearchDanyinZ::GetDyDataDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	return m_fileDY.GetOneDYDetail(pDyDetailForUI, nIdx);
}
nuUINT CSearchDanyinZ::GetDyCityTownName(PDYCITYTOWNNAME pDyCTname, long nCityID)
{
	nuDWORD nTotalCount = 0;
	nuWORD nCityCount = 0;
	nuWORD nTownCount = 0;
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2	pDyNodeB2 = (PDYNODE_B_2)m_fileDY.GetDanyinB2(&nTotalCount);
	nuPBYTE pDyNodeDE = (nuPBYTE)m_fileDY.GetDanyinDE(&nCityCount, &nTownCount);
	if ( pShData == NULL || pDyCTname == NULL || pDyNodeB2 == NULL || pDyNodeDE == NULL )
	{
		return 0;
	}
	pDyCTname->nTotalCount = 0;
	nuDWORD *pIdxList =NULL;
	nuINT nCount = 0;
	if( nCityID < 0 )
	{
		nuBOOL bNewName = nuFALSE;
		nCount		= pShData->nKeptCount;
		pIdxList	= pShData->pIndexList;
		for( nuINT i = 0; i < nCount; ++i )
		{
			bNewName = nuTRUE;
			for(nuBYTE j = 0; j < pDyCTname->nTotalCount; ++j )
			{
				if( pDyNodeB2[ pIdxList[i] ].nCityID == pDyCTname->pIdList[j] )
				{
					bNewName = nuFALSE;
					break;
				}
			}
			if( bNewName )
			{
				nuBYTE *pStr = pDyNodeDE + pDyNodeB2[ pIdxList[i] ].nCityID * DY_CITY_NAME_LEN;
				pDyCTname->pIdList[pDyCTname->nTotalCount] = pDyNodeB2[ pIdxList[i] ].nCityID;
				nuMemcpy(pDyCTname->ppName[pDyCTname->nTotalCount], pStr, DY_CITY_NAME_LEN);
				int nlast = NURO_MIN(DY_CITY_NAME_LEN/2, DY_NAME_CITYTOWN_MAX_NUM);
				pDyCTname->ppName[pDyCTname->nTotalCount][nlast] = 0;
				++pDyCTname->nTotalCount;
				if( pDyCTname->nTotalCount == DY_NAME_CITYTOWN_MAX_COUNT )
				{
					break;
				}
			}
		}
	}
	else
	{
		if( nCityID != pShData->nCityID )
		{
			return 0;
		}
		nuBOOL bNewName = nuFALSE ;
		nCount		= pShData->nCountOfCity;
		pIdxList	= pShData->pCityIdxList;
		for( nuINT i = 0; i < nCount; ++i )
		{
			bNewName = nuTRUE;
			for(nuBYTE j = 0; j < pDyCTname->nTotalCount; ++j )
			{
				if( pDyNodeB2[ pIdxList[i] ].nTownID == pDyCTname->pIdList[j] )
				{
					bNewName = nuFALSE;
					break;
				}
			}
			if( bNewName )
			{
				nuBYTE *pStr = pDyNodeDE + nCityCount * DY_CITY_NAME_LEN + 
					pDyNodeB2[ pIdxList[i] ].nTownID * DY_TOWN_NAME_LEN;
				pDyCTname->pIdList[pDyCTname->nTotalCount] = pDyNodeB2[ pIdxList[i] ].nTownID;
				nuMemcpy(pDyCTname->ppName[pDyCTname->nTotalCount], pStr, DY_TOWN_NAME_LEN);
				nuINT nlast = NURO_MIN(DY_TOWN_NAME_LEN/2, DY_NAME_CITYTOWN_MAX_NUM);
				pDyCTname->ppName[pDyCTname->nTotalCount][nlast] = 0;
				++pDyCTname->nTotalCount;
				if( pDyCTname->nTotalCount == DY_NAME_CITYTOWN_MAX_COUNT )
				{
					break;
				}
			}
		}
	}
	//queue
	if (pDyCTname->nTotalCount>1)
	{
		nuWORD tempid = 0;
		nuWCHAR temp[DY_NAME_CITYTOWN_MAX_NUM] = {0};
		nuUINT tag2 = 0;
		nuUINT last = 0;
		nuUINT times = pDyCTname->nTotalCount - 1; 
		nuUINT i = 0;
		nuUINT j = 0;
		nuUINT len = sizeof(nuWCHAR)*DY_NAME_CITYTOWN_MAX_NUM;
		for (j=0; j<times; ++j)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;++i)
			{
				if(pDyCTname->pIdList[tag2]<=pDyCTname->pIdList[i])
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				tempid = pDyCTname->pIdList[last];
				nuMemcpy(temp, pDyCTname->ppName[last], len);
				pDyCTname->pIdList[last] = pDyCTname->pIdList[tag2];
				nuMemcpy(pDyCTname->ppName[last], pDyCTname->ppName[tag2], len);
				pDyCTname->pIdList[tag2] = tempid;
				nuMemcpy(pDyCTname->ppName[tag2], temp, len);
			}
		}
	}
	return 1;
}
nuVOID CSearchDanyinZ::SearchDY_MapStart()
{
	nuDWORD nTotalCount = 0;
	PDANYIN_A pDanyinA = (PDANYIN_A)m_fileDY.GetDanyinA(&nTotalCount);
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData != NULL || pDanyinA != NULL )
	{
		for( nuDWORD i = 0; i < nTotalCount; ++i )
		{
			nuWCHAR *pWs = (nuWCHAR*)pDanyinA[i].pName;
			AddKeyMask(pWs[0]);
			if( pShData->nKeptCount < DY_MAX_KEPT_RECORD )
			{
				pShData->pIndexList[pShData->nKeptCount++] = i;
			}
		}
		pShData->nTotalFound = nTotalCount;
	}
}

nuBOOL CSearchDanyinZ::VoicePoiSetMap(nuINT nIdx)
{
    if (!m_fileDY.OpenNewFile(nIdx))
    {
        return 0;
    }
	return true;
}

nuBOOL CSearchDanyinZ::VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{	
	DANYINHEAD header={0};
	m_fileDY.ReadData(0,&header,sizeof(DANYINHEAD));
	
	if(nIdx > header.nTotalCount || nIdx < 0 )
	{
		return nuFALSE;
	}

	long addr=0;
	addr=sizeof(DANYINHEAD)+( sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) + sizeof(DYNODE_B_3) )*header.nTotalCount+sizeof(DYNODE_C) * nIdx;
	
	m_fileDY.ReadData(addr,&pDyDetailForUI->point,sizeof(pDyDetailForUI->point));
	
	nuBYTE nOtherLen={0};
#ifndef ADDRNAME
	m_fileDY.ReadData(header.nFNAddr+ sizeof(DANYIN_A) * nIdx,pDyDetailForUI->wsOther,20*sizeof(nuWCHAR));
#else
	DYNODE_B_3 dyNodeB3={0};
	long nSize = header.nOthersAddrAddr+ sizeof(DYNODE_B_3) * nIdx;
	
	m_fileDY.ReadData(nSize,&dyNodeB3,sizeof(DYNODE_B_3));
	m_fileDY.ReadData(dyNodeB3.nOtherAddr,&nOtherLen,1);
	m_fileDY.ReadData(dyNodeB3.nOtherAddr+1,pDyDetailForUI->wsOther,nOtherLen*sizeof(nuWCHAR));
//	m_fileDY.CloseNewFile();
#endif	
//	pDyDetailForUI->wsOther[nOtherLen/sizeof(nuWCHAR)] = 0;

	return nuTRUE;
}
nuBOOL CSearchDanyinZ::VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx)
{

	DANYINHEAD header={0};
	m_fileDY.ReadData(0,&header,sizeof(DANYINHEAD));

	if(nIdx > header.nTotalCount || nIdx < 0 )
	{
		return nuFALSE;
	}
	DYNODE_B_2 temp = {0};
	long addr = 0;
	addr=sizeof(DANYINHEAD)+( sizeof(DANYIN_A) + sizeof(DYNODE_B_1)  )*header.nTotalCount +  nIdx*sizeof(DYNODE_B_2);

	m_fileDY.ReadData(addr,&temp,sizeof(temp));

	addr=header.nCityNameAddr +  DY_CITY_NAME_LEN * temp.nCityID ;
	m_fileDY.ReadData(addr,pCityName,DY_CITY_NAME_LEN);

	addr=header.nTownNameAddr +  DY_TOWN_NAME_LEN * temp.nTownID;
	m_fileDY.ReadData(addr,pTownName,DY_TOWN_NAME_LEN);

//	m_fileDY.CloseNewFile();
	
	
	return nuTRUE;
}
nuBOOL CSearchDanyinZ::VoicePoiEnd()
{
	m_fileDY.CloseNewFile();
	return true;
}
nuUINT   CSearchDanyinZ::SetSearchMode(nuINT nType)
{
	m_fileDY.SetSearchMode(nType);
	return nuTRUE;
}
nuBOOL	CSearchDanyinZ::NAME_Order(nuINT n)
{
	//nuINT n = m_nTotal;
	//return 1;

/*	if (n==0 || nuTRUE) //?????Ƨ?
	{
		return nuFALSE;
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
			return nuFALSE;
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
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);


	nuWCHAR wsNumber[]={19968,20108,19977,22235,20116,20845,19971,20843,20061,21313,0};
	Trans_DATANAME temp  = {0}; //new nuWCHAR[NURO_SEARCH_BUFFER_MAX>>1];
	//nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
	//DYWORD   = {0};
	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=0;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	nuINT nTemp =0 ,nTempData = 0;
	for(nIdx2=0;nIdx2<n-1;nIdx2++)
		for(nIdx=0;nIdx<n-1;nIdx++)

		{
			nuINT nLen  = nuWcslen(wsListName[nIdx].name);
			nuINT nLen2 = nuWcslen(wsListName[nIdx+1].name);


			if(nLen > nLen2 )
			{
				//nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
				temp				=	wsListName[nIdx];
				wsListName[nIdx]	=	wsListName[nIdx+1];
				wsListName[nIdx+1]  =   temp;

				nTemp				=	pShData->pIndexList[nIdx];
				pShData->pIndexList[nIdx]		=	pShData->pIndexList[nIdx+1];
				pShData->pIndexList[nIdx+1]		=   nTemp;

				nTemp				=	m_nWordStartIdx[nIdx];
				m_nWordStartIdx[nIdx]	=	m_nWordStartIdx[nIdx+1];
				m_nWordStartIdx[nIdx+1]  =   nTemp;

				
			}			
		}


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

					nuINT nLen  = nuWcslen(wsListName[nIdx].name);
					nuINT nLen2 = nuWcslen(wsListName[nIdx+1].name);
					if(nLen < nLen2)
					{
						continue;
					}
					parser  = TransCode(wsListName[nIdx].name[k]);
					parser2 = TransCode(wsListName[nIdx+1].name[k]);


					nuINT nNumber=-1;
					nuINT nNumber2=-1;

					nuINT parserNumber = wsListName[nIdx].name[k];
					nuINT parserNumber2 = wsListName[nIdx+1].name[k];

					for(nuINT j=0;j<10;j++)
					{
						if(parserNumber == wsNumber[j])
						{
							nNumber = j;	
						}
						if(parserNumber2 == wsNumber[j])
						{
							nNumber2 = j;
						}
					}

					

					if((nNumber > -1) && (nNumber2 > -1))
					{
						if(nNumber > nNumber2)  
						{
							//	nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
							temp				=	wsListName[nIdx];
							wsListName[nIdx]	=	wsListName[nIdx+1];
							wsListName[nIdx+1]  =   temp;

							nTemp				=	pShData->pIndexList[nIdx];
							pShData->pIndexList[nIdx]		=	pShData->pIndexList[nIdx+1];
							pShData->pIndexList[nIdx+1]		=   nTemp;

							nTemp				=	m_nWordStartIdx[nIdx];
							m_nWordStartIdx[nIdx]	=	m_nWordStartIdx[nIdx+1];
							m_nWordStartIdx[nIdx+1]  =   nTemp;
						}
					}
					else
					{

						if(parser > parser2)  
						{
							

								//	nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
								temp				=	wsListName[nIdx];
								wsListName[nIdx]	=	wsListName[nIdx+1];
								wsListName[nIdx+1]  =   temp;

								nTemp				=	pShData->pIndexList[nIdx];
								pShData->pIndexList[nIdx]		=	pShData->pIndexList[nIdx+1];
								pShData->pIndexList[nIdx+1]		=   nTemp;

								nTemp				=	m_nWordStartIdx[nIdx];
								m_nWordStartIdx[nIdx]	=	m_nWordStartIdx[nIdx+1];
								m_nWordStartIdx[nIdx+1]  =   nTemp;
							}			


						
					}
				}
		}




		if(data!=NULL)
		{
			delete[] data;
			data = NULL;
		}
*/		return nuTRUE;
}
nuINT	CSearchDanyinZ::TransCode(nuINT code)
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
nuINT CSearchDanyinZ::GetPoiTotal()
{
	m_nIndex = 0;
	m_bNew   = nuTRUE;
	nuMemset(&m_nIdx,0,sizeof(m_nIdx));

	nuINT n = 0;
	//DYWORD *temp =NULL; 
	//nuMemset(pData,0,nSize);

	
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if(pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
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

	//temp=new DYWORD[pShData->nKeptCount];
	//nuMemset(temp,0,sizeof(DYWORD)*pShData->nKeptCount);
	nuWORD	m_ntempMemIdx = -1;
	nuLONG nSize = sizeof(DYWORD)*pShData->nKeptCount;
	PDYWORD temp = NULL;
	//PDYWORD temp = (PDYWORD)m_pMmApi->MM_GetDataMassAddr(m_ntempMemIdx);


	if(m_ntempMemIdx==-1)
		temp = (PDYWORD)m_pMmApi->MM_GetDataMemMass(nSize, &m_ntempMemIdx);

	//
	nuINT j=0,nIdx=0;
	nuINT n_Len=nuWcslen(m_wsName);

	nuINT x=-1; //Just Clear Data 
	m_fileDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x);

	nuDWORD t1 = nuGetTickCount();
	
	if(m_nSearchPoiKeyName)
	{
		nuINT nLimit = 0;
		for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
		{

			nuINT n=0;
			
			if( !m_fileDY.GetOneDYData_WORD_KEY(n_Len,m_nWordStartIdx[j++],&temp[nIdx], pIdxList[i],n,m_nSearchPoiKeyName) )
			{
				if(n==-1)
				{
					continue;
				}
				if(temp[n].nFindCount<200)
				{
					m_nIdx[m_nIndex]=pIdxList[i];
					temp[n].nFindCount++;
					m_nIndex++;

				}
				else
				{
					break;
				}
				continue;
			}
			if(n==-1)
			{
				continue;
			}
			/*if(pDyData->Data[nIdx].name[0] == test[0] && pDyData->Data[nIdx].name[1] == test[1])
			{
				j--;
				if( !m_fileDY.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&pDyData->Data[nIdx], pIdxList[i],n) )
				{
					pDyData->Data[n].nFindCount++;
					continue;
				}
			}*/
			if(temp[nIdx].nFindCount<200)
			{
				m_nIdx[m_nIndex]=pIdxList[i];
				temp[n].nFindCount++;
				m_nIndex++;
				
			}
			else
			{
				break;
			}
			++nIdx;
			/*if(!m_bSort)
			{
				if(nIdx==4)
					break; 
			}*/
		}
	}
	temp[nIdx].name[0] = 0;
	nuINT nLen = nuWcslen(m_nSearchPoiKeyName);

	nuBOOL bFindData = nuFALSE ;
	//bFind
	for(nuINT k=0;k < nIdx;k++)
	{
		nuBOOL bFind = nuFALSE;
		nuINT  o = 0;
		nuINT  nFind=0;
		nuINT  lLen = nuWcslen(temp[k].name);
		for(nuINT j=0;j<nLen;j++)
		{
			if(temp[k].name[j] == m_nSearchPoiKeyName[o])
			{
				nFind = 1;
				o++;
				for(nuINT jj=j+1;jj<lLen;jj++)
				{
					if(temp[k].name[jj] == 45 || temp[k].name[jj] == 32)
					{
						continue;
					}
					if(temp[k].name[jj] != m_nSearchPoiKeyName[o])
					{
						break;
					}
					o++;
					nFind++;
				}
			}			
		}
		if(nFind == nLen)
		{
			bFind = nuTRUE;
			n = temp[k].nFindCount;
			break;
		}
	}
	return n;
}
