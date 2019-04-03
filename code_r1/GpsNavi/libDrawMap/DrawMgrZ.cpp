// DrawMgrZ.cpp: implementation of the CDrawMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawMgrZ.h"
#include "NuroClasses.h"
#include "NuroEngineStructs.h"
#include "NuroConstDefined.h"
#include "NuroModuleApiStruct.h"
#include "libDrawMap.h"
#include "RoadName.h"
#include "MapDataConstructZ.h"
#include "MemoMgrZ.h"

extern class CRoadName*			g_pRoadName;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern PGDIAPI         g_pDMLibGDI;
extern PMEMMGRAPI      g_pDMLibMM;
extern PFILESYSAPI     g_pDMLibFS;
extern PMATHTOOLAPI    g_pDMLibMT;

#define _EEYE_BMP_FILE				nuTEXT("Media\\pic\\ccd.bmp")

#define ZOOM_SCREEN_OBJ_SHP_COUNT			50
#define ZOOM_SCREEN_OBJ_POINT_COUNT			500
typedef struct tagZOOMROADOBJ
{
	SCREENHEAD		head;
	SCREENOBJSHP	pShape[ZOOM_SCREEN_OBJ_SHP_COUNT];
	nuroPOINT		ptVertex[ZOOM_SCREEN_OBJ_POINT_COUNT];
}ZOOMROADOBJ, *PZOOMROADOBJ;

CDrawMgrZ::CDrawMgrZ()
{
	m_pScreenMap = NULL;
}

CDrawMgrZ::~CDrawMgrZ()
{
	Free();
}

nuBOOL CDrawMgrZ::Initialize()
{
	PDRAWMEMORY pMemData = (PDRAWMEMORY)g_pDMLibFS->FS_GetData(DATA_DRAWMEM);
	if( pMemData == NULL )
	{
		return nuFALSE;
	}
	pMemData->nLineMemIdx	= -1;
	pMemData->nMapObjMemIdx	= -1;
	pMemData->nMapSHPMemIdx	= -1;
	if( !m_zoomRoadCol.Initialize(g_pDMMapCfg->commonSetting.nRoadTypeCount, ZOOM_ROAD_COL_MEM_SIZE) )
	{
		return nuFALSE;
	}
	if( !m_drawReal3Dpic.Initialize() )
	{
		return nuFALSE;
	}
	m_pMemoMgr = NULL;
	if( !nuReadConfigValue("CROSSEXTENDDIS", &m_nCrossExtendDis) )
	{
		m_nCrossExtendDis = 5;
	}

	nuLONG ntmp = 0;
	if( !nuReadConfigValue("DRAWRAWCAR", &ntmp) )
	{
		ntmp = 1;
	}
	m_bDrawRawCar = (ntmp != 0) ? nuTRUE : nuFALSE;
	if( !nuReadConfigValue("CROSSICON", &ntmp) )
	{
		ntmp = 1;
	}
	m_bDrawCrossIcon = (nuBYTE)ntmp;
	if( !nuReadConfigValue("DOTTEDLINECLASS", &ntmp) )
	{
		ntmp = 9;
	}
	m_nDottedLineClass = (nuBYTE)ntmp;
	if( !nuReadConfigValue("ZOOMARROWWIDTH", &ntmp) )
	{
		ntmp = 8;
	}
	m_nZoomArrowWidth	= (nuBYTE)ntmp;
	nuMemset(&m_bmpEEye, 0, sizeof(m_bmpEEye));
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, g_pDMGdata->pTsPath);
	nuTcscat(tsFile, _EEYE_BMP_FILE);
	g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpEEye, tsFile);
	m_bmpEEye.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;
	return nuTRUE;
}

nuVOID CDrawMgrZ::Free()
{
	if( m_pMemoMgr != NULL )
	{
		m_pMemoMgr->Free();
		delete m_pMemoMgr;
		m_pMemoMgr = NULL;
	}
	if( m_bmpEEye.bmpHasLoad )
	{
		g_pDMLibGDI->GdiDelNuroBMP(&m_bmpEEye);
	}
	m_drawReal3Dpic.Free();
	m_zoomRoadCol.Free();
}

nuBOOL CDrawMgrZ::AllocDrawMem()
{
	PDRAWMEMORY pMemData = (PDRAWMEMORY)g_pDMLibFS->FS_GetData(DATA_DRAWMEM);
	if( pMemData == NULL )
	{
		return nuFALSE;
	}
	if( g_pDMLibMM->MM_GetDataMassAddr(pMemData->nLineMemIdx) == NULL )
	{
		g_pDMLibMM->MM_GetDataMemMass( MEM_SIZE_NAVI_LINE, &pMemData->nLineMemIdx );
	}
	if( g_pDMLibMM->MM_GetDataMassAddr(pMemData->nMapSHPMemIdx) == NULL )
	{
		g_pDMLibMM->MM_GetDataMemMass( sizeof(SCREENOBJ), &pMemData->nMapSHPMemIdx);
	}
	/*
	if( g_pDMLibMM->MM_GetDataMassAddr(pMemData->nMapObjMemIdx) == NULL )
	{
		g_pDMLibMM->MM_GetDataMemMass( MEM_SIZE_MAP_OBJ, &pMemData->nMapObjMemIdx);
	}
	*/
	return nuTRUE;
}

nuBOOL CDrawMgrZ::FreeDrawMem()
{
	PDRAWMEMORY pMemData = (PDRAWMEMORY)g_pDMLibFS->FS_GetData(DATA_DRAWMEM);
	if( pMemData == NULL )
	{
		return nuFALSE;
	}
	g_pDMLibMM->MM_RelDataMemMass(&pMemData->nLineMemIdx );
	g_pDMLibMM->MM_RelDataMemMass(&pMemData->nMapSHPMemIdx);
	/*
	g_pDMLibMM->MM_RelDataMemMass(&pMemData->nMapObjMemIdx);
	*/
	return nuTRUE;
}

