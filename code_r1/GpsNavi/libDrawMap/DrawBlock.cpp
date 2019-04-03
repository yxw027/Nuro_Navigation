// DrawBlock.cpp: implementation of the CDrawBlock class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawBlock.h"
#include "RoadName.h"
#include "PoiName.h"
#include "DrawSeaZ.h"
#include "libDrawMap.h"

#include "DrawKilometerDL.h"
#include <stdio.h>
#include <math.h>

extern class CRoadName*			g_pRoadName;
extern class CPoiName*			g_pPoiName;
//extern class CPoiName*			g_pPoiZoom;
extern class CDrawKilometerDL*  g_pDrawKilometerDL;

#define CONJ_ERROR							0x00
#define CONJ_FIRSTLINE						0x01
#define CONJ_FROM_FROM						0x02
#define CONJ_FROM_TO						0x03
#define CONJ_TO_FROM						0x04
#define CONJ_TO_TO							0x05

#define DIS_CONJIONT						3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawBlock::CDrawBlock()
{
	m_Dis = 10;
	m_absError = 6.0e-04;
}

CDrawBlock::~CDrawBlock()
{
	Free();
}

nuBOOL CDrawBlock::Initialize()
{
	if( !m_roadCol.Initialize( g_pDMMapCfg->commonSetting.nRoadTypeCount ) )
	{
		return nuFALSE;
	}
	if( !m_bgLineCol.Initialize( g_pDMMapCfg->commonSetting.nBgLineTypeCount, BGLINE_COL_MEM_SIZE ) )
	{
		return nuFALSE;
	}
	if( !m_bgAreaCol.Initialize( g_pDMMapCfg->commonSetting.nBgAreaTypeCount ) )
	{
		return nuFALSE;
	}
	/*&20081209&
	if( !m_zoomRoadCol.Initialize( g_pDMMapCfg->commonSetting.nRoadTypeCount, ZOOM_ROAD_COL_MEM_SIZE ) )
	{
		return nuFALSE;
	}
	*///&20081209&
	return nuTRUE;
}

nuVOID CDrawBlock::Free()
{
//&20081209&	m_zoomRoadCol.Free();
	m_bgAreaCol.Free();
	m_bgLineCol.Free();
	m_roadCol.Free();
	m_pScreenMap	= NULL;
}

nuBOOL CDrawBlock::LoadScreenMap()
{
	m_pScreenMap = (PSCREENMAP)g_pDMLibFS->FS_GetScreenMap();
	return nuTRUE;
}

nuVOID CDrawBlock::FreeScreenMap()
{
	m_bgLineCol.DelLineList();
	m_roadCol.DelLineList();
	m_bgAreaCol.DelAreaSeg();
//&20081209&	m_zoomRoadCol.DelLineList();
}

