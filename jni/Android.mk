TOP_LOCAL_PATH := $(call my-dir)

include $(call all-subdir-makefiles)

RESULT:= $(shell cd $(NDK_PROJECT_PATH)/jni/ffmpeg-0.7-rc1 && ./config-ffmpeg.sh)

LOCAL_PATH := $(TOP_LOCAL_PATH)
ifdef USE_X264_TREE
    $(info "GPL version, with H264 encoding support")
    LOCAL_X264_C_INCLUDE := $(LOCAL_PATH)/ffmpeg-0.7-rc1/x264install/include
    LOCAL_X264_LDLIB := -L$(LOCAL_PATH)/ffmpeg-0.7-rc1/x264install/lib -lx264
endif

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := 	$(LOCAL_PATH)/ffmpeg-0.7-rc1	\
			$(LOCAL_X264_C_INCLUDE)	\
			$(LOCAL_PATH)/ffmpeg-0.7-rc1/opencore-amr_install/include	\
			$(LOCAL_PATH)/media	\
			$(LOCAL_PATH)/media/rx

LOCAL_STATIC_LIBRARIES := libavformat libavcodec libavutil libpostproc libswscale 
LOCAL_LDLIBS :=	-llog $(LOCAL_X264_LDLIB)	\
		-L$(LOCAL_PATH)/ffmpeg-0.7-rc1/opencore-amr_install/lib		\
		-lc -lm -ldl -lgcc -lz -lopencore-amrnb

LOCAL_MODULE := android-media
LOCAL_SRC_FILES :=	media/my-cmdutils.c media/init-media.c media/socket-manager.c	\
			media/tx/video-tx.c media/tx/audio-tx.c		\
			media/rx/sdp-manager.c media/rx/video-rx.c media/rx/audio-rx.c
			

include $(BUILD_SHARED_LIBRARY)

