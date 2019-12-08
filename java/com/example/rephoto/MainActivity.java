package com.example.rephoto;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
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

    public enum State {
        NO_REF,
        SHOW_REF,
        HAVE_REF,
        REPHOTO,
        REPHOTO_DONE,
        FINISH
    }

    State state;

    private static double scaleTakeImage = 1;
    private Size previewSize;
    /** Camera show*/
    private SurfaceHolder mHolder;
    private CameraView cameraView;

    /** view */
    private FrameLayout preview;
    private DrawView drawView;

    private Button takePhotoButton;
    private Button openButton;
    private Button okButton;
    private Button clearButton;
    private Button changeButton;

    /** */
    private Bitmap tempBitmap;

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
        cameraView = new CameraView(this, drawView);
        preview.addView(cameraView);

        /** Button */
        takePhotoButton = (Button)findViewById(R.id.take_picture);
        takePhotoButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                takePhotoClickAction();
            }
        });
    }

    private void gotoSHOW_REF() {
        state = State.SHOW_REF;
        okButton.setVisibility(View.VISIBLE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.INVISIBLE);
    }

    private void gotoHAVE_REF() {
        state = State.HAVE_REF;
        okButton.setVisibility(View.GONE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.VISIBLE);
    }

    private void gotoREPHOTO() {
        state = State.REPHOTO;
        okButton.setVisibility(View.GONE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button_stop);
    }

    private void gotoREPHOTO_DONE() {
        state = State.REPHOTO_DONE;
        okButton.setVisibility(View.VISIBLE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.INVISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button);
    }

    private void takePhotoClickAction() {
        final Bitmap bitmap = tempBitmap;
        if (bitmap != null) {
            Log.i("Take picture", bitmap.getWidth() + "," + bitmap.getHeight());
            switch (state) {
                case NO_REF:
                    cameraView.takePicture();
                    gotoSHOW_REF();
                    break;
                case HAVE_REF:
                    gotoREPHOTO();
                    break;
                case REPHOTO:
                    cameraView.takePicture();
                    gotoREPHOTO_DONE();
            }
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