nuBOOL CDrawBlock::CollectScreenMap()
{
	FreeScreenMap();
	LoadScreenMap();
	if( m_pScreenMap == NULL )
	{
		return nuFALSE;
	}
	NURORECT  rtBlock = {0};
	NURORECT  rect    = {0};
	NUROPOINT ptFrom  = {0};
	NUROPOINT ptTo	  = {0};
	PDWROADBLK pDwRoadBlk = NULL;
	PDWBGLINEBLK	pDwBgLineBlk = NULL;
	PDWBGAREABLK	pDwBgAreaBlk = NULL;
	nuLONG nIdx = 0;
	
	for(nuWORD i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for(nuWORD j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++)
		{
			nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
			//Collect screen Roads
			pDwRoadBlk = &m_pScreenMap->pDwList[i].pBlockList[j].roadBlk;
			//g_pDrawKilometerDL->AddKilometers(m_pScreenMap->pDwList[i].pBlockList[j].nBlockIdx);
//			nuShowString(nuTEXT("CollectScreenMap 1"));
			if( pDwRoadBlk->pDwRoadHead != NULL )
			{
				for(nuDWORD k = 0; k < pDwRoadBlk->pDwRoadHead->nRoadCount; k++)
				{
					/*
					if( pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType == 11 )
					{
						nuINT nTest = 11;
					}
					*/
					if( pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType >=
						g_pDMMapCfg->commonSetting.nRoadTypeCount )//Judge the RoadType
					{
						continue;
					}
					rect.left	= pDwRoadBlk->pRoadRect[k].left;
					rect.top	= pDwRoadBlk->pRoadRect[k].top;
					rect.right	= pDwRoadBlk->pRoadRect[k].right;
					rect.bottom = pDwRoadBlk->pRoadRect[k].bottom;
					rect.left	+= rtBlock.left;
					rect.right	+= rtBlock.left;
					rect.top	+= rtBlock.top;
					rect.bottom	+= rtBlock.top;
					if( g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowCenterLine || 
						g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowRimLine	 ||
						g_pDMMapCfg->zoomSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowCenterLine	 || 
						g_pDMMapCfg->zoomSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowRimLine )
					{
						ptFrom.x = ptTo.x = rtBlock.left;
						ptFrom.y = ptTo.y = rtBlock.top;
						ptFrom.x += pDwRoadBlk->pSPointCoor[pDwRoadBlk->pDwRoadData[k].nVertexAddrIdx].x;
						ptFrom.y += pDwRoadBlk->pSPointCoor[pDwRoadBlk->pDwRoadData[k].nVertexAddrIdx].y;
						nIdx = pDwRoadBlk->pDwRoadData[k].nVertexAddrIdx + pDwRoadBlk->pDwRoadData[k].nVertexCount - 1;
						ptTo.x += pDwRoadBlk->pSPointCoor[nIdx].x;
						ptTo.y += pDwRoadBlk->pSPointCoor[nIdx].y;
						if( ( g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowCenterLine || 
							  g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowRimLine	 ) &&
							nuRectCoverRect(&rect, &g_pDMGdata->transfData.nuShowMapSize) )
						{
							m_roadCol.AddLineSeg(i, j, k, ptFrom, ptTo, pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType);
						}
						/*&20081209&
						if( ( g_pDMMapCfg->zoomSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowCenterLine	 || 
							  g_pDMMapCfg->zoomSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowRimLine ) && 
							  nuRectCoverRect(&rect, &g_pDMGdata->zoomScreenData.nuZoomMapSize) )
						{
							m_zoomRoadCol.AddLineSeg(i, j, k, ptFrom, ptTo, pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType);
						}
						*///&20081209&
					}
				}
			}
//			nuShowString(nuTEXT("CollectScreenMap 2"));
			pDwBgLineBlk = &m_pScreenMap->pDwList[i].pBlockList[j].bgLineBlk;
			if( pDwBgLineBlk->pDwBgLineHead != NULL )
			{
				for(nuDWORD k = 0; k < pDwBgLineBlk->pDwBgLineHead->nBgLineCount; k++)
				{/*
					if( pDwBgLineBlk->pDwBgLineData[k].nBgLineType != 2 )
					{
						continue;//test
					}*/
					if( pDwBgLineBlk->pDwBgLineData[k].nBgLineType >= g_pDMMapCfg->commonSetting.nBgLineTypeCount ||
						( !g_pDMMapCfg->scaleSetting.pBgLineSetting[pDwBgLineBlk->pDwBgLineData[k].nBgLineType].nShowCenterLine && 
						  !g_pDMMapCfg->scaleSetting.pBgLineSetting[pDwBgLineBlk->pDwBgLineData[k].nBgLineType].nShowRimLine) )
					{
						continue;
					}
					rect.left	= pDwBgLineBlk->pDwBgLineData[k].sRectBound.left;
					rect.top	= pDwBgLineBlk->pDwBgLineData[k].sRectBound.top;
					rect.right	= pDwBgLineBlk->pDwBgLineData[k].sRectBound.right;
					rect.bottom	= pDwBgLineBlk->pDwBgLineData[k].sRectBound.bottom;
					rect.left	+= rtBlock.left;
					rect.right	+= rtBlock.left;
					rect.top	+= rtBlock.top;
					rect.bottom	+= rtBlock.top;
					if( nuRectCoverRect(&rect, &g_pDMGdata->transfData.nuShowMapSize) )
					{
						ptFrom.x = ptTo.x = rtBlock.left;
						ptFrom.y = ptTo.y = rtBlock.top;
						ptFrom.x += pDwBgLineBlk->pSPointCoor[ pDwBgLineBlk->pDwBgLineData[k].nVertexIdx ].x;
						ptFrom.y += pDwBgLineBlk->pSPointCoor[ pDwBgLineBlk->pDwBgLineData[k].nVertexIdx ].y;
						nIdx = pDwBgLineBlk->pDwBgLineData[k].nVertexIdx + pDwBgLineBlk->pDwBgLineData[k].nVertexCount - 1;
						ptTo.x += pDwBgLineBlk->pSPointCoor[ nIdx ].x;
						ptTo.y += pDwBgLineBlk->pSPointCoor[ nIdx ].y;
						m_bgLineCol.AddLineSeg(i, j, k, ptFrom, ptTo, pDwBgLineBlk->pDwBgLineData[k].nBgLineType);
					}
				}
			}
//			nuShowString(nuTEXT("CollectScreenMap 3"));
			pDwBgAreaBlk = &m_pScreenMap->pDwList[i].pBlockList[j].bgAreaBlk;
			if( pDwBgAreaBlk->pDwBgAreaHead != NULL )
			{
				for( nuDWORD k = 0; k < pDwBgAreaBlk->pDwBgAreaHead->nBgAreaCount; k++ )
				{
					/* 20081127*/
					if( pDwBgAreaBlk->pDwBgAreaData[i].nBgAreaType == SEA_AREA_TYPE )
					{
						//continue;
					}
					
					if( pDwBgAreaBlk->pDwBgAreaData[k].nBgAreaType >= m_bgAreaCol.m_nAreaType ||
						!g_pDMMapCfg->scaleSetting.pBgAreaSetting[ pDwBgAreaBlk->pDwBgAreaData[k].nBgAreaType ].nShowArea )
					{
						continue;
					}
					rect.left	= pDwBgAreaBlk->pDwBgAreaData[k].sRectBount.left;
					rect.top	= pDwBgAreaBlk->pDwBgAreaData[k].sRectBount.top;
					rect.right	= pDwBgAreaBlk->pDwBgAreaData[k].sRectBount.right;
					rect.bottom	= pDwBgAreaBlk->pDwBgAreaData[k].sRectBount.bottom;
					rect.left	+= rtBlock.left;
					rect.right	+= rtBlock.left;
					rect.top	+= rtBlock.top;
					rect.bottom	+= rtBlock.top;
					if( nuRectCoverRect(&rect, &g_pDMGdata->transfData.nuShowMapSize) )
					{
						m_bgAreaCol.AddAreaSeg(i, j, k, pDwBgAreaBlk->pDwBgAreaData[k].nBgAreaType);
					}
				}
			}
//			nuShowString(nuTEXT("CollectScreenMap 4"));
		}
	}
//	nuShowString(nuTEXT("CollectScreenMap 5"));
	return nuTRUE;
}
nuBOOL CDrawBlock::SetDTIColor(nuUINT trafficevent,nuVOID* pRSet/* = NULL*/)
{
	if (pRSet == NULL)
	{
		return nuFALSE;
	}
	PROADSETTING pRS = (PROADSETTING)pRSet;
	if (trafficevent == 0)
	{
        //test log:
		pRS->nCenterLineColorR = 180;
		pRS->nCenterLineColorG	=180;
		pRS->nCenterLineColorB = 180;
		return nuTRUE;
	}
	else if (trafficevent == 1)
	{
		//green;

		pRS->nCenterLineColorR = 16;//107;
		pRS->nCenterLineColorG = 170;//207;
		pRS->nCenterLineColorB = 8;//107;
	}
	else if (trafficevent == 2)
	{

		//yello;
		pRS->nCenterLineColorR = 239;//250;
		pRS->nCenterLineColorG =203;//180;
		pRS->nCenterLineColorB = 16;//10;
	}
	else if (trafficevent == 3)
	{

		//red;
		pRS->nCenterLineColorR = 206;//255;
		pRS->nCenterLineColorG =16;//0;
		pRS->nCenterLineColorB = 16;//0;
	}
	return nuTRUE;
}
//draw DTI road;edite by chang;
nuBOOL CDrawBlock::DrawDTIRoad(nuBOOL bRimLine/* = nuTRUE*/, nuBOOL bDrawRdName/* = nuTRUE*/)
{
	nuWORD nClassCount = m_roadCol.m_nClassCount, nRoadPtCount;

	nuINT nClassIdx, nClassStep, nRoadPtIdx, nRoadPtStep;
	      nClassIdx= nClassStep= nRoadPtIdx= nRoadPtStep=0;

	NUROPOINT ptTmp1 = {0};
	NUROPOINT ptTmp2 = {0};
	NUROPOINT ptTmp11= {0};
	NUROPOINT ptTmp22= {0};
	NURORECT  rtBlock = {0};
	
	nuLONG     nSkipLen = 0, /*dx, dy,*/ nLineWidth = 0, nSampleBit;
	PCONJLINE  pConjLine = NULL;
	PLINESEG   pLineSeg = NULL;
	PDWROADBLK pDwRoadBlk = NULL;
	nuINT      nObjType=0;
	nuBYTE	   nDrawType=0;
	//for DIT test;
	NUROPOINT  ptDTIList[POINTLISTCOUNT]= {0};
	nuSHORT    nDTINowCount = 0;

	ROADSETTING DTIRaodSetting = {0};

	//add by chang for onway;
	MYPOINT  ptDTIList1[SPCOORBUFSIZE]= {0};
	MYPOINT  ptDTIList2[SPCOORBUFSIZE]= {0};
	nuINT nOneWayType = -1;


	//------------For col Road name-------------------
	//------------------------------------------------
	if( bRimLine )
	{
		//draw rim;
		nClassIdx = 0;
		nClassStep = 1;
		g_pRoadName->ResetRdNameList();
	}
	else
	{
		//draw center;
		nClassIdx = m_roadCol.m_nClassCount - 1;
		nClassStep = -1;
	}

	while( nClassCount > 0 )
	{
		//there is not road at this class;
		if( m_roadCol.m_pLineList[nClassIdx].pConjHead == NULL )
		{
			nClassCount --;
			nClassIdx += nClassStep;
			continue;
		}

		//rim or center;
		if( bRimLine )
		{
			nDrawType = DRAW_TYPE_ROADRIM;
		}
		else
		{
			nDrawType = DRAW_TYPE_ROADCENTER;
		}

		//same road class;same color;
		nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
											    g_pDMGdata->uiSetData.b3DMode, 
											    &g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx],
											    &nLineWidth );

		//for DTI test;
		DTIRaodSetting = g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx];
		///////////////////////
		if( nObjType == DRAW_OBJ_SKIP || 
		   (nObjType == DRWA_OBJ_ONLYCOLNAME && !bDrawRdName) )
		{
			nClassCount --;
			nClassIdx += nClassStep;
			continue;
		}

		//draw name or not;in this func,bAddName is always nuFALSE;
		pConjLine = m_roadCol.m_pLineList[nClassIdx].pConjHead;

		//why?
		nSkipLen = 2 *g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;
		nSampleBit = 0;
		if( nSkipLen != 0 )
		{
			while( nSkipLen )
			{
				nSkipLen = nSkipLen >> 1;
				nSampleBit ++;
			}
		}
		while( pConjLine != NULL )
		{
			/*
			ptTmp1 = pConjLine->ptFrom;
			ptTmp11.x = ptTmp1.x >> nSampleBit;
			ptTmp11.y = ptTmp1.y >> nSampleBit;
			g_pDMLibMT->MT_MapToBmp(ptTmp1.x, ptTmp1.y, &ptDTIList[0].x, &ptDTIList[0].y);
			nDTINowCount = 1;*/
			nDTINowCount = 0;

			//for DTI;
			////////////////////////

			pLineSeg = pConjLine->pLineHead;

			while( pLineSeg != NULL )
			{
				//////////////////////////
				pDwRoadBlk = &m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx ].roadBlk;
				nRoadPtCount = pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nVertexCount;
				nOneWayType = pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nOneWay;

				//for DTI;
				nuUINT nMapIdx = m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].nDwIdx;
				nuUINT blockindex = m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx ].nBlockIdx;

				nuUINT trafficevent1 = g_pDMLibFS->FS_GetTMCTrafficInfo(nMapIdx,blockindex,pLineSeg->nScreenRoadIdx);

				TMCRoadInfoStru* pdirdata = g_pDMLibFS->FS_GetTMCDataInfo(nMapIdx,blockindex,pLineSeg->nScreenRoadIdx);

				nuUINT trafficevent = 0;//trafficevent3; 

				nuUINT dir1 = 0;
				//nuUINT trafficevent1 = 0;
								
				nuUINT dir2 = 0;
				nuUINT trafficevent2 = 0;
				if (pdirdata != NULL)
				{
					if (pdirdata[0].Dir == 0  ||
						pdirdata[1].Dir == 0)
					{
						if (pdirdata[0].Dir != 0)
						{
							trafficevent1 = pdirdata[0].Traffic;
						}

						if (pdirdata[1].Dir != 0)
						{
							trafficevent1 = pdirdata[1].Traffic;
						}

						trafficevent = trafficevent1;
					}
					else
					{
						if (pdirdata[0].Dir ==1)
						{
							dir1 = pdirdata[0].Dir;
							trafficevent1 = pdirdata[0].Traffic;

							dir2 = pdirdata[1].Dir;
							trafficevent2 = pdirdata[1].Traffic;
						}
						else
						{
							dir1 = pdirdata[1].Dir;
							trafficevent1 = pdirdata[1].Traffic;

							dir2 = pdirdata[0].Dir;
							trafficevent2 = pdirdata[0].Traffic;
						}

					}
				}
				else{
					/*
					nuWCHAR temp[256] = {0};
					wsprintf(temp,L"\n %d---%d----%d",nMapIdx,
						blockindex,
						pLineSeg->nScreenRoadIdx);
					OutputDebugStr(temp);*/
				}

				//undo: reset trafficevent according to dir;

				if(trafficevent1 != 5)//0)
				{
					if( pLineSeg->nConjWay == CONJ_FROM_TO )
					{
						nRoadPtIdx = 0;
						nRoadPtStep = 1;
					}
					else
					{
						nRoadPtIdx = nRoadPtCount - 1;
						nRoadPtStep = -1;
					}

					nuBlockIDtoRect(m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx].nBlockID, &rtBlock);

					//juedge oneway before draw;//it is one line two direction;
					if ( 0 == nOneWayType && dir1 != 0 && dir2 != 0 )
					{
						/*
						SetDTIColor(0,&DTIRaodSetting);  //???????????
						//selectobject;
						nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
							g_pDMGdata->uiSetData.b3DMode, 
							&DTIRaodSetting,
							&nLineWidth );
						ptTmp1.x = 0;
						ptTmp1.y = 0;
						ptTmp11.x = 0;
						ptTmp11.y = 0;
						while( nRoadPtCount > 0 )
						{
							ptTmp2.x = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].x + rtBlock.left;
							ptTmp2.y = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].y + rtBlock.top;
							ptTmp22.x = ptTmp2.x >> nSampleBit;
							ptTmp22.y = ptTmp2.y >> nSampleBit;
							if( ptTmp22.x != ptTmp11.x || ptTmp22.y != ptTmp11.y )
							{
								g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptDTIList[nDTINowCount].x, &ptDTIList[nDTINowCount].y);
								//for DTI;
								///////////////////////////////////////////////
								nDTINowCount++;


								ptTmp1 = ptTmp2;
								ptTmp11 = ptTmp22;
								if( nDTINowCount == POINTLISTCOUNT )
								{
									//Draw line...
									g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);
									ptDTIList[0] = ptDTIList[nDTINowCount-1];
									nDTINowCount = 1;
									//--------------------------------------------------------------------
								}
							}
							nRoadPtCount --;
							nRoadPtIdx += nRoadPtStep;
						}

						if( nDTINowCount > 1 )
						{

							//Draw line...
							g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);

							ptDTIList[0] = ptDTIList[nDTINowCount-1];
							nDTINowCount = 1;
						}	*/					

						//get point data;
						NUROPOINT exttemp[] = {0}; 
						nuMemset(ptDTIList1,0,sizeof(ptDTIList1));
						nuMemset(ptDTIList2,0,sizeof(ptDTIList2));
						for( nuINT i = 0;i < nRoadPtCount;i++ )
						{
							m_SPointCoorBuf[i].x  = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + i ].x + rtBlock.left;
							m_SPointCoorBuf[i].y = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + i ].y + rtBlock.top;
						}

						//short to double;

						// 						for(int i = 0;i < nRoadPtCount;i++)
						// 						{
						// 							m_PointArray[i].x = m_SPointCoorBuf[i].x * 1.0;
						// 							m_PointArray[i].y = m_SPointCoorBuf[i].y * 1.0;
						// 						}

						//????????
						int tempRoadPtcount = 0;
						for(int i = 0;i < nRoadPtCount;i++)
						{
							if (i == 0)
							{
								m_PointArray[tempRoadPtcount].x = m_SPointCoorBuf[i].x * 1.0;
								m_PointArray[tempRoadPtcount].y = m_SPointCoorBuf[i].y * 1.0;
								tempRoadPtcount++;
							}
							else
							{
								if (m_SPointCoorBuf[i-1].x == m_SPointCoorBuf[i].x &&
									m_SPointCoorBuf[i-1].y == m_SPointCoorBuf[i].y   )
								{
									continue;
								}
								m_PointArray[tempRoadPtcount].x = m_SPointCoorBuf[i].x * 1.0;
								m_PointArray[tempRoadPtcount].y = m_SPointCoorBuf[i].y * 1.0;
								tempRoadPtcount++;

							}
						}

						nRoadPtCount = tempRoadPtcount;

						if (nRoadPtCount == 2)
						{
							GetPointOfVerticalLine(m_PointArray[0],m_PointArray[1],&ptDTIList1[0],&ptDTIList2[0]);
							GetPointOfVerticalLine(m_PointArray[1],m_PointArray[0],&ptDTIList2[1],&ptDTIList1[1]);

						}
						else
						{
							for (int i = 0;i < nRoadPtCount-2;i++)
							{
								//first line;
								if (i == 0)
								{
									GetPointOfVerticalLine(m_PointArray[i],m_PointArray[i+1],&ptDTIList1[i],&ptDTIList2[i]);
								}

								//middle line;
								GetPointOfSameSide(m_PointArray[i],m_PointArray[i+1],m_PointArray[i+2],&ptDTIList1[i+1],&ptDTIList2[i+1]);

								//debug code;
								//last line;
								if ((i+2) == (nRoadPtCount - 1))
								{
									GetPointOfVerticalLine(m_PointArray[i+2],m_PointArray[i+1],&ptDTIList2[i+2],&ptDTIList1[i+2]);
								}

							}
						}


						//deal with ptDTIList1;

						SetDTIColor(trafficevent2,&DTIRaodSetting);
						//selectobject;
						nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
							g_pDMGdata->uiSetData.b3DMode, 
							&DTIRaodSetting,
							&nLineWidth );

						for(int i = 0;i < nRoadPtCount;i++)
						{
							m_SPointCoorBuf[i].x = ptDTIList1[i].x ;
							m_SPointCoorBuf[i].y = ptDTIList1[i].y ;

							//debug code;
						}

						if( pLineSeg->nConjWay == CONJ_FROM_TO )
						{
							nRoadPtIdx = 0;
							nRoadPtStep = 1;
						}
						else
						{
							nRoadPtIdx = nRoadPtCount - 1;
							nRoadPtStep = -1;
						}
						nDTINowCount = 0;
						nuWORD nCounttemp1 = nRoadPtCount;
						ptTmp1.x = 0;
						ptTmp1.y = 0;
						ptTmp11.x = 0;
						ptTmp11.y = 0;
						while( nCounttemp1 > 0 )
						{
							ptTmp2.x = m_SPointCoorBuf[nRoadPtIdx].x;
							ptTmp2.y = m_SPointCoorBuf[nRoadPtIdx].y;
							ptTmp22.x = ptTmp2.x >> nSampleBit;
							ptTmp22.y = ptTmp2.y >> nSampleBit;
							if( ptTmp22.x != ptTmp11.x || ptTmp22.y != ptTmp11.y )
							{
								g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptDTIList[nDTINowCount].x, &ptDTIList[nDTINowCount].y);
								//for DTI;
								///////////////////////////////////////////////
								nDTINowCount++;

								ptTmp1 = ptTmp2;
								ptTmp11 = ptTmp22;

								if( nDTINowCount == POINTLISTCOUNT )
								{
									//Draw line...
									g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);
									nDTINowCount = 0;
									//--------------------------------------------------------------------
								}
							}
							nCounttemp1 --;
							nRoadPtIdx += nRoadPtStep;
						}
						if( nDTINowCount > 1 )
						{
							//Draw line...
							g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);

							nDTINowCount = 0;
						}


						//deal with ptDTIList2;
						SetDTIColor(trafficevent1,&DTIRaodSetting);
						//selectobject;
						nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
							g_pDMGdata->uiSetData.b3DMode, 
							&DTIRaodSetting,
							&nLineWidth );

						for(int i = 0;i < nRoadPtCount;i++)
						{
							m_SPointCoorBuf[i].x = ptDTIList2[i].x ;
							m_SPointCoorBuf[i].y = ptDTIList2[i].y ;
						}

						if( pLineSeg->nConjWay == CONJ_FROM_TO )
						{
							nRoadPtIdx = 0;
							nRoadPtStep = 1;
						}
						else
						{
							nRoadPtIdx = nRoadPtCount - 1;
							nRoadPtStep = -1;
						}
						nDTINowCount = 0;
						nuWORD nCounttemp2 = nRoadPtCount;
						ptTmp1.x = 0;
						ptTmp1.y = 0;
						ptTmp11.x = 0;
						ptTmp11.y = 0;
						while( nCounttemp2 > 0 )
						{
							ptTmp2.x = m_SPointCoorBuf[nRoadPtIdx].x;
							ptTmp2.y = m_SPointCoorBuf[nRoadPtIdx].y;
							ptTmp22.x = ptTmp2.x >> nSampleBit;
							ptTmp22.y = ptTmp2.y >> nSampleBit;
							if( ptTmp22.x != ptTmp11.x || ptTmp22.y != ptTmp11.y )
							{
								g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptDTIList[nDTINowCount].x, &ptDTIList[nDTINowCount].y);
								//for DTI;
								///////////////////////////////////////////////
								nDTINowCount++;

								ptTmp1 = ptTmp2;
								ptTmp11 = ptTmp22;

								if( nDTINowCount == POINTLISTCOUNT )
								{
									//Draw line...
									g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);
									nDTINowCount = 0;
									//--------------------------------------------------------------------
								}
							}
							nCounttemp2 --;
							nRoadPtIdx += nRoadPtStep;
						}
						if( nDTINowCount > 1 )
						{
							//Draw line...

							g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);

							nDTINowCount = 0;
						}

					}
					else
					{
						SetDTIColor(trafficevent,&DTIRaodSetting);
						//selectobject;
						nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
							g_pDMGdata->uiSetData.b3DMode, 
							&DTIRaodSetting,
							&nLineWidth );
						ptTmp1.x = 0;
						ptTmp1.y = 0;
						ptTmp11.x = 0;
						ptTmp11.y = 0;
						while( nRoadPtCount > 0 )
						{
							ptTmp2.x = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].x + rtBlock.left;
							ptTmp2.y = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].y + rtBlock.top;
							ptTmp22.x = ptTmp2.x >> nSampleBit;
							ptTmp22.y = ptTmp2.y >> nSampleBit;
							if( ptTmp22.x != ptTmp11.x || ptTmp22.y != ptTmp11.y )
							{
									g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptDTIList[nDTINowCount].x, &ptDTIList[nDTINowCount].y);
									//for DTI;
									///////////////////////////////////////////////
									nDTINowCount++;


								ptTmp1 = ptTmp2;
								ptTmp11 = ptTmp22;
								if( nDTINowCount == POINTLISTCOUNT )
								{
									//Draw line...
									g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);
									ptDTIList[0] = ptDTIList[nDTINowCount-1];
									nDTINowCount = 1;
									//--------------------------------------------------------------------
								}
							}
							nRoadPtCount --;
							nRoadPtIdx += nRoadPtStep;
						}

						if( nDTINowCount > 1 )
						{

							//Draw line...
							g_pDMLibMT->MT_DrawObject(nObjType, ptDTIList, nDTINowCount, nLineWidth);

							ptDTIList[0] = ptDTIList[nDTINowCount-1];
							nDTINowCount = 1;
						}						
					}
				}
				else
				{
// 					ptTmp1.x = 0;
// 					ptTmp1.y = 0;
// 					ptTmp11.x = 0;
// 					ptTmp11.y = 0;
					nDTINowCount=0;
				}
				pLineSeg = pLineSeg->pNext;
			}
			pConjLine = pConjLine->pNext;
		}
		nClassCount --;
		nClassIdx += nClassStep;
	}
	return nuTRUE;
}

