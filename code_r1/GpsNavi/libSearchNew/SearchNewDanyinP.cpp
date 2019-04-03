// SearchNewDanyinP.cpp: implementation of the CSearchNewDanyinP class.
//
 
//////////////////////////////////////////////////////////////////////
#include "SearchNewDanyinP.h"
#include "NuroClasses.h"
#include "stdio.h"
 #include "locale.h"
#define  NDY  
//#define ZENRIN
//#include "windows.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DY_SEARCH_MODE_NO					0x00//渣昫腔脤戙
#define DY_SEARCH_MODE_RESTART				0x01//笭陔A植0弇离羲宎刲坰
#define DY_SEARCH_MODE_FORWARD				0x02//等秞趼睫崝樓ㄛ砃刲坰
#define DY_SEARCH_MODE_BACKWARD				0x03//等秞趼睫熬屾ㄛ剒猁砃刲坰
#define DY_SEARCH_MODE_KEEP					0x04//等秞趼睫羶衄曹趙ㄛ祥剒猁刲坰

CSearchNewDanyinP::CSearchNewDanyinP()
{
	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_wsName,0,sizeof(m_wsName));		//Prosper add by 20121216 ,  
//	nuMemset(sTemp,0,sizeof(sTemp));
//	nTempTotal=0;
//	bWrite=true;
	
    //wsListName = NULL;
#ifndef VALUE_EMGRT 
	m_bNameSort = nuTRUE; 
	m_bSort = nuFALSE;
	m_bSortCity = nuFALSE;
#else
	m_bNameSort = nuFALSE;
#endif
	m_nNDYMemLenIdx   = -1;
	m_nNDYIDMemLenIdx = -1;
	//nALLDanyinLen = NULL;
	//m_IDInfo = NULL;
	m_nTotalLen = 0;
	//#ifdef ZENRIN
	nSameNameIdx = 0;
	wsTempName = NULL;
	nSameCity  = NULL;
	nSameTown  = NULL;
	nSameNameIdx = 0;	
	m_wsNameList= NULL;

	m_nSKBROADMMIdx = m_nSKBROADMMIdx2 = m_nSKBPOIMMIdx = m_nSKBPOIMMIdx2 = -1;
	m_bSetAreaCenterPos = false;
	nuMemset(&m_TypeCenterPos,0,sizeof(m_TypeCenterPos));
	//#endif
}

CSearchNewDanyinP::~CSearchNewDanyinP()
{
	Free();
}

nuBOOL CSearchNewDanyinP::Initialize()
{
	InitSearchSet();
	if( !m_fileNewDY.Initialize() )
	{
		return nuFALSE;
	}
	m_nShDataMemIdx = -1;
	m_bReSearchStroke = nuTRUE;
	//nuMemset(m_wsName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  

	nuMemset(m_nSearchPoiKeyName,0,sizeof(m_nSearchPoiKeyName));		//Prosper add by 20121216 ,  
	nuMemset(m_nSearchPoiDanyinName,0,sizeof(m_nSearchPoiDanyinName));
	return nuTRUE;
}

nuVOID CSearchNewDanyinP::Free()
{
	if( m_nShDataMemIdx != -1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nShDataMemIdx);
	}	
	if(m_nNDYMemLenIdx !=-1)
	{
		m_pMmApi->MM_RelDataMemMass(&m_nNDYMemLenIdx);
	}
	if(m_nNDYIDMemLenIdx !=-1)
	{
		m_pMmApi->MM_RelDataMemMass(&m_nNDYIDMemLenIdx);
	}
	m_fileNewDY.Free();
	/*if(wsListName)
	{
		delete[] wsListName;
		wsListName = NULL;
	}*/
	/*if(nALLDanyinLen)
	{
		delete[] nALLDanyinLen;
		nALLDanyinLen = NULL;
	}
	if(m_IDInfo)
	{
		delete[] m_IDInfo;
		m_IDInfo = NULL;
	}*/
//	#ifdef ZENRIN
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
	if(m_wsNameList)
	{
		delete[] m_wsNameList;
		m_wsNameList = NULL;
	}
//	#endif
}

