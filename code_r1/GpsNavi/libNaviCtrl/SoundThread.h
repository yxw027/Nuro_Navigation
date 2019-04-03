// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: SoundThread.h,v 1.10 2010/08/24 03:11:16 wangwenjun Exp $
// $Author: wangwenjun $
// $Date: 2010/08/24 03:11:16 $
// $Locker:  $
// $Revision: 1.10 $
// $Source: /home/nuCode/libNaviCtrl/SoundThread.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDTHREAD_H__3FC7F536_4E2B_4BD8_838C_C3436AC5164C__INCLUDED_)
#define AFX_SOUNDTHREAD_H__3FC7F536_4E2B_4BD8_838C_C3436AC5164C__INCLUDED_

#include "NuroDefine.h"
#include "GApiNaviCtrl.h"

#ifdef NURO_USE_N19TTS
#include "LoadPPTTS.h"
#endif

//#include "nuOS.h"
#define SOUND_STACK_SIZE			(1024*16)

#ifdef TOBE_SYSTEM
	#define SOUND_LIST_NUM				10
#else
	#define SOUND_LIST_NUM				3
#endif
#define VOICE_NAME_LEN				68

#define VOICE_TYPE_DEFAULT			0x00
#define VOICE_TYPE_ROAD				0x01//road direction voice, Sound1 is distance, sound2 is turn flag, sound3 is road Type
#define VOICE_TYPE_OTHERS			0x02//nuVOID in others folder
#define VOICE_TYPE_UI_DEF			0x03

#define REVISED_VOICE				0xFF

#define MAX_CYBERON_TTS_VOLUME		500

typedef struct tagSOUNDDATA
{
	nuBYTE	bNewVoice:1;
	nuBYTE	nVoiceType:7;
	nuBYTE	nLangType;
	nuWORD	nNameNum;
	nuWORD	nSound1;//distance code or LangSame voice code
	nuWORD	nSound2;//Trunning voice code
	nuWORD	nSound3;//RoadType voice code
	nuWCHAR	tsNAME[VOICE_NAME_LEN];
}SOUNDDATA, *PSOUNDDATA;

#define TTS_ADDR_START_NUM	50
#define TTS_ADDR_END_NUM	20	

#define TTS_ADDR_START_STR	"<say-as interpret-as=\"address\">"
#define TTS_ADDR_END_STR	"</say-as>"	

class CSoundThread  
{
public:
	CSoundThread();
	virtual ~CSoundThread();

	nuBOOL InitSoundThread();
	nuVOID FreeSoundThread();

	nuBOOL NewSoundPlay( nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
		                 nuWORD nRoadNameNum, nuWCHAR* pRoadName, nuBYTE bAsyn = 1 );
	nuBOOL Navi_SoundPlay( nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
						 nuWORD nRoadNameNum, nuWCHAR* pRoadName, nuBYTE bAsyn = 1 );
	nuVOID PlayDeiveAndEatimation(nuWORD nSound1,nuWCHAR *wsRoadName);
public:
	static nuDWORD SThreadAction(nuPVOID lpVoid);

	nuBOOL PopSound(PSOUNDDATA pSoundData);
	nuBOOL Navi_PopSound(PSOUNDDATA pSoundData);
	nuVOID CleanSound();
	nuVOID Navi_CleanSound();
	nuVOID StopSoundPlaying();
protected:
	nuBOOL PushSound( nuBYTE nVoiceType, nuBYTE nLangType, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
		              nuWORD nNameNum, nuWCHAR *pName );
	nuBOOL Navi_PushSound( nuBYTE nVoiceType, nuBYTE nLangType, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
					  nuWORD nNameNum, nuWCHAR *pName );
	nuBOOL SoundAction( nuBYTE nVoiceType, nuBYTE nLanguage, nuWORD nSound1, nuWORD nSound2, nuWORD nSound3, 
		                nuWORD nRoadNameNum, nuWCHAR *pRoadName );
	nuBOOL IsSound();

private:
	nuBOOL ReLoadTTS(const nuLONG RevisedCode, const nuLONG lVolunm, const nuLONG lSpeed, const nuLONG lPitch); //added by daniel

public:
	nuHANDLE			m_hThread;
	nuDWORD     		m_dwThreadID;

	SOUNDDATA			m_pSoundList[SOUND_LIST_NUM];
	SOUNDDATA			m_pNavi_SoundList[SOUND_LIST_NUM];
	nuBYTE				m_nReadIdx;
	nuBYTE				m_nNavi_ReadIdx;
	nuBYTE				m_nWriteIdx;
	nuBYTE				m_nNavi_WriteIdx;
//	nuBYTE				m_nSoundCount;
	nuBOOL				m_bQuit;
	nuBOOL				m_bOutThreadLoop;
	nuBOOL				m_bTTSOK;
	nuBOOL				m_bStopSound;
	#ifdef ANDROID
	NC_SOUNDCallBackProc m_pfPlayWave;
	#endif
protected:
	nuBOOL				m_bIsWorking;
	nuBOOL				m_bNavi_IsWorking;
	nuBOOL				m_bIsPlaying;
	nuBOOL				m_bNavi_IsPlaying;
	nuBOOL              m_bIsCleanAll;  // Added by Damon for Clean not play now String 20111124
	SOUNDDATA			m_soundDataFirst;
	SOUNDDATA			m_Navi_soundDataFirst;
	nuWCHAR				m_wsTtsRdStart[TTS_ADDR_START_NUM];
	nuWCHAR				m_wsTtsRdEnd[TTS_ADDR_END_NUM];
	nuBYTE*				m_pSoundBuff;
	nuBOOL				m_bCallBack;

#ifdef NURO_USE_N19TTS
	CLoadPPTTS			m_loadPPTTS;
#endif

private:

#if defined(NURO_OS_UCOS) || defined(NURO_OS_ECOS) || defined(NURO_OS_JZ)
	nuINT					m_pStack[SOUND_STACK_SIZE];
#endif
#ifdef NURO_OS_HV_UCOS
	nuUCHAR             m_ReRouteSBuf[500 * 1024] ;
	nuLONG				m_nReRouLen;
	nuBOOL m_bSaveReRouteSBuf;
#endif

private://added by daniel
	nuLONG m_lSpeed;
	nuLONG m_lVolunm;
	nuLONG m_lPitch;

};

#endif // !defined(AFX_SOUNDTHREAD_H__3FC7F536_4E2B_4BD8_838C_C3436AC5164C__INCLUDED_)
