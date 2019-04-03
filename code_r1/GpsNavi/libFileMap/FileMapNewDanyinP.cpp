// FileMapNewDanyinP.cpp: implementation of the CFileMapNewDanyinP class.
//
//////////////////////////////////////////////////////////// //////////
#include "FileMapNewDanyinP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMapNewDanyinP::CFileMapNewDanyinP()
{
	m_nMapIdx = -1;
	m_pFile	= NULL;
	m_pFile2= NULL;
	m_pFileCB = NULL;
	m_nNewDanYinAMemIdx = -1;
	m_nNewDanYinBMemIdx = -1;
	m_nNewDanYinB2MemIdx = -1;
	m_nNewDanYinDEMemIdx = -1;
	m_nDYSearchMode = -1;

	m_nNewDanYinLenMemIdx = -1;
	m_nNewDanYinCityMemIdx = -1;
	m_nNewDanYinDanYinMemIdx = -1;
	m_nNewDanYinNameMemIdx = -1;
	m_nNewDanYinCityTownMemIdx = -1;
	m_SearchResIdx = -1;
	m_nBerforeCityLen = 0;

	wsALLCityName = wsALLTownName =NULL;
	m_nNewDanYinROADINFOIdx = -1;
	m_nRoadCount = 0;
	m_bFUZZY = nuFALSE;
}

CFileMapNewDanyinP::~CFileMapNewDanyinP()
{
	Free();
}

nuBOOL CFileMapNewDanyinP::Initialize()
{
	m_nMapIdx = -1;
	m_pFile	= NULL;
	m_nNewDanYinAMemIdx = -1;
	m_nNewDanYinB2MemIdx = -1;
	m_nNewDanYinDEMemIdx = -1;
	m_bNeedReadB2 = nuTRUE;
	m_bNeedReadDE = nuTRUE;
	m_SearchResIdx = -1; 
	m_nNewDanYinCityMemIdx = -1;
	m_nNewDanYinDanYinMemIdx = -1;
	m_nNewDanYinNameMemIdx = -1;
	m_nNewDanYinCityTownMemIdx = -1;
	m_nNewDanYinROADINFOIdx =-1;
	return nuTRUE;
}

nuVOID CFileMapNewDanyinP::Free()
{
	if( m_nNewDanYinLenMemIdx != (nuWORD)-1)
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinLenMemIdx);
	}
	if( m_nNewDanYinAMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinAMemIdx);
	}
	if( m_nNewDanYinBMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinBMemIdx);
	}
	if( m_nNewDanYinB2MemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinB2MemIdx);
	}
	if( m_nNewDanYinDEMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinDEMemIdx);
	}


	if( m_nNewDanYinCityMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinCityMemIdx);
	}
	if( m_nNewDanYinDanYinMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinDanYinMemIdx);
	}
	if( m_nNewDanYinNameMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinNameMemIdx);
	}
	if( m_nNewDanYinCityTownMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinCityTownMemIdx);
	}
	if(m_SearchResIdx !=(nuWORD)-1)
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_SearchResIdx);
	}
	if(wsALLCityName)
	{
		delete []wsALLCityName;
		wsALLCityName = NULL;
	}
	if(wsALLTownName)
	{
		delete []wsALLTownName;
		wsALLTownName = NULL;
	}
	if( m_nNewDanYinROADINFOIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinROADINFOIdx);
	}
	CloseFile();
}

