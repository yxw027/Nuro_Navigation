// DrawKilometerDL.cpp: implementation of the CDrawKilometerDL class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawKilometerDL.h"
#include "NuroModuleApiStruct.h"
#include "libDrawMap.h"
#include "NuroMethod/libMethods.h"
#ifdef DANIEL_MEMORYDETECT
#include "DETECT_MEMORY.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern PGDIAPI		  g_pDMLibGDI;
extern PFILESYSAPI    g_pDMLibFS;
extern PMATHTOOLAPI   g_pDMLibMT;

CDrawKilometerDL::CDrawKilometerDL()
{
	nuMemset(&m_bmpKilo, NULL, sizeof(NURO_BMP));
	m_pKmDataList = NULL;
	m_nKmCount    = 0;
	m_byIconType  = 0;
	m_Color       = 0;
	m_bUISet      = nuFALSE;
	nuMemset(&m_LogFont, NULL, sizeof(NUROFONT));
	//
	Initialize();
}

CDrawKilometerDL::~CDrawKilometerDL()
{

}

nuBOOL CDrawKilometerDL::Initialize()
{
	m_nKmCount = 0;
	m_pKmDataList = new KMDATA[_MAX_KILO_ICON_COUNT_];
	if( NULL == m_pKmDataList )
	{
#ifdef MEMORY_DEBUG
		vWriteDebug("DrawKilometer.cpp 45, m_pKmDataList", sizeof(KMDATA) * _MAX_KILO_ICON_COUNT_);
#endif
		return nuFALSE;
	}
	m_nTickCount = 0;
	m_wsKMName[0] = 0;;
	m_lKiloNum = 0;
	m_dAddKiloNum = 0;
	m_lRoadidx = -1;
	m_lBlockidx = -1;
	return nuTRUE;
}

nuVOID CDrawKilometerDL::Free()
{
	m_nKmCount = 0;
	if( NULL != m_pKmDataList)
	{
		delete[] m_pKmDataList;
		m_pKmDataList = NULL;
	}
	if( m_bmpKilo.pBmpBuff )
	{
		g_pDMLibGDI->GdiDelNuroBMP(&m_bmpKilo);
		m_bmpKilo.pBmpBuff = NULL;
	}
}

nuUINT CDrawKilometerDL::ResetKmList()
{
	m_nKmCount = 0;
	return 1;
}

nuUINT CDrawKilometerDL::LoadKiloIcon(nuBYTE byIconType)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, g_pDMLibFS->pGdata->pTsPath);
	nuTcscat(tsFile, _KILO_BMP_FILE_PATH_);
	if( !byIconType )
	{
		nuTcscat(tsFile, nuTEXT("NaviKing\\kilo.bmp"));
	}
	else
	{
		nuTcscat(tsFile, nuTEXT("BNaviKing\\kilo.bmp"));
	}
	if( !g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpKilo, tsFile) )
	{
		return 0;
	}
	return 1;
}

