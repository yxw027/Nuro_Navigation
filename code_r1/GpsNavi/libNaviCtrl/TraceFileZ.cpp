// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TraceFileZ.cpp,v 1.3 2010/01/11 05:02:28 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/01/11 05:02:28 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libNaviCtrl/TraceFileZ.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "TraceFileZ.h"
#include "NuroDefine.h"
#include "NuroConstDefined.h"

#define _TRACE_FILE					nuTEXT("record.trc")
#define _TRACE_FILE_COPY			nuTEXT("recordcpy.trc")
extern FILESYSAPI					g_fileSysApi;
extern MEMMGRAPI					g_memMgrApi;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTraceFileZ::CTraceFileZ()
{

}

CTraceFileZ::~CTraceFileZ()
{
	Free();
}

nuBOOL CTraceFileZ::Initialize()
{
	m_nShowID			= 0;
	if( !nuReadConfigValue("MINTRAVELDIS", &m_nMinTravelDis) )
	{
		m_nMinTravelDis = 50;
	}
	nuBOOL bres = nuFALSE;
	if( ReadTraceHead() )
	{
		bres = nuTRUE;
	}
	return bres;
}

void CTraceFileZ::Free()
{
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	if( pTraceMem != NULL )
	{
		g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nNewTraceMemIdx);
		g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nShowTraceMemIdx);
	}
}

nuUINT CTraceFileZ::ReadTraceHead()
{
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFile, _TRACE_FILE);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile )
	{
		nuBOOL bres = nuFALSE;
		if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 &&
			nuFread(&m_traceForUI.fileHead, sizeof(TRACEFILEHEAD), 1, pFile) == 1 &&
			nuFread(&m_traceForUI.pFileTraceHead, sizeof(TRACEHEAD), MAX_TRACE_RECOAD_NUM, pFile) == MAX_TRACE_RECOAD_NUM )
		{
			bres = nuTRUE;
		}
		nuFclose(pFile);
		if( bres )
		{
			return 1;
		}
	}
	nuMemset(&m_traceForUI, 0, sizeof(m_traceForUI));
	m_traceForUI.fileHead.nNowIdleAddres = sizeof(TRACEFILEHEAD) + sizeof(TRACEHEAD)*MAX_TRACE_RECOAD_NUM;
	pFile = nuTfopen(tsFile, NURO_FS_WB);
	if( !pFile )
	{
		return 0;
	}
	nuUINT nres = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 &&
		nuFwrite(&m_traceForUI.fileHead, sizeof(TRACEFILEHEAD), 1, pFile) == 1 &&
		nuFwrite(&m_traceForUI.pFileTraceHead, sizeof(TRACEHEAD), MAX_TRACE_RECOAD_NUM, pFile) == MAX_TRACE_RECOAD_NUM )
	{
		nres = 1;
	}
	nuFclose(pFile);
	return nres;
}

nuPVOID CTraceFileZ::GetTraceHead()
{
	return &m_traceForUI;
}

