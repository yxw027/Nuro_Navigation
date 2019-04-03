// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: SoundThread.cpp,v 1.25 2010/10/26 07:27:17 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/10/26 07:27:17 $
// $Locker:  $
// $Revision: 1.25 $
// $Source: /home/nuCode/libNaviCtrl/SoundThread.cpp,v $
////////////////////////////////////////////////////////////////////////

#include "SoundThread.h"
#include "NuroModuleApiStruct.h"
#include "nuTTSdefine.h"
#include "nuTTSInterFace.h"
#include "NuroClasses.h"

#ifndef ANDROID
#include "CyberonTTSCtrl.h"
#endif

#ifdef NURO_OS_ECOS
#define _SOUND_EX				nuTEXT(".mp3")
#else
#define _SOUND_EX				nuTEXT(".wav")
#endif

nuWCHAR wsNumber[14]={0x96F6,19968,20108,19977,22235,20116,20845,19971,20843,20061,21313,0x767E, 0x9EDE};

//#define _USE_POST_VOICE_CTR
#if (defined(_WIN32_WCE) && defined(_USE_POST_VOICE_CTR) )
#include <windows.h>
#define WM_NAVI_VOL   (WM_USER+142)
#define VOL_START    0x00000000
#define VOL_END    0x00010001

#define _SHELL_WINDOW_NAME		TEXT("SHELL")
HWND	g_hWndNaviZk = NULL;
#endif

extern FILESYSAPI	g_fileSysApi;
#ifdef EJTTS
	#define _PLAY_VAVE(sound, pFolder) nuTcscpy(tsWaveFile, tsSoundFile);\
		nuTcscat(tsWaveFile, pFolder);\
		nuItot(sound, tsValue, 10);\
		nuTcscat(tsWaveFile, tsValue);\
		nuTcscat(tsWaveFile, _SOUND_EX);\
		if(m_pfPlayWave != NULL)\
		m_pfPlayWave(tsWaveFile, NULL);

	#define _PLAY_TTS \
		if(m_pfPlayWave != NULL)\
		m_pfPlayWave(NULL, wsFullName);
#endif

#define _GET_TTS_TEXT(sound, pFileTxt) nuTcscpy(tsWaveFile, tsSoundFile);\
	nuTcscat(tsWaveFile, pFileTxt);\
	if( g_fileSysApi.FS_ReadWcsFromFile(tsWaveFile, sound, ws1, NAME_LINE_NUM_IN_FILE*sizeof(nuWCHAR)) > 0 ) \
	nuWcscat(wsFullName, ws1)

/*#ifdef NURO_OS_UCOS
#define _PLAY_VAVE(sound, pFolder) nuTcscpy(tsWaveFile, tsSoundFile);\
	nuTcscat(tsWaveFile, pFolder);\
	nuItot(sound, tsValue, 10);\
	nuTcscat(tsWaveFile, tsValue);\
	nuTcscat(tsWaveFile, _SOUND_EX);\
nuPlaySound(tsWaveFile, m_pSoundBuff, NURO_SND_SYNC)

#else
#ifndef ANDROID 
#define _PLAY_VAVE(sound, pFolder) nuTcscpy(tsWaveFile, tsSoundFile);\
	nuTcscat(tsWaveFile, pFolder);\
	nuItot(sound, tsValue, 10);\
	nuTcscat(tsWaveFile, tsValue);\
	nuTcscat(tsWaveFile, _SOUND_EX);\
	nuPlaySound(tsWaveFile, m_pSoundBuff, NURO_SND_SYNC)
#else
#define _PLAY_VAVE(sound, pFolder) nuTcscpy(tsWaveFile, tsSoundFile);\
	nuTcscat(tsWaveFile, pFolder);\
	nuItot(sound, tsValue, 10);\
	nuTcscat(tsWaveFile, tsValue);\
	nuTcscat(tsWaveFile, _SOUND_EX);\
	if(m_pfPlayWave != NULL)\
	m_pfPlayWave(tsWaveFile, NULL)//revised by daniel for NaviKing 20120111
#endif
#endif 

#ifdef NURO_USE_CYBERON_TTS
#ifndef ANDROID  
#define _PLAY_TTS if( !m_bQuit && m_bTTSOK){\
					Cyberon_TTS_PlaySound(wsFullName);\
				  }
#else
#define _PLAY_TTS \
	if(m_pfPlayWave != NULL)\
	m_pfPlayWave(NULL, wsFullName);
#endif
#endif

//#include "windows.h"
#ifdef NURO_USE_TTSMODULE
#ifndef ANDROID
#define _PLAY_TTS if( !m_bQuit && m_bTTSOK) nuTTSSession(wsFullName, (char*)m_pSoundBuff, NURO_MAX_WAVFILE_SIZE, NURO_SND_SYNC)
#else
#define _PLAY_TTS if( !m_bQuit && m_bTTSOK){\
	nuTTSSession(wsFullName, (nuCHAR*)m_pSoundBuff, NURO_MAX_WAVFILE_SIZE, NURO_SND_SYNC, tsPath, tsTTSFilePath);\
	if(m_pfPlayWave != NULL)\
		m_pfPlayWave(g_fileSysApi.pUserCfg->nSoundVolume,tsTTSFilePath);\
	}
#endif
#else
#ifdef NURO_USE_N19TTS
#define _PLAY_TTS	m_loadPPTTS.TTSTalkEx(wsFullName, NULL, 0);\
//MessageBoxW(NULL, wsFullName, L"", 0);
#else
//#include "windows.h"
#ifndef _PLAY_TTS
#define _PLAY_TTS //MessageBoxW(NULL, wsFullName, L"", 0);
#endif
#endif
#endif*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
nuDWORD CSoundThread::SThreadAction(nuPVOID lpVoid)
{
	class CSoundThread*	pThis = (class CSoundThread*)lpVoid;
	SOUNDDATA soundData= {0};
	pThis->m_bOutThreadLoop = nuFALSE;
	/* 20090423 zhikun
	pThis->m_bTTSOK	= nuFALSE;
#ifdef NURO_USE_TTSMODULE
	NUROTTSARG	nuroTTSarg;
	char		cnFilePath[NURO_MAX_PATH];
	char		dmFilePath[NURO_MAX_PATH];
	const nuTCHAR* pTsPath = g_fileSysApi.pGdata->pTsPath;
	int i = 0;
	while( pTsPath[i] != NULL )
	{
		cnFilePath[i] = dmFilePath[i] = (char)pTsPath[i];
		++i;
	}
	cnFilePath[i] = dmFilePath[i] = 0;
//	nuTcscpy(cnFilePath, g_fileSysApi.pGdata->pTsPath);
//	nuTcscpy(dmFilePath, g_fileSysApi.pGdata->pTsPath);
	nuStrcat(cnFilePath, "Media\\Data\\CNPackage.dat");
	nuStrcat(dmFilePath, "Media\\Data\\DMPackage.dat");
	nuroTTSarg.CNVoiceFile	= (char*)cnFilePath;
	nuroTTSarg.DMVoiceFile	= (char*)dmFilePath;
	nuroTTSarg.ENVoiceFile	= NULL;
	pThis->m_bTTSOK	= nuTTSLoad(&nuroTTSarg);
#endif*/
	/*
	if( pThis->m_bTTSOK )
	{
		pThis->m_pSoundBuff = new nuBYTE[NURO_MAX_WAVFILE_SIZE];
	}
	else
	{
#ifdef NURO_OS_UCOS
		pThis->m_pSoundBuff = new nuBYTE[NURO_MAX_WAVFILE_SIZE];
#endif
	}
	*/
	pThis->m_pSoundBuff = new nuBYTE[NURO_MAX_WAVFILE_SIZE];
#if (defined(_WIN32_WCE) && defined(_USE_POST_VOICE_CTR) )
	g_hWndNaviZk = ::FindWindow(NULL, _SHELL_WINDOW_NAME);
