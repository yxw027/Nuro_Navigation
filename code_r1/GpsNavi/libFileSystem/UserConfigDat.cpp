// UserConfigDat.cpp: implementation of the CUserConfigDat class.
//
//////////////////////////////////////////////////////////////////////

#include "UserConfigDat.h"
#include "NuroConstDefined.h"
#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserConfigDat::CUserConfigDat()
{
	nuMemset(&m_userCfg, 0, sizeof(USERCONFIG));
}

CUserConfigDat::~CUserConfigDat()
{

}

nuBOOL CUserConfigDat::Initialize()
{
	nuTCHAR tsFile[NURO_MAX_FILE_PATH] = {0};
	if( !LibFS_GetFilePath(FILE_NAME_USER_CFG_TMP, tsFile, NURO_MAX_FILE_PATH) )
	{
		return nuFALSE;
	}
	if( !ReadUserCfgDat(tsFile) )
	{
		if( !LibFS_GetFilePath(FILE_NAME_USER_CFG, tsFile, NURO_MAX_FILE_PATH) )
		{
			return nuFALSE;
		}
		if( !ReadUserCfgDat(tsFile) )
		{
			//SaveUserCfgDat(tsFile);
			return nuFALSE;
		}
	}
	//Now threse are limited to zero.
	m_userCfg.nScreenLTx	= 0;
	m_userCfg.nScreenLTy	= 0;
	return nuTRUE;
}

nuVOID CUserConfigDat::Free()
{

}

nuBOOL CUserConfigDat::ReadUserCfgDat(nuTCHAR *tsFile)
{
	if( tsFile == NULL )
	{
		return nuFALSE;
	}
__android_log_print(ANDROID_LOG_INFO, "navi", "tsFile %s", tsFile);
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	nuBOOL bRes = nuFALSE;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 &&
		nuFread(&m_userCfg, sizeof(USERCONFIG), 1, pFile) == 1 )
	{
		bRes = nuTRUE;
	}
	nuFclose(pFile);
