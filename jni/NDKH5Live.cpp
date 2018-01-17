
#include "jnihelper/Watch.h"
#include "NDKH5Live.h"
#include "h5liveserver.h"


jint NDKH5Live::start(JNIEnv *env, jobject javaThis,jstring webroot, jint port)
{
	JavaString jroot(env, webroot);
	std::string root = jroot.get();
	return H5liveServer::get().start(root, port);
	
}
jint NDKH5Live::push264(JNIEnv *env, jobject javaThis,jstring channel, jbyteArray data)
{
	JavaString jchan(env, channel);
	std::string chan = jchan.get();

	ByteArray h264;
	h264.set(env, data);

	
	return H5liveServer::get().push264(chan, (unsigned char*)h264.get(), h264.size(), 0);
	
}
jint NDKH5Live::addStream(JNIEnv *env, jobject javaThis,jstring channel)
{
	
	JavaString jchan(env, channel);
	std::string chan = jchan.get();
	
	return H5liveServer::get().addStream(chan);
	

}
jint NDKH5Live::removeStream(JNIEnv *env, jobject javaThis,jstring channel)
{
	
	JavaString jchan(env, channel);
	std::string chan = jchan.get();
	
	return H5liveServer::get().removeStream(chan);
	

}

jint NDKH5Live::stop(JNIEnv *env, jobject javaThis)
{
	
	
	return 0;

}

void NDKH5Live::initialize(JNIEnv *env)
{
	setClass(env);

	addNativeMethod("H5liveStart", (void*)&start, kTypeInt,kTypeString,kTypeInt,NULL);
	addNativeMethod("H5liveStop", (void*)&stop, kTypeInt,NULL);
	addNativeMethod("H5livePush", (void*)&push264, kTypeInt,kTypeString,kTypeArray(kTypeByte),NULL);
	addNativeMethod("H5liveAddStream", (void*)&addStream, kTypeInt, kTypeString, NULL);
	addNativeMethod("H5liveRemoveStream", (void*)&removeStream, kTypeInt, kTypeString, NULL);

	registerNativeMethods(env);
}