nuUINT CTraceFileZ::StartTrace(nuBYTE nState)
{
	nuUINT nRes = TRACE_OPEN_FAILURE;
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	if( pTraceMem == NULL )
	{
		return nRes;
	}
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	//
	if( nState == TRACE_OPEN_STATE_NEW )
	{
		if( pTraceMem->nTraceState == TRACE_STATE_OPEN )
		{
			nRes = TRACE_OPEN_HASOPENED;
		}
		else if( pTraceMem->nTraceState == TRACE_STATE_SUSPEND )
		{
			pTraceMem->nTraceState = TRACE_STATE_OPEN;
			nRes = TRACE_OPEN_SUCCESS_RESUME;
		}
		else
		{
			//
			//
			nuMemset(&m_traceForUI.nNowTraceHead, 0, sizeof(TRACEHEAD));
			m_nRecordTimer	= 0;
			m_ptLast.x		= 0;
			m_ptLast.y		= 0;
			//
			nuWORD nID;
			nuBOOL bFind;
			for( nID = 1; nID < MAX_TRACE_RECOAD_NUM + 100; ++nID )
			{
				bFind = nuTRUE;
				for(nuBYTE i = 0; i < m_traceForUI.fileHead.nNowRecordNum; ++i)
				{
					if( m_traceForUI.pFileTraceHead[i].nID == nID )
					{
						bFind = nuFALSE;
						break;
					}
				}
				if( bFind )
				{
					break;
				}
			}
			NUROTIME nuTime;
			nuGetLocalTime(&nuTime);
			m_traceForUI.nNowTraceHead.nID		= nID;
			m_traceForUI.nNowTraceHead.nYear	= nuTime.nYear;//g_fileSysApi.pGdata->timerData.nYear;
			m_traceForUI.nNowTraceHead.nMonth	= nuTime.nMonth;//g_fileSysApi.pGdata->timerData.nMonth;
			m_traceForUI.nNowTraceHead.nDay		= nuTime.nDay;//g_fileSysApi.pGdata->timerData.nDay;
			m_traceForUI.nNowTraceHead.nHour	= nuTime.nHour;//(nuBYTE)g_fileSysApi.pGdata->timerData.nHour;
			m_traceForUI.nNowTraceHead.nMinute	= nuTime.nMinute;//g_fileSysApi.pGdata->timerData.nMinute;
			//
			pTraceMem->nTraceState = TRACE_STATE_OPEN;
			nRes = TRACE_OPEN_SUCCESS_NEW;
		}
	}
	else if( nState == TRACE_OPEN_STATE_RESUME )
	{
		if( pTraceMem->nTraceState == TRACE_STATE_OPEN )
		{
			nRes = TRACE_OPEN_HASOPENED;
		}
		else if( pTraceMem->nTraceState == TRACE_STATE_SUSPEND )
		{
			pTraceMem->nTraceState = TRACE_STATE_OPEN;
			nRes = TRACE_OPEN_SUCCESS_RESUME;
		}
	}
	pTraceMem->bLock = 0;
	return nRes;
}