/*	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bNightDay %d", m_userCfg.bNightDay);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg b3DMode %d", m_userCfg.b3DMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nFixOnRoad %d", m_userCfg.nFixOnRoad);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bCCDVoice %d", m_userCfg.bCCDVoice);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bMoveWaiting %d", m_userCfg.bMoveWaiting);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMoveWaitTotalTime %d", m_userCfg.nMoveWaitTotalTime);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScreenType %d", m_userCfg.nScreenType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nLanguage %d", m_userCfg.nLanguage);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScreenLTx %d", m_userCfg.nScreenLTx);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScreenLTy %d", m_userCfg.nScreenLTy);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScreenWidth %d", m_userCfg.nScreenWidth);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScreenHeight %d", m_userCfg.nScreenHeight);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nRouteListNum %d", m_userCfg.nRouteListNum);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nWaveTTSType %d", m_userCfg.nWaveTTSType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMapDirectMode %d", m_userCfg.nMapDirectMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScaleShowMode %d", m_userCfg.nScaleShowMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bShowLongLat %d", m_userCfg.bShowLongLat);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nZoomAutoMode %d", m_userCfg.nZoomAutoMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bPlayCCD %d", m_userCfg.bPlayCCD);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nPlaySpeedWarning %d", m_userCfg.nPlaySpeedWarning);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSimSpeed %d", m_userCfg.nSimSpeed);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bAutoDayNight %d", m_userCfg.bAutoDayNight);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nTimeToNight %d", m_userCfg.nTimeToNight);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nTimeToDay %d", m_userCfg.nTimeToDay);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nCarIconType %d", m_userCfg.nCarIconType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nDayBgPic %d", m_userCfg.nDayBgPic);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nNightBgPic %d", m_userCfg.nNightBgPic);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nShowPOI %d", m_userCfg.nShowPOI);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nRouteMode %d", m_userCfg.nRouteMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSplitScreenMode %d", m_userCfg.nSplitScreenMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nNowScreenType %d", m_userCfg.nNowScreenType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMoveType %d", m_userCfg.nMoveType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bLastNavigation %d", m_userCfg.bLastNavigation);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nPastMarket %d", m_userCfg.nPastMarket);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nZoomBmpWidth %d", m_userCfg.nZoomBmpWidth);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nZoomBmpHeight %d", m_userCfg.nZoomBmpHeight);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nVoiceControl %d", m_userCfg.nVoiceControl);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMemoVoice %d", m_userCfg.nMemoVoice);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSimuMode %d", m_userCfg.nSimuMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nLastScaleIndex %d", m_userCfg.nLastScaleIndex);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nLastAutoMoveType %d", m_userCfg.nLastAutoMoveType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nVoiceIconTimer %d", m_userCfg.nVoiceIconTimer);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bCrossAutoZoom %d", m_userCfg.bCrossAutoZoom);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bBigMapRoadFont %d", m_userCfg.bBigMapRoadFont);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bMoveMap %d", m_userCfg.bMoveMap);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg reserve %d", m_userCfg.reserve[0]);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nOneKeyCallVoice %d", m_userCfg.nOneKeyCallVoice);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nTMCSwitchFlag %d", m_userCfg.nTMCSwitchFlag);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nTMCFrequency %d", m_userCfg.nTMCFrequency);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nShowStartWarn %d", m_userCfg.nShowStartWarn);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nBoardCast %d", m_userCfg.nBoardCast);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nRefreshRate %d", m_userCfg.nRefreshRate);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nShowRoad %d", m_userCfg.nShowRoad);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nOneKeyTrack %d", m_userCfg.nOneKeyTrack);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nOneKeyNavi %d", m_userCfg.nOneKeyNavi);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nKeySoundSwitch %d", m_userCfg.nKeySoundSwitch);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSpNumInput %d", m_userCfg.nSpNumInput);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nKeyWordInput %d", m_userCfg.nKeyWordInput);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSpeedWarningPic %d", m_userCfg.nSpeedWarningPic);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nTouchView %d", m_userCfg.nTouchView);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nUIType %d", m_userCfg.nUIType);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nUILanguage %d", m_userCfg.nUILanguage);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nNaviTimeMode %d", m_userCfg.nNaviTimeMode);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bFriedSate %d", m_userCfg.bFriedSate);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nFriedFreq %d", m_userCfg.nFriedFreq);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bWheather %d", m_userCfg.bWheather);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bLogTraject %d", m_userCfg.bLogTraject);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nDYM %d", m_userCfg.nDYM);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMultiRouteMode1 %d", m_userCfg.nMultiRouteMode1);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMultiRouteMode2 %d", m_userCfg.nMultiRouteMode2);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nMultiRouteMode3 %d", m_userCfg.nMultiRouteMode3);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bHidenMapRoadInfo %d", m_userCfg.bHidenMapRoadInfo);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSpeedWarningVoice %d", m_userCfg.nSpeedWarningVoice);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nCenterTarget %d", m_userCfg.nCenterTarget);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nRouteOption %d", m_userCfg.nRouteOption);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nUiControl %d", m_userCfg.nUiControl);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nDefaultCityID %d", m_userCfg.nDefaultCityID);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nRoadInput %d", m_userCfg.nRoadInput);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nPoiInput %d", m_userCfg.nPoiInput);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bNightLight %d", m_userCfg.bNightLight);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bOperateVoice %d", m_userCfg.bOperateVoice);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bShowBigPic %d", m_userCfg.bShowBigPic);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg bTargetLine %d", m_userCfg.bTargetLine);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nScaleIdx %d", m_userCfg.nScaleIdx);
	__android_log_print(ANDROID_LOG_INFO, "uiconfig", "pUserCfg nSoundVolume %d", m_userCfg.nSoundVolume);*/
#ifdef VALUE_EMGRT
	m_userCfg.nSimuMode = 0;
	m_userCfg.bCrossAutoZoom = 1;
#endif
	return bRes;
}

nuBOOL CUserConfigDat::SaveUserCfgDat(nuTCHAR *tsFile)
{
	if( tsFile == NULL )
	{
		return nuFALSE;
	}
	nuFILE* pFile = nuTfopen(tsFile, NURO_FS_OB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	nuBOOL bRes = nuFALSE;
	if( nuFwrite(&m_userCfg, sizeof(USERCONFIG), 1, pFile) == 1 )
	{
		bRes = nuTRUE;
	}
	nuFclose(pFile);

	return bRes;
}

nuBOOL CUserConfigDat::CoverFromDefault()
{
	nuTCHAR tsFile[NURO_MAX_FILE_PATH] = {0};
	if( !LibFS_GetFilePath(FILE_NAME_USER_CFG, tsFile, NURO_MAX_FILE_PATH) )
	{
		return nuFALSE;
	}
	USERCONFIG userCfg = {0};
	userCfg = m_userCfg;
	if( !ReadUserCfgDat(tsFile) )
	{
		m_userCfg = userCfg;
		return nuFALSE;
	}
	m_userCfg.nScreenLTx = userCfg.nScreenLTx;
	m_userCfg.nScreenLTy = userCfg.nScreenLTy;
	m_userCfg.nScreenWidth	= userCfg.nScreenWidth;
	m_userCfg.nScreenHeight	= userCfg.nScreenHeight;
	if( !LibFS_GetFilePath(FILE_NAME_USER_CFG_TMP, tsFile, NURO_MAX_FILE_PATH) )
	{
		return nuFALSE;
	}
	if( !SaveUserCfgDat(tsFile) )
	{
		return nuFALSE;
	}
	//Now threse are limited to zero.
	return nuTRUE;
}
