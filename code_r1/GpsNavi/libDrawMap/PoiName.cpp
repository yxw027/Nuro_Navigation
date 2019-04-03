// PoiName.cpp: implementation of the CPoiName class.
//
//////////////////////////////////////////////////////////////////////

#include "PoiName.h"
#include "RoadName.h"
#include "libDrawMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoiName::CPoiName()
{
	m_pRoadName	= NULL;
	m_pPoiList	= NULL;
	m_nPoiCount	= 0;
	m_nPoiIndex = 0;
	m_nMapType	= MAP_DEFAULT;
	//
	m_nPoiTotalTypeCount = 0;
	m_nPoiType1Count     = 0;
	m_pPoiType2CountList = NULL;
	m_pformerList = NULL;
}

CPoiName::~CPoiName()
{
	Free();
	if (NULL != m_pPoiType2CountList)
	{
		delete []m_pPoiType2CountList;
		m_pPoiType2CountList = NULL;
	}
}

nuBOOL CPoiName::Initialize(class CRoadName* pRoadName, PPOISETTING	pPoiSetting, nuBYTE nMapType/* = MAP_DEFAULT*/ )
{
	m_nScaleCount   = 0;
	m_pPriorityInfo = NULL;
	//
	/*if( ReadPoiPriorityFile() )
	{
		m_bPriority = nuTRUE;
		m_pPoiList = new POINODE[_POI_MAX_COUNTS_];
		if( NULL == m_pPoiList )
		{
			return nuFALSE;
		}
		nuMemset(m_pPoiList, 0, sizeof(POINODE)*_POI_MAX_COUNTS_);
		m_nMaxPoiCount = _POI_MAX_COUNTS_;
		m_pformerList = new POINODE[25];
		m_pformerCount = 0;
		nuMemset(m_pformerList,0,sizeof(POINODE)*25);
	}
	else*/
	{
		m_bPriority = nuFALSE;
		m_pPoiList = (PPOINODE)g_pDMLibMM->MM_GetStaticMemMass(sizeof(POINODE)*g_pDMMapCfg->commonSetting.nMaxPOINameCount);
		if( m_pPoiList == NULL )
		{
			return nuFALSE;
		}
		nuMemset(m_pPoiList, 0, sizeof(POINODE)*g_pDMMapCfg->commonSetting.nMaxPOINameCount);
	}
	m_pRoadName		= pRoadName;
	m_pPoiSetting	= pPoiSetting;
	m_nPoiCount		= g_pDMMapCfg->commonSetting.nMaxPOINameCount;
	m_nMapType		= nMapType;
	//
	if( !nuReadConfigValue("MAXOFSAMEPOIICON", &m_nMaxNumofSameIcon) )
	{
		m_nMaxNumofSameIcon = 3;
	}
	return nuTRUE;
} 

nuVOID CPoiName::Free()
{
	if(m_pformerList)
	{
		delete[] m_pformerList;
		m_pformerList = NULL;
	}
	m_pRoadName	= NULL;
	if (m_bPriority && m_pPoiList)
	{
		delete[] m_pPoiList;
		m_pPoiList = NULL;
	}

	m_nPoiCount	= 0;
	m_nPoiIndex = 0;
	//
	if( NULL != m_pPriorityInfo )
	{
		for( nuWORD i = 0; i < m_nScaleCount; i++ )
		{
			if( NULL != m_pPriorityInfo[i].pPoiPriority )
			{
				delete []m_pPriorityInfo[i].pPoiPriority;
				m_pPriorityInfo[i].pPoiPriority = NULL;
			}
		}
		delete []m_pPriorityInfo;
		m_pPriorityInfo = NULL;
	}
}

