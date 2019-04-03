#ifndef _NURO_TTS_INTERFACE_20081223
#define _NURO_TTS_INTERFACE_20081223

#include "NuroDefine.h"


typedef struct tagNUROWAVHEADER
{
	nuCHAR          RIFFTag[4]; /*   should   be   "RIFF"   */   
	nuLONG          dwFileSize; /*   byte_number   behind   it   */   
	nuCHAR          WAVETag[4]; /*   should   be   "WAVE"   */   
	nuCHAR          fmtTag[4];       /*   should   be   "fmt   "   */   
	nuULONG         dwfmtSize;           /*   should   be   0x10   */   
	nuUSHORT        wFormatTag;   /*   should   be   1   */   
	nuUSHORT        wChannels;   
	nuULONG	        dwSamplesPerSec;   
	nuULONG	        dwAvgBytesPerSec;   
	nuUSHORT	    wBlockAlign;   /*2*/
	nuUSHORT	    uiBitsPerSample;   
	nuCHAR			dataTag[4];         /*   should   be   "data"   */   
	nuULONG	        dwPCMSize;             /*   byte_number   of   PCM   data   in   byte*/   
}NUROWAVHEADER;

typedef struct tagNUROTTSARG
{
	nuCHAR*         CNVoiceFile;//中文数据文件路径
	nuCHAR*         ENVoiceFile;//英文数据文件路径
	nuCHAR*         DMVoiceFile;//定制数据文件路径
}NUROTTSARG;

nuBOOL	nuTTSLoad(NUROTTSARG* pArg);
/* TTSSession
*<0 tts error
*-1 tts sys error
*-2 no init
*-3 sys voice buffer out
*-4 out of wavbuffer
*-5 no define NURO_USE_TTSMODULE
*>=0 ok! return real len of wav
*/
nuINT	nuTTSSession(nuWCHAR* pWcs, nuCHAR* pWavBuffer, nuINT nMaxBufferLen, nuDWORD fdwSound);
nuVOID	nuTTSFree();
nuVOID  nuTTSFirstInit();

#ifdef NURO_OS_LINUX // Added by Dengxu @ 2012 2 14

nuINT nuTTSSession(nuWCHAR* pWcs, nuCHAR* pWavBuffer, nuINT nMaxBufferLen, nuDWORD fdwSound, nuTCHAR *tsPath, nuTCHAR *tsTTSFilePath);

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
    int jt_Fseek( FILE*  stream, long offset, int origin ); // Added by Dengxu @ 2012 2 14
    size_t jt_Fread( void *buffer, size_t size, size_t count, FILE*  stream ); // Added by Dengxu @ 2012 2 14
    FILE* jt_Fopen( const char *filename, const char *mode ); // Added by Dengxu @ 2012 2 14
    int jt_Fclose( FILE* stream ); // Added by Dengxu @ 2012 2 14

#ifdef __cplusplus
}
#endif
#endif


#endif