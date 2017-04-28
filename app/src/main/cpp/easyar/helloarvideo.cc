/**
* Copyright (c) 2015-2016 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
* EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
* and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
*/

#include "ar.hpp"
#include "renderer.hpp"
#include <jni.h>
#include <GLES2/gl2.h>

#define JNIFUNCTION_NATIVE(sig) Java_cn_easyar_samples_helloarvideo_MainActivity_##sig

extern "C" {
    JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv* env, jobject object, jint w, jint h));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv* env, jobject obj));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv* env, jobject obj, jboolean portrait));
};

typedef struct context {
    JavaVM *javaVM;
    jclass mainActivityClz;
    jobject mainActivityObj;
    bool isPlaying;
} Context;
Context g_ctx;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    memset(&g_ctx, 0, sizeof(g_ctx));

    g_ctx.javaVM = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

namespace EasyAR {
namespace samples {

class HelloARVideo : public AR
{
public:
    HelloARVideo();
    ~HelloARVideo();
    virtual void initGL();
    virtual void resizeGL(int width, int height);
    virtual void render();
    virtual bool clear();
private:
    Vec2I view_size;
    VideoRenderer* renderer[3];
    int tracked_target;
    int active_target;
    int texid[3];
    ARVideo* video;
    VideoRenderer* video_renderer;
};

HelloARVideo::HelloARVideo()
{
    view_size[0] = -1;
    tracked_target = 0;
    active_target = 0;
    for(int i = 0; i < 3; ++i) {
        texid[i] = 0;
        renderer[i] = new VideoRenderer;
    }
    video = NULL;
    video_renderer = NULL;
}

HelloARVideo::~HelloARVideo()
{
    for(int i = 0; i < 3; ++i) {
        delete renderer[i];
    }
}

void HelloARVideo::initGL()
{
    augmenter_ = Augmenter();
    augmenter_.attachCamera(camera_);
    for(int i = 0; i < 3; ++i) {
        renderer[i]->init();
        texid[i] = renderer[i]->texId();
    }
}

void HelloARVideo::resizeGL(int width, int height)
{
    view_size = Vec2I(width, height);
}

void HelloARVideo::render()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Frame frame = augmenter_.newFrame();
    if(view_size[0] > 0){
        AR::resizeGL(view_size[0], view_size[1]);
        if(camera_ && camera_.isOpened())
            view_size[0] = -1;
    }
    augmenter_.setViewPort(viewport_);
    augmenter_.drawVideoBackground();
    glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);

