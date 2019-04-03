#include "nuroGpsRelease.h"
#include "AndroidMain.h"
#include "util.inc"
#include <stdio.h>
#include <android/log.h>
#include "NuroSearchSDK.h"
#include "NURO_DEF.h"

#define CLASS_NAME_SIZE             64
#define _DLG_THD_CODE_NODE_COUNT    128

// Java Virtual Mechine
extern JavaVM *g_jvm;

extern jobject g_NuroCoreJavaObject;

extern jclass g_NuroCoreCls;
extern jclass  g_SearchOKCls;
extern jobject g_SearchOKObject;

static nuHANDLE nuTid;
static CLoadCallMgrZK    g_Loader;
static CAndroidMain      g_theApp;
UI_GET_DATA     CAndroidMain::s_UI_GET_DATA = {0};
#include <stdio.h>
static bool bRun=false;
static nuINT  nVoice=0;

bool bStart = false;
bool bNeedClose = false;

nuSHORT *gRgb565Screen = NULL;
nuSHORT *gRgb565PicScreen = NULL;
nuSHORT *gRgbPic = NULL;
nuINT *g_EventMap = NULL;
nuINT g_EventMapCount = 0;
nuCHAR g_Path[512] = {0};

static void nuAndroidCallBackTTS(nuTCHAR *tsWavPath, nuWCHAR *wsTTS)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;

    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (status < 0)
    {
        status = g_jvm->AttachCurrentThread(&env, NULL);
        if (status < 0)
        {
            return;
        }
        isAttached = true;
    }

    jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
    if (cls != 0)
    {
	if(tsWavPath == NULL)
	{
		jmethodID MethodID = env->GetStaticMethodID(cls, "PlayTTS", "(Ljava/lang/String;)V");
		if (MethodID != 0)
		{
		    	jstring TTSText = NULL;
			TTSText = env->NewString((const jchar*)wsTTS, (jsize)nuWcslen((const nuWCHAR*)wsTTS));		
			if(TTSText == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "TTSText is NULL");
				return ;
			}			
		    	env->CallStaticVoidMethod(cls, MethodID, TTSText);
		}
	}
	else
	{
		jmethodID MethodID = env->GetStaticMethodID(cls, "PlayWav", "(Ljava/lang/String;)V");
		if (MethodID != 0)
		{
		    	jstring WavPath = NULL;
			WavPath = env->NewStringUTF(tsWavPath);		
			if(WavPath == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, "navi", "WavPath is NULL");
				return ;
			}			
		    	env->CallStaticVoidMethod(cls, MethodID, WavPath);
		}
	}
        env->DeleteLocalRef(cls); 
    }

    if (isAttached)// From native thread
    {
        g_jvm->DetachCurrentThread();
    }
}

/*ONEKEYBUFF CAndroidMain::s_SEND_OneKeyBuff = {0};
OneKeyToNavi        CAndroidMain::s_OneKeyToNavi;
nuWCHAR             CAndroidMain::s_wPhoneNumber[20];
nuWCHAR             CAndroidMain::s_wHSMac_UID[20];
MAINCTRLVER         CAndroidMain::s_MainCtrlVer;
ShowPng             CAndroidMain::s_ShowPng;
*/
nuWCHAR             CAndroidMain::s_wHSMac_UID[20];
nuBOOL CAndroidMain::SetVolume(JNIEnv * env, jclass cls,nuINT nParser)
{
	nVoice = nParser;	
	return nuTRUE;
}

static nuVOID CallInt4JavaFunction(const char *functionName, int lat, int lng, int dir, int speed)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;

    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (status < 0)
    {
        status = g_jvm->AttachCurrentThread(&env, NULL);
        
        if (status < 0)
        {
            return;
        }

        isAttached = true;
    }

    jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
    if (cls != 0)
    {
        jmethodID fID = env->GetStaticMethodID(cls, functionName, "(IIII)V");

        if (functionName != 0)
        {
            env->CallStaticVoidMethod(cls, fID, lat, lng, dir, speed);
        }
        env->DeleteLocalRef(cls); // 甇文??閬??橘?撅?禿eference ????12銝芣憭批?
    }

    if (isAttached)
    {
        g_jvm->DetachCurrentThread();
    }
}

static nuVOID CallIntJavaFunction(const char *functionName, int n)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;

    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (status < 0)
    {
        status = g_jvm->AttachCurrentThread(&env, NULL);
        
        if (status < 0)
        {
            return;
        }

        isAttached = true;
    }

    jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
    if (cls != 0)
    {
        jmethodID fID = env->GetStaticMethodID(cls, functionName, "(I)V");

        if (functionName != 0)
        {
            env->CallStaticVoidMethod(cls, fID, n);
        }
        env->DeleteLocalRef(cls); 
    }

    if (isAttached)
    {
        g_jvm->DetachCurrentThread();
    }
}


static nuVOID CallVoidJavaFunction(const char *functionName)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;

    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (status < 0)
    {
        status = g_jvm->AttachCurrentThread(&env, NULL);
        
        if (status < 0)
        {
            return;
        }

        isAttached = true;
    }

    jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
    if (cls != 0)
    {
        jmethodID fID = env->GetStaticMethodID(cls, functionName, "()V");

        if (functionName != 0)
        {
            env->CallStaticVoidMethod(cls, fID);
        }
        env->DeleteLocalRef(cls); 
    }

    if (isAttached)
    {
        g_jvm->DetachCurrentThread();
    }
}


static void CallStringJavaFunction(const char* functionname,const char* cityId)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;
  
    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
 
    if (status < 0)
    {
       status = g_jvm->AttachCurrentThread(&env, NULL);
       if (status < 0)
       {
          return;
       }
      
       isAttached = true;
     }
   
     jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
     
     if (cls != 0)
     {   
        jmethodID fID = env->GetStaticMethodID(cls, functionname, "(Ljava/lang/String;)V");
        jstring strCityId = env->NewStringUTF(cityId);
        
        if (functionname != 0)
        {
            env->CallStaticVoidMethod(cls, fID, strCityId);
        }

        env->DeleteLocalRef(cls); 

     }   
            
     if (isAttached)
     {   
         g_jvm->DetachCurrentThread();
     }   

}


static void CallString6JavaFunction(const char* functionname,const char* servicecenter,const char* nearinfo,const char* onekey,const char* roadservice,
        const char* sms,const char* trackph)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;
  
    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
 
    if (status < 0)
    {
       status = g_jvm->AttachCurrentThread(&env, NULL);
       if (status < 0)
       {
          return;
       }
      
       isAttached = true;
     }
   
     jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
     if (cls != 0)
     {   
        jmethodID fID = env->GetStaticMethodID(cls, functionname, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        
        //chat to jstring;
        jstring strServiceCenter = env->NewStringUTF(servicecenter);
        jstring strNearInfo = env->NewStringUTF(nearinfo);
        jstring strOneKey  =  env->NewStringUTF(onekey);
        jstring strRoadService = env->NewStringUTF(roadservice);
        jstring strSMS         = env->NewStringUTF(sms);
        jstring strTrackPH     = env->NewStringUTF(trackph);

        
        //////////////////////////////////
        if (functionname != 0)
        {
            env->CallStaticVoidMethod(cls, fID, strServiceCenter, strNearInfo, strOneKey, strRoadService,strSMS,strTrackPH);
        }
        env->DeleteLocalRef(cls); // 甇文??閬??橘?撅?禿eference ????12銝芣憭批?
     }   
            
     if (isAttached)
     {   
         g_jvm->DetachCurrentThread();
     }   

}
static nuVOID AndroidFlush(nuPVOID buf, nuINT x, nuINT y)
{
    //TODO call android java layer callback function
    
    /*
    JNIEnv *curr_env = NULL;

    //curr_env = GetEnv();
    g_jvm->AttachCurrentThread(&curr_env, NULL);

    // = curr_env->FindClass("com/nuror1/NuroCore");
    jmethodID flushSurface = curr_env->GetStaticMethodID(g_NuroCoreCls, "flushSurface", "()V");
    curr_env->CallStaticVoidMethod(g_NuroCoreCls, flushSurface);

    g_jvm->DetachCurrentThread();

    */

    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;
    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (status < 0)
    {
        status = g_jvm->AttachCurrentThread(&env, NULL);
        
        if (status < 0)
        {
            return;
        }

        isAttached = true;
    }

    jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
    if (cls != 0)
    {
        jmethodID flushSurface = env->GetStaticMethodID(cls, "flushSurface", "()V");

        if (flushSurface != 0)
        {
            env->CallStaticVoidMethod(cls, flushSurface);
        }
        env->DeleteLocalRef(cls); 
    }

    if (isAttached)// From native thread
    {
        g_jvm->DetachCurrentThread();
    }
}

