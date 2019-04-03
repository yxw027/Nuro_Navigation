#ifndef __NUDEFINE_H_2012_7_20
#define __NUDEFINE_H_2012_7_20

//#define NURO_USE_TTSMODULE
//#define TTS_VER_6
//#define DATA_MODE_SMALL_FILE

//#define NURO_USE_CYBERON_TTS
#ifdef NURO_USE_CYBERON_TTS
#undef NURO_USE_TTSMODULE
#undef DATA_MODE_SMALL_FILE 
#endif

#define EJTTS
//#define NURO_USE_HWMODULE

//#define USE_DYM_DLL

#define  VALUE_EMGRT
//#define  ZENRIN

#define ECI
#define KD

//#define  GET_HS_DEBUG_INFO
#define  LCMM
#define  MAX_RouteRule					6
//#define  HUDTEST      //hudtest


#ifdef 	ANDROID
#include <android/log.h> 
//#undef  NURO_USE_TTSMODULE
#undef 	DATA_MODE_SMALL_FILE
//#undef 	VALUE_EMGRT
#undef 	USE_DYM_DLL
#define ANDROID_GPS
#define USE_HUD_DR_CALLBACK 
#define SameRoad
#endif
 
//#define NURO_DEBUG

#include "Inc/NuroConfig.h"
#include "Inc/NuroTypes.h"
#include "NuroClib/NuroClib.h"
#include "NuroOsApi/NuroOSApi.h"
#include "NuroCrossInterface/NuroBMP.h"
#include "NuroCrossInterface/libInterface.h"


//////////////////////////////////////////////////////////////////////////
// EXTEND MARCOS
#define NURO_ABS(a) (((a) < 0) ? (-(a)) : (a))
#define NURO_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define NURO_MIN(a, b) (((a) > (b)) ? (b) : (a))

#define nuRGB(r,g,b)          ((nuCOLORREF)(((nuBYTE)(r)|((nuWORD)((nuBYTE)(g))<<8))|(((nuDWORD)(nuBYTE)(b))<<16)))
#define nuMAKEWORD(a, b)      ((nuWORD)(((nuBYTE)(a)) | ((nuWORD)((nuBYTE)(b))) << 8))
#define nuMAKELONG(a, b)      ((nuLONG)(((nuWORD)(a)) | ((nuDWORD)((nuWORD)(b))) << 16))
#define nuLOWORD(l)           ((nuWORD)(l))
#define nuHIWORD(l)           ((nuWORD)(((nuDWORD)(l) >> 16) & 0xFFFF))
#define nuLOBYTE(w)           ((nuBYTE)(w))
#define nuHIBYTE(w)           ((nuBYTE)(((nuWORD)(w) >> 8) & 0xFF))
#define nuLOBIT4(a)			  (((nuBYTE)a) & 0x0F )
#define nuHIBIT4(a)			  ((((nuBYTE)a) & 0xF0 ) >> 4 )
#define nuBYTELOTOHI(a)		  ((nuBYTE)( ((a & 0x0F) << 4) | (a & 0x0F)))

#define nuGetRValue(rgb)     ( (nuINT)((rgb)&0xff) )
#define nuGetGValue(rgb)     ( (nuINT)((rgb>> 8)&0xff) )
#define nuGetBValue(rgb)     ( (nuINT)((rgb>>16)&0xff) )

#define PI 3.141593

//-------------------------------------------------------

#ifndef NURO_OS_LINUX
#define TASK_ID_TYPE		     nuDWORD
#endif

#define NR_LONG_MAX			     2147483647L

#define POI_ICON_START_NUM	     16001
/* Background Modes */
#define NURO_TRANSPARENT         1
#define NURO_OPAQUE              2
#define NURO_BKMODE_LAST         2
//-------------------------------------------------------

#ifndef NURO_MAX_FILE_PATH
#define NURO_MAX_FILE_PATH	     512
#endif

#ifndef NURO_MAX_WAVFILE_SIZE
#define NURO_MAX_WAVFILE_SIZE	(1024 * 700)
#endif

#define NURO_L(x) L##x

#define NURO_SND_SYNC   0x0000
#define NURO_SND_ASYNC  0x0001

#define  NURO_USE_DIB_BITMAP

