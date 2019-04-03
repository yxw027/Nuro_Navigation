// FileRn2O.cpp: implementation of the CFileRn2O class.
//
//////////////////////////////////////////////////////////////////////
#include "FileRn2O.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileRn2O::CFileRn2O()
{
	m_nNm_SearchResMemIdx = -1;	
	//m_SearchRes = NULL;
}

CFileRn2O::~CFileRn2O()
{
	if( m_nNm_SearchResMemIdx != -1)
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNm_SearchResMemIdx);
	}
}

nuBOOL CFileRn2O::Initialize()
{
	m_nRnAMemIdx = (nuWORD)(-1);
	m_nRnBorCMemIdx = (nuWORD)(-1);
	m_nRnDMemIdx = (nuWORD)(-1);
	m_nRnEMemIdx = (nuWORD)(-1);
	nuMemset(&m_rn2Header,0,sizeof(m_rn2Header));
	m_pFile = NULL;
	m_nMapIdx = -1;
	m_nMode = 0;
	return nuTRUE;
}

nuVOID CFileRn2O::Free()
{
	if(m_nRnAMemIdx != (nuWORD)(-1) )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnAMemIdx);
	}
	if( m_nRnBorCMemIdx != (nuWORD)(-1) )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnBorCMemIdx);
	}
	if( m_nRnDMemIdx != (nuWORD)(-1) )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnDMemIdx);
	}
	if( m_nRnEMemIdx != (nuWORD)(-1) )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnEMemIdx);
	}
	/*if(m_SearchRes)
	{
		delete []m_SearchRes;
		m_SearchRes = NULL;
	}*/
	closeFile();
}

nuBOOL CFileRn2O::OpenFile(const nuTCHAR* ptsFileEx)
{
	if(ptsFileEx == NULL )
	{
		return nuFALSE;
	}
	closeFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile,ptsFileEx);
	if(  !m_pFsApi->FS_FindFileWholePath(m_nMapIdx,tsFile,NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen( tsFile,NURO_FS_RB);
	if( m_pFile )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID CFileRn2O::closeFile()
{
	if( m_pFile != NULL )
	{
		nuFclose( m_pFile);
		m_pFile = NULL;
	}
}

nuBOOL CFileRn2O::SetFileMode(nuUINT nMode)
{
	if( m_nMode != nMode )
	{
		m_nMode = nMode;
		closeFile();
	}
	return nuTRUE;
}

nuBOOL CFileRn2O::SetMapIdx(nuUINT nMapIdx)
{
	if( m_nMode == SH_ENTER_MODE_NO )
	{
		return nuFALSE;
	}
	if( m_pFile == NULL || m_nMapIdx != nMapIdx )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnAMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnBorCMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnDMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nRnEMemIdx);
		m_pMm_SHApi->MM_CollectDataMem(0);

		m_nMapIdx = nMapIdx;
		if( !OpenFile( nuTEXT(".SDN") ) )
		{
			return nuFALSE;
		}
		if( nuFseek( m_pFile,0,NURO_SEEK_SET ) !=0 ||
			nuFread(&m_rn2Header,sizeof(RN2_HEADER),1,m_pFile) != 1 )
		{
			return nuFALSE;
		}
		//分配A区地址，并读到内存
		long nSize = m_rn2Header.nRoadCount;
		nuPBYTE pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize,&m_nRnAMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile,sizeof(RN2_HEADER),NURO_SEEK_SET) != 0 ||
			nuFread(pData,nSize,1,m_pFile) != 1 )
		{
			m_pMm_SHApi->MM_RelDataMemMass(&m_nRnAMemIdx);
			return nuFALSE;
		}
		//分配B或C区地址，并读到内存
		nuLONG nDis = 0;
		if( nuLOWORD(m_nMode) == SH_ENTER_MODE_DY )
		{
			nSize = m_rn2Header.nRoadNameAddr-m_rn2Header.nZhuyinAddr;
			nDis = m_rn2Header.nZhuyinAddr;
		}
		else if( nuLOWORD(m_nMode) == SH_ENTER_MODE_NAME )
		{
			nSize = m_rn2Header.nCityTownAddr-m_rn2Header.nRoadNameAddr;
			nDis = m_rn2Header.nRoadNameAddr;
		}
		else
		{
			return nuFALSE;
		}
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize,&m_nRnBorCMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek( m_pFile,nDis,NURO_SEEK_SET ) != 0 ||
			nuFread(  pData,nSize,1,m_pFile ) != 1 )
		{
			m_pMm_SHApi->MM_RelDataMemMass(&m_nRnBorCMemIdx);
			return nuFALSE;
		}
	/*	nuBYTE *pLength = (nuBYTE*)m_pMm_SHApi->MM_GetDataMassAddr( m_nRnAMemIdx );
		for( int i = 0; i < 8528; ++i )
		{
			//nAddr += (pLength[i]<<1);//2 Bytes RoadName 
			pData= pData + pLength[i];
	}
	nuWCHAR *w;
		w = new nuWCHAR[pLength[8528]/2+1];
		nuMemcpy(w,pData,pLength[8528]);
		w[pLength[8528]/2] = 0;
		MessageBoxW(NULL,w,0,NULL);
	delete []w;*/
	
		//分配E区地址，并且读到内存
		nSize = m_rn2Header.nRoadCount;
		nDis = m_rn2Header.nCityTownAddr+sizeof(RN2_D_NODE)*m_rn2Header.nRoadCount;
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize,&m_nRnEMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile,nDis,NURO_SEEK_SET ) != 0 ||
			nuFread(pData,nSize,1,m_pFile) != 1 )
		{
			m_pMm_SHApi->MM_RelDataMemMass(&m_nRnEMemIdx);
			return nuFALSE;
		}

	}
	return nuTRUE;
}

