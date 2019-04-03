// TransfByRTZK.cpp: implementation of the CTransfByRTZK class.
//
//////////////////////////////////////////////////////////////////////

#include "TransfByRTZK.h"
#include <tchar.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransfByRTZK::CTransfByRTZK()
{
	m_ppstuDataA3	= NULL;
	m_pdwA3Count	= NULL;
	m_nRtAreaCount	= 0;
}

CTransfByRTZK::~CTransfByRTZK()
{
	Free();
}

bool CTransfByRTZK::ReadRTDataA3(const TCHAR *ptzFileRT)
{
	if( NULL == ptzFileRT )
	{
		return false;
	}
	FILE *pFile = _tfopen(ptzFileRT, TEXT("rb"));
	if( NULL == pFile )
	{
		return false;
	}
	RT_HEAD_COUNT stuHeader;
	if( fseek(pFile, 0, SEEK_SET) != 0 ||
		fread(&stuHeader, sizeof(RT_HEAD_COUNT), 1, pFile) != 1 )
	{
		fclose(pFile);
		return false;
	}
	DWORD* pdwAddr = new DWORD[stuHeader.nRtAreaCount];
	if( NULL == pdwAddr )
	{
		fclose(pFile);
		return false;
	}
	if( fseek(pFile, 4 + 4 * stuHeader.nRtAreaCount, SEEK_SET) != 0 ||
		fread(pdwAddr, 4, stuHeader.nRtAreaCount, pFile) != stuHeader.nRtAreaCount )
	{
		fclose(pFile);
		delete[] pdwAddr;
		pdwAddr=NULL;
		return false;
	}
	//pointer array
	m_ppstuDataA3 = new PRT_DATA_A3[stuHeader.nRtAreaCount];
	if( NULL == m_ppstuDataA3 )
	{
		fclose(pFile);
		delete[] pdwAddr;
		pdwAddr=NULL;
		return false;
	}
	m_pdwA3Count = new DWORD[stuHeader.nRtAreaCount];
	if( NULL == m_pdwA3Count )
	{
		fclose(pFile);
		delete[] pdwAddr;
		pdwAddr=NULL;
		delete[] m_ppstuDataA3;
		m_ppstuDataA3 = NULL;
		return false;
	}
	memset(m_ppstuDataA3, NULL, 4 * stuHeader.nRtAreaCount);
	m_nRtAreaCount = stuHeader.nRtAreaCount;
	bool bres = true;
	RT_DATA_A_HEADER stuRtDataAHead;
	for(WORD i = 0; i < stuHeader.nRtAreaCount; ++i)
	{
		if( fseek(pFile, pdwAddr[i], SEEK_SET) != 0	|| 
			fread(&stuRtDataAHead, sizeof(RT_DATA_A_HEADER), 1, pFile) != 1 ||
			fseek(pFile, stuRtDataAHead.nIndexAddr, SEEK_SET) != 0 )
		{
			bres = false;
			break;
		}
		m_ppstuDataA3[i] = new RT_DATA_A3[stuRtDataAHead.nTotalRoadCount];
		if( NULL == m_ppstuDataA3[i] )
		{
			bres = false;
			break;
		}
		m_pdwA3Count[i] = stuRtDataAHead.nTotalRoadCount;
		if( fread(m_ppstuDataA3[i], sizeof(RT_DATA_A3), m_pdwA3Count[i], pFile) != m_pdwA3Count[i] )
		{
			bres = false;
			break;
		}
	}
	fclose(pFile);
	delete[] pdwAddr;
	pdwAddr=NULL;
	return bres;
}

bool CTransfByRTZK::FindNodeIDByIdx(DWORD& nFNodeID, DWORD& nTNodeID, WORD nBlkIdx, WORD nRdIdx)
{
	if( NULL != m_ppstuDataA3 && NULL != m_pdwA3Count )
	{
		for(WORD i = 0; i < m_nRtAreaCount; ++i)
		{
			if( NULL != m_ppstuDataA3[i] )
			{
				for(DWORD j = 0; j < m_pdwA3Count[i]; ++j)
				{
					if( m_ppstuDataA3[i][j].nBlkIdx	== nBlkIdx	&&
						m_ppstuDataA3[i][j].nRdIdx	== nRdIdx	)
					{
						nFNodeID = m_ppstuDataA3[i][j].nFNodeID;
						nTNodeID = m_ppstuDataA3[i][j].nTNodeID;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void CTransfByRTZK::Free()
{
	if( NULL != m_pdwA3Count )
	{
		delete[] m_pdwA3Count;
		m_pdwA3Count = NULL;
	}
	if( NULL != m_ppstuDataA3 )
	{
		for(WORD i = 0; i < m_nRtAreaCount; ++i)
		{
			if( NULL != m_ppstuDataA3[i] )
			{
				delete[] m_ppstuDataA3[i];
				m_ppstuDataA3[i]=NULL;
			}
		}
		delete[] m_ppstuDataA3;
		m_ppstuDataA3 = NULL;
		m_nRtAreaCount = 0;
	}
}