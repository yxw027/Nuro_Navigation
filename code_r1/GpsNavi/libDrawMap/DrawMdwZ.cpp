// DrawMdwZ.cpp: implementation of the CDrawMdwZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawMdwZ.h"
#include "libDrawMap.h"
#include "MapDataConstructZ.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawMdwZ::CDrawMdwZ()
{
	m_pMdwData	= NULL;
}

CDrawMdwZ::~CDrawMdwZ()
{
	free();
}

nuBOOL CDrawMdwZ::initialize()
{
	if( !m_fileMdw.initialize() )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CDrawMdwZ::free()
{
	m_fileMdw.free();
	m_pMdwData = NULL;
}

nuBOOL CDrawMdwZ::LoadMdwData(nuroRECT& rtMapSize)
{
	m_pMdwData = (PMDWDATA)m_fileMdw.LoadMdwData(rtMapSize);
	if( m_pMdwData )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

nuVOID CDrawMdwZ::FreeMdwData()
{
	m_fileMdw.FreeMdwData();
	m_pMdwData = NULL;
}

nuBOOL CDrawMdwZ::DrawMdwZ()
{
	if( m_pMdwData == NULL || m_pMdwData->nBlkCount == 0 )
	{
		return nuFALSE;
	}
	//col mdw blocks
	PMDWBLKCOL pMdwBlkCol = new MDWBLKCOL[m_pMdwData->nBlkCount];
	if( pMdwBlkCol == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pMdwBlkCol, 0, sizeof(MDWBLKCOL)*m_pMdwData->nBlkCount);
	nuINT i;
	for(i = 0; i < m_pMdwData->nBlkCount; ++i)
	{
		nuPBYTE pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_pMdwData->pMdwNode[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		CMapDataConstructZ::ColMdwBlock(pData, &pMdwBlkCol[i]);
	}
	//draw mdw road
	nuroPOINT ptList[POINTLISTCOUNT]= {0};
	nuroRECT rtRoad= {0};
	nuINT nPtAdd = 0;
	nuLONG dx=0, dy=0;
//	for( i = m_pMdwData->nClassCnt - 1; i >= 0; ++i )
	{
		for(nuBYTE j = 0; j < m_pMdwData->nBlkCount; ++j)
		{
			if( pMdwBlkCol[j].pMdwHead == NULL )
			{
				continue;
			}
			for( nuDWORD k = 0; k < pMdwBlkCol[j].pMdwHead->nRoadCount; ++k)
			{
				nuWORD l;
				nuLONG nIdx = nuMAKELONG(pMdwBlkCol[j].pMdwRoad[k].nVertexAddLow, pMdwBlkCol[j].pMdwRoad[k].nVertexAddHigh);
				rtRoad.left = rtRoad.right = pMdwBlkCol[j].ptVertext[nIdx].x;
				rtRoad.top = rtRoad.bottom = pMdwBlkCol[j].ptVertext[nIdx].y;
				for( l = 1; l < pMdwBlkCol[j].pMdwRoad[k].nVertexCount; ++l)
				{
					if( pMdwBlkCol[j].ptVertext[nIdx + l].x < rtRoad.left )
					{
						rtRoad.left = pMdwBlkCol[j].ptVertext[nIdx + l].x;
					}
					else if( pMdwBlkCol[j].ptVertext[nIdx + l].x > rtRoad.right )
					{
						rtRoad.right = pMdwBlkCol[j].ptVertext[nIdx + l].x;
					}
					if ( pMdwBlkCol[j].ptVertext[nIdx + l].y < rtRoad.top )
					{
						rtRoad.top = pMdwBlkCol[j].ptVertext[nIdx + l].y;
					}
					else if( pMdwBlkCol[j].ptVertext[nIdx + l].y > rtRoad.bottom )
					{
						rtRoad.bottom = pMdwBlkCol[j].ptVertext[nIdx + l].y;
					}
				}
				if( !nuRectCoverRect(&rtRoad, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				for( l = 0; l < pMdwBlkCol[j].pMdwRoad[k].nVertexCount; ++l)
				{
					g_pDMLibMT->MT_MapToBmp( pMdwBlkCol[j].ptVertext[nIdx+l].x, 
						pMdwBlkCol[j].ptVertext[nIdx+l].y,
						&ptList[nPtAdd].x,
						&ptList[nPtAdd].y );
					if( l != 0 && l != pMdwBlkCol[j].pMdwRoad[k].nVertexCount - 1 )
					{
						dx = ptList[nPtAdd].x - ptList[nPtAdd-1].x;
						dy = ptList[nPtAdd].y - ptList[nPtAdd-1].y;
						if( NURO_ABS(dx) < 4 && NURO_ABS(dy) < 4)
						{
							continue;
						}
					}
					++nPtAdd;
					if( nPtAdd == POINTLISTCOUNT )
					{
						g_pDMLibMT->MT_DrawObject(DRAW_OBJ_2D_LINE, ptList, nPtAdd, 1);
						ptList[0] = ptList[nPtAdd - 1];
						nPtAdd = 1;
					}
				}
				if( nPtAdd > 1 )
				{
					g_pDMLibMT->MT_DrawObject(DRAW_OBJ_2D_LINE, ptList, nPtAdd, 1);
					//test
				}
				nPtAdd = 0;
			}
		}
	}
	delete []pMdwBlkCol;
	pMdwBlkCol=NULL;
	return nuTRUE;
}

nuBOOL CDrawMdwZ::ColMdwData()
{
	if( m_pMdwData == NULL || m_pMdwData->nBlkCount == 0)
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
	PMDWBLKCOL pMdwBlkCol = new MDWBLKCOL[m_pMdwData->nBlkCount];
	if( pMdwBlkCol == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pMdwBlkCol, 0, sizeof(MDWBLKCOL)*m_pMdwData->nBlkCount);
	nuINT i;
	for(i = 0; i < m_pMdwData->nBlkCount; ++i)
	{
		nuPBYTE pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_pMdwData->pMdwNode[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
//		nuBlockIDtoRectEx(m_pMdwData->pMdwNode[i].nBlkID, &pMdwBlkCol[i].rtBlock, );
		CMapDataConstructZ::ColMdwBlock(pData, &pMdwBlkCol[i]);
	}
	//
	nuroRECT rtRoad= {0};
	nuroPOINT ptLast = {NR_LONG_MAX, NR_LONG_MAX};
	nuWORD nNowVertex = 0;
	nuWORD nVertexIdx = pScObj->head.nTotalVertex;
	nuLONG dx=0, dy=0;
	for(i = 0; i < m_pMdwData->nClassCnt; ++i)
	{
		nuLONG nSkipLen = g_pDMMapCfg->scaleSetting.pRoadSetting[i].nCenterLineWidth *
				   g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;//SCALETOSCREEN_LEN;
		nNowVertex = 0;
		for(nuBYTE j = 0; j < m_pMdwData->nBlkCount; ++j)
		{
			for( nuDWORD k = pMdwBlkCol[j].nTmpIdx; k < pMdwBlkCol[j].pMdwHead->nRoadCount; ++k )
			{
				if( pMdwBlkCol[j].pMdwRoad[k].nClass > (nuDWORD)i )
				{
					pMdwBlkCol[j].nTmpIdx = k;
					break;
				}
				if( pMdwBlkCol[j].pMdwRoad[k].nClass == 5 )
				{
					continue;
				}
				if( !g_pDMMapCfg->scaleSetting.pRoadSetting[i].nShowCenterLine &&
					!g_pDMMapCfg->scaleSetting.pRoadSetting[i].nShowRimLine )//Not show
				{
					continue;
				}
				nuWORD l;
				nuLONG nIdx = nuMAKELONG(pMdwBlkCol[j].pMdwRoad[k].nVertexAddLow, pMdwBlkCol[j].pMdwRoad[k].nVertexAddHigh);
				rtRoad.left = rtRoad.right = pMdwBlkCol[j].ptVertext[nIdx].x;
				rtRoad.top = rtRoad.bottom = pMdwBlkCol[j].ptVertext[nIdx].y;
				for( l = 1; l < pMdwBlkCol[j].pMdwRoad[k].nVertexCount; ++l)
				{
					if( pMdwBlkCol[j].ptVertext[nIdx + l].x < rtRoad.left )
					{
						rtRoad.left = pMdwBlkCol[j].ptVertext[nIdx + l].x;
					}
					else if( pMdwBlkCol[j].ptVertext[nIdx + l].x > rtRoad.right )
					{
						rtRoad.right = pMdwBlkCol[j].ptVertext[nIdx + l].x;
					}
					if ( pMdwBlkCol[j].ptVertext[nIdx + l].y < rtRoad.top )
					{
						rtRoad.top = pMdwBlkCol[j].ptVertext[nIdx + l].y;
					}
					else if( pMdwBlkCol[j].ptVertext[nIdx + l].y > rtRoad.bottom )
					{
						rtRoad.bottom = pMdwBlkCol[j].ptVertext[nIdx + l].y;
					}
				}
				if( !nuRectCoverRect(&rtRoad, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				dx = pMdwBlkCol[j].ptVertext[nIdx].x - ptLast.x;
				dy = pMdwBlkCol[j].ptVertext[nIdx].y - ptLast.y;
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
						pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_ROAD;
						pScObj->pShape[pScObj->head.nShapCount].nType = i;
						pScObj->head.nShapCount += 1;
						pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount += 1;
						pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex += nNowVertex;
						if( pScObj->head.nShapCount == MEM_SIZE_MAP_SHPNUM )
						{
							delete []pMdwBlkCol;
							pMdwBlkCol=NULL;
							return nuTRUE;
						}
					}
					else
					{
						nVertexIdx -= nNowVertex;
					}
					nNowVertex = 0;
				}
				for(; l < pMdwBlkCol[j].pMdwRoad[k].nVertexCount; ++l)
				{
					if( nNowVertex != 0 && l != pMdwBlkCol[j].pMdwRoad[k].nVertexCount - 1 )
					{
						dx = pMdwBlkCol[j].ptVertext[nIdx+l].x - ptLast.x;
						dy = pMdwBlkCol[j].ptVertext[nIdx+l].y - ptLast.y;
						dx = NURO_ABS(dx);
						dy = NURO_ABS(dy);
						if( dx <= nSkipLen && dy <= nSkipLen )
						{
							continue;
						}
					}
					g_pDMLibMT->MT_MapToBmp( pMdwBlkCol[j].ptVertext[nIdx + l].x,
						pMdwBlkCol[j].ptVertext[nIdx + l].y,
						&pScObj->ptVertex[nVertexIdx].x, 
						&pScObj->ptVertex[nVertexIdx].y);
					++nVertexIdx;
					++nNowVertex;
					if( nVertexIdx == MEM_SIZE_MAP_POINTS )
					{
						if( nNowVertex >= 2 )
						{
							pScObj->head.nTotalVertex = nVertexIdx;
							pScObj->pShape[pScObj->head.nShapCount].nPointCount = nNowVertex;
							pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_ROAD;
							pScObj->pShape[pScObj->head.nShapCount].nType = i;
							pScObj->head.nShapCount += 1;
							pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount += 1;
						pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex += nNowVertex;
						}
						delete []pMdwBlkCol;
						pMdwBlkCol=NULL;
						return nuTRUE;
					}
					ptLast.x = pMdwBlkCol[j].ptVertext[nIdx + l].x;
					ptLast.y = pMdwBlkCol[j].ptVertext[nIdx + l].y;
				}
			}
		}
		if( nNowVertex >= 2 )
		{
			pScObj->head.nTotalVertex = nVertexIdx;
			pScObj->pShape[pScObj->head.nShapCount].nPointCount = nNowVertex;
			pScObj->pShape[pScObj->head.nShapCount].objType	= SHPOBJ_TYPE_ROAD;
			pScObj->pShape[pScObj->head.nShapCount].nType = i;
			pScObj->head.nShapCount += 1;
			pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount += 1;
			pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex += nNowVertex;
			if( pScObj->head.nShapCount == MEM_SIZE_MAP_SHPNUM )
			{
				break;
			}
		}
		else
		{
			nVertexIdx -= nNowVertex;
		}
	}
	//
	delete []pMdwBlkCol;
	pMdwBlkCol=NULL;
	return nuTRUE;
}

