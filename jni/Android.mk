LOCAL_PATH := $(call my-dir)
 
 
include $(CLEAR_VARS)


LOCAL_MODULE :=PocoNet


LOCAL_SRC_FILES := $(LOCAL_PATH)/poco1.8.1/lib/Android/armeabi-v7a/libPocoNet.a


include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=PocoFoundation
LOCAL_SRC_FILES := $(LOCAL_PATH)/poco1.8.1/lib/Android/armeabi-v7a/libPocoFoundation.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH) ../src ./poco1.8.1/include ../ext/libflv/include
			
LOCAL_LDLIBS += -llog -lz -lm -ldl -lc

LOCAL_LDFLAGS += -L../libs/$(TARGET_ARCH_ABI) 
 
LOCAL_CXXFLAGS +=-DENABLE_LOGGING
LOCAL_CFLAGS +=-DENABLE_LOGGING
LOCAL_MODULE    := h5live
LOCAL_SRC_FILES +=  NDKH5Live.cpp ../src/h5liveserver.cpp ../src/CWHttpServer.cpp ../src/CWRequestHandler.cpp \
				   ../src/CWRequestHandlerFactory.cpp ../src/CWWebSocketServer.cpp  ../src/flvmuxer.cpp \
				   ../src/mediatype.cpp ../src/MediaTypeMapper.cpp ../src/streamchannel.cpp \
				   ../ext/libflv/src/BaseWriter.cpp ../ext/libflv/src/flv_bytestream.c ../ext/libflv/src/FlvEncoder.cpp \
					jnihelper/ByteArray.cpp  jnihelper/ClassRegistry.cpp jnihelper/CloudWalkSDK.cpp\
				   jnihelper/JavaClass.cpp jnihelper/JavaClassUtils.cpp jnihelper/JavaExceptionUtils.cpp \
				   jnihelper/JavaString.cpp jnihelper/JavaStringArray.cpp jnihelper/JavaThreadUtils.cpp \
				   jnihelper/JniHelpers.cpp  jnihelper/FloatArray.cpp\
				   jnihelper/ShortArray.cpp jnihelper/Watch.cpp  jnihelper/IntArray.cpp
LOCAL_STATIC_LIBRARIES := PocoNet 			   
LOCAL_STATIC_LIBRARIES += PocoFoundation

include $(BUILD_SHARED_LIBRARY)

 