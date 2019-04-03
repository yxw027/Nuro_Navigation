//
//	Copyright (c) 2007 Cyberon Corp.  All right reserved.
//	File: CTTS.h
//	Description: SDK TTS APIs

#ifndef __CTTS_H__
#define __CTTS_H__

#include "ttsdll.h"

#ifdef __cplusplus
extern "C"{
#endif

nuBOOL BSR_TTS_Init(nuHANDLE* lphTTS, const nuTCHAR *lpszTTSsDir, const nuTCHAR *lpszBINsDir = NULL, nuBOOL bIsHighQuality = FALSE);
//	FUNCTION: nuBOOL BSR_TTS_Init(nuHANDLE* lphTTS, const nuTCHAR *lpszTTSsDir, const nuTCHAR *lpszBINsDir, nuBOOL bIsHighQuality)
//
//	PURPOSE: To initialize TTS engine
//
//	PARAMETERS:
//		lphTTS [out]
//			Pointer to the handle of TTS
//
//		lpszTTSsDir [in]
//			The full path for TTS DLL files
//
//		lpszBINsDir [in]
//			The full path for TTS binary files (.bin)
//
//		bIsHighQuality [in]
//			To use high quality TTS or not.
//			TRUE indicates to use, FALSE indicates not to use.
//
//	RETURN VALUE:
//		TRUE indicates success. FALSE indicates failure.
//
//	COMMENTS:
//		None

nuCHAR* BSR_TTS_Out(const nuTCHAR* lpszSen, nuHANDLE hTTS ,TTS_TYPE nType = TTS_TYPE_PROMPT, nuINT nDefaultLangID = -1);
//	FUNCTION: nuCHAR* BSR_TTS_Out(const nuTCHAR* szSen, nuHANDLE hTTS ,TTSType nType, nuINT nDefaultLangID)
//
//	PURPOSE: Output a wave buffer which is produced by TTS
//
//	PARAMETERS:
//		lpszSen [in]
//			Pointer to the nuTCHAR command-string
//
//		hTTS [in]
//			Handle of TTS engine create by BSR_TTS_Init
//
//		nType [in]
//			To indicate how the given string szSen will be pronounced.
//			If szSen is a name string, you could set this value to TTS_TYPE_NAME.
//
//		nDefaultLangID [in]
//			Set the default value of language identifier.
//			-1 indicates using OS language as default language.
//
//	RETURN VALUE:
//		Pointer to the wave buffer indicates success. NULL indicates failure.
//
//	COMMENTS:
//		None

nuCHAR* BSR_TTS_OutEx(const nuTCHAR* lpszSen, nuHANDLE hTTS , nuINT* pnOutBufSize, TTS_TYPE nType = TTS_TYPE_PROMPT, nuINT nDefaultLangID = -1);
//	FUNCTION: nuCHAR* BSR_TTS_OutEx(const nuTCHAR* szSen, nuHANDLE hTTS ,TTSType nType, nuINT nDefaultLangID, nuINT* pnOutBufSize)
//
//	PURPOSE: Output a wave buffer which is produced by TTS
//
//	PARAMETERS:
//		lpszSen [in]
//			Pointer to the nuTCHAR command-string
//
//		hTTS [in]
//			Handle of TTS engine create by BSR_TTS_Init
//
//		pnOutBufSize [out]
//			Get the size of the outputted wave buffer		
//
//		nType [in]
//			To indicate how the given string szSen will be pronounced.
//			If szSen is a name string, you could set this value to TTS_TYPE_NAME.
//
//		nDefaultLangID [in]
//			Set the default value of language identifier.
//			-1 indicates using OS language as default language.
//
//	RETURN VALUE:
//		Pointer to the wave buffer indicates success. NULL indicates failure.
//
//	COMMENTS:
//		None

nuBOOL BSR_TTS_Release(nuHANDLE hTTS);
//	FUNCTION: nuINT BSR_TTS_Release(nuHANDLE hTTS)
//
//	PURPOSE: Release TTS engine
//
//	PARAMETERS:
//		hTTS [in]
//			Handle of TTS engine created by BSR_TTS_Init
//
//	RETURN VALUE:
//		TRUE indicates success. FALSE indicates failure.
//
//	COMMENTS:
//		None

VOID BSRResample(nuINT nSrcSampleRate, nuSHORT *lpSrcSample, nuINT nSrcBlockSize, nuSHORT *lpDestSample, nuINT nDestBlockSize);
//	FUNCTION: VOID BSRResample(nuINT nSrcSampleRate, nuSHORT *lpSrcSample, nuINT nSrcBlockSize, nuSHORT *lpDestSample, nuINT nDestBlockSize)
//
//	PURPOSE: Down-sample wave data in buffer to 8 KHz wave data
//
//	PARAMETERS:
//		nSrcSampleRate [in]
//			Sample rate of wave data in lpScrSample.
//			This value could be one of following values : 8000, 11025, 16000, 22050, 44100.
//
//		lpSrcSample [in]
//			Source sample buffer.
//
//		nSrcBlockSize [in]
//			Buffer size of lpSrcSample.
//
//		lpDestSample [out]
//			Destination sample buffer after re-sample.
//
//		nDestBlockSize [in]
//			Buffer size of lpDestSample.
//
//	RETURN VALUE:
//		None
//
//	COMMENTS:
//		None

VOID BSRResampleTo16K(nuINT nSrcSampleRate, nuSHORT *lpSrcSample, nuINT nSrcBlockSize, nuSHORT *lpDestSample, nuINT nDestBlockSize);
//	FUNCTION: VOID BSRResampleTo16K(nuINT nSrcSampleRate, nuSHORT *lpSrcSample, nuINT nSrcBlockSize, nuSHORT *lpDestSample, nuINT nDestBlockSize)
//
//	PURPOSE: Down-sample wave data in buffer to 16 KHz wave data
//
//	PARAMETERS:
//		nSrcSampleRate [in]
//			Sample rate of wave data in lpScrSample.
//			This value could be one of following values : 16000, 22050, 44100.
//
//		lpSrcSample [in]
//			Source sample buffer.
//
//		nSrcBlockSize [in]
//			Buffer size of lpSrcSample.
//
//		lpDestSample [out]
//			Destination sample buffer after re-sample.
//
//		nDestBlockSize [in]
//			Buffer size of lpDestSample.
//
//	RETURN VALUE:
//		None
//
//	COMMENTS:
//		None

VOID BSRSetScale(nuUINT nScale);
//	FUNCTION: VOID BSRSetScale(unsigned int nScale)
//
//	PURPOSE: Set the volume of TTS output
//
//	PARAMETERS:
//		nScale [in]
//			Scale option.
//			nScale = 0 : x 1
//			nScale = 1 : x 1.5
//			nScale = 2 : x 2
//			nScale = 3 : x 2.5
//
//	RETURN VALUE:
//		None
//
//	COMMENTS:
//		None

#ifdef __cplusplus
}
#endif

#endif