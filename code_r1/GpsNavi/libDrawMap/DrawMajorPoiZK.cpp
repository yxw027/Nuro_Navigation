// DrawMajorPoiZK.cpp: implementation of the CDrawMajorPoiZK class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawMajorPoiZK.h"
#include "libDrawMap.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _MAJOR_POI_FILE					nuTEXT("Map\\Major\\major.poi")
#define _MAJOR_POI_MAX_SHOW_NUM				20
#define _MAJOR_POI_BMP_D				nuTEXT("Media\\Pic\\mpoi_d.bmp")
#define _MAJOR_POI_BMP_N				nuTEXT("Media\\Pic\\mpoi_n.bmp")

CDrawMajorPoiZK::CDrawMajorPoiZK()
{
	m_nMemIdx = -1;
	m_bmpIcon.bmpHasLoad = 0;
}

CDrawMajorPoiZK::~CDrawMajorPoiZK()
{
	Free();
}

nuBOOL CDrawMajorPoiZK::Initialize()
{
	m_bmpIcon.bmpHasLoad = 0;
	m_nMemIdx = -1;
	m_nDayNight = !m_pFsApi->pUserCfg->bNightDay;
	LoadIconBmp(m_pFsApi->pUserCfg->bNightDay);
	return nuTRUE;
}

nuVOID CDrawMajorPoiZK::Free()
{
	FreeMajorPoi();
	if( m_bmpIcon.bmpHasLoad )
	{
		g_pDMLibGDI->GdiDelNuroBMP(&m_bmpIcon);
		m_bmpIcon.bmpHasLoad = 0;
	}
}

nuVOID CDrawMajorPoiZK::FreeMajorPoi()
{
	m_pMmApi->MM_RelDataMemMass(&m_nMemIdx);
}

nuVOID CDrawMajorPoiZK::LoadIconBmp(nuBYTE nDayNight)
{
	if( m_nDayNight != nDayNight )
	{
		m_nDayNight = nDayNight;
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		if( !m_pFsApi->FS_GetFilePath(FILE_NAME_PATH, tsFile, NURO_MAX_PATH) )
		{
			return;
		}
		if( m_nDayNight )
		{
			nuTcscat(tsFile, _MAJOR_POI_BMP_N);
		}
		else
		{
			nuTcscat(tsFile, _MAJOR_POI_BMP_D);
		}
		if( m_bmpIcon.bmpHasLoad )
		{
			g_pDMLibGDI->GdiDelNuroBMP(&m_bmpIcon);
			m_bmpIcon.bmpHasLoad = 0;
		}
		if( g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpIcon, tsFile) )
		{
			m_bmpIcon.bmpType = NURO_BMP_TYPE_USE_TRANSCOLOR;	
		}	
	}
}

nuBOOL CDrawMajorPoiZK::ReadMajorPoi()
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
	MAJOR_POI_HEADER header = {0};
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&header, sizeof(MAJOR_POI_HEADER), 1, pFile) != 1 )
	{
		nuFclose(pFile);
		pFile=NULL;
		return nuFALSE;
	}
	nuDWORD nSize = sizeof(MAJOR_POI_HEADER) + sizeof(MAJOR_POI_NODE) * header.nPoiCount;
	pData = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(nSize, &m_nMemIdx);
	if( pData == NULL )
	{
		nuFclose(pFile);
		pFile=NULL;
		return nuFALSE;
	}
	header.nNameAddrSt = nSize;
	nuMemcpy(pData, &header, sizeof(MAJOR_POI_HEADER));
	pData += sizeof(MAJOR_POI_HEADER);
	nSize -= sizeof(MAJOR_POI_HEADER);
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

