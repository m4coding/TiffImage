//
// Created by mochangsheng on 2016/11/22.
//

#ifndef TIFFIMAGE_TIFFINPUTSTREAM_H
#define TIFFIMAGE_TIFFINPUTSTREAM_H
#include "tiffio.h"
#include "JavaInputStreamAdaptor.h"

class TiffInputStream {

public:
    // ctor/dtor
    TiffInputStream();
    ~TiffInputStream();

public:
    enum SeekDir {
        beg,
        cur,
        end,
    };

public:
    // factory methods
    TIFF* makeFileStream(JavaInputStreamAdaptor* str);

public:
    // tiff client methods
    static tsize_t read(thandle_t fd, tdata_t buf, tsize_t size);
    static tsize_t write(thandle_t fd, tdata_t buf, tsize_t size);
    static toff_t seek(thandle_t fd, toff_t offset, int origin);
    static toff_t size(thandle_t fd);
    static int close(thandle_t fd);
    static int map(thandle_t fd, tdata_t* phase, toff_t* psize);
    static void unmap(thandle_t fd, tdata_t base, toff_t size);

public:
    // query method
    TIFF* getTiffHandle() const { return m_tif; }
    unsigned int getStreamLength() { return m_streamLength; }

private:
    // internal methods
    unsigned long getSize(thandle_t fd);
    //unsigned int tell(thandle_t fd);
    bool seekInt(thandle_t fd, unsigned int offset, int origin);
    bool isOpen(thandle_t fd);

private:
    thandle_t m_this;
    TIFF* m_tif;
    static const char* m_name;
    JavaInputStreamAdaptor* m_inStream;
    long m_streamLength;
};
#endif //TIFFIMAGE_TIFFINPUTSTREAM_H
