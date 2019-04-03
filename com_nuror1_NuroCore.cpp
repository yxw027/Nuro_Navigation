#include "com_nuror1_NuroCore.h"
#include "NuroCore.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.inc"
#include "NuroOpenedDefine.h"
#include "NURO_DEF.h"
//#include "mmfdll.h"
#include <android/log.h>

#include "NuroSearchSDK.h"
#include "nuSearchDefine.h"

JavaVM *g_jvm = NULL;
jobject g_NuroCoreJavaObject = NULL;
jclass g_NuroCoreCls = NULL;

jobject g_SearchOKObject = NULL;
nuBOOL bStartNuro = nuFALSE;
static nuINT s_nVoice;
static JNIEnv *g_pCCallJAVAEnv = NULL;
static jclass    m_c_Jni;    
static jmethodID m_c_jni_mid,m_c_jni_mid2,m_c_jni_mid_newhud,m_c_jni_mid_newhudb; 
static jclass JNI_X_Jni = NULL;
static jclass    m_c_Jni3;    
static jmethodID m_c_jni_mid3,m_c_jni_mid_getNameALL; 


static jclass DrawInfoClass = NULL;
static jobject  DrawInfoobject = NULL;
static jclass   RoadListClass = NULL;

static jfieldID DrawMapInfoField = NULL;
static jobject DrawMapInfoObject = NULL;
static jclass  DrawMapInfoClass  = NULL;

static  jfieldID DrawNaviInfoField = NULL;
static  jobject  DrawNaviInfoObject = NULL;
static  jclass   DrawNaviInfoClass = NULL;

static jfieldID  DrawInfoLaneGuideField = NULL;
static jobject   DrawInfoLaneGuideObject = NULL;
static jclass    DrawInfoLaneGuideClass = NULL;

static jclass m_c_Jni_playSound 	   = NULL; 
static jmethodID m_c_jni_mid_playSound = NULL;

static jmethodID m_c_Jni_midBmp,m_c_jni_mid_SetState,m_c_jni_mid_Exit; 
jclass    m_c_Jni_Voice; 
//Prosper 2014.12.26, add new data call back
static nuUINT SetHUD(nuLONG lRoadtype, nuLONG lTurnFlag, nuLONG lSpeed, nuLONG lDistance, nuLONG CCD, nuLONG nOver,nuLONG ,nuLONG ,nuLONG,nuBYTE *,GPSDATA TempGpsData,nuroPOINT Now_Pos);
static nuUINT SetHudDR(long nLat,long nLng,double nAngle,long nLatFix,long nLngFix,double nAngleFix,bool tunnel,long nDRAngle,long nSpeed);
static nuUINT SetJNISound(nuINT nVoice,nuTCHAR *tsFilePath);

static bool bSetMMF=false;
static bool bSetInit=false;

static void *dl_handle = NULL;
//Prosper 2014.12.26 ,add new method call back
static nuUINT NaviThreadCallBack(PCALL_BACK_PARAM pParam);

static nuUINT SoundThreadCallBack(PCALL_BACK_PARAM pParam);

//Prosper 2015.08.17 ,add new method call back
static nuUINT UISetEngineCallBack(PCALL_BACK_PARAM pParam);

//extern void SetHUD(nuLONG lRoadtype, nuLONG lTurnFlag, nuLONG lSpeed, nuLONG lDistance, nuLONG CCD);
//olean Java_com_nuror1_NuroCore_onMouseDown(JNIEnv * env, jclass cls, jint nVal1, jint nVal2)
//

//extern void SetHudDR(long nLat,long nLng,double nAngle,long nLatFix,long nLngFix,double nAngleFix,bool tunnel);

#define JOIN_KEY_ENTER 28
#define JOIN_KEY_BACK  1 
#define JOIN_KEY_UP	  103
#define JOIN_KEY_DOWN  108
#define JOIN_KEY_LEFT  105
#define JOIN_KEY_RIGHT 106

#define JOIN_KEY_UP_2  19
#define JOIN_KEY_DOWN_2  20
#define JOIN_KEY_LEFT_2  21
#define JOIN_KEY_RIGHT_2 22
#define JOIN_KEY_ENTER_2 66

#define JOIN_KEY_LEFTBRACE  26
#define JOIN_KEY_RIGHTBRACE 27



static JNIEnv* GetEnv()
{
	JNIEnv *env;
	if( g_jvm->GetEnv((void**) &env, JNI_VERSION_1_6 ) != JNI_OK )
 	{
		jint status = g_jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) 
		{
            //sLOG(ANDROID_LOG_DEBUG, "JNI", "callback_handler: failed to attach current thread");
			env = NULL;
		}
	}
	return env;
}

/********************************* jni implementation ***************************/

JNIEXPORT JNICALL jboolean Java_com_nuror1_NuroCore_setNuroPath (JNIEnv *env, jclass cls, jstring path)
{
    return g_NuroCore->SetNuroPath(env, cls, path);
}

JNIEXPORT jboolean Java_com_nuror1_NuroCore_setNuroBuffer(JNIEnv * env, jclass cls, jobject obj, jint nVal1, jint nVal2)
{ 
	__android_log_print(ANDROID_LOG_INFO, "navi", "Java_com_nuror1_NuroCore_setNuroBuffer OK.....");
	return g_NuroCore->SetNuroBuffer(env, cls, obj, nVal1, nVal2);
}

jboolean Java_com_nuror1_NuroCore_setNuroPicBuffer(JNIEnv * env, jclass cls, jobject obj, jint nVal1, jint nVal2)
{ 
	__android_log_print(ANDROID_LOG_INFO, "Hsin", "Java_com_nuror1_NuroCore_setNuroPicBuffer OK.....");
	return g_NuroCore->SetNuroPicBuffer(env, cls, obj, nVal1, nVal2);
}

JNIEXPORT jboolean Java_com_nuror1_NuroCore_injectEventMap(JNIEnv * env, jclass cls, jobject obj)
{
	return g_NuroCore->InjectEventMap(env, cls, obj);
}

JNIEXPORT jint Java_com_nuror1_NuroCore_getFromEventMap(JNIEnv * env, jclass cls, jobject obj)
{
	return g_NuroCore->GetFromEventMap(env, cls, obj);
}

JNIEXPORT void Java_com_nuror1_NuroCore_setToEventMap(JNIEnv * env, jclass cls, jobject obj, jint nVal)
{
	return g_NuroCore->SetToEventMap(env, cls, obj, nVal);
}

JNIEXPORT jstring Java_com_nuror1_NuroCore_getPreparedAudio(JNIEnv * env, jclass cls)
{
	return stoJstring(env, g_NuroCore->GetPreparedAudio(env, cls));
}

JNIEXPORT void Java_com_nuror1_NuroCore_setPreparedAudio(JNIEnv * env, jclass cls)
{
	g_NuroCore->SetPreparedAudio(env, cls);
}

JNIEXPORT jint Java_com_nuror1_NuroCore_getVolume(JNIEnv * env, jclass cls)
{
    return s_nVoice;
	// return g_NuroCore->GetVolume(env, cls);
}
JNIEXPORT jboolean Java_com_nuror1_NuroCore_setVolume(JNIEnv * env, jclass cls,jint nVoice)
{
	return g_NuroCore->SetVolume(env, cls,nVoice);
}

JNIEXPORT jboolean Java_com_nuror1_NuroCore_startNuro(JNIEnv * env, jclass cls)
{
	__android_log_print(ANDROID_LOG_INFO, "navi", "First bStartNuro %d.....",bStartNuro);
	bStartNuro = g_NuroCore->StartNuro(env, cls,(void *)UISetEngineCallBack,(void *)SetHudDR,(void *)SetJNISound,(void *)NaviThreadCallBack); 
	__android_log_print(ANDROID_LOG_INFO, "navi", "bStartNuro %d",bStartNuro);
	return bStartNuro;  
}

JNIEXPORT void Java_com_nuror1_NuroCore_stopNuro(JNIEnv * env, jclass cls)
{
	g_NuroCore->StopNuro(env, cls);
}

jboolean Java_com_nuror1_NuroCore_getNuroState(JNIEnv * env, jclass cls)
{
        return g_NuroCore->GetNuroState(env, cls);
}

jboolean Java_com_nuror1_NuroCore_goPreDialog(JNIEnv * env, jclass cls)
{
	return g_NuroCore->GoPreDialog(env, cls);
}

jboolean Java_com_nuror1_NuroCore_onMouseDown(JNIEnv * env, jclass cls, jint nVal1, jint nVal2)
{
        jboolean b = g_NuroCore->OnMouseDown(env, cls, nVal1, nVal2);
	return b;
}

jboolean Java_com_nuror1_NuroCore_onMouseUp(JNIEnv * env, jclass cls, jint nVal1, jint nVal2)
{
	return g_NuroCore->OnMouseUp(env, cls, nVal1, nVal2);
}

jboolean Java_com_nuror1_NuroCore_onMouseMove(JNIEnv * env, jclass cls, jint nVal1, jint nVal2)
{
	return g_NuroCore->OnMouseMove(env, cls, nVal1, nVal2);
}
jboolean Java_com_nuror1_NuroCore_onKeyDown(JNIEnv * env, jclass cls, jint nVal1)
{
	nuINT NaviKeyCode = 0;
	if(!NaviKeyCode)
	return true;
	//__android_log_print(ANDROID_LOG_INFO, "NaviKeyCode", "NaviKeyCode %d",NaviKeyCode);
	return g_NuroCore->onKeyDown(env, cls, NaviKeyCode);
}


jboolean Java_com_nuror1_NuroCore_onKeyUp(JNIEnv * env, jclass cls, jint nVal1)
{
	nuINT NaviKeyCode = 0;
	if(!NaviKeyCode)
	return true;
	return g_NuroCore->onKeyUp(env, cls, NaviKeyCode);
}
JNIEXPORT jboolean Java_com_nuror1_NuroCore_setGps(JNIEnv * env, jclass cls, jobject obj)
{
	return g_NuroCore->SetGps(env, cls, obj);
}

static nuBOOL bSet = nuFALSE;

JNIEXPORT jboolean Java_com_nuror1_NuroCore_setGpsNema(JNIEnv * env, jclass cls, jstring GpsData,jboolean  bOK)
{
	if(bStartNuro)
	return g_NuroCore->SetGpsNema(env, cls, GpsData, bOK);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env;
	if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
	{	
		return -1;
	}
	g_jvm = vm;
	jclass cls = env->FindClass("com/nuror1/NuroCore");
	jmethodID constr = env->GetMethodID(cls, "<init>", "()V");
	jobject obj = env->NewObject(cls, constr);
	g_NuroCoreJavaObject = env->NewGlobalRef(obj);
	return JNI_VERSION_1_6;
}


void JniUnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) 
    {
        return;
    } 
    env->DeleteGlobalRef(g_SearchOKObject);
    env->DeleteGlobalRef(g_NuroCoreJavaObject);

    env->DeleteGlobalRef(DrawInfoobject);
    env->DeleteGlobalRef(RoadListClass);
    env->DeleteGlobalRef(DrawMapInfoObject);
    env->DeleteGlobalRef(DrawMapInfoClass);
    env->DeleteGlobalRef(DrawNaviInfoObject);
    env->DeleteGlobalRef(DrawNaviInfoClass);
    env->DeleteGlobalRef(DrawInfoLaneGuideObject);
    env->DeleteGlobalRef(DrawInfoLaneGuideClass);    
}


jbyte  b1[15]  = {0};
		