bool CDrawBlock::GetPointOfVerticalLine(MYPOINT pt1,MYPOINT pt2,MYPOINT *ptres1,MYPOINT *ptres2)
{
	return NewGetPointOfVerticalLine( pt1, pt2, ptres1, ptres2);
	double m_scaleDis=m_Dis;
	if(g_pDMMapCfg->scaleSetting.scaleTitle.nScale!=0)
	{
		m_scaleDis=m_Dis*10.0/g_pDMMapCfg->scaleSetting.scaleTitle.nScale;
	}

	//???????:
	if (pt1.x == pt2.x && pt1.y == pt2.y)
	{
		*ptres1=pt1;
		*ptres2=pt1;
		return false;
	}

	//???????;pt1.x = pt2.x ?? pt1.y = pt2.y;

	if (pt1.x == pt2.x && pt1.y != pt2.y)
	{
		MYPOINT ptv1 = {0},ptv2 = {0};

		ptv1.x = pt1.x + m_scaleDis;
		ptv1.y = pt1.y;

		ptv2.x = pt1.x - m_scaleDis;
		ptv2.y = pt1.y;

		*ptres1 = ptv1;
		*ptres2 = ptv2;
		return true;
	}
	else if (pt1.x != pt2.x && pt1.y == pt2.y)
	{
		MYPOINT ptv1 = {0},ptv2 = {0};

		ptv1.x = pt1.x;
		ptv1.y = pt1.y + m_scaleDis;

		ptv2.x = pt1.x;
		ptv2.y = pt1.y - m_scaleDis;

		*ptres1 = ptv1;
		*ptres2 = ptv2;

		return true;
	}

	//line : y = k*x + b;
	double  k  = 0;
	double  kv = 0,bv = 0;

	k = (pt2.y - pt1.y)/(pt2.x - pt1.x);

	//vertical line: y = kv * x + bv;

	kv = - 1/k;
	bv = pt1.y - kv*pt1.x;

	//??:?????p(m,n),??n = kv * m + bv; d*d = (m-pt1.x)*(m-pt1.x) + (n - pt1.y)*(n - pt1.y);
	MYPOINT ptv1 = {0},ptv2 = {0};
	double a = 0,b = 0,c = 0;
	a = 1 + kv*kv;
	b = 2*((bv - pt1.y)*kv - pt1.x);
	c = pt1.x*pt1.x + (bv - pt1.y)*(bv - pt1.y)-m_scaleDis*m_scaleDis;

	ptv1.x = (-b + sqrt(b*b - 4*a*c))/(2*a);
	ptv2.x = (-b - sqrt(b*b - 4*a*c))/(2*a);

	ptv1.y = kv*ptv1.x + bv;
	ptv2.y = kv*ptv2.x + bv;

	*ptres1 = ptv1;
	*ptres2 = ptv2;

	return 1;
}

bool CDrawBlock::NewGetPointOfVerticalLine(MYPOINT pt1,MYPOINT pt2,MYPOINT *ptres1,MYPOINT *ptres2)
{
	double m_scaleDis=m_Dis;
	if(g_pDMMapCfg->scaleSetting.scaleTitle.nScale!=0)
	{
		//m_scaleDis=m_Dis*30.0/g_pDMMapCfg->scaleSetting.scaleTitle.nScale;
		m_scaleDis=5.0;
	}

	//???????:
	if (pt1.x == pt2.x && pt1.y == pt2.y)
	{
		*ptres1=pt1;						*ptres2=pt1;
		return false;
	}
	else if (pt1.x == pt2.x)
	{
		MYPOINT ptv1 = {0},ptv2 = {0};
		//modify by shan       
		if (pt1.y > pt2.y)
		{
			ptv1.x = pt1.x + m_scaleDis;		ptv1.y = pt1.y;
			ptv2.x = pt1.x - m_scaleDis;		ptv2.y = pt1.y;
		} 
		else
		{
			ptv1.x = pt1.x - m_scaleDis;		ptv1.y = pt1.y;
			ptv2.x = pt1.x + m_scaleDis;		ptv2.y = pt1.y;
		}
		*ptres1 = ptv1;						*ptres2 = ptv2;
		return true;
	}
	else if (pt1.y == pt2.y)
	{
		MYPOINT ptv1 = {0},ptv2 = {0};
		//modify by shan
		if (pt1.x >pt2.x)
		{
			ptv1.x = pt1.x;						ptv1.y = pt1.y - m_scaleDis;
			ptv2.x = pt1.x;						ptv2.y = pt1.y + m_scaleDis;
		} 
		else
		{
			ptv1.x = pt1.x;						ptv1.y = pt1.y + m_scaleDis;
			ptv2.x = pt1.x;						ptv2.y = pt1.y - m_scaleDis;
		}
		*ptres1 = ptv1;						*ptres2 = ptv2;
		return true;
	}

	double  k  = 0;
	double  kv = 0,bv = 0;
	kv=pt2.x-pt1.x;
	bv=pt2.y-pt1.y;

	k=sqrt(kv*kv+bv*bv);
	if(bv<0&&kv>0)
	{
		(*ptres1).x = pt1.x-m_scaleDis*bv/k;		(*ptres1).y = pt1.y + m_scaleDis*kv/k;
		(*ptres2).x = pt1.x+m_scaleDis*bv/k;		(*ptres2).y = pt1.y - m_scaleDis*kv/k;
	}
	//else if (bv>0&&kv>0)
	//{
	(*ptres1).x = pt1.x-m_scaleDis*bv/k;		(*ptres1).y = pt1.y + m_scaleDis*kv/k;
	(*ptres2).x = pt1.x+m_scaleDis*bv/k;		(*ptres2).y = pt1.y - m_scaleDis*kv/k;
	//}
	// 	(*ptres1).x = pt1.x-m_scaleDis*bv/k;		(*ptres1).y = pt1.y + m_scaleDis*kv/k;
	// 	(*ptres2).x = pt1.x+m_scaleDis*bv/k;		(*ptres2).y = pt1.y - m_scaleDis*kv/k;

	return 1;
}


double CDrawBlock::mult(MYPOINT a, MYPOINT b, MYPOINT c)
{
	return (a.x-c.x)*(b.y-c.y)-(b.x-c.x)*(a.y-c.y);
}

//aa, bb???????? cc, dd?????????? ????true, ?????false
bool CDrawBlock::intersect(MYPOINT aa, MYPOINT bb, MYPOINT cc, MYPOINT dd)
{
	if ( NURO_MAX(aa.x, bb.x)<NURO_MIN(cc.x, dd.x) )
	{
		return false;
	}
	if ( NURO_MAX(aa.y, bb.y)<NURO_MIN(cc.y, dd.y) )
	{
		return false;
	}
	if ( NURO_MAX(cc.x, dd.x)<NURO_MIN(aa.x, bb.x) )
	{
		return false;
	}
	if ( NURO_MAX(cc.y, dd.y)<NURO_MIN(aa.y, bb.y) )
	{
		return false;
	}
	if ( mult(cc, bb, aa)*mult(bb, dd, aa)<0 )
	{
		return false;
	}
	if ( mult(aa, dd, cc)*mult(dd, bb, cc)<0 )
	{
		return false;
	}
	return true;
}

double CDrawBlock::Inner_cos_of_vectors(MYPOINT pt1,MYPOINT pt2,MYPOINT pt3)
{
	//vector1;
	MYPOINT vector1 = {0},vector2 = {0};
	double a = 0;
	double temp = 0;

	vector1.x = pt2.x - pt1.x ;
	vector1.y = pt2.y - pt1.y ;

	vector2.x = pt2.x - pt3.x;
	vector2.y = pt2.y - pt3.y;

	temp = (vector1.x*vector2.x + vector1.y*vector2.y)/(sqrt(vector1.x*vector1.x + vector1.y*vector1.y)*sqrt(vector2.x*vector2.x + vector2.y*vector2.y));

	return temp;
}
bool CDrawBlock::IsEqual(double a, double b, double absError, double relError )
{
	if (a==b)
	{
		return true;
	}

	if (fabs(a-b)<absError )
	{
		return true;
	}

	return false;

	// 	if (fabs(a>b))
	// 	{
	// 		return  (fabs((a-b)/a)>relError ) ? true : false;
	// 	}

	//	return  (fabs((a-b)/b)>relError ) ? true : false;
}


