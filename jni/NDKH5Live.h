#ifndef MY_FACE_SERVICE_LIT_H
#define MY_FACE_SERVICE_LIT_H


#include "jnihelper/JniHelpers.h"
#include "jnihelper/CloudWalkSDK.h"
#include <string>


using namespace spotify::jni;
  
class NDKH5Live:public JavaClass {
public:
  // Direct instantiation not allowed
  NDKH5Live() : JavaClass() {}
  NDKH5Live(JNIEnv *env) : JavaClass(env) { initialize(env); }
  
  ~NDKH5Live() {}
  const char* getCanonicalName() const {
    return MAKE_CANONICAL_NAME(PACKAGE, h5live);
  }
  void initialize(JNIEnv *env);
  void mapFields() {}
private:

	static jint start(JNIEnv *env, jobject javaThis,jstring webroot, jint port);
	static jint push264(JNIEnv *env, jobject javaThis,jstring channel, jbyteArray data);
	static jint addStream(JNIEnv *env, jobject javaThis,jstring channel);
	static jint removeStream(JNIEnv *env, jobject javaThis,jstring channel);
	static   jint stop(JNIEnv *env, jobject javaThis);
};


#endif
