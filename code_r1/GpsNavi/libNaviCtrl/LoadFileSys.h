// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadFileSys.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadFileSys.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADFILESYS_H__9BE93689_EECC_4BEF_AB83_6D8664C16B0B__INCLUDED_)
#define AFX_LOADFILESYS_H__9BE93689_EECC_4BEF_AB83_6D8664C16B0B__INCLUDED_

#include "NuroDefine.h"
#include "NuroNaviCtrlConfig.h"

class CLoadFileSys
{
public:
	typedef nuBOOL (*FS_InitFileSystemProc)(nuPVOID, nuPVOID, nuPVOID);
	typedef nuVOID (*FS_FreeFileSystemProc)();

public:
	CLoadFileSys();
	virtual ~CLoadFileSys();
	
	nuBOOL  InitModule(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpOutFs);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADFILESYS_H__9BE93689_EECC_4BEF_AB83_6D8664C16B0B__INCLUDED_)
