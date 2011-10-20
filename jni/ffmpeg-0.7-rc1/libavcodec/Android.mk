LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/../av.mk
LOCAL_SRC_FILES := $(FFFILES)
LOCAL_C_INCLUDES :=		\
	$(LOCAL_PATH)		\
	$(LOCAL_PATH)/..	\
	$(LOCAL_PATH)/../x264install/include	\
	$(MY_AMR_INSTALL)/include
LOCAL_CFLAGS += $(FFCFLAGS)
LOCAL_LDLIBS := -lz -L$(MY_AMR_INSTALL)/lib -lopencore-amrnb
LOCAL_STATIC_LIBRARIES := $(FFLIBS)
LOCAL_MODULE := $(FFNAME)
include $(BUILD_STATIC_LIBRARY)

