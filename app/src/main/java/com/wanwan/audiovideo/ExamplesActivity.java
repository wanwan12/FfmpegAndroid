package com.wanwan.audiovideo;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.wanwan.ffmpeglib.Examples;

public class ExamplesActivity extends AppCompatActivity implements View.OnClickListener {
    private static final String TAG = ExamplesActivity.class.getSimpleName();
    private static final String FILTER_DESC1 = "lutyuv='u=128:v=128'";//黑白
    private static final String FILTER_DESC2 = "hflip";//镜像
    private static final String FILTER_DESC3 = "hue='h=60:s=-3'";
    private static final String FILTER_DESC4 = "crop=2/3*in_w:2/3*in_h";
    private static final String FILTER_DESC5 = "drawbox=x=200:y=200:w=300:h=300:color=pink@0.5";//画300*300的框
    private static final String FILTER_DESC6 = "movie='duanwu_03.jpg'[wm];[in][wm]overlay=5:5[out]";
    private static final String FILTER_DESC7 = "drawgrid=width=100:height=100:thickness=4:color=pink@0.9";

    private TextView tvShowMessage;
    private SurfaceView svPlayVideo;

    private Examples examples = new Examples();

    private HandlerThread mThread;
    private Handler mHandler;
    private SurfaceHolder mSurfaceHolder;
    private boolean mSurfaceReady = false;
    private int playState = -1;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_filter);
        tvShowMessage = findViewById(R.id.tv_show_message);
        svPlayVideo = findViewById(R.id.sv_play_video);
        findViewById(R.id.btn_play_video).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter1).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter2).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter3).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter4).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter5).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter6).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter7).setOnClickListener(this);


        mThread = new HandlerThread("ExamplesActivity");
        mThread.start();
        mHandler = new Handler(mThread.getLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                String folderurl = Environment.getExternalStorageDirectory().getAbsolutePath();
                String inputurl = folderurl + "/DCIM/sangenhuijia.mp4";
                switch (msg.what) {
                    case 0:
                        playState = examples.play(mSurfaceHolder.getSurface(), inputurl);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("playVideo :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 1:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC1);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay1 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 2:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC2);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay2 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 3:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC3);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay3 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 4:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC4);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay4 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 5:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC5);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay5 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 6:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC6);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay6 :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 7:
                        playState = examples.filterplay(mSurfaceHolder.getSurface(), inputurl,FILTER_DESC7);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("filterplay7 :playState = " + playState + "\n");
                            }
                        });
                        break;
                }

            }
        };
        mSurfaceHolder = svPlayVideo.getHolder();
        mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                mSurfaceReady = true;
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                mSurfaceReady = true;
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                mSurfaceReady = false;
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_play_video:
                mHandler.sendEmptyMessage(0);
                break;
            case R.id.btn_play_video_filter1:
                mHandler.sendEmptyMessage(1);
                break;
            case R.id.btn_play_video_filter2:
                mHandler.sendEmptyMessage(2);
                break;
            case R.id.btn_play_video_filter3:
                mHandler.sendEmptyMessage(4);
                break;
            case R.id.btn_play_video_filter4:
                mHandler.sendEmptyMessage(4);
                break;
            case R.id.btn_play_video_filter5:
                mHandler.sendEmptyMessage(5);
                break;
            case R.id.btn_play_video_filter6:
                mHandler.sendEmptyMessage(6);
                break;
            case R.id.btn_play_video_filter7:
                mHandler.sendEmptyMessage(7);
                break;

        }
    }
}