#define MAX_NUM_OF_ONETYPE1						3
#define ICONDIS									16
#define POI_BAN_CUT								2
#define POI_HIGH								16
#define POI_SPECIAL_HIGHT						32
nuINT CPoiName::AddPoiNode(nuBYTE nType1, nuBYTE nType2, nuBYTE nNameLen, nuWORD nDwIdx, nuDWORD nNameAddr,
						 nuBYTE nPoiType, nuWORD cityID, nuDWORD nIcon, nuroPOINT ptPos)
{
	if( m_bPriority )
	{
		return AddPoiNodeEx(nType1, nType2, nNameLen, nDwIdx, nNameAddr, nPoiType, cityID, nIcon, ptPos, 0);
	}	
	//
	if( m_pPoiList == NULL || m_nPoiCount == m_nPoiIndex || m_pPoiSetting == NULL)
	{
		return ADD_RES_FULL;
	}
	if( nDwIdx == -1 || nNameAddr == -1 || nType1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
	{
		return ADD_RES_FAIL;
	}
	NURORECT rtName = {0}, rtScreen = {0};
	nuINT nFH = m_pPoiSetting[nType1].nFontSize;
	nuINT nFW = nFH/2;
	nuBOOL b3Dmode = 0;
	nuBYTE byMode = 0;
	byMode = 0;
	if( m_pPoiSetting[nType1].nShowName )
	{
		byMode |= POI_SHOW_NAME;
	}
	if( nPoiType || m_pPoiSetting[nType1].nShowIcon )
	{
		byMode |= POI_SHOW_ICON;
	}
	if( !byMode )
	{
		return ADD_RES_FAIL;
	}
	if( TypeIsFull(nType1, nType2) )
	{
		return ADD_RES_FAIL;
	} 
	nuINT nPoiIconH = 0;
	if( nPoiType )
	{
		nPoiIconH = POI_SPECIAL_HIGHT;
	}
	else
	{
		nPoiIconH = POI_HIGH;
	}
	if( m_nMapType == MAP_DEFAULT )
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW * nNameLen / POI_BAN_CUT;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFH + nPoiIconH;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - 
						  nFW * nNameLen * (POI_BAN_CUT - 1) / POI_BAN_CUT;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - 10;
		b3Dmode = g_pDMGdata->uiSetData.b3DMode;
		if( b3Dmode )
		{
			rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
		}
		g_pDMLibMT->MT_MapToBmp(ptPos.x, ptPos.y, &ptPos.x, &ptPos.y);
	}
	else//Zoom map
	{
		rtScreen.left	= 0 + nFW * nNameLen / POI_BAN_CUT;
		rtScreen.top	= 0 + nFH + nPoiIconH;
		rtScreen.right	= g_pDMGdata->zoomScreenData.nZoomBmpWidth - nFW * nNameLen * (POI_BAN_CUT - 1) / POI_BAN_CUT;
		rtScreen.bottom	= g_pDMGdata->zoomScreenData.nZoomBmpHeight;
		b3Dmode = g_pDMGdata->zoomScreenData.b3DMode;
		if( b3Dmode )
		{
			rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
		}
		g_pDMLibMT->MT_ZoomMapToBmp(ptPos.x, ptPos.y, &ptPos.x, &ptPos.y);
	}
	if( ptPos.y > rtScreen.bottom )
	{
		return ADD_RES_FAIL;
	}
	if( b3Dmode )
	{
		g_pDMLibMT->MT_Bmp2Dto3D(ptPos.x, ptPos.y, m_nMapType);
	}
	if( !nuPointInRect(&ptPos, &rtScreen) )
	{
		return ADD_RES_FAIL;
	}

	nuBYTE nIconType = 1;
	if(nPoiType !=0 && g_pDMGdata->uiSetData.b3DMode)//
	{
		nuUINT iLocalIcon = (ptPos.y-g_pDMGdata->uiSetData.nSkyHeight)*5/(rtScreen.bottom-g_pDMGdata->uiSetData.nSkyHeight);
		//nIcon = cityID*255 + iLocalIcon*255 + nPoiType;
		switch(iLocalIcon)
		{
		case 0:
			nIcon = 3826;
			nIconType = 1;
			break;
		case 1:
			nIcon = 4336;
			nIconType = 3;
			break;
		case 2:
			nIcon = 4846;
			nIconType = 5;
			break;
		case 3:
			nIcon = 4846;
			nIconType = 5;
			break;
		case 4:
			nIcon = 4846;
			nIconType = 5;
			break;
		}
		if(this->m_nMapType == MAP_ZOOM)
		{
			nIcon = 4846;
			nIconType = 5;
		}
	}

	rtName.left		= ptPos.x - nNameLen*nFW/2;
	rtName.right	= ptPos.x + nNameLen*nFW/2;
	rtName.top		= ptPos.y - nFH - nPoiIconH*(nIconType+3)/4;
	rtName.bottom	= ptPos.y - nPoiIconH*(nIconType+3)/4;

	if( !CheckPoiRect(rtName, ptPos, &byMode) )
	{
		return ADD_RES_FAIL;
	}
	if( IsPoiOverlaped(ptPos.x, ptPos.y) )
	{
		return ADD_RES_FAIL;
	}
	if( m_pRoadName != NULL && (byMode&POI_SHOW_NAME) )
	{
		if( m_pRoadName->CheckRect(&rtName) )
		{
			byMode &= ~POI_SHOW_NAME;
		}
	}
	
	m_pPoiList[m_nPoiIndex].nDwIdx	= nDwIdx;
	m_pPoiList[m_nPoiIndex].nIcon	= nIcon;
	m_pPoiList[m_nPoiIndex].nMode	= byMode;
	m_pPoiList[m_nPoiIndex].nNameAddr	= nNameAddr;
	m_pPoiList[m_nPoiIndex].nType1	= nType1;
	m_pPoiList[m_nPoiIndex].nType2	= nType2;
	m_pPoiList[m_nPoiIndex].ptPos	= ptPos;
	m_pPoiList[m_nPoiIndex].rtNAME	= rtName;
	m_pPoiList[m_nPoiIndex].nPoiType = nPoiType;
	m_pPoiList[m_nPoiIndex].nIconType = nIconType;
	m_nPoiIndex++;
	return ADD_RES_NEW;
}