    AugmentedTarget::Status status = frame.targets()[0].status();
    if(status == AugmentedTarget::kTargetStatusTracked){
        int id = frame.targets()[0].target().id();
        if(active_target && active_target != id) {
            video->onLost();
            delete video;
            video = NULL;
            tracked_target = 0;
            active_target = 0;
        }
        if (!tracked_target) {
            if (video == NULL) {
                if(frame.targets()[0].target().name() == std::string("argame") && texid[0]) {
                    video = new ARVideo;
                    video->openVideoFile("video.mp4", texid[0]);
                    video_renderer = renderer[0];
                }
                else if(frame.targets()[0].target().name() == std::string("namecard") && texid[1]) {
                    video = new ARVideo;
                    video->openTransparentVideoFile("transparentvideo.mp4", texid[1]);
                    video_renderer = renderer[1];
                }
                else if(frame.targets()[0].target().name() == std::string("idback") && texid[2]) {
                    video = new ARVideo;
                    video->openStreamingVideo("http://7xl1ve.com5.z0.glb.clouddn.com/sdkvideo/EasyARSDKShow201520.mp4", texid[2]);
                    video_renderer = renderer[2];
                }
                else if(frame.targets()[0].target().name() == std::string("playMp3_1")) {
                    // TODO 有时播放报错，应该是变量不同步，需要加锁。
//                    04-28 19:59:36.527 5439-5439/cn.easyar.samples.helloarvideo W/MediaPlayer: mediaplayer went away with unhandled events
//                    04-28 19:59:36.527 5439-5439/cn.easyar.samples.helloarvideo W/MediaPlayer: mediaplayer went away with unhandled events
//                    04-28 19:59:36.528 5439-5718/cn.easyar.samples.helloarvideo E/Surface: getSlotFromBufferLocked: unknown buffer: 0xb851d910
//
//                    [ 04-28 19:59:36.537  3798: 5712 I/         ]
//                    [CAMERA]ispv3_dynamic_fps_thread, cur_expo:0x77c, target_vts:0x12c2
//
//
//                    [ 04-28 19:59:36.538  3798: 5712 I/         ]
//                    [CAMERA]ispv3_fps_set_vts, flow:0, vts:0x12c2, fps:20
//                    04-28 19:59:36.540 5439-5718/cn.easyar.samples.helloarvideo E/AndroidRuntime: FATAL EXCEPTION: GLThread 10412
//                    Process: cn.easyar.samples.helloarvideo, PID: 5439
//                    java.lang.RuntimeException: failure code: -38
//                    at android.media.MediaPlayer.invoke(MediaPlayer.java:711)
//                    at android.media.MediaPlayer.getInbandTrackInfo(MediaPlayer.java:2143)
//                    at android.media.MediaPlayer.populateInbandTracks(MediaPlayer.java:2356)
//                    at android.media.MediaPlayer.scanInternalSubtitleTracks(MediaPlayer.java:2348)
//                    at android.media.MediaPlayer.prepare(MediaPlayer.java:1194)
//                    at io.weichao.util.MediaUtil.playMp3_1(MediaUtil.java:52)
//                    at cn.easyar.samples.helloarvideo.MainActivity.playMP3_1(MainActivity.java:112)
//                    at cn.easyar.samples.helloarvideo.MainActivity.nativeRender(Native Method)
//                    at cn.easyar.samples.helloarvideo.Renderer.onDrawFrame(Renderer.java:25)
//                    at android.opengl.GLSurfaceView$GLThread.guardedRun(GLSurfaceView.java:1544)
//                    at android.opengl.GLSurfaceView$GLThread.run(GLSurfaceView.java:1249)
                    if (!g_ctx.isPlaying) {
                        g_ctx.isPlaying = true;
                        int status;
                        JNIEnv *env = NULL;
                        status = g_ctx.javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
                        if (status < 0) {
                            status = g_ctx.javaVM->AttachCurrentThread(&env, NULL);
                            if (status < 0) {
                                env = NULL;
                            }
                        }
                        jclass clazz = g_ctx.mainActivityClz;
                        jobject obj = g_ctx.mainActivityObj;
                        jmethodID m_id = env->GetMethodID(clazz, "playMP3_1", "()V");
                        env->CallVoidMethod(obj, m_id);
                    }
                }
            }
            if (video) {
                video->onFound();
                tracked_target = id;
                active_target = id;
            }
        }
        Matrix44F projectionMatrix = getProjectionGL(camera_.cameraCalibration(), 0.2f, 500.f);
        Matrix44F cameraview = getPoseGL(frame.targets()[0].pose());
        ImageTarget target = frame.targets()[0].target().cast_dynamic<ImageTarget>();
        if(tracked_target) {
            video->update();
            video_renderer->render(projectionMatrix, cameraview, target.size());
        }
    } else {
        if (tracked_target) {
            video->onLost();
            tracked_target = 0;
        }
        if (g_ctx.isPlaying) {
            g_ctx.isPlaying = false;
            int status;
            JNIEnv *env = NULL;
            status = g_ctx.javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
            if (status < 0) {
                status = g_ctx.javaVM->AttachCurrentThread(&env, NULL);
                if (status < 0) {
                    env = NULL;
                }
            }
            jclass clazz = g_ctx.mainActivityClz;
            jobject obj = g_ctx.mainActivityObj;
            jmethodID m_id = env->GetMethodID(clazz, "stopMP3_1", "()V");
            env->CallVoidMethod(obj, m_id);
        }
    }
}

bool HelloARVideo::clear()
{
    AR::clear();
    if(video){
        delete video;
        video = NULL;
        tracked_target = 0;
        active_target = 0;
    }
    return true;
}

}
}
EasyAR::samples::HelloARVideo ar;

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv* env, jobject instance))
{
    jclass clz = env->GetObjectClass(instance);
    g_ctx.mainActivityClz = (jclass)env->NewGlobalRef(clz);
    g_ctx.mainActivityObj = env->NewGlobalRef(instance);
    g_ctx.isPlaying = false;

    bool status = ar.initCamera();
    ar.loadAllFromJsonFile("targets.json");
    ar.loadFromImage("namecard.jpg");
    status &= ar.start();
    return status;
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv* env, jobject))
{
    ar.clear();

    env->DeleteGlobalRef(g_ctx.mainActivityClz);
    env->DeleteGlobalRef(g_ctx.mainActivityObj);
    g_ctx.mainActivityObj = NULL;
    g_ctx.mainActivityClz = NULL;
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv*, jobject))
{
    ar.initGL();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv*, jobject, jint w, jint h))
{
    ar.resizeGL(w, h);
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv*, jobject))
{
    ar.render();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv*, jobject, jboolean portrait))
{
    ar.setPortrait(portrait);
}
