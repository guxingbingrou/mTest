LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	../mouseTest.c 




LOCAL_MODULE:= mouse

include $(BUILD_EXECUTABLE)
