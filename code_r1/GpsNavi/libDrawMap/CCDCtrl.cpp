// CCDCtrl.cpp: implementation of the CCCDCtrl class.
//
//////////////////////////////////////////////////////////////////////
#include "NuroClasses.h"
#include "nuTTSdefine.h"
#include "CCDCtrl.h"
#include "libDrawMap.h"
#include "NuroConstDefined.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _CCD_BMP_FILE				nuTEXT("Media\\pic\\ccd.bmp")
CCCDCtrl::CCCDCtrl()
{
	m_pCcdData = NULL;
	m_nNowCCDNum = 0;
}

CCCDCtrl::~CCCDCtrl()
{
	Free();
}

nuBOOL CCCDCtrl::Initialize()
{
	nuMemset(m_pCCDPlayed, 0, sizeof(m_pCCDPlayed));
	nuMemset(&m_bmpCCD, 0, sizeof(NURO_BMP));
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, g_pDMGdata->pTsPath);
	nuTcscat(tsFile, _CCD_BMP_FILE);
	g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpCCD, tsFile);
	m_bmpCCD.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
	return nuTRUE;
}

nuVOID CCCDCtrl::Free()
{
	g_pDMLibGDI->GdiDelNuroBMP(&m_bmpCCD);
	nuMemset(&m_bmpCCD, 0, sizeof(NURO_BMP));
}

