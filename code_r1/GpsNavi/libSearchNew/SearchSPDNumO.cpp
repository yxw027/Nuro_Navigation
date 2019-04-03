// SearchSPDNumO.cpp: implementation of the CSearchSPDNumO class.
//
//////////////////////////////////////////////////////////////////////
#include "SearchSPDNumO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SPD_SEARCH_MODE_NO					0x00//渣昫腔脤戙
#define SPD_SEARCH_MODE_RESTART				0x01//笭陔A植0弇离羲宎刲坰
#define SPD_SEARCH_MODE_FORWARD				0x02//等秞趼睫崝樓ㄛ砃刲坰
#define SPD_SEARCH_MODE_BACKWARD			0x03//等秞趼睫熬屾ㄛ剒猁砃綴刲坰
#define SPD_SEARCH_MODE_KEEP				0x04//等秞趼睫羶衄曹趙ㄛ祥剒猁刲坰

CSearchSPDNumO::CSearchSPDNumO()
{
	m_nSHMemIdx = (nuWORD)-1;
	m_bKickNumber	= nuFALSE;
	m_bNextName		= nuFALSE;
	n_pNORMALRES = NULL; 
	m_nDataIdx = 0;
	m_nwsListNameMemIdx = -1;
	//wsListName = NULL;
	#ifndef VALUE_EMGRT
	m_Doorinfo		= NULL;
	#endif
	//m_bKEYNAME = nuFALSE;
}

CSearchSPDNumO::~CSearchSPDNumO()
{
	if(m_nwsListNameMemIdx != (nuWORD)(-1) )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nwsListNameMemIdx);
	}
	Free();
}

nuBOOL CSearchSPDNumO::Initialize()
{
	m_nSHMemIdx = (nuWORD)-1;
	if( !m_fileSpd.Initialize())
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CSearchSPDNumO::Free()
{
	if( m_nSHMemIdx != (nuWORD)(-1) )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nSHMemIdx);
	}
	m_fileSpd.Free();
	m_bKickNumber	= nuFALSE;
	m_bNextName		= nuFALSE;
	m_nDataIdx =  0;
	if(n_pNORMALRES != NULL)
	{
		delete []n_pNORMALRES;
		n_pNORMALRES=NULL;
	}
	/*if(wsListName)
	{
		delete[] wsListName;
		wsListName = NULL;
	}*/
#ifndef VALUE_EMGRT
	if(m_Doorinfo)
	{
		delete[] m_Doorinfo;
		m_Doorinfo = NULL;
	}
#endif
}

nuUINT CSearchSPDNumO::SetSHMode( nuUINT nMode )
{
	m_searchSet.nMode = nMode;
	m_fileSpd.SetFileMode(m_searchSet.nMode);
	return 1;
}

nuUINT CSearchSPDNumO::SetSHRegion(nuBYTE nSetMode,nuDWORD nData)
{

	if(nSetMode == SH_REGION_NONAMENUMBER) //SH_REGION_NONUBER
	{
	//	m_bKEYNAME = nuFALSE;
		m_bKickNumber = nuTRUE;
		/*nuWCHAR wsName[] = L"白雞";
		SetSHString((char*)wsName,
			nuWcslen(wsName)*sizeof(nuWCHAR));*/
	}
	if(nSetMode == SH_REGION_NAMENUMBER)
	{
		m_bNextName = nuTRUE;
		m_bKickNumber = nuFALSE;
		if(n_pNORMALRES == NULL)
		{
			n_pNORMALRES = new NURO_SEARCH_SDN_LIST[NURO_SEARCH_BUFFER_MAX_POI];
		}
		nuMemset(n_pNORMALRES,0,sizeof(NURO_SEARCH_SDN_LIST)*NURO_SEARCH_BUFFER_MAX_POI);
		m_nDataIdx = 0;
		/*nuWCHAR wsName[] = L"白雞";
		SetSHString((char*)wsName,
			nuWcslen(wsName)*sizeof(nuWCHAR));*/
	}
	if( nSetMode == SH_REGION_MAP )
	{
		m_searchSet.nMapIdx = nuLOWORD(nData);
		if( !m_fileSpd.SetMapIdx(m_searchSet.nMapIdx))
		{
			return 0;
		}	
		PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMemMass(sizeof(SPDSEARCHDATA),&m_nSHMemIdx);
		}
		if( pShData == NULL )
		{
			return 0;
		}
		pShData->nTotalFound	= 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		pShData->nKeptCount		= 0;
		pShData->nCityID        = (nuWORD)(-1);
		pShData->nTownID        = (nuWORD)(-1);
		pShData->nCountOfCity   = 0;
		pShData->nCountOfTown   = 0;
		m_bReSearchSpd = nuTRUE;
		nuMemset(m_pAStartIdx,0,sizeof(m_pAStartIdx));
		m_nASIdxCount = 0;
		nuMemset( m_sName,0,sizeof(m_sName));
		m_nNameNum = 0;
		nuMemset(&m_keyMask,0,sizeof(m_keyMask));
	}
	else if( nSetMode == SH_REGION_CITY )
	{
		PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
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
			pShData->nTownID = (nuWORD)-1;
		}
		else
		{
			if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
			{
				m_bReSearchSpd = nuTRUE;
				SearchSPD_City();
				return 1;
			}
			else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
			{
				m_bReSearchSpd = nuTRUE;
				nuMemset(m_pAStartIdx,0,sizeof(m_pAStartIdx));
				m_nASIdxCount = 0;
				nuMemset( m_sName,0,sizeof(m_sName));
				m_nNameNum = 0;
				nuMemset(&m_keyMask,0,sizeof(m_keyMask));
				return 1;
			}
			else
			{
				return 0;
			}
		}
		m_bReSearchSpd = nuTRUE;
	}
	else if( nSetMode == SH_REGION_TOWN )
	{
		PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL || pShData->nTownID == (nuWORD)nData )
		{
			return 0;
		}
		if( pShData->nTownID == (nuWORD)nData)
		{
			return 1;
		}
		pShData->nTownID = (nuWORD)nData;
		if( nData != (nuDWORD)(-1) )
		{
			if( ( m_searchSet.nMode & 0xffff0000 )  == SH_ENTER_MODE_PANA )
			{
				m_bReSearchSpd = nuTRUE;
				SearchSPD_Town();
				return 1;
			}
			else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
			{
				m_bReSearchSpd = nuTRUE;
				nuMemset(m_pAStartIdx,0,sizeof(m_pAStartIdx));
				m_nASIdxCount = 0;
				nuMemset( m_sName,0,sizeof(m_sName));
				m_nNameNum = 0;
				nuMemset(&m_keyMask,0,sizeof(m_keyMask));
				return 1;
			}
			else
			{
				return 0;
			}
		}
		m_bReSearchSpd = nuTRUE;
	}
	return 1;
}

