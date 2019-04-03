// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LoadMathTool.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/LoadMathTool.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADMATHTOOL_H__CBD3DA70_4B32_44EE_8F00_9FECE3256B17__INCLUDED_)
#define AFX_LOADMATHTOOL_H__CBD3DA70_4B32_44EE_8F00_9FECE3256B17__INCLUDED_

#include "NuroDefine.h"
#include "NuroNaviCtrlConfig.h"

class CLoadMathTool  
{
public:
	typedef nuBOOL(*MT_InitMathToolProc)(nuPVOID, nuPVOID, nuPVOID, nuPVOID);
	typedef nuVOID(*MT_FreeMathToolProc)();

public:
	CLoadMathTool();
	virtual ~CLoadMathTool();

	nuBOOL  InitModule(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpInFS, nuPVOID lpOutMT);
	nuVOID  UnLoadModule();

protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADMATHTOOL_H__CBD3DA70_4B32_44EE_8F00_9FECE3256B17__INCLUDED_)