nuPVOID CFileRn2O::GetRdALength(nuDWORD *pCount)
{
	if( pCount )
	{
		*pCount = m_rn2Header.nRoadCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr( m_nRnAMemIdx );
}

nuPVOID CFileRn2O::GetRdStroke(nuDWORD *pCount)
{
	if( pCount )
	{
		*pCount = m_rn2Header.nRoadCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr( m_nRnEMemIdx );
}

nuPVOID CFileRn2O::GetRdBOrCString(nuDWORD *pCount)
{
	if( pCount )
	{
		*pCount = m_rn2Header.nRoadCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nRnBorCMemIdx);
}

nuPVOID CFileRn2O::GetRdDData(nuDWORD *pCount)
{
	nuPBYTE pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMassAddr(m_nRnDMemIdx);
	if( pData == NULL )
	{
		long nSize = sizeof(RN2_D_NODE)*m_rn2Header.nRoadCount;
		if( nSize == 0 || m_pFile == NULL )
		{
			return NULL;
		}
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize,&m_nRnDMemIdx);
		if( pData == NULL )
		{
			return NULL;
		}
		if( nuFseek( m_pFile, m_rn2Header.nCityTownAddr,NURO_SEEK_SET) != 0 ||
			nuFread(pData,nSize,1,m_pFile) != 1 )
		{
			m_pMm_SHApi->MM_RelDataMemMass(&m_nRnDMemIdx);
			return NULL;
		}
	}
	if( pCount )
	{
		*pCount = m_rn2Header.nRoadCount;
	}
	return pData;
}

nuBOOL CFileRn2O::GetRdOneData(PSH_ITEM_FORUI pItem, nuDWORD nIdx)
{
	if( pItem == NULL || nIdx >= m_rn2Header.nRoadCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	// Get CityID and TownID
	PRN2_D_NODE pRn2DNode = (PRN2_D_NODE)GetRdDData(NULL);
	if( pRn2DNode == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pLength = (nuPBYTE)GetRdALength(NULL);
	if( pLength == NULL )
	{
		return nuFALSE;
	}
	nuMemset( pItem,0,sizeof(SH_ITEM_FORUI));
	/*Get RoadName Info From C Addr*/
	nuDWORD i = 0;
	nuLONG nAddr = m_rn2Header.nRoadNameAddr;
	for( ; i < nIdx; ++i )
	{
		//nAddr += (pLength[i]<<1);//2 Bytes RoadName 
		nAddr= nAddr + pLength[i];
	}
	nuSIZE nNum = NURO_MIN((SH_NAME_FORUI_NUM-1),pLength[i]/2);
	if( nuFseek( m_pFile, nAddr,NURO_SEEK_SET) != 0 ||
		nuFread( pItem->wsName,sizeof(nuWCHAR),nNum,m_pFile) != nNum )
	{
		return nuFALSE;
	}
	/*Get Stroke Info In E Addr*/
	nuPBYTE pStroke = (nuPBYTE)GetRdStroke( NULL );
	pItem->nStroke = pStroke[nIdx];
	pItem->nCityID = pRn2DNode[nIdx].nCityID;
	pItem->nTownID = pRn2DNode[nIdx].nTownID;

	/*Get CityTownName */
	//Open .CB File 
	nuFILE* CB = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile,nuTEXT(".CB"));
	if(  !m_pFsApi->FS_FindFileWholePath(m_nMapIdx,tsFile,NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	CB = nuTfopen( tsFile,NURO_FS_RB );
	if( CB == NULL )
	{
		return nuFALSE;
	}
	SEARCH_CB_HEADER CBHeader = {0};
	if( nuFseek( CB,0,NURO_SEEK_SET ) != 0 ||
		nuFread( &CBHeader,sizeof(SEARCH_CB_HEADER),1,CB ) != 1 )
	{
		nuFclose(CB);
		CB=NULL;
		return nuFALSE;
	}
	if( nuFseek( CB, sizeof(SEARCH_CB_HEADER)+ EACH_CITY_NAME_LEN * pRn2DNode[nIdx].nCityID,NURO_SEEK_SET ) != 0 ||
		nuFread( pItem->wsCityName,EACH_CITY_NAME_LEN,1,CB ) != 1 )
	{
		nuFclose(CB);
		CB=NULL;
		return nuFALSE;
	}
	nuDWORD nFileLen = nuFgetlen(CB); //获取文件长度
	nAddr = nFileLen - CBHeader.nTownCount*sizeof(nuDWORD);//获取按照CityTown排序的索引开头地址
	nuDWORD nTownInfo = 0;
	SEARCH_CB_TOWNINFO TOWNINFO = {0};
	for( i = 0; i < CBHeader.nTownCount; i++ )
	{
		//读取每个按照CityTown排序的索引地址
		if( nuFseek(CB,nAddr+i*sizeof(nuDWORD),NURO_SEEK_SET) != 0 ||
			nuFread( &nTownInfo,sizeof(nuDWORD),1,CB ) != 1 )
		{
			nuFclose(CB);
			CB=NULL;
			return nuFALSE;
		}
		//读取TownInfo结构
		if( nuFseek( CB,nTownInfo,NURO_SEEK_SET) != 0 ||
			nuFread( &TOWNINFO, sizeof(TOWNINFO),1,CB ) != 1 )
		{
			nuFclose(CB);
			CB=NULL;
			return nuFALSE;
		}
		//从读出来的TownInfo结构里面的Citycode和TownID和设定的 CityID以及TownID比对
		if( pRn2DNode[nIdx].nCityID == TOWNINFO.nCityCode &&
			pRn2DNode[nIdx].nTownID == TOWNINFO.nTownCode )
		{
			nuMemcpy( pItem->wsTownName,TOWNINFO.wsTownName,EACH_TOWN_NAME_LEN );
			break;
		}
	}
	//MessageBoxW(NULL,pItem->wsCityName,0,NULL);
	//MessageBoxW(NULL,pItem->wsTownName,0,NULL);
	nuFclose(CB);
	CB = NULL;
	return nuTRUE;
}
nuBOOL CFileRn2O::GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n)
{
	if(n==-1) //Clear Parser
	{
		m_nDYWordIdx = 0 ;
		if(1)//m_SearchRes==NULL)
		{
			nuLONG	nSize = {0}; 
			nuPBYTE pData = NULL;
			nSize = sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_POI;
			if(m_nNm_SearchResMemIdx==-1)
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNm_SearchResMemIdx);
			nuMemset(pData,0,nSize);

			//m_SearchRes = new DYWORD[NURO_SEARCH_BUFFER_MAX_POI];
		}
		return nuFALSE;
	}
	if(m_nDYWordIdx == NURO_SEARCH_BUFFER_MAX_POI)
	{
		return nuFALSE;
	}
	if( nIdx >= m_rn2Header.nRoadCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	// Get CityID and TownID
	PRN2_D_NODE pRn2DNode = (PRN2_D_NODE)GetRdDData(NULL);
	if( pRn2DNode == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pLength = (nuPBYTE)GetRdALength(NULL);
	if( pLength == NULL )
	{
		return nuFALSE;
	}
	//nuMemset( pItem,0,sizeof(SH_ITEM_FORUI));
	/*Get RoadName Info From C Addr*/
	nuDWORD i = 0;
	nuLONG nAddr = m_rn2Header.nRoadNameAddr;
	for( ; i < nIdx; ++i )
	{
		//nAddr += (pLength[i]<<1);//2 Bytes RoadName 
		nAddr= nAddr + pLength[i];
	}
	nuSIZE nNum = NURO_MIN((SH_NAME_FORUI_NUM-1),pLength[i]/2);
	nuWCHAR wsTemp[SH_NAME_FORUI_NUM]={0};
	if( nuFseek( m_pFile, nAddr,NURO_SEEK_SET) != 0 ||
		nuFread( wsTemp,sizeof(nuWCHAR),nNum,m_pFile) != nNum )
	{
		return nuFALSE;
	}
	nuWCHAR sTemp[SH_NAME_FORUI_NUM]={0};
	for(nuINT i=nWordStart,j=0;j<nWords;i++,j++)
	 sTemp[j]=wsTemp[i];
		//pDyNode->name[j]=wsTemp[i];


	/*Get Stroke Info In E Addr*/
	nuPBYTE pStroke = (nuPBYTE)GetRdStroke( NULL );
	
	PDYWORD m_SearchRes = (PDYWORD)m_pMm_SHApi->MM_GetDataMassAddr(m_nNm_SearchResMemIdx);

	nuBOOL bSameName=false;
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
	return nuTRUE;

}
nuDWORD CFileRn2O::GetCityCount()
{
	nuFILE* CB = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile,nuTEXT(".CB"));
	if(  !m_pFsApi->FS_FindFileWholePath(m_nMapIdx,tsFile,NURO_MAX_PATH) )
	{
		return 0;
	}
	CB = nuTfopen( tsFile,NURO_FS_RB );
	if( CB == NULL )
	{
		return 0;
	}
	//搜索City的信息时候
	SEARCH_CB_HEADER CBHeader = {0};
	if( nuFseek( CB,0,NURO_SEEK_SET ) != 0 ||
		nuFread( &CBHeader,sizeof(SEARCH_CB_HEADER),1,CB ) != 1 )
	{
		nuFclose(CB);
		CB=NULL;
		return 0;
	}
	nuFclose(CB);
	CB=NULL;
	return CBHeader.nCityCount;

}

nuBOOL CFileRn2O::GetRdCityTown(PSH_CITYTOWN_FORUI pCityTown)
{
	if( pCityTown == NULL )
	{
		return nuFALSE;
	}
	//Open .CB File 
	nuFILE* CB = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile,nuTEXT(".CB"));
	if(  !m_pFsApi->FS_FindFileWholePath(m_nMapIdx,tsFile,NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	CB = nuTfopen( tsFile,NURO_FS_RB );
	if( CB == NULL )
	{
		return nuFALSE;
	}
	//搜索City的信息时候
	SEARCH_CB_HEADER CBHeader = {0};
	if( nuFseek( CB,0,NURO_SEEK_SET ) != 0 ||
		nuFread( &CBHeader,sizeof(SEARCH_CB_HEADER),1,CB ) != 1 )
	{
		nuFclose(CB);
		CB=NULL;
		return nuFALSE;
	}
	nuDWORD nAddr = 0;
	if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_ALL || 
		pCityTown->nType == SH_CITYTOWN_TYPE_CITY_USED )
	{
		for( int i = 0; i < pCityTown->nTotalCount; i++ )
		{
			nAddr = sizeof(SEARCH_CB_HEADER)+ PDY_CITYTOWN_NAME_LEN * pCityTown->pIdList[i];
			if( nuFseek( CB, nAddr, NURO_SEEK_SET ) != 0 ||
				nuFread( pCityTown->ppName[i],PDY_CITYTOWN_NAME_LEN,1, CB ) != 1 )
			{
				nuFclose(CB);
				CB=NULL;
				return nuFALSE;
			}
		}
		
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_ALL ||
			 pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_USED )
			 //搜索Town信息的时候
	{
		nuDWORD nFileLen = nuFgetlen(CB); //获取文件长度
		nAddr = nFileLen - CBHeader.nTownCount*sizeof(nuDWORD);//获取按照CityTown排序的索引开头地址
		nuDWORD nTownInfo = 0;
		SEARCH_CB_TOWNINFO TOWNINFO = {0};
		nuDWORD nTownCount = 0; //记录Town的个数
		for( nuDWORD i = 0; i < CBHeader.nTownCount; i++ )
		{
			//读取每个按照CityTown排序的索引地址
			if( nuFseek(CB,nAddr+i*sizeof(nuDWORD),NURO_SEEK_CUR) !=0 ||
				nuFread( &nTownInfo,sizeof(nuDWORD),1,CB ) != 1 )
			{
				nuFclose(CB);
				CB=NULL;
				return nuFALSE;
			}
			//读取TownInfo结构
			if( nuFseek( CB,nTownInfo,NURO_SEEK_SET) != 0 ||
				nuFread( &TOWNINFO, sizeof(TOWNINFO),1,CB ) != 1 )
			{
				nuFclose(CB);
				CB=NULL;
				return nuFALSE;
			}
			//从读出来的TownInfo结构里面的Citycode和TownID和设定的 CityID以及TownID比对
			if( pCityTown->nCityID == TOWNINFO.nCityCode &&
				pCityTown->pIdList[nTownCount] == TOWNINFO.nTownCode )
			{
				nuMemcmp( pCityTown->ppName[nTownCount],TOWNINFO.wsTownName,PDY_CITYTOWN_NAME_LEN );
				nTownCount++;
				if( nTownCount > pCityTown->nTotalCount )
				{
					break;
				}
			}

		}


	}
	else
	{
		nuFclose(CB);
		CB=NULL;
		return nuFALSE;
	}
	nuFclose(CB);
	CB=NULL;
	return nuTRUE;
}

nuBOOL CFileRn2O::GetSPDRoadDetail(PSH_ROAD_DETAIL pOneRoad,nuDWORD nIdx)
{
	if( pOneRoad == NULL || nIdx >= m_rn2Header.nRoadCount || m_pFile == NULL )
	{
		return nuFALSE;
	}

	RN2_F_NODE rn2FData = {0};
	long nAddr = m_rn2Header.nCityTownAddr + sizeof(RN2_D_NODE)*m_rn2Header.nRoadCount + m_rn2Header.nRoadCount*sizeof(nuBYTE);
	nAddr += sizeof( RN2_F_NODE )*nIdx;
	if( nuFseek( m_pFile, nAddr,NURO_SEEK_SET ) != 0 ||
		nuFread( &rn2FData, sizeof(RN2_F_NODE),1,m_pFile ) != 1 ||
		rn2FData.nMinNum == (nuWORD)-1 ) // 没有门牌号码
	{
		return nuFALSE;
	}
	pOneRoad->nDrNumCount = rn2FData.nDrNumCount;
	//此路段只有一个门牌号码
	if( rn2FData.nDrNumCount == 0 )
	{
		pOneRoad->nMaxDrnum = (nuWORD)rn2FData.nMinNum;
		pOneRoad->nMinDrNum = (nuWORD)rn2FData.nMinNum;
		return nuTRUE;
	}
	//此路段有多个门牌号码，把最大的那个找出来
	RN2_H_NODE rn2HData = {0};
	nAddr = rn2FData.pOtherDrNumAddr+sizeof(RN2_H_NODE)*(rn2FData.nDrNumCount-1);
	if( nuFseek( m_pFile,nAddr,NURO_SEEK_SET) != 0 ||
		nuFread ( &rn2HData,sizeof(RN2_H_NODE),1,m_pFile ) != 1 )
	{
		return nuFALSE;
	}
	pOneRoad->nMaxDrnum = rn2FData.nMinNum + rn2HData.nDrNumDis;
	pOneRoad->nMinDrNum = rn2FData.nMinNum;
	return nuTRUE;
}

nuBOOL CFileRn2O::GetSPDDrNumCoor(nuDWORD nIdx,nuDWORD nDoorNum,nuroPOINT* point)
{
	if( point == NULL || m_pFile == NULL || nIdx >= m_rn2Header.nRoadCount )
	{
		return nuFALSE;
	}
	RN2_F_NODE rn2FData = {0};
	long nAddr = m_rn2Header.nCityTownAddr + sizeof(RN2_D_NODE)*m_rn2Header.nRoadCount + m_rn2Header.nRoadCount*sizeof(nuBYTE);
	nAddr += sizeof( RN2_F_NODE )*nIdx;
	if( nuFseek( m_pFile, nAddr,NURO_SEEK_SET ) != 0 ||
		nuFread( &rn2FData, sizeof(RN2_F_NODE),1,m_pFile ) != 1 )
	{
		return nuFALSE;
	}
	//当前门牌号码和最小的门牌号码一样 或者 当前道路门牌号码信息为0
	if( nDoorNum == rn2FData.nMinNum || rn2FData.nDrNumCount == 0 )
	{
		point->x = rn2FData.nMinNumX;
		point->y = rn2FData.nMinNumY;
		return nuTRUE;
	}
	//
	nuWORD nHMemIdx = 0;
	nuDWORD nSize = ( rn2FData.nDrNumCount )*sizeof(RN2_H_NODE);
	PRN2_H_NODE pRn2HData = ( PRN2_H_NODE )m_pMm_SHApi->MM_GetDataMemMass(nSize,&nHMemIdx);
	if( pRn2HData == NULL )
	{
		return nuFALSE;
	}
	nAddr = rn2FData.pOtherDrNumAddr;
	if( nuFseek( m_pFile,nAddr,NURO_SEEK_SET) != 0 ||
		nuFread ( pRn2HData,nSize,1,m_pFile ) != 1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass( &nHMemIdx );
		return nuFALSE;
	}
	if( nDoorNum > rn2FData.nMinNum && 
		nDoorNum < pRn2HData[0].nDrNumDis + rn2FData.nMinNum )
	{
		point->x = rn2FData.nMinNumX;
		point->y = rn2FData.nMinNumY;
		m_pMm_SHApi->MM_RelDataMemMass( &nHMemIdx );
		return nuTRUE; 
	}
	if(nDoorNum ==-1)
	{
		nDoorNum = pRn2HData[0].nDrNumDis + rn2FData.nMinNum;
	}
	for( nuINT i = 0; i < rn2FData.nDrNumCount; i++ )
	{
		if( nDoorNum == pRn2HData[i].nDrNumDis + rn2FData.nMinNum )
		{
			point->x = pRn2HData[i].nDrNumXDis + rn2FData.nMinNumX;
			point->y = pRn2HData[i].nDrNumYDis + rn2FData.nMinNumY;
			m_pMm_SHApi->MM_RelDataMemMass( &nHMemIdx );
			return nuTRUE;
		}
		if( nDoorNum > ( pRn2HData[i].nDrNumDis + rn2FData.nMinNum ) &&
			nDoorNum < ( pRn2HData[i+1].nDrNumDis + rn2FData.nMinNum ) )
		{
			point->x = pRn2HData[i].nDrNumXDis+rn2FData.nMinNumX;
			point->y = pRn2HData[i].nDrNumYDis+rn2FData.nMinNumY;
			m_pMm_SHApi->MM_RelDataMemMass( &nHMemIdx );
			return nuTRUE;
		}
	}
	m_pMm_SHApi->MM_RelDataMemMass( &nHMemIdx );
	return nuFALSE;
}
nuBOOL CFileRn2O::GetRdOneData_GETNAME(PSH_ITEM_FORUI pItem, nuDWORD nIdx)
{
	if( pItem == NULL || nIdx >= m_rn2Header.nRoadCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	// Get CityID and TownID
	PRN2_D_NODE pRn2DNode = (PRN2_D_NODE)GetRdDData(NULL);
	if( pRn2DNode == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pLength = (nuPBYTE)GetRdALength(NULL);
	if( pLength == NULL )
	{
		return nuFALSE;
	}
	nuMemset( pItem,0,sizeof(SH_ITEM_FORUI));
	/*Get RoadName Info From C Addr*/
	nuDWORD i = 0;
	nuLONG nAddr = m_rn2Header.nRoadNameAddr;
	for( ; i < nIdx; ++i )
	{
		//nAddr += (pLength[i]<<1);//2 Bytes RoadName 
		nAddr= nAddr + pLength[i];
	}
	nuSIZE nNum = NURO_MIN((SH_NAME_FORUI_NUM-1),pLength[i]/2);
	if( nuFseek( m_pFile, nAddr,NURO_SEEK_SET) != 0 ||
		nuFread( pItem->wsName,sizeof(nuWCHAR),nNum,m_pFile) != nNum )
	{
		return nuFALSE;
	}
	return nuTRUE;
}
