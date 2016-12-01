

#include "JNIHelpers.h"
#include "Log.h"

#define LOG_TAG ("JNIHelpers")

void jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    jclass clazz = env->FindClass(className);
    if (!clazz) {
        LOGE("Unable to find exception class %s", className);
        /* ClassNotFoundException now pending */
        return;
    }

    if (env->ThrowNew(clazz, msg) != JNI_OK) {
        LOGE("Failed throwing '%s' '%s'", className, msg);
        /* an exception, most likely OOM, will now be pending */
    }
    env->DeleteLocalRef(clazz);
}