nuUINT CTraceFileZ::CloseTrace(nuBYTE nState)
{
	nuUINT nRes = TRACE_CLOSE_FAILURE;
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	
	if( nState == TRACE_CLOSE_STATE_SUSPEND )
	{
		if( pTraceMem->nTraceState == TRACE_STATE_OPEN ||
			pTraceMem->nTraceState == TRACE_STATE_SUSPEND )
		{
			pTraceMem->nTraceState = TRACE_STATE_SUSPEND;
			nRes = TRACE_CLOSE_SUCCESS;
		}
		else
		{
			nRes = TRACE_CLOSE_HASECLOSEN;
		}
	}
	else if( nState == TRACE_CLOSE_STATE_SAVE )
	{
		if( pTraceMem->nTraceState == TRACE_STATE_CLOSE )
		{
			nRes = TRACE_CLOSE_FAILURE;
		}
		else
		{
			if( m_traceForUI.fileHead.nNowRecordNum >= MAX_TRACE_RECOAD_NUM  )
			{
				nRes = TRACE_CLOSE_WILLCOVEROLD;
			}
			else
			{
				if( SaveTrace(pTraceMem) == TRACE_SAVE_SUCESS )
				{
					nRes = TRACE_CLOSE_SUCCESS;
				}
				g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nNewTraceMemIdx);
				pTraceMem->nTraceState = TRACE_STATE_CLOSE;
				nuMemset(&m_traceForUI.nNowTraceHead, 0, sizeof(TRACEHEAD));
			}
		}
	}
	else if( nState == TRACE_CLOSE_STATE_COVEROLD )
	{
		if( pTraceMem->nTraceState == TRACE_STATE_CLOSE )
		{
			nRes = TRACE_CLOSE_FAILURE;
		}
		else
		{
			if( SaveTrace(pTraceMem) == TRACE_SAVE_SUCESS )
			{
				nRes = TRACE_CLOSE_SUCCESS;
			}
			g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nNewTraceMemIdx);
			pTraceMem->nTraceState = TRACE_STATE_CLOSE;
			nuMemset(&m_traceForUI.nNowTraceHead, 0, sizeof(TRACEHEAD));
		}
	}
	else
	{
		g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nNewTraceMemIdx);
		pTraceMem->nTraceState = TRACE_STATE_CLOSE;
		nuMemset(&m_traceForUI.nNowTraceHead, 0, sizeof(TRACEHEAD));
		nRes = TRACE_CLOSE_SUCCESS;
	}
	pTraceMem->bLock = 0;
	return nRes;
}
nuUINT CTraceFileZ::SaveTrace(PTRACEMEMORY pTraceMem)
{
	if( pTraceMem->nPointCount == 0 )
	{
		return TRACE_SAVE_FAILURE;
	}
	nuUINT nRes = TRACE_SAVE_FAILURE;
	nuroPOINT *ptList = (nuroPOINT*)g_memMgrApi.MM_GetDataMassAddr(pTraceMem->nNewTraceMemIdx);
	if( ptList == NULL )
	{
		return TRACE_SAVE_FAILURE;
	}
	if( m_traceForUI.fileHead.nNowRecordNum >= MAX_TRACE_RECOAD_NUM )
	{
		//... Delete some record
		nuBYTE nDelIdx = 0;
		for(nuBYTE  i = 1; i < MAX_TRACE_RECOAD_NUM; ++i )
		{
			if( m_traceForUI.pFileTraceHead[i].nYear < m_traceForUI.pFileTraceHead[nDelIdx].nYear )
			{
				nDelIdx = i;
			}
			else if( m_traceForUI.pFileTraceHead[i].nYear == m_traceForUI.pFileTraceHead[nDelIdx].nYear )
			{
				if( m_traceForUI.pFileTraceHead[i].nMonth < m_traceForUI.pFileTraceHead[nDelIdx].nMonth )
				{
					nDelIdx = i;
				}
				else if( m_traceForUI.pFileTraceHead[i].nMonth == m_traceForUI.pFileTraceHead[nDelIdx].nMonth )
				{
					if( m_traceForUI.pFileTraceHead[i].nDay < m_traceForUI.pFileTraceHead[nDelIdx].nDay )
					{
						nDelIdx = i;
					}
					else if( m_traceForUI.pFileTraceHead[i].nDay == m_traceForUI.pFileTraceHead[nDelIdx].nDay )
					{
						if( m_traceForUI.pFileTraceHead[i].nHour < m_traceForUI.pFileTraceHead[nDelIdx].nHour )
						{
							nDelIdx = i;
						}
						else if( m_traceForUI.pFileTraceHead[i].nHour == m_traceForUI.pFileTraceHead[nDelIdx].nHour )
						{
							if( m_traceForUI.pFileTraceHead[i].nMinute < m_traceForUI.pFileTraceHead[nDelIdx].nMinute )
							{
								nDelIdx = i;
							}
						}
					}
				}
			}
		}
		nuMemset( &m_traceForUI.pFileTraceHead[nDelIdx], 
				  0, 
				  sizeof(TRACEHEAD) );
		--m_traceForUI.fileHead.nNowRecordNum;
	}
	//...
	nuBYTE i;
	for(i = 0; i < MAX_TRACE_RECOAD_NUM; ++i)
	{
		if( m_traceForUI.pFileTraceHead[i].nID == 0 )
		{
			break;
		}
	}
	if( i == MAX_TRACE_RECOAD_NUM )
	{
		return nRes;
	}
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFile, _TRACE_FILE);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_OB);
	if( pFile )
	{
		m_traceForUI.pFileTraceHead[i] = m_traceForUI.nNowTraceHead;
		m_traceForUI.pFileTraceHead[i].nPtAddress = TRACE_FILE_HEAD_SIZE + 
			sizeof(nuroPOINT) * MAX_TRACE_ONERECORD_NUM * i;
		m_traceForUI.pFileTraceHead[i].nPtCount	= pTraceMem->nPointCount;
		++m_traceForUI.fileHead.nNowRecordNum;
		m_traceForUI.fileHead.nNowTotalPoints	+=pTraceMem->nPointCount;
		nuMemset(&m_traceForUI.nNowTraceHead, 0, sizeof(TRACEHEAD));
		nuSIZE nSizeWrited;
		nuSIZE nFileSize = nuFgetlen(pFile);
		nFileSize -= m_traceForUI.pFileTraceHead[i].nPtAddress;
		if( nFileSize < sizeof(nuroPOINT) * MAX_TRACE_ONERECORD_NUM )
		{
			nSizeWrited = sizeof(nuroPOINT) * MAX_TRACE_ONERECORD_NUM;
		}
		else
		{
			nSizeWrited = sizeof(nuroPOINT) * pTraceMem->nPointCount;
		}
	
		if( nuFseek(pFile, m_traceForUI.pFileTraceHead[i].nPtAddress, NURO_SEEK_SET) == 0 &&
			nuFwrite(ptList, nSizeWrited, 1, pFile) == 1 )
		{
			nRes = TRACE_SAVE_SUCESS;
		}
		if( nRes == TRACE_SAVE_SUCESS )
		{
			if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
				nuFwrite(&m_traceForUI.fileHead, sizeof(TRACEFILEHEAD), 1, pFile) != 1 || 
				nuFseek(pFile, sizeof(TRACEFILEHEAD) + sizeof(TRACEHEAD) * i, NURO_SEEK_SET) != 0 ||
				nuFwrite(&m_traceForUI.pFileTraceHead[i], sizeof(TRACEHEAD), 1, pFile) != 1 )
			{
				nRes = TRACE_SAVE_FAILURE;
			}
		}
		nuFclose(pFile);
	}
	return nRes;
}

