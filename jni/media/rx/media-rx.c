/*
 * Kurento Android Media: Android Media Library based on FFmpeg.
 * Copyright (C) 2011  Tikal Technologies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <util/log.h>
#include <init-log.h>
#include <jni.h>
#include <video-rx.h>
#include <audio-rx.h>

static char* LOG_TAG = "NDK-media-rx";

static jobject video_receiver;
static jclass VideoFrame_class;
static jmethodID video_mid, VideoFrame_init_mid;
static JNIEnv* video_env;

enum {
	QUEUE_SIZE = 5, // FIXME: Coupled with VideoRecorderComponent
	ANDROID_PIX_FMT = PIX_FMT_RGB32,
};

static DecodedFrame adf[QUEUE_SIZE+1];

static int buffer_nbytes, n_frame;
static int current_width, current_height;

static jobject
create_videoframe_obj(JNIEnv *env, DecodedFrame *df)
{
	return (*env)->NewObject(env, VideoFrame_class, VideoFrame_init_mid,
				(jintArray)df->priv_data, df->width, df->height,
				df->time_base.num, df->time_base.den,
				df->pts, df->start_time);
}

static void
android_put_video_frame_rx(DecodedFrame *decoded_frame)
{
	jobject df_obj;

	df_obj = create_videoframe_obj(video_env, decoded_frame);
	if (df_obj)
		(*video_env)->CallVoidMethod(video_env, video_receiver,
							video_mid, df_obj);
	(*video_env)->DeleteLocalRef(video_env, df_obj);
}

static void
decoded_frames_fill(int width, int height)
{
	int i, picture_nbytes;

	// Determine required picture size
	picture_nbytes = avpicture_get_size(ANDROID_PIX_FMT, width, height);
	media_log(MEDIA_LOG_INFO, LOG_TAG,
			"buffer_nbytes: %d\tpicture_nbytes: %d",
			buffer_nbytes, picture_nbytes);

	if (picture_nbytes > buffer_nbytes) {
		for (i=0; i<QUEUE_SIZE+1; i++) {
			(*video_env)->DeleteLocalRef(video_env, adf[i].priv_data);
			adf[i].priv_data = (jintArray)(*video_env)->NewIntArray(
					video_env, picture_nbytes/sizeof(jint));
			adf[i].buffer = (uint8_t*)(*video_env)->GetIntArrayElements(
					video_env, adf[i].priv_data, NULL);
			(*video_env)->ReleaseIntArrayElements(video_env,
					adf[i].priv_data, (jint*)(adf[i].buffer), 0);
		}
		buffer_nbytes = picture_nbytes;
	}

	for (i=0; i<QUEUE_SIZE+1; i++) {
		avpicture_fill((AVPicture*) adf[i].pFrameRGB, adf[i].buffer,
						ANDROID_PIX_FMT, width, height);
	}
}

static DecodedFrame*
android_get_decoded_frame(int width, int height)
{
	DecodedFrame *df = NULL;

	if ((current_width != width) || (current_height != height)) {
		decoded_frames_fill(width, height);
		current_width = width;
		current_height = height;
		media_log(MEDIA_LOG_INFO, LOG_TAG,
				"current_width: %d\tcurrent_height: %d",
				current_width, current_height);
	}

	df = &adf[n_frame];
	n_frame = (n_frame+1) % (QUEUE_SIZE+1);

	return df;
}

static void
android_release_decoded_frame(void)
{
	int i;

	for (i=0; i<QUEUE_SIZE+1; i++) {
		(*video_env)->DeleteLocalRef(video_env, adf[i].priv_data);
		av_free(adf[i].pFrameRGB);
	}
}

static FrameManager android_frame_manager = {
	.pix_fmt			= ANDROID_PIX_FMT,
	.put_video_frame_rx		= android_put_video_frame_rx,
	.get_decoded_frame		= android_get_decoded_frame,
	.release_decoded_frame		= android_release_decoded_frame,
};

jint
Java_com_kurento_kas_media_rx_MediaRx_startVideoRx(JNIEnv* env, jclass class,
				jstring sdp, jint maxDelay, jobject videoReceiver)
{
	int i, ret;
	const char *p_sdp = NULL;

	jclass cls = NULL;

	buffer_nbytes = 0;
	n_frame = 0;
	current_width = 0;
	current_height = 0;

	video_mid = NULL;
	VideoFrame_init_mid = NULL;
	video_receiver = NULL;
	VideoFrame_class = NULL;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = (*env)->GetStringUTFChars(env, sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = (*env)->GetObjectClass(env, videoReceiver);
	video_mid = (*env)->GetMethodID(env, cls, "putVideoFrameRx",
				"(Lcom/kurento/kas/media/rx/VideoFrame;)V");
	if (!video_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
		"putVideoFrameRx(Lcom/kurento/kas/media/rx/VideoFrame;)V no exist");
		ret = -2;
		goto end;
	}

	VideoFrame_class = (*env)->FindClass(env, "com/kurento/kas/media/rx/VideoFrame");
	if (!VideoFrame_class) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG,
				"com/kurento/kas/media/rx/VideoFrame not found");
		ret = -3;
		goto end;
	}

	VideoFrame_init_mid = (*env)->GetMethodID(env, VideoFrame_class,
							"<init>", "([IIIIIJJ)V");
	if (!VideoFrame_init_mid) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "init([IIIIIJJ)V not found");
		ret = -4;
		goto end;
	}

	video_env = env;
	video_receiver = videoReceiver;

	//Allocate AVFrames structures
	for (i=0; i<QUEUE_SIZE+1; i++) {
		adf[i].pFrameRGB = avcodec_alloc_frame();
		if (adf[i].pFrameRGB == NULL) {
			ret = -3;
			goto end;
		}
	}

	ret = start_video_rx(p_sdp, maxDelay, &android_frame_manager);

end:
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, video_receiver);
	(*env)->DeleteLocalRef(env, VideoFrame_class);
	(*env)->ReleaseStringUTFChars(env, sdp, p_sdp);

	return ret;
}

jint
Java_com_kurento_kas_media_rx_MediaRx_stopVideoRx(JNIEnv* env, jclass class)
{
	return stop_video_rx();
}

static jobject audio_receiver;
static jmethodID audio_mid;
static JNIEnv* audio_env;

static void
android_put_audio_samples_rx(uint8_t *samples, int size, int nframe)
{
	jbyteArray jbuf = NULL;
	jbuf = (jbyteArray)(*audio_env)->NewByteArray(audio_env, size);
	(*audio_env)->SetByteArrayRegion(audio_env, jbuf, 0, size, (jbyte*)samples);
	(*audio_env)->CallVoidMethod(audio_env, audio_receiver, audio_mid, jbuf, size, nframe);
	(*audio_env)->DeleteLocalRef(audio_env, jbuf);
}

jint
Java_com_kurento_kas_media_rx_MediaRx_startAudioRx(JNIEnv* env, jclass class,
				jstring sdp, jint maxDelay, jobject audioReceiver)
{
	int ret;
	const char *p_sdp = NULL;

	jclass cls;

	if (init_log() != 0)
		media_log(MEDIA_LOG_WARN, LOG_TAG, "Couldn't init android log");

	p_sdp = (*env)->GetStringUTFChars(env, sdp, NULL);
	if (p_sdp == NULL) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "OutOfMemoryError");
		return -1;
	}

	cls = (*env)->GetObjectClass(env, audioReceiver);

	audio_mid = (*env)->GetMethodID(env, cls, "putAudioSamplesRx", "([BII)V");
	if (audio_mid == 0) {
		media_log(MEDIA_LOG_ERROR, LOG_TAG, "putAudioSamplesRx([BII)V no exists");
		ret = -2;
		goto end;
	}
	audio_env = env;
	audio_receiver = audioReceiver;

	ret = start_audio_rx(p_sdp, maxDelay, &android_put_audio_samples_rx);

end:
	(*env)->ReleaseStringUTFChars(env, sdp, p_sdp);

	return ret;
}

jint
Java_com_kurento_kas_media_rx_MediaRx_stopAudioRx(JNIEnv* env, jclass class)
{
	return stop_audio_rx();
}

