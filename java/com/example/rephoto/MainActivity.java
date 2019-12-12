package com.example.rephoto;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

import org.opencv.android.OpenCVLoader;
import org.opencv.core.Size;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import static com.example.rephoto.State.FINISH;
import static com.example.rephoto.State.HAVE_REF;
import static com.example.rephoto.State.NO_REF;
import static com.example.rephoto.State.REPHOTO;
import static com.example.rephoto.State.REPHOTO_DONE;
import static com.example.rephoto.State.SHOW_REF;

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

    GetPathFromUri4kitkat getPathFromUri4kitkat = new GetPathFromUri4kitkat();

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
        initButton();

        gotoNO_REF();

    }

    private void initButton() {
        takePhotoButton = (Button)findViewById(R.id.take_picture);
        takePhotoButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                takePhotoClickAction();
            }
        });

        openButton = (Button) findViewById(R.id.openButton);
        openButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                takeOpenClickAction();
            }
        });

        okButton = (Button) findViewById(R.id.okButton);
        okButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                takeOkClickAction();
            }
        });

        clearButton = (Button) findViewById(R.id.closeButton);
        clearButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                takeClearClickAction();
            }
        });
    }


    private void takePhotoClickAction() {
        switch (MyUtility.state) {
            case NO_REF:
                cameraView.takePicture();
                gotoSHOW_REF();
                break;
            case HAVE_REF:
                MyUtility.state = REPHOTO;
                gotoREPHOTO();
                break;
            case REPHOTO:
                cameraView.takePicture();
                gotoREPHOTO_DONE();
                /**choose image*/
        }
    }

    private void takeOpenClickAction() {
        switch (MyUtility.state) {
            case NO_REF:
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("image/*");
                startActivityForResult(intent, 100);
        }
    }

    private void takeOkClickAction() {
        switch (MyUtility.state) {
            case SHOW_REF:
                final EditText inputServer = new EditText(MainActivity.this);
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setTitle("Input Image Name:").setView(inputServer).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });

                builder.setPositiveButton("ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String nowName = inputServer.getText().toString();
                        if (nowName == "") {
                            nowName = System.currentTimeMillis() + "";
                        }
                        MyUtility.state = HAVE_REF;
                        gotoHAVE_REF();
                        //cameraView.saveRefImage(nowName);
                    }
                });
                builder.show();
                Log.i("goto haveref", "haveref");

                break;

            case REPHOTO_DONE:
                break;
            case SWITCH:
                break;
        }
    }

    private void takeClearClickAction() {
        switch (MyUtility.state) {
            case NO_REF:
                break;
            case SHOW_REF:
                MyUtility.state = NO_REF;
                gotoNO_REF();
                break;
            case HAVE_REF:
                MyUtility.state = NO_REF;
                gotoNO_REF();
                break;
            case REPHOTO_DONE:
                MyUtility.state = HAVE_REF;
                gotoHAVE_REF();
                break;
            case FINISH:
                MyUtility.state = FINISH;
                gotoHAVE_REF();
                break;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == Activity.RESULT_OK && requestCode == 100) {
            Uri selectedImage = data.getData();
            if (selectedImage != null) {
                String imagePath = getPathFromUri4kitkat.getPath(MainActivity.this, selectedImage);
                String imageName = imagePath.substring(imagePath.lastIndexOf("/") + 1, imagePath.lastIndexOf("-"));
                try {
                    Bitmap bitmap = BitmapFactory.decodeStream(this.getContentResolver().openInputStream(selectedImage));
                    bitmap = MyUtility.scaleBitmap(bitmap, (float) (1920.0f / bitmap.getWidth()), (float) (1080.0f / bitmap.getHeight()));
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    private void gotoNO_REF() {
        okButton.setVisibility(View.GONE);
        openButton.setVisibility(View.VISIBLE);
        clearButton.setVisibility(View.GONE);
        takePhotoButton.setVisibility(View.VISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button);

    }

    private void gotoSHOW_REF() {
        okButton.setVisibility(View.VISIBLE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.INVISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button);

    }

    private void gotoHAVE_REF() {
        okButton.setVisibility(View.GONE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.VISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button);

    }

    private void gotoREPHOTO() {
        okButton.setVisibility(View.GONE);
        openButton.setVisibility(View.GONE);
        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.VISIBLE);

        takePhotoButton.setBackgroundResource(R.drawable.snap_button_stop);
    }

    private void gotoREPHOTO_DONE() {
        okButton.setVisibility(View.VISIBLE);
        openButton.setVisibility(View.GONE);

        clearButton.setVisibility(View.VISIBLE);
        takePhotoButton.setVisibility(View.INVISIBLE);
        takePhotoButton.setBackgroundResource(R.drawable.snap_button);
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