nuUINT CTraceFileZ::PendTrace(nuLONG x, nuLONG y)
{
	nuUINT nRes = 0;
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM); 
	if( pTraceMem->nTraceState == TRACE_STATE_CLOSE || 
		pTraceMem->nPointCount == MAX_TRACE_ONERECORD_NUM ||
		++m_nRecordTimer >= TRACE_TIMERS_TO_RECORD )
	{
		return nRes;
	}
	nuLONG dx = x - m_ptLast.x;
	nuLONG dy = y - m_ptLast.y;
	dx = NURO_ABS(dx);
	dy = NURO_ABS(dy);
	if( dx < m_nMinTravelDis && dy < m_nMinTravelDis )
	{
		return nRes;
	}
	m_nRecordTimer = 0;
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	//
	
	g_memMgrApi.MM_LockDataMemMove();//锁定不能移动内存
	nuroPOINT point = {x, y};
	nuroPOINT *ptPoint = (nuroPOINT*)g_memMgrApi.MM_GetDataMassAddr(pTraceMem->nNewTraceMemIdx);
	if( ptPoint == NULL )
	{
		ptPoint = (nuroPOINT*)g_memMgrApi.MM_GetDataMemMass(sizeof(nuroPOINT) * MAX_TRACE_ONERECORD_NUM, &pTraceMem->nNewTraceMemIdx);
		pTraceMem->nPointCount = 0;
	}
	//
	if( ptPoint && m_traceForUI.nNowTraceHead.nPtCount < MAX_TRACE_ONERECORD_NUM)
	{
		ptPoint[pTraceMem->nPointCount++] = point;
		m_ptLast.x = x;
		m_ptLast.y = y;
		nRes = 1;
	}
	else
	{
		nRes = 0;
	}
	g_memMgrApi.MM_FreeDataMemMove();
	//
	pTraceMem->bLock = 0;
	return nRes;
}

