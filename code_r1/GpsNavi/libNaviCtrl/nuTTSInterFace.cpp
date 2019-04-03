#ifdef ANDROID
#include <stdlib.h>
#endif

#include "nuTTSInterFace.h"
#include <stdarg.h>

#if (defined NURO_OS_WINCE) || (defined NURO_OS_WINDOWS)
#ifdef NURO_USE_TTSMODULE
#pragma comment(lib, "ejTTS.lib")
#endif
#endif

#ifdef NURO_USE_TTSMODULE
#if(defined NURO_OS_HV_UCOS || defined TTS_VER_6)
 #include "eJTTS60.h"
typedef struct tagUserData
{	
	char *	pOutputBuffer;	// 合成语音数据输出buffer
	long nOutbufferLen;
	long nBufferNeed;
	char *pStrIN;
	long  StrInLen;
	unsigned long	hTTS;	// 合成引擎句柄
}jtUserData;

jtErrCode jtExam_InputTextProc(void* pParameter, 
	void* pText, long* piSize)
{
	jtUserData *pUserData = (jtUserData*)pParameter;
	
	// 如果读取的数据长度为0，并将0返回给引擎，
	// 引擎将认为没有文本需要合成了，对于段式回调
	// 合成方式，合成过程将会结束，如果是篇章模式，
	// 引擎将会不断地轮询该回调函数，以获得文本
        int nTmpsize = *piSize;
	nuMemset( pText, 0, nTmpsize );
	*piSize = NURO_MAX( 0, NURO_MIN( *piSize, pUserData->StrInLen ) );
	if (  *piSize != 0 )
	{
		nuMemcpy( pText, pUserData->pStrIN, *piSize );
		pUserData->StrInLen -= *piSize;
		pUserData->pStrIN   += *piSize;
	}
	return jtTTS_ERR_NONE;
}
// 合成语音数据输出回调函数
// 当合成一段语音时,此函数会被多次调用,每次输出一定量的语音数据
// 用户可在此函数内将语音数据送入音频播放接口
jtErrCode jtExam_OutputVoiceProc(void* pParameter, 
								 long iOutputFormat, void* pData, long iSize)
{
	jtUserData *	pUserData = NULL;
	
	// 注：如果未设置用户数据，不要使用此判断
	if(pParameter == NULL)
	{
		return jtTTS_ERR_NONE;
	}	
	pUserData = (jtUserData *)pParameter;
	
	
	// 如果iSize为-1，则是当前文本的
	// 所有合成数据均已输出，如果需要停止引擎，
	// 则可以在这里进行
	if(iSize == -1 )
	{
		// jtTTS_SynthesizeText和jtTTS_Synthesize
		// 将输入的文本合成完毕后，会自动退出，
		// 可以不调用jtTTS_SynthStop,而
		// jtTTS_SynthStart合成完毕后，并不主动退出，
		// 需调用jtTTS_SynthStop使之退出
		jtTTS_SynthStop(pUserData->hTTS);
		return jtTTS_ERR_NONE;
	}
	nuMemcpy( pUserData->pOutputBuffer + pUserData->nBufferNeed, pData, iSize );
	nuMemset( pData, 0, iSize );
	pUserData->nBufferNeed += iSize;
	return jtTTS_ERR_NONE;
}
#else
  #include "nuTTSModule.h"
#endif