nuUINT SetHUD(nuLONG lRoadtype, nuLONG lTurnFlag, nuLONG lSpeed, nuLONG lDistance, nuLONG CCD , nuLONG  lOVER , nuLONG lNavi,nuLONG licon,nuLONG lRoadSpeedWarning,nuBYTE *pBuffer,GPSDATA TempGpsData,nuroPOINT Now_Pos)
{
   // return 0;
   // __android_log_print(ANDROID_LOG_INFO, "dengxu", "SETHUD");

    //ConnectSocket();
    JNIEnv*env= GetEnv();

#ifndef CALLBACK        
#ifdef  hud_old     
    jobject newObject   = env->NewObject(m_c_Jni, m_c_jni_mid, (jlong)lRoadtype, (jlong)lTurnFlag, 
            (jlong)lSpeed, (jlong)lDistance, (jlong)CCD, (jlong)lOVER , (jlong)lNavi, 
            (jlong)licon, (jlong)lRoadSpeedWarning);
#else
    nuMemset(&b1, 0, sizeof(b1));     
    nuMemcpy(b1, pBuffer, 15);
	
	nuLONG t1=0,t2=0;
	int h=0;
	t1 = TempGpsData.nTime.nYear*10000+TempGpsData.nTime.nMonth*100+TempGpsData.nTime.nDay; 
	 if(TempGpsData.nTime.nHour>16)
		h=TempGpsData.nTime.nHour-8;
	else
		h=TempGpsData.nTime.nHour;
	t2 = h*10000+TempGpsData.nTime.nMinute*100+TempGpsData.nTime.nSecond; 


    {
        char buf[480];
        sprintf(buf, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", t1, t2,TempGpsData.nTime.nYear,TempGpsData.nTime.nMonth,TempGpsData.nTime.nDay,TempGpsData.nTime.nHour,TempGpsData.nTime.nMinute,TempGpsData.nTime.nSecond, (nuLONG)Now_Pos.x, (nuLONG)Now_Pos.y);
       // __android_log_print(ANDROID_LOG_INFO, "dengxu", buf);
    }

    jobject newObject2   = env->NewObject(JNI_X_Jni, m_c_jni_mid,
            (jlong)lRoadtype, (jlong) lTurnFlag, (jlong) lSpeed, (jlong) lDistance, (jlong) CCD, (jlong) lOVER , 
            (jlong)lNavi,(jlong) licon,(jlong) lRoadSpeedWarning
            ,(jbyte)pBuffer[0],(jbyte)pBuffer[1],(jbyte)pBuffer[2],(jbyte)pBuffer[3],(jbyte)pBuffer[4],(jbyte)pBuffer[5],
            (jbyte)pBuffer[6],(jbyte)pBuffer[7],(jbyte)pBuffer[8],(jbyte)pBuffer[9],(jbyte)pBuffer[10],(jbyte)pBuffer[11],
	      (jlong)Now_Pos.x,(jlong)Now_Pos.y,(jlong)TempGpsData.nAngle,(jlong)t1,(jlong)t2);

   
#endif
   
    env->DeleteLocalRef(newObject2);
    g_jvm->DetachCurrentThread();
#endif
    return 0;
}

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

void Cal_NMEA_Chksum(nuCHAR* str,nuCHAR *chk)
{
 	nuCHAR cs=0;
 
 	nuINT len=nuStrlen(str);

	for(nuINT i=0;i<len;i++) 
 	{
  		cs = cs ^ str[i];
 	}

	nuCHAR s[]="0123456789ABCDEF";
	chk[0]=s[cs/16];
	chk[1]=s[cs%16];
}

extern "C" bool MMF_LoadSo(int time)
{
    return false;
#if 0
  //void *dl_handle;
  //typedef void (*func)(char*, int); 
  typedef void (*func_WriteCom)(char*, int);
  typedef void  (*func_GetInit)(int GpsTime, int* x1, int* x2, int* x3);

  func_GetInit  f1 = NULL;
  func_WriteCom f2 = NULL;
  
  //char *error=0;

  /* Open the shared object */
  dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
  if (!dl_handle) {
    printf( "!!! %s\n", dlerror() );
    return 0;
  }    
  return 1;

  //f1 = (func_GetInit)dlsym( dl_handle, "tkmmf_GetInitNum" );
  /*error=dlerror();
  if (error != NULL) {
    printf( "!!! %s\n", error );
    return 0;
  }*/

  int x1 =0, x2=0, x3=0;
  
  f1(time, &x1, &x2, &x3);
  
  	char cNumber[10]={0};
	   nuCHAR cs[256]={0};
	   nuCHAR ss[3]={0};
	  nuStrcat(cs,"$DRDLL,");
	  nuItoa(x1, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
          nuItoa(x2, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuItoa(x3, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  //nuStrcat(cs,",");
	  Cal_NMEA_Chksum(cs,ss);	
	  nuStrcat(cs,"*");
	  nuStrcat(cs,ss);
	  
	 int len=strlen(cs);
	  cs[len]=0x0D;
          cs[len+1]=0x0A;
	   
	f2 = (func_WriteCom)dlsym( dl_handle, "tkmmf_WriteCOM" );
		
	f2(cs,strlen(cs));
	/*FILE *fp = fopen("/mnt/sdcard/extsd/DR_Str1.txt", "a+");
    	fprintf(fp, "--->>> x1:%d x2:%d x3:%d %s\n",x1,x2,x3 , cs);
    	fclose(fp);
	*/

	JNIEnv*env= GetEnv();

	  jstring StrMMF = env->NewStringUTF(cs);
	  
	 /* jobject newObject   = env->NewObject(m_c_Jni, m_c_jni_mid2,
					    (jstring)StrMMF);
	  */
	  
	  env->DeleteLocalRef(StrMMF);
        /* Close the object */
       dlclose( dl_handle );
#endif
}
extern "C" bool MMF_CheckInit(int x1,int x2,int x3)
{
#if 0
	void *dl_handle;
	//void  (*func_GetInit)(int GpsTime, int* x1, int* x2, int* x3);
	char *error;

	typedef int   (*func_CheckInitNum)(int r1, int r2, int r3);
	func_CheckInitNum f = NULL;
	
	  /* Open the shared object */
	  dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  }    

	  /* Resolve the symbol (method) from the object */
	  f = (func_CheckInitNum)dlsym( dl_handle, "tkmmf_CheckInitNum" );
	
	  bool bRes=f(x1,x2,x3);
	 /* FILE *fp = fopen("/mnt/sdcard/extsd/DRBOOL.txt", "a+");
    	  fprintf(fp, "--->>> %d",bRes);
    	  fclose(fp);
	*/
	  dlclose( dl_handle );	
	return bRes;
#endif
    return false;
}

extern "C" bool MMF_SetInit()
{
    return false;
	/**************************************
		Need ADD CONFIG.INI
	***************************************/
	
	
	void *dl_handle;
	//void  (*func_GetInit)(int GpsTime, int* x1, int* x2, int* x3);
	char *error;

	typedef void  (*func_1)(float fAngle);

	typedef void  (*func_2)(int time);

	typedef void  (*func_3)(int speed);

	func_1 f1 = NULL;
	func_2 f2 = NULL;
	func_3 f3 = NULL;

	  /* Open the shared object */
	  dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  }    

	  /* Resolve the symbol (method) from the object */
	  f1 = (func_1)dlsym( dl_handle, "tkmmf_SetLimitAngle" );
	
	  f1(1.5);
	  f2 = (func_2)dlsym( dl_handle, "tkmmf_SetLimitTime" );
          f2(20);
	  
	 f3 = (func_3)dlsym( dl_handle, "tkmmf_SetLimitSpeed" );
	 f3(20);
	
	  
	
	  dlclose( dl_handle );	
}

extern "C" bool MMF_ResetMMF()
{
    return false;
	//void *dl_handle;
	typedef void  (*func_setResetMMF)(void);

	char *error;

	func_setResetMMF f = NULL;

	
	  /* Open the shared object */
	  /*dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  }    
	 */
	  /* Resolve the symbol (method) from the object */
	  f = (func_setResetMMF)dlsym( dl_handle, "tkmmf_ResetMMF" );
	
	  f();
	
	 // dlclose( dl_handle );	
}

extern "C" bool MMF_GetPred(long in_x, long in_y, float in_angle,long in_speed, float AngularChange,long gps_x,long gps_y,long gps_angle)
{
    return false;
	//void *dl_handle;
	typedef void  (*func_GetPOS)(long in_x, long in_y, float in_angle, long in_speed, float AngularChange, 
								  long* out_x, long* out_y, float* out_angle);
	char *error;

	func_GetPOS f = NULL;

	
	  /* Open the shared object */
	  /*dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  }    
	*/
	  /* Resolve the symbol (method) from the object */
	  f = (func_GetPOS)dlsym( dl_handle, "tkmmf_GetPredicPos" );
	  
	  //int out_x = 0, int out_y = 0, float out_angle =0;
	  long out_x = 0, out_y = 0;
	  float out_angle =0;
	  f(in_x, in_y, in_angle, in_speed, AngularChange,&out_x, &out_y,  &out_angle);
	
	  //int out_angle2 = (int)out_angle;
	float fgps_angle = gps_angle/1.0;
	MMF_MODFIFY(out_x, out_y,out_angle,AngularChange,in_speed, 
								fgps_angle, gps_y, gps_x);

	//  dlclose( dl_handle );	
}

extern "C" bool MMF_MODFIFY(long in_x, long in_y,float in_angle, float AngularChange, long fCarSpeed, 
								float gps_angle, long gps_posX, long gps_posY)
{
    return false;
	//void *dl_handle;
	
	typedef int  (*func_MODFIFY)(long in_x, long in_y,float in_angle, float AngularChange, long fCarSpeed, 
								float gps_angle, long gps_posX, long gps_posY, 
								long* out_x, long* out_y, float* out_angle);
	func_MODFIFY f1 = NULL ;

	typedef void (*func_com)(char* data, int iLength);
	
	func_com f2 = NULL;

	char *error;

	
	  /* Open the shared object */
	  /*dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  } */   

	  /* Resolve the symbol (method) from the object */
	   f1 = (func_MODFIFY)dlsym( dl_handle, "tkmmf_ModifyRtoem" );
	  long out_x = 0, out_y = 0;
	  float out_angle =0;
	  int nRes=f1(in_x,in_y,in_angle, AngularChange, fCarSpeed,gps_angle, gps_posX, gps_posY,&out_x, &out_y,  &out_angle);
	  if(nRes==1 )
	{

	   char cNumber[10]={0};
	   nuCHAR cs[256]={0};
	   nuCHAR ss[3]={0};
	  
	  nuStrcat(cs,"$MMF,");
	  int nn=out_y;
          nuItoa(nn*10, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A,");
	  int n=out_x;
	  nuItoa((n*10), cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A,");
	  int nnn=out_angle;
	  nuItoa(nnn*10, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A");
          Cal_NMEA_Chksum(cs,ss);	
	  nuStrcat(cs,"*");
	  nuStrcat(cs,ss);
	  int len=strlen(cs);
	  cs[len]=0x0D;
          cs[len+1]=0x0A;
	   f2 = (func_com)dlsym( dl_handle, "tkmmf_WriteCOM" );
	
	    MMF_ResetMMF();	   
	    f2(cs, strlen(cs));
	  
 	/*    FILE *fp2 = fopen("/mnt/sdcard/extsd/DR_MMF.txt", "a+");
	    fprintf(fp2,"angle:%d x:%d y:%d \n",gps_angle, gps_posX, gps_posY);
	    fprintf(fp2,"%s\n",	cs);    	 
            fclose(fp2);  
	 */
 		bSetMMF=true;
          }
	else
	{
 	   char cNumber[10]={0};
	   nuCHAR cs[256]={0};
	   nuCHAR ss[3]={0};
	   nuStrcat(cs,"$MMF,");
	  int nn=out_y;
          nuItoa(nn*10, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A,");
	  int n=out_x;
	  nuItoa((n*10), cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A,");
	  int nnn=out_angle;
	  nuItoa(nnn*10, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuStrcat(cs,"A");
          Cal_NMEA_Chksum(cs,ss);	
	  nuStrcat(cs,"*");
	  nuStrcat(cs,ss);
	  int len=strlen(cs);
	  cs[len]=0x0D;
          cs[len+1]=0x0A;

	  /*  FILE *fp2 = fopen("/mnt/sdcard/extsd/DR_NoMMF.txt", "a+");
	    fprintf(fp2,"angle:%d x:%d y:%d \n",gps_angle, gps_posX, gps_posY);
	    fprintf(fp2,"%s\n",	cs);    	 
            fclose(fp2);*/
	}

	
	  
}

extern "C" bool MMF_ClOSE()
{
    return false;
	dlclose( dl_handle );	
}

/*
extern "C" bool LoadSo(int nMode,long nLat,long nLng,double nAngle,long nLatFix,long nLngFix,double nAngleFix) {
	
	void *dl_handle;	
	
	void  (*func_)(int r1, int r2, int r3); 

	
	
	
	

  	char *error;


	  dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
	  if (!dl_handle) {
	    printf( "!!! %s\n", dlerror() );
		return 0;
	  }    

	  void* vf = dlsym( dl_handle, "tkmmf_WriteCOM" );
	
	  
	//if(nMode==0)	
	int x1,x2,x3;
	//tkmmf_GetInitNum(123,&x1,&x2,&x3);
 (*func)("$PSRF104,0,0,0,96000,237759,922,12,8*22\r\n", 41);
	  char cNumber[10]={0};
	   nuCHAR cs[256]={0};
	   nuCHAR ss[3]={0};
	  nuStrcat(cs,"$DRDLL,");
	  nuItoa(x1, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
          nuItoa(x2, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  nuItoa(x3, cNumber, 10);
	  nuStrcat(cs,cNumber);
	  nuStrcat(cs,",");
	  Cal_NMEA_Chksum(cs,ss);	
	  nuStrcat(cs,"*");
	  nuStrcat(cs,ss);
		
	  //tkmmf_WriteCOM(cs, strlen(cs));

 
	//if(nMode==1)	          
	if(0 == tkmmf_CheckInitNum(r1,r2,r3))
	  {
	
          }
	  else
	  {
	 	
	  }
	
	tkmmf_SetLimitAngle(1.5);
	tkmmf_SetLimitTime(20);
	tkmmf_SetLimitSpeed(20);
	typedef struct DYPoint {
		int x;
		int y;
	};

	DYPoint in_pos={0},gps_pos={0},out_pos={0},predic_pos={0};
	int 	gps_angle=0,speed=0;
	float 	AngularChange=0,in_angle=0,out_angle=0;
	//if(nMode==2)	
	//tkmmf_GetPredicPos(gps_pos.x,gps_pos.y,gps_angle,speed,AngularChange,&in_pos.x,&in_pos.y,&in_angle);
	
		

	if(nMode==10)
	  dlclose( dl_handle );
}
*/


nuUINT SetHudDR(long nLat,long nLng,double nAngle,long nLatFix,long nLngFix,double nAngleFix,bool tunnel,long DRAngle,long nSpeed)
{

    return 0;
	if(bSetInit==false)
	{
	   /* FILE *fp = fopen("/mnt/sdcard/extsd/LOAD_MMF.txt", "a+");
	    fclose(fp);  */

	  MMF_LoadSo(123);
	  MMF_ResetMMF();
	
	   /* FILE *fp2 = fopen("/mnt/sdcard/extsd/LOAD_MMF_OK.txt", "a+");
	    fclose(fp2);  */

          bSetInit =true ; 		
	}	
	if(nLat==0) //SetTime , LoadSo
	{	
		if(bSetInit==false)
		{
		
		char cs[128]={0};
		char cNumber[10]={0};
		int time;
		int h=(int)nLng,m=(int)nAngle,s=(int)nLatFix;
		h-=8;		
		if(h<0)
		h+=24;		
		nuItoa(h, cNumber, 10);
		nuStrcat(cs,cNumber);
		nuItoa(m, cNumber, 10);
		if(m<10)
		nuStrcat(cs,"0");
		nuStrcat(cs,cNumber);
		nuItoa(s, cNumber, 10);
		if(s<10)
		nuStrcat(cs,"0");
		nuStrcat(cs,cNumber);
		time = atoi(cs);		
		/*FILE *fp = fopen("/mnt/sdcard/extsd/DR1.txt", "a+");
    	  	fprintf(fp, "--->>> %d %s\n",time , cs);
    	  	fclose(fp);*/		
		//MMF_LoadSo(time);
		//MMF_ResetMMF();		
		//MMF_Init(time);
		/*FILE *fp2 = fopen("/mnt/sdcard/extsd/DR1OK.txt", "a+");
    	  	fprintf(fp2, "--->>> %d %s\n",time , cs);
    	  	fclose(fp2);*/		
		bSetInit=true;		
		//FILE *fp2 = fopen("/mnt/sdcard/extsd/DR1OK.txt", "a+");		
		//fclose(fp2);		
		}
		//LoadSo(0,(int)nLng,(int)nAngle,(int)nLatFix,0,0,0);
		return true;		
	}
	if(nLat==1) //SetTime , LoadSo
	{
		//return 0;
		/*FILE *fp = fopen("/mnt/sdcard/extsd/DR2.txt", "a+");
    	  	fprintf(fp, "--->>> %d %d %d\n",(int)nLng,(int)nAngle,(int)nLatFix);
    	  	fclose(fp);*/
		bool bRes=MMF_CheckInit((int)nLng,(int)nAngle,(int)nLatFix);
		if(bRes)
		{
		  MMF_SetInit();			
		}
		/*FILE *fp2 = fopen("/mnt/sdcard/extsd/DR2OK.txt", "a+");
    	  	//fprintf(fp, "--->>> %d %d %d\n",(int)nLng,(int)nAngle,(int)nLatFix);
    	  	fclose(fp2);*/
		return true;
		//LoadSo(1,(int)nLng,(int)nAngle,(int)nLatFix,0,0,0);
	}
	if(nLat==2)
	{
		MMF_ResetMMF();
		
		return true;
	}
	  if(tunnel == 1 )
	{
	 
	   float angle1,angle2;
	   angle1=nAngleFix/1000.0;
	   angle2=DRAngle/1000.0;
	   //long nLatFix,long nLngFix,double nAngleFix

	   //FILE *fp = fopen("/mnt/sdcard/extsd/DR_GetPred.txt", "a+");
    	   //fprintf(fp, "POS: %d %d angle:%f speed: %d  DRAngle: %f\n",nLat,nLng,angle1,nSpeed,angle2);
           //fclose(fp);
	   
 	    
           //FILE *fp2 = fopen("/mnt/sdcard/extsd/DR_GetPred2.txt", "a+");
    	 //fclose(fp2); 
           float angle3 =(float) angle1;// - 90.0;
/*
	   if(angle3 < 0)
	   {
		angle3+=360.0;
	   }*/
	   long langle=nAngle - 90;
	   if(langle < 0)
	   {
		langle+=360;
	   }
	   /*
	   FILE *fp = fopen("/mnt/sdcard/extsd/DR_Parser.txt", "a+"); 
	   fprintf(fp, "POS: %d %d angle:%f speed: %d  DRAngle: %f\n" ,nLat,nLng,angle1,nSpeed,angle2);
	   fprintf(fp, "FIX_POS: %d %d angle:%f \n\n" ,nLatFix,nLngFix,nAngleFix);
	   fclose(fp);
	   */
	   //MMF_GetPred(nLat,nLng,angle1,nSpeed,angle2 );
 
           MMF_GetPred(nLatFix,nLngFix,angle3,nSpeed,angle2,nLat, nLng,langle);
	   
	   return true;	        
	/*    void *dl_handle;
  	   float (*func)(char*, int);
           char *error;

          dl_handle = dlopen( "/libMMF.so", RTLD_LAZY );
          if (!dl_handle) {
          printf( "!!! %s\n", dlerror() );
          return 0;
         }    

         func = dlsym( dl_handle, "tkmmf_WriteCOM" );
	  error = dlerror();
	  if (error != NULL) {
	    printf( "!!! %s\n", error );
	    return 0;
	  }
  
	nuINT 	x1,x2,x3;
	nuINT   GpsTime=(063345.000);
	tkmmf_GetInitNum(GpsTime, &x1, &x2, &x3 );

  //(*func)("$PSRF104,0,0,0,96000,237759,922,12,8*22\r\n", 41);

  dlclose( dl_handle );
	  return 0;
*/
	  //MMF();

    return 0;

	   int i=0;
	  long nMMF_Lat  = ( nLatFix - nLat ) * 10 ;
	  long nMMF_Lng  = ( nLngFix - nLng ) * 10 ;
	  double nMMF_Ang = ( nAngleFix - nAngle ) * 1000 ;
	  if(nMMF_Ang>3600)
	  {
			nMMF_Ang-=3600;
	  }
	  if(nMMF_Ang<-3600)
	  {
			nMMF_Ang+=3600;
	  }
	  char sMMF[100] = "$MMF,";
	  char sTmp[10] = {0};
	  nuItoa( nMMF_Lat, sTmp, 10 );
	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A,");
	  nuItoa( nMMF_Lng, sTmp, 10 );
	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A,");
	  nuItoa( nMMF_Ang, sTmp, 10 );
	  int tmplen=nuStrlen(sTmp);
	  sTmp[tmplen]=sTmp[tmplen-1];
	  sTmp[tmplen-1]=sTmp[tmplen-2];
  	  sTmp[tmplen-2]='.';

	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A*");
	  char CheckSum = 0;
	  for( i = 1; i < nuStrlen(sMMF); i++)
	  {
		  if( sMMF[i] == '*' )
		  {
			  break;
		  }
		  else
		  {
			CheckSum = CheckSum^sMMF[i];
		  }
	  }
	  nuItoa( CheckSum, sTmp, 16 );
	  if(sTmp[1] == 0)
	  {
		  sTmp[1] = sTmp[0];
		  sTmp[0] = '0';
		  sTmp[2] = 0;
	  }
	  nuStrcat( sMMF, sTmp );
	  int nLen = nuStrlen(sMMF);
	  sMMF[nLen] = 13;
	  sMMF[nLen + 1] = 10;
	  sMMF[nLen + 2] = 0;

	  JNIEnv*env= GetEnv();

	  jstring StrMMF = env->NewStringUTF(sMMF);
	  
	 /* jobject newObject   = env->NewObject(m_c_Jni, m_c_jni_mid2,
					    (jstring)StrMMF);
	  */
	  
	  env->DeleteLocalRef(StrMMF);
	 // env->DeleteLocalRef(newObject);
	}
//	  return sMMF;
//SetSoundPath("0",0,0);
}

static int s_soundflag = 1;
static char *s_path = NULL;
char _szBuff[1024]={0};

char *strfiltercpy(char *des, char *src, int crep, int ctorep)
{
    int c;
    char *r_des = des;
    while (*src)
    {
        c = *src;
        
        if (c == ctorep)
        {
            *des = crep;
        }
        else
        {
            *des = *src;
        }
    
        src++;
        des++;
    }

    *des = '\0';

    return r_des;
}

nuUINT SetJNISound(nuINT nVoice, nuTCHAR *tsFilePath)
{
    //char buf[256];
    
    s_nVoice = nVoice;
   
	JNIEnv*env= GetEnv();
	jstring StrMMF=NULL;

	strfiltercpy(_szBuff, tsFilePath, '/', '\\');
	s_path = _szBuff;
	s_soundflag = 0;
	StrMMF = env->NewStringUTF(s_path);

	jobject newObject   = env->NewObject(m_c_Jni, m_c_jni_mid_playSound,
					    (jlong)nVoice,(jstring)StrMMF);
	env->DeleteLocalRef(newObject);
	env->DeleteLocalRef(StrMMF);

   // __android_log_print(ANDROID_LOG_INFO, "voice", "tsFilePath %s",tsFilePath);
    return nuFALSE;

}


jboolean Java_com_nuror1_NuroCore_sendPoiStringUTF16BE(JNIEnv *env, jclass cls, jbyteArray buffer)
{
	return g_NuroCore->SendPoiStringUTF16BE(env, cls, buffer);
}

//extern nuBYTE *g_pSoundTouch;
jboolean Java_com_nuror1_NuroCore_keySound(JNIEnv * env, jclass cls)
{
  //  if (!g_pSoundTouch)
    {
        return false;
    }

    //return (1 == (*g_pSoundTouch));
}

nuWCHAR* w2js(JNIEnv* env, jstring str)  
{
    int len = env->GetStringLength(str);
    const jchar *w_stringchars;

    nuWCHAR *w_buffer = (nuWCHAR *)nuMalloc(sizeof (nuWCHAR) * (len + 1));
    w_stringchars = env->GetStringChars(str, 0);
    nuMemcpy(w_buffer, w_stringchars, sizeof (nuWCHAR) * len);
    w_buffer[len] = 0;
    env->ReleaseStringChars(str, w_stringchars);
    
    return w_buffer;
}


class Adapter {
        void *m_data;
        int   m_size;
    public:

        Adapter()
        {
            m_data = NULL;
            m_size = 0;
        }

        void *CreateLparam(JNIEnv *env, int dwData, jobject lpData)
        {
            jclass lpDataClass = env->GetObjectClass(lpData);

            if (dwData == 15)
            {
              /*  OneKeyToNaviTmp *localData = (OneKeyToNaviTmp *)nuMalloc(sizeof (OneKeyToNaviTmp));
                
                m_data = localData;
                m_size = sizeof (OneKeyToNaviTmp);

                jfieldID latID =  env->GetFieldID(lpDataClass, "latitude", getSignatureVal("long"));

                if (latID)
                {
                    localData->latitude = env->GetLongField(lpData, latID);
                }

                jfieldID lngID =  env->GetFieldID(lpDataClass, "longitude", getSignatureVal("long"));

                if (lngID)
                {
                    localData->longitude = env->GetLongField(lpData, lngID);
                }

                jfieldID nDesNameLenID =  env->GetFieldID(lpDataClass, "nDesNameLen", getSignatureVal("long"));

                if (nDesNameLenID)
                {
                    localData->nDesNameLen = env->GetLongField(lpData, nDesNameLenID);
                }

                jfieldID wDesNameID = env->GetFieldID(lpDataClass, "wDesName", getSignatureVal("String"));

                if (wDesNameID)
                {
                    nuWCHAR *name = w2js(env, (jstring)env->GetObjectField(lpData, wDesNameID));
                    nuWcscpy(localData->wDesName, name); 
                    nuFree(name);
                }
        {
               char buf[256];
               sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "dengxu11" ," %s", buf);
        }*/
            }
            else if (dwData == 18) 
            {
                m_size = env->GetStringLength((jstring)lpData);
                m_data = w2js(env, (jstring)lpData);
            }
            else if (dwData == 20) 
            {
                /*
                ShowPng *localData = (ShowPng *)nuMalloc(sizeof (ShowPng));
	        __android_log_print(ANDROID_LOG_INFO, "dengxu", ">>>> %s:%d",__FILE__, __LINE__);
                m_data = localData;
                m_size = sizeof(ShowPng);
	        __android_log_print(ANDROID_LOG_INFO, "dengxu", ">>>> %s:%d",__FILE__, __LINE__);

                jfieldID isShowID = env->GetFieldID(lpDataClass, "isShow", getSignatureVal("int"));
                localData->isShow = env->GetIntField(lpData, isShowID);
	        __android_log_print(ANDROID_LOG_INFO, "dengxu", ">>>> %s:%d",__FILE__, __LINE__);
            */
            }

	        //__android_log_print(ANDROID_LOG_INFO, "dengxu", ">>>> %s:%d",__FILE__, __LINE__);
            env->DeleteLocalRef(lpDataClass); // 此处需要释放，局部reference 有限制512个数大小

            return m_data;
        }

        int getDataSize()
        {
            return m_size;
        }

        ~Adapter()
        {
            if (m_data)
            {
                free(m_data);
            }
        }
};
bool g_bMoveState = false;
JNIEXPORT void Java_com_nuror1_NuroCore_sendCopyData(JNIEnv *env, jclass cls, jobject Downobject)
{
	COPYDATASTRUCT cpst = {0};
	jobject object = env->NewLocalRef(Downobject);
	jclass JavaClass = env->GetObjectClass(object);
	jfieldID dwDataID = env->GetFieldID(JavaClass, "dwData", getSignatureVal("long"));
	jfieldID lpObjectID = env->GetFieldID(JavaClass, "lpData", getSignatureVal("Object"));
	jlong dwData = env->GetLongField(object, dwDataID);
	jobject lpObjectIDEx = env->GetObjectField(object, lpObjectID);
	jclass JavaClassEx = env->GetObjectClass(lpObjectIDEx);
	if(g_bMoveState && dwData != NURO_DEF_UI_SET_MOVE_MAP)
	{
		//return ;
	}
	if(dwData == NURO_DEF_UI_SET_BUTTON_EVENT)
	{
		NURO_BUTTON_EVENT  *lpData = (NURO_BUTTON_EVENT *)nuMalloc(sizeof (NURO_BUTTON_EVENT));
		if(lpData == NULL)
		{
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nCode", "I");
     		int nCode =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nButState", "B");
		unsigned char nButState = env->GetByteField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nX", "I");
		int nX =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nY", "I");
		int nY =  env->GetIntField (lpObjectIDEx, DataID);
		
		lpData->nCode = nCode;
		lpData->nButState = nButState;
		lpData->nX = nX;
		lpData->nY = nY;
		
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_BUTTON_EVENT);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(lpData);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sendCopyData NURO_BUTTON_EVENT ok");
	}
	else if(dwData == NURO_DEF_UI_SET_MOVE_MAP)
	{
__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_MOVE_MAP\n");
		NURO_POINT2  *lpData = (NURO_POINT2 *)nuMalloc(sizeof (NURO_POINT2));
		if(lpData == NULL)
		{
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nX1", "I");
		int nX1 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nY1", "I");
		int nY1 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nX2", "I");
		int nX2 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nY2", "I");
		int nY2 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "MouseCount", "I");
		int MouseCount =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nMouseType", "I");
		int nMouseType =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "VelocityX1", "I");
		int VelocityX1 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "VelocityY1", "I");
		int VelocityY1 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "VelocityX2", "I");
		int VelocityX2 =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "VelocityY2", "I");
		int VelocityY2 =  env->GetIntField (lpObjectIDEx, DataID);

		lpData->nX1 = nX1;
		lpData->nY1 = nY1;
		lpData->nX2 = nX2;
		lpData->nY2 = nY2;
		lpData->MouseCount = MouseCount;
		lpData->nMouseType = nMouseType;
		lpData->VelocityX1 = VelocityX1;
		lpData->VelocityY1 = VelocityY1;
		lpData->VelocityX2 = VelocityX2;
		lpData->VelocityY2 = VelocityY2;
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_POINT2);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuSleep(100);
		nuFree(lpData);
		if(nMouseType == NURO_DEF_ANDROID_ACTION_MOVE)
		{
			g_bMoveState = true;
		}
		else if(nMouseType == NURO_DEF_ANDROID_ACTION_UP)
		{
			g_bMoveState = false;
		}
		__android_log_print(ANDROID_LOG_INFO, "navi", "nMouseType %d, x1 %d, y1 %d, x2 %d, y2 %d\n", nMouseType, nX1, nY1, nX2, nY2);
	}
	if(dwData == NURO_DEF_UI_SET_NAVI_POINT)
	{
		NURO_NAVI_POINT  *lpData = (NURO_NAVI_POINT *)nuMalloc(sizeof (NURO_NAVI_POINT));
		if(lpData == NULL)
		{
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nNaviIndex", "I");   		
		int nNaviIndex =  env->GetIntField (lpObjectIDEx, DataID);   		
		DataID = env->GetFieldID(JavaClassEx, "nRouteType", "I");
     		int nRouteType =  env->GetIntField (lpObjectIDEx, DataID);    		
		DataID = env->GetFieldID(JavaClassEx, "bRoute", "Z");	
		bool bRoute =  env->GetBooleanField (lpObjectIDEx, DataID);
		/*DataID = env->GetFieldID(JavaClassEx, "RoadName", "Ljava/lang/String;");
		//jobject jc_v = env->GetObjectArrayElement(env, jNuroCore, 0);
		jstring jstr = (jstring)env->GetObjectField(lpObjectIDEx, DataID);
		int length = (int)env->GetStringLength(jstr);  
		if(length > 48)
		{
			length = 48;
		}
		const wchar_t * RoadName = (wchar_t *)env->GetStringChars(jstr, NULL);       
		env->ReleaseStringChars(jstr, (jchar *)RoadName);*/

		DataID = env->GetFieldID(JavaClassEx, "nX", "I");  		
		int nX =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nY", "I");
		int nY =  env->GetIntField (lpObjectIDEx, DataID);
		
		lpData->nNaviIndex = nNaviIndex;
		lpData->nRouteType = nRouteType;
		lpData->bRoute	   = bRoute;
		//memcpy(lpData->RoadName, RoadName, sizeof(wchar_t) * length);
		lpData->nX = nX;
		lpData->nY = nY;
		
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_NAVI_POINT);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(lpData);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sendCopyData NURO_NAVI_POINT ok");
	}
	else if(dwData == NURO_DEF_UI_SET_USER_DATA)
	{
		NURO_USER_SET_DATA  *lpData = (NURO_USER_SET_DATA *)nuMalloc(sizeof (NURO_USER_SET_DATA));
		if(lpData == NULL)
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_USER_DATA Return");
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "bSetByDefault", "I");
		int bSetByDefault =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bNightDay", "I");
		int bNightDay =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "b3DMode", "I");
		int b3DMode =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nLanguage", "I");
		int nLanguage =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nMapDirectMode", "I");
		int nMapDirectMode =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nZoomAutoMode", "I");
		int nZoomAutoMode =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nPlaySpeedWarning", "I");
		int nPlaySpeedWarning =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nSimSpeed", "I");
		int nSimSpeed =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bAutoDayNight", "I");
		int bAutoDayNight =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nCarIconType", "I");
		int nCarIconType =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nDayBgPic", "I");
		int nDayBgPic =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nNightBgPic", "I");
		int nNightBgPic =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nRouteMode", "I");
		int nRouteMode =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bLastNavigation", "I");
		int bLastNavigation =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nPastMarket", "I");
		int nPastMarket =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nVoiceControl", "I");
		int nVoiceControl =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nLastScaleIndex", "I");
		int nLastScaleIndex =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nSpNumInput", "I");
		int nSpNumInput =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nKeyWordInput", "I");
		int nKeyWordInput =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nSpeedWarningPic", "I");
		int nSpeedWarningPic =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nUILanguage", "I");
		int nUILanguage =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nSpeedWarningVoice", "I");
		int nSpeedWarningVoice =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nRoadInput", "I");
		int nRoadInput =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nPoiInput", "I");
		int nPoiInput =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nScaleIdx", "I");
		int nScaleIdx =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nSoundVolume", "I");
		int nSoundVolume =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nTMCSwitchFlag", "I");
		int nTMCSwitchFlag =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nTMCFrequency", "I");
		int nTMCFrequency =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nTMCVoice", "I");
		int nTMCVoice =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nTimeToNight", "I");
		int nTimeToNight =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nTimeToDay", "I");
		int nTimeToDay =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bCCDVoice", "I");
		int bCCDVoice =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bBigMapRoadFont", "I");
		int bBigMapRoadFont =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nRouteOption", "I");
		int nRouteOption =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nWaveTTSType", "I");
		int nWaveTTSType =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nShowTip", "I");
		int nShowTip =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nShowPOI", "I");
		int nShowPOI =  env->GetIntField (lpObjectIDEx, DataID);
		

		lpData->bSetByDefault		= bSetByDefault;
		lpData->bNightDay 	 	= bNightDay;
		lpData->b3DMode   	 	= b3DMode;
		lpData->nLanguage		= nLanguage;
		lpData->nMapDirectMode   	= nMapDirectMode;
		lpData->nZoomAutoMode		= nZoomAutoMode;
		lpData->nPlaySpeedWarning	= nPlaySpeedWarning;
		lpData->nSimSpeed		= nSimSpeed;
		lpData->bAutoDayNight		= bAutoDayNight;
		lpData->nCarIconType	 	= nCarIconType;
		lpData->nDayBgPic		= nDayBgPic;
		lpData->nNightBgPic		= nNightBgPic;
		lpData->nRouteMode		= nRouteMode;
		lpData->bLastNavigation		= bLastNavigation;
		lpData->nPastMarket		= nPastMarket;
		lpData->nVoiceControl		= nVoiceControl;
		lpData->nLastScaleIndex		= nLastScaleIndex;
		lpData->nSpNumInput		= nSpNumInput;
		lpData->nKeyWordInput		= nKeyWordInput;
		lpData->nSpeedWarningPic	= nSpeedWarningPic;
		lpData->nUILanguage		= nUILanguage;
		lpData->nSpeedWarningVoice	= nSpeedWarningVoice;
		lpData->nRoadInput		= nRoadInput;
		lpData->nPoiInput		= nPoiInput;
		lpData->nScaleIdx	 	= nScaleIdx;
		lpData->nSoundVolume		= nSoundVolume;
		lpData->nTMCSwitchFlag		= nTMCSwitchFlag;
		lpData->nTMCVoice		= nTMCVoice;
		lpData->nTimeToNight		= nTimeToNight;
		lpData->nTimeToDay		= nTimeToDay;
		lpData->bCCDVoice		= bCCDVoice;
		lpData->bBigMapRoadFont		= bBigMapRoadFont;
		lpData->nRouteOption		= nRouteOption;
		lpData->nWaveTTSType		= nWaveTTSType;		
		lpData->nShowTip		= nShowTip;	
		lpData->nShowPOI		= nShowPOI;
		
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_USER_SET_DATA);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(lpData);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sendCopyData SET_USER_DATA ok");
	}
	else if(dwData == NURO_DEF_UI_SET_MAP_CENTER)
	{
		NURO_POINT  *lpData = (NURO_POINT *)nuMalloc(sizeof (NURO_POINT));
		if(lpData == NULL)
		{
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nX", "I");
		int nX =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nY", "I");
		int nY =  env->GetIntField (lpObjectIDEx, DataID);

		lpData->nX = nX;
		lpData->nY = nY;
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_POINT);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(lpData);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sendCopyData MAP_CENTER ok");
	}
	else if(dwData == NURO_DEF_UI_SET_NAVIGATION)
	{		
		jfieldID DataID = env->GetFieldID(JavaClassEx, "Nuro_NaviType", "I");
		int Nuro_NaviType =  env->GetIntField (lpObjectIDEx, DataID);

		cpst.dwData = dwData;
		cpst.cbData = sizeof(Nuro_NaviType);
		cpst.lpData = &Nuro_NaviType;
		g_NuroCore->OnCopyData(&cpst);
		__android_log_print(ANDROID_LOG_INFO, "navi", "sendCopyData SET_NAVIGATION");
	}
	else if(dwData == NURO_DEF_UI_SET_PACK_PIC)
	{
		//__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_PACK_PIC");
		PNURO_PACK_PIC_PARAMETER pPicPara = (PNURO_PACK_PIC_PARAMETER)nuMalloc(sizeof (NURO_PACK_PIC_PARAMETER));
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nTop", "I");
		int nTop =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nBottom", "I");
		int nBottom =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nLeft", "I");
		int nLeft =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nRight", "I");
		int nRight =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nPackPathID", "I");
		int nPackPathID =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nPicID", "I");
		int nPicID =  env->GetIntField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bInitSet", "Z");
		bool bInitSet =  env->GetBooleanField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bLoadMemory", "Z");
		bool bLoadMemory =  env->GetBooleanField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bEngineShow", "Z");
		bool bEngineShow =  env->GetBooleanField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "bShowPic", "Z");
		bool bShowPic =  env->GetBooleanField (lpObjectIDEx, DataID);
		pPicPara->nTop 		= nTop;
		pPicPara->nBottom 	= nBottom;
		pPicPara->nLeft 	= nLeft;
		pPicPara->nRight 	= nRight;
		pPicPara->nPackPathID 	= nPackPathID;
		pPicPara->nPicID 	= nPicID;
		pPicPara->bInitSet	= bInitSet;
		pPicPara->bLoadMemory 	= bLoadMemory;
		pPicPara->bEngineShow 	= bEngineShow;
		pPicPara->bShowPic 	= bShowPic;

		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_PACK_PIC_PARAMETER);
		cpst.lpData = pPicPara;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(pPicPara);
		__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_PACK_PIC ");
	}
	else if(dwData == NURO_DEF_UI_SET_TMC_EVENT)
	{
		NURO_TMC_EVENT  *lpData = (NURO_TMC_EVENT *)nuMalloc(sizeof(NURO_TMC_EVENT));
		if(lpData == NULL)
		{
			return ;
		}
		jfieldID DataID = env->GetFieldID(JavaClassEx, "nMode", "B");
		int nMode =  env->GetByteField (lpObjectIDEx, DataID);
		DataID = env->GetFieldID(JavaClassEx, "nShowEvent", "B");
		int nShowEvent =  env->GetByteField (lpObjectIDEx, DataID);
		lpData->nMode = nMode;
		lpData->nShowEvent = nShowEvent;
		cpst.dwData = dwData;
		cpst.cbData = sizeof(NURO_TMC_EVENT);
		cpst.lpData = lpData;
		g_NuroCore->OnCopyData(&cpst);
		nuFree(lpData);
		__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_TMC_EVENT ");
	}
	else if(dwData == NURO_DEF_UI_SET_CAR_ROAD)
	{
		cpst.dwData = dwData;
		g_NuroCore->OnCopyData(&cpst);
		__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_SET_CAR_ROAD ");
	}
	else if(dwData == NURO_DEF_UI_SET_EXIT)
	{
		exit(1);
	}
	env->DeleteLocalRef(object);
}