nuBOOL CPoiName::TypeIsFull(nuBYTE nType1, nuBYTE nType2)
{
	nuINT nType1Num = 0;
	for(nuINT i = 0; i < m_nPoiIndex; i++)
	{
		if( m_pPoiList[i].nType1 == nType1 )
		{
			if(m_pPoiList[i].nPoiType == 0)
			{
				nType1Num ++;
			}
			if( nType1Num == MAX_NUM_OF_ONETYPE1 )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}
nuBOOL CPoiName::CheckPoiRect(nuroRECT rect, const NUROPOINT &pt, nuBYTE* pByMode)
{
	nuBYTE bDrawName = (*pByMode)&POI_SHOW_NAME;
	nuBYTE bDrawIcon = (*pByMode)&POI_SHOW_ICON;
	for(nuINT i = 0; i < m_nPoiIndex; i++)
	{
		if( bDrawIcon && (m_pPoiList[i].nMode&POI_SHOW_ICON) )
		{
			nuINT len1 = pt.x - m_pPoiList[i].ptPos.x; 
			nuINT len2 = pt.y - m_pPoiList[i].ptPos.y;
			len1 = NURO_ABS(len1);
			len2 = NURO_ABS(len2);
			if( len1 < ICONDIS*(m_pPoiList[i].nIconType+3)/4 && len2 < ICONDIS*(m_pPoiList[i].nIconType+3)/4 )
			{
				return nuFALSE;
			}
		}
		if( bDrawName && (m_pPoiList[i].nMode&POI_SHOW_NAME) )
		{
			if( nuRectCoverRect(&rect, &m_pPoiList[i].rtNAME) )
			{
				*pByMode = (*pByMode)&(~POI_SHOW_NAME);
				if( *pByMode )
				{
					return nuTRUE;
				}
				else
				{
					return nuFALSE;
				}
			}
		}
	}
	return nuTRUE;
}

nuVOID CPoiName::ResetPoiList()
{
	m_nPoiIndex = 0;
}


#define POIICONSW				32
#define POIICONSH				32

#define POIICONW				16
#define POIICONH				16
nuBOOL CPoiName::DrawPoi()
{
	if( m_bPriority )
	{
		SelectPoiNode();
	}
	if( m_pPoiList == NULL || m_pPoiSetting == NULL || m_nPoiIndex == 0 )
	{
		return nuFALSE;
	}
	m_pformerCount = (m_nPoiIndex>25)?25:m_nPoiIndex;
	//nuMemcpy(m_pformerList, m_pPoiList, sizeof(POINODE) * m_pformerCount);
	
	nuWCHAR wsName[NURO_MAX_PATH]={0};
	nuWORD	nNameLen = NURO_MAX_PATH;//sizeof(wsName);//
	nuBOOL* pBdraw = new nuBOOL[m_nPoiIndex];
	if( pBdraw == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pBdraw, 0, sizeof(nuBOOL)*m_nPoiIndex);
	nuBYTE n3Dmode = 0;
	nuLONG nInOut = 0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuDWORD nIconSize = 0;
	if( m_nMapType == MAP_DEFAULT )
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	else
	{
		n3Dmode = g_pDMGdata->zoomScreenData.b3DMode;
	}
	for(nuINT i = 0; i < m_nPoiIndex; i++)
	{
		nuINT j = i;
		nuLONG nYalue	= 10000;
		for( nuINT k = 0; k < m_nPoiIndex; k ++ )
		{
			if( !pBdraw[k] && m_pPoiList[k].ptPos.y < nYalue )
			{
				j = k;
				nYalue = m_pPoiList[k].ptPos.y;
			}
		}
		if( pBdraw[j] )
		{
			continue;
		}
		pBdraw[j] = nuTRUE;
		nIconSize = 0;
		if( m_pPoiList[j].nMode & POI_SHOW_ICON )
		{
			//Draw poi Icon....
			if( m_pPoiList[j].nPoiType != 0 )
			{
				m_drawSpPoi.OpenBmpFile(g_pDMGdata->pTsPath);
				nIconSize  = m_drawSpPoi.DrawSpPOI(m_pPoiList[j].ptPos.x, 
					m_pPoiList[j].ptPos.y,
					m_pPoiList[j].nPoiType);
			}
			if( !m_pPoiList[j].nPoiType && !nIconSize )
			{
				if( n3Dmode && m_pPoiList[j].ptPos.y < g_pDMGdata->transfData.nBmpHeight/2 ) //Small Poi Icon
				{
					nIconSize = g_pDMLibGDI->GdiDrawPoi( m_pPoiList[j].ptPos.x, 
						m_pPoiList[j].ptPos.y, 
						m_pPoiList[j].nIcon - POI_ICON_START_NUM, 
						NURO_RTPOS_CB, nuTRUE ); 
				}
				else
				{
					nIconSize = g_pDMLibGDI->GdiDrawPoi( m_pPoiList[j].ptPos.x, 
						m_pPoiList[j].ptPos.y, 
						m_pPoiList[j].nIcon - POI_ICON_START_NUM, 
						NURO_RTPOS_CB, nuFALSE );
				}
			}
			/*
			if(m_pPoiList[j].nPoiType != 0 && g_pDMGdata->uiSetData.b3DMode)
			{
				g_pDMLibGDI->GdiDrawIconSp(
					m_pPoiList[j].ptPos.x - POIICONSW,// - POIICONW*(m_pPoiList[j].nIconType+3)/4,
					m_pPoiList[j].ptPos.y - POIICONSW,// - POIICONH*(m_pPoiList[j].nIconType+3)/4,
					m_pPoiList[j].nIcon,
					g_pDMGdata->uiSetData.bNightOrDay);
			}
			else
			{
				g_pDMLibGDI->GdiDrawIcon( 
					m_pPoiList[j].ptPos.x - POIICONW, 
					m_pPoiList[j].ptPos.y - POIICONH,
					m_pPoiList[j].nIcon );
			}
			*/
		}
		if( (m_pPoiList[j].nMode & POI_SHOW_NAME) &&
			g_pDMLibFS->FS_GetPoiName(m_pPoiList[j].nDwIdx, m_pPoiList[j].nNameAddr, wsName, &nNameLen) )
		{
			//Draw poi Name....
			NURORECT rtText  = {0};
			rtText = m_pPoiList[j].rtNAME;
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_POI_FONT, n3Dmode, &m_pPoiSetting[m_pPoiList[j].nType1], &nInOut) != DRAW_OBJ_POINAME )
			{
				continue;
			}
			nuBOOL bDrawRim = (nuBOOL)m_pPoiSetting[m_pPoiList[j].nType1].nFontBgTransparent;
			nuCOLORREF nRimClr = nuRGB(m_pPoiSetting[m_pPoiList[j].nType1].nFontBgColorR,
				m_pPoiSetting[m_pPoiList[j].nType1].nFontBgColorG,
				m_pPoiSetting[m_pPoiList[j].nType1].nFontBgColorB );
			g_pDMLibGDI->GdiDrawTextW(wsName, -1, &rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE);
			nuLONG len = rtText.right - rtText.left;
			if( m_pPoiList[j].nPoiType != 0 )
			{
				rtText.left		= m_pPoiList[j].ptPos.x - len/2;
				rtText.right	= rtText.left + len;
			}
			else
			{
				rtText.left		= m_pPoiList[j].ptPos.x - len/2;// - POIICONW/2;
				rtText.right	= rtText.left + len;// - POIICONW/2;
			}
			len = rtText.bottom - rtText.top;
            nuWCHAR chiwy[64]={0};
           // nuWcscpy(chiwy, nuTEXT("喪帣适"));
			if (0 == nuWcscmp(chiwy, wsName))
			{
				rtText.top		= m_pPoiList[j].ptPos.y - len - nuLOWORD(nIconSize) + 50;
				rtText.bottom	= m_pPoiList[j].ptPos.y - nuLOWORD(nIconSize) + 50;
			}
			else
			{
			rtText.top		= m_pPoiList[j].ptPos.y - len - nuLOWORD(nIconSize);
			rtText.bottom	= m_pPoiList[j].ptPos.y - nuLOWORD(nIconSize);
			}	
//			g_pDMLibGDI->GdiDrawTextW(wsName, -1, &rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE | NURO_DT_VCENTER);
			g_pDMLibGDI->GdiDrawTextWEx(wsName, -1, &rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE | NURO_DT_VCENTER, bDrawRim, nRimClr);
		}
	}
	m_drawSpPoi.CloseBmpFile();
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	delete []pBdraw;
	pBdraw=NULL;
	return nuTRUE;
}

