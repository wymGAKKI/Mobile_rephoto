package com.example.rephoto;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.Pair;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;

import java.util.List;

public class DrawView extends SurfaceView implements SurfaceHolder.Callback {

    private SurfaceHolder mHolder;
    /** draw */
    private Canvas mCanvas;

    public DrawView(Context context) {
        super(context);
        mHolder = this.getHolder();
        mHolder.addCallback(this);

        /** Set background transparent */
        mHolder.setFormat(PixelFormat.TRANSLUCENT);
    }

    public void getCanvas() {
        mCanvas = mHolder.lockCanvas();
    }


    public void update() {
        if (mCanvas != null) {
            mHolder.unlockCanvasAndPost(mCanvas);
        }
    }

    /** clear all*/
    public void clearDraw() {
        if (mCanvas != null) {
            Paint paint = new Paint();
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
            mCanvas.drawPaint(paint);
        }
    }

    public void drawBitmap(Bitmap bitmap) {
        if (mCanvas != null) {
            Paint paint = new Paint();
            paint.setColor(Color.parseColor("#A0000000"));
            paint.setStrokeWidth(5);
            paint.setStyle(Paint.Style.STROKE);
            mCanvas.drawBitmap(bitmap,
                    new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                    new RectF(0, 0, bitmap.getWidth(), bitmap.getHeight()), null);
        }
    }

    public void drawLines(List<Pair<PointF, PointF>> lines, int color) {
        if (mCanvas != null) {
            Paint paint = new Paint();
            paint.setColor(color);
            paint.setStrokeWidth(10);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
