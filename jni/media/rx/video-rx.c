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



static char buf[256]; //Log
static char* LOG_TAG = "NDK";

static int isReceiving = 0;
static int sws_flags = SWS_BICUBIC;



jint
Java_com_tikal_android_media_rx_MediaRx_stopVideoRx(JNIEnv* env,
				jobject thiz)
{
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Stop video");
	isReceiving = 0;
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

	int i, audioStream, videoStream;

	uint8_t *buffer;
	int numBytes;

	int frameFinished;

	int w, h;

	AVPacket packet;

	static struct SwsContext *img_convert_ctx;

	int ret;
	
	jintArray out_buffer_video;
	

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
	

	snprintf(buf, sizeof(buf), "nb_streams: %d", pFormatCtx->nb_streams);
	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
	
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
	i = 0;
	isReceiving = 1;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		//Is this a packet from the video stream?
		if (packet.stream_index == videoStream) {
			//Decode video frame
			avcodec_decode_video(pDecodecCtxVideo, pFrame, &frameFinished, packet.data, packet.size);
			//Did we get a video frame?
			if (frameFinished) {
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
		}

		//Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);
		if (isReceiving == 0)
			break;
	}
	
	
	(*env)->ReleaseStringUTFChars(env, sdp_str, pSdpString);
	
	//Free the RGB image
	av_free(buffer);
	av_free(pFrameRGB);

	//Free the YUV frame
	av_free(pFrame);
	
	//Close the codec
	avcodec_close(pDecodecCtxVideo);

	//Close the video file
	av_close_input_file(pFormatCtx);

	return 0;
}


