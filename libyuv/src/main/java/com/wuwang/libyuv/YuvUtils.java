package com.wuwang.libyuv;

/**
 * Created by aiya on 2017/7/24.
 */

public class YuvUtils {

    public static native int RgbaToI420(int type,byte[] rgba,int stride,byte[] yuv,
                                         int y_stride,int u_stride,int v_stride,int width,int height);

    public static native int RgbaToI420(int type,byte[] rgba,byte[] yuv,int width,int height);


    public static native int I420ToRgba(int type,byte[] yuv,int y_stride,int u_stride,int v_stride,
                                        byte[] rgba,int stride,int width,int height);

    public static native int I420ToRgba(int type,byte[] yuv,byte[] rgba,int width,int height);

    public static native void I420ToNV21(byte[] yuv420p,byte[] yuv420sp,int width,int height,boolean swapUV);

    public static native void NV21ToI420(byte[] yuv420sp,byte[] yuv420p,int width,int height,boolean swapUV);

    public static native void NV21Scale(byte[] src_data,int width,int height,byte[] out,
                                            int dst_width,int dst_height,int type);

    public static native void I420Scale(byte[] src_data,int width,int height,byte[] out,
                                        int dst_width,int dst_height,int type,boolean swapUV);

    public static native void RgbaScale(byte[] src_data,int width,int height,byte[] out,
                                        int dst_width,int dst_height,int type);

    public static native void NV21ToI420Rotate(byte[] src,int width,int height,byte[] dst,int de,boolean swapUV);


    static {
        System.loadLibrary("DoggyYuv");
    }

}
