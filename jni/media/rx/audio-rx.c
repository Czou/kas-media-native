/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include <init-media.h>

#include <jni.h>
#include <pthread.h>
#include <android/log.h>

#include "libavformat/avformat.h"


static char buf[256]; //Log
static char* LOG_TAG = "NDK-audio-rx";

enum {
	DATA_SIZE = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2,
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int receive = 0;



jint
Java_com_tikal_android_media_rx_MediaRx_stopAudioRx(JNIEnv* env,
				jobject thiz)
{
	pthread_mutex_lock(&mutex);
	receive = 0;
	pthread_mutex_unlock(&mutex);
	return 0;
}


jint
Java_com_tikal_android_media_rx_MediaRx_startAudioRx(JNIEnv* env, jobject thiz,
				jstring sdp_str, jobject audioPlayer)
{
	
	const char *pSdpString = NULL;

	AVFormatContext *pFormatCtx;
	AVCodecContext *pDecodecCtxAudio;
	AVCodec *pDecodecAudio;
	
	AVPacket avpkt;
	uint8_t *avpkt_data_init;
	
	jintArray out_buffer_audio;
	uint8_t outbuf[DATA_SIZE];

	int i, ret, audioStream, out_size, len;
	
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Entro en startAudioRx");

	pSdpString = (*env)->GetStringUTFChars(env, sdp_str, NULL);
	if (pSdpString == NULL)
		return -1; // OutOfMemoryError already thrown

	snprintf(buf, sizeof(buf), "pSdpString: \n%s", pSdpString);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	if( (ret= init_media()) != 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Couldn't init media");
		return ret;
	}

	// Open audio file
	if ( (ret = av_open_input_sdp(&pFormatCtx, pSdpString, NULL)) != 0 ) {
		snprintf(buf, sizeof(buf), "Couldn't process sdp: %d", ret);
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
		av_strerror(ret, buf, sizeof(buf));
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
		return -2;
	}
	
	// Retrieve stream information
	if ( (ret = av_find_stream_info(pFormatCtx)) < 0) {
		snprintf(buf, sizeof(buf), "Couldn't find stream information: %d", ret);
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
		close_context(pFormatCtx);
		return -3;
	}
	
	// Find the first audio stream
	audioStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			audioStream = i;
			break;
		}
	}
	if (audioStream == -1) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Didn't find a audio stream");
		return -4;
	}
	
	// Get a pointer to the codec context for the audio stream
	pDecodecCtxAudio = pFormatCtx->streams[audioStream]->codec;
	
	
	

	// Find the decoder for the video stream
	if(pDecodecCtxAudio->codec_id == CODEC_ID_AMR_NB) {
		pDecodecAudio = avcodec_find_decoder_by_name("libopencore_amrnb");
	}
	else {
		pDecodecAudio = avcodec_find_decoder(pDecodecCtxAudio->codec_id);
	}
		
	
	if (pDecodecAudio == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Unsupported audio codec!");
		return -5;
	}
	
snprintf(buf, sizeof(buf), "pDecodecAudio->name: %s", pDecodecAudio->name);
__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	
	// Open audio codec
	if (avcodec_open(pDecodecCtxAudio, pDecodecAudio) < 0)
		return -6; // Could not open codec
	
snprintf(
	buf,
	sizeof(buf),
	"c->sample_rate: %d; c->Frame Size: %d; c->bit_rate: %d; c->FMT: %d; FMT S16 : %d ",
	pDecodecCtxAudio->sample_rate, pDecodecCtxAudio->frame_size,
	pDecodecCtxAudio->bit_rate, pDecodecCtxAudio->sample_fmt,
	SAMPLE_FMT_S16);
__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	
	
	//Prepare Call to Method Java.
	jclass cls = (*env)->GetObjectClass(env, audioPlayer);
	
	jmethodID midAudio = (*env)->GetMethodID(env, cls, "putAudioSamplesRx", "([BI)V");
	if (midAudio == 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "putAudioSamplesRx([BI)V no exist!");
		return -8;
	}
	
	//Definir el tamaño de out_buffer
	out_buffer_audio = (jbyteArray)(*env)->NewByteArray(env, DATA_SIZE);
	
	//READING THE DATA
	pthread_mutex_lock(&mutex);
	receive = 1;
	pthread_mutex_unlock(&mutex);
	for(;;) {
		pthread_mutex_lock(&mutex);
		if (!receive) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);

		if (av_read_frame(pFormatCtx, &avpkt) >= 0) {
			avpkt_data_init = avpkt.data;
			//Is this a avpkt from the audio stream?
			if (avpkt.stream_index == audioStream) {
	/*
	snprintf(buf, sizeof(buf), "avpkt->pts: %d", avpkt.pts);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "avpkt->dts: %d", avpkt.dts);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "avpkt->size: %d", avpkt.size);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	*/
				while (avpkt.size > 0) {
					//Decode audio frame
					out_size = DATA_SIZE;
					len = avcodec_decode_audio3(pDecodecCtxAudio, (int16_t *) outbuf, &out_size, &avpkt);
					if (len < 0) {
						__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Error in audio decoding.");
						break;
					}
					
					pthread_mutex_lock(&mutex);
					if (!receive) {
						pthread_mutex_unlock(&mutex);
						break;
					}
					if (out_size > 0) {
						(*env)->SetByteArrayRegion(env, out_buffer_audio, 0, out_size, (jbyte *) outbuf);
						(*env)->CallVoidMethod(env, audioPlayer, midAudio, out_buffer_audio, out_size);
					}
					pthread_mutex_unlock(&mutex);
					
					avpkt.size -= len;
					avpkt.data += len;
				}
			}
			//Free the packet that was allocated by av_read_frame
			avpkt.data = avpkt_data_init;
			av_free_packet(&avpkt);
		}
	}
	
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	
	//Close the codec
	avcodec_close(pDecodecCtxAudio);
	
	//Close the audio file
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Close the context...");
	close_context(pFormatCtx);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "ok");

	return 0;
}