bool CDrawBlock::GetPointOfSameSide(MYPOINT pt1,MYPOINT pt2,MYPOINT pt3,MYPOINT *ptres1,MYPOINT *ptres2)
{
	double m_scaleDis=m_Dis;
	if(g_pDMMapCfg->scaleSetting.scaleTitle.nScale!=0)
	{
		//m_scaleDis=m_Dis*30.0/g_pDMMapCfg->scaleSetting.scaleTitle.nScale;
		m_scaleDis=5.0;
	}
	//???????:
	if ((pt1.x == pt2.x && pt1.y == pt2.y) || (pt2.x == pt3.x && pt2.y == pt3.y))
	{
		*ptres1=pt2;
		*ptres2=pt2;
		return false;
	}


	//??,???????,?????????,????????.

	////////////////////////////////////////////////////////////
	//line P1P2;??? p1p2??; ???:A*x + B*y + c = 0;   ?????y - k*x + c = 0;
	double k12 = 0,b12 = 0;
	double c1 = 0,c2 = 0;

	double k23 = 0,b23 = 0;
	double c3 = 0,c4 = 0;


	MYPOINT ptinter1 = {0},ptinter2 = {0},ptinter3 = {0},ptinter4 = {0};  //ptinter1?ptinter2?p1p2??????p2p3????????
	MYPOINT ptrsul1 = {0},ptrsul2 = {0};
	//????1:
	if (pt1.x == pt2.x)
	{
		//???x????: x = pt1.x = pt2.x;
		//???m_Dis?????:x = pt2.x + m_Dis;
		//                      x = pt2.x - m_Dis;

		////////////////////////////////////////////////
		//line23;??? p2p3??;

		//????1:
		if (pt2.x == pt3.x)
		{
			//???x????: x = pt2.x = pt3.x;
			//???m_Dis?????:x = pt2.x + m_Dis;
			//                      x = pt2.x - m_Dis;
			if (pt2.y>pt1.y)
			{
				ptrsul1.x = pt2.x - m_scaleDis;
				ptrsul1.y = pt2.y;
				ptrsul2.x = pt2.x + m_scaleDis;
				ptrsul2.y = pt2.y;

			}
			else
			{
				ptrsul1.x = pt2.x + m_scaleDis;
				ptrsul1.y = pt2.y;
				ptrsul2.x = pt2.x - m_scaleDis;
				ptrsul2.y = pt2.y;
			}

			*ptres1 = ptrsul1;
			*ptres2 = ptrsul2;
			//debug code;
			if (ptres1->x == 0 || ptres2->y == 0)
			{
				int k = 0;
			}
			return true;

		}
		else if (pt2.y == pt3.y)
		{
			//???y????:y = pt2.y = pt3.y;
			//???m_Dis?????:y = pt2.y + m_Dis;
			//                      y = pt2.y - m_Dis;

			if (pt2.y > pt1.y)
			{
				ptinter1.x = pt2.x - m_scaleDis;
				ptinter1.y = pt2.y + m_scaleDis;

				ptinter2.x = pt2.x + m_scaleDis;
				ptinter2.y = pt2.y + m_scaleDis;

				ptinter3.x = pt2.x - m_scaleDis;
				ptinter3.y = pt2.y - m_scaleDis;

				ptinter4.x = pt2.x + m_scaleDis;
				ptinter4.y = pt2.y - m_scaleDis;
			}
			else
			{
				ptinter1.x = pt2.x + m_scaleDis;
				ptinter1.y = pt2.y + m_scaleDis;

				ptinter2.x = pt2.x - m_scaleDis;
				ptinter2.y = pt2.y + m_scaleDis;


				ptinter3.x = pt2.x + m_scaleDis;
				ptinter3.y = pt2.y - m_scaleDis;

				ptinter4.x = pt2.x - m_scaleDis;
				ptinter4.y = pt2.y - m_scaleDis;
			}
		}
		else   //undo:
		{
			k23 = (pt3.y - pt2.y)/(pt3.x - pt2.x);
			b23 = k23*pt3.x- pt3.y;
			//line23 fun:y - k23*x - b23 = 0;

			//parallel of line23;??? p2p3??????;
			//c3:y - k23*x + c3 = 0;
			//c4:y - k23*x + c4 = 0;

			// 			if (pt3.x>pt2.x)
			// 			{
			// 				c3 = b23 - m_scaleDis*sqrt(1+k23*k23);
			// 				c4 = b23 + m_scaleDis*sqrt(1+k23*k23);
			// 			}
			// 			else
			// 			{
			// 				c3 = b23 + m_scaleDis*sqrt(1+k23*k23);
			// 				c4 = b23 - m_scaleDis*sqrt(1+k23*k23);
			// 			}
			c3 = b23 - m_scaleDis*sqrt(1+k23*k23);
			c4 = b23 + m_scaleDis*sqrt(1+k23*k23);

			// 			//make sure that:c3 > c4;
			// 			if (c3 < c4)
			// 			{
			// 				double temp = c3;
			// 				c3 = c4;
			// 				c4 = temp;
			// 			}

			//???m_Dis?????:x = pt2.x + m_Dis;
			//                      x = pt2.x - m_Dis;

			if (pt2.y > pt1.y)
			{
				ptinter1.x = pt2.x - m_scaleDis;
				ptinter1.y = k23*ptinter1.x - c3;

				ptinter2.x = pt2.x - m_scaleDis;
				ptinter2.y = k23*ptinter1.x - c4;


				ptinter3.x = pt2.x + m_scaleDis;
				ptinter3.y = k23*ptinter3.x - c3;

				ptinter4.x = pt2.x + m_scaleDis;

				ptinter4.y = k23*ptinter4.x - c4;
			}
			else
			{
				ptinter1.x = pt2.x + m_scaleDis;
				ptinter1.y = k23*ptinter1.x - c3;

				ptinter2.x = pt2.x + m_scaleDis;
				ptinter2.y = k23*ptinter1.x - c4;


				ptinter3.x = pt2.x - m_scaleDis;
				ptinter3.y = k23*ptinter3.x - c3;

				ptinter4.x = pt2.x - m_scaleDis;
				ptinter4.y = k23*ptinter4.x - c4;
			}

		}
	}
	else if (pt1.y == pt2.y)
	{
		//???y????:y = pt1.y = pt2.y;
		//???m_Dis?????:y = pt2.y + m_Dis;
		//                      y = pt2.y - m_Dis;
		////////////////////////////////////////////////
		//line23;??? p2p3??;

		//????1:
		if (pt2.x == pt3.x)
		{
			//???x????: x = pt2.x = pt3.x;
			//???m_Dis?????:x = pt2.x + m_Dis;
			//                      x = pt2.x - m_Dis;

			if (pt2.x>pt1.x)
			{
				ptinter1.x = pt2.x + m_scaleDis;
				ptinter1.y = pt2.y + m_scaleDis;

				ptinter2.x = pt2.x - m_scaleDis;
				ptinter2.y = pt2.y + m_scaleDis;


				ptinter3.x = pt2.x + m_scaleDis;
				ptinter3.y = pt2.y - m_scaleDis;

				ptinter4.x = pt2.x - m_scaleDis;
				ptinter4.y = pt2.y - m_scaleDis;
			}
			else
			{
				ptinter1.x = pt2.x + m_scaleDis;
				ptinter1.y = pt2.y - m_scaleDis;

				ptinter2.x = pt2.x - m_scaleDis;
				ptinter2.y = pt2.y - m_scaleDis;


				ptinter3.x = pt2.x + m_scaleDis;
				ptinter3.y = pt2.y +m_scaleDis;

				ptinter4.x = pt2.x - m_scaleDis;
				ptinter4.y = pt2.y + m_scaleDis;
			}


		}
		else if (pt2.y == pt3.y)
		{
			//???y????:y = pt2.y = pt3.y;
			//???m_Dis?????:y = pt2.y + m_Dis;
			//                      y = pt2.y - m_Dis;

			if (pt2.x >pt1.x)
			{
				ptrsul1.x = pt2.x;
				ptrsul1.y = pt2.y + m_scaleDis;
				ptrsul2.x = pt2.x;
				ptrsul2.y = pt2.y - m_scaleDis;
			}
			else
			{
				ptrsul1.x = pt2.x;
				ptrsul1.y = pt2.y - m_scaleDis;
				ptrsul2.x = pt2.x;
				ptrsul2.y = pt2.y + m_scaleDis;
			}

			*ptres1 = ptrsul1;
			*ptres2 = ptrsul2;

			//debug code;
			if (ptres1->x == 0 || ptres2->y == 0)
			{
				int k = 0;
			}
			return true;

		}
		else
		{
			k23 = (pt3.y - pt2.y)/(pt3.x - pt2.x);
			b23 = k23*pt3.x- pt3.y;
			//line23 fun:y - k23*x + b23 = 0;

			//parallel of line23;??? p2p3??????;
			//c3:y - k23*x + c3 = 0;
			//c4:y - k23*x + c4 = 0;

			c3 = b23 + m_scaleDis*sqrt(1+k23*k23);
			c4 = b23 - m_scaleDis*sqrt(1+k23*k23);

			//make sure that:c3 > c4;
			// 			if (c3 < c4)
			// 			{
			// 				double temp = c3;
			// 				c3 = c4;
			// 				c4 = temp;
			// 			}

			//???m_Dis?????:y = pt2.y + m_Dis;
			//                      y = pt2.y - m_Dis;

			if (pt2.x >pt1.x)
			{
				ptinter1.y = pt2.y + m_scaleDis;
				ptinter1.x = (ptinter1.y + c3)/k23;

				ptinter2.y = pt2.y + m_scaleDis;
				ptinter2.x = (ptinter2.y + c4)/k23;

				ptinter3.y = pt2.y - m_scaleDis;
				ptinter3.x = (ptinter3.y + c3)/k23;

				ptinter4.y = pt2.y - m_scaleDis;
				ptinter4.x = (ptinter4.y + c4)/k23;
			}
			else
			{
				ptinter1.y = pt2.y - m_scaleDis;
				ptinter1.x = (ptinter1.y + c3)/k23;

				ptinter2.y = pt2.y - m_scaleDis;
				ptinter2.x = (ptinter2.y + c4)/k23;

				ptinter3.y = pt2.y + m_scaleDis;
				ptinter3.x = (ptinter3.y + c3)/k23;

				ptinter4.y = pt2.y + m_scaleDis;
				ptinter4.x = (ptinter4.y + c4)/k23;
			}

		}
	}
	else
	{

		//line12 fun:y - k12*x + b12 = 0;
		k12 = (pt2.y - pt1.y)/(pt2.x - pt1.x);
		b12 =  k12*pt1.x - pt1.y ;

		//parallel of line12;??? p1p2??????;
		//c1:y - k12*x + c1 = 0;
		//c2:y - k12*x + c2 = 0;
		if(pt2.x>pt1.x)
		{
			c1 = b12 - m_scaleDis*sqrt(1+k12*k12);
			c2 = b12 + m_scaleDis*sqrt(1+k12*k12);
		}
		else
		{
			c1 = b12 + m_scaleDis*sqrt(1+k12*k12);
			c2 = b12 - m_scaleDis*sqrt(1+k12*k12);
		}

		// 
		// 		//make sure c1 > c2;
		// 		if (c1 < c2)
		// 		{
		// 			double temp = c1;
		// 			c1 = c2;
		// 			c2 = temp;		
		// 		}

		//fun:   y - k12 + c1 = 0   and y - k12 + c2 = 0;

		////////////////////////////////////////////////
		//line23;??? p2p3??;

		//????1:
		if (pt2.x == pt3.x)
		{
			//???x????: x = pt2.x = pt3.x;
			//???m_Dis?????:x = pt2.x + m_Dis;
			//                      x = pt2.x - m_Dis;

			ptinter1.x = pt2.x + m_scaleDis;
			ptinter1.y = k12*ptinter1.x - c1;

			ptinter2.x = pt2.x - m_scaleDis;
			ptinter2.y = k12*ptinter2.x - c1;

			ptinter3.x = pt2.x + m_scaleDis;
			ptinter3.y = k12*ptinter3.x - c2;

			ptinter4.x = pt2.x - m_scaleDis;
			ptinter4.y = k12*ptinter4.x - c2;

		}
		else if (pt2.y == pt3.y)
		{
			//???y????:y = pt2.y = pt3.y;
			//???m_Dis?????:y = pt2.y + m_Dis;
			//                      y = pt2.y - m_Dis;

			ptinter1.y = pt2.y + m_scaleDis;
			ptinter1.x = (ptinter1.y + c1)/k12;

			ptinter2.y = pt2.y - m_scaleDis;
			ptinter2.x = (ptinter2.y + c1)/k12;

			ptinter3.y = pt2.y + m_scaleDis;
			ptinter3.x = (ptinter3.y + c2)/k12;

			ptinter4.y = pt2.y - m_scaleDis;
			ptinter4.x = (ptinter4.y + c2)/k12;

		}
		else
		{
			k23 = (pt3.y - pt2.y)/(pt3.x - pt2.x);
			b23 = k23*pt3.x- pt3.y;
			//line23 fun:y - k23*x - b23 = 0;

			//parallel of line23;??? p2p3??????;
			//c3:y - k23*x + c3 = 0;
			//c4:y - k23*x + c4 = 0;

			c3 = b23 - m_scaleDis*sqrt(1+k23*k23);
			c4 = b23 + m_scaleDis*sqrt(1+k23*k23);

			//make sure that:c3 > c4;
			// 			if (c3 < c4)
			// 			{
			// 				double temp = c3;
			// 				c3 = c4;
			// 				c4 = temp;
			// 			}

			//fun:   y - k23*x + c3 = 0   and y - k23*x + c4 = 0;

			//point of interrection;
			//?????????;
			//c1 ? c3,c4??? ;c2 ?c3 , c4???;

			//L12 ??? L23;
			//if (k12 == k23)//??
			if(IsEqual(k12,k23,0.01,0))
			{
				GetPointOfVerticalLine(pt2,pt3,&ptinter1,&ptinter3);
				ptrsul1 = ptinter1;
				ptrsul2 = ptinter3;

				*ptres1 = ptrsul1;
				*ptres2 = ptrsul2;


				//debug code;
				if (ptres1->x == 0 || ptres2->y == 0)
				{
					int k = 0;
				}
				return true;
			}

			//c1 and c3
			//c1 and c4

			//c1:y - k12*x + c1 = 0;  y = k12*x -c1;
			//c3:y - k23*x + c3 = 0;  y = k23 - c3;

			ptinter1.x =  (c1 - c3)/(k12 - k23);
			ptinter1.y =  k12 * ptinter1.x - c1;

			//c1:y - k12*x + c1 = 0;  y = k12*x -c1;
			//c4:y - k23*x + c4 = 0;  y = k23*x -c4;

			ptinter2.x =  (c1 - c4)/(k12 - k23);
			ptinter2.y =  k12 * ptinter2.x - c1;

			//c2 and c3
			//c2 and c4	

			//c2 and c3
			//c2:y - k12*x + c2 = 0;  y = k12 - c2;
			//c3:y - k23*x + c3 = 0;  y = k23 - c3;

			ptinter3.x = (c2 - c3)/(k12 - k23);
			ptinter3.y = k12 * ptinter3.x - c2;


			//c2 and c4	
			//c2:y - k12*x + c2 = 0;  y = k12 - c2;
			//c4:y - k23*x + c4 = 0;  y = k23*x -c4;

			ptinter4.x = (c2 - c4)/(k12 - k23);
			ptinter4.y = k12 * ptinter4.x - c2;
		}
	}

	//????????????;

	//point 1;
	double pcos1 = Inner_cos_of_vectors(pt1,pt2,ptinter1);
	double pcos2 = Inner_cos_of_vectors(ptinter1,pt2,pt3);

	if (IsEqual(pcos1,pcos2,m_absError,0))
	{
		ptrsul1 = ptinter1;
	}
	//point 2;
	double pcos11 = Inner_cos_of_vectors(pt1,pt2,ptinter2);
	double pcos22 = Inner_cos_of_vectors(ptinter2,pt2,pt3);

	if (IsEqual(pcos11,pcos22,m_absError,0))
	{
		ptrsul1 = ptinter2;
	}

	if (ptrsul1.x == 0.0)
	{
		int x = 0;
	}

	//point 3;
	double pcos111 = Inner_cos_of_vectors(pt1,pt2,ptinter3);
	double pcos222 = Inner_cos_of_vectors(ptinter3,pt2,pt3);

	if (IsEqual(pcos111,pcos222,m_absError,0))
	{
		ptrsul2 = ptinter3;
	}

	//point 4;
	double pcos1111 = Inner_cos_of_vectors(pt1,pt2,ptinter4);
	double pcos2222 = Inner_cos_of_vectors(ptinter4,pt2,pt3);

	if (IsEqual(pcos1111,pcos2222,m_absError,0))
	{
		ptrsul2 = ptinter4;
	}

	*ptres1 = ptrsul1;
	*ptres2 = ptrsul2;

	//debug code;
	if(ptres1->x - pt2.x < -10000 ||  ptres1->x - pt2.x > 10000 )
	{
		int kk1 = 0;
	}

	if(ptres1->y - pt2.y < -10000 ||  ptres1->y - pt2.y > 10000 )
	{
		int kk2 = 0;
	}

	if(ptres2->x - pt2.x < -10000 ||  ptres2->x - pt2.x > 10000 )
	{
		int kk3 = 0;
	}

	if(ptres2->y - pt2.y < -10000 ||  ptres2->y - pt2.y > 10000 )
	{
		int kk4 = 0;
	}
	//---------------------
	if (ptres1->x <1 || ptres1->y < 1 || ptres2->x < 1|| ptres2->y <1)
	{
		int k = 0;
	}

	return true;
}

