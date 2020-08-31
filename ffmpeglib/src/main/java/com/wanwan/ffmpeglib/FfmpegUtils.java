package com.wanwan.ffmpeglib;

public class FfmpegUtils {
    static {
        System.loadLibrary("native-lib");
    }

    public native int decode(String inputurl, String outputurl);


    public native String avcodecinfo();

    public native int avcodecVersion();

    public native int play(Object surface,String inputurl);
//
//    public native int playAvFilter(Object surface,String inputurl);
}
