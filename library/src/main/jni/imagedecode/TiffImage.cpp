//
// Created by mochangsheng on 2016/11/14.
//

#include "TiffImage.h"


#define LOG_TAG  ("TiffImage_Native")

static std::istream* iss;
static std::filebuf* fbIn;

//重定向error输出到logcat
static void logError(const char* module, const char* fmt, va_list ap)
{
    char errorBuffer[1024];
    vsnprintf(errorBuffer, 1024, fmt, ap);
    LOGE("========logError module==%s, error%s",module, errorBuffer);
}

TiffImage::TiffImage() {
    buffer = NULL;
    bufferSize = 0;
    width = 0;
    height = 0;
    javaInputStreamAdaptor = NULL;
    tiffInputStream = NULL;
}

TiffImage::~TiffImage() {
    if (buffer != NULL) {
        _TIFFfree(buffer);
        buffer = NULL;
    }

    if (javaInputStreamAdaptor != NULL) {
        delete javaInputStreamAdaptor;
        javaInputStreamAdaptor = NULL;
    }

    if (tiffInputStream != NULL) {
        delete  tiffInputStream;
        tiffInputStream = NULL;
    }

    /*if (tiffStream != NULL) {
        delete tiffStream;
        tiffStream = NULL;
    }*/
}

void TiffImage::decode(TIFF *tif) {

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    bufferSize = width * height * sizeof (unsigned int);
    buffer = (unsigned int*) _TIFFmalloc(bufferSize);
    if (buffer != NULL) {
        if (TIFFReadRGBAImageOriented(tif, width, height, (uint32*) buffer, ORIENTATION_TOPLEFT, 0)) {
            LOGI("width==%d, height==%d", width, height);
            // Convert ABGR to ARGB
            int i = 0;
            int j = 0;
            int tmp = 0;
            for (i = 0; i < height; i++) {
                for ( j = 0; j < width; j++ ) {
                    tmp = buffer[ j + width * i ];
                    buffer[ j + width * i ] = ( tmp & 0xff000000 ) | ( ( tmp & 0x00ff0000 ) >> 16 ) | ( tmp & 0x0000ff00 ) | ( ( tmp & 0xff ) << 16 );
                }
            }
        } else {
            LOGE("TIFFReadRGBAImageOriented error");
            _TIFFfree(buffer);
            buffer = NULL;
        }
    }
}

unsigned int* TiffImage::decode(const char *path) {

    //自定义Tiff error重定向，输出error信息
    TIFFSetErrorHandler(logError);

    TIFF* tif = TIFFOpen(path, "r");
    if (tif) {
        decode(tif);
        TIFFClose(tif);
        return buffer;
    } else {
        LOGE("=======TiffImage::decode TIFFOpen error path==%s", path);
    }

    return NULL;
}

unsigned int *TiffImage::decode(JNIEnv *env, jobject is, jbyteArray ar) {

    //自定义Tiff error重定向，输出error信息
    TIFFSetErrorHandler(logError);

    javaInputStreamAdaptor = new JavaInputStreamAdaptor(env,is, ar);
    tiffInputStream = new TiffInputStream();
    tiff* tif = tiffInputStream->makeFileStream(javaInputStreamAdaptor);

    /* fbIn = new std::filebuf();
     if (!fbIn->open("/mnt/usb/sda1/tiff/Free-Converter.com-20160929225037-7629883098.tif", std::ios::in)) {
         LOGI("======TiffImage::makeInputStream fail to open file");
         return NULL;
     }

     LOGI("======TiffImage::makeInputStream open file success");

     iss = new std::istream(fbIn);

     tiffStream = new TiffStream();
     tiff* tiff = tiffStream->makeFileStream(iss);*/

    if (tif) {
        decode(tif);
        return buffer;
    } else {
        LOGE("=======TiffImage::decode(JNIEnv *env, jobject is, jbyteArray ar) error tiff==NULL");
    }

    return NULL;
}

int TiffImage::getHeight() {
    return height;
}

int TiffImage::getWidth() {
    return width;
}

unsigned long TiffImage::getSize() {
    return bufferSize;
}

ImageInfo TiffImage::getImageInfo(const char *path) {

    ImageInfo imageInfo;
    imageInfo.width = -1;
    imageInfo.height = -1;

    //自定义Tiff error重定向，输出error信息
    TIFFSetErrorHandler(logError);

    TIFF* tif = TIFFOpen(path, "r");
    if (tif) {
        int w = 0, h = 0;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        imageInfo.width = w;
        imageInfo.height = h;
        TIFFClose(tif);
    } else {
        LOGE("=======TiffImage::getImageInfo TIFFOpen error path==%s", path);
    }

    return imageInfo;
}








