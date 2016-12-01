#ifndef JNIHELPERS_H
#define JNIHELPERS_H

#include <jni.h>

#define METHOD_COUNT(methodArray) (sizeof(methodArray) / sizeof(methodArray[0]))

#define ILLEGAL_STATE_EXEPTION ("java/lang/IllegalStateException")
#define ILLEGAL_ARGUMENT_EXEPTION ("java/lang/IllegalArgumentException")

void jniThrowException(JNIEnv* env, const char* className, const char* msg);


#endif //JNIHELPERS_H
