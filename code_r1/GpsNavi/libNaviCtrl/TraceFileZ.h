// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TraceFileZ.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/TraceFileZ.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACEFILEZ_H__1D78C363_CB12_4DD2_9A6B_C421A090D4FD__INCLUDED_)
#define AFX_TRACEFILEZ_H__1D78C363_CB12_4DD2_9A6B_C421A090D4FD__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "NuroModuleApiStruct.h"

#define TRACE_TIMERS_TO_RECORD		5//每隔多少秒记录一次轨迹

#define TRACE_SAVE_FAILURE			0x00
#define TRACE_SAVE_SUCESS			0x01
class CTraceFileZ  
{
public:
	CTraceFileZ();
	virtual ~CTraceFileZ();

	nuBOOL Initialize();
	nuVOID Free();

	//Invoked by UI
	nuPVOID GetTraceHead();
	nuUINT	StartTrace(nuBYTE nState);
	nuUINT	CloseTrace(nuBYTE nState);
	nuUINT  DeleteTrace(nuWORD nID);
	nuUINT  ShowTrace(nuWORD nID);
	nuUINT  CloseShow();
	nuUINT	GetTracePoint(nuroPOINT& point, nuWORD nID, nuDWORD nIdx);
	nuUINT	ImportTrace();
	nuUINT	ExportTrace();
	//Invoked by NaviThead
	nuUINT  PendTrace(nuLONG x, nuLONG y);
protected:
	nuUINT  ReadTraceHead();
//	nuUINT  SaveTraceOld(PTRACEMEMORY pTraceMem);
	nuUINT  SaveTrace(PTRACEMEMORY pTraceMem);

protected:
	nuBYTE		m_nRecordTimer;
	nuWORD		m_nShowID;//被显示的轨迹ID
	TRACEFORUI	m_traceForUI;
	nuroPOINT	m_ptLast;
	nuLONG		m_nMinTravelDis;
};

#endif // !defined(AFX_TRACEFILEZ_H__1D78C363_CB12_4DD2_9A6B_C421A090D4FD__INCLUDED_)
