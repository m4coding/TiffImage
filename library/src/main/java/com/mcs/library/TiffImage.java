package com.mcs.library;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.util.Log;
import android.util.TypedValue;

import java.io.IOException;
import java.io.InputStream;

/**
 * @author mochangsheng
 * @version 1.0
 * @description Tiff解码类
 * @created 2016/11/15
 * @changeRecord [修改记录] <br/>
 */

public class TiffImage {

    private static final String TAG = TiffImage.class.getSimpleName();
    private static boolean DEBUG = true;

    static {
        System.loadLibrary("tiff_image_jni");
    }

    private long mNativeContext; // accessed by native methods
    private String mPath;

    public TiffImage() {
        _nativeSetup();
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            _nativeRelease();
        } finally {
            super.finalize();
        }
    }

    public Bitmap decodeResource(Resources res, int id) {
        Bitmap bm = null;
        InputStream is = null;

        try {
            final TypedValue value = new TypedValue();
            is = res.openRawResource(id, value);
            bm = decode(is);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (is != null) is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return bm;
    }

    public Bitmap decode(String path) {
        mPath = path;

        int[] picture = _decode(mPath);

        return convertIntToBitmap(picture);
    }

    public  Bitmap decode(InputStream is) {

        byte [] tempStorage = new byte[16 * 1024];

        int[] picture = _decode(is, tempStorage);

        return convertIntToBitmap(picture);
    }

    private Bitmap convertIntToBitmap(int[] picture) {

        Bitmap b = null;

        if (picture != null) {
            if (DEBUG) Log.d(TAG, "=======decode picture==" + picture.length);
            b = Bitmap.createBitmap(picture, getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        } else {
            Log.e(TAG, "=======decode picture==null");
        }

        return b;
    }

    public String getPath() {
        return mPath;
    }

    public static ImageInfo getImageInfo(String path) {
        return _getImageInfo(path);
    }

    public int getWidth() {
        return _getWidth();
    }

    public int getHeight() {
        return _getHeight();
    }

    public long getSize() {
        return _getSize();
    }

    public void release() {
        _nativeRelease();
    }

    /**********Native Method************/
    private native void _nativeSetup();
    private native void _nativeRelease();
    private native int[] _decode(String path);
    private native int[] _decode(InputStream is, byte[] storage);
    private native int _getWidth();
    private native int _getHeight();
    private native long _getSize();
    private static native ImageInfo _getImageInfo(String path);

    public static class ImageInfo {
        public int width;
        public int height;

        public ImageInfo(int width, int height) {
            this.width = width;
            this.height = height;
        }
    }

}
