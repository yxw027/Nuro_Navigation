// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: CheckEEyesZ.cpp,v 1.4 2010/06/29 04:04:00 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/06/29 04:04:00 $
// $Locker:  $
// $Revision: 1.4 $
// $Source: /home/nuCode/libFileSystem/CheckEEyesZ.cpp,v $
//////////////////////////////////////////////////////////////////////////
#include "CheckEEyesZ.h"
#include "NuroConstDefined.h"
#include "nuTTSdefine.h"
#include "FileEEyesZ.h"
#include "NuroClasses.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define EEYE_X_BLK_VALUE(x)  ( x < 0 ) ? \
		((x + HALF_MAX_LONGITUDE) / EEYES_BLOCK_SIZE_X) : \
		(x / EEYES_BLOCK_SIZE_X + HALF_MAX_LONGITUDE / EEYES_BLOCK_SIZE_X)

#define EEYE_Y_BLK_VALUE(y)  ( y < 0 ) ? \
		((y + HALF_MAX_LATITUDE) / EEYES_BLOCK_SIZE_Y) : \
		(y/EEYES_BLOCK_SIZE_Y + HALF_MAX_LATITUDE/EEYES_BLOCK_SIZE_Y)

#define ANGLEDIFTO180(angle) angle = NURO_ABS(angle) % 360; \
		if( angle > 180 )	angle = 360 - angle

CCheckEEyesZ::CCheckEEyesZ()
{
	m_pGData = NULL;
}

CCheckEEyesZ::~CCheckEEyesZ()
{
	Free();
}

nuBOOL CCheckEEyesZ::Initialize(PGLOBALENGINEDATA	pGData)
{
	nuMemset(&m_eEyesData, 0, sizeof(m_eEyesData));
	nuMemset(m_pEEyesPly, 0, sizeof(m_pEEyesPly));
	m_pGData = pGData;
	m_nEEyesPlyCount = 0;
	m_nWsLimitNum = 0;
	m_nWsDisLimitNum = 0;
	return nuTRUE;
}

nuVOID CCheckEEyesZ::Free()
{
	RemoveEEyesBlkUnused();
}

nuBOOL CCheckEEyesZ::CheckEEyes(nuroPOINT point, nuLONG nDis, nuSHORT nAngle, PROADFOREEYES pRdForEEyes)
{
	if( !LoadEEyes(point, nDis, nAngle, pRdForEEyes->nDwIdx) )
	{
		return nuFALSE;
	}
	nuroRECT	rtRange = {0};
	rtRange.left	= point.x - nDis;
	rtRange.right	= point.x + nDis;
	rtRange.top		= point.y - nDis;
	rtRange.bottom	= point.y + nDis;
	RemoveEEyesOutRange(rtRange);
	ColJudgeRange(point, nDis, nAngle, rtRange);
	JudgeEEyes(rtRange, point, nDis, nAngle);
	return nuTRUE;
}

nuVOID CCheckEEyesZ::RemoveEEyesOutRange(nuroRECT& rtRange)
{
	for(nuINT i = 0; i < MAX_NUM_OF_EEYES_PLAYED; ++i )
	{
		if( m_pEEyesPly[i].bUsed &&
			!nuPtInRect(m_pEEyesPly[i].point, rtRange) )
		{
			m_pEEyesPly[i].bUsed = 0;
			--m_nEEyesPlyCount;
		}
	}
}

