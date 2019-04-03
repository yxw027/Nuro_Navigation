// TransfBrokenRdZK.cpp: implementation of the CTransfBrokenRdZK class.
//
//////////////////////////////////////////////////////////////////////

#include "nuApiInterFace.h"
#include "nuroClib.h"
#include "TransfBrokenRdZK.h"
#include "libFileSystem.h"
#include "TransfByRTZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransfBrokenRdZK::CTransfBrokenRdZK()
{
	m_pstuDisaterHead	= NULL;
	m_dwDisasterCount	= 0;
}

CTransfBrokenRdZK::~CTransfBrokenRdZK()
{
	FreeData();
}

bool CTransfBrokenRdZK::TransfRoadPoint(const TCHAR *ptzFileRaw, const TCHAR *ptzFileRT, const TCHAR *ptzFileOut)
{
	if( !ReadDisasterRawData(ptzFileRaw) )
	{
		return false;
	}
	CTransfByRTZK tranfByRt;
	if( !tranfByRt.ReadRTDataA3(ptzFileRT) )
	{
		return false;
	}
	nuFILE *pfOut = nuTfopen(ptzFileOut, NURO_FS_WB);
	bool bOk = false;
	nuDWORD nFID, nTID;
	if( NULL != pfOut )
	{
		PDISATER_OUT pDstOut = new DISATER_OUT[m_dwDisasterCount];
		if( NULL != pDstOut )
		{
			bOk = true;
			SEEKEDROADDATA stuRdData;
			for(nuDWORD i = 0; i < m_dwDisasterCount; ++i)
			{
				if( !LibFS_SeekForRoad( m_pstuDisaterHead[i].point.x, 
					m_pstuDisaterHead[i].point.y, 
					500,
					-1,
					&stuRdData ) )
				{
					bOk = false;
					break;
				}
				if( !tranfByRt.FindNodeIDByIdx( nFID, nTID, nuLOWORD(stuRdData.nBlkIdx), nuLOWORD(stuRdData.nRoadIdx) ) )
				{
					bOk = false;
					break;
				}
				pDstOut[i].nFNodeID	= nFID;
				pDstOut[i].nTNodeID	= nTID;
				pDstOut[i].nStartY	= (m_pstuDisaterHead[i].nStartYear);
				pDstOut[i].nStartM	= nuLOBYTE(m_pstuDisaterHead[i].nStartMonth);
				pDstOut[i].nStartD	= nuLOBYTE(m_pstuDisaterHead[i].nStartDay);
				pDstOut[i].nEndY	= (m_pstuDisaterHead[i].nEndYear);
				pDstOut[i].nEndM	= nuLOBYTE(m_pstuDisaterHead[i].nEndMonth);
				pDstOut[i].nEndD	= nuLOBYTE(m_pstuDisaterHead[i].nEndDay);
			}
			if( bOk )
			{
				SortDstOut(pDstOut, m_dwDisasterCount);
				nuFwrite(&m_dwDisasterCount, 4, 1, pfOut);
				nuFwrite(pDstOut, sizeof(DISATER_OUT), m_dwDisasterCount, pfOut);
			}
			delete[] pDstOut;
			pDstOut=NULL;
		}
		nuFclose(pfOut);
		pfOut=NULL;
	}
	FreeData();
	return true;
}

bool CTransfBrokenRdZK::ReadDisasterRawData(const TCHAR *ptzFileRaw)
{
	if( NULL == ptzFileRaw )
	{
		return false;
	}
	nuFILE* pFile = nuTfopen(ptzFileRaw, NURO_FS_RB);
	if( NULL == pFile )
	{
		return false;
	}
	m_dwDisasterCount = nuFgetlen(pFile);
	m_dwDisasterCount /= sizeof(DISASTER_RAW);
	if( nuFseek( pFile, 0, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pFile);
		pFile=NULL;
		return false;
	}
	m_pstuDisaterHead	= new DISASTER_RAW[m_dwDisasterCount];
	if( NULL == m_pstuDisaterHead )
	{
		nuFclose(pFile);
		pFile=NULL;
		return false;
	}
	bool bres = true;
	if( nuFread(m_pstuDisaterHead, sizeof( DISASTER_RAW), m_dwDisasterCount, pFile) != m_dwDisasterCount )
	{
		bres = false;
		delete []m_pstuDisaterHead;
		m_pstuDisaterHead = NULL;
	}
	nuFclose(pFile);
	pFile=NULL;
	return bres;
}

void CTransfBrokenRdZK::FreeData()
{
	if( NULL != m_pstuDisaterHead )
	{
		m_dwDisasterCount = 0;
		delete[] m_pstuDisaterHead;
		m_pstuDisaterHead = NULL;
	}
}

void CTransfBrokenRdZK::SortDstOut(PDISATER_OUT pDstOut, nuDWORD nCount)
{
	DISATER_OUT dstOut;
	for(nuDWORD i = 0; i < nCount; ++i)
	{
		for(nuDWORD j = 0; j < nCount - i - 1; ++j)
		{
			if( pDstOut[j].nFNodeID > pDstOut[j+1].nFNodeID )
			{
				dstOut = pDstOut[j+1];
				pDstOut[j+1] = pDstOut[j];
				pDstOut[j] = dstOut;
			}
		}
	}
}