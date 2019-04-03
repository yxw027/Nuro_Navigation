// DrawSeaZ.cpp: implementation of the CDrawSeaZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawSeaZ.h"
#include "libDrawMap.h"
#include "MapDataConstructZ.h"
#include "NuroClasses.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawSeaZ::CDrawSeaZ()
{
	m_pSeaData = NULL;
}

CDrawSeaZ::~CDrawSeaZ()
{
	Free();
}

nuBOOL CDrawSeaZ::Initialize()
{
	if( !m_fileSea.Initialize() )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CDrawSeaZ::Free()
{
	FreeData();
	m_fileSea.Free();
}

nuBOOL CDrawSeaZ::LoadData(const nuroRECT& rtMap, nuLONG nScale)
{
	m_pSeaData = (PSEADATA)m_fileSea.LoadSeaData(rtMap, nScale);
	if( m_pSeaData == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CDrawSeaZ::FreeData()
{
	m_fileSea.FreeSeaData();
	m_pSeaData = NULL;
}

nuBOOL CDrawSeaZ::DrawSeaZ()
{
	if( m_pSeaData == NULL || m_pSeaData->nBlkCount == 0 )
	{
		return nuFALSE;
	}
	nuINT nObjType = DRAW_OBJ_SKIP;
	nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_BGAREA, 
		g_pDMGdata->uiSetData.b3DMode,
		&g_pDMMapCfg->scaleSetting.pBgAreaSetting[SEA_AREA_TYPE],
		NULL);
	if( nObjType == DRAW_OBJ_SKIP )
	{
		return nuFALSE;
	}
	SEABLKCOL seaBlkCol={0};
	nuPBYTE pData={0};
	nuroRECT rtBoundary = {0};
	nuroRECT rtBlock ={0};
	nuroPOINT ptList[POINTLISTCOUNT] = {0};
	nuINT nPtAdd = 0;
//	nuLONG dx, dy;
	for( nuBYTE i = 0; i < m_pSeaData->nBlkCount; ++i )
	{
		pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_pSeaData->pSeaList[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		CMapDataConstructZ::ColSeaBlock(pData, &seaBlkCol);
		nuBlockIDtoRectEx(m_pSeaData->pSeaList[i].nBlkID, rtBlock, m_pSeaData->seaScaleData.nBlockSize);
		for ( nuDWORD j = 0; j < seaBlkCol.pSeaHead->nSeaAreaCount; ++j )
		{
			if( seaBlkCol.pSeaData[j].nAreaType == ISLAND_AREA_TYPE )
			{
				continue;
			}
			if( seaBlkCol.pSeaData[j].nAreaType == RIVER_AREA_TYPE &&
				g_pDMGdata->uiSetData.nBsdDrawMap == BSD_MODE_COMMON )
			{
				continue;
			}
			nuLONG nIdx = seaBlkCol.pSeaData[j].nVertexIdex;
			rtBoundary.left = rtBoundary.right = seaBlkCol.ptVertex[nIdx].x;
			rtBoundary.top = rtBoundary.bottom = seaBlkCol.ptVertex[nIdx].y;
			nuDWORD k;
			for( k = 1; k < seaBlkCol.pSeaData[j].nVertexCount; ++k )
			{
				if( seaBlkCol.ptVertex[nIdx+k].x < rtBoundary.left )
				{
					rtBoundary.left = seaBlkCol.ptVertex[nIdx+k].x;
				}
				else if( seaBlkCol.ptVertex[nIdx+k].x > rtBoundary.right )
				{
					rtBoundary.right = seaBlkCol.ptVertex[nIdx+k].x;
				}
				if( seaBlkCol.ptVertex[nIdx+k].y < rtBoundary.top )
				{
					rtBoundary.top = seaBlkCol.ptVertex[nIdx+k].y;
				}
				else if( seaBlkCol.ptVertex[nIdx+k].y > rtBoundary.bottom )
				{
					rtBoundary.bottom = seaBlkCol.ptVertex[nIdx+k].y;
				}
			}
			rtBoundary.left		= rtBlock.left + rtBoundary.left * m_pSeaData->seaScaleData.nDivisor;
			rtBoundary.right	= rtBlock.left + rtBoundary.right * m_pSeaData->seaScaleData.nDivisor;
			rtBoundary.top		= rtBlock.top + rtBoundary.top * m_pSeaData->seaScaleData.nDivisor;
			rtBoundary.bottom	= rtBlock.top + rtBoundary.bottom * m_pSeaData->seaScaleData.nDivisor;
			if( !nuRectCoverRect(&rtBoundary, &g_pDMGdata->transfData.nuShowMapSize) )
			{
				continue;
			}
			nPtAdd = 0;
			for( k = 0; k < seaBlkCol.pSeaData[j].nVertexCount; ++k )
			{
				g_pDMLibMT->MT_MapToBmp( rtBlock.left + seaBlkCol.ptVertex[nIdx+k].x * m_pSeaData->seaScaleData.nDivisor, 
					rtBlock.top + seaBlkCol.ptVertex[nIdx+k].y * m_pSeaData->seaScaleData.nDivisor,
					&ptList[nPtAdd].x,
					&ptList[nPtAdd].y );
				/**/
				if( k != 0 && k != seaBlkCol.pSeaData[j].nVertexCount - 1 )
				{
					if(ptList[nPtAdd].x == ptList[nPtAdd-1].x && ptList[nPtAdd].y == ptList[nPtAdd-1].y)
					{
						continue;
					}
				}
				
				++nPtAdd;
				if( nPtAdd == POINTLISTCOUNT )
				{
					break;
				}
			}
			if( nPtAdd > 2 )
			{
				g_pDMLibMT->MT_DrawObject( nObjType, ptList, nPtAdd, 0);
			}
		}
	}
	return nuTRUE;
}

nuBOOL CDrawSeaZ::ColSeaData()
{
	if( m_pSeaData == NULL || m_pSeaData->nBlkCount == 0)
	{
		return nuFALSE;
	}
	/*
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
	*/
	PSEABLKCOL pSeaBlkCol = new SEABLKCOL[m_pSeaData->nBlkCount];
	if( pSeaBlkCol == NULL )
	{
		return nuFALSE;
	}
	nuMemset(pSeaBlkCol, 0, sizeof(SEABLKCOL)*m_pSeaData->nBlkCount);
	//Sea col
	nuBYTE i =0;
	nuBYTE* pData =NULL;
	nuroPOINT ptList[POINTLISTCOUNT] = {0};
	nuINT nPtAdd = 0;
	for( i = 0; i < m_pSeaData->nBlkCount; ++i )
	{
		pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_pSeaData->pSeaList[i].nMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		CMapDataConstructZ::ColSeaBlock(pData, &pSeaBlkCol[i]);
		nuBlockIDtoRectEx( m_pSeaData->pSeaList[i].nBlkID,
			pSeaBlkCol[i].rtBlock,
			m_pSeaData->seaScaleData.nBlockSize );
	}
	nuINT nObjType = DRAW_OBJ_SKIP;

	nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_BGAREA, 
		g_pDMGdata->uiSetData.b3DMode,
		&g_pDMMapCfg->scaleSetting.pBgAreaSetting[ISLAND_AREA_TYPE],
		NULL);
	if( nObjType == DRAW_OBJ_SKIP )
	{
		delete []pSeaBlkCol;
		pSeaBlkCol=NULL;
		return nuFALSE;
	}

	NURORECT rtBoundary= {0};
	for( i = 0; i < 1; ++i )
	{
		for(nuBYTE j = 0; j < m_pSeaData->nBlkCount; ++j)
		{
			if( pSeaBlkCol[j].pSeaHead == NULL || pSeaBlkCol[j].pSeaHead->nSeaAreaCount == 0 )
			{
				continue;
			}
			for( nuDWORD k = 0; k < pSeaBlkCol[j].pSeaHead->nSeaAreaCount; ++k )
			{
				if( pSeaBlkCol[j].pSeaData[k].nAreaType != ISLAND_AREA_TYPE &&
					pSeaBlkCol[j].pSeaData[k].nAreaType != SPECIAL_AREA_TYPE )
				{
					continue;
				}
				nuLONG nIdx = pSeaBlkCol[j].pSeaData[k].nVertexIdex;
				rtBoundary.left = rtBoundary.right = pSeaBlkCol[j].ptVertex[nIdx].x;
				rtBoundary.top = rtBoundary.bottom = pSeaBlkCol[j].ptVertex[nIdx].y;
				nuDWORD l;
				for( l = 1; l < pSeaBlkCol[j].pSeaData[k].nVertexCount; ++l )
				{
					if( pSeaBlkCol[j].ptVertex[nIdx+l].x < rtBoundary.left )
					{
						rtBoundary.left = pSeaBlkCol[j].ptVertex[nIdx+l].x;
					}
					else if( pSeaBlkCol[j].ptVertex[nIdx+l].x > rtBoundary.right )
					{
						rtBoundary.right = pSeaBlkCol[j].ptVertex[nIdx+l].x;
					}
					if( pSeaBlkCol[j].ptVertex[nIdx+l].y < rtBoundary.top )
					{
						rtBoundary.top = pSeaBlkCol[j].ptVertex[nIdx+l].y;
					}
					else if( pSeaBlkCol[j].ptVertex[nIdx+l].y > rtBoundary.bottom )
					{
						rtBoundary.bottom = pSeaBlkCol[j].ptVertex[nIdx+l].y;
					}
				}
				rtBoundary.left		= pSeaBlkCol[j].rtBlock.left + rtBoundary.left * m_pSeaData->seaScaleData.nDivisor;
				rtBoundary.right	= pSeaBlkCol[j].rtBlock.left + rtBoundary.right * m_pSeaData->seaScaleData.nDivisor;
				rtBoundary.top		= pSeaBlkCol[j].rtBlock.top + rtBoundary.top * m_pSeaData->seaScaleData.nDivisor;
				rtBoundary.bottom	= pSeaBlkCol[j].rtBlock.top + rtBoundary.bottom * m_pSeaData->seaScaleData.nDivisor;
				if( !nuRectCoverRect(&rtBoundary, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				//
				nPtAdd = 0;
				for( l = 0; l < pSeaBlkCol[j].pSeaData[k].nVertexCount; ++l )
				{
					g_pDMLibMT->MT_MapToBmp( pSeaBlkCol[j].rtBlock.left + pSeaBlkCol[j].ptVertex[nIdx+l].x * m_pSeaData->seaScaleData.nDivisor, 
						pSeaBlkCol[j].rtBlock.top + pSeaBlkCol[j].ptVertex[nIdx+l].y * m_pSeaData->seaScaleData.nDivisor,
						&ptList[nPtAdd].x,
						&ptList[nPtAdd].y );
					/**/
					if( k != 0 && k != pSeaBlkCol[j].pSeaData[k].nVertexCount - 1 && nPtAdd > 0)
					{
						if(ptList[nPtAdd].x == ptList[nPtAdd-1].x && ptList[nPtAdd].y == ptList[nPtAdd-1].y)
						{
							continue;
						}
					}
					
					++nPtAdd;
					if( nPtAdd == POINTLISTCOUNT )
					{
						break;
					}
				}
				if( nPtAdd > 2 )
				{
					g_pDMLibMT->MT_DrawObject(nObjType, ptList, nPtAdd, 0);
				}
			}
		}
	}
//	DEBUGSTRING(nuTEXT("d3"));
	delete []pSeaBlkCol;
	pSeaBlkCol=NULL;
//	DEBUGSTRING(nuTEXT("d2"));
	return nuTRUE;
}
