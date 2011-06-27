/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include <my-cmdutils.h>
#include <init-media.h>
#include <socket-manager.h>

#include <jni.h>
#include <android/log.h>

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/opt.h"

#include "libavformat/rtpenc.h"


/*
	see	libavformat/output-example.c
		libavcodec/api-example.c
*/

static char buf[256]; //Log
static char* LOG_TAG = "NDK-audio-tx";

static int sws_flags = SWS_BICUBIC;

//Coupled with Java
int AUDIO_CODECS[] = {CODEC_ID_AMR_NB, CODEC_ID_MP2, CODEC_ID_AAC};
char* AUDIO_CODEC_NAMES[] = {"amr_nb", "mp2", "aac"};

static uint8_t *audio_outbuf;
static int audio_outbuf_size;

static AVOutputFormat *fmt;
static AVFormatContext *oc;
static AVStream *audio_st;


////////////////////////////////////////////////////////////////////////////////////////
//INIT AUDIO

static int open_audio(AVFormatContext *oc, AVStream *st)
{
	AVCodec *codec;
	AVCodecContext *c;
	int ret;
	
	c = st->codec;
	
	
	/* find the audio encoder */
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Codec not found");
		return -1;
	}

	/* open the codec */
	if ((ret = avcodec_open(c, codec)) < 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not open codec");
		return ret;
	}
	
	audio_outbuf_size = 100000;
	audio_outbuf = av_malloc(audio_outbuf_size);
	
	return 0;
}


/*
 * add an audio output stream
 */
static AVStream *add_audio_stream(AVFormatContext *oc, enum CodecID codec_id, int sample_rate, int bit_rate)
{
	AVCodecContext *c;
	AVStream *st;
	
	st = av_new_stream(oc, 1);
	if (!st) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not alloc stream");
		return NULL;
	}
	
	c = st->codec;
	c->codec_id = codec_id;
	c->codec_type = AVMEDIA_TYPE_AUDIO;
	
	/* put sample parameters */
	c->bit_rate = bit_rate;//AMR:12200;//MP2:64000
	c->sample_rate = sample_rate;//AMR:8000;//MP2:44100;
	c->channels = 1;
	c->sample_fmt = SAMPLE_FMT_S16;//AV_SAMPLE_FMT_S16;
	
	snprintf(buf, sizeof(buf), "bit_rate: %d", c->bit_rate);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "sample_rate: %d", c->sample_rate);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);

	// some formats want stream headers to be separate
	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return st;
}

jint
Java_com_tikal_android_media_tx_MediaTx_initAudio (JNIEnv* env,
						jobject thiz,
						jstring outfile, jint codec_id, jint sample_rate, jint bit_rate, jint payload_type)
{
	const char *pOutFile = NULL;
	URLContext *urlContext;
	int i, ret;

	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Entro en initAudio");	
	
	pOutFile = (*env)->GetStringUTFChars(env, outfile, NULL);
	if(pOutFile == NULL)
    		return -1; // OutOfMemoryError already thrown

	if( (ret= init_media()) != 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Couldn't init media");
		return ret;
	}
	
	for(i=0; i<AVMEDIA_TYPE_NB; i++){
		avcodec_opts[i]= avcodec_alloc_context2(i);
	}
	avformat_opts = avformat_alloc_context();
	sws_opts = sws_getContext(16,16,0, 16,16,0, sws_flags, NULL,NULL,NULL);
	
	/* auto detect the output format from the name. default is mp4. */
	fmt = av_guess_format(NULL, pOutFile, NULL);
	if (!fmt) {
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Could not deduce output format from file extension: using RTP.");
		fmt = av_guess_format("rtp", NULL, NULL);
	}
	if (!fmt) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not find suitable output format");
		return -1;
	}
	snprintf(buf, sizeof(buf), "Format established: %s", fmt->name);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	//FIXME check 0 <= codec_id <= size(AUDIO_CODECS)
	fmt->audio_codec = AUDIO_CODECS[codec_id];
	snprintf(buf, sizeof(buf), "audio codec_id: %d", codec_id);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	snprintf(buf, sizeof(buf), "Audio Codec stablished: %s", AUDIO_CODEC_NAMES[codec_id]);
	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
	
	/* allocate the output media context */
	oc = avformat_alloc_context();
	if (!oc) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Memory error: Could not alloc context");
		return -2;
	}
	oc->oformat = fmt;
	snprintf(oc->filename, sizeof(oc->filename), "%s", pOutFile);
	
	/* add the audio stream using the default format codecs
	and initialize the codecs */
	audio_st = NULL;
	
	if (fmt->audio_codec != CODEC_ID_NONE) {
		audio_st = add_audio_stream(oc, fmt->audio_codec, sample_rate, bit_rate);
	}
	
	/* set the output parameters (must be done even if no
	parameters). */
	if (av_set_parameters(oc, NULL) < 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Invalid output format parameters");
		return -3;
	}
	
	av_dump_format(oc, 0, pOutFile, 1);
	
	
	
	/* now that all the parameters are set, we can open the
	audio codec and allocate the necessary encode buffers */
	if (audio_st) {
		if((ret = open_audio(oc, audio_st)) < 0) {
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not open audio");
			return ret;
		}
	}
	
	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE)) {
		if ((ret = avio_open(&oc->pb, pOutFile, URL_WRONLY)) < 0) {
			snprintf(buf, sizeof(buf), "Could not open '%s' AVERROR_NOENT:%d", pOutFile, AVERROR_NOENT);
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
			return ret;
		}
	}
	
	//Free old URLContext
	if( (ret=ffurl_close(oc->pb->opaque)) < 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not free URLContext");
		return ret;
	}
	
	urlContext = get_audio_connection();
	if ((ret = rtp_set_remote_url (urlContext, pOutFile)) < 0) {
		snprintf(buf, sizeof(buf), "Could not open '%s'", pOutFile);
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
		return ret;
	}
	
	oc->pb->opaque = urlContext;
	
	
	/* write the stream header, if any */
	av_write_header(oc);

	RTPMuxContext *rptmc = oc->priv_data;
	rptmc->payload_type = payload_type;

	(*env)->ReleaseStringUTFChars(env, outfile, pOutFile);
	
	return audio_st->codec->frame_size;
}

