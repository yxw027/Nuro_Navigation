// FilePinyinZ.cpp: implementation of the CFilePinyinZ class.
//
//////////////////////////////////////////////////////////////////////
#include "FilePinyinZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePinyinZ::CFilePinyinZ()
{

}

CFilePinyinZ::~CFilePinyinZ()
{
	Free();
}

nuBOOL CFilePinyinZ::Initialize()
{
	m_nPYAMemIdx	= (nuWORD)(-1);
	m_nPYBorCMemIdx	= (nuWORD)(-1);
	m_nPYDMemIdx	= (nuWORD)(-1);
	nuMemset(&m_pdyHeader, 0, sizeof(m_pdyHeader));
	m_pFile		= NULL;
	m_nMapIdx	= -1;
	m_nMode		= 0;
	return nuTRUE;
}

nuVOID CFilePinyinZ::Free()
{
	if( m_nPYAMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYAMemIdx);
	}
	if( m_nPYBorCMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYBorCMemIdx);
	}
	if( m_nPYDMemIdx != -1 )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYDMemIdx);
	}
	CloseFile();
}

nuBOOL CFilePinyinZ::OpenFile(const nuTCHAR* pTsFileEx)
{
	if( pTsFileEx == NULL )
	{
		return nuFALSE;
	}
	CloseFile();
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, pTsFileEx);
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

