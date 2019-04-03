// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: TurningDraw.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/TurningDraw.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TURNINGDRAW_H__B7AB23B7_DD19_4BFD_8840_A40D88B625CA__INCLUDED_)
#define AFX_TURNINGDRAW_H__B7AB23B7_DD19_4BFD_8840_A40D88B625CA__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

class CTurningDraw 
{
public:
	CTurningDraw();
	virtual ~CTurningDraw();
	nuBOOL  Initialize(class CNaviThread* pNaviThd);
	nuVOID  Free();
	nuVOID  SetTurnDraw(PACTIONSTATE pAcState, nuSHORT nMapAngle = 90);

public:
	nuUINT TurningProc(PACTIONSTATE pAcState);
	nuUINT TurningProc(PACTIONSTATE pAcState, nuSHORT nNowAngle, nuSHORT nDesAngle);

protected:
	class CNaviThread*	m_pNaviThd;
	nuSHORT				m_nOldMapAngle;
	nuSHORT				m_nNewMapAngle;
};

#endif // !defined(AFX_TURNINGDRAW_H__B7AB23B7_DD19_4BFD_8840_A40D88B625CA__INCLUDED_)
