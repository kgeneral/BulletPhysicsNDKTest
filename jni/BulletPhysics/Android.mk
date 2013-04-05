include $(CLEAR_VARS)
LOCAL_MODULE := libbullet
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/BulletPhysics/ \
	
FILE_LIST := \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletCollision/BroadphaseCollision/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletCollision/CollisionDispatch/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletCollision/CollisionShapes/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletCollision/Gimpact/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletCollision/NarrowPhaseCollision/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletDynamics/Character/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletDynamics/ConstraintSolver/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletDynamics/Dynamics/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/BulletDynamics/Vehicle/*.cpp) \
			$(wildcard $(LOCAL_PATH)/BulletPhysics/LinearMath/*.cpp) \

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS = -O3 -mno-thumb
LOCAL_CPPFLAGS = -O3 -mno-thumb
include $(BUILD_STATIC_LIBRARY)