nuBOOL CDrawBlock::DrawRoad(nuBOOL bRimLine/* = nuTRUE*/, nuBOOL bDrawRdName/* = nuTRUE*/)
{
	nuWORD nClassCount = m_roadCol.m_nClassCount, nRoadPtCount;
	nuINT nClassIdx, nClassStep, nRoadPtIdx, nRoadPtStep;
	nClassIdx= nClassStep= nRoadPtIdx= nRoadPtStep=0;
	NUROPOINT ptTmp1 = {0};
	NUROPOINT ptTmp2 = {0};
	NUROPOINT ptTmp11= {0};
	NUROPOINT ptTmp22= {0};
	NUROPOINT ptList[POINTLISTCOUNT]= {0};
	NURORECT rtBlock = {0};;
	
	nuSHORT nNowCount = 0;
	nuLONG nSkipLen = 0, /*dx, dy,*/ nLineWidth = 0, nSampleBit;
	PCONJLINE pConjLine = NULL;
	PLINESEG pLineSeg = NULL;
	PDWROADBLK pDwRoadBlk = NULL;
	nuINT nObjType=0;
	nuBYTE	nDrawType=0;
	nuBOOL bAddName = nuFALSE;

	//------------For col Road name-------------------
	nuWORD	tmpDwIdx = -1;
	DWROADCOMMON tmpRoadCommon= {0};
	nuSHORT nNameStartIdx = 0;
	//------------------------------------------------
	if( bRimLine )
	{
		nClassIdx = 0;
		nClassStep = 1;
		g_pRoadName->ResetRdNameList();
	}
	else
	{
		nClassIdx = m_roadCol.m_nClassCount - 1;
		nClassStep = -1;
	}
	while( nClassCount > 0 )
	{
		if( m_roadCol.m_pLineList[nClassIdx].pConjHead == NULL )
		{
			nClassCount --;
			nClassIdx += nClassStep;
			continue;
		}
		if( bRimLine )
		{
			nDrawType = DRAW_TYPE_ROADRIM;
		}
		else
		{
			nDrawType = DRAW_TYPE_ROADCENTER;
		}
		/*//test
		if( nClassIdx == 11 )
		{
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nShowCenterLine	= 1;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nShowRimLine		= 1;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorR = 255;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorG	= 0;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorB = 100;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorR	= 255;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorG	= 155;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorB	= 0;
		}
		
		if( nClassIdx == 13 )
		{
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorR = 0;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorG	= 0;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineColorB = 255;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorR	= 0;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorG	= 155;
			g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nRimLineColorB	= 255;
		}
		*///

		nObjType = g_pDMLibMT->MT_SelectObject( nDrawType, 
											    g_pDMGdata->uiSetData.b3DMode, 
											    &g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx],
											    &nLineWidth );
		if( nObjType == DRAW_OBJ_SKIP || 
		   (nObjType == DRWA_OBJ_ONLYCOLNAME && !bDrawRdName) )
		{
			nClassCount --;
			nClassIdx += nClassStep;
			continue;
		}
		if( bRimLine && bDrawRdName )
		{
			bAddName = g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nShowName;
		}
		else
		{
			bAddName = nuFALSE;
		}
		pConjLine = m_roadCol.m_pLineList[nClassIdx].pConjHead; //m_pRoadLineList[nClassIdx].pConjHead;
		nSkipLen = 2 * //g_pDMMapCfg->scaleSetting.pRoadSetting[nClassIdx].nCenterLineWidth *
				   g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;//SCALETOSCREEN_LEN;
		nSampleBit = 0;
		if( nSkipLen != 0 )
		{
			while( nSkipLen )
			{
				nSkipLen = nSkipLen >> 1;
				nSampleBit ++;
			}
		}
		while( pConjLine != NULL )
		{
			ptTmp1 = pConjLine->ptFrom;
			ptTmp11.x = ptTmp1.x >> nSampleBit;
			ptTmp11.y = ptTmp1.y >> nSampleBit;
			g_pDMLibMT->MT_MapToBmp(ptTmp1.x, ptTmp1.y, &ptList[0].x, &ptList[0].y);
			//
			nNowCount = 1;
			pLineSeg = pConjLine->pLineHead;
			//-------------For col Road name------------------------
			if( bRimLine )
			{
				nNameStartIdx = 0;
			}
			//------------------------------------------------------
			while( pLineSeg != NULL )
			{
				pDwRoadBlk = &m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx ].roadBlk;
				nRoadPtCount = pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nVertexCount;
				//-------------------For col Road name---------------
				if( bRimLine )
				{
					tmpDwIdx = m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].nDwIdx;
					tmpRoadCommon = pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nRoadCommonIdx ]; 
				}
				//----------------------------------------------------
				if( pLineSeg->nConjWay == CONJ_FROM_TO )
				{
					nRoadPtIdx = 0;
					nRoadPtStep = 1;
				}
				else
				{
					nRoadPtIdx = nRoadPtCount - 1;
					nRoadPtStep = -1;
				}
				nuBlockIDtoRect(m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx].nBlockID, &rtBlock);
				while( nRoadPtCount > 0 )
				{
					ptTmp2.x = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].x + rtBlock.left;
					ptTmp2.y = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].y + rtBlock.top;
					ptTmp22.x = ptTmp2.x >> nSampleBit;
					ptTmp22.y = ptTmp2.y >> nSampleBit;
					/*
					dx = ptTmp1.x - ptTmp2.x;
					dy = ptTmp1.y - ptTmp2.y;
					if( NURO_ABS(dx) >= nSkipLen || NURO_ABS(dy) >= nSkipLen )
					*/
					if( ptTmp22.x != ptTmp11.x || ptTmp22.y != ptTmp11.y )
					{
						g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						nNowCount ++;
						ptTmp1 = ptTmp2;
						ptTmp11 = ptTmp22;
						if( nNowCount == POINTLISTCOUNT )
						{
							//Draw line...
							g_pDMLibMT->MT_DrawObject(nObjType, ptList, nNowCount, nLineWidth);
							ptList[0] = ptList[nNowCount-1];
							nNowCount = 1;
							//--------------------For col Road name-------------------------------
							if( bAddName )
							{
								g_pRoadName->AddRoadName( tmpDwIdx, 
														  tmpRoadCommon.nRoadNameAddr, 
														  tmpRoadCommon.nRoadNameLen,
														  tmpRoadCommon.nRoadType,
														  ptList,
														  nNameStartIdx,
														  nNowCount - 1 );
							}
							nNameStartIdx = 0;
							//--------------------------------------------------------------------
						}
					}
					nRoadPtCount --;
					nRoadPtIdx += nRoadPtStep;
				}
				//-------------------For col Road name-----------------------------------
				pLineSeg = pLineSeg->pNext;
				if( bAddName &&
					( pLineSeg == NULL || tmpDwIdx != m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].nDwIdx ||
					  tmpRoadCommon.nRoadNameAddr != pDwRoadBlk->pDwRoadCommon[pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nRoadCommonIdx].nRoadNameAddr) )
				{
					g_pRoadName->AddRoadName( tmpDwIdx,
											  tmpRoadCommon.nRoadNameAddr,
											  tmpRoadCommon.nRoadNameLen,
											  tmpRoadCommon.nRoadType,
											  ptList,
											  nNameStartIdx,
											  nNowCount - 1 );
					nNameStartIdx = nNowCount - 1;
					if( pLineSeg != NULL )
					{
						tmpDwIdx = m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].nDwIdx;
						tmpRoadCommon = pDwRoadBlk->pDwRoadCommon[pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nRoadCommonIdx];
					}
				}
				//------------------------------------------------------------------------
			}
			/*
			if( nNowCount < POINTLISTCOUNT )
			{
				g_pDMLibMT->MT_MapToBmp(pConjLine->ptTo.x, pConjLine->ptTo.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
				nNowCount ++;
			}
			*/
			if( nNowCount > 1 )
			{
#if 0
				g_pDMLibMT->MT_MapToBmp(pConjLine->ptTo.x, pConjLine->ptTo.y, &ptList[nNowCount-1].x, &ptList[nNowCount-1].y);
#endif
				//Draw line...
				g_pDMLibMT->MT_DrawObject(nObjType, ptList, nNowCount, nLineWidth);
			}
			pConjLine = pConjLine->pNext;
		}
		nClassCount --;
		nClassIdx += nClassStep;
	}
	return nuTRUE;
}

