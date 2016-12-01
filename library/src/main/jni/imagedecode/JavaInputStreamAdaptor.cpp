//
// Created by mochangsheng on 2016/11/22.
//

#include "JavaInputStreamAdaptor.h"

static jmethodID    gInputStream_readMethodID;
static jmethodID    gInputStream_skipMethodID;
static jmethodID    gInputStream_resetMethodID;
static jmethodID    gInputStream_availableMethodID;
static jmethodID    gInputStream_markMethodID;

static jclass findClassCheck(JNIEnv* env, const char classname[]) {
    jclass clazz = env->FindClass(classname);
    return clazz;
}

static jmethodID getMethodIDCheck(JNIEnv* env, jclass clazz,
                                  const char methodname[], const char type[]) {
    jmethodID id = env->GetMethodID(clazz, methodname, type);
    return id;
}

static int register_CreateJavaInputStreamAdaptor(JNIEnv* env) {
    jclass inputStream_Clazz = findClassCheck(env, "java/io/InputStream");
    gInputStream_readMethodID = getMethodIDCheck(env, inputStream_Clazz, "read", "([BII)I");
    gInputStream_skipMethodID = getMethodIDCheck(env, inputStream_Clazz, "skip", "(J)J");
    gInputStream_availableMethodID = getMethodIDCheck(env, inputStream_Clazz, "available", "()I");
    gInputStream_resetMethodID = getMethodIDCheck(env, inputStream_Clazz, "reset", "()V");
    gInputStream_markMethodID = getMethodIDCheck(env, inputStream_Clazz, "mark", "(I)V");

    env->DeleteLocalRef(inputStream_Clazz);

    return 0;
}

JavaInputStreamAdaptor::JavaInputStreamAdaptor(JNIEnv *env, jobject js, jbyteArray ar) {
    fEnv = env;
    fJavaInputStream = js;
    fJavaByteArray = ar;
    fCapacity = env->GetArrayLength(ar);
    fBytesRead = 0;
    fIsAtEnd = false;
    register_CreateJavaInputStreamAdaptor(env);
}

JavaInputStreamAdaptor::~JavaInputStreamAdaptor() {

}

size_t JavaInputStreamAdaptor::read(void *buffer, size_t size) {
   /* if (NULL == buffer) {
        if (0 == size) {
            return 0;
        } else {
              InputStream.skip(n) can return <=0 but still not be at EOF
                If we see that value, we need to call read(), which will
                block if waiting for more data, or return -1 at EOF

            size_t amountSkipped = 0;
            do {
                size_t amount = this->doSkip(size - amountSkipped);
                if (0 == amount) {
                    char tmp;
                    amount = this->doRead(&tmp, 1);
                    if (0 == amount) {
                        // if read returned 0, we're at EOF
                        fIsAtEnd = true;
                        break;
                    }
                }
                amountSkipped += amount;
            } while (amountSkipped < size);
            return amountSkipped;
        }

        return 0;
    }*/
    return this->doRead(buffer, size);
}

size_t JavaInputStreamAdaptor::available() {
    JNIEnv* env = fEnv;

    jlong availableSize = env->CallIntMethod(fJavaInputStream,
                                        gInputStream_availableMethodID);

    if (DEBUG) LOGI("=====JavaInputStreamAdaptor::available size==%ld",(size_t)availableSize);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGE("------- available threw an exception\n");
        return 0;
    }

    if (availableSize < 0) {
        availableSize = 0;
    }

    return (size_t)availableSize;
}

bool JavaInputStreamAdaptor::isAtEnd() {
    return fIsAtEnd;
}

size_t JavaInputStreamAdaptor::doRead(void *buffer, size_t size) {
    JNIEnv* env = fEnv;
    size_t bytesRead = 0;
    if (DEBUG) LOGI("=====doRead Buffer==%d",*((char*)buffer));
    void* tmpBuffer = buffer;
    // read the bytes
    do {
        jint requested = 0;
        if (size > static_cast<size_t>(fCapacity)) {
            requested = fCapacity;
        } else {
            // This is safe because requested is clamped to (jint)
            // fCapacity.
            requested = static_cast<jint>(size);
        }

        jint n = env->CallIntMethod(fJavaInputStream,
                                    gInputStream_readMethodID, fJavaByteArray, 0, requested);

        if (DEBUG) LOGI("=====doRead requested==%d, n==%d", requested,n);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("---- read threw an exception\n");
            // Consider the stream to be at the end, since there was an error.
            fIsAtEnd = true;
            return 0;
        }

        if (n < 0) { // n == 0 should not be possible, see InputStream read() specifications.
            fIsAtEnd = true;
            break;  // eof
        }

        if (DEBUG) LOGI("=====fJavaByteArray size==%d", env->GetArrayLength(fJavaByteArray));

        env->GetByteArrayRegion(fJavaByteArray, 0, n,
                                reinterpret_cast<jbyte*>(buffer));

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("---- read:GetByteArrayRegion threw an exception\n");
            // The error was not with the stream itself, but consider it to be at the
            // end, since we do not have a way to recover.
            fIsAtEnd = true;
            return 0;
        }

        buffer = (void*)((char*)buffer + n);
        bytesRead += n;
        size -= n;
        fBytesRead += n;
    } while (size != 0);

    if (DEBUG) LOGI("=====doRead size==%d, bytesRead==%d, buffer==%d", size, bytesRead, *((char*)tmpBuffer));

    available();

    return bytesRead;
}

size_t JavaInputStreamAdaptor::doSkip(size_t size) {
    JNIEnv* env = fEnv;
    jlong skipped = env->CallLongMethod(fJavaInputStream,
                                        gInputStream_skipMethodID, (jlong)size);

    available();

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGE("------- skip threw an exception\n");
        return 0;
    }
    if (skipped < 0) {
        skipped = 0;
    }

    if (DEBUG) LOGI("======doSkip size==%u",size);

    return (size_t)skipped;
}

void JavaInputStreamAdaptor::reset() {
    JNIEnv* env = fEnv;
    env->CallVoidMethod(fJavaInputStream,gInputStream_resetMethodID);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGE("------- reset threw an exception\n");
    }
}

void JavaInputStreamAdaptor::mark(size_t readLimit) {
    JNIEnv* env = fEnv;

    if (DEBUG) LOGI("======JavaInputStreamAdaptor::mark readLimit==%d",readLimit);

    env->CallVoidMethod(fJavaInputStream,gInputStream_markMethodID, readLimit);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGE("------- mark threw an exception\n");
    }
}

















