include $(CLEAR_VARS)
LOCAL_MODULE := libirrlicht
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/Irrlicht/ \
	
FILE_LIST := \
			$(wildcard $(LOCAL_PATH)/Irrlicht/*.cpp) \

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS = -O3 -mno-thumb
LOCAL_CPPFLAGS = -O3 -mno-thumb
include $(BUILD_STATIC_LIBRARY)