#if(defined NURO_OS_UCOS || defined ANDROID || defined NURO_OS_LINUX)
#ifndef STD_ANSI_C
nuLONG jtts_ftell( FILE*  stream )
{
	return nuFtell((nuFILE*)stream);
}
nuINT jtts_fseek( FILE*  stream, nuLONG offset, nuINT origin )
{
	return nuFseek((nuFILE*)stream, offset, origin);
}
size_t jtts_fread( nuVOID *buffer, size_t size, size_t count, FILE*  stream )
{
	return nuFread(buffer, size, count, (nuFILE*)stream);
}
size_t jtts_fwrite( nuVOID *buffer, size_t size, size_t count, FILE*  stream )
{
	return nuFwrite(buffer, size, count, (nuFILE*)stream);
}
FILE* jtts_fopen( const nuCHAR *filename, const nuCHAR *mode )
{
	if(filename==NULL || mode==NULL)
	{
		return NULL;
	}
	if((mode[0] == 'r')&& (mode[1] == 'b'))
	{
		return (FILE*)nuFopen(filename, NURO_FS_RB);
	}
	else if((mode[0] == 'w')&& (mode[1] == 'b'))
	{
		return (FILE*)nuFopen(filename, NURO_FS_WB);
	}
	return NULL;
}
nuINT jtts_fclose( FILE* stream )
{
	return nuFclose((nuFILE*)stream);
}
nuVOID* jtts_memset(nuVOID *dest, nuINT c, size_t count)
{
	return nuMemset(dest, c, count);
}
nuVOID* jtts_memcpy(nuVOID *dest, const nuVOID *src, size_t count)
{
	return nuMemcpy(dest, src, count);
}
nuVOID* jtts_malloc(nuUINT size)
{
	return nuMalloc(size);
}
nuVOID jtts_free(nuVOID* p)
{
	nuFree(p);
}
nuINT jtts_printf(const nuCHAR *format, ...)
{
#ifdef NURO_OS_UCOS
    g_pnuOs->pfPrintf(format);

    //add by dengxu
#elif defined NURO_OS_LINUX 
    int num = 0;
    va_list ap;
    va_start(ap, format);
    num = vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
    return num;
#else
    return 0;
#endif

    return 0;
}

nuINT jtts_sprintf(nuCHAR * buffer, const nuCHAR * format,...)
{
	nuINT nRet = 0;
	va_list args; 
    va_start(args, format); 
    nRet = vsprintf(buffer, format, args);
    va_end(args);
    return nRet;
}

nuINT jtts_atoi(const nuCHAR *string)
{
	return atoi(string);
}
nuFLOAT jtts_atof(const nuCHAR *string)
{
	return (nuFLOAT)atof(string);
}

nuCHAR *jtts_strtok(nuCHAR *strToken, const nuCHAR *strDelimit)
{
	return nuStrtok(strToken, strDelimit);
}

#endif
long jt_ftell( FILE*  stream )
{
    return nuFtell((nuFILE*)stream);
}

int jt_Fseek( FILE*  stream, long offset, int origin )
{
    return nuFseek((nuFILE*)stream, offset, origin);
}
size_t jt_Fread( void *buffer, size_t size, size_t count, FILE*  stream )
{
    return nuFread(buffer, size, count, (nuFILE*)stream);
}

size_t jt_fwrite( void *buffer, size_t size, size_t count, FILE*  stream )
{
    return nuFwrite(buffer, size, count, (nuFILE*)stream);
}

FILE* jt_Fopen( const char *filename, const char *mode )
{   
    if(filename==NULL || mode==NULL)
    {
        return NULL;
    }
    if((mode[0] == 'r')&& (mode[1] == 'b'))
    {
        return (FILE*)nuFopen(filename, NURO_FS_RB);
    }
    else if((mode[0] == 'w')&& (mode[1] == 'b'))
    {
        return (FILE*)nuFopen(filename, NURO_FS_WB);
    }
    return NULL;
}   
int jt_Fclose( FILE* stream )
{   
    return nuFclose((nuFILE*)stream);
}

void* jt_memset(void *dest, int c, size_t count)
{   
    return nuMemset(dest, c, count);
}

void* jt_memcpy(void *dest, const void *src, size_t count)
{
    return nuMemcpy(dest, src, count);
}
void* jt_malloc(unsigned int size)
{
    return nuMalloc(size);
}
void jt_free(void* p)
{
    nuFree(p);
}
int jt_printf(const char *format, ...)
{
#ifdef NURO_OS_UCOS
    g_pnuOs->pfPrintf(format);
    //add by dengxu
#elif defined NURO_OS_LINUX
    int num = 0;
	va_list ap;
    va_start(ap, format);
    num = vprintf(format, ap);
    va_end(ap);
    fflush(stdout);
    return num;
#else
    return 0;
#endif
}

int jt_sprintf(char * buffer, const char * format,...)
{
    int nRet = 0;
	va_list args;
    va_start(args, format); 
    nRet = vsprintf(buffer, format, args);
    va_end(args);
    return nRet;
}   

int jt_atoi(const char *string)
{   
    return atoi(string);
}   
float jt_atof(const char *string)
{   
    return (float)atof(string);
}   

char *jt_strtok(char *strToken, const char *strDelimit)
{   
    return nuStrtok(strToken, strDelimit);
}



#endif


