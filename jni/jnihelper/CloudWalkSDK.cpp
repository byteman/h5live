#include "CloudWalkSDK.h"

#include "NDKH5Live.h"


ClassRegistry gClasses;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void*) {
  LOG_INFO("Initializing JNI");
  JNIEnv *env = jniHelpersInitialize(jvm);
  if (env == NULL) {
    return -1;
  }
  
  
  gClasses.add(env, new NDKH5Live(env));
  


  LOG_INFO("Initialization complete");
  return JAVA_VERSION;
}

