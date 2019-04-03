// DrawBglZ.cpp: implementation of the CDrawBglZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawBglZ.h"
#include "MapDataConstructZ.h"
#include "libDrawMap.h"
#include "NuroConstDefined.h"

#define  MAX_ROAD_ICON_NUM      20
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawBglZ::CDrawBglZ()
{
	m_pBglData	= NULL;
	m_nBglRdIconIdx = 0;
}

CDrawBglZ::~CDrawBglZ()
{

}

nuBOOL CDrawBglZ::Initialize()
{
	if( !m_fileBgl.Initialize() )
	{
		return nuFALSE;
	}
	m_pBglData = NULL;
	//
	m_pRdIconList = new RDICONNODE[MAX_ROAD_ICON_NUM];
	if( NULL == m_pRdIconList )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CDrawBglZ::Free()
{
	m_pBglData = NULL;
	m_fileBgl.Free();
	//
	if( NULL != m_pRdIconList )
	{
		delete[] m_pRdIconList;
		m_pRdIconList = NULL;
	}
}

nuBOOL CDrawBglZ::LoadData(const nuroRECT& rtMap, nuLONG nScale)
{
	m_pBglData = (PBGLDATA)m_fileBgl.LoadBglData(rtMap, nScale, g_pDMGdata->uiSetData.nBglStartScaleValue);
	if( m_pBglData )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID CDrawBglZ::FreeData()
{
	m_fileBgl.FreeBglData();
	m_pBglData = NULL;
}

nuBOOL CDrawBglZ::DrawBgl()
{
	return nuTRUE;
}

nuBOOL CDrawBglZ::ColBglData()
{
	ResetRdIconList();
	//
	if( m_pBglData == NULL || m_pBglData->nBlkCount == 0 )
	{
		return nuFALSE;
	}
	PDRAWMEMORY pMemData = (PDRAWMEMORY)g_pDMLibFS->FS_GetData(DATA_DRAWMEM);
	if( pMemData == NULL )
	{
		return nuFALSE;
	}
	PSCREENOBJ pScObj = (PSCREENOBJ)g_pDMLibMM->MM_GetDataMassAddr(pMemData->nMapSHPMemIdx);
	if( pScObj == NULL )
	{
		return nuFALSE;
	}
	//
	PBGLBLKCOL pBglBlkCol = new BGLBLKCOL[m_pBglData->nBlkCount];
	if( pBglBlkCol == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pBglBlkCol, 0, sizeof(BGLBLKCOL) * m_pBglData->nBlkCount);
	nuBYTE i;
	for( i = 0; i < m_pBglData->nBlkCount; ++i )
	{
		nuPBYTE pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_pBglData->pBglNode[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		nuBlockIDtoRectEx(m_pBglData->pBglNode[i].nBlkID, pBglBlkCol[i].rtBlock, m_pBglData->nBlkSize);
		CMapDataConstructZ::ColBglBlock(pData, &pBglBlkCol[i]);
		if( pBglBlkCol[i].pHead->nBglCount > 10000 )
		{
			nuINT iCountk = 0;
		}
	}
	nuroRECT rtRoad = {0};
	nuroPOINT ptLast = {NR_LONG_MAX, NR_LONG_MAX};
	nuWORD nNowVertex = 0;
	nuWORD nVertexIdx = pScObj->head.nTotalVertex;
	nuLONG dx, dy;
	dx = dy = 0;
//	nuBYTE nNowClass;
//	nuBOOL bNewClass;
	nuLONG nSkipLen = 0;
	for( i = 0; i < _MAX_BGL_CLASS_COUNT; ++i )
	{
//		bNewClass = nuTRUE;
		for( nuBYTE j = 0; j < m_pBglData->nBlkCount; ++j )
		{
			if( pBglBlkCol[j].pHead == NULL || pBglBlkCol[j].pHead->nBglCount == 0 )
			{
				continue;
			}//Skip the block without data
			for( nuDWORD k = pBglBlkCol[j].nTmpClsIdx; k < pBglBlkCol[j].pHead->nBglCount; ++k )
			{
				/*
				if( bNewClass )
				{
					bNewClass = nuFALSE;
					nNowClass = pBglBlkCol[j].pData[k].nClass;
					nSkipLen = g_pDMMapCfg->scaleSetting.pRoadSetting[nNowClass].nCenterLineWidth *
						g_pDMMapCfg->scaleSetting.scaleTitle.nScale / SCALETOSCREEN_LEN;
				}
				else if( nNowClass != pBglBlkCol[j].pData[k].nClass )
				{
					pBglBlkCol[j].nTmpClsIdx = k;
					break;
				}
				*/
				if( i != pBglBlkCol[j].pData[k].nClass )
				{
					pBglBlkCol[j].nTmpClsIdx = k;
					break;
				}
				/*test 
				if( pBglBlkCol[j].pData[k].nClass > 1 || j != 5 || ( k != 8 ))
				{
					continue;
				}
				*/
				if( i >= g_pDMMapCfg->commonSetting.nRoadTypeCount )
				{
					continue;
				}
				if( pBglBlkCol[j].pData[k].nVertextCount < 2 )
				{
					continue;
				}
				if( !g_pDMMapCfg->scaleSetting.pRoadSetting[i].nShowCenterLine &&
					!g_pDMMapCfg->scaleSetting.pRoadSetting[i].nShowRimLine )
				{
					continue;
				}
			
				nuWORD l;
				nuLONG idx = nuMAKELONG(pBglBlkCol[j].pData[k].nVertexAddressLo, pBglBlkCol[j].pData[k].nVertexAddRessHi);
				rtRoad.left = rtRoad.right = pBglBlkCol[j].ptVertext[idx].x;
				rtRoad.top  = rtRoad.bottom = pBglBlkCol[j].ptVertext[idx].y;
				for( l = 1; l < pBglBlkCol[j].pData[k].nVertextCount; ++l )
				{
					if( pBglBlkCol[j].ptVertext[idx+l].x < rtRoad.left )
					{
						rtRoad.left = pBglBlkCol[j].ptVertext[idx+l].x;
					}
					else if( pBglBlkCol[j].ptVertext[idx+l].x > rtRoad.right )
					{
						rtRoad.right = pBglBlkCol[j].ptVertext[idx+l].x;
					}
					if( pBglBlkCol[j].ptVertext[idx+l].y < rtRoad.top )
					{
						rtRoad.top = pBglBlkCol[j].ptVertext[idx+l].y;
					}
					else if( pBglBlkCol[j].ptVertext[idx+l].y > rtRoad.bottom )
					{
						rtRoad.bottom = pBglBlkCol[j].ptVertext[idx+l].y;
					}
				}
				rtRoad.left		= rtRoad.left * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.left;
				rtRoad.right	= rtRoad.right * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.left;
				rtRoad.top		= rtRoad.top * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.top;
				rtRoad.bottom	= rtRoad.bottom * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.top;//
				if( !nuRectCoverRect(&rtRoad, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				dx = pBglBlkCol[j].ptVertext[idx].x * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.left - ptLast.x;
				dy = pBglBlkCol[j].ptVertext[idx].y * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.top - ptLast.y;
				dx = NURO_ABS(dx);
				dy = NURO_ABS(dy);
				
				if( dx <= nSkipLen && dy <= nSkipLen )
				{
					l = 1;
				}
				else
				{
					l = 0;
					if( nNowVertex >= 2 )
					{
						pScObj->head.nTotalVertex = nVertexIdx;
						pScObj->pShape[pScObj->head.nShapCount].nPointCount = nNowVertex;
						pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_BGLINE;
						pScObj->pShape[pScObj->head.nShapCount].nType = i;//nNowClass;
						pScObj->head.nShapCount += 1;
						pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount += 1;
						pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nTotalVertex += nNowVertex;
						if( pScObj->head.nShapCount == MEM_SIZE_MAP_SHPNUM )
						{
							delete []pBglBlkCol;
							pBglBlkCol=NULL;
							return nuTRUE;
						}
					}
					else
					{
						nVertexIdx -= nNowVertex;
					}
					nNowVertex = 0;
				}
				for( ; l < pBglBlkCol[j].pData[k].nVertextCount; ++l )
				{
					g_pDMLibMT->MT_MapToBmp( pBglBlkCol[j].ptVertext[idx + l].x * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.left,
						pBglBlkCol[j].ptVertext[idx + l].y * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.top,
						&pScObj->ptVertex[nVertexIdx].x, 
						&pScObj->ptVertex[nVertexIdx].y);
						/*
					*/
					if( nNowVertex > 1 && l != pBglBlkCol[j].pData[k].nVertextCount - 1 )
					{
						if( pScObj->ptVertex[nVertexIdx].x == pScObj->ptVertex[nVertexIdx - 1].x &&
							pScObj->ptVertex[nVertexIdx].y == pScObj->ptVertex[nVertexIdx - 1].y )
						{
							continue;
						}
						
						dx = pScObj->ptVertex[nVertexIdx].x - pScObj->ptVertex[nVertexIdx - 1].x;
						dy = pScObj->ptVertex[nVertexIdx].y - pScObj->ptVertex[nVertexIdx - 1].y;
						if( NURO_ABS(dx) < 3 && NURO_ABS(dy) < 3 )
						{
							continue;
						}
					}
					//---------------- Collect Road Icon Information ----------------//
					nuWCHAR IconName[100] = {0};
					RNEXTEND rnExtend = {0};
					rnExtend.nRNLen = 12;
					rnExtend.nIconType = pBglBlkCol[j].pData[k].nRouteType;
					rnExtend.nIconNum = pBglBlkCol[j].pData[k].nRouteIndex - 1;
					if( rnExtend.nIconType != 0 && rnExtend.nIconNum >= 0 )
					{
						nuLONG sx = pScObj->ptVertex[nVertexIdx].x;
						nuLONG sy = pScObj->ptVertex[nVertexIdx].y;		
						AddBglRdIconInfo(sx - 12, sy - 10, rnExtend);
					}
					//--------------------------------------------------------------//
					++nVertexIdx;
					++nNowVertex;
					if( nVertexIdx == MEM_SIZE_MAP_POINTS )
					{
						if( nNowVertex >= 2 )
						{
							pScObj->head.nTotalVertex = nVertexIdx;
							pScObj->pShape[pScObj->head.nShapCount].nPointCount = nNowVertex;
							pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_BGLINE;
							pScObj->pShape[pScObj->head.nShapCount].nType = i;//nNowClass;
							pScObj->head.nShapCount += 1;
							pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount += 1;
							pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nTotalVertex += nNowVertex;
						}
						delete []pBglBlkCol;
						pBglBlkCol=NULL;
						return nuTRUE;
					}
				}
				ptLast.x = pBglBlkCol[j].ptVertext[idx + pBglBlkCol[j].pData[k].nVertextCount - 1].x * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.left;
				ptLast.y = pBglBlkCol[j].ptVertext[idx + pBglBlkCol[j].pData[k].nVertextCount - 1].y * m_pBglData->nDivisor + pBglBlkCol[j].rtBlock.top;
				//
			}
		}
		if( nNowVertex >= 2 )
		{
			pScObj->head.nTotalVertex = nVertexIdx;
			pScObj->pShape[pScObj->head.nShapCount].nPointCount = nNowVertex;
			pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_BGLINE;
			pScObj->pShape[pScObj->head.nShapCount].nType = i;//nNowClass;
			pScObj->head.nShapCount += 1;
			pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount += 1;
			pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nTotalVertex += nNowVertex;
			if( pScObj->head.nShapCount == MEM_SIZE_MAP_SHPNUM )
			{
				break;
			}
		}
		else
		{
			nVertexIdx -= nNowVertex;
		}
		nNowVertex = 0;
	}
	delete []pBglBlkCol;
	return nuTRUE;
}

nuBOOL CDrawBglZ::AddBglRdIconInfo(nuLONG sx, nuLONG sy, RNEXTEND rnExtend)
{
	if( m_nBglRdIconIdx >= MAX_ROAD_ICON_NUM || NULL == m_pRdIconList )
	{
		return nuFALSE;
	}
	for( nuWORD i = 0; i < m_nBglRdIconIdx; i++ )
	{
		if( m_pRdIconList[i].nIconIndex == rnExtend.nIconNum  &&
			m_pRdIconList[i].byIconType == rnExtend.nIconType )
		{
			return nuFALSE;
		}
	}
	//
	NURORECT rtScreen, tmpRect = {0};
	rtScreen.left	= g_pDMGdata->transfData.nBmpLTx;
	rtScreen.top	= g_pDMGdata->transfData.nBmpLTy;
	rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth;
	rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight;
	if( g_pDMGdata->uiSetData.b3DMode )
	{
		rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
	}
	nuroPOINT pt = {0};
	pt.x = sx;
	pt.y = sy;
	if( g_pDMGdata->uiSetData.b3DMode )
	{
		g_pDMLibMT->MT_Bmp2Dto3D( pt.x, pt.y, MAP_DEFAULT);
	}
	if( !nuPointInRect(&pt, &rtScreen) )
	{
		return nuFALSE;
	}
	//
	tmpRect.top		= pt.y;
	tmpRect.left	= pt.x;
#ifdef VALUE_EMGRT
	if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
		g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
	{
		tmpRect.bottom	= tmpRect.top + 35;
		tmpRect.right	= tmpRect.left + 61;
	}
	else
	{
		tmpRect.bottom	= tmpRect.top + 20;
		tmpRect.right	= tmpRect.left + 36;
	}
#else
	tmpRect.bottom	= tmpRect.top + 48;
	tmpRect.right	= tmpRect.left + 48;
#endif
	nuWORD i;
	for( i = 0; i < m_nBglRdIconIdx; i++ )
	{
		if( nuRectCoverRect(&tmpRect, &m_pRdIconList[i].rect) )
		{
			return nuFALSE;
		}
		if( rnExtend.nIconType == m_pRdIconList[i].byIconType && 
			rnExtend.nIconNum == m_pRdIconList[i].nIconIndex )
		{
			if( NURO_ABS(sx - m_pRdIconList[i].rect.left) < 400 && 
				NURO_ABS(sy - m_pRdIconList[i].rect.top) < 200 )
			{
				return nuFALSE;
			}
		}
	}
	//
	m_pRdIconList[m_nBglRdIconIdx].rect = tmpRect;
	m_pRdIconList[m_nBglRdIconIdx].byIconType = rnExtend.nIconType;	
	m_pRdIconList[m_nBglRdIconIdx].nIconIndex = rnExtend.nIconNum;

	m_nBglRdIconIdx ++;

	return nuTRUE;
}

nuBOOL CDrawBglZ::DrawBglRdIcon()
{ 
#ifndef VALUE_EMGRT
	return DrawBglRdIconTW();
#endif
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);

	if( m_nBglRdIconIdx <= 0 || NULL == m_pRdIconList )
	{
		return nuFALSE;
	}
	nuWORD i;
	for( i = 0; i < m_nBglRdIconIdx; i++ )
	{
		RNEXTEND rnExtend = {0};
		nuWCHAR wsRdNum[256] = {0};
		rnExtend.nRNLen = 12;
		rnExtend.nIconType = m_pRdIconList[i].byIconType;
		rnExtend.nIconNum = m_pRdIconList[i].nIconIndex;
		if( !g_pDMLibFS->FS_GetRoadIconInfo(0, g_pDMLibFS->pGdata->uiSetData.nScaleValue,
			g_pDMLibFS->pGdata->uiSetData.nBglStartScaleValue, &rnExtend, wsRdNum) )
		{
			continue;
		}
		if( nuWcslen(wsRdNum) != 4 )
		{
			//continue;
		}
		g_pDMLibGDI->GdiDrawIcon(m_pRdIconList[i].rect.left, m_pRdIconList[i].rect.top, m_pRdIconList[i].byIconType);
		//Set Font Color
		if( 15 == m_pRdIconList[i].byIconType || 14 == m_pRdIconList[i].byIconType )
		{
			g_pDMLibGDI->GdiSetTextColor(nuRGB(0, 0, 0)); //Black
		}
		else
		{
			g_pDMLibGDI->GdiSetTextColor(nuRGB(255, 255, 255)); //White
		}
		//
		nuroRECT rect_Tmp = {0};
		rect_Tmp.left =	m_pRdIconList[i].rect.left;
		rect_Tmp.top  = m_pRdIconList[i].rect.top;
		if( 11 == m_pRdIconList[i].byIconType || 13 == m_pRdIconList[i].byIconType )
		{
			if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
				g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
			{
				rect_Tmp.right	=	rect_Tmp.left + 61;
				rect_Tmp.bottom	=	rect_Tmp.top + 40;
			}
			else
			{
				rect_Tmp.right	=	rect_Tmp.left + 36;
				rect_Tmp.bottom	=	rect_Tmp.top + 22;
			}
		}
		else
		{
			if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
				g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
			{
				rect_Tmp.right	=	rect_Tmp.left + 61;
				rect_Tmp.bottom	=	rect_Tmp.top + 35;
			}
			else
			{
				rect_Tmp.right	=	rect_Tmp.left + 36;
				rect_Tmp.bottom	=	rect_Tmp.top + 18;
			}		
		}	
		//Set Font
		NUROFONT nuLogFont = {0};
		if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
			g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
		{
			nuLogFont.lfHeight = 24;
		}
		else
		{
			nuLogFont.lfHeight = 14;
		}		
		g_pDMLibGDI->GdiSetFont(&nuLogFont);			
		g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, 
			NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER );
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}

nuBOOL CDrawBglZ::DrawBglRdIconTW()
{
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);

	if( m_nBglRdIconIdx <= 0 || NULL == m_pRdIconList )
	{
		return nuFALSE;
	}
	nuWORD i;
	for( i = 0; i < m_nBglRdIconIdx; i++ )
	{
		RNEXTEND rnExtend = {0};
		nuWCHAR wsRdNum[256] = {0};
		rnExtend.nRNLen = 12;
		rnExtend.nIconType = m_pRdIconList[i].byIconType;
		rnExtend.nIconNum = m_pRdIconList[i].nIconIndex;
		if( !g_pDMLibFS->FS_GetRoadIconInfo(0, g_pDMLibFS->pGdata->uiSetData.nScaleValue,
			g_pDMLibFS->pGdata->uiSetData.nBglStartScaleValue, &rnExtend, wsRdNum) )
		{
			continue;
		}
		if( g_pDMLibGDI->GdiDrawIcon(m_pRdIconList[i].rect.left, m_pRdIconList[i].rect.top, m_pRdIconList[i].byIconType) )
		{
			g_pDMLibGDI->GdiSetTextColor(nuRGB(255, 255, 255)*(m_pRdIconList[i].byIconType - 1));
			NUROFONT nuFont = {0};
			nuroRECT rect_Tmp = {0};
			if( nuWcslen(wsRdNum) == 1 )
			{
				nuFont.lfHeight	= 36;
				if( rnExtend.nIconType > 0 )
				{
					nuFont.lfHeight	-= 8;
				}
				g_pDMLibGDI->GdiSetFont(&nuFont);							
				g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &m_pRdIconList[i].rect, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
			}
			else if( nuWcslen(wsRdNum) == 2 )
			{
				if( wsRdNum[1] < 58 ) //57对应9
				{
					nuFont.lfHeight	= 26;
					if( rnExtend.nIconType > 0 )
					{
						m_pRdIconList[i].rect.top -= 3;
					}
					g_pDMLibGDI->GdiSetFont(&nuFont);							
					g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &m_pRdIconList[i].rect, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
				}
				else
				{
					nuWCHAR wsRdNum2[10] = {0};
					wsRdNum2[0] = wsRdNum[1];
					wsRdNum[1] = 0;
					NUROFONT nuFont2	= {0};			
					nuFont2.lfHeight	= 15;
					g_pDMLibGDI->GdiSetFont(&nuFont2);
					g_pDMLibGDI->GdiExtTextOutWEx( m_pRdIconList[i].rect.left + 18, 
						m_pRdIconList[i].rect.top + 25, 
						wsRdNum2, nuWcslen(wsRdNum2), false, 0 );
					nuFont.lfHeight	= 25;
					g_pDMLibGDI->GdiSetFont(&nuFont);
					rect_Tmp.left   = m_pRdIconList[i].rect.left;
					rect_Tmp.right  = m_pRdIconList[i].rect.right;
					rect_Tmp.top    = m_pRdIconList[i].rect.top - 9;
					rect_Tmp.bottom = m_pRdIconList[i].rect.bottom - 7;
					g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
				}	
			}
			else if( nuWcslen(wsRdNum) == 3 )
			{
				if( wsRdNum[2] < 58 ) //57对应9
				{
					nuFont.lfHeight	= 20;
					g_pDMLibGDI->GdiSetFont(&nuFont);							
					g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &m_pRdIconList[i].rect, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
				}
				else
				{
					nuWCHAR wsRdNum2[10] = {0};
					wsRdNum2[0] = wsRdNum[2];
					wsRdNum[2] = 0;
					NUROFONT nuFont2    = {0};
					nuFont2.lfHeight	= 15;
					g_pDMLibGDI->GdiSetFont(&nuFont2);
					g_pDMLibGDI->GdiExtTextOutWEx( m_pRdIconList[i].rect.left + 18, 
						m_pRdIconList[i].rect.top + 25, 
						wsRdNum2, nuWcslen(wsRdNum2), false, 0 );
					nuFont.lfHeight	= 22;
					g_pDMLibGDI->GdiSetFont(&nuFont);
					rect_Tmp.left   = m_pRdIconList[i].rect.left;
					rect_Tmp.right  = m_pRdIconList[i].rect.right;
					rect_Tmp.top    = m_pRdIconList[i].rect.top - 10;
					rect_Tmp.bottom = m_pRdIconList[i].rect.bottom - 10;
					g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
				}
			}
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}

nuBOOL CDrawBglZ::ResetRdIconList()
{
	m_nBglRdIconIdx = 0;
	return nuTRUE;
}
