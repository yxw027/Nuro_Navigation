#ifdef __cplusplus
extern "C"    {
#endif

#ifdef HZRECOG_EXPORTS
	#define	HZRECOGAPI __declspec( dllexport )
#else
	#define	HZRECOGAPI /*__declspec( dllimport )*/
#endif

#define HZ_VOID void
#define HZ_INT8 char
#define HZ_UINT8 unsigned char
#define HZ_INT16 short
#define HZ_UINT16 unsigned short
#define HZ_INT32 long				
#define HZ_UINT32 unsigned long		

#define HZ_BOOL HZ_INT32
#define HZ_BYTE HZ_UINT8			
#define HZ_WORD HZ_UINT16			
#define HZ_DWORD HZ_UINT32			

// Recognition Kernel Initialization
// call this function before using other functions in handwritting recognition kernel
// the pointer is ROM address for placing HZRecog.dat or is file name string
// Return value :	TRUE	success
//					FALSE	fail
HZRECOGAPI HZ_BOOL  HZInitCharacterRecognition(HZ_VOID* pPointer);

// Recognition Kernel Exit
// call this function before using other functions handwritting recognition kernel no more
// Return value :	TRUE	success
//					FALSE	fail
HZRECOGAPI HZ_BOOL  HZExitCharacterRecognition(HZ_VOID);

// Main Recognition Function
// Return value is recognition result number, its valid value are [-1, MAX_CANDIDATE_NUM]
// pnStrokeBuffer is handwritting buffer of Chinese character.
// (x, y) make up one point, and x and y are short type, their valid value are from -1 to 32767.
// (-1, 0) is end flag of stroke, (-1, -1) is end flag of character.
// pwResultBuffer is recognition result buffer, we recommend its size are MAX_CANDIDATE_NUM+1
// uMatchRange is one of four kinds of match_range or combination of them. Note: it is not null(zero)
// moreover, uMatchRange may include ADAPTATION_MATCH_RANGE only after calling 'HZInitAdaptation'
HZRECOGAPI HZ_INT32  HZCharacterRecognize(HZ_INT16* pnStrokeBuffer, HZ_WORD* pwResult);

#define RECOG_RANGE_NUMBER			0x00000001		// Digits, 0-9
#define RECOG_RANGE_UPPERCASE		0x00000002		// Uppercase English letters, 26
#define RECOG_RANGE_LOWERCASE		0x00000004		// Lowercase English letters, 26
#define RECOG_RANGE_INTERPUNCTION	0x00000008		// Punctuations and symbols, 43
#define RECOG_RANGE_GESTURE			0x00000010		// Gestures, 47
///// Below options not recommended 
#define RECOG_RANGE_GB1_ONLY		0x00000100
#define RECOG_RANGE_GB1_BIG51		0x00000200
#define RECOG_RANGE_GB1_BIG52		0x00000400
#define RECOG_RANGE_GB2_ONLY		0x00000800
#define RECOG_RANGE_GB2_BIG51		0x00001000
#define RECOG_RANGE_GB2_BIG52		0x00002000
#define RECOG_RANGE_BIG51_ONLY		0x00004000
#define RECOG_RANGE_BIG52_ONLY		0x00008000
#define RECOG_RANGE_GBK_ONLY		0x00010000
#define RECOG_RANGE_GB2_RADICAL		0x00020000
///// Above options not recommended
#define RECOG_RANGE_ADAPTATION		0x10000000		// Self-adaptation

// 3755 GB first level Chinese characters
#define RECOG_RANGE_GB1				(RECOG_RANGE_GB1_ONLY | RECOG_RANGE_GB1_BIG51 | RECOG_RANGE_GB1_BIG52)
// 3008 GB second level Chinese characters
#define RECOG_RANGE_GB2				(RECOG_RANGE_GB2_ONLY | RECOG_RANGE_GB2_BIG51 | RECOG_RANGE_GB2_BIG52)
// 5401 frequently used Big5 Chinese characters
#define RECOG_RANGE_BIG51			(RECOG_RANGE_GB1_BIG51 | RECOG_RANGE_GB2_BIG51 | RECOG_RANGE_BIG51_ONLY)
// 7659 non-frequently used Big5 Chinese characters
#define RECOG_RANGE_BIG52			(RECOG_RANGE_GB1_BIG52 | RECOG_RANGE_GB2_BIG52 | RECOG_RANGE_BIG52_ONLY)

// English uppercase and lowercase letters, 52
#define RECOG_RANGE_SYMBOL			(RECOG_RANGE_UPPERCASE | RECOG_RANGE_LOWERCASE)
// Total 6763 GB Chinese characters
#define RECOG_RANGE_GB				(RECOG_RANGE_GB1 | RECOG_RANGE_GB2)
// Total 13060 Big5 Chinese characters
#define RECOG_RANGE_BIG5			(RECOG_RANGE_BIG51 | RECOG_RANGE_BIG52)
// Total 21003 GBK Chinese characters
#define RECOG_RANGE_GBK				(RECOG_RANGE_GB | RECOG_RANGE_BIG5 | RECOG_RANGE_GBK_ONLY | RECOG_RANGE_GB2_RADICAL)
// All character code recognizable by the engine
#define RECOG_RANGE_ALL				(RECOG_RANGE_NUMBER | RECOG_RANGE_SYMBOL | RECOG_RANGE_INTERPUNCTION | RECOG_RANGE_GESTURE | RECOG_RANGE_GBK | RECOG_RANGE_ADAPTATION)

// DisplayPage
#define DP_NOCHANGE			0		// Display mode is depend on recognition model, do not change between simplified and traditional characters(default)
#define DP_TOSIMPLIFIED		1		// Display characters in simplified mode, change traditional characters into simplified characters
#define DP_TOTRADITIONAL	2		// Display characters in traditional mode, change simplified character into traditional characters

// Full or Half Character
#define FH_FULL				0		// DBC
#define FH_HALF				1		// SBC(default)

// Define Gesture
#define DG_GESTURE_NUM		47 
#define DG_ZERO				0x0000	// No gesture(default)
#define DG_BACKSPACE		0x0008	// Backspace
#define DG_ENTER			0x000D	// Enter
#define DG_DELETE			0x001E	// Delete
#define DG_UNDO				0x001A	// Undo
#define DG_COPY				0x001B	// Copy
#define DG_PASTE			0x001C	// Paste
#define DG_CUT				0x001D	// Paste
#define DG_SPACE			0x0020	// Space

// Define Slant Angle
#define SD_NONE				0		// No slant adjustment(default)
#define SD_90DEGREE			2		// Slant adjust in 90 degrees, 45 degrees in left side and 45 degrees in right side 
#define SD_180DEGREE		4		// Slant adjust in 180 degrees, 90 degrees in left side and 90 degrees in right side 
#define SD_360DEGREE		7		// Slant adjust in 360 degrees

typedef enum 
{
	PARAM_CANDNUMB,			// 1~50ㄛ10 is recommendedㄛdefault value is 10
	PARAM_RECORANG,			// RECOG_RANGE_xxxㄛdefault value is RECOG_RANGE_GB
	PARAM_DISPCODE,			// DP_xxx, default value is DP_NOCHANGE
	PARAM_FULLHALF,			// FH_xxxㄛdefault value is FH_FULL
	PARAM_DEFGESTURE,		// 47 unsigned short that define functions of 47 gestures
	PARAM_SPEEDUP,			// speed up recognition speed, but not recommended, default value is TRUE
	PARAM_LANGUAGE,			// set current active language, default value is english, only available for multilingual engine
	PARAM_SLANTDEGREE,		// SD_XXㄛdefault value is SD_NONE
	PARAM_CACHEBUFFER,		// set catch bufferㄛdefault is NULL or not set
} HZPARAM;

HZRECOGAPI HZ_VOID HZSetParam(HZPARAM nParam, HZ_UINT32 dwValue);
HZRECOGAPI HZ_UINT32 HZGetParam(HZPARAM nParam);
int hwtest();
#define HWBIG5WORD   0x00
#define HWGBWORD     0x01

#ifdef __cplusplus
}
#endif 



