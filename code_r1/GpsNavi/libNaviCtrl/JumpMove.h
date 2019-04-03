// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: JumpMove.h,v 1.4 2010/09/03 07:01:46 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/09/03 07:01:46 $
// $Locker:  $
// $Revision: 1.4 $
// $Source: /home/nuCode/libNaviCtrl/JumpMove.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JUMPMOVE_H__CB2A2C2F_9FC2_4BA2_91CA_41C53CF90D6B__INCLUDED_)
#define AFX_JUMPMOVE_H__CB2A2C2F_9FC2_4BA2_91CA_41C53CF90D6B__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "NuroEngineStructs.h"
#include "GApiNaviCtrl.h"

#ifdef _WIN32_WCE
#define MOVE_STEP_MIN					10
#else
#define MOVE_STEP_MIN					10
#endif

#define SLICE_MOVE_TIME					50
#define MIN_MOVE_SPEED					10
#define MAX_MOVE_SPEED					60

class CJumpMove  
{
public:
	CJumpMove();
	virtual ~CJumpMove();

	nuBOOL	Initialize(class CNaviThread* pNaviThd);
	nuVOID	Free();

	nuBOOL	MoveStart(nuBYTE nDirection);
	nuBOOL	MoveStop(nuBYTE nDirection);

	nuBOOL	JumpToPoint(PACTIONSTATE pActionState, nuLONG x, nuLONG y);
//===========================================SDK==================================//
	nuBOOL  ColJumpMoveEx(PACTIONSTATE pActionState, NUROPOINT ptScreen);
	nuBOOL	JumpToPointEx(PACTIONSTATE pActionState, nuLONG x, nuLONG y);
//===========================================SDK==================================//
	nuBOOL  DrawCenterPoint(PACTIONSTATE pActionState);
	nuBOOL	JumpMoveProc(PACTIONSTATE pActionState);
	nuBOOL	ColJumpMove(PACTIONSTATE pActionState);
	nuBOOL	IsMoving();
	nuBOOL  SetMoveStep(nuINT nStep);
	nuBOOL  BmpCopyEx2(PNURO_BMP pDesBmp, nuINT desx, nuINT desy, PNURO_BMP pSrcBmp, nuINT srcx, nuINT srcy,nuINT desw, nuINT desh, nuINT srcw, nuINT srch);
public:
	DrawMapIconProc m_pfSaveDrawData;
	nuBOOL          m_bJumpMoving;

protected:
	nuBOOL	ColMapPoint(NUROPOINT& point);
	nuINT	ColMoveTimes(nuLONG x, nuLONG y);
	nuBOOL	ColMapPointFromScreenCenter(nuLONG& x, nuLONG& y);
	//moving of 45
	nuBOOL	CreateMoveBmp(nuINT nScreenW, nuINT nScreenH);
	nuVOID	DeleteMoveBmp();
	nuBOOL	JumpMove2DProc(PACTIONSTATE pActionState);
	nuBOOL	ColNextBmpCenter(nuBYTE nDirect, nuroPOINT& point);

protected:
	nuBYTE	            m_nNewDirection;
	nuBYTE	            m_nOldDirection;
	nuWORD	            m_nHStep;
	nuWORD	            m_nVStep;
	nuWORD	            m_nHMoveStep;

	nuWORD	            m_nVMoveStep;

	nuWORD	            m_nMoveSpeed;
	nuWORD	            m_nDisToChangeSpeed;
	nuWORD	            m_nDisPast;
	nuDWORD	            m_nLastTicks;
	class CNaviThread*	m_pNaviThd;
	NURO_BMP	        m_bmpMove;
	nuLONG		        m_nJumpMoveType;
	
	nuWORD	            m_nSingleMoveSleepTime;
	nuWORD		        m_nTimesToSleep;
	nuWORD		        m_nGlidMoveWTimes;
	nuWORD		        m_nStartDis;
	nuWORD		        m_nJumpMoveStep;
	nuLONG              m_nJmpMoveSleepTime;
public:
	nuWORD		        m_nSingleMoveStep;
};

#endif // !defined(AFX_JUMPMOVE_H__CB2A2C2F_9FC2_4BA2_91CA_41C53CF90D6B__INCLUDED_)