nuBOOL CDrawMgrZ::CollectDrawMem()
{
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
	nuMemset(&pScObj->head, 0, sizeof(SCREENHEAD));
	nuMemset(pScObj->pShpCount, 0, sizeof(SCREENHEAD)*SHPOJB_TYPE_COUNT);
	g_pRoadName->ResetRdNameList();
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawBgArea(nuPVOID lpVoid, nuBYTE nDrawMode)
{
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
//	CollectShp(pScObj);
	/*
	nuINT nDrawType;
	nuBYTE nObjType = 255;
	nuBYTE nType = 255;
	for(nuLONG i = pScObj->head.nShapCount-1; i >= 0; --i )
	{
		if( nObjType != pScObj->pShape[i].objType ||
			nType != pScObj->pShape[i].nType )
		{
			nObjType = pScObj->pShape[i].objType;
			nType = pScObj->pShape[i].nType;
			switch ( nObjType )
			{
			case SHPOBJ_TYPE_ROAD:
				break;
			case SHPOBJ_TYPE_BGLINE:
				break;
			case SHPOBJ_TYPE_BGAREA:
				break;
			default:
				nDrawType = DRAW_OBJ_SKIP;
				break;
			}
		}
		if( nDrawType == DRAW_OBJ_SKIP )
		{
			continue;
		}
	}
	*/
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawRoad(nuPVOID lpVoid, nuBYTE nDrawMode /* = DRAWRD_MODE_COMMON */)
{
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
	//Draw road rim
	nuLONG i;
	nuINT nType = -1, nObjType = DRAW_OBJ_SKIP;
	nuLONG nLineWidth = 0;
	nuINT nIdx = 0;
	if( nDrawMode == DRAWRD_MODE_COMMON )
	{
		for( i = 0; i < pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; ++i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ROADRIM, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					&nLineWidth );
			}
			if( nObjType != DRAW_OBJ_SKIP && nObjType != DRWA_OBJ_ONLYCOLNAME )
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, nLineWidth);
			}
			nIdx += pScObj->pShape[i].nPointCount;
		}
	}
	//Draw road center
	if( nDrawMode == DRAWRD_MODE_COMMON || nDrawMode == DRAWRD_MODE_ONLYCENTER )
	{
		nType = -1;
		nIdx = 0;
		nObjType = DRAW_OBJ_SKIP;
		for( i = 0; i < pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; ++i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ROADCENTER, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					&nLineWidth );
			}
			if( nObjType != DRAW_OBJ_SKIP )
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, nLineWidth);
			}
			nIdx += pScObj->pShape[i].nPointCount;
		}
	}
	if( nDrawMode == DRAWRD_MODE_FAKELINE )
	{
		nType = -1;
		nIdx = 0;
		nObjType = DRAW_OBJ_SKIP;
		for( i = 0; i < pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; ++i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOMFAKE_LINE, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					NULL );
			}
			if( nObjType != DRAW_OBJ_SKIP )
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, 0);
			}
			nIdx += pScObj->pShape[i].nPointCount;
		}
	}
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawBgLine(nuPVOID lpVoid, nuBYTE nDrawMode /* = DRAWRD_MODE_COMMON */)
{
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
	//Draw bgline rim
	nuLONG i = 0;
	nuINT nType = -1, nObjType = DRAW_OBJ_SKIP;
	nuLONG nLineWidth = 0;
	nuINT nIdx = 0;
	if( nDrawMode == DRAWRD_MODE_COMMON )
	{
		nType = -1;
		nIdx = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex;
		nObjType = DRAW_OBJ_SKIP;
		for( i = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; i < pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount; ++i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ROADRIM, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					&nLineWidth );
			}
			if( nObjType != DRAW_OBJ_SKIP && nObjType != DRWA_OBJ_ONLYCOLNAME )
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, nLineWidth);
			}
			nIdx += pScObj->pShape[i].nPointCount;
		}
	}
	//Draw bgline center
	if( nDrawMode == DRAWRD_MODE_COMMON || nDrawMode == DRAWRD_MODE_ONLYCENTER )
	{
		nType = -1;
		nIdx = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex + pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nTotalVertex; 
		nObjType = DRAW_OBJ_SKIP;
		/*
		for( i = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; i < pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount; ++i)
		{
			nIdx += pScObj->pShape[i].nPointCount;
		}
		*/
		for( i = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount + pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount - 1; i >= pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; --i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ROADCENTER, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					&nLineWidth );
			}
//			g_pDMLibGDI->GdiPolyline(&pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount);
			/*
			*/
			nIdx -= pScObj->pShape[i].nPointCount;
			if( nObjType != DRAW_OBJ_SKIP)
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, nLineWidth);
			}
		}
	}
	//Draw bgline fakeline
	if( nDrawMode == DRAWRD_MODE_FAKELINE )
	{
		nType = -1;
		nIdx = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex;
		nObjType = DRAW_OBJ_SKIP;
		for( i = pScObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount; i < pScObj->pShpCount[SHPOBJ_TYPE_BGLINE].nShapCount; ++i)
		{
			if( nType != (nuINT)pScObj->pShape[i].nType )
			{
				nType = pScObj->pShape[i].nType;
				nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOMFAKE_LINE, 
					g_pDMGdata->uiSetData.b3DMode, 
					&g_pDMMapCfg->scaleSetting.pRoadSetting[nType],
					&nLineWidth );
			}
			if( nObjType != DRAW_OBJ_SKIP )
			{
				g_pDMLibMT->MT_DrawObject(nObjType, &pScObj->ptVertex[nIdx], pScObj->pShape[i].nPointCount, nLineWidth);
			}
			nIdx += pScObj->pShape[i].nPointCount;
		}
	}
	return nuTRUE;
}

nuBOOL CDrawMgrZ::ZoomCrossProc(nuPVOID lpVoid)
{
	PZOOMCROSSSTATE pZoomState = (PZOOMCROSSSTATE)lpVoid;
	if( pZoomState->nPicID > 0 )
	{
		if( m_drawReal3Dpic.DrawReal3DPic(pZoomState->nPicID) )
		{
			return nuTRUE;
		}
		pZoomState->nPicID = 0;
	}
	//
	nuDWORD nMode = DW_DATA_MODE_SMALL | DW_DATA_MODE_RDW;
	PDWBLKDATASMALL pDwBlkDataSmall = (PDWBLKDATASMALL)g_pDMLibFS->FS_LoadDwBlock(g_pDMGdata->zoomScreenData.nuZoomMapSize, 
		nMode);//Load Data needed
	if( pDwBlkDataSmall == NULL || pDwBlkDataSmall->nBlkCount == 0 )
	{
		return nuFALSE;
	}
	nuBOOL bRes = DrawZoomCrossRoad(lpVoid, pDwBlkDataSmall);
	if( m_bDrawCrossIcon )
	{
		bRes &= DrawZoomCrossPoi(lpVoid, pDwBlkDataSmall);
	}
	g_pDMLibFS->FS_FreeDwBlock(nMode);
	//..Draw NaviInfo
	bRes &= DrawZoomNaviRoad();
	return bRes;
}

