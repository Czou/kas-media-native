/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include <init-media.h>

#include <jni.h>
#include <android/log.h>

#include "libavformat/avformat.h"


static char buf[256]; //Log
static char* LOG_TAG = "NDK";

enum {
	DATA_SIZE = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2,
};

static int isReceiving = 0;



jint
Java_com_tikal_android_media_rx_MediaRx_stopAudioRx(JNIEnv* env,
				jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Stop audio");
	isReceiving = 0;
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

	int i, audioStream, frame_size, len, ret;

	AVPacket packet;
	
	jintArray out_buffer_audio;
	static uint8_t audio_buf[DATA_SIZE];
	

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
		//Close the video file
		av_close_input_file(pFormatCtx);
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
	pDecodecAudio = avcodec_find_decoder(pDecodecCtxAudio->codec_id);
	if (pDecodecAudio == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Unsupported audio codec!");
		return -5; // Codec not found
	}
	
	// Open audio codec
	if (avcodec_open(pDecodecCtxAudio, pDecodecAudio) < 0)
		return -6; // Could not open codec
	
	
	
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
	i = 0;
	isReceiving = 1;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		//Is this a packet from the audio stream?
		if (packet.stream_index == audioStream) {
			//Decode audio frame
			frame_size = DATA_SIZE;
			len = avcodec_decode_audio3(pDecodecCtxAudio, (int16_t *) audio_buf, &frame_size, &packet);	
			if (len >= 0) {
				(*env)->SetByteArrayRegion(env, out_buffer_audio, 0, frame_size, (jbyte *) audio_buf);
				(*env)->CallVoidMethod(env, audioPlayer, midAudio, out_buffer_audio, frame_size);
			}
		}
		
		//Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
		if (isReceiving == 0)
			break;
	}
	
 free:
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	
	//Close the codec
	avcodec_close(pDecodecCtxAudio);

	//Close the video file
	av_close_input_file(pFormatCtx);

	return 0;
}