nuBOOL CCCDCtrl::LoadCCDData()
{
	nuroRECT	rtRange = {0};
	rtRange.left	= g_pDMGdata->carInfo.ptCarIcon.x - CCD_CHECK_DISTANCE;
	rtRange.right	= g_pDMGdata->carInfo.ptCarIcon.x + CCD_CHECK_DISTANCE;
	rtRange.top		= g_pDMGdata->carInfo.ptCarIcon.y - CCD_CHECK_DISTANCE;
	rtRange.bottom	= g_pDMGdata->carInfo.ptCarIcon.y + CCD_CHECK_DISTANCE;
	m_pCcdData = (PCCDDATA)g_pDMLibFS->FS_GetCCDData(rtRange);
	if( m_pCcdData == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CCCDCtrl::CheckCCDData()
{
	if( m_pCcdData == NULL )
	{
		return nuFALSE;
	}
	nuroRECT	rtRange = {0};
	nuroPOINT	ptList[4] = {0};
	nuroPOINT	point  = {0};
	nuSHORT nAngle =0;
	
	nuLONG dx = (nuLONG)(CCD_CHECK_DISTANCE * nuCos(g_pDMGdata->carInfo.nCarAngle));
	nuLONG dy = (nuLONG)(CCD_CHECK_DISTANCE * nuSin(g_pDMGdata->carInfo.nCarAngle));
	nuLONG Dis = (nuLONG)(CCD_CHECK_DISTANCE * nuTan(CCD_CHECK_ANGLE));
	Dis = NURO_ABS(Dis);
	ptList[3].x = g_pDMGdata->carInfo.ptCarIcon.x + dx;
	ptList[3].y = g_pDMGdata->carInfo.ptCarIcon.y + dy;
	//
	ptList[0].x = g_pDMGdata->carInfo.ptCarIcon.x + Dis * dy / CCD_CHECK_DISTANCE;
	ptList[0].y = g_pDMGdata->carInfo.ptCarIcon.y - Dis * dx / CCD_CHECK_DISTANCE;
	ptList[1].x = g_pDMGdata->carInfo.ptCarIcon.x - Dis * dy / CCD_CHECK_DISTANCE;
	ptList[1].y = g_pDMGdata->carInfo.ptCarIcon.y + Dis * dx / CCD_CHECK_DISTANCE;
	ptList[2].x = ptList[3].x - Dis * dy / CCD_CHECK_DISTANCE;
	ptList[2].y = ptList[3].y + Dis * dx / CCD_CHECK_DISTANCE;
	ptList[3].x = ptList[3].x + Dis * dy / CCD_CHECK_DISTANCE;
	ptList[3].y = ptList[3].y - Dis * dx / CCD_CHECK_DISTANCE;
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
	RemoveCCDOut(rtRange);
	++m_nMinTimePlayed;
	/*test*//*test
	nuroPOINT ptTest[4];
	ptTest[0].x = ptTest[3].x = rtRange.left;
	ptTest[0].y = ptTest[1].y = rtRange.top;
	ptTest[1].x = ptTest[2].x = rtRange.right;
	ptTest[2].y = ptTest[3].y = rtRange.bottom;
	g_pDMLibMT->MT_MapToBmp( ptTest[0].x, ptTest[0].y, &ptTest[0].x, &ptTest[0].y);
	g_pDMLibMT->MT_MapToBmp( ptTest[1].x, ptTest[1].y, &ptTest[1].x, &ptTest[1].y);
	g_pDMLibMT->MT_MapToBmp( ptTest[2].x, ptTest[2].y, &ptTest[2].x, &ptTest[2].y);
	g_pDMLibMT->MT_MapToBmp( ptTest[3].x, ptTest[3].y, &ptTest[3].x, &ptTest[3].y);
	g_pDMLibGDI->GdiPolygon( ptTest, 4);
	*/
	for(nuWORD i = 0; i < m_pCcdData->nCCDBlockCount; i++)
	{
//		MessageBoxW(NULL, L"CCD1", L"", 0);
		if( m_pCcdData->pCCDBlockList[i].ppCCDNodeList  == NULL ||
			*m_pCcdData->pCCDBlockList[i].ppCCDNodeList	== NULL )
		{
			continue;
		}
//		MessageBoxW(NULL, L"CCD2", L"", 0);
		for(nuDWORD j = 0; j < m_pCcdData->pCCDBlockList[i].nCCDCount; j++)
		{
			point.x = (*m_pCcdData->pCCDBlockList[i].ppCCDNodeList)[j].x;
			point.y = (*m_pCcdData->pCCDBlockList[i].ppCCDNodeList)[j].y;
			/*test*//*test
			nuroPOINT ptTmp;
			g_pDMLibMT->MT_MapToBmp( point.x, point.y, &ptTmp.x, &ptTmp.y);
			g_pDMLibGDI->GdiEllipse( ptTmp.x - 5, 
									 ptTmp.y - 5,
									 ptTmp.x + 5,
									 ptTmp.y + 5 );
			*/
			if( !nuPtInRect(point, rtRange) )
			{
				continue;
			}
			if( IsInList(point.x, point.y) )
			{
				continue;
			}
			//
			dx = point.x - g_pDMGdata->carInfo.ptCarIcon.x;
			dy = point.y - g_pDMGdata->carInfo.ptCarIcon.y;
			Dis = (nuLONG)nuSqrt(dx*dx + dy*dy);
			if( Dis > CCD_CHECK_DISTANCE )
			{
				continue;
			}
			nAngle = (((nuSHORT)((nuAtan2(dy, dx) * 180 / PI) + 360 )) % 360);
			nAngle = nAngle - g_pDMGdata->carInfo.nCarAngle;
			nAngle = NURO_ABS(nAngle) % 360;
			if( nAngle > 180 )
			{
				nAngle = 360 - nAngle;
			}
			if( nAngle < CCD_CHECK_ANGLE )
			{
				AddACCD(point.x, point.y);
				//Playing ccd Voice
				/**/
				nuWCHAR wsCCDname[100] = {0};
				wsCCDname[0] = NULL;
				g_pDMLibFS->FS_GetCCDName(m_pCcdData->pCCDBlockList[i].nDwIdx, 
					(*m_pCcdData->pCCDBlockList[i].ppCCDNodeList)[j].nNameAddr, 
					wsCCDname, 
					100*sizeof(nuWCHAR) );
//				MessageBoxW(NULL, wsCCDname, L"", 0);
				if( m_nMinTimePlayed >= CCD_MIN_VOICE_PLAYED )
				{
					m_nMinTimePlayed = 0;
					//g_pDMGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_CCDFIND, nuWcslen(wsCCDname), wsCCDname);
				}
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}
/*
nuBOOL CCCDCtrl::CCDVoice()
{
	nuroRECT	rtRange;
	nuroPOINT	ptList[4], point;
	nuSHORT nAngle;
	nuLONG dx = (nuLONG)(CCD_CHECK_DISTANCE * nuCos(g_pDMGdata->carInfo.nCarAngle));
	nuLONG dy = (nuLONG)(CCD_CHECK_DISTANCE * nuSin(g_pDMGdata->carInfo.nCarAngle));
	nuLONG Dis = (nuLONG)(CCD_CHECK_DISTANCE * nuTan(CCD_CHECK_ANGLE));
	Dis = NURO_ABS(Dis);
	ptList[3].x = g_pDMGdata->carInfo.ptCarIcon.x + dx;
	ptList[3].y = g_pDMGdata->carInfo.ptCarIcon.y + dy;
	//
	ptList[0].x = g_pDMGdata->carInfo.ptCarIcon.x + Dis * dy / CCD_CHECK_DISTANCE;
	ptList[0].y = g_pDMGdata->carInfo.ptCarIcon.y - Dis * dx / CCD_CHECK_DISTANCE;
	ptList[1].x = g_pDMGdata->carInfo.ptCarIcon.x - Dis * dy / CCD_CHECK_DISTANCE;
	ptList[1].y = g_pDMGdata->carInfo.ptCarIcon.y + Dis * dx / CCD_CHECK_DISTANCE;
	ptList[2].x = ptList[3].x - Dis * dy / CCD_CHECK_DISTANCE;
	ptList[2].y = ptList[3].y + Dis * dx / CCD_CHECK_DISTANCE;
	ptList[3].x = ptList[3].x + Dis * dy / CCD_CHECK_DISTANCE;
	ptList[3].y = ptList[3].y - Dis * dx / CCD_CHECK_DISTANCE;
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
	m_pCcdData = (PCCDDATA)g_pDMLibMM->FS_GetCCDData(rtRange);
	if( m_pCcdData == NULL )
	{
		return nuFALSE;
	}
	RemoveCCDOut(rtRange);
	for(nuWORD i = 0; i < m_pCcdData->nCCDBlockCount; i++)
	{
		if( m_pCcdData->pCCDBlockList[i].ppCCDNodeList  == NULL ||
			*m_pCcdData->pCCDBlockList[i].ppCCDNodeList	== NULL )
		{
			continue;
		}
		for(nuDWORD j = 0; j < m_pCcdData->pCCDBlockList[i].nCCDCount; j++)
		{
			point.x = (*m_pCcdData->pCCDBlockList[i].ppCCDNodeList)[j].x;
			point.y = (*m_pCcdData->pCCDBlockList[i].ppCCDNodeList)[j].y;
			if( !nuPtInRect(point, rtRange) )
			{
				continue;
			}
			if( IsInList(point.x, point.y) )
			{
				continue;
			}
			//
			dx = point.x - g_pDMGdata->carInfo.ptCarIcon.x;
			dy = point.y - g_pDMGdata->carInfo.ptCarIcon.y;
			Dis = (nuLONG)nuSqrt(dx*dx + dy*dy);
			if( Dis > CCD_CHECK_DISTANCE )
			{
				continue;
			}
			nAngle = ( (nuSHORT)(nuAtan2(dy, dx) * 180 / PI) + 360 ) % 360;
			nAngle = nAngle - g_pDMGdata->carInfo.nCarAngle;
			nAngle = NURO_ABS(nAngle) % 360;
			if( nAngle > 180 )
			{
				nAngle = 360 - nAngle;
			}
			if( nAngle < CCD_CHECK_ANGLE )
			{
				AddACCD(point.x, point.y);
				//Playing ccd Voice
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}
*/
nuBOOL CCCDCtrl::IsInList(nuLONG x, nuLONG y)
{
	for(nuINT i = 0; i < CCD_MAX_NUM_PLAYED; i++)
	{
		if( m_pCCDPlayed[i].bUsed )
		{
			if( m_pCCDPlayed[i].point.x == x && m_pCCDPlayed[i].point.y == y )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuBOOL CCCDCtrl::AddACCD(nuLONG x, nuLONG y)
{
	nuINT iNotUsed = 0;
	for(nuINT i = 0; i < CCD_MAX_NUM_PLAYED; i++)
	{
		if( !m_pCCDPlayed[i].bUsed )
		{
			iNotUsed = i;
			break;
		}
	}
	m_pCCDPlayed[iNotUsed].bUsed	= 1;
	m_pCCDPlayed[iNotUsed].point.x	= x;
	m_pCCDPlayed[iNotUsed].point.y	= y;
	++m_nNowCCDNum;
	return nuTRUE;
}

nuBOOL CCCDCtrl::RemoveCCDOut(const nuroRECT &rtRange)
{
	for(nuINT i = 0; i < CCD_MAX_NUM_PLAYED; i++)
	{
		if( m_pCCDPlayed[i].bUsed )
		{
			if( !nuPtInRect(m_pCCDPlayed[i].point, rtRange) )
			{
				--m_nNowCCDNum;
				m_pCCDPlayed[i].bUsed = 0;
			}
		}
	}
	if( m_nNowCCDNum == 0 )
	{
		m_nMinTimePlayed = CCD_MIN_VOICE_PLAYED;
	}
	return nuTRUE;
}

nuBOOL CCCDCtrl::DrawCCDPlayed()
{
	if( !m_bmpCCD.bmpHasLoad )
	{
		return nuFALSE;
	}
	nuroRECT rtScreen = {0};
	rtScreen.left	= 0;
	rtScreen.top	= 0;
	rtScreen.right	= rtScreen.left + g_pDMGdata->transfData.nBmpWidth;
	rtScreen.bottom = rtScreen.top + g_pDMGdata->transfData.nBmpHeight;
	/*
	if( g_pDMGdata->uiSetData.b3DMode )
	{
		rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
	}
	*/
	nuroPOINT point = {0};
	for(nuINT i = 0; i < CCD_MAX_NUM_PLAYED; ++i)
	{
		if( !m_pCCDPlayed[i].bUsed ) 
		{
			continue;
		}
		g_pDMLibMT->MT_MapToBmp(m_pCCDPlayed[i].point.x, m_pCCDPlayed[i].point.y, &point.x, &point.y);
		if( !nuPtInRect(point, rtScreen) )
		{
			continue;
		}
		if( g_pDMGdata->uiSetData.b3DMode )
		{
			g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
		}
		g_pDMLibGDI->GdiDrawBMP(point.x - m_bmpCCD.bmpWidth/2,
								point.y - m_bmpCCD.bmpHeight, 
								m_bmpCCD.bmpWidth,
								m_bmpCCD.bmpHeight,
								&m_bmpCCD,
								0,
								0 );
	}
	return nuTRUE;
}