nuDWORD ExitCallback(nuPVOID arg_addr)
{
    while (1) 
    {   
        nuSleep(300); 
        if (bNeedClose && bStart)
        {
            {
                 char buf[256];
                 sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
                 __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
            
            bStart = false;
            bNeedClose = false;

            nuSleep(300); 

            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
            g_Loader.OnDestroyDialog();
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }
           
            {
              char buf[256];
                       sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
            }


            int status;
            JNIEnv *env = NULL;
            bool isAttached = false;

            status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

            {
               char buf[256];
               sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
             }

            if (status < 0)
            {
                status = g_jvm->AttachCurrentThread(&env, NULL);
        
                isAttached = true;
            
                {
                      char buf[256];
                      sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
                 __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
                }

            
            }

            jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
    
            if (cls != 0)
            {
                {    
                   char buf[256];
                  sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
                   __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
                  }
                jmethodID callExit = env->GetStaticMethodID(cls, "callExit", "()V");

                if (callExit != 0)
                {
                    {    
                        char buf[256];
                        sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
                        __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
                    }
                    env->CallStaticVoidMethod(cls, callExit);
                }
            }

            if (isAttached)// From native thread
            {
                {    
                            char buf[256];
                            sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
                           __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
                  }
                g_jvm->DetachCurrentThread();
            }

            {    
              char buf[256];
                 sprintf(buf, "%s:%d\n", __FILE__, __LINE__);
              __android_log_print(ANDROID_LOG_INFO,  "navi" ," %s", buf);
              }

            /*
            for( int i = 0; i < 480; ++i)
            {   
                for(int j = 0; j < 800; ++j)
                {   
                    gRgb565Screen[i * 800 + j] = 0x8711;
                }   
            }   

            SDL_Flip(gSdlScreen);
            dumpcheck();
            */

            break;
        }
    }
    return 0;
}

nuHANDLE OpenMaster()
{
    return nuCreateThread(NULL, 0, ExitCallback, NULL, 0, NULL);
}

nuVOID CloseMaster(nuHANDLE nuThread)
{
    nuDelThread(nuThread);
}

nuINT CAndroidMain::FindBlock(const nuCHAR *sID)
{
    for (nuINT iIndex = 0; iIndex < Q_SIZE; iIndex++)
    {
        if (m_EventTable[iIndex].sID && (0 == nuStrcmp(sID, m_EventTable[iIndex].sID)))
        {
            return iIndex;
        }
    }

    return -1;
}

nuINT CAndroidMain::FindFirstFreeBlock()
{
    for (nuINT iIndex = 0; iIndex < Q_SIZE; iIndex++)
    {
        if (!m_EventTable[iIndex].sID)
        {
            return iIndex;
        }
    }

    return -1;
}

nuBOOL CAndroidMain::AddEventTable(const nuCHAR *sID, nuINT e)
{
    nuINT i = FindFirstFreeBlock();

    if (-1 == i)
    {
        return nuFALSE;
    }

    m_EventTable[i].sID = sID;
    m_EventTable[i].e = e;

    return nuTRUE;
}

nuBOOL CAndroidMain::SetEventTable(const nuCHAR *sID, nuINT e)
{
    nuINT i = FindBlock(sID);
    
    if (-1 == i)
    {
        return nuFALSE;
    }
    
    m_EventTable[i].e = e;
    
    return nuTRUE;
}
    
nuBOOL CAndroidMain::GetEventTable(const nuCHAR *sID, nuINT& e)
{
    nuINT i = FindBlock(sID);
    
    if (-1 == i)
    {
        return nuFALSE;
    }
    
    e = m_EventTable[i].e;
    
    return nuTRUE;
}
    
nuBOOL CAndroidMain::SetNuroPath(JNIEnv * env, jclass cls, jstring path)
{
    char buf[256];
    nuSetModulePath(jstringTostring2(buf, 256, env, path));
    return nuTRUE;
}

nuBOOL CAndroidMain::SetNuroBuffer(JNIEnv * env, jclass cls, nuPVOID obj, nuINT w, nuINT h)
{
    if(w == 0)
    {
	__android_log_print(ANDROID_LOG_INFO, "navi", "gRgb565Screen OK.....");
    	gRgb565Screen = (nuSHORT *)env->GetDirectBufferAddress((jobject)obj);
    }
    else if(w == 1)
    {
	__android_log_print(ANDROID_LOG_INFO, "navi", "gRgb565Pic OK.....");
	gRgb565PicScreen = (nuSHORT *)env->GetDirectBufferAddress((jobject)obj);
    }
    nuSetFlushCallback(AndroidFlush);
    return nuTRUE;
}

nuBOOL CAndroidMain::SetNuroPicBuffer(JNIEnv * env, jclass cls, nuPVOID obj, nuINT w, nuINT h)
{
     __android_log_print(ANDROID_LOG_INFO, "Hsin", "SetNuroPicBuffer OK.....");
     gRgb565PicScreen = (nuSHORT *)env->GetDirectBufferAddress((jobject)obj);
    
   /* if(bStart)
    {
        COPYDATASTRUCT pCopyDataStruct;
        pCopyDataStruct.dwData = 0x0009;

        pCopyDataStruct.lpData = gRgb565PicScreen;
        g_Loader.UISetEngine( pCopyDataStruct.dwData, pCopyDataStruct.lpData,NULL);
 	__android_log_print(ANDROID_LOG_INFO, "Hsin", "CopyData OK.....");
    }*/
    return nuTRUE;
}

nuBOOL CAndroidMain::SetNuroEventMap(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal1)
{
    g_EventMap = (nuINT *)env->GetDirectBufferAddress((jobject)obj);
    g_EventMapCount = nVal1;
    return nuTRUE;
}

nuBOOL CAndroidMain::InjectEventMap(JNIEnv * env, jclass cls, nuPVOID obj)
{
	// TODO
	nuCHAR *pName;
	// FILE *fp;
	jobject temp;
	temp = env->NewLocalRef((jobject)obj) ;
	pName = jstringTostring(env, getObjectClassName(env, (jobject)temp));
	env->DeleteLocalRef(temp);

	return AddEventTable(pName, nuFALSE);
}


nuINT CAndroidMain::GetFromEventMap(JNIEnv * env, jclass cls, nuPVOID obj)
{
	// TODO
	nuINT e = nuFALSE;
	nuCHAR className[CLASS_NAME_SIZE];

	jobject temp;
	temp = env->NewLocalRef((jobject)obj) ;

	jstringTostring2(className, sizeof (className), env, getObjectClassName(env, (jobject)temp));
	GetEventTable(className, e);
	env->DeleteLocalRef(temp);
    	return e;
}


