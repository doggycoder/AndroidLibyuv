package com.wuwang.libyuv;

/**
 * Created by aiya on 2017/7/27.
 */

public final class Key {

    private Key(){};

    //0-3 表示转换类型
    //4-7 表示rgba_stride的宽度的倍数
    //8-11 表示yuv_stride宽度移位数
    //12-15 表示uv左移位数

    public static final int RGBA_TO_I420=0x01001040;
    public static final int ABGR_TO_I420=0x01001041;
    public static final int BGRA_TO_I420=0x01001042;
    public static final int ARGB_TO_I420=0x01001043;
    public static final int RGB24_TO_I420=0x01001034;
    public static final int RGB565_TO_I420=0x01001025;

    public static final int I420_TO_RGBA=0x02001040;
    public static final int I420_TO_ABGR=0x02001041;
    public static final int I420_TO_BGRA=0x02001042;
    public static final int I420_TO_ARGB=0x02001043;
    public static final int I420_TO_RGB24=0x02001034;
    public static final int I420_TO_RGB565=0x02001025;

    public static final int SCALE_MODE_NONE=0;
    public static final int SCALE_MODE_LINEAR=1;
    public static final int SCALE_MODE_BI_LINEAR=2;
    public static final int SCALE_MODE_BOX=3;

    public static final int ROTATE_90=0;
    public static final int ROTATE_180=1;
    public static final int ROTATE_270=2;

    //类型
    //低16位分别表示ABGR所在的位置
    //28-31表示类型分类
    //24-27表示字节数
    public static final int TYPE_RGBA=0x14003210;
    public static final int TYPE_RGB=0x13002100;
    public static final int TYPE_ARGB=0x14002103;

}
