// NaviData.cpp: implementation of the CNaviData class.
//
//////////////////////////////////////////////////////////////////////
#include "NuroNaviConst.h"
#include "NuroModuleApiStruct.h"
#include "NaviData.h"
#include "NuroClasses.h"

//-----------------SDK--------------------//
#include "NURO_DEF.h"
//-----------------SDK--------------------//
#define SameRoad
#define  SAMEROADLEN   20

//#include "LoadFS.h"

//#include "LoadMT.h"
//#include "windows.h"
#define  JUDGELEN		 40
#define  ARROWKEEPLEN    100
#define  DRAWARROWLEN    10
#define  SCALEBASE		 32
#define  ZOOMLEN		 32
#define  GENLEN			 16
#define  SPEEDADDTIMES   10

#define   CROSSLEN		 150
#define   LENGETPT	     200 //??È¡?Âž?Â·?Úµ?Ê±?Ä¹æ¶?Ä³???

#define   LEN_COMBINE    45
extern PMATHTOOLAPI			g_pInMtApi;
extern PFILESYSAPI			g_pInFsApi;
extern PMEMMGRAPI			g_pInMmApi;
extern PROUTEAPI			g_pInRTApi;
//extern PGDIAPI			g_pInGDIApi;
#ifdef ANDROID
	#define TIME 5	
	#define ANDROID_ROYAL_SLEEP
#endif

#ifdef ANDROID_ROYAL_SLEEP
nuLONG g_nSleepChangNCCNameCount = 0;
nuLONG g_nSleepNaviInfoCount     = 0;
nuLONG g_nSleepCombinedSeachCount = 0;
#define ROYALTEK_SLEEP 5
#endif

typedef union tagUNIONNAVILINE
{
	ZOOMNAVILINE *pZoomNaviLine;
	NAVILINEPOINT *pNaviLinePoint;
}UNIONNAVILINE,*PUNIONNAVILINE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define VOICE_CODE_ROAD_ASY		1
CNaviData::CNaviData()
{
	//Initialize();
	if( !nuReadConfigValue( "HIGHESTISPEED", &m_nHighSpeed ) )
	{
		m_nHighSpeed = 120;
	}
	if( !nuReadConfigValue( "SLOWESTISPEED", &m_nSlowSpeed ) )
	{
		m_nSlowSpeed = 50;
	}
	if ( !nuReadConfigValue( "NAVILINETYPE", &m_nNaviLineType ) )
	{
		m_nNaviLineType = 0;
	}
	if ( !nuReadConfigValue( "HIGHWAYEXTERN", &m_nHeighwayExtern ) )
	{
		m_nHeighwayExtern = 0;
	}
	if ( !nuReadConfigValue( "ARROWWIDTH1", &m_nArrowLineWidth1 ) )
	{
		m_nArrowLineWidth1 = 10;
	}
	if ( !nuReadConfigValue( "ARROWWIDTH2", &m_nArrowLineWidth2 ) )
	{
		m_nArrowLineWidth2 = 8;
	}
	if ( !nuReadConfigValue( "SHOWHIGHWAYPIC", &m_nShowHighWayPic ) )
	{
		m_nShowHighWayPic = 0;
	}
	if( !nuReadConfigValue( "GONAVI", &m_nGoNavi ))
	{
		m_nGoNavi = 0;
	}
	if ( !nuReadConfigValue( "BMPARROWLEN", &m_nBmpArrowLen ))
	{
		m_nBmpArrowLen = 78;
	}
	if ( !nuReadConfigValue( "SCALESHOWARROW", &m_nScalShowArrow ))
	{
		m_nScalShowArrow = 3;
	}
	if( !nuReadConfigValue("SAMEROADLEN", &m_nSameRoadLen) )
	{
		m_nSameRoadLen = 17;
	}
	if( !nuReadConfigValue("NAVI_LINE_RED", &m_nNaviLineRed) )
	{
		m_nNaviLineRed = 255;
	}
	if( !nuReadConfigValue("NAVI_LINE_GREEN", &m_nNaviLineGreen) )
	{
		m_nNaviLineGreen = 0;
	}
	if( !nuReadConfigValue("NAVI_LINE_BLUE", &m_nNaviLineBlue) )
	{
		m_nNaviLineBlue = 0;
	}
	if( !nuReadConfigValue("ARRAW_RED", &m_nNaviArrawRed) )
	{
		m_nNaviArrawRed = 0;
	}
	if( !nuReadConfigValue("ARRAW_GREEN", &m_nNaviArrawGreen) )
	{
		m_nNaviArrawGreen = 0;
	}
	if( !nuReadConfigValue("ARRAW_BLUE", &m_nNaviArrawBlue) )
	{
		m_nNaviArrawBlue = 255;
	}
#ifdef ANDROID_ROYAL_TUNNEL
	if( !nuReadConfigValue("SLEEPROYALTEK", &m_nRoyaltekSleep) )
	{
		m_nRoyaltekSleep = ROYALTEK_SLEEP;
	}
#endif
	nuMemset( &m_NaviNextNSSIgnore, 0, sizeof(m_NaviNextNSSIgnore) );
	m_nNowSpeed = 0;
	m_nPreScal = 0xFF;
	XShift=0;
	YShift=0;
	m_bFirstSound = nuFALSE;
	m_lTMCMemIdx = 0;
//====================SDK=================//
	m_pShowRoadBuf = NULL;
//====================SDK=================//
    bRestNaviData2();
}

CNaviData::~CNaviData()
{
	Free();
}

nuBOOL CNaviData::Initialize()
{	
#ifdef LCMM
	 m_lChoosePath = 0;
	 m_bSelectRouteRule = nuFALSE;
#endif
	ResetNaviData();

	m_nCrossIndexInSouce = 0;
	nuMemset( &m_ptDraw, 0 ,sizeof(m_ptDraw));
	nuMemset( &m_CrossPoint, 0, sizeof(m_CrossPoint) );
	m_CrossPoint.ptArray = &m_ptDraw[0];
	m_bDrawBefoeArrow = nuFALSE;
	m_bFirstCalDis    = nuTRUE;
	m_PtRouteEndMax.x = m_PtRouteEndMax.y = 0;
	m_PtRouteEndMin.x = m_PtRouteEndMin.y = 20000000;
	return nuTRUE;
}

nuVOID CNaviData::Free()
{
	FreeNaviData();
//====================SDK=================//
	if(NULL != m_pShowRoadBuf)
	{
		delete [] m_pShowRoadBuf;
		m_pShowRoadBuf = NULL;
	}
//====================SDK=================//
}

nuBOOL CNaviData::ResetNaviData()
{
	
	m_nTHCTimer = 0;
	g_pInFsApi->pGdata->drawInfoNavi.bCarInNaviLine		= 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx	= 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum	= 0;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget   = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget  = 0;
	m_ptNavi.pNFTC	= NULL;
	m_ptNavi.pNRBC	= NULL;
	m_ptNavi.nSubIdx= 0;
	m_ptNavi.nPtIdx	= 0;
	m_ptSimu.pNFTC	= NULL;
	
	m_pLocalRouteData = NULL;
	nuMemset( m_bHasPlayVoice, 0, sizeof(m_bHasPlayVoice) );
	m_nLastCrossToTar = 0;
	nuMemset( &m_NaviForZoom, 0, sizeof(NAVINSS));
	m_nBackCrossToTar = 0;
	m_nNowLen = 0;
	m_nNextLen = 0;
	m_nTraffNum = 0;
	nuMemset( &m_ptNextCross,0,sizeof(m_ptNextCross));
	nuMemset( &m_ptLastNextCross,0,sizeof(m_ptLastNextCross));
	nuMemset( &m_NaviForArrow, 0, sizeof(m_NaviForArrow) );
	nuMemset( &m_NaviForZoomPre, 0, sizeof(m_NaviForZoomPre) );	
	nuMemset( &m_ptNext, 0, sizeof(m_ptNext));
	nuMemset( &m_StrHighWayLast, 0, sizeof(m_StrHighWayLast) );
	nuMemset( &m_StrHighWayCmp, 0, sizeof(m_StrHighWayCmp) );
	nuMemset( &m_NodeArr, 0 ,sizeof(SHOWNODE) * SAVENODENUM );
	nuMemset( &m_nSaveIndex, 0, sizeof(nuDWORD) * SAVENODENUM ); 
	nuMemset( &g_pInFsApi->pGdata->drawInfoNavi.HighWayCross, 0, sizeof(g_pInFsApi->pGdata->drawInfoNavi.HighWayCross) );
	g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID = 0;
	g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = 0;
	m_nSaveNum         = 0;
	m_bInPassNode      = nuFALSE;
	m_nArrowCrossNum   = 0;	
    m_nCombinLen       = 0; 
	m_bCombinDrawArrow = nuFALSE;
	m_bEstimation = nuFALSE;
	m_bInFreeWay = nuFALSE;
	m_bFirstSpeed = nuFALSE;
	m_lCarSpeed = 0;
#ifdef ZENRIN
	nuMemset( &m_pTempNSSInfo,0,sizeof(m_pTempNSSInfo));
	nuMemset( &m_pVoiceNSSInfo,0,sizeof(m_pVoiceNSSInfo));
	m_bRoadCross = nuFALSE;
	m_bVoiceRoadCross = nuFALSE;
#endif
	      
	
	m_nMaxLocalTimers = 0;
	PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
	while ( pTmpNFTC )
	{
		pTmpNFTC->RunPass = nuFALSE;
		pTmpNFTC = pTmpNFTC->NextFT;
	}
	m_nPreScal = 0xFF;
	XShift=0;
	YShift=0;
	return nuTRUE;
}
nuVOID CNaviData::FreeNaviData()
{
	FreeLocalRouteData();
}

#define DEFAULT_SIMU_DIS						5
nuBOOL CNaviData::GetSimulationData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpOut)
{
	if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	if( nLen <= 0 )
	{
		nLen = DEFAULT_SIMU_DIS;
	}
	PSIMULATIONDATA pSimData = (PSIMULATIONDATA)lpOut;
	nuLONG dx = 0, dy = 0;

	if( bReStart )
	{
		Initialize();
		PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];

		if( pNFTC == NULL )
		{
			return nuFALSE;
		}
		if( pNFTC->NRBC == NULL || pNFTC->NRBC->NSS == NULL )
		{
			return nuFALSE;
		}
		m_ptSimu.pNFTC	= pNFTC;
		m_ptSimu.pNRBC	= pNFTC->NRBC;
		m_ptSimu.nSubIdx= 0;
		m_ptSimu.nPtIdx	= 0;
		m_ptSimu.ptMapped	= pNFTC->NRBC->NSS[0].ArcPt[0];
		nuMemcpy( &m_ptNavi, &m_ptSimu, sizeof(m_ptNavi) );
		pSimData->nX	= m_ptSimu.ptMapped.x;
		pSimData->nY	= m_ptSimu.ptMapped.y;
		dx = pNFTC->NRBC->NSS[0].ArcPt[1].x - pNFTC->NRBC->NSS[0].ArcPt[0].x;
		dy = pNFTC->NRBC->NSS[0].ArcPt[1].y - pNFTC->NRBC->NSS[0].ArcPt[0].y;
		pSimData->nAngle	= GetNssFirstAngle( &(pNFTC->NRBC->NSS[0]) );//nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
		if( pSimData->nAngle < 0 )
		{
			pSimData->nAngle	+= 360;
		}
		XShift=0;
		YShift=0;
	}
	else
	{
		PNAVIFROMTOCLASS pNFTC	= m_ptSimu.pNFTC;
		PNAVIRTBLOCKCLASS pNRBC = m_ptSimu.pNRBC;
		nuroPOINT ptLast= {0};
		nuLONG dxy = 0;
		nuBOOL bFirst = nuTRUE;
		if( pNFTC == NULL  )
		{
			return nuFALSE;
		}
		while( pNFTC != NULL )
		{
			while( pNRBC != NULL && pNRBC->RTBStateFlag == 0 )
			{
				nuDWORD i;
				nuLONG j;
				if( bFirst )
				{
					i = m_ptSimu.nSubIdx;
				}
				else
				{
					i = 0;
				}
				for( ; i < pNRBC->NSSCount; i++ )
				{
					if( bFirst )
					{
						j = m_ptSimu.nPtIdx;
						ptLast = m_ptSimu.ptMapped;
					}
					else
					{
						j = 0;
					}
					for( ; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
					{
						dx = pNRBC->NSS[i].ArcPt[j+1].x - ptLast.x;
						dy = pNRBC->NSS[i].ArcPt[j+1].y - ptLast.y;
						dxy = (nuLONG)nuSqrt(dx*dx +dy*dy);
						if( dxy == 0 || dxy <= nLen )
						{
							ptLast = pNRBC->NSS[i].ArcPt[j+1];
							/************************************************/
							if( dxy == nLen )
							{
								m_ptSimu.ptMapped.x = ptLast.x ;
								m_ptSimu.ptMapped.y = ptLast.y;
								m_ptSimu.pNFTC		= pNFTC;
								m_ptSimu.pNRBC		= pNRBC;
								m_ptSimu.nSubIdx	= i;
								m_ptSimu.nPtIdx		= j + 1;
								pSimData->nX		= m_ptSimu.ptMapped.x;
								pSimData->nY		= m_ptSimu.ptMapped.y;
								XShift = 0;
								YShift = 0;
								if( j + 1 == pNRBC->NSS[i].ArcPtCount - 1)
								{
									NAVINSS NowNSS,NextNSS;
									NowNSS.pNFTC	= pNFTC;
									NowNSS.pNRBC	= pNRBC;
									NowNSS.pNSS		= &(pNRBC->NSS[i]);
									NowNSS.nIndex	= i;
									if( GetNextNSS( NowNSS, &NextNSS ))
									{
										dx = NextNSS.pNSS->ArcPt[1].x - NextNSS.pNSS->ArcPt[0].x;
										dy = NextNSS.pNSS->ArcPt[1].y - NextNSS.pNSS->ArcPt[0].y;
										pSimData->nAngle	= nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
									}
									else
									{
										pSimData->nAngle = -1;
									}
								}
								else
								{
									pSimData->nAngle	= nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
								}
								return nuTRUE;
								
							}
							
							/************************************************************************/
							nLen -= dxy;
						}
						else
						{
//<<<<<<< NaviData.cpp
//=======
							m_ptSimu.ptMapped.x = ptLast.x + (nLen * dx + XShift*dxy/100) / dxy;
							m_ptSimu.ptMapped.y = ptLast.y + (nLen * dy + YShift*dxy/100) / dxy;
							XShift = ((nLen * dx + XShift)*100 / dxy) % 100;
							YShift = ((nLen * dy + YShift)*100 / dxy) % 100;
							/* 2010.04.06 Louis mark weigen old code
>>>>>>> 1.45
							m_ptSimu.ptMapped.x = ptLast.x + nLen * dx / dxy;
							m_ptSimu.ptMapped.y = ptLast.y + nLen * dy / dxy;
							*/
							m_ptSimu.pNFTC		= pNFTC;
							m_ptSimu.pNRBC		= pNRBC;
							m_ptSimu.nSubIdx	= i;
							m_ptSimu.nPtIdx		= j;
							pSimData->nX		= m_ptSimu.ptMapped.x;
							pSimData->nY		= m_ptSimu.ptMapped.y;
							
							pSimData->nAngle	= nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
							if ( pSimData->nAngle == 180 )
							{
								pSimData->nAngle	= nulAtan2(dy, dx);
							}
							if( pSimData->nAngle < 0 )
							{
								pSimData->nAngle	+= 360;
							}
							return nuTRUE;
						}
					}
					bFirst	= nuFALSE;
				}
				pNRBC	= pNRBC->NextRTB;
			}
			m_ptSimu.pNFTC	= pNFTC;
			pNFTC	= pNFTC->NextFT;
			if( pNFTC != NULL )
			{
				pNRBC = pNFTC->NRBC;
			}
		}
		//get to the end of the Routing Data;
		if ( m_ptSimu.pNRBC )
		{
			m_ptSimu.nSubIdx    = m_ptSimu.pNRBC->NSSCount - 1;
			m_ptSimu.ptMapped	= m_ptSimu.pNFTC->TCoor;
			m_ptSimu.nPtIdx     = m_ptSimu.pNRBC->NSS[m_ptSimu.pNRBC->NSSCount - 1].ArcPtCount - 1;
			m_ptSimu.pNRBC		= NULL;
		}
		
		
		//pSimData->nAngle	= -1;
		pSimData->nX		= m_ptSimu.ptMapped.x;
		pSimData->nY		= m_ptSimu.ptMapped.y;
		
		nuDWORD nNRBCIndex = 1;
		PNAVIRTBLOCKCLASS pTmpNRBC = m_ptSimu.pNFTC->NRBC;
		while( pTmpNRBC && pTmpNRBC->NextRTB )
		{
			pTmpNRBC = pTmpNRBC->NextRTB;
		}
		if ( pTmpNRBC )
		{
			PNAVISUBCLASS pTmpNSS = &pTmpNRBC->NSS[pTmpNRBC->NSSCount - 1];
			nuDWORD nPtIndex = pTmpNSS->ArcPtCount - 1;
            while ( 0 == nuMemcmp( &(pTmpNSS->ArcPt[nPtIndex]),&(pTmpNSS->ArcPt[nPtIndex - 1 ]),sizeof(nuroPOINT) ) )
            {
				nPtIndex -= 1;
				if ( nPtIndex == 0 )
				{
					break;
				}
            }
			if ( nPtIndex >= 1  )
			{
				nuLONG nAngleDx = pTmpNSS->ArcPt[nPtIndex].x - pTmpNSS->ArcPt[nPtIndex - 1 ].x;
				nuLONG nAngleDy = pTmpNSS->ArcPt[nPtIndex].y - pTmpNSS->ArcPt[nPtIndex - 1 ].y;
				pSimData->nAngle = nulAtan2(nAngleDy, nAngleDx);
			}
			
		}
		else
		{
			nuINT sss = 1;
		}
		
	}
	return nuTRUE;
}

nuBOOL CNaviData::GetSimuData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpOut)
{
	if( g_pInFsApi->pGdata == NULL || 
		g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	if( nLen <= 0 )
	{
		nLen = DEFAULT_SIMU_DIS;
	}
	PSIMULATIONDATA pSimData = (PSIMULATIONDATA)lpOut;
	nuLONG dx = 0, dy = 0;
	PNAVIFROMTOCLASS pNFTC = NULL;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	nuLONG i= 0;
	nuLONG j= 0;
	nuBOOL bFind = nuFALSE;
	if( bReStart )
	{
		Initialize();
		pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
		if( NULL == pNFTC )
		{
			return nuFALSE;
		}
		pNRBC = pNFTC->NRBC;
		if( NULL == pNRBC || NULL == pNRBC->NSS )
		{
			return nuFALSE;
		}
		m_ptNavi.pNFTC		= pNFTC;
		m_ptNavi.pNRBC		= pNRBC;
		m_ptNavi.nSubIdx	= 0;
		m_ptNavi.nPtIdx		= 0;
		m_ptNavi.ptMapped	= pNFTC->NRBC->NSS[0].ArcPt[0];
		
		m_nSimuRdRealLen = 0;
		i = 0;
		j = 0;
		while( i < pNRBC->NSSCount )
		{
			while( j < pNRBC->NSS[i].ArcPtCount - 1 )
			{
				dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
				dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
				if( 0 != dx || 0 != dy )
				{
					m_nSimuRdSegLen	 = m_nSimuRdNowLen = (nuLONG)nuSqrt(dx*dx + dy*dy);
					bFind = nuTRUE;
					break;
				}
				++j;
			}
			if( bFind )
			{
				break;
			}
			j = 0;
			++i;
		}
		if( !bFind )
		{
			return nuFALSE;
		}
		m_ptSimu.ptMapped = pNRBC->NSS[i].ArcPt[j];
	}
	else
	{
		if( m_ptSimu.pNFTC == NULL  )
		{
			return nuFALSE;
		}
		pNFTC = m_ptSimu.pNFTC;
		pNRBC = m_ptSimu.pNRBC;
		i = m_ptSimu.nSubIdx;
		j = m_ptSimu.nPtIdx;
		m_nSimuRdRealLen += nLen;
		if( m_nSimuRdRealLen <= m_nSimuRdSegLen )
		{
			dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
			dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
			bFind = nuTRUE;
		}
		else
		{
			++j;
			while( NULL != pNFTC )
			{
				while( NULL != pNRBC && 0 == pNRBC->RTBStateFlag )
				{
					while( i < pNRBC->NSSCount )
					{
						while( j < pNRBC->NSS[i].ArcPtCount - 1 )
						{
							dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
							m_nSimuRdNowLen = (nuLONG)nuSqrt(dx*dx + dy*dy);
							m_nSimuRdSegLen += m_nSimuRdNowLen;
							if( m_nSimuRdRealLen <= m_nSimuRdSegLen )
							{
								bFind = nuTRUE;
								break;
							}
							++j;
						}
						if( bFind )
						{
							break;
						}
						j = 0;
						++i;
					}
					if( bFind )
					{
						break;
					}
					//
					m_ptSimu.pNRBC = pNRBC;
					pNRBC	= pNRBC->NextRTB;
					i = 0;
				}
				if( bFind )
				{
					break;
				}
				m_ptSimu.pNFTC = pNFTC;
				pNFTC	= pNFTC->NextFT;
				if( pNFTC )
				{
					pNRBC = pNFTC->NRBC;
				}
			}
		}
		if( bFind )
		{
			nLen = m_nSimuRdRealLen + m_nSimuRdNowLen - m_nSimuRdSegLen;
			m_ptSimu.ptMapped.x = pNRBC->NSS[i].ArcPt[j].x + nLen * dx / m_nSimuRdNowLen;
			m_ptSimu.ptMapped.y = pNRBC->NSS[i].ArcPt[j].y + nLen * dy / m_nSimuRdNowLen;
		}
		else
		{
			pNFTC	= m_ptSimu.pNFTC;
			pNRBC	= m_ptSimu.pNRBC;
			for( i = pNRBC->NSSCount - 1; i >= 0; --i )
			{
				for( j = pNRBC->NSS[i].ArcPtCount - 2; j >= 0; --j )
				{
					dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
					if( 0 != dx || 0 != dy )
					{
						m_ptSimu.ptMapped = pNRBC->NSS[i].ArcPt[j+1];
						bFind = nuTRUE;
						break;
					}
				}
				if( bFind )
				{
					break;
				}
			}
			if( !bFind )
			{
				return nuFALSE;
			}
		}
	}
	m_ptSimu.pNFTC		= pNFTC;
	m_ptSimu.pNRBC		= pNRBC;
	m_ptSimu.nSubIdx	= i;
	m_ptSimu.nPtIdx		= j;
	pSimData->nX		= m_ptSimu.ptMapped.x;
	pSimData->nY		= m_ptSimu.ptMapped.y;
	pSimData->nAngle	= nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
	if( pSimData->nAngle < 0 )
	{
		pSimData->nAngle	+= 360;
	}
	return nuTRUE;
}

nuBOOL CNaviData::DrawNaviRoad(nuBYTE bZoom)
{
	//edit by chang;
	//DrawNaviRoad2(bZoom);
	//return DrawNaviRoad2(bZoom);
	///////////////////////////////////
	/*if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	if( m_nNaviLineType &&  g_pInFsApi->pDrawInfo->pcTimerData->nAutoMoveType == AUTO_MOVE_GPS)
	{
		return DrawNaviRoad2(bZoom);
	}*/
	///return nuFALSE;
	//open by chang;
	nuBYTE nDrawType = 0;
	NURORECT RectDraw = {0};;
	typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
	MT_MapToBmpProc pf_MapToBmp;
	nuBYTE n3DMode = 0;
	
	PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	NUROPOINT ptList[POINTLISTCOUNT] = {0}, pt1 = {0};
	NUROPOINT ptListArrow[POINTLISTCOUNT] = {0};
	nuDWORD i = 0, j = 0;
	nuLONG	nNowCountArrow = 0;
	nuLONG nLineWidtharr = 0;
	nuLONG	nNowCount = 0;
	nuLONG nSkipLen = 0, dx = 0, dy = 0, nLineWidth = 0, bFullPT = nuFALSE;
	nuBOOL bLastInScreen = nuFALSE;
	
	if( bZoom == MAP_ZOOM )
	{
		if( !g_pInFsApi->pGdata->zoomScreenData.bZoomScreen )
		{
			return nuFALSE;
		}
		nDrawType	= DRAW_TYPE_ZOOM_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp = g_pInMtApi->MT_ZoomMapToBmp ;
		n3DMode		= g_pInFsApi->pGdata->zoomScreenData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//  SCALETOSCREEN_LEN;
	}
	else
	{
		nDrawType	= DRAW_TYPE_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
		pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
		n3DMode		= g_pInFsApi->pGdata->uiSetData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	}	
	nuINT nDrawObj = 0;//g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);

	nLineWidtharr = nLineWidth;
	if(g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale == 400000)
	{
		while( pNFTC != NULL )
		{
			if ( pNFTC == m_ptNavi.pNFTC && !bFullPT)
			{
				pNRBC = m_ptNavi.pNRBC;
			}
			else
			{
				pNRBC = pNFTC->NRBC;
			}
			while( pNRBC != NULL && !bFullPT)
			{
				if( pNRBC->NSS != NULL )
				{
					if( pNRBC == m_ptNavi.pNRBC )
					{
						if (   pNFTC == m_ptNavi.pNFTC 
							&& pNRBC == m_ptNavi.pNRBC 
							)
						{
							i = m_ptNavi.nSubIdx;
						}
						else
						{
							i = 0;
						}
						for( ; i < pNRBC->NSSCount; i++ )
						{
							if( !nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw) )
							{
								bLastInScreen = nuFALSE;
								continue;
							}
							if( (!bLastInScreen && nNowCount > 1) || nNowCount == POINTLISTCOUNT )
							{
								nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ROAD, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
								g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
								nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
								g_pInMtApi->MT_DrawObject(nDrawObj, ptListArrow, nNowCountArrow, nLineWidtharr);
							}
							if (   pNFTC == m_ptNavi.pNFTC 
								&& pNRBC == m_ptNavi.pNRBC
								&& i     == m_ptNavi.nSubIdx
								)
							{
								j = m_ptNavi.nPtIdx;  
							}
							else
							{
								j = 0;
							}
							for(; j < pNRBC->NSS[i].ArcPtCount; j++)
							{ 
								if( j != 0 )
								{
									dx = pNRBC->NSS[i].ArcPt[j].x - pt1.x;
									dy = pNRBC->NSS[i].ArcPt[j].y - pt1.y;
									if( NURO_ABS(dx) < nSkipLen && NURO_ABS(dy) < nSkipLen )
									{
										continue;
									}
								}
								if (   pNFTC == m_ptNavi.pNFTC 
									&& pNRBC == m_ptNavi.pNRBC
									&& i     == m_ptNavi.nSubIdx 
									&& j	 == m_ptNavi.nPtIdx)
								{
									pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
									pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
								}
								else
								{
									pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptList[nNowCount].x, &ptList[nNowCount].y);
									pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
								}
								nNowCount ++;
								nNowCountArrow++;
								pt1 = pNRBC->NSS[i].ArcPt[j];
								if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
								{
									break;
								}
							}
							bLastInScreen = nuTRUE;
							if(g_pInFsApi->pUserCfg->nTMCSwitchFlag && 
								!g_pInFsApi->pUserCfg->bMoveMap &&
								g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale < 
									(nuDWORD)g_pInFsApi->pGdata->uiSetData.nBsdStartScaleVelue)
							{
								SetNaviDTIColor(g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx), 
									g_pInFsApi->pMapCfg->naviLineSetting);
								nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
								g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);	
								nNowCount = 0;					
							}
							if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
							{
								break;
							}
						}
					}
					else
					{
						pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						pf_MapToBmp(pNRBC->TInfo.FixCoor.x,  pNRBC->TInfo.FixCoor.y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
						nNowCount ++;
						nNowCountArrow++;
						if(nNowCount >=  POINTLISTCOUNT || nNowCountArrow >= POINTLISTCOUNT)
						{
							break;
						}
					}
				}
				else if(bLastInScreen)
				{
					if(((pNRBC->TInfo.FixCoor.x < RectDraw.left) || (pNRBC->TInfo.FixCoor.x > RectDraw.right)) ||
						((pNRBC->TInfo.FixCoor.y < RectDraw.top) || (pNRBC->TInfo.FixCoor.y > RectDraw.bottom)))
					{
						pNRBC = pNRBC->NextRTB;
						continue;
					}
					pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
					pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
					nNowCount++;
					nNowCountArrow++;
					if(nNowCount >=  POINTLISTCOUNT || nNowCountArrow >= POINTLISTCOUNT)
					{
						break;
					}
				}
				if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
				{
					break;
				}
				pNRBC = pNRBC->NextRTB;
			}
			if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
			{
				break;
			}
			pNFTC = pNFTC->NextFT;
		}
	}
	else
	{
	while( pNFTC != NULL )
	{
		if ( pNFTC == m_ptNavi.pNFTC && !bFullPT)
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL && !bFullPT)
		{
			if( pNRBC->NSS != NULL )
			{
				if (   pNFTC == m_ptNavi.pNFTC 
					&& pNRBC == m_ptNavi.pNRBC 
					)
				{
					i = m_ptNavi.nSubIdx;
				}
				else
				{
					i = 0;
				}
				for( ; i < pNRBC->NSSCount; i++ )
				{
					if( !nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw) )
					{
						bLastInScreen = nuFALSE;
						continue;
					}
					if( (!bLastInScreen && nNowCount > 1) || nNowCount == POINTLISTCOUNT )
					{
						//Draw Navi Line...
						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ROAD, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						g_pInMtApi->MT_DrawObject(nDrawObj, ptListArrow, nNowCountArrow, nLineWidtharr);
						nNowCount = 0;
						nNowCountArrow = 0;
					}
					if (   pNFTC == m_ptNavi.pNFTC 
						&& pNRBC == m_ptNavi.pNRBC
						&& i     == m_ptNavi.nSubIdx
						)
					{
						j = m_ptNavi.nPtIdx;  
					}
					else
					{
						j = 0;
					}
					for(; j < pNRBC->NSS[i].ArcPtCount; j++)
					{ 
						if( j != 0 )
						{
							dx = pNRBC->NSS[i].ArcPt[j].x - pt1.x;
							dy = pNRBC->NSS[i].ArcPt[j].y - pt1.y;
							if( NURO_ABS(dx) < nSkipLen && NURO_ABS(dy) < nSkipLen )
							{
								continue;
							}
						}
						if (   pNFTC == m_ptNavi.pNFTC 
							&& pNRBC == m_ptNavi.pNRBC
							&& i     == m_ptNavi.nSubIdx 
							&& j	 == m_ptNavi.nPtIdx)
						{
							pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
							pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
						}
						else
						{
							pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptList[nNowCount].x, &ptList[nNowCount].y);
							pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
						}
						nNowCount ++;
						nNowCountArrow++;
						pt1 = pNRBC->NSS[i].ArcPt[j];
						if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
						{
							break;
						}
					}
					bLastInScreen = nuTRUE;
					if(g_pInFsApi->pUserCfg->nTMCSwitchFlag && 
						!g_pInFsApi->pUserCfg->bMoveMap &&
						g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale < 
							(nuDWORD)g_pInFsApi->pGdata->uiSetData.nBsdStartScaleVelue)
					{
						nuUINT trafficevent3 = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
						TMCRoadInfoStru* pdirdata = g_pInFsApi->FS_GetTMCDataInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
						
						nuUINT traffic = 0;
						if (pdirdata == NULL || pdirdata[0].Dir== 0 ||pdirdata[1].Dir== 0)
						{
							traffic = trafficevent3;
						}
						else
						{
							if (pNRBC->NSS[i].nOneWay != 0 && pdirdata[0].Dir!= 0 && pdirdata[1].Dir != 0)
							{
								if (pdirdata[0].Dir == pNRBC->NSS[i].nOneWay)
								{
									traffic = pdirdata[0].Traffic;
								}
								else
								{
									traffic = pdirdata[1].Traffic;
								}
							}

						}

						SetNaviDTIColor(traffic, 
							g_pInFsApi->pMapCfg->naviLineSetting);
						nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);	
						//nNowCount = 0;					
					}
					if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
					{
						break;
					}
				}
			}
			else if(bLastInScreen)
			{
				if(((pNRBC->TInfo.FixCoor.x < RectDraw.left) || (pNRBC->TInfo.FixCoor.x > RectDraw.right)) ||
					((pNRBC->TInfo.FixCoor.y < RectDraw.top) || (pNRBC->TInfo.FixCoor.y > RectDraw.bottom)))
				{
					pNRBC = pNRBC->NextRTB;
					continue;
				}
				pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
				pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptListArrow[nNowCountArrow].x, &ptListArrow[nNowCountArrow].y);
				nNowCount++;
				nNowCountArrow++;
				if(nNowCount >=  POINTLISTCOUNT || nNowCountArrow >= POINTLISTCOUNT)
				{
					break;
				}
			}
			if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
			{
				break;
			}
			pNRBC = pNRBC->NextRTB;
		}
		if( nNowCount == POINTLISTCOUNT || nNowCountArrow == POINTLISTCOUNT)
		{
			break;
		}
		pNFTC = pNFTC->NextFT;
	}
	}
	//Draw Navi Line....
	if(!g_pInFsApi->pUserCfg->nTMCSwitchFlag || 
		g_pInFsApi->pUserCfg->bMoveMap || 
		g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale >= 
		(nuDWORD)g_pInFsApi->pGdata->uiSetData.nBsdStartScaleVelue)
	{
		g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR = m_nNaviLineRed;
		g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG = m_nNaviLineGreen;
		g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB = m_nNaviLineBlue;
		
		nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
		g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
	}
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteArrowColorR = m_nNaviArrawRed;
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteArrowColorG = m_nNaviArrawGreen;
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteArrowColorB = m_nNaviArrawBlue;
	nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
	g_pInMtApi->MT_DrawObject(nDrawObj, ptListArrow, nNowCountArrow, nLineWidtharr);
	DrawArrow(nuFALSE);
	return nuTRUE;
}
nuBOOL CNaviData::SetNaviDTIColor(nuUINT trafficevent, NAVILINESETTING &RSet/* = NULL*/)
{
	if (trafficevent == 0)
	{
		RSet.nRouteLineColorR = 0;//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR;
		RSet.nRouteLineColorG = 0;//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG;
		RSet.nRouteLineColorB = 250;//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB;
		return nuTRUE;
	}
	else if (trafficevent == 1)
	{
		RSet.nRouteLineColorR = 107;//15;//107;
		RSet.nRouteLineColorG = 243;//250;//207;
		RSet.nRouteLineColorB = 48;//12;//107;
	}
	else if (trafficevent == 2)
	{
		RSet.nRouteLineColorR = 237;//250;//250;
		RSet.nRouteLineColorG = 255;//250;//180;
		RSet.nRouteLineColorB = 39;//12;//10;
	}
	else if (trafficevent == 3)
	{
		RSet.nRouteLineColorR = 255;//250;//255;
		RSet.nRouteLineColorG = 1;//0;//0;
		RSet.nRouteLineColorB = 192;//150;//0;
	}
	return nuTRUE;
}
nuBOOL CNaviData::DrawNaviRoad2(nuBYTE bZoom)
{
	///return nuFALSE;
	nuBYTE nDrawType = 0;
	NURORECT RectDraw = {0};
	typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
	MT_MapToBmpProc pf_MapToBmp;
	nuBYTE n3DMode = 0;
#ifdef LCMM
	nuBYTE LineR = 0,LineB = 0,LineG = 0,Width = 0;
	nuINT RouteCounter_LCMM = 0;
	nuBOOL bDisplayChoosedRoutePath = nuFALSE;
	LineR = g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR;
	LineG = g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG;
	LineB = g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB;
	Width = g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth;
	//m_ptNavi.pNFTC;
	//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth += 2*g_pInFsApi->pGdata->routeGlobal.RouteCount;
	PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC;
	if(g_pInFsApi->pGdata->routeGlobal.RouteCount > 1 && !m_bSelectRouteRule)
	{
		pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[m_lChoosePath];
		if(pNFTC == NULL)
		{ 
			return nuFALSE;
		}
	}
	else
	{
		//pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
		pNFTC = m_ptNavi.pNFTC;
		if(pNFTC == NULL)
		{ 
			return nuFALSE;
		}
	}
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	NUROPOINT ptList[POINTLISTCOUNT] = {0}, pt1 = {0};
	nuLONG	nNowCount = 0;
	nuLONG nSkipLen = 0, dx = 0, dy = 0, nLineWidth = 0;
	nuBOOL bLastInScreen = nuFALSE;
	
	if( bZoom == MAP_ZOOM )
	{
		if( !g_pInFsApi->pGdata->zoomScreenData.bZoomScreen )
		{
			return nuFALSE;
		}
		nDrawType	= DRAW_TYPE_ZOOM_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp = g_pInMtApi->MT_ZoomMapToBmp ;
		n3DMode		= g_pInFsApi->pGdata->zoomScreenData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	}
	else
	{
		nDrawType	= DRAW_TYPE_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
		pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
		n3DMode		= g_pInFsApi->pGdata->uiSetData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	}	
	if(m_lChoosePath == 0)
	{
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR = 0;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG = 0;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB = 130;
	}
	else if(m_lChoosePath == 1)
	{
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR = 255;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG = 0;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB = 255;
	}
	else if(m_lChoosePath == 2)
	{
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR = 50;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG = 200;
		//g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB = 10;
	}
	nuINT nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
	while( pNFTC != NULL )
	{
		if ( pNFTC == m_ptNavi.pNFTC  )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL )
		{
			if( pNRBC->NSS != NULL )
			{
				nuDWORD i;
				if (   pNFTC == m_ptNavi.pNFTC 
					&& pNRBC == m_ptNavi.pNRBC 
					)
				{
					i = m_ptNavi.nSubIdx;
					g_pInFsApi->pGdata->drawInfoNavi.bLastCross = nuFALSE;
					if((pNRBC->NextRTB == NULL) && (m_ptNavi.nSubIdx == (pNRBC->NSSCount - 1)))
					{
						g_pInFsApi->pGdata->drawInfoNavi.bLastCross = nuTRUE;
					}
				}
				else
				{
					i = 0;
				}
				for( ; i < pNRBC->NSSCount; i++ )
				{
					if( !nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw) )
					{
						bLastInScreen = nuFALSE;
						continue;
					}
					if( (!bLastInScreen && nNowCount > 1) || nNowCount ==  POINTLISTCOUNT)//
					{
						//Draw Navi Line...
						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						//						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						//						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						//						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ROAD, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						nNowCount = 0;
					}
					nuLONG j;
					if (   pNFTC == m_ptNavi.pNFTC 
						&& pNRBC == m_ptNavi.pNRBC
						&& i     == m_ptNavi.nSubIdx
						)
					{
						j = m_ptNavi.nPtIdx;  
					}
					else
					{
						j = 0;
					}
					for(; j < pNRBC->NSS[i].ArcPtCount; j++)
					{ 
						if( j != 0 )
						{
							dx = pNRBC->NSS[i].ArcPt[j].x - pt1.x;
							dy = pNRBC->NSS[i].ArcPt[j].y - pt1.y;
							if( NURO_ABS(dx) < nSkipLen && NURO_ABS(dy) < nSkipLen )
							{
								continue;
							}
						}
						if (   pNFTC == m_ptNavi.pNFTC 
							&& pNRBC == m_ptNavi.pNRBC
							&& i     == m_ptNavi.nSubIdx 
							&& j	 == m_ptNavi.nPtIdx)
						{
							pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						}
						else
						{
							pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptList[nNowCount].x, &ptList[nNowCount].y);							
						}
						
						nNowCount ++;
						pt1 = pNRBC->NSS[i].ArcPt[j];
						if( nNowCount >=  POINTLISTCOUNT)//
						{
							break;
						}
					}
					bLastInScreen = nuTRUE;
					if(g_pInFsApi->pUserCfg->nTMCSwitchFlag && !g_pInFsApi->pUserCfg->bMoveMap && g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale < 
						(nuDWORD)g_pInFsApi->pGdata->uiSetData.nBsdStartScaleVelue)
					{
						SetNaviDTIColor(g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx), 
							g_pInFsApi->pMapCfg->naviLineSetting);
						nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						nNowCount = 0;
					}
				}	
			}
			//----------added by daniel for LCMM---------------------//
		#ifdef LCMM
			else
			{
				if(bLastInScreen)
				{
					/*if(((pNRBC->FInfo.FixCoor.x < RectDraw.left) || (pNRBC->FInfo.FixCoor.x > RectDraw.right)) ||
						((pNRBC->FInfo.FixCoor.y < RectDraw.top) || (pNRBC->FInfo.FixCoor.y > RectDraw.bottom)))
					{
						pNRBC = pNRBC->NextRTB;
						continue;
					}
					pf_MapToBmp(pNRBC->FInfo.FixCoor.x, pNRBC->FInfo.FixCoor.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
					if(RouteCounter_LCMM == 1)
					{
						ptList[nNowCount].x -= 5;
						ptList[nNowCount].y -= 5;
					}
					else if(RouteCounter_LCMM == 2)
					{
						ptList[nNowCount].x += 5;
						ptList[nNowCount].y += 5;
					}
					nNowCount++;*/
					if(((pNRBC->TInfo.FixCoor.x < RectDraw.left) || (pNRBC->TInfo.FixCoor.x > RectDraw.right)) ||
						((pNRBC->TInfo.FixCoor.y < RectDraw.top) || (pNRBC->TInfo.FixCoor.y > RectDraw.bottom)))
					{
						pNRBC = pNRBC->NextRTB;
						continue;
					}
					pf_MapToBmp(pNRBC->TInfo.FixCoor.x, pNRBC->TInfo.FixCoor.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
					nNowCount++;
					if(nNowCount  >=  POINTLISTCOUNT)
					{
						break;
					}
				}
			}
		#endif
			//----------added by daniel for LCMM---------------------//
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;	
	}
	//Draw Navi Line....
	if(!g_pInFsApi->pUserCfg->nTMCSwitchFlag ||
		g_pInFsApi->pUserCfg->bMoveMap || 
		g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale >= 
						(nuDWORD)g_pInFsApi->pGdata->uiSetData.nBsdStartScaleVelue)
	{
		g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
	}
	//g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
	DrawArrow(nuFALSE);
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorR = LineR;
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorG = LineG;
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineColorB = LineB;
	g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth  = Width;
#else
	PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC;
	if ( pNFTC == NULL )
	{
		pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC;
	}
	PNAVIRTBLOCKCLASS pNRBC= NULL;
	NUROPOINT ptList[POINTLISTCOUNT] = {0}, pt1 = {0};
	nuLONG	nNowCount = 0;
	nuLONG nSkipLen = 0, dx = 0, dy = 0, nLineWidth = 0;
	nuBOOL bLastInScreen = nuFALSE;
	
	if( bZoom == MAP_ZOOM )
	{
		if( !g_pInFsApi->pGdata->zoomScreenData.bZoomScreen )
		{
			return nuFALSE;
		}
		nDrawType	= DRAW_TYPE_ZOOM_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp = g_pInMtApi->MT_ZoomMapToBmp ;
		n3DMode		= g_pInFsApi->pGdata->zoomScreenData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	}
	else
	{
		nDrawType	= DRAW_TYPE_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
		pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
		n3DMode		= g_pInFsApi->pGdata->uiSetData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	}	
	nuINT nDrawObj = g_pInMtApi->MT_SelectObject(nDrawType, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
	while( pNFTC != NULL )
	{
		if ( pNFTC == m_ptNavi.pNFTC  )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL )
		{
			if( pNRBC->NSS != NULL )
			{
				nuDWORD i;
				if (   pNFTC == m_ptNavi.pNFTC 
					&& pNRBC == m_ptNavi.pNRBC 
					)
				{
					i = m_ptNavi.nSubIdx;
					g_pInFsApi->pGdata->drawInfoNavi.bLastCross = 0;
					if(pNRBC->NSS[i].Reserve == 1)
					{
						 g_pInFsApi->pGdata->drawInfoNavi.bLastCross = 1;
					}
				}
				else
				{
					i = 0;
				}
				for( ; i < pNRBC->NSSCount; i++ )
				{
					if( !nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw) )
					{
						bLastInScreen = nuFALSE;
						continue;
					}
					if( (!bLastInScreen && nNowCount > 1) || nNowCount == POINTLISTCOUNT )
					{
						//Draw Navi Line...
						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						//						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						//						g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
						//						nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ROAD, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
						nNowCount = 0;
					}
					nuLONG j;
					if (   pNFTC == m_ptNavi.pNFTC 
						&& pNRBC == m_ptNavi.pNRBC
						&& i     == m_ptNavi.nSubIdx
						)
					{
						j = m_ptNavi.nPtIdx;  
					}
					else
					{
						j = 0;
					}
					for(; j < pNRBC->NSS[i].ArcPtCount; j++)
					{ 
						if( j != 0 )
						{
							dx = pNRBC->NSS[i].ArcPt[j].x - pt1.x;
							dy = pNRBC->NSS[i].ArcPt[j].y - pt1.y;
							if( NURO_ABS(dx) < nSkipLen && NURO_ABS(dy) < nSkipLen )
							{
								continue;
							}
						}
						if (   pNFTC == m_ptNavi.pNFTC 
							&& pNRBC == m_ptNavi.pNRBC
							&& i     == m_ptNavi.nSubIdx 
							&& j	 == m_ptNavi.nPtIdx)
						{
							pf_MapToBmp(m_ptNavi.ptMapped.x, m_ptNavi.ptMapped.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						}
						else
						{
							pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						}
						
						nNowCount ++;
						pt1 = pNRBC->NSS[i].ArcPt[j];
						if( nNowCount == POINTLISTCOUNT )
						{
							break;
						}
					}
					bLastInScreen = nuTRUE;
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;	
	}
	//Draw Navi Line....
	g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
	//nDrawObj = g_pInMtApi->MT_SelectObject(DRAW_TYPE_NAVI_ARROW, n3DMode, &g_pInFsApi->pMapCfg->naviLineSetting, &nLineWidth);
	//g_pInMtApi->MT_DrawObject(nDrawObj, ptList, nNowCount, nLineWidth);
	DrawArrow(nuFALSE);
#endif
	return nuTRUE;
}

nuUINT CNaviData::PointToNaviLine(nuLONG x, nuLONG y, nuLONG nCoverDis, short nAngle, PSEEKEDROADDATA pRoadData)
{
	PNAVIFROMTOCLASS pNFTC = NULL;
	PNAVIFROMTOCLASS pNFTCEnd = NULL;
	nuBOOL bFromPtNavi = nuFALSE;
	
	if (  bSimMode() )
	{
		if ( m_ptNavi.pNFTC != NULL && m_ptNavi.pNRBC != NULL )
		{
			bFromPtNavi = nuTRUE;
			pNFTC = m_ptNavi.pNFTC;
		}
		else
		{
			pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
		}
		
	}
	else
	{
		if ( m_ptNavi.pNFTC != NULL )
		{
			if( m_ptNavi.pNFTC->RunPass )
			{
				pNFTC = m_ptNavi.pNFTC;
				bFromPtNavi = nuTRUE;
				if( pNFTC->NextFT != NULL )
				{
					pNFTCEnd = pNFTC->NextFT->NextFT;
				}
			}
			else
			{
				pNFTC = m_ptNavi.pNFTC;
				pNFTCEnd = pNFTC->NextFT;
			}
		}
		else
		{
			pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
			pNFTCEnd = pNFTC->NextFT;
		}
	}
	
	if( pNFTC == NULL || pRoadData == NULL )
	{
		return INNAVI_POINT_TO_NAVI_ERROR;
	}
	nuUINT nRes = INNAVI_POINT_TO_NAVI_ERROR;
	nuLONG tmpDis, dx = 0, dy = 0;
	nuroPOINT point = {x, y}, ptFixed = {0};
	nuroRECT rtRoad = {0};
	pRoadData->nDis	=  nCoverDis;
	nuLONG DCx = (nuLONG)(100 * nulCos(nAngle)/nuGetlBase());
	nuLONG DCy = (nuLONG)(100 * nulSin(nAngle)/nuGetlBase());
	nuBOOL bInRightWay = nuFALSE, bInAntiWay = nuFALSE;
	POINTINNAVILINE ptAnti = {0};
	SEEKEDROADDATA roadAnti = {0};
	roadAnti.nDis = nCoverDis;
	
	//	nuBOOL bFirst = nuFALSE;
		while( pNFTC != pNFTCEnd )
		{
			PNAVIRTBLOCKCLASS pNRBC = NULL;
			if (   bFromPtNavi 
				&& m_ptNavi.pNFTC == pNFTC )
			{
				pNRBC = m_ptNavi.pNRBC;
			}
 			else
			{
				pNRBC = pNFTC->NRBC;
			}
			//PNAVISUBCLASS    pTempNSS = NULL;
			while( pNRBC != NULL )
			{			
				if( pNRBC->NSS != NULL )
				{
					nuDWORD i = 0;
					if (   bFromPtNavi
						&& m_ptNavi.pNFTC == pNFTC
						&& m_ptNavi.pNRBC == pNRBC )
					{
						i = m_ptNavi.nSubIdx;
					}
					for( ; i < pNRBC->NSSCount; i ++ )
					{
						rtRoad.left		= pNRBC->NSS[i].Bound.left - pRoadData->nDis;
						rtRoad.top		= pNRBC->NSS[i].Bound.top - pRoadData->nDis;
						rtRoad.right	= pNRBC->NSS[i].Bound.right + pRoadData->nDis;
						rtRoad.bottom	= pNRBC->NSS[i].Bound.bottom + pRoadData->nDis;
						
						if( !nuPtInRect(point, rtRoad) )
						{
							//pTempNSS = &(pNRBC->NSS[i]);
							continue;
						}
						nuLONG j = 1;
						if (   bFromPtNavi 
							&& m_ptNavi.pNFTC   == pNFTC
							&& m_ptNavi.pNRBC   == pNRBC
							&& m_ptNavi.nSubIdx == i )
						{
							j = m_ptNavi.nPtIdx + 1;
						}
						for(; j < pNRBC->NSS[i].ArcPtCount; j++)
						{
							if( pNRBC->NSS[i].ArcPt[j].x > pNRBC->NSS[i].ArcPt[j-1].x )
							{
								rtRoad.left		= pNRBC->NSS[i].ArcPt[j-1].x - pRoadData->nDis;
								rtRoad.right	= pNRBC->NSS[i].ArcPt[j].x + pRoadData->nDis;
							}
							else
							{
								rtRoad.left		= pNRBC->NSS[i].ArcPt[j].x - pRoadData->nDis;
								rtRoad.right	= pNRBC->NSS[i].ArcPt[j-1].x + pRoadData->nDis;
							}
							//
							if( pNRBC->NSS[i].ArcPt[j].y > pNRBC->NSS[i].ArcPt[j-1].y )
							{
								rtRoad.top		= pNRBC->NSS[i].ArcPt[j-1].y - pRoadData->nDis;
								rtRoad.bottom	= pNRBC->NSS[i].ArcPt[j].y + pRoadData->nDis;
							}
							else
							{
								rtRoad.top		= pNRBC->NSS[i].ArcPt[j].y - pRoadData->nDis;
								rtRoad.bottom	= pNRBC->NSS[i].ArcPt[j-1].y + pRoadData->nDis;
							}
							//
							if( !nuPtInRect(point, rtRoad) )
							{
								continue;
							}
							if( !nuPtFixToLineSeg(point, pNRBC->NSS[i].ArcPt[j-1], pNRBC->NSS[i].ArcPt[j], &ptFixed, &tmpDis) )
							{
								continue;
							}
							if( tmpDis >= pRoadData->nDis )
							{
								continue;
							}
							
							/*
							* Judge whether the difference between the car angle and the road direction
							* is larger than pi/2.
							* less than pi/4 is right fixing.
							*/
							if( nAngle >= 0 )
							{
								dx = pNRBC->NSS[i].ArcPt[j].x - pNRBC->NSS[i].ArcPt[j-1].x;
								dy = pNRBC->NSS[i].ArcPt[j].y - pNRBC->NSS[i].ArcPt[j-1].y;
								nuLONG DA = dx*DCy - dy*DCx;
								nuLONG DB = dx*DCx + dy*DCy;
								DA = NURO_ABS(DA);
								DB = NURO_ABS(DB);
								nuLONG DAB = dx*DCx + dy*DCy;
								
								if( DA < DB )
								{
									if( DAB >= 0 )
									{
										bInRightWay = nuTRUE;
										pRoadData->nDis		= tmpDis;
										pRoadData->nDwIdx	= (nuWORD)pNRBC->FileIdx;
										pRoadData->nBlkIdx	= pNRBC->NSS[i].BlockIdx;
										pRoadData->nRoadIdx	= pNRBC->NSS[i].RoadIdx;
										pRoadData->nCityID	= pNRBC->NSS[i].CityID;
										pRoadData->nTownID	= pNRBC->NSS[i].TownID;
										pRoadData->nPtIdx	= (nuWORD)(j-1);
										pRoadData->ptMapped	= ptFixed;
										pRoadData->nNameAddr	= pNRBC->NSS[i].RoadNameAddr;
										pRoadData->nRoadClass	= (nuWORD)pNRBC->NSS[i].RoadClass;
										pRoadData->nRdDx	= (short)dx;
										pRoadData->nRdDy	= (short)dy;
										pRoadData->bGPSState=pNRBC->NSS[i].bGPSState;
										pRoadData->nOneWay  = pNRBC->NSS[i].nOneWay;
										m_ptNavi.pNFTC	= pNFTC;
										m_ptNavi.pNRBC	= pNRBC;
										m_ptNavi.nSubIdx	= i;
										m_ptNavi.nPtIdx		= j-1;
										m_ptNavi.ptMapped	= ptFixed;
									}
								}//the angle diff is less than PI/2, right direction
								else
								{
									if( DAB < 0 && tmpDis <= roadAnti.nDis )
									{
										bInAntiWay = nuTRUE;
										roadAnti.nDis		= tmpDis;
										roadAnti.nDwIdx		= (nuWORD)pNRBC->FileIdx;
										roadAnti.nBlkIdx	= pNRBC->NSS[i].BlockIdx;
										roadAnti.nRoadIdx	= pNRBC->NSS[i].RoadIdx;
										roadAnti.nCityID	= pNRBC->NSS[i].CityID;
										roadAnti.nTownID	= pNRBC->NSS[i].TownID;
										roadAnti.nPtIdx		= (nuWORD)(j-1);
										roadAnti.ptMapped	= ptFixed;
										roadAnti.nNameAddr	= pNRBC->NSS[i].RoadNameAddr;
										roadAnti.nRoadClass	= (nuWORD)pNRBC->NSS[i].RoadClass;
										roadAnti.nOneWay	= pNRBC->NSS[i].nOneWay;
										pRoadData->nOneWay  = pNRBC->NSS[i].nOneWay;
										pRoadData->nRdDx	= -((short)dx);
										pRoadData->nRdDy	= -((short)dy);
										pRoadData->bGPSState=pNRBC->NSS[i].bGPSState;
										ptAnti.pNFTC	= pNFTC;
										ptAnti.pNRBC	= pNRBC;
										ptAnti.nSubIdx	= i;
										ptAnti.nPtIdx	= j-1;
										ptAnti.ptMapped	= ptFixed;
									}
								}//the angle diff is larger than PI/2, wrong direction
							}
							else
							{



								bInRightWay = nuTRUE;
								pRoadData->nDis		= tmpDis;
								pRoadData->nDwIdx	= (nuWORD)pNRBC->FileIdx;
								pRoadData->nBlkIdx	= pNRBC->NSS[i].BlockIdx;
								pRoadData->nRoadIdx	= pNRBC->NSS[i].RoadIdx;
								pRoadData->nCityID	= pNRBC->NSS[i].CityID;
								pRoadData->nTownID	= pNRBC->NSS[i].TownID;
								pRoadData->nPtIdx	= (nuWORD)(j-1);
								pRoadData->ptMapped	= ptFixed;
								pRoadData->nNameAddr	= pNRBC->NSS[i].RoadNameAddr;
								pRoadData->nRoadClass	= (nuWORD)pNRBC->NSS[i].RoadClass;
								pRoadData->nRdDx	= (short)dx;
								pRoadData->nRdDy	= (short)dy;
								pRoadData->bGPSState=pNRBC->NSS[i].bGPSState;
								pRoadData->nOneWay  = pNRBC->NSS[i].nOneWay;
								m_ptNavi.pNFTC	= pNFTC;
								m_ptNavi.pNRBC	= pNRBC;
								m_ptNavi.nSubIdx	= i;
								m_ptNavi.nPtIdx		= j-1;
								m_ptNavi.ptMapped	= ptFixed;
							}
					}
					//	pTempNSS = &(pNRBC->NSS[i]);	
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}	
	if ( bInRightWay || bInAntiWay )
	{
		nuDWORD nNSSIndex = 0;
		PNAVIRTBLOCKCLASS pTmpNRBC = NULL;
		nuLONG nPtIndex = 0;
		nuroPOINT ptTmp = {0};
		if ( bInRightWay )
		{
			nNSSIndex = m_ptNavi.nSubIdx;
			pTmpNRBC  = m_ptNavi.pNRBC;
			nPtIndex  = m_ptNavi.nPtIdx + 1;
			ptTmp.x   = m_ptNavi.ptMapped.x;
			ptTmp.y   = m_ptNavi.ptMapped.y;
			pRoadData->ptNextCoor = pTmpNRBC->NSS[nNSSIndex].ArcPt[pTmpNRBC->NSS[nNSSIndex].ArcPtCount - 1];
		}
		else if ( bInAntiWay )
		{
			nNSSIndex = ptAnti.nSubIdx;
			pTmpNRBC  = ptAnti.pNRBC;
			nPtIndex  = ptAnti.nPtIdx + 1;
			ptTmp.x   = ptAnti.ptMapped.x;
			ptTmp.y   = ptAnti.ptMapped.y;
			pRoadData->ptNextCoor = pTmpNRBC->NSS[nNSSIndex].ArcPt[0];
		}
        nuWORD nDisToNextCoor = 0;
        nuLONG ndx = pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex].x - ptTmp.x;
		nuLONG ndy = pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex].y - ptTmp.y;
        nDisToNextCoor = (nuLONG)nuSqrt(ndx * ndx + ndy * ndy);
		nPtIndex += 1;
		
		
		for ( ; nPtIndex < pTmpNRBC->NSS[nNSSIndex].ArcPtCount; nPtIndex ++ )
		{
			ndx = pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex].x - pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex - 1].x;
			ndy = pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex].y - pTmpNRBC->NSS[nNSSIndex].ArcPt[nPtIndex - 1].y ; 
			nDisToNextCoor += (nuLONG)nuSqrt(ndx * ndx + ndy * ndy);
		}
		
		if ( bInRightWay )
		{
			pRoadData->nDisToNextCoor = nDisToNextCoor;
		}
		else if ( bInAntiWay )
		{
			pRoadData->nDisToNextCoor =  pTmpNRBC->NSS[nNSSIndex].RoadLength - nDisToNextCoor;
		}
	}
	else
	{
		pRoadData->nDisToNextCoor = 0;
		pRoadData->ptNextCoor.x = 0;
		pRoadData->ptNextCoor.y = 0;
	}
	if( bInRightWay )
	{
		g_pInFsApi->pGdata->drawInfoNavi.bCarInNaviLine = 1;
		nRes = INNAVI_POINT_TO_NAVI_RIGHT;
	}
	else if( bInAntiWay )
	{
		*pRoadData	= roadAnti;
		//m_ptNavi = ptAnti;
		g_pInFsApi->pGdata->drawInfoNavi.bCarInNaviLine = 1;
		nRes = INNAVI_POINT_TO_NAVI_ANTIDIRT;
	}
	else
	{
		g_pInFsApi->pGdata->drawInfoNavi.bCarInNaviLine = 0;
		nRes = INNAVI_POINT_TO_NAVI_ERROR;
	}
	/*
	#if (defined(_DEBUG) || defined(DEBUG))
	nuPrintfStr(L"Dis = ");
	nuPrintfNum(pRoadData->nDis);
	nuPrintfStr(L",\n");
#endif*/
	return nRes;
}

#define CHECK_EXTEND_DIS				(2000*COORCONSTANT)
nuPVOID CNaviData::CheckScreenRoad(nuroRECT rtRange,nuBOOL bNow /*= nuFALSE*/)
{
	#ifndef LCMM
		nuINT pTemp = 0;
		nuBOOL bSaveThcLength = nuFALSE;//add by daniel for pass target ReRoute Issue 20100901
		if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
		{
			return nuFALSE;
		}
		m_nTHCTimer++;
		if ( nuFALSE == m_bFirstGetMRouteData &&( m_nTHCTimer > 6 || bNow == nuTRUE) )
		{
			if( nuFALSE == g_pInRTApi->RT_GetMRouteData( g_pInFsApi->pGdata->routeGlobal.routingData.NFTC ) )
			{
				g_pInRTApi->RT_ReleaseRoute(  &(g_pInFsApi->pGdata->routeGlobal.routingData) );
				return nuFALSE;
			}
			pTemp = 1;
			m_bFirstGetMRouteData = nuTRUE;
		}
		PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC; 
		PNAVIRTBLOCKCLASS pNRBC = NULL;
		nuroRECT rect = {0};
		++m_nMaxLocalTimers;

		
		#define MAX_ROUTE_NUM	2
		nuINT nRouteNum = 0;
		while( pNFTC != NULL )
		{
			pNRBC = pNFTC->NRBC;
			while( pNRBC != NULL )
			{
				//			if( pNRBC->NSS == NULL )
				if( IsRTBUnroute(pNRBC) )
				{
					if( pNRBC->FInfo.FixCoor.x > pNRBC->TInfo.FixCoor.x )
					{
						rect.left	= pNRBC->TInfo.FixCoor.x;
						rect.right	= pNRBC->FInfo.FixCoor.x;
					}
					else
					{
						rect.left	= pNRBC->FInfo.FixCoor.x;
						rect.right	= pNRBC->TInfo.FixCoor.x;
					}
					if( pNRBC->FInfo.FixCoor.y > pNRBC->TInfo.FixCoor.y )
					{
						rect.top	= pNRBC->TInfo.FixCoor.y;
						rect.bottom	= pNRBC->FInfo.FixCoor.y;
					}
					else
					{
						rect.top	= pNRBC->FInfo.FixCoor.y;
						rect.bottom	= pNRBC->TInfo.FixCoor.y;
					}
					rect.left	-= CHECK_EXTEND_DIS;
					rect.right	+= CHECK_EXTEND_DIS;
					rect.top	-= CHECK_EXTEND_DIS;
					rect.bottom	+= CHECK_EXTEND_DIS;
					if(  ( nuRectCoverRect(&rtRange, &rect) && g_pInFsApi->pGdata->uiSetData.nScaleIdx < 10 ) 
					  || m_nMaxLocalTimers >= 8 
					  || bNow == nuTRUE)
					{
						m_nMaxLocalTimers = 0;
	//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
	#ifdef VALUE_EMGRT
						if( pNRBC->NextRTB != NULL && pNRBC->NextRTB->RealDis == 0 && 
							pNRBC->NextRTB->RTBStateFlag == 1 && pNRBC->NextRTB->WeightDis == 0)
						{
							pNRBC->NextRTB->RealDis = pNRBC->RealDis;
							pNRBC->NextRTB->WeightDis = pNRBC->WeightDis;
							bSaveThcLength = nuTRUE;
						}
	#endif
	//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//					
						g_pInRTApi->RT_LocalRoute(&pNRBC);
	//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
	#ifdef VALUE_EMGRT
						if(bSaveThcLength)
						{
							pNRBC->NextRTB->RealDis -= pNRBC->RealDis;
							pNRBC->NextRTB->WeightDis -= pNRBC->WeightDis;
						}
	#endif
	//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
						
						AddLocalRouteData(pNRBC);
						++nRouteNum;
						FreeLocalRouteData();
						if ( nRouteNum >= MAX_ROUTE_NUM )
						{
							break;
						}
						pTemp = 1;
					}
				}
				pNRBC = pNRBC->NextRTB;
			}
			pNFTC = pNFTC->NextFT;
		}
	#else
		nuINT RouteCounter_LCMM = 0;
		nuINT pTemp = 0;
		while(RouteCounter_LCMM < g_pInFsApi->pGdata->routeGlobal.RouteCount)
		{
			if(g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[RouteCounter_LCMM] != NULL )
			{
				if(pTemp >= 1)
				{
					return nuFALSE;
				}
				pTemp++;
			}
			RouteCounter_LCMM++;
		}
		pTemp = 0;
		//while(RouteCounter_LCMM < g_pInFsApi->pGdata->routeGlobal.RouteCount)
		{
			nuBOOL bSaveThcLength = nuFALSE;//add by daniel for pass target ReRoute Issue 20100901
			if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]	== NULL )
			{
				return nuFALSE;
			}
			m_nTHCTimer++;
			if ( nuFALSE == m_bFirstGetMRouteData &&( m_nTHCTimer > 6 || bNow == nuTRUE) )
			{
				if( nuFALSE == g_pInRTApi->RT_GetMRouteData( g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0] ) )
				{
					g_pInRTApi->RT_ReleaseRoute(  &(g_pInFsApi->pGdata->routeGlobal.routingData) );
					return nuFALSE;
				}
				pTemp = 1;
				m_bFirstGetMRouteData = nuTRUE;
			}
			PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
			PNAVIRTBLOCKCLASS pNRBC = NULL;
			nuroRECT rect = {0};
			++m_nMaxLocalTimers;

			
			#define MAX_ROUTE_NUM	2
			nuINT nRouteNum = 0;
			while( pNFTC != NULL )
			{
				pNRBC = pNFTC->NRBC;
				while( pNRBC != NULL )
				{
					//			if( pNRBC->NSS == NULL )
					if( IsRTBUnroute(pNRBC) )
					{
						if( pNRBC->FInfo.FixCoor.x > pNRBC->TInfo.FixCoor.x )
						{
							rect.left	= pNRBC->TInfo.FixCoor.x;
							rect.right	= pNRBC->FInfo.FixCoor.x;
						}
						else
						{
							rect.left	= pNRBC->FInfo.FixCoor.x;
							rect.right	= pNRBC->TInfo.FixCoor.x;
						}
						if( pNRBC->FInfo.FixCoor.y > pNRBC->TInfo.FixCoor.y )
						{
							rect.top	= pNRBC->TInfo.FixCoor.y;
							rect.bottom	= pNRBC->FInfo.FixCoor.y;
						}
						else
						{
							rect.top	= pNRBC->FInfo.FixCoor.y;
							rect.bottom	= pNRBC->TInfo.FixCoor.y;
						}
						rect.left	-= CHECK_EXTEND_DIS;
						rect.right	+= CHECK_EXTEND_DIS;
						rect.top	-= CHECK_EXTEND_DIS;
						rect.bottom	+= CHECK_EXTEND_DIS;
						if(  ( nuRectCoverRect(&rtRange, &rect) && g_pInFsApi->pGdata->uiSetData.nScaleIdx < 10 ) 
						  || m_nMaxLocalTimers >= 8 
						  || bNow == nuTRUE)
						{
							m_nMaxLocalTimers = 0;
		//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
		#ifdef VALUE_EMGRT
							if( pNRBC->NextRTB != NULL && pNRBC->NextRTB->RealDis == 0 && 
								pNRBC->NextRTB->RTBStateFlag == 1 && pNRBC->NextRTB->WeightDis == 0)
							{
								pNRBC->NextRTB->RealDis = pNRBC->RealDis;
								pNRBC->NextRTB->WeightDis = pNRBC->WeightDis;
								bSaveThcLength = nuTRUE;
							}
		#endif
		//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//					
							g_pInRTApi->RT_LocalRoute(&pNRBC);
		//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
		#ifdef VALUE_EMGRT
							if(bSaveThcLength)
							{
								pNRBC->NextRTB->RealDis -= pNRBC->RealDis;
								pNRBC->NextRTB->WeightDis -= pNRBC->WeightDis;
							}
		#endif
		//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
							
							AddLocalRouteData(pNRBC);
							++nRouteNum;
							FreeLocalRouteData();
							if ( nRouteNum >= MAX_ROUTE_NUM )
							{
								break;
							}
							pTemp = 1;
						}
					}
					pNRBC = pNRBC->NextRTB;
				}
				pNFTC = pNFTC->NextFT;
			}
			//RouteCounter_LCMM++;
		}
	#endif
	return (nuPVOID)pTemp;
}
nuVOID CNaviData::AddLocalRouteData(PNAVIRTBLOCKCLASS pNRBC)
{
	if( pNRBC != NULL )
	{
		PLOCALROUTEDATA pTmp  = NULL;
		pTmp = new LOCALROUTEDATA;
		if( pTmp != NULL )
		{
			pTmp->pNext	= NULL;
			pTmp->pNRBC	= pNRBC;
			if( m_pLocalRouteData == NULL )
			{
				m_pLocalRouteData = pTmp;
			}
			else
			{
				PLOCALROUTEDATA pTmp1 = m_pLocalRouteData;
				while( pTmp1->pNext != NULL )
				{
					pTmp1 = pTmp1->pNext;
				}
				pTmp1->pNext = pTmp;
			}
		}
	}
}
nuVOID CNaviData::FreeLocalRouteData()
{
	PLOCALROUTEDATA pTmp = NULL;

	while( m_pLocalRouteData != NULL )
	{
		pTmp = m_pLocalRouteData->pNext;
		if ( m_pLocalRouteData->pNRBC->RTBStateFlag == 0 )
		{
			m_nFristTotalLen -= m_pLocalRouteData->pNRBC->RealDis;
			m_nFristTotalTime -= m_pLocalRouteData->pNRBC->WeightDis;
			//DrawNaviRoad( g_pInFsApi->pDrawInfo->pcZoomScreenData->bZoomScreen );
		}		
		delete m_pLocalRouteData;
		m_pLocalRouteData = pTmp;
	}
}
nuBOOL CNaviData::ColNaviInfo()
{
	//	ROUTINGLISTNODE RtList[1000];
	//	nuINT asdsa = GetRoutingList( RtList, 100, 0);
	/**/
	nuMemset(	g_pInFsApi->pGdata->drawInfoNavi.pRtList, 
		0, 
		g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum*sizeof(SHOWNODE) );
	//nuLONG nPreDisTotarget = g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget;
	//nuLONG nPreTime        = g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget;
     nuBOOL bSave = nuFALSE;
	 nuLONG nTmpFristTotalLen  = 0;
	 nuLONG nTmpFristTotalTime = 0;
	 nuBOOL bSaveTHCLength = nuTRUE;//add by daniel for pass target ReRoute Issue 20100901
	/*if ( nuFALSE == m_bFirstGetMRouteData )
	{
		m_nFristTotalLen = 0;
		m_nFristTotalTime = 0;
	}*/

	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget = 0;
	g_pInFsApi->pDrawInfo->drawInfoCommon.nRealPicID = 0; 
	m_bFindHighwayRealPic = nuFALSE;
	
	
	//	GetRTList();
	if( m_ptNavi.pNFTC == NULL || m_ptNavi.pNRBC == NULL )
	{
		return nuFALSE;
	}
	
	PNAVIFROMTOCLASS	pNFTC = m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS	pNRBC = NULL;
	
	nuMemset( &m_struSeachInfo, 0 ,sizeof(SEARCHJUDGEINFO));
	/**************add**************/
	m_struSeachInfo.nLenToNext		= 0;	//nuLONG	nLenToNext = 0;   //for next cross and citytown 
	m_struSeachInfo.bFindNextCross	= nuFALSE;
	m_struSeachInfo.bFindNextTown		= nuFALSE;
	m_struSeachInfo.nFindCrossNum		= 0;
	m_struSeachInfo.bIngore			= nuFALSE;
	m_nLenSlowWayAdd	= 0;
	m_nLenHighWayAdd	= 0;
	m_nLenRTB			= 0;
	/**********add by wenjun*********/
	nuLONG nSpeed = 0;
	
	nuLONG dx = 0, dy = 0, dxy = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget	= 0;
	while(pNFTC != NULL )
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		//
		while( pNRBC != NULL )
		{
			ChangNCCName( pNFTC, pNRBC );
			nuLONG i;
			if( pNRBC == m_ptNavi.pNRBC )
			{
				i = m_ptNavi.nSubIdx;
				g_pInFsApi->pGdata->drawInfoNavi.nRoadState = pNRBC->NSS[i].RoadVoiceType;
				dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
				dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
				dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
				if(m_bFirstCalDis)
				{//added by daniel
					CalMinAndMaxCoor_D(pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1]);
				}
				//g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget += dxy;
				//m_struSeachInfo.nLenToNext += dxy;
				nuLONG nDisTotal = 0;
				nuLONG nDisNowToEnd = dxy;
				for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
				{
					dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
					dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
					if (  j >= m_ptNavi.nPtIdx + 1 )
					{	
						nDisNowToEnd += dxy;
					}
					nDisTotal += dxy;
					if(m_bFirstCalDis)
					{
						CalMinAndMaxCoor_D(pNRBC->NSS[i].ArcPt[j]);
					}
					//g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget += dxy;
					//m_struSeachInfo.nLenToNext += dxy;
				}
				if ( nDisTotal )
				{
					nDisNowToEnd = nDisNowToEnd * pNRBC->NSS[i].RoadLength / nDisTotal;
				}
				NAVINSS NowNSSInfo = {0},NextNSSInfo = {0},SPTNSSInfo = {0};;
				NowNSSInfo.pNFTC = pNFTC;
				NowNSSInfo.pNRBC = pNRBC;
				NowNSSInfo.pNSS  = &m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx];
				NowNSSInfo.nIndex = m_ptNavi.nSubIdx;
				nuDWORD nPreDis = 0;
#ifdef ECI
				//JudgeEci3DPic(NowNSSInfo, nDisNowToEnd);
				if(NowNSSInfo.pNSS->RoadClass <= 3 || NowNSSInfo.pNSS->RoadClass == 5 )
				{
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = JudgeDraw3DPic(NowNSSInfo, nDisNowToEnd, SPTNSSInfo);
				}
				else
				{
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = JudgeDraw3DPic(NowNSSInfo, nDisNowToEnd ,SPTNSSInfo);
					if(!ColNextDis(SPTNSSInfo, nPreDis))
					{//End Node Show Pic
						nPreDis = 500;
					}
					if( nPreDis < 500 )
					{//Next TurnFlag Dis < 500 No Show Pic
						g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = 0;
					}
				}
#endif
#ifdef VALUE_EMGRT
				if(GetNextNSS(NowNSSInfo, &NextNSSInfo))
				{
					if(NowNSSInfo.pNSS->RoadVoiceType < 4 && NextNSSInfo.pNSS->RoadVoiceType < 4 && NowNSSInfo.pNSS->CrossCount > 1)
					{
						m_bSoundAfore = nuTRUE;
					}
				}
#endif
           /*#ifndef VALUE_EMGRT
				NAVINSS NowNSSInfo = {0},NextNSSInfo = {0};
				NowNSSInfo.pNFTC = pNFTC;
				NowNSSInfo.pNRBC = pNRBC;
				NowNSSInfo.pNSS  = &m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx];
				NowNSSInfo.nIndex = m_ptNavi.nSubIdx;
				g_pInFsApi->pGdata->drawInfoNavi.RealPicID = CalDraw3DPic(NowNSSInfo, nDisNowToEnd);
				m_nNowDis = nDisNowToEnd;
				if(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass < 4)
				{
					g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = g_pInFsApi->pGdata->drawInfoNavi.RealPicID;
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID    = 0;
				}
				else
				{
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = g_pInFsApi->pGdata->drawInfoNavi.RealPicID;
					g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = 0;
				}
            #endif*/
				g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget = nDisNowToEnd;
				m_struSeachInfo.nLenToNext = nDisNowToEnd;
				/***********************************add *************************************/
				if(g_pInFsApi->pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION)
				{
					nSpeed = nuLONG((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed)*0.2778); // 1 km/h = 0.2778 m/s 			
					//g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget +=  m_struSeachInfo.nLenToNext/nSpeed;
					if (   SPEEDADDTIMES > 0 
						&& g_pInFsApi->pDrawInfo->pcCarInfo->nMiddleSpeed > SPEEDADDTIMES )
					{
						if (   NURO_ABS(m_nNowSpeed - g_pInFsApi->pDrawInfo->pcCarInfo->nMiddleSpeed ) >= 5 )
						{
							nuLONG nTmpSpeed = (m_nNowSpeed  * (SPEEDADDTIMES - 1) + g_pInFsApi->pDrawInfo->pcCarInfo->nMiddleSpeed ) / SPEEDADDTIMES;
							if( m_nNowSpeed == nTmpSpeed )
							{
								m_nNowSpeed =  g_pInFsApi->pDrawInfo->pcCarInfo->nMiddleSpeed;
							}
							else
							{
								m_nNowSpeed = nTmpSpeed;
							}
						}
						else
						{
							m_nNowSpeed =  g_pInFsApi->pDrawInfo->pcCarInfo->nMiddleSpeed;
						}
						if ( m_nNowSpeed != 0) 
						{
							g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget +=  m_struSeachInfo.nLenToNext * 36 / ( m_nNowSpeed * 10);
						}
					}
					else
					{
						nSpeed = nuLONG((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed) ); // 1 km/h = 0.2778 m/s 
						g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget +=  m_struSeachInfo.nLenToNext * 36 / ( nSpeed * 10);
					}					
				}
				m_struSeachInfo.index = i;
				m_struSeachInfo.pNFTC = pNFTC;
				m_struSeachInfo.pNRBC = pNRBC;
				CombinedSeach( &m_struSeachInfo );
				
				i++;
				while( i < (nuLONG)pNRBC->NSSCount )
				{
					nuLONG lDis = 0;
					/*for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
					{
						dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
						dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
						dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
						lDis += dxy;
						if(m_bFirstCalDis)
						{
							CalMinAndMaxCoor_D(pNRBC->NSS[i].ArcPt[j]);//added by daniel
						}
					}*/
					#ifdef VALUE_EMGRT
					g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget += pNRBC->NSS[i].RoadLength;//2012.10.22 
					#endif
					m_struSeachInfo.nLenToNext += pNRBC->NSS[i].RoadLength;
					/**************************************add***********************************/
					if(g_pInFsApi->pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION)
					{
						nSpeed = (nuLONG)((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed)*0.2778);  
						g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget += pNRBC->NSS[i].RoadLength/nSpeed;
						if ( pNRBC->NSS[i].RoadClass <= 4 )
						{
							m_nLenHighWayAdd +=  pNRBC->NSS[i].RoadLength;//lDis;
						}
						else
						{
							m_nLenSlowWayAdd += pNRBC->NSS[i].RoadLength;//lDis;
						}
					}
					m_struSeachInfo.index = i;
					m_struSeachInfo.pNFTC = pNFTC;
					m_struSeachInfo.pNRBC = pNRBC;
					CombinedSeach( &m_struSeachInfo );					
					/*****************************add by wenjun******************************/					
					i ++;
				}
			}
			else
			{
			#ifdef VALUE_EMGRT
				g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget +=  pNRBC->RealDis;
				nSpeed = (nuLONG)( (m_nSlowSpeed + m_nHighSpeed) / 2 *0.2778); 
				g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget +=  pNRBC->RealDis/nSpeed;
			#endif
				/**************************************add*******************************/
				i = 0;
				if( pNRBC->NSSCount == 0 )
				{
					//?°ç®?å°?¶éŽ?è®¡ç®å?äœè?è¡ï?ïŒæ??¶æ?é«éè·¯?éåºŠè¿è??¢ç?
					nSpeed = (nuLONG)( (m_nSlowSpeed + m_nHighSpeed) / 2 *0.2778); 
				#ifndef VALUE_EMGRT
					g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget += pNRBC->RealDis/nSpeed;
				#endif
					//m_nLenRTB += pNRBC->RealDis;
				#ifdef VALUE_EMGRT
					/*if(  nuFALSE == m_bFirstGetMRouteData && pNRBC->RTBStateFlag == 2 )
					{
						bSave = nuTRUE;
					}
					if ( nuFALSE == m_bFirstGetMRouteData )
					{
						nTmpFristTotalLen += pNRBC->RealDis;
						nTmpFristTotalTime += pNRBC->WeightDis;
					}*/
				#endif
					if(m_bFirstCalDis)
					{
						CalMinAndMaxCoor_D(pNRBC->FInfo.FixCoor);//addded by daniel
						CalMinAndMaxCoor_D(pNRBC->TInfo.FixCoor);
					}				
					m_struSeachInfo.nLenToNext += pNRBC->RealDis;
					m_struSeachInfo.index = i;
					m_struSeachInfo.pNFTC = pNFTC;
					m_struSeachInfo.pNRBC = pNRBC;
					CombinedSeach( &m_struSeachInfo );
					i++;					
				}
				else
				{
					if ( pNRBC->RTBStateFlag == 0 )
					{
						#ifdef VALUE_EMGRT
						//g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget += pNRBC->RealDis;//2012.10.22
						#endif
						if(m_bFirstCalDis)
						{
							CalMinAndMaxCoor_D(pNRBC->FInfo.FixCoor);//addded by daniel
							CalMinAndMaxCoor_D(pNRBC->TInfo.FixCoor);
						}
					}
					while( i < (nuLONG)pNRBC->NSSCount && &(pNRBC->NSS[i] ) )
					{
						nuLONG lDis = 0;
						for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
						{
							dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
							dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
							lDis += dxy;
							if(m_bFirstCalDis)
							{
								CalMinAndMaxCoor_D(pNRBC->NSS[i].ArcPt[j]);//added by daniel
							}
						}
						if (g_pInFsApi->pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION)
						{
						#ifndef VALUE_EMGRT
							nSpeed = (nuLONG)((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed)*0.2778);  
							g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget += pNRBC->NSS[i].RoadLength/nSpeed;
						#endif
							/**/if ( pNRBC->NSS[i].RoadClass <= 4 )
							{
								m_nLenHighWayAdd +=  pNRBC->NSS[i].RoadLength;//lDis;
							}
							else
							{
								m_nLenSlowWayAdd += pNRBC->NSS[i].RoadLength;//lDis;
							}
						}
						m_struSeachInfo.nLenToNext += pNRBC->NSS[i].RoadLength;//lDis;
						m_struSeachInfo.index = i;
						m_struSeachInfo.pNFTC = pNFTC;
						m_struSeachInfo.pNRBC = pNRBC;
						CombinedSeach( &m_struSeachInfo );
						i++;	
					}
//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
#ifdef VALUE_EMGRT
					if(bSaveTHCLength)
					{
						if( pNRBC->NextRTB != NULL && pNRBC->RTBStateFlag == 1 && pNRBC->NextRTB->RealDis == 0 && 
							pNRBC->NextRTB->RTBStateFlag == 1 && pNRBC->NextRTB->WeightDis == 0)
						{
							nTmpFristTotalLen += pNRBC->RealDis;
							nTmpFristTotalTime += pNRBC->WeightDis;
							bSaveTHCLength = nuFALSE;
						}
					}
#endif
//----------------------add by daniel for pass target ReRoute Issue 20100901-------------------------------//
				}				
				/*****************************add by wenjun*************************/
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
		bSaveTHCLength = nuTRUE;//add by daniel for pass target ReRoute Issue 20100901
	}
	m_bFirstCalDis = nuFALSE;
#ifdef VALUE_EMGRT
	/*if( bSave || ( m_bFirstGetMRouteData == nuFALSE && bSave == nuFALSE ))
	{
		m_nFristTotalLen = nTmpFristTotalLen;
		m_nFristTotalTime = nTmpFristTotalTime;
	}*/
#else//2012.10.22
	g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget = m_struSeachInfo.nLenToNext;//2012.10.22
#endif
	/*if (m_nFristTotalLen > 0 )
	{
		g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget += m_nFristTotalLen;
	}*/
	
	if( g_pInFsApi->pGdata->timerData.nAutoMoveType == AUTO_MOVE_SIMULATION )
	{
		nuLONG nSpeed = g_pInFsApi->pUserCfg->nSimSpeed;
		if ( nSpeed != 0 )
		{
			g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget = g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget  * 36 /(nSpeed * 10)  ;
		}	
	}
	else
	{
		/*if ( m_nFristTotalTime > 0 )
		{
			g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget += m_nFristTotalTime;
		}
		g_pInFsApi->pGdata->drawInfoNavi.nTimeToTarget +=  m_nLenHighWayAdd * 36 / ( m_nHighSpeed * 10) 
			+ m_nLenSlowWayAdd * 36 / ( m_nSlowSpeed * 10)  ; */ 
		//+ m_nLenRTB * 36 / ( m_nHighSpeed * 10) ;
	}	
	
	/*if( JudgeDraw3DPic() )
	{
		g_pInFsApi->pDrawInfo->drawInfoCommon.nRealPicID = m_struSeachInfo.n3DPic;
	}*/

	JudegeLaneGuideInfo();

	GetHighWayList();
	GetHighExternList();
	
    
	if (   m_struSeachInfo.nDisToNextNFTC < 200 
		&& !bSimMode() )
	{
	#ifdef LCMM
		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	#else
		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC; 
	#endif
		while( pTmpNFTC && pTmpNFTC->RunPass )
		{
			pTmpNFTC = pTmpNFTC->NextFT;
		}
		if (  pTmpNFTC->NextFT)
		{
			nuLONG nDx = m_ptNavi.ptMapped.x - pTmpNFTC->NextFT->NRBC->NSS[0].ArcPt[0].x ;
			nuLONG nDy = m_ptNavi.ptMapped.y - pTmpNFTC->NextFT->NRBC->NSS[0].ArcPt[0].y ;
			if ( nuSqrt( nDx * nDx + nDy * nDy ) < 200 )
			{
				if ( pTmpNFTC )
				{
					pTmpNFTC->RunPass = nuTRUE;		
					g_pInFsApi->pUserCfg->nPastMarket++;
				}
			}
		}
		
	}
	//Set3DPAfterWork();
	//JudgeRoadType( &g_pInFsApi->pGdata->drawInfoMap.roadCenterOn );
    /*PlayNaviSound( );
	
	  JudgeZoomRoad( );
	  nuINT nBackValue = g_pInFsApi->pGdata->drawInfoNavi.nCrossNearType; 
	  if( nBackValue != 0)
	  {
	  char ss[10] = {0};
	  nuItoa( nBackValue, ss, 10 );
	  //nuShowString(ss);
	  }
	  nuINT i = GetCarAngle();
	  char ss[10] = {0};
	  nuItoa( i, ss, 10 );
	  //MessageBox(NULL,ss,"",0);
	  char ww[10] = {0};
	  nuItoa( m_nNowLen , ww, 10 );
	//nuShowString(ww);*/
	return nuTRUE;
}

/* nType meanse the list type, pass node list, crossing list or mixture list. */
nuLONG CNaviData::GetRoutingListCode(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS)
{
	if( !pNowNSS || !pNextNSS )
	{
		return AF_LoseHere;
	}
	nuLONG nRet = 0;
#ifdef VALUE_EMGRT
	nRet	= GetRoutingListCodeEMG( pNowNSS, pNextNSS );
#else
	nRet	= GetRoutingListCodeNew( pNowNSS, pNextNSS );
#endif
	if(   ( nRet == AF_Out_Freeway || nRet == AF_Out_Overpass )
		&& pNowNSS->CrossCount == 1 )
	{
		nRet = AF_LoseHere;
	}	
	if (   pNowNSS->RoadVoiceType != VRT_TrafficCircle 
		&& pNextNSS->RoadVoiceType == VRT_TrafficCircle  )
	{
		nRet = AF_In_TrafficCircle;
	}
	if (   pNowNSS->RoadClass <= 3 
		&& pNextNSS->RoadClass <= 3
		&& pNowNSS->TurnFlag == 0 )
	{
		if (   nRet != AF_In_Tollgate
			&& nRet != AF_In_Tunnel 
			)
		{
			nRet = AF_LoseHere; 			
		}
	}
	if ( pNowNSS->RoadClass == 4 
		&& pNextNSS->RoadClass == 4 )
	{
		if ( nRet == AF_Out_Tunnel )
		{
			nRet = AF_LoseHere; 
		}
	}
	
	return nRet;
	
}

nuBOOL CNaviData::ColNextCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo,nuLONG nLenToNextCross)
{
	nuLONG xx = 0;
	NAVINSS NSSInfo;
	nuBOOL b_SameRoad = nuFALSE;
	nuMemset( &NSSInfo, 0 ,sizeof(NSSInfo));
	if( NowNSSInfo.pNSS == NULL || NowNSSInfo.pNRBC == NULL || NowNSSInfo.pNFTC == NULL )
	{
		return nuFALSE;
	}
	if ( NextNSSInfo.pNSS )
	{
		if (   NowNSSInfo.pNSS->RoadVoiceType  == VRT_TrafficCircle 
			&& NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle   )
		{
			return nuFALSE;
		}
	}
	
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDwIdx  = (nuUSHORT)NowNSSInfo.pNRBC->FileIdx ;
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nCityID = NowNSSInfo.pNSS->CityID;
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nTownID = NowNSSInfo.pNSS->TownID;
	if( NextNSSInfo.pNSS == NULL && NextNSSInfo.pNRBC == NULL ) 
	{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance = nLenToNextCross;
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1024 + AF_EndSite * 10;
		g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = T_Afore; //2012/10/22 HUD CallBack
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.point.x = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1].x;
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.point.y = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1].y;
		//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
		nuWCHAR wc[20];
		nuMemset( wc, 0, sizeof(wc) );
		g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName, MAX_LISTNAME_NUM);
		return nuTRUE;
	}
	else
	{
		if( NowNSSInfo.pNFTC != NextNSSInfo.pNFTC )  //Pass Node
		{
			g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance  = nLenToNextCross;
			g_pInFsApi->pGdata->drawInfoNavi.nextCross.point	  = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1];//NextNSSInfo.pNSS->ArcPt[0];
			g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  =  nuWORD(1024 + AF_MidSit * 10);
			g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = T_Afore; //2012/10/22 HUDCallBack
			//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
			g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName, MAX_LISTNAME_NUM);
			
			return nuTRUE;
		}
		else if( NextNSSInfo.pNSS == NULL )  //æ­€åº?è??ªè??å¥œ
		{
			g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance = nLenToNextCross;
			g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  = nuWORD(1024 + AF_MapBoundary * 10 + NowNSSInfo.pNSS->TurnFlag);
			g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = T_Afore; //2012/10/22 HUDCallBack
			GetRTBName( NextNSSInfo.pNRBC->FileIdx, 
				NextNSSInfo.pNRBC->FInfo.RTBID, 
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName );
			CheckScreenRoad( g_pInFsApi->pGdata->transfData.nuShowMapSize, nuTRUE );
			return nuFALSE;
		}
		else
		{
		 #ifdef ZENRIN
			if(NextNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType != VRT_ROADCROSS)
			{
				if ( NowNSSInfo.pNSS->TurnFlag <= T_Afore )
				{
					m_pTempNSSInfo.pNSS = NowNSSInfo.pNSS;
					m_pTempNSSInfo.pNRBC = NowNSSInfo.pNRBC;
					m_pTempNSSInfo.nIndex = NowNSSInfo.nIndex;
					m_pTempNSSInfo.pNFTC = NowNSSInfo.pNFTC;
					m_bRoadCross = nuTRUE;
					return nuFALSE;
				}
			}
			if(NextNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS)
			{
				if ( NowNSSInfo.pNSS->TurnFlag <= T_Afore )
				{
					return nuFALSE;
				}
			}
			if(NextNSSInfo.pNSS->RoadVoiceType != VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS)
			{
				if (m_bRoadCross && NowNSSInfo.pNSS->TurnFlag <= T_Afore)
				{
					NowNSSInfo.pNSS = m_pTempNSSInfo.pNSS;
					NowNSSInfo.pNRBC = m_pTempNSSInfo.pNRBC;
					NowNSSInfo.nIndex = m_pTempNSSInfo.nIndex;
					NowNSSInfo.pNFTC = m_pTempNSSInfo.pNFTC;
					m_bRoadCross = nuFALSE;
					b_SameRoad = nuTRUE;
				}
				else if(NowNSSInfo.pNSS->TurnFlag <= T_Afore)
				{
					return nuFALSE;
				}
			}
         #endif
			xx = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
         #ifdef SameRoad
			if (xx == AF_Out_Fastway)
			{
				if (NowNSSInfo.pNSS->RoadLength <= SAMEROADLEN  && NowNSSInfo.pNSS->RoadClass  == 4 &&
					NowNSSInfo.pNSS->RoadClass == NextNSSInfo.pNSS->RoadClass )
				{
					GetNextNSS(NextNSSInfo,&NSSInfo);
						if( NSSInfo.pNSS == NULL || NSSInfo.pNRBC == NULL || NSSInfo.pNSS == NULL  )
						{
							return nuFALSE;
						}
						if (NowNSSInfo.pNSS->RoadClass == NSSInfo.pNSS->RoadClass)
						{
							xx = 0;
						}
				}
			}
			if (NowNSSInfo.pNSS->RoadLength <= m_nSameRoadLen  && 
				(NowNSSInfo.pNSS->RoadClass == 6 || NowNSSInfo.pNSS->RoadClass == 7 || NowNSSInfo.pNSS->RoadClass == 9) && 
				NowNSSInfo.pNSS->RoadClass == NextNSSInfo.pNSS->RoadClass && NowNSSInfo.pNSS->TurnFlag <= 1 )
			{
				nuWCHAR  wsNowRoad[MAX_LISTNAME_NUM] = {0};//
				nuWCHAR  wsNextRoad[MAX_LISTNAME_NUM] = {0};//
				RNEXTEND RnEx = {0};
				g_pInFsApi->FS_GetRoadName( NextNSSInfo.pNRBC->FileIdx, NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, wsNextRoad, MAX_LISTNAME_NUM, &RnEx);
				g_pInFsApi->FS_GetRoadName( NowNSSInfo.pNRBC->FileIdx, NowNSSInfo.pNSS->RoadNameAddr, NowNSSInfo.pNSS->RoadClass, wsNowRoad, MAX_LISTNAME_NUM, &RnEx);
				if (nuWcscmp(wsNowRoad, wsNextRoad) == 0)
				{
					if (xx == AF_IN_FASTROAD || xx == AF_IN_SLOWROAD)
					{
						xx = 0;
					}
				}
			}
         #endif
			if ( NowNSSInfo.pNSS->RoadClass <= 3 )
			{
				if (   xx == AF_In_Tollgate
					|| xx == AF_In_Tunnel 
					|| xx == AF_Out_Tunnel)
				{
					xx = AF_LoseHere; 			
				}
			}
			if(xx == AF_LoseHere)
			{
				if( NowNSSInfo.pNSS->TurnFlag != T_No && bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ) )
				{
				#ifdef ZENRIN
					if ( b_SameRoad && NowNSSInfo.pNSS->TurnFlag <= T_Afore)
					{
						return nuFALSE;
					}
				#endif
                #ifdef SameRoad
					if (NowNSSInfo.pNSS->TurnFlag == T_Afore || NowNSSInfo.pNSS->TurnFlag == T_LeftSide || NowNSSInfo.pNSS->TurnFlag == T_RightSide )
					{
						if (JudgeSameRoadName(NowNSSInfo, NextNSSInfo)
				#ifdef ECI
							&& NowNSSInfo.pNSS->Real3DPic == 0 
				#endif
							)
						{//shyanx
							return nuFALSE;
						}
					}
                #endif
					g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = GetTurnFlagNum(xx,NowNSSInfo.pNSS);//2012/10/22 HUDCallBack
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance = nLenToNextCross;
					GetCrossData(g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName,NowNSSInfo.pNSS,NextNSSInfo.pNSS, xx, NowNSSInfo.pNRBC);
					return nuTRUE;
					
				}
				else if( NowNSSInfo.pNSS->TurnFlag == T_No && NowNSSInfo.pNSS->Real3DPic == 0) 
				{
					return nuFALSE;
				}
			}
			else if(  NowNSSInfo.pNSS->RoadClass > 3  
				|| NextNSSInfo.pNSS->RoadClass > 3  
				|| bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ) )	 
			{
			#ifdef SameRoad
				if (xx == AF_IN_FASTROAD || xx == AF_IN_SLOWROAD)
				{
					if (NowNSSInfo.pNSS->TurnFlag == T_LeftSide || NowNSSInfo.pNSS->TurnFlag == T_RightSide )
					{
						if (JudgeSameRoadName(NowNSSInfo, NextNSSInfo)
			#ifdef ECI
							&& NowNSSInfo.pNSS->Real3DPic == 0
			#endif
							)
						{//shyanx
							return nuFALSE;
						}
					}
				}
			#endif
				g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = GetTurnFlagNum(xx,NowNSSInfo.pNSS);//2012/10/22 HUDCallBack
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance = nLenToNextCross;
				GetCrossData(g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName,NowNSSInfo.pNSS,NextNSSInfo.pNSS, xx,NowNSSInfo.pNRBC);
				return nuTRUE;
			}
		}
	}

#ifndef ECI
    if ( NowNSSInfo.pNSS->Real3DPic != 0 )//&& NowNSSInfo.pNSS->RoadClass > 3 )
	
    {
		if (   NowNSSInfo.pNSS->RoadClass <= 3 
			&& 0 == m_nShowHighWayPic
			)
		{
			return nuFALSE;
		}
		NAVINSS TmpNSS;
		nuBOOL bSearch = nuFALSE;
		nuMemset( &TmpNSS, 0 ,sizeof(TmpNSS) );
		if ( !GetNextNSS( NowNSSInfo, &TmpNSS ) )
		{
			return nuFALSE;
		}

		if (   TmpNSS.pNSS != NULL 
			&& TmpNSS.pNSS->Real3DPic != NowNSSInfo.pNSS->Real3DPic
			&& TmpNSS.pNSS->ArcPtCount >= 2 )
		{
			SOUNDPLAY *pSoundData = NULL;
			if ( m_struSeachInfo.nFindCrossNum == 0 )
			{
				pSoundData = &m_struSeachInfo.SoundPlayFirst;
			}
			else if( m_struSeachInfo.nFindCrossNum == 1 )
			{
				pSoundData = &m_struSeachInfo.SoundPlayNext;
			}
			else
			{
				return nuFALSE;
			}
			RNEXTEND RnEx = {0};
			if ( nuFALSE == m_struSeachInfo.bFindNextCross )
			{

				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance  = nLenToNextCross;
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.point	  = NextNSSInfo.pNSS->ArcPt[0];
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  = 1024 + T_Afore;
				g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag = T_Afore;//2012/10/22 HUDCallBack
				if ( NextNSSInfo.pNSS->PTSNameAddr > 4  )
				{
					bSearch = !GetPTSName((nuUSHORT)NextNSSInfo.pNRBC->FileIdx, 
											NextNSSInfo.pNSS->PTSNameAddr, 
											g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName,
											MAX_LISTNAME_NUM *sizeof(nuWCHAR));
					pSoundData->nChangeCode = xx;
				}
				if ( bSearch )
				{
					g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx, 
					NextNSSInfo.pNSS->RoadNameAddr,
					NextNSSInfo.pNSS->RoadClass, 
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName,
					sizeof(g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName),
					&RnEx);
					pSoundData->nChangeCode = 0;
				}
			}

			pSoundData->nDistance = nLenToNextCross;
			pSoundData->nTurning = T_Afore;//è¯·çŽè¡?
			if (   NextNSSInfo.pNRBC->FileIdx     != NowNSSInfo.pNRBC->FileIdx
				|| NextNSSInfo.pNSS->RoadNameAddr != NowNSSInfo.pNSS->RoadNameAddr 
				|| NextNSSInfo.pNSS->RoadClass    != NowNSSInfo.pNSS->RoadClass )
			{
				if ( NextNSSInfo.pNSS->RoadNameAddr != -1 )
				{
					g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx, 
						NextNSSInfo.pNSS->RoadNameAddr, 
						NextNSSInfo.pNSS->RoadClass, 
						pSoundData->wsRoadName, 
						sizeof(pSoundData->wsRoadName), 
						&RnEx );
				}
			}
			pSoundData->NaviInfo = NowNSSInfo;
			(m_struSeachInfo.nFindCrossNum)++;
			return nuTRUE;
		}
	}
#endif
	return nuFALSE;
}

nuBOOL CNaviData::GetCrossData(nuWCHAR *wsName, PNAVISUBCLASS pNowNSS,PNAVISUBCLASS pNextNSS, nuLONG nChangCode,PNAVIRTBLOCKCLASS pNRBC)
{	
	if( pNowNSS->RoadVoiceType == VRT_TrafficCircle && pNextNSS->RoadVoiceType == VRT_TrafficCircle ) 
	{
		return nuFALSE;
	}
	RNEXTEND RnEx = {0};
	nuBOOL bSearch = nuTRUE;
	if ( pNextNSS->PTSNameAddr > 4  )
	{
		bSearch = !GetPTSName((nuUSHORT)pNRBC->FileIdx, pNextNSS->PTSNameAddr, wsName, MAX_LISTNAME_NUM *sizeof(nuWCHAR));
	}
	if ( bSearch )
	{
		g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, 
			pNextNSS->RoadNameAddr, 
			pNextNSS->RoadClass, 
			wsName, 
			MAX_LISTNAME_NUM *sizeof(nuWCHAR), 
			&RnEx);
		if (   pNowNSS->RoadClass <= 3 
			&& pNextNSS->RoadClass > 3 
			&& pNowNSS->RoadNameAddr == pNextNSS->RoadNameAddr )
		{
		#ifdef  VALUE_EMGRT
			AddExternStr( nChangCode, wsName, MAX_LISTNAME_NUM );
		#endif
		}
	}
	
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.point	  = pNextNSS->ArcPt[0] ;
	if ( ChangeIcon( nChangCode ) )
	{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  = nuWORD(1024 + nChangCode );
	}
	else
	{
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  = nuWORD(1024 + nChangCode * 10 + pNowNSS->TurnFlag);
	}
	
	//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = pNowNSS->Real3DPic;
    /*
	if( pNowNSS->RoadLength <= CROSSLEN )
	{
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleIn =  (short)nulAtan2( pNowNSS->ArcPt[pNowNSS->ArcPtCount - 1].y - pNowNSS->ArcPt[0].y, pNowNSS->ArcPt[pNowNSS->ArcPtCount - 1].x - pNowNSS->ArcPt[0].x );
	}
	else
	{
	nuLONG nTotallen = 0;
	for ( nuINT i = pNowNSS->ArcPtCount - 1; i >= 1; i-- )
	{
	nTotallen += nulSqrt( (pNowNSS->ArcPt[i].x - pNowNSS->ArcPt[i - 1].x), (pNowNSS->ArcPt[i].y - pNowNSS->ArcPt[i - 1].y) );
	if( nTotallen >= CROSSLEN )
	{
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleIn = (short)nulAtan2( pNowNSS->ArcPt[pNowNSS->ArcPtCount - 1].y - pNowNSS->ArcPt[i-1].y, pNowNSS->ArcPt[pNowNSS->ArcPtCount - 1].x - pNowNSS->ArcPt[i-1].x );			
				break;
				}
				}	
}*/
	if( pNextNSS->RoadLength <= CROSSLEN )
	{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleOut = (short) nulAtan2( pNextNSS->ArcPt[pNextNSS->ArcPtCount - 1].y - pNextNSS->ArcPt[0].y, pNextNSS->ArcPt[pNextNSS->ArcPtCount - 1].x - pNextNSS->ArcPt[0].x );
	}
	else
	{
		nuLONG nTotallen = 0;
		for ( nuINT i = pNextNSS->ArcPtCount - 1; i >= 1; i-- )
		{
			nTotallen += nulSqrt( (pNextNSS->ArcPt[i].x - pNextNSS->ArcPt[i - 1].x), (pNextNSS->ArcPt[i].y - pNextNSS->ArcPt[i - 1].y) );
			if( nTotallen >= CROSSLEN )
			{
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleOut = (short)nulAtan2( pNextNSS->ArcPt[i].y - pNextNSS->ArcPt[0].y, pNextNSS->ArcPt[i].x - pNextNSS->ArcPt[0].x );
				break;
			}
		}	
	}
	
	return nuTRUE;
}

nuBOOL CNaviData::ColNextCityTown(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS, nuLONG nLenToNextTown)



{
	if( pNowNSS == NULL || pNextNSS == NULL )
	{
		return nuFALSE;
	}
	if(pNowNSS->CityID == pNextNSS->CityID && pNowNSS->TownID == pNextNSS->TownID)
	{
		return nuFALSE;
	}
	g_pInFsApi->pGdata->drawInfoNavi.nextCityTown.point     = pNextNSS->ArcPt[0] ;
	g_pInFsApi->pGdata->drawInfoNavi.nextCityTown.nDistance = nLenToNextTown;
	if(nLenToNextTown < 100)
	{
		//nuShowString("city");
	}
	return nuTRUE;
}

nuVOID CNaviData::GetRTList()
{
	#ifdef LCMM
		PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	#else
		PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC; 
	#endif
	SHOWNODE tmpnode = {0}; 
	nuLONG nLenToLast = 0;
	nuLONG nNodeLen = 0;
	nuLONG nType = AF_LoseHere;
	nuBYTE nTraffNum = 0;
	if( pNFTC == NULL)
	{
		return ;
	}
	NAVINSS NowNSSInfo,NextNSSInfo;
	nuMemset( &NowNSSInfo, 0, sizeof(NowNSSInfo) );
	nuMemset( &NextNSSInfo, 0, sizeof(NextNSSInfo) );
	while ( pNFTC != NULL)
	{
		PNAVIRTBLOCKCLASS pNRBC = pNFTC->NRBC;
		while ( pNRBC != NULL) 
		{
			if( pNRBC->NSS != NULL )
			{
				for( nuDWORD i = 0; i < pNRBC->NSSCount; i ++ )
				{	
					nNodeLen   += pNRBC->NSS[i].RoadLength; 
					NowNSSInfo.pNFTC = pNFTC;
					NowNSSInfo.pNRBC = pNRBC;
					NowNSSInfo.pNSS		= &(pNRBC->NSS[i]);
					NowNSSInfo.nIndex	= i;
					GetNextNSS( NowNSSInfo,&NextNSSInfo );
					if( ( NextNSSInfo.pNSS == NULL ) )
						break;
					nType = GetRoutingListCode( &(pNRBC->NSS[i]),NextNSSInfo.pNSS );
					tmpnode.point	  = NextNSSInfo.pNSS->ArcPt[0];
					tmpnode.nIconType = nuWORD(1024 + nType * 10 + pNRBC->NSS[i].TurnFlag);
					nuBOOL bInOROutHighWay =  ( (pNRBC->NSS[i].RoadClass <= 3) && (NextNSSInfo.pNSS->RoadClass <= 3) ) ? nuFALSE:nuTRUE;
					if( NowNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
					{
						nTraffNum = 0;
						NAVINSS Temp1,Temp2;
						Temp1 = NextNSSInfo;
						while(1)
						{
							if(!GetNextNSS(Temp1,&Temp2))
							{
								break;
							}
							if( Temp1.pNSS->RoadVoiceType == VRT_TrafficCircle  && Temp2.pNSS->RoadVoiceType == VRT_TrafficCircle)
							{
								nTraffNum++;
							}
							else
							{
								break;
							}
							Temp1 = Temp2;	
						}
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum ; 
					}
					else if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle )
					{
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum;
					}
					
					nuINT tmpType = 1;
					switch( tmpType/*g_pInFsApi->pGdata->drawInfoNavi.nRtListType */) 
					{
					case ROUTELIST_TYPE_CROSS:
						switch(nType) 
						{
						case AF_In_Freeway:
						case AF_Out_Freeway:
						case AF_In_Fastway:
						case AF_Out_Fastway:
						case AF_In_SlipRoad:
						case AF_In_Overpass:
						case AF_Out_Overpass:
						case AF_In_Tunnel:
						case AF_In_Underpass:
						case AF_In_UnOverpass:
						case AF_In_Tollgate:
						case AF_In_RestStop:
						case AF_In_TrafficCircle:
						case AF_Out_TrafficCircle:
						case AF_IN_FASTROAD:
						case AF_IN_SLOWROAD:
						case AF_CityTownBoundary:
						case AF_MapBoundary:
						case AF_OUT_SLOWROAD:
						case AF_IN_AUXROAD:
						case AF_IN_MAINROAD:
							g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								//GetRoadName(&(pNRBC->NSS[i]),tmpnode.wsName);
								RNEXTEND RnEx = {0};
								g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
								AddListNode(tmpnode,bInOROutHighWay);	
							}
							nNodeLen = 0;
							break;
						case AF_MidSit:
							g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);	
							}
							nNodeLen = 0;
							break;
						case AF_EndSite:
							g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);	
							}
							nNodeLen = 0;
							break;
						case AF_LoseHere:
							if(pNRBC->NSS[i].TurnFlag != T_No )
							{   
								if( pNRBC->NSS[i].RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
								{
									break;
								}
								else
								{
									g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
									if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
									{
										tmpnode.nDistance = nNodeLen;
										RNEXTEND RnEx= {0};
										g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
										AddListNode(tmpnode,bInOROutHighWay);
										nNodeLen = 0;
									}
								}
								
							}
							break;
						default:
							break;
						}
						break;
						case ROUTELIST_TYPE_PASSNODE:
							switch(nType) 
							{
							case AF_Has_SlipRoad:
							case AF_Has_RestStop:
								g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
								if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
								{
									tmpnode.nDistance = nNodeLen;
									RNEXTEND RnEx = {0};
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
									AddListNode(tmpnode,bInOROutHighWay);
								}
								nNodeLen = 0;
								break;
							default:
								break;
							}
							break;
							default:
								break;
					}
				}
			}
			else
			{
				tmpnode.nDistance = pNRBC->RealDis;
				tmpnode.nIconType = 5107;//nuWORD(1024 + AF_MapBoundary * 10 + NowNSSInfo.pNSS->TurnFlag);
				GetRTBName( pNRBC->FileIdx, pNRBC->FInfo.RTBID, tmpnode.wsName );
				AddListNode(tmpnode,nuFALSE);
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
		if( pNFTC != NULL )
		{
			g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
			if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{
				tmpnode.nDistance = nNodeLen;
				tmpnode.nIconType = nuWORD(1024 + AF_MidSit * 10);
				g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
				AddListNode(tmpnode,nuFALSE);	
			}
			nNodeLen = 0;
		}
		
	}	
	if( g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum )
	{
		tmpnode.nDistance	= nNodeLen;
		tmpnode.nIconType	= 1024 + AF_EndSite * 10;
		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
		PNAVIFROMTOCLASS PTmpNFTC2 = NULL ;






		while( pTmpNFTC )
		{
			PTmpNFTC2 = pTmpNFTC;
			pTmpNFTC  = pTmpNFTC->NextFT;
		}
		if( PTmpNFTC2 )
		{
			tmpnode.point.x		= PTmpNFTC2->TCoor.x;
			tmpnode.point.y		= PTmpNFTC2->TCoor.y;
		}
		nuWCHAR wc[20];
		nuMemset( wc, 0, sizeof(wc) );
		g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
		AddListNode(tmpnode,nuFALSE);
	}
	
}

nuBOOL CNaviData::GetNextNSS( NAVINSS NowNSSInfo ,PNAVINSS pNextNSSInfo)
{
	if( pNextNSSInfo == NULL )
	{
		return nuFALSE;
	}
	
	nuLONG i = NowNSSInfo.nIndex;
	PNAVIFROMTOCLASS pNFTC = NowNSSInfo.pNFTC;
	PNAVIRTBLOCKCLASS pNRBC = NowNSSInfo.pNRBC;
	PNAVISUBCLASS  pNSS = NowNSSInfo.pNSS;
	nuMemset( pNextNSSInfo, 0 ,sizeof(NAVINSS) );
	while ( pNFTC != NULL )
	{
		if( pNFTC != NowNSSInfo.pNFTC )
		{
			pNRBC = pNFTC->NRBC;
		}	
		while ( pNRBC != NULL )
		{
			if( NowNSSInfo.pNFTC == pNFTC && NowNSSInfo.pNRBC == pNRBC)
			{
				if(    NowNSSInfo.nIndex == NowNSSInfo.pNRBC->NSSCount - 1 
					|| NowNSSInfo.pNRBC->NSSCount == 0
					|| NowNSSInfo.pNRBC->NSS == NULL )
				{
					i = -1;
					pNRBC = pNRBC->NextRTB;
					/*
					if ( pNRBC != NULL && pNRBC->NSS == NULL )
					{
					g_pInRTApi->RT_LocalRoute( &pNRBC );
					}
					*/
					continue;
				}
			}
			i += 1;
			pNextNSSInfo->pNFTC = pNFTC;
			pNextNSSInfo->pNRBC = pNRBC;
			pNextNSSInfo->pNSS = &pNRBC->NSS[i];
			pNextNSSInfo->nIndex = i;
			if ( pNextNSSInfo->pNRBC == NULL || pNextNSSInfo->pNRBC->RTBStateFlag != 0  )
			{
				return nuFALSE;
			}
			return nuTRUE;
			
		}
		pNFTC = pNFTC->NextFT;
	}
	return nuFALSE;
}

nuVOID CNaviData::AddListNode(SHOWNODE shownode, nuBOOL bInOutHighWay)
{
	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
	nuLONG nNewIconType = shownode.nIconType;
	if ( shownode.nIconType < 5000 )
	{
		nuLONG nTurnFlag = ( shownode.nIconType - 1024 )%10;
		nuLONG nRoadType = ( shownode.nIconType - 1024 )/10;
		if (   nRoadType == AF_In_TrafficCircle
			|| nRoadType == AF_Out_TrafficCircle
#ifdef ZENRIN
			|| nRoadType == AF_In_Tollgate
#endif
			)
		{
			nNewIconType = 5010 + nRoadType;
		}
		else if( nTurnFlag != 0 )
		{
			nNewIconType = 5000 + nTurnFlag ;
		}
		else
		{
			nNewIconType = 5010 + nRoadType;
		}
		
	}
#if (  defined VALUE_EMGRT )
	if( shownode.nIconType - 5010 == AF_Has_RestStop ) 
	{
		nuWCHAR xx[4] = {0x670d,0x52a1,0x533a}; //add ?å¡??
		if ( NULL == nuWcsstr( shownode.wsName, xx ) )
		{
		nuWcscat( shownode.wsName,xx );
	}
		
	}
#endif
	shownode.nIconType =(nuWORD) nNewIconType;
	if( bInOutHighWay )
	{
		shownode.nIconType += 1000;
	}
	if( g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum == 0 )
	{
		*(g_pInFsApi->pGdata->drawInfoNavi.pRtList) = shownode ;
	}
	else
	{
		SHOWNODE *p = g_pInFsApi->pGdata->drawInfoNavi.pRtList;
		for(nuINT i = 0 ;i < g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum; i++)
		{
			p++;
		}
		*p = shownode;		
	}
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum++;
	
}



nuBOOL CNaviData::GetRoadName(PNAVISUBCLASS pNowNSS, nuWCHAR *wsRoadName)
{
	nuLONG nRoadNameAdd = pNowNSS->RoadNameAddr;
	
	nuMemset(wsRoadName,0,MAX_LISTNAME_NUM);
	nuTCHAR filename[260]= {0};
	nuGetModulePath(NULL, filename, 260);

	nuTcscat(filename,nuTEXT("MAP\\Uone_HJO_TW\\Uone_HJO_TW.RN"));
	nuFILE * f =  nuTfopen(filename,NURO_FS_RB);
	if(f == NULL)
	{
		return nuFALSE;
	}
	if(0 != nuFseek(f,nRoadNameAdd,NURO_SEEK_SET))
	{
		nuFclose(f);
		return nuFALSE;
	}
	char RdNameLen;
	if( nuFread( &RdNameLen, 1, 1, f) != 1 )
	{
		nuFclose(f);
		return nuFALSE;
	}
	if(0 != nuFseek(f,nRoadNameAdd+4,NURO_SEEK_SET))
	{
		nuFclose(f);
		return nuFALSE;
	}
	if( nuFread(wsRoadName,RdNameLen,1,f) != 1 )
	{
		nuFclose(f);
		return nuFALSE;
	}
	nuFclose(f);
	if(nRoadNameAdd == -1)
	{
		switch(pNowNSS->RoadClass)
		{
		case 0:
			break;
		case 1:
			break;
		default:
			break;
		}
		nuWCHAR wc[20];
		nuMemset( wc, 0 ,sizeof(wc) );
		nuAsctoWcs(wc, 20, "General road", nuStrlen("General road"));
		nuWcscpy(wsRoadName,wc);
	}
	return nuTRUE;
}


nuBOOL CNaviData::ColNextVoice(PNAVINSS pNowNSSInfo,PNAVINSS pNextNSSInfo,nuLONG nLenToNext, SOUNDPLAY& SoundPlay)
{
	NAVINSS NSSInfo;
	NAVINSS NaviNSS;
	nuMemset( &NSSInfo, 0 ,sizeof(NSSInfo));
	nuMemset( &NaviNSS, 0 ,sizeof(NaviNSS));
	nuLONG nChangeCode = 0 ;
	nuBOOL b_SameRoad = nuFALSE;//
	if( pNowNSSInfo == NULL || pNextNSSInfo == NULL || pNextNSSInfo->pNSS == NULL || pNowNSSInfo->pNSS == NULL  )
	{
		return nuFALSE;
	}
#ifdef ZENRIN
	if (pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS && pNowNSSInfo->pNSS->RoadVoiceType != VRT_ROADCROSS)
	{
		if ( pNowNSSInfo->pNSS->TurnFlag <= T_Afore )
		{
			m_pVoiceNSSInfo.pNSS = pNowNSSInfo->pNSS;
			m_pVoiceNSSInfo.pNRBC = pNowNSSInfo->pNRBC;
			m_pVoiceNSSInfo.nIndex = pNowNSSInfo->nIndex;
			m_pVoiceNSSInfo.pNFTC = pNowNSSInfo->pNFTC;
			m_bVoiceRoadCross = nuTRUE;
			return nuFALSE;
		}
	}
	if(pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS && pNowNSSInfo->pNSS->RoadVoiceType== VRT_ROADCROSS)
	{
		if ( pNowNSSInfo->pNSS->TurnFlag <= T_Afore )
		{
			return nuFALSE;
		}
	}
	if (pNextNSSInfo->pNSS->RoadVoiceType != VRT_ROADCROSS && pNowNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS)
	{
		if (m_bVoiceRoadCross && pNowNSSInfo->pNSS->TurnFlag <= T_Afore)
		{
			pNowNSSInfo->pNSS = m_pVoiceNSSInfo.pNSS;
			pNowNSSInfo->pNRBC = m_pVoiceNSSInfo.pNRBC;
			pNowNSSInfo->nIndex = m_pVoiceNSSInfo.nIndex;
			pNowNSSInfo->pNFTC = m_pVoiceNSSInfo.pNFTC;
			m_bVoiceRoadCross = nuFALSE;
			b_SameRoad = nuTRUE;
		}
		else if(pNowNSSInfo->pNSS->TurnFlag <= T_Afore)
		{
			return nuFALSE;
		}
	}
#endif
	nChangeCode = GetRoutingListCode(pNowNSSInfo->pNSS,pNextNSSInfo->pNSS);
#ifdef SameRoad
    if (nChangeCode == AF_Out_Fastway)
	{
		if (pNextNSSInfo->pNSS->RoadLength <= SAMEROADLEN  && pNowNSSInfo->pNSS->RoadClass == 4 &&
			pNowNSSInfo->pNSS->RoadClass == pNextNSSInfo->pNSS->RoadClass )
		{
			NaviNSS.nIndex = pNextNSSInfo->nIndex;
			NaviNSS.pNFTC  = pNextNSSInfo->pNFTC;
			NaviNSS.pNRBC  = pNextNSSInfo->pNRBC;
			NaviNSS.pNSS   = pNextNSSInfo->pNSS;
			GetNextNSS(NaviNSS, &NSSInfo);
			if( NSSInfo.pNFTC == NULL || NSSInfo.pNRBC == NULL || NSSInfo.pNSS == NULL  )
			{
				return nuFALSE;
			}
			if (NSSInfo.pNSS->RoadClass == pNowNSSInfo->pNSS->RoadClass)
			{
				nChangeCode = 0;
			}
		}
	}
	if (pNowNSSInfo->pNSS->RoadLength <= m_nSameRoadLen  && 
		( pNowNSSInfo->pNSS->RoadClass == 6 || pNowNSSInfo->pNSS->RoadClass == 7 || pNowNSSInfo->pNSS->RoadClass == 9 )&& 
		pNowNSSInfo->pNSS->RoadClass == pNextNSSInfo->pNSS->RoadClass && pNowNSSInfo->pNSS->TurnFlag <= 1 )
	{
		nuWCHAR  wsNowRoad[MAX_LISTNAME_NUM] = {0};//
		nuWCHAR  wsNextRoad[MAX_LISTNAME_NUM] = {0};
		RNEXTEND RnEx = {0};
		g_pInFsApi->FS_GetRoadName( pNextNSSInfo->pNRBC->FileIdx, pNextNSSInfo->pNSS->RoadNameAddr, pNextNSSInfo->pNSS->RoadClass, wsNextRoad, MAX_LISTNAME_NUM, &RnEx);
		g_pInFsApi->FS_GetRoadName( pNowNSSInfo->pNRBC->FileIdx, pNowNSSInfo->pNSS->RoadNameAddr, pNowNSSInfo->pNSS->RoadClass, wsNowRoad, MAX_LISTNAME_NUM, &RnEx);
		if (nuWcscmp(wsNowRoad, wsNextRoad) == 0)
		{
			if (nChangeCode == AF_IN_FASTROAD || nChangeCode == AF_IN_SLOWROAD)
			{
				nChangeCode = 0;
			}
		}
	}
#endif
	if ( pNowNSSInfo->pNSS )
	{
		if( pNowNSSInfo->pNFTC != pNextNSSInfo->pNFTC )
		{
			SoundPlay.nChangeCode = 32;//Pass Node
			SoundPlay.nDistance = nLenToNext;
			SoundPlay.nTurning = pNowNSSInfo->pNSS->TurnFlag;
			//g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)SoundPlay.wsRoadName, MAX_LISTNAME_NUM);
			SoundPlay.NaviInfo = *pNowNSSInfo;
			return nuTRUE;
		}
		else if(   ( pNowNSSInfo->pNSS->TurnFlag != T_No && bJudgeHighWayCross( pNowNSSInfo, pNextNSSInfo )) 	 
			|| CheckIsRVoiceL(nChangeCode) )
		{
			if (   pNowNSSInfo->pNSS
				&& pNextNSSInfo->pNSS
				&& pNextNSSInfo->pNSS->RoadLength <= LEN_COMBINE
				&& pNextNSSInfo->pNSS->RoadVoiceType != VRT_TrafficCircle
				//&& NowNSSInfo.pNSS->TurnFlag == T_Afore 
				)
			{
				bColCombinCrossData( pNowNSSInfo, pNextNSSInfo, nLenToNext,nuFALSE );
			}
			if (nChangeCode == AF_LoseHere)
			{			
			#ifdef ZENRIN
				if ( b_SameRoad  && pNowNSSInfo->pNSS->TurnFlag <= T_Afore)
				{
					return nuFALSE;
				}
			#endif
			#ifdef SameRoad
				if ( pNowNSSInfo->pNSS->TurnFlag == T_Afore || pNowNSSInfo->pNSS->TurnFlag == T_LeftSide || pNowNSSInfo->pNSS->TurnFlag == T_RightSide)
				{
					if (JudgeSameRoadName(*pNowNSSInfo,*pNextNSSInfo) 
			#ifdef ECI
						&& pNowNSSInfo->pNSS->Real3DPic == 0
			#endif
						)
					{//shyanx
						return nuFALSE;
					}
				}
			}
		#endif
            #ifdef SameRoad
			if (nChangeCode != AF_Out_TrafficCircle)
			{
				if (pNowNSSInfo->pNSS->TurnFlag == T_LeftSide || pNowNSSInfo->pNSS->TurnFlag == T_RightSide )
				{
					if (JudgeSameRoadName(*pNowNSSInfo,*pNextNSSInfo)
			#ifdef ECI	
						&& pNowNSSInfo->pNSS->Real3DPic == 0
			#endif	
						)
					{//shyanx
						return nuFALSE;
					}
				}
			}
            #endif
			SoundPlay.nChangeCode = nChangeCode;
			SoundPlay.nDistance	  = nLenToNext;
			SoundPlay.nTurning	  = pNowNSSInfo->pNSS->TurnFlag;
			RNEXTEND RnEx = {0};
			nuBOOL bGetName = nuFALSE;
#ifdef VALUE_EMGRT
			if(    nChangeCode == AF_In_Freeway 
				&&( pNextNSSInfo->pNRBC->FileIdx     != pNowNSSInfo->pNRBC->FileIdx|| pNextNSSInfo->pNSS->RoadNameAddr != pNowNSSInfo->pNSS->RoadNameAddr )
				 )
				//|| nChangeCode == AF_In_Overpass )//è¿é??»é€?å?? æ¯äœé??¶è·¯?äž­äžå??«âé??¶â?è¿æ¶
				                                    //äžé??¶å?äžé??¶æ¶å°±ä??­å??ç?è·¯å?ïŒè??·å°±äŒå??ç?
			{
				bGetName = nuTRUE;
			}
#endif
			if (   pNextNSSInfo->pNRBC->FileIdx     != pNowNSSInfo->pNRBC->FileIdx
				|| pNextNSSInfo->pNSS->RoadNameAddr != pNowNSSInfo->pNSS->RoadNameAddr 
				|| nChangeCode == AF_Out_Freeway 
				|| nChangeCode == AF_Out_Overpass
				|| bGetName)
			{
                                //Test  æµè?2äžªè?ç»­ä?é«éç?è·¯å£
				/*#define DIS_SEARCH_SLIPROAD   1000
				nuBYTE nIndexOut = 0;
				if( nChangeCode == AF_Out_Freeway 
				|| nChangeCode == AF_Out_Overpass )
			{
					nuINT nLen = pNowNSSInfo->pNSS->RoadLength;
					NAVINSS NowNss,PreNss;
					nuMemcpy( &NowNss, pNowNSSInfo, sizeof(NowNss) );
					while( nLen <= DIS_SEARCH_SLIPROAD )
					{
						if( nuFALSE == GetPreNSS( NowNss, &PreNss )  )
						{
							break;
						}
						if( NULL == PreNss.pNSS )
						{
							break;
						}
						if( PreNss.pNSS->CrossCount > 1 )
						{
							++nIndexOut;
						}
						nLen += PreNss.pNSS->RoadLength;
						nuMemcpy( &NowNss,&PreNss, sizeof(NowNss) );

					}

				}
                                */
           #ifdef VALUE_EMGRT
				if (   nChangeCode == AF_Out_Freeway 
					|| nChangeCode == AF_Out_Overpass )
				{
					if ( pNowNSSInfo->pNSS->RoadNameAddr != -1)
					{
						g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNowNSSInfo->pNRBC->FileIdx, pNowNSSInfo->pNSS->RoadNameAddr, pNowNSSInfo->pNSS->RoadClass, SoundPlay.wsRoadName, MAX_LISTNAME_NUM, &RnEx);
						nuWCHAR wsTmp[2] = { 0x28 };
						nuWCHAR *pTmp = NULL;
						if( pTmp = nuWcsstr( SoundPlay.wsRoadName, wsTmp) )
						{
							*pTmp = 0;
						}
						nuBOOL bAdd = nuFALSE;
						nuWCHAR wsNameTmp[MAX_LISTNAME_NUM] = {0};
						nuWCHAR wsEnter[3] = {36827, 20837 };//?è??¥â?
						//nuWcscat( wsNameTmp,wsEnter );
						nuBOOL bFindSPT = nuFALSE;
						if( pNextNSSInfo->pNSS->SPTFlag == 1   )
						{
							//MessageBox( NULL, L"aaaa",L"",0);
							nuTCHAR sSptFileName[256] = {0};
							nuBOOL bHaveSpt = nuFALSE;
							nuTcscpy(sSptFileName, nuTEXT(".SPT"));
					        if( !g_pInFsApi->FS_FindFileWholePath(pNextNSSInfo->pNRBC->FileIdx, sSptFileName, 256 ) )
							{
								return nuFALSE;
							}
							//MessageBox( NULL, L"1",L"",0);
							SPTNODELISTINFO SptPre ={0};
							SPTNODELISTINFO SptNext ={0};
							SptPre.nFileIndex = pNowNSSInfo->pNRBC->FileIdx;
							SptPre.NodeId.nBlockID = pNowNSSInfo->pNSS->BlockIdx;//pNRBC->RTBIdx;pNRBC->NSS[nNodeIndex].BlockIdx
							SptPre.NodeId.nRoadID  = pNowNSSInfo->pNSS->RoadIdx;
							SptNext.nFileIndex     = pNextNSSInfo->pNRBC->FileIdx;
							SptNext.NodeId.nBlockID = pNextNSSInfo->pNSS->BlockIdx;
							SptNext.NodeId.nRoadID  = pNextNSSInfo->pNSS->RoadIdx;
							nuULONG nNameAddr = 0;
							nuULONG nNameLen = 0;
							if( m_SptFile.GetNodeNameAddr( sSptFileName, SptPre, SptNext, nNameAddr, nNameLen ) )
							{	
								//MessageBox( NULL, L"2",L"",0);
								if( m_SptFile.GetNodeName(sSptFileName,nNameAddr,nNameLen,(char *)wsNameTmp, MAX_LISTNAME_NUM*2 ) )
								{
									//MessageBox( NULL, L"3",L"",0);
									bFindSPT = nuTRUE;
									bAdd = nuTRUE;
								}

							}
						}
						if( nuFALSE == bFindSPT )
						{
				        if ( pNextNSSInfo->pNSS->RoadNameAddr != pNowNSSInfo->pNSS->RoadNameAddr )
				        {
							g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNextNSSInfo->pNRBC->FileIdx, pNextNSSInfo->pNSS->RoadNameAddr, pNextNSSInfo->pNSS->RoadClass, wsNameTmp, MAX_LISTNAME_NUM, &RnEx);
							if( nuWcslen( wsNameTmp ) )
							{
								bAdd = nuTRUE;
							}
							if( pTmp = nuWcsstr( wsNameTmp, wsTmp) )
							{
								*pTmp = 0;
							}
				        }
						if(  pNextNSSInfo->pNSS->RoadClass == 5 && pNowNSSInfo->pNSS->RoadClass <= 4 ) //? äžº?ä?é«æ¶äžæ¯é«éï?äžæ¥?å??å?ç§°ä??¯ä??·ç?è¯å°±äŒæ??®é?
						{
							AddExternStr( AF_In_SlipRoad,wsNameTmp,25 );
							bAdd = nuTRUE;
						}
						}
						if( bAdd )
						{
							nuWCHAR wsADD[10] = {0};
							if( bFindSPT )
							{
								wsADD[0] = 24448 ;
								nuWcscat( SoundPlay.wsRoadName, wsADD ); //"åŸ"
							}
							else
							{
							nuWcscat( SoundPlay.wsRoadName, wsEnter );
							}
							nuWcscat( SoundPlay.wsRoadName, wsNameTmp );
							if ( bFindSPT)
							{
								wsADD[0] = 26041 ;
								wsADD[1] = 21521  ;
								nuWcscat( SoundPlay.wsRoadName, wsADD );//"?¹å?"
							}
						}
					}
					
				}
				else
	      #endif
				{
					if(    pNextNSSInfo->pNSS->RoadNameAddr != -1 
						|| ( pNextNSSInfo->pNSS->RoadClass == 9 && pNowNSSInfo->pNSS->RoadClass <= 7 ) )
					{
						nuBOOL bSearch = nuFALSE;
						if ( pNextNSSInfo->pNSS->PTSNameAddr > 4  )
						{
							bSearch = GetPTSName((nuUSHORT)pNextNSSInfo->pNRBC->FileIdx, pNextNSSInfo->pNSS->PTSNameAddr, SoundPlay.wsRoadName, MAX_LISTNAME_NUM *sizeof(nuWCHAR));
						}
						if( !bSearch && pNextNSSInfo->pNSS->PTSNameAddr <= 4 )
						{
							g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNextNSSInfo->pNRBC->FileIdx, pNextNSSInfo->pNSS->RoadNameAddr, pNextNSSInfo->pNSS->RoadClass, SoundPlay.wsRoadName, MAX_LISTNAME_NUM, &RnEx);
						}
						
						#ifdef VALUE_EMGRT
												nuWCHAR wsTmp[2] = { 0x28 };
												nuWCHAR *pTmp = NULL;
												if( pTmp = nuWcsstr( SoundPlay.wsRoadName, wsTmp) )
												{
													*pTmp = 0;
												}
							if( bGetName )
							{
								SoundPlay.nChangeCode = 0;
							}
							AddExternStr( nChangeCode, SoundPlay.wsRoadName, MAX_LISTNAME_NUM);

						#endif						
					}

				}
				
				
			}
			//g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNextNSSInfo->pNRBC->FileIdx, pNextNSSInfo->pNSS->RoadNameAddr, pNextNSSInfo->pNSS->RoadClass, SoundPlay.wsRoadName, MAX_LISTNAME_NUM, &RnEx);
			SoundPlay.NaviInfo = *pNowNSSInfo;
			return nuTRUE;
		}
	}
	
	return nuFALSE;
}
nuBOOL CNaviData::CheckIsRVoiceL(nuLONG RoutingListCode)
{
	
	switch(RoutingListCode)
	{
	case AF_In_Freeway:
	case AF_Out_Freeway:
	case AF_In_Fastway:
	case AF_Out_Fastway:
	case AF_In_SlipRoad:
		//		case AF_Has_SlipRoad:
	case AF_In_Overpass:
	case AF_Out_Overpass:
	case AF_In_Tunnel:
	case AF_Out_Tunnel:
	case AF_In_Underpass:
	case AF_In_UnOverpass:
	case AF_In_Tollgate:
	case AF_In_RestStop:
		//		case AF_Has_RestStop:
	case AF_In_TrafficCircle:
	case AF_Out_TrafficCircle:
	case AF_IN_FASTROAD:
	case AF_IN_SLOWROAD:
	case AF_CityTownBoundary:
	case AF_MapBoundary:
	case AF_MidSit:
	case AF_EndSite:
	case AF_OUT_SLOWROAD:
	case AF_IN_FERRY:
	case AF_IN_UTURN:
	case AF_IN_LTURN:
	case AF_IN_RTURN:
	case AF_IN_MAINROAD:
	case AF_IN_AUXROAD:
		return nuTRUE;
	default:
		return nuFALSE;
	}
}

nuBOOL CNaviData::PlayNaviSound()
{
	if( m_struSeachInfo.bIngore )
	{
		return nuFALSE;
	}
	SOUNDPLAY First = m_struSeachInfo.SoundPlayFirst;
	if ( First.nChangeCode == 32 )
	{
		nuINT ffffff = 0;
	}
	SOUNDPLAY Next = m_struSeachInfo.SoundPlayNext;
    nuINT FindCrossNum = m_struSeachInfo.nFindCrossNum;
	nuWORD nDis = 0 ;
	nuBOOL bFastWay = nuFALSE;
	nuBOOL bOutFreeWay = nuFALSE;
	//if( !(First.NaviInfo.pNSS->TurnFlag != T_No/*|| CheckIsRVoiceL( First.nChangeCode)*/ ))
	//{
	//	return nuFALSE;
	//}
	
	//Play the Frist Sound
	if( m_NaviNSS.pNFTC != First.NaviInfo.pNFTC || m_NaviNSS.pNRBC != First.NaviInfo.pNRBC || m_NaviNSS.pNSS != First.NaviInfo.pNSS )
	{
		nuMemset( m_bHasPlayVoice, 0, sizeof(m_bHasPlayVoice) );
		m_NaviNSS.pNFTC		=  First.NaviInfo.pNFTC;
		m_NaviNSS.pNRBC		=  First.NaviInfo.pNRBC;
		m_NaviNSS.pNSS	    =  First.NaviInfo.pNSS;
		m_bEstimation = nuFALSE;
		m_bFirstSpeed = nuFALSE; 
	}
#ifndef VALUE_EMGRT
	if ((nuINT)g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance > 3000 && m_bInFreeWay)
	{//Next Distance > 3km After InFreeWay 
		m_bEstimation = nuTRUE;
	}
	if (m_bEstimation && m_bInFreeWay)
	{//Estimation Play Sound 
		nuWCHAR wsEstimationName[MAX_LISTNAME_NUM] = {0};
		nuMemcpy(wsEstimationName,g_pInFsApi->pGdata->drawInfoNavi.nextCross.wsName, sizeof(wsEstimationName));
		SoundNamefilter(wsEstimationName, sizeof(wsEstimationName) / 2);
		__android_log_print(ANDROID_LOG_INFO, "RoadList", "wsEstimationName %s",wsEstimationName);
		g_pInFsApi->pGdata->NC_SoundPlay( VOICE_CODE_ROAD_ASY, DIS_ESTIMATION, 0 , (nuUSHORT)First.nChangeCode, 0, wsEstimationName);
        m_bEstimation = nuFALSE;
        m_bInFreeWay = nuFALSE;
	}
#endif
#ifdef VALUE_EMGRT
	if(m_bSoundAfore && (nuINT)g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance < 500 && !m_bFreeWayFlag)
	{
		g_pInFsApi->pGdata->NC_SoundPlay( VOICE_CODE_ROAD_ASY, DIS_100M, T_Afore , 0, 0, 0);
		m_bFreeWayFlag = nuTRUE;
	}
	if ((nuINT)g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance >= 500)
	{
		m_bFreeWayFlag = nuFALSE;
		m_bSoundAfore = nuFALSE;
	}
#endif
	if( !(JudgeDis( &First, &nDis, First.nDistance, &bFastWay )))
	{
		return nuFALSE;
	}	
	nuWCHAR wsName[MAX_LISTNAME_NUM] = {0};
	bChangeSymbols( First.wsRoadName , wsName );
	if(    First.nChangeCode == AF_IN_UTURN 
		|| First.nChangeCode == AF_IN_LTURN
		|| First.nChangeCode == AF_IN_RTURN
		)
	{
		First.nTurning = T_No;
	}
	if ( First.nChangeCode == AF_In_TrafficCircle || First.nChangeCode == AF_Out_TrafficCircle)
	{//¶i€J¶êÀôandÂ÷¶}¶êÀô €£Œœ³ø€èŠV
		First.nTurning = T_No;
	}
	//SoundAfterWork(wsName, NULL);//added by daniel for LCMM 20120111
#ifndef VALUE_EMGRT
	SoundNamefilter(wsName, sizeof(wsName) / 2);
	if (nDis == DIS_100M)
	{
		if (First.nChangeCode == AF_In_Freeway)
		{
			m_bInFreeWay = nuTRUE;
		}
	}
#endif
#ifndef VALUE_EMGRT
	if (nDis == DIS_100M && !m_bFirstSound )
	{
		if ( First.nTurning == T_Left)
		{
			First.nTurning = T_LeftLeft;
		}
	    else if ( First.nTurning == T_Right)
		{
			First.nTurning = T_RightRight;
		}
		else if ( First.nTurning == T_LeftSide)
		{
			First.nTurning = T_LeftLeftSide;
		}
		else if ( First.nTurning == T_RightSide)
		{
			First.nTurning = T_RightRightSide;
		}
		else if ( First.nTurning == T_LUturn)
		{
			First.nTurning = T_LULUturn;
		}
		else if ( First.nTurning == T_RUturn)
		{
			First.nTurning = T_RURUturn;
		}
		m_bFirstSound = nuTRUE;
	}
	else
	{
		m_bFirstSound = nuTRUE;
	}
#endif
	__android_log_print(ANDROID_LOG_INFO, "RoadList", "InNavi Sound wsName %s",wsName);
	nuBOOL bbbb = g_pInFsApi->pGdata->NC_SoundPlay( VOICE_CODE_ROAD_ASY, nDis,(nuUSHORT) First.nTurning, (nuUSHORT)First.nChangeCode, 0, wsName );
	//Add 09-01-10
	if ( nDis == 2 && First.nChangeCode == 32 )
	{
		//First.NaviInfo.pNFTC->RunPass = nuTRUE;
		nuMemset( &m_ptLastNextCross, 0 ,sizeof(m_ptLastNextCross) );
		nuMemset( &m_struSeachInfo.SoundPlayFirst.NaviInfo, 0 ,sizeof(m_struSeachInfo.SoundPlayFirst.NaviInfo));
		//g_pInFsApi->pUserCfg->nPastMarket++;
	}
	//Play the Second Sound
	if( FindCrossNum == 2 )
	{
		if (m_struSeachInfo.SoundPlayFirst.nChangeCode != AF_Out_Freeway)
		{
			if( !(JudgeDis( &Next , &nDis, Next.nDistance, &bFastWay )))
			{
				if( bFastWay)
				{
					if( nDis == DIS_2KM || nDis == DIS_1KM)
					{
						return nuTRUE;
					}
				}
				else
				{
					if( nDis == DIS_500M || nDis == DIS_300M )
					{
						return nuTRUE;
					}
				}
			}
		}
		else
		{
			if( !(JudgeDis( &Next , &nDis, Next.nDistance, &bFastWay )))
			{
				if( nDis == DIS_2KM || nDis == DIS_1KM)
				{
					return nuTRUE;
				}
			}
			if( Next.nDistance - First.nDistance < VOICEDIS500_E_FastWay)
			{
				bOutFreeWay = nuTRUE;
			}
		}
		if( Next.nDistance - First.nDistance < ((Next.NaviInfo.pNSS->RoadClass <= 3) ? VOICEDIS500_E_FastWay : DIS_BETWEENINGORE)
			|| Next.nChangeCode == AF_Out_TrafficCircle || bOutFreeWay )
		{
			bChangeSymbols(  Next.wsRoadName , wsName );
			if(    Next.nChangeCode == AF_IN_UTURN 
				|| Next.nChangeCode == AF_IN_LTURN
				|| Next.nChangeCode == AF_IN_RTURN
				)
			{
				Next.nTurning = T_No;
			}
			if ( First.nChangeCode == AF_In_TrafficCircle || First.nChangeCode == AF_Out_TrafficCircle)
			{
				First.nTurning = T_No;
			}
			//SoundAfterWork(wsName, NULL);//added by daniel for LCMM 20120111
#ifndef VALUE_EMGRT
			SoundNamefilter(wsName, sizeof(wsName) / 2);
			if (Next.nChangeCode == AF_In_Freeway)
			{
				m_bInFreeWay = nuTRUE;
			}
#endif
			__android_log_print(ANDROID_LOG_INFO, "RoadList", "InNavi Sound wsName %s",wsName);
			g_pInFsApi->pGdata->NC_SoundPlay(VOICE_CODE_ROAD_ASY, DIS_FOLLOW, (nuUSHORT)Next.nTurning, (nuUSHORT)Next.nChangeCode, 0, wsName );
			if (  Next.nChangeCode == 32 )
			{
				//Next.NaviInfo.pNFTC->RunPass = nuTRUE;
				nuMemset( &m_ptLastNextCross, 0 ,sizeof(m_ptLastNextCross) );
				nuMemset( &m_struSeachInfo.SoundPlayFirst.NaviInfo, 0 ,sizeof(m_struSeachInfo.SoundPlayFirst.NaviInfo));
				//g_pInFsApi->pUserCfg->nPastMarket++;
			}
			if (m_struSeachInfo.SoundPlayFirst.nChangeCode != AF_Out_Freeway)
			{
				m_NaviNextNSSIgnore.pNFTC	= Next.NaviInfo.pNFTC;
				m_NaviNextNSSIgnore.pNRBC	= Next.NaviInfo.pNRBC;
				m_NaviNextNSSIgnore.pNSS	= Next.NaviInfo.pNSS;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CNaviData::JudgeDis(PSOUNDPLAY pSoundPlay,  nuWORD *pDisPlay, nuLONG nDistance, nuBOOL *pbFastWay)
{
	if( pSoundPlay == NULL || pbFastWay == NULL )
	{
		return nuFALSE;
	}
    *pbFastWay = nuFALSE;
	
	if( pSoundPlay->NaviInfo.pNSS->RoadClass <= 3 )
	{
		*pbFastWay = nuTRUE;
	}
	
	if( *pbFastWay )
	{
		if( nDistance > VOICEDIS2000_E && nDistance < VOICEDIS2000_S )
		{
			if( m_bHasPlayVoice[0] )
			{
				return nuFALSE;
			}
			m_bHasPlayVoice[0] = nuTRUE;
			*pDisPlay = DIS_2KM;
			
		}
		else if( nDistance > VOICEDIS1000_E && nDistance < VOICEDIS1000_S )
		{
			if( m_bHasPlayVoice[1] )
			{
				return nuFALSE;
			}
			m_bHasPlayVoice[1] = nuTRUE;
			*pDisPlay = DIS_1KM;
		}
		else if( nDistance > 0 && nDistance < VOICEDIS500_S_FastWay)
		{
			if( m_bHasPlayVoice[2] )
			{
				return nuFALSE;
			}
			m_bHasPlayVoice[2] = nuTRUE;
			*pDisPlay = DIS_500M;
		}
		else
		{
			return nuFALSE;
		}
	}
	else
	{
		if( nDistance > VOICEDIS500_E_SlowWay && nDistance < VOICEDIS500_S_SlowWay )
		{
			if( m_bHasPlayVoice[0] )
			{
				return nuFALSE;
			}
			m_bHasPlayVoice[0] = nuTRUE;
			*pDisPlay = DIS_500M;
		}
		else if( nDistance > VOICEDIS300_E && nDistance < VOICEDIS300_S )
		{
			if( m_bHasPlayVoice[1] )
			{
				return nuFALSE;
			}
			m_bHasPlayVoice[1] = nuTRUE;
			*pDisPlay = DIS_300M;
		}
		else if( nDistance > VOICEDIS100_E && nDistance < VOICEDIS100_S )
		{//100m 
			if( m_bHasPlayVoice[2] )
			{
				return nuFALSE;
			}
		#ifndef VALUE_EMGRT
			if(g_pInFsApi->pGdata->timerData.nAutoMoveType != AUTO_MOVE_SIMULATION)
			{
				if (!m_bFirstSpeed)
				{// 100m Car Speed 
					m_lCarSpeed = g_pInFsApi->pDrawInfo->pcCarInfo->nCarSpeed;
					m_bFirstSpeed = nuTRUE;
				}
				if(m_lCarSpeed > 50)
				{//Car Speed > 50  100m Play Sound 
					if (nDistance < 150)
					{
						m_bHasPlayVoice[2] = nuTRUE;
					}
					else
					{
						return nuFALSE;
					}
				}
				else if(25 < m_lCarSpeed && m_lCarSpeed <= 50)
				{//Car Speed 25 ~ 50  75m Play Sound
					if (nDistance < 75)
					{
						m_bHasPlayVoice[2] = nuTRUE;
					}
					else
					{
						return nuFALSE;
					}
				}
				else if(0 <= m_lCarSpeed && m_lCarSpeed <= 25)
				{//Car Speed 0 ~ 25  50m Play Sound
					if (nDistance < 50)
					{
						m_bHasPlayVoice[2] = nuTRUE;
					}
					else
					{
						return nuFALSE;
					}
				}
			}
			else
			{//Simulation Sound Play
				m_bHasPlayVoice[2] = nuTRUE;
			}
        #else
			m_bHasPlayVoice[2] = nuTRUE;
        #endif
			*pDisPlay = DIS_100M;
		}
		else
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}

nuBOOL CNaviData::CombinedSeach(PSEARCHJUDGEINFO pSeachInfo)
{
	#ifdef ANDROID_ROYAL_SLEEP

	#endif
	if( pSeachInfo == NULL)
	{
		return nuFALSE;
	}
	NAVINSS NextNSSInfo,NowNSSInfo;
	nuMemset( &NextNSSInfo, 0, sizeof(NextNSSInfo));
	nuMemset( &NowNSSInfo, 0, sizeof(NowNSSInfo));
	NowNSSInfo.pNFTC	= pSeachInfo->pNFTC;
	NowNSSInfo.pNRBC	= pSeachInfo->pNRBC;
	NowNSSInfo.pNSS		= &(pSeachInfo->pNRBC->NSS[pSeachInfo->index]);
	NowNSSInfo.nIndex	= pSeachInfo->index;
	
	nuLONG nLenToNext = pSeachInfo->nLenToNext;
	GetNextNSS( NowNSSInfo, &NextNSSInfo );
	FixOverpassRoad( &NowNSSInfo, &NextNSSInfo );
	nuBOOL bEnd = ( NextNSSInfo.pNRBC == NULL ) ? nuTRUE : nuFALSE;
    
	if (   NowNSSInfo.pNFTC 
		&& NextNSSInfo.pNSS
		&& !pSeachInfo->bFindNextNFTC
		&& !NowNSSInfo.pNFTC->RunPass
		&& NowNSSInfo.pNFTC != NextNSSInfo.pNFTC )
	{
		pSeachInfo->bFindNextNFTC = nuTRUE;
		pSeachInfo->nDisToNextNFTC = pSeachInfo->nLenToNext;
	}
	
	if ( NowNSSInfo.pNSS && NowNSSInfo.pNSS->PTSNameAddr > 4 )
	{
		NowNSSInfo.pNSS->RoadVoiceType = VRT_Tollgate;
	}
	if (  NextNSSInfo.pNSS && NextNSSInfo.pNSS->PTSNameAddr > 4 )
	{
		NextNSSInfo.pNSS->RoadVoiceType = VRT_Tollgate;
	}
		
	if( !(pSeachInfo->bFindNextCross) )
	{	
		NAVINSS TmpNSS;
		nuMemcpy( &TmpNSS, &NowNSSInfo, sizeof(TmpNSS) );
		nuBOOL bColNowLen = nuFALSE;
		//nuLONG xx = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
		if ( !ColHighWayCrossData( NowNSSInfo, NextNSSInfo, nLenToNext ))//
		{
			bColNowLen = nuTRUE;
			pSeachInfo->bFindNextCross	 = ColNextCrossData( NowNSSInfo, NextNSSInfo, nLenToNext );
		}
		if (   pSeachInfo->bFindNextCross 
			&& NowNSSInfo.pNSS
			&& NextNSSInfo.pNSS
			&& ( NextNSSInfo.pNSS->RoadLength <= LEN_COMBINE || NextNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS )
			&& NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle
			//&& NowNSSInfo.pNSS->TurnFlag == T_Afore 
			)
		{
			bColCombinCrossData( &NowNSSInfo, &NextNSSInfo, nLenToNext );
		}
		if ( pSeachInfo->bFindNextCross )
		{
			if ( NowNSSInfo.pNSS )
			{
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nBlkIdx  = NowNSSInfo.pNSS->BlockIdx;
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRoadIdx = NowNSSInfo.pNSS->RoadIdx;
			}
			else
			{
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nBlkIdx = -1;
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRoadIdx= -1;
			}
			if ( bColNowLen && g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance <= 0 )
			{
				//#ifdef VALUE_EMGRT
				g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = 0;
				//#endif
				ColZoomCrossData(NowNSSInfo,NextNSSInfo) ;
			}
			
			if ( m_nNowLen < g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance
				&& m_nNowLen != 0 )
			{
				nuINT xx = 0;
			}
			/*
			//LANS_SET LansInfo = {0};
			nuTCHAR filename[260] = {0};
			nuTcscpy(filename, nuTEXT(".LIF"));
			if( !g_pInFsApi->FS_FindFileWholePath(NowNSSInfo.pNRBC->FileIdx, filename, NURO_MAX_PATH) )
			{
				return nuFALSE;
			}
			m_Lif.GetLIFInfo(NowNSSInfo.pNRBC->FileIdx, NowNSSInfo.pNSS->BlockIdx, NowNSSInfo.pNSS->RoadIdx, NowNSSInfo.pNSS->TurnAngle,&g_pInFsApi->pGdata->drawInfoNavi.laneInfo ,filename);
			//Test
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.nNowCount = 2;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[0].nArrowCount = 1;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[0].nLaneNo = 1;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[0].nLanHighLight = nuFALSE;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[0].pASList[0] = 0;
			
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[1].nArrowCount = 2;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[1].nLaneNo = 2;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[1].nLanHighLight = nuTRUE;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[1].pASList[0] = 2;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[1].pASList[1] = 0;
			
			/*g_pInFsApi->pGdata->drawInfoNavi.laneInfo.nNowCount = LansInfo.nLANSNum;
			nuBYTE nLanIndex = 0;
			nuBYTE nArrowIndex = 0;
			for ( nLanIndex = 0; nLanIndex < LansInfo.nLANSNum; nLanIndex++ )
			{
			if ( nLanIndex >= MAX_LANE_COUNTS )
			{
			break;
			}
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[nLanIndex].nLaneCar  = LansInfo.LansStru[nLanIndex].nLANE_NO;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[nLanIndex].nLaneType = LansInfo.LansStru[nLanIndex].nLANE_TYPE;
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[nLanIndex].nArrowCount = LansInfo.LansStru[nLanIndex].ArrowSet.nArrowNum;
			for ( nArrowIndex = 0; nArrowIndex < LansInfo.LansStru[nLanIndex].ArrowSet.nArrowNum; nArrowIndex++ )
			{
			g_pInFsApi->pGdata->drawInfoNavi.laneInfo.pLaneList[nLanIndex].pASList[nArrowIndex] = LansInfo.LansStru[nLanIndex].ArrowSet.nArrow[nArrowIndex].nAS;
			}
			
			  }
			*/
			// 
			if ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance <= 0  )
			{
				static nuBOOL bChange = nuFALSE;
				
				if (   m_ptNext.pNFTC == NULL 
					&& m_ptNext.pNRBC == NULL
					&& m_ptNext.pNSS  == NULL
					)
				{
					m_ptNext.pNFTC  = NowNSSInfo.pNFTC;
					m_ptNext.pNRBC  = NowNSSInfo.pNRBC;
					m_ptNext.pNSS   = NowNSSInfo.pNSS;
					m_ptNext.nIndex = NowNSSInfo.nIndex;
					//g_pInFsApi->pUserCfg->nPastMarket = 0;
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nCrossIndex = 0;
					m_bInPassNode = nuFALSE;
					#ifndef ECI
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
					#endif
					Set3DPAfterWork(NowNSSInfo, NextNSSInfo);
					if ( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType == nuWORD(1024 + AF_MidSit * 10) )//PassNode
					{
						m_bInPassNode = nuTRUE;
					}
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nChangeCross = nuTRUE;
				#ifndef VALUE_EMGRT
					if ( NowNSSInfo.pNSS->TurnFlag == T_LUturn )//¥ª°jÂà ŠXšÖ¹DžôœbÀY¥[ªø
					{
						GetNextNSS(NowNSSInfo,&NextNSSInfo);
						if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
						{
							return 0;
						}
						if ( NowNSSInfo.pNSS->TurnFlag == T_LUturn )
						{
							m_nCombinLen = NextNSSInfo.pNSS->RoadLength;
							m_bCombinDrawArrow = nuTRUE;
						}
					}
                #endif
					GetNaviArrowPoint( &m_ptNext, &g_pInFsApi->pGdata->drawInfoNavi.nextCross);
					
				}
				else if (   m_ptNext.pNFTC   != NowNSSInfo.pNFTC
					||  m_ptNext.pNRBC   != NowNSSInfo.pNRBC
					||  m_ptNext.pNSS    != NowNSSInfo.pNSS
					||  m_ptNext.nIndex  != NowNSSInfo.nIndex
					)
				{
					m_ptNext.pNFTC  = NowNSSInfo.pNFTC;
					m_ptNext.pNRBC  = NowNSSInfo.pNRBC;
					m_ptNext.pNSS   = NowNSSInfo.pNSS;
					m_ptNext.nIndex = NowNSSInfo.nIndex;
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nCrossIndex++;
					#ifndef ECI
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;	
				    #endif
				    Set3DPAfterWork(NowNSSInfo, NextNSSInfo);
					if ( m_bInPassNode )
					{
						//g_pInFsApi->pUserCfg->nPastMarket++;
						m_bInPassNode = nuFALSE;
					}
					if ( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType == nuWORD(1024 + AF_MidSit * 10) )//PassNode
					{
						m_bInPassNode = nuTRUE;
					}
					//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nChangeCross = nuTRUE;
					//GetNaviArrowPoint();
					bChange = nuTRUE;
				}
				
				
				//else
				{
					if ( bChange )
					{
						nuLONG nLenToNextCross = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
						nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
						if (   nLenToNextCross < 200 
							|| nLenToBeforeCross > 50 
							|| m_nGoNavi == 1 )
						{
	                   #ifndef VALUE_EMGRT
							if (NowNSSInfo.pNSS->TurnFlag == T_LUturn )
							{
								GetNextNSS(NowNSSInfo,&NextNSSInfo);
								if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
								{
									return 0;
								}
								if (NowNSSInfo.pNSS->TurnFlag == T_LUturn )
								{
									m_nCombinLen = NextNSSInfo.pNSS->RoadLength;
									m_bCombinDrawArrow = nuTRUE;
								}
							}
                       #endif
							GetNaviArrowPoint( &m_ptNext, &g_pInFsApi->pGdata->drawInfoNavi.nextCross);
							g_pInFsApi->pGdata->drawInfoNavi.nextCross.nChangeCross = nuTRUE;
							bChange = nuFALSE;
						}
						
					}
					//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nChangeCross = nuFALSE;
				}
			}
		}
					
		if( NowNSSInfo.pNSS != NULL && NextNSSInfo.pNSS != NULL )
		{
			/*if( pSeachInfo->bFindNextCross )
			{
				if( NowNSSInfo.pNSS->Real3DPic != 0 )
				{
					m_struSeachInfo.n3DPic = NowNSSInfo.pNSS->Real3DPic;
				}
			}*/
			if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle )
			{
				if(g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType < 1214 )
				{ 
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1204;
				}
				else
				{
#ifdef VALUE_EMGRT
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1214;
#else
					g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 51214;//1214;
					if ( m_nTraffNum < 1)
					{
						g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1214;
					}
#endif
				}
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType += m_nTraffNum;
			}
		}
	}
	if( !(pSeachInfo->bFindNextTown) )
	{
		//	pSeachInfo->bFindNextTown = ColNextCityTown( pNowNSS,pNextNSS,nLenToNext);
	}
	if( pSeachInfo->nZoomNum == 0 )
	{
		
	}
	if( NowNSSInfo.pNSS != NULL && NextNSSInfo.pNSS != NULL )
	{
		if(    NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle 
			&& NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle
			&& NowNSSInfo.pNSS->CrossCount >= 2 )
		{
			pSeachInfo->nTrafNum++;
			return nuTRUE;
		}	
	}
	if( bEnd && pSeachInfo->nFindCrossNum == 0 )
	{
		pSeachInfo->SoundPlayFirst.NaviInfo.pNFTC = pSeachInfo->pNFTC;
		pSeachInfo->SoundPlayFirst.NaviInfo.pNRBC = pSeachInfo->pNRBC;
		pSeachInfo->SoundPlayFirst.NaviInfo.pNSS  = &(pSeachInfo->pNRBC->NSS[pSeachInfo->index]);
		pSeachInfo->SoundPlayFirst.nChangeCode = 33;//End Node
		pSeachInfo->SoundPlayFirst.nDistance = nLenToNext;
		pSeachInfo->SoundPlayFirst.nTurning = 0;//pSeachInfo->SoundPlayFirst.NaviInfo.pNSS->TurnFlag;
		(pSeachInfo->nFindCrossNum)++;
	}
	if( pSeachInfo->nFindCrossNum == 1)
	{
		if( /*!(pSeachInfo->bIngore) &&*/ !(pSeachInfo->bIngoreNext) )
		{
			if( NowNSSInfo.pNSS != NULL && NextNSSInfo.pNSS != NULL )
			{
				if(NowNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle)
				{
					pSeachInfo->bIngoreNext = nuTRUE;
					return nuTRUE;
				}
				else if(ColNextVoice( &NowNSSInfo, &NextNSSInfo, nLenToNext,pSeachInfo->SoundPlayNext ))
				{
					if( pSeachInfo->bInTraf )
					{
						if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle)
						{
							pSeachInfo->SoundPlayNext.nTurning = 9 + (pSeachInfo->nTrafNum );
							pSeachInfo->nFindCrossNum++;
							if(g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType < 1214 )
							{ 
								g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1204;
							}
							else
							{
#ifdef VALUE_EMGRT
								g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 1214;
#else
								g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType = 51214;//1214;
#endif
							}
							g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType  += pSeachInfo->nTrafNum ;
							m_nTraffNum = pSeachInfo->nTrafNum ;
						}
					}
					else
					{
						(pSeachInfo->nFindCrossNum)++;
					}	
				}
			}		
		}
	}
	else if(pSeachInfo->nFindCrossNum == 0)
	{
		if( m_NaviNextNSSIgnore.pNFTC == NowNSSInfo.pNFTC && m_NaviNextNSSIgnore.pNRBC == NowNSSInfo.pNRBC && m_NaviNextNSSIgnore.pNSS == NowNSSInfo.pNSS)
		{	
			pSeachInfo->bIngore = nuTRUE;
			//return nuTRUE;
		}
		if(ColNextVoice( &NowNSSInfo, &NextNSSInfo, nLenToNext, pSeachInfo->SoundPlayFirst ))
		{
			if( NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle)
			{
				pSeachInfo->bInTraf = nuTRUE;
				pSeachInfo->nTrafNum++;
			}
			(pSeachInfo->nFindCrossNum)++;		
		}
	}
	return nuTRUE;
}
nuBYTE CNaviData::JudgeZoomRoad()
{
	if ( !g_pInFsApi->pGdata->uiSetData.bNavigation )
	{
		return CROSSNEARTYPE_DEFAULT;
	}
	if (1 == m_nGoNavi)
	{
		return JudgeZoomRoad1();
	}
	nuBYTE nCrossNearType = CROSSNEARTYPE_DEFAULT;
	nuBYTE nCrossNearTypeTmp = CROSSNEARTYPE_DEFAULT;
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
	if ( m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass <= 3  )
	{
		nCrossNearType = CROSSNEARTYPE_HIGHWAY;
		NAVINSS NowNSS = {0};
		NowNSS.pNFTC  = m_ptNavi.pNFTC;
		NowNSS.pNRBC  = m_ptNavi.pNRBC;
		NowNSS.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
		NowNSS.nIndex = m_ptNavi.nSubIdx;
		NAVINSS NextNSS = {0};
		if (  GetNextNSS( NowNSS, &NextNSS ) )
		{
			if(   ( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIconType - 1024 )/10 == AF_Out_Freeway 
				|| NextNSS.pNSS->RoadClass > 3 
				)
				
			{
				if(  g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance <= IN_REAL3DPIC )
				{
					
					if(   m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].Real3DPic != 0  )
					{
						nCrossNearType =  CROSSNEARTYPE_PIC_IN;
					}
					else
					{
						
						NAVINSS NowNSS ={0};
						NowNSS.pNFTC  = m_ptNavi.pNFTC;
						NowNSS.pNRBC  = m_ptNavi.pNRBC;
						NowNSS.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
						NowNSS.nIndex = m_ptNavi.nSubIdx;
						
						NAVINSS NextNSS = {0};
						
						nuLONG nLen3DToNext = 0;//
						//m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadLength;
						
						nuLONG dx = NowNSS.pNSS->ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
						nuLONG dy = NowNSS.pNSS->ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
						nLen3DToNext = (nuLONG)nuSqrt(dx * dx + dy * dy);
						nuDWORD nIndex = m_ptNavi.nPtIdx + 1;
						for ( ; nIndex < m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPtCount - 1; nIndex++ )
						{
							dx = NowNSS.pNSS->ArcPt[nIndex + 1].x - NowNSS.pNSS->ArcPt[nIndex ].x;
							dy = NowNSS.pNSS->ArcPt[nIndex + 1].y - NowNSS.pNSS->ArcPt[nIndex ].y;
							nLen3DToNext += (nuLONG)nuSqrt(dx * dx + dy * dy);
						}
						while( nLen3DToNext < IN_REAL3DPIC )
						{    
							if ( !GetNextNSS( NowNSS, &NextNSS ) )
							{
								break;
							}
							
							if ( NextNSS.pNSS->Real3DPic != 0 )
							{
								nCrossNearType =  CROSSNEARTYPE_PIC_IN;
								break;
							}
							else
							{
								nLen3DToNext += NextNSS.pNSS->RoadLength;
								nuMemcpy( &NowNSS, &NextNSS, sizeof(NowNSS) );
							}
						}
					}
					if ( nCrossNearType != CROSSNEARTYPE_PIC_IN)
					{
						if( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance <= IN_CROSSDIS_LEVEL_2 )
						{
							nCrossNearType = CROSSNEARTYPE_SCALE0_IN;
						}
						else if( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance <= IN_CROSSDIS_LEVEL_1  )
						{
							nCrossNearType = CROSSNEARTYPE_SCALE1_IN;
						}
					}
				}
				
			}
			else if( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID != 0 )
			{
				if(  g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance <= IN_REAL3DPIC )
				{
					nCrossNearType =  CROSSNEARTYPE_PIC_IN;
				}
			}
		}
		
	}

	{
		if( g_pInFsApi->pDrawInfo->drawInfoCommon.nRealPicID )
		{
			if( nLenToNextCross < IN_REAL3DPIC )
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_PIC_IN;
			}
		}
		else if( m_nNowLen == 0 )  //First in the Cross
		{
			if( nLenToNextCross >= IN_CROSSDIS_LEVEL_1 )
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_DEFAULT;
			}
			else if( nLenToNextCross > IN_CROSSDIS_LEVEL_2)
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_SCALE1_IN;
			}
			else 
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_SCALE0_IN;
			}
		}
		else
		{
			if( nLenToNextCross > IN_CROSSDIS_LEVEL_1 )
			{
				if( nLenToBeforeCross < OUT_CROSSDIS_LEVEL_1 )
				{
					nCrossNearTypeTmp = CROSSNEARTYPE_SCALE0_OUT;
				}
				else if( nLenToBeforeCross < OUT_CROSSDIS_LEVEL_2 )
				{
					nCrossNearTypeTmp = CROSSNEARTYPE_SCALE1_OUT;
				}
				else
				{
					nCrossNearTypeTmp = CROSSNEARTYPE_DEFAULT;
				}
			}
			else if( nLenToNextCross >= IN_CROSSDIS_LEVEL_2 )
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_SCALE1_IN;
			}
			else
			{
				nCrossNearTypeTmp = CROSSNEARTYPE_SCALE0_IN;
			}
		}
	}
	//
	if ( nCrossNearType == CROSSNEARTYPE_DEFAULT )
	{
		nCrossNearType = nCrossNearTypeTmp;
	}
	//
	if (   nCrossNearTypeTmp == CROSSNEARTYPE_PIC_IN
		|| nCrossNearTypeTmp == CROSSNEARTYPE_HIGHWAY )
	{
		if( nLenToNextCross >= IN_CROSSDIS_LEVEL_1 )
		{
			nCrossNearTypeTmp = CROSSNEARTYPE_DEFAULT;
		}
		else if( nLenToNextCross > IN_CROSSDIS_LEVEL_2)
		{
			nCrossNearTypeTmp = CROSSNEARTYPE_SCALE1_IN;
		}
		else 
		{
			nCrossNearTypeTmp = CROSSNEARTYPE_SCALE0_IN;
		}
	}
	/**/
	if( nCrossNearTypeTmp == CROSSNEARTYPE_SCALE0_OUT )
	{
		nCrossNearTypeTmp = CROSSNEARTYPE_SCALE0_IN;
	}
	if ( nCrossNearType == CROSSNEARTYPE_SCALE0_OUT  )
	{
		nCrossNearType = CROSSNEARTYPE_SCALE0_IN;
	}
	if (   nCrossNearTypeTmp == CROSSNEARTYPE_SCALE0_IN 
		|| nCrossNearTypeTmp == CROSSNEARTYPE_SCALE1_IN  )
	{
		nCrossNearTypeTmp = CROSSZOOM_TYPE_IN;
	}
	else if (  nCrossNearTypeTmp == CROSSNEARTYPE_SCALE0_OUT 
		    || nCrossNearTypeTmp == CROSSNEARTYPE_SCALE1_OUT)
	{
		nCrossNearTypeTmp = CROSSZOOM_TYPE_OUT;
	}
	else
	{
		nCrossNearTypeTmp = CROSSZOOM_TYPE_DEFAULT;
	}
	g_pInFsApi->pGdata->drawInfoNavi.nCrossZoomType = nCrossNearTypeTmp;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = nLenToNextCross;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToBeforeCross   = nLenToBeforeCross;
// 	if ( nCrossNearTypeTmp == CROSSZOOM_TYPE_IN )
// 	{
// 		g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = nLenToNextCross;
// 	}
// 	else if ( nCrossNearTypeTmp == CROSSZOOM_TYPE_OUT )
// 	{
// 		g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = nLenToBeforeCross;
// 	}
// 	else
// 	{
// 		g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = 0;
// 	}
	//Test
	return nCrossNearType;
}


nuINT CNaviData::GetCarAngle()
{
	nuINT nAngle = (g_pInFsApi->pGdata->carInfo.nCarAngle - ( g_pInFsApi->pGdata->carInfo.nMapAngle - 90) + 360) % 360;
	if( nAngle <= 15 || nAngle >= 345 )
	{
		return 0;
	}
	else
	{
		return (( nAngle - 15)/30 + 1);
	}
	
}

nuUINT CNaviData::GetRoutingList( PROUTINGLISTNODE pRTlist, nuINT nNum, nuINT nIndex)
{
	if( pRTlist == NULL )
	{
		return 0;
	}
	nuINT nRetValue = 0;
	nuBYTE nTraffNum = 0;
	NAVINSS NowNSSInfo,NextNSSInfo;
	nuMemset( &NowNSSInfo, 0, sizeof(NowNSSInfo) );
	nuMemset( &NextNSSInfo, 0, sizeof(NextNSSInfo) );
	PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	ROUTINGLISTNODE tmpnode; 
	nuLONG nLenToLast = 0;
	nuLONG nNodeLen = 0;
	nuLONG nType = AF_LoseHere;
	if( pNFTC == NULL)
	{
		return 0;
	}
	while ( pNFTC != NULL)
	{
		PNAVIRTBLOCKCLASS pNRBC = pNFTC->NRBC;
		while ( pNRBC != NULL) 
		{
			if( pNRBC->NSS != NULL && pNRBC->RTBStateFlag == 0 )
			{
				for( nuDWORD i = 0; i < pNRBC->NSSCount; i ++ )
				{
					nNodeLen   += pNRBC->NSS[i].RoadLength; 
					NowNSSInfo.pNFTC = pNFTC;
					NowNSSInfo.pNRBC = pNRBC;
					NowNSSInfo.pNSS		= &(pNRBC->NSS[i]);
					NowNSSInfo.nIndex	= i;
					if ( NowNSSInfo.pNSS == NULL )
					{
						break;
					}
					
					if( !GetNextNSS( NowNSSInfo,&NextNSSInfo ) )
					{
						break;
					}
					nType = GetRoutingListCode( &(pNRBC->NSS[i]),NextNSSInfo.pNSS );
					tmpnode.point	  = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1];
					if( ChangeIcon( nType ) )
					{
						tmpnode.nIconType = nuWORD(1024 + nType );
					}
					else
					{
					tmpnode.nIconType = nuWORD(1024 + nType * 10 + pNRBC->NSS[i].TurnFlag);
					}
					
					if( NowNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
					{
						nTraffNum = 0;
						NAVINSS Temp1,Temp2;
						Temp1 = NextNSSInfo;
						while(1)
						{
							if(!GetNextNSS(Temp1,&Temp2))
							{
								break;
							}
							if (   Temp1.pNSS == NULL 
								|| Temp2.pNSS == NULL 
								)
							{
								break;
							}
							if( Temp1.pNSS->RoadVoiceType == VRT_TrafficCircle  && Temp2.pNSS->RoadVoiceType == VRT_TrafficCircle)
							{
								nTraffNum++;
							}
							else
							{
								break;
							}
							Temp1 = Temp2;	
						}
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum ; 
					}
					else if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle )
					{
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum;
					}
					nuBOOL bSave = nuFALSE;
					nuBOOL bClear = nuFALSE;
					switch(nType) 
					{
					case AF_In_Freeway:
					case AF_Out_Freeway:
					case AF_In_Fastway:
					case AF_Out_Fastway:
					case AF_In_SlipRoad:
					case AF_In_Overpass:
					case AF_Out_Overpass:
					case AF_In_Tunnel:
					case AF_In_Underpass:
					case AF_In_UnOverpass:
					case AF_In_Tollgate:
					case AF_In_RestStop:
					case AF_In_TrafficCircle:
					case AF_Out_TrafficCircle:
					case AF_IN_FASTROAD:
					case AF_IN_SLOWROAD:
					case AF_CityTownBoundary:
					case AF_MapBoundary:
					case AF_OUT_SLOWROAD:
						   bClear = nuTRUE;
						if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue  )
						{
							bSave = nuTRUE;
							RNEXTEND RnEx = {0};
							nuBOOL bSearch = nuTRUE;
							if ( NextNSSInfo.pNSS->PTSNameAddr > 4  )
							{
								bSearch = !GetPTSName((nuUSHORT)NextNSSInfo.pNRBC->FileIdx, NextNSSInfo.pNSS->PTSNameAddr, tmpnode.wsName , MAX_LISTNAME_NUM *sizeof(nuWCHAR));
							}
							if ( bSearch )
							{
								g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx,NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
							}
							
						}
						break;
					case AF_MidSit:
						bClear = nuTRUE;
						if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue  )
						{
							bSave = nuTRUE;
							g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								
						}
						break;
					case AF_EndSite:
						bClear = nuTRUE;
						if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue  )
						{
							bSave = nuTRUE;
							g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
						}
						break;
					case AF_LoseHere:
						if(   pNRBC->NSS[i].TurnFlag != T_No 
							&& bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ) 	)
						{   
							if( pNRBC->NSS[i].RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
							{
								break;
							}
							else
							{
								bClear = nuTRUE;
								if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue  )
								{
									bSave = nuTRUE;						
									RNEXTEND RnEx = {0};
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
								}
							}
							
						}
						break;
					default:
						break;
					}
					if( bSave )
					{
						tmpnode.nDistance = nNodeLen;
						*pRTlist = tmpnode;
						pRTlist++;
						nRetValue++;
						if (   NowNSSInfo.pNSS
							&& NextNSSInfo.pNSS
							&& NextNSSInfo.pNSS->RoadLength <= LEN_COMBINE
							&& NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle
							//&& NowNSSInfo.pNSS->TurnFlag == T_Afore 
							)
						{
							bColCombinCrossData( &NowNSSInfo, &NextNSSInfo, nNodeLen,nuFALSE );
						}
						nNodeLen = 0;
					}
					else if( bClear )
					{
						nNodeLen = 0;
						nRetValue++;
					}
				}
			}
			else
			{
				if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue  )
				{
					if ( pNRBC->RTBStateFlag == 1  )
					{
						GetRTBName( pNRBC->FileIdx, pNRBC->FInfo.RTBID, tmpnode.wsName );
					}
					else if ( pNRBC->RTBStateFlag == 2 )
					{
						nuWORD nMapCount = g_pInFsApi->FS_GetMapCount();
						if ( nMapCount > 1 )
						{
							PMAPNAMEINFO pTmp = (PMAPNAMEINFO)g_pInFsApi->FS_GetMpnData();
							if (   pTmp 
								&& pNRBC->FileIdx < nMapCount )
							{
								nuMemset(  tmpnode.wsName, 0, sizeof( tmpnode.wsName) );
								nuWcscat( tmpnode.wsName ,(pTmp + pNRBC->FileIdx )->wMapFriendName );
							}
						}
						
					}
					tmpnode.point = pNRBC->FInfo.FixCoor;
					#ifdef VALUE_EMGRT
						tmpnode.nDistance = 0;
					#else
						tmpnode.nDistance = nNodeLen;
					#endif
					tmpnode.nIconType = 5107;
					//GetRTBName( pNRBC->FInfo.MapID, pNRBC->FInfo.RTBID, tmpnode.wsName );
					*pRTlist = tmpnode;
					pRTlist++;
					nNodeLen = pNRBC->RealDis; 
				}
				nRetValue++;
				
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
		if( pNFTC != NULL )
		{
			//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
			if( nRetValue >= nIndex && (nIndex +  nNum ) > nRetValue)
			{
				tmpnode.nDistance = nNodeLen;
				tmpnode.point     = pNFTC->FCoor;
				tmpnode.nIconType = nuWORD(1024 + AF_MidSit * 10);
				g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
				*pRTlist = tmpnode;
				pRTlist++;
				nNodeLen = 0;
			}
			nRetValue++;
		}
	}
	if( nRetValue < nNum + nIndex )
	{
		tmpnode.nDistance	= nNodeLen;
		tmpnode.nIconType	= 1024 + AF_EndSite * 10;
		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
		PNAVIFROMTOCLASS PTmpNFTC2 = NULL ;
		while( pTmpNFTC )
		{
			PTmpNFTC2 = pTmpNFTC;
			pTmpNFTC  = pTmpNFTC->NextFT;
		}
		if( PTmpNFTC2 )
		{
			tmpnode.point.x		= PTmpNFTC2->TCoor.x;
			tmpnode.point.y		= PTmpNFTC2->TCoor.y;
		}
		nuWCHAR wc[20];
		nuMemset( wc, 0, sizeof(wc) );
		g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
		*pRTlist = tmpnode;
		pRTlist++;
		nNodeLen = 0;
	}
	
	nRetValue++;
	return nRetValue;
}
nuBOOL CNaviData::ColSecCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo)
{
	PNAVISUBCLASS pNowNSS  = NowNSSInfo.pNSS;
	PNAVISUBCLASS pNextNSS = NextNSSInfo.pNSS;
	NAVINSS   NextNSSInfo1 ;
	nuMemset( &NextNSSInfo1, 0, sizeof(NextNSSInfo1) );
	nuLONG xx = 0;
	nuBOOL bStop = nuFALSE;
	while ( !bStop ) 
	{
		xx = GetRoutingListCode( pNowNSS, pNextNSS );
		if ( NowNSSInfo.pNSS )
		{
			m_struSeachInfo.nSecLen +=  NowNSSInfo.pNSS->RoadLength;
		}
		if( xx == AF_LoseHere )
		{
			if(    NowNSSInfo.pNSS->TurnFlag != T_No
				&& bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ))
			{
				bStop = nuTRUE;
				return nuTRUE;
			}
		}
		else
		{
			bStop = nuTRUE;
			return nuTRUE;
		}
		pNowNSS = pNextNSS;
		if( GetNextNSS(NextNSSInfo,&NextNSSInfo1) )
		{
			pNextNSS = NextNSSInfo1.pNSS;	
		}
		else
		{
			bStop = nuTRUE;
			return nuTRUE;
		}
	}
	   return nuTRUE;
}

nuBOOL CNaviData::ColZoomCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo)
{
	 /**/
	if(    m_NaviForZoomPre.pNFTC == NULL
		&& m_NaviForZoomPre.pNRBC == NULL
		&& m_NaviForZoomPre.pNSS  == NULL
		&& m_NaviForZoom.pNFTC    == NULL 
		&& m_NaviForZoom.pNRBC    == NULL
		&& m_NaviForZoom.pNSS     == NULL )   //ç¬¬ä?äžªäº€?ç¹
	{  
		m_nNowLen = 0;
		//m_NaviForZoomPre = m_NaviForZoom;
		m_NaviForZoom    = NowNSSInfo;
	}
	else if (  m_NaviForZoom.pNFTC != NowNSSInfo.pNFTC 
			|| m_NaviForZoom.pNRBC != NowNSSInfo.pNRBC
			|| m_NaviForZoom.pNSS  != NowNSSInfo.pNSS
		     )
	{
		m_nNowLen = 0;
		NAVINSS TmpNSSInfo = m_NaviForZoom;
		NAVINSS TmpNextNSS = {0};
		//m_nNowLen = TmpNSSInfo.pNSS->RoadLength;
		while( GetNextNSS( TmpNSSInfo, &TmpNextNSS ))
		{
			if (   TmpNSSInfo.pNFTC   == NowNSSInfo.pNFTC 
				&& TmpNSSInfo.pNRBC   == NowNSSInfo.pNRBC
				&& TmpNSSInfo.pNSS    == NowNSSInfo.pNSS
				)
			{
				break;
			}
			if ( TmpNextNSS.pNSS == NULL )
			{
				m_nNowLen += TmpNextNSS.pNRBC->RealDis;
			}
			else
			{
				m_nNowLen += TmpNextNSS.pNSS->RoadLength;
			}

			TmpNSSInfo.pNFTC	 = TmpNextNSS.pNFTC;
			TmpNSSInfo.pNRBC	 = TmpNextNSS.pNRBC;
			TmpNSSInfo.pNSS		 = TmpNextNSS.pNSS;
			TmpNSSInfo.nIndex	 = TmpNextNSS.nIndex;
		}
		m_NaviForZoomPre = m_NaviForZoom;
		m_NaviForZoom    = NowNSSInfo;
		//Test
		if (   m_nNowLen - g_pInFsApi->pDrawInfo->pcDrawInfoNavi->nextCross.nDistance < 0
			 )
		{
			m_nNowLen = 0;
		}
		//m_nNowLen += g_pInFsApi->pDrawInfo->pcDrawInfoNavi->nextCross.nDistance;
	}
	return nuTRUE;
  
   
	/*nuBOOL  bRet = JudgeCross( NowNSSInfo, NextNSSInfo );
	if ( NowNSSInfo.pNRBC->NSS == NULL )
	{
		m_pNRBC = NowNSSInfo.pNRBC;
	}
	if( bRet )
	{
		nuBOOL bEndSit = nuFALSE;
		nuBOOL bFirstSit = nuFALSE;
		m_nNowLen = 0;
		if(  NextNSSInfo.pNFTC == NULL 
			&& NextNSSInfo.pNRBC == NULL
			&& NextNSSInfo.pNSS  == NULL )   //ç»ç¹
		{
			bEndSit = nuTRUE;
		}
		if(    m_NaviForZoom.pNFTC != NextNSSInfo.pNFTC 
			|| m_NaviForZoom.pNRBC != NextNSSInfo.pNRBC
			|| m_NaviForZoom.pNSS  != NextNSSInfo.pNSS )  //åœå??è·¯?¿åºŠ?¯åŠå·²ç?è®¡ç?è¿?
		{
			if (  m_NaviForZoom.pNFTC != NextNSSInfo.pNFTC
				|| m_NaviForZoom.pNRBC != NextNSSInfo.pNRBC
				|| m_NaviForZoom.pNSS  != NextNSSInfo.pNSS )
			{
				m_NaviForZoomPre = m_NaviForZoom;
				m_NaviForZoom = NextNSSInfo;
			}
			if(    m_NaviForZoomPre.pNFTC ==  NULL
				&& m_NaviForZoomPre.pNRBC == NULL
				&& m_NaviForZoomPre.pNSS  == NULL )   //ç¬¬ä?äžªäº€?ç¹
			{  
				bFirstSit = nuTRUE;
				m_nNowLen = 0;
			}
			else if( NowNSSInfo.pNRBC == m_pNRBC && NowNSSInfo.pNRBC->NSS != NULL )
			{
				bFirstSit = nuTRUE;
				m_nNowLen = 0;
				m_NaviForZoom = NextNSSInfo;
				nuMemset( &m_NaviForZoomPre, 0, sizeof(m_NaviForZoomPre) );
			}
			else
			{
				NAVINSS NextNSS ;
				NAVINSS NSSPre;
				nuMemset( &NextNSS, 0 ,sizeof(NextNSS));
				nuMemset( &NSSPre, 0 ,sizeof(NSSPre) );
				if ( !m_NaviForZoomPre.pNSS )
				{
					NSSPre = m_NaviForZoomPre;
				}
				NSSPre = m_NaviForZoomPre;
				
				while ( GetNextNSS( NSSPre, &NextNSS ))//åœå???äžªNSSä¹éŽå­åš?¶ä?Cross?æ¶?ï??ä?äžªè??Žï??¯ä??ä??Žæ²¡?å¶ä»ç?Cross
				{
					if( !NSSPre.pNSS )
					{
						return nuFALSE;
					}
					if(JudgeCross( NSSPre, NextNSS ))
					{
						if (   NextNSS.pNFTC == m_NaviForZoom.pNFTC
							&& NextNSS.pNRBC == m_NaviForZoom.pNRBC
							&& NextNSS.pNSS  == m_NaviForZoom.pNSS )
						{
							break;
						}
						else

						{
							m_NaviForZoomPre = NextNSS;
						}
					}
					NSSPre.pNFTC		= NextNSS.pNFTC;
					NSSPre.pNRBC		= NextNSS.pNRBC;
					NSSPre.pNSS			= NextNSS.pNSS;
					NSSPre.nIndex		= NextNSS.nIndex;
				}
			}
		}
		//?·å?åœå??è·¯?é¿åº?

		if( !bFirstSit )
		{
			NAVINSS TmpNSSInfo = m_NaviForZoomPre;
			NAVINSS TmpNextNSS;
			nuMemset( &TmpNextNSS, 0, sizeof(TmpNextNSS) );
			
			
			//m_nNowLen = pTmpNSSInfo->pNSS->RoadLength;
			while(GetNextNSS( TmpNSSInfo, &TmpNextNSS ))
			{
				if( !TmpNextNSS.pNSS )
				{
					return nuFALSE;
				}
				if( TmpNSSInfo.pNSS )




				{
					m_nNowLen += TmpNSSInfo.pNSS->RoadLength;
					if( !bEndSit )
					{
						if (   TmpNextNSS.pNFTC == m_NaviForZoom.pNFTC
							&& TmpNextNSS.pNRBC == m_NaviForZoom.pNRBC
							&& TmpNextNSS.pNSS  == m_NaviForZoom.pNSS ) 
						{
							break;
						}
					}	
					TmpNSSInfo.pNFTC	 = TmpNextNSS.pNFTC;
					TmpNSSInfo.pNRBC	 = TmpNextNSS.pNRBC;
					TmpNSSInfo.pNSS		 = TmpNextNSS.pNSS;
					TmpNSSInfo.nIndex	 = TmpNextNSS.nIndex;
				}
				else
				{
					m_nNowLen += TmpNSSInfo.pNRBC->RealDis;
					TmpNSSInfo.pNFTC	 = TmpNextNSS.pNFTC;
					TmpNSSInfo.pNRBC	 = TmpNextNSS.pNRBC;
					TmpNSSInfo.pNSS		 = TmpNextNSS.pNSS;
					TmpNSSInfo.nIndex	 = TmpNextNSS.nIndex;
				}	
			}
			if (   bEndSit
				&& TmpNSSInfo.pNSS != NULL
				&& !GetNextNSS( TmpNSSInfo, &TmpNextNSS ))
			{
				m_nNowLen += TmpNSSInfo.pNSS->RoadLength;
			}
		}		
		m_struSeachInfo.nZoomNum++;
	}
	return bRet;
	*/
}

#define  ARROWLEN  40
nuBOOL CNaviData::DrawArrow(nuBYTE bZoom)
{
	if (1 == m_nGoNavi)
	{
		if( DrawArrow2(bZoom) )
		{
			return nuTRUE;
		}
	}
	
	if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	JudgeArrowNSS( bZoom );

	PNAVIFROMTOCLASS TmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 

	nuBYTE nDrawType = 0;
	NURORECT RectDraw = {0};
	nuBYTE n3DMode = 0;
	nuINT nDrawObj = 0;
	typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
	MT_MapToBmpProc pf_MapToBmp;
	nuLONG nSkipLen = 0, nLineWidth = 0;
	nuLONG nDrawArrowLen = 0;
	if ( m_nPreScal == 0xFF || m_nPreScal != g_pInFsApi->pGdata->uiSetData.nScaleIdx )
	{
		m_nPreScal = g_pInFsApi->pGdata->uiSetData.nScaleIdx;
	}
	if( bZoom == MAP_ZOOM )
	{
		if( !g_pInFsApi->pGdata->zoomScreenData.bZoomScreen )
		{
			return nuFALSE;
		}
		if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= 1 )
		{
			nLineWidth = m_nArrowLineWidth1;
		}
		else if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= m_nScalShowArrow )
		{
			nLineWidth = m_nArrowLineWidth2;
		}
		else
		{
			return nuFALSE;
			nLineWidth = 10;
		}
		nDrawType	= DRAW_TYPE_ZOOM_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp = g_pInMtApi->MT_ZoomMapToBmp ;
		n3DMode		= g_pInFsApi->pGdata->zoomScreenData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
		if( n3DMode )
		{
			nDrawObj = DRAW_OBJ_ZOOM_3D_ARROWLINE;	
		}
		else
		{
			nDrawObj = DRAW_OBJ_ZOOM_2D_ARROWLINE;	
		}
		nDrawArrowLen = ZOOMLEN * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / SCALEBASE; 
	}
	else
	{
		if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= 1 )
		{
			nLineWidth = m_nArrowLineWidth1;
		}
		else if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= m_nScalShowArrow  )
		{
			nLineWidth = m_nArrowLineWidth2;
		}
		else
		{
			return nuFALSE;
			nLineWidth = 8;
		}
		nDrawType	= DRAW_TYPE_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
		pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
		n3DMode		= g_pInFsApi->pGdata->uiSetData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
		if( n3DMode )
		{
			nDrawObj = DRAW_OBJ_3D_ARROWLINE;	
		}
		else
		{
			nDrawObj = DRAW_OBJ_2D_ARROWLINE;	
		}
		nDrawArrowLen = GENLEN * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / SCALEBASE;
	}
	nuLONG dLen = 0;
	
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
	if(   ( m_nNowLen == 0
		|| m_bDrawBefoeArrow
		||  (  nLenToBeforeCross > 100 || nLenToNextCross < 100)
		)  
		&& ( nuMemcmp( &g_pInFsApi->pGdata->drawInfoNavi.nextCross.point,
			&m_CrossPoint.ptArray[m_CrossPoint.nCenterIndex],
			sizeof(NUROPOINT) )
			|| m_nCrossIndexInSouce != g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIdxCrossInList
		   ) )
	{
	#ifndef VALUE_EMGRT
		if ((g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag != T_LUturn && nLenToBeforeCross > 100 ) || 
			(g_pInFsApi->pGdata->drawInfoNavi.lTurnFlag != T_LUturn && nLenToNextCross < 100 ))
		{
			m_bCombinDrawArrow = nuFALSE;
			m_nCombinLen =0;
		}
    #endif
		CROSSPOINT TmpCross = {0};
		TmpCross.ptArray = g_pInFsApi->pGdata->drawInfoNavi.nextCross.ptCrossList;
		TmpCross.nPtNum = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nCrossPtCount;
		TmpCross.nCenterIndex = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIdxCrossInList;
		SelPt( &TmpCross, &m_CrossPoint, ARROWLEN );		
	}
	NUROPOINT ptDrawTmp[CROSS_ARROW_POINT_NUM] = {0};
	CROSSPOINT TmpCross = {0};
	TmpCross.ptArray = &ptDrawTmp[0];
	SelPt( &m_CrossPoint,
		   &TmpCross,  
		   GetArrowLen(),
		   nuFALSE);

    if( !nuPointInRect( &TmpCross.ptArray[TmpCross.nCenterIndex], &(RectDraw)) )
	{
		return nuFALSE;
	}
	for ( nuINT nIndex = 0; nIndex < TmpCross.nPtNum; nIndex++ )
	{ 
		pf_MapToBmp(TmpCross.ptArray[nIndex].x, TmpCross.ptArray[nIndex].y, &TmpCross.ptArray[nIndex].x, &TmpCross.ptArray[nIndex].y);
	}
	g_pInMtApi->MT_DrawObject(nDrawObj, TmpCross.ptArray, TmpCross.nPtNum , nLineWidth);
    return nuTRUE;
	
}

nuBOOL CNaviData::JudgeArrowNSS( nuBYTE bZoom )
{
	nuLONG nLenToNextCross = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
	nuBOOL bChange = nuFALSE;
	// 	nuLONG nSkipLen = 0;
	// 	if ( bZoom == MAP_ZOOM)
	// 	{
	// 		nSkipLen = 20;
	// 	}
	// 	else
	// 	{
	// 		nSkipLen  = 100; 
	// 	}
	nuINT nArrowCrossNum = 0;
	if( m_struSeachInfo.nFindCrossNum == 2)
	{
		if( m_struSeachInfo.bInTraf )
		{
			nArrowCrossNum= 2;
		}
		else
		{
			if( m_struSeachInfo.SoundPlayNext.nDistance - m_struSeachInfo.SoundPlayFirst.nDistance < JUDGELEN )
			{
				nArrowCrossNum = 2;
			}
			else
			{
				nArrowCrossNum	= 1;
			}
		}
	}
	else
	{
		nArrowCrossNum	= 1;
	}
	
	if( m_nNowLen == 0 ) //Frist In
	{
		bChange = nuTRUE;
		if( nArrowCrossNum == 2 )
		{
			m_NaviForArrow = m_struSeachInfo.SoundPlayNext.NaviInfo; 
			m_nArrowCrossNum = 2;
		}
		else if(nArrowCrossNum == 1 )
		{
			m_NaviForArrow = m_struSeachInfo.SoundPlayFirst.NaviInfo;
			m_nArrowCrossNum = 1;
		}
	}
	
	else if ( nLenToBeforeCross > ARROWKEEPLEN || nLenToNextCross < 100 )
	{
		if( m_nArrowCrossNum == 1 )
		{
			if (   m_NaviForArrow.pNFTC != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNFTC 
				|| m_NaviForArrow.pNRBC != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNRBC
				|| m_NaviForArrow.pNSS  != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNSS )
			{
				bChange = nuTRUE;
			}
		}
		else if ( m_nArrowCrossNum == 2 )
		{
			
			if (   m_NaviForArrow.pNFTC != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNFTC 
				|| m_NaviForArrow.pNRBC != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNRBC
				|| m_NaviForArrow.pNSS  != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNSS )
			{
				NAVINSS NextNaviNSS;
				nuMemset( &NextNaviNSS, 0, sizeof(NextNaviNSS) );
				PNAVINSS pNextNaviNSS = &NextNaviNSS;
				if ( GetNextNSS( m_struSeachInfo.SoundPlayFirst.NaviInfo, pNextNaviNSS) )
				{
					if(    m_NaviForArrow.pNFTC != pNextNaviNSS->pNFTC 
						|| m_NaviForArrow.pNRBC != pNextNaviNSS->pNRBC
						|| m_NaviForArrow.pNSS  != pNextNaviNSS->pNSS  )
					{
						bChange = nuTRUE;
					}
				}
				
			}
		}
	}	
	if( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance == g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget )
	{
		bChange = nuFALSE;
		m_ptLastNextCross.pNFTC = NULL;
	}
	if( bChange )
	{
		m_ptLastNextCross	= m_struSeachInfo.SoundPlayFirst.NaviInfo; 
		m_nArrowCrossNum =  nArrowCrossNum;
		if( nArrowCrossNum == 2 )
		{
			m_NaviForArrow = m_struSeachInfo.SoundPlayNext.NaviInfo;
			if ( m_struSeachInfo.bInTraf )
			{
				m_ptNextCross = m_struSeachInfo.SoundPlayNext.NaviInfo;
			}
			else
			{
				if(m_struSeachInfo.SoundPlayNext.nDistance - m_struSeachInfo.SoundPlayFirst.nDistance < JUDGELEN )
				{
					m_ptNextCross = m_struSeachInfo.SoundPlayNext.NaviInfo;
				}
				else
				{
					m_ptNextCross.pNFTC = NULL;
				}
			}
		}	
		else
		{
			m_NaviForArrow = m_struSeachInfo.SoundPlayFirst.NaviInfo;
			m_ptNextCross.pNFTC = NULL;
		}
	}
	
	return nuTRUE;
}

nuBOOL CNaviData::JudgeCross(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo)
{
	if( NowNSSInfo.pNSS == NULL )
	{
		return  nuFALSE;
	}
	if(    NextNSSInfo.pNSS  == NULL 
		&& NextNSSInfo.pNFTC != NULL
		&& NextNSSInfo.pNRBC  != NULL )  //ç»ç??¹æ??æ¥äžæ¥?Blockäž­ç?NSSäžºNULLæ²¡æ??å???
	{
		return  nuFALSE;
	}
	else if (   NextNSSInfo.pNSS  == NULL
		&& NextNSSInfo.pNFTC == NULL
		&& NextNSSInfo.pNRBC  == NULL)
	{
		return nuTRUE;
	}
	if( NowNSSInfo.pNFTC != NextNSSInfo.pNFTC   )  //Pass Node
	{
		return nuTRUE;
	}
	nuLONG xx = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
	if(xx == AF_LoseHere)
	{
		if(    NowNSSInfo.pNSS->TurnFlag != T_No
			&& bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ) )
		{
			return nuTRUE;
		}
		else if(NowNSSInfo.pNSS->Real3DPic != 0 && NowNSSInfo.pNSS->RoadClass > 3)
		{
			return nuTRUE;
		}
	}
	else if(  NowNSSInfo.pNSS->RoadClass > 3  
		|| NextNSSInfo.pNSS->RoadClass > 3   )
	{ 
		return nuTRUE;
	}
	
	return nuFALSE;
}
nuBOOL CNaviData::GetRTBName(nuLONG MapIdx,nuLONG RTBID,nuWCHAR *RTBName)
{
	nuFILE *RTNFile = NULL;
	char NameLen=0;
	nuMemset( RTBName, 0, LIST_NAME_NUM * sizeof(nuWCHAR));
	nuTCHAR filename[260]  = {0};
	/*nuGetModuleFileName(NULL,filename,260);
	for(nuINT i =nuTcslen(filename) - 1; i >= 0; i--)
	{
		if( filename[i] == '\\' )
		{
			filename[i+1] = '\0';
			break;
		}
	}
	nuTcscat( filename, nuTEXT("MAP\\TW\\TW.rtn") );*/

	nuTcscpy(filename, nuTEXT(".rtn"));
	if( !g_pInFsApi->FS_FindFileWholePath( MapIdx, filename, NURO_MAX_PATH) )
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "FindFileWholePath GetRTBName Fail %s,  MapIdx %d", filename);
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "NaviList", "GetRTBName filename: %s, MapIdx %d, RTBID %d", filename, MapIdx, RTBID);
	RTNFile = nuTfopen(filename, NURO_FS_RB);
	if( RTNFile == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuTfopen GetRTBName Fail");
		return nuFALSE;
	}
	if(0 != nuFseek(RTNFile,0,NURO_SEEK_SET))
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuFseek GetRTBName Fail");
		nuFclose(RTNFile);
		return nuFALSE;
	}
	if( nuFread(&NameLen,1,1,RTNFile) != 1 )
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuFread GetRTBName Fail");
		nuFclose( RTNFile );
		return nuFALSE;
	}
	__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuFread GetRTBName NameLen %d", NameLen);
	if(0 != nuFseek(RTNFile,1+(NameLen*RTBID),NURO_SEEK_SET))
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuFseek GetRTBName Fail");
		nuFclose(RTNFile);
		return nuFALSE;
	}
	if( nuFread( RTBName,NameLen,1,RTNFile) != 1 )
	{
		__android_log_print(ANDROID_LOG_INFO, "NaviList", "nuFread GetRTBName Fail");
		nuFclose( RTNFile );
		return nuFALSE;
	}
	nuFclose( RTNFile );
	return nuTRUE;
}

nuBOOL CNaviData::JudgeDraw3DPic()
{
	if( m_struSeachInfo.n3DPic == 0 )
	{
		return nuFALSE;
	}
	/*NAVINSS	 NowNSSInfo = {0}, NextNSSInfo = {0};
	NowNSSInfo.pNFTC  = m_ptNavi.pNFTC;
	NowNSSInfo.pNRBC  = m_ptNavi.pNRBC;
	NowNSSInfo.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
	NowNSSInfo.nIndex = m_ptNavi.nSubIdx;
	GetNextNSS(NowNSSInfo , &NextNSSInfo);
	if(( NowNSSInfo.pNSS->RoadVoiceType <= 4 && NowNSSInfo.pNSS->RoadVoiceType != 3) &&
		( NextNSSInfo.pNSS->RoadVoiceType <= 4 && NextNSSInfo.pNSS->RoadVoiceType != 3))
	{
		return nuFALSE;
	}*/
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	if( nLenToNextCross <= 600 )
	{
		return nuTRUE;
	}
    return nuFALSE;
}

nuBOOL CNaviData::GetHighWayList()
{
	if ( 1 == m_nGoNavi )
	{
		return GetRtListNew();
	}

	NAVINSS NowNSSInfo,NextNSSInfo;
	
	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum = 0;
	//
	PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	SHOWNODE tmpnode ={0}; 
	nuLONG nLenToLast = 0;
	nuLONG nNodeLen = 0;
	nuLONG nType = AF_LoseHere;
	nuBYTE nTraffNum = 0;
	nuBOOL bEndInHighWay = nuFALSE;
	if( pNFTC == NULL)
	{
		return nuFALSE;
	}
	nuMemset( &NowNSSInfo, 0, sizeof(NowNSSInfo) );
	nuMemset( &NextNSSInfo, 0, sizeof(NextNSSInfo) );
	while ( pNFTC != NULL)
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while ( pNRBC != NULL) 
		{
			if(  (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum >= g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum)
				|| (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx > g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{
				break;
			}
			if ( pNRBC->RTBStateFlag == 1 && pNRBC->NSS == NULL )
			{
				if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
				{
					g_pInRTApi->RT_LocalRoute( &pNRBC );
				}
			}
			if( pNRBC->NSS != NULL && pNRBC->RTBStateFlag == 0 )
			{
				nuINT nFrist =0;
				if( pNRBC == m_ptNavi.pNRBC )
				{
					nFrist = m_ptNavi.nSubIdx; 
				}
				for( nuDWORD i = nFrist; i < pNRBC->NSSCount; i ++ )
				{	
					if(    &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]) == &(pNRBC->NSS[i])
						&& &pNRBC->NSS[i] != NULL )
					{
						nuLONG dx = 0, dy = 0, dxy = 0;
						dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
						dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
						dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
						nNodeLen += dxy;
						nuLONG nTotalLen = 0;
						for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
						{
							dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
							dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
							if ( j >= m_ptNavi.nPtIdx + 1)
							{
								nNodeLen += dxy;
							}
							nTotalLen += dxy;
						}
						if (  nTotalLen )
						{
							nNodeLen = nNodeLen * pNRBC->NSS[i].RoadLength / nTotalLen;
						}
						
						
					}
					else if ( &pNRBC->NSS[i] != NULL )
					{
						nNodeLen   += pNRBC->NSS[i].RoadLength; 
					}
					
					NowNSSInfo.pNFTC = pNFTC;
					NowNSSInfo.pNRBC = pNRBC;
					NowNSSInfo.pNSS		= &(pNRBC->NSS[i]);
					NowNSSInfo.nIndex	= i;
					if ( NowNSSInfo.pNSS == NULL )
					{
						break;
					}
					if(    NowNSSInfo.pNSS->RoadClass > 3 
						&& (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  )  )
					{
						if( NowNSSInfo.pNSS->RoadClass == 5 ) //äº€æ???
						{
							NAVINSS TmpNextNSS,TmpNowNSS;
							nuMemset( &TmpNextNSS, 0, sizeof(TmpNextNSS) );
							nuMemset( &TmpNowNSS,  0, sizeof(TmpNowNSS) );
							nuMemcpy( &TmpNowNSS, &NowNSSInfo, sizeof(TmpNowNSS) );
							if ( !GetNextNSS( TmpNowNSS,&TmpNextNSS ) )
							{
								break;
							}
                            if (   TmpNextNSS.pNFTC == NULL 
								|| TmpNextNSS.pNRBC == NULL
								|| TmpNextNSS.pNSS  == NULL
								)
                            {
								continue;
                            }
							while( TmpNextNSS.pNSS->RoadClass == 5  )
							{
								nuMemcpy( &TmpNowNSS, &TmpNextNSS, sizeof(TmpNowNSS) );
								if( !GetNextNSS( TmpNowNSS,&TmpNextNSS ))
								{
									break;
								}
								if ( TmpNextNSS.pNSS == NULL )
								{
									if ( TmpNowNSS.pNSS->RoadClass <= 3 )
									{
										bEndInHighWay = nuTRUE;
										m_StrHighWayLast.nDWIndex   = TmpNowNSS.pNRBC->FileIdx;
										m_StrHighWayLast.nBlkIndex  = TmpNowNSS.pNSS->BlockIdx;
										m_StrHighWayLast.nRdIndex   = TmpNowNSS.pNSS->RoadIdx;
										nuLONG nDx,nDy;
										nDx = TmpNowNSS.pNSS->ArcPt[1].x - TmpNowNSS.pNSS->ArcPt[0].x;
										nDy = TmpNowNSS.pNSS->ArcPt[1].y - TmpNowNSS.pNSS->ArcPt[0].y;
										m_StrHighWayLast.nAngle     = nulAtan2( nDy, nDx );
										m_StrHighWayLast.ptMap      = TmpNowNSS.pNSS->ArcPt[TmpNowNSS.pNSS->ArcPtCount - 1 ];
										m_StrHighWayLast.ptNextCoor = m_StrHighWayLast.ptMap; 
										m_StrHighWayLast.bEndPt     = nuTRUE;
									}
									break;
								}

							}
							if (   TmpNextNSS.pNFTC  != NULL
								&& TmpNextNSS.pNRBC  != NULL 
								&& TmpNextNSS.pNSS   != NULL
								)
							{
								if ( TmpNextNSS.pNSS->RoadClass > 3 ) //äžåšé«éä???
								{
									return nuTRUE;
								}
							}
							else//è¡šç€ºåœå??©ä??éœ?¯äº€æµé?
							{
								return nuTRUE;
							}
						}
						else
						{
							return nuTRUE;
						}
					}
					GetNextNSS( NowNSSInfo,&NextNSSInfo );					
					if( ( NextNSSInfo.pNSS == NULL ) )
					{
						bEndInHighWay = nuTRUE;
						m_StrHighWayLast.nDWIndex   = NowNSSInfo.pNRBC->FileIdx;
						m_StrHighWayLast.nBlkIndex  = NowNSSInfo.pNSS->BlockIdx;
						m_StrHighWayLast.nRdIndex   = NowNSSInfo.pNSS->RoadIdx;
						nuLONG nDx,nDy;
						nDx = NowNSSInfo.pNSS->ArcPt[1].x - NowNSSInfo.pNSS->ArcPt[0].x;
						nDy = NowNSSInfo.pNSS->ArcPt[1].y - NowNSSInfo.pNSS->ArcPt[0].y;
						m_StrHighWayLast.nAngle     = nulAtan2( nDy, nDx );
						m_StrHighWayLast.ptMap      = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1 ];
						m_StrHighWayLast.ptNextCoor = m_StrHighWayLast.ptMap; 
						m_StrHighWayLast.bEndPt     = nuTRUE;
						break;
					}
					nType = GetRoutingListCode( &(pNRBC->NSS[i]),NextNSSInfo.pNSS );
					nuBOOL bInOROutHighWay ;//=  ( (pNRBC->NSS[i].RoadClass <= 3) && (NextNSSInfo.pNSS->RoadClass <= 3) ) ? nuFALSE:nuTRUE;
					if (   pNRBC->NSS[i].RoadClass <= 3
						&& NextNSSInfo.pNSS->RoadClass > 3 )
					{
						bInOROutHighWay = nuTRUE;
					}
					else
					{
						bInOROutHighWay = nuFALSE;
					}
					
					tmpnode.point	  = NextNSSInfo.pNSS->ArcPt[0];
					if( ChangeIcon( nType ) )
					{
						tmpnode.nIconType = nuWORD(1024 + nType );
					}
					else
					{
					tmpnode.nIconType = nuWORD(1024 + nType * 10 + pNRBC->NSS[i].TurnFlag);
					}
					
					/*switch ( nType )
					{
					case AF_In_Tollgate:
					tmpnode.nIconType = 5024;
					break;
					
					}*/
					
					if( NowNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
					{
						nTraffNum = 0;
						NAVINSS Temp1,Temp2;
						Temp1 = NextNSSInfo;
						while(1)
						{
 							if(!GetNextNSS(Temp1,&Temp2))
							{
								break;
							}
							if (   Temp1.pNSS == NULL
								|| Temp2.pNSS == NULL
								)
							{
								break;
							}
							if( Temp1.pNSS->RoadVoiceType == VRT_TrafficCircle  && Temp2.pNSS->RoadVoiceType == VRT_TrafficCircle)
							{
								nTraffNum++;
							}
							else
							{
								break;
							}
							Temp1 = Temp2;	
						}
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum ; 
					}
					else if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle )
					{
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum;
					}
					
					nuINT tmpType = 1;
					switch( tmpType/*g_pInFsApi->pGdata->drawInfoNavi.nRtListType */) 
					{
					case ROUTELIST_TYPE_CROSS:
						switch(nType) 
						{
						case AF_In_Freeway:
						case AF_Out_Freeway:
						case AF_In_Fastway:
						case AF_Out_Fastway:
						case AF_In_TrafficCircle:
						case AF_Out_TrafficCircle:
						case AF_In_Overpass:
						case AF_Out_Overpass:
						case AF_IN_SLOWROAD:
						case AF_OUT_SLOWROAD:
						case AF_In_SlipRoad:
						case AF_In_Tunnel:
						case AF_In_Underpass:
						case AF_In_UnOverpass:
						case AF_In_Tollgate:
						case AF_In_RestStop:
						case AF_IN_FASTROAD:
						case AF_CityTownBoundary:
						case AF_MapBoundary:
						case AF_IN_AUXROAD:
						case AF_IN_MAINROAD:
							
							//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								//GetRoadName(&(pNRBC->NSS[i]),tmpnode.wsName);
								RNEXTEND RnEx = {0};
								nuBOOL bSearch = nuTRUE;
								if ( NextNSSInfo.pNSS->PTSNameAddr > 4  )
								{
									bSearch = !GetPTSName((nuUSHORT)NextNSSInfo.pNRBC->FileIdx, NextNSSInfo.pNSS->PTSNameAddr, tmpnode.wsName , MAX_LISTNAME_NUM *sizeof(nuWCHAR));
								}
								if ( bSearch )
								{
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx,NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
								}
										
								AddListNode(tmpnode, bInOROutHighWay );
								if (  nType == AF_Out_Freeway )
								{
									m_StrHighWayLast.nDWIndex   = NowNSSInfo.pNRBC->FileIdx;
									m_StrHighWayLast.nBlkIndex  = NowNSSInfo.pNSS->BlockIdx;
									m_StrHighWayLast.nRdIndex   = NowNSSInfo.pNSS->RoadIdx;
									nuLONG nDx,nDy;
									nDx = NowNSSInfo.pNSS->ArcPt[1].x - NowNSSInfo.pNSS->ArcPt[0].x;
									nDy = NowNSSInfo.pNSS->ArcPt[1].y - NowNSSInfo.pNSS->ArcPt[0].y;
									m_StrHighWayLast.nAngle     = nulAtan2( nDy, nDx );
									m_StrHighWayLast.ptMap      = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1 ];
									m_StrHighWayLast.ptNextCoor = m_StrHighWayLast.ptMap;
									m_StrHighWayLast.bEndPt     = nuFALSE;
								}
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_MidSit:
							//	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_EndSite:
							//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_LoseHere:
							if(  pNRBC->NSS[i].TurnFlag != T_No 
								&& bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ))
							{   
								if( pNRBC->NSS[i].RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
								{
									break;
								}
								else
								{
									//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
									if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
									{
										tmpnode.nDistance = nNodeLen;
										RNEXTEND RnEx = {0};
										g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
										AddListNode(tmpnode,bInOROutHighWay);
										nNodeLen = 0;
									}
									else
									{
										break;
									}
								}
								
							}
							break;
						default:
							break;
						}
						break;
						case ROUTELIST_TYPE_PASSNODE:
							switch(nType) 
							{
							case AF_Has_SlipRoad:
							case AF_Has_RestStop:
								//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
								if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
								{
									tmpnode.nDistance = nNodeLen;
									RNEXTEND RnEx = {0};
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
									AddListNode(tmpnode,bInOROutHighWay);
									nNodeLen = 0;
								}
								else
								{
									break;
								}
								nNodeLen = 0;
								break;
							default:
								break;
							}
							break;
							default:
								break;
					}
					if(    nType == AF_LoseHere
						&& pNRBC->NSS[i].RoadClass <= 3 
						&& NextNSSInfo.pNSS->RoadClass <= 3/**/
						)
					{
						if ( NowNSSInfo.pNSS->CrossCount != 0 )
						{
                            nuBOOL bFindNCC = nuFALSE;
							nuUINT nFindIndex = 0;
							nuBOOL bStop = nuFALSE;
							nuUINT count = 0;
							for ( ; count < NowNSSInfo.pNSS->CrossCount; count++)
							{			
								if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NextNSSInfo.pNSS->BlockIdx 
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NextNSSInfo.pNSS->RoadIdx
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NextNSSInfo.pNSS->RoadNameAddr )
								{
									continue;
								}
								if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NowNSSInfo.pNSS->BlockIdx 
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NowNSSInfo.pNSS->RoadIdx
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NowNSSInfo.pNSS->RoadNameAddr )
								{
									continue;
								}
								if (   NowNSSInfo.pNSS->NCC[count].CrossClass == 5
									/*&& NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Freeway*/ )
								{
									NowNSSInfo.pNSS->NCC[count].CrossVoiceType = VRT_SlipRoad;
								}
								if(    NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_SlipRoad
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_RestStop
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Tollgate
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_StopcockWay
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_GuideWay )
								{
									if (  NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_RestStop )
									{
										if ( bFindNCC )
										{
											if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx       == NowNSSInfo.pNSS->NCC[nFindIndex].CrossBlockIdx
												&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx        == NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadIdx
												&& NowNSSInfo.pNSS->NCC[count].CrossClass          == NowNSSInfo.pNSS->NCC[nFindIndex].CrossClass
												&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr   == NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr
												)
											{
												bStop = nuTRUE;
											}
										}
										else
										{
											bStop = nuTRUE;
										}	
									}
									/**/switch ( NowNSSInfo.pNSS->NCC[count].CrossVoiceType )
									{
										case VRT_SlipRoad:
											tmpnode.nIconType	= 5026;
											break;
										case VRT_RestStop:
											tmpnode.nIconType	= 5030;
											break;
									}
									
									tmpnode.point		= NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount -1 ];
									bFindNCC = nuTRUE;
									nFindIndex = count;
									if ( bStop )
									{
										break;
									}
								}	
							}
							if ( bFindNCC )
							{
								if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) 
									&& (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
								{
									tmpnode.nDistance	= nNodeLen;
									RNEXTEND RnEx = {0};
									nuBOOL bReadSuccess = nuFALSE;
									if( NowNSSInfo.pNSS->NCC[nFindIndex].nFlag == 3)
									{
										nuTCHAR sFileName[256] = {0};
										nuTcscpy(sFileName, nuTEXT(".SPT"));
										if( g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sFileName, 256 ) )
										{
											CSPT SptFile;
											//char sTmpName[MAX_LISTNAME_NUM] = {0};
											if ( SptFile.GetNodeName( sFileName, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].nRoadNameLen, ((char *)tmpnode.wsName), MAX_LISTNAME_NUM * 2 ) )
											{
												bReadSuccess = nuTRUE;
												//nuAsctoWcs( tmpnode.wsName, MAX_LISTNAME_NUM, sTmpName, MAX_LISTNAME_NUM );
											}
										}
									}
									else if ( NowNSSInfo.pNSS->NCC[nFindIndex].nFlag == 1 )
									{
										nuTCHAR sFileName[256] = {0};
										nuTcscpy(sFileName, nuTEXT(".MWP"));
										if( g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sFileName, 256 ) )
										{
											CMWP MwpFile;
											//char sTmpName[MAX_LISTNAME_NUM] = {0};
											if ( MwpFile.GetNodeName( sFileName, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].nRoadNameLen, ((char *)tmpnode.wsName), MAX_LISTNAME_NUM * 2 ) )
											{
												bReadSuccess = nuTRUE;
												//nuAsctoWcs( tmpnode.wsName, MAX_LISTNAME_NUM, sTmpName, MAX_LISTNAME_NUM );
											}
										}
									}
									if( bReadSuccess == nuFALSE )
									{
										g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].CrossClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
										if( NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr  == NowNSSInfo.pNSS->RoadNameAddr )
										{
											AddNameStr( tmpnode.wsName );
										}

										
									}
									AddListNode(tmpnode,nuFALSE);
									nNodeLen = 0;
								}
							}
							
						}
					}
				}
			}
			else if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{

				if ( pNRBC->RTBStateFlag == 1  )
				{
					GetRTBName( pNRBC->FInfo.MapID, pNRBC->FInfo.RTBID, tmpnode.wsName );
				}
				else if ( pNRBC->RTBStateFlag == 2 )
				{
					nuWORD nMapCount = g_pInFsApi->FS_GetMapCount();
					if ( nMapCount > 1 )
					{
						PMAPNAMEINFO pTmp = (PMAPNAMEINFO)g_pInFsApi->FS_GetMpnData();
						if (   pTmp 
							&& pNRBC->FileIdx < nMapCount )
						{
							nuWcscat( tmpnode.wsName ,(pTmp + pNRBC->FileIdx )->wMapFriendName );
						}
					}

				}
				tmpnode.nDistance = nNodeLen;
				tmpnode.point     = pNRBC->FInfo.FixCoor;
				tmpnode.nIconType = 5107;
				AddListNode(tmpnode,nuFALSE);
				nNodeLen = pNRBC->RealDis;
				
			}

			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
		if( pNFTC != NULL )
		{
			//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
			if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{
				tmpnode.nDistance = nNodeLen;
				tmpnode.point     = pNFTC->FCoor;
				tmpnode.nIconType = nuWORD(1024 + AF_MidSit * 10);
				g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
				AddListNode(tmpnode,nuFALSE);
				nNodeLen = 0;
			}
			
		}
	}	
	/**/if(   g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum
	       && bEndInHighWay )
	{
		tmpnode.nDistance	= nNodeLen;
		tmpnode.nIconType	= 5109;

		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 

		PNAVIFROMTOCLASS PTmpNFTC2 = NULL ;
		while( pTmpNFTC )
		{
			PTmpNFTC2 = pTmpNFTC;
			pTmpNFTC  = pTmpNFTC->NextFT;
		}
		if( PTmpNFTC2 )
		{
			tmpnode.point.x		= PTmpNFTC2->TCoor.x;
			tmpnode.point.y		= PTmpNFTC2->TCoor.y;
		}
		nuWCHAR wc[20];
		nuMemset( wc, 0, sizeof(wc) );
		g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
		AddListNode(tmpnode,nuFALSE);
		nNodeLen = 0;
	}
	return nuTRUE;
}

nuBOOL CNaviData::ColNaviLineZ(  nuBOOL bNaviPtStart)
{
	if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	PDRAWMEMORY pDrawMemory = (PDRAWMEMORY)g_pInFsApi->FS_GetData(DATA_DRAWMEM);
	if( pDrawMemory == NULL )
	{
		return nuFALSE;
	}
	PNAVILINEDATA pNaviLine = (PNAVILINEDATA)g_pInMmApi->MM_GetDataMassAddr(pDrawMemory->nLineMemIdx);
	if( pNaviLine == NULL )
	{
		return nuFALSE;
	}
	//
	PNAVIFROMTOCLASS pNFTC;
	if( bNaviPtStart )
	{
		pNFTC = m_ptNavi.pNFTC;
	}
	else
	{
		pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	}
	nuBOOL bZoom = g_pInFsApi->pGdata->zoomScreenData.bZoomScreen;
	
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	NURORECT RectDraw = {0},RectDraw2 = {0};
	typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
	MT_MapToBmpProc pf_MapToBmp,pf_MapToBmp2;
	nuLONG nSkipLen = 0, nSkipLen2 = 0;
	nuUSHORT nNowCount = 0;
	nuBYTE nNowCount2 = 0;
	nuLONG nTotalCount = 0, nTotalCount2 = 0;
	nuLONG  dx = 0, dy = 0;
	nuINT  nCountPtMax = 0, nCountPtMax2 = 0;
	nuBOOL bNSSInZoom = nuFALSE;
	nuBOOL bZoomAddStop = nuFALSE;
	
	NUROPOINT pt1 = {0};
	//
	nuMemset( pNaviLine, 0 ,sizeof(NAVILINEDATA) );
	
	RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
	pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
	nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
	nCountPtMax    = LINE_NAVIRD_MAX_POINTS;
	if( bZoom  )
	{
		RectDraw2	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp2 = g_pInMtApi->MT_ZoomMapToBmp ;
		nSkipLen2	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
		nCountPtMax2    = LINE_ZOOMRD_MAX_POINTS;  	
	}
	//
	while( pNFTC != NULL && ( nTotalCount < nCountPtMax) )
	{
		if ( pNFTC == m_ptNavi.pNFTC && bNaviPtStart )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		
		while( pNRBC != NULL && ( nTotalCount < nCountPtMax))
		{
			if( pNRBC->NSS != NULL && ( nTotalCount < nCountPtMax))
			{
				nuDWORD i;
				if (   pNFTC == m_ptNavi.pNFTC 
					&& pNRBC == m_ptNavi.pNRBC 
					&& bNaviPtStart)
				{
					i = m_ptNavi.nSubIdx;
				}
				else
				{
					i = 0;
				}
				for(  ; i < pNRBC->NSSCount; i++ )
				{
					if( nTotalCount >= nCountPtMax )
					{
						break;
					}
					if(! nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw) )
					{
						if( nNowCount != 0 )
						{	
							if ( pNaviLine->naviLine.nLine1Count == 0 )
							{
								pNaviLine->naviLine.nLine1Count = nNowCount;
							}
							else if ( pNaviLine->naviLine.nLine2Count == 0 )
							{
								pNaviLine->naviLine.nLine2Count = nNowCount;
							}
							else if ( pNaviLine->naviLine.nLine3Count == 0 )
							{
								pNaviLine->naviLine.nLine3Count = nNowCount;
							}
							else if ( pNaviLine->naviLine.nLine4Count == 0 )
							{
								pNaviLine->naviLine.nLine4Count = nNowCount;
							}
							nNowCount = 0;
						}
						continue;
					}
					bNSSInZoom = nuFALSE;
					if( bZoom )
					{
						if ( !nuRectCoverRect(&pNRBC->NSS[i].Bound, &RectDraw2)  )
						{
							if( nNowCount2 != 0 )
							{
								if ( pNaviLine->zoomLine.nLine1Count == 0 )
								{
									pNaviLine->zoomLine.nLine1Count = nNowCount2;
								}
								else if ( pNaviLine->zoomLine.nLine2Count == 0 )
								{
									pNaviLine->zoomLine.nLine2Count = nNowCount2;
								}
								else if ( pNaviLine->zoomLine.nLine3Count == 0 )
								{
									pNaviLine->zoomLine.nLine3Count = nNowCount2;
								}
								else if ( pNaviLine->zoomLine.nLine4Count == 0 )
								{
									pNaviLine->zoomLine.nLine4Count = nNowCount2;
								}
								nNowCount2 = 0;
								
							}
						}
						else
						{
							if ( bZoomAddStop )
							{
								bNSSInZoom = nuFALSE;
							}
							else
							{	
								bNSSInZoom = nuTRUE;
							}
							
						}
					}
					
					nuLONG j;
					if (   pNFTC == m_ptNavi.pNFTC 
						&& pNRBC == m_ptNavi.pNRBC
						&& i     == m_ptNavi.nSubIdx
						&& bNaviPtStart)
					{
						j = m_ptNavi.nPtIdx;  
					}
					else
					{
						j = 0;
					}
					for( ; j < pNRBC->NSS[i].ArcPtCount; j++)
					{ 
						if (   m_ptNavi.pNFTC	== pNFTC 
							&& m_ptNavi.pNRBC	== pNRBC
							&& m_ptNavi.nSubIdx	== i
							&& m_ptNavi.nPtIdx  == j )
						{
							pf_MapToBmp(m_ptNavi.ptMapped.x , m_ptNavi.ptMapped.y, &( pNaviLine->naviLine.ptNaviLine[nTotalCount].x ) , &( pNaviLine->naviLine.ptNaviLine[nTotalCount].y ));
							if ( bNSSInZoom )
							{
								pf_MapToBmp2(m_ptNavi.ptMapped.x , m_ptNavi.ptMapped.y, &( pNaviLine->zoomLine.ptZoomLine[nTotalCount2].x ) , &(pNaviLine->zoomLine.ptZoomLine[nTotalCount2].y ));	
								nNowCount2 ++;
								nTotalCount2++;
								if( nTotalCount2 >= LINE_ZOOMRD_MAX_POINTS )
								{
									bZoomAddStop = nuTRUE;
									bNSSInZoom = nuFALSE;
								}
							}
							
						}
						else
						{
							if( j != 0 )
							{
								dx = pNRBC->NSS[i].ArcPt[j].x - pt1.x;
								dy = pNRBC->NSS[i].ArcPt[j].y - pt1.y;
								if (   bNSSInZoom 
									&& (NURO_ABS(dx) >= nSkipLen2 || NURO_ABS(dy) >= nSkipLen2 )
									)
								{
									pf_MapToBmp2(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &( pNaviLine->zoomLine.ptZoomLine[nTotalCount2].x ) , &(pNaviLine->zoomLine.ptZoomLine[nTotalCount2].y ));	
									nNowCount2 ++;
									nTotalCount2++;
									if( nTotalCount2 >= LINE_ZOOMRD_MAX_POINTS )
									{
										bZoomAddStop = nuTRUE;
										bNSSInZoom = nuFALSE;
									}
									
								}
								if( j != pNRBC->NSS[i].ArcPtCount - 1 )
								{
									if( NURO_ABS(dx) < nSkipLen && NURO_ABS(dy) < nSkipLen )
									{
										continue;
									}
								}	
							}
							if (  bNSSInZoom && j == 0 )
							{
								pf_MapToBmp2(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &( pNaviLine->zoomLine.ptZoomLine[nTotalCount2].x ) , &(pNaviLine->zoomLine.ptZoomLine[nTotalCount2].y ));	
								nNowCount2 ++;
								nTotalCount2++;
								if( nTotalCount2 >= LINE_ZOOMRD_MAX_POINTS )
								{
									bZoomAddStop = nuTRUE;
									bNSSInZoom = nuFALSE;
								}
								
							}
							pf_MapToBmp(pNRBC->NSS[i].ArcPt[j].x, pNRBC->NSS[i].ArcPt[j].y, &( pNaviLine->naviLine.ptNaviLine[nTotalCount].x ) , &(pNaviLine->naviLine.ptNaviLine[nTotalCount].y ));	
						}
						
						nNowCount ++;
						nTotalCount++;
						pt1 = pNRBC->NSS[i].ArcPt[j];
						if( nTotalCount == nCountPtMax )
						{
							break;
						}
					}
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	
	if( nNowCount != 0 )
	{
		if( nNowCount != 0 )
		{	
			if ( pNaviLine->naviLine.nLine1Count == 0 )
			{
				pNaviLine->naviLine.nLine1Count = nNowCount;
			}
			else if ( pNaviLine->naviLine.nLine2Count == 0 )
			{
				pNaviLine->naviLine.nLine2Count = nNowCount;
			}
			else if ( pNaviLine->naviLine.nLine3Count == 0 )
			{
				pNaviLine->naviLine.nLine3Count = nNowCount;
			}
			else if ( pNaviLine->naviLine.nLine4Count == 0 )
			{
				pNaviLine->naviLine.nLine4Count = nNowCount;
			}
			nNowCount = 0;
		}
	}
	if( bZoom )
	{
		if( nNowCount2 != 0 )
		{
			if ( pNaviLine->zoomLine.nLine1Count == 0 )
			{
				pNaviLine->zoomLine.nLine1Count = nNowCount2;
			}
			else if ( pNaviLine->zoomLine.nLine2Count == 0 )
			{
				pNaviLine->zoomLine.nLine2Count = nNowCount2;
			}
			else if ( pNaviLine->zoomLine.nLine3Count == 0 )
			{
				pNaviLine->zoomLine.nLine3Count = nNowCount2;
			}
			else if ( pNaviLine->zoomLine.nLine4Count == 0 )
			{
				pNaviLine->zoomLine.nLine4Count = nNowCount2;
			}
			nNowCount2 = 0;
			
		}
		
	}
	return nuTRUE;
}


nuBOOL CNaviData::GetNaviArrowPoint( NAVINSS *pNaviNss, CROSSNODE *pCrossNode)
{
	if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	NAVINSS DrawNaviNSS;
	NAVINSS NextNaviNSS = {0};
	nuMemset( &DrawNaviNSS, 0 ,sizeof(DrawNaviNSS));
	DrawNaviNSS = *pNaviNss;
	if( DrawNaviNSS.pNFTC == NULL || DrawNaviNSS.pNRBC == NULL || DrawNaviNSS.pNSS == NULL )
	{
		return nuFALSE;
	}
	NUROPOINT* ptDraw = pCrossNode->ptCrossList ;//g_pInFsApi->pGdata->drawInfoNavi.nextCross.ptCrossList;
	nuBYTE & nDrawPtNum = pCrossNode->nCrossPtCount ;//g_pInFsApi->pGdata->drawInfoNavi.nextCross.nCrossPtCount;
	nDrawPtNum = 0;
	//	NUROPOINT ptPre,ptNext;
	nuLONG dLen = 0;
	nuLONG nDrawArrowLen = LENGETPT;//ZOOMLEN * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / SCALEBASE; 
	GetPointFromNSS2( DrawNaviNSS.pNSS, nuFALSE, ptDraw, nDrawPtNum, nDrawArrowLen );
	
	//ç®­å€Ž?é¢?é¿åºŠä?å€ç?è¯è?è¡æ©å±?
	while ( nDrawArrowLen > 0 )
	{
		if ( DrawNaviNSS.nIndex > 0 )
		{
			NextNaviNSS.nIndex = DrawNaviNSS.nIndex - 1;
			NextNaviNSS.pNFTC  = DrawNaviNSS.pNFTC;
			NextNaviNSS.pNRBC  = DrawNaviNSS.pNRBC;
			NextNaviNSS.pNSS   = &(DrawNaviNSS.pNRBC->NSS[NextNaviNSS.nIndex]);
			if ( NextNaviNSS.pNSS == NULL )
			{
				return nuTRUE;
			}
		}
		else 
		{
			if ( !GetPreNSS(DrawNaviNSS, &NextNaviNSS ))
			{
				break;
			}
		}
		if( !GetPointFromNSS2( NextNaviNSS.pNSS, nuFALSE, ptDraw, nDrawPtNum, nDrawArrowLen ) )
		{
			break;
		}
		if( nDrawPtNum ==  CROSS_ARROW_POINT_NUM )
		{
			break;
		}
		nuMemcpy( &DrawNaviNSS, &NextNaviNSS, sizeof(DrawNaviNSS));
	}
	nuINT nTotal = 0;
	nuINT nAddLen = 0;
	nuINT nIndex = 0;
	for( nuINT nPtNum = 0; nPtNum < nDrawPtNum - 1; nPtNum++ )
	{
		nuLONG dxx = ptDraw[nPtNum].x - ptDraw[nPtNum + 1].x;
		nuLONG dyy = ptDraw[nPtNum].y - ptDraw[nPtNum + 1].y;
		dxx = NURO_MAX(NURO_ABS(dxx), NURO_ABS(dyy));
		if( dxx > 10 )
		{
			nIndex = nPtNum;
			break;
		}
		else
		{
			if( dxx > nAddLen )
			{
				nAddLen = dxx;
				nIndex = nPtNum;
			}
			nTotal += dxx;
			if ( nTotal > 30 )
			{
				break;
			}
			
		}
		/*
		nAddLen += nulSqrt( ptDraw[nPtNum].x - ptDraw[nPtNum + 1].x,  ptDraw[nPtNum].y - ptDraw[nPtNum + 1].y );
		if( nAddLen >= nTotal )
		{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleIn = (short)nulAtan2( ptDraw[0].y -  ptDraw[nPtNum + 1].y, ptDraw[0].x -  ptDraw[nPtNum + 1].x);		
		break;
		}
		*/
	}
	pCrossNode->nAngleInDx = (short)(ptDraw[nIndex].x -  ptDraw[nIndex + 1].x);
	pCrossNode->nAngleInDy = (short)(ptDraw[nIndex].y -  ptDraw[nIndex + 1].y);
	pCrossNode->nIdxCrossInList = nDrawPtNum - 1;
	//nuLONG xx = nulAtan2( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleInDy, g_pInFsApi->pGdata->drawInfoNavi.nextCross.nAngleInDx );
	//ä¿®æ­£ptDraw?ç¹?ä?çœ?
	for (nuBYTE i = 0; i < nDrawPtNum/2; i++ )
	{
		NUROPOINT ptTmp = ptDraw[i];
		ptDraw[i]	= ptDraw[nDrawPtNum - 1 - i];
		ptDraw[nDrawPtNum - 1 - i] = ptTmp;
	}
	
	//?æ¥äžæ¥?ç¹ 
	/** */
	if ( GetNextNSS( *pNaviNss,&NextNaviNSS) )
	{
		nuINT nIndex = nDrawPtNum - 1;
		PNAVISUBCLASS pNextNSS = NextNaviNSS.pNSS; 
		if( !pNextNSS )
		{
			return nuFALSE;
		}
		nDrawArrowLen = LENGETPT;
		GetPointFromNSS2(pNextNSS, nuTRUE, ptDraw, nDrawPtNum, nDrawArrowLen);
		//ç®­å€Ž?é¢?é¿åºŠä?å€ç?è¯è?è¡æ©å±?
		while ( nDrawArrowLen > 0)
		{
			nuMemcpy( &DrawNaviNSS, &NextNaviNSS, sizeof(NextNaviNSS) );
			GetNextNSS( DrawNaviNSS,&NextNaviNSS);
			pNextNSS = NextNaviNSS.pNSS; 
			if( !pNextNSS )
			{
				return nuFALSE;
			}
			if ( !GetPointFromNSS2(pNextNSS, nuTRUE, ptDraw, nDrawPtNum, nDrawArrowLen) )
			{
				break;
			}
			if( nDrawPtNum ==  CROSS_ARROW_POINT_NUM )
			{
				break;
			}
		}		
	}
	
	return nuTRUE;
}

nuBOOL CNaviData::GetPointFromNSS(PNAVISUBCLASS pNSS, nuBOOL bFrist, PNUROPOINT pPoint, nuBYTE &nNowPtNum, nuLONG &nRemainLen)
{
	if ( pNSS == NULL || pPoint == NULL || nRemainLen == 0 || nNowPtNum >= CROSS_ARROW_POINT_NUM)
	{
		return nuFALSE;
	}
	nuBYTE nTempIndex =  nNowPtNum;
	if( pNSS->RoadLength <= nRemainLen )
	{
		nRemainLen -= pNSS->RoadLength;
		if ( bFrist )
		{
			for ( nuINT nIndex = 0; nIndex <= pNSS->ArcPtCount - 1; nIndex++ )
			{
				nuMemcpy( &(pPoint[nNowPtNum]), &pNSS->ArcPt[nIndex],sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
			}
		}
		else
		{
			for ( nuINT nIndex = pNSS->ArcPtCount - 1; nIndex >= 0; nIndex-- )
			{
				nuMemcpy( &(pPoint[nNowPtNum]), &pNSS->ArcPt[nIndex],sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
			}
			
		}
		
		return nuTRUE;
	}
	NUROPOINT ptPre = {0},ptNext = {0};
	nuLONG i = 0;
	if ( bFrist )
	{
		nuMemcpy( &(pPoint[nNowPtNum]), &pNSS->ArcPt[0],sizeof(NUROPOINT) );
		nNowPtNum++;
		if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
		{
			return  nuTRUE;
		}
		for (  i = 0; i < pNSS->ArcPtCount - 1; i++ )
		{
			ptPre = pNSS->ArcPt[i];
			if( (i + 1) <= pNSS->ArcPtCount - 1)
			{
				ptNext = pNSS->ArcPt[i+1];
			}
			else
			{
				break;
			}
			nuLONG dTmpLen = (nuLONG)(nuSqrt((ptNext.x - ptPre.x) *(ptNext.x - ptPre.x) + (ptNext.y - ptPre.y) *(ptNext.y - ptPre.y)));
			if(  dTmpLen <= nRemainLen )
			{
				nRemainLen -= dTmpLen;
				nuMemcpy( &(pPoint[nNowPtNum]), &ptNext,sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
				ptPre = ptNext;
				if( i + 2 <= pNSS->ArcPtCount - 1 )
				{
					ptNext = pNSS->ArcPt[i+2];
				}
				else
				{
					break;
				}	
			}
			else
			{
				NUROPOINT pt = {0};
				pt.x = nRemainLen * (ptNext.x - ptPre.x) /dTmpLen + ptPre.x; 
				pt.y = nRemainLen * (ptNext.y - ptPre.y)/dTmpLen + ptPre.y ;
				nuMemcpy( &(pPoint[nNowPtNum]), &pt,sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
				nRemainLen = 0;
				break;
			}
		}
	}
	else
	{
		nuMemcpy( &(pPoint[nNowPtNum]), &pNSS->ArcPt[pNSS->ArcPtCount-1],sizeof(NUROPOINT) );
		nNowPtNum++;
		if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
		{
			return  nuTRUE;
		}
		for (  i = pNSS->ArcPtCount - 1; i >= 0; i-- )
		{
			ptNext	= pNSS->ArcPt[i];
			if( i > 0)
			{
				ptPre = pNSS->ArcPt[i-1];
			}
			else
			{
				break;
			}
			nuLONG dTmpLen = (nuLONG)(nuSqrt((ptNext.x - ptPre.x) *(ptNext.x - ptPre.x) + (ptNext.y - ptPre.y) *(ptNext.y - ptPre.y)));
			if( dTmpLen <= nRemainLen )
			{
				nRemainLen -= dTmpLen;
				nuMemcpy( &(pPoint[nNowPtNum]), &ptPre,sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
				ptNext = ptPre;
				if( i >= 2)
				{
					ptPre = pNSS->ArcPt[i-2];
				}
				else
				{
					break;
				}	
			}
			else
			{
				NUROPOINT pt = {0};
				pt.x = nRemainLen * (ptPre.x - ptNext.x)/dTmpLen + ptNext.x; 
				pt.y = nRemainLen * (ptPre.y - ptNext.y)/dTmpLen + ptNext.y ;
				nuMemcpy( &(pPoint[nNowPtNum]), &pt,sizeof(NUROPOINT) );
				nNowPtNum++;
				if ( CROSS_ARROW_POINT_NUM <= nNowPtNum )
				{
					return  nuTRUE;
				}
				nRemainLen = 0;
				break;
			}
		}
	}
    return nuTRUE;
}

nuBOOL CNaviData::GetPreNSS(NAVINSS NowNSSInfo, PNAVINSS pPreNSS)
{
	//return nuFALSE;
	if( pPreNSS == NULL )
	{
		return nuFALSE;
	}
	nuLONG i = NowNSSInfo.nIndex;

	PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
		
	PNAVIRTBLOCKCLASS pNRBC = pNFTC->NRBC ;
	PNAVIFROMTOCLASS pTmpNFTC = NULL;
	PNAVIRTBLOCKCLASS pTmpNRBC = NULL ;
	PNAVISUBCLASS  pTmpNSS = NULL;
	nuDWORD  nTmpIndex = 0;
	nuMemset( pPreNSS, 0 ,sizeof(NAVINSS) );
	while ( pNFTC != NULL )
	{
		pNRBC = pNFTC->NRBC;	
		while ( pNRBC != NULL) 
		{
			if( pNRBC->NSS != NULL )
			{
				for( nuDWORD i = 0; i < pNRBC->NSSCount; i ++ )
				{
					if (   NowNSSInfo.pNFTC   == pNFTC 
						&& NowNSSInfo.pNRBC   == pNRBC
						&& NowNSSInfo.nIndex  == i )
					{
						pPreNSS->pNFTC  = pTmpNFTC;
						pPreNSS->pNRBC  = pTmpNRBC;
						pPreNSS->pNSS   = pTmpNSS;
						pPreNSS->nIndex = nTmpIndex;	
						return nuTRUE;
					}
                    pTmpNFTC = pNFTC;
					pTmpNRBC = pNRBC;
					pTmpNSS  = &pNRBC->NSS[pNRBC->NSSCount - 1];
					nTmpIndex = i;
				}
				
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	return nuFALSE;
	
}

nuBOOL CNaviData::GetPointFromNSS2(PNAVISUBCLASS pNSS, nuBOOL bFrist, PNUROPOINT pPoint, nuBYTE &nNowPtNum, nuLONG &nRemainLen)
{
	if ( pNSS == NULL || pPoint == NULL )
	{
		return nuFALSE;
	}
	nuBYTE nTmpLen = nNowPtNum;
	GetPointFromNSS( pNSS, bFrist, pPoint ,nNowPtNum,nRemainLen);
	
	
	return nuTRUE;
}

nuBOOL CNaviData::SelPt(PCROSSPOINT pCrossArrayIn, PCROSSPOINT pCrossArrayOut, nuLONG nLenDraw,nuBOOL bChangeCross)
{
	if ( pCrossArrayIn == NULL || pCrossArrayOut == NULL  )
	{
		return nuFALSE;
	}
	nuBYTE nIndex = 0;
	nuBYTE &nNowSaveIndex = pCrossArrayOut->nPtNum;//åœå?ä¿å??°ç??çŽ¢åŒ?
	nNowSaveIndex = 0;
	//?Ÿåºäž­éŽ?¹ç?Index
	nuBYTE nCrossIndex = -1; //äº€å?è·¯å£?çŽ¢åŒ?
	if ( bChangeCross )
	{
		if ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance <= 0 )
		{
			m_nCrossIndexInSouce = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIdxCrossInList;
			nCrossIndex = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nIdxCrossInList;
		}
		else
		{
			m_nCrossIndexInSouce = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nIdxCrossInList;
			nCrossIndex = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nIdxCrossInList;
		}
	}
	else
	{
		nCrossIndex = pCrossArrayIn->nCenterIndex;
	}
	
	
	//äž­éŽ?¹ä?å­åšPOINT?°ç???
	if ( nCrossIndex == -1 )
	{
		return nuFALSE;
	}
    //?å?è·¯å£?é¢?ç¹
	nuLONG nTmpLenTotal = 0;
	
    nuLONG nTmpLen1 = 0;
	nuLONG nTmpLen2 = 0 ;
	PNUROPOINT ptSource = pCrossArrayIn->ptArray;
	PNUROPOINT ptSave = pCrossArrayOut->ptArray;
	for ( nIndex = nCrossIndex ; nIndex > 0; nIndex-- )
	{
		nTmpLen1 += (nuLONG)nuSqrt(    (ptSource[nIndex].x - ptSource[nIndex - 1].x ) * (ptSource[nIndex].x - ptSource[nIndex - 1].x ) 
			+  (ptSource[nIndex].y - ptSource[nIndex - 1].y ) * (ptSource[nIndex].y - ptSource[nIndex - 1].y) );
		if ( nTmpLen1 >= nLenDraw && nTmpLen2 <= nLenDraw )
		{
			nuBYTE i = nIndex - 1;
			if ( nTmpLen1 > nLenDraw )
			{
				i = nIndex ;
				NUROPOINT pt = {0};
				pt.x = ( nLenDraw - nTmpLen2 )*(ptSource[nIndex - 1].x - ptSource[nIndex].x) /( nTmpLen1 - nTmpLen2 ) + ptSource[nIndex].x; 
				pt.y = ( nLenDraw - nTmpLen2 )*(ptSource[nIndex - 1].y - ptSource[nIndex].y) / ( nTmpLen1 - nTmpLen2 ) + ptSource[nIndex].y ;
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&pt,
					sizeof(NUROPOINT) );
			}
			for( ; i <= nCrossIndex; i++ ) 
			{
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&ptSource[i],
					sizeof(NUROPOINT) );
			}
			pCrossArrayOut->nCenterIndex = nNowSaveIndex - 1;	
			break;
			
		}
        if ( nTmpLen1 < nLenDraw && nIndex == 1)
        {
			nuINT j = 0;
			for ( j = 0; j <= nCrossIndex; j++ )
			{
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&ptSource[j],
					sizeof(NUROPOINT) );
			}
        }
		nTmpLen2 = nTmpLen1;
		pCrossArrayOut->nCenterIndex = nNowSaveIndex - 1;	
		
	}
    //«e¬q
#ifndef VALUE_EMGRT
	if (m_bCombinDrawArrow)
	{
		if ( m_nCombinLen > 0  && m_nCombinLen < 20)
		{
			nLenDraw += 10;
		}
		else if ( m_nCombinLen >= 20 && m_nCombinLen < 30) 
		{
			nLenDraw += 15;
		}
		else if ( m_nCombinLen >= 30 && m_nCombinLen < 40) 
		{
			nLenDraw += 20;
		}
		else if ( m_nCombinLen >= 40 && m_nCombinLen < 45) 
		{
			nLenDraw += 25;
		}
	}
#endif
	nTmpLenTotal = 0;
	nTmpLen1 =  0;
	nTmpLen2 =  0 ;
	for ( nIndex = nCrossIndex + 1; nIndex < pCrossArrayIn->nPtNum; nIndex++ )
	{
		nTmpLen1 += (nuLONG)nuSqrt(  (ptSource[nIndex].x - ptSource[nIndex - 1].x ) * (ptSource[nIndex].x - ptSource[nIndex - 1].x ) 
			+  (ptSource[nIndex].y - ptSource[nIndex - 1].y ) * (ptSource[nIndex].y - ptSource[nIndex - 1].y) );
		
		if ( nTmpLen1 >= nLenDraw && nTmpLen2 <= nLenDraw )
		{
			nuBYTE i = 0;
			nuBYTE nEnd = nIndex;
			if ( nTmpLen1 > nLenDraw )
			{
				nEnd = nIndex - 1;
			}
			for( i = nCrossIndex + 1; i <= nEnd; i++ ) 
			{
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&ptSource[i],
					sizeof(NUROPOINT) );
			}
			if ( nTmpLen1 > nLenDraw )
			{
				NUROPOINT pt = {0};
				pt.x = ( nLenDraw - nTmpLen2 )*(ptSource[nIndex].x - ptSource[nIndex - 1].x) / ( nTmpLen1 - nTmpLen2 ) + ptSource[nIndex - 1].x; 
				pt.y = ( nLenDraw - nTmpLen2 )*(ptSource[nIndex].y - ptSource[nIndex - 1].y) / ( nTmpLen1 - nTmpLen2 ) + ptSource[nIndex - 1].y ;
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&pt,
					sizeof(NUROPOINT) );
			}
			break;
		}
		if ( nTmpLen1 < nLenDraw && nIndex == pCrossArrayIn->nPtNum - 1 )
		{
			nuINT j = 0;
			for ( j = nCrossIndex + 1; j < pCrossArrayIn->nPtNum; j++ )
			{
				nuMemcpy( &ptSave[nNowSaveIndex++], 
					&ptSource[j],
					sizeof(NUROPOINT) );
			}
			
		}
		
		nTmpLen2 = nTmpLen1;
	}
	
	return nuTRUE;
}

nuLONG CNaviData::GetRoutingListCodeNew(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS)
{
	nuLONG nNowVoiceType = 0;
	nuLONG nNextVoiceType = 0;
	if ( pNowNSS->RoadVoiceType == VRT_Tollgate &&  pNextNSS->RoadVoiceType == VRT_Tollgate)
	{
		return AF_LoseHere;
	}
	if ( pNowNSS->RoadClass == 5  )
	{
		pNowNSS->RoadVoiceType = VRT_SlipRoad;
	}
	if ( pNowNSS->RoadClass <= 3  && VRT_Fastway == pNowNSS->RoadVoiceType )
	{
		pNowNSS->RoadVoiceType = VRT_Freeway;
	}
	if ( pNextNSS->RoadClass == 5 )
	{
		pNextNSS->RoadVoiceType = VRT_SlipRoad;
	}
    if ( pNextNSS->RoadClass <= 3 && VRT_Fastway == pNextNSS->RoadVoiceType)
	{
		pNextNSS->RoadVoiceType = VRT_Freeway;
	}

	nNowVoiceType	 = pNowNSS->RoadVoiceType;
	nNextVoiceType	 = pNextNSS->RoadVoiceType;
	
	switch(nNowVoiceType)
	{
		//case VRT_Tunnel:
	/*	*/
	case VRT_Underpass:
	case VRT_UnOverpass:
//	case VRT_Tollgate:
	case VRT_RestStop:
		return AF_LoseHere;
#ifndef ZENRIN
	case VRT_Tollgate:
		pNextNSS->RoadNameAddr=pNowNSS->RoadNameAddr;
		return AF_In_Tollgate;
#endif
	}
	switch(nNextVoiceType)
	{
	case VRT_CityTownBoundary:
		return AF_CityTownBoundary;
	case VRT_MapBoundary:	
		return AF_MapBoundary;
	case VRT_MidSite:		
		return AF_MidSit;
	case VRT_EndSite:		
		return AF_EndSite;
	}
	
	if (   nNowVoiceType != nNextVoiceType
		&& (  (nNowVoiceType != VRT_Defway || nNextVoiceType != VRT_Defway  )
		     || pNowNSS->RoadClass != pNextNSS->RoadClass )

		&& 	nNowVoiceType != VRT_ROADCROSS 
		&& nNextVoiceType != VRT_ROADCROSS )
	{
		/**/
		if(nNowVoiceType==VRT_TrafficCircle)		
		{
			return AF_Out_TrafficCircle;
		}
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_In_TrafficCircle;
		}
		if (  nNextVoiceType  == VRT_FastToSlow )
		{
			return AF_IN_SLOWROAD;
		}
		if (  nNextVoiceType  == VRT_SlowToFast )
		{//shyanx
			return AF_IN_FASTROAD;
		}
#ifdef ZENRIN
		if(nNextVoiceType==VRT_Tollgate)			
		{
			return AF_In_Tollgate;
		}
#endif
		if(nNextVoiceType==VRT_RestStop)	
		{
			return AF_In_RestStop;
		}
		if(nNextVoiceType==VRT_Tunnel)	
		{
			return AF_In_Tunnel;
		}
		if(nNowVoiceType==VRT_Freeway)
		{
			return AF_Out_Freeway;
		}
		if(nNextVoiceType==VRT_Freeway)
		{
			return AF_In_Freeway;
		}
#if (!defined VALUE_EMGRT)
		if(  nNowVoiceType == VRT_Fastway )
		{
			if ( nNextVoiceType == VRT_SlowWay )
			{
				return AF_IN_SLOWROAD;
			}
			else
			{
				return AF_Out_Fastway;
			}
		}
		if(   nNextVoiceType == VRT_Fastway 
		   && nNowVoiceType  == VRT_SlowWay	)
		{
			return AF_In_Fastway;
		}
#endif
	
		
		if(nNowVoiceType == VRT_Overpass && pNowNSS->RoadClass != pNextNSS->RoadClass)
		{
			return AF_Out_Overpass;
		}
		if(nNextVoiceType == VRT_Overpass && pNowNSS->RoadClass != pNextNSS->RoadClass)	
		{
			return AF_In_Overpass;
		}
		if(nNextVoiceType==VRT_SlipRoad)
		{
			if ( pNowNSS->RoadClass <= 3)
			{
				return AF_Out_Freeway;



			}
			else
			{
				return AF_In_SlipRoad;
			}
		}
		
		
		if(nNextVoiceType==VRT_Underpass)
		{
			return AF_In_Underpass;
		}
		if(nNextVoiceType==VRT_UnOverpass)
		{
			return AF_In_UnOverpass;
		}
		/*if(nNowVoiceType==VRT_TrafficCircle)		
		{
			return AF_Out_TrafficCircle;
		}
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_In_TrafficCircle;
		}*/
		
		if (nNowVoiceType == VRT_Tunnel )
		{
			return AF_Out_Tunnel;
		}
	}
	else
	{
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_LoseHere;//In TrafficCircle, ignore the road crossing
		}
	}
	/*if( pNowNSS->CrossCount > 1 )
	{
	for ( nuWORD i = 0; i < pNowNSS->CrossCount; i++)
	{
	if (  ( pNowNSS->NCC[i].CrossBlockIdx != pNowNSS->BlockIdx || pNowNSS->NCC[i].CrossRoadIdx  != pNowNSS->RoadIdx )
				&&( pNowNSS->NCC[i].CrossBlockIdx != pNextNSS->BlockIdx || pNowNSS->NCC[i].CrossRoadIdx  != pNextNSS->RoadIdx )
				)
				{
				if (   pNowNSS->TurnAngle >= -10
				&& pNowNSS->TurnAngle <= 10  )
				{
				pNowNSS->TurnFlag = T_Afore;
				break;
				}
				}
				}
				
	}*/
	/*for ( nuWORD i = 0; i < pNowNSS->CrossCount; i++)
	{
				if( pNowNSS->NCC[i].CrossVoiceType == VRT_SlipRoad) 
				{
				return AF_Has_SlipRoad;
				}
				else if( pNowNSS->NCC[i].CrossVoiceType == VRT_RestStop )
				{
				return AF_Has_RestStop;
				}
				else if( pNowNSS->NCC[i].CrossVoiceType == VRT_Tollgate )
				{
				
				  }
				  else if( pNowNSS->NCC[i].CrossVoiceType == VRT_ScanSpeed)
				  {
				  
					}
	}*/
	if( pNowNSS->TurnFlag == T_No && pNowNSS->RoadNameAddr != pNextNSS->RoadNameAddr)
	{
		return AF_LoseHere;//AF_ChangeRoadName;
	}
	return AF_LoseHere;
}

nuBOOL CNaviData::bJudgeHighWayCross(PNAVINSS pNowNSS, PNAVINSS pNextNSS)
{
	if (   pNowNSS == NULL
		|| pNowNSS == NULL )
	{
		return nuFALSE;
	}
	if(pNowNSS->pNSS->CrossCount == 2 &&   pNowNSS->pNSS->RoadClass == 4 && pNextNSS->pNSS->RoadClass == 4 && pNowNSS->pNSS->RoadNameAddr != pNextNSS->pNSS->RoadNameAddr)
	{
		return nuTRUE;
	}
	nuBOOL bRet = nuTRUE;
	nuBOOL bHighWay = nuFALSE;
	//	if (   pNowNSS->pNSS->RoadClass <= 3 
	//		&& pNextNSS->pNSS->RoadClass <= 3 )
	{
		nuDWORD nNCCNum = pNowNSS->pNSS->CrossCount;
		nuDWORD i = 0;
		bRet = nuFALSE;
		for ( i = 0; i < nNCCNum; i++ )
		{
			//	if ( pNowNSS->pNSS->NCC[i].CrossClass <= 3 ) 
			{
				if (   pNowNSS->pNSS->NCC[i].CrossBlockIdx != pNextNSS->pNSS->BlockIdx 
					|| pNowNSS->pNSS->NCC[i].CrossRoadIdx  != pNextNSS->pNSS->RoadIdx
					|| pNowNSS->pNSS->NCC[i].CrossRoadNameAddr != pNextNSS->pNSS->RoadNameAddr )
				{
					if (   pNowNSS->pNSS->RoadClass <= 3 
						&& pNextNSS->pNSS->RoadClass <= 3
						&& pNowNSS->pNSS->NCC[i].CrossClass >= 5 )
					{
						bRet = nuFALSE;
						bHighWay = nuTRUE;
						break;
					}
					else if (  pNowNSS->pNSS->NCC[i].CrossBlockIdx != pNowNSS->pNSS->BlockIdx 
						|| pNowNSS->pNSS->NCC[i].CrossRoadIdx  != pNowNSS->pNSS->RoadIdx
						|| pNowNSS->pNSS->NCC[i].CrossRoadNameAddr != pNowNSS->pNSS->RoadNameAddr )
					{
						bRet = nuTRUE;
						break;
					}
				}
				
			}
		}
	}
	if ( !bHighWay )
	{
		bRet = nuTRUE;
	}
	return bRet;
}

nuBOOL CNaviData::bColCombinCrossData(NAVINSS *pNowNSSInfo, NAVINSS *pNextNSSInfo, nuLONG &nLenToNextCross,nuBOOL bReCalNextCross)
{
	if ( pNextNSSInfo->pNSS->RoadVoiceType == VRT_TrafficCircle )
	{
		return nuFALSE;
	}
#define  MAX_TURNNUM	7
	typedef struct tagTURNRECORD
	{
		nuBYTE nTurn[MAX_TURNNUM];
		nuBYTE nLen[MAX_TURNNUM];
		NAVINSS NowNSS[MAX_TURNNUM];
		NAVINSS NextNSS[MAX_TURNNUM];
		nuBYTE nTurnNum;
	}TURNRECORD;
	NAVINSS TmpNowNSS,TmpNextNSS = {0};
    nuMemcpy( &TmpNowNSS, pNextNSSInfo, sizeof(NAVINSS) );
	nuBYTE nTurnFlag = GetTurnFlag( pNowNSSInfo->pNSS, pNextNSSInfo->pNSS );

	//TEst
/*	nuWCHAR wsName[256] = {0};
	RNEXTEND RnEx;
	if ( !g_pInFsApi->FS_GetRoadName(   26,
										pNowNSSInfo->pNSS->RoadNameAddr , 
										pNowNSSInfo->pNSS->RoadClass, 
										wsName, 
										MAX_LISTNAME_NUM,
										&RnEx) );
*/
    if ( !GetNextNSS( TmpNowNSS, &TmpNextNSS ) )
    {
		return nuFALSE;
    }
	TURNRECORD turnArray = {0};
	nuMemset( &turnArray, 0, sizeof(TURNRECORD) );
	
	nuLONG nDisAdd = 0;
	nuBOOL bCross = nuFALSE;
#ifdef ZENRIN
	nuBOOL bCrossZ = nuFALSE;
#endif
	if( TmpNowNSS.pNSS->RoadVoiceType == VRT_ROADCROSS )
	{
		bCross = nuTRUE;
	}
	while(nuTRUE)
	{
		nDisAdd += TmpNowNSS.pNSS->RoadLength;
		if (   ( bCross && TmpNowNSS.pNSS->RoadVoiceType != VRT_ROADCROSS)
			||  (nuFALSE == bCross && nDisAdd > LEN_COMBINE ) )
		{
			break;
		}
		//GetRoutingListCode( TmpNowNSS.pNSS, TmpNextNSS.pNSS );
        nuBYTE nTmpTurn = GetTurnFlag( TmpNowNSS.pNSS, TmpNextNSS.pNSS );
		if ( nTmpTurn != T_No )
		{
			turnArray.nTurn[turnArray.nTurnNum]   = nTmpTurn;
			turnArray.nLen[turnArray.nTurnNum]    = (nuBYTE)nDisAdd;
            turnArray.NowNSS[turnArray.nTurnNum]  = TmpNowNSS;
			turnArray.NextNSS[turnArray.nTurnNum] = TmpNextNSS;
			nDisAdd = 0;
			turnArray.nTurnNum++;
			if ( turnArray.nTurnNum >= MAX_TURNNUM )
			{
				break;
			}
		}
		
		nuMemcpy( &TmpNowNSS, &TmpNextNSS, sizeof(NAVINSS) );
		if ( TmpNowNSS.pNSS == NULL )
		{
			break;
		}
		if ( !GetNextNSS(TmpNowNSS, &TmpNextNSS) )
		{
			break;
		}
	}
	if ( turnArray.nTurnNum == 0 )
	{
#ifdef ZENRIN
		if ( nTurnFlag == T_LeftSide )
		{
			if ( pNextNSSInfo->pNSS->TurnFlag == T_No || pNextNSSInfo->pNSS->TurnFlag == T_RightSide)
			{
				if ((pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) > 50 &&
					(pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) < 135 &&
					 pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS )
				{
					pNowNSSInfo->pNSS->TurnFlag  = T_Left;
					bCrossZ = nuTRUE;
					if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
					{
						return nuFALSE;
					}
				}
			}
		}
		if(!bCrossZ)
		{
			return nuFALSE;
		}
#else
		return nuFALSE;
#endif
	}
    else
    {
		//?®å??ªæ¯?ä?å­åœ¢?å·Š?³å?èœ¬ç?ç±»å?
		if ( nTurnFlag == T_Afore ) 
		{
			if ( turnArray.nTurn[0] == T_Afore && turnArray.nTurnNum == 1 ) //Ö±??
			{
				
			}
			else if (  turnArray.nTurn[0] == T_Left 
				&& turnArray.nTurn[1] == T_Afore
				&& turnArray.nTurnNum == 2 
				&& pNextNSSInfo->pNSS->Real3DPic == 0 ) //å·Šèœ¬
			{
			/*	nuMemcpy( pNowNSSInfo, &(turnArray.NowNSS[0]), sizeof(NAVINSS) );
				nuMemcpy( pNowNSSInfo, &(turnArray.NextNSS[0]), sizeof(NAVINSS) );
				nLenToNextCross += turnArray.nLen[0];

				pNextNSSInfo->pNSS->TurnFlag = T_No;
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[1].pNSS->RoadVoiceType;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[1].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[1].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[1].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}*/
			}
			else if (  turnArray.nTurn[0] == T_Left 
				&& turnArray.nTurn[1] == T_Left
				&& turnArray.nTurn[2] == T_Afore
				&& turnArray.nTurnNum == 3 ) //å·Šå»»èœ?
			{
				nuMemcpy( pNowNSSInfo, &(turnArray.NowNSS[0]), sizeof(NAVINSS) );
				nuMemcpy( pNextNSSInfo, &(turnArray.NextNSS[0]), sizeof(NAVINSS) );
				nLenToNextCross += turnArray.nLen[0];
				pNowNSSInfo->pNSS->TurnFlag = T_LUturn;
				pNextNSSInfo->pNSS->TurnFlag = T_No;
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[2].pNSS->RoadVoiceType;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[2].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[2].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[2].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
			}
		#ifdef ZENRIN
			else if (turnArray.nTurn[0] == T_Right)
			{
				if ((pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) < -50 &&
					(pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) > -135 &&

					 pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS)
				{
					pNextNSSInfo->pNSS->TurnFlag = T_No;
					pNowNSSInfo->pNSS->TurnFlag  = T_Right;
					pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
					pNextNSSInfo->pNRBC->FileIdx      = turnArray.NextNSS[0].pNRBC->FileIdx;
					pNextNSSInfo->pNSS->RoadNameAddr  = turnArray.NextNSS[0].pNSS->RoadNameAddr;
					pNextNSSInfo->pNSS->RoadClass     = turnArray.NextNSS[0].pNSS->RoadClass;
					if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
					{
						return nuFALSE;
					}
				}
					
			}
		#endif
		}
		else if ( nTurnFlag == T_Left )
		{
			if ( turnArray.nTurn[0] == T_Left  ) //å·Šå»»èœ?
			{
				pNowNSSInfo->pNSS->TurnFlag = T_LUturn;
				nuLONG i = 0;
				NAVINSS   TmpNSS = *pNextNSSInfo;
				NAVINSS   TmpNext = {0};
				if(pNextNSSInfo->pNSS->TurnFlag != T_Left)
				{
					while(i++ < turnArray.nTurnNum)
					{
						if(TmpNSS.pNSS->TurnFlag != T_Left)
						{
							if(!GetNextNSS(TmpNSS, &TmpNext))
							{
								break;
							}
							if (TmpNext.pNSS->TurnFlag == T_Left)
							{
								TmpNext.pNSS->TurnFlag = T_No;
							}
						}
						else
						{
							TmpNSS.pNSS->TurnFlag = T_No;
							break;
						}
						TmpNSS = TmpNext;
					}
				}
				else
				{
					pNextNSSInfo->pNSS->TurnFlag = T_No;
				}				
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[0].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[0].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[0].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
			}
			if ( turnArray.nTurn[0] == T_Afore && pNextNSSInfo->pNSS->Real3DPic == 0 )//å·Šèœ¬
			{
				pNextNSSInfo->pNSS->TurnFlag = T_No;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[0].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[0].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[0].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
			}
			else if( turnArray.nTurn[0] == T_Right
#ifdef ZENRIN
				 && pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS
#endif
				)
			{
				pNowNSSInfo->pNSS->TurnFlag = T_LeftSide;
				nuLONG i = 0;
				NAVINSS   TmpNSS = *pNextNSSInfo;
             #ifdef ZENRIN
				nuLONG Angle = pNowNSSInfo->pNSS->TurnAngle;
				Angle += TmpNSS.pNSS->TurnAngle;
             #endif
				NAVINSS   TmpNext = {0};
				if(pNextNSSInfo->pNSS->TurnFlag != T_Right)
				{
					while(i++ < turnArray.nTurnNum)
					{
						if(TmpNSS.pNSS->TurnFlag != T_Right)
						{
							if(!GetNextNSS(TmpNSS, &TmpNext))
							{
								break;
							}
						#ifdef ZENRIN
							Angle += TmpNext.pNSS->TurnAngle;
                         #endif
							if (TmpNext.pNSS->TurnFlag == T_Right)
							{
							#ifdef ZENRIN
								if(NURO_ABS(Angle) < 15)
								{
									pNowNSSInfo->pNSS->TurnAngle = Angle;
									TmpNext.pNSS->TurnAngle = 0;
								}
							#endif
								TmpNext.pNSS->TurnFlag = T_No;
							}
						}
						else
						{
						#ifdef ZENRIN
							if(NURO_ABS(Angle) < 15)
							{
								pNowNSSInfo->pNSS->TurnAngle = Angle;
								TmpNSS.pNSS->TurnAngle = 0;
							}
						#endif
							TmpNSS.pNSS->TurnFlag = T_No;
							break;
						}
						TmpNSS = TmpNext;
					}
				}
				else
				{
				#ifdef ZENRIN
					if(NURO_ABS(Angle) < 15)
					{
						pNowNSSInfo->pNSS->TurnAngle = Angle;
						pNextNSSInfo->pNSS->TurnAngle = 0;
					}
				#endif
					pNextNSSInfo->pNSS->TurnFlag = T_No;
				}
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[0].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[0].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[0].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
			}
			
		}
		else if ( nTurnFlag == T_Right )
		{
			if ( turnArray.nTurn[0] == T_Right )//?³å»»èœ?
			{
				pNowNSSInfo->pNSS->TurnFlag = T_RUturn;
				nuLONG i = 0;
				NAVINSS   TmpNSS = *pNextNSSInfo;
				NAVINSS   TmpNext = {0};
				if(pNextNSSInfo->pNSS->TurnFlag != T_Right)
				{
					while(i++ < turnArray.nTurnNum)
					{
						if(TmpNSS.pNSS->TurnFlag != T_Right)
						{
							if(!GetNextNSS(TmpNSS, &TmpNext))
							{
								break;
							}
							if (TmpNext.pNSS->TurnFlag == T_Right)
							{
								TmpNext.pNSS->TurnFlag = T_No;
							}
						}
						else
						{
							TmpNSS.pNSS->TurnFlag = T_No;
							break;
						}
						TmpNSS = TmpNext;
					}
				}
				else
				{
					pNextNSSInfo->pNSS->TurnFlag = T_No;
				}				
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[0].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[0].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[0].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
				
			}
			else if( turnArray.nTurn[0] == T_Left 
/*#ifdef ZENRIN
				&& pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS
#endif*/
				)
			{
				if((pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) >= 35 &&
					(pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) < 135 )
				{
					pNowNSSInfo->pNSS->TurnFlag = T_Left;
				}
				else
				{
				pNowNSSInfo->pNSS->TurnFlag = T_RightSide;
				}
				nuLONG i = 0;
				NAVINSS   TmpNSS = *pNextNSSInfo;
				NAVINSS   TmpNext = {0};
				if(pNextNSSInfo->pNSS->TurnFlag != T_Left)
				{
					while(i++ < turnArray.nTurnNum)
					{
						if(TmpNSS.pNSS->TurnFlag != T_Left)
						{
							if(!GetNextNSS(TmpNSS, &TmpNext))
							{
								break;
							}
							if (TmpNext.pNSS->TurnFlag == T_Left)
							{
								TmpNext.pNSS->TurnFlag = T_No;
							}
						}
						else
						{
							TmpNSS.pNSS->TurnFlag = T_No;
							break;
						}
						TmpNSS = TmpNext;
					}
				}
				else
				{
					pNextNSSInfo->pNSS->TurnFlag = T_No;
				}			
				pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
				pNextNSSInfo->pNRBC->FileIdx = turnArray.NextNSS[0].pNRBC->FileIdx;
				pNextNSSInfo->pNSS->RoadNameAddr = turnArray.NextNSS[0].pNSS->RoadNameAddr;
				pNextNSSInfo->pNSS->RoadClass    = turnArray.NextNSS[0].pNSS->RoadClass;
				if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
				{
					return nuFALSE;
				}
			}
		}
#ifdef ZENRIN
		else if ( nTurnFlag == T_LeftSide )
		{
			if ( pNextNSSInfo->pNSS->TurnFlag == T_No || pNextNSSInfo->pNSS->TurnFlag == T_RightSide )
			{
				if ((pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) > 50 &&
					(pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) < 135 && 
					 pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS)
				{
					pNowNSSInfo->pNSS->TurnFlag  = T_Left;
					pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
					pNextNSSInfo->pNRBC->FileIdx      = turnArray.NextNSS[0].pNRBC->FileIdx;
					pNextNSSInfo->pNSS->RoadNameAddr  = turnArray.NextNSS[0].pNSS->RoadNameAddr;
					pNextNSSInfo->pNSS->RoadClass     = turnArray.NextNSS[0].pNSS->RoadClass;
					if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
					{
						return nuFALSE;
					}
				}
			}
			else if (pNextNSSInfo->pNSS->TurnFlag == T_Left)
			{
				if ((pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) > 50 &&
					(pNowNSSInfo->pNSS->TurnAngle + pNextNSSInfo->pNSS->TurnAngle) < 135 &&
					 pNextNSSInfo->pNSS->RoadVoiceType == VRT_ROADCROSS)
				{
					pNowNSSInfo->pNSS->TurnFlag  = T_Left;
					pNextNSSInfo->pNSS->TurnFlag = T_No;
					pNextNSSInfo->pNSS->RoadVoiceType = turnArray.NextNSS[0].pNSS->RoadVoiceType;
					pNextNSSInfo->pNRBC->FileIdx      = turnArray.NextNSS[0].pNRBC->FileIdx;
					pNextNSSInfo->pNSS->RoadNameAddr  = turnArray.NextNSS[0].pNSS->RoadNameAddr;
					pNextNSSInfo->pNSS->RoadClass     = turnArray.NextNSS[0].pNSS->RoadClass;
					if( bReCalNextCross && !ColNextCrossData( *pNowNSSInfo, *pNextNSSInfo, nLenToNextCross ) )
					{
						return nuFALSE;
					}
				}
			}
		}
#endif
    }
	return nuTRUE;
}

nuBYTE CNaviData::GetTurnFlag(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS)
{
	nuBYTE nTurnFlag = T_No;
	if ( !pNowNSS || ! pNextNSS )
	{
		return nTurnFlag;
	}
	nTurnFlag = (nuBYTE)pNowNSS->TurnFlag;
	if( pNowNSS->CrossCount > 1 )
	{
		for ( nuWORD i = 0; i < pNowNSS->CrossCount; i++)
		{
			if (  ( pNowNSS->NCC[i].CrossBlockIdx != pNowNSS->BlockIdx || pNowNSS->NCC[i].CrossRoadIdx  != pNowNSS->RoadIdx )
				&&( pNowNSS->NCC[i].CrossBlockIdx != pNextNSS->BlockIdx || pNowNSS->NCC[i].CrossRoadIdx  != pNextNSS->RoadIdx )
				)
			{
				if (   pNowNSS->TurnAngle >= -10
					&& pNowNSS->TurnAngle <= 10  )
				{
					nTurnFlag = T_Afore;
					break;
				}
			}
		}
		
	}
	return nTurnFlag;
}

nuBOOL CNaviData::bChangeSymbols(nuWCHAR *wsRoadName, nuWCHAR *wsOut)
{
	if (wsRoadName == NULL || wsOut == NULL )
	{
		return nuFALSE;
	}
	/*
	nuBYTE nIndex = 0;
	for ( nIndex = 0; nIndex < nuWcslen(wsRoadName); nIndex ++ )
	{
	nuWCHAR sTmp = *(wsRoadName + nIndex);
	if ( *(wsRoadName + nIndex) == 0x7C) //"|"
	{
	sTmp = 0x2C;//','
	}
	nuMemcpy( wsOut + nIndex, &sTmp, sizeof(nuWCHAR) );
	}
	*( wsOut + nuWcslen(wsRoadName)) = 0; 
	*/
	if( g_pInFsApi->pUserCfg->nWaveTTSType != WAVETTS_TYPE_TTSNOROADNAME )
	{
		nuINT nCount = nuWcslen(wsRoadName);
		nuWcscpy(wsOut, wsRoadName);
		/**/
		for( nuINT i = 0; i < nCount; ++i )
		{
			if( wsRoadName[i] == '&' )
			{
				wsOut[i] = ',';
			}
		}
	}
	
	return nuTRUE;
}

nuBOOL CNaviData::GetHighWayExtern(PCROSSFOLLOWED pFollowCross, nuBOOL bIngoreFirst, nuBYTE &nNumGet,SHOWNODE *NodeArray, nuBYTE nArrayNum)
{
	if (   !pFollowCross 
		|| pFollowCross->nRdSegCount == 0 
		|| pFollowCross->pRdSegList == NULL 
		|| nNumGet <= 0 )
	{
		return nuFALSE;
	}
    if ( nNumGet > nArrayNum )
    {
		return nuFALSE;
    }
	nuDWORD nIndex = 0;
	nuLONG nType = 0;
	nuLONG nNodeLen = 0;
	nuBOOL bAdd = nuFALSE;
	nuBYTE nAddTotal = 0;
	
	SHOWNODE TmpNode = {0};
    PROADSEGDATA pRoadDataFirst = NULL;
	PROADSEGDATA pRoadDataNext  = NULL;

	ROAD_SEG_ITEM RoadItem = {0};
	nuLONG nDWIndex = 0;
	nuBYTE nArrayIndex = 0;
	
	for( nIndex = 1; nIndex < pFollowCross->nRdSegCount; nIndex++ )
	{
		/**/
		if ( nIndex == 1 && !bIngoreFirst )
		{
			nNodeLen += g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nDisToNextCoor;
		}
		else
		{
			nNodeLen += ( PROADSEGDATA(pFollowCross->pRdSegList + (nIndex - 1)))->RoadLength;
		}
		pRoadDataFirst = ( PROADSEGDATA(pFollowCross->pRdSegList + (nIndex - 1)));
		pRoadDataNext  = ( PROADSEGDATA(pFollowCross->pRdSegList + nIndex ));
		if ( pRoadDataFirst->RoadClass == 5 )
		{
			pRoadDataFirst->RoadVoiceType = VRT_SlipRoad;
		}
		if ( pRoadDataNext->RoadClass == 5 )
		{
			pRoadDataNext->RoadVoiceType = VRT_SlipRoad;
		}
		nuLONG nNowVoiceType  = pRoadDataFirst->RoadVoiceType;
		nuLONG nNextVoiceType = pRoadDataNext->RoadVoiceType;
		nuLONG nNowClass      = pRoadDataFirst->RoadClass;
		nuLONG nNextClass     = pRoadDataNext->RoadClass;
		if ( (nType = GetVoiceType( nNowVoiceType, nNowClass, nNextVoiceType, nNextClass )) != AF_LoseHere )
		{
			TmpNode.nIconType = nuWORD(1024 + nType * 10);
			nDWIndex = pRoadDataNext->nMapIdx;
			RoadItem.nBlockIdx = pRoadDataNext->BlockIdx;
			RoadItem.nRoadIdx = pRoadDataNext->RoadIdx;
			if ( AF_Out_Freeway == nType )
			{
				//Test
				RoadItem.nRoadIdx = (pRoadDataNext + 0)->RoadIdx;
				bAdd = nuTRUE;
			}
			//bAdd = nuTRUE;	 
		}
		else
		{
			if ( pRoadDataFirst->nCrossCount > 0 &&  pRoadDataFirst->pRdCross != NULL )
			{
				nuWORD nCrossIndex = 0;
				for ( ; nCrossIndex < pRoadDataFirst->nCrossCount; nCrossIndex++ )
				{
					if (   pRoadDataFirst->pRdCross[nCrossIndex].nMapIdx == pRoadDataFirst->nMapIdx
						&& pRoadDataFirst->pRdCross[nCrossIndex].BlockIdx == pRoadDataFirst->BlockIdx
						&& pRoadDataFirst->pRdCross[nCrossIndex].RoadIdx == pRoadDataFirst->RoadIdx
						)
					{
						continue;
					}
					if (   pRoadDataFirst->pRdCross[nCrossIndex].nMapIdx == pRoadDataNext->nMapIdx
						&& pRoadDataFirst->pRdCross[nCrossIndex].BlockIdx == pRoadDataNext->BlockIdx
						&& pRoadDataFirst->pRdCross[nCrossIndex].RoadIdx == pRoadDataNext->RoadIdx
						)
					{
						continue;
					}
					if (  pRoadDataFirst->pRdCross[nCrossIndex].RoadClass == 5
						 )
					{
						pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType = VRT_SlipRoad;
					}
					if(    pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType == VRT_SlipRoad
						|| pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType == VRT_RestStop
						|| pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType == VRT_Tollgate
						|| pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType == VRT_StopcockWay
						|| pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType == VRT_GuideWay )
					{
						switch ( pRoadDataFirst->pRdCross[nCrossIndex].RoadVoiceType )
						{
							case VRT_SlipRoad:
								TmpNode.nIconType	= 5026;
								bAdd = nuTRUE;
								break;
							case VRT_RestStop:
								TmpNode.nIconType	= 5030;
								bAdd = nuTRUE;
							break;
						}
					}
					if ( bAdd )
					{
						nDWIndex = pRoadDataFirst->pRdCross[nCrossIndex].nMapIdx;
						RoadItem.nBlockIdx = pRoadDataFirst->pRdCross[nCrossIndex].BlockIdx;
						RoadItem.nRoadIdx  = pRoadDataFirst->pRdCross[nCrossIndex].RoadIdx;
						break;
					}
					
				}	
			}
		
		}
		if ( bAdd )
		{
			nAddTotal++;
			bAdd = nuFALSE;
			if ( bIngoreFirst && nAddTotal == 1 )
			{
				nNodeLen = 0;
			}
			else 
			{
				if ( bIngoreFirst )
				{
					m_nSaveIndex[nAddTotal - 2 ] = nIndex - 1;
				}
				else
				{
					m_nSaveIndex[nAddTotal - 1 ] = nIndex - 1;
				}
				TmpNode.nDistance = nNodeLen;
				TmpNode.point = ( PROADSEGDATA(pFollowCross->pRdSegList + (nIndex - 1)))->EndCoor;
				ROAD_SEG_DATA  SegData ={0};
				
				SegData.nCountUsed = 1;
				SegData.pRoadSeg = &RoadItem;
				
				nDWIndex = g_pInFsApi->FS_MapIDIndex( nDWIndex, MAP_ID_TO_INDEX );
				
				if( g_pInFsApi->FS_GetRoadSegData( nDWIndex, &SegData ) )
				{
					pRoadDataNext->nNameAddr = SegData.pRoadSeg->nNameAddress;
				}
				else
				{
					pRoadDataNext->nNameAddr = 0;
				}
				RNEXTEND RnEx = {0};
				if ( !g_pInFsApi->FS_GetRoadName(   nDWIndex ,
					pRoadDataNext->nNameAddr , 
					pRoadDataNext->RoadClass, 
					TmpNode.wsName, 
					MAX_LISTNAME_NUM,
					&RnEx) )
				{
					;
				}
				NodeArray[nArrayIndex] = TmpNode;
				nArrayIndex++;
				nNodeLen = 0;
				if ( nArrayIndex >= nNumGet )
				{
					break;
				}
				
			}

		}

		
	}
	if ( bIngoreFirst && nAddTotal == 1 )
	{
		nNumGet = nAddTotal - 1;
	}
	else
	{
		nNumGet = nAddTotal ;
	}
	return nuTRUE;
}

nuLONG CNaviData::GetVoiceType(nuLONG nNowVoiceType, nuLONG nNowClass, nuLONG nNextVoiceType, nuLONG nNextClass)
{
	switch(nNowVoiceType)
	{
	case VRT_Tunnel:
	case VRT_Underpass:
	case VRT_UnOverpass:
	case VRT_Tollgate:
	case VRT_RestStop:
		return AF_LoseHere;
	}
	switch(nNextVoiceType)
	{
	case VRT_CityTownBoundary:
		return AF_CityTownBoundary;
	case VRT_MapBoundary:	
		return AF_MapBoundary;
	case VRT_MidSite:		
		return AF_MidSit;
	case VRT_EndSite:		
		return AF_EndSite;
	}
	
	if (   nNowVoiceType != nNextVoiceType
		&& (  (nNowVoiceType != VRT_Defway || nNextVoiceType != VRT_Defway  )
	        || nNowClass!= nNextClass ) )
	{
		/**/
		if (  nNextVoiceType == VRT_FastToSlow )
		{
			return AF_IN_SLOWROAD;
		}
		if (  nNowVoiceType  == VRT_SlowToFast )
		{
			return AF_IN_FASTROAD;
		}
		if(nNextVoiceType == VRT_Tollgate)			
		{
			return AF_In_Tollgate;
		}
		if(nNextVoiceType == VRT_RestStop)	
		{
			return AF_In_RestStop;
		}
		if(nNextVoiceType == VRT_Tunnel)	
		{
			return AF_In_Tunnel;
		}
		if(nNowVoiceType == VRT_Freeway)
		{
			return AF_Out_Freeway;
		}
		if(nNextVoiceType == VRT_Freeway)
		{
			return AF_In_Freeway;
		}
		if(    nNowVoiceType == VRT_Fastway )
		{
			if ( nNextVoiceType == VRT_SlowWay )
			{
				return AF_IN_SLOWROAD;
			}
			else
			{
				return AF_Out_Fastway;
			}
		}
		if(   nNextVoiceType == VRT_Fastway 
		   && nNowVoiceType  == VRT_SlowWay	)
		{
			return AF_In_Fastway;
		}
		if(nNextVoiceType==VRT_SlipRoad)
		{
			if ( nNowClass <= 3)
			{
				return AF_Out_Freeway;
			}
			else
			{
				return AF_In_SlipRoad;
			}
		}
		if(nNowVoiceType==VRT_Overpass)
		{
			return AF_Out_Overpass;
		}
		if(nNextVoiceType==VRT_Overpass)	
		{
			return AF_In_Overpass;
		}
		if(nNextVoiceType==VRT_Underpass)
		{
			return AF_In_Underpass;
		}
		if(nNextVoiceType==VRT_UnOverpass)
		{
			return AF_In_UnOverpass;
		}
		if(nNowVoiceType==VRT_TrafficCircle)		
		{
			return AF_Out_TrafficCircle;
		}
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_In_TrafficCircle;
		}
		if (nNowVoiceType == VRT_Tunnel )
		{
			return AF_Out_Tunnel;
		}
	}
	else
	{
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_LoseHere;//In TrafficCircle, ignore the road crossing
		}
	}

	return AF_LoseHere;

}

nuBOOL CNaviData::GetHighExternList()
{
	if ( m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass > 3)
	{
		return nuFALSE;
	}
	if ( m_nHeighwayExtern == 1 )
    {
		if ( (	m_StrHighWayCmp.nDWIndex   == 0 
			&& m_StrHighWayCmp.nBlkIndex  == 0
			&& m_StrHighWayCmp.nRdIndex   == 0 )
			|| nuMemcmp( &m_StrHighWayCmp, &m_StrHighWayLast, sizeof(m_StrHighWayLast) ) != 0
			)
		{	
			nuMemcpy( &m_StrHighWayCmp, &m_StrHighWayLast, sizeof(m_StrHighWayLast) );
			if ( m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass <= 3)   	
			{	
			/*if ( g_pInFsApi->pGdata->pCrossFollowed )
			{
			g_pInRTApi->RT_ReleaseNextCrossData( (CROSSFOLLOWED **)(&(g_pInFsApi->pGdata->pCrossFollowed)) );
			g_pInFsApi->pGdata->pCrossFollowed = NULL;
			}*/
			/*
			g_pInRTApi->RT_GetNextCrossData( g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nDwIdx,
											g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nBlkIdx,
											g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nRoadIdx,
											nulAtan2(g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nRdDy , g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nRdDx),
											g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.ptMapped,
											g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.ptNextCoor,
											&(g_pInFsApi->pGdata->pCrossFollowed)
											);
			GetHighWayExtern(g_pInFsApi->pGdata->pCrossFollowed );*/
				

				
			/**/	
				g_pInRTApi->RT_GetNextCrossData( m_StrHighWayLast.nDWIndex,
												m_StrHighWayLast.nBlkIndex,
												m_StrHighWayLast.nRdIndex,
												m_StrHighWayLast.nAngle,
												m_StrHighWayLast.ptMap,
												m_StrHighWayLast.ptNextCoor,
												&(g_pInFsApi->pGdata->pCrossFollowed)
												);

				m_nSaveNum = 2;
				GetHighWayExtern( g_pInFsApi->pGdata->pCrossFollowed,
								  !m_StrHighWayLast.bEndPt,
								  m_nSaveNum,
								  m_NodeArr,
								  3			
								 );
				
			}
		}
		if ( g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount < 3  )
		{
			nuULONG nNumAdd = NURO_MIN( ( 3 - g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount), m_nSaveNum );
			nuBYTE nIndex = 0;
			for( ; nIndex < nNumAdd; nIndex++ )
			{
				AddListNode( m_NodeArr[nIndex], nuFALSE );
			}
		}
    }
	return nuTRUE;
	
}

nuBYTE CNaviData::JudgeRoadType( PSEEKEDROADDATA pNowRoadInfo )
{
	nuBYTE nRet = CROSSNEARTYPE_DEFAULT;
	if (  m_nHeighwayExtern != 1 )
	{
		return nRet;
	}
	if ( pNowRoadInfo == NULL )
	{
		return nRet;
	}
	if ( pNowRoadInfo->nRoadClass <= 3  )
	{
		nRet = CROSSNEARTYPE_HIGHWAY;
	}
	else //äžåšé«éä?èµæ??æžç©º
	{
		if ( g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount > 0)
		{
			g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount = 0;
			g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum   = 0;
			nuMemset( m_NodeArr, 0, sizeof(SHOWNODE) * SAVENODENUM );
			m_nSaveNum = 0;
		}	
	}

    if (  CROSSNEARTYPE_HIGHWAY == nRet )
    {
		//éŠå??Ÿåºåœå???šé?è·¯åš?çŽ¢?ºæ¥è·¯å?äž­ç?äœçœ®
		nuBOOL bFind = nuFALSE;
		nuBOOL bInRightWay = nuFALSE;
		nuDWORD nFindIndex = 0;

		//éŠå??€æ­èµæ??¯ï?1.èµæ?äžºç©º 2.?¹å??ä? 3.è·è?äºä?äžªè·¯??
		nuBOOL bJudge = nuFALSE;
		nuBYTE nGetNum = 0;
		if ( g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount == 0 )
		{
			bJudge = nuTRUE;
			nGetNum = 3;
		}
		else 
		{
			if ( g_pInFsApi->pGdata->pCrossFollowed )
			{  
				nuDWORD nSegCnt = 0;
				PROADSEGDATA pTmpSeg = NULL;
				for(; nSegCnt < g_pInFsApi->pGdata->pCrossFollowed->nRdSegCount; nSegCnt++ )
				{
					pTmpSeg = g_pInFsApi->pGdata->pCrossFollowed->pRdSegList + nSegCnt; 
					nuLONG nMapIdx = g_pInFsApi->FS_MapIDIndex( pTmpSeg->nMapIdx, MAP_ID_TO_INDEX );
					if (   pNowRoadInfo->nDwIdx   == nMapIdx
						&& pNowRoadInfo->nBlkIdx  == pTmpSeg->BlockIdx
						&& pNowRoadInfo->nRoadIdx == pTmpSeg->RoadIdx 
						)
					{
						bFind = nuTRUE;
						nFindIndex = nSegCnt;
						if ( nuMemcmp( &pNowRoadInfo->ptNextCoor, &pTmpSeg->EndCoor, sizeof(NUROPOINT)) == 0 )
						{
							bInRightWay = nuTRUE;
						}
						break;
					}
				}
				//?žå??æ¹?ä???æ°?çŽ¢,elseæ­?¡®?æ¹?ç?è¯å°±èŠå€?­å??æ¯?Šè?äºä?äžªè·¯??
				if ( bInRightWay == nuFALSE )
				{
					bJudge = nuTRUE;
					nGetNum = 3;
				}
				else
				{
                    nuBYTE nIndex = 0;
					nuBYTE nFindResult = 0; //0è¡šç€ºæ²¡æ??Ÿå°ïŒ?å°±æ¯ç¬¬ä?äžªï?äŸæ¬¡...
					for( ; nIndex < m_nSaveNum; nIndex++ )
					{
						if ( nFindIndex <= m_nSaveIndex[nIndex])
						{
							nFindResult = nIndex + 1;
							break;
						}
					}
					if ( nFindResult != 1 ) //1å°±æ¯è¡šç€º?šç¬¬äžäžªä????ä»¥æ???è?è¡æ?çŽ?
					{
						bJudge = nuTRUE;
						if ( nFindResult == 0 )
						{
							nGetNum = 3;
						}
						else // = 2 || 3?èŠæŸ?äžª??
						{
							nGetNum = nFindResult - 1;
						}
					}
				}
			}	
		}
		//åŒå§æ¥?Ÿæ°?æ°??
		
		if ( bJudge && nGetNum > 0 && nGetNum <= 3)
		{
			//m_nSaveNum = nGetNum;
			m_nSaveNum = 3;
			
			g_pInRTApi->RT_GetNextCrossData( pNowRoadInfo->nDwIdx, 
				pNowRoadInfo->nBlkIdx,
				pNowRoadInfo->nRoadIdx,
				nulSqrt( pNowRoadInfo->nRdDx, pNowRoadInfo->nRdDy ),
				pNowRoadInfo->ptMapped,
				pNowRoadInfo->ptNextCoor,
				&(g_pInFsApi->pGdata->pCrossFollowed)
				);
			GetHighWayExtern( g_pInFsApi->pGdata->pCrossFollowed,
				nuFALSE,
				m_nSaveNum,
				m_NodeArr,
				SAVENODENUM			
				);
		   bInRightWay = nuTRUE;
			
		}
		//è®¡ç?ç¬¬ä?äžªç?è·çŠ»
		if ( g_pInFsApi->pGdata->pCrossFollowed )
		{
			nuDWORD nStartIndex = 0;
			m_NodeArr[0].nDistance = 0;
			if ( bInRightWay )
			{
				nStartIndex = nFindIndex + 1;
				m_NodeArr[0].nDistance += g_pInFsApi->pGdata->drawInfoMap.roadCenterOn.nDisToNextCoor;
			}
			
			for ( ; nStartIndex <= m_nSaveIndex[0]; nStartIndex++ )
			{
			/*if ( nStartIndex == nFindIndex )
			{
			
			  }
				else*/
				{
					m_NodeArr[0].nDistance += ( PROADSEGDATA( g_pInFsApi->pGdata->pCrossFollowed->pRdSegList + nStartIndex ) )->RoadLength;
				}
				
			}
		}

    }
	//Test
	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum   = 0;

    for( nuBYTE i = 0; i < m_nSaveNum; i++ )
	{
		AddListNode( m_NodeArr[i],nuFALSE );
	}
	return nRet;
}

nuBOOL CNaviData::IsRTBUnroute(PNAVIRTBLOCKCLASS pNRBC)
{
	if( pNRBC               == NULL	 || 
	   pNRBC->RTBStateFlag == 1   
		 )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

nuBOOL CNaviData::GetPTSName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuWORD nBufLen)
{
	if( nDwIdx == -1 || pNameBuf == NULL )
	{
		return nuFALSE;
	}
	nuFILE *pFile = NULL;
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	if( nNameAddr >= 0 )
	{
		nuTcscpy(sFile, nuTEXT(".PTS"));
		if( !g_pInFsApi->FS_FindFileWholePath(nDwIdx, sFile, NURO_MAX_PATH) )
		{
			return nuFALSE;
		}
		pFile = nuTfopen(sFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(pFile, nNameAddr, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuBYTE nNameLen = 0;
		if ( nuFread( &nNameLen, sizeof(nuBYTE), 1, pFile) != 1)
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if ( nNameLen >  nBufLen )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread( pNameBuf, nNameLen, 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuFclose(pFile);
		*((nuWCHAR *)pNameBuf + nNameLen/2) = 0;

		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
	
}


nuBOOL CNaviData::bSimMode()
{
	nuBYTE nMoveType = MAP_NAVI_MODE_STOP; 
	if ( g_pInFsApi->pGdata->timerData.nAutoMoveType != MAP_NAVI_MODE_STOP )
	{
		if( g_pInFsApi->pGdata->timerData.nAutoMoveType == AUTO_MOVE_TIMER )
		{
			nMoveType = g_pInFsApi->pGdata->timerData.nAutoMoveType;
		}
		else
		{
			nMoveType = g_pInFsApi->pGdata->timerData.nAutoMoveType;
		}
	}
	return ( nMoveType == AUTO_MOVE_SIMULATION );
}

//DEL nuBOOL CNaviData::DrawLIFInfo()
//DEL {
//DEL 	
//DEL }

//?®å??ªé?å¯¹é??å¬è·?
nuBOOL CNaviData::ChangNCCName(PNAVIFROMTOCLASS pNFTC,PNAVIRTBLOCKCLASS pNRBC )
{
	if (   !pNFTC
		|| !pNRBC
		|| pNRBC->RTBStateFlag != 0  
		|| pNRBC->RTBWMPFixFlag != 0 )
	{
		return nuFALSE;
	}
	if ( pNRBC->NodeIDCount == 0 || pNRBC->NSS == NULL  )
	{
		return nuFALSE;
	}
	nuDWORD *pNode = pNRBC->NodeIDList;
    nuTCHAR sSptFileName[256] = {0};
	nuTCHAR sFileName[256] = {0};
	nuBOOL bHaveSpt = nuFALSE;
	nuTcscpy(sSptFileName, nuTEXT(".SPT"));
	nuTcscpy(sFileName, nuTEXT(".MWP"));
	if( !g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sFileName, 256 ) )
	{
		return nuFALSE;
	}
	if( !g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sSptFileName, 256 ) )
	{
		return nuFALSE;
	}

	
	
	//Test file is Exist
	nuFILE *fp = NULL;
	if( !(fp = nuTfopen( sSptFileName, NURO_FS_RB )) )
	{
		fp = nuTfopen( sFileName, NURO_FS_RB );
	}
	else
	{
		bHaveSpt = nuTRUE;
	}
	if ( !fp )
	{
		pNRBC->RTBWMPFixFlag = 2;
		return nuFALSE;
	}
	else
	{
		nuFclose( fp );
	}

	nuDWORD nNodeIndex = 0;
	PNAVISUBCLASS pNSS = NULL;
	for ( ;nNodeIndex < pNRBC->NodeIDCount - 1; nNodeIndex++ )
	{
		if ( pNRBC->NSS[nNodeIndex].RoadClass <= 3 )
		{
			nuLONG nNCCIndex = 0;//pNRBC->NSS[nNodeIndex].CrossCount;
			nuBOOL bNeedSeek = nuFALSE;
			for (;nNCCIndex < pNRBC->NSS[nNodeIndex].CrossCount; nNCCIndex++  )
			{
				/**/if (   pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossBlockIdx     == pNRBC->NSS[nNodeIndex].BlockIdx
					&& pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadIdx      == pNRBC->NSS[nNodeIndex].RoadIdx 
					&& pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadNameAddr == pNRBC->NSS[nNodeIndex].RoadNameAddr )
				{
					continue;
				}
				NAVINSS NowNSS = {0},NextNSS = {0};
				NowNSS.pNFTC  = pNFTC;
				NowNSS.pNRBC  = pNRBC;
				NowNSS.pNSS   = &( pNRBC->NSS[nNodeIndex] );
				NowNSS.nIndex = nNodeIndex;
				if ( GetNextNSS( NowNSS, &NextNSS ) )
				{
					if (  pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossBlockIdx      == NextNSS.pNSS->BlockIdx
						&& pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadIdx      == NextNSS.pNSS->RoadIdx 
						&& pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadNameAddr == NextNSS.pNSS->RoadNameAddr)
					{
						continue;
					}
				}
				if ( pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nFlag == 0 )
				{
					
					nuULONG nNameAddr = 0, nNameLen = 0;
					NODELISTINFO NodeListInfo = {0};
					NodeListInfo.nFileIndex = pNRBC->FileIdx;
					NodeListInfo.nRTBIndex  = pNRBC->RTBIdx;
					NodeListInfo.nNodeID    = (*(pNRBC->NodeIDList + nNodeIndex + 1) & 0xFFFFF) + ( ( pNRBC->RTBIdx << 20 ) & 0XFFF00000 );
					NodeListInfo.nNodeIndex = nNodeIndex + 1;
				
					SPTNODELISTINFO SptPre ={0};
					SPTNODELISTINFO SptNext ={0};
					SptPre.nFileIndex = pNRBC->FileIdx;
					SptPre.NodeId.nBlockID = pNRBC->NSS[nNodeIndex].BlockIdx;//pNRBC->RTBIdx;pNRBC->NSS[nNodeIndex].BlockIdx
					SptPre.NodeId.nRoadID  = pNRBC->NSS[nNodeIndex].RoadIdx;
					SptNext.nFileIndex     = pNRBC->FileIdx;
					SptNext.NodeId.nBlockID = pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossBlockIdx;
					SptNext.NodeId.nRoadID  = pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadIdx;

					
					if(  pNRBC->NSS[nNodeIndex].SPTFlag == 1 && bHaveSpt && m_SptFile.GetNodeNameAddr( sSptFileName, SptPre, SptNext, nNameAddr, nNameLen))
					{
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadNameAddr = nNameAddr;
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nRoadNameLen      = nNameLen;
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nFlag = 3;                        
					}
					else if ( m_MwpFile.GetNodeNameAddr( sFileName, NodeListInfo, nNameAddr, nNameLen ) )
					{
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].CrossRoadNameAddr = nNameAddr;
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nRoadNameLen      = nNameLen;
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nFlag = 1;
					}
					else
					{
						pNRBC->NSS[nNodeIndex].NCC[nNCCIndex].nFlag =  2;
					}
				}
				
			}
		}
		
	}
	m_MwpFile.FreeNewMem();
	pNRBC->RTBWMPFixFlag = 1;
	return nuTRUE;
}

nuBOOL CNaviData::AddNameStr( nuWCHAR* wsName)
{
	nuWCHAR StrSign[3] = {0};
	nuAsctoWcs(StrSign, sizeof(StrSign), "(", sizeof("("));
	nuWcscat( wsName,StrSign);
	nuWCHAR StrTmp[5] = { 20986,21475 };
	nuWcscat(wsName,StrTmp);
	nuAsctoWcs(StrSign, sizeof(StrSign), ")", sizeof(")"));
	nuWcscat(wsName,StrSign );
	return nuTRUE;
}

nuLONG CNaviData::GetNssFirstAngle(PNAVISUBCLASS pNss)
{
	if ( !pNss )
	{
		return 0;
	}
	nuLONG nRet = 0;
	nuINT nIndex = 0;
	for ( ; nIndex < pNss->ArcPtCount - 1; nIndex++  )
	{
		nuLONG dx = 0,dy = 0;
		dx = pNss->ArcPt[nIndex + 1].x - pNss->ArcPt[nIndex ].x;
		dy = pNss->ArcPt[nIndex + 1].y - pNss->ArcPt[nIndex ].y;
		if (   0 == dx
			&& 0 == dy
			)
		{
			continue;
		}
		nRet = nulAtan2(dy, dx);
		break;
	}
	return nRet;
}

nuBYTE CNaviData::JudgeZoomRoad1()
{
	nuBYTE nCrossNearType = CROSSNEARTYPE_HIGHWAY;
	nuBYTE nCrossNearTypeTmp = CROSSZOOM_TYPE_IN;
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	if ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance )
	{
		nLenToNextCross = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance;
	}
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
#ifdef ECI
	if (g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID &&  g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicDis < IN_REAL3DPIC)
	{
		nCrossNearType = CROSSNEARTYPR_PIC_IN_SMALL;
	}
	g_pInFsApi->pGdata->drawInfoNavi.nCrossZoomType = nCrossNearTypeTmp;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = nLenToNextCross;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToBeforeCross   = nLenToBeforeCross;
#else
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance;
	if ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance )
	{
		nLenToNextCross = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance;
	}
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;

	
	if (   ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID && g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance <= IN_REAL3DPIC )
		
		)//||  ( g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID && g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance < IN_REAL3DPIC )
	{
		nCrossNearType = CROSSNEARTYPE_PIC_IN;
	}
	if (   g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance == 0 
		&&  g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID
		&& g_pInFsApi->pGdata->drawInfoNavi.nextCross.nDistance < IN_REAL3DPIC
		)
	{
		nCrossNearType = CROSSNEARTYPE_PIC_IN;
	}

	if ( g_pInFsApi->pUserCfg->bShowBigPic == 0 && CROSSNEARTYPE_PIC_IN == nCrossNearType )
	{
		nCrossNearType = CROSSNEARTYPR_PIC_IN_SMALL;
	}
	if ( g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID )
	{
		if (   g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID != g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID 
			&& g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID != g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID 
			)
		{
			g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID = 0;
		}
		else
		{
			if ( nCrossNearType == CROSSNEARTYPE_PIC_IN || nCrossNearType == CROSSNEARTYPR_PIC_IN_SMALL)
			{
				nCrossNearType = CROSSNEARTYPE_HIGHWAY;
			}
		}
	}

	g_pInFsApi->pGdata->drawInfoNavi.nCrossZoomType = nCrossNearTypeTmp;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToCrossZoomType = nLenToNextCross;
	g_pInFsApi->pGdata->drawInfoNavi.nDisToBeforeCross   = nLenToBeforeCross;
#endif
/*#ifdef VALUE_EMGRT
		if ( m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass > 3 && CROSSNEARTYPE_HIGHWAY == nCrossNearType )
		{
			nCrossNearType = CROSSNEARTYPE_DEFAULT;
		}
#endif*/

	return nCrossNearType;	
	 
}

nuBOOL CNaviData::GetRtListNew()
{
	NAVINSS NowNSSInfo,NextNSSInfo,NSSInfo, TempNSSInfo={0} ;
	
	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum = 0;
	//
	PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	SHOWNODE tmpnode = {0}; 
	nuLONG nLenToLast = 0;
	nuLONG nNodeLen = 0;
	nuLONG nType = AF_LoseHere;
	nuBYTE nTraffNum = 0;
	nuBOOL bEndInHighWay = nuFALSE,b_HighListCross = nuFALSE , b_SameRoad = nuFALSE;
	if( pNFTC == NULL)
	{
		return nuFALSE;
	}
	nuMemset( &NowNSSInfo, 0, sizeof(NowNSSInfo) );
	nuMemset( &NextNSSInfo, 0, sizeof(NextNSSInfo) );
	nuMemset( &NSSInfo, 0, sizeof(NSSInfo) );
	/*#ifdef VALUE_EMGRT
	if ( m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadClass > 3 )
	{
		return nuFALSE;
	}
	#endif*/
	while ( pNFTC != NULL)
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while ( pNRBC != NULL) 
		{
			if(  (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum >= g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum)
				|| (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx > g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{
				break;
			}
			if ( pNRBC->RTBStateFlag == 1 && pNRBC->NSS == NULL )
			{
				if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
				{
					g_pInRTApi->RT_LocalRoute( &pNRBC );
				}
			}
			
			if( pNRBC->NSS != NULL && pNRBC->RTBStateFlag == 0 )
			{
				nuINT nFrist = 0;
				if( pNRBC == m_ptNavi.pNRBC )
				{
					nFrist = m_ptNavi.nSubIdx; 
				}
				for( nuDWORD i = nFrist; i < pNRBC->NSSCount; i ++ )
				{	
					if(    &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]) == &(pNRBC->NSS[i])
						&& &pNRBC->NSS[i] != NULL )
					{
						nuLONG dx = 0, dy = 0, dxy = 0;
						dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
						dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
						dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
						nNodeLen += dxy;
						nuLONG nTotalLen = 0;
						for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
						{
							dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
							dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
							if ( j >= m_ptNavi.nPtIdx + 1)
							{
								nNodeLen += dxy;
							}
							nTotalLen += dxy;
						}
						if ( (i == nFrist) && nTotalLen )
						{
							nNodeLen = nNodeLen * pNRBC->NSS[i].RoadLength / nTotalLen;
						}
					}
					else if ( &pNRBC->NSS[i] != NULL )
					{
						nNodeLen   += pNRBC->NSS[i].RoadLength; 
						//nNodeLen   += pNRBC->NSS[i].RoadLength; 
						/*nuLONG dx, dy, dxy;
						nuLONG nTotalLen = 0;
						for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
						{
							dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
							dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
							nNodeLen += dxy;
						}		*/
					}
					
					NowNSSInfo.pNFTC = pNFTC;
					NowNSSInfo.pNRBC = pNRBC;
					NowNSSInfo.pNSS		= &(pNRBC->NSS[i]);
					NowNSSInfo.nIndex	= i;
					if ( NowNSSInfo.pNSS == NULL )
					{
						break;
					}
					GetNextNSS( NowNSSInfo,&NextNSSInfo );					
					if( ( NextNSSInfo.pNSS == NULL ) )
					{
						bEndInHighWay = nuTRUE;
						m_StrHighWayLast.nDWIndex   = NowNSSInfo.pNRBC->FileIdx;
						m_StrHighWayLast.nBlkIndex  = NowNSSInfo.pNSS->BlockIdx;
						m_StrHighWayLast.nRdIndex   = NowNSSInfo.pNSS->RoadIdx;
						nuLONG nDx,nDy;
						nDx = NowNSSInfo.pNSS->ArcPt[1].x - NowNSSInfo.pNSS->ArcPt[0].x;
						nDy = NowNSSInfo.pNSS->ArcPt[1].y - NowNSSInfo.pNSS->ArcPt[0].y;
						m_StrHighWayLast.nAngle     = nulAtan2( nDy, nDx );
						m_StrHighWayLast.ptMap      = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1 ];
						m_StrHighWayLast.ptNextCoor = m_StrHighWayLast.ptMap; 
						m_StrHighWayLast.bEndPt     = nuTRUE;
						break;
					}
				#ifdef ZENRIN
					if (NextNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType != VRT_ROADCROSS)
					{
						if ( NowNSSInfo.pNSS->TurnFlag <= T_Afore)
						{
							TempNSSInfo.pNSS = NowNSSInfo.pNSS;
							TempNSSInfo.pNRBC = NowNSSInfo.pNRBC;
							TempNSSInfo.nIndex = NowNSSInfo.nIndex;
							TempNSSInfo.pNFTC = NowNSSInfo.pNFTC;
							b_HighListCross = nuTRUE;
							continue;
						}
					}
					if (NextNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS)
					{
						if ( NowNSSInfo.pNSS->TurnFlag <= T_Afore)
						{


							continue;
						}
					}
					if (NextNSSInfo.pNSS->RoadVoiceType != VRT_ROADCROSS && NowNSSInfo.pNSS->RoadVoiceType == VRT_ROADCROSS)
					{
						if (b_HighListCross && NowNSSInfo.pNSS->TurnFlag <= T_Afore)
						{
							NowNSSInfo.pNSS = TempNSSInfo.pNSS;
							NowNSSInfo.pNRBC = TempNSSInfo.pNRBC;
							NowNSSInfo.nIndex = TempNSSInfo.nIndex;
							NowNSSInfo.pNFTC = TempNSSInfo.pNFTC;
							b_HighListCross = nuFALSE;
							b_SameRoad = nuTRUE;
						}
						else if( NowNSSInfo.pNSS->TurnFlag <= T_Afore )
						{
							continue;
						}
					}
					nType = GetRoutingListCode( NowNSSInfo.pNSS,NextNSSInfo.pNSS);
				#else
				    nType = GetRoutingListCode( &(pNRBC->NSS[i]),NextNSSInfo.pNSS);
                #endif
				#ifdef SameRoad
					if (nType == AF_Out_Fastway)
					{
						if (NextNSSInfo.pNSS->RoadLength <= SAMEROADLEN  && pNRBC->NSS[i].RoadClass  == 4 &&
							pNRBC->NSS[i].RoadClass == NextNSSInfo.pNSS->RoadClass )
						{
							GetNextNSS(NextNSSInfo,&NSSInfo);
							if( NSSInfo.pNRBC == NULL || NSSInfo.pNFTC == NULL || NSSInfo.pNSS == NULL  )
							{
								return nuFALSE;
							}
							if (pNRBC->NSS[i].RoadClass == NSSInfo.pNSS->RoadClass)
							{
								nType = 0;
							}
						}
					}
					if (pNRBC->NSS[i].RoadLength <= m_nSameRoadLen  && 
					   (pNRBC->NSS[i].RoadClass == 6 || pNRBC->NSS[i].RoadClass == 7 || pNRBC->NSS[i].RoadClass == 9 ) && 
						pNRBC->NSS[i].RoadClass == NextNSSInfo.pNSS->RoadClass && pNRBC->NSS[i].TurnFlag <= 1 )
					{
						nuWCHAR  wsNowRoad[MAX_LISTNAME_NUM] = {0};//
						nuWCHAR  wsNextRoad[MAX_LISTNAME_NUM] = {0};//
						RNEXTEND RnEx = {0};
						g_pInFsApi->FS_GetRoadName( NextNSSInfo.pNRBC->FileIdx, NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, wsNextRoad, MAX_LISTNAME_NUM, &RnEx);
						g_pInFsApi->FS_GetRoadName( pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, wsNowRoad, MAX_LISTNAME_NUM, &RnEx);
						if (nuWcscmp(wsNowRoad, wsNextRoad) == 0)
						{
							if (nType == AF_IN_FASTROAD || nType == AF_IN_SLOWROAD)
							{
								nType = 0;
							}
						}
					}
				#endif
					nuBOOL bInOROutHighWay ;//=  ( (pNRBC->NSS[i].RoadClass <= 3) && (NextNSSInfo.pNSS->RoadClass <= 3) ) ? nuFALSE:nuTRUE;
					if (   pNRBC->NSS[i].RoadClass <= 3
						&& NextNSSInfo.pNSS->RoadClass <= 3 )
					{
						bInOROutHighWay = nuFALSE;
					}
					else
					{
						bInOROutHighWay = nuTRUE;
					}
					
					tmpnode.point	  = NextNSSInfo.pNSS->ArcPt[0];
					if( ChangeIcon(nType ) )
					{
						tmpnode.nIconType = nuWORD(1024 + nType);
					}
					else
					{
					#ifdef ZENRIN
						if( NowNSSInfo.pNSS->TurnFlag == T_No && nType == AF_LoseHere && NowNSSInfo.pNSS->Real3DPic != 0 )
					#else
						if(pNRBC->NSS[i].TurnFlag == T_No && nType == AF_LoseHere && NowNSSInfo.pNSS->Real3DPic != 0 )
				    #endif
						{
							//pNRBC->NSS[i].TurnFlag = T_Afore;
							tmpnode.nIconType = nuWORD(1024 + nType * 10 + T_Afore);
						}
					#ifdef ZENRIN
						tmpnode.nIconType = nuWORD(1024 + nType * 10 + NowNSSInfo.pNSS->TurnFlag);
					#else
						tmpnode.nIconType = nuWORD(1024 + nType * 10 + pNRBC->NSS[i].TurnFlag);
				    #endif
					}
					
					/*switch ( nType )
					{
					case AF_In_Tollgate:
					tmpnode.nIconType = 5024;
					break;
					
					}*/
				#ifdef VALUE_EMGRT
					if( NowNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
					{
						nTraffNum = 1;
						NAVINSS Temp1,Temp2;
						Temp1 = NextNSSInfo;
						while(1)
						{
 							if(!GetNextNSS(Temp1,&Temp2))
							{
								break;
							}
							if (   Temp1.pNSS == NULL
								|| Temp2.pNSS == NULL
								)
							{
								break;
							}
							if( Temp1.pNSS->RoadVoiceType == VRT_TrafficCircle  && Temp2.pNSS->RoadVoiceType == VRT_TrafficCircle)
							{
								nTraffNum++;
							}
							else
							{
								break;
							}
							Temp1 = Temp2;	
						}
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum ; 
					}
					else if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType != VRT_TrafficCircle )
					{
						tmpnode.nIconType -= (nuUSHORT)NowNSSInfo.pNSS->TurnFlag;
						tmpnode.nIconType += nTraffNum;
					}
                 #endif
					nuINT tmpType = 1;
					switch( tmpType/*g_pInFsApi->pGdata->drawInfoNavi.nRtListType */) 
					{
					case ROUTELIST_TYPE_CROSS:
						switch(nType) 
						{
						case AF_In_Freeway:
						case AF_Out_Freeway:
						case AF_In_Fastway:
						case AF_Out_Fastway:
						case AF_In_TrafficCircle:
						case AF_Out_TrafficCircle:
						case AF_In_Overpass:
						case AF_Out_Overpass:
						case AF_IN_SLOWROAD:
						case AF_OUT_SLOWROAD:
						case AF_In_SlipRoad:
						case AF_In_Tunnel:
						case AF_Out_Tunnel:
						case AF_In_Underpass:
						case AF_In_UnOverpass:
						case AF_In_Tollgate:
						case AF_In_RestStop:
						case AF_IN_FASTROAD:
						case AF_CityTownBoundary:
						case AF_MapBoundary:
						case AF_Out_SlipRoad:
						case AF_IN_AUXROAD:
						case AF_IN_MAINROAD:
							
							//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
							#ifdef SameRoad
								if (nType == AF_IN_FASTROAD || nType == AF_IN_SLOWROAD)
								{
									if (NowNSSInfo.pNSS->TurnFlag == T_LeftSide || NowNSSInfo.pNSS->TurnFlag == T_RightSide )
									{
										if (JudgeSameRoadName(NowNSSInfo, NextNSSInfo) 
							#ifdef ECI
											&& NowNSSInfo.pNSS->Real3DPic == 0
						    #endif
											)
										{//shyanx
											break;
										}
									}
								}
							#endif
								tmpnode.nDistance = nNodeLen;
								//GetRoadName(&(pNRBC->NSS[i]),tmpnode.wsName);
								RNEXTEND RnEx = {0};
								nuBOOL bSearch = nuTRUE;
								if ( NextNSSInfo.pNSS->PTSNameAddr > 4  )
								{
									bSearch = !GetPTSName((nuUSHORT)NextNSSInfo.pNRBC->FileIdx, NextNSSInfo.pNSS->PTSNameAddr, tmpnode.wsName , MAX_LISTNAME_NUM *sizeof(nuWCHAR));
								}
								if ( bSearch )
								{
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx,NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
								}
										
								AddListNode(tmpnode, bInOROutHighWay );
								if (  nType == AF_Out_Freeway )
								{
									m_StrHighWayLast.nDWIndex   = NowNSSInfo.pNRBC->FileIdx;
									m_StrHighWayLast.nBlkIndex  = NowNSSInfo.pNSS->BlockIdx;
									m_StrHighWayLast.nRdIndex   = NowNSSInfo.pNSS->RoadIdx;
									nuLONG nDx = 0 ,nDy = 0;
									nDx = NowNSSInfo.pNSS->ArcPt[1].x - NowNSSInfo.pNSS->ArcPt[0].x;
									nDy = NowNSSInfo.pNSS->ArcPt[1].y - NowNSSInfo.pNSS->ArcPt[0].y;
									m_StrHighWayLast.nAngle     = nulAtan2( nDy, nDx );
									m_StrHighWayLast.ptMap      = NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount - 1 ];
									m_StrHighWayLast.ptNextCoor = m_StrHighWayLast.ptMap;
									m_StrHighWayLast.bEndPt     = nuFALSE;
								}
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_MidSit:
							//	g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_EndSite:
							//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
							if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
							{
								tmpnode.nDistance = nNodeLen;
								g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
								AddListNode(tmpnode,bInOROutHighWay);
							}
							else
							{
								break;
							}
							nNodeLen = 0;
							break;
						case AF_LoseHere:
						#ifdef ZENRIN
							if( NowNSSInfo.pNSS->TurnFlag != T_No &&
						#else
						    if( pNRBC->NSS[i].TurnFlag != T_No &&
					    #endif
								 bJudgeHighWayCross( &NowNSSInfo, &NextNSSInfo ))
							{  
                            #ifdef ZENRIN
								if ( b_SameRoad && NowNSSInfo.pNSS->TurnFlag <= T_Afore )
								{
									break;
								}
                            #endif
							#ifdef SameRoad
								if(NowNSSInfo.pNSS->TurnFlag == T_Afore || NowNSSInfo.pNSS->TurnFlag == T_LeftSide || NowNSSInfo.pNSS->TurnFlag == T_RightSide )
								{
									if (JudgeSameRoadName(NowNSSInfo,NextNSSInfo)
							#ifdef ECI
										&& NowNSSInfo.pNSS->Real3DPic == 0
							#endif
										)
									{//shyanx
										break;
									}
								}
                            #endif
                           	#ifdef ZENRIN
								if( NowNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
							#else
							    if( pNRBC->NSS[i].RoadVoiceType == VRT_TrafficCircle && NextNSSInfo.pNSS->RoadVoiceType == VRT_TrafficCircle )
							#endif
								{
									break;
								}
								else
								{
									//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
									if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
									{
										tmpnode.nDistance = nNodeLen;
										RNEXTEND RnEx= {0};
										g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, NextNSSInfo.pNSS->RoadNameAddr, NextNSSInfo.pNSS->RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
										AddListNode(tmpnode,bInOROutHighWay);
										nNodeLen = 0;
									}
									else
									{
										break;
									}
								}	
							}
							break;
						default:
							break;
						}
						break;
						case ROUTELIST_TYPE_PASSNODE:
							switch(nType) 
							{
							case AF_Has_SlipRoad:
							case AF_Has_RestStop:
								//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
								if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
								{
									tmpnode.nDistance = nNodeLen;
									RNEXTEND RnEx = {0};
									g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
									AddListNode(tmpnode,bInOROutHighWay);
									nNodeLen = 0;
								}
								else
								{
									break;
								}
								nNodeLen = 0;
								break;
							default:
								break;
							}
							break;
							default:
								break;
					}
					if(    nType == AF_LoseHere
						&& pNRBC->NSS[i].RoadClass <= 3 
						&& NextNSSInfo.pNSS->RoadClass <= 3/**/ )
					{
						if ( NowNSSInfo.pNSS->CrossCount != 0 )
						{
                            nuBOOL bFindNCC = nuFALSE;
							nuUINT nFindIndex = 0;
							nuBOOL bStop = nuFALSE;
							nuUINT count = 0;
							for ( ; count < NowNSSInfo.pNSS->CrossCount; count++)
							{			
								if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NextNSSInfo.pNSS->BlockIdx 
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NextNSSInfo.pNSS->RoadIdx
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NextNSSInfo.pNSS->RoadNameAddr )
								{
									continue;
								}
								if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NowNSSInfo.pNSS->BlockIdx 
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NowNSSInfo.pNSS->RoadIdx
									&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NowNSSInfo.pNSS->RoadNameAddr )
								{
									continue;
								}
								if (   NowNSSInfo.pNSS->NCC[count].CrossClass == 5
									&& NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Freeway )
								{
									NowNSSInfo.pNSS->NCC[count].CrossVoiceType = VRT_SlipRoad;
								}
								if(    NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_SlipRoad
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_RestStop
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Tollgate
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_StopcockWay
									|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_GuideWay )
								{
									if (  NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_RestStop )
									{
										if ( bFindNCC )
										{
											if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx       == NowNSSInfo.pNSS->NCC[nFindIndex].CrossBlockIdx
												&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx        == NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadIdx
												&& NowNSSInfo.pNSS->NCC[count].CrossClass          == NowNSSInfo.pNSS->NCC[nFindIndex].CrossClass
												&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr   == NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr
												)
											{
												bStop = nuTRUE;
											}
										}
										else
										{
											bStop = nuTRUE;
										}	
									}
									/**/switch ( NowNSSInfo.pNSS->NCC[count].CrossVoiceType )
									{
										case VRT_SlipRoad:
											tmpnode.nIconType	= 5026;
											break;
										case VRT_RestStop:
											tmpnode.nIconType	= 5030;
											break;
									}
									
									tmpnode.point		= NowNSSInfo.pNSS->ArcPt[NowNSSInfo.pNSS->ArcPtCount -1 ];
									bFindNCC = nuTRUE;
									nFindIndex = count;
									if ( bStop )
									{
										break;
									}
								}	
							}
							if ( bFindNCC )
							{
								if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
								{
									tmpnode.nDistance	= nNodeLen;
									RNEXTEND RnEx = {0};
									if( NowNSSInfo.pNSS->NCC[nFindIndex].nFlag == 3)
									{
										nuTCHAR sFileName[256] = {0};
										nuTcscpy(sFileName, nuTEXT(".SPT"));
										if( g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sFileName, 256 ) )
										{
											CSPT SptFile;
											//char sTmpName[MAX_LISTNAME_NUM] = {0};
											if ( SptFile.GetNodeName( sFileName, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].nRoadNameLen, ((char *)tmpnode.wsName), MAX_LISTNAME_NUM * 2 ) )
											{
												//nuAsctoWcs( tmpnode.wsName, MAX_LISTNAME_NUM, sTmpName, MAX_LISTNAME_NUM );
											}
										}
									}
									else if ( NowNSSInfo.pNSS->NCC[nFindIndex].nFlag == 1 )
									{
										nuTCHAR sFileName[256] = {0};
										nuTcscpy(sFileName, nuTEXT(".MWP"));
										if( g_pInFsApi->FS_FindFileWholePath(pNRBC->FileIdx, sFileName, 256 ) )
										{
											CMWP MwpFile;
											//char sTmpName[MAX_LISTNAME_NUM] = {0};
											if ( MwpFile.GetNodeName( sFileName, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].nRoadNameLen, ((char *)tmpnode.wsName), MAX_LISTNAME_NUM * 2 ) )
											{
												//nuAsctoWcs( tmpnode.wsName, MAX_LISTNAME_NUM, sTmpName, MAX_LISTNAME_NUM );
											}
										}
									}
									else
									{
										g_pInFsApi->FS_GetRoadName( (nuUSHORT)pNRBC->FileIdx, NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr, NowNSSInfo.pNSS->NCC[nFindIndex].CrossClass, tmpnode.wsName, MAX_LISTNAME_NUM, &RnEx);
										if( NowNSSInfo.pNSS->NCC[nFindIndex].CrossRoadNameAddr  == NowNSSInfo.pNSS->RoadNameAddr )
										{
											AddNameStr( tmpnode.wsName );
										}

										
									}
									AddListNode(tmpnode,nuFALSE);
									nNodeLen = 0;
								}
							}
							
						}
					}
				}
			}
			else if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{

				if ( pNRBC->RTBStateFlag == 1  )
				{
					GetRTBName( pNRBC->FInfo.MapID, pNRBC->FInfo.RTBID, tmpnode.wsName );
				}
				else if ( pNRBC->RTBStateFlag == 2 )
				{
					nuWORD nMapCount = g_pInFsApi->FS_GetMapCount();
					if ( nMapCount > 1 )
					{
						PMAPNAMEINFO pTmp = (PMAPNAMEINFO)g_pInFsApi->FS_GetMpnData();
						if (   pTmp 
							&& pNRBC->FileIdx < nMapCount )
						{
							nuWcscat( tmpnode.wsName ,(pTmp + pNRBC->FileIdx )->wMapFriendName );
						}
					}

				}
				tmpnode.nDistance = nNodeLen;
				tmpnode.point     = pNRBC->FInfo.FixCoor;
				tmpnode.nIconType = 5107;
				AddListNode(tmpnode,nuFALSE);
				nNodeLen = pNRBC->RealDis;
				
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
		if( pNFTC != NULL )
		{
			//g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount++;
			if( (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum) && (g_pInFsApi->pGdata->drawInfoNavi.nRtListNowIdx <= g_pInFsApi->pGdata->drawInfoNavi.nTotalListCount -1  ) )
			{
				tmpnode.nDistance = nNodeLen;
				tmpnode.point     = pNFTC->FCoor;
				tmpnode.nIconType = nuWORD(1024 + AF_MidSit * 10);
				g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_PASS_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
				AddListNode(tmpnode,nuFALSE);
				nNodeLen = 0;
			}
			
		}
	}	
	/**/if(   g_pInFsApi->pGdata->drawInfoNavi.nRtListNowNum < g_pInFsApi->pGdata->drawInfoNavi.nRtListPageNum
	       && bEndInHighWay )
	{
		tmpnode.nDistance	= nNodeLen;
		tmpnode.nIconType	= 5109;
		PNAVIFROMTOCLASS pTmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];

		PNAVIFROMTOCLASS PTmpNFTC2 = NULL ;
		while( pTmpNFTC )
		{
			PTmpNFTC2 = pTmpNFTC;
			pTmpNFTC  = pTmpNFTC->NextFT;
		}
		if( PTmpNFTC2 )
		{
			tmpnode.point.x		= PTmpNFTC2->TCoor.x;
			tmpnode.point.y		= PTmpNFTC2->TCoor.y;
		}
		nuWCHAR wc[20];
		nuMemset( wc, 0, sizeof(wc) );
		g_pInFsApi->FS_ReadConstWcs(WCS_TYPE_DES_NAME, 0, (nuWCHAR*)tmpnode.wsName, MAX_LISTNAME_NUM);
		AddListNode(tmpnode,nuFALSE);
		nNodeLen = 0;
	}
	return nuTRUE;
}

nuBOOL CNaviData::ColHighWayCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo, nuLONG nLenToNextCross)
{
	if ( 1 != m_nGoNavi)
	{
		return nuFALSE;
	}
	if ( NowNSSInfo.pNFTC != NextNSSInfo.pNFTC )
	{
		return nuFALSE;
	}
	if ( m_struSeachInfo.bFindNextCross || m_struSeachInfo.nFindHighwayCross > 0 )
	{
		return nuFALSE;
	}
	if (   !NowNSSInfo.pNSS 
		|| !NextNSSInfo.pNSS 
		)
	{
		return nuFALSE;
	}
	g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance = 0;
	nuBOOL bFindNCC = nuFALSE;
	nuUINT nFindIndex = 0;
	nuUINT count = 0;
	nuLONG xx = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
#ifndef VALUE_EMGRT
	if (xx == AF_In_Tollgate)
	{
		bFindNCC = nuTRUE; 			
	}
	else if(xx == AF_In_Tunnel)
	{
		return nuFALSE;
	}
#endif
	if(NowNSSInfo.pNSS->CrossCount == 2 && NowNSSInfo.pNSS->RoadClass == 4 && NextNSSInfo.pNSS->RoadClass == 4 && NowNSSInfo.pNSS->RoadNameAddr != NextNSSInfo.pNSS->RoadNameAddr)
	{//special Road FastWay Sound Play Turn Flag
		return nuFALSE;
	}	
	if (   NowNSSInfo.pNSS->CrossCount <= 1 
		|| NowNSSInfo.pNSS->RoadClass > 3 
		|| NextNSSInfo.pNSS->RoadClass > 3 
		)
	{
		if ( !bFindNCC  )
		{
			/*if(NowNSSInfo.pNSS->RoadClass <= 4 && NextNSSInfo.pNSS->RoadClass > 4 && NowNSSInfo.pNSS->CrossCount <= 2)
		    {
				g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance = nLenToNextCross;
		    }*/
			return nuFALSE;
		}
		
	}
	
	
	
	for ( ; count < NowNSSInfo.pNSS->CrossCount; count++)
	{			
		if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NextNSSInfo.pNSS->BlockIdx 
			&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NextNSSInfo.pNSS->RoadIdx
			&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NextNSSInfo.pNSS->RoadNameAddr )
		{
			continue;
		}
		if (   NowNSSInfo.pNSS->NCC[count].CrossBlockIdx == NowNSSInfo.pNSS->BlockIdx 
			&& NowNSSInfo.pNSS->NCC[count].CrossRoadIdx  == NowNSSInfo.pNSS->RoadIdx
			&& NowNSSInfo.pNSS->NCC[count].CrossRoadNameAddr == NowNSSInfo.pNSS->RoadNameAddr )
		{
			continue;
		}
		if (   NowNSSInfo.pNSS->NCC[count].CrossClass == 5
			&& NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Freeway )
		{
			NowNSSInfo.pNSS->NCC[count].CrossVoiceType = VRT_SlipRoad;
		}
		if(    NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_SlipRoad
			|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_RestStop
			|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_Tollgate
			|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_StopcockWay
			|| NowNSSInfo.pNSS->NCC[count].CrossVoiceType == VRT_GuideWay )
		{
			bFindNCC = nuTRUE;
			nFindIndex = count;
			break;
		}	
	}
	if ( !bFindNCC )
	{
		return nuFALSE;
	}
	g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance = nLenToNextCross;
	g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.point     = NextNSSInfo.pNSS->ArcPt[0];
	nuBOOL bSearch = nuTRUE;
	RNEXTEND RnEx;
	if ( NextNSSInfo.pNSS->PTSNameAddr > 4  )
	{
		bSearch = !GetPTSName((nuUSHORT)NextNSSInfo.pNRBC->FileIdx, 
								NextNSSInfo.pNSS->PTSNameAddr, 
								g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.wsName,
								MAX_LISTNAME_NUM *sizeof(nuWCHAR));
	}
	if ( bSearch )
	{
		g_pInFsApi->FS_GetRoadName( (nuUSHORT)NextNSSInfo.pNRBC->FileIdx,
									NextNSSInfo.pNSS->RoadNameAddr, 
									NextNSSInfo.pNSS->RoadClass, 
									g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.wsName, 
									MAX_LISTNAME_NUM,
									&RnEx);
	}
	if (   g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDwIdx !=  NowNSSInfo.pNRBC->FileIdx 
		|| g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nBlkIdx !=  NowNSSInfo.pNSS->BlockIdx
		|| g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRoadIdx != NowNSSInfo.pNSS->RoadIdx
		)
	{
		g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDwIdx   =  NowNSSInfo.pNRBC->FileIdx;
	    g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nBlkIdx  =  NowNSSInfo.pNSS->BlockIdx;
		g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRoadIdx = NowNSSInfo.pNSS->RoadIdx;
		GetNaviArrowPoint( &NowNSSInfo, &g_pInFsApi->pGdata->drawInfoNavi.HighWayCross );
#ifdef ECI
		g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
#else
		if(xx == AF_In_Tollgate && NextNSSInfo.pNSS->RoadVoiceType == VRT_Tollgate)
		{
			//#ifdef VALUE_EMGRT
			g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = NextNSSInfo.pNSS->Real3DPic;
		    //#endif
		}
		else
		{
			//#ifdef VALUE_EMGRT
			g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
		    //#endif
		}
#endif
		Set3DPAfterWork(NowNSSInfo,  NextNSSInfo);
		g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nChangeCross = nuTRUE;
	}
	
	ColZoomCrossData(NowNSSInfo,NextNSSInfo) ;
     m_struSeachInfo.nFindHighwayCross = 1;
	return nuTRUE;
	
}

nuBOOL CNaviData::DrawArrow2(nuBYTE bZoom)
{
	if( g_pInFsApi->pGdata == NULL || g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	if ( g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance <= 0 )
	{
		return nuFALSE;
	}
	JudgeArrowNSS( bZoom );
	PNAVIFROMTOCLASS TmpNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];

	nuBYTE nDrawType = 0;
	NURORECT RectDraw = {0};
	nuBYTE n3DMode = 0;
	nuINT nDrawObj = 0;
	typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
	MT_MapToBmpProc pf_MapToBmp;
	nuLONG nSkipLen = 0, nLineWidth = 0;
	nuLONG nDrawArrowLen = 0;
	if ( m_nPreScal == 0xFF || m_nPreScal != g_pInFsApi->pGdata->uiSetData.nScaleIdx )
	{
		m_nPreScal = g_pInFsApi->pGdata->uiSetData.nScaleIdx;
	}
	if( bZoom == MAP_ZOOM )
	{
		if( !g_pInFsApi->pGdata->zoomScreenData.bZoomScreen )
		{
			return nuFALSE;
		}
		if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= 1 )
		{
			nLineWidth = m_nArrowLineWidth1;
		}
		else if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx == 2 )
		{
			nLineWidth = m_nArrowLineWidth2;
		}
		else
		{
			return nuFALSE;
			nLineWidth = 10;
		}
		nDrawType	= DRAW_TYPE_ZOOM_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->zoomScreenData.nuZoomMapSize;
		pf_MapToBmp = g_pInMtApi->MT_ZoomMapToBmp ;
		n3DMode		= g_pInFsApi->pGdata->zoomScreenData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
		if( n3DMode )
		{
			nDrawObj = DRAW_OBJ_ZOOM_3D_ARROWLINE;	
		}
		else
		{
			nDrawObj = DRAW_OBJ_ZOOM_2D_ARROWLINE;	
		}
		nDrawArrowLen = ZOOMLEN * g_pInFsApi->pMapCfg->zoomSetting.scaleTitle.nScale / SCALEBASE; 
	}
	else
	{
		if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx <= 1 )
		{
			nLineWidth = m_nArrowLineWidth1;
		}
		else if ( g_pInFsApi->pGdata->uiSetData.nScaleIdx == 2 )
		{
			nLineWidth = m_nArrowLineWidth2;
		}
		else
		{
			return nuFALSE;
			nLineWidth = 8;
		}
		nDrawType	= DRAW_TYPE_NAVI_ROAD;
		RectDraw	= g_pInFsApi->pGdata->transfData.nuShowMapSize;	
		pf_MapToBmp = g_pInMtApi->MT_MapToBmp;
		n3DMode		= g_pInFsApi->pGdata->uiSetData.b3DMode;
		nSkipLen	= g_pInFsApi->pMapCfg->naviLineSetting.nRouteLineWidth * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / g_pInFsApi->pDrawInfo->pcDrawInfoMap->nScalePixels;//SCALETOSCREEN_LEN;
		if( n3DMode )
		{
			nDrawObj = DRAW_OBJ_3D_ARROWLINE;	
		}
		else
		{
			nDrawObj = DRAW_OBJ_2D_ARROWLINE;	
		}
		nDrawArrowLen = GENLEN * g_pInFsApi->pMapCfg->scaleSetting.scaleTitle.nScale / SCALEBASE;
	}

	nuLONG dLen = 0;
	nuLONG nLenToNextCross   = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nDistance;
	nuLONG nLenToBeforeCross = m_nNowLen - nLenToNextCross;
	if(    m_nNowLen == 0
		|| ( nLenToBeforeCross > 100 || nLenToNextCross < 100
	    )
		 )
	{
		CROSSPOINT TmpCross = {0};
		TmpCross.ptArray = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.ptCrossList;
		TmpCross.nPtNum = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nCrossPtCount;
		TmpCross.nCenterIndex = g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nIdxCrossInList;
		SelPt( &TmpCross,
			&m_CrossPoint,  
			ARROWLEN );		
	}
	NUROPOINT ptDrawTmp[CROSS_ARROW_POINT_NUM] = {0};
	CROSSPOINT TmpCross = {0};
	TmpCross.ptArray = &ptDrawTmp[0];

	SelPt( &m_CrossPoint,
		&TmpCross,  
		GetArrowLen(),
		nuFALSE);

    if( !nuPointInRect( &TmpCross.ptArray[TmpCross.nCenterIndex], &(RectDraw)) )
	{
		return nuFALSE;
	}
	for ( nuINT nIndex = 0; nIndex < TmpCross.nPtNum ; nIndex++ )
	{ 
		pf_MapToBmp(TmpCross.ptArray[nIndex].x, TmpCross.ptArray[nIndex].y, &TmpCross.ptArray[nIndex].x, &TmpCross.ptArray[nIndex].y);
	}
	g_pInMtApi->MT_DrawObject(nDrawObj, TmpCross.ptArray, TmpCross.nPtNum , nLineWidth);
    return nuTRUE;
}

nuBOOL CNaviData::GetCarInfoForCCD(PCCDINFO pCcdInfo)
{
	if ( pCcdInfo == NULL )
	{
		return nuFALSE;
	}
	if (   pCcdInfo->nCarMapIdx  != m_ptNavi.pNRBC->FileIdx 
		|| pCcdInfo->nCarBlkIdx  != m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].BlockIdx
		|| pCcdInfo->nCarRdIdx   != m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadIdx
		)
	{
			return nuFALSE;
	}
	
	nuLONG nDx = 0, nDy = 0, nAddDis = 0;
	nDx = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
	nDy = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
	nAddDis = nuSqrt( nDx * nDx + nDy * nDy );
	//PNAVISUBCLASS pNowNSS = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
	NAVINSS NowNSS = {0};
	NowNSS.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
	NowNSS.pNRBC  = m_ptNavi.pNRBC;
	NowNSS.pNFTC  = m_ptNavi.pNFTC;
	NowNSS.nIndex = m_ptNavi.nSubIdx;
	nuLONG nPtIndex =  0;
	for( nPtIndex = m_ptNavi.nPtIdx + 1; nPtIndex < m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPtCount - 1; nPtIndex++ )
	{
		nDx = NowNSS.pNSS->ArcPt[nPtIndex].x - NowNSS.pNSS->ArcPt[nPtIndex + 1].x;
		nDy = NowNSS.pNSS->ArcPt[nPtIndex].y - NowNSS.pNSS->ArcPt[nPtIndex + 1].y;
		nAddDis += nuSqrt( nDx * nDx + nDy * nDy );
	}
	pCcdInfo->nCcdRdCount = 0;
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].ptFirst = pCcdInfo->ptCarMapped;
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].ptEnd   = pCcdInfo->ptCarNextCoor;
    pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdMapIdx = NowNSS.pNRBC->FileIdx;
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdBlkIdx = NowNSS.pNSS->BlockIdx;
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdRdIdx  = NowNSS.pNSS->RoadIdx;
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nOneWay    = NowNSS.pNSS->nOneWay;	
	pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nRoadClass = NowNSS.pNSS->RoadClass;	
	++pCcdInfo->nCcdRdCount;
    while( nAddDis <= _CCD_CHECK_MAX_DIS )
	{
		NAVINSS NextNSS;
		if ( !GetNextNSS( NowNSS, &NextNSS ))
		{
			break;
		}
		nuMemcpy( &NowNSS, &NextNSS, sizeof(NowNSS));
		
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].ptFirst = NowNSS.pNSS->ArcPt[0];
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].ptEnd   = NowNSS.pNSS->ArcPt[NowNSS.pNSS->ArcPtCount - 1];
		
        pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdMapIdx = NowNSS.pNRBC->FileIdx;
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdBlkIdx = NowNSS.pNSS->BlockIdx;
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nCcdRdIdx  = NowNSS.pNSS->RoadIdx;
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nOneWay    = NowNSS.pNSS->nOneWay;	
		pCcdInfo->pCcdRoad[pCcdInfo->nCcdRdCount].nRoadClass = NowNSS.pNSS->RoadClass;	
		++pCcdInfo->nCcdRdCount;
		if ( pCcdInfo->nCcdRdCount >= _CCD_CHECK_MAX_ROADS )
		{
			break;
		}
		
		nAddDis += NowNSS.pNSS->RoadLength;
	}
	return nuTRUE;
}

nuLONG CNaviData::GetArrowLen()
{
	NUROPOINT pt1 = {0};
	NUROPOINT pt2 = {0};
	pt2.x = m_nBmpArrowLen;
	g_pInMtApi->MT_BmpToMap(pt1.x, pt1.y, &pt1.x, &pt1.y );
	g_pInMtApi->MT_BmpToMap(pt2.x, pt2.y, &pt2.x, &pt2.y );
	return (nuLONG)nuSqrt( (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y) );
	

}

nuLONG CNaviData::GetRoutingListCodeEMG(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS)
{
	nuLONG nNowVoiceType = 0;
	nuLONG nNextVoiceType = 0;
	if ( pNowNSS->RoadClass == 5 && pNowNSS->RoadVoiceType != VRT_Tollgate )
	{
		pNowNSS->RoadVoiceType = VRT_SlipRoad;
	}
	if ( pNowNSS->RoadClass <= 3  && VRT_Fastway == pNowNSS->RoadVoiceType )
	{
	//	pNowNSS->RoadVoiceType = VRT_Freeway;
	}
	if ( pNextNSS->RoadClass == 5 && pNextNSS->RoadVoiceType != VRT_Tollgate )
	{
		pNextNSS->RoadVoiceType = VRT_SlipRoad;
	}
    if ( pNextNSS->RoadClass <= 3 && VRT_Fastway == pNextNSS->RoadVoiceType)
	{
	//	pNextNSS->RoadVoiceType = VRT_Freeway;
	}
	if ( pNowNSS->PTSNameAddr > 4 )
	{
		pNowNSS->RoadVoiceType = VRT_Tollgate;
	}
	if ( pNextNSS->PTSNameAddr > 4 )
	{
		pNextNSS->RoadVoiceType = VRT_Tollgate;
	}

	nNowVoiceType	 = pNowNSS->RoadVoiceType;
	nNextVoiceType	 = pNextNSS->RoadVoiceType;
	if( pNextNSS->RoadClass == 5 && pNowNSS->RoadClass <= 3 )
	{
		return AF_Out_Freeway;;
	}
	
	switch(nNowVoiceType)
	{
		case VRT_Tunnel:
		case VRT_Underpass:
		case VRT_UnOverpass:
		case VRT_Tollgate:
		case VRT_RestStop:
			return AF_LoseHere;	
	}
	switch(nNextVoiceType)
	{
		case VRT_CityTownBoundary:
			return AF_CityTownBoundary;
		case VRT_MapBoundary:	
			return AF_MapBoundary;
		case VRT_MidSite:		
			return AF_MidSit;
		case VRT_EndSite:		
			return AF_EndSite;
	}
	
	if (   nNowVoiceType != nNextVoiceType
		&& (  (nNowVoiceType != VRT_Defway || nNextVoiceType != VRT_Defway  )
		     || pNowNSS->RoadClass != pNextNSS->RoadClass )
		&& 	nNowVoiceType != VRT_ROADCROSS 
		&& nNextVoiceType != VRT_ROADCROSS )
	{
		if(nNowVoiceType==VRT_TrafficCircle)		
		{
			return AF_Out_TrafficCircle;
		}
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_In_TrafficCircle;
		}
		if ( nNowVoiceType == VRT_UTURN )
		{
			pNowNSS->TurnFlag = T_No;
			return AF_LoseHere;
		}
		if ( nNextVoiceType == VRT_UTURN )
		{
			return AF_IN_UTURN;
		}
		if ( nNextVoiceType == VRT_RTURN )
		{
			return AF_IN_RTURN;
		}
		if ( nNextVoiceType == VRT_LTURN )
		{
			return AF_IN_LTURN;
		}
		if ( pNextNSS->RoadClass == 9 && VRT_SlowWay == nNextVoiceType )
		{
			return AF_IN_AUXROAD;
		}
		if ( pNowNSS->RoadClass == 9 
			&& VRT_SlowWay == nNowVoiceType )
		{
			if(   pNowNSS->RoadNameAddr == pNextNSS->RoadNameAddr 
				&& pNextNSS->RoadClass < pNowNSS->RoadClass )
			{
				return AF_IN_MAINROAD;
			}
		}
		
		if ( nNextVoiceType == VRT_FERRY )
		{
			return AF_IN_FERRY;
		}
		if(nNextVoiceType==VRT_Tollgate)			
		{
			return AF_In_Tollgate;
		}
		if(nNextVoiceType==VRT_RestStop)	
		{
			return AF_In_RestStop;
		}
		if(nNextVoiceType==VRT_Tunnel)	
		{
			return AF_In_Tunnel;
		}
		if( nNextVoiceType == VRT_Fastway && pNowNSS->RoadClass != pNextNSS->RoadClass)
		{
			return AF_In_Fastway;
		}
		if( nNextVoiceType == VRT_Freeway && pNowNSS->RoadClass != pNextNSS->RoadClass)
		{
			return AF_In_Freeway;
		}
		if( nNextVoiceType == VRT_Overpass && nNowVoiceType == VRT_SlipRoad )
		{
			return AF_In_Overpass;
		}
		if(nNowVoiceType == VRT_Freeway && pNowNSS->RoadClass != pNextNSS->RoadClass)
		{
			return AF_Out_Freeway;
		}
		if(nNextVoiceType==VRT_Freeway)
		{
			return AF_LoseHere;
		}	
		
		if(nNowVoiceType == VRT_Overpass && pNowNSS->RoadClass != pNextNSS->RoadClass)
		{
			return AF_Out_Overpass;
		}
		if(nNextVoiceType == VRT_Overpass && pNowNSS->RoadClass != pNextNSS->RoadClass)	
		{
			return AF_LoseHere;
		}
		if(nNextVoiceType==VRT_SlipRoad)
		{
			if ( pNowNSS->RoadClass <= 3)
			{
				return AF_Out_Freeway;
			}
			/*else if( pNowNSS->CrossCount > 1 )
			{
				return AF_In_SlipRoad;
			}*/
		}

		if(nNextVoiceType==VRT_Underpass)
		{
			return AF_In_Underpass;
		}
		if(nNextVoiceType==VRT_UnOverpass)
		{
			return AF_In_UnOverpass;
		}
		
	}
	else
	{
		if(nNextVoiceType==VRT_TrafficCircle)
		{
			return AF_LoseHere;//In TrafficCircle, ignore the road crossing
		}
	}

	if( pNowNSS->TurnFlag == T_No && pNowNSS->RoadNameAddr != pNextNSS->RoadNameAddr)
	{
		return AF_LoseHere;//AF_ChangeRoadName;
	}
	return AF_LoseHere;
}
nuBOOL CNaviData::AddExternStr(nuLONG nChangeCode,nuWCHAR *wsSource,nuINT nStrLen)
{
	if( nChangeCode == AF_In_SlipRoad )
	{
		nuWCHAR StrTmp[3] = { 21277,36947 }; //?é?
		nuINT nSourceLen = nuWcslen( wsSource );
		if ( nuWcslen(StrTmp) >= nStrLen - nSourceLen )
		{
			return nuFALSE;
		}
		nuWcscat( wsSource, StrTmp);
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CNaviData::FixOverpassRoad( PNAVINSS pNowNSS, PNAVINSS pNextNSS)
{
    if (   pNowNSS->pNSS
		&& pNowNSS->pNSS->RoadVoiceType == VRT_Overpass 
		&& pNowNSS->pNSS->CrossCount == 1
		&& pNextNSS->pNRBC != NULL
		&& pNextNSS->pNSS
		&& pNextNSS->pNSS->RoadVoiceType != VRT_Overpass )
    {
		NAVINSS NextNSSTmp = {0},NowNSSTmp;
		nuBOOL bFind = nuFALSE;
		nuMemcpy( &NowNSSTmp, pNowNSS, sizeof(NowNSSTmp));
		nuLONG nIndex = 0;
		while(1)
		{
			if( nuFALSE == GetNextNSS( NowNSSTmp, &NextNSSTmp ))
			{
				return nuFALSE;
			}
			if ( NextNSSTmp.pNSS->CrossCount > 1 )
			{
				break;
			}
			if( NextNSSTmp.pNSS->RoadVoiceType == VRT_Overpass )
			{
				bFind = nuTRUE;
				break;
			}
			++nIndex;
			nuMemcpy( &NowNSSTmp, &NextNSSTmp, sizeof(NextNSSTmp));
		}
		if( bFind )
		{
			nuMemcpy( &NowNSSTmp, pNextNSS, sizeof(NowNSSTmp));
			while( nIndex  )
			{
				NowNSSTmp.pNSS->RoadVoiceType = VRT_Overpass;
				//NowNSSTmp.pNSS->RoadClass = 4;
				GetNextNSS( NowNSSTmp,&NextNSSTmp);
				nuMemcpy( &NowNSSTmp, &NextNSSTmp, sizeof(NowNSSTmp));
				--nIndex;
			}
		}
		return nuTRUE;
    }
	return nuFALSE;
}

nuBOOL CNaviData::bRestNaviData2()
{
	m_nFristTotalLen = 0;
	m_nFristTotalTime = 0;
	m_bFirstGetMRouteData = nuFALSE; 
	m_bFirstCalDis    = nuTRUE;
	m_PtRouteEndMax.x = m_PtRouteEndMax.y = 0;
	m_PtRouteEndMin.x = m_PtRouteEndMin.y = 20000000;
	return nuTRUE;
}

nuBOOL CNaviData::ChangeIcon(nuLONG &nIcon)
{
	if( nIcon == AF_IN_UTURN )
	{
		nIcon = T_LUturn;
		return nuTRUE;
	}
	else if ( nIcon == AF_IN_LTURN )
	{
		nIcon = T_Left;
		return nuTRUE;
	}
	else if( nIcon == AF_IN_RTURN )
	{
		nIcon = T_Right;
		return nuTRUE;
	}
	return nuFALSE;
}
nuVOID CNaviData::CalMinAndMaxCoor_D(NUROPOINT point)
{//added bu daniel 
	if(m_PtRouteEndMin.x > point.x)
	{
		m_PtRouteEndMin.x = point.x;
	}
	if(m_PtRouteEndMin.y > point.y)
	{
		m_PtRouteEndMin.y = point.y;
	}
	if(m_PtRouteEndMax.x < point.x)
	{
		m_PtRouteEndMax.x = point.x;
	}
	if(m_PtRouteEndMax.y < point.y)
	{
		m_PtRouteEndMax.y = point.y;
	}
}
nuVOID CNaviData::SoundAfterWork(nuWCHAR * wsSoundName, NAVINSS* pNowNSSInfo)
{
	nuBYTE k = 0;
	for(; k < MAX_LISTNAME_NUM; k++)
	{
		if(wsSoundName[k] == 0x003B)//";"åŸé¢?šæ¿??
		{
			wsSoundName[k] = '\0';
			break;
		}
	}
	NAVINSS	 NowNSS = {0}, NextNSS = {0};
	if(pNowNSSInfo == NULL)
	{
		NowNSS.pNFTC  = m_ptNavi.pNFTC;
		NowNSS.pNRBC  = m_ptNavi.pNRBC;
		NowNSS.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
		NowNSS.nIndex = m_ptNavi.nSubIdx;
	}
	else
	{
		nuMemcpy(&NowNSS, pNowNSSInfo, sizeof(NAVINSS));
	}
	if(NowNSS.pNSS->RoadClass != 5)

	{
		nuLONG NSSIndex = m_ptNavi.nSubIdx;
		nuLONG NextIndex = m_ptNavi.nSubIdx + 1;
		GetNextNSS(NowNSS , &NextNSS);
		if(NextNSS.pNSS != NULL )//&& NextNSS.pNSS->RoadClass == 5
		{	
			nuBYTE i = 0;
			for(; i < MAX_LISTNAME_NUM; i++)
			{
				if(wsSoundName[i] == 0x007C)//|ä»¥å??å»??
				{
					wsSoundName[i] = '\0';
					break;
				}
			}
		}
		else
		{
			return ;
		}
	}
	else if(NowNSS.pNSS->RoadClass == 5)
	{
		nuWCHAR wsTempName[MAX_LISTNAME_NUM] = {0};
		nuBYTE i = 0, j = 0;
		nuBOOL bSave = nuFALSE;
		for(; i < MAX_LISTNAME_NUM; i++)
		{
			if(wsSoundName[i] == 0x0028)//()?§ç?ä¿ç? 
			{
				bSave = nuTRUE;
				i++;
			}
			if(bSave && wsSoundName[i] != 0x0029)
			{
				wsTempName[j] = wsSoundName[i];
				j++;				
			}
			else if(wsSoundName[i] == 0x0029)
			{
				wsTempName[j] = 0x65B9;//??
				j++;
				wsTempName[j] = 0x5411;//??
				bSave = nuFALSE;
				nuMemcpy(wsSoundName, wsTempName, sizeof(nuWCHAR) * MAX_LISTNAME_NUM);
				return ;
			}
		}
	}
}
#ifdef LCMM
	nuVOID CNaviData::EstimateTimeAndDistance(nuLONG *TargetDis, nuLONG *TargetTime)
	{
		//Louis new add change path
		nuLONG i = 0;
		if(TargetDis==NULL) return;
		if(TargetTime==NULL) return;
		nuMemset(TargetDis , 0, sizeof(nuLONG) * g_pInFsApi->pGdata->routeGlobal.RouteCount);
		nuMemset(TargetTime , 0, sizeof(nuLONG) * g_pInFsApi->pGdata->routeGlobal.RouteCount);
		for(i = 0; i < g_pInFsApi->pGdata->routeGlobal.RouteCount; i++)
		{
			EstimateTimeAndDistanceLU(TargetDis[i], TargetTime[i],i);
		}
		return;
/*
		nuLONG RouteCounter_LCMM = 0, i = 0;
		nuMemset(TargetDis , 0, sizeof(nuLONG) * g_pInFsApi->pGdata->routeGlobal.RouteCount);
		nuMemset(TargetTime , 0, sizeof(nuLONG) * g_pInFsApi->pGdata->routeGlobal.RouteCount);
		PNAVIFROMTOCLASS pNFTC = NULL;
		PNAVIRTBLOCKCLASS pNRBC = NULL;
		
		for(RouteCounter_LCMM = 0; RouteCounter_LCMM < g_pInFsApi->pGdata->routeGlobal.RouteCount; RouteCounter_LCMM++)
		{
			if(g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[RouteCounter_LCMM] != NULL)
			{
				pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[RouteCounter_LCMM];
			}
			while(pNFTC != NULL)
			{
				pNRBC = pNFTC->NRBC;
				while(pNRBC != NULL)
				{
					TargetDis[RouteCounter_LCMM] += pNRBC->RealDis;
					pNRBC = pNRBC->NextRTB;
				}
				pNFTC = pNFTC->NextFT;
			}
			if(m_nNowSpeed <= 0)
			{
				m_nNowSpeed = 1;
			}
			TargetTime[RouteCounter_LCMM] = TargetDis[RouteCounter_LCMM] * 36 / (m_nNowSpeed * 10);
		}
*/
	}

	nuVOID CNaviData::EstimateTimeAndDistanceLU(nuLONG &TargetDis, nuLONG &TargetTime,nuLONG TargetIdx)
	{
		nuLONG RouteCounter_LCMM = 0, i = 0;
		TargetDis	=	0;
		TargetTime	=	0;
		PNAVIFROMTOCLASS pNFTC = NULL;
		PNAVIRTBLOCKCLASS pNRBC = NULL;
        nuLONG nSpeed = 0;
		
	/*	if(g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[TargetIdx] != NULL)
		{
			pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[TargetIdx];
		}
		while(pNFTC != NULL)
		{
			pNRBC = pNFTC->NRBC;
			while(pNRBC != NULL)
			{
				TargetDis += pNRBC->RealDis;
				pNRBC = pNRBC->NextRTB;
			}
			pNFTC = pNFTC->NextFT;
		}
		if(m_nNowSpeed <= 0)
		{
			m_nNowSpeed = 1;
		}
		TargetTime = TargetDis * 36 / (m_nNowSpeed * 10);*/
		//---------------By shyanx -Three Path Route Time and Distance Calculate Method---------------//
		if(g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[TargetIdx] != NULL)
		{
			pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[TargetIdx];
		}
		while(pNFTC != NULL)
		{
			pNRBC = pNFTC->NRBC;
			while(pNRBC != NULL)
			{
				if( pNRBC == g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[TargetIdx]->NRBC)
				{
					while( i < pNRBC->NSSCount)
				    {
						nSpeed = (nuLONG)((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed)*0.2778);
					    TargetDis   += pNRBC->NSS[i].RoadLength;
					    TargetTime  += pNRBC->NSS[i].RoadLength/nSpeed;
					    i++;
				    }
				}
				else
				{
					if ( pNRBC->NSSCount == 0 )
				    {
						nSpeed = (nuLONG)( (m_nSlowSpeed + m_nHighSpeed) / 2 *0.2778); 
					    TargetDis   += pNRBC->RealDis;
					    TargetTime  += pNRBC->RealDis/nSpeed;
				    }
				    else
				    {
						if ( pNRBC->RTBStateFlag == 0 )
					    {
						   i = 0;
						   while( i < (nuLONG)pNRBC->NSSCount && &(pNRBC->NSS[i] ) )
						   {
							   nSpeed = (nuLONG)((( pNRBC->NSS[i].RoadClass <= 4)?m_nHighSpeed:m_nSlowSpeed)*0.2778);
						       TargetDis   += pNRBC->NSS[i].RoadLength;
						       TargetTime  += pNRBC->NSS[i].RoadLength/nSpeed;
							   i++;
						   }
					    }
					}
				}
				pNRBC = pNRBC->NextRTB;
			}
			pNFTC = pNFTC->NextFT;
		}
		//---------------------------------------------------------------------------
	}

	nuVOID CNaviData::ChooseRouteLineth(nuLONG SelectRouteLineth)
	{
		if(!m_bSelectRouteRule)
		{
			m_bSelectRouteRule = nuTRUE;
			m_lChoosePath = SelectRouteLineth;
			nuLONG RouteCounter_LCMM = 0;
			while(RouteCounter_LCMM < g_pInFsApi->pGdata->routeGlobal.RouteCount)
			{
				if(SelectRouteLineth == RouteCounter_LCMM)
				{
					RouteCounter_LCMM++;
					continue;
				}
				g_pInRTApi->RT_ReleaseNFTC( g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[RouteCounter_LCMM]);
				g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[RouteCounter_LCMM] = NULL;
				RouteCounter_LCMM++;
			}
			if(SelectRouteLineth != 0)
			{
				g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0] = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[SelectRouteLineth];
				g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[SelectRouteLineth] = NULL;
			}
			if(g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0] != NULL)
			{
				m_ptSimu.pNFTC	=  m_ptNavi.pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
				m_ptSimu.pNRBC	=  m_ptNavi.pNRBC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]->NRBC;
				m_ptSimu.nSubIdx = m_ptNavi.nSubIdx	= 0;
				m_ptSimu.nPtIdx	 = m_ptNavi.nPtIdx	= 0;
			}
			
			g_pInFsApi->pGdata->routeGlobal.RouteCount = 1;

		}	
	}
	
	nuVOID CNaviData::DisplayRoutePath(nuLONG SelectRoutePath)
	{
		nuLONG TargetDis,TargetTime;
		m_lChoosePath = SelectRoutePath;
		EstimateTimeAndDistanceLU(TargetDis, TargetTime,SelectRoutePath);
		g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget=TargetDis;
	}

	nuLONG CNaviData::TMCRoute(TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount, TARTGETINFO NewStart, nuLONG RouteRule)
	{
		/*StructTMCPassData TMCData[4];
		TMCData[0].TMCPass = 1;
		TMCData[0].TMCLocationAddr = 161152;
		TMCData[0].TMCNodeCount = 11;
		TMCData[0].TMCRouteTime = 10;
		TMCData[0].TMCRouteWeighting = -1;
		TMCData[1].TMCPass = 1;
		TMCData[1].TMCLocationAddr = 161328;
		TMCData[1].TMCNodeCount = 38;
		TMCData[1].TMCRouteTime = 10;
		TMCData[1].TMCRouteWeighting = -1;
		TMCData[2].TMCPass = 1;
		TMCData[2].TMCLocationAddr = 161936;
		TMCData[2].TMCNodeCount = 37;
		TMCData[2].TMCRouteTime = 10;
		TMCData[2].TMCRouteWeighting = -1;
		TMCData[3].TMCPass = 1;
		TMCData[3].TMCLocationAddr = 162528;
		TMCData[3].TMCNodeCount = 19;
		TMCData[3].TMCRouteTime = 10;
		TMCData[3].TMCRouteWeighting = -1;
		if( !pTMCData )
		{
			return 0;
		}*/
		//return 0;
		NewStart.FixCoor.x = m_ptNavi.ptMapped.x;
		NewStart.FixCoor.y = m_ptNavi.ptMapped.y;
		__android_log_print(ANDROID_LOG_INFO, "TMC", "InNavi TMC ROUTE start!!!!");
		return g_pInRTApi->RT_TMCRoute( m_ptNavi.pNFTC, m_ptNavi.pNRBC, NewStart, pTMCData, TMCDataCount, RouteRule);	
	}

	nuVOID CNaviData::JudegeLaneGuideInfo()
	{
		//nuLONG NextTurnFlag = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].TurnFlag;
		nuLONG NextTurnFlag = 0;
		if((m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].BlockIdx != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNSS->BlockIdx 
			&& m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadIdx != m_struSeachInfo.SoundPlayFirst.NaviInfo.pNSS->RoadIdx)
			|| NURO_ABS(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].TurnAngle) < 4)
		{
			NextTurnFlag = 0;
		}
		else
		{
			NextTurnFlag = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].TurnFlag;
		}
		
		if(NextTurnFlag == 0)
		{
			nuINT i = m_ptNavi.nSubIdx + 1;
			nuBYTE NextRoadCount = 0;
			nuBOOL bExitjudge   = nuFALSE;
			PNAVIFROMTOCLASS pNFTC = m_ptNavi.pNFTC; 
			PNAVIRTBLOCKCLASS pNRBC = m_ptNavi.pNRBC;
			NAVISUBCLASS	  NSS  = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx];
			while(NULL != pNFTC && !bExitjudge)
			{
				//pNRBC = pNFTC->NRBC;
				while(NULL != pNRBC && !bExitjudge)
				{
					if(pNRBC->RTBStateFlag != 0)
					{
						return;
					}
					for(; i < pNRBC->NSSCount; i++)
					{
						NSS = pNRBC->NSS[i];
						if( NSS.TurnFlag > 1 &&  NSS.RoadVoiceType == VRT_ROADCROSS)
						{
							NextTurnFlag = NSS.TurnFlag;
						}
						NextRoadCount++;
						if(NextRoadCount == 2)
						{
							bExitjudge = nuTRUE;
							break;
						}
					}
					pNRBC = pNRBC->NextRTB;
				}
				pNFTC = pNFTC->NextFT;
			}
		}
		
		nuroPOINT NowRoad_StartCoor, NowRoad_EndCoor;
		nuMemcpy(&NowRoad_StartCoor, &m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].StartCoor, sizeof(nuroPOINT));
		nuMemcpy(&NowRoad_EndCoor, &m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].EndCoor, sizeof(nuroPOINT));
		
		nuTCHAR filename[260] = {0};
		nuTcscpy(filename, nuTEXT(".LG"));
		if(g_pInFsApi->FS_FindFileWholePath(m_ptNavi.pNRBC->FileIdx, filename, NURO_MAX_PATH) )
		{
			g_pInFsApi->pGdata->drawInfoNavi.LaneGuide.LaneCount = 0;
			nuMemset(g_pInFsApi->pGdata->drawInfoNavi.LaneGuide.bLight, 0, sizeof(nuBOOL) * MAX_LANE_COUNTS);
			nuMemset(g_pInFsApi->pGdata->drawInfoNavi.LaneGuide.LightNum, 0, sizeof(nuBYTE) * MAX_LANE_COUNTS);
			m_CLaneGuide.Get_LaneGuideInfo(filename, NowRoad_StartCoor, NowRoad_EndCoor, 
				NextTurnFlag, &g_pInFsApi->pGdata->drawInfoNavi.LaneGuide);
		}
	}
	nuVOID CNaviData::Set3DPAfterWork(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo)
	{
		/*NAVINSS	 NowNSSInfo, NextNSSInfo;
		NowNSSInfo.pNFTC  = m_ptNavi.pNFTC;
		NowNSSInfo.pNRBC  = m_ptNavi.pNRBC;
		NowNSSInfo.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
		NowNSSInfo.nIndex = m_ptNavi.nSubIdx;
		GetNextNSS(NowNSSInfo , &NextNSSInfo);*/
		//if( NULL == NextNSSInfo.pNSS)
		{
			return;
		}
		if(NowNSSInfo.pNSS->RoadVoiceType <= 4 && 
			NowNSSInfo.pNSS->RoadVoiceType != 0 &&
			NowNSSInfo.pNSS->RoadVoiceType != 3)
		{
			if(NextNSSInfo.pNSS->RoadVoiceType <= 4 && 
				NextNSSInfo.pNSS->RoadVoiceType != 0 &&
				NextNSSInfo.pNSS->RoadVoiceType != 3)
			{
				if(NowNSSInfo.pNSS->RoadVoiceType != 20)
				{
					if(NowNSSInfo.pNSS->Real3DPic == 0 && 
						NextNSSInfo.pNSS->RoadLength <= IN_REAL3DPIC && 
						NextNSSInfo.pNSS->RoadVoiceType != 20)
					{
						g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID = NextNSSInfo.pNSS->Real3DPic;
					}
					else
					{
						g_pInFsApi->pGdata->drawInfoNavi.nIgnorePicID = NowNSSInfo.pNSS->Real3DPic;
					}
				}
			}
		}
	}
	nuLONG CNaviData::GetTurnFlagNum(nuLONG VoiceType,PNAVISUBCLASS NowNSSInfo)
	{
		switch(VoiceType)
		{
		case AF_LoseHere:
			if(NowNSSInfo->TurnFlag != 0)
			{
				return NowNSSInfo->TurnFlag;
			}
			else
			{
				return T_Afore;
			}
			break;
		case AF_In_Underpass:
		case AF_In_Fastway:
		case AF_In_Overpass:
		case AF_In_Freeway:
		case AF_In_Tunnel:
		case AF_Out_Underpass:
		case AF_Out_Tunnel:
		case AF_In_UnOverpass:
		case AF_In_TrafficCircle:
			return T_Afore;
			break;
		case AF_Out_TrafficCircle:
		case AF_In_SlipRoad:
		case AF_Out_SlipRoad:
			if (NowNSSInfo->TurnFlag == 0)
			{
				return T_Afore;
			}
			else
			{
				return NowNSSInfo->TurnFlag;
			}
			break;
		case AF_IN_FASTROAD:
		case AF_IN_SLOWROAD:
		case AF_OUT_FASTROAD:
		case AF_Out_Fastway:
		case AF_Out_Freeway:
		case AF_Out_Overpass:
		case AF_OUT_SLOWROAD:
			if (NowNSSInfo->TurnFlag == 0)
			{
				return T_Afore;
			}
			else
			{
				return NowNSSInfo->TurnFlag;
			}
			break;
		default:
			break;
		}
	}
	nuBOOL CNaviData::JudgeSameRoadName(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo)
	{
		nuBOOL bRoadNameAddr = nuFALSE;
		nuINT	 Count = 0 ;
		nuWCHAR  wsNowRoadName[MAX_LISTNAME_NUM] = {0};//
		nuWCHAR  wsNextRoadName[MAX_LISTNAME_NUM] = {0};//
		nuINT    nCount = 0, NCCCount = 0;
		nuBOOL   bRoadName = nuFALSE;
		RNEXTEND RnEx = {0};
		nuINT    RoadNum = 0;
		if ( NowNSSInfo.pNSS->RoadVoiceType == VRT_FastToSlow && NextNSSInfo.pNSS->RoadVoiceType == VRT_FastToSlow)
		{//ºCš®¹D
			RoadNum = 1;
		}
		else if ( NowNSSInfo.pNSS->RoadVoiceType == VRT_SlowToFast && NextNSSInfo.pNSS->RoadVoiceType == VRT_SlowToFast)	
		{//§Öš®¹D
			RoadNum = 2;
		}
		else if((NowNSSInfo.pNSS->RoadClass == 7 && NextNSSInfo.pNSS->RoadClass == 7)||(NowNSSInfo.pNSS->RoadClass == 6 && NextNSSInfo.pNSS->RoadClass == 6))
		{//§Öš®¹D
			RoadNum = 3;
		}
		switch(RoadNum)
		{
		case 1:
		case 2:
			for(nCount = 0; nCount < NowNSSInfo.pNSS->CrossCount; nCount++ )
			{// €ežôŠ³¥æ¬y¹DŽN­nŒœ³øÅã¥Ü
				if (NowNSSInfo.pNSS->NCC[nCount].CrossAngle == NowNSSInfo.pNSS->TurnAngle ||
					NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) == 180 )
				{

					continue;

				}
				if (NowNSSInfo.pNSS->NCC[nCount].CrossClass == 5 )
				{
					return nuFALSE;
				}
			}
			if ( NowNSSInfo.pNSS->RoadNameAddr ==  NextNSSInfo.pNSS->RoadNameAddr)
			{
				return nuTRUE;
			}
			break;
		case 3:
			for(nCount = 0; nCount < NowNSSInfo.pNSS->CrossCount; nCount++ )
			{// €ežôŠ³¥æ¬y¹DŽN­nŒœ³øÅã¥Ü 
				if (NowNSSInfo.pNSS->NCC[nCount].CrossRoadNameAddr == NowNSSInfo.pNSS->RoadNameAddr)
				{
					Count++;
				}

				if ( Count >= 2 )
				{
					bRoadNameAddr = nuTRUE;
				}
				if (NowNSSInfo.pNSS->NCC[nCount].CrossAngle == NowNSSInfo.pNSS->TurnAngle ||
					NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) == 180 )
				{
					continue;
				}
				if (NowNSSInfo.pNSS->NCC[nCount].CrossClass == 5 )
				{
					return nuFALSE;
				}
				if(NowNSSInfo.pNSS->NCC[nCount].CrossVoiceType == 43)
				{//§ÃžôŠ³¥kÂà±M¥Î¹D«h€£­nŒ·³ø
					return nuTRUE;
				}
			}
			if(bRoadNameAddr)
			{//šâ±øor€T±ø³£¬°ŠPžôŠW
				for(nCount = 0; nCount < NowNSSInfo.pNSS->CrossCount; nCount++ )
				{// €U€@±øªºš€«× > §Ãžôªºš€«× ŽN­nŒ·³øŸa¥ªŸa¥k
					if (NowNSSInfo.pNSS->NCC[nCount].CrossAngle == NowNSSInfo.pNSS->TurnAngle ||
						NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) == 180 )
					{
						continue;
					}
					if (NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) < NURO_ABS(NowNSSInfo.pNSS->TurnAngle))
					{
						return nuFALSE;
					}
				}
			}
			if ( NowNSSInfo.pNSS->RoadNameAddr ==  NextNSSInfo.pNSS->RoadNameAddr)
			{
				return nuTRUE;
			}
			break;
		default:
			if (NowNSSInfo.pNSS->RoadVoiceType == VRT_SlowToFast && NextNSSInfo.pNSS->RoadVoiceType == 0 )
			{
				return nuFALSE;
			}
			if (NowNSSInfo.pNSS->RoadVoiceType == VRT_SlipRoad && NextNSSInfo.pNSS->RoadVoiceType == VRT_SlipRoad )
			{
				return nuFALSE;
			}
			for(nCount = 0; nCount < NowNSSInfo.pNSS->CrossCount; nCount++)
			{
				if(NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) == 180)
				{
					continue;
				}
				NCCCount++;
			}
			if( NCCCount < 3 )
			{
				for(nCount = 0; nCount < NowNSSInfo.pNSS->CrossCount; nCount++)
				{
					if (NowNSSInfo.pNSS->NCC[nCount].CrossAngle == NowNSSInfo.pNSS->TurnAngle ||
						NURO_ABS(NowNSSInfo.pNSS->NCC[nCount].CrossAngle) == 180 )
					{
						continue;
					}
					if ((nuINT)(NowNSSInfo.pNSS->RoadClass - NowNSSInfo.pNSS->NCC[nCount].CrossClass) <= -2 )
					{
						bRoadName = nuTRUE;
					}
				}
				if (bRoadName)
				{
					if ( NowNSSInfo.pNSS->RoadNameAddr ==  NextNSSInfo.pNSS->RoadNameAddr)
					{
						return nuTRUE;
					}
				}
			}
			break;
		}
			return nuFALSE;
	}
#endif
nuBOOL CNaviData::GetCarInfoForKM(PKMINFO pKMInfo)
{
	if ( pKMInfo == NULL )
	{
		return nuFALSE;
	}
	if (   pKMInfo->nCarMapIdx  != m_ptNavi.pNRBC->FileIdx 
		|| pKMInfo->nCarBlkIdx  != m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].BlockIdx
		|| pKMInfo->nCarRdIdx   != m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].RoadIdx
		)
	{
			return nuFALSE;
	}
	
	nuLONG nDx = 0, nDy = 0, nAddDis = 0;
	nDx = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
	nDy = m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
	nAddDis = nuSqrt( nDx * nDx + nDy * nDy );
	//PNAVISUBCLASS pNowNSS = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
	NAVINSS NowNSS = {0};
	NowNSS.pNSS   = &(m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx]);
	NowNSS.pNRBC  = m_ptNavi.pNRBC;
	NowNSS.pNFTC  = m_ptNavi.pNFTC;
	NowNSS.nIndex = m_ptNavi.nSubIdx;
	nuLONG nPtIndex =  0;
	for( nPtIndex = m_ptNavi.nPtIdx + 1; nPtIndex < m_ptNavi.pNRBC->NSS[m_ptNavi.nSubIdx].ArcPtCount - 1; nPtIndex++ )
	{
		nDx = NowNSS.pNSS->ArcPt[nPtIndex].x - NowNSS.pNSS->ArcPt[nPtIndex + 1].x;
		nDy = NowNSS.pNSS->ArcPt[nPtIndex].y - NowNSS.pNSS->ArcPt[nPtIndex + 1].y;
		nAddDis += nuSqrt( nDx * nDx + nDy * nDy );
	}
	pKMInfo->nKMRdCount = 0;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].ptFirst	 = pKMInfo->ptCarMapped;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].ptEnd		 = pKMInfo->ptCarNextCoor;
    pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMMapIdx  = NowNSS.pNRBC->FileIdx;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMBlkIdx  = NowNSS.pNSS->BlockIdx;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMRdIdx   = NowNSS.pNSS->RoadIdx;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nRoadClass = NowNSS.pNSS->RoadClass;
	pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nOneWay    = NowNSS.pNSS->nOneWay;	
	++pKMInfo->nKMRdCount;
    while( nAddDis <= _KM_CHECK_MAX_DIS )
	{
		NAVINSS NextNSS = {0};
		if ( !GetNextNSS( NowNSS, &NextNSS ))
		{
			break;
		}
		nuMemcpy( &NowNSS, &NextNSS, sizeof(NowNSS));
		
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].ptFirst    = NowNSS.pNSS->ArcPt[0];
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].ptEnd      = NowNSS.pNSS->ArcPt[NowNSS.pNSS->ArcPtCount - 1];
        pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMMapIdx  = NowNSS.pNRBC->FileIdx;
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMBlkIdx  = NowNSS.pNSS->BlockIdx;
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nKMRdIdx   = NowNSS.pNSS->RoadIdx;
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nRoadClass = NowNSS.pNSS->RoadClass;
		pKMInfo->pKMRoad[pKMInfo->nKMRdCount].nOneWay    = NowNSS.pNSS->nOneWay;
		++pKMInfo->nKMRdCount;
		if ( pKMInfo->nKMRdCount >= _KM_CHECK_MAX_ROADS )
		{
			break;
		}
		nAddDis += NowNSS.pNSS->RoadLength;
	}
	return nuTRUE;
}
nuVOID CNaviData::SoundNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount)
{
	nuINT i = 1, j = 0;
	nuBOOL bRest = nuFALSE;
	nuWCHAR wsName[MAX_LISTNAME_NUM] = {0};
	if(wsRoadName[0] == 0x0028)
	{
		return ;
	}
	while(i < nWordCount)
	{//§PÂ_¬O§_Š³ (XX¥ð®§¯ž) (XXªA°È°Ï)
		if(wsRoadName[i] == 0x0028 )
		{
			i+=1;
			while( i < (nWordCount-2))
			{
				if((wsRoadName[i] == 0x670D && wsRoadName[i+1] == 0x52D9 && wsRoadName[i+2] == 0x5340 ) ||
					(wsRoadName[i] == 0x4F11 && wsRoadName[i+1] == 0x606F && wsRoadName[i+2] == 0x7AD9 ))
				{
					bRest = nuTRUE;
					break;
				}
				i++;
			}
		}
		if (bRest)
		{
			break;
		}
		i++;
	}
	if (bRest)
	{//Š³ªºžÜšú()€ºªº¥ð®§¯ž©ÎªA°È°ÏŠW€l
		i=1;
		while(i < nWordCount)
		{
			if(wsRoadName[i] == 0x0028 )
			{
				i+=1;
				while( i < nWordCount )
				{
					if (wsRoadName[i] != 0x0029)
					{
						wsName[j] = wsRoadName[i];	
					}
					else
					{
						break;
					}
					j++;
					i++;
				}
			}
			i++;
		}
		nuWcscpy(wsRoadName, wsName);
	}
	else
	{
		i=1;
		while(i < nWordCount)
		{
			if((wsRoadName[i] >= 0x0021 && wsRoadName[i] < 0x0030) ||
				(wsRoadName[i] > 0x0039 && wsRoadName[i] < 0x00C0))
			{
				wsRoadName[i] = 0;
				break;
			}
			i++;
		}
	}	
}
#ifndef VALUE_EMGRT
nuLONG CNaviData::CalDraw3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd)
{
	NAVINSS NextNSSInfo = {0}, TempNowNSSInfo = {0};
	if(nDisNowToEnd > 600)
	{
		return 0;
	}
//	if(NowNSSInfo.pNSS->TurnFlag > 1 && NowNSSInfo.pNSS->Real3DPic <= 0)
//	{//²{Šbžô¬qšSŠ³¹êŽº¹ÏŠ³€èŠVŽN·|return false
//		return 0;	
//	}
	if(NowNSSInfo.pNSS->Real3DPic > 0)
	{
		g_pInFsApi->pGdata->drawInfoNavi.RealPicDis = nDisNowToEnd;
		return NowNSSInfo.pNSS->Real3DPic;
	}
	//GetNextNSS( NowNSSInfo , &NextNSSInfo);
	//if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
	//{
		//return 0;
	//}
	TempNowNSSInfo = NowNSSInfo;
	//nDisNowToEnd += NextNSSInfo.pNSS->RoadLength;
	while(nDisNowToEnd <= 600)
	{
		if(TempNowNSSInfo.pNSS->TurnFlag > 1 && TempNowNSSInfo.pNSS->Real3DPic <= 0)
		{//²{Šbžô¬qšSŠ³¹êŽº¹ÏŠ³€èŠVŽN·|return false
			return 0;	
		}
		GetNextNSS( TempNowNSSInfo , &NextNSSInfo);
		if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
		{
			return 0;
		}
		if (TempNowNSSInfo.pNSS->RoadClass <= 3 && TempNowNSSInfo.pNSS->RoadClass <= 3)
		{
			if(NextNSSInfo.pNSS->Real3DPic > 0 && TempNowNSSInfo.pNSS->CrossCount > 1)

			{
				return 0;
			}
		}
		nDisNowToEnd += NextNSSInfo.pNSS->RoadLength;
		if(nDisNowToEnd <= 600 && NextNSSInfo.pNSS->Real3DPic > 0 )
		{
			g_pInFsApi->pGdata->drawInfoNavi.RealPicDis = nDisNowToEnd;
			return NextNSSInfo.pNSS->Real3DPic;
		}
		TempNowNSSInfo = NextNSSInfo;
	}
	/*while(TempNowNSSInfo.pNSS->Real3DPic == 0 && TempNowNSSInfo.pNSS->TurnFlag <= 1)
	{
		GetNextNSS( TempNowNSSInfo , &NextNSSInfo);
		if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
		{
			return 0;
		}
		nDisNowToEnd += NextNSSInfo.pNSS->RoadLength;
		if(nDisNowToEnd > 500)
		{
			return 0;
		}
		TempNowNSSInfo = NextNSSInfo;
	}
	if(TempNowNSSInfo.pNSS->Real3DPic > 0 && nDisNowToEnd <= 500)
	{
		g_pInFsApi->pGdata->drawInfoNavi.RealPicDis = nDisNowToEnd;
		return TempNowNSSInfo.pNSS->Real3DPic;
	}*/
	return 0;
}
#endif
nuVOID CNaviData::SetSimulation()
{
	m_ptSimu.pNFTC = m_ptNavi.pNFTC;//g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
	m_ptSimu.pNRBC = m_ptNavi.pNRBC;//g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]->NRBC;
	m_ptSimu.nSubIdx = m_ptNavi.nSubIdx;
	m_ptSimu.nPtIdx  = m_ptNavi.nPtIdx;
	m_ptSimu.ptMapped= g_pInFsApi->pGdata->carInfo.ptCarIcon;
	m_nSimuRdRealLen = 0;
	m_nSimuRdSegLen  = 0;
}
nuBOOL CNaviData::GetSimuTunnelData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpOut)
{
	if( g_pInFsApi->pGdata == NULL || 
		g_pInFsApi->pGdata->routeGlobal.routingData.NFTC	== NULL )
	{
		return nuFALSE;
	}
	if( nLen <= 0 )
	{
		nLen = DEFAULT_SIMU_DIS;
	}
	PSIMULATIONDATA pSimData = (PSIMULATIONDATA)lpOut;
	nuLONG dx = 0, dy = 0;
	PNAVIFROMTOCLASS pNFTC = NULL;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	nuLONG i = 0;
	nuLONG j = 0;
	nuBOOL bFind = nuFALSE;
	if( bReStart )
	{
		Initialize();
		pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
		if( NULL == pNFTC )
		{
			return nuFALSE;
		}
		pNRBC = pNFTC->NRBC;
		if( NULL == pNRBC || NULL == pNRBC->NSS )
		{
			return nuFALSE;
		}
		m_ptNavi.pNFTC		= pNFTC;
		m_ptNavi.pNRBC		= pNRBC;
		m_ptNavi.nSubIdx	= 0;
		m_ptNavi.nPtIdx		= 0;
		m_ptNavi.ptMapped	= pNFTC->NRBC->NSS[0].ArcPt[0];
		
		m_nSimuRdRealLen = 0;
		i = 0;
		j = 0;
		while( i < pNRBC->NSSCount )
		{
			while( j < pNRBC->NSS[i].ArcPtCount - 1 )
			{
				dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
				dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
				if( 0 != dx || 0 != dy )
				{
					m_nSimuRdSegLen	 = m_nSimuRdNowLen = (nuLONG)nuSqrt(dx*dx + dy*dy);
					bFind = nuTRUE;
					break;
				}
				++j;
			}
			if( bFind )
			{
				break;
			}
			j = 0;
			++i;
		}
		if( !bFind )
		{
			return nuFALSE;
		}
		m_ptSimu.ptMapped = pNRBC->NSS[i].ArcPt[j];
	}
	else
	{
		if( m_ptSimu.pNFTC == NULL  )
		{
			return nuFALSE;
		}
		pNFTC = m_ptSimu.pNFTC;
		pNRBC = m_ptSimu.pNRBC;
		i = m_ptSimu.nSubIdx;
		j = m_ptSimu.nPtIdx;
		m_nSimuRdRealLen += nLen;		

		if( m_nSimuRdRealLen <= m_nSimuRdSegLen )
		{
			dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
			dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
			bFind = nuTRUE;
		}
		else
		{
			if (m_nSimuRdSegLen == 0)
			{
				dx = pNRBC->NSS[i].ArcPt[j+1].x - m_ptNavi.ptMapped.x;
				dy = pNRBC->NSS[i].ArcPt[j+1].y - m_ptNavi.ptMapped.y;
				m_nSimuRdSegLen = m_nSimuRdNowLen = (nuLONG)nuSqrt(dx*dx + dy*dy);	
			}
			j++;
			while( NULL != pNFTC )
			{
				while( NULL != pNRBC && 0 == pNRBC->RTBStateFlag )
				{
					while( i < pNRBC->NSSCount )
					{
						while( j < pNRBC->NSS[i].ArcPtCount - 1 )
						{
							dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
							dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
							m_nSimuRdNowLen = (nuLONG)nuSqrt(dx*dx + dy*dy);
							m_nSimuRdSegLen += m_nSimuRdNowLen;
							if( m_nSimuRdRealLen <= m_nSimuRdSegLen )
							{
								bFind = nuTRUE;
								break;
							}
							++j;
						}
						if( bFind )
						{
							break;
						}
						j = 0;
						++i;
					}
					if( bFind )
					{
						break;
					}
					//
					m_ptSimu.pNRBC = pNRBC;
					pNRBC	= pNRBC->NextRTB;
					i = 0;
				}
				if( bFind )
				{
					break;
				}
				m_ptSimu.pNFTC = pNFTC;
				pNFTC	= pNFTC->NextFT;
				if( pNFTC )
				{
					pNRBC = pNFTC->NRBC;
				}
			}
		}
		if( bFind )
		{
			nLen = m_nSimuRdRealLen + m_nSimuRdNowLen - m_nSimuRdSegLen;
			m_ptSimu.ptMapped.x = pNRBC->NSS[i].ArcPt[j].x + nLen * dx / m_nSimuRdNowLen;
			m_ptSimu.ptMapped.y = pNRBC->NSS[i].ArcPt[j].y + nLen * dy / m_nSimuRdNowLen;
		}
		else
		{
			pNFTC	= m_ptSimu.pNFTC;
			pNRBC	= m_ptSimu.pNRBC;
			for( i = pNRBC->NSSCount - 1; i >= 0; --i )
			{
				for( j = pNRBC->NSS[i].ArcPtCount - 2; j >= 0; --j )
				{
					dx = pNRBC->NSS[i].ArcPt[j+1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j+1].y - pNRBC->NSS[i].ArcPt[j].y;
					if( 0 != dx || 0 != dy )
					{
						m_ptSimu.ptMapped = pNRBC->NSS[i].ArcPt[j+1];
						bFind = nuTRUE;
						break;
					}
				}
				if( bFind )
				{
					break;
				}
			}
			if( !bFind )
			{
				return nuFALSE;
			}
		}
	}
	m_ptSimu.pNFTC		= pNFTC;
	m_ptSimu.pNRBC		= pNRBC;
	m_ptSimu.nSubIdx	= i;
	m_ptSimu.nPtIdx		= j;
	pSimData->nX		= m_ptSimu.ptMapped.x;
	pSimData->nY		= m_ptSimu.ptMapped.y;
	pSimData->nAngle	= nulAtan2(dy, dx);//(nuLONG)(nuAtan2(dy, dx)*180/PI);
	if( pSimData->nAngle < 0 )
	{
		pSimData->nAngle	+= 360;
	}
	m_ptNavi.pNFTC		= pNFTC;
	m_ptNavi.pNRBC		= pNRBC;
	m_ptNavi.nSubIdx	= i;
	m_ptNavi.nPtIdx		= j;
	m_ptNavi.ptMapped.x	= m_ptSimu.ptMapped.x;
	m_ptNavi.ptMapped.y = m_ptSimu.ptMapped.y;
	return nuTRUE;
}
#ifdef ECI
nuVOID CNaviData::JudgeEci3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd)
{
	NAVINSS NextNSSInfo = {0};
	nuLONG nDis = nDisNowToEnd, n3DPic = 0;

	g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = 0;
	if( NowNSSInfo.pNSS->Real3DPic != 0)
	{
		n3DPic =  NowNSSInfo.pNSS->Real3DPic;
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicID = NowNSSInfo.pNSS->Real3DPic;
		g_pInFsApi->pGdata->drawInfoNavi.HighWayCross.nRealPicID = 0;
		while( NowNSSInfo.pNSS->Real3DPic == n3DPic)
		{
			GetNextNSS(NowNSSInfo, &NextNSSInfo);
			if (NextNSSInfo.pNSS  == NULL|| NextNSSInfo.pNFTC  == NULL|| NextNSSInfo.pNFTC == NULL)
			{
				return;
			}
			if (NextNSSInfo.pNSS->Real3DPic != n3DPic)
			{
				g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicDis = nDis;
			}
			nDis += NextNSSInfo.pNSS->RoadLength;
			NowNSSInfo = NextNSSInfo;
		}
	}
}

nuLONG CNaviData::JudgeDraw3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd ,NAVINSS &PicNSSInfo)
{
	RNEXTEND RnEx = {0};
	nuLONG nType = 0;
	NAVINSS NextNSSInfo = {0}, TempNowNSSInfo = {0};
	nuLONG l3DPic = -1;
	nuDWORD lDis = nDisNowToEnd;
	if(nDisNowToEnd > 600)
	{
		return 0;
	}
	if(NowNSSInfo.pNSS->TurnFlag > 1 && NowNSSInfo.pNSS->Real3DPic <= 0)
	{
		return 0;	
	}
	if(NowNSSInfo.pNSS->Real3DPic > 0)
	{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicDis = nDisNowToEnd;
		PicNSSInfo = NowNSSInfo;
		return NowNSSInfo.pNSS->Real3DPic;
	}
	GetNextNSS( NowNSSInfo , &NextNSSInfo);
	if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
	{
		return 0;
	}
	nType = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
	TempNowNSSInfo = NextNSSInfo;
	nDisNowToEnd += NextNSSInfo.pNSS->RoadLength;
	while(TempNowNSSInfo.pNSS->Real3DPic == 0 && TempNowNSSInfo.pNSS->TurnFlag <= 1 && nType == 0)
	{
		GetNextNSS( TempNowNSSInfo , &NextNSSInfo);
		if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
		{
			return 0;
		}
		nType = GetRoutingListCode(NowNSSInfo.pNSS,NextNSSInfo.pNSS);
		nDisNowToEnd += NextNSSInfo.pNSS->RoadLength;
		if(nDisNowToEnd > 600)
		{
			return 0;
		}
		TempNowNSSInfo = NextNSSInfo;
	}
	if(TempNowNSSInfo.pNSS->Real3DPic > 0 && nDisNowToEnd <= 600)
	{
		g_pInFsApi->pGdata->drawInfoNavi.nextCross.nRealPicDis = nDisNowToEnd;
		PicNSSInfo = TempNowNSSInfo;
		return TempNowNSSInfo.pNSS->Real3DPic;
	}
	return 0;
}
nuBOOL CNaviData::ColNextDis(NAVINSS NowNSSInfo,nuDWORD &nPreDis)
{
	nuDWORD lDis = 0;
	NAVINSS NextNSSInfo = {0};
	GetNextNSS( NowNSSInfo , &NextNSSInfo);
	if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
	{
		return 0;
	}
	while(NextNSSInfo.pNSS->TurnFlag <= 1 || NextNSSInfo.pNSS->TurnFlag == 5 || NextNSSInfo.pNSS->TurnFlag == 4) 
	{
		GetNextNSS(NowNSSInfo , &NextNSSInfo);
		if(NextNSSInfo.pNSS == NULL || NextNSSInfo.pNFTC == NULL || NextNSSInfo.pNRBC == NULL)
		{
			return 0;
		}
		nPreDis += NextNSSInfo.pNSS->RoadLength;
		NowNSSInfo = NextNSSInfo;
	}
	return 1;
}
#endif
nuBOOL CNaviData::TMCNaviPathData()
{
	__android_log_print(ANDROID_LOG_INFO, "TMC", "TMCNaviPathData!!!!");
	while (g_pInFsApi->pGdata->drawInfoNavi.bTMCListUsed)
	{
		nuSleep(50);
	}
	g_pInFsApi->pGdata->drawInfoNavi.bTMCListUsed = 1;
	FreeTMCNaviPathData();
	if(!InitTMCNaviPathData())
	{
			__android_log_print(ANDROID_LOG_INFO, "TMC", "TMCNaviPathData return!!!!");
		return nuFALSE; 
	}
	nuINT i = 0,nTMCCount= 0;
	nuBOOL bFlag = nuFALSE;
	nuLONG dx = 0, dy = 0, dxy = 0 ;
	nuLONG nTMCDis = 0,TotalDis = 0;
	nuLONG lUnsureDis = 0, lNormalDis = 0, lSlowDis = 0, lCrowdDis = 0;
	PNAVIFROMTOCLASS    pNFTC =  m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS   pNRBC =  NULL;
	TotalDis =  g_pInFsApi->pGdata->drawInfoNavi.nDisToTarget;
	g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = 0;
	g_pInFsApi->pGdata->drawInfoNavi.lUnsureDis = 0;
	g_pInFsApi->pGdata->drawInfoNavi.lNormalDis = 0;
	g_pInFsApi->pGdata->drawInfoNavi.lSlowDis = 0;
	g_pInFsApi->pGdata->drawInfoNavi.lCrowdDis = 0;
	TMCNaviCrowdDis();
	while(pNFTC != NULL)
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL )
		{
			if( pNRBC == m_ptNavi.pNRBC )
			{
				i = m_ptNavi.nSubIdx;
				dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
				dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
				dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
				nuLONG nDisTotal = 0;
				nuLONG nDisNowToEnd = dxy;
				for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
				{
					dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
					dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
					if (  j >= m_ptNavi.nPtIdx + 1 )
					{	
						nDisNowToEnd += dxy;
					}
					nDisTotal += dxy;
				}
				if ( nDisTotal )
				{
					nDisNowToEnd = nDisNowToEnd * pNRBC->NSS[i].RoadLength / nDisTotal;
				}
				nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
				
				TMCRoadInfoStru* pdirdata = g_pInFsApi->FS_GetTMCDataInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);

				if (pdirdata == NULL || pdirdata[0].Dir== 0 ||pdirdata[1].Dir== 0)
				{
					nWeighting = nWeighting;
				}
				else
				{
					if (pNRBC->NSS[i].nOneWay != 0 && pdirdata[0].Dir!= 0 && pdirdata[1].Dir != 0)
					{
						if (pdirdata[0].Dir == pNRBC->NSS[i].nOneWay)
						{
							nWeighting = pdirdata[0].Traffic;
						}
						else
						{
							nWeighting = pdirdata[1].Traffic;
						}
					}

				}
				g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nRoadType = nWeighting;
				switch(nWeighting)
				{
					case 0:
						lUnsureDis += pNRBC->NSS[i].RoadLength;
						break;
					case 1:
						lNormalDis += pNRBC->NSS[i].RoadLength;
						break;
					case 2:
						lSlowDis += pNRBC->NSS[i].RoadLength;
						break;
					case 3:
						lCrowdDis += pNRBC->NSS[i].RoadLength;
						break;
					default:
						break;
				}
				g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nDistance = nDisNowToEnd;
				nTMCDis = nDisNowToEnd;
				i ++;
				nTMCCount ++;
				while( i < (nuLONG)pNRBC->NSSCount)
				{
					nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
					TMCRoadInfoStru* pdirdata = g_pInFsApi->FS_GetTMCDataInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
				
					if (pdirdata == NULL || pdirdata[0].Dir== 0 ||pdirdata[1].Dir== 0)
					{
						nWeighting = nWeighting;
					}
					else
					{
						if (pNRBC->NSS[i].nOneWay != 0 && pdirdata[0].Dir!= 0 && pdirdata[1].Dir != 0)
						{
							if (pdirdata[0].Dir == pNRBC->NSS[i].nOneWay)
							{
								nWeighting = pdirdata[0].Traffic;
							}
							else
							{
								nWeighting = pdirdata[1].Traffic;
							}
						}

					}				
					switch(nWeighting)
					{
						case 0:
							lUnsureDis += pNRBC->NSS[i].RoadLength;
							break;
						case 1:
							lNormalDis += pNRBC->NSS[i].RoadLength;
							break;
						case 2:
							lSlowDis += pNRBC->NSS[i].RoadLength;
							break;
						case 3:
							lCrowdDis += pNRBC->NSS[i].RoadLength;
							break;
						default:
							break;
					}
					nTMCDis += pNRBC->NSS[i].RoadLength;
					if(nTMCDis >= TMCMAXDIS)
					{
						switch(nWeighting)
						{
							case 0:
								lUnsureDis -= pNRBC->NSS[i].RoadLength;
								nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
								bFlag = nuTRUE;
								break;
							case 1:
								lNormalDis -= pNRBC->NSS[i].RoadLength;
								nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
								bFlag = nuTRUE;
								break;
							case 2:
								lSlowDis -= pNRBC->NSS[i].RoadLength;
								nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
								bFlag = nuTRUE;
								break;
							case 3:
								lCrowdDis -= pNRBC->NSS[i].RoadLength;
								nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
								bFlag = nuTRUE;
								break;
							default:
								break;
						}
						break;
					}
					g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nRoadType = nWeighting;
					g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nDistance = pNRBC->NSS[i].RoadLength;
					nTMCCount ++;
					i++;
				}
			}
			else
			{
				i = 0;
				//if(m_ptNavi.pNRBC->FInfo.MapID == pNRBC->FInfo.MapID)//·í«e©ÒŠb¿€¥«
				{
					if( pNRBC->NSSCount == 0 )
					{
						lUnsureDis += pNRBC->RealDis;
						nTMCDis += pNRBC->RealDis;
						if(nTMCDis >= TMCMAXDIS)
						{
							bFlag = nuTRUE;
							nTMCDis = nTMCDis - pNRBC->RealDis;
							lUnsureDis = lUnsureDis -  pNRBC->RealDis;
							break;
						}				
						g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nRoadType = 0;
						g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nDistance = pNRBC->RealDis;
						nTMCCount++;
					}
					else
					{
						if ( pNRBC->RTBStateFlag == 0 )
						{
							while( i < (nuLONG)pNRBC->NSSCount )
							{	
								nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
								switch(nWeighting)
								{
									case 0:
										lUnsureDis += pNRBC->NSS[i].RoadLength;
										break;
									case 1:
										lNormalDis += pNRBC->NSS[i].RoadLength;
										break;
									case 2:
										lSlowDis += pNRBC->NSS[i].RoadLength;
										break;
									case 3:
										lCrowdDis += pNRBC->NSS[i].RoadLength;
										break;
									default:
										break;
								}
								nTMCDis +=  pNRBC->NSS[i].RoadLength;
								if(nTMCDis >= TMCMAXDIS)
								{
									switch(nWeighting)
									{
										case 0:
											lUnsureDis -= pNRBC->NSS[i].RoadLength;
											nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
											bFlag = nuTRUE;
											break;
										case 1:
											lNormalDis -= pNRBC->NSS[i].RoadLength;
											nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
											bFlag = nuTRUE;
											break;
										case 2:
											lSlowDis -= pNRBC->NSS[i].RoadLength;
											nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
											bFlag = nuTRUE;
											break;
										case 3:
											lCrowdDis -= pNRBC->NSS[i].RoadLength;
											nTMCDis = nTMCDis - pNRBC->NSS[i].RoadLength;
											bFlag = nuTRUE;
											break;
										default:
											break;
									}
									break;
								}
								g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nDistance = pNRBC->NSS[i].RoadLength;
								g_pInFsApi->pGdata->drawInfoNavi.pTMCList[nTMCCount].nRoadType = nWeighting;
								nTMCCount++;
								i++;	
							}
						}
					}	
				}
			}
			if(bFlag)
			{
				break;
			}
			pNRBC = pNRBC->NextRTB;
		}
		if(bFlag)
		{
			break;
		}
		pNFTC = pNFTC->NextFT;
	}
	if(nTMCDis < TMCMAXDIS && TotalDis > TMCMAXDIS)
	{
		lUnsureDis += TMCMAXDIS - lUnsureDis - lNormalDis - lSlowDis - lCrowdDis;
	}
	g_pInFsApi->pGdata->drawInfoNavi.lUnsureDis = lUnsureDis;
	g_pInFsApi->pGdata->drawInfoNavi.lNormalDis = lNormalDis;
	g_pInFsApi->pGdata->drawInfoNavi.lSlowDis	= lSlowDis;
	g_pInFsApi->pGdata->drawInfoNavi.lCrowdDis  = lCrowdDis;
	g_pInFsApi->pGdata->drawInfoNavi.bTMCListUsed = 0;
	__android_log_print(ANDROID_LOG_INFO, "TMC", "TMCNaviPathData OK!!!!");
	return 1;
}

nuBOOL CNaviData::InitTMCNaviPathData()
{
	nuINT i = 0;
	nuBOOL bFlag = nuFALSE;
	nuLONG dx = 0, dy = 0, dxy = 0 ;
	nuLONG nTMCDis = 0,TotalDis = 0;
	g_pInFsApi->pGdata->drawInfoNavi.nTMCCount = 0;
	PNAVIFROMTOCLASS    pNFTC =  m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS   pNRBC =  NULL;
	while(pNFTC != NULL)
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL )
		{
			if( pNRBC == m_ptNavi.pNRBC )
			{	
				i = m_ptNavi.nSubIdx;
				dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
				dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
				dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
				nuLONG nDisTotal = 0;
				nuLONG nDisNowToEnd = dxy;
				for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
				{
					dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
					dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
					if (  j >= m_ptNavi.nPtIdx + 1 )
					{	
						nDisNowToEnd += dxy;
					}
					nDisTotal += dxy;
				}
				if ( nDisTotal )
				{
					nDisNowToEnd = nDisNowToEnd * pNRBC->NSS[i].RoadLength / nDisTotal;
				}
				nTMCDis = nDisNowToEnd;
				g_pInFsApi->pGdata->drawInfoNavi.nTMCCount ++;
				i ++;
				while( i < (nuLONG)pNRBC->NSSCount)
				{
					nTMCDis += pNRBC->NSS[i].RoadLength;
					if(nTMCDis >= TMCMAXDIS)
					{
						bFlag = nuTRUE;
						break;
					}
					g_pInFsApi->pGdata->drawInfoNavi.nTMCCount ++;
					i ++;
				}
			}
			else
			{
				i = 0;
				{
					if( pNRBC->NSSCount == 0 )
					{
						nTMCDis += pNRBC->RealDis;
						if(nTMCDis >= TMCMAXDIS)
						{
							bFlag = nuTRUE;
							break;
						}				
						g_pInFsApi->pGdata->drawInfoNavi.nTMCCount ++;
					}
					else
					{
						if ( pNRBC->RTBStateFlag == 0 )
						{
							while( i < (nuLONG)pNRBC->NSSCount )
							{	
								nTMCDis += pNRBC->NSS[i].RoadLength;
								if(nTMCDis >= TMCMAXDIS)
								{
									bFlag = nuTRUE;
									break;
								}
								g_pInFsApi->pGdata->drawInfoNavi.nTMCCount ++;
								i++;	
							}
						}
					}	
				}
			}
			if(bFlag)
			{
				break;
			}
			pNRBC = pNRBC->NextRTB;
		}
		if(bFlag)
		{
			break;
		}
		pNFTC = pNFTC->NextFT;
	}
	m_lTMCMemIdx = 0;
	g_pInFsApi->pGdata->drawInfoNavi.pTMCList = new TMCDATA[g_pInFsApi->pGdata->drawInfoNavi.nTMCCount];//(TMCDATA*)g_pInMmApi->MM_GetDataMemMass(g_pInFsApi->pGdata->drawInfoNavi.nTMCCount*sizeof(TMCDATA), &m_lTMCMemIdx);
	if( g_pInFsApi->pGdata->drawInfoNavi.pTMCList  == NULL )
	{
		return nuFALSE;
	}
	nuMemset( g_pInFsApi->pGdata->drawInfoNavi.pTMCList, 0, sizeof(TMCDATA) *g_pInFsApi->pGdata->drawInfoNavi.nTMCCount);
	__android_log_print(ANDROID_LOG_INFO, "TMC", "InNavi nTMCCount: %ld", g_pInFsApi->pGdata->drawInfoNavi.nTMCCount);
	return nuTRUE;
}
nuVOID CNaviData::FreeTMCNaviPathData()
{
	if(NULL != g_pInFsApi->pGdata->drawInfoNavi.pTMCList)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "FreeTMCNaviPathData");
		delete [] g_pInFsApi->pGdata->drawInfoNavi.pTMCList;//g_pInMmApi->MM_RelDataMemMass(&m_lTMCMemIdx);
		g_pInFsApi->pGdata->drawInfoNavi.pTMCList = NULL;
	}
}
nuVOID CNaviData::TMCNaviCrowdDis()
{
	nuINT i = 0,nTMCCount= 0;
	nuBOOL bFlag = nuFALSE;
	nuLONG dx = 0, dy = 0, dxy = 0 ;
	nuLONG nTMCDis = 0,TotalDis = 0;
	PNAVIFROMTOCLASS    pNFTC =  m_ptNavi.pNFTC;
	PNAVIRTBLOCKCLASS   pNRBC =  NULL;
	while(pNFTC != NULL)
	{
		if( pNFTC == m_ptNavi.pNFTC )
		{
			pNRBC = m_ptNavi.pNRBC;
		}
		else
		{
			pNRBC = pNFTC->NRBC;
		}
		while( pNRBC != NULL )
		{
			if( pNRBC == m_ptNavi.pNRBC )
			{
				i = m_ptNavi.nSubIdx;
				dx = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].x - m_ptNavi.ptMapped.x;
				dy = pNRBC->NSS[i].ArcPt[m_ptNavi.nPtIdx + 1].y - m_ptNavi.ptMapped.y;
				dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
				nuLONG nDisTotal = 0;
				nuLONG nDisNowToEnd = dxy;
				for( nuLONG j = 0; j < pNRBC->NSS[i].ArcPtCount - 1; j++ )
				{
					dx = pNRBC->NSS[i].ArcPt[j + 1].x - pNRBC->NSS[i].ArcPt[j].x;
					dy = pNRBC->NSS[i].ArcPt[j + 1].y - pNRBC->NSS[i].ArcPt[j].y;
					dxy = (nuLONG)nuSqrt(dx * dx + dy * dy);
					if (  j >= m_ptNavi.nPtIdx + 1 )
					{	
						nDisNowToEnd += dxy;
					}
					nDisTotal += dxy;
				}
				if ( nDisTotal )
				{
					nDisNowToEnd = nDisNowToEnd * pNRBC->NSS[i].RoadLength / nDisTotal;
				}
				nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
				switch(nWeighting)
				{
					case 0:
					case 1:
					case 2:
						break;
					case 3:
						g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = nTMCDis;
						return;
						break;
					default:
						break;
				}
				nTMCDis = nDisNowToEnd;
				i ++;
				while( i < (nuLONG)pNRBC->NSSCount)
				{
					nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
					switch(nWeighting)
					{
						case 0:
						case 1:
						case 2:
							break;
						case 3:
							g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = nTMCDis;
							return;
							break;
						default:
							break;
					}
					nTMCDis += pNRBC->NSS[i].RoadLength;
					i++;
				}
			}
			else
			{
				i = 0;
				{
					if( pNRBC->NSSCount == 0 )
					{
						nTMCDis += pNRBC->RealDis;
					}
					else
					{
						if ( pNRBC->RTBStateFlag == 0 )
						{
							while( i < (nuLONG)pNRBC->NSSCount )
							{	
								nuUINT nWeighting = g_pInFsApi->FS_GetTMCTrafficInfo(pNRBC->FileIdx,pNRBC->NSS[i].BlockIdx,pNRBC->NSS[i].RoadIdx);
								switch(nWeighting)
								{
									case 0:
									case 1:
									case 2:
										break;
									case 3:
										g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = nTMCDis;
										return;
										break;
									default:
										break;
								}
								nTMCDis +=  pNRBC->NSS[i].RoadLength;
								i++;	
							}
						}
					}	
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	g_pInFsApi->pGdata->drawInfoNavi.lTMCCrowdDis = nTMCDis;
	return ;
}
//============================For SDK==========================================//
nuVOID CNaviData::Initial_D()
{
	m_nDisplayCount	     = 0;
	m_nRoadListCount     = 0;
	if(m_pShowRoadBuf != NULL)
	{
		delete [] m_pShowRoadBuf;
		m_pShowRoadBuf = NULL;
	}
}

nuBOOL CNaviData::GetRouteListCount_D(nuINT &NuroRoadCount, nuINT &ShowRoadCount)
{
	Initial_D();
	PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	if(pNFTC == NULL)
	{
		return nuFALSE;
	}
	ShowRoadCount			= 0;
	NuroRoadCount			= 0;
	m_nRoadListCount		= 0;
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	while(pNFTC != NULL)
	{
		pNRBC = pNFTC->NRBC;
		while ( pNRBC!= NULL)
		{
			if(pNRBC->RTBStateFlag == 1)
			{
				m_nRoadListCount++;
			}
			else
			{
				m_nRoadListCount += pNRBC->NSSCount;
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0];
	nuLONG OrIdx = 0, lTempRoadState = 0;
	nuWCHAR wsRoadName[MAX_LISTNAME_NUM] = {0};
	RNEXTEND RnEx = {0};
	nuWCHAR  PreRoadName[MAX_LISTNAME_NUM] = {0};
	while(pNFTC != NULL)
	{
		pNRBC = pNFTC->NRBC;
		while ( pNRBC != NULL)
		{
			if(pNRBC->RTBStateFlag == 1)
			{
				m_nDisplayCount++;
				OrIdx++;
			}
			else
			{
				for(nuLONG i = 0; i < pNRBC->NSSCount; i++) 
				{	
					wsRoadName[0]=0;
					g_pInFsApi->FS_GetRoadName( (nuWORD)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr, pNRBC->NSS[i].RoadClass, wsRoadName, MAX_LISTNAME_NUM, &RnEx);
					SoundNamefilter(wsRoadName, nuWcslen(wsRoadName));
					pNRBC->NSS[i].ShowIndex = -1;
					if(nuWcscmp(wsRoadName, PreRoadName) != 0 /*|| 
						(pNRBC->NSS[i].RoadVoiceType != lTempRoadState && 
						pNRBC->NSS[i].RoadVoiceType != 31 && 
						pNRBC->NSS[i].TurnFlag > 0)*/)
					{				
						lTempRoadState = pNRBC->NSS[i].RoadVoiceType;
						pNRBC->NSS[i].ShowIndex = m_nDisplayCount;
						m_nDisplayCount++;					
						nuWcscpy(PreRoadName, wsRoadName);
					}
					OrIdx++;
					if(m_nDisplayCount > m_nRoadListCount || OrIdx > m_nRoadListCount)
					{//error
						Initial_D();
						return nuFALSE;
					}
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	NuroRoadCount = m_nRoadListCount;
	ShowRoadCount   = m_nDisplayCount;
	return nuTRUE;
}
nuLONG CNaviData::GetRouteListData_D(NURO_NAVI_LIST* pShowRoadBuf,nuINT nShowRoadCount, nuINT nShowRoadListIndex)
{
	if(m_nDisplayCount == 0 || pShowRoadBuf == NULL)
	{
		Initial_D();
		return nuFALSE;
	}
	nuINT Idx = 0, nDis  = 0, nTotalCounter = 0;
	nuLONG lTempRoadState = 0, lTempNameIdx  = 0;
	nuLONG ReturnDisplayIdx = 0, RoadIdx = 0;
	nuINT PreTurnFlag = 0;
	nuWCHAR wsRoadName[MAX_LISTNAME_NUM] = {0};
	nuBOOL	bEnough = nuFALSE, bCalDis = nuFALSE;
	RNEXTEND RnEx = {0};
	PNAVIRTBLOCKCLASS pNRBC = NULL;
	NAVINSS NowNSSInfo = {0},TempNextInfo = {0};
	PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	if(pNFTC == NULL)
	{
		return nuFALSE;
	}
	while(pNFTC != NULL && !bEnough)
	{
		pNRBC = pNFTC->NRBC;
		while ( pNRBC != NULL && !bEnough)
		{		
			if(pNRBC->RTBStateFlag == 1)
			{
				nuMemset(&pShowRoadBuf[ReturnDisplayIdx], 0, sizeof(NURO_NAVI_LIST));
				if(GetRTBName(pNRBC->FileIdx, pNRBC->FInfo.RTBID, wsRoadName))
				{
			//---------------------------Input Buffer---------------------------------------//
					nuWcsncpy((nuWCHAR*)pShowRoadBuf[ReturnDisplayIdx].wsName, wsRoadName, LIST_NAME_NUM);
					pShowRoadBuf[ReturnDisplayIdx].nIconType = 5107;
					pShowRoadBuf[ReturnDisplayIdx].nDistance = pNRBC->WeightDis;
					pShowRoadBuf[ReturnDisplayIdx].nReserve	 = 1;
					pShowRoadBuf[ReturnDisplayIdx].nX	 = pNRBC->FInfo.FixCoor.x;
					pShowRoadBuf[ReturnDisplayIdx].nY	 = pNRBC->FInfo.FixCoor.y;
			//---------------------------Input Buffer---------------------------------------//
					ReturnDisplayIdx++;
					if(ReturnDisplayIdx >= nShowRoadCount)
					{
						bEnough = nuTRUE;
					}
				}				
			}
			else
			{
				for(nuLONG i = 0; i < pNRBC->NSSCount; i++) 
				{
					if(bCalDis)
					{
						nDis += pNRBC->NSS[i].RoadLength;
					}
					if(pNRBC->NSS[i].ShowIndex >= 0 && (!bEnough || pNRBC->NSS[i].RoadVoiceType != lTempRoadState))
					{
						if(Idx >= nShowRoadListIndex && ReturnDisplayIdx < nShowRoadCount)
						{
							if(ReturnDisplayIdx >= m_nDisplayCount)
							{//error
								Initial_D();
								return 0;
							}
							bCalDis = nuTRUE;
							if(pNRBC->NSS[i].RoadVoiceType == VRT_ROADCROSS)
							{
								NowNSSInfo.pNRBC = pNRBC;
								NowNSSInfo.pNFTC = pNFTC;
								NowNSSInfo.pNSS = &pNRBC->NSS[i];
								NowNSSInfo.nIndex = i;
								if(GetNextNSS( NowNSSInfo,&TempNextInfo ))
								{
									g_pInFsApi->FS_GetRoadName( (nuWORD)TempNextInfo.pNRBC->FileIdx, TempNextInfo.pNSS->RoadNameAddr,
																TempNextInfo.pNSS->RoadClass, wsRoadName, MAX_LISTNAME_NUM, &RnEx);
								}
								else
								{
									g_pInFsApi->FS_GetRoadName( (nuWORD)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr,
																pNRBC->NSS[i].RoadClass, wsRoadName, MAX_LISTNAME_NUM, &RnEx);						
								}
							}
							else
							{
								g_pInFsApi->FS_GetRoadName( (nuWORD)pNRBC->FileIdx, pNRBC->NSS[i].RoadNameAddr,
															pNRBC->NSS[i].RoadClass, wsRoadName, MAX_LISTNAME_NUM, &RnEx);						
							}
							SoundNamefilter(wsRoadName, nuWcslen(wsRoadName));
							nuWcsncpy((nuWCHAR*)pShowRoadBuf[ReturnDisplayIdx].wsName, wsRoadName, LIST_NAME_NUM);
							pShowRoadBuf[ReturnDisplayIdx].nReserve	 				= 0;
							pShowRoadBuf[ReturnDisplayIdx].nX					= pNRBC->NSS[i].ArcPt[0].x;
							pShowRoadBuf[ReturnDisplayIdx].nY					= pNRBC->NSS[i].ArcPt[0].y;
							pShowRoadBuf[ReturnDisplayIdx].nDistance		    = nDis;
							lTempNameIdx										= pNRBC->NSS[i].RoadNameAddr;
							lTempRoadState										= pNRBC->NSS[i].RoadVoiceType;
							nDis												= 0;
							if(i > 0)
							{
								pShowRoadBuf[ReturnDisplayIdx].nIconType = nuWORD(1024 + PreTurnFlag);
							}
							ReturnDisplayIdx++;
							if(ReturnDisplayIdx >= nShowRoadCount)
							{
								bEnough = nuTRUE;
							}
						 }
						 Idx++;					
					}
					PreTurnFlag = pNRBC->NSS[i].TurnFlag;
				}				
				nTotalCounter++;
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	__android_log_print(ANDROID_LOG_INFO, "NaviList", "ReturnDisplayIdx %d, nShowRoadCount %d, m_nDisplayCount %d\n", ReturnDisplayIdx, nShowRoadCount, m_nDisplayCount);
	return ReturnDisplayIdx;
}
nuBOOL CNaviData::GetRoadListIndex_D(nuINT &DisplayIndex, nuINT &nRoadListIndex)
{
	Initial_D();
	if(m_nDisplayCount <= 0)
	{
		nuINT NuroRoadCount = 0, ShowRoadCount = 0, nRoadListIndex = 0, nShowRoadListIndex = 0;
		if(GetRouteListCount_D(NuroRoadCount, ShowRoadCount))
		{
			if(m_nDisplayCount <= 0)
			{
				return nuFALSE;
			}
		}
		else
		{
			return nuFALSE;
		}
	}
	
	nuLONG lTempRoadState = 0, lTempNameIdx  = 0;
	nuWCHAR wsRoadName[MAX_LISTNAME_NUM] = {0}, wsTempRoadName[MAX_LISTNAME_NUM] = {0};
	PNAVIFROMTOCLASS pNFTC = g_pInFsApi->pGdata->routeGlobal.routingData.NFTC[0]; 
	if(pNFTC == NULL)
	{
		return nuFALSE;
	}
	PNAVIRTBLOCKCLASS pNRBC = pNFTC->NRBC;
	DisplayIndex = nRoadListIndex = 0;
	while(pNFTC != NULL)
	{
		pNRBC = pNFTC->NRBC;
		while ( pNRBC != NULL)
		{		
			if(pNRBC->RTBStateFlag == 1 )
			{
				nRoadListIndex++;
				DisplayIndex++;
			}
			else
			{
				for(nuLONG i = 0; i < pNRBC->NSSCount; i++) 
				{
					nRoadListIndex++;					
					if(DisplayIndex >=  m_nRoadListCount)
					{
						Initial_D();
						return nuFALSE;
					}	
				}
			}
			pNRBC = pNRBC->NextRTB;
		}
		pNFTC = pNFTC->NextFT;
	}
	if(DisplayIndex < 0)
	{
		DisplayIndex = 0;
	}
	if(nRoadListIndex < 0)
	{
		nRoadListIndex = 0;
	}
	return nuTRUE;
}
nuPVOID CNaviData::GetRoutingListEx(PROUTINGLISTNODE *ppRTlist, nuINT *pNum, nuINT *pIndex)
{
	nuINT NuroRoadCount = 0, ShowRoadCount = 0;
	GetRouteListCount_D(NuroRoadCount, ShowRoadCount);
	if(m_pShowRoadBuf != NULL)
	{
		delete [] m_pShowRoadBuf;
		m_pShowRoadBuf = NULL;
	}
	m_pShowRoadBuf = new NURO_NAVI_LIST[m_nDisplayCount];
	*pNum = (nuLONG)GetRouteListData_D(m_pShowRoadBuf, m_nDisplayCount, 0);
	//*pNum = (nuLONG)m_nDisplayCount;
	return (nuPVOID)m_pShowRoadBuf;
}
//=========================================For SDK==================================================================//