nuUINT CTraceFileZ::DeleteTrace(nuWORD nID)
{
	if( m_traceForUI.fileHead.nNowRecordNum == 0 || nID == 0 )
	{
		return TRACE_DELETE_NOTHISRECORD;
	}
	nuBYTE i;
	for( i = 0; i < MAX_TRACE_RECOAD_NUM; ++i )
	{
		if( m_traceForUI.pFileTraceHead[i].nID == nID )
		{
			break;
		}
	}
	if( i == MAX_TRACE_RECOAD_NUM )
	{
		return TRACE_DELETE_NOTHISRECORD;
	}
	//
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFile, _TRACE_FILE);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_OB);
	nuUINT nRes = TRACE_SAVE_FAILURE;
	if( pFile )
	{
		nuMemset(&m_traceForUI.pFileTraceHead[i], 0, sizeof(TRACEHEAD));
		--m_traceForUI.fileHead.nNowRecordNum;
		if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFwrite(&m_traceForUI.fileHead, sizeof(TRACEFILEHEAD), 1, pFile) != 1 || 
			nuFseek(pFile, sizeof(TRACEFILEHEAD) + sizeof(TRACEHEAD) * i, NURO_SEEK_SET) == 0 &&
			nuFwrite(&m_traceForUI.pFileTraceHead[i], sizeof(TRACEHEAD), 1, pFile) == 1 )
		{
			nRes = TRACE_SAVE_SUCESS;
		}
		nuFclose(pFile);
	}
	pTraceMem->bLock = 0;
	return nRes;
}

//由于会开辟DataMem，所以必须放到NaviThread，不能直接在UI线程调用。
nuUINT CTraceFileZ::ShowTrace(nuWORD nID)
{
	if( m_nShowID == nID )
	{
		return TRACE_SHOW_SUCESS;
	}
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	if( nID == m_traceForUI.nNowTraceHead.nID )
	{
		m_nShowID = nID;
		pTraceMem->nShowTrace = TRACESHOW_STATE_NOW;
		return TRACE_SHOW_SUCESS;
	}
	nuBYTE i;
	for( i = 0; i < MAX_TRACE_RECOAD_NUM; ++i )
	{
		if( m_traceForUI.pFileTraceHead[i].nID == nID )
		{
			break;
		}
	}
	if( i == MAX_TRACE_RECOAD_NUM || m_traceForUI.pFileTraceHead[i].nPtCount == 0 )
	{
		return TRACE_SHOW_FAILURE;
	}
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	nuUINT nRes = TRACE_SHOW_FAILURE;
	pTraceMem->nShowCount = m_traceForUI.pFileTraceHead[i].nPtCount;
	g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nShowTraceMemIdx);
	nuPBYTE pData = (nuPBYTE)g_memMgrApi.MM_GetDataMemMass(sizeof(nuroPOINT)*pTraceMem->nShowCount, &pTraceMem->nShowTraceMemIdx);
	if( pData )
	{
		nuTCHAR tsFile[NURO_MAX_PATH];
		nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
		nuTcscat(tsFile, _TRACE_FILE);
		nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile )
		{
			if( nuFseek(pFile, m_traceForUI.pFileTraceHead[i].nPtAddress, NURO_SEEK_SET) == 0 &&
				nuFread(pData, sizeof(nuroPOINT), pTraceMem->nShowCount, pFile) == pTraceMem->nShowCount )
			{
				m_nShowID = nID;
				pTraceMem->nShowTrace = TRACESHOW_STATE_RECORD;
				nRes = TRACE_SHOW_SUCESS;
			}
			else
			{
				g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nShowTraceMemIdx);
				pTraceMem->nShowCount = 0;
			}
			//
			nuFclose(pFile);
		}
	}
	//
	pTraceMem->bLock = 0;
	return nRes;
}