void Java_com_nuror1_NuroCore_sendWindProc(JNIEnv *env, jclass cls, jlong message, jlong wparam, jlong lparam)
{
    g_NuroCore->WindowProc(message, wparam, lparam);
}
nuBYTE *g_pTMCUsed = NULL;
jboolean Java_com_nuror1_NuroCore_GetTMCstate(JNIEnv *env, jclass cls)
{
    //__android_log_print(ANDROID_LOG_INFO, "NaviThreadCallBack", "GetTMCstate ok ................... %d %d",g_pTMCUsed,*g_pTMCUsed);

    if(*g_pTMCUsed) 
    {	
        return true;
    }
    else
    {
        return false;
    }

}
jboolean Java_com_nuror1_NuroCore_SetTMCstate(JNIEnv *env, jclass cls, jboolean bTMC)
{
    nuBYTE value = 0;
    if(bTMC) 
    {
        *g_pTMCUsed = 1;
    }
    else 
    {
        *g_pTMCUsed = 0;
    }

   // __android_log_print(ANDROID_LOG_INFO, "NaviThreadCallBack", "SetTMCstate ok ...................%d %d %d",bTMC,g_pTMCUsed,*g_pTMCUsed);

}

JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_DrawInfo(JNIEnv *env, jclass cls, jobject object)
{
	jclass tmpclass;
	jfieldID tmpfieldID;
	jobject tmpobject;
    
	tmpobject = object;
	if(tmpobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoobject tmpobject is NULL");
		return ;	
	}
	DrawInfoobject = (jclass)env->NewGlobalRef(tmpobject);
	if(DrawInfoobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoobject NewGlobalRef is NULL");
		return ;	
	}
	tmpclass = env->GetObjectClass(object);
	if(tmpclass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoClass tmpclass is NULL");
		return ;	
	}
    	DrawInfoClass = (jclass)env->NewGlobalRef(tmpclass);
    	if(DrawInfoClass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoClass NewGlobalRef is NULL");
		return ;
	}
    	tmpfieldID = env->GetFieldID(DrawInfoClass, "DrawMapInfo", "Lcom/nuror1/NuroDrawInfo$NURO_DrawMapInfo;");
    	if(tmpfieldID == NULL)
    	{
    		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawMapInfoField tmpfieldID is NULL");
		return ;
	}
	DrawMapInfoField = tmpfieldID;
	
	tmpobject = env->GetObjectField(DrawInfoobject, DrawMapInfoField);
	if(tmpobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawMapInfoObject tmpobject is NULL");
		return ;
	}
	DrawMapInfoObject = (jobject)env->NewGlobalRef(tmpobject);
	if(tmpobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawMapInfoObject NewGlobalRef is NULL");
		return ;
	}
	tmpclass = env->GetObjectClass(DrawMapInfoObject);
	if(tmpclass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawMapInfoClass tmpclass is NULL");
		return ;
	}
	DrawMapInfoClass = (jclass)env->NewGlobalRef(tmpclass);
	if(DrawMapInfoClass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawMapInfoClass NewGlobalRefd is NULL");
		return ;
	}
	tmpfieldID = env->GetFieldID(DrawInfoClass,"DrawNaviInfo","Lcom/nuror1/NuroDrawInfo$NURO_Total_Navi_Info;");
	if(tmpfieldID == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawNaviInfoField tmpfieldID is NULL");
		return ;
	}
	DrawNaviInfoField = tmpfieldID;

	tmpobject = env->GetObjectField(DrawInfoobject, DrawNaviInfoField);
	if(tmpobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawNaviInfoObject tmpobject is NULL");
		return ;
	}
	DrawNaviInfoObject = (jobject)env->NewGlobalRef(tmpobject);
	if(DrawNaviInfoObject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawNaviInfoObject NewGlobalRef is NULL");
		return ;
	}
	tmpclass = env->GetObjectClass(DrawNaviInfoObject);
	if(tmpclass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawNaviInfoClass tmpclass is NULL");
		return ;
	}
	DrawNaviInfoClass = (jclass)env->NewGlobalRef(tmpclass);
	if(DrawNaviInfoClass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawNaviInfoClass NewGlobalRef is NULL");
		return ;
	}
	tmpfieldID = env->GetFieldID(DrawInfoClass,"DrawLaneGuide","Lcom/nuror1/NuroDrawInfo$NURO_Lane_Guide_Info;");
	if(tmpfieldID == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoLaneGuideField tmpfieldID is NULL");
		return ;
	}
	DrawInfoLaneGuideField = tmpfieldID;

	tmpobject = env->GetObjectField(DrawInfoobject, DrawInfoLaneGuideField);
	if(tmpobject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoLaneGuideObject tmpobject is NULL");
		return ;
	}
	DrawInfoLaneGuideObject = (jobject)env->NewGlobalRef(tmpobject);//env->GetObjectField(DrawInfoobject, DrawInfoLaneGuideField);
	if(DrawInfoLaneGuideObject == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoLaneGuideObject NewGlobalRef is NULL");
		return ;
	}
	tmpclass = env->GetObjectClass(DrawInfoLaneGuideObject);
	if(tmpclass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoLaneGuideObject tmpclass is NULL");
		return ;
	}
	DrawInfoLaneGuideClass = (jclass)env->NewGlobalRef(tmpclass);
	if(DrawInfoLaneGuideClass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfoLaneGuideClass NewGlobalRef is NULL");
		return ;
	}
	tmpclass  = env->FindClass("com/nuror1/NuroDrawInfo$NURO_NaviList");
	if(tmpclass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListClass tmpclass is NULL");
		return ;
	}
	RoadListClass = (jclass)env->NewGlobalRef(tmpclass);
	if(RoadListClass == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListClass NewGlobalRef is NULL");
		return ;
	}
}
static nuCHAR s_sMapName[10];
nuUINT NaviThreadCallBack(PCALL_BACK_PARAM pParam)
{
    //switch (pParam->nCallType)
    //{
	//case  CALLBACK_TYPE_JAVA_DATA:
	//{
		NURO_DrawInfo DrawInfo = {0};
		nuMemset(&DrawInfo, 0, sizeof(NURO_DrawInfo));
		nuMemcpy(&DrawInfo , pParam->pVOID, sizeof(NURO_DrawInfo));

		if(DrawInfoClass!=NULL)
		{
			JNIEnv*env= GetEnv();
			//jfieldID DrawMapInfoField = env->GetFieldID(DrawInfoClass, "DrawMapInfo", "Lcom/nuror1/NuroDrawInfo$NURO_DrawMapInfo;");
			//jobject DrawMapInfoObject = env->GetObjectField(DrawInfoobject, DrawMapInfoField);
			//jclass DrawMapInfoClass = env->GetObjectClass(DrawMapInfoObject);

			//NaviState
	 		jfieldID NaviStateID = env->GetFieldID(DrawInfoClass,"NaviState","Z");
			if(NaviStateID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "NaviStateID is NULL");
				return 0;
			}
			env->SetBooleanField(DrawInfoobject,NaviStateID,DrawInfo.bNaviState);

			//MoveStated
	    		jfieldID MoveStateID = env->GetFieldID(DrawInfoClass,"MoveState","B");
			if(MoveStateID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "MoveStateID is NULL");
				return 0;
			}
			env->SetByteField(DrawInfoobject,MoveStateID,DrawInfo.nMoveState);
			//__android_log_print(ANDROID_LOG_INFO, "navi", "nMoveState %d", DrawInfo.nMoveState);
			//=============================NURO_DrawMapInfo===================================
			//MapPath
			//__android_log_print(ANDROID_LOG_INFO, "navi", "MapPath");
			jstring MapPath=NULL;
			MapPath = env->NewStringUTF(DrawInfo.DrawMapInfo.tsMapPath);
			if(MapPath == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "MapPath is NULL");
				return 0;
			}
			jfieldID MapPathID = env->GetFieldID(DrawMapInfoClass,"MapPath","Ljava/lang/String;");
			if(MapPathID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "MapPathID is NULL");
				return 0;
			}
			env->SetObjectField(DrawMapInfoObject,MapPathID,MapPath);
			env->DeleteLocalRef(MapPath);

			//RoadName
			jstring RoadName=NULL;
			RoadName = env->NewString((const jchar*)DrawInfo.DrawMapInfo.wsRoadName, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawMapInfo.wsRoadName));
			if(RoadName == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "RoadName is NULL");
				return 0;
			}			
			jfieldID RoadNameID = env->GetFieldID(DrawMapInfoClass,"RoadName","Ljava/lang/String;");
			if(RoadNameID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "RoadNameID is NULL");
				return 0;
			}	
			env->SetObjectField(DrawMapInfoObject,RoadNameID,RoadName);
			env->DeleteLocalRef(RoadName);

			//CityName
			jstring CityName=NULL;
			CityName = env->NewString((const jchar*)DrawInfo.DrawMapInfo.wsCityName, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawMapInfo.wsCityName));
			if(CityName == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CityName is NULL");
				return 0;
			}
			jfieldID CityNameID = env->GetFieldID(DrawMapInfoClass,"CityName","Ljava/lang/String;");
			if(CityNameID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CityNameID is NULL");
				return 0;
			}
			env->SetObjectField(DrawMapInfoObject,CityNameID,CityName);
			env->DeleteLocalRef(CityName);

			//KiloName
			jstring KiloName=NULL;
			KiloName = env->NewString((const jchar*)DrawInfo.DrawMapInfo.wsKiloName, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawMapInfo.wsKiloName));
			if(KiloName == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "KiloName is NULL");
				return 0;
			}
			jfieldID KiloNameID = env->GetFieldID(DrawMapInfoClass,"KiloName","Ljava/lang/String;");
			if(KiloNameID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "KiloNameID is NULL");
				return 0;
			}
			env->SetObjectField(DrawMapInfoObject,KiloNameID,KiloName);
			env->DeleteLocalRef(KiloName);

			//CenterPoi
			jstring CenterPoi=NULL;
			CenterPoi = env->NewString((const jchar*)DrawInfo.DrawMapInfo.wsCenterPoi, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawMapInfo.wsCenterPoi));
			if(CenterPoi == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CenterPoi is NULL");
				return 0;
			}
			jfieldID CenterPoiID = env->GetFieldID(DrawMapInfoClass,"CenterPoi","Ljava/lang/String;");
			if(CenterPoiID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CenterPoiID is NULL");
				return 0;
			}
			env->SetObjectField(DrawMapInfoObject,CenterPoiID,CenterPoi);
			env->DeleteLocalRef(CenterPoi);
			
			//CCD
			jfieldID CCDID = env->GetFieldID(DrawMapInfoClass,"CCD","Z");
			if(CCDID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CCDID is NULL");
				return 0;
			}
			env->SetBooleanField(DrawMapInfoObject,CCDID,DrawInfo.DrawMapInfo.bCCD);
			
			jfieldID bShowPicID = env->GetFieldID(DrawMapInfoClass,"bShowPic","Z");
			if(bShowPicID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "3dpic", "bShowPicID is NULL");
				return 0;
			}
			env->SetBooleanField(DrawMapInfoObject,bShowPicID,DrawInfo.DrawMapInfo.bShowPic);
			if(DrawInfo.DrawMapInfo.bShowPic)
			{
				__android_log_print(ANDROID_LOG_INFO, "3dpic", "bShowPicID show");
			}	
			//SpeedLimit
			jfieldID SpeedLimitID = env->GetFieldID(DrawMapInfoClass,"SpeedLimit","B");
			if(SpeedLimitID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "SpeedLimitID is NULL");
				return 0;
			}
			env->SetByteField(DrawMapInfoObject,SpeedLimitID,DrawInfo.DrawMapInfo.nSpeedLimit);

			//CarOnMapID
			jfieldID CarOnMapID = env->GetFieldID(DrawMapInfoClass,"CarOnMapID","J");
			if(CarOnMapID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CarOnMapID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CarOnMapID,DrawInfo.DrawMapInfo.CarOnMapID);

			//CarOnCityID
			jfieldID CarOnCityID = env->GetFieldID(DrawMapInfoClass,"CarOnCityID","J");
			if(CarOnCityID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CarOnCityID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CarOnCityID,DrawInfo.DrawMapInfo.CarOnCityID);
			
			//CarOnTownID
			jfieldID CarOnTownID = env->GetFieldID(DrawMapInfoClass,"CarOnTownID","J");
			if(CarOnTownID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CarOnTownID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CarOnTownID,DrawInfo.DrawMapInfo.CarOnTownID);
			
			//CenterOnMapID
			jfieldID CenterOnMapID = env->GetFieldID(DrawMapInfoClass,"CenterOnMapID","J");
			if(CenterOnMapID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CenterOnMapID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CenterOnMapID,DrawInfo.DrawMapInfo.CenterOnMapID);

			//CenterOnCityID
			jfieldID CenterOnCityID = env->GetFieldID(DrawMapInfoClass,"CenterOnCityID","J");
			if(CenterOnCityID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CenterOnCityID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CenterOnCityID,DrawInfo.DrawMapInfo.CenterOnCityID);

			//CenterOnTownID
			jfieldID CenterOnTownID = env->GetFieldID(DrawMapInfoClass,"CenterOnTownID","J");
			if(CenterOnTownID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CenterOnTownID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CenterOnTownID,DrawInfo.DrawMapInfo.CenterOnTownID);			

			//CCDSpeed
			jfieldID CCDSpeedID = env->GetFieldID(DrawMapInfoClass,"CCDSpeed","J");
			if(CCDSpeedID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "CCDSpeedID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,CCDSpeedID,DrawInfo.DrawMapInfo.lCCDSpeed);
			
			//DirectionAngle
			jfieldID DirectionAngleID = env->GetFieldID(DrawMapInfoClass,"DirectionAngle","J");
			if(DirectionAngleID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "DirectionAngleID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,DirectionAngleID,DrawInfo.DrawMapInfo.nDirectionAngle);

			//ScaleValue
			jfieldID ScaleValueID = env->GetFieldID(DrawMapInfoClass,"ScaleValue","J");
			if(ScaleValueID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ScaleValueID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ScaleValueID,DrawInfo.DrawMapInfo.nScaleValue);

			//ptCurrentCarID
			jfieldID ptCurrentCarXID = env->GetFieldID(DrawMapInfoClass,"ptCurrentCarX","J");
			if(ptCurrentCarXID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptCurrentCarXID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptCurrentCarXID,DrawInfo.DrawMapInfo.ptCurrentCarX);
			jfieldID ptCurrentCarYID = env->GetFieldID(DrawMapInfoClass,"ptCurrentCarY","J");
			if(ptCurrentCarYID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptCurrentCarYID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptCurrentCarYID,DrawInfo.DrawMapInfo.ptCurrentCarY);

			//ptMapCarID
			jfieldID ptMapCarXID = env->GetFieldID(DrawMapInfoClass,"ptMapCarX","J");
			if(ptMapCarXID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptMapCarXID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptMapCarXID,DrawInfo.DrawMapInfo.ptMapCarX);
			jfieldID ptMapCarYID = env->GetFieldID(DrawMapInfoClass,"ptMapCarY","J");
			if(ptMapCarYID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptMapCarYID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptMapCarYID,DrawInfo.DrawMapInfo.ptMapCarY);

			//ptMapCenter
			jfieldID ptMapCenterXID = env->GetFieldID(DrawMapInfoClass,"ptMapCenterX","J");
			if(ptMapCenterXID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptMapCenterXID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptMapCenterXID,DrawInfo.DrawMapInfo.ptMapCenterX);
			jfieldID ptMapCenterYID = env->GetFieldID(DrawMapInfoClass,"ptMapCenterY","J");
			if(ptMapCenterYID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "ptMapCenterYID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,ptMapCenterYID,DrawInfo.DrawMapInfo.ptMapCenterY);

			//lCarAngle
			jfieldID lCarAngleID = env->GetFieldID(DrawMapInfoClass,"lCarAngle","J");
			if(lCarAngleID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lCarAngleID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,lCarAngleID,DrawInfo.DrawMapInfo.lCarAngle);

			//lMapAngle
			jfieldID lMapAngleID = env->GetFieldID(DrawMapInfoClass,"lMapAngle","J");
			if(lMapAngleID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lMapAngleID is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,lMapAngleID,DrawInfo.DrawMapInfo.lMapAngle);

			//=============================NURO_Total_Navi_Info================================
			//jfieldID DrawNaviInfoField = env->GetFieldID(DrawInfoClass,"DrawNaviInfo","Lcom/nuror1/NuroDrawInfo$NURO_Total_Navi_Info;");
			//jobject  DrawNaviInfoObject = env->GetObjectField(DrawInfoobject, DrawNaviInfoField);
			//jclass   DrawNaviInfoClass = env->GetObjectClass(DrawNaviInfoObject);
			//__android_log_print(ANDROID_LOG_INFO, "navi", "DrawInfo.bNaviState %d", DrawInfo.bNaviState);
			if(DrawInfo.bNaviState)
			{
				//NextRoadCount
				jfieldID NextRoadCountID = env->GetFieldID(DrawNaviInfoClass,"NextRoadCount","J");
				if(NextRoadCountID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "NextRoadCountID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,NextRoadCountID,DrawInfo.DrawNaviInfo.lNextRoadCount);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "NextRoadCountID ");
				//IconType
				jfieldID IconTypeID = env->GetFieldID(DrawNaviInfoClass,"IconType","S");
				if(IconTypeID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "IconTypeID is NULL");
					return 0;
				}
				env->SetShortField(DrawNaviInfoObject,IconTypeID,DrawInfo.DrawNaviInfo.nIconType);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "IconType %d", DrawInfo.DrawNaviInfo.nIconType);
				//RoadName
				jstring NextRoadName=NULL;
				NextRoadName = env->NewString((const jchar*)DrawInfo.DrawNaviInfo.wsRoadName, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawNaviInfo.wsRoadName));
				if(NextRoadName == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "NextRoadName is NULL");
					return 0;
				}
				jfieldID NextRoadNameID = env->GetFieldID(DrawNaviInfoClass,"RoadName","Ljava/lang/String;");
				if(NextRoadNameID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "NextRoadNameID is NULL");
					return 0;
				}
				//__android_log_print(ANDROID_LOG_INFO, "navi", "NextRoadNameID ");
				env->SetObjectField(DrawNaviInfoObject,NextRoadNameID,NextRoadName);
				env->DeleteLocalRef(NextRoadName);

				//Distance
				jfieldID DistanceID = env->GetFieldID(DrawNaviInfoClass,"Distance","J");
				if(DistanceID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "DistanceID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,DistanceID,DrawInfo.DrawNaviInfo.lDistance);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "DistanceID ");
				//DisToTarget
				jfieldID DisToTargetID = env->GetFieldID(DrawNaviInfoClass,"DisToTarget","J");
				if(DisToTargetID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "DisToTargetID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,DisToTargetID,DrawInfo.DrawNaviInfo.lDisToTarget);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "DisToTargetID ");	
				//TimeToTarget
				jfieldID TimeToTargetID = env->GetFieldID(DrawNaviInfoClass,"TimeToTarget","J");
				if(TimeToTargetID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "TimeToTargetID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,TimeToTargetID,DrawInfo.DrawNaviInfo.lTimeToTarget);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "TimeToTargetID ");
				//RealCrossdDis
				jfieldID lRealCrossdDisID = env->GetFieldID(DrawNaviInfoClass,"lRealCrossdDis","J");
				if(lRealCrossdDisID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "lRealCrossdDisID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,lRealCrossdDisID,DrawInfo.DrawNaviInfo.lRealCrossdDis);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "lRealCrossdDisID ");
				//RealID
				jfieldID lRealID = env->GetFieldID(DrawNaviInfoClass,"lRealID","J");
				if(lRealID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "lRealID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,lRealID,DrawInfo.DrawNaviInfo.lRealID);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "lRealID ");
				//PassNum
				jfieldID PassNumID = env->GetFieldID(DrawNaviInfoClass,"PassNum","J");
				if(PassNumID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "PassNumID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,PassNumID,DrawInfo.DrawNaviInfo.lPassNum);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "PassNumID ");
				//RoadListNum
				jfieldID RoadListNumID = env->GetFieldID(DrawNaviInfoClass,"RoadListNum","J");
				if(RoadListNumID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListNumID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,RoadListNumID,DrawInfo.DrawNaviInfo.RoadListNum);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListNumID ");
				//RoutingListCount
				jfieldID RoutingListCountID = env->GetFieldID(DrawNaviInfoClass,"RoutingListCount","J");
				if(RoutingListCountID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "RoutingListCountID is NULL");
					return 0;
				}
				env->SetLongField(DrawNaviInfoObject,RoutingListCountID,DrawInfo.DrawNaviInfo.RoutingListCount);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "RoutingListCountID ");
				//RoadList
				jfieldID   RoadListField = env->GetFieldID(DrawInfoClass,"RoadList","Lcom/nuror1/NuroDrawInfo$NURO_NaviList;");
				if(RoadListField == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListField is NULL");
					return 0;
				}
				jobject RoadListObject = env->GetObjectField(DrawInfoobject, RoadListField);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListField ");
				if(RoadListObject == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListObject is NULL");
					return 0;
				}
				jclass    jRoadListClass = env->GetObjectClass(RoadListObject);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "RoadListObject ");
				if(jRoadListClass == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "jRoadListClass is NULL");
					return 0;
				}
				//__android_log_print(ANDROID_LOG_INFO, "navi", "jRoadListClass ");
				for(int  i=0; i < _MAX_NEXTROADLIST; i++ )
				{
					//SetIconType
					jmethodID SetDataCountID = env->GetMethodID(jRoadListClass, "SetIconType", "(IS)V");
					if(SetDataCountID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "jRoadListClass is NULL");
						return 0;
					}
					env->CallVoidMethod(RoadListObject,SetDataCountID,i,DrawInfo.DrawNaviInfo.RoadList[i].nIconType);
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST jRoadListClass ");
					//SetDistance
					jmethodID SetDistanceID = env->GetMethodID(jRoadListClass, "SetDistance", "(IJ)V");
					if(SetDistanceID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "SetDistanceID is NULL");
						return 0;
					}
					env->CallVoidMethod(RoadListObject,SetDistanceID,i,DrawInfo.DrawNaviInfo.RoadList[i].lDistance);
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST SetDistanceID ");
					//SetRoadName
					jstring setName=NULL;
					setName = env->NewString((const jchar*)DrawInfo.DrawNaviInfo.RoadList[i].wsRoadName, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawNaviInfo.RoadList[i].wsRoadName));		
					if(setName == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "setName is NULL");
						return 0;
					}				
					jmethodID setNameID = env->GetMethodID(jRoadListClass, "SetRoadName", "(ILjava/lang/String;)V");
					if(setNameID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "setNameID is NULL");
						return 0;
					}
					env->CallVoidMethod(RoadListObject,setNameID,i,setName);
					env->DeleteLocalRef(setName);
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST setNameID ");
					//SetIconNum
					jstring setIconNum=NULL;
					setIconNum = env->NewString((const jchar*)DrawInfo.DrawNaviInfo.RoadList[i].wsIconNum, (jsize)nuWcslen((const nuWCHAR*)DrawInfo.DrawNaviInfo.RoadList[i].wsIconNum));		
					if(setIconNum == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "setIconNum is NULL");
						return 0;
					}				
					jmethodID setIconNumID = env->GetMethodID(jRoadListClass, "SetIconNum", "(ILjava/lang/String;)V");
					if(setIconNumID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "setIconNumID is NULL");
						return 0;
					}
					env->CallVoidMethod(RoadListObject,setIconNumID,i,setIconNum);
					env->DeleteLocalRef(setIconNum);	
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST setIconNumID ");
				}
				env->DeleteLocalRef(RoadListObject);
				env->DeleteLocalRef(jRoadListClass);
				jfieldID lLaneCountID = env->GetFieldID(DrawInfoLaneGuideClass,"lLaneCount","J");
				if(lLaneCountID == NULL)
				{
					__android_log_print(ANDROID_LOG_INFO, "navi", "lLaneCountID is NULL");
					return 0;
				}
				env->SetLongField(DrawInfoLaneGuideObject,lLaneCountID, DrawInfo.DrawNaviInfo.LaneGuideData.lLaneCount);
				//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST lLaneCountID %d", DrawInfo.DrawNaviInfo.LaneGuideData.lLaneCount);
				for(int i = 0;i < DrawInfo.DrawNaviInfo.LaneGuideData.lLaneCount; i ++)
				{
					jmethodID SetLaneEnableID = env->GetMethodID(DrawInfoLaneGuideClass, "SetLaneEnable", "(IZ)V");
					if(SetLaneEnableID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "SetLaneEnableID is NULL");
						return 0;
					}
					env->CallVoidMethod(DrawInfoLaneGuideObject,SetLaneEnableID,i,DrawInfo.DrawNaviInfo.LaneGuideData.bLaneEnable[i]);
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST SetLaneEnableID ");
					jmethodID SetLaneID = env->GetMethodID(DrawInfoLaneGuideClass, "SetLaneID", "(IS)V");
					if(SetLaneID == NULL)
					{
						__android_log_print(ANDROID_LOG_INFO, "navi", "SetLaneID is NULL");
						return 0;
					}
					env->CallVoidMethod(DrawInfoLaneGuideObject,SetLaneID,i,DrawInfo.DrawNaviInfo.LaneGuideData.nLaneID[i]);
