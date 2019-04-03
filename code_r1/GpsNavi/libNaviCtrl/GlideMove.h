// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: GlideMove.h,v 1.3 2010/09/03 07:01:46 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/09/03 07:01:46 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libNaviCtrl/GlideMove.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLIDEMOVE_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_)
#define AFX_GLIDEMOVE_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "NuroEngineStructs.h"
#include "NuroConstDefined.h"

class CGlideMove  
{
public:
	CGlideMove();
	virtual ~CGlideMove();
	nuBOOL Initialzie(class CNaviThread* pNaviThd);
	nuVOID Free();

	nuBOOL StartMove(nuBYTE nDirect);
	nuBOOL StopMove(nuBYTE nDirect);
	nuBOOL MoveProc(PACTIONSTATE pActionState);
	nuBOOL IsMoving();

protected:
	nuBOOL ColNextPageCenter(nuBYTE nDirect, PNUROPOINT pPoint);
	nuBOOL ColNowPageCenter(nuBYTE nDirect, PNUROPOINT pPoint);
	nuBOOL CompositeBmp(nuBYTE nDirt, nuLONG nLen);

protected:
	nuBYTE              m_nNewMoveDirt;
	nuBYTE              m_nOldMoveDirt;
	nuWORD              m_nMaxStep;
	nuWORD              m_nStep;
	nuWORD              m_nLenMoved;
	nuWORD              m_nMaxLen;
	class CNaviThread*	m_pNaviThd;
	nuroPOINT           m_ptScreen;
	nuroPOINT           m_ptMap;
	nuWORD              m_nStartDis;
	nuWORD              m_nTimesToSleep;
	nuWORD              m_nSleepTime;
};

#endif // !defined(AFX_GLIDEMOVE_H__33BB0AFE_C664_4891_9594_97796782DF99__INCLUDED_)