nuBOOL CDrawKilometerDL::AddKilometers(nuDWORD nBlockIdx)
{
	if( m_nKmCount >= 20 )
	{
		return nuFALSE;
	}
	nuLONG nCount = 0;
	KMInfoData   *m_pKMData = g_pDMLibFS->FS_GetKMDataDL(nBlockIdx, nCount);
	if(m_pKMData == NULL || nCount <= 0)
	{
		return nuFALSE;
	}
	KMDATA TempKmData = {0};
	nuINT i = 0;
	for( i = 0; i < nCount; i++ )
	{
		if( m_pKMData[i].KmName )
		{
			nuroPOINT ptKilo = {0};
			nuroRECT rtScreen= {0};
			nuBYTE byMapType = 0;
			nuBOOL b3Dmode =nuFALSE;
			if( byMapType == 0 )
			{
				rtScreen.left	= g_pDMGdata->transfData.nBmpLTx;
				rtScreen.top	= g_pDMGdata->transfData.nBmpLTy;
				rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth;
				rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight;
				b3Dmode = g_pDMGdata->uiSetData.b3DMode;
				if( b3Dmode )
				{
					rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
				}
			}
			else//Zoom map
			{
				rtScreen.left	= 0;
				rtScreen.top	= 0;
				rtScreen.right	= g_pDMGdata->zoomScreenData.nZoomBmpWidth;
				rtScreen.bottom	= g_pDMGdata->zoomScreenData.nZoomBmpHeight;
				b3Dmode = g_pDMGdata->zoomScreenData.b3DMode;
				if( b3Dmode )
				{
					rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
				}
			}
			g_pDMLibMT->MT_MapToBmp(m_pKMData[i].X ,m_pKMData[i].Y, &ptKilo.x, &ptKilo.y);
			if( ptKilo.y > rtScreen.bottom )
			{
				continue;
			}
			if( b3Dmode )
			{
				g_pDMLibMT->MT_Bmp2Dto3D(ptKilo.x, ptKilo.y, byMapType);
			}
			if( !nuPointInRect(&ptKilo, &rtScreen) )
			{
				continue;
			}
			m_pKmDataList[m_nKmCount].nScreenX = ptKilo.x;
			m_pKmDataList[m_nKmCount].nScreenY = ptKilo.y;
			m_pKmDataList[m_nKmCount].nMapX = m_pKMData[i].X;
			m_pKmDataList[m_nKmCount].nMapY = m_pKMData[i].Y;
			m_pKmDataList[m_nKmCount].nDis = m_pKMData[i].KmName / 100;
			m_pKmDataList[m_nKmCount].nClass = m_pKMData[i].nClass;
			m_pKmDataList[m_nKmCount].nBlockidx = m_pKMData[i].lBlockIdx;
			m_pKmDataList[m_nKmCount].nRoadidx = m_pKMData[i].lRoadIdx;
			if( IsIconOverlaped(m_pKmDataList[m_nKmCount]) )
			{
				continue;
			}			
			m_nKmCount ++;
		}
	}
	return nuTRUE;
}

nuUINT CDrawKilometerDL::DrawKilometers(nuBYTE byIconType)
{
	if( 0 == m_nKmCount || NULL == m_pKmDataList )
	{
		return 0;
	}
	if( NULL == m_bmpKilo.pBmpBuff || byIconType != m_byIconType )
	{
		m_byIconType = byIconType;
		if( 0 == LoadKiloIcon(byIconType) )
		{
			return 0;
		}
	}
	if( !m_bUISet )
	{
		SetKiloFontColor(m_Color, m_LogFont, 0);
	}
	nuINT sx = 0, sy = 0;
	nuINT i = 0;
	for( i = 0; i < m_nKmCount; i++ )
	{
		sx = m_pKmDataList[i].nScreenX - m_bmpKilo.bmpWidth/2;
		sy = m_pKmDataList[i].nScreenY - m_bmpKilo.bmpHeight/2;
		g_pDMLibGDI->GdiDrawBMP(sx, sy, m_bmpKilo.bmpWidth, m_bmpKilo.bmpHeight, &m_bmpKilo, 0, 0);
		nuUINT nLen = 0;
		nuWCHAR wsDis[4] = {0};
		nuItow(m_pKmDataList[i].nDis, wsDis, 10);
		nLen = nuWcslen(wsDis);
		if( m_byIconType )
		{
			if( 1 == nLen )
			{
				sx = sx + 24;
			}
			else if( 2 == nLen )
			{
				sx = sx + 14;
			}
			else if( 3 == nLen )
			{
				sx = sx + 8;
			}
			sy = sy + 7;
		}
		else
		{
			sy += m_bmpKilo.bmpHeight/4;
			if( 1 == nLen )
			{
				sx = sx + m_bmpKilo.bmpWidth/3+2;
				sy -= 3;
			}
			else if( 2 == nLen )
			{
				sx = sx + m_bmpKilo.bmpWidth/3-2; 
				sy -= 3;
			}
			else if( 3 == nLen )
			{
				sx = sx + m_bmpKilo.bmpWidth/7;
				sy-= 3;
			}			
		}
		g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, wsDis, nLen, nuTRUE, m_Color);
	}
	m_nKmCount = 0;
	return 1;
}

