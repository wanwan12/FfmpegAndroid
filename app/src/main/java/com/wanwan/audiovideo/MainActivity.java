package com.wanwan.audiovideo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.TextView;

import com.wanwan.ffmpeglib.FfmpegUtils;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private static final String TAG = MainActivity.class.getSimpleName();

    private TextView tvShowMessage;
    private SurfaceView svPlayVideo;

    private FfmpegUtils ffmpegUtils = new FfmpegUtils();

    private HandlerThread mThread;
    private Handler mHandler;
    private SurfaceHolder mSurfaceHolder;
    private boolean mSurfaceReady = false;
    private int playState=  -1;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvShowMessage = findViewById(R.id.tv_show_message);
        svPlayVideo = findViewById(R.id.sv_play_video);
        findViewById(R.id.btn_version).setOnClickListener(this);
        findViewById(R.id.btn_avcodec_type).setOnClickListener(this);
        findViewById(R.id.btn_video_to_yuv).setOnClickListener(this);
        findViewById(R.id.btn_play_video).setOnClickListener(this);
        findViewById(R.id.btn_play_video_filter).setOnClickListener(this);

        mThread = new HandlerThread("MainActivity");
        mThread.start();
        mHandler = new Handler(mThread.getLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 0:
                        String folderurl = Environment.getExternalStorageDirectory().getAbsolutePath();
                        String inputurl = folderurl + "/DCIM/001.mp4";
                        String outputurl = folderurl + "/DCIM/001.yuv";
                        Log.e(TAG, "inputurl = " + inputurl);
                        Log.e(TAG, "outputurl = " + outputurl);
                        ffmpegUtils.decode(inputurl, outputurl);
                        break;
                    case 1:
                        String folderurl2 = Environment.getExternalStorageDirectory().getAbsolutePath();
                        String inputurl2 = folderurl2 + "/DCIM/001.mp4";
                        playState = ffmpegUtils.play(mSurfaceHolder.getSurface(), inputurl2);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                tvShowMessage.append("playVideo :playState = " + playState + "\n");
                            }
                        });
                        break;
                    case 2:
                        String folderurl3 = Environment.getExternalStorageDirectory().getAbsolutePath();
                        String inputurl3 = folderurl3 + "/DCIM/001.mp4";
//                        playState = ffmpegUtils.playAvFilter(mSurfaceHolder.getSurface(), inputurl3);
//                        runOnUiThread(new Runnable() {
//                            @Override
//                            public void run() {
//                                tvShowMessage.append("playVideo :playState = " + playState + "\n");
//                            }
//                        });
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
            case R.id.btn_version:
                int version = ffmpegUtils.avcodecVersion();

                String versionStr = version / 1000000 + "." + version / 1000 % 1000 + "." + version % 1000;

                tvShowMessage.append("avcodecVersion = " + versionStr + "\n");
                break;
            case R.id.btn_avcodec_type:
                String avcodecinfo = ffmpegUtils.avcodecinfo();
                tvShowMessage.append(avcodecinfo + "\n");
                break;
            case R.id.btn_video_to_yuv:
                mHandler.sendEmptyMessage(0);
                break;
            case R.id.btn_play_video:
                if (mSurfaceReady) {
                    mHandler.sendEmptyMessage(1);
                    tvShowMessage.append("playVideo : Start " + "\n");
                } else {
                    tvShowMessage.append("playVideo : Surface no ready " + "\n");
                }
                break;
            case R.id.btn_play_video_filter:
                if (mSurfaceReady) {
                    mHandler.sendEmptyMessage(2);
                    tvShowMessage.append("playVideoFilter : Start " + "\n");
                } else {
                    tvShowMessage.append("playVideoFilter : Surface no ready " + "\n");
                }
                break;
        }
    }
}