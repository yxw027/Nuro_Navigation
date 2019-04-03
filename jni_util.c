/////////////// For Jni //////////////////
// Created by Dengxu @ 2012 03 19
// ///////////////////////////////////////

#include "jni_util.h"

#include <stdlib.h>
#include <string.h>

char *jstringTostring(JNIEnv *env, jstring jstr)
{
#ifndef __cplusplus
#define env (*env)
#define FindClass(a) FindClass(&env, a)
#define NewStringUTF(a) NewStringUTF(&env, a)
#define GetMethodID(a, b, c) GetMethodID(&env, a, b, c)
#define CallObjectMethod(a, b, c) CallObjectMethod(&env, a, b, c)
#define GetArrayLength(a) GetArrayLength(&env, a)
#define GetByteArrayElements(a, b) GetByteArrayElements(&env, a, b)
#define ReleaseByteArrayElements(a, b, c) ReleaseByteArrayElements(&env, a, b, c)
#define NewByteArray(a) NewByteArray(&env, a)
#define SetByteArrayRegion(a, b, c, d) SetByteArrayRegion(&env, a, b, c, d)
#define NewObject(a, b, c, d) NewObject(&env, a, b, c, d)
#endif 
    
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    
    if (alen > 0)
    {
        rtn = (char *)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    
    //Release memory allocated.
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;

#ifndef __cplusplus
#undef env 
#undef FindClass
#undef NewStringUTF
#undef GetMethodID
#undef CallObjectMethod
#undef GetArrayLength
#undef GetByteArrayElements
#undef ReleaseByteArrayElements
#undef NewByteArray
#undef SetByteArrayRegion
#undef NewObject
#endif
}

jstring stoJstring(JNIEnv *env, const char *pat)
{
#ifndef __cplusplus
#define env (*env)
#define FindClass(a) FindClass(&env, a)
#define NewStringUTF(a) NewStringUTF(&env, a)
#define GetMethodID(a, b, c) GetMethodID(&env, a, b, c)
#define CallObjectMethod(a, b, c) CallObjectMethod(&env, a, b, c)
#define GetArrayLength(a) GetArrayLength(&env, a)
#define GetByteArrayElements(a, b) GetByteArrayElements(&env, a, b)
#define ReleaseByteArrayElements(a, b, c) ReleaseByteArrayElements(&env, a, b, c)
#define NewByteArray(a) NewByteArray(&env, a)
#define SetByteArrayRegion(a, b, c, d) SetByteArrayRegion(&env, a, b, c, d)
#define NewObject(a, b, c, d) NewObject(&env, a, b, c, d)
#endif
    
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");
    
    //Memory to JVM Proxy
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);

#ifndef __cplusplus
#undef env 
#undef FindClass
#undef NewStringUTF
#undef GetMethodID
#undef CallObjectMethod
#undef GetArrayLength
#undef GetByteArrayElements
#undef ReleaseByteArrayElements
#undef NewByteArray
#undef SetByteArrayRegion
#undef NewObject
#endif
}