__android_log_print(ANDROID_LOG_INFO, "LaneGuide", "Copy Enable %d, LightID %d", DrawInfo.DrawNaviInfo.LaneGuideData.bLaneEnable[i], DrawInfo.DrawNaviInfo.LaneGuideData.nLaneID[i]); 
					//__android_log_print(ANDROID_LOG_INFO, "navi", "NEXTROADLIST SetLaneID ");
				}				
			}
			//GPSINFO
			jfieldID TmpID = env->GetFieldID(DrawMapInfoClass,"lDate","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lDate is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID,DrawInfo.DrawMapInfo.lDate);
			TmpID = env->GetFieldID(DrawMapInfoClass,"lTime","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lTime is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID,DrawInfo.DrawMapInfo.lTime);
			TmpID = env->GetFieldID(DrawMapInfoClass,"dcourse","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "dcourse is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID,DrawInfo.DrawMapInfo.dcourse);
			TmpID = env->GetFieldID(DrawMapInfoClass,"lGPSX","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lGPSX is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.lGPSX);
			TmpID = env->GetFieldID(DrawMapInfoClass,"lGPSY","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lGPSY is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.lGPSY);
			TmpID = env->GetFieldID(DrawMapInfoClass,"lSpeed","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "lSpeed is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.lSpeed);
			TmpID = env->GetFieldID(DrawMapInfoClass,"nRoadDis","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "nRoadDis is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.nRoadDis);
			TmpID = env->GetFieldID(DrawMapInfoClass,"nRdDx","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "nRdDx is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.nRdDx);
			TmpID = env->GetFieldID(DrawMapInfoClass,"nRdDy","J");
			if(TmpID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "nRdDy is NULL");
				return 0;
			}
			env->SetLongField(DrawMapInfoObject,TmpID, DrawInfo.DrawMapInfo.nRdDy);
			jfieldID bRoutingID = env->GetFieldID(DrawInfoClass,"bRouting","Z");
			if(bRoutingID == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "bRoutingID is NULL");
				return 0;
			}
			env->SetBooleanField(DrawInfoobject,bRoutingID,DrawInfo.bRouting);
		}
	//}
	/*break;
	case 0xFFEE:
	{		
		JNIEnv*env= GetEnv();
		jobject newObject   = env->NewObject(m_c_Jni_Voice, m_c_jni_mid_SetState,(jboolean)pParam->lCallID);
		env->DeleteLocalRef(newObject);		
	}
	break;
	case 0xFFFF:

	{		
		JNIEnv*env= GetEnv();
		jobject newObject   = env->NewObject(m_c_Jni_Voice, m_c_jni_mid_Exit,(jboolean)pParam->lCallID);
		env->DeleteLocalRef(newObject);		
	}
	break;
      
        default:
            break;
    }*/

    return 1;
}
nuUINT SoundThreadCallBack(PCALL_BACK_PARAM pParam)
{
	
}
JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_TransferCoordinate(JNIEnv *env, jclass cls, jint nDef, jint nX, jint nY, jobject object)
{
	NURO_POINT ptInput;
	jfieldID jID     = NULL;
	if(NURO_DEF_UI_GET_MAP_TO_BMP_POINT != nDef && NURO_DEF_UI_GET_BMP_TO_MAP_POINT != nDef)
	{
		return;
	}
	ptInput.nX = nX;
	ptInput.nY = nY;
	__android_log_print(ANDROID_LOG_INFO, "navi", "NuroCore_TransferCoordinate %d %d", nX, nY);
	jobject  TmpObject = env->NewLocalRef(object);
	g_NuroCore->UIGetEngineData(nDef, (nuPVOID)&ptInput, (nuPVOID)&ptInput);
	__android_log_print(ANDROID_LOG_INFO, "navi", "NuroCore_TransferCoordinate1 %d %d", ptInput.nX, ptInput.nY);
	jclass   EngineDataClass = env->GetObjectClass(object);
	jID = env->GetFieldID(EngineDataClass,"nX","I");
	env->SetIntField(object,jID, ptInput.nX);
	jID = env->GetFieldID(EngineDataClass,"nY","I");
	env->SetIntField(object,jID, ptInput.nY);
	env->DeleteLocalRef(TmpObject);
}
JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_GetUIConfig(JNIEnv *env, jclass cls, jobject object)
{
	jfieldID jID     = NULL;
	NURO_USER_SET_DATA  UserData = {0};
	jobject  TmpObject = env->NewLocalRef(object);
	jclass   EngineDataClass = env->GetObjectClass(TmpObject);
	g_NuroCore->UIGetEngineData(NURO_DEF_UI_GET_UI_CONFIG, (nuPVOID)&UserData, NULL);
	jID = env->GetFieldID(EngineDataClass,"bNightDay","I");
	env->SetIntField(object,jID, UserData.bNightDay);
	jID = env->GetFieldID(EngineDataClass,"b3DMode","I");
	env->SetIntField(object,jID, UserData.b3DMode);
	jID = env->GetFieldID(EngineDataClass,"nLanguage","I");
	env->SetIntField(object,jID, UserData.nLanguage);
	jID = env->GetFieldID(EngineDataClass,"nMapDirectMode","I");
	env->SetIntField(object,jID, UserData.nMapDirectMode);	
	jID = env->GetFieldID(EngineDataClass, "nZoomAutoMode", "I");
	env->SetIntField(object,jID, UserData.nZoomAutoMode);
	jID = env->GetFieldID(EngineDataClass, "nPlaySpeedWarning", "I");
	env->SetIntField(object,jID, UserData.nPlaySpeedWarning);
	jID = env->GetFieldID(EngineDataClass, "nSimSpeed", "I");
	env->SetIntField(object,jID, UserData.nSimSpeed);
	jID = env->GetFieldID(EngineDataClass, "bAutoDayNight", "I");
	env->SetIntField(object,jID, UserData.bAutoDayNight);
	jID = env->GetFieldID(EngineDataClass, "nCarIconType", "I");
	env->SetIntField(object,jID, UserData.nCarIconType);
	jID = env->GetFieldID(EngineDataClass, "nDayBgPic", "I");
	env->SetIntField(object,jID, UserData.nDayBgPic);
	jID = env->GetFieldID(EngineDataClass, "nNightBgPic", "I");
	env->SetIntField(object,jID, UserData.nNightBgPic);
	jID = env->GetFieldID(EngineDataClass, "nRouteMode", "I");
	env->SetIntField(object,jID, UserData.nRouteMode);
	jID = env->GetFieldID(EngineDataClass, "bLastNavigation", "I");
	env->SetIntField(object,jID, UserData.bLastNavigation);
	jID = env->GetFieldID(EngineDataClass, "nPastMarket", "I");
	env->SetIntField(object,jID, UserData.nPastMarket);
	jID = env->GetFieldID(EngineDataClass, "nVoiceControl", "I");
	env->SetIntField(object,jID, UserData.nVoiceControl);
	jID = env->GetFieldID(EngineDataClass, "nLastScaleIndex", "I");
	env->SetIntField(object,jID, UserData.nLastScaleIndex);
	jID = env->GetFieldID(EngineDataClass, "nSpNumInput", "I");
	env->SetIntField(object,jID, UserData.nSpNumInput);
	jID = env->GetFieldID(EngineDataClass, "nKeyWordInput", "I");
	env->SetIntField(object,jID, UserData.nKeyWordInput);
	jID = env->GetFieldID(EngineDataClass, "nSpeedWarningPic", "I");
	env->SetIntField(object,jID, UserData.nSpeedWarningPic);
	jID = env->GetFieldID(EngineDataClass, "nUILanguage", "I");
	env->SetIntField(object,jID, UserData.nUILanguage);
	jID = env->GetFieldID(EngineDataClass, "nSpeedWarningVoice", "I");
	env->SetIntField(object,jID, UserData.nSpeedWarningVoice);
	jID = env->GetFieldID(EngineDataClass, "nRoadInput", "I");
	env->SetIntField(object,jID, UserData.nRoadInput);
	jID = env->GetFieldID(EngineDataClass, "nPoiInput", "I");
	env->SetIntField(object,jID, UserData.nPoiInput);
	jID = env->GetFieldID(EngineDataClass, "nScaleIdx", "I");
	env->SetIntField(object,jID, UserData.nScaleIdx);
	jID = env->GetFieldID(EngineDataClass, "nSoundVolume", "I");
	env->SetIntField(object,jID, UserData.nSoundVolume);	
	jID = env->GetFieldID(EngineDataClass, "nTMCSwitchFlag", "I");
	env->SetIntField(object,jID, UserData.nTMCSwitchFlag);
	jID = env->GetFieldID(EngineDataClass, "nTMCFrequency", "I");
	env->SetIntField(object,jID, UserData.nTMCFrequency);
	jID = env->GetFieldID(EngineDataClass, "nTMCVoice", "I");
	env->SetIntField(object,jID, UserData.nTMCVoice);
	jID = env->GetFieldID(EngineDataClass, "nTimeToNight", "I");
	env->SetIntField(object,jID, UserData.nTimeToNight);
	jID = env->GetFieldID(EngineDataClass, "nTimeToDay", "I");
	env->SetIntField(object,jID, UserData.nTimeToDay);
	jID = env->GetFieldID(EngineDataClass, "bCCDVoice", "I");
	env->SetIntField(object,jID, UserData.bCCDVoice);
	jID = env->GetFieldID(EngineDataClass, "bBigMapRoadFont", "I");
	env->SetIntField(object,jID, UserData.bBigMapRoadFont);
	jID = env->GetFieldID(EngineDataClass, "nRouteOption", "I");
	env->SetIntField(object,jID, UserData.nRouteOption);
	jID = env->GetFieldID(EngineDataClass, "nWaveTTSType", "I");
	env->SetIntField(object,jID, UserData.nWaveTTSType);
	jID = env->GetFieldID(EngineDataClass, "nShowTip", "I");
	env->SetIntField(object,jID, UserData.nShowTip);
	jID = env->GetFieldID(EngineDataClass, "nShowPOI", "I");
	env->SetIntField(object,jID, UserData.nShowPOI);
	env->DeleteLocalRef(TmpObject);
}
JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_GetNaviList(JNIEnv *env, jclass cls, jint GetIndex, jint Get_List_On_Page)
{
	jmethodID Method; 
	jclass   EngineDataClass;
	jfieldID jID     		= NULL;
	jstring wsRoadName 		= NULL;
	nuLONG nNaviListCount  		= 0;
	NURO_SET_ROUTE_LIST SetNaviList = {0};
	PNURO_NAVI_LIST pNaviList       = NULL;  
	PNURO_NAVI_LIST pTmpNaviList    = NULL;
	nuSHORT 		    i	= 0; 
	pNaviList = (PNURO_NAVI_LIST)g_NuroCore->UIGetEngineData(NURO_DEF_UI_GET_ROUTE_LIST, (nuPVOID)&SetNaviList, (nuPVOID)pTmpNaviList);
	nNaviListCount = SetNaviList.nNum;
	if(nNaviListCount == 0)
	{
		return ;
	}
	jclass JavaClass = env->FindClass("com/nuror1/NURO_NAVI_LIST$NURO_NAVI_LIST_API"); 
	Method = env->GetMethodID(JavaClass, "SetNaviList", "(IISIIIILjava/lang/String;)V");//Ljava/lang/String;
	int nIconType = 0, nDistance = 0, nX = 0, nY = 0;
	wsRoadName = env->NewString((const jchar*)pNaviList[i].wsName, (jsize)nuWcslen((const nuWCHAR*)pNaviList[0].wsName));
	jobject object = env->NewObject(JavaClass, Method, nNaviListCount, GetIndex, nIconType, nDistance, nX, nY, wsRoadName);	
	env->CallVoidMethod(object, Method, nNaviListCount, i, pNaviList[0].nIconType, pNaviList[0].nReserve, pNaviList[0].nDistance, pNaviList[0].nX, pNaviList[0].nY, wsRoadName);
	env->DeleteLocalRef(wsRoadName);
	for(i = GetIndex; i < nNaviListCount; i++)
	{		
		wsRoadName = env->NewString((const jchar*)pNaviList[i].wsName, (jsize)nuWcslen((const nuWCHAR*)pNaviList[i].wsName));
		env->CallVoidMethod(object, Method, 0, i, pNaviList[i].nIconType, pNaviList[i].nReserve, pNaviList[i].nDistance, pNaviList[i].nX, pNaviList[i].nY, wsRoadName);
		env->DeleteLocalRef(wsRoadName);
		wsRoadName = NULL;
	}
	env->DeleteLocalRef(object);
	__android_log_print(ANDROID_LOG_INFO, "navi", "Get_Navi_List OK");
}
typedef struct tagTMCDATA
{
	nuUINT		nRoadType;
	nuLONG		nDistance;
}TMCDATA, *PTMCDATA;
JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_GetTmcPathData(JNIEnv *env, jclass jcls)
{
	jmethodID Method; 
	int TMCCount = 0, Index = 0;
	int nRoadType = 0, nDistance = 0;
	TMCDATA  *pTmcPathData = NULL;
	pTmcPathData = (TMCDATA*)g_NuroCore->UIGetEngineData(NURO_DEF_UI_GET_TMC_PATH_DATA, (nuPVOID)&TMCCount, (nuPVOID)pTmcPathData);
	if(TMCCount <= 0)
	{
		return ;
	}
 	if(TMCCount >= 100)
	{
		TMCCount = 100;
	}

	jclass JavaClass = env->FindClass("com/nuror1/NuroTMCPathData$NURO_TMC_PATH_API"); 
	Method = env->GetMethodID(JavaClass, "SetTMCPathList", "(IIII)V");
	jobject object = env->NewObject(JavaClass, Method, TMCCount, Index, nRoadType, nDistance);
	env->CallVoidMethod(object, Method, TMCCount, Index, pTmcPathData[0].nRoadType, pTmcPathData[0].nDistance);//new array

	for(int i = 0; i < TMCCount; i++)
	{
		env->CallVoidMethod(object, Method, 0, i, pTmcPathData[i].nRoadType, pTmcPathData[i].nDistance);
	}
	env->DeleteLocalRef(object);
	__android_log_print(ANDROID_LOG_INFO, "navi", "NURO_DEF_UI_GET_TMC_PATH_DATA");
}
JNIEXPORT nuUINT UISetEngineCallBack(PCALL_BACK_PARAM pParam)
{	
	__android_log_print(ANDROID_LOG_INFO, "list", "UISetEngineCallBack Start");
	return nuTRUE;
}