nuVOID CFilePinyinZ::CloseFile()
{
	if( m_pFile != NULL )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuBOOL CFilePinyinZ::SetFileMode(nuUINT nMode)
{
	if( m_nMode != nMode )
	{
		m_nMode = nMode;
		CloseFile();
	}
	return nuTRUE;
}

nuBOOL CFilePinyinZ::SetMapIdx(nuUINT nMapIdx)
{
	if( m_nMode == SH_PY_MODE_NO )
	{
		return nuFALSE;
	}
	if( m_pFile == NULL || m_nMapIdx != nMapIdx )
	{
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYDMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYBorCMemIdx);
		m_pMm_SHApi->MM_RelDataMemMass(&m_nPYAMemIdx);
		m_pMm_SHApi->MM_CollectDataMem(0);
		//
		m_nMapIdx = nMapIdx;
		if( m_nMode & SH_PY_MODE_ROAD )
		{
			if( !OpenFile(nuTEXT(".rdy")) )
			{
				return nuFALSE;
			}
		}
		else if( m_nMode & SH_PY_MODE_POI )
		{
			if( !OpenFile(nuTEXT(".pdy")) )
			{
				return nuFALSE;
			}
		}
		else
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFread(&m_pdyHeader, sizeof(PDY_HEADER), 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
		nuLONG nSize = m_pdyHeader.nTotalCount;
		nuPBYTE pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nPYAMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, sizeof(PDY_HEADER), NURO_SEEK_SET) != 0 ||
			nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
		//Allocate Data memory from MemMgr.dll
		nuLONG nDis = 0;
		if( m_nMode & SH_PY_MODE_STR_C )
		{
			nSize = m_pdyHeader.nDataAddr - m_pdyHeader.nNameAddr;
			nDis = m_pdyHeader.nNameAddr;
		}
		else if( m_nMode & SH_PY_MODE_STR_B )
		{
			nSize = m_pdyHeader.nNameAddr - m_pdyHeader.nDanYinAddr;
			nDis = m_pdyHeader.nDanYinAddr;
		}
		else
		{
			return nuFALSE;
		}
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nPYBorCMemIdx);
		if( pData == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(m_pFile, nDis, NURO_SEEK_SET) != 0 ||
			nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}

nuPVOID CFilePinyinZ::GetPyALength(nuDWORD *pCount)
{
	if( pCount )
	{
		*pCount = m_pdyHeader.nTotalCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nPYAMemIdx);
}

nuPVOID CFilePinyinZ::GetPyBorCString(nuDWORD *pCount)
{
	if( pCount )
	{
		*pCount = m_pdyHeader.nTotalCount;
	}
	return m_pMm_SHApi->MM_GetDataMassAddr(m_nPYBorCMemIdx);
}

nuPVOID CFilePinyinZ::GetPyDData(nuDWORD* pCount)
{
	nuPBYTE pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMassAddr(m_nPYDMemIdx);
	if( pData == NULL )
	{
		long nSize = sizeof(PDY_D_NODE)*m_pdyHeader.nTotalCount;
		if( nSize == 0 || m_pFile == NULL )
		{
			return NULL;
		}
		pData = (nuPBYTE)m_pMm_SHApi->MM_GetDataMemMass(nSize, &m_nPYDMemIdx);
		if( pData == NULL )
		{
			return NULL;
		}
		if( nuFseek(m_pFile, m_pdyHeader.nDataAddr, NURO_SEEK_SET) != 0 ||
			nuFread(pData, nSize, 1, m_pFile) != 1 )
		{
			m_pMm_SHApi->MM_RelDataMemMass(&m_nPYDMemIdx);
			return NULL;
		}
	}
	if( pCount )
	{
		*pCount = m_pdyHeader.nTotalCount;
	}
	return pData;
}

nuBOOL CFilePinyinZ::GetPyOneData(PSH_ITEM_FORUI pItem, nuDWORD nIdx)
{
	if( pItem == NULL || nIdx >= m_pdyHeader.nTotalCount || m_pFile == NULL )
	{
		return nuFALSE;
	}
	PPDY_D_NODE pPdyDNode2 = (PPDY_D_NODE)GetPyDData(NULL);
	if( pPdyDNode2 == NULL )
	{
		return nuFALSE;
	}//It must be here. May there is some memory collection in this invocation.
	nuPBYTE pLength = (nuPBYTE)GetPyALength(NULL);
	if( pLength == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pItem, 0, sizeof(SH_ITEM_FORUI));
	nuDWORD i = 0;
	nuLONG nAddr = m_pdyHeader.nNameAddr;
	for( ; i < nIdx; ++i )
	{
		nAddr += (pLength[i]<<1);
	}
	//
	nuSIZE nNum = NURO_MIN((SH_NAME_FORUI_NUM-1), pLength[i]);
	if( nuFseek(m_pFile, nAddr, NURO_SEEK_SET) != 0 ||
		nuFread(pItem->wsName, sizeof(nuWCHAR), nNum, m_pFile) != nNum )
	{
		return nuFALSE;
	}
	pItem->nStroke	= pPdyDNode2[nIdx].nPinYin;
	pItem->nType	= pPdyDNode2[nIdx].nType;
	pItem->nCityID	= pPdyDNode2[nIdx].nCityID;
	pItem->nTownID	= pPdyDNode2[nIdx].nTownID;

	nAddr = m_pdyHeader.nCityAddr + pItem->nCityID * PDY_CITYTOWN_NAME_LEN;
	nNum = NURO_MIN((SH_NAME_FORUI_CITYORTOWN_NUM-1), PDY_CITYTOWN_NAME_LEN/sizeof(nuWCHAR));
	if( nuFseek(m_pFile, nAddr, NURO_SEEK_SET) != 0 ||
		nuFread(pItem->wsCityName, sizeof(nuWCHAR), nNum, m_pFile) != nNum )
	{
		return nuFALSE;
	}
	nAddr = m_pdyHeader.nTownAddr + pItem->nTownID * PDY_CITYTOWN_NAME_LEN;
	nNum = NURO_MIN((SH_NAME_FORUI_CITYORTOWN_NUM-1), PDY_CITYTOWN_NAME_LEN/sizeof(nuWCHAR));
	if( nuFseek(m_pFile, nAddr, NURO_SEEK_SET) != 0 ||
		nuFread(pItem->wsTownName, sizeof(nuWCHAR), nNum, m_pFile) != nNum )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CFilePinyinZ::GetPyOneDetail(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx)
{
	if( pOneDetail == NULL || m_pFile == NULL || nIdx >= m_pdyHeader.nTotalCount )
	{
		return nuFALSE;
	}
	nuMemset(pOneDetail, 0, sizeof(SH_DETAIL_FORUI));
	nuLONG nAddr = m_pdyHeader.nPIInfoAddr + nIdx * 4;
	if( nuFseek(m_pFile, nAddr, NURO_SEEK_SET) != 0 ||
		nuFread(&nAddr, 4, 1, m_pFile) != 1 )
	{
		return nuFALSE;
	}
	nuBOOL bRes = nuFALSE;
	if( nAddr )
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuTcscpy(tsFile, nuTEXT(".PI"));
		if( !m_pFsApi->FS_FindFileWholePath(m_nMapIdx, tsFile, NURO_MAX_PATH) )
		{
			return nuFALSE;
		}
		nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
		PI_A_NODE piANode = {0};
		if (nuFseek(pFile, nAddr, NURO_SEEK_SET) == 0 ||
			nuFread(&piANode, sizeof(PI_A_NODE), 1, pFile) == 1)
		{
			bRes = nuTRUE;
			pOneDetail->point.x = piANode.nX;
			pOneDetail->point.y = piANode.nY;
			POIPANODE poiPaNode = {0};
			if( m_pFsApi->FS_ReadPaData(m_nMapIdx, piANode.nOtherInfoAddr, &poiPaNode) )
			{
				long nSize = sizeof(pOneDetail->wsOther);
				nuPBYTE pBuff = (nuPBYTE)pOneDetail->wsOther;
				nuINT nCopy;
				if( poiPaNode.nPhoneLen )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nPhoneLen);
					nuMemcpy(pBuff, poiPaNode.wsPhone, nCopy);
					pOneDetail->nPhoneLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
				if( poiPaNode.nSubBranchLen && nSize > 0 )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nSubBranchLen);
					nuMemcpy(pBuff, poiPaNode.wsSubBranch, nCopy);
					pOneDetail->nSubBranchLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
				if( poiPaNode.nFaxLen && nSize > 0 )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nFaxLen);
					nuMemcpy(pBuff, poiPaNode.wsFax, nCopy);
					pOneDetail->nFaxLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
				if( poiPaNode.nAddressLen && nSize > 0 )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nAddressLen);
					nuMemcpy(pBuff, poiPaNode.wsAddress, nCopy);
					pOneDetail->nAddressLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
				if( poiPaNode.nWebLen && nSize > 0 )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nWebLen);
					nuMemcpy(pBuff, poiPaNode.wsWeb, nCopy);
					pOneDetail->nWebLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
				if( poiPaNode.nOpenTimeLen && nSize > 0 )
				{
					nCopy = NURO_MIN(nSize, poiPaNode.nOpenTimeLen);
					nuMemcpy(pBuff, poiPaNode.wsOpenTime, nCopy);
					pOneDetail->nOpenTimeLen = nCopy;
					pBuff += nCopy;
					nSize -= nCopy;
				}
			}
		}
		nuFclose(pFile);
		pFile=NULL;
	}
	return bRes;
}

