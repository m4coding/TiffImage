//
// Created by mochangsheng on 2016/11/22.
//

#include "TiffInputStream.h"
#define LOG_TAG  ("TiffInputStream_Native")
const char* TiffInputStream::m_name = "TiffInputStream";

//重定向error输出到logcat
static void logError(const char* module, const char* fmt, va_list ap)
{
    char errorBuffer[1024];
    vsnprintf(errorBuffer, 1024, fmt, ap);
    LOGE("========logError module==%s, error==%s",module, errorBuffer);
}

TiffInputStream::TiffInputStream() {
    m_tif = NULL;
    m_inStream = NULL;
    m_streamLength = 0;
    m_this = reinterpret_cast<thandle_t>(this);
}

TiffInputStream::~TiffInputStream() {
    LOGI("==========TiffInputStream::~TiffInputStream");
    if(m_tif != NULL) {
        TIFFClose(m_tif);
        m_tif = NULL;
    }
}

TIFF *TiffInputStream::makeFileStream(JavaInputStreamAdaptor *str) {
    m_inStream = str;
    m_streamLength = m_inStream->available();
    //mark标记一下，用于reset使用
    m_inStream->mark(m_streamLength * 2);

    //自定义Tiff error重定向，输出error信息
    TIFFSetErrorHandler(logError);

    m_tif =  TIFFClientOpen(m_name,
                            "r",
                            m_this,
                            read,
                            write,
                            seek,
                            close,
                            size,
                            map,
                            unmap);
    return m_tif;
}

tsize_t TiffInputStream::read(thandle_t fd, tdata_t buf, tsize_t size) {

    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);
    if(ts->m_inStream != NULL) {
        long remain = ts->m_inStream->available();
        size_t actual = (size_t) (remain < size ? remain : size);
        size_t size = ts->m_inStream->read(reinterpret_cast<char*>(buf), actual);
        return size;
    } else {
        return 0;
    }
}

//暂不实现
tsize_t TiffInputStream::write(thandle_t fd, tdata_t buf, tsize_t size) {
    return 0;
}

toff_t TiffInputStream::seek(thandle_t fd, toff_t offset, int origin) {

    if (DEBUG) {
        LOGI("=====TiffStream::seek offset==%ud,origin==%d",offset, origin);
    }

    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);

    if(ts->seekInt(fd, offset, origin)) {
        return offset;
    } else {
        return -1;
    }
}

toff_t TiffInputStream::size(thandle_t fd) {
    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);
    return ts->getSize(fd);
}

int TiffInputStream::close(thandle_t fd) {

    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);
    if(ts->m_inStream != NULL) {
        ts->m_inStream = NULL;
        return 0;
    }
    return -1;
}

//空实现
int TiffInputStream::map(thandle_t fd, tdata_t *phase, toff_t *psize) {
    return 0;
}

//空实现
void TiffInputStream::unmap(thandle_t fd, tdata_t base, toff_t size) {

}

unsigned long TiffInputStream::getSize(thandle_t fd) {
    return m_streamLength;
}


bool TiffInputStream::seekInt(thandle_t fd, unsigned int offset, int origin) {

    if (DEBUG) {
        LOGI("=====TiffStream::seek offset==%ud,origin==%d",offset, origin);
    }

    if(!isOpen(fd)) return false;
    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);
    if(ts->m_inStream != NULL) {
        long int skipSize = 0;
        long int hadSkipSize = 0;
        switch(origin) {
            case beg:
                skipSize = offset;
                ts->m_inStream->reset();
                hadSkipSize = ts->m_inStream->doSkip(skipSize);
                if (DEBUG) LOGI("======seekInt skiptype==beg");
                break;
            case cur:
                skipSize = offset;
                hadSkipSize = ts->m_inStream->doSkip(skipSize);
                if (DEBUG) LOGI("======seekInt skiptype==cur");
                break;
            case end:
                ts->m_inStream->reset();
                skipSize = ts->m_streamLength - offset;
                hadSkipSize = ts->m_inStream->doSkip(skipSize);
                if (DEBUG) LOGI("======seekInt skiptype==end");
                break;
        }

        if (DEBUG) LOGI("======seekInt skipType==%d,offset==%d,m_streamLength==%ld,hadSkipSize==%ld"
                     ,origin,offset,ts->m_streamLength,hadSkipSize);

        return true;
    }

    return false;
}

bool TiffInputStream::isOpen(thandle_t fd) {
    TiffInputStream* ts = reinterpret_cast<TiffInputStream*>(fd);
    return (ts->m_inStream != NULL);
}





























