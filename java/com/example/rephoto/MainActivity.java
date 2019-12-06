package com.example.rephoto;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import org.opencv.android.OpenCVLoader;
import org.opencv.core.Size;

import java.io.IOException;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        if (!OpenCVLoader.initDebug()) {
            Log.i("Opencv Init", "error");
        }
        System.loadLibrary("opencv_java3");
        System.loadLibrary("native-lib");
    }

    private static double scaleTakeImage = 1;
    private Size previewSize;
    /** Camera show*/
    private Camera mCamera;
    private SurfaceHolder mHolder;
    private CameraView cameraView;

    /** view */
    private FrameLayout preview;
    private DrawView drawView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        //TextView tv = findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI());

        /** Force landscape*/
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN) ;//隐藏状态栏

        init();
    }

    private void init() {
        /**init view*/
        preview = (FrameLayout) findViewById(R.id.preview);

        drawView = new DrawView(this);
        preview.addView(drawView);
        drawView.setZOrderOnTop(true);
        drawView.setZOrderMediaOverlay(true);

        /** camera*/
        cameraView = new CameraView(this);
        preview.addView(cameraView);
    }

    class CameraView extends SurfaceView implements SurfaceHolder.Callback {
        public CameraView(Context context) {
            super(context);
            Log.i("Screen size", context.getResources().getDisplayMetrics().widthPixels + "," + context.getResources().getDisplayMetrics().heightPixels);
            Log.i("CameraView", "INIT");
            initCamera();
            mHolder = this.getHolder();
            mHolder.addCallback(this);

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

    private Camera.PreviewCallback mCameraPreviewCallback = new Camera.PreviewCallback() {

        @Override
        public void onPreviewFrame(byte[] bytes, Camera camera) {

        }
    };

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

    public static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return c;
    }
    private void initLines() {
        //originLocate = new LocateInfo(1920, 1080, scaleFrameSize);
    }


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

    public Camera.Size getBestPictureSize() {
        Camera.Size size = mCamera.new Size(1920, 1080);
        return size;
    }

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
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