nuBOOL CDrawKilometerDL::IsIconOverlaped(KMDATA &KmData)
{
	KMDATA tempKmData = {0};
	if( 0 == m_nKmCount || NULL == m_pKmDataList )
	{
		return nuFALSE;
	}
	else
	{
		for( nuINT i = 0; i < m_nKmCount; i++ )
		{
			if( NURO_ABS(KmData.nScreenX - m_pKmDataList[i].nScreenX) <= 60 &&
				NURO_ABS(KmData.nScreenY - m_pKmDataList[i].nScreenY) <= 40 )
			{
				//if(JudePriority(KmData.nBlockidx, m_pKmDataList[i].nRoadidx, KmData.nRoadidx, m_pKmDataList[i].nRoadidx))
				{
					m_pKmDataList[i] = KmData;
				}
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuUINT CDrawKilometerDL::SetKiloFontColor(nuCOLORREF col, NUROFONT nuLogFont, nuBOOL bUISet)
{
	if( bUISet )
	{
		m_bUISet = nuTRUE;
		m_Color = col;
		m_LogFont = nuLogFont;
	}
	else
	{
		m_bUISet = nuFALSE;
		//Set Font Color
		m_Color = nuRGB(255, 255, 255);	//°×É«	
		//Set Font Size
		if( m_byIconType )
		{
			m_LogFont.lfHeight	= 26;
		}
		else
		{
			m_LogFont.lfHeight	= 11;
			m_LogFont.lfWidth	= 1;
			m_LogFont.lfWeight  = 40;
		}
	}
	g_pDMLibGDI->GdiSetTextColor(m_Color);
	g_pDMLibGDI->GdiSetFont(&m_LogFont);

	return 1;
}

nuBOOL CDrawKilometerDL::GetNearKilo(nuLONG* pDis, nuWCHAR* pwsKilos)
{
	if( NULL == pDis || NULL == pwsKilos )
	{
		return nuFALSE;
	}
	nuLONG nDx = 0, nDy = 0, Index = -1;
	nuLONG KMCount = 0, Distance = 0;
	//KMInfoData *pKmData = NULL;
	if(g_pDMLibFS->pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
	{
		if(g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nRoadClass > 4)
		{
			m_wsKMName[0] = 0;
			return nuFALSE;
		}
		if(m_lRoadidx != g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nBlkIdx || m_lBlockidx != g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nRoadIdx)
		{
			m_pKmData = NULL;
			m_pKmData = g_pDMLibFS->FS_GetKMDataMapping(g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn, KMCount);
		}
	}
	else
	{
		if(g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nRoadClass > 4)
		{
			m_wsKMName[0] = 0;
			return nuFALSE;
		}
		if(m_lRoadidx != g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nBlkIdx || m_lBlockidx != g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nRoadIdx)
		{
			m_pKmData = NULL;
			m_pKmData = g_pDMLibFS->FS_GetKMDataMapping(g_pDMLibFS->pGdata->drawInfoMap.roadCarOn, KMCount);
		}
	}
	
	if(NULL != m_pKmData)
	{
		m_lRoadidx  = m_pKmData->lRoadIdx;
		m_lBlockidx = m_pKmData->lBlockIdx;
		m_dAddKiloNum = (nuDOUBLE)m_pKmData[0].KmName;
		m_lKiloNum = m_pKmData[0].KmName;
		nuItow((m_pKmData[0].KmName / 100), pwsKilos, 10);
		nuWcscpy(m_wsKMName, pwsKilos);
		return nuTRUE;		
	}
	if(0 != m_wsKMName[0])
	{ 
		/*if(m_nTickCount == 0)
		{
			m_nTickCount = nuGetTickCount();
			return nuFALSE;
		}
		nuDOUBLE dDisToKm = 0;
		nuLONG Tick = nuGetTickCount();
		if(g_pDMLibFS->pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
		{
			dDisToKm = g_pDMLibFS->pUserCfg->nSimSpeed * g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nLimitSpeed * (Tick - m_nTickCount);
		}
		else
		{
			dDisToKm = g_pDMLibFS->pGdata->carInfo.nShowSpeed * (Tick - m_nTickCount);
		}
		m_dAddKiloNum += dDisToKm / 3600;
		m_nTickCount = Tick;
		if((m_dAddKiloNum - m_lKiloNum) >= 100)
		{
			m_lKiloNum+=100;
		}
		nuItow((m_lKiloNum / 100), pwsKilos, 10);*/
		nuWcscpy(pwsKilos, m_wsKMName);
	}
	else
	{
		return nuFALSE;
	}
	return nuTRUE;
}
/*nuVOID CDrawKilometerDL::MappingKm(const nuLONG &KmCount, const NUROPOINT &ptCar, PKMDATA pShowKmData, 
								   KMInfoData *pFindKmData, nuLONG &Index, nuLONG &Dis, nuWCHAR *pwsKilos)
{
	nuLONG nDx = 0, nDy = 0, Distance = 0;
	//nDx = ptCar.x - pFindKmData[0].X;
	//nDy = ptCar.y - pFindKmData[0].Y;
	//Dis = nuSqrt(nDx * nDx + nDy * nDy);
	for(nuINT i = 0; i < KmCount; i++)
	{
		if(pFindKmData[i].nClass > 5)
		{
			continue;
		}
		nDx = ptCar.x - pFindKmData[i].X;
		nDy = ptCar.y - pFindKmData[i].Y;
		Distance = nuSqrt(nDx * nDx + nDy * nDy);
		if(Distance <= 50)
		{
			Index = i;
			Dis = Distance;
		}
	}
	if(Index >= 0)
	{
		m_dAddKiloNum = (nuDOUBLE)pFindKmData[Index].KmName;
		m_lKiloNum = pFindKmData[Index].KmName;
		nuItow((pFindKmData[Index].KmName / 100), pwsKilos, 10);
		nuWcscpy(m_wsKMName, pwsKilos);
	}
}*/

nuVOID CDrawKilometerDL::MappingKm(const nuLONG &KmCount, const NUROPOINT &ptCar, PKMDATA pShowKmData, 
								   KMInfoData *pFindKmData, nuLONG &Index, nuLONG &Dis, nuWCHAR *pwsKilos)
{
	nuLONG nDx = 0, nDy = 0, Distance = 0;
	nDx = ptCar.x - pFindKmData[0].X;
	nDy = ptCar.y - pFindKmData[0].Y;
	Dis = nuSqrt(nDx * nDx + nDy * nDy);
	Index = 0;
	for(nuINT i = 1; i < KmCount; i++)
	{
		if(pFindKmData[i].nClass > 5)
		{
			continue;
		}
		nDx = ptCar.x - pFindKmData[i].X;
		nDy = ptCar.y - pFindKmData[i].Y;
		Distance = nuSqrt(nDx * nDx + nDy * nDy);
		if(Dis >= Distance)
		{
			Index = i;
			Dis = Distance;
		}
	}
	if(Index >= 0)
	{
		//m_nKiloNum = pFindKmData[Index].KmName;
		nuItow((pFindKmData[Index].KmName / 100), pwsKilos, 10);
		nuWcscpy(m_wsKMName, pwsKilos);
	}
}

nuBOOL CDrawKilometerDL::JudePriority(nuLONG BlockIdx_1, nuLONG BLockIdx_2, nuLONG RoadIdx_1, nuLONG RoadIdx_2)
{
	if(g_pDMLibFS->pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION)
	{
		if(BlockIdx_1 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nBlkIdx )
		{
			if(RoadIdx_1 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nRoadIdx)
			{
				if(BLockIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nBlkIdx 
					&& RoadIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nRoadIdx)
				{
					return nuFALSE;// no change
				}
				else
				{
					return nuTRUE;
				}
			}	
			else if(BLockIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nBlkIdx)
			{
				if(RoadIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCenterOn.nRoadIdx)
				{
					return nuFALSE;
				}
			}
			else
			{
				return nuTRUE;
			}
		}
		else
		{
			return nuFALSE;
		}
	}
	else
	{
		if(BlockIdx_1 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nBlkIdx )
		{
			if(RoadIdx_1 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nRoadIdx)
			{
				if(BLockIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nBlkIdx 
					&& RoadIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nRoadIdx)
				{
					return nuFALSE;// no change
				}
				else
				{
					return nuTRUE;
				}
			}	
			else if(BLockIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nBlkIdx)
			{
				if(RoadIdx_2 == g_pDMLibFS->pGdata->drawInfoMap.roadCarOn.nRoadIdx)
				{
					return nuFALSE;
				}
			}
			else
			{
				return nuTRUE;
			}
		}
		else
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}