#define NURO_TTS_BUFFER_MAX (1024*5)
static nuDWORD	dwHandle = 0;
static nuVOID* pGlobalData = NULL;
static nuCHAR* pVoiceData = NULL;
signed char *gpCNFile = NULL;
signed char *gpDMFile = NULL;

#endif//ifdef NURO_USE_TTSMODULE

nuBOOL GetFileContentJ( nuTCHAR *strFileName,signed char **pFileMem )
{
	nuINT nDicLen = 0;
	nuFILE *pFile = nuTfopen( strFileName, NURO_FS_RB );
	if ( !pFile )
	{
		nuTTSFree();
		return nuFALSE;
	}
	if( nuFseek( pFile, 0,  NURO_SEEK_END ) )
	{
		nuFclose( pFile );
		return nuFALSE;
	}
	nDicLen = nuFtell( pFile );
	*pFileMem = ( signed char *)nuMalloc( nDicLen );
	nuMemset( *pFileMem, 0, nDicLen );
	if ( !(*pFileMem) )
	{
		nuFclose( pFile );
		return nuFALSE;
	}
	if( nuFseek( pFile, 0, NURO_SEEK_SET ) )
	{
		nuFclose( pFile );
		return nuFALSE;
	}
	if( nuFread( *pFileMem, nDicLen, 1, pFile ) != 1 )
	{
		nuFclose( pFile );
		return nuFALSE;
	}
	nuFclose( pFile );

	return nuTRUE;
}

//#define  DATA_MODE_SMALL_FILE
nuBOOL nuTTSLoad(NUROTTSARG* pArg)
{
#ifdef NURO_USE_TTSMODULE
#if (defined  NURO_OS_HV_UCOS || defined TTS_VER_6)
	jtErrCode nErr = jtTTS_ERR_NONE;
	nuLONG nGlobalData = 0;
//	DebugOutStr( pArg->CNVoiceFile );
//	DebugOutStr( pArg->ENVoiceFile );
//	DebugOutStr( pArg->DMVoiceFile );
#ifndef  DATA_MODE_SMALL_FILE
	if( !GetFileContentJ( pArg->CNVoiceFile,&gpCNFile) )
	{
		//		printf( "Open CNVoiceFile error");
		nuTTSFree();
		return nuFALSE;
	}
	if( !GetFileContentJ( pArg->DMVoiceFile, &gpDMFile ) )
	{
		//printf( "Open DMVoiceFile error");
		nuTTSFree();
		return nuFALSE;
	}
#endif
	
#ifdef DATA_MODE_SMALL_FILE	
	jtTTS_GetExtBufSize(  (const signed char*)pArg->CNVoiceFile, 
						(const signed char*)pArg->ENVoiceFile, 
						(const signed char*)pArg->DMVoiceFile,
						&nGlobalData
					  );
#else
	jtTTS_GetExtBufSize(  (const signed char*)gpCNFile, 
					(const signed char*)NULL, 
					(const signed char*)gpDMFile,
					&nGlobalData
					  );
#endif
	pGlobalData = new nuCHAR[nGlobalData];//(nuCHAR*)nuMalloc(nGlobalData);
	if(pGlobalData == NULL)
	{
		return nuFALSE;
	}
	nuMemset(pGlobalData,0,nGlobalData);
	
	//Add 10-02-24
	

	/*if( !GetFileContentJ( pArg->CNVoiceFile,&gpCNFile) )
	{
//		printf( "Open CNVoiceFile error");
		nuTTSFree();
		return nuFALSE;
	}
	if( !GetFileContentJ( pArg->DMVoiceFile, &gpDMFile ) )
	{
		printf( "Open DMVoiceFile error");
		nuTTSFree();
		return nuFALSE;
	}*/
	
    jtErrCode nErrRet = jtTTS_ERR_NONE;
	#ifdef DATA_MODE_SMALL_FILE
	if(( nErrRet = jtTTS_Init((const signed char*)pArg->CNVoiceFile,
				  (const signed char*)pArg->ENVoiceFile,
				  (const signed char*)pArg->DMVoiceFile,				
				   &dwHandle, 
				  pGlobalData) ) != jtTTS_ERR_NONE)
	#else
	  if(( nErrRet = jtTTS_Init((const signed char*)gpCNFile,
		  (const signed char*)NULL,
		  (const signed char*)gpDMFile ,
		  &dwHandle, 
	  pGlobalData) ) != jtTTS_ERR_NONE)
	#endif
	{
//		printf( "\n gpCNFile = %d, err = %d \n",gpCNFile , nErrRet);
//		printf( "nuTTSLoad 0");
		nuTTSFree();
		return nuFALSE;
	}
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_WAV_FORMAT, jtTTS_FORMAT_PCM_16K16B);
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_UNICODE);
	if(nErr != jtTTS_ERR_NONE)
	{
		nuTTSFree();
		return nuFALSE;
	}
	nuLONG tts_volume = 0;
	if (!nuReadConfigValue("TTS_VOLUME", &tts_volume))
	{
		tts_volume = 1000;
	}
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_VOLUME, tts_volume);
	if(nErr != jtTTS_ERR_NONE)
	{
		nuTTSFree();
		return nuFALSE;
	}
	pVoiceData = (nuCHAR*)nuMalloc(NURO_TTS_BUFFER_MAX);
	if(pVoiceData == NULL)
	{
		nuTTSFree();
		return nuFALSE;
	}
	long tts_speed = 0;
	if (!nuReadConfigValue("TTS_SPEED", &tts_speed))
	{
		tts_speed = jtTTS_SPEED_NORMAL;
	}
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_SPEED, tts_speed);
	if(nErr != jtTTS_ERR_NONE)
	{
		//printf("jTTSSetParam nErr = %d\n",nErr);
		return nErr;
	}

	jtTTS_SetParam(dwHandle, jtTTS_PARAM_PITCH, jtTTS_PITCH_NORMAL);
	/* 设置直接文本输入 */
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_INPUTTXT_MODE, 
		jtTTS_INPUT_TEXT_CALLBACK );

	/* 设置音频输出回调 */
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_INPUT_CALLBACK, 
		(nuLONG)jtExam_InputTextProc);

	/* 设置音频输出回调 */
	nErr = jtTTS_SetParam(dwHandle, jtTTS_PARAM_OUTPUT_CALLBACK, 
		(nuLONG)jtExam_OutputVoiceProc);
	 nuTTSFirstInit();
