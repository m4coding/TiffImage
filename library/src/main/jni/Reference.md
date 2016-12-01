### 编译过程中遇到的问题 ###

（1）当使用LOCAL\_SHARED\_LIBRARIES时，若还是没有引用到库中的函数，则可以用

		nm -D xxx.so
		
		objdump -tT xxx.so

命令来看下so库是否包含这些函数。

（2）使用TIFFSetErrorHandler方法可以重定向错误，导出，如导入到android locat输出
		
		static char* errorBuffer;	
		//自定义Tiff error重定向，输出error信息
	    TIFFSetErrorHandler(logError);

		static void logError(const char* module, const char* fmt, va_list ap)
		{
		    if (!errorBuffer) {
		        errorBuffer = new char[1024];
		    }
		    vsnprintf(errorBuffer, 1024, fmt, ap);
		    LOGE("========logError module==%s, error%s",module, errorBuffer);
		}

（3）使用TIFFReadRGBAImage函数出现异常，error打印：

	errorSorry, requested compression method is not configured, 原因是没有在tiffconf.h配置头文件中配置JPEG_SUPPORT宏

（4）错误信息：

	module==JPEGLib, errorWrong JPEG library version: library is 62, caller expects 90
	配置的jpeg库版本不一致导致的

（5）编译libjpeg时需要配置头文件jconfig.h，可以从android源码中获取

（6）libtiff库，开启tiffconf.h中的宏：
	#define OJPEG_SUPPORT
	#define JPEG_SUPPORT
	#define ZIP_SUPPORT
	添加jdhuff_add.c中的函数到libjpeg库中的jdhuff.c中

（7）最好编译jpge静态库，因为如果系统也有libjpeg动态库，那么会导致新加的方法不能添加进入

#### jni初始化内部类 示例####

	//通过命令，javap -s -p TiffImage$ImageInfo.class  
	//javap -s -p TiffImage.class 可以查看到类详细情况

	//静态内部类
	public class TiffImage {

		public static class ImageInfo {
			public ImageInfo(int width, int height) {

			}
		}
	}

	//jni初始化
    jclass  jclass1 = env->FindClass("com/mcs/library/TiffImage$ImageInfo");
    jmethodID  jmethodID1 = env->GetMethodID(jclass1,"<init>", "(II)V");
    jobject  jobject1 = env->NewObject(jclass1,jmethodID1,imageInfo.width, imageInfo.height);



	//内部类
	public class TiffImage {

		public static class ImageInfo {
			public ImageInfo(int width, int height) {

			}
		}
	}

	//jni初始化
    jclass  jclass1 = env->FindClass("com/mcs/library/TiffImage$ImageInfo");
	//参数隐藏着一个TiffImage类，注意！！！
    jmethodID  jmethodID1 = env->GetMethodID(jclass1,"<init>", "(Lcom/mcs/library/TiffImage;II)V");
    jobject  jobject1 = env->NewObject(jclass1,jmethodID1,thiz,imageInfo.width, imageInfo.height);


(8)libtiff 4.0.5版本，使用源码中tiffstream.cpp，用于打开iostrem，添加Application.mk，添加如下内容

	APP_STL := gnustl_static
	修改引用头文件<iostream>
	修改static void unmap(thandle_t fd, tdata_t base, tsize_t size);第三个参数类型为toff_t
	强制转换ts->m_outStream->seekp(offset, org);为ts->m_outStream->seekp(offset, (ios_base::seekdir) org);

(9)使用BufferedInputStream时，需要注意下skip方法的skip是跳过buffer中的而不是全部的，使用ByteArrayInputStream代替