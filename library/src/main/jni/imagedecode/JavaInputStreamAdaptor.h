//
// Created by mochangsheng on 2016/11/22.
//

#ifndef TIFFIMAGE_JAVAINPUTSTREAMADAPTOR_H
#define TIFFIMAGE_JAVAINPUTSTREAMADAPTOR_H
#include "jni.h"
#include "Log.h"
#include <stdio.h>

/**
 *  Wrapper for a Java InputStream.
 */
class JavaInputStreamAdaptor {
public:
    JavaInputStreamAdaptor(JNIEnv* env, jobject js, jbyteArray ar);
    ~JavaInputStreamAdaptor();

    virtual size_t read(void* buffer, size_t size);
    virtual bool isAtEnd();
    virtual size_t available();
    size_t doSkip(size_t size);
    void reset();
    void mark(size_t readLimit);

private:
    size_t doRead(void* buffer, size_t size);

    JNIEnv*     fEnv;
    jobject     fJavaInputStream;   // the caller owns this object
    jbyteArray  fJavaByteArray;     // the caller owns this object
    jint        fCapacity;
    size_t      fBytesRead;
    bool        fIsAtEnd;
};
#endif //TIFFIMAGE_JAVAINPUTSTREAMADAPTOR_H
