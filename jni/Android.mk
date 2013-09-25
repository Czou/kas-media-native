LOCAL_PATH := $(NDK_PROJECT_PATH)
include $(CLEAR_VARS)

MEDIA_SOURCES := kc-media-native/media
MEDIA_INCLUDES := $(NDK_PROJECT_PATH)/$(MEDIA_SOURCES)
export EXTERNAL := $(NDK_PROJECT_PATH)/kc-media-native/external

export MY_FFMPEG_SOURCE := $(EXTERNAL)/ffmpeg
export MY_FFMPEG_INSTALL := $(MY_FFMPEG_SOURCE)

export MY_AMR_INSTALL := $(NDK_PROJECT_PATH)/jni/target/opencore-amr_install
export MY_AMR_C_INCLUDE := $(MY_AMR_INSTALL)/include
export MY_AMR_LDLIB := -L$(MY_AMR_INSTALL)/lib -lopencore-amrnb

ifdef ENABLE_X264
	export MY_X264_INSTALL := $(NDK_PROJECT_PATH)/jni/target/x264_install
	export MY_X264_C_INCLUDE := $(MY_X264_INSTALL)/include
	export MY_X264_LDLIB := -L$(MY_X264_INSTALL)/lib -lx264
	LOCAL_CFLAGS += -DUSE_X264
endif

RESULT := $(shell export EXTERNAL=$(EXTERNAL); \
		export MY_FFMPEG_INSTALL=$(MY_FFMPEG_INSTALL); \
		export MY_AMR_INSTALL=$(MY_AMR_INSTALL); \
		export MY_X264_INSTALL=$(MY_X264_INSTALL); \
		$(NDK_PROJECT_PATH)/jni/configure-make-all.sh)

# These need to be in the right order
FFMPEG_LIBS := $(addprefix $(MY_FFMPEG_SOURCE)/, \
	libavformat/libavformat.a \
	libavcodec/libavcodec.a \
	libswscale/libswscale.a \
	libavutil/libavutil.a)

LOCAL_CFLAGS += -Wall -pedantic -std=c99

LOCAL_LDLIBS += $(FFMPEG_LIBS) $(MY_AMR_LDLIB) $(MY_X264_LDLIB) \
		-llog -lc -lm -ldl -lgcc -lz


LOCAL_C_INCLUDES := 	$(MY_FFMPEG_INSTALL) \
			$(MY_AMR_C_INCLUDE) \
			$(MY_X264_C_INCLUDE) \
			$(MEDIA_INCLUDES) \
			$(MEDIA_INCLUDES)/util \
			$(MEDIA_INCLUDES)/rx \
			$(MEDIA_INCLUDES)/tx \
			$(MEDIA_INCLUDES)/../media-oo \
			$(LOCAL_PATH)/jni/media \
			$(LOCAL_PATH)/jni/media-oo


LOCAL_CPPFLAGS += -D__STDC_CONSTANT_MACROS


LOCAL_MODULE := android-media
LOCAL_SRC_FILES :=	$(MEDIA_SOURCES)/init-media.c $(MEDIA_SOURCES)/my-cmdutils.c $(MEDIA_SOURCES)/socket-manager.c \
			$(MEDIA_SOURCES)/util/log.c $(MEDIA_SOURCES)/util/utils.c \
			$(MEDIA_SOURCES)/tx/audio-tx.c \
			$(MEDIA_SOURCES)/rx/sdp-manager.c $(MEDIA_SOURCES)/rx/video-rx.c $(MEDIA_SOURCES)/rx/audio-rx.c \
			jni/media/init-log.c \
			jni/media-oo/MediaTx.cpp \jni/media/rx/media-rx.c jni/media/media-port-manager.c \
			\
			jni/media-oo/util/utils.c \
			kc-media-native/media-oo/VideoTx.cpp

include $(BUILD_SHARED_LIBRARY)
