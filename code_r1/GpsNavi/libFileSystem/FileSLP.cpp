// FileSLP.cpp: implementation of the CFileSLP class.
//
//////////////////////////////////////////////////////////////////////

#include "FileSLP.h"
//#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileSLP::CFileSLP()
{
	m_ppSLP_Data = NULL;
}

CFileSLP::~CFileSLP()
{
	Free();
}

nuBOOL CFileSLP::LOAD_SLP(nuLONG lMapIdx)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".SLP"));
	if( !m_pFsApi->FS_FindFileWholePath(lMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}

	/*if( !LibFS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}*/
	nuFILE *pFile = NULL;
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	//SLPHEARDER Hearder2 = {0};
	nuMemset(&m_SLP_Hearder, 0, sizeof(SLPHEARDER));
	//m_SLP_Hearder.nSLP_Count = 0;

	if( nuFread(&m_SLP_Hearder, sizeof(m_SLP_Hearder), 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	//m_SLP_Hearder.nSLP_Count -= 3;
	m_ppSLP_Data = (SLPDATA**)m_pMmApi->MM_AllocMem(sizeof(SLPDATA) * m_SLP_Hearder.nSLP_Count);
	//m_ppSLP_Data = new SLPDATA[m_SLP_Hearder.nCount];
	if((*m_ppSLP_Data) == NULL)
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	if( nuFseek(pFile, sizeof(SLPDATA)*3 +200, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread((*m_ppSLP_Data), sizeof(SLPDATA)*m_SLP_Hearder.nSLP_Count, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}

	nuFclose(pFile);
	return nuTRUE;
}
nuLONG CFileSLP::FindSLP(nuLONG lMapIdx,nuDWORD nBlkIdx,nuDWORD nRoadIdx,PSLPDATA TempData)
{
	nuBOOL bRes  = nuFALSE;
	nuINT  nLimit = 0;
	nuLONG Count = 0; 
	if( LOAD_SLP(lMapIdx) == nuFALSE )
	{
		Free();
		return nuFALSE;
	}
	nLimit= (nuINT)m_SLP_Hearder.nSLP_Count;
	for(nuINT i=0;i<nLimit;i++)
	{	
		if(nBlkIdx == (*m_ppSLP_Data)[i].nSlope_BkIdx && nRoadIdx == (*m_ppSLP_Data)[i].nSlope_RdIdx)
		{
			TempData[Count] = (*m_ppSLP_Data)[i];
		    /*TempData[Count].nSlope_BkIdx    = (*m_ppSLP_Data)[i].nSlope_BkIdx;
			TempData[Count].nSlope_RdIdx    = (*m_ppSLP_Data)[i].nSlope_RdIdx;
			TempData[Count].nLink_BkIdx		= (*m_ppSLP_Data)[i].nLink_BkIdx;
			TempData[Count].nLink_RdIdx     = (*m_ppSLP_Data)[i].nLink_RdIdx;*/
			Count++;
		}
		if( (*m_ppSLP_Data)[i].nSlope_BkIdx == nBlkIdx &&  nRoadIdx <  (*m_ppSLP_Data)[i].nLink_RdIdx  )
		{
			continue;;
		}
		if( (*m_ppSLP_Data)[i].nSlope_BkIdx > nBlkIdx)
		{
			break;
		}
	}
	Free();
	return Count;
}
nuVOID  CFileSLP::Free()
{
	if(m_ppSLP_Data)
	{
        m_pMmApi->MM_FreeMem((nuPVOID*)m_ppSLP_Data);
        m_ppSLP_Data = NULL;
	}	
}
/*
nuBOOL	CFileSLP::BinarSearch(nuINT code)
{	

	nuINT idx=-1;
	nuINT nLimit = m_SLP_Hearder.nCount;
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	

	low	  = 0;
	upper = nLimit-1;

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
}*/