nuVOID CAndroidMain::SetToEventMap(JNIEnv * env, jclass cls, nuPVOID obj, nuINT nVal)
{
	// TODO
	nuINT e;
	nuCHAR className[CLASS_NAME_SIZE];

	jobject temp;
	temp = env->NewLocalRef((jobject)obj);

	jstringTostring2(className, sizeof (className), env, getObjectClassName(env, (jobject)temp));
	SetEventTable(className, nVal);
	env->DeleteLocalRef(temp);
}

bool g_AudioNewUse = false;

nuCHAR * CAndroidMain::GetPreparedAudio(JNIEnv * env, jclass cls)
{
    if (g_AudioNewUse)
    {
        return (char *)"none-null";
    }
	
    return (char *)"null";
}

void CAndroidMain::SetPreparedAudio(JNIEnv * env, jclass cls)
{
    g_AudioNewUse = false;
}

nuINT CAndroidMain::GetVolume(JNIEnv * env, jclass cls)
{
    return nVoice;
}

//Prosper add new callback,2014,12,16
nuBOOL CAndroidMain::StartNuro(JNIEnv * env, jclass cls,nuPVOID pHud,nuPVOID pHudDR,nuPVOID pVoice,nuPVOID pCallBack)
{
#ifdef NURO_DEBUG
    __android_log_print(ANDROID_LOG_INFO,"navi","===============StartNuro");
#endif
    // TODO
    if (bStart)
    {
        return nuFALSE;
    }
#ifdef NURO_DEBUG
    __android_log_print(ANDROID_LOG_INFO,"navi","===============StartNuro...");
#endif
    if (nuFALSE == g_Loader.OnInitDialog(
                pCallBack, 
                NULL, 
                NULL,
                (nuPVOID)nuAndroidCallBackTTS,
                NULL)
            )
    {
        return nuFALSE;
    }
  
    bStart = true;
#ifdef NURO_DEBUG
    __android_log_print(ANDROID_LOG_INFO,"navi","end=========StartNuro");
#endif 
    return nuTRUE;
}

nuBOOL CAndroidMain::GetNuroState(JNIEnv * env, jclass cls)
{
    if(bStart)
        __android_log_print(ANDROID_LOG_INFO,"navi","CandroidMian::GetNuroState()====bStart is true");
    else
        __android_log_print(ANDROID_LOG_INFO,"navi","CAndroidMain::GetNuroState()====bStart is false");
    
    return bStart;
}

nuBOOL CAndroidMain::GoPreDialog(JNIEnv * env, jclass cls)
{
    NURO_UI_MESSAGE msg;
    msg.nMsgID = 0x9604;
    g_Loader.Message(msg);
    return nuTRUE;
}
    
static nuWCHAR s_uid[32];

nuVOID CAndroidMain::StopNuro(JNIEnv * env, jclass cls)
{
    CloseMaster(nuTid);
    // g_Loader.OnDestroyDialog();
}

nuBOOL CAndroidMain::OnMouseDown(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2)
{
    if (bStart)
    {
        g_Loader.MouseDown(nVal1, nVal2);
    }

    return nuTRUE;
}


nuBOOL CAndroidMain::OnMouseUp(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2)
{
    // TODO
    if (bStart)
    {
        g_Loader.MouseUp(nVal1, nVal2);
    }

    return nuTRUE;
}


nuBOOL CAndroidMain::OnMouseMove(JNIEnv * env, jclass cls, nuINT nVal1, nuINT nVal2)
{
    // TODO
    if (bStart)
    {
        g_Loader.MouseMove(nVal1, nVal2);
    }

    return nuTRUE;
}
nuBOOL CAndroidMain::onKeyDown(JNIEnv * env, jclass cls, nuINT nVal1)
{
    __android_log_print(ANDROID_LOG_INFO, "navi", "KeyDown %d",nVal1);
    g_Loader.KeyDown(nVal1);
    return nuTRUE;
}
nuBOOL CAndroidMain::onKeyUp(JNIEnv * env, jclass cls, nuINT nVal1)
{
    __android_log_print(ANDROID_LOG_INFO, "navi", "KeyUp %d",nVal1);
    g_Loader.KeyUp(nVal1);
    return nuTRUE;
}
#ifdef ANDROID_D
extern GPSDATA g_gpsdata;

#endif
nuBOOL CAndroidMain::SetGps(JNIEnv * env, jclass cls, nuPVOID obj)
{
	// TODO
	return nuFALSE;
	#ifdef ANDROID_D
	jobject temp;
	temp = env->NewLocalRef(obj) ;

	jobject status = (jobject)temp;

	jclass status_cls = env->GetObjectClass(status);
	jfieldID fid;
	jobjectArray objArray;
	jint length;
	bool isOK;
	int satellitesNum;
	double latitude;
	double longitude;
	double altitude;
	float bear;
	float speed;
	int year;
	int month;
	int date;
	int hour;
	int minute;
	int second;

	fid = env->GetFieldID(status_cls, "isOK", getSignatureVal("boolean"));
	isOK = (bool)env->GetBooleanField(status, fid);
	satellitesNum = (int)env->GetIntField(status, env->GetFieldID(status_cls, "satellitesNum", getSignatureVal("int")));
	latitude = (double)env->GetDoubleField(status, env->GetFieldID(status_cls, "latitude", getSignatureVal("double")));
	longitude = (double)env->GetDoubleField(status, env->GetFieldID(status_cls, "longitude", getSignatureVal("double")));
	altitude = (double)env->GetDoubleField(status, env->GetFieldID(status_cls, "altitude", getSignatureVal("double")));
	bear = (float)env->GetFloatField(status, env->GetFieldID(status_cls, "bear", getSignatureVal("float")));
	speed = (float)env->GetFloatField(status, env->GetFieldID(status_cls, "speed", getSignatureVal("float")));
	year = (int)env->GetIntField(status, env->GetFieldID(status_cls, "year", getSignatureVal("int")));
	month = (int)env->GetIntField(status, env->GetFieldID(status_cls, "month", getSignatureVal("int")));
	date = (int)env->GetIntField(status, env->GetFieldID(status_cls, "date", getSignatureVal("int")));
	hour = (int)env->GetIntField(status, env->GetFieldID(status_cls, "hour", getSignatureVal("int")));
	minute = (int)env->GetIntField(status, env->GetFieldID(status_cls, "minute", getSignatureVal("int")));
	second = (int)env->GetIntField(status, env->GetFieldID(status_cls, "second", getSignatureVal("int")));


	if (isOK)
	{
		g_gpsdata.nStatus = 1;
	}
	else
	{
		g_gpsdata.nStatus = 0;
	}

	Transform((float)(longitude * 100), (float)(latitude * 100), (nuUINT)altitude, 
	    (nuUINT&)g_gpsdata.nLongitude, (nuUINT&)g_gpsdata.nLatitude);

	g_gpsdata.nAngle = ((360 - (nuSHORT)bear) + 90) % 360;      //
	g_gpsdata.nSpeed = (nuSHORT)speed;		            //
	g_gpsdata.nYear = (nuWORD)year;
	g_gpsdata.nMonth = (nuBYTE)month;
	g_gpsdata.nDay = (nuBYTE)date;
	g_gpsdata.nHours = (nuBYTE)hour;
	g_gpsdata.nMinutes = (nuBYTE)minute;
	g_gpsdata.nSeconds = (nuBYTE)second;
	g_gpsdata.nStarNumUsed = (nuBYTE)satellitesNum;	    //
	g_gpsdata.nActiveStarNum = (nuBYTE)satellitesNum;
	g_gpsdata.nGyroState = (nuBYTE)0;
	g_gpsdata.nReserve = (nuBYTE)2;                     // 0:GPS no work; 1:GPS 2D; 2:GPS 3D
	g_gpsdata.nAntennaHeight = (nuLONG)altitude;	    //
	g_gpsdata.nReciveTimes = (nuDWORD)0;                //

	fid = env->GetFieldID(status_cls, "satlist", "[Lcom/nuro/r1/shlib/SatellitesStatus;");
	objArray = (jobjectArray)env->GetObjectField(status, fid);
	length = env->GetArrayLength(objArray);

	for (jint i = 0; i < satellitesNum; i++)
	{
		jobject tmpobject = env->GetObjectArrayElement(objArray, i);
		jobject stlist;
		stlist = env->NewLocalRef(tmpobject) ;
		jclass stlist_cls = env->GetObjectClass(stlist);
		g_gpsdata.pStarList[i].nStarID = (nuSHORT)env->GetIntField(stlist, env->GetFieldID(stlist_cls, "PRN", getSignatureVal("int")));
		g_gpsdata.pStarList[i].nAngle_elevation = (nuSHORT)env->GetFloatField(stlist, env->GetFieldID(stlist_cls, "elev", getSignatureVal("float")));
		g_gpsdata.pStarList[i].nSignalNoiseRatio = (nuSHORT)env->GetFloatField(stlist, env->GetFieldID(stlist_cls, "snr", getSignatureVal("float")));
		g_gpsdata.pStarList[i].nAngle_position = (nuSHORT)env->GetFloatField(stlist, env->GetFieldID(stlist_cls, "azi", getSignatureVal("float")));
		env->DeleteLocalRef(stlist);
	}
	env->DeleteLocalRef(status);
#endif
 	
	
    return nuTRUE;
}