nuBOOL CDrawBlock::DrawZoomRoad()
{
	/*&20081209&
	PCONJLINE pConjLine;
	PLINESEG pLineSeg;
	PDWROADBLK pDwRoadBlk;
	NUROPOINT ptTmp1, ptTmp2;
	NUROPOINT ptList[POINTLISTCOUNT];
	NURORECT rtBlock;
	nuLONG nSkipLen = 0, dx, dy, nLineWidth;
	nuINT nRoadPtIdx, nRoadPtStep, nObjType;
	nuSHORT nNowCount = 0;
	nuWORD nRoadPtCount;
	for( nuINT i = m_zoomRoadCol.m_nClassCount - 1; i >=0; i-- )
	{
		if( m_zoomRoadCol.m_pLineList[i].pConjHead	== NULL )
		{
			continue;
		}
		nSkipLen = g_pDMMapCfg->zoomSetting.pRoadSetting[i].nCenterLineWidth *
				   g_pDMMapCfg->zoomSetting.scaleTitle.nScale / SCALETOSCREEN_LEN;
		pConjLine = m_zoomRoadCol.m_pLineList[i].pConjHead;
		while( pConjLine != NULL )
		{
			ptTmp1	= pConjLine->ptFrom;
			g_pDMLibMT->MT_ZoomMapToBmp(ptTmp1.x, ptTmp1.y, &ptList[0].x, &ptList[0].y);
			nNowCount	= 1;
			pLineSeg = pConjLine->pLineHead;
			while( pLineSeg != NULL )
			{
				pDwRoadBlk = &m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx ].roadBlk;
				nRoadPtCount = pDwRoadBlk->pDwRoadData[pLineSeg->nScreenRoadIdx].nVertexCount;
				if( pLineSeg->nConjWay == CONJ_FROM_TO )
				{
					nRoadPtIdx = 0;
					nRoadPtStep = 1;
				}
				else
				{
					nRoadPtIdx = nRoadPtCount - 1;
					nRoadPtStep = -1;
				}
				nuBlockIDtoRect(m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx].nBlockID, &rtBlock);
				while( nRoadPtCount > 0 )
				{
					ptTmp2.x = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].x + rtBlock.left;
					ptTmp2.y = pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[ pLineSeg->nScreenRoadIdx ].nVertexAddrIdx + nRoadPtIdx ].y + rtBlock.top;
					dx = ptTmp1.x - ptTmp2.x;
					dy = ptTmp1.y - ptTmp2.y;
					if( NURO_ABS(dx) >= nSkipLen || NURO_ABS(dy) >= nSkipLen )
					{
						g_pDMLibMT->MT_ZoomMapToBmp(ptTmp2.x, ptTmp2.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
						nNowCount ++;
						ptTmp1 = ptTmp2;
						if( nNowCount == POINTLISTCOUNT-1 )
						{
							break;
						}
					}
					nRoadPtCount --;
					nRoadPtIdx += nRoadPtStep;
				}
				pLineSeg = pLineSeg->pNext;
				if( (pLineSeg == NULL && nNowCount > 1) || nNowCount == POINTLISTCOUNT-1 )
				{
					//Draw roads in zoom screen...
					nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOM_ROADRIM, 
														SHOWMODE_2DOBJ, 
														&g_pDMMapCfg->zoomSetting.pRoadSetting[i],
														&nLineWidth);
					if( nObjType != DRAW_OBJ_SKIP )
					{
						g_pDMLibMT->MT_DrawObject(nObjType, ptList, nNowCount, nLineWidth);
					}
					nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOM_ROADCENTER, 
														SHOWMODE_2DOBJ, 
														&g_pDMMapCfg->zoomSetting.pRoadSetting[i],
														&nLineWidth);
					if( nObjType != DRAW_OBJ_SKIP )
					{
						g_pDMLibMT->MT_DrawObject(nObjType, ptList, nNowCount, nLineWidth);
					}
				}
			}
			pConjLine = pConjLine->pNext;
		}
	}
	*///&20081209&
	return nuTRUE;
}

nuBOOL CDrawBlock::DrawBgLine()
{
	PCONJLINE pConjLine= NULL;
	PLINESEG pLineSeg= NULL;
	PDWBGLINEBLK pBgLineBlk= NULL;
	NURORECT rtBlock= {0};
	NUROPOINT ptTmp1= {0};
	NUROPOINT ptTmp2= {0};
	NUROPOINT ptList[POINTLISTCOUNT]= {0};
	nuINT nListIdx = 0;
	nuINT nSegPtCount = 0, nSegPtIdx = 0, nSegPtStep = 0, nSkipLen = 0;
	nuLONG dx = 0, dy = 0;
	nuBYTE nObjType = 0;
	for( nuINT i = m_bgLineCol.m_nClassCount - 1; i >= 0; i-- )
	{
		pConjLine = m_bgLineCol.m_pLineList[i].pConjHead;//m_pBgLineList[i].pConjHead;
		nSkipLen = g_pDMMapCfg->scaleSetting.pBgLineSetting[i].nCenterLineWidth *
				   g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;//SCALETOSCREEN_LEN;
		while( pConjLine != NULL )
		{
			ptTmp1 = pConjLine->ptFrom;
			g_pDMLibMT->MT_MapToBmp( ptTmp1.x, ptTmp1.y, &ptList[0].x, &ptList[0].y );
			nListIdx = 1;
			pLineSeg = pConjLine->pLineHead;
			while( pLineSeg != NULL )
			{
				pBgLineBlk = &m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx ].bgLineBlk;
				nSegPtCount = pBgLineBlk->pDwBgLineData[pLineSeg->nScreenRoadIdx].nVertexCount;
				if( pLineSeg->nConjWay == CONJ_FROM_TO )
				{
					nSegPtIdx = pBgLineBlk->pDwBgLineData[ pLineSeg->nScreenRoadIdx].nVertexIdx;
					nSegPtStep = 1;
				}
				else
				{
					nSegPtIdx = pBgLineBlk->pDwBgLineData[ pLineSeg->nScreenRoadIdx].nVertexIdx + nSegPtCount - 1;
					nSegPtStep = -1;
				}
				nuBlockIDtoRect(m_pScreenMap->pDwList[ pLineSeg->nScreenDwIdx ].pBlockList[ pLineSeg->nScreenBlkIdx].nBlockID, &rtBlock);
				while( nSegPtCount > 0 )
				{
					ptTmp2.x = pBgLineBlk->pSPointCoor[ nSegPtIdx ].x + rtBlock.left;
					ptTmp2.y = pBgLineBlk->pSPointCoor[ nSegPtIdx ].y + rtBlock.top;
					dx = ptTmp2.x - ptTmp1.x;
					dy = ptTmp2.y - ptTmp1.y;
					if( NURO_ABS(dx) > nSkipLen || NURO_ABS(dy) > nSkipLen )
					{
						g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptList[nListIdx].x, &ptList[nListIdx].y);
						nListIdx ++;
						ptTmp1 = ptTmp2;
						if( nListIdx == POINTLISTCOUNT )
						{
							break;
						}
					}
					//
					nSegPtIdx += nSegPtStep;
					nSegPtCount--;
				}
				pLineSeg = pLineSeg->pNext;
				if( nListIdx > 1 && (pLineSeg == NULL || nListIdx == POINTLISTCOUNT) )
				{
					/*
					g_pDMLibMT->MT_DrawObject(DRAW_OBJ_2D_LINE, ptList, nListIdx, 0);
					*/
					//Draw Bgline Rim...
					nObjType = g_pDMLibMT->MT_SelectObject(DRAW_TYPE_BGLINERIM, g_pDMGdata->uiSetData.b3DMode, &g_pDMMapCfg->scaleSetting.pBgLineSetting[i], NULL);
					g_pDMLibMT->MT_DrawObject(nObjType, ptList, nListIdx, 0);
					//Darw Bgline Center...
					nObjType = g_pDMLibMT->MT_SelectObject(DRAW_TYPE_BGLINECENTER, g_pDMGdata->uiSetData.b3DMode, &g_pDMMapCfg->scaleSetting.pBgLineSetting[i], NULL);
					g_pDMLibMT->MT_DrawObject(nObjType, ptList, nListIdx, 0);
					nListIdx = 0;
				}
			}
			pConjLine = pConjLine->pNext;
		}
	}
	return nuTRUE;
}

nuBOOL CDrawBlock::DrawBgArea()
{
	nuINT nObjType = 0;
	PAREANODE pAreaNode= NULL;
	NURORECT rtBlock= {0};
	NUROPOINT ptTmp1= {0};
	NUROPOINT ptTmp2= {0};
	nuDWORD nBlockID = -1;
	nuLONG dx = 0;
	nuLONG dy = 0;
	nuINT nListIdx = 0;
	nuINT nSkipLen = 0;
	NUROPOINT ptList[POINTLISTCOUNT]= {0};
	for(nuUINT i = 0; i < m_bgAreaCol.m_nAreaType; i++ )
	{
		if( m_bgAreaCol.m_pAreaList[i].pAreaHead == NULL || m_bgAreaCol.m_nAreaType == 0 )
		{
			continue;
		}
		nObjType = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_BGAREA, 
											g_pDMGdata->uiSetData.b3DMode,
											&g_pDMMapCfg->scaleSetting.pBgAreaSetting[i],
											NULL);
		if( nObjType == DRAW_OBJ_SKIP )
		{
			continue;
		}
		nSkipLen = g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;
		pAreaNode = m_bgAreaCol.m_pAreaList[i].pAreaHead;
		while( pAreaNode != NULL )
		{
			if( nBlockID != m_pScreenMap->pDwList[ pAreaNode->nScreenDwIdx ].pBlockList[ pAreaNode->nScreenBlkIdx ].nBlockID )
			{
				nBlockID = m_pScreenMap->pDwList[ pAreaNode->nScreenDwIdx ].pBlockList[ pAreaNode->nScreenBlkIdx ].nBlockID;
				nuBlockIDtoRect(nBlockID, &rtBlock);
			}
			PDWBGAREABLK pBgAreaBlk = &m_pScreenMap->pDwList[ pAreaNode->nScreenDwIdx ].pBlockList[ pAreaNode->nScreenBlkIdx].bgAreaBlk;
			nListIdx = 0;
			for( nuINT j = 0; j < pBgAreaBlk->pDwBgAreaData[ pAreaNode->nScreenSegIdx ].nVertexCount; j++ )
			{
				ptTmp2.x = pBgAreaBlk->pSPointCoor[ pBgAreaBlk->pDwBgAreaData[ pAreaNode->nScreenSegIdx ].nVertexIdx + j ].x + rtBlock.left;
				ptTmp2.y = pBgAreaBlk->pSPointCoor[ pBgAreaBlk->pDwBgAreaData[ pAreaNode->nScreenSegIdx ].nVertexIdx + j ].y + rtBlock.top;

				/*
				if( j != 0 && j != pBgAreaBlk->pDwBgAreaData[ pAreaNode->nScreenSegIdx ].nVertexCount - 1 )
				{
					dx = ptTmp2.x - ptTmp1.x;
					if( NURO_ABS(dx) <= nSkipLen )
					{
						dy = ptTmp2.y - ptTmp1.y;
						if( NURO_ABS(dy) <= nSkipLen )
						{
							continue;
						}
					}
				}
				*/
				g_pDMLibMT->MT_MapToBmp(ptTmp2.x, ptTmp2.y, &ptList[nListIdx].x, &ptList[nListIdx].y);
				if( j != 0 && j != pBgAreaBlk->pDwBgAreaData[ pAreaNode->nScreenSegIdx ].nVertexCount - 1 )
				{
					dx = ptList[nListIdx].x - ptList[nListIdx-1].x;
					dy = ptList[nListIdx].y - ptList[nListIdx-1].y;
					dx = NURO_ABS(dx);
					dy = NURO_ABS(dy);
					if( dx < 1 && dy < 1 )
					{
						continue;
					}
				}
				++nListIdx;
				ptTmp1 = ptTmp2;
				if( nListIdx >= POINTLISTCOUNT )
				{
					g_pDMLibMT->MT_DrawObject( nObjType, ptList, nListIdx, 0);
					ptList[1] = ptList[nListIdx-1];
					nListIdx = 2;//Keep the first point, keep the last point as the second poin
				}
			}
			if( nListIdx > 2 )
			{
				g_pDMLibMT->MT_DrawObject( nObjType, ptList, nListIdx, 0);
			}
			pAreaNode = pAreaNode->pNext;
		}
	}
	return nuTRUE;
}

