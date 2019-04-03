// FileMapDanyinZ.cpp: implementation of the CFileMapDanyinZ class.
//
//////////////////////////////////////////////////////////////////////
#include "FileMapDanyinZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMapDanyinZ::CFileMapDanyinZ()
{
	m_nMapIdx = -1;
	m_pFile	= NULL;
	m_pFile2= NULL;
	m_nDanYinAMemIdx = -1;
	m_nDanYinBMemIdx = -1;
	m_nDanYinB2MemIdx = -1;
	m_nDanYinDEMemIdx = -1;
	m_nDYSearchMode = -1;
//	m_bNeedSort = nuFALSE;
//	wsListName = NULL;
//	data = NULL;
}

CFileMapDanyinZ::~CFileMapDanyinZ()
{
	Free();
}

nuBOOL CFileMapDanyinZ::Initialize()
{
	m_nMapIdx = -1;
	m_pFile	= NULL;
	m_nDanYinAMemIdx = -1;
	m_nDanYinB2MemIdx = -1;
	m_nDanYinDEMemIdx = -1;
	m_bNeedReadB2 = nuTRUE;
	m_bNeedReadDE = nuTRUE;
	m_SearchRes = NULL;    //Prosper 2012.12

	return nuTRUE;
}

nuVOID CFileMapDanyinZ::Free()
{
	if( m_nDanYinAMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinAMemIdx);
	}
	if( m_nDanYinBMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinBMemIdx);
	}
	if( m_nDanYinB2MemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinB2MemIdx);
	}
	if( m_nDanYinDEMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinDEMemIdx);
	}
	if(	m_SearchRes)    //Prosper 2012.12
	{
		delete []m_SearchRes;
		m_SearchRes=NULL;
	}

	CloseFile();
}

nuBOOL CFileMapDanyinZ::SetMapID(nuWORD nMapIdx)
{
	if( m_pFile == NULL || m_nMapIdx != nMapIdx )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinAMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinBMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinB2MemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinDEMemIdx);
		m_pMm_SHApi->MM_CollectDataMem(0);
		//
		m_nMapIdx = nMapIdx;
		if( !OpenFile() )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFread(&m_dyHead, sizeof(DANYINHEAD), 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
		nuLONG	nSize = {0}; 
		nuPBYTE pData = NULL;
		if(m_nDYSearchMode == -1)
		{
			nSize = m_dyHead.nTotalCount * sizeof(DANYIN_A);
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nDanYinAMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, sizeof(DANYINHEAD), NURO_SEEK_SET) != 0 ||
			nuFread(pData, sizeof(DANYIN_A), m_dyHead.nTotalCount, m_pFile) != m_dyHead.nTotalCount )
		{
			return nuFALSE;
		}

		}
		else //add by Prosper , if need load name buffer
		{
			nSize = m_dyHead.nTotalCount * sizeof(DYNODE_B_1);
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nDanYinBMemIdx);
			if( pData == NULL )
			{
				return nuFALSE;
			}
			if( nuFseek(m_pFile, sizeof(DANYINHEAD)+m_dyHead.nTotalCount * sizeof(DANYIN_A), NURO_SEEK_SET) != 0 ||
				nuFread(pData, sizeof(DYNODE_B_1), m_dyHead.nTotalCount, m_pFile) != m_dyHead.nTotalCount )
			{
				return nuFALSE;
			}
		}
		//Allocate Data memory from MemMgr.dll
		nSize = m_dyHead.nTotalCount * sizeof(DYNODE_B_2);
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nDanYinB2MemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		m_bNeedReadB2 = nuTRUE;
		nSize = m_dyHead.nCityCount * DY_CITY_NAME_LEN + m_dyHead.nTownCount * DY_TOWN_NAME_LEN;
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nDanYinDEMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		m_bNeedReadDE = nuTRUE;
	} 
	return nuTRUE;
}