nuUINT CSearchSPDNumO::SetSHString( const nuCHAR* pBuff,nuBYTE nBufLen )
{
	nuMemset(m_nSearchName,0,32);
	nuMemcpy(m_nSearchName,pBuff,sizeof(nuWCHAR )*nBufLen);
	nuMemset(m_nWordStartIdx,-1,sizeof(m_nWordStartIdx));
	m_nDataIdx = 0 ;
	//	nuMemset(m_nWordIdx,-1,sizeof(m_nWordIdx));
//	m_nTotal = 0;
	if(m_nSearchName[0]>15000)	
	{
	//	m_searchSet.nMode = SH_ENTER_MODE_NAME;
	}
//	if(m_bKEYNAME)
	{
		//m_searchSet.nMode = SH_ENTER_MODE_NAME;
	}

	if( pBuff == NULL || nBufLen == 0 )
	{
		return 0;
	}
	nuBYTE i = 0;
	nuBYTE nCharWide = 1;
	nuWORD nBuffNum = nBufLen;
	if( nuLOWORD(m_searchSet.nMode) == SH_ENTER_MODE_DY )
	{
		nCharWide = sizeof(nuWCHAR);
		nBuffNum = nBufLen/sizeof(nuWCHAR);
		nuWCHAR* wsName = (nuWCHAR*)m_sName;
		nuWCHAR* wsBuff = (nuWCHAR*)pBuff;
		for( i = 0; i < m_nNameNum && i < nBuffNum; ++i )
		{
			if( wsName[i] != wsBuff[i] )
			{
				break;
			}
		}
		

		/*for( i = 0; i < m_nNameNum && i < nBufLen; ++i )
		{
			if( m_sName[i] != pBuff[i] )
			{
				break;
			}
		}*/
	}
	else if( nuLOWORD(m_searchSet.nMode) == SH_ENTER_MODE_NAME )
	{
		nCharWide = sizeof(nuWCHAR);
		nBuffNum = nBufLen/sizeof(nuWCHAR);
		nuWCHAR* wsName = (nuWCHAR*)m_sName;
		nuWCHAR* wsBuff = (nuWCHAR*)pBuff;
		for( i = 0; i < m_nNameNum && i < nBuffNum; ++i )
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
	nuBYTE nSHMode = 0;
	if( m_nNameNum == i && i == nBuffNum )//睿奻棒珨欴ㄛ祥蚚笭陔刲坰
	{
		nSHMode = SPD_SEARCH_MODE_KEEP;
	}
	else if( i == 0 )//菴珨跺趼睫憩祥珨欴ㄛ笭陔刲坰
	{
		nSHMode = SPD_SEARCH_MODE_RESTART;
		m_nASIdxCount = 0;
	}
	else 
	{
		if( i == m_nNameNum )
		{
			nSHMode = SPD_SEARCH_MODE_FORWARD;
		}
		else
		{
			nSHMode = SPD_SEARCH_MODE_BACKWARD;
			if( i < m_nASIdxCount )
			{
				m_nASIdxCount = i;
			}//砃綴刲坰ㄛ剒猁笭扢刲坰腔萸
		}
	}
	if( nBuffNum > SPD_MAX_STR_COUNT )
	{
		nBufLen = SPD_MAX_STR_COUNT * nCharWide;
		nBuffNum = SPD_MAX_STR_COUNT;
	}
	m_nNameNum = nBuffNum;
	nuMemcpy( m_sName,pBuff,nBufLen );
	if( nuLOWORD(m_searchSet.nMode) == SH_ENTER_MODE_DY )
	{
		if( (m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
		{
			if( !SearchSPD_B(nSHMode) ) //侂狟唳腔
			{
				return 0;
			}
		}
		else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO ) 
		{
			if( !SearchSPD_B_Nuro(nSHMode) ) //赻模唳腔ㄛ剒猁瓚隅絞怀腔CityID,TownID
			{
				return 0;
			}
		}
		else 
		{
			return 0;
		}
		
	}
	else if( nuLOWORD(m_searchSet.nMode) == SH_ENTER_MODE_NAME )
	{
		if( ( m_searchSet.nMode & 0xffff0000 ) ==  SH_ENTER_MODE_PANA )
		{
			if( !SearchSPD_C(nSHMode) )
			{
				return 0;
			}
		}
		else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
		{
			if( !SearchSPD_C_Nuro(nSHMode) )
			{
				return 0;
			}
			if(m_bKickNumber)
			{

				nuLONG nSize = NURO_SEARCH_SORT_NAME* sizeof(Trans_DATANAME);
				PTrans_DATANAME wsListName = (PTrans_DATANAME)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);
				nuMemset(wsListName,0,nSize);
				/*
				if(m_nwsListNameMemIdx==-1)
				{
					nuLONG nSize = NURO_SEARCH_SORT_NAME* sizeof(Trans_DATANAME);
					nuPBYTE pData = (nuPBYTE)m_pMmApi->MM_GetDataMemMass(nSize,&m_nwsListNameMemIdx);

					//wsListName = new Trans_DATANAME[NURO_SEARCH_SORT_NAME];	
					//nuMemset(wsListName,0,sizeof(Trans_DATANAME)*NURO_SEARCH_SORT_NAME);

					nuMemset(pData,0,nSize);
					
				}*/
				/*if(wsListName == NULL)
				{
					wsListName = new Trans_DATANAME[NURO_SEARCH_SORT_NAME];	
					nuMemset(wsListName,0,sizeof(Trans_DATANAME)*NURO_SEARCH_SORT_NAME);
				}
				else
				{
					nuMemset(wsListName,0,sizeof(Trans_DATANAME)*NURO_SEARCH_SORT_NAME);
				}*/
				/*if(wsListName == NULL )
				{
					return nuFALSE ;
				}*/
				PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
				nuINT nTotal = pShData->nTotalFound;
				if(nTotal >= NURO_SEARCH_SORT_NAME)
				{
					nTotal = NURO_SEARCH_SORT_NAME;
				}

				nuINT k=0;
				for(nuINT i=0;i<nTotal ;i++)
				{
					SH_ITEM_FORUI temp = {0};
					m_fileSpd.GetRdOneData_GETNAME(&temp, pShData->pIndexList[i]);
					if(k==0)
					{
						pShData->pIndexList[k]=pShData->pIndexList[i];
						nuWcsncpy(wsListName[i].name,temp.wsName,m_nWordEnd[i]);
						m_nWordEnd[k]=m_nWordEnd[i];
						k++;
						continue;
					}
					nuBOOL bSame=nuTRUE;
					nuINT l=0;
					for(;l<k;l++)
					{
						bSame=nuTRUE;
						for(nuINT j=0;j<m_nWordEnd[i];j++)
						{
							if(temp.wsName[j]!=wsListName[l].name[j])
							{
								bSame = nuFALSE;
								//continue;
								break;
							}

						}
						//if(bSame)
						//	continue;
						//nWordCount++;
					}
					if(!bSame)
					{
						pShData->pIndexList[k]=pShData->pIndexList[i];
						nuWcsncpy(wsListName[k].name,temp.wsName,m_nWordEnd[i]);
						m_nWordEnd[k]=m_nWordEnd[i];
						k++;
					}

					
				}
				if(m_bKickNumber)
					pShData->nKeptCount=k;
				
				//NAME_Order(nTotal);
			}
		}
		else 
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
#ifndef VALUE_EMGRT
nuBOOL CSearchSPDNumO::GetDoorNumberInfo(SH_ROAD_DETAIL  &DoorinfoS)
{
	if(m_nDataIdx < 1)
		return nuFALSE;

	if(m_Doorinfo)
	{
		delete[] m_Doorinfo;
		m_Doorinfo = NULL;
	}
	m_Doorinfo = new SEARCH_PDN_NODE_DI[m_nDataIdx];
	if(NULL == m_Doorinfo)
	{
		return nuFALSE;
	}
	nuMemset(m_Doorinfo,0,sizeof(SEARCH_PDN_NODE_DI)*m_nDataIdx);
	
	m_ndoor=0;
	nuBOOL bRes=nuFALSE;
	nuBOOL bJump=nuFALSE;
	nuINT  nLen=nuWcslen(m_nSearchName);

    for(nuINT j=0;j<m_nDataIdx;j++)
	{
		if(n_pNORMALRES[j].name[nLen] == 0 || 0!=nuWcsncmp(n_pNORMALRES[j].name,m_nSearchName,nLen))
		{
			continue;
		}
		bJump = nuFALSE;
		bRes=nuFALSE;
		nuINT  nLen2=nuWcslen(n_pNORMALRES[j].name);
		nuBOOL bFindTag1=nuFALSE,bFindTag2=nuFALSE;
		nuINT  nNumber1 = 0;
		nuINT  nNumber2 = 0;
		nuINT  nTen = 10;
		nuINT  nTen2 = 10;
		for(nuINT k = nLen; k < nLen2;k++)
		{
			switch(n_pNORMALRES[j].name[k])
			{
				case 24055:
					{
						bFindTag1 = nuFALSE;
						nNumber1=0;		
						nTen=1;
						k++;
						bJump=true;
					}
					break;
				case 24324:
					{
						bFindTag1 = nuFALSE;
						nNumber1=0;		
						nTen=1;
						bJump=nuTRUE;
						k++;
					}
					break;
				case 20043:
					{
						bFindTag1 = nuTRUE;
						k++;
					}
					break;
				case 34399:
						bRes = nuTRUE;
				break;
				
			}
			if(bRes || bJump)
			{
				if(nNumber1 && nNumber2)
				{
					m_Doorinfo[m_ndoor].doorNumber  =  nNumber1;
					m_Doorinfo[m_ndoor].doorNumber2 =  nNumber2;
					
					if( !m_fileSpd.GetSPDDrNumCoor( n_pNORMALRES[j].nIdx,0,&m_Doorinfo[m_ndoor].pos) )
					{
						//break;
					}
					m_ndoor++;
				}
				break;
			}
			if(!bFindTag1)
			{
				nNumber1  = nTen * nNumber1 + (n_pNORMALRES[j].name[k] - 48);		
			}
			if (bFindTag1 )
			{	
				nNumber2  = nTen2 * nNumber2 + (n_pNORMALRES[j].name[k] - 48);	
			}
		}
		
	}

	SEARCH_PDN_NODE_DI temp = {0};

	for(nuINT kk=0;kk<m_ndoor;kk++)
		for(nuINT k=0;k+1<m_ndoor;k++)
	{
		if(m_Doorinfo[k].doorNumber > m_Doorinfo[k+1].doorNumber)
		{
			temp = m_Doorinfo[k];
			m_Doorinfo[k] = m_Doorinfo[k+1];
			m_Doorinfo[k+1] = temp;
		}
		if(m_Doorinfo[k].doorNumber == m_Doorinfo[k+1].doorNumber)
		{
			if(m_Doorinfo[k].doorNumber2 > m_Doorinfo[k+1].doorNumber2)
			{

				temp = m_Doorinfo[k];
				m_Doorinfo[k] = m_Doorinfo[k+1];
				m_Doorinfo[k+1] = temp;

			}
		}
		
	}

	if(m_ndoor > 0)
	{
		DoorinfoS.nDrNumCount = m_ndoor;
		DoorinfoS.nMinDrNum = m_Doorinfo[0].doorNumber;//nMin;
		if(m_ndoor > 1)
			DoorinfoS.nMaxDrnum = m_Doorinfo[m_ndoor-1].doorNumber;//nMax;
		else
			DoorinfoS.nMaxDrnum = m_Doorinfo[0].doorNumber;
	}
	if(m_ndoor <1)
		return nuFALSE;

	return nuTRUE;	
}
nuBOOL CSearchSPDNumO::GetDoorNumberInfoData(nuVOID* pBuffer)
{
	nuMemcpy(pBuffer,m_Doorinfo,sizeof(SEARCH_PDN_NODE_DI)*m_ndoor );
	return nuTRUE;
}
#endif
nuBOOL CSearchSPDNumO::SearchSPD_City()
{
	nuDWORD nCount =0;
	PRN2_D_NODE pSPDD = (PRN2_D_NODE)m_fileSpd.GetRdDData(&nCount);
	if( pSPDD == NULL )
	{
		return nuFALSE;
	}//祥夔溫善pShData眳狟ㄛ秪峈涴爵醱頗煦饜囀湔﹝
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//蛁砩涴爵祥夔睿奻醱腔遙弇离
	pShData->nCountOfCity = 0;
	for( nuDWORD i = 0; i < pShData->nKeptCount; ++i )
	{
		if( pSPDD[pShData->pIndexList[i]].nCityID == pShData->nCityID )
		{
			pShData->pCityIdxList[pShData->nCountOfCity] = pShData->pIndexList[i];
			++pShData->nCountOfCity;
			if( pShData->nCountOfCity >= SPD_MAX_CITY_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CSearchSPDNumO::SearchSPD_Town()
{
	nuDWORD nCount = 0;
	PRN2_D_NODE pSPDD = (PRN2_D_NODE)m_fileSpd.GetRdDData(&nCount);
	if( pSPDD == NULL )
	{
		return nuFALSE;
	}//祥夔溫善pShData眳狟ㄛ秪峈涴爵醱頗煦饜囀湔﹝
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return nuFALSE;
	}//蛁砩涴爵祥夔睿奻醱腔遙弇离
	pShData->nCountOfTown = 0;
	for( nuDWORD i = 0; i < pShData->nCountOfCity; ++i )
	{
		if( pSPDD[pShData->pCityIdxList[i]].nTownID == pShData->nTownID )
		{
			pShData->pTownIdxList[pShData->nCountOfTown] = pShData->pCityIdxList[i];
			++pShData->nCountOfTown;
			if( pShData->nCountOfTown >= SPD_MAX_TOWN_RECORD )
			{
				break;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CSearchSPDNumO::SearchSPD_B(nuBYTE nShMode)
{
	if( nShMode == SPD_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	nuBYTE *pLength = (nuBYTE*)m_fileSpd.GetRdALength(&nTotalCount);
	nuWCHAR *pZhuyin = (nuWCHAR*)m_fileSpd.GetRdBOrCString(NULL);

	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pLength == NULL || pZhuyin == NULL || pShData == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;
	nuMemset(&m_keyMask,0,sizeof(m_keyMask));
	if( nShMode == SPD_SEARCH_MODE_RESTART || nShMode == SPD_SEARCH_MODE_BACKWARD ) //笭陔刲坰
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
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
		{
			SearchSPD_B_STRCMP(pZhuyin,pLength[i]/2,i,pShData);
			pZhuyin += pLength[i]/2;
		}
		
	}
	else //婓眒冪梑善腔暮翹笢扆梑
	{
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx = pShData->pIndexList[ nOldKeptCount - 1 ];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		nuDWORD idx = 0;
		for( i = 0; i < nOldKeptCount ; ++i )
		{
			while ( idx < pShData->pIndexList[i] )
			{
				pZhuyin += pLength[idx]/2;
				++idx;
			}
			SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
		}
		//樟哿梑遜羶衄梑善腔暮翹
		if( nFoundDataMode == SPD_FOUNDDATA_MODE_FIND_PART && idx < nTotalCount )
		{
			pZhuyin += pLength[idx];
			++idx;
			while (idx < nTotalCount )
			{
				SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
				pZhuyin += pLength[idx]/2;
				++idx;
			}
		}
	}
	m_bReSearchSpd = nuTRUE;
	return nuTRUE;
}

nuBOOL CSearchSPDNumO::SearchSPD_C(nuBYTE nShMode)
{
	if( nShMode == SPD_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	nuBYTE *pLength = (nuBYTE*)m_fileSpd.GetRdALength(&nTotalCount);
	nuWCHAR *pName = (nuWCHAR*)m_fileSpd.GetRdBOrCString(NULL);
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pLength == NULL || pName == NULL || pShData == NULL )
	{
		return nuFALSE;
	}
	pShData->nCityID = -1;
	pShData->nTownID = -1;
	nuMemset(&m_keyMask,0,sizeof(m_keyMask));
	if( nShMode == SPD_SEARCH_MODE_RESTART || nShMode == SPD_SEARCH_MODE_BACKWARD ) //笭陔刲坰
	{
		nuDWORD nAIdxStart = 0;
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
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
		{
			SearchSPD_C_STRCMP(pName,pLength[i]/2,i,pShData);
			pName += pLength[i]/2;
		}
	}
	else //婓眒冪梑善腔暮翹笢扆梑
	{
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx = pShData->pIndexList[ nOldKeptCount - 1 ];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		nuDWORD idx = 0;
		for( i = 0; i < nOldKeptCount ; ++i )
		{
			while ( idx < pShData->pIndexList[i] )
			{
				pName += pLength[idx]/2;
				++idx;
			}
			SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
		}
		//樟哿梑遜羶衄梑善腔暮翹
		if( nFoundDataMode == SPD_FOUNDDATA_MODE_FIND_PART && idx < nTotalCount )
		{
			pName += pLength[idx];
			++idx;
			while (idx < nTotalCount )
			{
				SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
				pName += pLength[idx]/2;
				++idx;
			}
		}
	}
	m_bReSearchSpd = nuTRUE;
	return nuTRUE;
}

nuVOID CSearchSPDNumO::SearchSPD_C_STRCMP( nuWCHAR *pstr,nuBYTE nstrlen, nuDWORD idx, PSPDSEARCHDATA pShData )
{
	/*nuWCHAR *w;
	w = new nuWCHAR[nstrlen];
	nuMemcpy(w,pstr,nstrlen);
	MessageBoxW(NULL,w,0,NULL);
	delete []w;*/
	nuWCHAR* wsName = (nuWCHAR*)m_sName;
	nuBOOL bHaveKept = nuFALSE;
	for( nuBYTE j = 0; j < nstrlen; ++j )
	{
		if( pstr[j] == NULL || nstrlen - j < m_nNameNum )
		{
			break;
		}
		if( pstr[j] == wsName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum; ++k )
			{
				if(pstr[k+j] != wsName[k] )
				{
					break;
				}
			}
			while( m_nASIdxCount < k && m_nASIdxCount < SPD_MAX_STR_COUNT )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}
			nuINT nWordEnd=0;
			if( k == m_nNameNum )//梑善珨跺眈睫磁腔
			{
				if(m_bKickNumber)
				{
					nuBOOL bRes=nuFALSE;

					m_nWordEnd[pShData->nKeptCount]  = nstrlen;
					for(nuINT i=m_nNameNum;i<nstrlen;i++)
					{
						if(pstr[i] > 48 && pstr[i] < 58)
						{
							bRes=true;
							m_nWordEnd[pShData->nKeptCount] = i;
							break;
						}
					}
					/*if (bRes)
						break;
					*/
				}
				if(m_bNextName)
				{
					nuMemset(n_pNORMALRES[m_nDataIdx].name,0,sizeof(n_pNORMALRES[m_nDataIdx].name));
					nuWcsncpy(n_pNORMALRES[m_nDataIdx].name,pstr,nstrlen);
					n_pNORMALRES[m_nDataIdx].nIdx=idx;
					m_nDataIdx++;
				}
				/*if( k+j < nstrlen )
				{
					AddKeyMask(pstr[k+j]);
				}*/
				if( !bHaveKept )
				{
					++pShData->nTotalFound;
					if( pShData->nKeptCount < SPD_MAX_KEPT_RECORD )
					{
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_FIND_ALL;
					}
					else
					{
						pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_FIND_PART;
					}
					bHaveKept = nuTRUE;
				}
				j += ( k-1 );
			}
		}
	}
}

nuVOID CSearchSPDNumO::SearchSPD_B_STRCMP( nuWCHAR *pstr, nuBYTE nstrlen, nuDWORD idx, PSPDSEARCHDATA pShData )
{
	
	nuWCHAR* wsName = (nuWCHAR*)m_sName;
	nuBOOL bHaveKept = nuFALSE;
	for( nuBYTE j = 0; j < nstrlen; ++j )
	{
		if( nstrlen - j < m_nNameNum )
		{
			break;
		}
		if( pstr[j] == wsName[0] )
		{
			nuBYTE k;
			for( k = 1; k < m_nNameNum; ++k )
			{
				if(pstr[k+j] != wsName[k] )
				{
					break;
				}
			}
			while( m_nASIdxCount < k && m_nASIdxCount < SPD_MAX_STR_COUNT )
			{
				m_pAStartIdx[m_nASIdxCount] = idx;
				++m_nASIdxCount;
			}
			if( k == m_nNameNum )//梑善珨跺眈睫磁腔
			{
				if(m_bKickNumber)
				{
				nuBOOL bRes=nuFALSE;
				m_nWordEnd[pShData->nKeptCount]  = nstrlen;	
				for(nuINT i=m_nNameNum;i<nstrlen;i++)
				{
					if(pstr[i] > 48 && pstr[i] < 58)
					{
						bRes=true;
						m_nWordEnd[pShData->nKeptCount]  = i;
						break;
					}
				}
				/*	if (bRes)
						break;
				*/
				}
				if(m_bNextName&&0)
				{
					nuWcscpy(n_pNORMALRES[m_nDataIdx].name,pstr);
					n_pNORMALRES[m_nDataIdx].nIdx=idx;
					m_nDataIdx++;
				}
				if( k+j < nstrlen )
				{
					AddKeyMask(pstr[k+j]);
				}
				if( !bHaveKept )
				{
					++pShData->nTotalFound;
					if( pShData->nKeptCount < SPD_MAX_KEPT_RECORD )
					{
				/*		nuTCHAR t[64];
						wsprintf(t,nuTEXT("%d"),idx);
						MessageBox(NULL,t,0,NULL);
	nuWCHAR *w;
						w = new nuWCHAR[nstrlen+1];
						nuMemcpy(w,pstr,nstrlen*2);
						w[nstrlen] = 0;
						MessageBoxW(NULL,w,0,NULL);
	delete []w;*/
						m_nWordStartIdx[pShData->nKeptCount] =j;
						//m_nWordIdx[pShData->nKeptCount] = idx;
						//m_nTotal++;
						pShData->pIndexList[pShData->nKeptCount++] = idx;
						pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_FIND_ALL;
					}
					else
					{
						pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_FIND_PART;
					}
					bHaveKept = nuTRUE;
				}
				j += ( k-1 );
			}
		}
	}
}

nuUINT CSearchSPDNumO::GetSPDDataHead(PSH_HEADER_FORUI pShHeader)
{
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShHeader == NULL || pShData == NULL )
	{
		return 0;
	}
	else 
	{
		pShHeader->nTotalCount = pShData->nTotalFound;
		pShHeader->pDyKeyMask = &m_keyMask;
		return 1;
	}
}

nuUINT CSearchSPDNumO::GetSPDDataPage( PSH_PAGE_FORUI pPageData,nuDWORD nItemIdx )
{
	//DYWORDFORUI pDyData={0};
	//GetDYGetWordData(&pDyData);
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pPageData == NULL || pShData == NULL )
	{
		return 0;
	}
	nuDWORD nCount=0;
	nuDWORD *pIdxList=NULL;
	if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
	{
		if( pShData->nTownID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pTownIdxList;
			nCount   = pShData->nCountOfTown;
		}
		else if( pShData->nCityID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pCityIdxList;
			nCount   = pShData->nCountOfCity;
		}
		else
		{
			pIdxList = pShData->pIndexList;
			nCount   = pShData->nKeptCount;
		}
	}
	else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
	{
		pIdxList = pShData->pIndexList;
		nCount   = pShData->nKeptCount;
	}
	else
	{
		return 0;
	}
	if( nItemIdx >= nCount )
	{
		return 0;
	}
	if( pPageData->nPageItemCount > DY_NAME_FORUI_MAX_COUNT )
	{
		pPageData->nPageItemCount = DY_NAME_FORUI_MAX_COUNT;
	}
	nuWORD nCountRead = nuWORD( nCount - nItemIdx );
	if( nCountRead > pPageData->nPageItemCount )
	{
		nCountRead = pPageData->nPageItemCount;
	}
	pPageData->nThisPageCount = 0;
	for( nuDWORD i = nItemIdx; i < nCountRead + nItemIdx; ++i )
	{
		if( !m_fileSpd.GetRdOneData(&pPageData->dyNodeList[pPageData->nThisPageCount], pIdxList[i]) )
		{
			break;
		}
		if(m_bKickNumber)
			pPageData->dyNodeList[pPageData->nThisPageCount].wsName[m_nWordEnd[i]]=0;
		pPageData->pNodeIdxList[pPageData->nThisPageCount] = pIdxList[i];
		++pPageData->nThisPageCount;
	}
	pPageData->nNowPageItemIdx = (nuWORD)nItemIdx;
	pPageData->nTotalCount = nCount;
	pPageData->nIdxSelected = 0;
	if( m_bReSearchSpd )
	{
		SearchSPD_Stroke(pIdxList,nCount);
		m_bReSearchSpd = nuFALSE;
	}
	pPageData->nMinStroke = pShData->nMinStroke;
	pPageData->nMaxStroke = pShData->nMaxStroke;
	return 1;
}

nuVOID CSearchSPDNumO::SearchSPD_Stroke(nuDWORD *pIdxList, int nCount)
{
	nuPBYTE pStroke = (nuPBYTE)m_fileSpd.GetRdStroke(NULL);
	if( pStroke == NULL )
	{
		return;
	}
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return;
	}
	pShData->nMinStroke = pShData->nMaxStroke = pStroke[pIdxList[0]];
	for( int i =0; i < nCount; ++i )
	{
		if( pStroke[pIdxList[i]] < pShData->nMinStroke )
		{	
			pShData->nMinStroke = pStroke[pIdxList[i]];
		}
		else if( pStroke[pIdxList[i]] > pShData->nMaxStroke )
		{
			pShData->nMaxStroke = pStroke[pIdxList[i]];
		}
	}
}

nuUINT CSearchSPDNumO::GetSPDRoadDetail(PSH_ROAD_DETAIL pOneRoad, nuDWORD nIdx)
{
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pOneRoad == NULL || pShData == NULL )
	{
		return 0;
	}
	nuDWORD nCount=0;
	nuDWORD *pIdxList=NULL;
	if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
	{
		if( pShData->nTownID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pTownIdxList;
			nCount   = pShData->nCountOfTown;
		}
		else if( pShData->nCityID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pCityIdxList;
			nCount   = pShData->nCountOfCity;
		}
		else
		{
			pIdxList = pShData->pIndexList;
			nCount   = pShData->nKeptCount;
		}
	}
	else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
	{
		pIdxList = pShData->pIndexList;
		nCount   = pShData->nKeptCount;
	}
	else
	{
		return 0;
	}
	if( nIdx >= nCount )
	{
		return 0;
	}
	if( !m_fileSpd.GetSPDRoadDetail(pOneRoad, pIdxList[nIdx]) )
	{
		return 0;
	}
	return 1;
}

nuUINT CSearchSPDNumO::GetSPDDrNumCoor(nuDWORD nIdx,nuDWORD nDoorNum,nuroPOINT* point )
{
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( point == NULL || pShData == NULL )
	{
		return 0;
	}
	nuDWORD nCount=0;
	nuDWORD *pIdxList=NULL;
	if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
	{
		if( pShData->nTownID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pTownIdxList;
			nCount   = pShData->nCountOfTown;
		}
		else if( pShData->nCityID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pCityIdxList;
			nCount   = pShData->nCountOfCity;
		}
		else
		{
			pIdxList = pShData->pIndexList;
			nCount   = pShData->nKeptCount;
		}	
	}
	else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
	{
		pIdxList = pShData->pIndexList;
		nCount   = pShData->nKeptCount;
	}
	else
	{
		return 0;
	}
	if( nIdx >= nCount )
	{
		return 0;
	}
	if( !m_fileSpd.GetSPDDrNumCoor( pIdxList[nIdx],nDoorNum,point) )
	{
		return 0;
	}
	return 1;
}

nuUINT CSearchSPDNumO::GetSPDCityTown(PSH_CITYTOWN_FORUI pCityTown)
{
	if( pCityTown == NULL )
	{
		return 0;
	}
	pCityTown->nTotalCount = 0;
	if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_ALL )
	{
		nuDWORD nCityCount = m_fileSpd.GetCityCount();
		while( pCityTown->nTotalCount < SH_NAME_CITYTOWN_MAX_COUNT && pCityTown->nTotalCount < nCityCount )
		{
			pCityTown->pIdList[pCityTown->nTotalCount] = pCityTown->nTotalCount;
			++pCityTown->nTotalCount;
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_USED )
	{
		PRN2_D_NODE pRn2D = ( PRN2_D_NODE )m_fileSpd.GetRdDData(NULL);
		if( pRn2D == NULL )
		{
			return 0;
		}
		PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == NULL )
		{
			return 0;
		}
		nuDWORD i = 0;
		nuDWORD nStartID = pCityTown->nIDStart;
		nuDWORD nLastID = 0;
		for( i = 0; i < pShData->nKeptCount; i++ )
		{
			InsertCityTownID( pCityTown,pRn2D[pShData->pIndexList[i] ].nCityID );
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_ALL )
	{
		nuDWORD nCount=0;
		PRN2_D_NODE pRn2D = (PRN2_D_NODE)m_fileSpd.GetRdDData(&nCount);
		if( pRn2D == NULL )
		{
			return 0;
		}
		for( nuDWORD i = 0; i < nCount; ++i )
		{
			if( pCityTown->nCityID == pRn2D[i].nCityID )
			{
				InsertCityTownID( pCityTown,pRn2D[i].nTownID );
			}
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_USED )
	{
		PRN2_D_NODE pRn2D = ( PRN2_D_NODE )m_fileSpd.GetRdDData(NULL);
		if( pRn2D == NULL )
		{
			return 0;
		}
		PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
		if( pShData == 0 )
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
			InsertCityTownID( pCityTown,pRn2D[pShData->pCityIdxList[i] ].nTownID );
		}
	}
	pCityTown->nNowCount = pCityTown->nTotalCount;
	if( m_fileSpd.GetRdCityTown(pCityTown) )
	{
		return 1;
	}
	return 0;
}

nuBYTE CSearchSPDNumO::InsertCityTownID(PSH_CITYTOWN_FORUI pCityTown, nuWORD nID )
{
	if( pCityTown->nTotalCount == SH_NAME_CITYTOWN_MAX_COUNT )
	{
		return SPD_ID_INSERT_FAILURE;
	}
	int j;
	for( j = 0; j < pCityTown->nTotalCount; ++j )
	{
		if( nID == pCityTown->pIdList[j] )
		{
			return SPD_ID_INSERT_SAME;
		}
		else if( nID < pCityTown->pIdList[j] )
		{
			break;
		}
	}
	++pCityTown->nTotalCount;
	for( int k = pCityTown->nTotalCount -1; k < j; --k )
	{
		pCityTown->pIdList[k] = pCityTown->pIdList[k-1];
	}
	pCityTown->pIdList[j] = nID;
	return SPD_ID_INSERT_SUCCESS;

}

nuUINT CSearchSPDNumO::GetSPDDataStroke(PSH_PAGE_FORUI pPageData, nuBYTE nStroke)
{
	if( pPageData == NULL )
	{
		return 0;
	}
	nuPBYTE pStroke = (nuPBYTE)m_fileSpd.GetRdStroke(NULL);
	if( pStroke == NULL )
	{
		return 0;
	}
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pShData == NULL )
	{
		return 0;
	}
	nuDWORD *pIdxList=NULL;
	nuINT nCount=0;
	if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
	{
		if( pShData->nTownID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pTownIdxList;
			nCount   = pShData->nCountOfTown;
		}
		else if( pShData->nCityID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pCityIdxList;
			nCount   = pShData->nCountOfCity;
		}
		else
		{
			pIdxList = pShData->pIndexList;
			nCount   = pShData->nKeptCount;
		}
	}
	else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
	{
		pIdxList = pShData->pIndexList;
		nCount   = pShData->nKeptCount;
	}
	else
	{
		return 0;
	}
	int i;
	if( pPageData->nIdxSelected >= pPageData->nThisPageCount )
	{
		pPageData->nIdxSelected = 0;
	}
	if( nStroke == pPageData->dyNodeList[pPageData->nIdxSelected].nStroke )
	{
		return 1;
	}
	else if( nStroke > pPageData->dyNodeList[pPageData->nIdxSelected].nStroke )
	{
		nuBOOL bFind = nuFALSE;
		for( i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pPageData->pNodeIdxList[pPageData->nIdxSelected] )
			{
				break;
			}
		}
		for( ; i < nCount; ++i )
		{
			if( pStroke[pIdxList[i]] >= nStroke )
			{
				bFind = nuTRUE;
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}
	}
	else
	{
		nuBOOL bFind = nuFALSE;
		nuINT i;
		for( i = 0; i < nCount; ++i )
		{
			if( pIdxList[i] == pPageData->pNodeIdxList[pPageData->nIdxSelected] )
			{
				break;
			}
		}
		for( ; i >= 0; --i )
		{
			if( pStroke[pIdxList[i]] <= nStroke )
			{
				bFind = nuTRUE;
				nStroke = pStroke[pIdxList[i]];
				break;
			}
		}
		if( !bFind )
		{
			return 0;
		}
		for( ; i > 0; --i )
		{
			if( pStroke[pIdxList[i-1]] != nStroke )
			{
				break;
			}
		}
	}//捩赫熬屾
	nuWORD nNewPageItemIdx = (nuWORD)((i/pPageData->nPageItemCount) * pPageData->nPageItemCount );
	if( pPageData->nNowPageItemIdx != nNewPageItemIdx )
	{
		pPageData->nNowPageItemIdx = nNewPageItemIdx;
		nuWORD nCountRead = nuWORD(nCount - nNewPageItemIdx);
		if( nCountRead > pPageData->nPageItemCount )
		{
			nCountRead = pPageData->nPageItemCount;
		}
		pPageData->nThisPageCount = 0;
		for( nuWORD j = nNewPageItemIdx; j < nCountRead + nNewPageItemIdx; ++j )
		{
			if( !m_fileSpd.GetRdOneData( &pPageData->dyNodeList[pPageData->nThisPageCount],pIdxList[j] ) )
			{
				break;
			}
			pPageData->pNodeIdxList[pPageData->nThisPageCount] = pIdxList[j];
			++pPageData->nThisPageCount;
		}
		pPageData->nIdxSelected = (nuBYTE)(i%pPageData->nPageItemCount);
		pPageData->nTotalCount = nCount;
		return 1;	
	}
	return 1;
}

nuBOOL CSearchSPDNumO::SearchSPD_B_Nuro(nuBYTE nShMode)
{
	if( nShMode == SPD_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	PRN2_D_NODE pSPDD = (PRN2_D_NODE)m_fileSpd.GetRdDData(NULL);
	nuBYTE *pLength = (nuBYTE*)m_fileSpd.GetRdALength(&nTotalCount);
	nuWCHAR *pZhuyin = (nuWCHAR*)m_fileSpd.GetRdBOrCString(NULL);

	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pLength == NULL || pZhuyin == NULL || pShData == NULL )
	{
		return nuFALSE;
	}
	nuMemset(&m_keyMask,0,sizeof(m_keyMask));
	if( nShMode == SPD_SEARCH_MODE_RESTART || nShMode == SPD_SEARCH_MODE_BACKWARD ) //笭陔刲坰
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
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		if( pShData->nCityID == (nuWORD)-1 )
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pZhuyin += pLength[idx]/2;
				++idx;
			}
			nuDWORD i = 0;
			for(  i = nAIdxStart; i < nTotalCount; ++i )
			{
				SearchSPD_B_STRCMP(pZhuyin,pLength[i]/2,i,pShData);
				//pZhuyin += pLength[i]/2;
			}
			pZhuyin += pLength[i]/2;
		}
		else if( pShData->nTownID == (nuWORD)-1 )
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pZhuyin += pLength[idx]/2;
				++idx;
			}
			for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
			{
				if( pSPDD[i].nCityID == pShData->nCityID )
				{
					SearchSPD_B_STRCMP(pZhuyin,pLength[i]/2,i,pShData);
					//pZhuyin += pLength[i]/2;
				}
				pZhuyin += pLength[i]/2;
			}
		}
		else 
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pZhuyin += pLength[idx]/2;
				++idx;
			}
			nuDWORD i = 0;
			for(  i = nAIdxStart; i < nTotalCount; ++i )
			{
				if( pSPDD[i].nCityID == pShData->nCityID && pSPDD[i].nTownID == pShData->nTownID )
				{
					SearchSPD_B_STRCMP(pZhuyin,pLength[i]/2,i,pShData);
					//pZhuyin += pLength[i]/2;
				}
				pZhuyin += pLength[i]/2;
			}
		}
		
	}
	else //婓眒冪梑善腔暮翹笢扆梑
	{
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx = pShData->pIndexList[ nOldKeptCount - 1 ];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		nuDWORD idx = 0;
		if( pShData->nCityID == (nuWORD)-1 )
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pZhuyin += pLength[idx]/2;
					++idx;
				}
				SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
			}
			
		}
		else if( pShData->nTownID == (nuWORD)-1 )
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pZhuyin += pLength[idx]/2;
					++idx;
				}
				if( pSPDD[idx].nCityID == pShData->nCityID )
				{
					SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
				}
			}
		}
		else 
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pZhuyin += pLength[idx]/2;
					++idx;
				}
				if( pSPDD[idx].nCityID == pShData->nCityID && pSPDD[idx].nTownID == pShData->nTownID )
				{
					SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
				}
			}
		}
		//樟哿梑遜羶衄梑善腔暮翹
		if( nFoundDataMode == SPD_FOUNDDATA_MODE_FIND_PART && idx < nTotalCount )
		{
			pZhuyin += pLength[idx]/2;
			++idx;
			if( pShData->nCityID == (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{
					SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
					pZhuyin += pLength[idx]/2;
					++idx;
				}
			}
			else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID == (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{
					if( pShData->nCityID == pSPDD[idx].nCityID )
					{
						SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);	
					}
					pZhuyin += pLength[idx]/2;
					++idx;
					
				}
			}
			else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID != (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{	
					if( pShData->nCityID == pSPDD[idx].nCityID && pShData->nTownID == pSPDD[idx].nTownID )
					{
						SearchSPD_B_STRCMP(pZhuyin,pLength[idx]/2,idx,pShData);
						
					}
					pZhuyin += pLength[idx]/2;
					++idx;
				}
			}
			else
			{
				return nuFALSE;
			}
		}
	}
	m_bReSearchSpd = nuTRUE;
	return nuTRUE;
}


