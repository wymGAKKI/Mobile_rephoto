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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
