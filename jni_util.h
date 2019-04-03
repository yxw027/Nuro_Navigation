/////////////// For Jni //////////////////
// Created by Dengxu @ 2012 03 19
// ///////////////////////////////////////
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
// 将jstring转换为char*
char *jstringTostring(JNIEnv *env, jstring jstr);

//将char*转换为jstring
jstring stoJstring(JNIEnv *env, const char *pat);


#ifdef __cplusplus
}
#endif