#else
	ERRCODE nErr = ERR_NONE;
	nuINT nGlobalData = jTTSGetExtBufSize();
	pGlobalData = new nuCHAR[nGlobalData];//(nuCHAR*)nuMalloc(nGlobalData);
	if(pGlobalData == NULL)
	{
		return nuFALSE;
	}
	nuMemset(pGlobalData,0,nGlobalData);
	if(jTTSInit(pArg->CNVoiceFile, pArg->ENVoiceFile
		, pArg->DMVoiceFile, &dwHandle, pGlobalData) != ERR_NONE)
	{
		nuTTSFree();
		return nuFALSE;
	}
	nErr = jTTSSetParam(dwHandle, PARAM_WAVFORMAT, FORMAT_WAV_16K16B);
	nErr = jTTSSetParam(dwHandle, PARAM_CODEPAGE, CODEPAGE_UNICODE);
	if(nErr != ERR_NONE)
	{
		nuTTSFree();
		return nuFALSE;
	}
	nuLONG tts_volume = 0;
	if (!nuReadConfigValue("TTS_VOLUME", &tts_volume))
	{
  		tts_volume = 1000;
	}
	nErr = jTTSSetParam(dwHandle, PARAM_VOLUME, tts_volume);
	if(nErr != ERR_NONE)
	{
		nuTTSFree();
		return nuFALSE;
	}
	pVoiceData = (nuCHAR*)nuMalloc(NURO_TTS_BUFFER_MAX);
	if(pVoiceData == NULL)
	{
		nuTTSFree();
		return nuFALSE;
	}
#endif
	
#endif
	return nuTRUE;
}

#ifdef  NURO_USE_TTSMODULE

#ifdef NURO_OS_LINUX
extern "C" void sndPlaySound(char *snd_buf, int len);
#endif

#ifndef ANDROID