nuBOOL CDrawBlock::CollectScreenPoi()
{
	g_pPoiName->ResetPoiList();
//	g_pPoiZoom->ResetPoiList();
	if( m_pScreenMap == NULL )
	{
		return nuFALSE;
	}
	nuroRECT rtBlock= {0}, rtTmp= {0};
	nuroPOINT ptTmp= {0};
	nuLONG nTypeID1 = 0, nTypeID2 = 0;//, nDisplay = 1, nIcon;
	PPTNODE pPtNode= NULL;
	//
	rtTmp.left	= g_pDMGdata->transfData.nMapCenterX - 100;
	rtTmp.right	= g_pDMGdata->transfData.nMapCenterX + 100;
	rtTmp.top	= g_pDMGdata->transfData.nMapCenterY - 100;
	rtTmp.bottom= g_pDMGdata->transfData.nMapCenterY + 100;

	nuINT iResZoomAdd = ADD_RES_FAIL;
	nuINT iResNormalAdd = ADD_RES_FAIL;
	nuWORD i=0, j=0, k=0;
	PDWPOIBLK pPoiBlk= NULL;

	if( g_pDMGdata->drawInfoMap.bFindCenterPoi )
	{
		pPtNode = (PPTNODE)g_pDMLibFS->FS_GetPtNodeByType(g_pDMGdata->drawInfoMap.poiCenterOn.nMapIdx,
								  g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType1, 
								  g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType2 );
		/* &20081209&
		if( g_pDMGdata->zoomScreenData.bZoomScreen )
		{
			g_pPoiZoom->AddPoiNode( g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType1, 
									g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType2, 
									g_pDMGdata->drawInfoMap.poiCenterOn.nPOINameLen,
									g_pDMGdata->drawInfoMap.poiCenterOn.nMapIdx,
									g_pDMGdata->drawInfoMap.poiCenterOn.nNameAddr,
									g_pDMGdata->drawInfoMap.poiCenterOn.nReserve,
									g_pDMGdata->drawInfoMap.poiCenterOn.nCityID,
									pPtNode->nResID,
									g_pDMGdata->drawInfoMap.poiCenterOn.pCoor );
		}
		*///&20081209&
		if( pPtNode != NULL && pPtNode->nDisplay )
		{
			g_pPoiName->AddPoiNode( g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType1, 
				g_pDMGdata->drawInfoMap.poiCenterOn.nPOIType2, 
				g_pDMGdata->drawInfoMap.poiCenterOn.nPOINameLen,
				g_pDMGdata->drawInfoMap.poiCenterOn.nMapIdx,
				g_pDMGdata->drawInfoMap.poiCenterOn.nNameAddr,
				g_pDMGdata->drawInfoMap.poiCenterOn.nReserve,
				g_pDMGdata->drawInfoMap.poiCenterOn.nCityID,
				pPtNode->nResID,
				g_pDMGdata->drawInfoMap.poiCenterOn.pCoor );
			
		}
	}//show the center poi.
	//////////////////////////////////////////////////////////////////////////
	/*&20081209&
	for( i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for( j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++)
		{
			if( m_pScreenMap->pDwList[i].pBlockList[j].poiBlk.pDwPOIHead == NULL )
			{
				continue;
			}
			nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
			pPoiBlk = &(m_pScreenMap->pDwList[i].pBlockList[j].poiBlk);
			for( k = 0; k < pPoiBlk->pDwPOIHead->nPOICount; k++ )
			{
				if( pPoiBlk->pDwPOIData[k].nReserve == 0)
				{
					continue;
				}
				ptTmp.x = rtBlock.left + pPoiBlk->pDwPOIData[k].spCoor.x;
				ptTmp.y = rtBlock.top + pPoiBlk->pDwPOIData[k].spCoor.y;

				//New PT
				nTypeID1 = pPoiBlk->pDwPOIData[k].nPOIType1;
				nTypeID2 = pPoiBlk->pDwPOIData[k].nPOIType2;
				if( nTypeID1 < 0 || nTypeID1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
				{
					continue;
				}
				pPtNode = (PPTNODE)g_pDMLibFS->FS_GetPtNodeByType(pPoiBlk->pDwPOIData[k].nPOIType1, pPoiBlk->pDwPOIData[k].nPOIType2);
				if( pPtNode == NULL )
				{
					continue;
				}
				if( g_pDMGdata->zoomScreenData.bZoomScreen && iResZoomAdd != ADD_RES_FULL &&
					nuPointInRect(&ptTmp, &g_pDMGdata->zoomScreenData.nuZoomMapSize) )
				{
					//氝樓扽衾zoom毓峓腔poi
					iResZoomAdd = g_pPoiZoom->AddPoiNode( pPoiBlk->pDwPOIData[k].nPOIType1,
						pPoiBlk->pDwPOIData[k].nPOIType2,
						pPoiBlk->pDwPOIData[k].nPOINameLen,
						m_pScreenMap->pDwList[i].nDwIdx,
						pPoiBlk->pDwPOIData[k].nNameAddr,
						pPoiBlk->pDwPOIData[k].nReserve,
						pPoiBlk->pDwPOIData[k].nCityID,
						pPtNode->nResID,
						ptTmp );
				}

				if( iResNormalAdd != ADD_RES_FULL )
				{
					iResNormalAdd = g_pPoiName->AddPoiNode( pPoiBlk->pDwPOIData[k].nPOIType1,
						pPoiBlk->pDwPOIData[k].nPOIType2,
						pPoiBlk->pDwPOIData[k].nPOINameLen,
						m_pScreenMap->pDwList[i].nDwIdx,
						pPoiBlk->pDwPOIData[k].nNameAddr,
						pPoiBlk->pDwPOIData[k].nReserve,
						pPoiBlk->pDwPOIData[k].nCityID,
						pPtNode->nResID,
						ptTmp);
				}

				if(iResZoomAdd == ADD_RES_FULL && iResNormalAdd == ADD_RES_FULL)
				{
					return nuTRUE;
				}
			}
		}
	}
	*///&20081209&
	//////////////////////////////////////////////////////////////////////////

	//Special POI col
	for( i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for( j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++)
		{
			if( m_pScreenMap->pDwList[i].pBlockList[j].poiBlk.pDwPOIHead == NULL )
			{
				continue;
			}
			nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
			pPoiBlk = &(m_pScreenMap->pDwList[i].pBlockList[j].poiBlk);
			for( k = 0; k < pPoiBlk->pDwPOIHead->nPOICount; k++ )
			{
				if( pPoiBlk->pDwPOIData[k].nReserve == 0)
				{
					continue;
				}
				/*
				if( pPoiBlk->pDwPOIData[k].nReserve == 8 )
				{
					nuINT outle = 0;
				}
				*/
				ptTmp.x = rtBlock.left + pPoiBlk->pDwPOIData[k].spCoor.x;
				ptTmp.y = rtBlock.top + pPoiBlk->pDwPOIData[k].spCoor.y;

				/* &20081209&
				if( g_pDMGdata->zoomScreenData.bZoomScreen && 
					!nuPointInRect(&ptTmp, &g_pDMGdata->zoomScreenData.nuZoomMapSize ) )
				{
					continue;
				}
				*///&20081209&
				if( /*!g_pDMGdata->zoomScreenData.bZoomScreen && */!nuPointInRect(&ptTmp, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				//New PT
				nTypeID1 = pPoiBlk->pDwPOIData[k].nPOIType1;
				nTypeID2 = pPoiBlk->pDwPOIData[k].nPOIType2;
				if( nTypeID1 < 0 || nTypeID1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
				{
					continue;
				}
				pPtNode = (PPTNODE)g_pDMLibFS->FS_GetPtNodeByType(g_pDMGdata->drawInfoMap.poiCenterOn.nMapIdx,
										  pPoiBlk->pDwPOIData[k].nPOIType1, 
										  pPoiBlk->pDwPOIData[k].nPOIType2);
				if( pPtNode == NULL /*|| !pPtNode->nDisplay*/ )
				{
					continue;
				}

				/* &20081209&
				if( g_pDMGdata->zoomScreenData.bZoomScreen && iResZoomAdd != ADD_RES_FULL)
				{
					iResZoomAdd = g_pPoiZoom->AddPoiNode( pPoiBlk->pDwPOIData[k].nPOIType1,
						pPoiBlk->pDwPOIData[k].nPOIType2,
						pPoiBlk->pDwPOIData[k].nPOINameLen,
						m_pScreenMap->pDwList[i].nDwIdx,
						pPoiBlk->pDwPOIData[k].nNameAddr,
						pPoiBlk->pDwPOIData[k].nReserve,
						pPoiBlk->pDwPOIData[k].nCityID,
						pPtNode->nResID,
						ptTmp );
				}
				*///&20081209&
				if(  iResNormalAdd != ADD_RES_FULL )
				{
					/**/
					
					
					iResNormalAdd = g_pPoiName->AddPoiNode( pPoiBlk->pDwPOIData[k].nPOIType1,
						pPoiBlk->pDwPOIData[k].nPOIType2,
						pPoiBlk->pDwPOIData[k].nPOINameLen,
						m_pScreenMap->pDwList[i].nDwIdx,
						pPoiBlk->pDwPOIData[k].nNameAddr,
						pPoiBlk->pDwPOIData[k].nReserve,
						pPoiBlk->pDwPOIData[k].nCityID,
						pPtNode->nResID,
						ptTmp);
				}

				if( iResZoomAdd == ADD_RES_FULL && iResNormalAdd == ADD_RES_FULL)
				{
					return nuTRUE;
				}
			}
		}
	}
	
	for(i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for(nuWORD j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++)
		{
			if( m_pScreenMap->pDwList[i].pBlockList[j].poiBlk.pDwPOIHead == NULL )
			{
				continue;
			}
			nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
			pPoiBlk = &(m_pScreenMap->pDwList[i].pBlockList[j].poiBlk);
			for( k = 0; k < pPoiBlk->pDwPOIHead->nPOICount; k++ )
			{
				if( pPoiBlk->pDwPOIData[k].nReserve != 0)
				{
					continue;
				}

				ptTmp.x = rtBlock.left + pPoiBlk->pDwPOIData[k].spCoor.x;
				ptTmp.y = rtBlock.top + pPoiBlk->pDwPOIData[k].spCoor.y;
				if( !nuPointInRect(&ptTmp, &g_pDMGdata->transfData.nuShowMapSize) )
				{
					continue;
				}
				//New PT
				nTypeID1 = pPoiBlk->pDwPOIData[k].nPOIType1;
				nTypeID2 = pPoiBlk->pDwPOIData[k].nPOIType2;
				if( nTypeID1 < 0 || nTypeID1 >= g_pDMMapCfg->commonSetting.nPOIType1Count )
				{
					continue;
				}
				pPtNode = (PPTNODE)g_pDMLibFS->FS_GetPtNodeByType(g_pDMGdata->drawInfoMap.poiCenterOn.nMapIdx,
										  pPoiBlk->pDwPOIData[k].nPOIType1, 
										  pPoiBlk->pDwPOIData[k].nPOIType2);
				if( pPtNode == NULL || !pPtNode->nDisplay )
				{
					continue;
				}
				//
				if( iResNormalAdd != ADD_RES_FULL )
				{
					iResNormalAdd = g_pPoiName->AddPoiNode( pPoiBlk->pDwPOIData[k].nPOIType1,
						pPoiBlk->pDwPOIData[k].nPOIType2,
						pPoiBlk->pDwPOIData[k].nPOINameLen,
						m_pScreenMap->pDwList[i].nDwIdx,
						pPoiBlk->pDwPOIData[k].nNameAddr,
						pPoiBlk->pDwPOIData[k].nReserve,
						pPoiBlk->pDwPOIData[k].nCityID,
						pPtNode->nResID,
						ptTmp );
				}
				else
				{
					return nuTRUE;
				}
			}
		}
	}
	return nuTRUE;
}

nuBOOL CDrawBlock::SeekForRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData)
{
	LoadScreenMap();
	if( m_pScreenMap == NULL || nCoverDis < 0 )
	{
		return nuFALSE;
	}
	PDWROADBLK pRoadBlk= NULL;
	BLOCKIDARRAY blockIDArray= {0};
	SEEKEDROADDATA roadData= {0};
	nuroRECT rect= {0}, rtBlock= {0}, rtRoad= {0};
	nuDWORD nBlockID=0;
	nuroPOINT point = {x, y}, pt1= {0}, pt2= {0}, ptFixed= {0};
	nuBOOL bFind =nuFALSE, bFindRoad = nuFALSE;
	nuLONG tmpDis=0;
	rect.left	= x - nCoverDis;
	rect.top	= y - nCoverDis;
	rect.right	= x + nCoverDis;
	rect.bottom	= y + nCoverDis;
	nuRectToBlockIDArray(rect, &blockIDArray);
	for(nuWORD i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for( nuWORD j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++ )
		{
			if( m_pScreenMap->pDwList[i].pBlockList[j].roadBlk.pDwRoadHead == NULL )
			{
				continue;
			}
			//Compare BlockID
			bFind = nuFALSE;
			for( nuLONG m = blockIDArray.nYstart; m <= blockIDArray.nYend; m++ )
			{
				for( nuLONG n = blockIDArray.nXstart; n <= blockIDArray.nXend; n++ )
				{
					nBlockID = m * XBLOCKNUM + n;
					if( nBlockID == m_pScreenMap->pDwList[i].pBlockList[j].nBlockID )
					{
						bFind = nuTRUE;
						break;
					}
				}
				if( bFind )
				{
					break;
				}
			}
			if( bFind )
			{
				nuBlockIDtoRect(nBlockID, &rtBlock);
				pRoadBlk = &m_pScreenMap->pDwList[i].pBlockList[j].roadBlk;
				for( nuDWORD k = 0; k < pRoadBlk->pDwRoadHead->nRoadCount; ++k )
				{
#ifdef VALUE_EMGRT
					if( pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType >= ROAD_CLASS_IGNORE )
					{
						continue;
					}
#endif
					rtRoad.left		= rtBlock.left + pRoadBlk->pRoadRect[k].left - nCoverDis;
					rtRoad.top		= rtBlock.top + pRoadBlk->pRoadRect[k].top - nCoverDis;
					rtRoad.right	= rtBlock.left + pRoadBlk->pRoadRect[k].right + nCoverDis;
					rtRoad.bottom	= rtBlock.top + pRoadBlk->pRoadRect[k].bottom + nCoverDis;
					if( !nuPtInRect(point, rtRoad) )
					{
						continue;
					}
					pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].x + rtBlock.left;
					pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].y + rtBlock.top;
					for( nuDWORD l = 1; l < pRoadBlk->pDwRoadData[k].nVertexCount; ++l )
					{
						pt1 = pt2;
						pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].x + rtBlock.left;
						pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].y + rtBlock.top;
						rtRoad.left		= NURO_MIN(pt1.x, pt2.x) - nCoverDis;
						rtRoad.right	= NURO_MAX(pt1.x, pt2.x) + nCoverDis;
						rtRoad.top		= NURO_MIN(pt1.y, pt2.y) - nCoverDis;
						rtRoad.bottom	= NURO_MAX(pt1.y, pt2.y) + nCoverDis;
						if( !nuPtInRect(point, rtRoad) )
						{
							continue;
						}
						if( nuPtFixToLineSeg(point, pt1, pt2, &ptFixed, &tmpDis) && tmpDis < nCoverDis )
						{
							//if(pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType>10) continue; //Louis 20120330以道路等級限制手動移圖的可拉回導路
							bFindRoad = nuTRUE;
							nCoverDis = tmpDis;
							roadData.nDis	= nuLOWORD(tmpDis);
							roadData.nDwIdx	= m_pScreenMap->pDwList[i].nDwIdx;
							roadData.nBlkIdx= m_pScreenMap->pDwList[i].pBlockList[j].nBlockIdx;
							roadData.nBlockID	= m_pScreenMap->pDwList[i].pBlockList[j].nBlockID;
							roadData.nRoadIdx	= k;
							roadData.nNameAddr	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadNameAddr;
							roadData.nRoadClass	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType;
							roadData.nTownID	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nTownID;
							roadData.nCityID	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nCityID;
							roadData.ptMapped	= ptFixed;
							roadData.nLimitSpeed= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nSpeedLimit;
							roadData.bGPSState	= pRoadBlk->pDwRoadData[i].bGPSState;
							roadData.nOneWay	= pRoadBlk->pDwRoadData[k].nOneWay;
							
						}
					}
				}
			}
		}
	}
	if( bFindRoad )
	{
		*pRoadData	= roadData;
	}
	return bFindRoad;
}
nuBOOL CDrawBlock::CarToRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, nuLONG nCarAngle, PSEEKEDROADDATA pRoadData)
{
	LoadScreenMap();
	if( m_pScreenMap == NULL || nCoverDis < 0 )
	{
		return nuFALSE;
	}
	PDWROADBLK pRoadBlk= NULL;
	BLOCKIDARRAY blockIDArray= {0};
	SEEKEDROADDATA roadData= {0};
	nuroRECT rect= {0}, rtBlock= {0}, rtRoad= {0};
	nuDWORD nBlockID =0 ;
	nuroPOINT point = {x, y}, pt1= {0}, pt2= {0}, ptFixed= {0};
	nuBOOL bFind=nuFALSE, bFindRoad = nuFALSE;
	nuLONG tmpDis=0, tmpAngle=0, dx=0, dy=0;
	rect.left	= x - nCoverDis;
	rect.top	= y - nCoverDis;
	rect.right	= x + nCoverDis;
	rect.bottom	= y + nCoverDis;
	nuRectToBlockIDArray(rect, &blockIDArray);
	for(nuWORD i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for( nuWORD j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++ )
		{
			if( m_pScreenMap->pDwList[i].pBlockList[j].roadBlk.pDwRoadHead == NULL )
			{
				continue;
			}
			//Compare BlockID
			bFind = nuFALSE;
			for( nuLONG m = blockIDArray.nYstart; m <= blockIDArray.nYend; m++ )
			{
				for( nuLONG n = blockIDArray.nXstart; n <= blockIDArray.nXend; n++ )
				{
					nBlockID = m * XBLOCKNUM + n;
					if( nBlockID == m_pScreenMap->pDwList[i].pBlockList[j].nBlockID )
					{
						bFind = nuTRUE;
						break;
					}
				}
				if( bFind )
				{
					break;
				}
			}
			if( bFind )
			{
				nuBlockIDtoRect(nBlockID, &rtBlock);
				pRoadBlk = &m_pScreenMap->pDwList[i].pBlockList[j].roadBlk;
				for( nuDWORD k = 0; k < pRoadBlk->pDwRoadHead->nRoadCount; k++ )
				{
					rtRoad.left		= rtBlock.left + pRoadBlk->pRoadRect[k].left - nCoverDis;
					rtRoad.top		= rtBlock.top + pRoadBlk->pRoadRect[k].top - nCoverDis;
					rtRoad.right	= rtBlock.left + pRoadBlk->pRoadRect[k].right + nCoverDis;
					rtRoad.bottom	= rtBlock.top + pRoadBlk->pRoadRect[k].bottom + nCoverDis;
					if( !nuPtInRect(point, rtRoad) )
					{
						continue;
					}
					pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].x + rtBlock.left;
					pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx ].y + rtBlock.top;
					for( nuDWORD l = 1; l < pRoadBlk->pDwRoadData[k].nVertexCount; l++ )
					{
						pt1 = pt2;
						pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].x + rtBlock.left;
						pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].y + rtBlock.top;
						rtRoad.left		= NURO_MIN(pt1.x, pt2.x) - nCoverDis;
						rtRoad.right	= NURO_MAX(pt1.x, pt2.x) + nCoverDis;
						rtRoad.top		= NURO_MIN(pt1.y, pt2.y) - nCoverDis;
						rtRoad.bottom	= NURO_MAX(pt1.y, pt2.y) + nCoverDis;
						if( !nuPtInRect(point, rtRoad) )
						{
							continue;
						}
						dx = pt2.x - pt1.x;
						dy = pt2.y - pt1.y;
						tmpAngle = nulAtan2(dy, dx);
//						tmpAngle = (nuLONG)(nuAtan2(dy, dx) * 180/PI);
						tmpAngle = (nCarAngle - tmpAngle)%360;
						tmpAngle = NURO_ABS(tmpAngle);
						if( tmpAngle > 180 )
						{
							tmpAngle = 360 - tmpAngle;
						}
						if( pRoadBlk->pDwRoadData[k].nOneWay == 1 )
						{
							if( tmpAngle > ANGLE_FIXTOROAD )
							{
								continue;
							}
						}
						else if( pRoadBlk->pDwRoadData[k].nOneWay == 2 )
						{
							if( tmpAngle < 180 - ANGLE_FIXTOROAD )
							{
								continue;
							}
							dx = -dx;
							dy = -dy;
						}
						else
						{
							if( tmpAngle > ANGLE_FIXTOROAD && tmpAngle < 180 - ANGLE_FIXTOROAD )
							{
								continue;
							}
							else if( tmpAngle >= 180 - ANGLE_FIXTOROAD )
							{
								dx = -dx;
								dy = -dy;
							}
						}
						if( nuPtFixToLineSeg(point, pt1, pt2, &ptFixed, &tmpDis) && tmpDis < nCoverDis )
						{
							bFindRoad = nuTRUE;
							nCoverDis = tmpDis;
							roadData.nDis		= nuLOWORD(tmpDis);
							roadData.nDwIdx		= m_pScreenMap->pDwList[i].nDwIdx;
							roadData.nBlkIdx	= m_pScreenMap->pDwList[i].pBlockList[j].nBlockIdx;
							roadData.nBlockID	= m_pScreenMap->pDwList[i].pBlockList[j].nBlockID;
							roadData.nRoadIdx	= k;
							roadData.nNameAddr	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadNameAddr;
							roadData.nRoadClass	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType;
							roadData.nTownID	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nTownID;
							roadData.nCityID	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nCityID;
							roadData.nLimitSpeed= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nSpeedLimit;
							roadData.ptMapped	= ptFixed;
							roadData.nRdDx		= nuLOWORD(dx);
							roadData.nRdDy		= nuLOWORD(dy);
							roadData.bGPSState  = pRoadBlk->pDwRoadData[i].bGPSState;
						}
					}
				}
			}
		}
	}
	if( bFindRoad )
	{
		*pRoadData = roadData;
	}
	return bFindRoad;
}


