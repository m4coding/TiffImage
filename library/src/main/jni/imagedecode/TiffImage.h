//
// Created by mochangsheng on 2016/11/14.
//

#ifndef TIFFIMAGE_TIFFIMAGE_H
#define TIFFIMAGE_TIFFIMAGE_H
#include "tiffio.h"
#include "Log.h"
#include "JavaInputStreamAdaptor.h"
#include "TiffInputStream.h"
#include "../tiff/contrib/stream/tiffstream.h"
#include <fstream>

typedef struct ImageInfo {
    int width;
    int height;
} ImageInfo;


class TiffImage {

public:
    TiffImage();
    ~TiffImage();
    int getWidth();
    int getHeight();
    unsigned long getSize();
    void decode(TIFF* tif);
    unsigned int* decode(const char *path);
    unsigned int* decode(JNIEnv* env, jobject is, jbyteArray ar);
    static ImageInfo getImageInfo(const char *path);
    //tiff* makeInputStream(JNIEnv* env, jobject is, jbyteArray ar);

private:
    unsigned int* buffer;
    unsigned long bufferSize;
    int width;
    int height;
    JavaInputStreamAdaptor* javaInputStreamAdaptor;
    TiffInputStream* tiffInputStream;
    TiffStream* tiffStream;

};


#endif //TIFFIMAGE_TIFFIMAGE_H
