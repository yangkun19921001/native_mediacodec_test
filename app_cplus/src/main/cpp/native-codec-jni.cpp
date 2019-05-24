/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This is a JNI example where we use native methods to play video
 * using the native AMedia* APIs.
 * See the corresponding Java source file located at:
 *
 *   src/com/example/nativecodec/NativeMedia.java
 *
 * In this example we use assert() for "impossible" error conditions,
 * and explicit handling and recovery for more likely error conditions.
 */

#include <assert.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
#include <android/log.h>
#define TAG "NativeCodec"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern int main_video(void);
extern volatile int g_total_decode_frames;
extern volatile int g_total_decode_frames_ok;

extern "C" {

jboolean Java_com_example_nativecodec_NativeCodec_createStreamingMediaPlayer(JNIEnv* env,
                                                                             jclass clazz, jobject assetMgr, jstring filename)
{
    main_video();

    return JNI_TRUE;
}

// set the playing state for the streaming media player
void Java_com_example_nativecodec_NativeCodec_setPlayingStreamingMediaPlayer(JNIEnv* env,
                                                                             jclass clazz, jboolean isPlaying)
{

}


// shut down the native media system
void Java_com_example_nativecodec_NativeCodec_shutdown(JNIEnv* env, jclass clazz)
{

}


// set the surface
void Java_com_example_nativecodec_NativeCodec_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{

}


// rewind the streaming media player
void Java_com_example_nativecodec_NativeCodec_rewindStreamingMediaPlayer(JNIEnv *env, jclass clazz)
{


}

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_nativecodec_NativeCodec_getDecodeFrameCount(JNIEnv *env, jclass type) {

    return g_total_decode_frames;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_nativecodec_NativeCodec_getDecodeFrameCountOK(JNIEnv *env, jclass type) {

    return g_total_decode_frames_ok;
}