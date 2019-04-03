// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: CodeList.cpp,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/CodeList.cpp,v $
//////////////////////////////////////////////////////////////////////

#include "CodeList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodeList::CCodeList()
{
	nuMemset(m_pCodeList, 0, sizeof(m_pCodeList));
	m_nNowCount	= 0;
	m_nFirstIdx	= 0;
	m_bWorking	= nuFALSE;
}

CCodeList::~CCodeList()
{
	Free();
}

nuBOOL CCodeList::Initialize()
{
	return nuTRUE;
}

nuVOID CCodeList::Free()
{

}

nuVOID CCodeList::ClearList()
{
	m_nFirstIdx = m_nNowCount = 0;
}

nuBOOL CCodeList::PushNode(PCODENODE pNode, nuBYTE byPushType/* = PUSH_ADD_ATEND*/) 
{
	nuBOOL res  = nuFALSE;
	nuWORD nIdx = 0;
	while( m_bWorking )
	{
		nuSleep(10);
	}
	m_bWorking = nuTRUE;
	if( byPushType == PUSH_ADD_CIRCLE )
	{
		nIdx = (m_nFirstIdx + m_nNowCount)%LISTCOUNT;
		CodeCopy(&m_pCodeList[nIdx], pNode);
		if( m_nNowCount < LISTCOUNT )
		{
			m_nNowCount ++;
		}
		else
		{
			m_nFirstIdx = (m_nFirstIdx+1)%LISTCOUNT;
		}
		res = nuTRUE;
	}
	else if( byPushType == PUSH_ADD_ATEND )
	{
		if( m_nNowCount < LISTCOUNT )
		{
			nIdx = (m_nFirstIdx + m_nNowCount)%LISTCOUNT;
			CodeCopy(&m_pCodeList[nIdx], pNode);
			m_nNowCount ++;
			res = nuTRUE;
		}
	}
	else if( byPushType == PUSH_ADD_ATHEAD )
	{
		if( m_nNowCount < LISTCOUNT )
		{
			m_nFirstIdx--;
			if( m_nFirstIdx < 0 )
			{
				m_nFirstIdx += LISTCOUNT;
			}
			CodeCopy(&m_pCodeList[m_nFirstIdx], pNode);
			m_nNowCount ++;
			res = nuTRUE;
		}
	}
	else if( byPushType == PUSH_ADD_DELOTHERS )
	{
		CodeCopy(&m_pCodeList[0], pNode);
		m_nFirstIdx = 0;
		m_nNowCount = 1;
		res = nuTRUE;
	}
	else if( byPushType == PUSH_ADD_ISEMPTY )
	{
		if( m_nNowCount == 0 )
		{
			CodeCopy(&m_pCodeList[0], pNode);
			m_nFirstIdx = 0;
			m_nNowCount = 1;
			res = nuTRUE;
		}
	}
	m_bWorking = nuFALSE;
	return res;
}

nuBOOL CCodeList::PopNode(PCODENODE pNode, nuBYTE byPopType /*= POP_ATHEAD*/)
{
	nuBOOL res = nuFALSE;
	while( m_bWorking )
	{
		nuSleep(10);
	}
	m_bWorking = nuTRUE;
	if( byPopType == POP_ATHEAD )
	{
		if( m_nNowCount > 0 )
		{
//			*pNode = m_pCodeList[m_nFirstIdx];
			CodeCopy(pNode, &m_pCodeList[m_nFirstIdx]);
			m_nFirstIdx = (m_nFirstIdx+1)%LISTCOUNT;
			m_nNowCount --;
			res = nuTRUE;
		}
	}
	m_bWorking = nuFALSE;
	return res;
}

nuVOID CCodeList::CodeCopy(PCODENODE pDesCode, PCODENODE pSrcCode)
{
	nuMemcpy(pDesCode, pSrcCode, sizeof(CODENODE));
	/*pDesCode->nDrawType		= pSrcCode->nDrawType;
	pDesCode->nActionType	= pSrcCode->nActionType;
	pDesCode->nX			= pSrcCode->nX;
	pDesCode->nY			= pSrcCode->nY;
	pDesCode->nExtend		= pSrcCode->nExtend;*/
}