nuINT nuTTSSession(nuWCHAR* pWcs, nuCHAR* pWavBuffer, nuINT nMaxBufferLen, nuDWORD fdwSound)
{
#ifdef NURO_USE_TTSMODULE
	if(dwHandle==0 || pGlobalData==NULL)
	{
		return -2;
	}
	
	nuINT nSize = sizeof(NUROWAVHEADER);
	
#if(defined NURO_OS_HV_UCOS || defined TTS_VER_6)
	jtErrCode dwError = jtTTS_ERR_NONE;
	nuMemset( pWavBuffer, 0, nMaxBufferLen );
	jtUserData UserData = {0};
	UserData.hTTS          = dwHandle;
	UserData.pOutputBuffer = pWavBuffer + nSize;
	UserData.nOutbufferLen = nMaxBufferLen;
	UserData.pStrIN = (char *)pWcs;   	                             
	UserData.nBufferNeed = 0;
	UserData.StrInLen = ( nuWcslen(pWcs) + 1 )* sizeof(nuWCHAR) ;
	/* 设置回调用户数据 */
	dwError = jtTTS_SetParam(dwHandle, jtTTS_PARAM_CALLBACK_USERDATA, 
		(long)(&UserData));

	dwError = jtTTS_SynthStart(dwHandle);
	//dwError = jtTTS_Synthesize(dwHandle);
    	//nuMemcpy( pOutBuffer, pVoiceData, UserData.nBufferNeed );
	nSize += UserData.nBufferNeed;
#else
	nuMemset(pVoiceData, 0, NURO_TTS_BUFFER_MAX);
	nuDWORD nLen;
	
	char* pOutBuffer = pWavBuffer;
	pOutBuffer += nSize;
	if (jTTSSessionStart(dwHandle, (const char*)pWcs) != ERR_NONE)
	{
		return -1;
	}
	ERRCODE nErr = ERR_NONE;
	while(1)
	{
		nLen = NURO_TTS_BUFFER_MAX;
		nErr = jTTSSessionGetData(dwHandle, pVoiceData,&nLen,0);
		if(nErr == ERR_NONE)
		{
			break;
		}
		else if(nErr == ERR_MORETEXT)
		{
			if(nLen > NURO_TTS_BUFFER_MAX)
			{
				return -3;
			}
			nSize += nLen;
			if(nSize > nMaxBufferLen)
			{
				nSize -= nLen;
				return -4;
			}
			nuMemcpy(pOutBuffer, pVoiceData, nLen);
			pOutBuffer += nLen;
			continue;
		}
		else
		{
			break;
		}
	}
	if(jTTSSessionStop(dwHandle) != ERR_NONE)
	{
		return -1;
	}
#endif
	NUROWAVHEADER wavHeader = {0};
	nuMemset(&wavHeader, 0, sizeof(NUROWAVHEADER));
	nuStrncpy(wavHeader.RIFFTag, "RIFF", 4);
	wavHeader.dwFileSize = nSize ; 
	nuStrncpy(wavHeader.WAVETag, "WAVE", 4);
	nuStrncpy(wavHeader.fmtTag, "fmt ", 4);
	wavHeader.dwfmtSize = 0x10;  
	wavHeader.wFormatTag = 1;
	wavHeader.wChannels = 1;   
	wavHeader.dwSamplesPerSec = 16*1000;   
	wavHeader.dwAvgBytesPerSec = 16*1000*2;   
	wavHeader.wBlockAlign = 2;   
	wavHeader.uiBitsPerSample = 16; 
	nuStrncpy(wavHeader.dataTag, "data", 4);
	wavHeader.dwPCMSize = nSize - sizeof(NUROWAVHEADER);	
	nuMemcpy(pWavBuffer, &wavHeader, sizeof(NUROWAVHEADER));
	//
	/*TCHAR sFilePath[NURO_MAX_PATH];
	nuGetModuleFileName(NULL, sFilePath, NURO_MAX_PATH);
	for(int i = nuTcslen(sFilePath) - 1; i >= 0; i--)
	{
		if( sFilePath[i] == nuTEXT('\\') )
		{
			sFilePath[i+1] = nuTEXT('\0');
			break;
		}
	}
	static int nIII = 0;
	char buf1[10] = {0};
	nuItoa(nIII, buf1, 10);
	TCHAR buf2[10] = {0};
	int nIndexx = 0;
	for ( ;nIndexx < 10 ;nIndexx++)
	{
		buf2[nIndexx] = buf1[nIndexx];
	}
	nuTcscat( sFilePath,buf2 );
	nuTcscat(sFilePath, nuTEXT(".wav"));
	nuFILE *fTmp= nuTfopen(sFilePath,NURO_FS_WB );
	nuFwrite( pWavBuffer, nSize, 1, fTmp );
	nuFclose( fTmp );
	++nIII;
	*/
#ifdef NURO_OS_HV_UCOS
		HV_PcmPlay( (char*)pWavBuffer + sizeof(NUROWAVHEADER), nSize - sizeof(NUROWAVHEADER) );
#endif

	
#ifdef NURO_OS_UCOS	
	switch(fdwSound)
	{
	case NURO_SND_SYNC://
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		while(!g_pnuOs->McatchGpsGetAudPlayStatus())
		{
			nuSleep(20);
		}
		break;
	case NURO_SND_ASYNC:
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		break;
	default:
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		while(!g_pnuOs->McatchGpsGetAudPlayStatus())
		{
			nuSleep(30);
		}
		break;
	}
#endif

#ifdef NURO_OS_WINDOWS
	sndPlaySound((nuTCHAR*)pWavBuffer, fdwSound | SND_MEMORY);
#endif

#ifdef NURO_OS_LINUX
    sndPlaySound((char*)pWavBuffer + sizeof (NUROWAVHEADER), nSize - sizeof (NUROWAVHEADER));
#endif

	return nSize;
#endif
	return -5;
}

