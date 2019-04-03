// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadSearch.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadSearch.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADSEARCH_H__266F9203_2C3A_489A_81F4_90B7F9CF4C3A__INCLUDED_)
#define AFX_LOADSEARCH_H__266F9203_2C3A_489A_81F4_90B7F9CF4C3A__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"

class CLoadSearch  
{
public:
	typedef nuBOOL(*SH_InitSearchZProc)(PAPISTRUCTADDR);
	typedef nuVOID(*SH_FreeSearchZProc)();

public:
	CLoadSearch();
	virtual ~CLoadSearch();

	nuBOOL  LoadModule(PAPISTRUCTADDR pApiAddr);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADSEARCH_H__266F9203_2C3A_489A_81F4_90B7F9CF4C3A__INCLUDED_)