#endif

	while( !pThis->m_bQuit )
	{
        //
		if(g_fileSysApi.pGdata->bWaitQuit)
		{
			nuSleep(500);
			continue;
		}

		if( pThis->m_soundDataFirst.bNewVoice )
        {
            if( NULL != g_fileSysApi.pGdata->pfCallBack && !pThis->m_bCallBack )
            {
                CALL_BACK_PARAM cbParam;
                cbParam.nCallType = CALLBACK_TYPE_VOICESTART;
                g_fileSysApi.pGdata->pfCallBack(&cbParam);
                pThis->m_bCallBack = nuTRUE;
            }
			pThis->m_bIsPlaying	= nuTRUE;
			pThis->SoundAction( pThis->m_soundDataFirst.nVoiceType, 
								pThis->m_soundDataFirst.nLangType, 
								pThis->m_soundDataFirst.nSound1,
								pThis->m_soundDataFirst.nSound2,
								pThis->m_soundDataFirst.nSound3,
								pThis->m_soundDataFirst.nNameNum,
								pThis->m_soundDataFirst.tsNAME );
			pThis->m_soundDataFirst.bNewVoice = 0;
			nuMemset(&soundData, 0, sizeof(soundData));
			pThis->m_bIsPlaying	= nuFALSE;
		}

		if( !pThis->m_bQuit && pThis->PopSound(&soundData) )
		{
            if( NULL != g_fileSysApi.pGdata->pfCallBack && !pThis->m_bCallBack )
            {
				CALL_BACK_PARAM cbParam;
                cbParam.nCallType = CALLBACK_TYPE_VOICESTART;
                g_fileSysApi.pGdata->pfCallBack(&cbParam);
                pThis->m_bCallBack = nuTRUE;
            }
			if (NULL != g_fileSysApi.pGdata->pfCallBack &&
				10   == soundData.nSound1 )
			{  // UserCCD  // Modfied By Damon For UserCCD Show ICon 20110915 
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_CCDSTART;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			else if(soundData.nVoiceType == VOICE_TYPE_OTHERS && O_SOUNDNAVITTS == soundData.nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack )
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SPECCH_START;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}

			if (1  == g_fileSysApi.pUserCfg->bCCDVoice ||
				10 != soundData.nSound1 )
			{   // Modified by Damon For CCD Must popMsg
				pThis->m_bIsPlaying	= nuTRUE;
				pThis->SoundAction( soundData.nVoiceType, 
									soundData.nLangType, 
									soundData.nSound1,
									soundData.nSound2,
									soundData.nSound3,
									soundData.nNameNum,
									soundData.tsNAME );
			}

			if( NULL != g_fileSysApi.pGdata->pfCallBack &&
				10   == soundData.nSound1)
			{   // UserCCD  // Modfied By Damon For UserCCD Show ICon 20110915 
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_CCDEND;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			else if(soundData.nVoiceType == VOICE_TYPE_OTHERS && O_SOUNDNAVITTS == soundData.nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack && !pThis->m_bStopSound)
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SPECCH_END;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			pThis->m_bIsPlaying	= nuFALSE;
			nuMemset(&soundData, 0, sizeof(soundData));
			nuSleep(25);
		}
		while( !pThis->m_bQuit && pThis->Navi_PopSound(&soundData) )
		{
			if( NULL != g_fileSysApi.pGdata->pfCallBack && !pThis->m_bCallBack )
			{
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_VOICESTART;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
				pThis->m_bCallBack = nuTRUE;
			}
			if (NULL != g_fileSysApi.pGdata->pfCallBack &&
				10   == soundData.nSound1 )
			{  // UserCCD  // Modfied By Damon For UserCCD Show ICon 20110915 
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_CCDSTART;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			else if(soundData.nVoiceType == VOICE_TYPE_OTHERS && O_SOUNDNAVITTS == soundData.nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack )
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SPECCH_START;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			if (1  == g_fileSysApi.pUserCfg->bCCDVoice ||
				10 != soundData.nSound1 )
			{   // Modified by Damon For CCD Must popMsg
				pThis->m_bIsPlaying	= nuTRUE;
				pThis->SoundAction( soundData.nVoiceType, 
					soundData.nLangType, 
					soundData.nSound1,
					soundData.nSound2,
					soundData.nSound3,
					soundData.nNameNum,
					soundData.tsNAME );
			}
			if( NULL != g_fileSysApi.pGdata->pfCallBack &&
				10   == soundData.nSound1)
			{   // UserCCD  // Modfied By Damon For UserCCD Show ICon 20110915 
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_CCDEND;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			else if(soundData.nVoiceType == VOICE_TYPE_OTHERS && O_SOUNDNAVITTS == soundData.nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack && !pThis->m_bStopSound)
			{
				CALL_BACK_PARAM cbParam = {0};
				cbParam.nCallType = CALLBACK_TYPE_SPECCH_END;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}
			pThis->m_bIsPlaying	= nuFALSE;
			nuMemset(&soundData, 0, sizeof(soundData));
			nuSleep(25);
		}
		if( NULL != g_fileSysApi.pGdata->pfCallBack && pThis->m_bCallBack )
        {
            CALL_BACK_PARAM cbParam;
            cbParam.nCallType = CALLBACK_TYPE_VOICEEND;
            g_fileSysApi.pGdata->pfCallBack(&cbParam);
            pThis->m_bCallBack = nuFALSE;
        }
		if( !pThis->m_bQuit )
		{
//			DEBUGSTRING(nuTEXT("Sound Thread sleep"));
			nuSleep(100);
		}
	}
	/* 2009.04.23 zhikun
	if( pThis->m_bTTSOK )
	{
		nuTTSFree();
		pThis->m_bTTSOK = nuFALSE;
	}*/
	/*if( NULL != g_fileSysApi.pGdata->pfCallBack && pThis->m_bCallBack )
    {
        CALL_BACK_PARAM cbParam;
        cbParam.nCallType = CALLBACK_TYPE_VOICEEND;
        g_fileSysApi.pGdata->pfCallBack(&cbParam);
        pThis->m_bCallBack = nuFALSE;
    }*/
	pThis->m_bQuit = nuTRUE;
	pThis->m_bOutThreadLoop = nuTRUE;
	if( pThis->m_pSoundBuff != NULL )
	{
		delete []pThis->m_pSoundBuff;
		pThis->m_pSoundBuff = NULL;
	}
	nuThreadWaitReturn(pThis->m_hThread);
	pThis->m_hThread = NULL;
	return 0;
}

CSoundThread::CSoundThread()
{
	m_bIsPlaying	= nuFALSE;
	m_bIsWorking	= nuFALSE;
	m_bIsCleanAll   = nuFALSE;
	m_bQuit			= nuTRUE;
	#ifndef NURO_OS_ECOS
	m_dwThreadID	= 0;
	//m_hThread.nThdID = 0;
	#endif
	m_hThread		= NULL;
	//m_hThread.handle = NULL;
	
	m_bOutThreadLoop= nuTRUE;
	nuMemset(&m_soundDataFirst, 0, sizeof(SOUNDDATA));

	m_pSoundBuff	= NULL;
#ifdef NURO_OS_HV_UCOS
	nuMemset( &m_ReRouteSBuf ,0, sizeof(m_ReRouteSBuf) ) ;
	m_bSaveReRouteSBuf = nuFALSE;
	m_nReRouLen = 0;
#endif
}

CSoundThread::~CSoundThread()
{
	FreeSoundThread();
}

nuBOOL CSoundThread::InitSoundThread()
{
	#ifdef ANDROID
	m_pfPlayWave = NULL;
	#endif
	//
	m_bQuit		= nuFALSE;
	m_bStopSound = nuFALSE;
	nuMemset(m_pSoundList, 0, sizeof(m_pSoundList));
	nuMemset(m_pNavi_SoundList, 0, sizeof(m_pNavi_SoundList));
	m_nReadIdx = m_nWriteIdx = 0;
	m_nNavi_ReadIdx = m_nNavi_WriteIdx = 0;
	m_bNavi_IsWorking = m_bNavi_IsPlaying = nuFALSE;

//	m_nSoundCount	= 0;
    
    m_bCallBack = nuFALSE;
	
	m_bTTSOK	= nuFALSE;
#ifdef NURO_USE_CYBERON_TTS
#ifndef ANDROID 
	m_lSpeed = 120, m_lVolunm = 300, m_lPitch = 75;
	if (!nuReadConfigValue("TTS_C_VOLUME", &m_lVolunm))
	{
		m_lVolunm=300;
	}
	if (!nuReadConfigValue("TTS_C_PITCH", &m_lPitch))
	{
		m_lPitch=75;
	}
	if (!nuReadConfigValue("TTS_C_SPEED", &m_lSpeed))
	{
		m_lSpeed=120;
	}
	m_bTTSOK = Cyberon_TTS_Init(g_fileSysApi.pGdata->pTsPath, m_lVolunm, m_lSpeed, m_lPitch);
#endif
#endif
#ifdef NURO_USE_TTSMODULE
	NUROTTSARG	nuroTTSarg;
	char		cnFilePath[NURO_MAX_PATH];
	char		dmFilePath[NURO_MAX_PATH];
	/*
	const nuTCHAR* pTsPath = g_fileSysApi.pGdata->pTsPath;
	int i = 0;
	while( pTsPath[i] != NULL )
	{
		cnFilePath[i] = dmFilePath[i] = (char)pTsPath[i];
		++i;
	}
	cnFilePath[i] = dmFilePath[i] = 0;
	*/
#ifdef UNICODE
	nuTCHAR	tsFilePath[NURO_MAX_PATH];
	nuTcscpy(tsFilePath, g_fileSysApi.pGdata->pTsPath);
	#ifndef ANDROID
	nuTcscat(tsFilePath, nuTEXT("Media\\Data\\CNPackage.dat"));
	#else
	nuTcscat(tsFilePath, nuTEXT("Media\\Data_android\\CNPackage.dat"));
	#endif
//	nuMbsToWcs(cnFilePath, tsFilePath, nuTcslen(tsFilePath));
	nuWcsToMbs(tsFilePath, cnFilePath, sizeof(cnFilePath));
	
	nuTcscpy(tsFilePath, g_fileSysApi.pGdata->pTsPath);
	#ifndef ANDROID	
	nuTcscat(tsFilePath, nuTEXT("Media\\Data\\DMPackage.dat"));
	#else
	nuTcscat(tsFilePath, nuTEXT("Media\\Data_android\\DMPackage.dat"));
	#endif
//	nuMbsToWcs(dmFilePath, tsFilePath, nuTcslen(tsFilePath));
	nuWcsToMbs(tsFilePath, dmFilePath, sizeof(dmFilePath));
#else
#ifndef ANDROID	
	nuTcscpy(cnFilePath, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(cnFilePath, nuTEXT("Media\\Data\\CNPackage.dat"));
	
	nuTcscpy(dmFilePath, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(dmFilePath, nuTEXT("Media\\Data\\DMPackage.dat"));
#else
	nuTcscpy(cnFilePath, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(cnFilePath, nuTEXT("Media\\Data_android\\CNPackage.dat"));
	
	nuTcscpy(dmFilePath, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(dmFilePath, nuTEXT("Media\\Data_android\\DMPackage.dat"));

#endif
#endif
	nuroTTSarg.CNVoiceFile	= (char*)cnFilePath;
	nuroTTSarg.DMVoiceFile	= (char*)dmFilePath;
	nuroTTSarg.ENVoiceFile	= NULL;
	m_bTTSOK	= nuTTSLoad(&nuroTTSarg);
#endif

#ifdef	NURO_USE_N19TTS
	m_loadPPTTS.LoadLib(g_fileSysApi.pGdata->pTsPath);
	m_loadPPTTS.TTSSetVolume(127);
	m_loadPPTTS.TTSSetSpeed(25);
	m_loadPPTTS.TTSSetPitch(25);
#endif

#ifdef NURO_OS_WINDOWS
	m_hThread = nuCreateThread(NULL, 0, SThreadAction, this, 0, &m_dwThreadID);
#endif

#ifdef NURO_OS_LINUX
	m_hThread = nuCreateThread(NULL, 0, SThreadAction, this, 0, &m_dwThreadID);
#endif

#if defined( NURO_OS_UCOS ) || defined( NURO_OS_JZ )
	m_hThread = nuCreateThread(NULL, 0, (LPNURO_THREAD_START_ROUTINE)SThreadAction, this, TASK_SOUND, (nuPDWORD)(&m_pStack[SOUND_STACK_SIZE-1]));
#endif

#ifdef NURO_OS_ECOS
	NURO_SECURITY_ATTRIBUTES attr;
	attr.nLength = (nuDWORD)(&m_dwThreadID);
	attr.bInheritHandle = (nuBOOL)(&m_hThread);
	m_hThread = nuCreateThread ( &attr,
								 4 * SOUND_STACK_SIZE,
								 (LPNURO_THREAD_START_ROUTINE)SThreadAction,
								 this,
								 TASK_SOUND,
								 (nuPDWORD)m_pStack );
#endif
#ifdef NURO_OS_HV_UCOS
	m_hThread = nuCreateThread( NULL,  SOUND_STACK_SIZE, (LPNURO_THREAD_START_ROUTINE)SThreadAction,this,TASK_SOUND,NULL );
#endif
	if( m_hThread == NULL )
	{
		return nuFALSE;
	}
	nuAsctoWcs(m_wsTtsRdStart, TTS_ADDR_START_NUM, TTS_ADDR_START_STR, nuStrlen(TTS_ADDR_START_STR) );
	nuAsctoWcs(m_wsTtsRdEnd, TTS_ADDR_END_NUM, TTS_ADDR_END_STR, nuStrlen(TTS_ADDR_END_STR) );


	return nuTRUE;
}

void CSoundThread::FreeSoundThread()
{
	m_bQuit = nuTRUE;
	while( !m_bOutThreadLoop )
	{
		nuSleep(25);
	}
#ifdef NURO_USE_CYBERON_TTS
#ifndef ANDROID
	if(m_bTTSOK)
	{
		Cyberon_TTS_Free();
	}
#endif
#endif
#ifdef NURO_USE_N19TTS
	m_loadPPTTS.FreeLib();
#endif
	if( m_bTTSOK )
	{
		nuTTSFree();
		m_bTTSOK = nuFALSE;
	}
	nuDelThread(m_hThread);
	//m_hThread = NULL;
	m_hThread = NULL;
	m_bQuit = nuFALSE;
	/*
	if( m_pSoundBuff != NULL )
	{
		delete []m_pSoundBuff;
		m_pSoundBuff = NULL;
	}
	*/
}

nuBOOL CSoundThread::NewSoundPlay(nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								nuWORD nRoadNameNum, nuWCHAR *pRoadName, nuBYTE bAsyn /*= 1*/)
{
	if (!m_bIsPlaying)
	{
		m_bIsCleanAll = nuFALSE;
	}
	if( /*(nVoiceType == VOICE_TYPE_ROAD || nVoiceType == VOICE_TYPE_OTHERS) &&*/
		(!(g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_NAVI))  )
	{
		return nuFALSE;
	}
	//
	if( bAsyn && REVISED_VOICE != nLanguage)
	{
		if( m_hThread == NULL )
		{
			return nuFALSE;
		}
		return PushSound(nVoiceType, nLanguage, nSound1, nSound2, nSound3, nRoadNameNum, pRoadName);
	}
	else
	{
		if( nVoiceType == VOICE_TYPE_UI_DEF )
		{
			if( m_bIsPlaying )
			{
				return nuFALSE;
			}
			if(REVISED_VOICE == nLanguage)
			{
				ReLoadTTS((const nuLONG)bAsyn, nSound1, nSound2, nSound3);
				return nuTRUE;
			}
			/*if( 21 == nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack )
			{
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_VOICE_WAV_START;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}*/
			nuWCHAR ws1[NAME_LINE_NUM_IN_FILE] = {0}, wsFullName[NAME_TTS_MAX] = {0};
			nuTCHAR tsSoundFile[NURO_MAX_PATH] = {0}, tsWaveFile[NURO_MAX_PATH] = {0}, tsValue[10]={0};
			nuTcscpy(tsSoundFile, g_fileSysApi.pGdata->pTsPath);
			nuTcscat(tsSoundFile, nuTEXT("Media\\sound\\"));
			#ifndef VALUE_EMGRT
			if(nSound3 < 20)
			#endif
			{
				nuItot(nSound1, tsValue, 10);
				nuTcscat(tsSoundFile, tsValue);
			}
			#ifdef  NURO_USE_CYBERON_TTS 	
			if(nSound3 < 20)
			{
				_GET_TTS_TEXT(nSound3, OTHER_FILE);
				_PLAY_TTS;
			}
			else
			{
				return PushSound(VOICE_TYPE_DEFAULT, nLanguage, nSound3, nSound2, nSound3, nRoadNameNum, pRoadName);
				nuItot(nSound3, tsValue, 10);
				nuTcscat(tsSoundFile, tsValue);
				nuTcscat(tsSoundFile, nuTEXT(".wav"));
			#ifndef ANDROID
				if(nSound3 == 21)
				{
					nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_SYNC);
				}
				else
				{
					nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_ASYNC);
				}
				//nuSleep(200);
			#else
				__android_log_print(ANDROID_LOG_INFO, "RoadList", "m_pfPlayWave %s",tsSoundFile);
				m_pfPlayWave(tsSoundFile, NULL);
			#endif
			}
			#else
			nuTcscat(tsSoundFile, _SOUND_EX);
			#endif
			/*
#ifdef NURO_OS_UCOS
			nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_SYNC);
#else
			*/
			#ifndef	NURO_USE_CYBERON_TTS 
			#ifndef ANDROID
			nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_ASYNC);
			#else
			m_pfPlayWave(tsSoundFile, NULL);
			#endif
			#endif
//#endif
			/*if( 21 == nSound3 &&
				NULL != g_fileSysApi.pGdata->pfCallBack )
			{
				CALL_BACK_PARAM cbParam;
				cbParam.nCallType = CALLBACK_TYPE_VOICE_WAV_END;
				g_fileSysApi.pGdata->pfCallBack(&cbParam);
			}*/
			return nuTRUE;
		}
		//
		if( m_bIsPlaying || m_soundDataFirst.bNewVoice )
		{
			return nuFALSE;
		}
		m_soundDataFirst.nVoiceType = nVoiceType;
		m_soundDataFirst.nLangType	= nLanguage;
		m_soundDataFirst.nSound1	= nSound1;
		m_soundDataFirst.nSound2	= nSound2;
		m_soundDataFirst.nSound3	= nSound3;
		if( pRoadName != NULL )
		{
			m_soundDataFirst.nNameNum	= nRoadNameNum;
			int nLen = NURO_MIN(VOICE_NAME_LEN - 1, nRoadNameNum) * sizeof(nuWCHAR);
			nuMemcpy(m_soundDataFirst.tsNAME, pRoadName, nLen);
			m_soundDataFirst.tsNAME[nLen/sizeof(nuWCHAR)] = 0;
		}
		else
		{
			m_soundDataFirst.nNameNum = 0;
			m_soundDataFirst.tsNAME[0] = NULL;
		}
		m_soundDataFirst.bNewVoice	= 1;
		return nuTRUE;
	}
}

nuBOOL CSoundThread::PushSound(nuBYTE nVoiceType, nuBYTE nLangType, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
							 nuWORD nNameNum, nuWCHAR *pName)
{
	long i;
	while( m_bIsWorking )
	{
		nuSleep(25);
	}
	m_bIsWorking = nuTRUE;
	m_pSoundList[m_nWriteIdx].bNewVoice	= 1;
	m_pSoundList[m_nWriteIdx].nVoiceType= nVoiceType;
	m_pSoundList[m_nWriteIdx].nLangType	= nLangType;
	m_pSoundList[m_nWriteIdx].nSound1	= nSound1;
	m_pSoundList[m_nWriteIdx].nSound2	= nSound2;
	m_pSoundList[m_nWriteIdx].nSound3	= nSound3;
	if( pName != NULL )
	{
		nuWcsncpy(m_pSoundList[m_nWriteIdx].tsNAME, pName, VOICE_NAME_LEN - 1);
		m_pSoundList[m_nWriteIdx].nNameNum	= nuWcslen(pName);//nNameNum;
		m_pSoundList[m_nWriteIdx].tsNAME[m_pSoundList[m_nWriteIdx].nNameNum] = 0;
	}
	else
	{
		m_pSoundList[m_nWriteIdx].nNameNum = 0;
		m_pSoundList[m_nWriteIdx].tsNAME[0] = NULL;
	}
	m_nWriteIdx = (m_nWriteIdx + 1) % SOUND_LIST_NUM;

	if(m_nReadIdx==m_nWriteIdx)
	{
		for(i=0;i<SOUND_LIST_NUM-1;i++)
		{
			m_pSoundList[m_nReadIdx].bNewVoice	= 0;
			m_nReadIdx=(m_nReadIdx + 1) % SOUND_LIST_NUM;
		}
	}

/*
	++m_nSoundCount;
	if( m_nSoundCount > SOUND_LIST_NUM )
	{
		m_nReadIdx = (m_nReadIdx + 1) % SOUND_LIST_NUM;
		--m_nSoundCount ;
	}
*/
	m_bIsWorking = nuFALSE;
	return nuTRUE;
}
nuBOOL CSoundThread::PopSound(PSOUNDDATA pSoundData)
{
	nuBOOL bRes = nuFALSE;
	while( m_bIsWorking )
	{
		nuSleep(25);
	}
	m_bIsWorking = nuTRUE;
	//
	if( m_pSoundList[m_nReadIdx].bNewVoice )
	{
		m_pSoundList[m_nReadIdx].bNewVoice	= 0;
		if(VOICE_CODE_REVISE_VOLUME == m_pSoundList[m_nReadIdx].nVoiceType ||
			VOICE_CODE_REVISE_SPEED == m_pSoundList[m_nReadIdx].nVoiceType ||
			VOICE_CODE_REVISE_PITCH == m_pSoundList[m_nReadIdx].nVoiceType)
		{
			ReLoadTTS(m_pSoundList[m_nReadIdx].nVoiceType, m_pSoundList[m_nReadIdx].nSound1, 
				m_pSoundList[m_nReadIdx].nSound2, m_pSoundList[m_nReadIdx].nSound3);
		}
		else
		{
			pSoundData->nVoiceType	= m_pSoundList[m_nReadIdx].nVoiceType;
			pSoundData->nLangType	= m_pSoundList[m_nReadIdx].nLangType;
			pSoundData->nNameNum	= m_pSoundList[m_nReadIdx].nNameNum;
			pSoundData->nSound1		= m_pSoundList[m_nReadIdx].nSound1;
			pSoundData->nSound2		= m_pSoundList[m_nReadIdx].nSound2;
			pSoundData->nSound3		= m_pSoundList[m_nReadIdx].nSound3;
			if( m_pSoundList[m_nReadIdx].nNameNum != 0 )
			{
				nuMemcpy(pSoundData->tsNAME, m_pSoundList[m_nReadIdx].tsNAME, (m_pSoundList[m_nReadIdx].nNameNum + 1) * sizeof(nuWCHAR));
			}
			//			--m_nSoundCount;
			bRes = nuTRUE;
		}
		m_nReadIdx = (m_nReadIdx + 1) % SOUND_LIST_NUM;
	}
	m_bIsWorking = nuFALSE;
	return bRes;
}
void CSoundThread::CleanSound()
{
	while( m_bIsWorking )
	{
		nuSleep(25);
	}
	nuMemset(m_pSoundList, 0, sizeof(m_pSoundList));
	m_nReadIdx = m_nWriteIdx = 0;
	m_bIsWorking  = nuFALSE;
	m_bIsCleanAll = nuTRUE; 
	Navi_CleanSound();
}

nuBOOL CSoundThread::SoundAction(nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3,
							   nuWORD nRoadNameNum, nuWCHAR *pRoadName)
{
	if (m_bIsCleanAll)
	{
		m_bIsCleanAll = nuFALSE;
		return nuTRUE;
	}
	m_bStopSound = nuFALSE;
	nuWCHAR ws1[NAME_LINE_NUM_IN_FILE] = {0}, wsFullName[NAME_TTS_MAX] = {0};
	nuTCHAR tsSoundFile[NURO_MAX_PATH] = {0}, tsWaveFile[NURO_MAX_PATH] = {0}, tsValue[10]={0};
	nuTCHAR tsPath[NURO_MAX_PATH] = {0}, tsTTSFilePath[NURO_MAX_PATH] = {0};
	nuBYTE	nWaveTTSType = g_fileSysApi.pGdata->uiSetData.nTTSWaveMode;
	nuTcscpy(tsSoundFile, g_fileSysApi.pGdata->pTsPath);
	nuTcscat(tsSoundFile, nuTEXT("Media\\sound\\"));
	nuTcscpy(tsPath, tsSoundFile);
	nuBOOL bres = nuFALSE;
#if (defined(_WIN32_WCE) && defined(_USE_POST_VOICE_CTR) )
	::PostMessage(HWND_BROADCAST , WM_NAVI_VOL, VOL_START, 0);
#endif
	if( nVoiceType == VOICE_TYPE_DEFAULT &&
		!m_bIsCleanAll)
	{
		nuItot(nSound1, tsValue, 10);
		nuTcscat(tsSoundFile, tsValue);
		nuTcscat(tsSoundFile, _SOUND_EX);
 
#ifdef NURO_OS_HV_UCOS
		if ( nSound1  == 20 )
		{
			if( nuFALSE == m_bSaveReRouteSBuf)
			{
				nuFILE *fp = nuTfopen(tsSoundFile, NURO_FS_RB);
				if(fp == NULL)
				{
					return 0;
				}
				m_nReRouLen = nuFgetlen(fp);
				if(nuFread( m_ReRouteSBuf, m_nReRouLen, 1, fp) != 1)
				{
					nuFclose(fp);
					return 0;
				}
				nuFclose(fp);
				m_bSaveReRouteSBuf = nuTRUE;
			}
			HV_PcmPlay((char*)m_ReRouteSBuf, m_nReRouLen);
		}
		else
		{
		nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_SYNC);
		}	
#else
		nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_SYNC);
		nuSleep(3);
#endif
		
#if (defined(_WIN32_WCE) && defined(_USE_POST_VOICE_CTR) )
		::PostMessage(HWND_BROADCAST , WM_NAVI_VOL, VOL_END, 0);
#endif
		m_bIsCleanAll = nuFALSE;
		return nuTRUE;
	}
	else
	{
		nuItot(nLanguage, tsValue, 10);
		nuTcscat(tsSoundFile, tsValue);
	}
	//
	//
	if( nVoiceType == VOICE_TYPE_ROAD )
	{
		if( nWaveTTSType == WAVETTS_TYPE_WAVE || nWaveTTSType == WAVETTS_TYPE_WAVETTS )
		{
			if( !m_bQuit && nSound1 != SOUND_IGNORE && !m_bIsCleanAll)
			{
			#ifdef NURO_USE_CYBERON_TTS
				_GET_TTS_TEXT(nSound1, DIS_FILE);
			#else
				_PLAY_VAVE(nSound1, DIS_FOLDER);
			#endif
			#ifdef NURO_USE_CYBERON_TTS
			#ifndef VALUE_EMGRT
				PlayDeiveAndEatimation(nSound1,wsFullName);
			#endif
			#endif
			}
			if(nSound1 == DIS_ESTIMATION)
			{
				if(nSound3 != 0 && nWaveTTSType != WAVETTS_TYPE_WAVETTS)
				{
					nSound2 = 0;
				}
				if(nWaveTTSType == WAVETTS_TYPE_WAVETTS)
				{
					nSound2 = 0;
				}
			}
			if( !m_bQuit && nSound2 != SOUND_IGNORE && !m_bIsCleanAll)
			{
				if(nSound2 < 10)
				{
				#ifdef NURO_USE_CYBERON_TTS
					_GET_TTS_TEXT(nSound2, TURN_FILE);
				#else
					_PLAY_VAVE(nSound2, TURN_FOLDER);
				#endif
				}
				else
				{
					_GET_TTS_TEXT(nSound2, TURN_FILE);
				#ifndef NURO_USE_CYBERON_TTS  
					_PLAY_TTS;
				#endif
				}
			}
			if( nSound3 == ROAD_TYPE_IN_CIRCLE		|| 
				nSound3 == ROAD_TYPE_OUT_CIRCLE		||
				nSound3 == ROAD_TYPE_IN_SLIPROAD    ||
				nSound3 == ROAD_TYPE_OUT_SLIPROAD   ||
				nSound3 == ROAD_TYPE_OUT_FREEWAY    ||
				nSound3 == ROAD_TYPE_OUT_FASTWAY    ||
				nSound3 == ROAD_TYPE_REACH_TARGET	||
				nWaveTTSType == WAVETTS_TYPE_WAVE   ||
				pRoadName == NULL ||
				nRoadNameNum == 0 )
			{
				if( !m_bQuit && nSound3 != SOUND_IGNORE && !m_bIsCleanAll)
				{
				#ifdef NURO_USE_CYBERON_TTS
					_GET_TTS_TEXT(nSound3, ROAD_FILE);
				#else
					_PLAY_VAVE(nSound3, ROAD_FOLDER);
				#endif
				}
				}
		#ifdef NURO_USE_CYBERON_TTS  
			if(wsFullName[0] != 0 && nWaveTTSType == WAVETTS_TYPE_WAVE || 
				nuWcslen(pRoadName) == 0 || nSound3 == ROAD_TYPE_IN_CIRCLE)
			{
				_PLAY_TTS;
				wsFullName[0] = 0;
			}
		#endif
			if( nWaveTTSType == WAVETTS_TYPE_WAVETTS && 
				nSound3 != ROAD_TYPE_IN_CIRCLE	&&
				nSound3 != ROAD_TYPE_REACH_TARGET )
			{
			#ifndef NURO_USE_CYBERON_TTS
				wsFullName[0] = 0;
			#endif
				if( !m_bQuit && nWaveTTSType == WAVETTS_TYPE_WAVETTS && nRoadNameNum > 0 && !m_bIsCleanAll)
				{
				#ifdef NURO_USE_TTSMODULE
					_GET_TTS_TEXT(ROAD_ENTER, ROAD_FILE);
					#if !(defined NURO_OS_HV_UCOS || defined TTS_VER_6 )
					nuWcscat(wsFullName, m_wsTtsRdStart);
					#endif
					nuWcscat(wsFullName, pRoadName);
					#if !(defined NURO_OS_HV_UCOS || defined TTS_VER_6 )
					nuWcscat(wsFullName, m_wsTtsRdEnd);
					#endif
				#endif
				#ifdef NURO_USE_CYBERON_TTS
				if(nWaveTTSType == WAVETTS_TYPE_WAVETTS)
				{
#ifndef VALUE_EMGRT
					nuWCHAR wsPassNodeName[6] = {0x5230, 0x9054, 0x7D93, 0x7531, 0x9EDE, 0};//Pass Node
					nuWCHAR wsEndNodeName[6] = {0x5230, 0x9054, 0x76EE, 0x7684, 0x5730, 0};//End Node
					nuBOOL bFlag = nuFALSE;
					nuWCHAR wsPoint[2]={0x002C};
					if (nSound1 == DIS_ESTIMATION || nSound1 == DIS_DRIVE)
					{
						if (nuWcscmp( pRoadName,wsPassNodeName) == 0 || nuWcscmp( pRoadName,wsEndNodeName) == 0)
						{
							bFlag = nuTRUE;
						}
					}
					if(nSound3 == ROAD_TYPE_IN_SLIPROAD )
					{
						bFlag = nuTRUE;
						nuWcscat(wsFullName, wsPoint);
					}
#endif
					if ( !bFlag )
					{
						_GET_TTS_TEXT(ROAD_ENTER, ROAD_FILE);
					}
					nuWcscat(wsFullName, pRoadName);
				}
				#endif
					nuWcscat(wsFullName, pRoadName);
					_PLAY_TTS;
				}
			}
		}
		else if( (nWaveTTSType == WAVETTS_TYPE_TTS ||  nWaveTTSType == WAVETTS_TYPE_TTSNOROADNAME)&&
			     !m_bIsCleanAll)
		{
			wsFullName[0] = 0;
			if( !m_bQuit && nSound1 != SOUND_IGNORE )
			{
				_GET_TTS_TEXT(nSound1, DIS_FILE);
			#ifndef NURO_USE_CYBERON_TTS
				PlayDeiveAndEatimation(nSound1,wsFullName);
			#endif
			}
			if( !m_bQuit && nSound2 != SOUND_IGNORE )
			{
				_GET_TTS_TEXT(nSound2, TURN_FILE);
			}
			if( nSound3 == ROAD_TYPE_IN_CIRCLE		|| 
				nSound3 == ROAD_TYPE_OUT_CIRCLE		||
				nSound3 == ROAD_TYPE_REACH_TARGET	||
				nSound3 == AF_IN_RTURN              ||
				nSound3	== AF_IN_LTURN				||
				nSound3	== AF_IN_UTURN              ||
				pRoadName == NULL	||
				nRoadNameNum == 0 )
			{
				if( !m_bQuit && nSound3 != SOUND_IGNORE )
				{
					_GET_TTS_TEXT(nSound3, ROAD_FILE);
				}
			}
			if( nRoadNameNum > 0 && nSound3 != ROAD_TYPE_IN_CIRCLE && nSound3 != ROAD_TYPE_REACH_TARGET)
			{
#ifdef VALUE_EMGRT
				#define ROAD_EXIT    34
				if (   nSound3 ==  ROAD_TYPE_OUT_FREEWAY 
					|| nSound3 ==  ROAD_TYPE_OUT_OVERPASS )
				{
					_GET_TTS_TEXT(ROAD_EXIT, ROAD_FILE);
				}
				else
#endif
				{
				_GET_TTS_TEXT(ROAD_ENTER, ROAD_FILE);
				}
			#ifdef NURO_USE_TTSMODULE	
				#if !(defined NURO_OS_HV_UCOS || defined TTS_VER_6 )
				nuWcscat(wsFullName, m_wsTtsRdStart);//for TTS say-as
				#endif
				nuWcscat(wsFullName, pRoadName);
				#if !(defined NURO_OS_HV_UCOS || defined TTS_VER_6 )
				nuWcscat(wsFullName, m_wsTtsRdEnd);//for TTS say-as
				#endif
			#endif
			#ifdef NURO_USE_CYBERON_TTS
			if(nWaveTTSType == WAVETTS_TYPE_WAVETTS)
			{
				nuWcscat(wsFullName, pRoadName);
			}
			#endif
			}
			nuWcscat(wsFullName, pRoadName);
			_PLAY_TTS;
		}
		bres = nuTRUE;
	}
	else if( nVoiceType == VOICE_TYPE_OTHERS )
	{
		if( O_CCDFIND == nSound3 &&
			NULL != g_fileSysApi.pGdata->pfCallBack )
		{
			CALL_BACK_PARAM cbParam;
			cbParam.nCallType = CALLBACK_TYPE_CCDSTART;
			g_fileSysApi.pGdata->pfCallBack(&cbParam);
		}
		//
/* LCMM 20110906 Louis change use new rule (old rule will make ccd play sound replay two kind of voice in WAVETTS_TYPE_WAVETTS type)
		if( (nWaveTTSType == WAVETTS_TYPE_WAVETTS || nWaveTTSType == WAVETTS_TYPE_TTS) && 
			nSound3 == O_EEYESFIND && nRoadNameNum > 0 )
		{
			wsFullName[0] = 0;
			nuWcscpy(wsFullName, pRoadName);
			_PLAY_TTS;
		}
		else if( nWaveTTSType == WAVETTS_TYPE_WAVE || nWaveTTSType == WAVETTS_TYPE_WAVETTS )
		{
			if( !m_bQuit && nSound3 != SOUND_IGNORE )
			{
				_PLAY_VAVE(nSound3, OTHER_FOLDER);
			}
		}
		else if( nWaveTTSType == WAVETTS_TYPE_TTS )
		{
			wsFullName[0] = 0;
			if( !m_bQuit && nSound3 != SOUND_IGNORE )
			{
				_GET_TTS_TEXT(nSound3, OTHER_FILE);
			}
			_PLAY_TTS;
		}
		if( nRoadNameNum > 0 &&  
			nSound3 == O_CCDFIND &&
			(nWaveTTSType == WAVETTS_TYPE_WAVETTS || nWaveTTSType == WAVETTS_TYPE_TTS) )
		{
			wsFullName[0] = 0;
			nuWcscpy(wsFullName, pRoadName);
			_PLAY_TTS;
		}
*/
		if (O_CCDFIND != nSound3 ||
			1  == g_fileSysApi.pUserCfg->bCCDVoice)
		{   // Modified by Damon For CCD Must popMsg
			//LCMM 20110906 Louis new rule
			if( (nWaveTTSType == WAVETTS_TYPE_WAVETTS || nWaveTTSType == WAVETTS_TYPE_TTS) && 
				(nSound3 == O_EEYESFIND) &&
				(nRoadNameNum > 0) && 
				!m_bIsCleanAll)
			{
				wsFullName[0] = 0;
				nuWcscpy(wsFullName, pRoadName);
				_PLAY_TTS;
			}
		#ifdef NURO_USE_CYBERON_TTS
			else if((nRoadNameNum > 0) &&
				(nSound3 == O_CCDFIND)&&
				!m_bIsCleanAll)
			{
				wsFullName[0] = 0;
				nuWcscpy(wsFullName, pRoadName);
				_PLAY_TTS;
			}
		#else//NURO_USE_CYBERON_TTS
			else if((nWaveTTSType == WAVETTS_TYPE_WAVETTS || nWaveTTSType == WAVETTS_TYPE_TTS) && 
				(nRoadNameNum > 0) &&
				(nSound3 == O_CCDFIND)&&
				!m_bIsCleanAll)
			{
				wsFullName[0] = 0;
				nuWcscpy(wsFullName, pRoadName);
				_PLAY_TTS;
			}
		#endif//NURO_USE_CYBERON_TTS
			else if((nWaveTTSType == WAVETTS_TYPE_WAVETTS || nWaveTTSType == WAVETTS_TYPE_TTS) && 
				(nRoadNameNum > 0) &&
				(nSound3 == O_SOUNDNAVITTS)&&
				!m_bIsCleanAll)
			{
				wsFullName[0] = 0;
				nuWcscpy(wsFullName, pRoadName);
				_PLAY_TTS;
			}
			else if( (nWaveTTSType == WAVETTS_TYPE_WAVE || nWaveTTSType == WAVETTS_TYPE_WAVETTS)&&
				!m_bIsCleanAll)
			{
				if( !m_bQuit && nSound3 != SOUND_IGNORE )
				{
			#ifdef  NURO_USE_CYBERON_TTS 
					_GET_TTS_TEXT(nSound3, OTHER_FILE);
					_PLAY_TTS;
			#else
					_PLAY_VAVE(nSound3, OTHER_FOLDER);
			#endif
				}
			}
			else if( nWaveTTSType == WAVETTS_TYPE_TTS &&
				!m_bIsCleanAll)
			{
				wsFullName[0] = 0;
				if( !m_bQuit && nSound3 != SOUND_IGNORE )
				{
					_GET_TTS_TEXT(nSound3, OTHER_FILE);
				}
				_PLAY_TTS;
			}
		}

		if( O_CCDFIND == nSound3 &&
			NULL != g_fileSysApi.pGdata->pfCallBack )
		{
			CALL_BACK_PARAM cbParam;
			cbParam.nCallType = CALLBACK_TYPE_CCDEND;
			g_fileSysApi.pGdata->pfCallBack(&cbParam);
		}
		bres = nuTRUE;
	}
	/*
	if( bSounBuffHeap )
	{
		delete []pSoudBuff;
		bSounBuffHeap = nuFALSE;
	}
	*/
#if (defined(_WIN32_WCE) && defined(_USE_POST_VOICE_CTR) )
	::PostMessage(HWND_BROADCAST , WM_NAVI_VOL, VOL_END, 0);
#endif 
	m_bIsCleanAll = nuFALSE;
	return bres;
}

nuBOOL CSoundThread::IsSound()
{
	if( m_pSoundList[m_nReadIdx].bNewVoice )
	{
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CSoundThread::Navi_PushSound(nuBYTE nVoiceType, nuBYTE nLangType, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
							 nuWORD nNameNum, nuWCHAR *pName)
{
	long i;
	while( m_bNavi_IsWorking )
	{
		nuSleep(25);
	}
	m_bNavi_IsWorking = nuTRUE;
	m_pNavi_SoundList[m_nNavi_WriteIdx].bNewVoice	= 1;
	m_pNavi_SoundList[m_nNavi_WriteIdx].nVoiceType	= nVoiceType;
	m_pNavi_SoundList[m_nNavi_WriteIdx].nLangType	= nLangType;
	m_pNavi_SoundList[m_nNavi_WriteIdx].nSound1		= nSound1;
	m_pNavi_SoundList[m_nNavi_WriteIdx].nSound2		= nSound2;
	m_pNavi_SoundList[m_nNavi_WriteIdx].nSound3		= nSound3;
	if( pName != NULL )
	{
		nuWcsncpy(m_pNavi_SoundList[m_nNavi_WriteIdx].tsNAME, pName, VOICE_NAME_LEN - 1);
		m_pNavi_SoundList[m_nNavi_WriteIdx].nNameNum	= nuWcslen(pName);//nNameNum;
		m_pNavi_SoundList[m_nNavi_WriteIdx].tsNAME[m_pNavi_SoundList[m_nNavi_WriteIdx].nNameNum] = 0;
	}
	else
	{
		m_pNavi_SoundList[m_nNavi_WriteIdx].nNameNum = 0;
		m_pNavi_SoundList[m_nNavi_WriteIdx].tsNAME[0] = NULL;
	}
	m_nNavi_WriteIdx = (m_nNavi_WriteIdx + 1) % SOUND_LIST_NUM;

	if(m_nNavi_ReadIdx==m_nNavi_WriteIdx)
	{
		for(i=0;i<SOUND_LIST_NUM-1;i++)
		{
			m_pNavi_SoundList[m_nNavi_ReadIdx].bNewVoice	= 0;
			m_nNavi_ReadIdx=(m_nNavi_ReadIdx + 1) % SOUND_LIST_NUM;
		}
	}

/*
	++m_nSoundCount;
	if( m_nSoundCount > SOUND_LIST_NUM )
	{
		m_nReadIdx = (m_nReadIdx + 1) % SOUND_LIST_NUM;
		--m_nSoundCount ;
	}
*/
	m_bNavi_IsWorking = nuFALSE;
	return nuTRUE;
}
nuBOOL CSoundThread::Navi_PopSound(PSOUNDDATA pSoundData)
{
	nuBOOL bRes = nuFALSE;
	while( m_bNavi_IsWorking )
	{
		nuSleep(25);
	}
	m_bNavi_IsWorking = nuTRUE;
	//
	if( m_pNavi_SoundList[m_nNavi_ReadIdx].bNewVoice )
	{
		m_pNavi_SoundList[m_nNavi_ReadIdx].bNewVoice	= 0;
		pSoundData->nVoiceType	= m_pNavi_SoundList[m_nNavi_ReadIdx].nVoiceType;
		pSoundData->nLangType	= m_pNavi_SoundList[m_nNavi_ReadIdx].nLangType;
		pSoundData->nNameNum	= m_pNavi_SoundList[m_nNavi_ReadIdx].nNameNum;
		pSoundData->nSound1		= m_pNavi_SoundList[m_nNavi_ReadIdx].nSound1;
		pSoundData->nSound2		= m_pNavi_SoundList[m_nNavi_ReadIdx].nSound2;
		pSoundData->nSound3		= m_pNavi_SoundList[m_nNavi_ReadIdx].nSound3;
		if( m_pNavi_SoundList[m_nNavi_ReadIdx].nNameNum != 0 )
		{
			nuMemcpy(pSoundData->tsNAME, m_pNavi_SoundList[m_nNavi_ReadIdx].tsNAME, (m_pNavi_SoundList[m_nNavi_ReadIdx].nNameNum + 1) * sizeof(nuWCHAR));
		}
		m_nNavi_ReadIdx = (m_nNavi_ReadIdx + 1) % SOUND_LIST_NUM;
//		--m_nSoundCount;
		bRes = nuTRUE;
	}
	m_bNavi_IsWorking = nuFALSE;
	return bRes;
}

void CSoundThread::Navi_CleanSound()
{
	while( m_bNavi_IsWorking )
	{
		nuSleep(25);
	}
	nuMemset(m_pNavi_SoundList, 0, sizeof(m_pNavi_SoundList));
	m_nNavi_ReadIdx = m_nNavi_WriteIdx = 0;
	m_bNavi_IsWorking  = nuFALSE;
}

nuBOOL CSoundThread::Navi_SoundPlay(nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
								nuWORD nRoadNameNum, nuWCHAR *pRoadName, nuBYTE bAsyn /*= 1*/)
{
	if (!m_bNavi_IsPlaying)
	{
		m_bIsCleanAll = nuFALSE;
	}
	if( /*(nVoiceType == VOICE_TYPE_ROAD || nVoiceType == VOICE_TYPE_OTHERS) &&*/
		(!(g_fileSysApi.pUserCfg->nVoiceControl & VOICE_CONTROL_NAVI))  )
	{
		return nuFALSE;
	}
	//
	if( bAsyn )
	{
		if( m_hThread == NULL )
		{
			return nuFALSE;
		}
		return Navi_PushSound(nVoiceType, nLanguage, nSound1, nSound2, nSound3, nRoadNameNum, pRoadName);
	}
	else
	{
		if( nVoiceType == VOICE_TYPE_UI_DEF )
		{
			if( m_bIsPlaying )
			{
				return nuFALSE;
			}
			nuWCHAR ws1[NAME_LINE_NUM_IN_FILE] = {0}, wsFullName[NAME_TTS_MAX] = {0};
			nuTCHAR tsSoundFile[NURO_MAX_PATH] = {0}, tsWaveFile[NURO_MAX_PATH] = {0}, tsValue[10]={0};
			nuTcscpy(tsSoundFile, g_fileSysApi.pGdata->pTsPath);
			nuTcscat(tsSoundFile, nuTEXT("Media\\sound\\"));
			#ifndef VALUE_EMGRT
			if(nSound3 < 20)
			#endif
			{
				nuItot(nSound1, tsValue, 10);
				nuTcscat(tsSoundFile, tsValue);
			}
			#ifdef  NURO_USE_CYBERON_TTS 	
			if(nSound3 < 20)
			{
				_GET_TTS_TEXT(nSound3, OTHER_FILE);
				_PLAY_TTS;
			}
			else
			{
				nuItot(nSound3, tsValue, 10);
				nuTcscat(tsSoundFile, tsValue);
				nuTcscat(tsSoundFile, nuTEXT(".wav"));
			#ifndef ANDROID
				nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_ASYNC);
			#else
				m_pfPlayWave(tsSoundFile, NULL);
			#endif
			}
			#else
			nuTcscat(tsSoundFile, _SOUND_EX);
			#endif
			/*
#ifdef NURO_OS_UCOS
			nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_SYNC);
#else
			*/
			#ifndef	NURO_USE_CYBERON_TTS 
			#ifndef ANDROID
			nuPlaySound(tsSoundFile, m_pSoundBuff, NURO_SND_ASYNC);
			#else
			m_pfPlayWave(tsSoundFile, NULL);
			#endif
			#endif
//#endif
			return nuTRUE;
		}
		//
		if( m_bNavi_IsPlaying || m_Navi_soundDataFirst.bNewVoice )
		{
			return nuFALSE;
		}
		m_Navi_soundDataFirst.nVoiceType = nVoiceType;
		m_Navi_soundDataFirst.nLangType	= nLanguage;
		m_Navi_soundDataFirst.nSound1	= nSound1;
		m_Navi_soundDataFirst.nSound2	= nSound2;
		m_Navi_soundDataFirst.nSound3	= nSound3;
		if( pRoadName != NULL )
		{
			m_Navi_soundDataFirst.nNameNum	= nRoadNameNum;
			int nLen = NURO_MIN(VOICE_NAME_LEN - 1, nRoadNameNum) * sizeof(nuWCHAR);
			nuMemcpy(m_Navi_soundDataFirst.tsNAME, pRoadName, nLen);
			m_Navi_soundDataFirst.tsNAME[nLen/sizeof(nuWCHAR)] = 0;
		}
		else
		{
			m_Navi_soundDataFirst.nNameNum = 0;
			m_Navi_soundDataFirst.tsNAME[0] = NULL;
		}
		m_Navi_soundDataFirst.bNewVoice	= 1;
		return nuTRUE;
	}
}
nuVOID CSoundThread::PlayDeiveAndEatimation(nuWORD nSound1,nuWCHAR *wsRoadName)
{
	if (nSound1 == DIS_DRIVE)
	{
		nuWCHAR DisArray[10] = {0};
		nuWCHAR nKm[4] = {0x516C, 0x91CC, 0x5F8C};
		nuDWORD	nDis = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance/1000;
		nuDWORD TempDis = 0;
		if(nDis != 0)
		{
			if ((nuINT)nDis >= 100)
			{
				TempDis = nDis/100;	
				DisArray[0] = wsNumber[TempDis];//百位數
				DisArray[1] = wsNumber[11];//百
				nDis = nDis%100;
				TempDis = nDis/10;
				if(TempDis == 0)
				{
					DisArray[2] = wsNumber[TempDis];
					TempDis = nDis%10;
					DisArray[3] = wsNumber[TempDis];//個位數
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
				else
				{
					DisArray[2] = wsNumber[TempDis];
					TempDis = nDis%10;
					DisArray[3] = wsNumber[10];//十
					TempDis = nDis%10;
					DisArray[4] = wsNumber[TempDis];//個位數
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
			}
			else if ((nuINT)nDis < 100 && (nuINT)nDis >= 10)
			{
				if((nuINT)nDis >= 20)
				{
					TempDis = nDis/10; 
					DisArray[0] = wsNumber[TempDis];
					DisArray[1] = wsNumber[10];
					TempDis = nDis%10;
					if(TempDis != 0)
					{
						DisArray[2] = wsNumber[TempDis];
					}
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
				else if((nuINT)nDis < 20)
				{
					DisArray[0] = wsNumber[10];
					TempDis = nDis%10;
					if(TempDis != 0)
					{
						DisArray[1] = wsNumber[TempDis];
					}
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
			}
			else if ((nuINT)nDis > 0 && (nuINT)nDis < 10)
			{
				DisArray[0] = wsNumber[nDis];
				nDis = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance%1000;
				nDis = nDis/100;
				if(nDis != 0)
				{
					DisArray[1] =  wsNumber[12];
					DisArray[2] =  wsNumber[nDis];
				}
				nuWcscat(wsRoadName, DisArray);
				nuWcscat(wsRoadName, nKm);
			}
		}
		else if ((nuINT)nDis == 0)
		{
			DisArray[0] = wsNumber[1];
			nuWcscat(wsRoadName, DisArray);
			nuWcscat(wsRoadName, nKm);
		}
	}
	if (nSound1 == DIS_ESTIMATION )
	{
		nuWCHAR DisArray[10] = {0};
		nuWCHAR nKm[4] = {0x516C, 0x91CC, 0x5F8C};
		nuDWORD	nDis = g_fileSysApi.pGdata->drawInfoNavi.nextCross.nDistance/1000;
		nuDWORD TempDis = 0;
		if(nDis != 0)
		{
			if ((nuINT)nDis >= 100)
			{
				TempDis = nDis/100;	
				DisArray[0] = wsNumber[TempDis];//百位數
				DisArray[1] = wsNumber[11];//百
				nDis = nDis%100;
				TempDis = nDis/10;
				if(TempDis == 0)
				{
					DisArray[2] = wsNumber[TempDis];
					TempDis = nDis%10;
					DisArray[3] = wsNumber[TempDis];//個位數
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
				else
				{
					DisArray[2] = wsNumber[TempDis];
					TempDis = nDis%10;
					DisArray[3] = wsNumber[10];//十
					TempDis = nDis%10;
					DisArray[4] = wsNumber[TempDis];//個位數
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
			}
			else if ((nuINT)nDis < 100 && (nuINT)nDis >= 10)
			{
				if((nuINT)nDis >= 20)
				{
					TempDis = nDis/10; 
					DisArray[0] = wsNumber[TempDis];
					DisArray[1] = wsNumber[10];
					TempDis = nDis%10;
					if(TempDis != 0)
					{
						DisArray[2] = wsNumber[TempDis];
					}
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
				else if((nuINT)nDis < 20)
				{
					DisArray[0] = wsNumber[10];
					TempDis = nDis%10;
					if(TempDis != 0)
					{
						DisArray[1] = wsNumber[TempDis];
					}
					nuWcscat(wsRoadName, DisArray);
					nuWcscat(wsRoadName, nKm);
				}
			}
			else if ((nuINT)nDis < 10)
			{
				DisArray[0] = wsNumber[nDis];
				nuWcscat(wsRoadName, DisArray);
				nuWcscat(wsRoadName, nKm);
			}
		}
	}
}
nuBOOL CSoundThread::ReLoadTTS(const nuLONG RevisedCode, const nuLONG lVolunm, const nuLONG lSpeed, const nuLONG lPitch)
{
#ifdef NURO_USE_CYBERON_TTS
	/*if( m_bTTSOK )
	{
		nuTTSFree();
		m_bTTSOK = nuFALSE;
	}*/
	switch(RevisedCode)
	{
	case VOICE_CODE_REVISE_VOLUME:
		m_lVolunm = lVolunm;
		Cyberon_TTS_SetVolumn(m_lVolunm);
		break;
	case VOICE_CODE_REVISE_SPEED:
		m_lSpeed = lSpeed;
		Cyberon_TTS_SetSpeed(m_lSpeed);
		break;
	case VOICE_CODE_REVISE_PITCH:
		m_lPitch = lPitch;
		Cyberon_TTS_SetPitch(m_lPitch);
		break;
	}
	//m_bTTSOK = Cyberon_TTS_Init(g_fileSysApi.pGdata->pTsPath, m_lVolunm, m_lSpeed, m_lPitch);
	//return m_bTTSOK;
	return nuTRUE;
#else
	return nuTRUE;
#endif
}
nuVOID CSoundThread::StopSoundPlaying()
{
	m_bStopSound = nuTRUE;
#ifdef NURO_USE_CYBERON_TTS
	Cyberon_TTS_SoundStop();
#endif
}
