package com.zkp.com.myapplication;

/**
 * Created by zkp on 2017/8/11.
 */

public class PngUtils {

    static {
        System.loadLibrary("pngutils");
    }

    public static native boolean checkIsPng(String fileName);

    public static native String readPngHighLevel(String fileName);

    public static native String lowLevelReadLowLevelWrite(String inFileName,String outFileName);

    public static native String highLevelReadHighLevelWrite(String inFileName,String outFileName);
}
