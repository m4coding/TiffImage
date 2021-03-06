LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#打印信息 编译的架构信息
$(info TARGET_ARCH:$(TARGET_ARCH))
$(info APP_PLATFORM:$(APP_PLATFORM))
$(info TARGET_ARCH_ABI:$(TARGET_ARCH_ABI))

#注意添加上libtiff/tif_unix.c (linux系统相关的)
TIFF_SRC_FILES := \
    libtiff/tif_aux.c \
    libtiff/tif_close.c \
    libtiff/tif_codec.c \
    libtiff/tif_color.c \
    libtiff/tif_compress.c \
    libtiff/tif_dir.c \
    libtiff/tif_dirinfo.c \
    libtiff/tif_dirread.c \
    libtiff/tif_dirwrite.c \
    libtiff/tif_dumpmode.c \
    libtiff/tif_error.c \
    libtiff/tif_extension.c \
    libtiff/tif_fax3.c \
    libtiff/tif_fax3sm.c \
    libtiff/tif_flush.c \
    libtiff/tif_getimage.c \
    libtiff/tif_jbig.c \
    libtiff/tif_jpeg.c \
    libtiff/tif_jpeg_12.c \
    libtiff/tif_luv.c \
    libtiff/tif_lzma.c \
    libtiff/tif_lzw.c \
    libtiff/tif_next.c \
    libtiff/tif_ojpeg.c \
    libtiff/tif_open.c \
    libtiff/tif_packbits.c \
    libtiff/tif_pixarlog.c \
    libtiff/tif_predict.c \
    libtiff/tif_print.c \
    libtiff/tif_read.c \
    libtiff/tif_strip.c \
    libtiff/tif_swab.c \
    libtiff/tif_thunder.c \
    libtiff/tif_tile.c \
    libtiff/tif_unix.c \
    libtiff/tif_version.c \
    libtiff/tif_warning.c \
    libtiff/tif_write.c \
    libtiff/tif_zip.c

#TIFF_STREAM_SRC_FILES := \
#    contrib/stream/tiffstream.cpp

LOCAL_SRC_FILES := \
    $(TIFF_SRC_FILES)
#    $(TIFF_STREAM_SRC_FILES)

$(info $(LOCAL_PATH)/libtiff/tiff_config_headers/$(TARGET_ARCH_ABI))

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/libtiff/tiff_config_headers/$(TARGET_ARCH_ABI)/ \
    $(LOCAL_PATH)/libtiff \
    $(LOCAL_PATH)/../jpeg \
    $(LOCAL_PATH)/contrib/stream

#链接系统内置的库
#liblog.so
#libz.so
LOCAL_LDLIBS := \
    -llog \
    -lz


LOCAL_STATIC_LIBRARIES := \
    libjpeg

LOCAL_MODULE := libtiff

include $(BUILD_STATIC_LIBRARY)