nuBOOL CSearchSPDNumO::SearchSPD_C_Nuro(nuBYTE nShMode)
{
	if( nShMode == SPD_SEARCH_MODE_KEEP )
	{
		return nuTRUE;
	}
	nuDWORD nTotalCount = 0;
	nuBYTE *pLength = (nuBYTE*)m_fileSpd.GetRdALength(&nTotalCount);
	nuWCHAR *pName = (nuWCHAR*)m_fileSpd.GetRdBOrCString(NULL);
	PRN2_D_NODE pSPDD = (PRN2_D_NODE)m_fileSpd.GetRdDData(NULL);
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if( pLength == NULL || pName == NULL || pShData == NULL )
	{
		return nuFALSE;
	}
	//pShData->nCityID = -1;
	//pShData->nTownID = -1;
	nuMemset(&m_keyMask,0,sizeof(m_keyMask));
	if( nShMode == SPD_SEARCH_MODE_RESTART || nShMode == SPD_SEARCH_MODE_BACKWARD ) //笭陔刲坰
	{
		nuDWORD nAIdxStart = 0;
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
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		if( pShData->nCityID == (nuWORD)-1 )
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pName += pLength[idx]/2;
				++idx;
			}
			for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
			{
				SearchSPD_C_STRCMP(pName,pLength[i]/2,i,pShData);
				pName += pLength[i]/2;
			}
		}
		else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID == (nuWORD)-1 )
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pName += pLength[idx]/2;
				++idx;
			}
			for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
			{
				if( pSPDD[i].nCityID == pShData->nCityID )
				{
					SearchSPD_C_STRCMP(pName,pLength[i]/2,i,pShData);
				}
				pName += pLength[i]/2;
			}
		}
		else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID != (nuWORD)-1 )
		{
			nuDWORD idx = 0;
			while ( idx < nAIdxStart )
			{
				pName += pLength[idx]/2;
				++idx;
			}
			for( nuDWORD i = nAIdxStart; i < nTotalCount; ++i )
			{
				if( pSPDD[i].nCityID == pShData->nCityID && pSPDD[i].nTownID == pShData->nTownID )
				{
					SearchSPD_C_STRCMP(pName,pLength[i]/2,i,pShData);
				}
				pName += pLength[i]/2;
			}
		}
		else
		{
			return nuFALSE;
		}
	}
	else //婓眒冪梑善腔暮翹笢扆梑
	{
		nuDWORD nOldKeptCount = pShData->nKeptCount;
		nuBYTE nFoundDataMode = pShData->nFoundDataMode;
		nuDWORD i = 0;
		nuDWORD nLastIdx = pShData->pIndexList[ nOldKeptCount - 1 ];
		m_nASIdxCount = 0;
		pShData->nTotalFound = 0;
		pShData->nKeptCount = 0;
		pShData->nFoundDataMode = SPD_FOUNDDATA_MODE_DEFAULT;
		nuDWORD idx = 0;
		if( pShData->nCityID == (nuWORD)-1 )
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pName += pLength[idx]/2;
					++idx;
				}
				SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
			}
			
		}
		else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID == (nuWORD)-1 )
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pName += pLength[idx]/2;
					++idx;
				}
				if( pSPDD[idx].nCityID == pShData->nCityID )
				{
					SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
				}
			}
		}
		else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID != (nuWORD)-1 )
		{
			for( i = 0; i < nOldKeptCount ; ++i )
			{
				while ( idx < pShData->pIndexList[i] )
				{
					pName += pLength[idx]/2;
					++idx;
				}
				if( pSPDD[idx].nCityID == pShData->nCityID && pSPDD[idx].nTownID == pShData->nTownID )
				{
					SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
				}
			}
		}
		else 
		{
			return nuFALSE;
		}
		//樟哿梑遜羶衄梑善腔暮翹
		if( nFoundDataMode == SPD_FOUNDDATA_MODE_FIND_PART && idx < nTotalCount )
		{
			pName += pLength[idx];
			++idx;
			if( pShData->nCityID == (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{
					SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
					pName += pLength[idx]/2;
					++idx;
				}
			}
			else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID == (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{
					if( pShData->nCityID == pSPDD[idx].nCityID )
					{
						SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
						
					}
					pName += pLength[idx]/2;
					++idx;
					
				}
			}
			else if( pShData->nCityID != (nuWORD)-1 && pShData->nTownID != (nuWORD)-1 )
			{
				while (idx < nTotalCount )
				{
					if( pShData->nCityID == pSPDD[idx].nCityID && pShData->nTownID == pSPDD[idx].nTownID )
					{
						SearchSPD_C_STRCMP(pName,pLength[idx]/2,idx,pShData);
					}
					pName += pLength[idx]/2;
					++idx;
				}
			}
			else
			{
				return nuFALSE;
			}

		}
	}
	m_bReSearchSpd = nuTRUE;
	return nuTRUE;
}
nuBOOL CSearchSPDNumO::NextWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort)
{
	nuBOOL  LastWord=nuTRUE;			//?P?_?O?_???怮??@?茼r
	nuINT n=candylen;				//KeyWordy?r??
	
	nuWCHAR*  pWord = (nuWCHAR*)pWordBuf;
	//NURO_SEARCH_NORMALRES *n_pNORMALRES =(NURO_SEARCH_NORMALRES*)pRoadBuf;//?sRoad?W?棪}?C
	nuWCHAR   KEYNAME[128]={0};
	nuMemcpy(KEYNAME,wKeyWord,n*2);

	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	nuINT nTemp =0 ;
	nuUINT i;
	for (i=0; i<m_nDataIdx; i++)
	{
		nuBOOL bRes=nuFALSE;
		for(nuINT j=0;j<n;j++)
		{
			if( KEYNAME[j] != n_pNORMALRES[i].name[j] )
			{	
				bRes=nuTRUE;
				break;
			}
		}
		if(bRes)
			continue;
		if(0 != n_pNORMALRES[i].name[n] )					//?????怮??@?茼r 	LastWord = nuFALSE;
		{
			LastWord = nuFALSE;
		}
		if(0 == n_pNORMALRES[i].name[n] )					//?怮??@?茼r
		{
			continue;
		}
		if (0 == i||0 == n_iCount)								
		{
			pWord[n_iCount] = n_pNORMALRES[i].name[n];			//?臚@?茼r???J pWCHAR[0]
			n_iCount++;
		}
		else if (n_pNORMALRES[i].name[n] != pWord[n_iCount-1])	//???茼r???P?P???r?}?C[n_iCount-1]???r??
		{
			nuBOOL bisAdd = nuTRUE;
			nuUINT j;
			for (j=0; j<n_iCount; j++)
			{
				if (n_pNORMALRES[i].name[n] == pWord[j])			//???茼r?? ?h?[?J??BOOL=FALSE
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
	
	return LastWord;//?P?_?O?_???怮??@?茼r
}
nuUINT CSearchSPDNumO::GetSPDDataList(nuWORD& nMax,nuVOID* pBuffer, nuUINT nMaxLen)
{
	if(nMaxLen==0)
	{
		nMax = m_nDataIdx;
		return nuTRUE;
	}
	nuMemcpy(pBuffer, n_pNORMALRES,nMaxLen);
	return nuTRUE;
}
nuUINT CSearchSPDNumO:: GetRoadSKB(void* pSKBBuf) //Prosper 2011.12.07
{		
	nuMemcpy(pSKBBuf,&m_keyMask,sizeof(DYKEYMASK));
	return true;
}
nuUINT   CSearchSPDNumO::GetDYGetWordData(PDYWORDFORUI pDyData)
{
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);
	if(  pShData == NULL )
	{
		return 0;
	}
	nuDWORD nCount=0;
	nuDWORD *pIdxList=NULL;
	if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_PANA )
	{
		if( pShData->nTownID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pTownIdxList;
			nCount   = pShData->nCountOfTown;
		}
		else if( pShData->nCityID != (nuWORD)(-1) )
		{
			pIdxList = pShData->pCityIdxList;
			nCount   = pShData->nCountOfCity;
		}
		else
		{
			pIdxList = pShData->pIndexList;
			nCount   = pShData->nKeptCount;
		}
	}
	else if( ( m_searchSet.nMode & 0xffff0000 ) == SH_ENTER_MODE_NURO )
	{
		pIdxList = pShData->pIndexList;
		nCount   = pShData->nKeptCount;
	}
	else
	{
		return 0;
	}

	nuINT j=0,nIdx=0;
	nuINT n_Len=nuWcslen(m_nSearchName);

	nuINT x=-1; //Just Clear Data 
	m_fileSpd.GetOneDYData_WORD(NULL,NULL,NULL, NULL,x);

	if(m_nSearchName)
	{
		for(nuDWORD i = 0; i < pShData->nKeptCount; ++i)
		{
			nuINT n=0;
			if( !m_fileSpd.GetOneDYData_WORD(n_Len,m_nWordStartIdx[j++],&pDyData->Data[nIdx], pIdxList[i],n) )
			{
				pDyData->Data[n].nFindCount++;
				continue;
			}
			pDyData->Data[nIdx].nFindCount++;
			++nIdx;
		}
	}
	pDyData->Data[nIdx].name[0] = 0;

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

	pDyData->nTotalCount = nIdx;	
	
	return true;
}
nuBOOL	CSearchSPDNumO::NAME_Order(nuINT n)
{
/*	
	//nuINT n = m_nTotal;


	if (n==0) //?????Ƨ?
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
	PSPDSEARCHDATA pShData = (PSPDSEARCHDATA)m_pMmApi->MM_GetDataMassAddr(m_nSHMemIdx);


	nuWCHAR wsNumber[]={19968,20108,19977,22235,20116,20845,19971,20843,20061,21313,0};
	Trans_DATANAME temp  = {0}; //new nuWCHAR[NURO_SEARCH_BUFFER_MAX>>1];
	//nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
	//DYWORD   = {0};
	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=7;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	nuINT nTemp =0 ;
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

						}
					}
					else
					{

						if(parser > parser2)  
						{
							nuINT nLen  = nuWcslen(wsListName[nIdx].name);
							nuINT nLen2 = nuWcslen(wsListName[nIdx+1].name);
							if(nLen >= nLen2)
							{

								//	nuMemset(temp,0,sizeof(nuWCHAR)*NURO_SEARCH_BUFFER_MAX>>1);
								temp				=	wsListName[nIdx];
								wsListName[nIdx]	=	wsListName[nIdx+1];
								wsListName[nIdx+1]  =   temp;

								nTemp				=	pShData->pIndexList[nIdx];
								pShData->pIndexList[nIdx]		=	pShData->pIndexList[nIdx+1];
								pShData->pIndexList[nIdx+1]		=   nTemp;

							}			


						}
					}
				}
		}




		if(data!=NULL)
		{
			delete[] data;
			data = NULL;
		}
*/
return nuTRUE;
}
nuINT	CSearchSPDNumO::TransCode(nuINT code)
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