nuBOOL CFileMapDanyinZ::OpenFile()
{
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".Dy"));
	if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID CFileMapDanyinZ::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuPVOID CFileMapDanyinZ::GetDanyinA(nuDWORD *pCount)
{
	if( m_nDanYinBMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinBMemIdx);
	}
	if(m_SearchRes==NULL)
	{
		m_SearchRes = new DYWORD[NURO_SEARCH_BUFFER_MAX_DYPOI];
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		m_nDYWordIdx=0;
	}
	else
	{
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		m_nDYWordIdx=0;
	}
	
	if( pCount )
	{
		*pCount = m_dyHead.nTotalCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nDanYinAMemIdx);
}
nuPVOID CFileMapDanyinZ::GetDanyinB(nuDWORD *pCount)
{
	if( m_nDanYinAMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nDanYinAMemIdx);
	}
	if(m_SearchRes==NULL)
	{
		m_SearchRes = new DYWORD[NURO_SEARCH_BUFFER_MAX_DYPOI];
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		m_nDYWordIdx=0;
	}
	else
	{
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		m_nDYWordIdx=0;
	}
	if( pCount )
	{
		*pCount = m_dyHead.nTotalCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nDanYinBMemIdx);
}
nuPVOID CFileMapDanyinZ::GetDanyinB2(nuDWORD *pCount)
{
	nuPVOID pData = m_pMm_SHApi->MM_GetDataMassAddr(m_nDanYinB2MemIdx);
	if( pData == NULL )
	{
		return NULL;
	}
	if( m_bNeedReadB2 )
	{
		if( m_pFile == NULL )
		{
			return NULL;
		}
		nuDWORD nSize = m_dyHead.nTotalCount * sizeof(DYNODE_B_2);
		if( nuFseek(m_pFile, sizeof(DANYINHEAD) + ( sizeof(DANYIN_A) + sizeof(DYNODE_B_1) ) * m_dyHead.nTotalCount, NURO_SEEK_SET) != 0 ||
			nuFread(pData, sizeof(DYNODE_B_2), m_dyHead.nTotalCount, m_pFile) != m_dyHead.nTotalCount )
		{
			return NULL;
		}
		// Added by Damon 2012 05/21
		m_bNeedReadB2 = nuFALSE;
		//-------------------------
	}
	if( pCount )
	{
		*pCount = m_dyHead.nTotalCount;
	}
	return pData;
}
nuBOOL CFileMapDanyinZ::GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n)
{
	if(n==-1)
	{
		m_nDYWordIdx = 0 ;
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		return nuFALSE;
	}
	if( pDyNode == NULL || nIdx >= m_dyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}
	nuWCHAR sComparedName[128]={0};
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_dyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(&sComparedName, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	#ifdef ZENRIN
	nuINT i=0,l=0;

	nuINT nLen = nuWcslen(sComparedName);
	for(;i<nLen;i++)
	{
		
		if(sComparedName[i]!=32)
		{
			/*if(sComparedName[i]>=97&&sComparedName[i]<=97+26)
				sComparedName[l++]=sComparedName[i]-97+65;
			else*/
				sComparedName[l++]=sComparedName[i];
		}
	}
	sComparedName [l] =0;
	sComparedName[DY_B_NAME_LEN/2] = 0;
	#endif
	nuBOOL bSameName=nuFALSE;
	nuBOOL bReName=nuFALSE;
	nuWCHAR sTemp[DY_B_NAME_LEN/2]={0};
	nuWCHAR sComparedName2[DY_B_NAME_LEN/2+1]={0};
	
	nuINT nLens=nuWcslen(sComparedName);
	if(nLens>=21)
		nLens=20;
	nuINT nSpace=0;
	for(nuINT i=0,j=0;i<nLens;i++)
	{
		if(sComparedName[i]!=32)
		{
			sComparedName2[j]=sComparedName[i];	
			j++;
		}
		else
		{
			nSpace++;
			int ll=0;
		}
	}
	if(nIdx==6510)
		int l=0;
	nuINT nShift=0;
	#ifndef ZENRIN
	if(nWordStart<nLens-1)
	{

	for(nuINT k=1;k<=nWordStart+nSpace;k++)
	{
		if(sComparedName[k] == 45 ) 
		{
			nuBOOL bFind=nuFALSE;
			if( sComparedName[k+1] > 1000 )
			{
				nShift++;
				bFind=nuTRUE;
			}
			if( sComparedName[k-1] >= 1000)
			{
				if(!bFind)
				nShift++;
			}
			if(sComparedName[k+1]<=1000 &&sComparedName[k-1]<=1000) //PASS "Not Chinese"-"Not Chinese"
			{
				n=-1;
				return nuFALSE;
			}
			//if(sComparedName[k] == 45)
			break;
		}
		//if(sComparedName[k] == 32)
		//nShift++;

			if(sComparedName[k] == 45 && sComparedName[k+1]<1000)
			int l=0;
	}

	}
#endif
	nWordStart+=nShift;
	for(nuINT k=0,l=0;k<nWords;l++)
	{
		#ifndef ZENRIN
		if(sComparedName2[nWordStart+l] != 45 )
		{
			if(sComparedName2[nWordStart+l]<1000)
				int ll=0;
			
			sTemp[k]=sComparedName2[nWordStart+l];
			k++;
		}
		#else
		if(1)//sComparedName2[nWordStart+l] != 45 )
		{
			
			sTemp[k]=sComparedName2[nWordStart+l];
			k++;
		}
		#endif
		
		
	}
	//sTemp[nWords]=0;

	nuINT n_CountIdx=0;
	for ( nuUINT l = 0; l < m_nDYWordIdx; l++ )
	{
		nuBOOL bSame=nuTRUE;
		
		for(nuINT i=0;i<nWords;i++)
		{
			if(m_SearchRes[l].name[i]!=sTemp[i])
			{
				bSame=nuFALSE;
				break;
			}
		}
		if (bSame)
		{
			++m_SearchRes[l].nFindCount;
			
			bSameName = nuTRUE;						    
			n_CountIdx=l;
			break;
		}
	}
	if(!bSameName)
	{
		if(m_nDYWordIdx >= NURO_SEARCH_BUFFER_MAX_DYPOI)
		{
			n = -1;
			return nuFALSE;
		}
		nuWcscpy(m_SearchRes[m_nDYWordIdx].name,sTemp);	
		m_nDYWordIdx++;
	}

	if(bSameName)
	{
		n = n_CountIdx;
		return nuFALSE;
	}
	else
	{
		nuWcscpy(pDyNode->name,sTemp);
		return nuTRUE;
	}
	
}
nuBOOL CFileMapDanyinZ::GetOneDYData_WORD_KEY(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n,nuVOID* pPoiKeyName)
{
	if(n==-1)
	{
		m_nDYWordIdx = 0 ;
		nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		return nuFALSE;
	}
	if( pDyNode == NULL || nIdx >= m_dyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}
	nuWCHAR sComparedName[DY_B_NAME_LEN/2+1]={0};
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_dyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(&sComparedName, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuWCHAR sComparedName3[DY_B_NAME_LEN/2+1]={0};
	nuMemcpy(sComparedName3,pPoiKeyName,40);

	/*int l=nuWcscmp(sComparedName3,sComparedName);
	if(nuWcsncmp(sComparedName,sComparedName3,nuWcslen(sComparedName3))!=0)
	{
		n = -1;
		return nuFALSE;
	}*/
	nuBOOL bFindTag= nuTRUE;
	nuINT m = 0;
	nuINT nKEYLen = nuWcslen(sComparedName);
	nuINT nKEYLen2 = nuWcslen(sComparedName3);
	for(nuINT i=0;i<nKEYLen;i++)
	{
		#ifndef ZENRIN
		if(sComparedName[i]==45)
		{
			continue;
		}
		#endif
		if(sComparedName[i]!=sComparedName3[m])
		{
			m=0;
			continue;
		}
		if(sComparedName[i]==sComparedName3[m])
		{
			m++;
		}
		if(m==nKEYLen2)
		{
			break;
		}

	}
	if(m!=nKEYLen2)
	{
		n=-1;
		return nuFALSE;
	}

	nuBOOL bSameName=nuFALSE;
	nuBOOL bReName=nuFALSE;
	nuWCHAR sTemp[DY_B_NAME_LEN/2]={0};
	nuWCHAR sComparedName2[DY_B_NAME_LEN/2+1]={0};

	nuINT nLens=nuWcslen(sComparedName);
	if(nLens>=21)
		nLens=20;
	nuINT nSpace=0;
	for(nuINT i=0,j=0;i<nLens;i++)
	{
		if(sComparedName[i]!=32)
		{
			sComparedName2[j]=sComparedName[i];	
			j++;
		}
		else
		{
			nSpace++;
			int ll=0;
		}
	}
	if(nIdx==6510)
		int l=0;
	nuINT nShift=0;
	#ifndef ZENRIN
	if(nWordStart<nLens-1)
	{

		for(nuINT k=1;k<=nWordStart+nSpace;k++)
		{
			if(sComparedName[k] == 45 ) 
			{
				nuBOOL bFind=nuFALSE;
				if( sComparedName[k+1] > 1000 )
				{
					nShift++;
					bFind=nuTRUE;
				}
				if( sComparedName[k-1] >= 1000)
				{
					if(!bFind)
						nShift++;
				}
				if(sComparedName[k+1]<=1000 &&sComparedName[k-1]<=1000) //PASS "Not Chinese"-"Not Chinese"
				{
					n=-1;
					return nuFALSE;
				}
				//if(sComparedName[k] == 45)
				break;
			}
			//if(sComparedName[k] == 32)
			//nShift++;

			if(sComparedName[k] == 45 && sComparedName[k+1]<1000)
				int l=0;
		}

	}
	#endif
	nWordStart+=nShift;
	for(nuINT k=0,l=0;k<nWords;l++)
	{
		#ifndef ZENRIN
		if(sComparedName2[nWordStart+l] != 45 )
		{
		
			sTemp[k]=sComparedName2[nWordStart+l];
			k++;
		}
		#else
		if(1)
		{
		
			sTemp[k]=sComparedName2[nWordStart+l];
			k++;
		}
		#endif


	}
	//sTemp[nWords]=0;

	nuINT n_CountIdx=0;
	for ( nuUINT l = 0; l < m_nDYWordIdx; l++ )
	{
		nuBOOL bSame=nuTRUE;

		for(nuINT i=0;i<nWords;i++)
		{
			if(m_SearchRes[l].name[i]!=sTemp[i])
			{
				bSame=nuFALSE;
				break;
			}
		}
		if (bSame)
		{
			++m_SearchRes[l].nFindCount;

			bSameName = nuTRUE;						    
			n_CountIdx=l;
			break;
		}
	}
	if(!bSameName)
	{
		if(m_nDYWordIdx >= NURO_SEARCH_BUFFER_MAX_DYPOI)
		{
			n = -1;
			return nuFALSE;
		}
		nuWcscpy(m_SearchRes[m_nDYWordIdx].name,sTemp);	
		m_nDYWordIdx++;
	}

	if(bSameName)
	{
		n = n_CountIdx;
		return nuFALSE;
	}
	else
	{
		nuWcscpy(pDyNode->name,sTemp);
		return nuTRUE;
	}

}

nuBOOL CFileMapDanyinZ::GetOneDYData(PDYNODEFORUI pDyNode, nuDWORD nIdx)
{
	if( pDyNode == NULL || nIdx >= m_dyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pDyNode, 0, sizeof(DYNODEFORUI));
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_dyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(pDyNode->wsName, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	#ifdef ZENRIN
	nuINT i=0,l=0;

	nuINT nLen = nuWcslen(pDyNode->wsName);
	for(;i<nLen;i++)
	{
		if(pDyNode->wsName[i]!=32)
		{
			if(pDyNode->wsName[i]>=97&&pDyNode->wsName[i]<=97+26)
				pDyNode->wsName[l++]=pDyNode->wsName[i]-97+65;
			else
				pDyNode->wsName[l++]=pDyNode->wsName[i];
		}
	}
	pDyNode->wsName [l] =0;
	pDyNode->wsName[DY_B_NAME_LEN/2] = 0;
	#endif
	nuBOOL bSameName=nuFALSE;
	for ( nuUINT l = 0; l < m_nDYWordIdx; l++ )
	{
		if ( 0 == nuWcsncmp(m_SearchRes[l].name, pDyNode->wsName, nuWcslen(pDyNode->wsName)))
		{
			++m_SearchRes[l].nFindCount;
			nuMemcpy(m_SearchRes[l].name,pDyNode->wsName,2*DY_NAME_FORUI_NUM);
			bSameName = nuTRUE;						    
			break;
		}
	}
	if(!bSameName)
	{
		m_SearchRes[m_nDYWordIdx].nFindCount = 1;
		nuMemcpy(m_SearchRes[m_nDYWordIdx].name,pDyNode->wsName,2*DY_NAME_FORUI_NUM);
		
		m_nDYWordIdx++;
	}
	pDyNode->nStroke	= pDyNode2[nIdx].nStroke;
	pDyNode->nType		= pDyNode2[nIdx].nType;
	pDyNode->nCityID	= pDyNode2[nIdx].nCityID;
	pDyNode->nTownID	= pDyNode2[nIdx].nTownID;
	nuPBYTE pStr = (pDEData + pDyNode2[nIdx].nCityID * DY_CITY_NAME_LEN);
	nuMemcpy(pDyNode->wsCityName, pStr, DY_CITY_NAME_LEN);
	pStr = pDEData + pDyNode2[nIdx].nTownID * DY_TOWN_NAME_LEN + m_dyHead.nCityCount * DY_CITY_NAME_LEN;
	nuMemcpy(pDyNode->wsTownName, pStr, DY_TOWN_NAME_LEN);
	return nuTRUE;
}
nuBOOL CFileMapDanyinZ::GetOneDYData_PAGE(PDYNODEFORUI pDyNode, nuDWORD nIdx ,nuVOID* pPoiKeyName, nuINT nStartPos)
{
	if( pDyNode == NULL || nIdx >= m_dyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}
	nuWCHAR wsKeyWord[DY_B_NAME_LEN/2]={0};
	nuMemcpy(wsKeyWord,pPoiKeyName,sizeof(wsKeyWord));
	
	nuWCHAR wsTemp[20]={0};
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_dyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(wsTemp, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	if(nIdx==58652)
		int k=0;
	nuBOOL bSame=nuTRUE;
	nuBOOL bFindHead=nuFALSE;
	nuINT nWords=nuWcslen(wsKeyWord);
	nuINT nWords2=nuWcslen(wsTemp);
	nuINT nFind=1;

	nuINT nShift=0;
	for(nuINT k=0;k<=nStartPos+nShift;k++)
	{
	    #ifndef ZENRIN
		if(wsTemp[k] == 45 && wsTemp[k+1] >= 1000 ) 
		{
			nShift++;
			
		}
		#endif
		
		if(wsTemp[k] == 32)	
			nShift++;
	//	#endif
	}

	nStartPos+=nShift;
	for(nuINT i=0;i<nWords;i++)
	{
		if(bFindHead)
			break;
		if(wsTemp[nStartPos+i] == wsKeyWord[0])
		{
			bFindHead=nuTRUE;
			for(int j=i+1,k=1;(nStartPos+j)<nWords2;j++)
			{
				if(wsKeyWord[k]==0)
				{
					break;
				}
			/*	 2013 0205
			if(wsTemp[nStartPos+j] != wsKeyWord[k])
				{
					if(wsTemp[nStartPos+j] == 45 )
					{
						continue;
					}
					else
					{
						break;
					}
				}
			*/	
			//	#ifdef NoNAME
			#ifdef ZENRIN
				if(wsTemp[nStartPos+j] == 32)
				{
					continue;
				}
			#else
				if(wsTemp[nStartPos+j] == 45 || wsTemp[nStartPos+j] == 32)
				{
					continue;
				}	
			#endif
			//	#endif
				if(wsTemp[nStartPos+j] == wsKeyWord[k])
				{
					nFind++;
					k++;
				}
				else
				{
					break;
				}
			}
			
		}
	}
	if(nFind!=nWords)
		bSame=nuFALSE;

	if(!bFindHead)
		bSame=nuFALSE;

	if (bSame)
	{
		nuMemcpy(pDyNode->wsName,wsTemp,sizeof(wsTemp));

	}
	else
		return nuFALSE;
	
	
	
	pDyNode->nStroke	= pDyNode2[nIdx].nStroke;
	pDyNode->nType		= pDyNode2[nIdx].nType;
	pDyNode->nCityID	= pDyNode2[nIdx].nCityID;
	pDyNode->nTownID	= pDyNode2[nIdx].nTownID;
	nuPBYTE pStr = (pDEData + pDyNode2[nIdx].nCityID * DY_CITY_NAME_LEN);
	nuMemcpy(pDyNode->wsCityName, pStr, DY_CITY_NAME_LEN);
	pStr = pDEData + pDyNode2[nIdx].nTownID * DY_TOWN_NAME_LEN + m_dyHead.nCityCount * DY_CITY_NAME_LEN;
	nuMemcpy(pDyNode->wsTownName, pStr, DY_TOWN_NAME_LEN);
	return nuTRUE;
}


nuPVOID CFileMapDanyinZ::GetDanyinDE(nuWORD *pDCount, nuWORD *pECount)
{
	nuPVOID pData = m_pMm_SHApi->MM_GetDataMassAddr(m_nDanYinDEMemIdx);
	if( pData == NULL )
	{
		return NULL;
	}
	if( m_bNeedReadDE )
	{
		nuDWORD nSize = sizeof(DANYINHEAD) + 
			(sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) + sizeof(DYNODE_B_3) + sizeof(DYNODE_C)) * m_dyHead.nTotalCount;
		if( m_pFile == NULL || nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			return NULL;
		}
		nSize = m_dyHead.nCityCount * DY_CITY_NAME_LEN + m_dyHead.nTownCount * DY_TOWN_NAME_LEN;
		if( nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			return NULL;
		}
	}
	if( pDCount != NULL )
	{
		*pDCount = (nuWORD)m_dyHead.nCityCount;
	}
	if( pECount != NULL )
	{
		*pECount = (nuWORD)m_dyHead.nTownCount;
	}
	return pData;
}

nuBOOL CFileMapDanyinZ::GetOneDYDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	if( pDyDetailForUI == NULL || m_pFile == NULL || nIdx >= m_dyHead.nTotalCount )
	{
		return nuFALSE;
	}
	nuDWORD nSize = sizeof(DANYINHEAD) + 
		( sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) + sizeof(DYNODE_B_3) ) * m_dyHead.nTotalCount + 
		sizeof(DYNODE_C) * nIdx;
	DYNODE_C dyNodeC = {0};
	if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
		nuFread(&dyNodeC, sizeof(DYNODE_C), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	pDyDetailForUI->point = dyNodeC.point;
	nuBYTE nOtherLen = 0;
	DYNODE_B_3 dyNodeB3 ={0};
	nSize = sizeof(DANYINHEAD) + 
		( sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) ) * m_dyHead.nTotalCount + 
		sizeof(DYNODE_B_3) * nIdx;
	if( nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 ||
		nuFread(&dyNodeB3, sizeof(DYNODE_B_3), 1, m_pFile) != 1 ||
		nuFseek(m_pFile, dyNodeB3.nOtherAddr, NURO_SEEK_SET) != 0	||
		nuFread(&nOtherLen, 1, 1, m_pFile) != 1 ||
		nuFread(pDyDetailForUI->wsOther, nOtherLen, 1, m_pFile) != 1)
	{
		return nuFALSE;
	}
	pDyDetailForUI->wsOther[nOtherLen/sizeof(nuWCHAR)] = 0;
	return nuTRUE;
}

nuUINT CFileMapDanyinZ::SetSearchMode(nuINT nType) 
{
	m_nDYSearchMode = nType;
	return nuTRUE;
}
nuBOOL CFileMapDanyinZ::ReadData(nuUINT addr, nuVOID *p, nuUINT len)
{
	nuFseek(m_pFile2 ,addr, NURO_SEEK_SET);
	
	if(nuFread(p, len, 1, m_pFile2)!=1)
	{
		return false;
	}

	return true;
	

}

nuBOOL CFileMapDanyinZ::OpenNewFile(nuWORD nMapIdx)
{
	CloseNewFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".Dy"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile2 = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile2 )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID CFileMapDanyinZ::CloseNewFile()
{
	if( m_pFile2 != NULL )
	{
		nuFclose(m_pFile2);
		m_pFile2 = NULL;
	}
}

nuBOOL CFileMapDanyinZ::GetOneDYData_GETNAME(PDYNODEFORUI pDyNode, nuDWORD nIdx)
{
	if( pDyNode == NULL || nIdx >= m_dyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pDyNode, 0, sizeof(DYNODEFORUI));
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_dyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(pDyNode->wsName, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	pDyNode->nStroke	= pDyNode2[nIdx].nStroke;
	pDyNode->nType		= pDyNode2[nIdx].nType;
	pDyNode->nCityID	= pDyNode2[nIdx].nCityID;
	pDyNode->nTownID	= pDyNode2[nIdx].nTownID;
	nuPBYTE pStr = (pDEData + pDyNode2[nIdx].nCityID * DY_CITY_NAME_LEN);
	nuMemcpy(pDyNode->wsCityName, pStr, DY_CITY_NAME_LEN);
	pStr = pDEData + pDyNode2[nIdx].nTownID * DY_TOWN_NAME_LEN + m_dyHead.nCityCount * DY_CITY_NAME_LEN;
	nuMemcpy(pDyNode->wsTownName, pStr, DY_TOWN_NAME_LEN);
	return nuTRUE;
}
nuINT   CFileMapDanyinZ::GetOneDYData_PAGE_START(nuVOID* pPoiKeyName)
{
	nuWCHAR  m_nSearchPoiKeyName[20];
    nuMemcpy(m_nSearchPoiKeyName,pPoiKeyName,sizeof(m_nSearchPoiKeyName));

	nuINT nTotal=0;
	nuBOOL bFIND=nuFALSE;
	nuINT i=0;
	for(;i<m_nDYWordIdx;i++)
	{
		if(nuWcscmp(m_SearchRes[i].name,m_nSearchPoiKeyName)==0)
		{
			bFIND = true;
			//nCount = m_SearchRes[i].nFindCount;
			break;
			//return ;
		}	
		else
		{
			if(!bFIND)
				nTotal+=m_SearchRes[i].nFindCount;
		}
		//nCount+=;
	}
	
	return i;
}

nuINT   CFileMapDanyinZ::GetWordIndx(nuVOID* pPoiKeyName)
{
	nuWCHAR  m_nSearchPoiKeyName[20];
	nuMemcpy(m_nSearchPoiKeyName,pPoiKeyName,sizeof(m_nSearchPoiKeyName));

	nuINT nTotal=0;
	nuBOOL bFIND=nuFALSE;
	nuINT i=0;
	for(;i<m_nDYWordIdx;i++)
	{
		if(nuWcscmp(m_SearchRes[i].name,m_nSearchPoiKeyName)==0)
		{
			bFIND = true;
			//nCount = m_SearchRes[i].nFindCount;
			break;
			//return ;
		}
		//nCount+=;
	}

	return i;
}
