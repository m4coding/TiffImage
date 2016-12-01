# TiffImage
decode tiff image for android


解码tiff图片，基于libtiff开源库

![](http://i.imgur.com/0mCC9jF.jpg)

### 支持 ###

本地路径读取，流方式的读取，Resource资源读取，assets资源读取


### 使用 ###

（1）本地路径读取

	TiffImage tiffImage = new TiffImage();

    Bitmap bitmap = tiffImage.decode(path);//path为绝对路径

	tiffImage.release();//释放资源

	imageView.setImageBitmap(bitmap);

（2）流方式的读取

	TiffImage tiffImage = new TiffImage();

	ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(bytes);

    Bitmap bitmap = tiffImage.decode(byteArrayInputStream);

	tiffImage.release();//释放资源

	imageView.setImageBitmap(bitmap);

（3）Resource资源读取

	TiffImage tiffImage = new TiffImage();

    Bitmap bitmap = tiffImage.decodeResource(getResources(),R.raw.test_res);

	tiffImage.release();//释放资源

	imageView.setImageBitmap(bitmap);

（4）assets资源读取

	TiffImage tiffImage = new TiffImage();

    InputStream inputStream = null;
    try {
        inputStream = getAssets().open("test_assets.tif");
        byte[] bytes= inputStream2Byte(inputStream);
        ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(bytes);
        Bitmap bitmap = tiffImage.decode(byteArrayInputStream);
    } catch (IOException e) {
        e.printStackTrace();
    }

	tiffImage.release();//释放资源

	imageView.setImageBitmap(bitmap);