jstring Java_com_nuror1_NuroCore_getTMCMapName(JNIEnv *env, jclass cls)
{
    return stoJstring(env, s_sMapName);
}

JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_SetSearchMode(JNIEnv *env, jclass cls, jint nMode)
{
	//return g_NuroCore->SetSearchMode(env, cls,nMode);
	__android_log_print(ANDROID_LOG_INFO, "TEST", "API :SetSearchMode ............%d",nMode);
 	return g_NuroCore->PostMessage(env, cls,_NR_MSG_SDK_SEARCH_SETSEARCHMODE,nMode,0);
}
JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_SetNameID(JNIEnv *env, jclass cls,jint nameID,jint nIdx)
{
	 __android_log_print(ANDROID_LOG_INFO, "sdkinfo", "API :Java_com_nuror1_NuroCore_SetNameID %d%d",nameID,nIdx);
	//return g_NuroCore->SetNameID(env, cls,nameID,nIdx);
	return g_NuroCore->PostMessage(env, cls,_NR_MSG_SDK_SEARCH_SETSEARCHID,nameID,nIdx);
}
JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_SetKey(JNIEnv *env, jclass cls,jint nType,jstring jstr)
{
	 __android_log_print(ANDROID_LOG_INFO, "sdkinfo", "API :Java_com_nuror1_NuroCore_SetKey %d",nType);
   	
	int nMode =0;
	if(nType == SET_SEARCH_CANDIDATE_ID)
	{
		nMode = _NR_MSG_SDK_SEARCH_SETCANDIDATEKEY;
		//return g_NuroCore->SetKey(env, cls,_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY,jstr);
	}
	if(nType == SET_SEARCH_FUZZYKEY_ID)
	{
		nMode = _NR_MSG_SDK_SEARCH_SETFUZZYKEY;
		//return g_NuroCore->SetKey(env, cls,_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY,jstr);
	}
	if(nType == SET_SEARCH_FUZZYKEY_PART_ID)
	{
		nMode = _NR_MSG_SDK_SEARCH_SETFUZZYKEY_PART;
		//return g_NuroCore->SetKey(env, cls,_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY,jstr);
	}
	if(nType == SET_SEARCH_SKB_ID)
	{	
		nMode = _NR_MSG_SDK_SEARCH_SETKEY;
		//return g_NuroCore->SetKey(env, cls,_NR_MSG_SDK_SEARCH_SETKEY,jstr);
	}
	if(nType == SET_SEARCH_PHONE_ID)
	{
		nMode = _NR_MSG_SDK_SEARCH_SETPHONEKEY; 
	}
	
	return g_NuroCore->SetKey(env, cls,nMode, jstr);
}

JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_GetListWord(JNIEnv *env, jclass cls,jint nameID, jobject object)
{
	 __android_log_print(ANDROID_LOG_INFO, "TEST", "API :GetListWord %d",nameID);
	//return g_NuroCore->PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETLISTALL,nameID,0);
	 return g_NuroCore->GetListWord(env, cls,nameID, object);
}

JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_GetKeyBoard(JNIEnv *env, jclass cls,jint nameID, jobject object)
{
	 __android_log_print(ANDROID_LOG_INFO, "TEST", "API :GetKeyBoard %d",nameID);
	return g_NuroCore->GetKeyBoard(env, cls,nameID, object);
	//return g_NuroCore->PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETKEYBOARD,nameID,0);
}

JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_GetCandidateWord(JNIEnv *env, jclass cls,jint nType, jobject object)
{
    __android_log_print(ANDROID_LOG_INFO, "TEST", "API :GetCandidateWord MODE %d %d",_NR_MSG_SDK_SEARCH_GETCANDIDATEWORD,nType);
				
   // return g_NuroCore->PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETCANDIDATEWORD,nType,0); 
    return g_NuroCore->GetCandidateWord(env, cls,nType, object);   
    
}
JNIEXPORT jboolean JNICALL Java_com_nuror1_NuroCore_GetNameALL(JNIEnv *env, jclass cls,jint nType, jobject object)
{
   __android_log_print(ANDROID_LOG_INFO, "TEST", "API :GetNameALL ............");
  return g_NuroCore->GetNameALL(env, cls,nType, object);   
}

