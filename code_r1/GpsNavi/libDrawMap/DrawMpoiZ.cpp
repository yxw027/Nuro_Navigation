// DrawMpoiZ.cpp: implementation of the CDrawMpoiZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawMpoiZ.h"
#include "libDrawMap.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _MAJOR_POI_FILE					nuTEXT("Map\\Major\\Major.poi")

CDrawMpoiZ::CDrawMpoiZ()
{
	m_nMemIdx = -1;
}

CDrawMpoiZ::~CDrawMpoiZ()
{

}

nuBOOL CDrawMpoiZ::Initialize()
{
	m_nMemIdx = -1;
	return nuTRUE;
}

nuVOID CDrawMpoiZ::Free()
{
	
}

nuBOOL CDrawMpoiZ::ReadMpoiData()
{
	nuBYTE *pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMemIdx);
	if( pData )
	{
		return nuTRUE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	if( !m_pFsApi->FS_GetFilePath(FILE_NAME_PATH, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	nuTcscat(tsFile, _MAJOR_POI_FILE);
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	nuDWORD nTotalCount = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&nTotalCount, 4, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		pFile=NULL;
		return nuFALSE;
	}
	nuDWORD nSize = nTotalCount * sizeof(MPOINODE) + 4;
	pData = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(nSize, &m_nMemIdx);
	if( pData == NULL )
	{
		nuFclose(pFile);
		pFile=NULL;
		return nuFALSE;
	}
	nuMemcpy(pData, &nTotalCount, 4);
	pData += 4;
	nSize -= 4;
	if( nuFread(pData, nSize, 1, pFile) != 1 )
	{
		m_pMmApi->MM_RelDataMemMass(&m_nMemIdx);
		nuFclose(pFile);
		pFile=NULL;
		return nuFALSE;
	}
	nuFclose(pFile);
	pFile=NULL;
	return nuTRUE;
}

nuBOOL CDrawMpoiZ::ColMpoiData(nuPVOID pVoid)
{
	PMPOIFILEDATA pMpoiData = (PMPOIFILEDATA)m_pMmApi->MM_GetDataMassAddr(m_nMemIdx);
	if( pMpoiData == NULL || 
		g_pDMLibFS->pMapCfg->scaleSetting.scaleTitle.nScale < 1000 )
	{
		return nuFALSE;
	}
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	pMpoiData->pMPOIList = (PMPOINODE)((nuDWORD)pMpoiData + 4);
	nuroPOINT ptScreen = {0};
	nuroRECT rtScreen = {0};//, rtText;
	rtScreen.left	= g_pDMGdata->transfData.nBmpLTx;
	rtScreen.top	= g_pDMGdata->transfData.nBmpLTy;
	rtScreen.right	= rtScreen.left + g_pDMGdata->transfData.nBmpWidth;
	rtScreen.bottom	= rtScreen.top + g_pDMGdata->transfData.nBmpHeight;
	nuBYTE nNowClass = -1;
	nuLONG nInOut = 0;
	for( nuDWORD i = 0; i < pMpoiData->nPOICount; ++i )
	{
		//test
		/*
		if( nNowClass != pMpoiData->pMPOIList[i].nClass )
		{
			nNowClass = pMpoiData->pMPOIList[i].nClass;
		}
		*/
		if( !g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pMpoiData->pMPOIList[i].nClass].nShowName )
		{
			continue;
		}
		
		if( !nuPtInRect(pMpoiData->pMPOIList[i].point, g_pDMGdata->transfData.nuShowMapSize) )
		{
			continue;
		}
		g_pDMLibMT->MT_MapToBmp(pMpoiData->pMPOIList[i].point.x,
			pMpoiData->pMPOIList[i].point.y,
			&ptScreen.x,
			&ptScreen.y );
		if( !nuPtInRect(ptScreen, rtScreen) )
		{
			continue;
		}
		if( g_pDMGdata->uiSetData.b3DMode )
		{
			g_pDMLibMT->MT_Bmp2Dto3D(ptScreen.x, ptScreen.y, MAP_DEFAULT);
		}
		if( nNowClass != pMpoiData->pMPOIList[i].nClass )
		{
			nNowClass = pMpoiData->pMPOIList[i].nClass;
			if( g_pDMLibMT->MT_SelectObject( DRAW_TYPE_POI_FONT, 
				g_pDMGdata->uiSetData.b3DMode, 
				&g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[nNowClass], 
				&nInOut ) != DRAW_OBJ_POINAME )
			{
				continue;
			}
		}
		g_pDMLibGDI->GdiEllipse( ptScreen.x,
								 ptScreen.y,
								 ptScreen.x + 4,
								 ptScreen.y + 4 );
		g_pDMLibGDI->GdiExtTextOutW( ptScreen.x - 20,
									 ptScreen.y - 20,
									 pMpoiData->pMPOIList[i].nName, 
									 4 );
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}
