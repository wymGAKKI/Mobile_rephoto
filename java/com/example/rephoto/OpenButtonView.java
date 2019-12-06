package com.example.rephoto;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.util.AttributeSet;

import androidx.appcompat.widget.AppCompatButton;

public class OpenButtonView extends AppCompatButton {

    private int mViewWidth;
    private int mViewHeight;

    private int mViewCenterX;
    private int mViewCenterY;

    private int mRadius;

    private PointF mPoint1, mPoint2, mPoint3, mPoint4;

    private Paint mCirclePaint;
    private Paint mSymbolPaint;

    public OpenButtonView(Context context) {
        super(context);
        init();
    }

    public OpenButtonView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public OpenButtonView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    void init() {
        /**抗锯齿*/

        mCirclePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mCirclePaint.setColor(Color.parseColor("#ccaaaaaa"));

        mSymbolPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mSymbolPaint.setColor(Color.WHITE);
        /**空心*/
        mSymbolPaint.setStyle(Paint.Style.STROKE);
        mSymbolPaint.setStrokeCap(Paint.Cap.ROUND);
        mSymbolPaint.setStrokeJoin(Paint.Join.ROUND);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        mViewWidth = measureWidth(widthMeasureSpec);
        mViewHeight = measureHeight(heightMeasureSpec);
        mViewCenterX = mViewWidth / 2;
        mViewCenterY = mViewHeight / 2;
        mRadius = mViewCenterX <= mViewCenterY ? mViewCenterX : mViewCenterY;
        mSymbolPaint.setStrokeWidth(mRadius * 2 / 18);
        setMeasuredDimension(mViewWidth, mViewHeight);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawCircle(mViewCenterX, mViewCenterY, mRadius, mCirclePaint);

        canvas.drawLine(mPoint1.x, mPoint1.y, mPoint2.x, mPoint2.y, mSymbolPaint);
        canvas.drawLine(mPoint3.x, mPoint3.y, mPoint4.x, mPoint4.y, mSymbolPaint);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        if (mViewWidth != 0) {
            mPoint1 = new PointF(mRadius / 2, mRadius);
            mPoint2 = new PointF(mRadius / 2 * 3, mRadius);
            mPoint3 = new PointF(mRadius, mRadius / 2);
            mPoint4 = new PointF(mRadius, mRadius / 2 * 3);
        }
    }

    private int measureHeight(int measureSpec) {
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);
        int result = 500;
        if (specMode == MeasureSpec.AT_MOST) {
            result = specSize;
        } else if (specMode == MeasureSpec.EXACTLY) {
            result = specSize;
        }
        return result;
    }


    private int measureWidth(int measureSpec) {
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);
        int result = 500;
        if (specMode == MeasureSpec.AT_MOST) {
            result = specSize;
        } else if (specMode == MeasureSpec.EXACTLY) {
            result = specSize;
        }
        return result;
    }
}
