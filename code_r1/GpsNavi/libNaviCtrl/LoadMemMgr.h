// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMemMgr.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadMemMgr.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADMEMMGR_H__8F7589F1_3034_44AF_AAD8_C8B7AC76B021__INCLUDED_)
#define AFX_LOADMEMMGR_H__8F7589F1_3034_44AF_AAD8_C8B7AC76B021__INCLUDED_

#include "NuroDefine.h"
#include "NuroNaviCtrlConfig.h"

class CLoadMemMgr  
{
public:
	typedef nuBOOL(*MM_InitMemMgrProc)(nuPVOID);
	typedef nuVOID(*MM_FreeMemMgrProc)();

public:
	CLoadMemMgr();
	virtual ~CLoadMemMgr();
	nuBOOL  InitModule(nuPVOID lpOutMM,nuPVOID lpOutMM2);
	nuVOID  UnLoadModule();


protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADMEMMGR_H__8F7589F1_3034_44AF_AAD8_C8B7AC76B021__INCLUDED_)
