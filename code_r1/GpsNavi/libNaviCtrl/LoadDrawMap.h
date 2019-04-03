// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadDrawMap.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadDrawMap.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADDRAWMAP_H__780A0A37_E304_45A6_919C_EA734A79A3A0__INCLUDED_)
#define AFX_LOADDRAWMAP_H__780A0A37_E304_45A6_919C_EA734A79A3A0__INCLUDED_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"

class CLoadDrawMap  
{
public:
	typedef nuBOOL(*DM_InitDrawMapProc)(PAPISTRUCTADDR, POPENRSAPI);
	typedef nuVOID(*DM_FreeDrawMapProc)();

public:
	CLoadDrawMap();
	virtual ~CLoadDrawMap();

	nuBOOL  InitModule(PAPISTRUCTADDR pApiAddr, POPENRSAPI pRsApi);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADDRAWMAP_H__780A0A37_E304_45A6_919C_EA734A79A3A0__INCLUDED_)
