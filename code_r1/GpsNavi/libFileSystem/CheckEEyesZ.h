// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: CheckEEyesZ.h,v 1.3 2009/08/28 05:27:31 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 05:27:31 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libFileSystem/CheckEEyesZ.h,v $
///////////////////////////////////////////////////////////
#if !defined(AFX_CHECKEEYESZ_H__7E5B0665_461A_4005_9354_80C4F880FEC6__INCLUDED_)
#define AFX_CHECKEEYESZ_H__7E5B0665_461A_4005_9354_80C4F880FEC6__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"
#include "NuroEngineStructs.h"
#include "NuroModuleApiStruct.h"
#include "FileSystemBaseZ.h"

#define EEYES_BLOCK_SIZE_X						(25000*COORCONSTANT)
#define EEYES_BLOCK_SIZE_Y						(25000*COORCONSTANT)
#define X_EEYES_BLOCK_COUNT						1440

#define MAX_ANGLE_OF_EEYES_CHECK					30
class CCheckEEyesZ : public CFileSystemBaseZ
{
public:
	CCheckEEyesZ();
	virtual ~CCheckEEyesZ();

	nuBOOL  Initialize(PGLOBALENGINEDATA	pGData);
	nuVOID  Free();
	nuBOOL  CheckEEyes(nuroPOINT point, nuLONG nDis, nuSHORT nAngle, PROADFOREEYES pRdForEEyes);

protected:
	nuBOOL  LoadEEyes(nuroPOINT point, nuLONG nDis, nuSHORT nAngle, nuWORD nDwIdx);
	nuDWORD EEyesPointToBlkID(nuLONG x, nuLONG y);
	nuVOID	RemoveEEyesBlkUnused(nuBOOL bAll = nuTRUE);
	nuBOOL	ReadEEyesBlkUsed(nuDWORD *pListID, nuINT nCount, nuWORD nDwIdx);
	nuVOID	ColJudgeRange(nuroPOINT& point, nuLONG nDis, nuSHORT nAngle, nuroRECT& rtRange);
	nuVOID  RemoveEEyesOutRange(nuroRECT& rtRange);
	nuBOOL	JudgeEEyes(nuroRECT& rtRange, nuroPOINT point, nuLONG nDis, nuSHORT nAngle);
	nuVOID	ColEEyeBlock(nuPBYTE pData, E_EYE_BLOCK& blkEEye);
	nuBOOL	IsEEyeInPlyList(nuroPOINT& point);
	nuBOOL	AddEEye(nuroPOINT& point);
	nuVOID	AddLimitSpeed(nuINT nSpeed, nuWORD nType, nuWCHAR *wsBuff, nuINT nWsNum);

public:
	EEYES_PLAYED		m_pEEyesPly[MAX_NUM_OF_EEYES_PLAYED];

protected:
	EEYES_DATA			m_eEyesData;
	PGLOBALENGINEDATA	m_pGData;
	nuBYTE				m_nEEyesPlyCount;
	nuBYTE				m_nWsLimitNum;
	nuWCHAR				m_wsLimit[5];
	nuBYTE				m_nWsDisLimitNum;
	nuWCHAR				m_wsDisLimit[6];
};

#endif // !defined(AFX_CHECKEEYESZ_H__7E5B0665_461A_4005_9354_80C4F880FEC6__INCLUDED_)
