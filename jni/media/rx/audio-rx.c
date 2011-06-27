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
static char* LOG_TAG = "NDK-audio-rx";

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
	AVCodecContext *pDecodecCtxAudio, *pDecodecCtxAudioAux;
	AVCodec *pDecodecAudio;

	int i, audioStream, outbuf_size, out_size, len, ret;

	AVPacket packet;
	
	jintArray out_buffer_audio;
	static *outbuf, outbuftemp[DATA_SIZE];
	

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
		snprintf(buf, sizeof(buf), "i: %d", i);
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
		if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			audioStream = i;
			break;
		}
	}
	if (audioStream == -1) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Didn't find a audio stream");
		return -4;
	}
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Get a pointer to the codec context for the audio stream");
	// Get a pointer to the codec context for the audio stream
	pDecodecCtxAudioAux = pFormatCtx->streams[audioStream]->codec;
	
	snprintf(buf, sizeof(buf), "pDecodecCtxAudioAux->id: %d", pDecodecCtxAudioAux->codec_id);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "CODEC_ID_AMR_NB: %d", CODEC_ID_AMR_NB);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	
	

	// Find the decoder for the video stream
	if(pDecodecCtxAudioAux->codec_id == CODEC_ID_AMR_NB) {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "AMR-NB");
		pDecodecAudio = avcodec_find_decoder_by_name("libopencore_amrnb");
		//pDecodecCtxAudio = avcodec_alloc_context();
		pDecodecCtxAudio = pDecodecCtxAudioAux;
	}
	else {
		pDecodecAudio = avcodec_find_decoder(pDecodecCtxAudioAux->codec_id);
		pDecodecCtxAudio = pDecodecCtxAudioAux;
	}
		
	
	if (pDecodecAudio == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Unsupported audio codec!");
		return -5; // Codec not found
	}
	
	
	
	// Open audio codec
	if (avcodec_open(pDecodecCtxAudio, pDecodecAudio) < 0)
		return -6; // Could not open codec
	
	snprintf(buf, sizeof(buf), "pDecodecCtxAudio->id: %d", pDecodecCtxAudio->codec_id);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
			snprintf(
					buf,
					sizeof(buf),
					"c->sample_rate: %d; c->Frame Size: %d c->FMT: %d FMT S16 : %d ",
					pDecodecCtxAudio->sample_rate, pDecodecCtxAudio->frame_size, pDecodecCtxAudio->sample_fmt,
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
	out_size = 0;
	i = 0;
	isReceiving = 1;
	outbuf = malloc(DATA_SIZE);
	int outbuf_init = outbuf;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		//Is this a packet from the audio stream?
		if (packet.stream_index == audioStream) {
//			__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "audioStream");
			//Decode audio frame
			outbuf_size = DATA_SIZE;
			len = avcodec_decode_audio3(pDecodecCtxAudio, (int16_t *) outbuftemp, &outbuf_size, &packet);
//			len = avcodec_decode_audio3(pDecodecCtxAudio, (int16_t *) outbuf, &outbuf_size, &packet);
//			snprintf(buf, sizeof(buf), "outbuf_size: %d", outbuf_size);
//			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
			if (len >= 0) {
//				out_buffer_audio = (jbyteArray)(*env)->NewByteArray(env, outbuf_size);
//				(*env)->SetByteArrayRegion(env, out_buffer_audio, 0, outbuf_size, (jbyte *) outbuftemp);//outbuf);
//				(*env)->CallVoidMethod(env, audioPlayer, midAudio, out_buffer_audio, outbuf_size);
//				(*env)->ReleaseByteArrayElements(env, out_buffer_audio, 0, outbuf_size, pSdpString);
//				memmove(outbuf, outbuftemp, outbuf_size);
				//FIXME check outbuf has suficient space
				if (outbuf_size < (DATA_SIZE - out_size)) {
					memcpy(outbuf, outbuftemp, outbuf_size);
				}
				
//				(*env)->SetByteArrayRegion(env, out_buffer_audio, 0, outbuf_size, (jbyte *) outbuf);
//				(*env)->CallVoidMethod(env, audioPlayer, midAudio, out_buffer_audio, outbuf_size);
				
				outbuf += outbuf_size;
				out_size += outbuf_size;
//				i++;
//				__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "181");
				if(out_size > 1000) {
//					__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "183");
					(*env)->SetByteArrayRegion(env, out_buffer_audio, 0, out_size, (jbyte *) outbuf_init);
//					__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "185");
					(*env)->CallVoidMethod(env, audioPlayer, midAudio, out_buffer_audio, out_size);
//					__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "186");
					out_size = 0;
//					i = 0;
					outbuf = outbuf_init;
//					__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "190");
				}

			}
		}
		//Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
		if (isReceiving == 0)
			break;
	}
	free(outbuf);
	
 free:
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	
	//Close the codec
	avcodec_close(pDecodecCtxAudio);
	
	//Close the audio file
	close_context(pFormatCtx);

	return 0;
}


