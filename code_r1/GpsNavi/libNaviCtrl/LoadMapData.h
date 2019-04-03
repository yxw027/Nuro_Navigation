// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMapData.h,v 1.1 2009/09/11 06:56:56 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/09/11 06:56:56 $
// $Locker:  $
// $Revision: 1.1 $
// $Source: /home/nuCode/libNaviCtrl/LoadMapData.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADMAPDATA_H__7A5E3EFB_F16D_4D32_84B8_325641B05FBB__INCLUDED_)
#define AFX_LOADMAPDATA_H__7A5E3EFB_F16D_4D32_84B8_325641B05FBB__INCLUDED_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"

class CLoadMapData  
{
public:
	typedef bool(*MD_InitMapDataProc)(PAPISTRUCTADDR pApiAddr);
	typedef void(*MD_FreeMapDataProc)();
public:
	CLoadMapData();
	virtual ~CLoadMapData();
	
	bool  InitModule(PAPISTRUCTADDR pApiAddr);
	void  UnLoadModule();
protected:
	nuHINSTANCE	m_hInst;

};

#endif // !defined(AFX_LOADMAPDATA_H__7A5E3EFB_F16D_4D32_84B8_325641B05FBB__INCLUDED_)