nuBOOL CDrawBlock::DrawZoomFake()
{
	LoadScreenMap();
	if( m_pScreenMap == NULL )
	{
		return nuTRUE;
	}
	NURORECT rtBlock= {0}, rect= {0};
	NUROPOINT ptOld= {0}, ptNew= {0};
	PDWROADBLK pDwRoadBlk= NULL;
	NUROPOINT ptList[POINTLISTCOUNT]= {0};
	nuSHORT nNowCount = 0;
	nuLONG nSkipLen = 0, dx=0, dy=0;
	nuINT nDrawObj=0;
	nuINT nClass = -1;
	nSkipLen = g_pDMMapCfg->scaleSetting.scaleTitle.nScale / g_pDMGdata->drawInfoMap.nScalePixels;//SCALETOSCREEN_LEN;
	for(nuWORD i = 0; i < m_pScreenMap->nDwCount; i++)
	{
		if( m_pScreenMap->pDwList[i].pBlockList == NULL )
		{
			continue;
		}
		for(nuWORD j = 0; j < m_pScreenMap->pDwList[i].nNumOfBlocks; j++)
		{
			nuBlockIDtoRect(m_pScreenMap->pDwList[i].pBlockList[j].nBlockID, &rtBlock);
		
			pDwRoadBlk = &m_pScreenMap->pDwList[i].pBlockList[j].roadBlk;
			if( pDwRoadBlk->pDwRoadHead != NULL )
			{
				for(nuDWORD k = 0; k < pDwRoadBlk->pDwRoadHead->nRoadCount; k++)
				{
					if( pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType >=
						g_pDMMapCfg->commonSetting.nRoadTypeCount )//Judge the RoadType
					{
						continue;
					}
					if( !g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowRimLine &&
						!g_pDMMapCfg->scaleSetting.pRoadSetting[ pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType ].nShowCenterLine )
					{
						continue;
					}
					rect.left	= pDwRoadBlk->pRoadRect[k].left;
					rect.top	= pDwRoadBlk->pRoadRect[k].top;
					rect.right	= pDwRoadBlk->pRoadRect[k].right;
					rect.bottom = pDwRoadBlk->pRoadRect[k].bottom;
					rect.left	+= rtBlock.left;
					rect.right	+= rtBlock.left;
					rect.top	+= rtBlock.top;
					rect.bottom	+= rtBlock.top;
					if( !nuRectCoverRect(&rect, &g_pDMGdata->transfData.nuShowMapSize) )
					{
						continue;
					}
					nNowCount = 0;
					if( nClass != pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType )
					{
						nClass = pDwRoadBlk->pDwRoadCommon[ pDwRoadBlk->pDwRoadData[k].nRoadCommonIdx ].nRoadType;
						nDrawObj = g_pDMLibMT->MT_SelectObject( DRAW_TYPE_ZOOMFAKE_LINE, 
															g_pDMGdata->uiSetData.b3DMode, 
															&g_pDMMapCfg->scaleSetting.pRoadSetting[nClass], 
															NULL);
					}
					for( nuWORD l = 0; l < pDwRoadBlk->pDwRoadData[k].nVertexCount; l ++ )
					{
						ptNew.x = rtBlock.left;
						ptNew.y = rtBlock.top;
						ptNew.x += pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].x;
						ptNew.y += pDwRoadBlk->pSPointCoor[ pDwRoadBlk->pDwRoadData[k].nVertexAddrIdx + l ].y;
						if( l == 0 || l == pDwRoadBlk->pDwRoadData[k].nVertexCount -1 )
						{
							g_pDMLibMT->MT_MapToBmp(ptNew.x, ptNew.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
							nNowCount ++;
						}
						else
						{
							dx = ptNew.x - ptOld.x;
							dy = ptNew.y - ptOld.y;
							if( NURO_ABS(dx) >= nSkipLen || NURO_ABS(dy) >= nSkipLen )
							{
								g_pDMLibMT->MT_MapToBmp(ptNew.x, ptNew.y, &ptList[nNowCount].x, &ptList[nNowCount].y);
								nNowCount ++;
							}
						}
						ptOld = ptNew;
					}
					g_pDMLibMT->MT_DrawObject(nDrawObj, ptList, nNowCount, 0);
				}
			}
		}
	}
	return nuTRUE;
}