JNIEXPORT void JNICALL Java_com_nuror1_NuroCore_MouseEvent(JNIEnv *env, jclass cls, jobject object)
{
	jfieldID jID     = NULL;
	NURO_POINT2 Pt   = {0};
	jobject  PtObject = env->NewLocalRef(object);
	jclass   PtClass = env->GetObjectClass(PtObject);
	jfieldID DataID = env->GetFieldID(PtClass, "MouseCount", "I");
     	int MouseCount =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "nMouseType", "I");
     	int nMouseType =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "VelocityX1", "I");
     	int VelocityX1 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "VelocityY1", "I");
     	int VelocityY1 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "nX1", "I");
     	int nX1 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "nY1", "I");
     	int nY1 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "VelocityX2", "I");
     	int VelocityX2 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "VelocityY2", "I");
     	int VelocityY2 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "nX2", "I");
     	int nX2 =  env->GetIntField (PtObject, DataID);
	DataID = env->GetFieldID(PtClass, "nY2", "I");
     	int nY2 =  env->GetIntField (PtObject, DataID);
	Pt.MouseCount = MouseCount;
	Pt.nMouseType = nMouseType;		
	Pt.VelocityX1 = VelocityX1;
	Pt.VelocityY1 = VelocityY1;
	Pt.nX1	      = nX1;
	Pt.nY1	      = nY1;		
	Pt.VelocityX2 = VelocityX2;
	Pt.VelocityY2 = VelocityY2;
	Pt.nX2	      = nX2;
	Pt.nY2	      = nY2;
	env->DeleteLocalRef(PtObject);
	if(nMouseType == NURO_DEF_ANDROID_ACTION_MOVE)
	{
		g_bMoveState = true;
	}
	else if(nMouseType == NURO_DEF_ANDROID_ACTION_UP)
	{
		g_bMoveState = false;
	}
	__android_log_print(ANDROID_LOG_INFO, "navi", "nMouseType %d, x1 %d, y1 %d, x2 %d, y2 %d\n", nMouseType, nX1, nY1, nX2, nY2);
	g_NuroCore->MouseEvent(Pt);
}