nuBOOL CFileMapNewDanyinP::SetMapID(nuWORD nMapIdx)
{


	if( m_pFile == NULL || m_nMapIdx != nMapIdx )
	{
		Free();
		
		m_pMm_SHApi->MM_CollectDataMem(0);
		//
		m_nMapIdx = nMapIdx;
		if( !OpenFile() )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFread(&m_ndyHead, sizeof(NEWDANYINHEAD), 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}

	}
	return nuTRUE;
}
nuBOOL CFileMapNewDanyinP::LoadData()
{
		
		if(wsALLCityName == NULL)
		{
			NDYNAME data [128] = {0};
			wsALLCityName = new NDYNAME[m_ndyHead.nCityCount];

			if( nuFseek(m_pFile, m_ndyHead.nCityStart ,NURO_SEEK_SET) != 0 ||
			nuFread(wsALLCityName, m_ndyHead.nCityCount*40, 1, m_pFile) != 1 )
			{
				return nuFALSE;
			}
			nuMemcpy(data,wsALLCityName,m_ndyHead.nCityCount*40);
			int k=0;
		}
		if(wsALLTownName == NULL)
		{
			wsALLTownName = new NDYNAME[m_ndyHead.nTownCount];
			if( nuFseek(m_pFile, m_ndyHead.nTownStart ,NURO_SEEK_SET) != 0 ||
				nuFread(wsALLTownName,  m_ndyHead.nTownCount*40, 1, m_pFile) != 1 )
			{
				return nuFALSE;
			}
		}
		
		if(m_nNewDanYinROADINFOIdx == (nuWORD)-1)
		{
			nuWORD nSize = sizeof(NDYROADINFO)*NURO_SEARCH_BUFFER_MAX;
			PNDYROADINFO pData = (PNDYROADINFO)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinROADINFOIdx);
			m_nRoadCount = 0;
			if(pData == NULL)
			{
				return nuFALSE;
			}
		}
	

	


		nuLONG	nSize = {0}; 
		nuPBYTE pData = NULL;
		
		m_bCity = nuTRUE;
	
		if(m_bCity)
		{
			nSize = m_ndyHead.nCityCount * 8;
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinCityMemIdx);

			if( pData == NULL )
			{
				return nuFALSE;
			}
			if( nuFseek(m_pFile, sizeof(m_ndyHead), NURO_SEEK_SET) != 0 ||
				nuFread(pData, nSize, 1, m_pFile) != 1 )
			{
				return nuFALSE;
			}

			
			

			if(m_nCityIdx!= (nuWORD)-1)
			{
				
				m_nBerforeCityWORDLen = 0;
				m_nBerforeCityLen = 0; 
					
				
				
				
				NDY_CITYDATA	*ndyCityDataALL = new NDY_CITYDATA[m_ndyHead.nCityCount];
				nuMemset(ndyCityDataALL, 0,sizeof(NDY_CITYDATA)*m_ndyHead.nCityCount);
				if( nuFseek(m_pFile, sizeof(m_ndyHead) ,NURO_SEEK_SET) != 0 ||
					nuFread(ndyCityDataALL, sizeof(NDY_CITYDATA)*m_ndyHead.nCityCount, 1, m_pFile) != 1 )
				{
					return nuFALSE;
				}
			
				if(m_nMapIdx==7) // for Correct GD.NDY NG 
				{
					ColCityRealCount(ndyCityDataALL);
				}
				for(nuINT i=0;i<m_ndyHead.nCityCount;i++)
				{
					if(i>0)
					{
						m_nBerforeCityWORDLen+=ndyCityDataALL[i].CityRoadAddr-ndyCityDataALL[i-1].CityRoadAddr;
						m_nBerforeCityLen+=ndyCityDataALL[i-1].nCityDataCount;
					}
					if(i==m_nCityIdx)
					{
						break;
					}
					
				}
				if(m_ndyHead.nCityCount == 1)
				{
					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(m_ndyHead.nTotalCount, &m_nNewDanYinLenMemIdx);
					nSize = m_ndyHead.nTotalCount;
					if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
					m_ndyCityData = ndyCityDataALL[m_nCityIdx];
				}
				else
				{
					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(ndyCityDataALL[m_nCityIdx].nCityDataCount, &m_nNewDanYinLenMemIdx);
					nSize = ndyCityDataALL[m_nCityIdx].nCityDataCount;
					if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
					m_ndyCityData = ndyCityDataALL[m_nCityIdx];
				}


				if(ndyCityDataALL)
				{
					delete []ndyCityDataALL;
					ndyCityDataALL = NULL;
				}
			}
			
			if(m_nDYSearchMode == SEARCH_NDY_SET_DANNY) // LOAD DANNY
			{
				if(m_nCityIdx==-1)
				{
					nSize = m_ndyHead.nNameStartAddr - m_ndyHead.nDanYinNameStartAddr;
					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinDanYinMemIdx);
				
					if( pData == NULL )
					{
						return nuFALSE;
					}
					
					if( nuFseek(m_pFile, m_ndyHead.nDanYinNameStartAddr, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
				}
				else
				{
					if(m_ndyHead.nCityCount == 1)
					{
						nSize = m_ndyHead.nTotalCount*2;
					}
					else
					{
						nSize = m_ndyCityData.nCityDataCount*2;
					}
					nuBYTE *nLen=new nuBYTE[nSize];
					if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
						nuFread(nLen, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
					nuINT nLenSize=0;
					for(nuINT j=0;j<nSize;j++)
					{
						nLenSize+=nLen[j];
					}
					if(nLen)
					{
						delete []nLen ;
						nLen = NULL;
					}

					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nLenSize*2, &m_nNewDanYinDanYinMemIdx);
			
					if( pData == NULL )
					{
						return nuFALSE;
					}
					if( nuFseek(m_pFile, m_ndyHead.nDanYinNameStartAddr+m_nBerforeCityWORDLen, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nLenSize*2, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
				}
			
			}
			else //add by Prosper , if need load name buffer
			{

				if(m_nCityIdx==-1)
				{
					
					nSize = m_ndyHead.nNameStartAddr - m_ndyHead.nDanYinNameStartAddr;
					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinNameMemIdx);
					if( pData == NULL )
					{
						return nuFALSE;
					}
					if( nuFseek(m_pFile, m_ndyCityData.CityRoadAddr, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
				}
				else
				{
					nSize = m_ndyCityData.nCityDataCount;
					//nuBYTE *nLen=new nuBYTE[nSize];
					nuPBYTE nLen = (nuPBYTE)m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinLenMemIdx);

					/*if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
						nuFread(nLen, nSize, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}*/
					nuINT nLenSize=0;
					for(nuINT j=0;j<nSize;j++)
					{
						nLenSize+=nLen[j];
					}

					pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nLenSize*2, &m_nNewDanYinNameMemIdx);
			
					
					if( pData == NULL )
					{
						return nuFALSE;
					}
					if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr+m_nBerforeCityWORDLen, NURO_SEEK_SET) != 0 ||
						nuFread(pData, nLenSize*2, 1, m_pFile) != 1 )
					{
						return nuFALSE;
					}
					nuBYTE * wT= pData;
					int k = 0;
				}
			}
		}
		
		else
		{

			if(m_nDYSearchMode == SEARCH_NDY_SET_DANNY) // LOAD DANNY
			{
				nSize = m_ndyHead.nNameStartAddr - m_ndyHead.nDanYinNameStartAddr;
				pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinDanYinMemIdx);
			
				if( pData == NULL )
				{
					return nuFALSE;
				}
				if( nuFseek(m_pFile, m_ndyHead.nDanYinNameStartAddr, NURO_SEEK_SET) != 0 ||
					nuFread(pData, nSize, 1, m_pFile) != 1 )
				{
					return nuFALSE;
				}
				
			}
			else //add by Prosper , if need load name buffer
			{
				nSize = m_ndyHead.nNameStartAddr - m_ndyHead.nDanYinNameStartAddr;
				pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinNameMemIdx);
				if( pData == NULL )
				{
					return nuFALSE;
				}
				if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr, NURO_SEEK_SET) != 0 ||
					nuFread(pData, nSize, 1, m_pFile) != 1 )
				{
					return nuFALSE;
				}
			

		}

		//City & Town Info 
		nSize = m_ndyHead.nCityCount * DY_CITY_NAME_LEN + m_ndyHead.nTownCount * DY_TOWN_NAME_LEN;
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinCityTownMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, m_ndyHead.nCityStart, NURO_SEEK_SET) != 0 ||
			nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
		m_bNeedReadDE = nuTRUE; //0718
	}
	return nuTRUE;
}

nuBOOL CFileMapNewDanyinP::OpenFile()
{
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".NDY"));
	if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		__android_log_print(ANDROID_LOG_INFO, "NDY", "path %d fail",m_nMapIdx);
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "NDY", "path %d ok",m_nMapIdx);
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	//m_pFile = nuTfopen("/mnt/sdcard/extsd/nurownds/TW.NDY", NURO_FS_RB);
	nuTCHAR tsFile2[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile2, nuTEXT(".CB"));
	if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile2, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFileCB = nuTfopen(tsFile2, NURO_FS_RB);
	if( nuFseek( m_pFileCB,0,NURO_SEEK_SET ) != 0 ||
		nuFread( &CBHeader,sizeof(SEARCH_CB_HEADER),1,m_pFileCB ) != 1 )
	{
		nuFclose(m_pFileCB);
		m_pFileCB=NULL;
		return nuFALSE;
	}
	

	if( m_pFile && m_pFileCB)
	{
		GetTownIDAll();
	
		return nuTRUE;
	}
	
	return nuFALSE;
}

nuVOID CFileMapNewDanyinP::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
	if( m_pFileCB != NULL )
	{
		nuFclose(m_pFileCB);
		m_pFileCB = NULL;
	}
}

