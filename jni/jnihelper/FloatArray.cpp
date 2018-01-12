/*
 * Copyright (c) 2014 Spotify AB
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "FloatArray.h"
#include "JavaExceptionUtils.h"
#include <stdlib.h>

namespace spotify {
namespace jni {

FloatArray::FloatArray() : _data(NULL), _num_bytes(0) {}

FloatArray::FloatArray(float *data, const size_t numBytes, bool copyData) :
_data(NULL), _num_bytes(0) {
  // In the rare (but possible) event that this constructor is called with
  // NULL but non-zero length data, correct the byte count so as to avoid
  // segfaults later on.
  if (data == NULL && numBytes > 0) {
    _num_bytes = 0;
  } else if (data != NULL && numBytes > 0) {
    set(data, numBytes, copyData);
  }
}

FloatArray::FloatArray(JNIEnv *env, jfloatArray data) :
_data(NULL), _num_bytes(0) {
  set(env, data);
}

FloatArray::~FloatArray() {
  if (_data != NULL) {
    //delete(_data);
    free(_data);
    _data = NULL;
  }
}

float *FloatArray::leak() {
  float *result = _data;
  _data = NULL;
  _num_bytes = 0;
  return result;
}

JniLocalRef<jfloatArray> FloatArray::toJavaFloatArray(JNIEnv *env) const {
  JniLocalRef<jfloatArray> result = env->NewFloatArray((jsize)_num_bytes);
  JavaExceptionUtils::checkException(env);
  if (_num_bytes == 0 || _data == NULL) {
    return result;
  }
  env->SetFloatArrayRegion(result, 0, (jsize)_num_bytes, (jfloat *)_data);
  return result.leak();
}

void FloatArray::set(float *data, const size_t numBytes, bool copyData) {
  if (data == NULL && numBytes > 0) {
    JNIEnv *env = JavaThreadUtils::getEnvForCurrentThread();
    JavaExceptionUtils::throwExceptionOfType(env, kTypeIllegalArgumentException, 
      "Cannot set data with non-zero size and NULL object");
    return;
  }

  // Make sure not to leak the old data if it exists
  if (_data != NULL) {
    //delete(_data);
    free(_data);
  }

  if (copyData) {
    //_data = new float[_num_bytes];
    _data = (float*)malloc(numBytes*sizeof(float));
    memcpy(_data, data, numBytes*sizeof(float));
  } else {
    _data = data;
  }
  _num_bytes = numBytes;
}

void FloatArray::set(JNIEnv *env, jfloatArray data) {
  if (_data != NULL) {
    delete(_data);
  }

  if (data != NULL) {
    _num_bytes = env->GetArrayLength(data);
    if (_num_bytes == 0) {
      _data = NULL;
    } else {
      //_data = new float[_num_bytes];
      _data = (float*)malloc(_num_bytes*sizeof(float));
      env->GetFloatArrayRegion(data, 0, (jsize)_num_bytes, (jfloat *)_data);
    }
  }
}

} // namespace jni
} // namespace spotify
