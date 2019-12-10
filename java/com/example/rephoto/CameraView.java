package com.example.rephoto;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceView;
import android.view.SurfaceHolder;

import org.opencv.core.Size;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.List;

public class CameraView extends SurfaceView implements SurfaceHolder.Callback {

    private Camera mCamera;
    private SurfaceHolder mHolder;
    private DrawView mDraw;

    private float scaleShowImage = 0.5f;
    private float scaleCalImage = 0.5f;

    private Bitmap takeRefImage;
    private Bitmap refImage;

    private Bitmap rephotoImage;

    private Bitmap alphaBlend;

    private Bitmap takeResultImage;
    private Bitmap resultImage;

    /** Do in each frame */
    private Camera.PreviewCallback mCameraPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            switch (MyUtility.state) {
                case NO_REF:
                    mDraw.getCanvas();
                    mDraw.clearDraw();
                    mDraw.update();
                    break;
                case SHOW_REF:
                    mDraw.getCanvas();
                    mDraw.clearDraw();
                    if (refImage != null) {
                        mDraw.drawBitmap(refImage);
                    }
                    mDraw.update();
                    break;
                case HAVE_REF:
                    mDraw.getCanvas();
                    mDraw.clearDraw();
                    mDraw.update();
                    break;
                case REPHOTO:
                    Camera.Size size = mCamera.getParameters().getPreviewSize();
                    try {
                        YuvImage image = new YuvImage(data, ImageFormat.NV21, size.width,
                                size.height, null);

                        if (image != null) {
                            ByteArrayOutputStream stream = new ByteArrayOutputStream();
                            image.compressToJpeg(new Rect(0, 0, size.width, size.height),
                                    100, stream);

                            Bitmap nowImage = MyUtility.rotateBitmap(BitmapFactory.decodeByteArray(
                                    stream.toByteArray(), 0, stream.size()), 0);

                            stream.close();

                            rephotoImage = nowImage;

                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    mDraw.getCanvas();
                    mDraw.clearDraw();
                    mDraw.drawBitmap(alphaBlend);
                    mDraw.update();
                    break;

            }
        }
    };

    private Camera.PictureCallback mCameraPictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            switch (MyUtility.state) {
                case NO_REF:
                    takeRefImage = MyUtility.rotateBitmap(BitmapFactory.decodeByteArray(data, 0, data.length), 0);
                    refImage = MyUtility.scaleBitmap(takeRefImage, scaleShowImage, scaleShowImage);
                    startCameraPreview();
                    break;
                case REPHOTO:
                    takeResultImage = MyUtility.rotateBitmap(BitmapFactory.decodeByteArray(data, 0, data.length), 0);
                    startCameraPreview();
                    resultImage = MyUtility.scaleBitmap(takeRefImage, scaleShowImage, scaleShowImage);
                    break;
            }
        }
    };



    /** camera view size */
    private Size previewSize;

    /** nothing, only keep from last version */
    private static double scaleTakeImage = 1;

    public CameraView(Context context, DrawView drawView) {
        super(context);
        initCamera();
        mHolder = this.getHolder();
        mHolder.addCallback(this);
        mDraw = drawView;
    }

    public void takePicture() {
        mCamera.takePicture(null, null, mCameraPictureCallback);
    }

    /** return a camera instance */
    public static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return c;
    }

    /** return a camera size for capturing*/
    public Camera.Size getBestPictureSize() {
        Camera.Size size = mCamera.new Size(1920, 1080);
        return size;
    }

    /** show all support size to log*/
    public void printSupportedSize() {
        Camera.Parameters parameters = mCamera.getParameters();
        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        for (Camera.Size size : sizes) {
            float now = 1.0f * size.width / size.height;
            Log.i("Size", size.width + "," + size.height + "," + now);
        }
        sizes = parameters.getSupportedPictureSizes();
        for (Camera.Size size : sizes) {
            float now = 1.0f * size.width / size.height;
            Log.i("Picture Size", size.width + "," + size.height + "," + now);
        }
    }

    /** TODO : compute navigation */
    private void initLines() {
        //originLocate = new LocateInfo(1920, 1080, scaleFrameSize);
    }

    /** create a camera */
    private void initCamera() {
        mCamera = getCameraInstance();
        /**旋转90度显示*/
        mCamera.setDisplayOrientation(90);

        Camera.Parameters parameters = mCamera.getParameters();
        Log.d("test", "------------------------_");

        /**设置照片大小*/
        Camera.Size bestPictureSize = getBestPictureSize();
        printSupportedSize();
        parameters.setPictureSize((int)(bestPictureSize.width*scaleTakeImage), (int)(bestPictureSize.height*scaleTakeImage));
        parameters.setZoom(1);
        parameters.setPictureSize(3840, 2160);
        parameters.setPreviewSize(bestPictureSize.width, bestPictureSize.height);
        Log.i("Init Camera", parameters.getPreviewSize().width + "," + parameters.getPreviewSize().height);

        previewSize = new Size((double)parameters.getPreviewSize().height, (double)parameters.getPreviewSize().width);
        initLines();

        //Camera.Parameters params = mCamera.getParameters();
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        //mCamera.setParameters(params);
        mCamera.setParameters(parameters);
    }


    /** start camera preview */
    private void startCameraPreview() {
        if (mCamera == null) {
            return ;
        }

        try {
            //*EDIT*//params.setFocusMode("continuous-picture");
            //It is better to use defined constraints as opposed to String, thanks to AbdelHady
            //Camera.Parameters params = mCamera.getParameters();
            //params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            //mCamera.setParameters(params);
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(mCameraPreviewCallback);
            mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        Log.i("CameraView", "Created");
        startCameraPreview();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
