package com.mcs.tiffimage;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.format.Formatter;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.mcs.library.TiffImage;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();
    private static final String DEFAULT_PIC_PATH = "default_pic";
    private static final String ASSETS_PIC_PATH = "test_assets.tif";
    private static final String ASSETS_TEST_PIC_PATH = "test_path.tif";
    private static final String ASSETS_TEST_STREAM_PATH = "test_stream.tif";


    private Button mButton;
    private ImageView mImageView;
    private String mPath,mStringPath;
    private Button mInfoButton;
    private Button mStreamButton, mResButton,mAssetsButton;
    private TextView mInfoTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initView();
        initData();
    }

    private void initView() {
        setContentView(R.layout.activity_main);
        mButton = (Button) findViewById(R.id.decode_button);
        mStreamButton = (Button) findViewById(R.id.decode_stream_button);
        mResButton = (Button) findViewById(R.id.decode_resource_button);
        mInfoButton = (Button) findViewById(R.id.info_button);
        mAssetsButton = (Button) findViewById(R.id.decode_assets_button);
        mImageView = (ImageView) findViewById(R.id.image);
        mInfoTextView = (TextView) findViewById(R.id.info_text);
        mButton.setOnClickListener(mOnClickListener);
        mInfoButton.setOnClickListener(mOnClickListener);
        mStreamButton.setOnClickListener(mOnClickListener);
        mResButton.setOnClickListener(mOnClickListener);
        mAssetsButton.setOnClickListener(mOnClickListener);
    }

    private void initData() {
        copyAssets();

        mPath = getFilesDir().getAbsolutePath() +
                File.separator + DEFAULT_PIC_PATH + File.separator + ASSETS_TEST_PIC_PATH;

        mStringPath = getFilesDir().getAbsolutePath() +
                File.separator + DEFAULT_PIC_PATH + File.separator + ASSETS_TEST_STREAM_PATH;
    }

    private void copyAssets() {
        String dirPath = getFilesDir().getAbsolutePath() + File.separator + DEFAULT_PIC_PATH;
        File file = new File(dirPath);

        if (!file.exists()) {
            file.mkdirs();
        }

        copyAssetsPath(dirPath, ASSETS_TEST_PIC_PATH, ASSETS_TEST_PIC_PATH);
        copyAssetsPath(dirPath, ASSETS_TEST_STREAM_PATH, ASSETS_TEST_STREAM_PATH);
    }

    private void copyAssetsPath(String outDir, String name, String assetsPath) {
        //复制
        AssetManager assetManager = getAssets();

        InputStream in = null;
        OutputStream out = null;
        try {
            in = assetManager.open(assetsPath);
            File outFile = new File(outDir, name);
            out = new FileOutputStream(outFile);
            byte[] buffer = new byte[1024];
            int read;
            while((read = in.read(buffer)) != -1){
                out.write(buffer, 0, read);
            }
        } catch(IOException e) {
            e.printStackTrace();
        }

        finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int id = v.getId();
            TiffImage tiffImage = null;
            byte[] bytes = null;
            Bitmap bitmap = null;
            ByteArrayInputStream byteArrayInputStream = null;
            boolean isSetBitmap = false;

            switch (id) {
                case R.id.decode_button: {
                    tiffImage = new TiffImage();
                    bitmap = tiffImage.decode(mPath);
                    isSetBitmap = true;
                }
                    break;
                case R.id.info_button: {
                    TiffImage.ImageInfo imageInfo = TiffImage.getImageInfo(mPath);
                    mInfoTextView.setText("width==" + imageInfo.width + "; height==" + imageInfo.height);
                    mInfoTextView.setVisibility(View.VISIBLE);
                }
                    return;
                case R.id.decode_stream_button: {
                    tiffImage = new TiffImage();
                    bytes = file2Byte(mStringPath);
                    byteArrayInputStream = new ByteArrayInputStream(bytes);
                    bitmap = tiffImage.decode(byteArrayInputStream);
                    isSetBitmap = true;
                }
                    break;
                case R.id.decode_resource_button: {
                    tiffImage = new TiffImage();
                    bitmap = tiffImage.decodeResource(getResources(),R.raw.test_res);
                    isSetBitmap = true;
                }
                    break;
                case R.id.decode_assets_button: {
                    tiffImage = new TiffImage();
                    InputStream inputStream = null;
                    try {
                        inputStream = MainActivity.this.getAssets().open(ASSETS_PIC_PATH);
                        bytes= inputStream2Byte(inputStream);
                        byteArrayInputStream = new ByteArrayInputStream(bytes);
                        bitmap = tiffImage.decode(byteArrayInputStream);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    isSetBitmap = true;
                }
                    break;
                default:
                    break;
            }

            if (isSetBitmap) {
                mImageView.setImageBitmap(bitmap);
            }

            if (bytes != null) {
                Log.d(TAG, "tiffImage decode stream width==" + tiffImage.getWidth() + "; height=="
                        + tiffImage.getHeight() + "; size==" +
                        Formatter.formatFileSize(MainActivity.this, tiffImage.getSize()) + "; byte'length==" +
                        Formatter.formatFileSize(MainActivity.this, bytes.length));
            } else {
                Log.d(TAG, "tiffImage decode width==" + tiffImage.getWidth() + "; height=="
                        + tiffImage.getHeight() + "; size==" +
                        Formatter.formatFileSize(MainActivity.this, tiffImage.getSize()));
            }

            if (tiffImage != null) {
                tiffImage.release();
                tiffImage = null;
            }
        }
    };


    public byte[] file2Byte(String filePath) {
        byte[] buffer = null;
        FileInputStream fis = null;
        try {
            File file = new File(filePath);
            fis = new FileInputStream(file);
            buffer = inputStream2Byte(fis);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return buffer;
    }

    public byte[] inputStream2Byte(InputStream inputStream) {
        byte[] buffer = null;
        InputStream fis = inputStream;
        ByteArrayOutputStream bos = null;

        try {
            bos = new ByteArrayOutputStream();
            byte[] b = new byte[1024];
            int n;
            while ((n = fis.read(b)) != -1) {
                bos.write(b, 0, n);
            }
            buffer = bos.toByteArray();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {

            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return buffer;
    }
}