nuBOOL CAndroidMain::Transform(nuFLOAT fLng, nuFLOAT fLat, nuUINT unHeight, 
                    nuUINT& unLngOut, nuUINT& unLatOut)
{
    unsigned int unLng;
    unsigned int unLat;
    
    {
        long nD, nF;
        nD = (long)(fLng/100);
        nF = (long)((fLng-(nD*100))*1000);
        unLng   = nD * 100000 + nF;
    }

    {
        long nD, nF;
        nD = (long)(fLat/100);
        nF = (long)((fLat-(nD*100))*1000);
        unLat   = nD * 100000 + nF;
    }

    return DoTransform(unLng, unLat, unHeight, unLngOut, unLatOut);
}

unsigned int wgtochina_lb(int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit, int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat);

nuBOOL CAndroidMain::DoTransform(nuUINT unLng, nuUINT unLat, nuUINT unHeight, 
        nuUINT& unLngOut, nuUINT& unLatOut)
{
    nuUINT unLng_mg = unLng * (1.024 * 36);
    nuUINT unLat_mg = unLat * (1.024 * 36);

   /* if (wgtochina_lb(1, unLng_mg, unLat_mg, unHeight, 1543, 153200231, &unLngOut, &unLatOut))
    {
        return nuFALSE;
    }
*/
    unLngOut = (nuUINT)((unLngOut * 1.0) / (1.024 * 36));
    unLatOut = (nuUINT)((unLatOut * 1.0) / (1.024 * 36));

    return nuTRUE;
}
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
#include <stdio.h>
static nuCHAR temp[1024]={0};
static nuINT m_nIdx = -1;
nuBOOL CAndroidMain::SetGpsNema(JNIEnv * env, jclass cls, jstring GpsData, jboolean bOK)
{
    nuINT nLen = 0;

    temp[0] = 0;
    jstringTostring3(temp, sizeof (temp), &nLen, env, GpsData);
    temp[nLen] = 0;

    if (!bStart) {
        return false;
    }
    
   /* {
        FILE *fp = fopen("/mnt/sdcard/card/mylog.txt", "a+");
        fwrite(temp, 1, nLen, fp);
        fclose(fp);
    }*/
    if(m_nIdx<127)
	{
		m_nIdx++;
	}
	else
	{
		m_nIdx=0;
	}
	/*nuMemset(s_UI_GET_DATA.GPS_DATA.sGPSDATA[m_nIdx],0,sizeof(s_UI_GET_DATA.GPS_DATA.sGPSDATA[m_nIdx]));
	nuMemcpy(s_UI_GET_DATA.GPS_DATA.sGPSDATA[m_nIdx],temp,nLen);	
	NURO_UI_MESSAGE nrMsg = {0};
    nrMsg.nMsgID  =  _NR_MSG_ANDROID_GPS_;
	nrMsg.nParam1 = m_nIdx;
	nrMsg.nParam2 = nLen;
    g_Loader.Message(nrMsg);	*/
    g_Loader.GPS_JNI_RecvData(temp, nLen);	
    
    return nuTRUE;
}

nuBOOL CAndroidMain::SetGpsNema1(JNIEnv * env, jclass cls, jstring GpsData,jboolean  bOK)
{
    nuCHAR ss[3]={0};
    nuCHAR cs[256]={0};
    const nuCHAR* c = env->GetStringUTFChars (GpsData, NULL);
    nuStrcpy(cs, c);
    nuBOOL bok=true;	


    if(bOK && nuStrlen(cs) >1 )
    {
        //pCallMgr->GPS_RecvData(cs,strlen(cs));	
        g_Loader.GPS_JNI_RecvData(cs, nuStrlen(cs));	

        /*FILE *pfile = NULL;
          pfile = fopen("/mnt/sdcard/AFTER_CAndroidMain.txt","a+");
          fprintf(pfile,"%d:%s\n",nuStrlen(cs),cs );	
          fclose(pfile);
          */
    }
    else
    {
 
        if(nuStrlen(cs)<122 && nuStrlen(cs)>1);
        {
            Cal_NMEA_Chksum(cs,ss);

            //kick long data by , 
            nuINT count=0;        
            if(cs[2]=='G')
            {
                for(nuINT j=0;j<nuStrlen(cs);j++)
                {	
                    if(cs[j]==',')
                        count++;
                }

            }

            if (count >20)
                bok=false;

            //蝯蕭?摰?嚙緯ema
            nuCHAR str[256]="";
            nuStrcat(str,"$"); 
            nuStrcat(str,c);
            nuStrcat(str,"*");
            nuStrcat(str,ss);	
            nuStrcpy(cs,str);
            nuINT len=nuStrlen(cs);
            cs[len]=0x0D;
            cs[len+1]=0x0A;
            cs[len+2]=0;



            if(len>1 && nuStrcmp(temp,cs)!=0 && bok)
            {

                nuMemset(temp,0,sizeof(temp));
                nuStrcpy(temp,cs);

                g_Loader.GPS_JNI_RecvData(cs,nuStrlen(cs));
                //pCallMgr->GPS_RecvData(cs,strlen(cs));

            }
        }
    }	
    return nuTRUE;
}

nuBOOL CAndroidMain::SendPoiStringUTF16BE(JNIEnv *env, jclass cls, jbyteArray buffer)
{
    return nuTRUE;
}


#ifdef USE_HUD_DR_CALLBACK 
nuUINT CAndroidMain::setHUDcall(nuVOID* pFun)
{
    g_Loader.SetHUDCallBackFunc((void *)pFun);
}
nuUINT CAndroidMain::setDRcall(nuVOID* pFun)
{
    g_Loader.SetDRCallBackFunc((void *)pFun);
}
#endif