nuUINT CSearchNewDanyinP::SetSearchRegion(nuBYTE nSetMode, nuDWORD nData)
{
	if( nSetMode == SEARCH_DY_SET_MAPIDX )
	{
		m_searchSet.nMapIdx = nuLOWORD(nData);
#ifdef NDY
		if( !m_fileNewDY.SetMapID(m_searchSet.nMapIdx) )
		{
			return 0;
		}
		nuMemset(m_nCityTownID,0,sizeof(m_nCityTownID));
		m_fileNewDY.GetTownIDInfo(m_nCityTownID);

#endif

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
		m_searchSet.nCityID = nuLOWORD(nData);;
		#ifdef NDY
			m_nCityID = nData;
			return nuTRUE;
		#endif
		return 1;

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

		m_searchSet.nTownID = nuLOWORD(nData);;
		/*if( !m_fileNewDY.SetID(m_searchSet.nCityID,m_searchSet.nTownID) )
		{
			return 0;
		}*/
		#ifdef NDY
			m_nTownID = nData;


			if( !m_fileNewDY.SetID(m_nCityID,m_nTownID) )
			{
				return 0;
			}
			if( !m_fileNewDY.LoadData() )
			{
				return 0;
			}
			return nuTRUE;
		#endif

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

nuUINT CSearchNewDanyinP::SetSearchDanyin(nuWCHAR *pWsSH, nuBYTE nWsNum)
{ 
	 m_bSort = nuTRUE;

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
	nuMemset(m_nWord,-1,sizeof(m_nWord));
	nuMemset(m_nIdx,0,sizeof(m_nIdx));

	m_nIndex = 0;
	

	nuMemset(m_wsName, 0, sizeof(m_wsName));
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));

	nuWcsncpy(m_wsName, pWsSH, NewDY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiDanyinName, pWsSH, NewDY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
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
nuUINT CSearchNewDanyinP::SetSearchDanyin2(nuWCHAR *pWsSH, nuBYTE nWsNum)
{ 
	 m_bSort = nuFALSE;

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

	nuWcsncpy(m_wsName, pWsSH, NewDY_MAX_WCHAR_NUM - 1);
	nuWcsncpy(m_nSearchPoiDanyinName, pWsSH, NewDY_MAX_WCHAR_NUM - 1); //ADD PROSPER 20121216
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
nuBOOL CSearchNewDanyinP::SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging)
{
	m_bSortCity= nuTRUE;
	nuMemset(m_nSearchPoiDanyinName, 0, sizeof(m_nSearchPoiDanyinName));//
	nuMemset(m_nSearchPoiKeyName, 0, sizeof(m_nSearchPoiKeyName));
	if (!pKeyWord)  
	{
		return nuFALSE; 
	}
	 nuWcscpy(m_nSearchPoiDanyinName, pKeyWord);
	 m_bNameSort = nuTRUE;

	 return nuTRUE;
}


nuUINT CSearchNewDanyinP::SetSearchKEY(nuWCHAR *pWsSH, nuBYTE nWsNum,nuBOOL bres)
{
	bFindALL = bres;
	m_bSortCity = nuTRUE;

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
		m_fileNewDY.GetOneDYData2(&temp, pShData->pIndexList[i]);
		nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		
	}
	NAME_Order(nTotal);
	*/
	return 1;
}
nuBOOL CSearchNewDanyinP::SearchDY_B(nuBYTE nShMode)
{
	if(m_wsNameList == NULL)
	{
		m_wsNameList= new Trans_DATANAME[NDY_MAX_KEPT_RECORD];
		nuMemset(m_wsNameList,0,sizeof(Trans_DATANAME)*NDY_MAX_KEPT_RECORD);
	}
	m_nTotalLen = 0;
	/*if( nShMode == DY_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}*/
	nNEXT_WORD_COUNT = 0;
	nuMemset(m_wsNEXT_WORD_Data,0,sizeof(m_wsNEXT_WORD_Data));
	
	nuDWORD nTotalCount;
	nuWCHAR* pDanyinA = (nuWCHAR*)m_fileNewDY.GetDanyinB(&nTotalCount);//??皞怠?pShData?喉?
	
	if(m_nNDYMemLenIdx == (nuWORD)-1)
	{
		nuPBYTE pData = NULL;
		pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)nTotalCount, &m_nNDYMemLenIdx);
		m_fileNewDY.GetDanyinLen2(pData);
	}
	if(m_nNDYIDMemLenIdx ==(nuWORD) -1)
	{
		PPDY_D_NODE pData2 = NULL;
		pData2=(PPDY_D_NODE)m_pMmApi->MM_GetDataMemMass((nuLONG)nTotalCount*sizeof(PDY_D_NODE), &m_nNDYIDMemLenIdx);
		m_fileNewDY.GetIDInfo(pData2);
	}
	if(m_nNDYMemLenIdx == (nuWORD)-1 || m_nNDYIDMemLenIdx ==(nuWORD) -1)
	{
		 __android_log_print(ANDROID_LOG_INFO, "NRN", "LOAD FAIL MEM Not Enough");
		return nuFALSE;
	}
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL || pDanyinA == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;//20090109.
	nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	if( true||nShMode == DY_SEARCH_MODE_RESTART || nShMode == DY_SEARCH_MODE_BACKWARD )//笭陔植A梑
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
		nuDWORD i  = 0;
		nuDWORD T1 = nuGetTickCount();
		for(i = nAIdxStart ; i < nTotalCount; ++i)
		{
			//			MessageBoxW(NULL, (nuWCHAR*)pDanyinA[i].pName, L"", 0);
			SearchDY_B_WSCMP((nuWCHAR*)pDanyinA, i, pShData);
			//SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[i].nName, i, pShData);
		}
		nuDWORD T2 = nuGetTickCount();
					 
                                        __android_log_print(ANDROID_LOG_INFO, "SKB", "LOAD TIME %d",T2-T1);
		int k=0;
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

		/*	SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[ pShData->pIndexList[i] ].nName, 
				pShData->pIndexList[i], 
				pShData );
		*/
		}
		if(  nFoundDataMode == DY_FOUNDDATA_MODE_FIND_PART )
		{
			for( i = nLastIdx; i < nTotalCount; ++i )
			{
				//SearchDY_B_WSCMP((nuWCHAR*)pDanyinA[i].nName, i, pShData);
			}
		}
	}
	m_bReSearchStroke = nuTRUE;
	return nuTRUE;
}
nuBOOL CSearchNewDanyinP::SearchDY_A(nuBYTE nShMode)
{
	//#ifdef ZENRIN
		nSameNameIdx = 0;
		if(NULL == wsTempName)
		wsTempName = new Trans_DATANAME[NDY_MAX_KEPT_RECORD];
		if(NULL == nSameCity)
		nSameCity  = new nuINT [NDY_MAX_KEPT_RECORD];
		if(NULL == nSameTown)
		nSameTown  = new nuINT [NDY_MAX_KEPT_RECORD];
		if(NULL == wsTempName || NULL == nSameCity || NULL == nSameTown)
		{
			 __android_log_print(ANDROID_LOG_INFO, "NRN", "NEW FAIL LA!!!!!");
			return false;
		}

		nuMemset(wsTempName,0,sizeof(Trans_DATANAME)*NDY_MAX_KEPT_RECORD);
		nuMemset(nSameCity,0,sizeof(nuINT)*NDY_MAX_KEPT_RECORD);
		nuMemset(nSameTown,0,sizeof(nuINT)*NDY_MAX_KEPT_RECORD);

	//#endif 
//	nShMode = DY_SEARCH_MODE_RESTART; 
	/*if( nShMode == DY_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}*/
	nuDWORD nTotalCount = 0;
	nuWCHAR *pDanyinA = (nuWCHAR*)m_fileNewDY.GetDanyinA(&nTotalCount);//郔疑溫婓pShData眳
	
	if(m_nNDYMemLenIdx == (nuWORD)-1)
	{
		nuPBYTE pData = NULL;
		pData=(nuPBYTE)m_pMmApi->MM_GetDataMemMass((nuLONG)nTotalCount, &m_nNDYMemLenIdx);
		m_fileNewDY.GetDanyinLen2(pData);
	}
	
	/*if(nALLDanyinLen == NULL)
	{
		nALLDanyinLen = new nuBYTE [nTotalCount];
		nuMemset(nALLDanyinLen,0,nTotalCount);
		m_fileNewDY.GetDanyinLen2(pData);	
	}*/
	/*
	if(m_IDInfo == NULL)
	{
		m_IDInfo = new PDY_D_NODE [nTotalCount];
		nuMemset(m_IDInfo,0,sizeof(m_IDInfo)*nTotalCount);
		m_fileNewDY.GetIDInfo(m_IDInfo);
	}
	*/
	
	if(m_nNDYIDMemLenIdx ==(nuWORD) -1)
	{
		PPDY_D_NODE pData2 = NULL;
		pData2=(PPDY_D_NODE)m_pMmApi->MM_GetDataMemMass((nuLONG)nTotalCount*sizeof(PDY_D_NODE), &m_nNDYIDMemLenIdx);
		m_fileNewDY.GetIDInfo(pData2);
	}
	if(m_nNDYMemLenIdx == (nuWORD)-1 || m_nNDYIDMemLenIdx ==(nuWORD) -1)
	{
		 __android_log_print(ANDROID_LOG_INFO, "NRN", "LOAD FAIL MEM Not Enough");
		return nuFALSE;
	}
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData == NULL || pDanyinA == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;//20090109.
	nuMemset(&m_keyMask, 0, sizeof(m_keyMask));
	nTime = 0;
	g_time=0;
	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	if( true || nShMode == DY_SEARCH_MODE_RESTART || nShMode == DY_SEARCH_MODE_BACKWARD )//笭陔植A梑
	{
		nuDWORD nAIdxStart;
		if( m_nASIdxCount == 0 || m_nTotalLen >0)
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
		if(m_nTotalLen >0)
		{
			m_nTotalLen = 0;
			//m_nASIdxCount = 0;
			//pDanyinA   -= m_nTotalLen;
		}

		nuDWORD n3 = nuGetTickCount();
		for(nuDWORD i = nAIdxStart ; i < nTotalCount; ++i)
		{
			//			MessageBoxW(NULL, (nuWCHAR*)pDanyinA[i].pName, L"", 0);
			SearchDY_A_WSCMP(pDanyinA, i, pShData);
		}
		nuDWORD n4 = nuGetTickCount();
		int l=0;

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
			SearchDY_A_WSCMP2(pDanyinA, 
				pShData->pIndexList[i],
				pShData->pLenList[i],
				pShData );
		}
		if( nFoundDataMode == DY_FOUNDDATA_MODE_FIND_PART )
		{
			for( i = nLastIdx; i < nTotalCount; ++i )
			{
				//SearchDY_A_WSCMP((nuWCHAR*)pDanyinA[i].pName, i, pShData);
			}
		}
	}
	
	m_bReSearchStroke = nuTRUE;
	return nuTRUE;
}
int nLong = 0;
nuVOID CSearchNewDanyinP::SearchDY_A_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData)
{   
	
	nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMassAddr(m_nNDYMemLenIdx);
	PPDY_D_NODE pData2 =(PPDY_D_NODE)m_pMmApi->MM_GetDataMassAddr(m_nNDYIDMemLenIdx);

	nuWCHAR wsComParedData[128]={0};
	nuDWORD t1 = nuGetTickCount();
	//nuINT nDanyinLen //= pShData->pLenList[idx];
	nuINT nDanyinLen  = pData[idx];//nALLDanyinLen[idx];//m_fileNewDY.GetDanyinLen(idx,m_nTotalLen,false);
	nuDWORD t2 = nuGetTickCount();
	nTime += t2-t1 ;
	pWsDY+=m_nTotalLen;

	nuINT nLen = nDanyinLen;
	if(nLen>nLong)
		nLong = nLen;
	if(pData == NULL || pData2 == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "NRN", "pData == NULL || pData2 == NULL ");
		return ;
	}
	if(m_nCityID > -1)
	{
		if(pData2[idx].nCityID!= m_nCityID)
		{
			m_nTotalLen += nLen;
			return;
		}
	}

	if(m_nTownID > -1)
	{
		if(pData2[idx].nTownID-m_nCityTownID[m_nCityID]!= m_nTownID)
		{
			m_nTotalLen += nLen;
			return;
		}
	}
	nuINT ll=0;
	nuMemcpy(wsComParedData,pWsDY,nDanyinLen*2);
    //nuMemcpy(wsDATA[idx],pWsDY,nDanyinLen*2);
	//OutputDebugStringA(wsComParedData);//,idx,nDanyinLen,wsComParedData);

	for(nuINT l=0;l<nDanyinLen;l++)
	{
		if(wsComParedData[l]>47&& wsComParedData[l]<58 
					|| wsComParedData[l]>64 && wsComParedData[l]<91
					|| wsComParedData[l]>96 && wsComParedData[l]<123
					|| wsComParedData[l]>12398 )
		{
			wsComParedData[ll]=wsComParedData[l];
			ll++;
		}
	}
	if(ll<NewDY_B_NAME_LEN/2)
	wsComParedData[ll]=0;
	else
	{
		int k=0;
	}

	if(ll!=nDanyinLen)
	{
		int  o = 0;
	}

	nDanyinLen = nuWcslen(wsComParedData);

  
	nuBOOL bHaveKept = nuFALSE;
