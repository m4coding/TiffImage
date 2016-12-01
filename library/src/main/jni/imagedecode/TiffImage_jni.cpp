//
// Created by mochangsheng on 2016/11/15.
//

#include "TiffImage_jni.h"

#define LOG_TAG  ("TiffImage_jni")
#define REGISTER_CLASS  ("com/mcs/library/TiffImage")

struct fields_t {
    jfieldID context;
};

static fields_t fields;

static TiffImage* getTiffImage(JNIEnv* env, jobject thiz)
{
    TiffImage* tiffImage = (TiffImage*) env->GetLongField(thiz, fields.context);
    return tiffImage;
}

static void setTiffImage(JNIEnv* env, jobject thiz, long tiffImage)
{
    TiffImage *old = (TiffImage*) env->GetLongField(thiz, fields.context);
    //释放之前的
    if (old != NULL) {
        delete old;
        old = NULL;
    }
    env->SetLongField(thiz, fields.context, tiffImage);
}

static jobject getImageInfoJNI(JNIEnv* env, jobject thiz, jstring path) {

    if (!path) {
        jniThrowException(env, ILLEGAL_ARGUMENT_EXEPTION, "Null pointer");
        return NULL;
    }

    const char *tmp = env->GetStringUTFChars(path, NULL);
    if (!tmp) {
        LOGE("OutOfMemoryError");
        return NULL;
    }

    ImageInfo imageInfo = TiffImage::getImageInfo(tmp);

    env->ReleaseStringUTFChars(path, tmp);
    tmp = NULL;

    jclass  jclass1 = env->FindClass("com/mcs/library/TiffImage$ImageInfo");

    jmethodID  jmethodID1 = env->GetMethodID(jclass1,"<init>", "(II)V");
    if (!jmethodID1) {
        LOGE("=========to GetMethodID error");
    }
    jobject  jobject1 = env->NewObject(jclass1, jmethodID1, imageInfo.width, imageInfo.height);

    env->DeleteLocalRef(jclass1);

    return jobject1;
}

static jint getWidthJNI(JNIEnv* env, jobject thiz) {

    TiffImage* tiffImage = getTiffImage(env, thiz);
    if (tiffImage == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION, "No tiffImage available");
        return -1;
    }

    return tiffImage->getWidth();
}

static jint getHeightJNI(JNIEnv* env, jobject thiz) {

    TiffImage* tiffImage = getTiffImage(env, thiz);
    if (tiffImage == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION, "No tiffImage available");
        return -1;
    }

    return tiffImage->getHeight();
}

static jlong getSizeJNI(JNIEnv* env, jobject thiz) {

    TiffImage* tiffImage = getTiffImage(env, thiz);
    if (tiffImage == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION, "No tiffImage available");
        return -1;
    }

    return tiffImage->getSize();
}

static jintArray decodeJNI(JNIEnv* env, jobject thiz, jstring path) {

    TiffImage* tiffImage = getTiffImage(env, thiz);
    if (tiffImage == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION, "No tiffImage available");
        return NULL;
    }

    if (!path) {
        jniThrowException(env, ILLEGAL_ARGUMENT_EXEPTION, "Null pointer");
        return NULL;
    }

    const char *tmp = env->GetStringUTFChars(path, NULL);
    if (!tmp) {
        LOGE("OutOfMemoryError");
        return NULL;
    }

    jintArray array = NULL;
    unsigned int* data = tiffImage->decode(tmp);
    if (data) {
        size_t size = tiffImage->getSize();
        array = (jintArray) env->NewIntArray(size/4);//int类型 4个字节
        if (!array) {
            LOGI("decodeJNI: OutOfMemoryError");
        } else {
            jint *bytes = env->GetIntArrayElements(array, NULL);
            if (bytes != NULL) {
                memcpy(bytes, data, size);
                env->ReleaseIntArrayElements(array, bytes, 0);
            }
        }
    }

    env->ReleaseStringUTFChars(path, tmp);
    tmp = NULL;

    return array;
}

static jintArray decodeStreamJNI(JNIEnv* env, jobject thiz, jobject is, jbyteArray ar) {

    TiffImage* tiffImage = getTiffImage(env, thiz);
    if (tiffImage == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION, "No tiffImage available");
        return NULL;
    }

    if (!is) {
        jniThrowException(env, ILLEGAL_ARGUMENT_EXEPTION, "Null pointer");
        return NULL;
    }

    jintArray array = NULL;
    unsigned int* data = tiffImage->decode(env,is, ar);

    if (data) {
        size_t size = tiffImage->getSize();
        array = (jintArray) env->NewIntArray(size/4);//int类型 4个字节
        if (!array) {
            LOGI("decodeJNI: OutOfMemoryError");
        } else {
            jint *bytes = env->GetIntArrayElements(array, NULL);
            if (bytes != NULL) {
                memcpy(bytes, data, size);
                env->ReleaseIntArrayElements(array, bytes, 0);
            }
        }
    }

    return array;
}

static void nativeReleaseJNI(JNIEnv* env, jobject thiz) {
    setTiffImage(env, thiz, 0);
}

static void nativeSetupJNI(JNIEnv* env, jobject thiz) {

    jclass  clazz = env->GetObjectClass(thiz);
    fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (fields.context == NULL) {
        return;
    }

    env->DeleteLocalRef(clazz);

    TiffImage* tiffImage = new TiffImage();
    if (tiffImage == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

    setTiffImage(env, thiz, (long)tiffImage);
}

static JNINativeMethod nativeMethods[] = {
        {"_nativeSetup", "()V", (void *)nativeSetupJNI},
        {"_nativeRelease", "()V", (void *)nativeReleaseJNI},
        {"_decode", "(Ljava/lang/String;)[I", (void *)decodeJNI},
        {"_decode", "(Ljava/io/InputStream;[B)[I", (void *)decodeStreamJNI},
        {"_getWidth", "()I", (void *)getWidthJNI},
        {"_getHeight", "()I", (void *)getHeightJNI},
        {"_getSize", "()J", (void *)getSizeJNI},
        {"_getImageInfo", "(Ljava/lang/String;)Lcom/mcs/library/TiffImage$ImageInfo;", (void *)getImageInfoJNI},
};

/**
 * 注册native方法
 */
static int registerNativeMethods(JNIEnv *env) {

    int result = -1;

    /* look up the class */
    jclass clazz = env->FindClass(REGISTER_CLASS);

    if (NULL != clazz) {

        if (env->RegisterNatives(clazz, nativeMethods, sizeof(nativeMethods)
                                                       / sizeof(nativeMethods[0])) == JNI_OK) {
            result = 0;
        }
    }
    return result;
}

/**
 * load库时被java虚拟机自动调用
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        goto bail;
    }

    if (registerNativeMethods(env) < 0) {
        LOGE("ERROR: HelloWorld native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}

/**
 * 卸载库时被java虚拟机自动调用
 */
void JNI_OnUnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env;
    jclass jClass = env->FindClass(REGISTER_CLASS);
    env->UnregisterNatives(jClass);
    env->DeleteLocalRef(jClass);
}