nuUINT CTraceFileZ::CloseShow()
{
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	while( pTraceMem->bLock )
	{
		nuSleep(10);
	}
	pTraceMem->bLock = 1;
	//
	m_nShowID = 0;
	pTraceMem->nShowTrace = TRACESHOW_STATE_NO;
	g_memMgrApi.MM_RelDataMemMass(&pTraceMem->nShowTraceMemIdx);
	pTraceMem->nShowCount = 0;
	//
	pTraceMem->bLock = 0;
	return 0;
}

nuUINT CTraceFileZ::GetTracePoint(nuroPOINT& point, nuWORD nID, nuDWORD nIdx)
{
	if( nID == 0 )
	{
		return nuFALSE;
	}
	nuUINT nRes = 0;
	if( m_traceForUI.nNowTraceHead.nID == nID )
	{
		PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
		if( pTraceMem == NULL || nIdx >= pTraceMem->nPointCount )
		{
			return 0;
		}
		nuroPOINT *ptPoint = (nuroPOINT*)g_memMgrApi.MM_GetDataMassAddr(pTraceMem->nNewTraceMemIdx);
		if( ptPoint == NULL )
		{
			return 0;
		}
		point = ptPoint[nIdx];
	}
	else
	{
		for(nuBYTE i = 0; i < m_traceForUI.fileHead.nNowRecordNum; ++i)
		{
			if( m_traceForUI.pFileTraceHead[i].nID == nID && nIdx < m_traceForUI.pFileTraceHead[i].nPtCount )
			{
				nuTCHAR tsFile[NURO_MAX_PATH];
				nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
				nuTcscat(tsFile, _TRACE_FILE);
				nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
				if( pFile )
				{
					if( nuFseek(pFile, m_traceForUI.pFileTraceHead[i].nPtAddress + sizeof(nuroPOINT)*nIdx, NURO_SEEK_SET) == 0 &&
						nuFread(&point, sizeof(nuroPOINT), 1, pFile) == 1 )
					{
						nRes = 1;
					}
					nuFclose(pFile);
				}
				break;
			}
		}
	}
	return nRes;
}

nuUINT CTraceFileZ::ImportTrace()
{
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	if( pTraceMem == NULL )
	{
		return 0;
	}
	while( pTraceMem->bLock )
	{
		nuSleep(25);
	}
	pTraceMem->bLock = 1;
	nuUINT nRes = 0;
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFile, _TRACE_FILE_COPY);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile != NULL )
	{
		nuFclose(pFile);
		nuTCHAR tsFile2[NURO_MAX_PATH];
		nuTcscpy(tsFile2, g_fileSysApi.pGdata->pTsPath);
		nuTcscat(tsFile2, _TRACE_FILE);
//		nuDeleteFile(tsFile2);
		if( nuCopyFile(tsFile, tsFile2, nuFALSE) )
		{
			nRes = ReadTraceHead();
		}
	}
	pTraceMem->bLock = 0;
	return nRes;
}

nuUINT CTraceFileZ::ExportTrace()
{
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_fileSysApi.FS_GetData(DATA_TRACEMEM);
	if( pTraceMem == NULL )
	{
		return 0;
	}
	while( pTraceMem->bLock )
	{
		nuSleep(25);
	}
	pTraceMem->bLock = 1;
	nuUINT nRes = 0;
	nuTCHAR tsFileCpy[NURO_MAX_PATH];
	nuTCHAR tsFileTrc[NURO_MAX_PATH];
	nuTcscpy(tsFileCpy, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFileCpy, _TRACE_FILE_COPY);
//	nuDeleteFile(tsFileCpy);
	nuTcscpy(tsFileTrc, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsFileTrc, _TRACE_FILE);
	nRes = nuCopyFile(tsFileTrc, tsFileCpy, nuFALSE);
	/*
	if( nuRenameFile(tsFileTrc, tsFileCpy) == 0 )
	{
		nuDeleteFile(tsFileTrc);
		nRes = ReadTraceHead();
	}
	*/
	pTraceMem->bLock = 0;
	return nRes;
}