/*	nuWCHAR s[52] = {0};
	nuWcsncpy(s,pWsDY,nDanyinLen);
	int l=0;*/

	for(nuBYTE j = 0; j < nDanyinLen; j++)
	{
		if( nDanyinLen - j < m_nNameNum )
		{
			break;
		}
		//
		if( wsComParedData[j] == m_wsName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum ; ++k )
			{
				if( wsComParedData[k+j] != m_wsName[k] )
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
				/*
				nuBOOL bRes = m_fileNewDY.CheckRoadSame(m_nTotalLen,nLen,idx);
				if(!bRes)
				{
					break;
				}*/
				if( k+j < nDanyinLen /*&& pWsDY[k+j] != NULL*/ )
				{
				/*	if(!m_fileNewDY.CheckID(idx,m_nCityID,m_nTownID))
					{
						break;
					}
				*/	AddKeyMask(wsComParedData[k+j]);
				}
				//
				if( !bHaveKept )
				{
					++pShData->nTotalFound;
					if( pShData->nKeptCount < NDY_MAX_KEPT_RECORD-1 ) 
					{
						
						m_nWordStartIdx[pShData->nKeptCount] = j;
						pShData->pLenList[pShData->nKeptCount]= m_nTotalLen;
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						//(nuWcslen(m_wsName)>1)
						//nuINT nDanyinLen = m_fileNewDY.GetDanyinLen(idx,m_nTotalLen,true);
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

	m_nTotalLen += nLen;//nDanyinLen;
}

nuVOID CSearchNewDanyinP::SearchDY_A_WSCMP2(nuWCHAR *pWsDY, nuDWORD idx, nuDWORD nStartLen,PDYSEARCHDATA pShData)
{
	
	nuINT nDanyinLen = m_fileNewDY.GetDanyinLen(idx,m_nTotalLen,false);
	pWsDY+=nStartLen;

	nuBOOL bHaveKept = nuFALSE;
	//nuWCHAR s[52] = {0};
	//nuWcsncpy(s,pWsDY,nDanyinLen);
	int l=0;

	for(nuBYTE j = 0; j < nDanyinLen; j++)
	{
		if( nDanyinLen - j < m_nNameNum )
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
				if( k+j < nDanyinLen /*&& pWsDY[k+j] != NULL*/ )
				{
					AddKeyMask(pWsDY[k+j]);
				}
				//
				if( !bHaveKept )
				{
					++pShData->nTotalFound;
					if( pShData->nKeptCount < NDY_MAX_KEPT_RECORD -1 ) 
					{
						m_nWordStartIdx[pShData->nKeptCount] = j;
						pShData->pLenList[pShData->nKeptCount]= m_nTotalLen;
						pShData->pLenList[pShData->nKeptCount]= nStartLen;
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						//if(nuWcslen(m_wsName)>1)
						//nuINT nDanyinLen = m_fileNewDY.GetDanyinLen(idx,nStartLen,true);
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
	pWsDY-=nStartLen;
}
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
//nuWCHAR wsNAME[289864][50] = {0};
nuVOID CSearchNewDanyinP::SearchDY_B_WSCMP(nuWCHAR *pWsDY, nuDWORD idx, PDYSEARCHDATA pShData)
{
	if(!bFindALL&&nNEXT_WORD_COUNT>=9)
	{
		return;
	}

	nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMassAddr(m_nNDYMemLenIdx);
	PPDY_D_NODE pData2 =(PPDY_D_NODE)m_pMmApi->MM_GetDataMassAddr(m_nNDYIDMemLenIdx);

	pWsDY+=m_nTotalLen;
	nuWCHAR s[128]={0};
	nuDWORD t1 = nuGetTickCount();
	//nuINT nDanyinLen //= pShData->pLenList[idx];
	nuINT nNameLen  = pData[idx];
	nuINT nDataLen  = pData[idx];
	nuINT ll = 0;
	if(m_nCityID > -1)
	{
		if(pData2[idx].nCityID!= m_nCityID)
		{
			m_nTotalLen += nNameLen;
			return;
		}
	}
	if(m_nTownID > -1)
	{
		if(pData2[idx].nTownID-m_nCityTownID[m_nCityID]!= m_nTownID)
		{
			m_nTotalLen += nNameLen;
			return;
		}
	}
	nuWCHAR wsComParedData[128]={0};
	nuMemcpy(wsComParedData,pWsDY,nNameLen*2);
	
	for(nuINT l=0;l<nNameLen;l++)
	{
		if(wsComParedData[l]>47&& wsComParedData[l]<58 
					|| wsComParedData[l]>64 && wsComParedData[l]<91
					|| wsComParedData[l]>96 && wsComParedData[l]<123
					|| wsComParedData[l]>12398 )
		{
			wsComParedData[ll]=wsComParedData[l];
			ll++;
		}
	}
	if(ll<NewDY_B_NAME_LEN/2)
	wsComParedData[ll]=0;
	else
	{
		int k=0;
	}
	nNameLen = nuWcslen(wsComParedData);

	if(nNameLen!=nDataLen)
		int k=0;

	int nLastIdx = 0;
	//nuMemcpy(wsNAME[idx],wsComParedData,nNameLen*2);

	/*
	nuINT sLen=nuWcslen(pWsDY);
	bool bFindTag=false;
	for(int i=0;i<sLen;i++)
	{
		if(pWsDY[i]==45&&i>2)
		{
			pWsDY[i]=0;
			bFindTag=true;
			break;
		}
	}
	if(bFindTag)
	{

	if(nuWcscmp(sTemp,pWsDY)==0)
	{
		nTempTotal++;
	}
	else
	{
		nuWcscpy(sTemp,pWsDY);
		nTempTotal=0;
		bWrite=true;
	}

	}
	if(nTempTotal>3 && bWrite)
	{
		bWrite=false;
		FILE *fp = fopen("c:\\nameSTR.txt","a+");
		setlocale(LC_ALL, "");
		wchar_t myString[40]={0}; 
		fwprintf(fp,L"%ls\n", sTemp);
		fclose(fp);
	}
	*/

	nuBYTE nMaxNum = DY_A_NAME_LEN/sizeof(nuWCHAR);
	nuBOOL bHaveKept = nuFALSE;
	for(nuBYTE j = 0; j < nNameLen; ++j)
	{
		if( pWsDY[j] == NULL || nMaxNum - j < m_nNameNum || bHaveKept)
		{
			break;
		}
		nuBYTE k =1;
		nuBOOL bFuzzy = nuTRUE;		
		//
		if( wsComParedData[j] == m_wsName[0] )
		{
			
			if(bFuzzy)
			{
				for( nuINT l = j+1; l < nNameLen;  l++)
				{
					if(wsComParedData[l]== m_wsName[k] )
					{
						k++;
						nLastIdx = l+1;
					}
				}
				//j = m_nNameNum;
				if(k!=m_nNameNum)
				{
					m_nTotalLen += nDataLen;
					return ;
				}
			}
			else
			{
				for( k = 1; k < m_nNameNum,(k+j)<nMaxNum ; )
				{
					if( wsComParedData[k+j] == m_wsName[k] )
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
				{
					m_nTotalLen += nDataLen;
					return ;
				}
			}
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
					/*nuBOOL bRes = m_fileNewDY.CheckRoadSame(wsComParedData,nNameLen,idx);
					if(!bRes)
					{
						break;
					}*/
					
					if(nNEXT_WORD_COUNT<NEXT_WORD_MAXCOUNT && wsComParedData[nLastIdx]!=0) //j+m_nNameNum
					{
						nuBOOL bSame = nuFALSE;
						for(nuINT jj=0;jj<nNEXT_WORD_COUNT;jj++)
						{
							if( m_wsNEXT_WORD_Data[jj] == wsComParedData[nLastIdx])
							{
								bSame = nuTRUE;
								break;
							}
						}
						if(!bSame) 
						{
							m_wsNEXT_WORD_Data[nNEXT_WORD_COUNT++]= wsComParedData[nLastIdx];
						}
						int l=0;
					}

					++pShData->nTotalFound;
					if( pShData->nKeptCount < NDY_MAX_KEPT_RECORD-1 )
					{
						m_nWordStartIdx[pShData->nKeptCount] = j;
						pShData->pLenList[pShData->nKeptCount]= m_nTotalLen;
						nuMemcpy(&m_wsNameList[pShData->nKeptCount],pWsDY,nNameLen*2);
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
	m_nTotalLen += nDataLen;
}

nuUINT CSearchNewDanyinP::GetDYDataHead(PDYCOMMONFORUI pDyCommonForUI)
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

nuUINT CSearchNewDanyinP::GetDYDataPage(PNDYDETAILFORUI pDyDataForUI, nuINT &nCount)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyDataForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	//
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

	nuINT nTotal = pShData->nKeptCount;

	
	

	pIdxList = pShData->pIndexList;

	for(nuDWORD i = 0; i <  pShData->nKeptCount; ++i)
	{
		//NDYDETAILFORUI temp = {0};
		if( !m_fileNewDY.GetOneDYDetail(&pDyDataForUI[i], pIdxList[i]) )
		{
			continue;
		}
		nuWcscpy(pDyDataForUI[i].wsName,m_wsNameList[i].name);
		
		
	}

	return 1;
}

nuUINT CSearchNewDanyinP::GetDYGetWordData(PDYWORDFORUI pDyData)
{
	
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if(pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
	#ifdef NDY
	nuDWORD  *pLenList=NULL;
	#endif
	
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
		#ifdef NDY
		pLenList = pShData->pLenList;
		#endif
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
		#ifdef NDY
		pLenList = pShData->pLenList;
		#endif
	}
	else
	{
		#ifdef NDY
		pLenList = pShData->pLenList;
		#endif
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
	}
	//
	
	//
	nuINT j=0,nIdx=0;
	nuINT n_Len=nuWcslen(m_wsName);

	nuINT x=-1; //Just Clear Data 
	m_fileNewDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x,m_wsName,NULL);
	
	nuMemset(m_nWord,-1,sizeof(m_nWord));
	nuDWORD t1 =nuGetTickCount();
	if(m_nSearchPoiDanyinName)
	{
		nuINT nLimit = 0;
		
		for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
		{
			nuINT n=0; 
			if( !m_fileNewDY.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&pDyData->Data[nIdx], pIdxList[i],n,m_wsName,pLenList[i]) )
			{
				if(n==-1)
				{
					m_nWord[i]=-1;
				//	m_nIdx[i]=-1;
					continue;
				}
				if(pDyData->Data[n].nFindCount<NDY_CANDYWORD_LIMIT-1)
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
			if(pDyData->Data[nIdx].nFindCount<NDY_CANDYWORD_LIMIT-1)
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
nuDWORD t2 =nuGetTickCount();

nuDWORD t3 =t2-t1;
nuDWORD N1 =nuGetTickCount();
	
	if(m_bSort)
	{
		
		m_bSort = nuFALSE;

	DYWORD temp={0};
	//Bubble
	for(nuINT nn=0;nn<nIdx-1;nn++)
	{
		for (nuINT j=0; j<nIdx-nn; j++)
		{ 
			
			if((int)pDyData->Data[j].nFindCount < (int)pDyData->Data[j+1].nFindCount)
			{
				temp = pDyData->Data[j];
				pDyData->Data[j] = pDyData->Data[j+1];
				pDyData->Data[j+1] = temp;
			} 

		}
	}

	}
	nuDWORD N2 =nuGetTickCount();
	pDyData->nTotalCount = nIdx;	
	nuDWORD N3= N2-N1;
	return 1;
}
nuUINT CSearchNewDanyinP::GetDYDataPage_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyDataForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
	nuDWORD  *pAddrList = NULL;
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
		pAddrList = pShData->pLenList;
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
		pAddrList = pShData->pLenList;
	}
	else
	{
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
		pAddrList = pShData->pLenList;
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
	//m_fileNewDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x,m_wsName,NULL);
	//#ifdef ZENRIN
		nSameNameIdx=0;
	//#endif
	//DYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	//nuINT n[1000]={0};
	//nuINT nn[1000]={0};

	if(m_bNameSort && nuFALSE)
	{
		nuDWORD t1 =nuGetTickCount();
		m_bNameSort = nuFALSE;
	
		for(nuINT i=0;i<nCount ;i++)
		{
			DYNODEFORUI temp = {0};
			m_fileNewDY.GetOneDYData_GETNAME(&temp, pShData->pIndexList[i],pShData->pLenList[i]);
			//nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		}
		//NAME_Order(nCount);

		nuDWORD t2 =nuGetTickCount();
		nuDWORD t3 =t2-t1;
		int l=0;
	}

	if(0&&m_bSortCity)
	{
		m_bSortCity =nuFALSE;
	nuINT tempIdx=0;
	DYNODEFORUI tempData = {0};
	DYNODEFORUI *temp  = new DYNODEFORUI[nCount];
	nuMemset(temp,0,sizeof(DYNODEFORUI)*nCount);

	for(nuDWORD i = 0; i < nCount;i++)
	{
		m_fileNewDY.GetOneDYData_GETNAME(&temp[i], pShData->pIndexList[i] , pShData->pLenList[i]);
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
	nuINT nComIdx=m_fileNewDY.GetWordIndx(m_nSearchPoiKeyName);
	
	nuMemset(pDyDataForUI,0,sizeof(pDyDataForUI));

	if(1)//!m_bNew)
	{

	for(nuDWORD i = 0; i < nCount;i++)
	{
		//prosper add for faster search 2013 04 16 
		if(m_nWord[i]!=nComIdx || m_nWord[i]==-1)
		{
			continue;
		}
		if(nIdx == nItemIdx)
			int k=0;
		if( !m_fileNewDY.GetOneDYData_PAGE(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount],pIdxList[i],m_nSearchPoiKeyName,m_nWordStartIdx[i] ,pAddrList[i]))
		{
			continue;
			//break;
		}
	//	#ifdef ZENRIN
		DYNODEFORUI temp = {0};
		m_fileNewDY.GetOneDYData_GETNAME(&temp, pIdxList[i],pAddrList[i]);
		
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
//#endif

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
		m_fileNewDY.GetOneDYData_GETNAME(&temp, m_nIdx[i] ,pAddrList[i]);
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
	nuDWORD t1 =nuGetTickCount();
	if( m_bReSearchStroke )
	{
		SearchDY_Stroke(pIdxList, nCount);
		m_bReSearchStroke = nuFALSE;
	}
	nuDWORD t2 =nuGetTickCount();
	nuDWORD t3 =t2-t1;
	int l=0;
	pDyDataForUI->nMinStroke = pShData->nMinStroke;
	pDyDataForUI->nMaxStroke = pShData->nMaxStroke;
	return 1;
}



nuUINT CSearchNewDanyinP::GetDYALLData(nuPVOID buffer,nuINT &nCopyCount)//PDYDATAFORUI pDyDataForUI, nuINT nCou)nt
{
	PNDYDETAILFORUI pDyDataForUI = (PNDYDETAILFORUI)buffer;
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pDyDataForUI == NULL || pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
	nuDWORD  *pAddrList = NULL;
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
		pAddrList = pShData->pLenList;
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
		pAddrList = pShData->pLenList;
	}
	else
	{
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
		pAddrList = pShData->pLenList;
	}
	nuINT nComIdx = 0;
	if(nuWcslen(m_nSearchPoiDanyinName)>0)
	{
		nComIdx = m_fileNewDY.GetWordIndx(m_nSearchPoiDanyinName);
	}

		
	nuINT nIdx = 0; 
	nuINT j = 0;
	for(nuDWORD i = 0; i < nCount;i++)
	{
		

		//prosper add for faster search 2013 04 16 
		//PDYNODEFORUI temp = new DYNODEFORUI[1];
		if(nuWcslen(m_nSearchPoiDanyinName)>0)
		{
			if(m_bSort)
			{
				if(/*m_bFirstOnly&&*/m_nWord[i]!=nComIdx || m_nWord[i]==-1) ////Prosper add 20130807
				{
					continue;
				}
			}
		/*	if(m_bFirstOnly&&m_nWord[i]!=nComIdx || m_nWord[i]==-1) ////Prosper add 20130807
			{
				continue;
			}
		*/
			DYNODEFORUI temp  = {0};

			if( !m_fileNewDY.GetOneDYData_PAGE(&temp,pIdxList[i],m_nSearchPoiDanyinName,m_nWordStartIdx[i] ,pAddrList[i]))
			{
				continue;
				//break;
			}
			
			if( !m_fileNewDY.GetOneDYDetail(&pDyDataForUI[j], pIdxList[i]) )
			{
				continue;
			}
			nuWcscpy(pDyDataForUI[j].wsName , temp.wsName);
			m_nCandyWordStartIdx[j] =m_nWordStartIdx[i];
			if(m_bSetAreaCenterPos)
			{
				nuDOUBLE DisX = 0;
				nuDOUBLE DisY = 0;
				nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
				nuDOUBLE x1,x2,y1,y2;
				if(pDyDataForUI[j].point.x>0&&pDyDataForUI[j].point.y>0)
				{
					x1=pDyDataForUI[j].point.x;
					y1=pDyDataForUI[j].point.y;
					x2=m_TypeCenterPos.x;
					y2=m_TypeCenterPos.y;
					DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
					DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
					nuLONG nowLen = DisX+DisY;
					pDyDataForUI[j].nDistance = nowLen; 
				}
			}
			j++;
			int l=0;
		}
		else
		{
				DYNODEFORUI temp  = {0};
			if( !m_fileNewDY.GetOneDYData_PAGE(&temp,pIdxList[i],m_nSearchPoiKeyName,m_nWordStartIdx[i] ,pAddrList[i]))
			{
				continue;
			}
			if( !m_fileNewDY.GetOneDYDetail(&pDyDataForUI[j], pIdxList[i]) )
			{
				continue;
			}
			nuWcscpy(pDyDataForUI[j].wsName , temp.wsName);
			m_nCandyWordStartIdx[j] =m_nWordStartIdx[i];
			if(m_bSetAreaCenterPos)
			{
				nuDOUBLE DisX = 0;
				nuDOUBLE DisY = 0;
				nuDOUBLE LatEffect = getLatEffect(m_TypeCenterPos);
				nuDOUBLE x1,x2,y1,y2;
				if(pDyDataForUI[j].point.x>0&&pDyDataForUI[j].point.y>0)
				{
					x1=pDyDataForUI[j].point.x;
					y1=pDyDataForUI[j].point.y;
					x2=m_TypeCenterPos.x;
					y2=m_TypeCenterPos.y;
					DisX = NURO_ABS(x1-x2)*LONEFFECTCONST*LatEffect/EFFECTBASE;
					DisY = NURO_ABS(y1-y2)*LATEFFECTCONST/EFFECTBASE; //Clare 2010.5.5
					nuLONG nowLen = DisX+DisY;
					pDyDataForUI[j].nDistance = nowLen; 
				}
			}	
			j++;
		}
			nIdx++;
	}
	//Sort by name index , prosper add 2016.08.11
	for(nuDWORD i = 0; i < j;i++)
	{
		for(nuDWORD ii = 0; ii < j-i-1;ii++)
		{
			if( m_nCandyWordStartIdx[ii]>m_nCandyWordStartIdx[ii+1]||
				m_nCandyWordStartIdx[ii]==m_nCandyWordStartIdx[ii+1] && nuWcslen(pDyDataForUI[ii].wsName)>nuWcslen(pDyDataForUI[ii+1].wsName))
			{
				nuINT nTemp = -1;
				nTemp = m_nCandyWordStartIdx[ii];
				m_nCandyWordStartIdx[ii] = m_nCandyWordStartIdx[ii+1];
				m_nCandyWordStartIdx[ii+1] = nTemp;

				NDYDETAILFORUI tempDataForUI = {0};

				tempDataForUI = pDyDataForUI[ii];
				pDyDataForUI[ii]=pDyDataForUI[ii+1];
				pDyDataForUI[ii+1]=tempDataForUI;
			}
		}
	}
	/*
	if(m_bSetAreaCenterPos && j>0)
	{
		for(int nIdx2=0;nIdx2<j-1;nIdx2++)
			for(int nIdx=0;nIdx<j-1;nIdx++)
			{
				NDYDETAILFORUI tmpData ={0} ;
				if(pDyDataForUI[nIdx].nDistance > pDyDataForUI[nIdx+1].nDistance)  
				{
						tmpData		=	pDyDataForUI[nIdx];
						pDyDataForUI[nIdx]	=	pDyDataForUI[nIdx+1];
						pDyDataForUI[nIdx+1]  =   tmpData;
				}
			}
		//m_bSetAreaCenterPos =false;
	}*/
	nCopyCount = j;

/*
	for(nuINT i=0; i < nCopyCount;i++)
	{
		DYNODEFORUI temp = {0};
		m_fileNewDY.GetOneDYData_GETNAME(&temp, m_nIdx[i] ,pAddrList[i]);
		nuWcscpy(pDyDataForUI[i].wsCityName,temp.wsCityName);
		nuWcscpy(pDyDataForUI[i].wsTownName,temp.wsTownName);
		nuWcscpy(pDyDataForUI[i].wsOther,temp.wsName);
		nIdx++;
	}
*/
	return 1;
}


//#ifdef ZENRIN
nuUINT CSearchNewDanyinP::GetDYData_COUNT()
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
	nuDWORD  *pAddrList = NULL;
	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
		pAddrList = pShData->pLenList;
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
		pAddrList = pShData->pLenList;
	}
	else
	{
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
		pAddrList = pShData->pLenList;
	}
	
	
	
	nuINT n_Len=nuWcslen(m_wsName);
	nuINT nIdx=0;
	nuINT x=-1;
	m_fileNewDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x,m_wsName,NULL);
	
	nSameNameIdx=0;
	
	if(m_bNameSort && nuFALSE)
	{
		nuDWORD t1 =nuGetTickCount();
		m_bNameSort = nuFALSE;
	
		for(nuINT i=0;i<nCount ;i++)
		{
			DYNODEFORUI temp = {0};
			m_fileNewDY.GetOneDYData_GETNAME(&temp, pShData->pIndexList[i] , pShData->pLenList[i]);
		//	nuMemcpy(wsListName[i].name,temp.wsName,sizeof(temp.wsName));
		}
	//	NAME_Order(nCount);

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
		m_fileNewDY.GetOneDYData_GETNAME(&temp[i], pShData->pIndexList[i] ,pShData->pLenList[i]);
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
		if( !m_fileNewDY.GetOneDYData_PAGE(&data,pIdxList[i],m_nSearchPoiKeyName,m_nWordStartIdx[i] ,pAddrList[i]))
		{
			continue;
			//break;
		}
		//#ifdef ZENRIN
		DYNODEFORUI temp = {0};
		m_fileNewDY.GetOneDYData_GETNAME(&temp, pIdxList[i] ,pAddrList[i]);
		
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
			nuWcscpy(wsTempName[nSameNameIdx].name,data.wsName);
			nSameCity[nSameNameIdx]=temp.nCityID;
			nSameTown[nSameNameIdx]=temp.nTownID;
			nSameNameIdx++;
		}
//#endif

		nIdx++;
	}

	return nSameNameIdx;
}
//#endif
nuBOOL CSearchNewDanyinP::SearchDY_City()
{
	nuDWORD nCount = 0;
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileNewDY.GetDanyinB2(&nCount);
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
			if( pShData->nCountOfCity >= NDY_MAX_CITY_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CSearchNewDanyinP::SearchDY_Town()
{
	nuDWORD nCount = 0;
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileNewDY.GetDanyinB2(&nCount);
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
			if( pShData->nCountOfTown >= NDY_MAX_TOWN_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuUINT CSearchNewDanyinP::GetDYDataStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileNewDY.GetDanyinB2(NULL);
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
			if( !m_fileNewDY.GetOneDYData(&pDyDataForUI->dyNodeList[pDyDataForUI->nThisPageCount], pIdxList[j]) )
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

nuVOID CSearchNewDanyinP::SearchDY_Stroke(nuDWORD *pIdxList, int nCount)
{
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2 pDyNodeB2 = (PDYNODE_B_2)m_fileNewDY.GetDanyinB2(NULL);
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

nuUINT CSearchNewDanyinP::GetDyDataDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	return m_fileNewDY.GetOneDYDetail(pDyDetailForUI, nIdx);
}
nuUINT CSearchNewDanyinP::GetDyCityTownName(PDYCITYTOWNNAME pDyCTname, long nCityID)
{
	nuDWORD nTotalCount = 0;
	nuWORD nCityCount = 0;
	nuWORD nTownCount = 0;
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	PDYNODE_B_2	pDyNodeB2 = (PDYNODE_B_2)m_fileNewDY.GetDanyinB2(&nTotalCount);
	nuPBYTE pDyNodeDE = (nuPBYTE)m_fileNewDY.GetDanyinDE(&nCityCount, &nTownCount);
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
nuVOID CSearchNewDanyinP::SearchDY_MapStart()
{
	nuDWORD nTotalCount = 0;
	PDANYIN_A pDanyinA = (PDANYIN_A)m_fileNewDY.GetDanyinA(&nTotalCount);
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if( pShData != NULL || pDanyinA != NULL )
	{
		for( nuDWORD i = 0; i < nTotalCount; ++i )
		{
			nuWCHAR *pWs = (nuWCHAR*)pDanyinA[i].pName;
			AddKeyMask(pWs[0]);
			if( pShData->nKeptCount < NDY_MAX_KEPT_RECORD-1 )
			{
				pShData->pIndexList[pShData->nKeptCount++] = i;
			}
		}
		pShData->nTotalFound = nTotalCount;
	}
}

nuBOOL CSearchNewDanyinP::VoicePoiSetMap(nuINT nIdx)
{
    if (!m_fileNewDY.OpenNewFile(nIdx))
    {
        return 0;
    }
	return true;
}

nuBOOL CSearchNewDanyinP::VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{	
	

	return nuTRUE;
}
nuBOOL CSearchNewDanyinP::VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx)
{

	DANYINHEAD header={0};
	m_fileNewDY.ReadData(0,&header,sizeof(DANYINHEAD));

	if(nIdx > header.nTotalCount || nIdx < 0 )
	{
		return nuFALSE;
	}
	DYNODE_B_2 temp = {0};
	long addr = 0;
	addr=sizeof(DANYINHEAD)+( sizeof(DANYIN_A) + sizeof(DYNODE_B_1)  )*header.nTotalCount +  nIdx*sizeof(DYNODE_B_2);

	m_fileNewDY.ReadData(addr,&temp,sizeof(temp));

	addr=header.nCityNameAddr +  DY_CITY_NAME_LEN * temp.nCityID ;
	m_fileNewDY.ReadData(addr,pCityName,DY_CITY_NAME_LEN);

	addr=header.nTownNameAddr +  DY_TOWN_NAME_LEN * temp.nTownID;
	m_fileNewDY.ReadData(addr,pTownName,DY_TOWN_NAME_LEN);

//	m_fileNewDY.CloseNewFile();
	
	
	return nuTRUE;
}
nuBOOL CSearchNewDanyinP::VoicePoiEnd()
{
	m_fileNewDY.CloseNewFile();
	return true;
}
nuUINT   CSearchNewDanyinP::SetSearchMode(nuINT nType)
{
	m_fileNewDY.SetSearchMode(nType);
	return nuTRUE;
}
nuINT CSearchNewDanyinP::GetPoiTotal()
{
	m_nIndex = 0;
	//m_bNew   = nuTRUE;
	nuMemset(&m_nIdx,0,sizeof(m_nIdx));
	nuINT n = 0;
	DYWORD *temp =NULL; 
	PDYSEARCHDATA pShData = (PDYSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nShDataMemIdx);
	if(pShData == NULL )
	{
		return 0;
	}
	//
	nuDWORD  nCount=0;
	nuDWORD  *pIdxList=NULL;
	#ifdef NDY
	nuDWORD  *pLenList=NULL;
	#endif

	if( pShData->nTownID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pTownIdxList;
		nCount	 = pShData->nCountOfTown;
	#ifdef NDY
		pLenList = pShData->pLenList;
	#endif
	}
	else if( pShData->nCityID != (nuWORD)(-1) )
	{
		pIdxList = pShData->pCityIdxList;
		nCount	 = pShData->nCountOfCity;
	#ifdef NDY
		pLenList = pShData->pLenList;
	#endif
	}
	else
	{
	#ifdef NDY
		pLenList = pShData->pLenList;
	#endif
		pIdxList = pShData->pIndexList;
		nCount	 = pShData->nKeptCount;
	}
	//

	//
	nuINT j=0,nIdx=0;
	nuINT n_Len=nuWcslen(m_wsName);

	nuINT x=-1; //Just Clear Data 
	//m_fileNewDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x,NULL,NULL);
	m_fileNewDY.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x,m_wsName,NULL);

	nuDWORD t1 = nuGetTickCount();
	//nuWCHAR test[]=L"美妝";
	if(m_nSearchPoiKeyName)
	{
		nuINT nLimit = 0;
		temp=new DYWORD[pShData->nKeptCount];
		nuMemset(temp,0,sizeof(DYWORD)*pShData->nKeptCount);
		for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
		{
		
			nuINT n=0;
			if( !m_fileNewDY.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&temp[nIdx], pIdxList[i],n,m_wsName,pLenList[i]) )
			{
				if(n==-1)
				{
					continue;
				}
				if(m_nIndex==NDY_CANDYWORD_LIMIT-1)
					continue;
				if(temp[n].nFindCount<NDY_CANDYWORD_LIMIT-1)
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
				if( !m_fileNewDY.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&pDyData->Data[nIdx], pIdxList[i],n) )
				{
					pDyData->Data[n].nFindCount++;
					continue;
				}
			}*/
			temp[nIdx].nFindCount++;
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
nuINT  CSearchNewDanyinP::GetTownNameAll(nuINT nCityIdx,nuVOID *pBuffer, nuUINT nMaxLen)
{
	nuMemset(pBuffer, 0, nMaxLen);
	return m_fileNewDY.GetTownNameAll(nCityIdx,pBuffer, nMaxLen);
}
nuINT CSearchNewDanyinP::GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	
	nuMemset(pBuffer, 0, nMaxLen);

	return m_fileNewDY.GetCityNameAll(pBuffer, nMaxLen);
}

nuBOOL CSearchNewDanyinP::GetNEXTAll(nuVOID* pKEYBuf, nuUINT& nCount)
{
	nuMemcpy(pKEYBuf,m_wsNEXT_WORD_Data,sizeof(m_wsNEXT_WORD_Data));
	nCount = nNEXT_WORD_COUNT;
	return nuTRUE;
}
nuVOID CSearchNewDanyinP::StopSKBSearch()
{
		m_SKBResT = 0;
        m_SKBLoadLen = 0;
        m_SKBMapIDT = -1;
        m_SKBCityIDT = -1;
        m_SKBTownIDT = -1;
        nuMemset(&infoT, 0, sizeof(infoT));
        nuMemset(m_SKBKeyStrT, 0, sizeof(m_SKBKeyStrT));
        nuMemset(m_SKBBuffer, 0, sizeof(m_SKBBuffer));

		
}
nuINT     CSearchNewDanyinP::GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount)
{
		nuMemset(pBuffer,0,nMaxLen);
        if (m_searchSet.nCityID==(nuWORD)-1)
        {
            return 0;
        }		
		if (   m_searchSet.nMapIdx  != m_SKBMapIDT   //彆剒猁刲坰
            || m_searchSet.nCityID != m_SKBCityIDT
            || m_searchSet.nTownID != m_SKBTownIDT
            || nuStrcmp(keyStr, m_SKBKeyStrT))
        {
            nuBOOL bReload = nuFALSE;
            m_SKBResT = 0;
            if (m_searchSet.nMapIdx != m_SKBMapIDT
                || m_searchSet.nCityID != m_SKBCityIDT)
            {
				if(m_nSKBROADMMIdx!= (nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&m_nSKBROADMMIdx);
				}
				if(m_nSKBROADMMIdx2!= (nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&m_nSKBROADMMIdx2);
				}
				if(m_nSKBPOIMMIdx!= (nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&m_nSKBPOIMMIdx);
				}
				if(m_nSKBPOIMMIdx2!= (nuWORD)-1)
				{
					m_pMmApi->MM_RelDataMemMass(&m_nSKBPOIMMIdx2);
				}
		m_pMmApi->MM_CollectDataMem(0);
		bReload = nuTRUE;
		__android_log_print(ANDROID_LOG_INFO, "SKB", "m_searchSet.nMapIdx %d %d  ",m_searchSet.nMapIdx , m_searchSet.nCityID);
            }
            nuINT keyLen = nuStrlen(keyStr);
            nuMemset(m_SKBBuffer, 0, sizeof(m_SKBBuffer));
			m_SKBMapIDT = m_searchSet.nMapIdx;
            m_SKBCityIDT = m_searchSet.nCityID;
            m_SKBTownIDT = m_searchSet.nTownID;
			nuMemset(m_SKBKeyStrT, NULL, sizeof(m_SKBKeyStrT));
            nuMemcpy(m_SKBKeyStrT, keyStr, keyLen);
			
            
			nuUINT count;
			nuWORD townID;
          
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
                
				nuUINT nRoadPos = infoT.roadPos;
				nuUINT nPoiPos = infoT.poiPos;
                nuUINT nTotlCount = infoT.roadCount + infoT.poiCount;   
				pos = infoT.roadPos;
				nuUINT m_SKBRoadLoadLen = 0 ,m_SKBPoiLoadLen = 0;
                  
				

                //nuCHAR *p = new nuCHAR[infoT.roadCount]; //GET LEN
		nuCHAR *pRoadB1 = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(infoT.roadCount*sizeof(nuCHAR), &m_nSKBROADMMIdx);
			   
                if (!skbfile.ReadData(nRoadPos, pRoadB1, infoT.roadCount))
                {
                    return 0;
                }
                nuUINT i = 0;
                for (; i<infoT.roadCount; ++i)
                {
                    m_SKBRoadLoadLen += pRoadB1[i]+2;
                }
				//nuCHAR *pRoad2 = new nuCHAR[m_SKBRoadLoadLen]; // GET DATA
                nuCHAR *pRoad2 = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(sizeof(nuCHAR)*m_SKBRoadLoadLen, &m_nSKBROADMMIdx2);
				//nuCHAR sNAME[47929]	={0};

		if (!skbfile.ReadData(nRoadPos+infoT.roadCount, pRoad2, m_SKBRoadLoadLen))
                {
                    m_SKBPoiLoadLen = 0;
                    return 0;
                }
				//nuMemcpy(sNAME,pRoad2,m_SKBRoadLoadLen);
				
				// nuCHAR *p2 = new nuCHAR[infoT.poiCount]; //GET LEN
		nuCHAR *pPoiB1 = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(infoT.poiCount*sizeof(nuCHAR), &m_nSKBPOIMMIdx);
			    
                if (!skbfile.ReadData(nPoiPos, pPoiB1, infoT.poiCount))
                {
                    return 0;
                }
                nuUINT i2 = 0;
                for (; i2<infoT.poiCount; ++i2)
                {
                    m_SKBPoiLoadLen += pPoiB1[i2]+2;
                }
				nuCHAR *pPoi2 = (nuCHAR*)m_pMmApi->MM_GetDataMemMass(infoT.poiCount*sizeof(nuCHAR), &m_nSKBPOIMMIdx2);
			    
				//nuCHAR *pPoi2 = new nuCHAR[m_SKBPoiLoadLen]; // GET DATA
                if (!skbfile.ReadData(nPoiPos+infoT.poiCount, pPoi2, m_SKBPoiLoadLen))
                {
                    m_SKBPoiLoadLen = 0;
                    return 0;
                }
				if (!m_SKBRoadLoadLen||!m_SKBPoiLoadLen )
				{
					return 0;
				}
				skbfile.Close();

			}
			
			


			nuCHAR *pB2 =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(m_nSKBROADMMIdx2);
			nuCHAR *pRoadB1 =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(m_nSKBROADMMIdx);
			if(m_nSKBROADMMIdx == (nuWORD)-1 || m_nSKBROADMMIdx2== (nuWORD)-1)
			{
				return nuFALSE;
			}
            nuMemcpy(&townID, pB2+pRoadB1[0], 2);
            if (JudgeSKB(pB2, keyStr, pRoadB1[0], keyLen, townID))
            {
                ++m_SKBResT;
            }
            nuUINT i = 0;
			for (; i<infoT.roadCount; )
            {
                pB2 += pRoadB1[i]+2;
                ++i;
                nuMemcpy(&townID, pB2+pRoadB1[i], 2);
                if (JudgeSKB(pB2, keyStr, pRoadB1[i], keyLen, townID))
                {
                    ++m_SKBResT;
                }
            }
			GetSKBInBuffer(pBuffer, m_SKBResT);
			nuCHAR* pB3 =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(m_nSKBPOIMMIdx2);
			nuCHAR *pPoiB1 =(nuCHAR*)m_pMmApi->MM_GetDataMassAddr(m_nSKBPOIMMIdx);
			if(m_nSKBPOIMMIdx2 == (nuWORD)-1 || m_nSKBPOIMMIdx2== (nuWORD)-1)
			{
				return nuFALSE;
			}
			//nuCHAR* pB3 = pPoi2;// + count;
            nuMemcpy(&townID, pB3+pPoiB1[0], 2);
            if (JudgeSKB(pB3, keyStr, pPoiB1[0], keyLen, townID))
            {
                ++m_SKBResT;
            }
            i = 0;
			for (; i<infoT.poiCount; )
            {
                pB3 += pPoiB1[i]+2;
                ++i;
                nuMemcpy(&townID, pB3+pPoiB1[i], 2);
                if (JudgeSKB(pB3, keyStr, pPoiB1[i], keyLen, townID))
                {
                    ++m_SKBResT;
                }
            }
		
        }
        if (resCount)
        {
            *resCount = m_SKBResT;
        }
        return GetSKBInBuffer(pBuffer, nMaxLen);

}
nuINT    CSearchNewDanyinP::GetSKBInBuffer(nuCHAR* pBuffer, nuINT nMaxLen)
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
nuBOOL   CSearchNewDanyinP::JudgeSKB(nuCHAR* strDes, nuCHAR* strSrc, nuINT nDesLen, nuINT nSrcLen, nuWORD townID)
{
	nuBOOL bInclude = nuFALSE;
        nuBYTE idx = 0;
        if (!nSrcLen && nDesLen)//彆KEY峈諾 氝樓忑趼
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
				if ('@' == strDes[0])
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
nuUINT CSearchNewDanyinP::SetAreaCenterPos(nuroPOINT pos)
{
	m_bSetAreaCenterPos = nuTRUE;
	m_TypeCenterPos = pos;
	return true;
}