#define CROSS_EXTEND_MIN_DIS		30
nuBOOL CDrawMgrZ::DrawZoomCrossRoad(nuPVOID lpVoid, PDWBLKDATASMALL pDwBlkDataSmall)
{
	PZOOMCROSSSTATE pZoomState = (PZOOMCROSSSTATE)lpVoid;
	if( pZoomState == NULL )
	{
		return nuFALSE;
	}
	PDWROADBLK pDwRoadBlk = new DWROADBLK[pDwBlkDataSmall->nBlkCount];
	if( pDwRoadBlk == NULL )
	{
		return nuTRUE;
	}
	nuMemset(pDwRoadBlk, 0, sizeof(DWROADBLK)*pDwBlkDataSmall->nBlkCount);
	//
	nuWORD nObjMemIdx = 0;
	if( g_pDMLibMM->MM_GetDataMemMass(sizeof(ZOOMROADOBJ), &nObjMemIdx) == NULL )
	{
		delete []pDwRoadBlk;
		pDwRoadBlk=NULL;
		return nuFALSE;
	}
	m_zoomRoadCol.DelLineList();//里面有GetDataMemMass（），所以在这之后只有不能再分配内存
	
	nuroRECT rtBlock = {0};
	nuroRECT rtRoad  = {0};
	nuBYTE i =0;
	CROSSZOOMROADEX czrdExList[10] = {0};
	nuINT nCzrdExCount = 0;
	//Collect ZoomRoad...
	for( i = 0; i < pDwBlkDataSmall->nBlkCount; ++i )
	{
		nuPBYTE pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(pDwBlkDataSmall->pDwList[i].nRdwMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		CMapDataConstructZ::ColRdwBlock(pData, &pDwRoadBlk[i]);
		if( pDwRoadBlk[i].pDwRoadHead == NULL || pDwRoadBlk[i].pDwRoadHead->nRoadCount == 0 )
		{
			continue;
		}
		nuroPOINT ptFrom = {0}, ptTo = {0};
		nuBlockIDtoRect(pDwBlkDataSmall->pDwList[i].nBlkID, &rtBlock);
		for(nuDWORD j = 0; j < pDwRoadBlk[i].pDwRoadHead->nRoadCount; ++j)
		{
			nuBYTE nRoadClass = pDwRoadBlk[i].pDwRoadCommon[ pDwRoadBlk[i].pDwRoadData[j].nRoadCommonIdx ].nRoadType;
			if( nRoadClass >= g_pDMMapCfg->commonSetting.nRoadTypeCount ||
				( !g_pDMMapCfg->zoomSetting.pRoadSetting[nRoadClass].nShowRimLine &&
				  !g_pDMMapCfg->zoomSetting.pRoadSetting[nRoadClass].nShowCenterLine) )
			{
				continue;
			}
			/*
			rtRoad.left		= pDwRoadBlk[i].pRoadRect[j].left + rtBlock.left;
			rtRoad.right	= pDwRoadBlk[i].pRoadRect[j].right + rtBlock.left;
			rtRoad.top		= pDwRoadBlk[i].pRoadRect[j].top + rtBlock.top;
			rtRoad.bottom	= pDwRoadBlk[i].pRoadRect[j].bottom + rtBlock.top;
			if( !nuRectCoverRect(&rtRoad, &g_pDMGdata->zoomScreenData.nuZoomMapSize) )
			{
				continue;
			}
			*/
			
			ptFrom.x = ptTo.x = rtBlock.left;
			ptFrom.y = ptTo.y = rtBlock.top;
			ptFrom.x += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx ].x;
			ptFrom.y += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx ].y;
			ptTo.x += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx + pDwRoadBlk[i].pDwRoadData[j].nVertexCount - 1 ].x;
			ptTo.y += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx + pDwRoadBlk[i].pDwRoadData[j].nVertexCount - 1 ].y;
			nuLONG dx1 = pZoomState->ptCenter.x - ptFrom.x;
			nuLONG dy1 = pZoomState->ptCenter.y - ptFrom.y;
			nuLONG dx2 = pZoomState->ptCenter.x - ptTo.x;
			nuLONG dy2 = pZoomState->ptCenter.y - ptTo.y;
			if( ( pZoomState->nCenterRdBlkIdx == pDwBlkDataSmall->pDwList[i].nBlkIdx && pZoomState->nCenterRdIdx == j ) || 
				( NURO_ABS(dx1) < m_nCrossExtendDis && NURO_ABS(dy1) < m_nCrossExtendDis ) ||
				( NURO_ABS(dx2) < m_nCrossExtendDis && NURO_ABS(dy2) < m_nCrossExtendDis ) )
			{
				m_zoomRoadCol.AddLineSeg(0, i, j, ptFrom, ptTo, nRoadClass);
				nuLONG dx = ptFrom.x - ptTo.x;
				nuLONG dy = ptFrom.y - ptTo.y;
				if( NURO_ABS(dx) < CROSS_EXTEND_MIN_DIS && 
					NURO_ABS(dy) < CROSS_EXTEND_MIN_DIS && 
					nCzrdExCount < 10 )
				{
					if( pZoomState->ptCenter.x == ptFrom.x && pZoomState->ptCenter.y == ptFrom.y )
					{
						czrdExList[ nCzrdExCount ].point = ptTo;
					}
					else
					{
						czrdExList[ nCzrdExCount ].point = ptFrom;
					}
					czrdExList[ nCzrdExCount ].nRoadClass = nRoadClass;
					++nCzrdExCount;
				}
			}
			
		}
	}
	/* 20090117 
	for( i = 0; i < pDwBlkDataSmall->nBlkCount; ++i )
	{
		if( pDwRoadBlk[i].pDwRoadHead == NULL || pDwRoadBlk[i].pDwRoadHead->nRoadCount == 0 )
		{
			continue;
		}
		nuBlockIDtoRect(pDwBlkDataSmall->pDwList[i].nBlkID, &rtBlock);
		nuroPOINT ptFrom, ptTo;
		for(nuDWORD j = 0; j < pDwRoadBlk[i].pDwRoadHead->nRoadCount; ++j)
		{
			nuBYTE nRoadClass = pDwRoadBlk[i].pDwRoadCommon[ pDwRoadBlk[i].pDwRoadData[j].nRoadCommonIdx ].nRoadType;
			if( nRoadClass >= g_pDMMapCfg->commonSetting.nRoadTypeCount ||
				( !g_pDMMapCfg->zoomSetting.pRoadSetting[nRoadClass].nShowRimLine &&
				!g_pDMMapCfg->zoomSetting.pRoadSetting[nRoadClass].nShowCenterLine) )
			{
				continue;
			}
			
			ptFrom.x = ptTo.x = rtBlock.left;
			ptFrom.y = ptTo.y = rtBlock.top;
			ptFrom.x += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx ].x;
			ptFrom.y += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx ].y;
			ptTo.x += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx + pDwRoadBlk[i].pDwRoadData[j].nVertexCount - 1 ].x;
			ptTo.y += pDwRoadBlk[i].pSPointCoor[ pDwRoadBlk[i].pDwRoadData[j].nVertexAddrIdx + pDwRoadBlk[i].pDwRoadData[j].nVertexCount - 1 ].y;
			//
			for( nuINT k = 0; k < nCzrdExCount; ++k )
			{
				if( czrdExList[k].nRoadClass == nRoadClass &&
					((czrdExList[k].point.x == ptFrom.x && czrdExList[k].point.y == ptFrom.y ) ||
					(czrdExList[k].point.x == ptTo.x && czrdExList[k].point.y == ptTo.y)) )
				{
					m_zoomRoadCol.AddLineSeg(0, i, j, ptFrom, ptTo, nRoadClass);
				}
			}
		}
	}
	*/
	//
	//Transfer points of Zoom objs to Screen points...
	PCONJLINE pConjLine = NULL;
	PLINESEG pLineSeg = NULL;
	NUROPOINT ptTmp1 = {0}, ptTmp2 = {0};
	nuLONG nSkipLen = 0, dx = 0, dy = 0;
	nuINT nRoadPtIdx =0, nRoadPtStep =0, nObjType=0;
	nuLONG nNowCount = 0;
	nuWORD nRoadPtCount=0;
	PZOOMROADOBJ pZoomObj = (PZOOMROADOBJ)g_pDMLibMM->MM_GetDataMassAddr(nObjMemIdx);
	pZoomObj->head.nShapCount	= 0;
	pZoomObj->head.nTotalVertex = 0;
	nuDWORD nBlockID = -1;
	nuWORD nCls =0;
	//Transfer points of NaviRoad to Screen points...
	/*
	if( g_pDMGdata->drawInfoNavi.nextCross.nCrossPtCount > 1)
	{
		for(i = 0; i < g_pDMGdata->drawInfoNavi.nextCross.nCrossPtCount; ++i)
		{
			g_pDMLibMT->MT_ZoomMapToBmp(g_pDMGdata->drawInfoNavi.nextCross.ptCrossList[i].x, 
				g_pDMGdata->drawInfoNavi.nextCross.ptCrossList[i].y,
				&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].x, 
				&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].y );
			++(pZoomObj->head.nTotalVertex);
		}
		pZoomObj->pShape[ pZoomObj->head.nShapCount ].nPointCount	= (nuWORD)pZoomObj->head.nTotalVertex;
		pZoomObj->pShape[ pZoomObj->head.nShapCount ].nType			= (nuBYTE)0;
		++(pZoomObj->head.nShapCount);
	}*/
	//Transfer points of ZoomRoad to Screen points...
	for(nCls = 0; nCls < m_zoomRoadCol.m_nClassCount; ++nCls)
	{
		if( pZoomObj->head.nTotalVertex == ZOOM_SCREEN_OBJ_POINT_COUNT ||
			pZoomObj->head.nShapCount   == ZOOM_SCREEN_OBJ_SHP_COUNT )
		{
			break;
		}
		if( m_zoomRoadCol.m_pLineList[nCls].pConjHead	== NULL )
		{
			continue;
		}
		nSkipLen = g_pDMMapCfg->zoomSetting.pRoadSetting[nCls].nCenterLineWidth *
			g_pDMMapCfg->zoomSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;
		pConjLine = m_zoomRoadCol.m_pLineList[nCls].pConjHead;
		while( pConjLine != NULL )
		{
			ptTmp1	= pConjLine->ptFrom;
			g_pDMLibMT->MT_ZoomMapToBmp( ptTmp1.x, 
				ptTmp1.y, 
				&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].x, 
				&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].y );
			nNowCount	= 1;
			++(pZoomObj->head.nTotalVertex);
			pLineSeg = pConjLine->pLineHead;
			while( pLineSeg != NULL )
			{
				PDWROADBLK pTmpRdBlk = &pDwRoadBlk[pLineSeg->nScreenBlkIdx];
				nRoadPtCount = pTmpRdBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nVertexCount;
				if( pLineSeg->nConjWay == CONJ_FROM_TO )
				{
					nRoadPtIdx  = 0;
					nRoadPtStep = 1;
				}
				else
				{
					nRoadPtIdx = nRoadPtCount - 1;
					nRoadPtStep = -1;
				}
				//
				if( nBlockID != pDwBlkDataSmall->pDwList[ pLineSeg->nScreenBlkIdx ].nBlkID )
				{
					nBlockID = pDwBlkDataSmall->pDwList[ pLineSeg->nScreenBlkIdx ].nBlkID;
					nuBlockIDtoRect(nBlockID, &rtBlock);
				}
				while( nRoadPtCount > 0 )
				{
					ptTmp2.x = pTmpRdBlk->pSPointCoor[ pTmpRdBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].x + rtBlock.left;
					ptTmp2.y = pTmpRdBlk->pSPointCoor[ pTmpRdBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].y + rtBlock.top;
					dx = ptTmp1.x - ptTmp2.x;
					dy = ptTmp1.y - ptTmp2.y;
					if( NURO_ABS(dx) >= nSkipLen || NURO_ABS(dy) >= nSkipLen )
					{
						if( pZoomObj->head.nTotalVertex == ZOOM_SCREEN_OBJ_POINT_COUNT )
						{
							break;
						}
						g_pDMLibMT->MT_ZoomMapToBmp( ptTmp2.x, 
							ptTmp2.y, 
							&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].x, 
							&pZoomObj->ptVertex[pZoomObj->head.nTotalVertex].y );
						++(pZoomObj->head.nTotalVertex);
						++nNowCount;
						ptTmp1 = ptTmp2;
					}
					nRoadPtCount --;
					nRoadPtIdx += nRoadPtStep;
				}
				pLineSeg = pLineSeg->pNext;
				if( pLineSeg == NULL || pZoomObj->head.nTotalVertex == ZOOM_SCREEN_OBJ_POINT_COUNT )
				{
					if( nNowCount > 1 )
					{
						pZoomObj->pShape[ pZoomObj->head.nShapCount ].nPointCount	= (nuWORD)nNowCount;
						pZoomObj->pShape[ pZoomObj->head.nShapCount ].nType			= (nuBYTE)nCls;
						++(pZoomObj->head.nShapCount);
					}
					else
					{
						pZoomObj->head.nTotalVertex -= (nuWORD)nNowCount;
					}
					nNowCount = 0;
					break;
				}
			}
			if( pZoomObj->head.nTotalVertex == ZOOM_SCREEN_OBJ_POINT_COUNT ||
				pZoomObj->head.nShapCount == ZOOM_SCREEN_OBJ_SHP_COUNT )
			{
				break;
			}
			pConjLine = pConjLine->pNext;
		}
	}
	//Draw Road... 
	nuLONG nLineWidth = 0;
	nNowCount = 0;
	nCls = -1;
	nObjType = DRAW_OBJ_SKIP;
	nuINT n;
	for(n = 0; n < pZoomObj->head.nShapCount; ++n)
	{
		if( nCls != pZoomObj->pShape[n].nType )
		{
			nCls = pZoomObj->pShape[n].nType;
			nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOM_ROADRIM, 
				g_pDMLibFS->pGdata->zoomScreenData.b3DMode, 
				&g_pDMMapCfg->zoomSetting.pRoadSetting[nCls],
				&nLineWidth );
		}
		if( nObjType != DRAW_OBJ_SKIP )
		{
			g_pDMLibMT->MT_DrawObject(nObjType, 
				&pZoomObj->ptVertex[nNowCount], 
				pZoomObj->pShape[n].nPointCount,
				nLineWidth);
		}
		nNowCount += pZoomObj->pShape[n].nPointCount;
	}//Draw road rim...
	
	nCls = -1;
	nObjType = DRAW_OBJ_SKIP;
	nNowCount = pZoomObj->head.nTotalVertex;
	for(n = pZoomObj->head.nShapCount - 1; n >=0; --n)
	{
		if( nCls != pZoomObj->pShape[n].nType )
		{
			nCls = pZoomObj->pShape[n].nType;
			nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOM_ROADCENTER, 
					g_pDMLibFS->pGdata->zoomScreenData.b3DMode, 
					&g_pDMMapCfg->zoomSetting.pRoadSetting[nCls],
					&nLineWidth);
		}
		nNowCount -= pZoomObj->pShape[n].nPointCount;
		if( nObjType != DRAW_OBJ_SKIP )
		{
			g_pDMLibMT->MT_DrawObject( nObjType, 
				&pZoomObj->ptVertex[nNowCount], 
				pZoomObj->pShape[n].nPointCount, 
				nLineWidth);
		}
	}//Draw road center...
	
	nCls = -1;
	nObjType = DRAW_OBJ_SKIP;
	nNowCount = pZoomObj->head.nTotalVertex;
	for ( n = pZoomObj->head.nShapCount - 1; n >=0; --n )
	{
		nNowCount -= pZoomObj->pShape[n].nPointCount;
		if( pZoomObj->pShape[n].nType >= m_nDottedLineClass )
		{
			continue;
		}
		if( nCls != pZoomObj->pShape[n].nType )
		{
			nCls = pZoomObj->pShape[n].nType;
			nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOM_ROADCLINE, 
				g_pDMLibFS->pGdata->zoomScreenData.b3DMode, 
				&g_pDMMapCfg->zoomSetting.pRoadSetting[nCls],
				&nLineWidth );
		}
		if( nObjType != DRAW_OBJ_SKIP )
		{
			g_pDMLibMT->MT_DrawObject( nObjType, 
				&pZoomObj->ptVertex[nNowCount], 
				pZoomObj->pShape[n].nPointCount, 
				nLineWidth);
		}
	}//Draw Center dotted line
	//Release resource...
	m_zoomRoadCol.FreeDataMem();
	g_pDMLibMM->MM_RelDataMemMass(&nObjMemIdx);
	delete []pDwRoadBlk;
	pDwRoadBlk=NULL;
	return nuTRUE;
}

