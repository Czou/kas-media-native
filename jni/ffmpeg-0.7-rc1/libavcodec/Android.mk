LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/../av.mk
LOCAL_SRC_FILES := $(FFFILES)
LOCAL_C_INCLUDES :=		\
	$(LOCAL_PATH)		\
	$(LOCAL_PATH)/..	\
	$(LOCAL_PATH)/../x264install/include	\
	$(LOCAL_PATH)/../opencore-amr_install/include
LOCAL_CFLAGS += $(FFCFLAGS)
LOCAL_LDLIBS := -lz -L$(LOCAL_PATH)/../opencore-amr_install/lib -lopencore-amrnb
LOCAL_STATIC_LIBRARIES := $(FFLIBS)
LOCAL_MODULE := $(FFNAME)
include $(BUILD_STATIC_LIBRARY)

