#include <assert.h>
#include "libyuv.h"
#include "jni.h"
#include "android/log.h"

#define YUV_UTILS_JAVA "com/wuwang/libyuv/YuvUtils"

#ifdef __cplusplus
extern "C" {
#endif

static int (*rgbaToI420Func[])(const uint8 *,int,uint8 *,int,uint8 *,int ,uint8 *,int,int,int)={
    libyuv::ABGRToI420,libyuv::RGBAToI420,libyuv::ARGBToI420,libyuv::BGRAToI420,
    libyuv::RGB24ToI420,libyuv::RGB565ToI420
};

static int (*i420ToRgbaFunc[])(const uint8 *,int, const uint8 *,int,const uint8 *,int,uint8 *,
            int ,int ,int )={
    libyuv::I420ToABGR,libyuv::I420ToRGBA,libyuv::I420ToARGB,libyuv::I420ToBGRA,
    libyuv::I420ToRGB24,libyuv::I420ToRGB565
};

static void (*rotatePlaneFunc[])(const uint8* src,int src_stride,uint8* dst,int dst_stride,
                                   int width,int height)={
        libyuv::RotatePlane90,libyuv::RotatePlane180,libyuv::RotatePlane270,
};

static void (*rotateUVFunc[])(const uint8* src, int src_stride, uint8* dst_a, int dst_stride_a,
                              uint8* dst_b, int dst_stride_b, int width, int height)={
        libyuv::RotateUV90,libyuv::RotateUV180,libyuv::RotateUV270,
};

int rgbaToI420(JNIEnv * env,jclass clazz,jbyteArray rgba,jint rgba_stride,
                jbyteArray yuv,jint y_stride,jint u_stride,jint v_stride,
                jint width,jint height,
                int (*func)(const uint8 *,int,uint8 *,int,uint8 *,int ,uint8 *,int,int,int)){
    size_t ySize=(size_t) (y_stride * height);
    size_t uSize=(size_t) (u_stride * height >> 1);
    jbyte * rgbaData= env->GetByteArrayElements(rgba,JNI_FALSE);
    jbyte * yuvData=env->GetByteArrayElements(yuv,JNI_FALSE);
    int ret=func((const uint8 *) rgbaData, rgba_stride, (uint8 *) yuvData, y_stride,
                 (uint8 *) (yuvData) + ySize, u_stride, (uint8 *) (yuvData )+ ySize + uSize,
                 v_stride, width, height);
    env->ReleaseByteArrayElements(rgba,rgbaData,JNI_OK);
    env->ReleaseByteArrayElements(yuv,yuvData,JNI_OK);
    return ret;
}

int i420ToRgba(JNIEnv * env,jclass clazz,jbyteArray yuv,jint y_stride,jint u_stride,jint v_stride,
                jbyteArray rgba,jint rgba_stride,jint width,jint height,
                int (*func)(const uint8 *,int, const uint8 *,int,const uint8 *,int,uint8 *,
                int ,int ,int )){
    size_t ySize=(size_t) (y_stride * height);
    size_t uSize=(size_t) (u_stride * height >> 1);
    jbyte * rgbaData= env->GetByteArrayElements(rgba,JNI_FALSE);
    jbyte * yuvData=env->GetByteArrayElements(yuv,JNI_FALSE);
    int ret=func((const uint8 *) yuvData, y_stride, (uint8 *) yuvData + ySize, u_stride,
                 (uint8 *) (yuvData)+ ySize + uSize, v_stride, (uint8 *) (rgbaData),
                 rgba_stride, width, height);
    env->ReleaseByteArrayElements(rgba,rgbaData,JNI_OK);
    env->ReleaseByteArrayElements(yuv,yuvData,JNI_OK);
    return ret;
}

int Jni_I420ToNV21(JNIEnv * env,jclass clazz,jbyteArray yuv420p,jbyteArray yuv420sp,jint width,jint height,jboolean swapUV){
    size_t ySize=(size_t) (width * height);
    size_t uSize=(size_t) (width * height >> 2);
    size_t stride[]={0,uSize};
    jbyte * yuv420pData=env->GetByteArrayElements(yuv420p,JNI_FALSE);
    jbyte * yuv420spData=env->GetByteArrayElements(yuv420sp,JNI_FALSE);
    int ret=libyuv::I420ToNV21((const uint8 *) yuv420pData, width,
                       (const uint8 *) (yuv420pData + ySize + stride[swapUV]), width >> 1,
                       (const uint8 *) (yuv420pData + ySize + stride[1-swapUV]), width >> 1,
                       (uint8 *) yuv420spData, width, (uint8 *) (yuv420spData + ySize), width, width, height);
    env->ReleaseByteArrayElements(yuv420p,yuv420pData,JNI_OK);
    env->ReleaseByteArrayElements(yuv420sp,yuv420spData,JNI_OK);
    return ret;
}

int Jni_NV21ToI420(JNIEnv * env,jclass clazz,jbyteArray yuv420p,jbyteArray yuv420sp,jint width,jint height,jboolean swapUV){
    size_t ySize=(size_t) (width * height);
    size_t uSize=(size_t) (width * height >> 2);
    size_t stride[]={0,uSize};
    jbyte * yuv420pData=env->GetByteArrayElements(yuv420p,JNI_FALSE);
    jbyte * yuv420spData=env->GetByteArrayElements(yuv420sp,JNI_FALSE);
    int ret=libyuv::NV21ToI420((const uint8 *) yuv420spData, width, (const uint8 *) (yuv420spData + ySize), width,
                               (uint8 *) yuv420pData, width,
                               (uint8 *) (yuv420pData + ySize + stride[swapUV]), width >> 1,
                               (uint8 *) (yuv420pData + ySize + stride[1-swapUV]), width >> 1, width, height);
    env->ReleaseByteArrayElements(yuv420p,yuv420pData,JNI_OK);
    env->ReleaseByteArrayElements(yuv420sp,yuv420spData,JNI_OK);
    return ret;
}

void Jni_NV21Scale(JNIEnv * env,jclass clazz,jbyteArray src,jint width,jint height,jbyteArray dst,jint dst_width,jint dst_height,int mode){
    size_t ySize=(size_t) (width * height);
    size_t dstYSize=(size_t) (dst_width*dst_height);
    jbyte * srcData=env->GetByteArrayElements(src,JNI_FALSE);
    jbyte * dstData=env->GetByteArrayElements(dst,JNI_FALSE);
    libyuv::ScalePlane((const uint8 *) srcData, width, width, height,
                       (uint8 *) dstData, dst_width, dst_width, dst_height,(libyuv::FilterMode) mode);
    libyuv::ScalePlane((const uint8 *)(srcData+ySize) , width, width, height>>1,
                       (uint8 *)(dstData+dstYSize), dst_width, dst_width, dst_height>>1,
                       (libyuv::FilterMode) mode);
    env->ReleaseByteArrayElements(src,srcData,JNI_OK);
    env->ReleaseByteArrayElements(dst,dstData,JNI_OK);
}

void Jni_I420Scale(JNIEnv * env,jclass clazz,jbyteArray src,jint width,jint height,jbyteArray dst,jint dst_width,jint dst_height,int mode,jboolean swapUV){
    int ySize=width * height;
    int swap[]={0,ySize>>2};
    size_t dstYSize=(size_t) (dst_width*dst_height);
    jbyte * srcData=env->GetByteArrayElements(src,JNI_FALSE);
    jbyte * dstData=env->GetByteArrayElements(dst,JNI_FALSE);
    libyuv::I420Scale((const uint8 *) srcData, width, (const uint8 *) (srcData + ySize), width >> 1,
                      (uint8 *)(srcData + ySize + (ySize >> 2)), width >> 1, width, height,
                      (uint8 *) dstData, dst_width, (uint8 *) (dstData + dstYSize+swap[swapUV]), dst_width >> 1,
                      (uint8 *) (dstData + dstYSize + swap[1-swapUV]), dst_width >> 1,
                      dst_width, dst_height, (libyuv::FilterMode) mode);
    env->ReleaseByteArrayElements(src,srcData,JNI_OK);
    env->ReleaseByteArrayElements(dst,dstData,JNI_OK);
}

void Jni_RgbaScale(JNIEnv * env,jclass clazz,jint type,jbyteArray src,jint src_width,jint src_height,jbyteArray dst,jint dst_width,jint dst_height,jint mode){
    int bytes=(type&0x0F000000)>>24;
    jbyte * srcData=env->GetByteArrayElements(src,JNI_FALSE);
    jbyte * dstData=env->GetByteArrayElements(dst,JNI_FALSE);
    libyuv::ARGBScale((const uint8 *) srcData, bytes * src_width, src_width, src_height,
                      (uint8 *) dstData, bytes*dst_width, dst_width, dst_height,
                      (libyuv::FilterMode) mode);
    env->ReleaseByteArrayElements(src,srcData,JNI_OK);
    env->ReleaseByteArrayElements(dst,dstData,JNI_OK);
}

void Jni_NV21ToI420Rotate(JNIEnv * env,jclass clazz,jbyteArray src,jint width,jint height,jbyteArray dst,jint de,jboolean swapUV){
    int dst_stride[]={height,width,height};
    size_t ySize=(size_t) (width * height);
    size_t swap[]={0,ySize>>2};
    jbyte * srcData=env->GetByteArrayElements(src,JNI_FALSE);
    jbyte * dstData=env->GetByteArrayElements(dst,JNI_FALSE);
    rotatePlaneFunc[de]((const uint8 *) srcData, width, (uint8 *) dstData, dst_stride[de], width, height);
    rotateUVFunc[de]((const uint8 *) (srcData+ySize), width, (uint8 *)(dstData+ySize+swap[swapUV]), dst_stride[de]>>1,
                     (uint8*)(dstData+ySize+swap[1-swapUV]),dst_stride[de]>>1,width>>1, height>>1);
    env->ReleaseByteArrayElements(src,srcData,JNI_OK);
    env->ReleaseByteArrayElements(dst,dstData,JNI_OK);
}

int Jni_RgbaToI420WithStride(JNIEnv * env,jclass clazz,jint type,jbyteArray rgba,jint rgba_stride,
                   jbyteArray yuv,jint y_stride,jint u_stride,jint v_stride,
                   jint width,jint height){
    uint8 cType= (uint8) (type & 0x0F);
    return rgbaToI420(env,clazz,rgba,rgba_stride,yuv,y_stride,u_stride,v_stride,width,height,rgbaToI420Func[cType]);
}

int Jni_RgbaToI420(JNIEnv * env,jclass clazz,jint type,jbyteArray rgba,jbyteArray yuv,jint width,jint height){
    uint8 cType=(uint8) (type & 0x0F);
    int rgba_stride= ((type & 0xF0) >> 4)*width;
    int y_stride=width;
    int u_stride=width>>1;
    int v_stride=u_stride;
    return rgbaToI420(env,clazz,rgba,rgba_stride,yuv,y_stride,u_stride,v_stride,width,height,rgbaToI420Func[cType]);
}

int Jni_I420ToRgbaWithStride(JNIEnv * env,jclass clazz,jint type,jbyteArray yuv,jint y_stride,jint u_stride,jint v_stride,
                     jbyteArray rgba,jint rgba_stride,
                     jint width,jint height){
    uint8 cType=(uint8) (type & 0x0F);
    return i420ToRgba(env,clazz,yuv,y_stride,u_stride,v_stride,rgba,rgba_stride,width,height,i420ToRgbaFunc[cType]);
}

int Jni_I420ToRgba(JNIEnv * env,jclass clazz,jint type,jbyteArray yuv,jbyteArray rgba,jint width,jint height){
    uint8 cType=(uint8) (type & 0x0F);
    int rgba_stride= ((type & 0xF0) >> 4)*width;
    int y_stride=width;
    int u_stride=width>>1;
    int v_stride=u_stride;
    return i420ToRgba(env,clazz,yuv,y_stride,u_stride,v_stride,rgba,rgba_stride,width,height,i420ToRgbaFunc[cType]);
}


//libyuv中，rgba表示abgrabgrabgr这样的顺序写入文件，java使用的时候习惯rgba表示rgbargbargba写入文件
static JNINativeMethod g_methods[]={
        {"RgbaToI420","(I[BI[BIIIII)I",   (void *)Jni_RgbaToI420WithStride},
        {"RgbaToI420","(I[B[BII)I",   (void *)Jni_RgbaToI420},

        {"I420ToRgba","(I[BIII[BIII)I",   (void *)Jni_I420ToRgbaWithStride},
        {"I420ToRgba", "(I[B[BII)I",   (void *)Jni_I420ToRgba},

        {"I420ToNV21","([B[BIIZ)V",(void *)Jni_I420ToNV21},
        {"NV21ToI420","([B[BIIZ)V",(void *)Jni_NV21ToI420},
        {"NV21Scale","([BII[BIII)V",(void *)Jni_NV21Scale},
        {"I420Scale","([BII[BIIIZ)V",(void *)Jni_I420Scale},
        {"RgbaScale","([BII[BIII)V",(void *)Jni_RgbaScale},
        {"NV21ToI420Rotate","([BII[BIZ)V",(void *)Jni_NV21ToI420Rotate},
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv* env = nullptr;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    jclass clazz=env->FindClass(YUV_UTILS_JAVA);
    env->RegisterNatives(clazz, g_methods, (int) (sizeof(g_methods) / sizeof((g_methods)[0])));

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *jvm, void *reserved){
    JNIEnv* env = nullptr;

    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    jclass clazz=env->FindClass(YUV_UTILS_JAVA);
    env->UnregisterNatives(clazz);
}


#ifdef __cplusplus
}
#endif





