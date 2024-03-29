# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=4735
LOCAL_PATH:= $(call my-dir)

include jni/BulletPhysics/Android.mk

include $(CLEAR_VARS)

LOCAL_MODULE    := libbulletphysics

LOCAL_C_INCLUDES := \
   $(LOCAL_PATH)/ \
   $(LOCAL_PATH)/BulletPhysics \

LOCAL_CFLAGS    := $(LOCAL_C_INCLUDES:%=-I%) -O3 -DANDROID_NDK -Wno-psabi

LOCAL_DEFAULT_CPP_EXTENSION := cpp

LOCAL_LDLIBS    := -llog -lGLESv2 -ldl -landroid

LOCAL_SRC_FILES := gl_code.cpp 
   
LOCAL_STATIC_LIBRARIES := libbullet

include $(BUILD_SHARED_LIBRARY)
