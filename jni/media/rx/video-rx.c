/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include <init-media.h>

#include <jni.h>
#include <android/log.h>

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <pthread.h>



static char buf[256]; //Log
static char* LOG_TAG = "NDK-video-rx";

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int isReceiving = 0;
static int sws_flags = SWS_BICUBIC;



jint
Java_com_tikal_android_media_rx_MediaRx_stopVideoRx(JNIEnv* env,
				jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Stop video");
	pthread_mutex_lock(&mutex);
	isReceiving = 0;
	pthread_mutex_unlock(&mutex);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Stop video OK");
	return 0;
}


jint
Java_com_tikal_android_media_rx_MediaRx_startVideoRx(JNIEnv* env, jobject thiz,
				jstring sdp_str, jobject videoPlayer)
{
	
	const char *pSdpString = NULL;

	AVFormatContext *pFormatCtx;
	AVCodecContext *pDecodecCtxVideo;
	AVCodec *pDecodecVideo;
	AVFrame *pFrame, *pFrameRGB;
	
	AVPacket avpkt;
	uint8_t *avpkt_data_init;
	
	jintArray out_buffer_video;
	uint8_t *buffer;
	
	int i, ret, audioStream, videoStream, numBytes, len, got_picture;
	
	struct SwsContext *img_convert_ctx;
	
	

	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Entro en startVideoRx");

	pSdpString = (*env)->GetStringUTFChars(env, sdp_str, NULL);
	if (pSdpString == NULL)
		return -1; // OutOfMemoryError already thrown

	snprintf(buf, sizeof(buf), "pSdpString: %s", pSdpString);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);

	if( (ret= init_media()) != 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Couldn't init media");
		return ret;
	}

	// Open video file
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
	
	// Find the first video stream
	videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	if (videoStream == -1) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Didn't find a video stream");
		return -4;
	}
	
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Get Streams");

	// Get a pointer to the codec context for the video stream
	pDecodecCtxVideo = pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream
	pDecodecVideo = avcodec_find_decoder(pDecodecCtxVideo->codec_id);
	if (pDecodecVideo == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Unsupported codec!");
		return -5; // Codec not found
	}
	
	// Open video codec
	if (avcodec_open(pDecodecCtxVideo, pDecodecVideo) < 0)
		return -6; // Could not open codec
	
	
	
	
	//STORING THE DATA
	//Allocate video frame
	pFrame = avcodec_alloc_frame();

	//Allocate an AVFrame structure
	pFrameRGB = avcodec_alloc_frame();
	if (pFrameRGB == NULL)
		return -7;
		
	// Determine required buffer size and allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB32, pDecodecCtxVideo->width,
			pDecodecCtxVideo->height);
	buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

	//Assign appropriate parts of buffer to image planes in pFrameRGB
	//Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
	avpicture_fill((AVPicture*) pFrameRGB, buffer, PIX_FMT_RGB32,
			pDecodecCtxVideo->width, pDecodecCtxVideo->height);

	snprintf(buf, sizeof(buf), "pDecodecCtxVideo->width: %d", pDecodecCtxVideo->width);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "pDecodecCtxVideo->height: %d", pDecodecCtxVideo->height);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	
	
	
	//Prepare Call to Method Java.
	jclass cls = (*env)->GetObjectClass(env, videoPlayer);
	
	jmethodID midVideo = (*env)->GetMethodID(env, cls, "putVideoFrameRx", "([III)V");
	if (midVideo == 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "putVideoFrameRx([III)V no exist!");
		return -8;
	}

	//Definir el tamaño de out_buffer
	//Se multiplica por 4 por RGBA*
	out_buffer_video = (jintArray)(*env)->NewIntArray(env, (pDecodecCtxVideo->width * 4 * pDecodecCtxVideo->height));
	
	
	//READING THE DATA
	pthread_mutex_lock(&mutex);
	isReceiving = 1;
	pthread_mutex_unlock(&mutex);
	for(;;) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "video-rx incoming read");
		pthread_mutex_lock(&mutex);
		if(!isReceiving) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		if (av_read_frame(pFormatCtx, &avpkt) >= 0) {
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "video-rx read ok");
			avpkt_data_init = avpkt.data;
			//Is this a avpkt from the video stream?
			if (avpkt.stream_index == videoStream) {
	/*
	snprintf(buf, sizeof(buf), "avpkt->pts: %d", avpkt.pts);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "avpkt->dts: %d", avpkt.dts);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	/*snprintf(buf, sizeof(buf), "avpkt->size: %d", avpkt.size);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	*/
				while (avpkt.size > 0) {
					//Decode video frame
					len = avcodec_decode_video2(pDecodecCtxVideo, pFrame, &got_picture, &avpkt);
					if (len < 0) {
						__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Error in video decoding.");
						break;
					}
					//Did we get a video frame?
					if (got_picture) {
						//Convert the image from its native format to RGB
						img_convert_ctx = sws_getContext(pDecodecCtxVideo->width,
								pDecodecCtxVideo->height, pDecodecCtxVideo->pix_fmt,
								pDecodecCtxVideo->width, pDecodecCtxVideo->height, PIX_FMT_RGB32,
								sws_flags, NULL, NULL, NULL);

						if (img_convert_ctx == NULL)
							return -8;

						sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
								pDecodecCtxVideo->height, ((AVPicture*) pFrameRGB)->data,
								((AVPicture*) pFrameRGB)->linesize);

						(*env)->SetByteArrayRegion(env, out_buffer_video, 0,
								(pDecodecCtxVideo->width * 4 * pDecodecCtxVideo->height),
								(jint *) pFrameRGB->data[0]);

						(*env)->CallVoidMethod(env, videoPlayer, midVideo, out_buffer_video,
								pDecodecCtxVideo->width, pDecodecCtxVideo->height);
					}
					avpkt.size -= len;
					avpkt.data += len;
				}
			}

			//Free the packet that was allocated by av_read_frame
			avpkt.data = avpkt_data_init;
			av_free_packet(&avpkt);
		}
		pthread_mutex_unlock(&mutex);
	}
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "video-rx after while");
	
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "222");
	//Free the RGB image
	av_free(buffer);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "224");
	av_free(pFrameRGB);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "226");

	//Free the YUV frame
	av_free(pFrame);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "231");
	//Close the codec
	avcodec_close(pDecodecCtxVideo);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "234");

	//Close the video file
	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Close the context...");
	close_context(pFormatCtx);
	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "ok");

	return 0;
}