#ifdef ANDROID
nuUINT CAndroidMain::setSOUNDcall(nuVOID* pFun)
{
    g_Loader.SetSOUNDCallBackFunc((void *)pFun);
    return 0;
}
#endif

nuBOOL CAndroidMain::WindowProc(nuUINT message, nuDWORD wparam, nuDWORD lparam)
{
  

    return nuTRUE;
}
    
void CAndroidMain::PickUpPhone()
{
    CallVoidJavaFunction("pickUp");
}

void CAndroidMain::HangUpPhone()
{
    CallVoidJavaFunction("hangUp");
}

void CAndroidMain::Dial(int nNo)
{
    CallIntJavaFunction("dial", nNo);
}

void CAndroidMain::SendPostion(int lat, int lng, int direction, int speed)
{
    CallInt4JavaFunction("sendPosition", lat, lng, direction, speed);
}
void CAndroidMain::DestCancel()
{
    CallVoidJavaFunction("destcancel");
}

void CAndroidMain::Volume(int n)
{
    CallIntJavaFunction("volume", n);
}


void CAndroidMain::setCityId(const char *cityId)
{
    CallStringJavaFunction("setCallBackCityId", cityId);
}


//Prosper add new roadlist cal(),2014,12,16
typedef struct _SYSTEMTIME {
    nuWORD wYear;
    nuWORD wMonth;
    nuWORD wDayOfWeek;
    nuWORD wDay;
    nuWORD wHour;
    nuWORD wMinute;
    nuWORD wSecond;
    nuWORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#define SECONDOFYEAR 31536000
#define MAXSECONDOFDAY  ((24)*(3600))
#define SECONDOFROUNDYEAR 31622400
bool IsRound(int year)
{
	/*is round year?*/

	if((year%100)&&(year%4==0)) return 1;
	if((year%100==0)&&(year%400==0)) return 1;
	return 0;

}
unsigned int SecondsFrom1970(nuUINT year,nuUINT month,nuUINT day,nuUINT hour,nuUINT minu,nuUINT second)
{
	SYSTEMTIME st;
	unsigned int tTemp=0;
	unsigned int tSecond=0;
	int month_s[2][12]={{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}};
	int nDays=0;
	int nCount=0;
	int i;
	int j;
	//GetLocalTime(&st);
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	st.wHour = hour;
	st.wMinute = hour;
	st.wSecond = second;
	//////////////////////////////////////////////////
	tSecond=st.wHour*3600+st.wMinute*60+st.wSecond;

	for (i=1970;i<st.wYear;++i)
	{

		if (IsRound(i))
			++nCount;
	}


	tTemp+=(st.wYear-1970-nCount)*SECONDOFYEAR+nCount*SECONDOFROUNDYEAR;

	if (st.wMonth>1)
	{
		if (IsRound(st.wYear))
		{
			for (j=0;j<st.wMonth-1;++j)
			{
				tTemp+=month_s[1][j]*MAXSECONDOFDAY;
			}
			tTemp+=(st.wDay-1)*MAXSECONDOFDAY+tSecond;
		}
		else
		{
			for (j=0;j<st.wMonth-1;++j)
			{
				tTemp+=month_s[0][j]*MAXSECONDOFDAY;
			}
			tTemp+=(st.wDay-1)*MAXSECONDOFDAY+tSecond;
		}
	}
	else
	{
		tTemp+=(st.wDay-1)*MAXSECONDOFDAY+tSecond;
	}
	return tTemp;
}
int Base64_Encode(char *Src, int SrcLength,  char *OutputBuffer, int OutputBufferLength)
{
	int GroupCount;
	int GroupResidualLength;
	int EncodeResultLength;
	int i;
	unsigned char *SrcData;
	unsigned char PaddingBuffer[3];
	static char Base64_EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 

	/*?摰??撜?頝箄飲隤寧??/
	GroupCount = SrcLength / 3;
	GroupResidualLength = SrcLength % 3;
	EncodeResultLength = GroupCount * 4;
	if(0 != GroupResidualLength)
		EncodeResultLength = EncodeResultLength + 4;


	if(EncodeResultLength > OutputBufferLength)
	{
		/*??日?喉蟡仿?/
		return 0;
	}

	/*?日?阡*/
	SrcData = (unsigned char *)Src;
	for(i = 0; i < GroupCount; i++)
	{
		OutputBuffer[i*4 + 0]= Base64_EncodeTable[ ( SrcData[i*3 + 0]&0xFC )>>2 ];
		OutputBuffer[i*4 + 1]= Base64_EncodeTable[ ((SrcData[i*3 + 0]&0x03)<<4) + ((SrcData[i*3 + 1]&0xF0)>>4) ];
		OutputBuffer[i*4 + 2]= Base64_EncodeTable[ ((SrcData[i*3 + 1]&0x0F)<<2) + ((SrcData[i*3+ 2]&0xC0)>>6) ];
		OutputBuffer[i*4 + 3]= Base64_EncodeTable[ SrcData[i*3 + 2]&0x3F ]; 
	}

	/*?日撣斤?芾??掩頞潸疚*/
	if(0 != GroupResidualLength)
	{
		PaddingBuffer[0] = 0;
		PaddingBuffer[1] = 0;
		PaddingBuffer[2] = 0;
		for(i = 0; i < GroupResidualLength; i++)
			PaddingBuffer[i] = SrcData[GroupCount*3 + i];

		OutputBuffer[GroupCount*4 + 0]= Base64_EncodeTable[ ( PaddingBuffer[0]&0xFC )>>2 ];
		OutputBuffer[GroupCount*4 + 1]= Base64_EncodeTable[ ((PaddingBuffer[0]&0x03)<<4) + ((PaddingBuffer[1]&0xF0)>>4) ];
		OutputBuffer[GroupCount*4 + 2]= Base64_EncodeTable[ ((PaddingBuffer[1]&0x0F)<<2) + ((PaddingBuffer[2]&0xC0)>>6) ];
		OutputBuffer[GroupCount*4 + 3]= Base64_EncodeTable[ PaddingBuffer[2]&0x3F ]; 

		if(GroupResidualLength == 1)
		{
			OutputBuffer[GroupCount*4 + 2]= '=';
			OutputBuffer[GroupCount*4 + 3]= '='; 
		}
		else if (GroupResidualLength == 2)
		{
			OutputBuffer[GroupCount*4 + 3]= '='; 
		}
	}
	return EncodeResultLength;
}

nuWCHAR* CharToWchar(const nuCHAR* c)  

{  

	nuWCHAR *m_wchar;
/*
	nuINT len = MultiByteToWideChar(CP_ACP,0,c,strlen(c),NULL,0);  

	m_wchar=new nuWCHAR[len+1];  

	MultiByteToWideChar(CP_ACP,0,c,strlen(c),m_wchar,len);  

	m_wchar[len]='\0';  
*/
	return m_wchar;  

}
static void CallTMCString6JavaFunction(const char* functionname,const char* str)
{
    int status;
    JNIEnv *env = NULL;
    bool isAttached = false;
  
    status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
 
    if (status < 0)
    {
       status = g_jvm->AttachCurrentThread(&env, NULL);
       if (status < 0)
       {
          return;
       }
      
       isAttached = true;
     }
   
     jclass cls = env->GetObjectClass(g_NuroCoreJavaObject);
     if (cls != 0)
     {   
        jmethodID fID = env->GetStaticMethodID(cls, functionname, "(Ljava/lang/String;)V");
        
        //chat to jstring;
        jstring jstr = env->NewStringUTF(str);
     /*   jstring strNearInfo = env->NewStringUTF(nearinfo);
        jstring strOneKey  =  env->NewStringUTF(onekey);
        jstring strRoadService = env->NewStringUTF(roadservice);
        jstring strSMS         = env->NewStringUTF(sms);
        jstring strTrackPH     = env->NewStringUTF(trackph);*/

        
        //////////////////////////////////
        if (functionname != 0)
        {
            env->CallStaticVoidMethod(cls, fID, jstr);
        }

	  env->DeleteLocalRef(jstr);
      env->DeleteLocalRef(cls); // 甇文??閬??橘?撅?禿eference ????12銝芣憭批?
	  //env->DeleteLocalRef(newObject);
     }   
            
     if (isAttached)
     {   
         g_jvm->DetachCurrentThread();
     }   

}
char* urlEncode(char* s)
{
	int len = nuStrlen(s);//s.GetLength();
	char *out = new char[len*9+1];
	nuMemset(out , 0 , len*9+1);
	int i , j;
	int ch = 0 ;

	static char  myhex[0xFF+1][4];  //add by zhouzd 2008-10-06
	static bool isinital = false;

	if ( !isinital )
	{
		for ( i = 0 ; i <= 0xFF ; ++i )
		{
			myhex[i][0] = '%';
			sprintf( myhex[i]+1 , "%02X" , i );
		}
		isinital = true;
	}

	for (i = 0 , j = 0; i < len ; ++i )
	{
		ch = s[i];//.GetAt(i);

		//printf("%c\n" , s.GetAt(i) );

		if ('A' <= ch && ch <= 'Z')         // 'A'..'Z'
		{
			out[j++] = ch;
		}
		else if ('a' <= ch && ch <= 'z')    // 'a'..'z'
		{
			out[j++] = ch;
		}
		else if ('0' <= ch && ch <= '9')    // '0'..'9'
		{
			out[j++] = ch;
		}
		else if (ch == ' ')           // space
		{
			out[j++] = '+';
		}
		else if (ch == '-' || ch == '_'        // 蟡亙???頞?
			|| ch == '.' || ch == '!'
			|| ch == '~' || ch == '*'
			|| ch == '\'' || ch == '('
			|| ch == ')' || ch == '/')
		{
			out[j++] = ch;
		}
		else if (ch <= 0x007f)     // ASCII隢瑞孜頞潛
		{    
			nuStrcat(out , myhex[ch]);
			j += 3;
		}
		else if (ch <= 0x07FF)         // 皞SCII <= 0x7FF
		{
			nuStrcat(out , myhex[0xc0 | (ch >> 6)]);
			nuStrcat(out , myhex[0x80 | (ch & 0x3F)]);
			j += 6;
		}
		else                       // 0x7FF < ch <= 0xFFFF
		{
			nuStrcat(out , myhex[0xe0 | (ch >> 12)]);
			nuStrcat(out , myhex[0x80 | ((ch >> 6) & 0x3F)]);
			nuStrcat(out , myhex[0x80 | (ch & 0x3F)]);
			j += 9;
		}
	}
	out[j] = '\0';
	//USES_CONVERSION;
	//CString result = A2W(out);

	//delete out;
	//out = NULL;

	return out;
}

nuWCHAR * JavaToWSZ(JNIEnv* env, jstring string)
{
    if (string == NULL)
        return NULL;
    int len = env->GetStringLength(string);
    const jchar* raw = env->GetStringChars(string, NULL);
    if (raw == NULL)
        return NULL;

    nuWCHAR* wsz = new nuWCHAR[len+1];
    nuMemcpy(wsz, raw, len*2);
    wsz[len] = 0;

    env->ReleaseStringChars(string, raw);

    return wsz;
}
static JNIEnv* GetEnv()
{
	JNIEnv *env;
	if( g_jvm->GetEnv((void**) &env, JNI_VERSION_1_6 ) != JNI_OK )
 	{
		jint status = g_jvm->AttachCurrentThread(&env, NULL);
		if(status < 0) 
		{
			env = NULL;
		}
	}
	return env;
}
static jclass  targetGetNameClass = NULL ;
static jobject getNameobject = NULL ;

static jclass targetGetCandidateWordClass = NULL;
static jobject  getCandidateWordobject = NULL;

static jclass targetGetKeyBoardClass = NULL;
static jobject  getGetKeyBoardobject = NULL;

static jclass targetGetListClass = NULL;
static jobject  getListobject = NULL;
nuVOID CAndroidMain::MouseEvent(NURO_POINT2 Pt)
{
	return	g_Loader.MouseEvent(Pt);

}
nuLONG CAndroidMain::OnCopyData(COPYDATASTRUCT* pCopyDataStruct)
{
	nuPVOID pReTurn = NULL;
	if(NURO_DEF_UI_SET_PACK_PIC == pCopyDataStruct->dwData)
	{
		pReTurn = (nuPVOID)gRgb565PicScreen;
	}
	return g_Loader.UISetEngine(pCopyDataStruct->dwData, pCopyDataStruct->lpData, pReTurn);
}
nuPVOID CAndroidMain::UIGetEngineData(nuLONG DataDef, nuPVOID pReturnData, nuPVOID pReturnData2)
{
	return g_Loader.UIGetEngineData(DataDef, pReturnData, pReturnData2);
}
nuBOOL CAndroidMain::SEARCHSDKCallBack(PCALL_BACK_PARAM pParam)
{

    switch (pParam->nCallType)
    {
	
	case _NR_MSG_SDK_SEARCH_GETNAMEALL:
	{		
			switch(pParam->lCallID)
			{
				case SET_SEARCH_STATE_ID:
				case SET_SEARCH_CITY_ID:
				case SET_SEARCH_TOWN_ID:
				{
					nuINT nLen = pParam->lDataLen; 
					nuINT nCount = pParam->lDataLen/EACH_CITY_NAME_LEN; 
					nuBYTE        *m_byBuffer =  (nuBYTE *)pParam->pVOID;
					nuWCHAR wsName[SDK_PROTWONMAXCOUNT][EACH_CITY_NAME_LEN/2]= {0}; 
					if(nLen>0)
					nuMemcpy(wsName,m_byBuffer,nLen);

					JNIEnv*env= GetEnv();
					jmethodID SetDataCountID = env->GetMethodID(targetGetNameClass, "SetDataCount", "(I)V");
					env->CallVoidMethod(getNameobject,SetDataCountID,nCount);
	    				for(nuINT k=0;k<nCount;k++)
					{
						jstring StrMMF=NULL;
						StrMMF = env->NewString((const jchar*)wsName[k], (jsize)nuWcslen((const nuWCHAR*)wsName[k]));						jmethodID setNameID = env->GetMethodID(targetGetNameClass, "setName", "(ILjava/lang/String;)V");					
						env->CallVoidMethod(getNameobject,setNameID,k,StrMMF);
						env->DeleteLocalRef(StrMMF);
					
					}
						__android_log_print(ANDROID_LOG_INFO, "sdkinfo", "Mode %d ok la~~~~~~ " , pParam->lCallID);
						//jobject newObject   = env->NewObject(m_c_Jni_Voice, m_c_jni_mid_getNameALL,StrMMF);
				}				
				break;
				case SET_SEARCH_TYPE1_ID:
				case SET_SEARCH_TYPE2_ID:
				{
					nuINT nLen = pParam->lDataLen; 
					nuINT nCount = pParam->lDataLen/EACH_TYPE_NAME_LEN; 
					nuBYTE        *m_byBuffer =  (nuBYTE *)pParam->pVOID;
					nuWCHAR wsName[SDK_CALTYPETOTALCOUT][EACH_TYPE_NAME_LEN/2] = {0};
					if(nLen>0)
					nuMemcpy(wsName,m_byBuffer,nLen);

					JNIEnv*env= GetEnv();
					jmethodID SetDataCountID = env->GetMethodID(targetGetNameClass, "SetDataCount", "(I)V");
					env->CallVoidMethod(getNameobject,SetDataCountID,nCount);
	    				for(nuINT k=0;k<nCount;k++)
					{
						jstring StrMMF=NULL;
						StrMMF = env->NewString((const jchar*)wsName[k], (jsize)nuWcslen((const nuWCHAR*)wsName[k]));						jmethodID setNameID = env->GetMethodID(targetGetNameClass, "setName", "(ILjava/lang/String;)V");					
						env->CallVoidMethod(getNameobject,setNameID,k,StrMMF);
						env->DeleteLocalRef(StrMMF);
					
					}
				}
				break;
				case SET_SEARCH_CANDIDATE_ID:
				{
					
				}		
				break;
				case SET_SEARCH_NEXTWORD_ID:
				{
					
					nuINT nCount = pParam->lDataLen; 
					nuBYTE        *m_byBuffer =  (nuBYTE *)pParam->pVOID;

					nuWCHAR wsName[NEXT_WORD_MAXCOUNT] = {0};
					if(nCount>0)
					nuMemcpy(wsName,m_byBuffer,sizeof(wsName));

					JNIEnv*env= GetEnv();
					jmethodID SetDataCountID = env->GetMethodID(targetGetNameClass, "SetDataCount", "(I)V");
					env->CallVoidMethod(getNameobject,SetDataCountID,nCount);
	    				for(nuINT k=0;k<nCount;k++)
					{
						nuWCHAR wTemp[2]={0};
						wTemp[0]=wsName[k];
						jstring StrMMF=NULL;
						StrMMF = env->NewString((const jchar*)wTemp, (jsize)nuWcslen((const nuWCHAR*)wTemp));						jmethodID setNameID = env->GetMethodID(targetGetNameClass, "setName", "(ILjava/lang/String;)V");					
						env->CallVoidMethod(getNameobject,setNameID,k,StrMMF);
						env->DeleteLocalRef(StrMMF);
				
					}
				}		
				break;
			}
	}
	break;
	case _NR_MSG_SDK_SEARCH_GETKEYBOARD:
	{		
			if(pParam->lCallID == SET_SEARCH_NEXTWORD_ID)
			{
					nuINT nCount = pParam->lDataLen; 
					nuBYTE        *m_byBuffer =  (nuBYTE *)pParam->pVOID;

					nuWCHAR wsName[NEXT_WORD_MAXCOUNT] = {0};
					if(nCount>0)
					nuMemcpy(wsName,m_byBuffer,sizeof(wsName));

					JNIEnv*env= GetEnv();
					jstring StrMMF=NULL;
					StrMMF = env->NewString((const jchar*)wsName, (jsize)nuWcslen((const nuWCHAR*)wsName));						

					jmethodID setNameID = env->GetMethodID(targetGetKeyBoardClass, "setString", "(ILjava/lang/String;)V");					
					env->CallVoidMethod(getGetKeyBoardobject,setNameID,nuWcslen(wsName),StrMMF);
					env->DeleteLocalRef(StrMMF);	
			}
			else
			{
					nuCHAR wsName[64]= {0}; 
					nuWCHAR wsName2[64]= {0}; 
					nuMemcpy(wsName,pParam->pVOID,sizeof(wsName));
					
					for(int k=0;k<64;k++)
						wsName2[k] = wsName[k];

					JNIEnv*env= GetEnv();
					jstring StrMMF=NULL;
					StrMMF = env->NewString((const jchar*)wsName2, (jsize)nuWcslen((const nuWCHAR*)wsName2));			jmethodID setNameID = env->GetMethodID(targetGetKeyBoardClass, "setString", "(ILjava/lang/String;)V");					
					env->CallVoidMethod(getGetKeyBoardobject,setNameID,nuWcslen(wsName2),StrMMF);
					env->DeleteLocalRef(StrMMF);
					
			}
	}
	break;
	case _NR_MSG_SDK_SEARCH_GETCANDIDATEWORD:
	{
		__android_log_print(ANDROID_LOG_INFO, "CAN", "@@@@@@ ....... _NR_MSG_SDK_SEARCH_GETCANDIDATEWORD");
		nuINT nLen = pParam->lDataLen; 
		nuINT nCount = pParam->lDataLen/sizeof(DYWORD);
		DYWORD *m_pRoadKeyWord = (DYWORD *)pParam->pVOID;

		JNIEnv*env= GetEnv();
		jmethodID SetDataCountID = env->GetMethodID(targetGetCandidateWordClass, "SetDataCount", "(I)V");
		env->CallVoidMethod(getCandidateWordobject,SetDataCountID,nCount);
		
		for(nuINT k=0;k<nCount;k++)
		{

		jstring StrMMF1=NULL;
		StrMMF1 = env->NewString((const jchar*)m_pRoadKeyWord[k].name, 
				(jsize)nuWcslen((const nuWCHAR*)m_pRoadKeyWord[k].name));
		
		jmethodID setNameID = env->GetMethodID(targetGetCandidateWordClass, "setData", "(ILjava/lang/String;I)V");					
					env->CallVoidMethod(getCandidateWordobject,setNameID,k,StrMMF1,m_pRoadKeyWord[k].nFindCount);


			env->DeleteLocalRef(StrMMF1);
		
			
		}
	}
	break;
	case _NR_MSG_SDK_SEARCH_GETLISTALL:
	{		
		//switch(pParam->lCallID)
		
		nuINT nLen = pParam->lDataLen; 
		nuINT nCount = pParam->lDataLen/sizeof(SDK_LISTDATA);
		SDK_LISTDATA * data = (SDK_LISTDATA *)pParam->pVOID;

		
		JNIEnv*env= GetEnv();
		jmethodID SetDataCountID = env->GetMethodID(targetGetListClass, "SetDataCount", "(I)V");
		env->CallVoidMethod(getListobject,SetDataCountID,nCount);
			
		for(nuINT k=0;k<nCount;k++)
		{

		jstring StrMMF1=NULL,StrMMF2=NULL,StrMMF3=NULL,StrMMF4=NULL;;
		StrMMF1 = env->NewString((const jchar*)data[k].wsName, 
				(jsize)nuWcslen((const nuWCHAR*)data[k].wsName));
		StrMMF2 = env->NewString((const jchar*)data[k].wsCityName, 
				(jsize)nuWcslen((const nuWCHAR*)data[k].wsCityName));
		StrMMF3 = env->NewString((const jchar*)data[k].wsTownName, 
			(jsize)nuWcslen((const nuWCHAR*)data[k].wsTownName));
		StrMMF4 = env->NewString((const jchar*)data[k].wsOther, 
			(jsize)nuWcslen((const nuWCHAR*)data[k].wsOther));
			
		jmethodID setNameID = env->GetMethodID(targetGetListClass, "setName", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJIJ)V");					
					env->CallVoidMethod(getListobject,setNameID,k,StrMMF1,StrMMF2,StrMMF3,StrMMF4,(jlong)data[k].pos.x,(jlong)data[k].pos.y,(jint)data[k].nType,(jlong)data[k].nDis);

			//jmethodID setNameID2 = env->GetMethodID(targetGetListClass, "setDis", "(IJ)V");					
			//env->CallVoidMethod(getListobject,setNameID2,(jint)k,(jlong)data[k].nDis);


			env->DeleteLocalRef(StrMMF1);
			env->DeleteLocalRef(StrMMF2);
			env->DeleteLocalRef(StrMMF3);
			env->DeleteLocalRef(StrMMF4);
			
					
		}
	}
	break;
       /* case CALLBACK_TYPE_TMCUSED:
            g_pTMCUsed = (nuBYTE*)pParam->pVOID;
            break;
        case CALLBACK_TYPE_MAP_NAME:
            {
                nuCHAR *pName = (nuCHAR *)pParam->pVOID;

                if (0 != nuStrcmp(s_sMapName, pName)) 
                {
                    nuStrcpy(s_sMapName, pName);
                }

                break;
            }*/
        default:
            break;
    }
/*	JNIEnv*env= GetEnv();
	jclass cls2 = env->FindClass("com/nuror1/NaviService");
        jmethodID searchmethodid = env->GetMethodID(cls2, "SearchFinsh", "(I)V");
	
	jobject newObject   = env->NewObject(cls2, searchmethodid,
					    (int)pParam->nCallType);
	 
	  env->DeleteLocalRef(newObject); 
*/	
	//extern jclass  g_SearchOKCls;
	//extern jobject g_SearchOKObject;

    return 1;
}

nuBOOL CAndroidMain::GetNameALL(JNIEnv *env, jclass cls,jint nType, jobject object)
{
	//jobject tmpobject;
	//tmpobject = object;
	//getNameobject = env->NewGlobalRef(tmpobject) ;

	getNameobject =  env->NewGlobalRef(object);
	targetGetNameClass = env->GetObjectClass(getNameobject);
	PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETNAMEALL,nType,0); 
}
nuBOOL CAndroidMain::GetCandidateWord(JNIEnv *env, jclass cls,jint nType, jobject object)
{
	//jobject tmpobject;
	//tmpobject = object;
	//getCandidateWordobject = env->NewGlobalRef(object) ;

	getCandidateWordobject = env->NewGlobalRef(object);
	targetGetCandidateWordClass = env->GetObjectClass(getCandidateWordobject);
	PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETCANDIDATEWORD,nType,0); 
}
nuBOOL CAndroidMain::GetKeyBoard(JNIEnv *env, jclass cls,jint nType, jobject object)
{
	//jobject tmpobject;
	//tmpobject = object;
	//getGetKeyBoardobject = env->NewGlobalRef(tmpobject) ;

	getGetKeyBoardobject = env->NewGlobalRef(object);
	targetGetKeyBoardClass = env->GetObjectClass(getGetKeyBoardobject);
	PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETKEYBOARD,nType,0); 
}
nuBOOL CAndroidMain::GetListWord(JNIEnv *env, jclass cls,jint nType, jobject object)
{
	//jobject tmpobject;
	//tmpobject = object;
	//getListobject = env->NewGlobalRef(tmpobject) ;

	getListobject =  env->NewGlobalRef(object);
	targetGetListClass = env->GetObjectClass(getListobject);
	PostMessage(env, cls,_NR_MSG_SDK_SEARCH_GETLISTALL,nType,0);
}
nuBOOL CAndroidMain::PostMessage(JNIEnv * env, jclass cls,nuINT ID,nuINT n1,nuINT n2)
{
   __android_log_print(ANDROID_LOG_INFO, "TEST", "API :CAndroidMain::PostMessage %d %d %d",ID,n1,n2);

    CALL_BACK_PARAM cbParam = {0};
    cbParam.nCallType = ID;
    cbParam.lCallID  = n1;
    cbParam.lDataLen  = n2;
    __android_log_print(ANDROID_LOG_INFO, "navi", "PostMessage %d" , n1);
    //g_Loader.Message(msg);
    COPYDATASTRUCT pCopyDataStruct;
    if(ID==_NR_MSG_SDK_SEARCH_SETSEARCHMODE && n1==SET_SEARCH_END)
    {
      __android_log_print(ANDROID_LOG_INFO, "navi", " SET_SEARCH_END\n");
      pCopyDataStruct.dwData = 0x0100;
    } 
    else
    {
      pCopyDataStruct.dwData = 0x0101;
    }

    if(ID==_NR_MSG_SDK_SEARCH_SETSEARCHID && n1 == SET_SEARCH_POI_DIS_ID)
    {
	
	pCopyDataStruct.dwData = 0x0111;
    }
    pCopyDataStruct.lpData = &cbParam;
    g_Loader.UISetEngine( pCopyDataStruct.dwData, pCopyDataStruct.lpData,NULL); 
    return nuTRUE;
}