#define _RED_ARROW_LEN				60
#define _RED_ARROW_LEN_1			100
nuBOOL CDrawMgrZ::DrawZoomNaviRoad()
{
	nuLONG nLineWidth = 12;
	nuINT nDrawObj = g_pDMLibMT->MT_SelectObject(DRAW_TYPE_ZOOM_NAVI_ROAD, 
		g_pDMLibFS->pGdata->zoomScreenData.b3DMode, 
		&g_pDMLibFS->pMapCfg->naviLineSetting, 
		&nLineWidth );
	nuroPOINT ptList[CROSS_ARROW_POINT_NUM] = {0};
	nuINT nNowPoint = 0;
	nuINT nCrossIdx = g_pDMGdata->drawInfoNavi.nextCross.nIdxCrossInList;
	nuINT i;
	for(i = 0; i < g_pDMGdata->drawInfoNavi.nextCross.nCrossPtCount; ++i)
	{
		g_pDMLibMT->MT_ZoomMapToBmp(g_pDMGdata->drawInfoNavi.nextCross.ptCrossList[i].x,
			g_pDMGdata->drawInfoNavi.nextCross.ptCrossList[i].y,
			&ptList[nNowPoint].x,
			&ptList[nNowPoint].y );
		++nNowPoint;
	}
//	g_pDMLibMT->MT_DrawObject(nDrawObj, ptList, nNowPoint, nLineWidth);//Draw naviline
	if( nCrossIdx != -1 )
	{
		nDrawObj = DRAW_OBJ_ZOOM_2D_ARROWLINE;
		nLineWidth = 8;
		nuINT nStartIdx, nEndIdx;
		nuLONG dx, dy, Dis = 0, nNowDis;
		nuroPOINT ptTmp;
		for( nStartIdx = nCrossIdx; nStartIdx > 0; --nStartIdx )
		{
			dx = ptList[nStartIdx].x - ptList[nStartIdx - 1].x;
			dy = ptList[nStartIdx].y - ptList[nStartIdx - 1].y;
			nNowDis = (nuLONG)nuSqrt(dx*dx + dy*dy);
			Dis += nNowDis;
			if( Dis >= _RED_ARROW_LEN_1 )
			{
				Dis -= _RED_ARROW_LEN_1;
				ptTmp.x = ptList[nStartIdx-1].x + dx * Dis / nNowDis;
				ptTmp.y = ptList[nStartIdx-1].y + dy * Dis / nNowDis;
				ptList[nStartIdx-1] = ptTmp;
				--nStartIdx;
				break;
			}
		}
		Dis = 0;
		for( nEndIdx = nCrossIdx; nEndIdx < nNowPoint - 1; ++nEndIdx )
		{
			dx = ptList[nEndIdx+1].x - ptList[nEndIdx].x;
			dy = ptList[nEndIdx+1].y - ptList[nEndIdx].y;
			nNowDis = (nuLONG)nuSqrt(dx*dx + dy*dy);
			Dis += nNowDis;
			if( Dis >= _RED_ARROW_LEN )
			{
				Dis = nNowDis + _RED_ARROW_LEN - Dis;
				ptList[nEndIdx+1].x = ptList[nEndIdx].x + dx * Dis / nNowDis;
				ptList[nEndIdx+1].y = ptList[nEndIdx].y + dy * Dis / nNowDis;
				++nEndIdx;
				break;
			}
		}
		//@Zhikun 2009.07.14
		nuINT nMinI = NURO_MIN( nCrossIdx, (nNowPoint - 1 - nCrossIdx) );
		for( i = 0; i < nMinI; ++i )
		{
			if ( nStartIdx-i-1 >=0 )
			{
			if( ptList[nStartIdx-i-1].x == ptList[nStartIdx+i+1].x &&
				ptList[nStartIdx-i-1].y == ptList[nStartIdx+i+1].y )
			{
				ptList[nStartIdx-i-1].x += 5;
				ptList[nStartIdx+i+1].x -= 5;
			}
		}
			
		}
		g_pDMLibMT->MT_DrawObject(nDrawObj, &ptList[nStartIdx], nEndIdx - nStartIdx + 1, m_nZoomArrowWidth/*nLineWidth*/);
	}
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawZoomCrossPoi(nuPVOID lpVoid, PDWBLKDATASMALL pDwBlkDataSmall)
{
	PZOOMCROSSSTATE pZoomState = (PZOOMCROSSSTATE)lpVoid;
	if( pZoomState == NULL )
	{
		return nuFALSE;
	}
	nuroRECT rtBlock = {0}, rtZoomBmp = {0};
	nuroPOINT point = {0};
	DWPOIBLK dwPoiBlk = {0};
	nuPBYTE pData = {0};
	PPTNODE pPtNode = NULL;
	rtZoomBmp.left	= 0;
	rtZoomBmp.right = g_pDMGdata->zoomScreenData.nuZoomBmp.bmpWidth;
	rtZoomBmp.top	= 0;
	rtZoomBmp.bottom= g_pDMGdata->zoomScreenData.nuZoomBmp.bmpHeight;
	for ( nuINT i = 0; i < pDwBlkDataSmall->nBlkCount; ++i )
	{
		if( pDwBlkDataSmall->pDwList[i].nPdwMemIdx == (nuWORD)(-1) )
		{
			continue;
		}
		nuBlockIDtoRect(pDwBlkDataSmall->pDwList[i].nBlkID, &rtBlock);
		if( !nuRectCoverRect(&rtBlock, &g_pDMGdata->zoomScreenData.nuZoomMapSize) )
		{
			continue;
		}
		pData = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(pDwBlkDataSmall->pDwList[i].nPdwMemIdx);
		if( pData == NULL )
		{
			continue;
		}
		CMapDataConstructZ::ColPdwBlock(pData, &dwPoiBlk);
		for( nuDWORD j = 0; j < dwPoiBlk.pDwPOIHead->nPOICount; ++j )
		{
			point.x = rtBlock.left + dwPoiBlk.pDwPOIData[j].spCoor.x;
			point.y = rtBlock.top  + dwPoiBlk.pDwPOIData[j].spCoor.y;
			if( !nuPointInRect(&point, &g_pDMGdata->zoomScreenData.nuZoomMapSize) )
			{
				continue;
			}
			if( dwPoiBlk.pDwPOIData[j].nPOIType1 < 0 || 
				dwPoiBlk.pDwPOIData[j].nPOIType1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
			{
				continue;
			}
			//pPtNode = (PPTNODE)g_pDMLibFS->FS_GetPtNodeByType( dwPoiBlk.pDwPOIData[j].nPOIType1, 
			//	dwPoiBlk.pDwPOIData[j].nPOIType2 ); //daniel modified 20160425
			if( pPtNode == NULL /*|| !pPtNode->nDisplay */)
			{
				continue;
			}
			g_pDMLibMT->MT_ZoomMapToBmp(point.x, point.y, &point.x, &point.y);
			if( !nuPointInRect(&point, &rtZoomBmp) )
			{
				continue;
			}
			g_pDMLibGDI->GdiDrawPoi( point.x, point.y, pPtNode->nResID - POI_ICON_START_NUM, NURO_RTPOS_CB, nuFALSE);
		}
	}
	return nuTRUE;
}

#define TRAVEL_PT_R			5
nuBOOL CDrawMgrZ::DrawTrace(nuPVOID pVoid)
{
	PTRACEMEMORY pTraceMem = (PTRACEMEMORY)g_pDMLibFS->FS_GetData(DATA_TRACEMEM);
	if( pTraceMem == NULL || pTraceMem->nShowTrace == TRACESHOW_STATE_NO)
	{
		return nuFALSE;
	}
	nuroPOINT* ptList = NULL;
	nuWORD nPtCount =0;
	while (pTraceMem->bLock)
	{
		nuSleep(TIMER_TIME_UNIT);
	}
	pTraceMem->bLock = 1;
	if( pTraceMem->nShowTrace == TRACESHOW_STATE_RECORD )
	{
		ptList = (nuroPOINT*)g_pDMLibMM->MM_GetDataMassAddr(pTraceMem->nShowTraceMemIdx);
		nPtCount = pTraceMem->nShowCount;
	}
	else
	{
		ptList = (nuroPOINT*)g_pDMLibMM->MM_GetDataMassAddr(pTraceMem->nNewTraceMemIdx);
		nPtCount = pTraceMem->nPointCount;
	}
	if( ptList )
	{
		NUROPEN nuPen = {0};
		nuPen.nRed		= 0;
		nuPen.nGreen	= 255;
		nuPen.nBlue		= 0;
		nuPen.nStyle	= NURO_PS_SOLID;
		nuPen.nWidth	= 1;
		g_pDMLibGDI->GdiSetPen(&nuPen);
		NUROBRUSH nuBrush  = {0};
		nuBrush.nRed	= 0;
		nuBrush.nGreen	= 0;
		nuBrush.nBlue	= 255;
		g_pDMLibGDI->GdiSetBrush(&nuBrush);
		nuroPOINT point = {0, 0}, ptScreen;
		nuLONG dx = 0, dy = 0;
		nuLONG nSkipLen = TRAVEL_PT_R * 2 * g_pDMMapCfg->scaleSetting.scaleTitle.nScale / 
			g_pDMGdata->drawInfoMap.nScalePixels;
		for(nuWORD i = 0; i < nPtCount; ++i)
		{
			if( !nuPointInRect(&ptList[i], &g_pDMGdata->transfData.nuShowMapSize) )
			{
				continue;
			}
			dx = ptList[i].x - point.x;
			dy = ptList[i].y - point.y;
			dx = NURO_ABS(dx);
			dy = NURO_ABS(dy);
			if( dx < nSkipLen && dy < nSkipLen )
			{
				continue;
			}
			point = ptList[i];
			g_pDMLibMT->MT_MapToBmp( ptList[i].x, ptList[i].y, &ptScreen.x, &ptScreen.y);
			if( g_pDMGdata->uiSetData.b3DMode )
			{
				g_pDMLibMT->MT_Bmp2Dto3D(ptScreen.x, ptScreen.y, MAP_DEFAULT);
			}
			g_pDMLibGDI->GdiEllipse( ptScreen.x - TRAVEL_PT_R, 
				ptScreen.y - TRAVEL_PT_R,
				ptScreen.x + TRAVEL_PT_R,
				ptScreen.y + TRAVEL_PT_R );
		}
	}
	pTraceMem->bLock = 0;
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawMemo(nuPVOID lpVoid)
{
	POTHERGLOBAL pOtherG = (POTHERGLOBAL)g_pDMLibFS->FS_GetData(DATA_OTHERGLOBAL);
	if( pOtherG == NULL || pOtherG->pMemoData == NULL )
	{
		return nuFALSE;
	}
	if( m_pMemoMgr == NULL )
	{
		m_pMemoMgr = new CMemoMgrZ();
		if( m_pMemoMgr != NULL )
		{
			m_pMemoMgr->Initialize();
		}
	}
	if( m_pMemoMgr == NULL )
	{
		return nuFALSE;
	}
	nuroRECT rtRect = {0}, rtScreen = {0};
	nuroPOINT point = {0};
	rtRect.left		= g_pDMLibFS->pGdata->carInfo.ptCarIcon.x - _MEMO_VOICE_DISTANCE;
	rtRect.top		= g_pDMLibFS->pGdata->carInfo.ptCarIcon.y - _MEMO_VOICE_DISTANCE;
	rtRect.right	= g_pDMLibFS->pGdata->carInfo.ptCarIcon.x + _MEMO_VOICE_DISTANCE;
	rtRect.bottom	= g_pDMLibFS->pGdata->carInfo.ptCarIcon.y + _MEMO_VOICE_DISTANCE;
	rtScreen.left	= g_pDMLibFS->pGdata->transfData.nBmpLTx;
	rtScreen.top	= g_pDMLibFS->pGdata->transfData.nBmpLTy;
	rtScreen.right	= g_pDMLibFS->pGdata->transfData.nBmpLTx + g_pDMLibFS->pGdata->transfData.nBmpWidth;
	rtScreen.bottom = g_pDMLibFS->pGdata->transfData.nBmpLTy + g_pDMLibFS->pGdata->transfData.nBmpHeight;
	m_pMemoMgr->RemoveOutMemo(&rtRect);
	for(nuDWORD i = 0; i < pOtherG->pMemoData->nNowCount; ++i)
	{
		point.x = pOtherG->pMemoData->pMemoList[i].x;
		point.y = pOtherG->pMemoData->pMemoList[i].y;
		if( !nuPointInRect(&point, &g_pDMLibFS->pGdata->transfData.nuShowMapSize) )
		{
			continue;
		}
		if( nuPointInRect(&point, &rtRect) )
		{
			nuLONG dx = point.x - g_pDMLibFS->pGdata->carInfo.ptCarIcon.x;
			nuLONG dy = point.y - g_pDMLibFS->pGdata->carInfo.ptCarIcon.y;
			nuLONG nDis = (nuLONG)nuSqrt(dx*dx+dy*dy);
			if( nDis < _MEMO_VOICE_DISTANCE && m_pMemoMgr->AddVoiceMemo(point.x, point.y) )
			{
				if(pOtherG->pMemoData->pMemoList[i].nVoiceIdx > 0)
				{
					g_pDMGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, pOtherG->pMemoData->pMemoList[i].nVoiceIdx, 0, 0, 0, NULL);
				}
			}
		}
		g_pDMLibMT->MT_MapToBmp(point.x, point.y, &point.x, &point.y);
		if( !nuPointInRect(&point, &rtScreen) )
		{
			continue;
		}
		if( g_pDMLibFS->pGdata->uiSetData.b3DMode )
		{
			g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
		}
		//Draw MemoIcon...
		m_pMemoMgr->DrawMemoIcon(point.x, point.y, pOtherG->pMemoData->pMemoList[i].nIconIdx);
		//Draw MemoName add @2011.07.27
		if( 4 == pOtherG->pMemoData->pMemoList[i].nIconIdx )
		{
			if( 0 != pOtherG->pMemoData->pMemoList[i].wsIconName[0] )
			{
				//设置字体颜色
				nuCOLORREF col = nuRGB(0, 0, 0);
				g_pDMLibGDI->GdiSetTextColor(col); 
				//
				nuINT nLen = nuWcslen(pOtherG->pMemoData->pMemoList[i].wsIconName);
				g_pDMLibGDI->GdiExtTextOutWEx(point.x - 10 * nLen / 2, point.y, pOtherG->pMemoData->pMemoList[i].wsIconName, 
					nLen, nuFALSE, col);
			}
		}	
	}
	return nuTRUE;
}

//add by xiaoqin @2011.07.29 轨迹显示
nuBOOL CDrawMgrZ::DrawTrajectory(nuPVOID lpVoid)
{
	POTHERGLOBAL pOtherG = (POTHERGLOBAL)g_pDMLibFS->FS_GetData(DATA_OTHERGLOBAL);
	if( pOtherG == NULL || pOtherG->pMemoPtData == NULL )
	{
		return nuFALSE;
	}
	//
	nuroPOINT MemoPt;
	nuMemset(&MemoPt, 0, sizeof(nuroPOINT));
	nuroRECT rtScreen = {0};
	rtScreen.left	= g_pDMLibFS->pGdata->transfData.nBmpLTx;
	rtScreen.top	= g_pDMLibFS->pGdata->transfData.nBmpLTy;
	rtScreen.right	= g_pDMLibFS->pGdata->transfData.nBmpLTx + g_pDMLibFS->pGdata->transfData.nBmpWidth;
	rtScreen.bottom = g_pDMLibFS->pGdata->transfData.nBmpLTy + g_pDMLibFS->pGdata->transfData.nBmpHeight;
	//
	if( g_pDMLibFS->pGdata->uiSetData.nScaleValue <= 75 )
	{
		for( nuDWORD j = 0; j < pOtherG->pMemoPtData->nCount; ++j )
		{
			MemoPt.x = pOtherG->pMemoPtData->pMemoPtList[j].x;
			MemoPt.y = pOtherG->pMemoPtData->pMemoPtList[j].y;
			if( !nuPointInRect(&MemoPt, &g_pDMLibFS->pGdata->transfData.nuShowMapSize) )
			{
				continue;
			}
			g_pDMLibMT->MT_MapToBmp(MemoPt.x, MemoPt.y, &MemoPt.x, &MemoPt.y);
			if( !nuPointInRect(&MemoPt, &rtScreen) )
			{
				continue;
			}
			if( g_pDMLibFS->pGdata->uiSetData.b3DMode )
			{
				g_pDMLibMT->MT_Bmp2Dto3D(MemoPt.x, MemoPt.y, MAP_DEFAULT);
			}
			//设置字体颜色
			nuCOLORREF col = nuRGB(100, 100, 100);
			g_pDMLibGDI->GdiSetTextColor(col); 
			g_pDMLibGDI->GdiExtTextOutWEx(MemoPt.x, MemoPt.y - 12, (nuWCHAR*) L".", 1, nuTRUE, col);
		}
	}
	else
	{
		nuWORD nIndex = g_pDMLibFS->pGdata->uiSetData.nScaleValue / 50;
		for( nuDWORD j = 0; j < pOtherG->pMemoPtData->nCount / nIndex; ++j )
		{
			MemoPt.x = pOtherG->pMemoPtData->pMemoPtList[j * nIndex].x;
			MemoPt.y = pOtherG->pMemoPtData->pMemoPtList[j * nIndex].y;
			if( !nuPointInRect(&MemoPt, &g_pDMLibFS->pGdata->transfData.nuShowMapSize) )
			{
				continue;
			}
			g_pDMLibMT->MT_MapToBmp(MemoPt.x, MemoPt.y, &MemoPt.x, &MemoPt.y);
			if( !nuPointInRect(&MemoPt, &rtScreen) )
			{
				continue;
			}
			if( g_pDMLibFS->pGdata->uiSetData.b3DMode )
			{
				g_pDMLibMT->MT_Bmp2Dto3D(MemoPt.x, MemoPt.y, MAP_DEFAULT);
			}
			//设置字体颜色
			nuCOLORREF col = nuRGB(100, 100, 100);
			g_pDMLibGDI->GdiSetTextColor(col); 
			g_pDMLibGDI->GdiExtTextOutWEx(MemoPt.x, MemoPt.y - 12, (nuWCHAR*) L".", 1, nuTRUE, col);
		}
	}
	return nuTRUE;
}

nuBOOL CDrawMgrZ::DrawEEyesPlayed(nuPVOID lpVoid)
{
	PCCD2_PLAYED  pEEyes = (PCCD2_PLAYED)g_pDMLibFS->FS_GetData(DATA_EEYESPLAYED);
	if( pEEyes == NULL || !m_bmpEEye.bmpHasLoad )
	{
		return nuFALSE;
	}
	nuroPOINT point = {0};
	nuroRECT rtScreen = {0};
	rtScreen.left	= 0;
	rtScreen.top	= 0;
	rtScreen.right	= rtScreen.left + g_pDMGdata->transfData.nBmpWidth;
	rtScreen.bottom = rtScreen.top + g_pDMGdata->transfData.nBmpHeight;
	for( nuINT i = 0; i < _CCD2_MAX_CHECK_COUNT; ++i )
	{
		if( !pEEyes[i].bUsed )
		{
			continue;
		}
		g_pDMLibMT->MT_MapToBmp(pEEyes[i].point.x, pEEyes[i].point.y, &point.x, &point.y);
		if( !nuPtInRect(point, rtScreen) )
		{
			continue;
		}
		if( g_pDMGdata->uiSetData.b3DMode )
		{
			g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
		}
		g_pDMLibGDI->GdiDrawBMP(point.x - m_bmpEEye.bmpWidth/2,
								point.y - m_bmpEEye.bmpHeight, 
								m_bmpEEye.bmpWidth,
								m_bmpEEye.bmpHeight,
								&m_bmpEEye,
								0,
								0 );
	}
	return nuTRUE;
}

nuBOOL CDrawMgrZ::ColMapRoad(nuPVOID lpVoid)
{
	//Get Blocks data from FileSystem module
	m_pScreenMap = (PSCREENMAP)g_pDMLibFS->FS_GetScreenMap();
	if( NULL == m_pScreenMap )
	{
		return nuFALSE;
	}
	//Get the memory used to store the data of drawings
	PDRAWMEMORY pMemData = (PDRAWMEMORY)g_pDMLibFS->FS_GetData(DATA_DRAWMEM);
	if( NULL == pMemData )
	{
		return nuFALSE;
	}
	PSCREENOBJ pShapeObj = (PSCREENOBJ)g_pDMLibMM->MM_GetDataMassAddr(pMemData->nMapSHPMemIdx);
	if( NULL == pShapeObj )
	{
		return nuFALSE;
	}
	//allocate a memory to store the RoadIdx of Blocks
	nuINT nTotalBlocks = 0;
	nuINT nBlkIdx;
	for(nBlkIdx = 0; nBlkIdx < m_pScreenMap->nDwCount; ++nBlkIdx)
	{
		nTotalBlocks += m_pScreenMap->pDwList[nBlkIdx].nNumOfBlocks;
	}
	if( 0 == nTotalBlocks )
	{
		return nuFALSE;
	}
	nuINT* pClsIdx = new nuINT[nTotalBlocks];
	if( NULL == pClsIdx )
	{
		return nuFALSE;
	}
	nuMemset(pClsIdx, 0, sizeof(nuINT)*nTotalBlocks);
	nuroRECT rtBlock = {0}, rect = {0};
	nuroPOINT ptFirst = {0};
	nuBOOL	bHaveNew = nuFALSE;
	nuWORD nCount = 0;
	nuWORD nVertexIdx = pShapeObj->head.nTotalVertex;
	for(nuINT nCls = 0; nCls < g_pDMLibFS->pMapCfg->commonSetting.nRoadTypeCount; ++nCls)
	{
		nBlkIdx = 0;
		nCount	= 0;
		for(nuINT i = 0; i < m_pScreenMap->nDwCount; ++i)
		{
			if( MEM_SIZE_MAP_POINTS == nVertexIdx ||
				MEM_SIZE_MAP_SHPNUM == pShapeObj->head.nShapCount )
			{
				break;
			}
			//
			if( NULL == m_pScreenMap->pDwList[i].pBlockList )
			{
				continue;
			}
			for(nuINT j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; ++j)
			{
				if( MEM_SIZE_MAP_POINTS == nVertexIdx ||
					MEM_SIZE_MAP_SHPNUM == pShapeObj->head.nShapCount )
				{
					break;
				}
				PDWROADBLK pRoadBlk = &m_pScreenMap->pDwList[i].pBlockList[j].roadBlk;
				nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
				for( nuINT k = pClsIdx[nBlkIdx]; k < (nuINT)pRoadBlk->pDwRoadHead->nRoadCount; ++k )
				{
					if( MEM_SIZE_MAP_POINTS == nVertexIdx ||
						MEM_SIZE_MAP_SHPNUM == pShapeObj->head.nShapCount )
					{
						break;
					}
					pClsIdx[nBlkIdx] = k;
					if( pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType > nCls )
					{
						break;
					}
					if( !g_pDMLibFS->pMapCfg->scaleSetting.pRoadSetting[nCls].nShowCenterLine &&
						!g_pDMLibFS->pMapCfg->scaleSetting.pRoadSetting[nCls].nShowRimLine )
					{
						continue;
					}
					rect.left	= pRoadBlk->pRoadRect[k].left;
					rect.top	= pRoadBlk->pRoadRect[k].top;
					rect.right	= pRoadBlk->pRoadRect[k].right;
					rect.bottom = pRoadBlk->pRoadRect[k].bottom;
					rect.left	+= rtBlock.left;
					rect.right	+= rtBlock.left;
					rect.top	+= rtBlock.top;
					rect.bottom	+= rtBlock.top;
					if( !nuRectCoverRect(&rect, &g_pDMGdata->transfData.nuShowMapSize) )
					{
						continue;
					}
					ptFirst.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].x + rtBlock.left;
					ptFirst.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].y + rtBlock.top;
					g_pDMLibMT->MT_MapToBmp( ptFirst.x, 
											 ptFirst.y, 
											 &pShapeObj->ptVertex[ nVertexIdx ].x, 
											 &pShapeObj->ptVertex[ nVertexIdx ].y);
					if( 0 == nCount )
					{
						++nVertexIdx;
						++nCount;
					}
					else if( 1 == nCount )
					{
						--nVertexIdx;
					}
					else
					{
						nuINT dx = pShapeObj->ptVertex[ nVertexIdx ].x - 
							pShapeObj->ptVertex[ nVertexIdx - 1 ].x;
						nuINT dy = pShapeObj->ptVertex[ nVertexIdx ].y - 
							pShapeObj->ptVertex[ nVertexIdx - 1 ].y;
						dx = NURO_ABS(dx);
						dy = NURO_ABS(dy);
						if( dx > 2 || dy > 2 )
						{
							pShapeObj->pShape[ pShapeObj->head.nShapCount ].objType		= SHPOBJ_TYPE_ROAD;
							pShapeObj->pShape[ pShapeObj->head.nShapCount ].nType		= (nuBYTE)nCls;
							pShapeObj->pShape[ pShapeObj->head.nShapCount ].nPointCount	= (nuWORD)nCount;
							pShapeObj->head.nShapCount		+= 1;
							pShapeObj->head.nTotalVertex	= nVertexIdx;
							pShapeObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount	+= 1;
							pShapeObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex	+= nCount;
							++nVertexIdx;
							nCount = 1;
							if( MEM_SIZE_MAP_SHPNUM == pShapeObj->head.nShapCount )
							{
								break;
							}
						}
					}
					//...
					for( nuINT l = 1; l < pRoadBlk->pDwRoadData[k].nVertexCount; ++l)
					{
						if( MEM_SIZE_MAP_POINTS == nVertexIdx )
						{
							break;
						}
						ptFirst.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].x + rtBlock.left;
						ptFirst.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].y + rtBlock.top;
						g_pDMLibMT->MT_MapToBmp( ptFirst.x, 
												 ptFirst.y, 
												 &pShapeObj->ptVertex[ nVertexIdx ].x, 
												 &pShapeObj->ptVertex[ nVertexIdx ].y );
						if( l != pRoadBlk->pDwRoadData[k].nVertexCount - 1 )
						{
							nuINT dx = pShapeObj->ptVertex[ nVertexIdx ].x - 
								pShapeObj->ptVertex[ nVertexIdx - 1 ].x;
							nuINT dy = pShapeObj->ptVertex[ nVertexIdx ].y - 
								pShapeObj->ptVertex[ nVertexIdx - 1 ].y;
							dx = NURO_ABS(dx);
							dy = NURO_ABS(dy);
							if( dx > 1 || dy > 1 )
							{
								++nVertexIdx;
								++nCount;
							}
						}
						else
						{
							++nVertexIdx;
							++nCount;
						}
					}
				}
			}
			++nBlkIdx;
		}
		if( nCount > 1 )
		{
			pShapeObj->pShape[ pShapeObj->head.nShapCount ].objType		= SHPOBJ_TYPE_ROAD;
			pShapeObj->pShape[ pShapeObj->head.nShapCount ].nType		= (nuBYTE)nCls;
			pShapeObj->pShape[ pShapeObj->head.nShapCount ].nPointCount	= (nuWORD)nCount;
			pShapeObj->pShpCount[SHPOBJ_TYPE_ROAD].nShapCount	+= 1;
			pShapeObj->pShpCount[SHPOBJ_TYPE_ROAD].nTotalVertex += nCount;
			pShapeObj->head.nTotalVertex = nVertexIdx;
			++pShapeObj->head.nShapCount;
		}
		else
		{
			nVertexIdx -= nCount;
		}
	}
	//
	delete[] pClsIdx;
	pClsIdx=NULL;
	return nuTRUE;
}
