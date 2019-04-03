// MapConfig.cpp: implementation of the CMapConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "MapConfig.h"
#include "NuroModuleApiStruct.h"
#include "NuroConstDefined.h"
#include "libFileSystem.h"

extern PMEMMGRAPI	g_pFsMmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapConfig::CMapConfig()
{
	m_pMapConfig	= NULL;
}

CMapConfig::~CMapConfig()
{
	Free();
}

nuBOOL CMapConfig::Initialize(nuBYTE bNightDay, nuBYTE nScaleIndx /* = 0 */)
{
	m_pMapConfig = (PMAPCONFIG)g_pFsMmApi->MM_GetStaticMemMass(sizeof(MAPCONFIG));
	if( m_pMapConfig == NULL )
	{
		return nuFALSE;
	}
	if( !ReadMapSetting(nScaleIndx, bNightDay, nuTRUE) )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CMapConfig::Free()
{
	m_pMapConfig = NULL;
}

nuBOOL CMapConfig::ReadMapSetting(nuBYTE nScaleIdx, nuBYTE bNightDay, nuBOOL bReadAll /*= nuFALSE*/)
{
	if( m_pMapConfig == NULL )
	{
		return nuFALSE;
	}
	if( !bReadAll && nScaleIdx == m_nScaleIdx && bNightDay == m_bNightDay )
	{
		return nuTRUE;
	}
	nuTCHAR tsFile[NURO_MAX_FILE_PATH] = {0};
	if( bNightDay )
	{
		if( !LibFS_GetFilePath(FILE_NAME_MAP_CFG_NIGHT, tsFile, NURO_MAX_FILE_PATH) )
		{
			return nuFALSE;
		}
	}
	else
	{
		if( !LibFS_GetFilePath(FILE_NAME_MAP_CFG_DAY, tsFile, NURO_MAX_FILE_PATH) )
		{
			return nuFALSE;
		}
	}
	COMMONSETSTRU	stCommonSet = {0};
	NAVISTRU		stNavi = {0};
	SCALEHEAD		stScaleHead = {0};
	POISETSTRU		stPoiSet = {0};
	LINESETSTRU		stLineSet = {0};
	AREASETSTRU		stAreaSet = {0};
	nuLONG nSize = 0;
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	if( bReadAll )
	{
		if( nuFseek(pFile, 0, NURO_SEEK_SET) )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread(&stCommonSet, sizeof(COMMONSETSTRU), 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuMemcpy(&m_pMapConfig->commonSetting, &stCommonSet, sizeof(COMMONSETSTRU));
		m_pMapConfig->pScaleList					= (nuLONG*)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nScaleLayerCount*sizeof(nuLONG));
		m_pMapConfig->zoomSetting.pRoadSetting		= (PROADSETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nRoadTypeCount*sizeof(ROADSETTING));
		m_pMapConfig->zoomSetting.pPoiSetting		= (PPOISETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nPOIType1Count*sizeof(POISETTING));
		m_pMapConfig->zoomSetting.pBgLineSetting	= (PBGLINESETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nBgLineTypeCount*sizeof(BGLINESETTING));
		m_pMapConfig->zoomSetting.pBgAreaSetting	= (PBGAREASETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nBgAreaTypeCount*sizeof(BGAREASETTING));
		m_pMapConfig->scaleSetting.pRoadSetting		= (PROADSETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nRoadTypeCount*sizeof(ROADSETTING));
		m_pMapConfig->scaleSetting.pPoiSetting		= (PPOISETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nPOIType1Count*sizeof(POISETTING));
		m_pMapConfig->scaleSetting.pBgLineSetting	= (PBGLINESETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nBgLineTypeCount*sizeof(BGLINESETTING));
		m_pMapConfig->scaleSetting.pBgAreaSetting	= (PBGAREASETTING)g_pFsMmApi->MM_GetStaticMemMass(stCommonSet.nBgAreaTypeCount*sizeof(BGAREASETTING));
		if( m_pMapConfig->zoomSetting.pRoadSetting		== NULL	||
			m_pMapConfig->zoomSetting.pPoiSetting		== NULL	||
			m_pMapConfig->zoomSetting.pBgLineSetting	== NULL	||
			m_pMapConfig->zoomSetting.pBgAreaSetting	== NULL	|| 
			m_pMapConfig->scaleSetting.pRoadSetting		== NULL	||
			m_pMapConfig->scaleSetting.pPoiSetting		== NULL	||
			m_pMapConfig->scaleSetting.pBgLineSetting	== NULL	||
			m_pMapConfig->scaleSetting.pBgAreaSetting	== NULL )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		//Read all scale value...
		for(nuLONG i = 0; i < stCommonSet.nScaleLayerCount; ++i)
		{
			nSize = sizeof(COMMONSETSTRU) + 2*sizeof(NAVISTRU) + i *
				( sizeof(SCALEHEAD) + sizeof(POISETSTRU) * m_pMapConfig->commonSetting.nPOIType1Count +
				sizeof(LINESETSTRU) * (m_pMapConfig->commonSetting.nRoadTypeCount + m_pMapConfig->commonSetting.nBgLineTypeCount) +
				sizeof(AREASETSTRU) * m_pMapConfig->commonSetting.nBgAreaTypeCount );
			if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			if( nuFread(&stScaleHead, sizeof(SCALEHEAD), 1, pFile) != 1 )//Read scale head
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			m_pMapConfig->pScaleList[i] = stScaleHead.nScale;
		}
	}
	if( bReadAll || m_bNightDay != bNightDay )
	{
		//Read route line setting
		nSize = sizeof(COMMONSETSTRU);
		if( bNightDay )
		{
			nSize += sizeof(NAVISTRU);
		}
		if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread(&stNavi, sizeof(NAVISTRU), 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuMemcpy(&m_pMapConfig->naviLineSetting, &stNavi, sizeof(NAVISTRU));
		//Read Zoom Scale setting
		nSize =sizeof(COMMONSETSTRU) + 2*sizeof(NAVISTRU);
		if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread(&stScaleHead, sizeof(SCALEHEAD), 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuMemcpy(&m_pMapConfig->zoomSetting.scaleTitle, &stScaleHead, sizeof(SCALEHEAD));
		nuBYTE i;
		for(i = 0; i < m_pMapConfig->commonSetting.nPOIType1Count; i++)
		{
			if( nuFread(&stPoiSet, sizeof(POISETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->zoomSetting.pPoiSetting[i], &stPoiSet, sizeof(POISETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nRoadTypeCount; i++)
		{
			if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->zoomSetting.pRoadSetting[i], &stLineSet, sizeof(LINESETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nBgLineTypeCount; i++)
		{
			if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->zoomSetting.pBgLineSetting[i], &stLineSet, sizeof(LINESETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nBgAreaTypeCount; i++)
		{
			if( nuFread(&stAreaSet, sizeof(AREASETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->zoomSetting.pBgAreaSetting[i], &stAreaSet, sizeof(AREASETSTRU));
		}
	}
	if( bReadAll && nScaleIdx == 0)
	{
		nScaleIdx = m_pMapConfig->commonSetting.nStartScale;
	}
	if( bReadAll || m_bNightDay != bNightDay || nScaleIdx != m_nScaleIdx )
	{
		//Read Scale Setting
		nSize = sizeof(COMMONSETSTRU) + 2*sizeof(NAVISTRU) + nScaleIdx *
			( sizeof(SCALEHEAD) + sizeof(POISETSTRU) * m_pMapConfig->commonSetting.nPOIType1Count +
			  sizeof(LINESETSTRU) * (m_pMapConfig->commonSetting.nRoadTypeCount + m_pMapConfig->commonSetting.nBgLineTypeCount) +
			  sizeof(AREASETSTRU) * m_pMapConfig->commonSetting.nBgAreaTypeCount );
		if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread(&stScaleHead, sizeof(SCALEHEAD), 1, pFile) != 1 )//Read scale head
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuMemcpy(&m_pMapConfig->scaleSetting.scaleTitle, &stScaleHead, sizeof(SCALEHEAD));
		nuBYTE i;
		for( i = 0; i < m_pMapConfig->commonSetting.nPOIType1Count; i++)//Read poi setting
		{
			if( nuFread(&stPoiSet, sizeof(POISETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->scaleSetting.pPoiSetting[i], &stPoiSet, sizeof(POISETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nRoadTypeCount; i++)//Read road setting
		{
			if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->scaleSetting.pRoadSetting[i], &stLineSet, sizeof(LINESETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nBgLineTypeCount; i++)//Read bgline setting
		{
			if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->scaleSetting.pBgLineSetting[i], &stLineSet, sizeof(LINESETSTRU));
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nBgAreaTypeCount; i++)//Read bgarea setting
		{
			if( nuFread(&stAreaSet, sizeof(AREASETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			nuMemcpy(&m_pMapConfig->scaleSetting.pBgAreaSetting[i], &stAreaSet, sizeof(AREASETSTRU));
		}
		nuFclose(pFile);
		if( bNightDay )
		{
			if( !LibFS_GetFilePath(FILE_NAME_MAP_CFG_NIGHT2, tsFile, NURO_MAX_FILE_PATH) )
			{
				return nuFALSE;
			}
		}
		else
		{
			if( !LibFS_GetFilePath(FILE_NAME_MAP_CFG_DAY2, tsFile, NURO_MAX_FILE_PATH) )
			{
				return nuFALSE;
			}
		}
		//Load the zoomscale addtional setting
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		nSize = sizeof(COMMONSETSTRU) + 2*sizeof(NAVISTRU) + sizeof(SCALEHEAD) + 
			sizeof(POISETSTRU)*m_pMapConfig->commonSetting.nPOIType1Count;
		if( bReadAll || m_bNightDay != bNightDay )
		{
			if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			for(i = 0; i < m_pMapConfig->commonSetting.nRoadTypeCount; i++)
			{
				if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
				{
					nuFclose(pFile);
					return nuFALSE;
				}
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerColorR	= stLineSet.nFontBgColorR;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerColorG	= stLineSet.nFontBgColorG;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerColorB	= stLineSet.nFontBgColorB;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerRimColorR = stLineSet.nFontColorR;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerRimColorG = stLineSet.nFontColorG;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerRimColorB = stLineSet.nFontColorB;
				m_pMapConfig->zoomSetting.pRoadSetting[i].nBannerRimWidth  = stLineSet.nReserve;
			}
		}
		//Load scale addtional setting
		nSize += nScaleIdx*(sizeof(SCALEHEAD) + sizeof(POISETSTRU) * m_pMapConfig->commonSetting.nPOIType1Count +
			 sizeof(LINESETSTRU) * (m_pMapConfig->commonSetting.nRoadTypeCount + m_pMapConfig->commonSetting.nBgLineTypeCount) +
			 sizeof(AREASETSTRU) * m_pMapConfig->commonSetting.nBgAreaTypeCount);
		if( nuFseek(pFile, nSize, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		for(i = 0; i < m_pMapConfig->commonSetting.nRoadTypeCount; i++)
		{
			if( nuFread(&stLineSet, sizeof(LINESETSTRU), 1, pFile) != 1 )
			{
				nuFclose(pFile);
				return nuFALSE;
			}
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerColorR	= stLineSet.nFontBgColorR;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerColorG	= stLineSet.nFontBgColorG;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerColorB	= stLineSet.nFontBgColorB;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerRimColorR = stLineSet.nFontColorR;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerRimColorG = stLineSet.nFontColorG;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerRimColorB = stLineSet.nFontColorB;
			m_pMapConfig->scaleSetting.pRoadSetting[i].nBannerRimWidth  = stLineSet.nReserve;
		}
	}
	nuFclose(pFile);
	m_nScaleIdx = nScaleIdx;
	m_bNightDay	= bNightDay;

	return nuTRUE;
}