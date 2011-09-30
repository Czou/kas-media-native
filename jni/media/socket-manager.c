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

/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include "socket-manager.h"

#include <jni.h>
#include <pthread.h>
#include <android/log.h>

#include "libavformat/rtsp.h"


enum {
	AUDIO,
	VIDEO,
};

static char buf[256]; //Log
static char* LOG_TAG = "NDK-socket-manager";

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static AVFormatContext *pAudioFormatCtx;
static int nAudio;

static AVFormatContext *pVideoFormatCtx;
static int nVideo;



static URLContext*
get_connection(int media_type)
{
	URLContext *urlContext = NULL;
	AVFormatContext *s;
	
	if(init_media() !=  0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Couldn't init media");
		return NULL;
	}
	
	pthread_mutex_lock(&mutex);
	
	if (media_type == AUDIO)
		s = pAudioFormatCtx;
	else if(media_type == VIDEO)
		s = pVideoFormatCtx;
	
	if(!s) {
		s = avformat_alloc_context();
		if (!s) {
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Memory error: Could not alloc context");
			s = NULL;
		} else if (avio_open(&s->pb, "rtp://0.0.0.0:0", AVIO_RDWR) < 0) {
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not open 'rtp://0.0.0.0:0'");
			av_free(s);
			s = NULL;
		}
	}
	
	if(s && s->pb) {
		urlContext = s->pb->opaque;
		if (media_type == AUDIO) {
			pAudioFormatCtx = s;
			nAudio++;
		}
		else if (media_type == VIDEO) {
			pVideoFormatCtx = s;
			nVideo++;
		}
	}
		
	pthread_mutex_unlock(&mutex);
	
	return urlContext;
}


static void
free_connection(URLContext *urlContext)
{
	pthread_mutex_lock(&mutex);
	
snprintf(buf, sizeof(buf), "before nAudio: %d", nAudio);
__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
snprintf(buf, sizeof(buf), "before nVideo: %d", nVideo);
__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);

	if(pAudioFormatCtx && pAudioFormatCtx->pb && (urlContext==pAudioFormatCtx->pb->opaque) && (--nAudio==0)) {
snprintf(buf, sizeof(buf), "free pAudioFormatCtx");
		av_free(pAudioFormatCtx);
snprintf(buf, sizeof(buf), "free ok");
		pAudioFormatCtx = NULL;
	}
	if(pVideoFormatCtx && pVideoFormatCtx->pb && (urlContext==pVideoFormatCtx->pb->opaque) && (--nVideo==0)) {
snprintf(buf, sizeof(buf), "free pVideoFormatCtx");
		av_free(pVideoFormatCtx);
snprintf(buf, sizeof(buf), "free ok");
		pVideoFormatCtx = NULL;
	}
	urlContext = NULL;
	
snprintf(buf, sizeof(buf), "after nAudio: %d", nAudio);
__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
snprintf(buf, sizeof(buf), "after nVideo: %d", nVideo);
__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);

	pthread_mutex_unlock(&mutex);
}


void
close_context(AVFormatContext *s)
{
	RTSPState *rt;
	RTSPStream *rtsp_st;
	int i;
	
	if (!s)
		return;
	
	//if is output
	if (s->oformat && s->pb) {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "free output");
		free_connection(s->pb->opaque);
		s->pb->opaque = NULL;
		if (!(s->oformat->flags & AVFMT_NOFILE)) {
			avio_close(s->pb);
		}
		av_free(s);
	}
	
	//if is input
	if (s->iformat && s->priv_data) {
		rt = s->priv_data;
		for (i = 0; i < rt->nb_rtsp_streams; i++) {
			rtsp_st = rt->rtsp_streams[i];
			__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "free input");
			free_connection( rtsp_st->rtp_handle );
			rtsp_st->rtp_handle = NULL;
		}
		av_close_input_file(s);	
	}
}


URLContext*
get_connection_by_local_port(int local_port)
{
	URLContext *urlContext = NULL;
	
	pthread_mutex_lock(&mutex);
	
	if (pVideoFormatCtx &&(rtp_get_local_rtp_port(pVideoFormatCtx->pb->opaque) == local_port) ) {
		urlContext = pVideoFormatCtx->pb->opaque;
		nVideo++;
	}
	else if (pAudioFormatCtx && (rtp_get_local_rtp_port(pAudioFormatCtx->pb->opaque) == local_port) ) {
		urlContext = pAudioFormatCtx->pb->opaque;
		nAudio++;
	}
	
	pthread_mutex_unlock(&mutex);
	return urlContext;
}


//AUDIO

URLContext*
get_audio_connection()
{
	return get_connection(AUDIO);
}

jint
Java_com_kurento_kas_media_ports_MediaPortManager_takeAudioLocalPort (JNIEnv* env, jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "takeAudioLocalPort");
	URLContext *urlContext = get_audio_connection();
	return rtp_get_local_rtp_port(urlContext);
}

void
Java_com_kurento_kas_media_ports_MediaPortManager_releaseAudioLocalPort (JNIEnv* env, jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "releaseAudioLocalPort");
	if (pAudioFormatCtx && pAudioFormatCtx->pb)
		free_connection(pAudioFormatCtx->pb->opaque);
}


//VIDEO

URLContext*
get_video_connection()
{
	return get_connection(VIDEO);
}

jint
Java_com_kurento_kas_media_ports_MediaPortManager_takeVideoLocalPort (JNIEnv* env, jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "takeVideoLocalPort");
	URLContext *urlContext = get_video_connection();
	return rtp_get_local_rtp_port(urlContext);
}

void
Java_com_kurento_kas_media_ports_MediaPortManager_releaseVideoLocalPort (JNIEnv* env, jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "releaseVideoLocalPort");
	if (pVideoFormatCtx && pVideoFormatCtx->pb)
		free_connection(pVideoFormatCtx->pb->opaque);
}

