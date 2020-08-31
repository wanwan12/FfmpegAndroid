package com.wanwan.audiovideo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

public class ShowImgActivity extends AppCompatActivity {
    private static final String TAG = ShowImgActivity.class.getSimpleName();
    private ImageView imgData1, imgData2, imgData3;
    private SurfaceView svData;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_img);
        initView();
        showImg();

    }

    private void initView() {
        imgData1 = findViewById(R.id.img_data1);
        imgData2 = findViewById(R.id.img_data2);
        imgData3 = findViewById(R.id.img_data3);
        svData = findViewById(R.id.sv_data);
    }

    private void showImg() {
        imgData1.setImageResource(R.mipmap.ic_launcher);
        imgData2.setImageDrawable(getResources().getDrawable(R.mipmap.ic_launcher));
        Bitmap bitmap =  BitmapFactory.decodeResource(getResources(),R.mipmap.ic_launcher);
        imgData3.setImageBitmap(bitmap);
        Log.d(TAG,"showImg main threadid = "+Thread.currentThread().getId());
        svData.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.d(TAG,"showImg SurfaceView threadid = "+ Thread.currentThread().getId());
                if (holder == null) {
                    return;
                }

                Paint paint = new Paint();//画笔
                paint.setAntiAlias(true);//设置是否抗锯齿
                paint.setStyle(Paint.Style.STROKE);//设置画笔风格

                Bitmap bitmap = BitmapFactory.decodeResource(getResources(),
                        R.mipmap.ic_launcher); // 获取bitmap【项目资源目录】

                Canvas canvas = holder.lockCanvas();  // 先锁定当前surfaceView的画布
                canvas.drawBitmap(bitmap, 0, 0, paint); //执行绘制操作
                holder.unlockCanvasAndPost(canvas); // 解除锁定并显示在界面上

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

}