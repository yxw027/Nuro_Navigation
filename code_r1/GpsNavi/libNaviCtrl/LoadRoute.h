// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadRoute.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadRoute.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADROUTE_H__311E7444_3D0B_4C46_B0E9_C6D95D08BD3E__INCLUDED_)
#define AFX_LOADROUTE_H__311E7444_3D0B_4C46_B0E9_C6D95D08BD3E__INCLUDED_

#include "NuroDefine.h"
#include "NuroNaviCtrlConfig.h"

class CLoadRoute  
{
public:
	typedef nuBOOL(*RT_InitRouteProc)(nuPVOID, nuPVOID, nuPVOID);
	typedef nuVOID(*RT_FreeRouteProc)();
public:
	CLoadRoute();
	virtual ~CLoadRoute();

	nuBOOL  InitModule(nuPVOID lpInMM, nuPVOID lpInFS, nuPVOID lpOutRT);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADROUTE_H__311E7444_3D0B_4C46_B0E9_C6D95D08BD3E__INCLUDED_)
