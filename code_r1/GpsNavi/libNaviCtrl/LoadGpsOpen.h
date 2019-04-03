// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadGpsOpen.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadGpsOpen.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADGPSOPEN_H__18D47D9A_BF72_4403_AC11_8CD9437D03F8__INCLUDED_)
#define AFX_LOADGPSOPEN_H__18D47D9A_BF72_4403_AC11_8CD9437D03F8__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"

class CLoadGpsOpen  
{
public:
	typedef nuBOOL(*GPS_InitGpsOpenProc)(PAPISTRUCTADDR);
	typedef nuVOID(*GPS_FreeGpsOpenProc)();

public:
	CLoadGpsOpen();
	virtual ~CLoadGpsOpen();

	nuBOOL LoadModule(PAPISTRUCTADDR pApiAddr);
	nuVOID UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADGPSOPEN_H__18D47D9A_BF72_4403_AC11_8CD9437D03F8__INCLUDED_)