////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//PUT AUIDO SAMPLES

static int write_audio_frame(AVFormatContext *oc, AVStream *st, int16_t *samples)
{
	AVCodecContext *c;
	AVPacket pkt;
	av_init_packet(&pkt);
	int ret;

	c = st->codec;
	
	pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);
	
	if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
		pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
	pkt.flags |= AV_PKT_FLAG_KEY;
	pkt.stream_index= st->index;
	pkt.data= audio_outbuf;
	
	/* write the compressed frame in the media file */
	ret = av_interleaved_write_frame(oc, &pkt);
	if (ret != 0) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Error while writing audio frame");
		return ret;
	}
	
	return 0;
}

jint
Java_com_tikal_android_media_tx_MediaTx_putAudioSamples (JNIEnv* env,
						jobject thiz,
						jshortArray in_buffer, jint in_size)
{
	int16_t *samples;
	int i, ret, nframes, frame_size;
	
	//FIXME: controlar el tamaño del array pasado y levantar un error si no es correcto.
	
	samples = (int16_t*)((*env)->GetShortArrayElements(env, in_buffer, JNI_FALSE));
	if(samples == NULL) {
		__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Error in samples (NULL)");
    		return -1;
    	}
	
	frame_size = audio_st->codec->frame_size;
	nframes = in_size / frame_size;


	for(i=0; i<nframes; i++) {
		if( (ret=write_audio_frame(oc, audio_st, &samples[i*frame_size])) < 0) {
			(*env)->ReleaseShortArrayElements(env, in_buffer, samples, 0);
			__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Could not write audio frame");
			return ret;
		}
	}	
	
	(*env)->ReleaseShortArrayElements(env, in_buffer, samples, 0);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//FINISH AUDIO

static void close_audio(AVFormatContext *oc, AVStream *st)
{
	if(st)
		avcodec_close(st->codec);
	av_free(audio_outbuf);
}

jint
Java_com_tikal_android_media_tx_MediaTx_finishAudio (JNIEnv* env,
					jobject thiz)
{
	int i;
	/* write the trailer, if any.  the trailer must be written
	* before you close the CodecContexts open when you wrote the
	* header; otherwise write_trailer may try to use memory that
	* was freed on av_codec_close() */
	if(oc) {
		av_write_trailer(oc);
		
		/* close codec */
		if (audio_st)
			close_audio(oc, audio_st);
	
		/* free the streams */
		for(i = 0; i < oc->nb_streams; i++) {
			av_freep(&oc->streams[i]->codec);
			av_freep(&oc->streams[i]);
		}
		if (oc->pb && !(fmt->flags & AVFMT_NOFILE)) {
			/* close the output file */
			avio_close(oc->pb);
		}
		close_context(oc);
	}
	
	for (i=0;i<AVMEDIA_TYPE_NB;i++)
		av_free(avcodec_opts[i]);
	av_free(avformat_opts);
	av_free(sws_opts);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

