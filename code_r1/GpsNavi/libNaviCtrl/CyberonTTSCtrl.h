#ifndef CYBERON_TTS_DEFINEBYDANIEL_20121220
#define CYBERON_TTS_DEFINEBYDANIEL_20121220

#include "NuroDefine.h"
#ifdef NURO_USE_CYBERON_TTS
#include "TTSMgrDLL.h"
#include "CTTS.h"
#include "waveio.h"
#include "CBSDKTool.h"

#define  TTS_TXT_FILE	L"\\TTS.txt"

#define MAX_SUPPORT_TTS_NUM 2
#define MAX_TTS_STR_BUF_LEN	2048

nuBOOL Cyberon_TTS_Init(const nuTCHAR *szPath, nuLONG lVolunm, nuLONG lSpeed, nuLONG lPitch);
nuDWORD Cyberon_TTS_PlaySound(nuWCHAR *wsTTSReport);
nuVOID  Cyberon_TTS_Free();
nuVOID Cyberon_TTS_SoundStop();
nuVOID Cyberon_TTS_SetVolumn(nuLONG lVolunm);
nuVOID Cyberon_TTS_SetSpeed(nuLONG lSpeed);
nuVOID Cyberon_TTS_SetPitch(nuLONG lPitch);
typedef struct _PlayBuffer
{
	HANDLE	hWavePlay;
	HANDLE	hTTS;
	nuDWORD	dwSize;
} PlayBuffer;

struct	CTTSDemo
{
	nuHWND			m_hWndMain;
	nuHWND			m_hEditBox;
	nuHWND			m_hList;
	nuHWND			m_hBtn;
	nuHWND			m_zhComboBox[MAX_SUPPORT_TTS_NUM];

	///////////////////////////////////
	// TTS Engine
	nuHANDLE		m_hTTS;
	nuHANDLE		m_hPlayThread;
	nuINT			m_nTTS;
	nuINT			m_pznLangID[MAX_SUPPORT_TTS_NUM];
	nuINT			m_nDefaultLangID;

	///////////////////////////////////
	// Flag
	BOOL			m_bStop;
	nuHANDLE		g_hWavePlay;
	//TTS string
	nuTCHAR			m_pwczTTSStr[MAX_TTS_STR_BUF_LEN];

};

#endif//NURO_USE_CYBERON_TTS
#endif//CYBERON_TTS_DEFINEBYDANIEL_20121220