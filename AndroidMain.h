#include "NuroDefine.h"
#include "NuroCore.h"
#include "LoadCallMgrZK.h"

#define FILENAME_SIZE 256
#define Q_SIZE 20
#include "stdio.h"
class CAndroidMain : public CNuroCore
{
    typedef struct tagEventTableType{
        const nuCHAR *sID;
        nuINT e;
    } EventTableType;

public:

    //CAndroidMain();

    // Call
    nuCHAR m_sFileName[FILENAME_SIZE];

    // Event
    EventTableType m_EventTable[Q_SIZE];

    nuINT FindBlock(const nuCHAR *sID);
    nuINT FindFirstFreeBlock();
    nuBOOL AddEventTable(const nuCHAR *sID, nuINT e);
    nuBOOL SetEventTable(const nuCHAR *sID, nuINT e);
    nuBOOL GetEventTable(const nuCHAR *sID, nuINT& e);
    nuBOOL Transform(nuFLOAT fLng, nuFLOAT fLat, nuUINT unHeight, 
    nuUINT& unLngOut, nuUINT& unLatOut);
    nuBOOL DoTransform(nuUINT unLng, nuUINT unLat, nuUINT unHeight, 
    nuUINT& unLngOut, nuUINT& unLatOut);
    
 
    /// NuroCore methods

    nuBOOL SetNuroPath(JNIEnv * env, jclass cls, jstring path);

    nuBOOL SetNuroBuffer(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal1, nuINT nVal2);
    nuBOOL SetNuroPicBuffer(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal1, nuINT nVal2);

	nuBOOL SetNuroEventMap(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal1);
	nuBOOL InjectEventMap(JNIEnv * env, jclass cls, nuPVOID obj);
	nuINT GetFromEventMap(JNIEnv * env, jclass cls, nuPVOID obj);
	nuVOID SetToEventMap(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal);
	nuCHAR * GetPreparedAudio(JNIEnv * env, jclass cls);
    nuVOID SetPreparedAudio(JNIEnv * env, jclass cls);
	nuBOOL SetVolume(JNIEnv * env, jclass cls,nuINT );
	nuINT  GetVolume(JNIEnv * env, jclass cls);
	nuBOOL StartNuro(JNIEnv * env, jclass cls,nuPVOID pHud,nuPVOID pHuddr,nuPVOID pVoice,nuPVOID pCallBack);
    nuVOID StopNuro(JNIEnv * env, jclass cls);
	nuBOOL OnMouseDown(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2);
	nuBOOL OnMouseUp(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2);
	nuBOOL OnMouseMove(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2);
	nuBOOL onKeyDown(JNIEnv * env, jclass cls, nuINT nVal1);
	nuBOOL onKeyUp(JNIEnv * env, jclass cls, nuINT nVal1);
	nuBOOL SetGps(JNIEnv * env, jclass cls, nuPVOID obj);
	nuBOOL SetGpsNema(JNIEnv * env, jclass cls, jstring GpsData, jboolean bOK);
	nuBOOL SetGpsNema1(JNIEnv * env, jclass cls, jstring GpsData, jboolean bOK); // Added by Dengxu @ 2013 06 14
    nuBOOL SendPoiStringUTF16BE(JNIEnv * env, jclass cls, jbyteArray buffer);
    nuBOOL GetNuroState(JNIEnv * env, jclass cls);
    nuBOOL GoPreDialog(JNIEnv * env, jclass cls);
    nuBOOL PostMessage(JNIEnv * env, jclass cls,nuINT ID,nuINT n1,nuINT n2);
    nuBOOL SetKey(JNIEnv * env, jclass cls,nuINT ID,jstring);
   
   
    nuBOOL WindowProc(nuUINT message, nuDWORD wparam, nuDWORD lparam);
nuVOID COPYDATAOK();
    ////////////////////////////////
	nuBOOL SEARCHSDKCallBack(PCALL_BACK_PARAM pParam);
	nuBOOL GetNameALL(JNIEnv *env, jclass cls,jint nType, jobject object);
	nuBOOL GetCandidateWord(JNIEnv *env, jclass cls,jint nType, jobject object);
	nuBOOL GetKeyBoard(JNIEnv *env, jclass cls,jint nType, jobject object);
	nuBOOL GetListWord(JNIEnv *env, jclass cls,jint nType, jobject object);

	nuBOOL setSDKKEY(nuINT nMode,nuWCHAR *wsName);

    void PickUpPhone();
    void HangUpPhone();
    void SendPostion(int lat, int lng, int direction, int speed);
    void Dial(int nNo);
    void Volume(int n);
    void DestCancel();  
 //   void CalTMCCarData(nuPVOID pData);
 //   bool setLocation(PTMCCARDATA   m_pTmcCarData/* = NULL*/,int dataLen/* =0*/);
	
    ////////////////////////////////

    //add by chang;2014-11-9
//    void vSendPhoneNum(PCopyDataPhoneNum pPhoneNum);
    void setCityId(const char *cityId);

//----------------For SDK-----------------------//
     nuLONG OnCopyData(COPYDATASTRUCT* pCopyDataStruct);
     nuPVOID UIGetEngineData(nuLONG DataDef, nuPVOID pReturnData, nuPVOID pReturnData2);
     nuVOID MouseEvent(NURO_POINT2 Pt);
//----------------For SDK-----------------------//

#ifdef USE_HUD_DR_CALLBACK
	nuUINT setHUDcall(nuVOID*);
    	nuUINT setDRcall(nuVOID*);
#endif
#ifdef ANDROID
	nuUINT setSOUNDcall(nuVOID*);
#endif

	

 /*   static ONEKEYBUFF          s_SEND_OneKeyBuff;
    static OneKeyToNavi        s_OneKeyToNavi;
    static nuWCHAR             s_wPhoneNumber[20];
    static nuWCHAR             s_wHSMac_UID[20];
    static MAINCTRLVER         s_MainCtrlVer;
    static ShowPng             s_ShowPng;
*/
    static nuWCHAR             s_wHSMac_UID[20];
    #define _TMC_DATA_COUNT_ 10

typedef struct tagLOCATIONDIFF
{
	short  lon_diff;
	short  lat_diff;
	unsigned char   time_diff;
	unsigned char   speed;
	unsigned short   angle;
}LOCATIONDIFF,*PLOCATIONDIFF;

typedef struct tagLOCATION 
{
	unsigned short data_len;
	long long id;
	unsigned char  navi_stat;
	unsigned char  vertx_num;
	long  llong;
	long  llat;
	long  ltime;
	unsigned char  speed;
	unsigned short  angle;
	LOCATIONDIFF location_diff[9];
	char end[2];
}LOCATION ,*PLOCATION;

    	//wchar_t m_wCoorBuffer[512];
	LOCATION m_location;
	static UI_GET_DATA           s_UI_GET_DATA;   // EXE 靽?TOBE????DATA
	//unsigned char     m_navistat;
	//wchar_t m_wLocation[1024];
};


