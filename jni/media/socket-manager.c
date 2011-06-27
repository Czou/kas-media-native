/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include "socket-manager.h"
#include <jni.h>
#include <android/log.h>
#include <pthread.h>

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
	if(pAudioFormatCtx && pAudioFormatCtx->pb && (urlContext==pAudioFormatCtx->pb->opaque) && (--nAudio==0)) {
		av_free(pAudioFormatCtx);
		pAudioFormatCtx = NULL;
	}
	if(pVideoFormatCtx && pVideoFormatCtx->pb && (urlContext==pVideoFormatCtx->pb->opaque) && (--nVideo==0)) {
		av_free(pVideoFormatCtx);
		pVideoFormatCtx = NULL;
	}
	urlContext = NULL;
	pthread_mutex_unlock(&mutex);
}


void
close_context(AVFormatContext *s)
{
	RTSPState *rt;
	RTSPStream *rtsp_st;
	int i;
	
	//if is output
	if (s->oformat) {
		free_connection(s->pb->opaque);
		av_free(s);
	}
	
	//if is input
	if (s->iformat) {
		rt = s->priv_data;
		for (i = 0; i < rt->nb_rtsp_streams; i++) {
			rtsp_st = rt->rtsp_streams[i];
			free_connection( rtsp_st->rtp_handle );
		}
		av_close_input_file(s);	
	}
}


URLContext*
get_connection_by_local_port(int local_port)
{
	URLContext *urlContext = NULL;
	
	pthread_mutex_lock(&mutex);
	
	if (pVideoFormatCtx &&(rtp_get_local_rtp_port(pVideoFormatCtx->pb->opaque) == local_port) )
		urlContext = pVideoFormatCtx->pb->opaque;
	else if (pAudioFormatCtx && (rtp_get_local_rtp_port(pAudioFormatCtx->pb->opaque) == local_port) )
		urlContext = pAudioFormatCtx->pb->opaque;
		
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
Java_com_tikal_android_media_MediaPortManager_takeAudioLocalPort (JNIEnv* env, jobject thiz)
{
	URLContext *urlContext = get_audio_connection();
	return rtp_get_local_rtp_port(urlContext);
}




//VIDEO

URLContext*
get_video_connection()
{
	return get_connection(VIDEO);
}

jint
Java_com_tikal_android_media_MediaPortManager_takeVideoLocalPort (JNIEnv* env, jobject thiz)
{
	URLContext *urlContext = get_video_connection();
	return rtp_get_local_rtp_port(urlContext);
}