nuWCHAR wsNAme1[128] = {0};
nuWCHAR wsNAme2[128] = {0};
nuWCHAR wsNAme3[128] = {0};
nuBOOL CAndroidMain::SetKey(JNIEnv * env, jclass cls,nuINT ID,jstring str)
{
   int len = env->GetStringLength(str);
   nuWCHAR *lpwcResult = JavaToWSZ(env,str);
     __android_log_print(ANDROID_LOG_INFO, "SetKey", "ID %d %d",ID,len);
  // const char * StrName = env->GetStringUTFChars(str,0);
    nuWCHAR wsNAme[128] = {0};
    nuMemcpy(wsNAme,lpwcResult,len*2);
    NURO_UI_MESSAGE msg= {0};
    msg.nMsgID  = ID;
    if(ID == _NR_MSG_SDK_SEARCH_SETKEY)
    {
	 __android_log_print(ANDROID_LOG_INFO, "SetKey", "_NR_MSG_SDK_SEARCH_SETKEY");
	nuMemset(wsNAme1,0,sizeof(wsNAme1));
	nuMemcpy(wsNAme1,wsNAme,sizeof(wsNAme));
    }
    else if(ID == _NR_MSG_SDK_SEARCH_SETCANDIDATEKEY)
    {
	 __android_log_print(ANDROID_LOG_INFO, "SetKey", "_NR_MSG_SDK_SEARCH_SETCANDIDATEKEY");
	nuMemset(wsNAme2,0,sizeof(wsNAme2));
	nuMemcpy(wsNAme2,wsNAme,sizeof(wsNAme));	
    }
    else if(ID == _NR_MSG_SDK_SEARCH_SETFUZZYKEY)
    {
	 __android_log_print(ANDROID_LOG_INFO, "SetKey", "_NR_MSG_SDK_SEARCH_SETFUZZYKEY");
	nuMemset(wsNAme2,0,sizeof(wsNAme2));
	nuMemcpy(wsNAme2,wsNAme,sizeof(wsNAme));	
    }
    else
    {
	 __android_log_print(ANDROID_LOG_INFO, "SetKey", "else");
	nuMemset(wsNAme3,0,sizeof(wsNAme3));
	nuMemcpy(wsNAme3,wsNAme,sizeof(wsNAme));	
    }
 __android_log_print(ANDROID_LOG_INFO, "nearpoi", "ID %d ......", ID);
   //g_Loader.Message(msg);
    PostMessage(env, cls,ID,0,0); 
  
  return nuTRUE;
}

nuBOOL CAndroidMain::setSDKKEY(nuINT nMode,nuWCHAR *wsName)
{
    if(nMode == _NR_MSG_SDK_SEARCH_SETKEY)
    	nuMemcpy(wsName,wsNAme1,sizeof(wsNAme1));
    else if(nMode == _NR_MSG_SDK_SEARCH_SETCANDIDATEKEY)
		nuMemcpy(wsName,wsNAme2,sizeof(wsNAme1));
    else
		nuMemcpy(wsName,wsNAme3,sizeof(wsNAme1));

  __android_log_print(ANDROID_LOG_INFO, "SetKey", "GET nMode %d",nMode);
    return nuTRUE;
}


