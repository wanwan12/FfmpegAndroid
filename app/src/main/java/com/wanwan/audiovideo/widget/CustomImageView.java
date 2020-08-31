package com.wanwan.audiovideo.widget;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.view.View;

import com.wanwan.audiovideo.R;

public class CustomImageView extends View {

    Paint paint = new Paint();
    Bitmap bitmap;

    public CustomImageView(Context context) {
        super(context);
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.STROKE);
        bitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);  // 获取bitmap
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (bitmap != null) {
            canvas.drawBitmap(bitmap, 0, 0, paint); //绘制
        }
    }
}