#else

#   ifdef NURO_OS_LINUX

#       ifdef USING_SAIGE_LIB
        //In nuApiDevice.h void saigePlaySound(char *snd_buf, int len);
#       else
        extern "C" void sndPlaySound(char *snd_buf, int len);
#       endif

#   endif  

extern "C" void nuAndroidCallBackTTS(void);

nuINT nuTTSSession(nuWCHAR* pWcs, nuCHAR* pWavBuffer, nuINT nMaxBufferLen, nuDWORD fdwSound, nuTCHAR *tsPath, nuTCHAR *tsTTSFilePath)
{
    //__android_log_print(ANDROID_LOG_DEBUG,"nuTTSSession",(const char*)pWcs);
    ////////////////////////////
#ifdef NURO_USE_TTSMODULE
	if(dwHandle==0 || pGlobalData==NULL)
	{
		return -2;
	}
	
	nuINT nSize = sizeof(NUROWAVHEADER);
	
#if(defined NURO_OS_HV_UCOS || defined TTS_VER_6)
	jtErrCode dwError = jtTTS_ERR_NONE;
	nuMemset( pWavBuffer, 0, nMaxBufferLen );
	jtUserData UserData = {0};
	UserData.hTTS          = dwHandle;
	UserData.pOutputBuffer = pWavBuffer + nSize;
	UserData.nOutbufferLen = nMaxBufferLen;
	UserData.pStrIN = (nuCHAR *)pWcs;
	UserData.nBufferNeed = 0;
	UserData.StrInLen = ( nuWcslen(pWcs) + 1 )* sizeof(nuWCHAR) ;
	/* 设置回调用户数据 */
	dwError = jtTTS_SetParam(dwHandle, jtTTS_PARAM_CALLBACK_USERDATA, 
		(nuLONG)(&UserData));

	dwError = jtTTS_SynthStart(dwHandle);
	//dwError = jtTTS_Synthesize(dwHandle);
    	//nuMemcpy( pOutBuffer, pVoiceData, UserData.nBufferNeed );
	nSize += UserData.nBufferNeed;
#else
	nuMemset(pVoiceData, 0, NURO_TTS_BUFFER_MAX);
	nuDWORD nLen;
	
	nuCHAR* pOutBuffer = pWavBuffer;
	pOutBuffer += nSize;
	if (jTTSSessionStart(dwHandle, (const nuCHAR*)pWcs) != ERR_NONE)
	{
		return -1;
	}
	ERRCODE nErr = ERR_NONE;
	while(1)
	{
		nLen = NURO_TTS_BUFFER_MAX;
		nErr = jTTSSessionGetData(dwHandle, pVoiceData,&nLen,0);
		if(nErr == ERR_NONE)
		{
			break;
		}
		else if(nErr == ERR_MORETEXT)
		{
			if(nLen > NURO_TTS_BUFFER_MAX)
			{
				return -3;
			}
			nSize += nLen;
			if(nSize > nMaxBufferLen)
			{
				nSize -= nLen;
				return -4;
			}
			nuMemcpy(pOutBuffer, pVoiceData, nLen);
			pOutBuffer += nLen;
			continue;
		}
		else
		{
			break;
		}
	}
	if(jTTSSessionStop(dwHandle) != ERR_NONE)
	{
		return -1;
	}
#endif

#ifdef NURO_OS_HV_UCOS
		HV_PcmPlay( (char*)pWavBuffer + sizeof(NUROWAVHEADER), nSize - sizeof(NUROWAVHEADER) );
#endif

	
#ifdef NURO_OS_UCOS	
	switch(fdwSound)
	{
	case NURO_SND_SYNC://
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		while(!g_pnuOs->McatchGpsGetAudPlayStatus())
		{
			nuSleep(20);
		}
		break;
	case NURO_SND_ASYNC:
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		break;
	default:
		g_pnuOs->pfMcatchPlayWave((UINT16*)pWavBuffer, nSize);
		while(!g_pnuOs->McatchGpsGetAudPlayStatus())
		{
			nuSleep(30);
		}
		break;
	}
#endif

    NUROWAVHEADER wavHeader = {0};
    nuMemset(&wavHeader, 0, sizeof(NUROWAVHEADER));
    nuStrncpy(wavHeader.RIFFTag, "RIFF", 4); 
    wavHeader.dwFileSize = nSize ; 
    nuStrncpy(wavHeader.WAVETag, "WAVE", 4); 
    nuStrncpy(wavHeader.fmtTag, "fmt ", 4); 
    wavHeader.dwfmtSize = 0x10;  
    wavHeader.wFormatTag = 1;
    wavHeader.wChannels = 1;   
    wavHeader.dwSamplesPerSec = 16*1000;   
    wavHeader.dwAvgBytesPerSec = 16*1000*2;   
    wavHeader.wBlockAlign = 2;   
    wavHeader.uiBitsPerSample = 16; 
    nuStrncpy(wavHeader.dataTag, "data", 4); 
    wavHeader.dwPCMSize = nSize - sizeof(NUROWAVHEADER);    
    nuMemcpy(pWavBuffer, &wavHeader, sizeof(NUROWAVHEADER));

    nuTCHAR sFilePath[NURO_MAX_PATH]={0};
    nuTcscpy(tsTTSFilePath, tsPath);
    nuTcscat(tsTTSFilePath, nuTEXT("tts.wav"));

    nuFILE *fTmp= nuTfopen(tsTTSFilePath, NURO_FS_WB );
    nuFwrite( pWavBuffer, nSize, 1, fTmp );
    nuFclose( fTmp );

    nuAndroidCallBackTTS();
    
    return nSize;
#endif
	return -5;
}
#endif 

