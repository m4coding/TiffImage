LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    TiffImage.cpp \
    JNIHelpers.cpp \
    TiffImage_jni.cpp \
    JavaInputStreamAdaptor.cpp \
    TiffInputStream.cpp \
    tiffstream.cpp

#libtiff库代码所在路径
TIFF_C_LIB_PATH := \
    $(LOCAL_PATH)/../tiff/libtiff

LOCAL_C_INCLUDES := \
    $(TIFF_C_LIB_PATH) \
    $(TIFF_C_LIB_PATH)/tiff_config_headers/$(TARGET_ARCH_ABI)

LOCAL_MODULE := libtiff_image_jni


#链接系统内置的库
#liblog.so
LOCAL_LDLIBS := \
    -llog  \
    -lz

LOCAL_STATIC_LIBRARIES := \
    libtiff

include $(BUILD_SHARED_LIBRARY)