#ifdef NURO_OS_WINDOWS
#include <windows.h>
#endif

//------------------------ConstDefined-------------
#define COORCONSTANT 1
#define HALF_MAX_LONGITUDE                      (1800*10000*COORCONSTANT)
#define HALF_MAX_LATITUDE                       (900*10000*COORCONSTANT)

#ifdef N19

#define EACHBLOCKSIZE                           (10000*COORCONSTANT)
#define BLOCKCOUNTOFEACHDEGREE                  10
#define XBLOCKNUM                               3600
#define YBLOCKNUM                               1800
#define MAXLENOFROADOUTBLOCK                    ((2000)*COORCONSTANT)

#else//N19

#ifndef VALUE_EMGRT
#define EACHBLOCKSIZE                           (2000*COORCONSTANT)
#define BLOCKCOUNTOFEACHDEGREE                  50
#define XBLOCKNUM                               18000
#define YBLOCKNUM                               9000
#define MAXLENOFROADOUTBLOCK                    ((2000)*COORCONSTANT)
#else
#define EACHBLOCKSIZE                           (4000*COORCONSTANT)
#define BLOCKCOUNTOFEACHDEGREE                  25
#define XBLOCKNUM                               9000
#define YBLOCKNUM                               4500
#define MAXLENOFROADOUTBLOCK                    ((2000)*COORCONSTANT)
#endif

#define ROAD_CLASS_IGNORE                       14
#endif//UONE

#ifndef FAR
#define FAR
#endif

// extend defines
#define USING_FILE_SETTING_VIEW_ANGLE
#define USING_FILE_SETTING_3D_PARAM_VIEW

#define _TMC_DISPLAY_MAP				0
#define _TMC_ROUTING					1
#define _TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY		2
#define _TMC_FLUSH_MAP					3
// ICON 
typedef struct tagNUROICONFILEINFO
{
    nuBYTE nuro[4];
    nuUINT version;
    nuUINT nFileLen;
    nuUINT nIconNum;
}NUROICONFILEINFO;

typedef struct tagNUROICONINFO
{
    nuWORD ID;
    nuWORD nW;
    nuWORD nH;
    nuBYTE nRes[2];
    nuUINT nStartPos;
}NUROICONINFO;

//?w?qPNG???Ƶ??c ??Louis20120911
typedef struct tagPNGCTRLSTRU
{
	int PngID;
	int bUseCount;
	int Width;
	int Height;
	unsigned char* Color;
}PNGCTRLSTRU,*PPNGCTRLSTRU;

//Added by Daniel ================Choose Road===START========== 20150207// 
#define _NO_CHOOSE							0xFF
#define _CURREN_ROAD_WEIGHT					80

//#define _USE_USER_CHOOSE_ROAD
#define _SHOW_CHOOSE_ROAD_NAME_COUNT		3
#define _CHOOSE_ROAD_DIS					50  //The Max Distance From Car To Road

#define _CHOOSE_MODE_DEFAULT			0
#define _CHOOSE_MODE_PASS_CURRENT_ROAD		1
#define _CHOOSE_MODE_USE_USER_CHOSEN_ROAD	2
//Added by Daniel ================Choose Road===End============= 20150207//
#define  NAME_SIZE        26
#define _VENDER_NECVOX							0x01
#define _VENDER_DAIWOO							0x02
#define _VENDER_ROTEK							0x03
#define _VENDER_ELEAD							0x04
#define _VENDER_PANA							0x05
#ifdef NURO_DEBUG
#include <stdio.h>
#endif

#define NURO_SET_UI_THREAD_CALLBACK	0x00000001
#define NURO_SET_NAVI_THREAD_CALLBACK	0x00000002 
#define NURO_SET_TIME_THREAD_CALLBACK	0x00000003
#define NURO_SET_SOUND_THREAD_CALLBACK	0x00000004
#define NURO_SET_OTHER_THREAD_CALLBACK	0x00000005

#define NURO_DEBUG

#define DRAW_MAP_FREE_BGL						0x01
#define DRAW_MAP_FREE_SEA						0x02
#define DRAW_MAP_FREE_MGR						0x04
#define DRAW_MAP_FREE_MDW						0x08

#endif // File nuDefine.h