// --- Added by Dengxu @ 2013 01 02
nuBOOL CPoiName::DrawPoiIcon_DX()
{
	if( m_bPriority )
	{
		SelectPoiNode();
	}
	if( m_pPoiList == NULL || m_pPoiSetting == NULL || m_nPoiIndex == 0 )
	{
		return nuFALSE;
	}
	nuWCHAR wsName[NURO_MAX_PATH]={0};
	nuWORD	nNameLen = sizeof(wsName);//NURO_MAX_PATH;
	nuBOOL* pBdraw = new nuBOOL[m_nPoiIndex];
	if( pBdraw == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pBdraw, 0, sizeof(nuBOOL)*m_nPoiIndex);
	nuBYTE n3Dmode = 0;
	nuLONG nInOut = 0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuDWORD nIconSize = 0;
	if( m_nMapType == MAP_DEFAULT )
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	else
	{
		n3Dmode = g_pDMGdata->zoomScreenData.b3DMode;
	}
	for(nuINT i = 0; i < m_nPoiIndex; i++)
	{
		nuINT j = i;
		nuLONG nYalue	= 10000;
		for( nuINT k = 0; k < m_nPoiIndex; k ++ )
		{
			if( !pBdraw[k] && m_pPoiList[k].ptPos.y < nYalue )
			{
				j = k;
				nYalue = m_pPoiList[k].ptPos.y;
			}
		}
		if( pBdraw[j] )
		{
			continue;
		}
		pBdraw[j] = nuTRUE;
		nIconSize = 0;
		if( m_pPoiList[j].nMode & POI_SHOW_ICON )
		{
			//Draw poi Icon....
			if( m_pPoiList[j].nPoiType != 0 )
			{
				m_drawSpPoi.OpenBmpFile(g_pDMGdata->pTsPath);
				nIconSize  = m_drawSpPoi.DrawSpPOI(m_pPoiList[j].ptPos.x, 
					m_pPoiList[j].ptPos.y,
					m_pPoiList[j].nPoiType);
			}
			if( !m_pPoiList[j].nPoiType && !nIconSize )
			{
				if( n3Dmode && m_pPoiList[j].ptPos.y < g_pDMGdata->transfData.nBmpHeight/2 ) //Small Poi Icon
				{
					nIconSize = g_pDMLibGDI->GdiDrawPoi( m_pPoiList[j].ptPos.x, 
						m_pPoiList[j].ptPos.y, 
						m_pPoiList[j].nIcon - POI_ICON_START_NUM, 
						NURO_RTPOS_CB, nuTRUE ); 
				}
				else
				{
					nIconSize = g_pDMLibGDI->GdiDrawPoi( m_pPoiList[j].ptPos.x, 
						m_pPoiList[j].ptPos.y, 
						m_pPoiList[j].nIcon - POI_ICON_START_NUM, 
						NURO_RTPOS_CB, nuFALSE );
				}
			}
		}
	}
	m_drawSpPoi.CloseBmpFile();
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	delete []pBdraw;
	pBdraw=NULL;
	return nuTRUE;
}