nuBOOL CFilePinyinZ::GetPyCityTown(PSH_CITYTOWN_FORUI pCityTown)
{
	if( m_pFile == NULL )
	{
		return nuFALSE;
	}
	if( pCityTown->nType == SH_CITYTOWN_TYPE_CITY_ALL || 
		pCityTown->nType == SH_CITYTOWN_TYPE_CITY_USED )
	{
		if( nuFseek(m_pFile, m_pdyHeader.nCityAddr, NURO_SEEK_SET) != 0 )
		{
			return nuFALSE;
		}
	}
	else if( pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_ALL ||
			 pCityTown->nType == SH_CITYTOWN_TYPE_TOWN_USED )
	{
		if( nuFseek(m_pFile, m_pdyHeader.nTownAddr, NURO_SEEK_SET) != 0 )
		{
			return nuFALSE;
		}
	}
	else
	{
		return nuFALSE;
	}
	nuINT nLastIdx = 0;
	for(nuINT i = 0; i < pCityTown->nTotalCount; ++i)
	{
		if( nuFseek(m_pFile, PDY_CITYTOWN_NAME_LEN * (pCityTown->pIdList[i] - nLastIdx), NURO_SEEK_CUR) != 0 ||
			nuFread(pCityTown->ppName[i], PDY_CITYTOWN_NAME_LEN, 1, m_pFile) != 1 )
		{
			return nuFALSE;
		}
		nLastIdx = pCityTown->pIdList[i];
	}
	return nuTRUE;
}
