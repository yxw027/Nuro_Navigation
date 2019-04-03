// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: LogFileCtrlZ.h,v 1.2 2009/08/31 01:12:40 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/31 01:12:40 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/nuLoadDll/LogFileCtrlZ.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILECTRLZ_H__60D3C0CE_98C4_4DCD_85DF_38AE857D6EF1__INCLUDED_)
#define AFX_LOGFILECTRLZ_H__60D3C0CE_98C4_4DCD_85DF_38AE857D6EF1__INCLUDED_

#include "NuroDefine.h"

class CLogFileCtrlZ  
{
public:
	CLogFileCtrlZ();
	virtual ~CLogFileCtrlZ();

	nuVOID OpenLogFile(nuTCHAR *pTsFile);
	nuVOID CloseLogFile();
	nuVOID OutString(const nuTCHAR* pTsStr);
	nuVOID OutNumber(nuLONG nNum);
	nuVOID SaveTicks();
	nuVOID OutTicksPast();

	nuVOID ClearLofFile(nuTCHAR *pTsFile);

protected:
	nuFILE*	m_pLogFile;
	nuDWORD	m_nLastTicks;

};

#endif // !defined(AFX_LOGFILECTRLZ_H__60D3C0CE_98C4_4DCD_85DF_38AE857D6EF1__INCLUDED_)
