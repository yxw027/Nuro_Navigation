/* filetype=cpp */

struct typemap_struct
{
    const nuCHAR *javaType;
    const nuCHAR *sig;
};

/* get value type's signature. */
static inline const nuCHAR *getSignatureVal(const nuCHAR *javaType, nuINT nArrayd = 0)
{
    static const nuCHAR *array = "[";
    static nuCHAR retSign[256];
    static struct typemap_struct typemap[] = 
    {   
        {"void",    "V"},
        {"byte",    "B"},
        {"boolean", "Z"},
        {"short",   "S"},
        {"char",    "C"},
        {"int",     "I"},
        {"long",    "J"},
        {"float",  "F"},
        {"double",  "D"},
        {"String",  "Ljava/lang/String;"},
        {"Object",  "Ljava/lang/Object;"}
    };  

    nuBOOL isfind = false;

    // array sign
    nuINT i;

    retSign[0] = '\0';

    for (i = 0; i < nArrayd; i++)
    {   
        nuStrcat(retSign, array);
    }   

    // type sign
    for (i = 0; i < (sizeof (typemap) / sizeof (typemap[0])); i++)
    {   
        if (0 == nuStrcmp(typemap[i].javaType, javaType))
        {   
            nuStrcat(retSign, typemap[i].sig);
            isfind = true;
            break;
        }
    }

    if (!isfind)
    {
        return NULL;
    }

    return retSign;
}


/* [jstring] to [nuCHAR *] */
static inline nuCHAR *jstringTostring(JNIEnv *env, jstring jstr)
{
    nuCHAR *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);

    if (alen > 0) 
    {    
        rtn = (nuCHAR *)nuMalloc(alen + 1);  
        nuMemcpy(rtn, ba, alen);
        rtn[alen] = 0; 
    }    

    /* Release memory allocated. */
    env->ReleaseByteArrayElements(barr, ba, 0);  
    env->DeleteLocalRef(barr);
    return rtn; 
}

/* [jstring] to [nuCHAR *] buf */
static inline nuCHAR *jstringTostring2(nuCHAR buffer[], nuINT size, JNIEnv *env, jstring jstr)
{
    nuCHAR *rtn = buffer;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);

    if (alen >= size)
    {
        alen = size - 1;
    }

    if (alen > 0) 
    {    
        nuMemcpy(rtn, ba, alen);
        rtn[alen] = 0; 
    }    

    /* Release memory allocated. */
    env->ReleaseByteArrayElements(barr, ba, 0);  
    env->DeleteLocalRef(barr);
    return rtn; 
}

/* New [jstring] to [nuCHAR *] buf */
static inline nuCHAR *jstringTostring3(nuCHAR buffer[], nuINT size, nuINT *pRetSize, JNIEnv *env, jstring jstr)
{
    nuCHAR *rtn = buffer;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);

    if (alen >= size)
    {
        alen = size - 1;
    }

    if (alen > 0) 
    {
        nuMemcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }

    if (pRetSize)
    {
        *pRetSize = alen;
    }

    /* Release memory allocated. */
    env->ReleaseByteArrayElements(barr, ba, 0);  
    env->DeleteLocalRef(barr);
    return rtn; 
}

/* [nuCHAR *] to [jstring] */
static inline jstring stoJstring(JNIEnv *env, const nuCHAR *pat)
{
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(nuStrlen(pat));
    env->SetByteArrayRegion(bytes, 0, nuStrlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");

    /* Memory to JVM Proxy */
    jstring strRet = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    env->DeleteLocalRef(strClass); 
    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(encoding);
    
    return strRet;
}

/* get class name */
static inline jstring getObjectClassName(JNIEnv *env, jobject obj) 
{
    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
    jobject clsobj = env->CallObjectMethod(obj, mid);
    jclass clscls = env->GetObjectClass(clsobj);
    jmethodID clsmid = env->GetMethodID(clscls, "getName", "()Ljava/lang/String;");
    jstring jstr = (jstring)env->CallObjectMethod(clsobj, clsmid);
    
    env->DeleteLocalRef(clscls);
    env->DeleteLocalRef(clsobj);
    env->DeleteLocalRef(cls);
    
    return jstr;
}