nuBOOL CDrawMajorPoiZK::ColMajorPoi(nuPVOID lpVoid)
{
	nuBYTE* pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_nMemIdx);
	if( pData == NULL || 
		g_pDMLibFS->pMapCfg->scaleSetting.scaleTitle.nScale < 1000 )
	{
		return nuFALSE;
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
	nuWCHAR wsName[128] = {0};
	nuroRECT rtList[_MAJOR_POI_MAX_SHOW_NUM];
	nuINT nNowCount = 0;
	nuroRECT rect = {0};
	LoadIconBmp(m_pFsApi->pUserCfg->bNightDay);
	////////////////////////////////////////////////////////////////
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	PMAJOR_POI_HEADER pHeader = (PMAJOR_POI_HEADER)pData;
	PMAJOR_POI_NODE	 pPoiList = (PMAJOR_POI_NODE)(pData + sizeof(MAJOR_POI_HEADER));
	nuroPOINT ptScreen = {0};
	nuroRECT rtScreen = {0}; //, rtText;
	rtScreen.left	= g_pDMGdata->transfData.nBmpLTx;
	rtScreen.top	= g_pDMGdata->transfData.nBmpLTy;
	rtScreen.right	= rtScreen.left + g_pDMGdata->transfData.nBmpWidth;
	rtScreen.bottom	= rtScreen.top + g_pDMGdata->transfData.nBmpHeight;
	nuBYTE nNowClass = -1;
	nuLONG nInOut = 0;
	nuLONG nNameAddress = pHeader->nNameAddrSt;
	for( nuDWORD i = 0; i < pHeader->nPoiCount; nNameAddress += pPoiList[i].nNameLen, ++i )
	{
		if( !g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowName &&
			!g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowIcon )
		{
			continue;
		}
		if( !nuPtInRect(pPoiList[i].point, g_pDMGdata->transfData.nuShowMapSize) )
		{
			continue;
		}
		g_pDMLibMT->MT_MapToBmp( pPoiList[i].point.x,
								 pPoiList[i].point.y,
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
		if( nuFseek(pFile, nNameAddress, NURO_SEEK_SET) != 0 ||
			nuFread(wsName, pPoiList[i].nNameLen, 1, pFile) != 1 )
		{
			continue;
		}
		wsName[pPoiList[i].nNameLen/2] = NULL;
		if( nNowClass != pPoiList[i].nClass )
		{
			nNowClass = pPoiList[i].nClass;
			if( g_pDMLibMT->MT_SelectObject( DRAW_TYPE_POI_FONT, 
											 g_pDMGdata->uiSetData.b3DMode, 
											 &g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[nNowClass], 
											 &nInOut ) != DRAW_OBJ_POINAME )
			{
				continue;
			}
		}
		nuroRECT rtText = { 0, 0, 10, 10 };
		if( g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowName )
		{
			g_pDMLibGDI->GdiDrawTextW(wsName, pPoiList[i].nNameLen/2, &rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE);
		}
		rect.left	= ptScreen.x - (rtText.right - rtText.left) / 2 + 4;
		rect.right	= rtText.right - rtText.left + rect.left;
		rect.top	= ptScreen.y - (rtText.bottom - rtText.top) - 3;
		rect.bottom	= ptScreen.y - 3;
		if( PoiCover(rtList, nNowCount, &rect) )
		{
			if( m_bmpIcon.bmpHasLoad )
			{
				rect.top	+= m_bmpIcon.bmpWidth*2;
				rect.bottom	+= m_bmpIcon.bmpWidth*2;
			}
			else
			{
				rect.top	+= 12;
				rect.bottom	+= 12;
			}
			
			if( PoiCover(rtList, nNowCount, &rect) )
			{
				continue;
			}
		}
		rtList[nNowCount++] = rect;
		if( m_bmpIcon.bmpHasLoad )
		{
			if( g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowIcon )
			{
				g_pDMLibGDI->GdiDrawBMP( ptScreen.x - m_bmpIcon.bmpWidth / 2, 
					                     ptScreen.y - m_bmpIcon.bmpWidth / 2, 
					                     m_bmpIcon.bmpWidth, 
					                     m_bmpIcon.bmpWidth, 
					                     &m_bmpIcon, 
					                     0, 
					                     m_bmpIcon.bmpWidth * pPoiList[i].nClass );
			}
		}
		else
		{
					if( g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowIcon )
			{
			g_pDMLibGDI->GdiEllipse( ptScreen.x,
									 ptScreen.y,
									 ptScreen.x + 4,
									 ptScreen.y + 4 );
			}
		}
		if( g_pDMLibFS->pMapCfg->scaleSetting.pPoiSetting[pPoiList[i].nClass].nShowName )
		{
			g_pDMLibGDI->GdiExtTextOutW( rect.left,
				                         rect.top,
				                         wsName, 
				                         pPoiList[i].nNameLen/2 );
		}
		
		if( _MAJOR_POI_MAX_SHOW_NUM == nNowCount )
		{
			break;
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	nuFclose(pFile);
	pFile=NULL;
	return nuTRUE;
}

inline nuBOOL CDrawMajorPoiZK::PoiCover(nuroRECT* ptRect, nuINT& nNowCount, nuroRECT* pRect)
{
	for( nuINT i = 0; i < nNowCount; ++i )
	{
		if( nuRectCoverRect(&ptRect[i], pRect) )
		{
			return nuTRUE;
		}
	}
	return nuFALSE;
}