#define MAX_DISTANCE_POI       80
nuBOOL CPoiName::IsPoiOverlaped(nuLONG x, nuLONG y)
{
	if( 0 == m_nPoiIndex || NULL == m_pPoiList )
	{
		return nuFALSE;
	}
	else
	{
		for( int i = 0; i < m_nPoiIndex; i++ )
		{
			if( NURO_ABS(x - m_pPoiList[i].ptPos.x) <= MAX_DISTANCE_POI &&
				NURO_ABS(y - m_pPoiList[i].ptPos.y) <= MAX_DISTANCE_POI )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuBOOL CPoiName::IsPoiOverlaped(POINODE* pNode)
{
	if (NULL == m_pPoiList || NULL == pNode)
	{
		return nuFALSE;
	}
	if (0 == m_nPoiIndex)
	{
		if (m_pRoadName != NULL)
		{
			if (m_pRoadName->CheckRect(&pNode->rtNAME))
			{
				pNode->nMode &= ~POI_SHOW_NAME;
			}
		}
	}
	for( int i = 0; i < m_nPoiIndex; i++ )
	{
		if( NURO_ABS(pNode->ptPos.x - m_pPoiList[i].ptPos.x) <= MAX_DISTANCE_POI &&
				NURO_ABS(pNode->ptPos.y - m_pPoiList[i].ptPos.y) <= MAX_DISTANCE_POI )
		{
			return nuTRUE;
		}
		if (m_pRoadName != NULL)
		{
			if (m_pRoadName->CheckRect(&pNode->rtNAME))
			{
				pNode->nMode &= ~POI_SHOW_NAME;
			}
		}
		if ((pNode->nMode & POI_SHOW_NAME) && (m_pPoiList[i].nMode & POI_SHOW_NAME))
		{
			if (nuRectCoverRect((&pNode->rtNAME),&(m_pPoiList[i].rtNAME )))
			{
				pNode->nMode &= ~POI_SHOW_NAME;
			}
		}
	}
	return nuFALSE;
}
nuINT CPoiName::AddPoiNodeEx(nuBYTE nType1, nuBYTE nType2, nuBYTE nNameLen, nuWORD nDwIdx, nuDWORD nNameAddr, 
							 nuBYTE nPoiType, nuWORD cityID, nuDWORD nIcon, nuroPOINT ptPos, nuINT nPriority)
{
	if( m_pPoiList == NULL || m_nMaxPoiCount == m_nPoiIndex || m_pPoiSetting == NULL)
	{
		return ADD_RES_FULL;
	}
	if( nDwIdx == -1 || nNameAddr == -1 || nType1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
	{
		return ADD_RES_FAIL;
	}
	//
	NURORECT rtName = {0}, rtScreen = {0};
	nuINT nFH = m_pPoiSetting[nType1].nFontSize;
	nuINT nFW = nFH/2;
	nuBOOL b3Dmode = 0;
	nuBYTE byMode = 0;
	byMode = 0;
	//if( m_pPoiSetting[nType1].nShowName )
	{
		byMode |= POI_SHOW_NAME;
	}
	//if( nPoiType || m_pPoiSetting[nType1].nShowIcon )
	{
		byMode |= POI_SHOW_ICON;
	}
	if( !byMode )
	{
		return ADD_RES_FAIL;
	}
	nuINT nPoiIconH = 0;
	if( nPoiType )
	{
		nPoiIconH = POI_SPECIAL_HIGHT;
	}
	else
	{
		nPoiIconH = POI_HIGH;
	}
	if( m_nMapType == MAP_DEFAULT )
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW * nNameLen / POI_BAN_CUT;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFH + nPoiIconH;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - 
			nFW * nNameLen * (POI_BAN_CUT - 1) / POI_BAN_CUT;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - 10;
		b3Dmode = g_pDMGdata->uiSetData.b3DMode;
		if( b3Dmode )
		{
			rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
		}
		g_pDMLibMT->MT_MapToBmp(ptPos.x, ptPos.y, &ptPos.x, &ptPos.y);
	}
	else//Zoom map
	{
		rtScreen.left	= 0 + nFW * nNameLen / POI_BAN_CUT;
		rtScreen.top	= 0 + nFH + nPoiIconH;
		rtScreen.right	= g_pDMGdata->zoomScreenData.nZoomBmpWidth - nFW * nNameLen * (POI_BAN_CUT - 1) / POI_BAN_CUT;
		rtScreen.bottom	= g_pDMGdata->zoomScreenData.nZoomBmpHeight;
		b3Dmode = g_pDMGdata->zoomScreenData.b3DMode;
		if( b3Dmode )
		{
			rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
		}
		g_pDMLibMT->MT_ZoomMapToBmp(ptPos.x, ptPos.y, &ptPos.x, &ptPos.y);
	}
	if( ptPos.y > rtScreen.bottom )
	{
		return ADD_RES_FAIL;
	}
	if( b3Dmode )
	{
		g_pDMLibMT->MT_Bmp2Dto3D(ptPos.x, ptPos.y, m_nMapType);
	}
	if( !nuPointInRect(&ptPos, &rtScreen) )
	{
		return ADD_RES_FAIL;
	}
	nuBYTE nIconType = 1;
	if( nPoiType !=0 && g_pDMGdata->uiSetData.b3DMode )
	{
		nuUINT iLocalIcon = (ptPos.y-g_pDMGdata->uiSetData.nSkyHeight)*5/(rtScreen.bottom-g_pDMGdata->uiSetData.nSkyHeight);
		//nIcon = cityID*255 + iLocalIcon*255 + nPoiType;
		switch(iLocalIcon)
		{
		case 0:
			nIcon = 3826;
			nIconType = 1;
			break;
		case 1:
			nIcon = 4336;
			nIconType = 3;
			break;
		case 2:
			nIcon = 4846;
			nIconType = 5;
			break;
		case 3:
			nIcon = 4846;
			nIconType = 5;
			break;
		case 4:
			nIcon = 4846;
			nIconType = 5;
			break;
		}
		if(this->m_nMapType == MAP_ZOOM)
		{
			nIcon = 4846;
			nIconType = 5;
		}
	}
	rtName.left		= ptPos.x - nNameLen*nFW/2;
	rtName.right	= ptPos.x + nNameLen*nFW/2;
	rtName.top		= ptPos.y - nFH - nPoiIconH*(nIconType+3)/4 - 26;
	rtName.bottom	= ptPos.y - nPoiIconH*(nIconType+3)/4 - 26;

// 	if( !CheckPoiRect(rtName, ptPos, &byMode) )
// 	{
// 		return ADD_RES_FAIL;
// 	}
// 	if( m_pRoadName != NULL && (byMode&POI_SHOW_NAME) )
// 	{
// 		if( m_pRoadName->CheckRect(&rtName) )
// 		{
// 			byMode &= ~POI_SHOW_NAME;
// 		}
// 	}

	nPriority = GetPriorityByType(nType1, nType2);
	if( -1 == nPriority )
	{
		return ADD_RES_FAIL;
	}
	m_pPoiList[m_nPoiIndex].nDwIdx	  = nDwIdx;
	m_pPoiList[m_nPoiIndex].nIcon	  = nIcon;
	m_pPoiList[m_nPoiIndex].nMode	  = byMode;
	m_pPoiList[m_nPoiIndex].nNameAddr = nNameAddr;
	m_pPoiList[m_nPoiIndex].nType1	  = nType1;
	m_pPoiList[m_nPoiIndex].nType2	  = nType2;
	m_pPoiList[m_nPoiIndex].ptPos	  = ptPos;
	m_pPoiList[m_nPoiIndex].rtNAME	  = rtName;
	m_pPoiList[m_nPoiIndex].nPoiType  = nPoiType;
	m_pPoiList[m_nPoiIndex].nIconType = nIconType;
	m_pPoiList[m_nPoiIndex].nPriority = nPriority;

	m_nPoiIndex++;

	return ADD_RES_NEW;
}

//齬唗
nuBOOL CPoiName::SortPoiNode()
{
	POINODE tempNode = {0};
	nuMemset(&tempNode, NULL, sizeof(POINODE));

	nuWORD i = 0, j = 0;
	for (i=0;i<m_pformerCount;i++)
	{
		for(j=0;j<m_nPoiIndex;j++)
		{
			if (m_pformerList[i].nNameAddr == m_pPoiList[j].nNameAddr)
			{
				m_pPoiList[j].nPriority = 0;
			}
		}
	}
	for( i = 0; i < m_nPoiIndex; i++ )
	{
		for( j = i; j < m_nPoiIndex; j++ )
		{
			if( m_pPoiList[j].nPriority < m_pPoiList[i].nPriority )
			{
				tempNode = m_pPoiList[i];
				m_pPoiList[i] = m_pPoiList[j];
				m_pPoiList[j] = tempNode;
			}
		}
	}
	return nuTRUE;
}

nuBOOL CPoiName::SelectPoiNode()
{
	//SortPoiNode();

	nuBYTE nScaleIdx = g_pDMLibFS->pGdata->uiSetData.nScaleIdx;

	//笭詁齬壺
	if( 0 == m_nPoiIndex || NULL == m_pPoiList )
	{
		return nuFALSE;
	}
	PPOINODE pNode = new POINODE[m_nPoiIndex];
	if( NULL == m_pPoiList )
	{
		return nuFALSE;
	}
	nuMemset(pNode, 0, sizeof(POINODE)*m_nPoiIndex);
	//
	nuWORD nPoiIndex = m_nPoiIndex;
	nuWORD i = 0;
	nuMemcpy(pNode, m_pPoiList, sizeof(POINODE) * m_nPoiIndex);
	for( i = 0; i < nPoiIndex; i++ )
	{
		nuMemset(&m_pPoiList[i], 0, sizeof(POINODE));
	}
	//
// 	m_pPoiList[0] = pNode[0];
	m_nPoiIndex = 0;
	nuUINT j = 0;
	for( j = 0; j < nPoiIndex; j++ )
	{
		if( m_nPoiIndex >= m_pPriorityInfo[nScaleIdx-1].nPoiCount )
		{
			break;
		}
		if( PoiIconIsFull(pNode[j].nIcon) )
		{
			continue;
		}
		//if( !IsPoiOverlaped(pNode[j].ptPos.x, pNode[j].ptPos.y) )
		if (!IsPoiOverlaped(&pNode[j]))
		{
			m_pPoiList[m_nPoiIndex] = pNode[j];
			m_nPoiIndex++;
		}
	}
	delete[] pNode;
	pNode = NULL;
	return nuTRUE;
}

nuBOOL CPoiName::ReadPoiPriorityFile()
{
	nuTCHAR tsFile[NURO_MAX_PATH]={0};
	nuTcscpy(tsFile, g_pDMLibFS->pGdata->pTsPath);
	nuTcscat(tsFile, _POI_PRIORITY_FILE_NAME_);
	//
	nuFILE* pFile = NULL;
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( NULL == pFile )
	{
		return nuFALSE;
	}
	nuSIZE nFileLen = nuFgetlen(pFile);
	if( nFileLen <= 0 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuBYTE *pData = (nuBYTE*)nuMalloc(nFileLen);
	if( NULL == pData )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( 1 != nuFread(pData, nFileLen, 1, pFile) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	//Read Scale Count
	nuSIZE i = 0, j = 0;
	nuCHAR sScaleCnt[10] = {0};
	while( i < nFileLen )
	{
		i = ReadScaleCount(pData, nFileLen);
		if( m_nScaleCount <= 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		//
		if( NULL == m_pPriorityInfo )
		{
			m_pPriorityInfo = new POI_PRIORITY_INFO[m_nScaleCount];
			if( NULL == m_pPriorityInfo )
			{
				return nuFALSE;
			}
			nuMemset(m_pPriorityInfo, 0, sizeof(POI_PRIORITY_INFO)*m_nScaleCount);
			for( nuWORD nScaleIdx = 0; nScaleIdx < m_nScaleCount; nScaleIdx++ )
			{
				i += DataAnalyse(&pData[i], nFileLen - i, nScaleIdx);
			}
		}
	}
	nuFclose(pFile);
	nuFree(pData);
	return nuTRUE;
}

nuUINT CPoiName::ReadScaleCount(nuBYTE *pData, nuUINT nDataLen)
{
	nuCHAR sScaleCnt[10] = {0};
	nuUINT i = 0, j = 0;
	for( i = 0; i < nDataLen; i++ )
	{
		if( '=' == pData[i] )
		{
			i++;
			while( !IsLineEnd(pData, nDataLen, i) )
			{
				sScaleCnt[j] = pData[i];
				i++;
				j++;
			}
			m_nScaleCount = nuStrtol(sScaleCnt, NULL, 10);
			break;
		}
	}
	return i;
}

nuUINT CPoiName::DataAnalyse(nuBYTE *pData, nuUINT nDataLen, nuWORD nScaleIdx)
{
	if (0 == m_nPoiTotalTypeCount)
	{
		ReadPTNode();
	}
	nuUINT i = 0, j = 0;
	nuCHAR sScaleIdx[10] = {0};
	nuCHAR sPoiCnt[10] = {0};
	nuCHAR sType1[10] = {0};
	nuCHAR sType2[10] = {0};
	nuBYTE byType1 = 0;
	nuBOOL bAllType2Priority = nuFALSE, bOtherType2Priority =nuFALSE;
	nuWORD nPriorityCount = 0;
	//
	int k = 0;
	for( i = 0; i < nDataLen; i++ )
	{
		while( '#' == pData[i] )//Skipping line, calculate the length of line
		{
			++i;
			while ( i < nDataLen - 1 )
			{
				if( 0x0D == pData[i] && 0x0A == pData[i+1] )
				{
					i += 2;
					break;
				}
				++i;
			}
		}
		//----- 黍絞俴腔nScaleIdx -----
		if( 48 <= pData[i] && 57 >= pData[i] )
		{
			j = 0;
			while( !IsLineEnd(pData, nDataLen, i) && ':' != pData[i] )
			{
				sScaleIdx[j] = pData[i];
				i++;
				j++;
			}
			nScaleIdx = nuStrtol(sScaleIdx, NULL, 10);
			//
			if (nScaleIdx < 0 || nScaleIdx > m_nScaleCount - 1)

			{
				break;
			}
			if (m_nPoiTotalTypeCount)
			{
				m_pPriorityInfo[nScaleIdx].pPoiPriority = new POI_PRIORITY[m_nPoiTotalTypeCount];
			}	
			if( NULL == m_pPriorityInfo[0].pPoiPriority )
			{
				break;
			}
		}
		//----- 黍藩跺掀瞰喜狟郔嗣褫眕珆尨腔POI跺杅 -----
		if( ':' == pData[i] )
		{
			i++;
			j = 0;
			while( !IsLineEnd(pData, nDataLen, i) && '=' != pData[i] )
			{
				sPoiCnt[j] = pData[i];
				i++;
				j++;
			}
			m_pPriorityInfo[nScaleIdx].nPoiCount = nuStrtol(sPoiCnt, NULL, 10);
		}
		//----- 黍POI腔湮煦濬睿苤煦濬 -----
		if( '=' == pData[i] )
		{
			i++;
			while( i < nDataLen && !IsLineEnd(pData, nDataLen, i) )
			{
				if (k >= m_nPoiTotalTypeCount)
				{
					break;
				}
				j = 0;
				nuMemset(&sType1, 0, sizeof(sType1));
				while( ',' != pData[i] )
				{
					sType1[j] = pData[i];
					i++;
					j++;
				}
				byType1 = nuStrtol(sType1, NULL, 10);
				//
				if( ',' == pData[i] )
				{
					i++;
					j = 0;
					bAllType2Priority = nuFALSE;
					bOtherType2Priority = nuFALSE;
					nuMemset(&sType2, 0, sizeof(sType2));
					while( ';' != pData[i] )
					{
						sType2[j] = pData[i];
						if( 'a' == sType2[j] || 'A' == sType2[j] )
						{
							bAllType2Priority = nuTRUE;
						}
						else if( 'b' == sType2[j] || 'B' == sType2[j] )
						{
							bOtherType2Priority = nuTRUE;
						}
						i++;
						j++;
					}
					//--------------------------------------------
					if (bAllType2Priority || bOtherType2Priority)
					{
						bAllType2Priority = nuFALSE;
						bOtherType2Priority = nuFALSE;
						if (m_pPoiType2CountList && byType1 < m_nPoiType1Count)
						{
							for (nuLONG n = 0; n < m_pPoiType2CountList[byType1]; n++)
							{
								if (IsOtherType2(byType1, n, nScaleIdx, k))
								{
									m_pPriorityInfo[nScaleIdx].pPoiPriority[k].byTypeID1 = byType1;
									m_pPriorityInfo[nScaleIdx].pPoiPriority[k].byTypeID2 = n;
									m_pPriorityInfo[nScaleIdx].pPoiPriority[k].nPriority = nPriorityCount;
									k++;
								}
							}
							nPriorityCount++;
						}
					}
					else
					{
						nuBYTE byType2 = 0;
						byType2 = nuStrtol(sType2, NULL, 10);
						if (byType1 < m_nPoiType1Count && byType2 < m_pPoiType2CountList[byType1])
						{
							if (IsOtherType2(byType1, byType2, nScaleIdx, k))
							{
								m_pPriorityInfo[nScaleIdx].pPoiPriority[k].byTypeID1 = byType1;
								m_pPriorityInfo[nScaleIdx].pPoiPriority[k].byTypeID2 = byType2;
								m_pPriorityInfo[nScaleIdx].pPoiPriority[k].nPriority = nPriorityCount;
								k++;
								nPriorityCount++;
							}
						}
					}
				}
				i++;
			}
			m_pPriorityInfo[nScaleIdx].nPriorityCount = k;
			break;
		}	
	}
	return i;
}

nuBOOL CPoiName::IsLineEnd(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i)
{
	if( 0x0D == pData[i] && i < (nDataLen-1) && 0x0A == pData[i+1] )
	{
		i += 2;
		return nuTRUE;
	}
	return nuFALSE;
}

nuINT CPoiName::GetPriorityByType(nuBYTE byType1, nuBYTE byType2)
{
	nuBYTE nScaleIdx = g_pDMLibFS->pGdata->uiSetData.nScaleIdx;
	//
	if( 0 == m_pPriorityInfo[nScaleIdx-1].nPoiCount ||
		0 == m_pPriorityInfo[nScaleIdx-1].nPriorityCount ||
		NULL == m_pPriorityInfo[nScaleIdx-1].pPoiPriority )
	{
		return -1;
	}
	//
	for( nuWORD i = 0; i < m_pPriorityInfo[nScaleIdx-1].nPriorityCount; i++ )
	{
		if( byType1 == m_pPriorityInfo[nScaleIdx-1].pPoiPriority[i].byTypeID1 && 
			byType2 == m_pPriorityInfo[nScaleIdx-1].pPoiPriority[i].byTypeID2 )
		{
			return m_pPriorityInfo[nScaleIdx-1].pPoiPriority[i].nPriority+10;
		}
	}
	return -1;
}

nuBOOL CPoiName::IsOtherType2(nuBYTE byType1, nuBYTE byType2, nuWORD nScaleIdx, nuWORD nPOICount)
{
	for( nuWORD i = 0; i < nPOICount; i++ )
	{
		if( byType1 == m_pPriorityInfo[nScaleIdx].pPoiPriority[i].byTypeID1 && 
			byType2 == m_pPriorityInfo[nScaleIdx].pPoiPriority[i].byTypeID2 )
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}

nuBOOL CPoiName::PoiIconIsFull(nuDWORD nIconID)
{
	nuINT nIconNum = 0;
	for( nuWORD i = 0; i < m_nPoiIndex; i++ )
	{
		if( m_pPoiList[i].nIcon == nIconID )
		{
			if( m_pPoiList[i].nPoiType == 0 )
			{
				nIconNum ++;
			}
			if( nIconNum == m_nMaxNumofSameIcon )
			{
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

//#include <stdio.h>
nuVOID CPoiName::ReadPTNode()
{
	//PPTNODE pPTAddr = (PPTNODE)g_pDMLibFS->FS_GetPtDataAddr(&m_nPoiTotalTypeCount);
	PPTNODE pPTAddr = NULL;//(PPTNODE)g_pDMLibFS->FS_GetPtDataAddr(&m_nPoiTotalTypeCount);
	for (int i = 0; i < m_nPoiTotalTypeCount; i++)
	{
		if (pPTAddr[i].nTypeID1 != pPTAddr[i+1].nTypeID1)
		{
			m_nPoiType1Count ++;
		}
	}
	if (NULL == m_pPoiType2CountList)
	{
		m_pPoiType2CountList = new nuWORD[m_nPoiType1Count];
		nuMemset(m_pPoiType2CountList, 0, sizeof(nuWORD)*m_nPoiType1Count);
		if (m_pPoiType2CountList)
		{
			int j = 0;
			for (int i = 0; i < m_nPoiTotalTypeCount; i++)
			{
				if (j == pPTAddr[i].nTypeID1)
				{
					m_pPoiType2CountList[j] ++;
				}
				else
				{
					j++;
					m_pPoiType2CountList[j] ++;
				}
			}
		}
	}
	
#if 0
	// print poi type2 counts
	char temp[256] = {0};
	for (int n = 0; n < m_nPoiType1Count; n++)
	{
		sprintf(temp, "\n m_pPoiType2CountList[%d] = %d", n, m_pPoiType2CountList[n]);
		OutputDebugStringA(temp);
	}
	int a;
	a = 1;
#endif
}