#endif

nuVOID nuTTSFree()
{
#ifdef NURO_USE_TTSMODULE
#if(defined NURO_OS_HV_UCOS || defined TTS_VER_6)
	if(dwHandle != 0)
	{
		jtTTS_End(dwHandle);
		dwHandle = 0;
	}
#else
	if(dwHandle != 0)
	{
		jTTSEnd(dwHandle);
		dwHandle = 0;
	}
#endif
	if(pGlobalData != NULL)
	{
		delete []pGlobalData;
		pGlobalData = NULL;
	}
	if(pVoiceData != NULL)
	{
		nuFree(pVoiceData);		
		pVoiceData = NULL;
	}
	if ( gpCNFile )
	{
		nuFree( gpCNFile );		
		gpCNFile = NULL;
	}
	if ( gpDMFile )
	{
		nuFree( gpDMFile );		
		gpDMFile = NULL;
	}
#endif
}

nuVOID nuTTSFirstInit()
{
#if ( (defined NURO_OS_HV_UCOS || defined TTS_VER_6) && defined NURO_USE_TTSMODULE )
	#define  MEM_SIZE  700 * 1024
	if ( !dwHandle )
	{
		return;
	}
	jtErrCode dwError = jtTTS_ERR_NONE;
	jtUserData UserData = {0};
    
	nuCHAR *pTmpMem = new nuCHAR[MEM_SIZE];
	UserData.hTTS          = dwHandle;
	UserData.pOutputBuffer = pTmpMem;
	UserData.nOutbufferLen = MEM_SIZE;
	UserData.pStrIN = "1";
	UserData.StrInLen = nuStrlen("1") ;
	/* 设置回调用户数据 */
	dwError = jtTTS_SetParam(dwHandle, jtTTS_PARAM_CALLBACK_USERDATA, 
		(nuLONG)(&UserData));
	dwError = jtTTS_Synthesize(dwHandle);// jtTTS_SynthStart(dwHandle);
	delete []pTmpMem;
	pTmpMem=NULL;
#endif
}
 
