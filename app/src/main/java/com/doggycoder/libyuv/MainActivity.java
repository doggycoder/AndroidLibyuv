package com.doggycoder.libyuv;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.Toast;

import com.wuwang.libyuv.Key;
import com.wuwang.libyuv.YuvUtils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity{

    private int width,height;
    private ImageView mBgView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        mBgView= (ImageView) findViewById(R.id.mBg);
    }

    public void onClick(View v){
        switch (v.getId()){
            case R.id.mBtn1:
                try {
                    Bitmap bitmap= BitmapFactory.decodeResource(getResources(),R.mipmap.bg);
                    width=bitmap.getWidth();
                    height=bitmap.getHeight();
                    File file=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache.yuv");
                    OutputStream os = new FileOutputStream(file);
                    ByteBuffer buffer=ByteBuffer.allocate(bitmap.getWidth()*bitmap.getHeight()*4);
                    bitmap.copyPixelsToBuffer(buffer);
                    byte[] yuvData=new byte[bitmap.getWidth()*bitmap.getHeight()*3/2];
                    YuvUtils.RgbaToI420(Key.RGBA_TO_I420,buffer.array(),yuvData,bitmap.getWidth(),bitmap.getHeight());
//                    rgbToYuv(buffer.array(),bitmap.getWidth(),bitmap.getHeight(),yuvData);
                    Log.e("wuwang","width*height:"+bitmap.getWidth()+"/"+bitmap.getHeight());
                    os.write(yuvData);
                    os.flush();
                    os.close();
                    Toast.makeText(getApplicationContext(),"转换完成"+bitmap.getWidth()+"/"+bitmap.getHeight(),Toast.LENGTH_SHORT).show();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case R.id.mBtn2:
                byte[] yuvData=new byte[width*height*3/2];
                byte[] rgbaData=new byte[width*height*4];
                File file=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache.yuv");
                try {
                    FileInputStream fis=new FileInputStream(file);
                    fis.read(yuvData,0,width*height*3/2);
                    YuvUtils.I420ToRgba(Key.I420_TO_RGBA,yuvData,rgbaData,width,height);
                    Bitmap bitmap=Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
                    ByteBuffer buffer=ByteBuffer.allocate(width*height*4);
                    buffer.put(rgbaData);
                    buffer.rewind();
                    bitmap.copyPixelsFromBuffer(buffer);
                    mBgView.setImageBitmap(bitmap);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case R.id.mBtn3:
                yuvToNV21();
                break;
            case R.id.mBtn4:
                scaleYuv420sp();
                break;
            case R.id.mBtn5:
                rotateYuv420sp();
                break;
        }
    }

    public void yuvToNV21(){
        byte[] yuvData=new byte[width*height*3/2];
        byte[] nv21=new byte[width*height*3/2];
        File file=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache.yuv");
        try {
            FileInputStream fis=new FileInputStream(file);
            fis.read(yuvData,0,width*height*3/2);
            //420p转nv21
            YuvUtils.I420ToNV21(yuvData,nv21,width,height,false);
            File file1=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv21.yuv");
            OutputStream os1 = new FileOutputStream(file1);
            os1.write(nv21);
            os1.flush();
            //420p转nv12
            YuvUtils.I420ToNV21(yuvData,nv21,width,height,true);
            File file2=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv12.yuv");
            OutputStream os2 = new FileOutputStream(file2);
            os2.write(nv21);
            os2.flush();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void scaleYuv420sp(){
        byte[] src=new byte[width*height*3/2];
        byte[] dst=new byte[width*height*3/2/5/5];
        File file=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv21.yuv");
        try {
            FileInputStream fis=new FileInputStream(file);
            fis.read(src,0,width*height*3/2);
            //420p转nv21
            YuvUtils.NV21Scale(src,width,height,dst,width/5,height/5,Key.SCALE_MODE_LINEAR);
            File file1=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv21_scale.yuv");
            OutputStream os1 = new FileOutputStream(file1);
            os1.write(dst);
            os1.flush();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void rotateYuv420sp(){
        byte[] src=new byte[width*height*3/2];
        byte[] dst=new byte[width*height*3/2];
        File file=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv21.yuv");
        try {
            FileInputStream fis=new FileInputStream(file);
            fis.read(src,0,width*height*3/2);
            //420p转nv21
            YuvUtils.NV21ToI420Rotate(src,width,height,dst,Key.ROTATE_90,false);
            File file1=new File(getExternalFilesDir(null).getAbsolutePath()+"/cache_nv21_rotate.yuv");
            OutputStream os1 = new FileOutputStream(file1);
            os1.write(dst);
            os1.flush();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void rgbToYuv(byte[] rgba,int width,int height,byte[] yuv) {
        final int frameSize = width * height;

        int yIndex = 0;
        int uIndex = frameSize;
        int vIndex = frameSize + frameSize/4;

        int R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                index = j * width + i;
                if(rgba[index*4]>127||rgba[index*4]<-128){
                    Log.e("color","-->"+rgba[index*4]);
                }
                R = rgba[index*4]&0xFF;
                G = rgba[index*4+1]&0xFF;
                B = rgba[index*4+2]&0xFF;

                Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                yuv[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    yuv[uIndex++] = (byte) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                    yuv[vIndex++] = (byte) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                }
            }
        }
    }
}
