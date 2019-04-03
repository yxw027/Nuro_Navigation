// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadInNavi.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadInNavi.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADINNAVI_H__342E8E8C_67DD_4FC4_AFBA_8B73F845C690__INCLUDED_)
#define AFX_LOADINNAVI_H__342E8E8C_67DD_4FC4_AFBA_8B73F845C690__INCLUDED_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"

class CLoadInNavi
{
public:
	typedef nuBOOL(*IN_InitInNaviProc)(PAPISTRUCTADDR pApiAddr);
	typedef nuVOID(*IN_FreeInNaviProc)();

public:
	CLoadInNavi();
	virtual ~CLoadInNavi();
	nuBOOL  InitModule(PAPISTRUCTADDR pApiAddr);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADINNAVI_H__342E8E8C_67DD_4FC4_AFBA_8B73F845C690__INCLUDED_)