nuBOOL CCheckEEyesZ::JudgeEEyes(nuroRECT& rtRange, nuroPOINT point, nuLONG nDis, nuSHORT nAngle)
{
	E_EYE_BLOCK	blkEEye = {0};
	nuPBYTE pData = NULL;
	nuroPOINT ptEEye = {0};
	nuLONG dx = 0, dy = 0, Dis = 0;
	nuSHORT nAngleDif = 0, nAngle2 = 0;
	nuWCHAR wsTTsTxt[200] = {0};
	for( nuWORD i = 0; i < m_eEyesData.nEEyesBlkCount; ++i )
	{
		pData = (nuPBYTE)m_pMmApi->MM_GetDataMassAddr(m_eEyesData.pEEyesBlkList[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		ColEEyeBlock(pData, blkEEye);
		for( nuDWORD j = 0; j < blkEEye.pEEyeHead->nEEyesCount; ++j )
		{
			ptEEye.x = blkEEye.pEEyeNode[j].x;
			ptEEye.y = blkEEye.pEEyeNode[j].y;
			if( !nuPtInRect(ptEEye, rtRange) || 
				IsEEyeInPlyList(ptEEye) )
			{
				continue;
			}
			dx = ptEEye.x - point.x;
			dy = ptEEye.y - point.y;
			Dis = (nuLONG)nuSqrt(dx*dx + dy*dy);
			if( Dis > nDis )
			{
				continue;
			}
			nAngleDif = nAngle - (((nuSHORT)blkEEye.pEEyeNode[j].angle)<<1);
			ANGLEDIFTO180(nAngleDif);
			if( blkEEye.pEEyeNode[j].nCheckDir == 1 )
			{
				if( nAngleDif > MAX_ANGLE_OF_EEYES_CHECK )
				{
					continue;
				}
			}
			else if( blkEEye.pEEyeNode[j].nCheckDir == 2 )
			{
				if( nAngleDif < 180 - MAX_ANGLE_OF_EEYES_CHECK )
				{
					continue;
				}
			}
			else
			{
				if( nAngleDif < 180 - MAX_ANGLE_OF_EEYES_CHECK &&
					nAngleDif > MAX_ANGLE_OF_EEYES_CHECK )
				{
					continue;
				}
			}
			//
			/**/
			nAngle2 = (((nuSHORT)((nuAtan2(dy, dx) * 180 / PI) + 360 )) % 360);
			nAngleDif = nAngle2 - nAngle;
			ANGLEDIFTO180(nAngleDif);
			if( nAngleDif > MAX_ANGLE_OF_EEYES_CHECK )
			{
				continue;
			}
			AddEEye(ptEEye);
			wsTTsTxt[0] = 0;
			nuUINT nNum = CFileEEyesZ::GetEEyeTypeName(blkEEye.pEEyeNode[j].nEType, wsTTsTxt, sizeof(wsTTsTxt));
//			MessageBoxW(NULL, wsTTsTxt, L"", 0);
			if( blkEEye.pEEyeNode[j].nSpeed > 0 )
			{
				AddLimitSpeed(blkEEye.pEEyeNode[j].nSpeed*10, blkEEye.pEEyeNode[j].nEType,  &wsTTsTxt[nNum], 200 - nNum);
			}
//			MessageBoxW(NULL, wsTTsTxt, L"", 0);
			if( nNum )
			{
				//m_pGData->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_EEYESFIND, nNum, wsTTsTxt);
			}
			else
			{
				//m_pGData->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_EEYESFIND, 0, NULL);
			}
			
		}
	}
	return nuFALSE;
}

nuVOID CCheckEEyesZ::AddLimitSpeed(nuINT nSpeed, nuWORD nType, nuWCHAR *wsBuff, nuINT nWsNum)
{
	if( m_nWsLimitNum == 0 )
	{
		m_nWsLimitNum = (nuBYTE)GetConstStr(CONST_STR_LIMIT, m_wsLimit, 5);
	}
	/* @Zhikun 2009.08.25
	if( 0 == m_nWsDisLimitNum )
	{
		m_nWsDisLimitNum = (nuBYTE)GetConstStr(CONST_STR_DIS_LIMIT, m_wsDisLimit, 6);
	}
	*/
	nuWCHAR wsNum[10] = {0};
	if( EMG_ET_TYPE_CM != nType )
	{
		nuItow(nSpeed, wsNum, 10);
		nuWcsncat(wsBuff, m_wsLimit, nWsNum);
		nuWcsncat(wsBuff, wsNum, nWsNum-m_nWsLimitNum);
	}
}

nuBOOL CCheckEEyesZ::AddEEye(nuroPOINT& point)
{
	for(nuINT i = 0; i < MAX_NUM_OF_EEYES_PLAYED; ++i )
	{
		if( !m_pEEyesPly[i].bUsed )
		{
			m_pEEyesPly[i].bUsed	= 1;
			m_pEEyesPly[i].point	= point;
			++m_nEEyesPlyCount;
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuVOID CCheckEEyesZ::ColEEyeBlock(nuPBYTE pData, E_EYE_BLOCK& blkEEye)
{
	blkEEye.pEEyeHead = (PE_EYE_HEAD)pData;
	pData += sizeof(E_EYE_HEAD);
	blkEEye.pEEyeNode = (PE_EYE_NODE)pData;
}

nuBOOL CCheckEEyesZ::IsEEyeInPlyList(nuroPOINT& point)
{
	for (nuINT i = 0; i < MAX_NUM_OF_EEYES_PLAYED; ++i )
	{
		if( m_pEEyesPly[i].bUsed )
		{
			if ( point.x == m_pEEyesPly[i].point.x &&
				 point.y == m_pEEyesPly[i].point.y )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuBOOL CCheckEEyesZ::LoadEEyes(nuroPOINT point, nuLONG nDis, nuSHORT nAngle, nuWORD nDwIdx)
{
	nuroRECT	rtRange = {0};
	rtRange.left	= point.x - nDis;
	rtRange.right	= point.x + nDis;
	rtRange.top		= point.y - nDis;
	rtRange.bottom	= point.y + nDis;
	nuLONG xS = EEYE_X_BLK_VALUE(rtRange.left);
	nuLONG xE = EEYE_X_BLK_VALUE(rtRange.right);
	nuLONG yS = EEYE_Y_BLK_VALUE(rtRange.top);
	nuLONG yE = EEYE_Y_BLK_VALUE(rtRange.bottom);
	nuDWORD pListID[MAX_EEYES_BLOCK_COUNT];
	nuINT nIDCount = 0;
	m_eEyesData.nKeptBit = 0;
	for(nuLONG i = yS; i <= yE; ++i)
	{
		nuLONG yId = i * X_EEYES_BLOCK_COUNT;
		nuDWORD nId;
		for(nuLONG j = xS; j <= xE; ++j)
		{
			nId = j + yId;
			nuWORD k;
			for( k = 0; k < m_eEyesData.nEEyesBlkCount; ++k )
			{
				if( nId == m_eEyesData.pEEyesBlkList[k].nBlkID )
				{
					m_eEyesData.nKeptBit |= (1<<k);
					break;
				}
			}
			if( k == m_eEyesData.nEEyesBlkCount && nIDCount < MAX_EEYES_BLOCK_COUNT )
			{
				pListID[nIDCount++] = nId;
			}
		}
	}
	RemoveEEyesBlkUnused(nuFALSE);
	if( nIDCount > 0 )
	{
		ReadEEyesBlkUsed(pListID, nIDCount, nDwIdx);
	}
	return nuTRUE;
}

nuVOID CCheckEEyesZ::RemoveEEyesBlkUnused(nuBOOL bAll /* = nuTRUE */)
{
	if( NULL == m_pMmApi )
	{
		return;
	}
	nuWORD i = 0;
	for( i = 0; i < m_eEyesData.nEEyesBlkCount; ++i )
	{
		if( bAll || !(m_eEyesData.nKeptBit & (1<<i)) )
		{
			m_pMmApi->MM_RelDataMemMass(&m_eEyesData.pEEyesBlkList[i].nMemIdx);
		}
	}
	nuWORD nDel = 0;
	i = 0;
	while( i < m_eEyesData.nEEyesBlkCount )
	{
		if( m_eEyesData.pEEyesBlkList[i].nMemIdx == (nuWORD)(-1) )
		{
			for( nuWORD j = i + 1; j < m_eEyesData.nEEyesBlkCount; ++j )
			{
				m_eEyesData.pEEyesBlkList[j - 1] = m_eEyesData.pEEyesBlkList[j];
			}
			-- m_eEyesData.nEEyesBlkCount;
		}
		else
		{
			++i;
		}
	}
}

nuBOOL CCheckEEyesZ::ReadEEyesBlkUsed(nuDWORD *pListID, nuINT nCount, nuWORD nDwIdx)
{
	CFileEEyesZ fileEEyes;
	for(nuINT i = 0; i < nCount && m_eEyesData.nEEyesBlkCount < MAX_EEYES_BLOCK_COUNT; ++i)
	{
		m_eEyesData.pEEyesBlkList[m_eEyesData.nEEyesBlkCount].nBlkID = pListID[i];
		fileEEyes.ReadEEyeBlk(m_eEyesData.pEEyesBlkList[m_eEyesData.nEEyesBlkCount], nDwIdx);
		++m_eEyesData.nEEyesBlkCount;
	}
	fileEEyes.CloseFile();
	return nuTRUE;
}

nuDWORD CCheckEEyesZ::EEyesPointToBlkID(nuLONG x, nuLONG y)
{
	if( y < 0 )
	{
		y = (y + HALF_MAX_LATITUDE) / EEYES_BLOCK_SIZE_Y;
	}
	else
	{
		y = y / EEYES_BLOCK_SIZE_Y + HALF_MAX_LATITUDE / EEYES_BLOCK_SIZE_Y;
	}
	if( x < 0 )
	{
		x = (x + HALF_MAX_LONGITUDE) / EEYES_BLOCK_SIZE_X;
	}
	else
	{
		x = x / EEYES_BLOCK_SIZE_X + HALF_MAX_LONGITUDE / EEYES_BLOCK_SIZE_X;
	}
	return (x + y * X_EEYES_BLOCK_COUNT);
}

nuVOID CCheckEEyesZ::ColJudgeRange(nuroPOINT& point, nuLONG nDis, nuSHORT nAngle, nuroRECT& rtRange)
{
	nuroPOINT	ptList[4] = {0};
	nuLONG dx = (nuLONG)(nDis * nuCos(nAngle));
	nuLONG dy = (nuLONG)(nDis * nuSin(nAngle));
	nuLONG Dis = (nuLONG)(nDis * nuTan(MAX_ANGLE_OF_EEYES_CHECK));
	Dis = NURO_ABS(Dis);
	ptList[3].x = point.x + dx;
	ptList[3].y = point.y + dy;
	//
	ptList[0].x = point.x + Dis * dy / nDis;
	ptList[0].y = point.y - Dis * dx / nDis;
	ptList[1].x = point.x - Dis * dy / nDis;
	ptList[1].y = point.y + Dis * dx / nDis;
	ptList[2].x = ptList[3].x - Dis * dy / nDis;
	ptList[2].y = ptList[3].y + Dis * dx / nDis;
	ptList[3].x = ptList[3].x + Dis * dy / nDis;
	ptList[3].y = ptList[3].y - Dis * dx / nDis;
	//
	if( NURO_ABS( ptList[0].x - ptList[2].x ) >= NURO_ABS( ptList[1].x - ptList[3].x ) )
	{
		rtRange.left   = ptList[0].x;
		rtRange.right  = ptList[2].x;
		rtRange.top    = ptList[3].y;
		rtRange.bottom = ptList[1].y;
	}
	else
	{
		rtRange.left   = ptList[1].x;
		rtRange.right  = ptList[3].x;
		rtRange.top    = ptList[0].y;
		rtRange.bottom = ptList[2].y;
	}
	if( rtRange.left > rtRange.right )
	{
		Dis = rtRange.left;
		rtRange.left  = rtRange.right;
		rtRange.right = Dis;
	}
	if(  rtRange.top > rtRange.bottom )
	{
		Dis = rtRange.bottom;
		rtRange.bottom = rtRange.top;
		rtRange.top = Dis;
	}
}
