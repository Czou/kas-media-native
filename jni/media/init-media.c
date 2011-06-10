/**
 * 
 * @author Miguel París Díaz
 * 
 */

#include "init-media.h"
#include <android/log.h>
#include <pthread.h>

static char buf[256]; //Log

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int initialized = 0;

/*
	see	libavutil/log.c
		ffserver.c
*/
static void
android_av_log(void *ptr, int level, const char *fmt, va_list vargs)
{
	int android_log = ANDROID_LOG_DEBUG;
	switch(level){
	case AV_LOG_ERROR:
			android_log = ANDROID_LOG_ERROR;
			break;	
	}
	vsnprintf(buf, sizeof(buf), fmt, vargs);
	__android_log_write(android_log, "av_log", buf);
}

static int
lockmgr(void **mtx, enum AVLockOp op)
{
	switch(op) {
		case AV_LOCK_CREATE:
			*mtx = malloc(sizeof(pthread_mutex_t));
			if(!*mtx)
				return 1;
			return !!pthread_mutex_init(*mtx, NULL);
		case AV_LOCK_OBTAIN:
			return !!pthread_mutex_lock(*mtx);
		case AV_LOCK_RELEASE:
			return !!pthread_mutex_unlock(*mtx);
		case AV_LOCK_DESTROY:
			pthread_mutex_destroy(*mtx);
			free(*mtx);
			return 0;
	}
	return 1;
}


int
init_media()
{
	int ret = 0;
	
	pthread_mutex_lock(&mutex);
	if(!initialized) {
		av_log_set_callback(android_av_log);
		av_register_all();
		ret = av_lockmgr_register(lockmgr);
		initialized++;
	}
	pthread_mutex_unlock(&mutex);
	
	return ret;
}