nuPVOID CFileMapNewDanyinP::GetDanyinA(nuDWORD *pCount)
{
	m_bFUZZY = nuFALSE;
	m_bFindPOI = nuFALSE;
	if( m_nNewDanYinBMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinBMemIdx);
	}
	if(m_SearchResIdx== (nuWORD)-1)
	{
		PDYWORD pData = NULL;
		pData=(PDYWORD)m_pMm_SHApi->MM_GetDataMemMass(sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI, &m_SearchResIdx);
		m_nDYWordIdx=0;
	}
	
	if( pCount )
	{
		if(m_nCityIdx==-1)
			*pCount = m_ndyHead.nTotalCount;
		else
		{
			if(m_ndyHead.nCityCount == 1)
			*pCount = m_ndyHead.nTotalCount;
			else
			*pCount = m_ndyCityData.nCityDataCount;
		}
	}

	return m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinDanYinMemIdx);
}
nuPVOID CFileMapNewDanyinP::GetDanyinB(nuDWORD *pCount)
{
	m_bFUZZY = nuTRUE;
	m_bFindPOI = nuFALSE;
	if( m_nNewDanYinAMemIdx != (nuWORD)-1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nNewDanYinAMemIdx);
	}
	if(m_SearchResIdx== (nuWORD)-1)
	{
		PDYWORD pData = NULL;
		pData=(PDYWORD)m_pMm_SHApi->MM_GetDataMemMass(sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI, &m_SearchResIdx);
		m_nDYWordIdx=0;
	}
	if(m_nCityIdx!=-1)
	{
		*pCount = m_ndyHead.nTotalCount;
	}

	if( pCount )
	{
		*pCount = m_ndyHead.nTotalCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinNameMemIdx);
}
nuPVOID CFileMapNewDanyinP::GetDanyinB2(nuDWORD *pCount)
{
	nuPVOID pData = m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinB2MemIdx);
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
		if(m_bCity==-1)
		{
			nuDWORD nSize = m_ndyHead.nNameStartAddr - m_ndyHead.nDanYinNameStartAddr;
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nNewDanYinNameMemIdx);
			// Added by Damon 2012 05/21
			m_bNeedReadB2 = nuFALSE;
		}
		else
		{
			/*if(m_bCity < m_bCity)
			{
				for(nuINT j=0;j<;j++)
				if( nuFseek(m_pFile, sizeof(m_ndyHead) + m_bCity*12 ,NURO_SEEK_SET) != 0 ||
					nuFread(&m_ndyCityData, sizeof(m_ndyCityData), 1, m_pFile) != 1 )
				{
					return nuFALSE;
				}
				m_nBerforeCityLen+=m_ndyCityData.nCityDataCount;
			}*/
			

			/*
			if( nuFseek(m_pFile, sizeof(m_ndyHead) + m_bCity*12 ,NURO_SEEK_SET) != 0 ||
				nuFread(&m_ndyCityData, sizeof(m_ndyCityData), 1, m_pFile) != 1 )
			{
				return nuFALSE;
			}*/
			nuDWORD nSize = m_ndyCityData.nCityDataCount;
			pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize*2, &m_nNewDanYinNameMemIdx);
			// Added by Damon 2012 05/21
			m_bNeedReadB2 = nuFALSE;
		}
		//-------------------------
	}
	if( pCount )
	{
		*pCount = m_ndyHead.nTotalCount;
	}
	return pData;
}
nuBOOL CFileMapNewDanyinP::GetOneDYData_WORD(nuINT nWords,nuINT nWordStart,PDYWORD pDyNode, nuDWORD nIdx ,nuINT &n,nuVOID *pPoiKeyName,nuINT nBeforeLen2)
{
	

	PDYWORD pData = (PDYWORD)m_pMm_SHApi->MM_GetDataMassAddr(m_SearchResIdx);

	if(n==-1) //Clear Parser
	{
		nuMemset(pData,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		m_nDYWordIdx = 0 ;
	//	nuMemset(m_SearchRes,0,sizeof(DYWORD)*NURO_SEARCH_BUFFER_MAX_DYPOI);
		nuMemset(m_KeyName,0,sizeof(m_KeyName));
		nuMemcpy(m_KeyName,pPoiKeyName,NewDY_B_NAME_LEN);
		return nuFALSE;
	}
	if( pDyNode == NULL || nIdx >= m_ndyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	

	nuWCHAR sComparedName[NewDY_B_NAME_LEN/2]={0};
	nuINT nNameLen = 0;
	if(m_nCityIdx==-1)
		nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr + nIdx,NURO_SEEK_SET) ;
	else
		nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr +m_nBerforeCityLen+ nIdx,NURO_SEEK_SET) ;
	nuFread(&nNameLen, 1, 1, m_pFile);

	if(nNameLen>(NewDY_B_NAME_LEN/2))
	{
		nNameLen = NewDY_B_NAME_LEN/2;		
	}
	int nShiftLen=0;
	//nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr + ,NURO_SEEK_SET) ;
	if(m_nCityIdx==-1)
	{
		if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + nBeforeLen2*2 ,NURO_SEEK_SET) != 0 ||
			nuFread(&sComparedName, nNameLen*2, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	else
	{
		nuDWORD nDanlen = m_ndyCityData.CityRoadAddr - m_ndyHead.nDanYinNameStartAddr;
		if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + nDanlen + nBeforeLen2*2 ,NURO_SEEK_SET) != 0 ||
			nuFread(&sComparedName, nNameLen*2, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}


	nuINT ll=0;
	for(nuINT l=0;l<nNameLen;l++)
	{
		if(sComparedName[l]>47&& sComparedName[l]<58 
					|| sComparedName[l]>64 && sComparedName[l]<91
					|| sComparedName[l]>96 && sComparedName[l]<123
					|| sComparedName[l]>=8544 && sComparedName[l]<=8554
					|| sComparedName[l]>12398 )
		{
			sComparedName[ll]=sComparedName[l];
			ll++;
		}
	}
	if(ll<NewDY_B_NAME_LEN/2)
	sComparedName[ll]=0;


	nuBOOL bSameName=nuFALSE;
	nuBOOL bReName=nuFALSE;
	nuWCHAR sTemp[NewDY_B_NAME_LEN/2]={0};
	nuWCHAR sComparedName2[NewDY_B_NAME_LEN/2+1]={0};

	
	nuBOOL bNoAddTag = nuFALSE;
	for(nuINT k=0,l=0;k<nWords;l++)
	{
		sTemp[k]=sComparedName[nWordStart+l];
		k++;	
	}
	
	
	nuINT n_CountIdx=0;
	for ( nuUINT l = 0; l < m_nDYWordIdx; l++ )
	{
		nuBOOL bSame=nuTRUE;
		for(nuINT i=0;i<nWords;i++)
		{
			if(pData[l].name[i]!=sTemp[i])
			{
				bSame=nuFALSE;
				break;
			}
		}
		if (bSame)
		{
			++pData[l].nFindCount;
			
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
		/*nuINT nLen =nuWcslen(sTemp);
		if(nLen < nuWcslen(m_KeyName))
		{
			n = -1;
			return nuFALSE;
		}*/
		
		nuWcsncpy(pData[m_nDYWordIdx].name,sTemp,nNameLen);
		/*if(pData[m_nDYWordIdx].name[0] <48)
			int l=0;*/
		if(m_nDYWordIdx==75)
			int k=0;
		m_nDYWordIdx++;
	}

	if(bSameName)
	{
		n = n_CountIdx;
		return nuFALSE;
	}
	else
	{
		//pDyNode->nIdx = nIdx;
		nuINT nLen =nuWcslen(sTemp);
		if(nLen>DY_NAME_FORUI_NUM)
			nLen=DY_NAME_FORUI_NUM; 
		nuWcsncpy(pDyNode->name,sTemp,nLen);
		return nuTRUE;
	}
}
nuBOOL CFileMapNewDanyinP::GetOneDYData(PDYNODEFORUI pDyNode, nuDWORD nIdx)
{
	if( pDyNode == NULL || nIdx >= m_ndyHead.nTotalCount || m_pFile == NULL )
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
	if( nuFseek(m_pFile, sizeof(DANYINHEAD) + sizeof(DANYIN_A)*m_ndyHead.nTotalCount + sizeof(DYNODE_B_1)*nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(pDyNode->wsName, sizeof(DYNODE_B_1), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuBOOL bSameName=nuFALSE;
	PDYWORD pData = (PDYWORD)m_pMm_SHApi->MM_GetDataMassAddr(m_SearchResIdx);
	for ( nuUINT l = 0; l < m_nDYWordIdx; l++ )
	{
		if ( 0 == nuWcsncmp(pData[l].name, pDyNode->wsName, nuWcslen(pDyNode->wsName)))
		{
			++pData[l].nFindCount;
			nuMemcpy(pData[l].name,pDyNode->wsName,2*DY_NAME_FORUI_NUM);
			bSameName = nuTRUE;						    
			break;
		}
	}
	if(!bSameName)
	{
		pData[m_nDYWordIdx].nFindCount = 1;
		nuMemcpy(pData[m_nDYWordIdx].name,pDyNode->wsName,2*DY_NAME_FORUI_NUM);
		
		m_nDYWordIdx++;
	}
	pDyNode->nStroke	= pDyNode2[nIdx].nStroke;
	pDyNode->nType		= pDyNode2[nIdx].nType;
	pDyNode->nCityID	= pDyNode2[nIdx].nCityID;
	pDyNode->nTownID	= pDyNode2[nIdx].nTownID;
	nuPBYTE pStr = (pDEData + pDyNode2[nIdx].nCityID * DY_CITY_NAME_LEN);
	nuMemcpy(pDyNode->wsCityName, pStr, DY_CITY_NAME_LEN);
	pStr = pDEData + pDyNode2[nIdx].nTownID * DY_TOWN_NAME_LEN + m_ndyHead.nCityCount * DY_CITY_NAME_LEN;
	nuMemcpy(pDyNode->wsTownName, pStr, DY_TOWN_NAME_LEN);
	return nuTRUE;
}

nuBOOL CFileMapNewDanyinP::GetOneDYData_PAGE(PDYNODEFORUI pDyNode, nuDWORD nIdx ,nuVOID* pPoiKeyName, nuINT nStartPos,nuINT nBeforeLen2)
{
	if( pDyNode == NULL || nIdx >= m_ndyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	/*PDYNODE_B_2 pDyNode2 = (PDYNODE_B_2)GetDanyinB2(NULL);
	if( pDyNode2 == NULL )
	{
		return nuFALSE;
	}
	nuPBYTE pDEData = (nuPBYTE)GetDanyinDE(NULL, NULL);
	if( pDEData == NULL )
	{
		return nuFALSE;
	}*/
//		nIdx	12983	unsigned long

	
	nuINT nNameLen = 0;
	if(m_nCityIdx==-1)
		nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr + nIdx,NURO_SEEK_SET) ;
	else
		nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr +m_nBerforeCityLen+ nIdx,NURO_SEEK_SET) ;
	nuFread(&nNameLen, 1, 1, m_pFile);

	if(nNameLen>(NewDY_B_NAME_LEN/2))
	{
		nNameLen = NewDY_B_NAME_LEN/2;		
	}
	int nShiftLen=0;
	//nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr + ,NURO_SEEK_SET) ;

	nuWCHAR wsTemp[128]={0};//NewDY_B_NAME_LEN/2]={0};
	
	if(m_nCityIdx==-1)
	{
		if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + nBeforeLen2*2 ,NURO_SEEK_SET) != 0 ||
			nuFread(&wsTemp, nNameLen*2, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	else
	{
		nuDWORD nDanlen = m_ndyCityData.CityRoadAddr - m_ndyHead.nDanYinNameStartAddr;
		if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + nDanlen + nBeforeLen2*2 ,NURO_SEEK_SET) != 0 ||
			nuFread(&wsTemp, nNameLen*2, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}

	nuWCHAR wsKeyWord[NewDY_B_NAME_LEN/2]={0};
	nuMemcpy(wsKeyWord,pPoiKeyName,sizeof(wsKeyWord));
	
#ifdef VALUE_EMGRT
	nuINT k=0;
	for(nuINT l=0;l<nNameLen;l++)
	{
		if(k==NewDY_B_NAME_LEN/2)
			break;

		if(wsTemp[l]>47&& wsTemp[l]<58 
			|| wsTemp[l]>64 && wsTemp[l]<91
			|| wsTemp[l]>96 && wsTemp[l]<123
			|| wsTemp[l]>8544 && wsTemp[l]<8554
			|| wsTemp[l]>12398 )
		{
			wsTemp[k]=wsTemp[l];
			k++;
		}
	}
	wsTemp[k]=0;
	nuINT lll= nuWcslen(wsTemp);
#endif

	nuBOOL bSame=nuTRUE;
	nuBOOL bFindHead=nuFALSE;
	nuINT nWords=nuWcslen(wsKeyWord);
	nuINT nWords2=nuWcslen(wsTemp);
	nuINT nFind=1;

	nuINT nShift=0;
	nuBOOL bCheckTag2 =nuFALSE;
	if(wsTemp[0]== (48+7) && wsTemp[1]==45)//(sComparedName2[nLen2] == 75 ||sComparedName2[nLen2] == 107))
	{
		//if(sComparedName2[nLen2-1] > 47 && sComparedName2[nLen2-1] < 58)
		bCheckTag2= nuTRUE;
	}
	if(wsTemp[nStartPos]== (45) && wsTemp[nStartPos+1]==45)//(sComparedName2[nLen2] == 75 ||sComparedName2[nLen2] == 107))
	{
	//	bCheckTag = nuFALSE;
		bCheckTag2= nuTRUE;
	}
	for(nuINT k=0;k<=nStartPos+nShift;k++)
	{
		if(!bCheckTag2&&wsTemp[k] == 45 && wsTemp[k+1] >= 1000 ) 
		{
			nShift++;
			//if(sComparedName[k] == 45)
			//break;
		}
		if(wsTemp[k] == 32)	
			nShift++;
	}

	nStartPos+=nShift;
	if(!m_bFUZZY)
	{
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
				if((!bCheckTag2 && wsTemp[nStartPos+j] == 45) || wsTemp[nStartPos+j] == 32)
				{
					continue;
				}
				if(wsTemp[nStartPos+j]>47&& wsTemp[nStartPos+j]<58 
					|| wsTemp[nStartPos+j]>64 && wsTemp[nStartPos+j]<91
					|| wsTemp[nStartPos+j]>96 && wsTemp[nStartPos+j]<123
					|| wsTemp[nStartPos+j]>8544 && wsTemp[nStartPos+j]<8554
					|| wsTemp[nStartPos+j]>12398 )
				{
					
				}
				else
				{
					continue;
				}
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
			/*for(int j=i+1,k=1;k<nWords;j++,k++)
			{
				if(wsTemp[nStartPos+j] == 45)
				{
					k--;
					continue;
				}
				if(wsTemp[nStartPos+j] != wsKeyWord[k])
				{
					bSame=nuFALSE;

					break;
}
			}*/
			}
		}
	}
	else
	{
		k=0;
		int nLen =  nuWcslen(wsTemp);
		for(int j=0;j<nLen;j++)
		{
			bFindHead=nuTRUE;
			if(wsTemp[j] == wsKeyWord[k] && nFind!=nWords)
			{
				nFind++;
				k++;
			}
		}
	}
	if(nFind!=nWords)
		bSame=nuFALSE;

	if(!bFindHead)
		bSame=nuFALSE;

	if (bSame)
	{
		nuINT nTempLen=nuWcslen(wsTemp);
		if(nTempLen>DY_NAME_FORUI_NUM)
			nTempLen=DY_NAME_FORUI_NUM;
		nuMemset(pDyNode->wsName,0,sizeof(pDyNode->wsName));
		nuMemcpy(pDyNode->wsName,wsTemp,nTempLen*2);
	}
	else
		return nuFALSE;
	
	DYNODE_B_2 DyNode2 ={0};
	if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + (m_nBerforeCityLen+nIdx)*sizeof(DYNODE_B_2) ,NURO_SEEK_SET) != 0 ||
		nuFread(&DyNode2, sizeof(DYNODE_B_2), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}

	
	pDyNode->nStroke	= DyNode2.nStroke;
	pDyNode->nType		= DyNode2.nType;
	pDyNode->nCityID	= DyNode2.nCityID;
	pDyNode->nTownID	= DyNode2.nTownID;

	//DYNODE_B_2 DyNode2 ={0};
	if( nuFseek(m_pFile, m_ndyHead.nCityStart + (DyNode2.nCityID)*40 ,NURO_SEEK_SET) != 0 ||
		nuFread(&pDyNode->wsCityName, sizeof(pDyNode->wsCityName), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}

	if( nuFseek(m_pFile, m_ndyHead.nTownStart + (DyNode2.nTownID)*40 ,NURO_SEEK_SET) != 0 ||
		nuFread(&pDyNode->wsTownName, sizeof(pDyNode->wsTownName), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	
	return nuTRUE;
}


nuPVOID CFileMapNewDanyinP::GetDanyinDE(nuWORD *pDCount, nuWORD *pECount)
{
	nuPVOID pData = m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinDEMemIdx);
	if( pData == NULL )
	{
		return NULL;
	}
	if( m_bNeedReadDE )
	{
		nuDWORD nSize = sizeof(DANYINHEAD) + 
			(sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) + sizeof(DYNODE_B_3) + sizeof(DYNODE_C)) * m_ndyHead.nTotalCount;
		if( m_pFile == NULL || nuFseek(m_pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			return NULL;
		}
		nSize = m_ndyHead.nCityCount * DY_CITY_NAME_LEN + m_ndyHead.nTownCount * DY_TOWN_NAME_LEN;
		if( nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			return NULL;
		}
	}
	if( pDCount != NULL )
	{
		*pDCount = (nuWORD)m_ndyHead.nCityCount;
	}
	if( pECount != NULL )
	{
		*pECount = (nuWORD)m_ndyHead.nTownCount;
	}
	return pData;
}

nuBOOL CFileMapNewDanyinP::GetOneDYDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx)
{
	if( pDyDetailForUI == NULL || m_pFile == NULL || (m_nBerforeCityLen+nIdx) >= m_ndyHead.nTotalCount )
	{
		return nuFALSE;
	}
	/*nuDWORD nSize = sizeof(DANYINHEAD) + 
		( sizeof(DANYIN_A) + sizeof(DYNODE_B_1) + sizeof(DYNODE_B_2) + sizeof(DYNODE_B_3) ) * m_ndyHead.nTotalCount + 
		sizeof(DYNODE_C) * nIdx;*/
	DYNODE_C dyNodeC = {0};
	if( nuFseek(m_pFile, m_ndyHead.nCoordiateStart+(m_nBerforeCityLen+nIdx)*sizeof(DYNODE_C), NURO_SEEK_SET) != 0 ||
		nuFread(&dyNodeC, sizeof(DYNODE_C), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	pDyDetailForUI->point = dyNodeC.point;

	DYNODE_B_2 DyNode2 ={0};
	if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + (m_nBerforeCityLen+nIdx)*sizeof(DYNODE_B_2) ,NURO_SEEK_SET) != 0 ||
		nuFread(&DyNode2, sizeof(DYNODE_B_2), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	pDyDetailForUI->nCityID = DyNode2.nCityID;
	pDyDetailForUI->nTownID = DyNode2.nTownID - n_CityTownID[DyNode2.nCityID];
	pDyDetailForUI->nType   = DyNode2.nType;

	nuWcscpy(pDyDetailForUI->wsCityName,wsALLCityName[DyNode2.nCityID].wsALLName);
	nuWcscpy(pDyDetailForUI->wsTownName,wsALLTownName[DyNode2.nTownID].wsALLName);

	nuBYTE nOtherLen = 0;
	DYNODE_B_3 dyNodeB3 ={0};
	

	
	if(pDyDetailForUI->nType)
	{
		nuBYTE nOtherLen = 0;
		DYNODE_B_3 dyNodeB3 ={0};
		
		nuBOOL bfind = true;
		if( nuFseek(m_pFile, m_ndyHead.nInfoAddrStart+(m_nBerforeCityLen+nIdx)*4, NURO_SEEK_SET) != 0 ||
			nuFread(&dyNodeB3, sizeof(DYNODE_B_3), 1, m_pFile) != 1 )
		{
			bfind = false;
		}
		
		if( nuFseek(m_pFile, dyNodeB3.nOtherAddr, NURO_SEEK_SET) != 0 )
		{
			bfind = false;
		}
		if(bfind)
		{
			if(	nuFread(&nOtherLen, 1, 1, m_pFile) != 1 ||
				nuFread(pDyDetailForUI->wsOther, nOtherLen, 1, m_pFile) != 1)
			{
				//return nuFALSE;
			}
			pDyDetailForUI->wsOther[nOtherLen/sizeof(nuWCHAR)] = 0;

			for(nuINT n=0;n<nOtherLen;n++)
			{
				if(pDyDetailForUI->wsOther[n]=='&')
				{
					pDyDetailForUI->wsOther[n] = 0;
					break;
				}
			}
		}
	}
	return nuTRUE;
}

nuUINT CFileMapNewDanyinP::SetSearchMode(nuINT nType) 
{
	m_nDYSearchMode = nType;
	return nuTRUE;
}
nuBOOL CFileMapNewDanyinP::ReadData(nuUINT addr, nuVOID *p, nuUINT len)
{
	nuFseek(m_pFile2 ,addr, NURO_SEEK_SET);
	
	if(nuFread(p, len, 1, m_pFile2)!=1)
	{
		return false;
	}

	return true;
	

}

nuBOOL CFileMapNewDanyinP::OpenNewFile(nuWORD nMapIdx)
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

nuVOID CFileMapNewDanyinP::CloseNewFile()
{
	if( m_pFile2 != NULL )
	{
		nuFclose(m_pFile2);
		m_pFile2 = NULL;
	}
}

nuBOOL CFileMapNewDanyinP::GetOneDYData_GETNAME(PDYNODEFORUI pDyNode, nuDWORD nIdx ,nuDWORD nStartLen)
{
	if( pDyNode == NULL || nIdx >= m_ndyHead.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	nuINT nLen = 0;
	if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(&nLen, sizeof(nuBYTE), 1, m_pFile) != 1 )
	{
		return 0;
	}
	if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + (nStartLen)*2 ,NURO_SEEK_SET) != 0 ||
		nuFread(&pDyNode->wsName, nLen*2, 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	
	
	return nuTRUE;
}

nuINT CFileMapNewDanyinP::GetDanyinLen(nuINT nIdx,nuINT nStartLen,nuBOOL bRes)
{
	nuINT nLen = 0;
	if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+nIdx, NURO_SEEK_SET) != 0 ||
		nuFread(&nLen, sizeof(nuBYTE), 1, m_pFile) != 1 )
	{
		return 0;
	}
/*	nuWCHAR sComparedName[48] = {0};
	if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr + (nStartLen)*2 ,NURO_SEEK_SET) != 0 ||
		nuFread(&sComparedName, nLen*2, 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
*/	return nLen;
}
nuINT CFileMapNewDanyinP::GetDanyinLen2(nuVOID *data)
{
	nuINT nLen = 0;
	if(m_nCityIdx==-1)
	{
		if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr, NURO_SEEK_SET) != 0 ||
			nuFread(data,m_ndyHead.nTotalCount , 1, m_pFile) != 1 )
		{
			return 0;
		}
	}
	else
	{
		if(m_ndyHead.nCityCount == 1)
		{
			if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
			nuFread(data,m_ndyHead.nTotalCount , 1, m_pFile) != 1 )
			{
				return 0;
			}

		}
		else
		{
			if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr+m_nBerforeCityLen, NURO_SEEK_SET) != 0 ||
			nuFread(data,m_ndyCityData.nCityDataCount , 1, m_pFile) != 1 )
			{
				return 0;
			}
		}
		
		
	}
	return true;
}
nuBOOL CFileMapNewDanyinP::SetID(nuWORD nCityIdx,nuWORD nTownIdx)
{
	m_nCityIdx = nCityIdx;
	m_nTownIdx = nTownIdx;
	return nuTRUE;
}
nuINT CFileMapNewDanyinP::GetIDInfo(nuVOID *data)
{
	if(m_nCityIdx == -1)
	{
		if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart, NURO_SEEK_SET) != 0 ||
			nuFread(data,m_ndyHead.nTotalCount*4 , 1, m_pFile) != 1 )
		{
			return 0;
		}
	}
	else
	{
		if(m_ndyHead.nCityCount ==1)
		{
			if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + m_nBerforeCityLen*4, NURO_SEEK_SET) != 0 ||
				nuFread(data,m_ndyHead.nTotalCount*4 , 1, m_pFile) != 1 )
			{
				return 0;
			}
		}
		else
		{
			if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + m_nBerforeCityLen*4, NURO_SEEK_SET) != 0 ||
				nuFread(data,m_ndyCityData.nCityDataCount*4 , 1, m_pFile) != 1 )
			{
				return 0;
			}
		

		
		}
	}
	return true;
}
nuBOOL CFileMapNewDanyinP::CheckID(nuDWORD nIdx,nuINT nCityIdx,nuINT nTownIdx)
{
	if(nCityIdx<0 && nTownIdx<0)
	{
		return nuTRUE;
	}
	PDY_D_NODE city_town_info;
	nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + nIdx*4,NURO_SEEK_SET) ;
	nuFread(&city_town_info,sizeof(city_town_info) , 1, m_pFile);

	if(nCityIdx > -1)
	{
		if(city_town_info.nCityID!= nCityIdx)
		{
			return nuFALSE;
		}
		else
		{
			return nuTRUE;
		}
	}

	if(nTownIdx > -1)
	{
		if(city_town_info.nTownID!= (nuINT)nTownIdx)
		{
			return nuFALSE;
		}
		else
		{
			return nuTRUE;
		}
	}

}
nuINT CFileMapNewDanyinP::GetTownNameAll(nuINT nCityID,nuVOID *pBuffer, nuUINT nMaxLen)
{
	/*PDY_D_NODE city_town_info;
	nuFseek(m_pFile, m_ndyHead.nTownStart ,NURO_SEEK_SET) ;
	nuWCHAR wsName[PROTWONMAXCOUNT][EACH_CITY_NAME_LEN/2]={0};
	for(nuINT i=0;i< m_ndyHead.nTownCount;i++)
	{
		nuFread(wsName[i],sizeof(DYNODE_B_1) , 1, m_pFile);
		int k=0;
	}
	nuMemcpy(pBuffer,wsName,nMaxLen);

	return m_ndyHead.nTownCount;*/

	if( m_pFileCB == NULL)
	{
		OpenFile();
	}
	if( m_pFileCB == NULL)
	{
		return nuFALSE;
	}

	nuBYTE* p = (nuBYTE*)pBuffer;
	if( nuFseek( m_pFileCB,sizeof(CBHeader)+CBHeader.nCityCount*EACH_CITY_NAME_LEN,NURO_SEEK_SET) != 0 )
	{
		nuFclose(m_pFileCB);
		m_pFileCB=NULL;
		return nuFALSE;
	}

	SEARCH_CB_TOWNINFO TOWNINFO = {0};
	nuINT j=0;
	for(nuINT i = 0; i < CBHeader.nTownCount; i++ )
	{
		if( nuFread(&TOWNINFO, sizeof(TOWNINFO), 1, m_pFileCB) != 1 )
		{
			nuFclose(m_pFileCB);
			m_pFileCB=NULL;
			return nuFALSE;
		}

		if( nuFseek( m_pFileCB,TOWNINFO.nTownBlkInfoCount*8,NURO_SEEK_CUR) != 0 )
		{
			nuFclose(m_pFileCB);
			m_pFileCB=NULL;
			return nuFALSE;
		}
		//植\E9??\A4\E6??\94TownInfo賦凳?\B5??\94Citycode?\BFTownID?\BF??\85\E8? CityID?\95?\AFTownID?\80??
		if(TOWNINFO.nCityCode==nCityID) /*DyNode2.nCityID == TOWNINFO.nCityCode &&
			DyNode2.nTownID == TOWNINFO.nTownCode )*/
		{
			nuMemcpy(p,TOWNINFO.wsTownName,EACH_TOWN_NAME_LEN );
			p += EACH_TOWN_NAME_LEN;
			j++;
		}
	}
	
	return j;
}
nuINT CFileMapNewDanyinP::GetTownIDAll()
{
	if( m_pFileCB == NULL)
	{
		OpenFile();
	}
	if( m_pFileCB == NULL)
	{
		return nuFALSE;
	}

	if( nuFseek( m_pFileCB,sizeof(CBHeader)+CBHeader.nCityCount*EACH_CITY_NAME_LEN,NURO_SEEK_SET) != 0 )
	{
		nuFclose(m_pFileCB);
		m_pFileCB=NULL;
		return nuFALSE;
	}

	SEARCH_CB_TOWNINFO TOWNINFO = {0};
	nuMemset(n_CityTownID,0,sizeof(n_CityTownID));
	//nuINT *n_CityTown= new nuINT[CBHeader.nCityCount];

	nuINT j=0;
	for(nuINT i = 0; i < CBHeader.nTownCount; i++ )
	{
		if( nuFread(&TOWNINFO, sizeof(TOWNINFO), 1, m_pFileCB) != 1 )
		{
			nuFclose(m_pFileCB);
			m_pFileCB=NULL;
			return nuFALSE;
		}

		if( nuFseek( m_pFileCB,TOWNINFO.nTownBlkInfoCount*8,NURO_SEEK_CUR) != 0 )
		{
			nuFclose(m_pFileCB);
			m_pFileCB=NULL;
			return nuFALSE;
		}
		n_CityTownID[TOWNINFO.nCityCode+1]=i+1;
		/*
		if(TOWNINFO.nCityCode==nCityID) 
		{
			nuMemcpy(p,TOWNINFO.wsTownName,EACH_TOWN_NAME_LEN );
			p += EACH_TOWN_NAME_LEN;
			j++;
		}*/
	}

	return true;
}
nuINT   CFileMapNewDanyinP::GetWordIndx(nuVOID* pPoiKeyName)
{
	PDYWORD pData = (PDYWORD)m_pMm_SHApi->MM_GetDataMassAddr(m_SearchResIdx);

	nuWCHAR  m_nSearchPoiKeyName[20];
	nuMemcpy(m_nSearchPoiKeyName,pPoiKeyName,sizeof(m_nSearchPoiKeyName));

	nuINT nTotal=0;
	nuBOOL bFIND=nuFALSE;
	nuINT i=0;
	for(;i<m_nDYWordIdx;i++)
	{
		if(nuWcscmp(pData[i].name,m_nSearchPoiKeyName)==0)
		{
			bFIND = true;
			//nCount = m_SearchRes[i].nFindCount;
			break;
			//return ;
		}
		//nCount+=;
	}
    if(!bFIND)
	{
		return -2;
	}
	return i;
}
nuINT CFileMapNewDanyinP::GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen)
{
	if( m_pFileCB == NULL)
	{
		OpenFile();
	} 
	if( m_pFileCB == NULL)
	{
		return nuFALSE;
	}
	//nuMemcpy(pBuffer,wsALLCityName,nMaxLen);

	//return m_ndyHead.nCityCount;

	if( nuFseek( m_pFileCB, sizeof(SEARCH_CB_HEADER),NURO_SEEK_SET ) != 0 ||
		nuFread(pBuffer, EACH_CITY_NAME_LEN*m_ndyHead.nCityCount, 1, m_pFileCB) != 1 )
	{
		nuFclose(m_pFileCB);
		m_pFileCB = NULL;
		return nuFALSE;
	}
	//GetTownIDAll();
	return m_ndyHead.nCityCount;
}
nuBOOL CFileMapNewDanyinP::CheckRoadSame(nuWORD m_nBeforeLen,nuINT nLen,nuDWORD nIdx)
{
	if(m_bFindPOI)
	{
		return nuTRUE;
	}
	PNDYROADINFO pData = (PNDYROADINFO)m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinROADINFOIdx);
	
	
	DYNODE_B_2 DyNode2 ={0};
	if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + (m_nBerforeCityLen+nIdx)*sizeof(DYNODE_B_2) ,NURO_SEEK_SET) != 0 ||
		nuFread(&DyNode2, sizeof(DYNODE_B_2), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	if(DyNode2.nType == 1)
	{
		m_bFindPOI = nuTRUE;
		return nuFALSE;
	}
	nuWCHAR wsName[128] = {0};
	if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr+m_nBerforeCityWORDLen+m_nBeforeLen*2 ,NURO_SEEK_SET) != 0 ||
		nuFread(&wsName, nLen*sizeof(nuWCHAR), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}

	//nuBOOL bFind = nuFALSE;
	for(nuINT i=0;i<m_nRoadCount;i++)
	{
		nuINT nWordLen = nuWcslen(pData[i].wsRoadName);
		if(nWordLen!= nuWcslen(wsName))
		{
			continue;
		}

		if(nuWcscmp(wsName,pData[i].wsRoadName)== 0 && pData[i].nTownID == DyNode2.nTownID)
		{
			return nuFALSE;
		}
	}
	nuWcscpy(pData[m_nRoadCount].wsRoadName,wsName);
	pData[m_nRoadCount].nTownID = DyNode2.nTownID;
	m_nRoadCount++;
	
	return nuTRUE;
}
nuBOOL CFileMapNewDanyinP::CheckRoadSame(nuWCHAR *wsRoadName,nuINT nLen,nuDWORD nIdx)
{
	if(m_bFindPOI)
	{
		//return nuTRUE;
	}
	PNDYROADINFO pData = (PNDYROADINFO)m_pMm_SHApi->MM_GetDataMassAddr(m_nNewDanYinROADINFOIdx);
	
	
	DYNODE_B_2 DyNode2 ={0};
	if( nuFseek(m_pFile, m_ndyHead.nFeatCodeStart + (m_nBerforeCityLen+nIdx)*sizeof(DYNODE_B_2) ,NURO_SEEK_SET) != 0 ||
		nuFread(&DyNode2, sizeof(DYNODE_B_2), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	if(DyNode2.nType == 1)
	{
		m_bFindPOI = nuTRUE;
		return nuFALSE;
	}
	nuWCHAR wsName[128] = {0};
	nuMemcpy(wsName,wsRoadName,sizeof(nuWCHAR)*nLen);
	/*nuWCHAR wsName[128] = {0};
	if( nuFseek(m_pFile, m_ndyHead.nNameStartAddr+m_nBerforeCityWORDLen+m_nBeforeLen*2 ,NURO_SEEK_SET) != 0 ||
		nuFread(&wsName, nLen*sizeof(nuWCHAR), 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}*/

	//nuBOOL bFind = nuFALSE;
	for(nuINT i=0;i<m_nRoadCount;i++)
	{
		nuINT nWordLen = nuWcslen(pData[i].wsRoadName);
		if(nWordLen!= nuWcslen(wsName))
		{
			continue;
		}

		if(nuWcscmp(wsName,pData[i].wsRoadName)== 0 && pData[i].nTownID == DyNode2.nTownID)
		{
			return nuFALSE;
		}
	}
	nuWcscpy(pData[m_nRoadCount].wsRoadName,wsName);
	pData[m_nRoadCount].nTownID = DyNode2.nTownID;
	m_nRoadCount++;
	
	return nuTRUE;
}
nuBOOL CFileMapNewDanyinP::ColCityRealCount(NDY_CITYDATA	*ndyCityDataALL)
{
	nuWORD nNewDanYinLenMemIdx = -1;
	nuINT nSize = m_ndyHead.nTotalCount;

	nuPBYTE pLenData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(m_ndyHead.nTotalCount, &nNewDanYinLenMemIdx);
	if(nNewDanYinLenMemIdx==(nuWORD)-1)
	{
		return nuFALSE;
	}
	if( nuFseek(m_pFile, m_ndyHead.nNameLenStartAddr, NURO_SEEK_SET) != 0 ||
		nuFread(pLenData, m_ndyHead.nTotalCount, 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuDWORD  nBeforeLen = 0;
	nuDWORD  nBeforeIdx = 0;

	nuINT i = 0; 
	for(;i<m_ndyHead.nCityCount;i++)
	{
		nuDWORD  nIdx = 0;
		nuDWORD nCityLen = 0;
		if(i+1!=m_ndyHead.nCityCount)
		{
			nuDWORD nCityCount = ndyCityDataALL[i+1].CityRoadAddr - ndyCityDataALL[i].CityRoadAddr;
			nuDWORD nowLen = 0;
			
			nIdx =nBeforeIdx ;
			nuINT nColCount =0;
			while(nCityLen<nCityCount)
			{
				nColCount++;
				nCityLen+=pLenData[nIdx++]*2;
			}
			nBeforeIdx =nIdx;
			nBeforeLen +=nCityLen;
			ndyCityDataALL[i].nCityDataCount = nColCount;
			int k = 0;
		}
		else
		{
			nuDWORD nLastCount = 0 ;
			if(m_ndyHead.nCityCount>1)	
			nLastCount = m_ndyHead.nTotalCount - nBeforeIdx;
			else
			nLastCount=	ndyCityDataALL[0].nCityDataCount - nBeforeIdx;

			ndyCityDataALL[i].nCityDataCount = nLastCount;
			//nCount[i]=nLastCount;
			//nBeforeLen + = nBeforeLen;
		}
		
		
	}
/*
	nuDWORD nTOTAL = 0;
	for(int ll=0;ll<m_ndyHead.nCityCount;ll++)
	{

		if(ndyCityDataALL[ll].nCityDataCount!=nCount[ll])
		{
			

			nuTCHAR tsFile[NURO_MAX_PATH] = {0};
			nuTcscpy(tsFile, nuTEXT(".NDY"));
			if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
			{
				return nuFALSE;
			}

			int l=0;
		}

		nTOTAL+= nCount[ll];
	}

	if(nTOTAL!=m_ndyHead.nTotalCount)
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
			nuTcscpy(tsFile, nuTEXT(".NDY"));
			if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
			{
				return nuFALSE;
			}
		int l=0;
	}
*/
	if(nNewDanYinLenMemIdx!=(nuWORD)-1)
	{
		m_pMm_SHApi->MM_RelDataMemMass(&nNewDanYinLenMemIdx);
	}

	return nuTRUE;
}
nuINT CFileMapNewDanyinP::GetTownIDInfo(nuVOID *data)
{
	nuMemcpy(data,n_CityTownID,sizeof(n_CityTownID));
	return true;
}
