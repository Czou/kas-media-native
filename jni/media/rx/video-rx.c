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

#include <init-media.h>

#include <jni.h>
#include <pthread.h>
#include <android/log.h>

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#include "sdp-manager.h"


static char buf[256]; //Log
static char* LOG_TAG = "NDK-video-rx";

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int receive = 0;
static int sws_flags = SWS_BICUBIC;



jint
Java_com_kurento_kas_media_rx_MediaRx_stopVideoRx(JNIEnv* env,
				jobject thiz)
{
	pthread_mutex_lock(&mutex);
	receive = 0;
	pthread_mutex_unlock(&mutex);
	return 0;
}


jint
Java_com_kurento_kas_media_rx_MediaRx_startVideoRx(JNIEnv* env, jobject thiz,
				jstring sdp_str, jobject videoPlayer)
{
	
	const char *pSdpString = NULL;

	AVFormatContext *pFormatCtx = NULL;
	AVCodecContext *pDecodecCtxVideo = NULL;
	AVCodec *pDecodecVideo = NULL;
	AVFrame *pFrame, *pFrameRGB;
	
	AVPacket avpkt;
	uint8_t *avpkt_data_init;
	
	jintArray out_buffer_video;
	uint8_t *buffer;
	
	int i, ret, audioStream, videoStream, numBytes, len, got_picture;
	
	struct SwsContext *img_convert_ctx;
	
	
	pSdpString = (*env)->GetStringUTFChars(env, sdp_str, NULL);
	if (pSdpString == NULL) {
		ret = -1; // OutOfMemoryError already thrown
		goto end;
	}

	snprintf(buf, sizeof(buf), "pSdpString: %s", pSdpString);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);

	if( (ret= init_media()) != 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Couldn't init media");
		goto end;
	}

	pthread_mutex_lock(&mutex);
	receive = 1;
	pthread_mutex_unlock(&mutex);
	for(;;) {
		pthread_mutex_lock(&mutex);
		if (!receive) {
			pthread_mutex_unlock(&mutex);
			goto end;
		}
		pthread_mutex_unlock(&mutex);

		// Open video file
		if ( (ret = av_open_input_sdp(&pFormatCtx, pSdpString, NULL)) != 0 ) {
			snprintf(buf, sizeof(buf), "Couldn't process sdp: %d", ret);
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
			av_strerror(ret, buf, sizeof(buf));
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
			ret = -2;
			goto end;
		}
	
		// Retrieve stream information
		if ( (ret = av_find_stream_info(pFormatCtx)) < 0) {
			av_strerror(ret, buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "Couldn't find stream information: %s", buf);
			__android_log_write(ANDROID_LOG_WARN, LOG_TAG, buf);
			close_context(pFormatCtx);
		} else
			break;
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
		ret = -4;
		goto end;
	}

	// Get a pointer to the codec context for the video stream
	pDecodecCtxVideo = pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream
	pDecodecVideo = avcodec_find_decoder(pDecodecCtxVideo->codec_id);
	if (pDecodecVideo == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Unsupported codec!");
		ret = -5; // Codec not found
		goto end;
	}
	
	// Open video codec
	if (avcodec_open(pDecodecCtxVideo, pDecodecVideo) < 0) {
		ret = -6; // Could not open codec
		goto end;
	}
	
	
	
	//STORING THE DATA
	//Allocate video frame
	pFrame = avcodec_alloc_frame();

	//Allocate an AVFrame structure
	pFrameRGB = avcodec_alloc_frame();
	if (pFrameRGB == NULL) {
		ret = -7;
		goto end;
	}
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
		ret = -8;
		goto end;
	}

	//Definir el tamaño de out_buffer
	//Se multiplica por 4 por RGBA*
	out_buffer_video = (jintArray)(*env)->NewIntArray(env, (pDecodecCtxVideo->width * 4 * pDecodecCtxVideo->height));
	
	
	//READING THE DATA
	for(;;) {
		pthread_mutex_lock(&mutex);
		if (!receive) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
		if (av_read_frame(pFormatCtx, &avpkt) >= 0) {
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
					pthread_mutex_lock(&mutex);
					if (!receive) {
						pthread_mutex_unlock(&mutex);
						break;
					}
					//Did we get a video frame?
					if (got_picture) {
						//Convert the image from its native format to RGB
						img_convert_ctx = sws_getContext(pDecodecCtxVideo->width,
								pDecodecCtxVideo->height, pDecodecCtxVideo->pix_fmt,
								pDecodecCtxVideo->width, pDecodecCtxVideo->height, PIX_FMT_RGB32,
								sws_flags, NULL, NULL, NULL);

						if (img_convert_ctx == NULL) {
							ret = -9;
							goto end;
						}

						sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
								pDecodecCtxVideo->height, ((AVPicture*) pFrameRGB)->data,
								((AVPicture*) pFrameRGB)->linesize);
						sws_freeContext(img_convert_ctx);

						(*env)->SetByteArrayRegion(env, out_buffer_video, 0,
								(pDecodecCtxVideo->width * 4 * pDecodecCtxVideo->height),
								(jint *) pFrameRGB->data[0]);

						(*env)->CallVoidMethod(env, videoPlayer, midVideo, out_buffer_video,
								pDecodecCtxVideo->width, pDecodecCtxVideo->height);
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

	ret = 0;

end:
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	//Free the RGB image
	av_free(buffer);
	av_free(pFrameRGB);

	//Free the YUV frame
	av_free(pFrame);
	//Close the codec
	if (pDecodecCtxVideo)
		avcodec_close(pDecodecCtxVideo);

	//Close the video file
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Close the context...");
	close_context(pFormatCtx);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "ok");

	return ret;
}

