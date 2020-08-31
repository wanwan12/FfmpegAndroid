package com.wanwan.ffmpeglib;

public class Examples {
    static {
        System.loadLibrary("ffmpeg-examples");
    }

    public native int aviolist(String inputurl);

    public native int play(Object surface,String inputurl);

    public native int filterplay(Object surface,String inputurl,String filterStr);
}
