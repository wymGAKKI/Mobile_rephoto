package com.example.rephoto;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.Point;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.Exchanger;

public class MyUtility {

    public static double THRES = 5;
    public static double INF = 1e9;
    public static String okInfo = "-----------";

    public static double getPolygonArea(Point[] points) {
        int length = points.length;
        double area = 0;
        for (int i = 0; i < length; i++) {
            int j = (i + 1) % length;
            area += points[i].x * points[j].y - points[i].y * points[j].x;
        }
        return Math.abs(area * 0.5);
    }

    public static double getSimilarity(Mat src1, Mat src2) {

        Mat hist0 = new Mat();
        Mat hist1 = new Mat();

        int hist_bins = 30;           //number of histogram bins
        int hist_range[]= {0,180};//histogram range
        MatOfFloat ranges = new MatOfFloat(0f, 256f);
        MatOfInt histSize = new MatOfInt(25);

        Imgproc.calcHist(Arrays.asList(src1), new MatOfInt(0), new Mat(), hist0, histSize, ranges);
        Imgproc.calcHist(Arrays.asList(src2), new MatOfInt(0), new Mat(), hist1, histSize, ranges);

        double test = Imgproc.compareHist(hist0, hist1, Imgproc.CV_COMP_CORREL);
        Log.i("Similiar", test +"");
        return test;
    }

    public static void swap(double[] data, int i, int j) {
        double temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    public static void swap(String[] data, int i, int j) {
        String temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    public static void writeFileSdcardFile(Context context, String fileName, String write_str) {
        try{

            String storePath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "rephoto";
            File appDir = new File(storePath);
            File file = new File(appDir, fileName);
            FileOutputStream fout = new FileOutputStream(file);
            byte [] bytes = write_str.getBytes();

            fout.write(bytes);
            fout.close();
            Uri uri = Uri.fromFile(file);
            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
        }

        catch(Exception e){
            e.printStackTrace();
        }
    }

    public static String readFileSdcardFile(Context context, String fileName) {
        String result = "";
        try {
            String storePath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "rephoto";
            File appDir = new File(storePath);
            File file = new File(appDir, fileName);
            FileInputStream fin = new FileInputStream(file);
            int length = fin.available();
            byte[] buffer = new byte[length];
            fin.read(buffer);
            result = new String(buffer, "UTF-8");
        } catch(Exception e) {
            e.printStackTrace();
        }
        return result;
    }


    public static boolean saveImageToGallery(Context context, Bitmap bmp, String floderName, String fileName) {
        // 首先保存图片
        String storePath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + floderName;
        Log.i("info", fileName);
        File appDir = new File(storePath);
        if (!appDir.exists()) {
            appDir.mkdir();
        }
        File file = new File(appDir, fileName);
        try {
            FileOutputStream fos = new FileOutputStream(file);
            //通过io流的方式来压缩保存图片
            boolean isSuccess = bmp.compress(Bitmap.CompressFormat.JPEG, 60, fos);
            fos.flush();
            fos.close();

            //把文件插入到系统图库
            //MediaStore.Images.Media.insertImage(context.getContentResolver(), file.getAbsolutePath(), fileName, null);

            //保存图片后发送广播通知更新数据库
            Uri uri = Uri.fromFile(file);
            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
            if (isSuccess) {
                return true;
            } else {
                return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static Bitmap rotateBitmap(Bitmap bitmap, float degree) {
        Matrix matrix = new Matrix();
        matrix.postRotate(degree);
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
    }

    public static Bitmap scaleBitmap(Bitmap bitmap, float scaleWidth, float scaleHeight) {
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
    }
}
