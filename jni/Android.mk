TOP_LOCAL_PATH := $(call my-dir)

include $(call all-subdir-makefiles)

LOCAL_PATH := $(TOP_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := 	$(LOCAL_PATH)/ffmpeg-0.7-rc1	\
			$(LOCAL_PATH)/ffmpeg-0.7-rc1/x264install/include	\
			$(LOCAL_PATH)/ffmpeg-0.7-rc1/opencore-amr_install/include	\
			$(LOCAL_PATH)/media	\
			$(LOCAL_PATH)/media/rx

LOCAL_STATIC_LIBRARIES := libavformat libavcodec libavutil libpostproc libswscale 
LOCAL_LDLIBS :=	-llog -L$(LOCAL_PATH)/ffmpeg-0.7-rc1/x264install/lib		\
		-L$(LOCAL_PATH)/ffmpeg-0.7-rc1/opencore-amr_install/lib		\
		-lc -lm -ldl -lgcc -lz -lx264 -lopencore-amrnb

LOCAL_MODULE := android-media
LOCAL_SRC_FILES :=	media/my-cmdutils.c media/init-media.c media/socket-manager.c	\
			media/tx/video-tx.c media/tx/audio-tx.c		\
			media/rx/sdp-manager.c media/rx/video-rx.c media/rx/audio-rx.c
			

include $(BUILD_SHARED_LIBRARY)

