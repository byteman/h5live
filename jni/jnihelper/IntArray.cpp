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

#include "IntArray.h"
#include "JavaExceptionUtils.h"
#include <stdlib.h>

namespace spotify {
namespace jni {

IntArray::IntArray() : _data(NULL), _num_bytes(0) {}

IntArray::IntArray(int *data, const size_t numBytes, bool copyData) :
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

IntArray::IntArray(JNIEnv *env, jintArray data) :
_data(NULL), _num_bytes(0) {
  set(env, data);
}

IntArray::~IntArray() {
  if (_data != NULL) {
    //delete(_data);
    free(_data);
    _data = NULL;
  }
}

int *IntArray::leak() {
  int *result = _data;
  _data = NULL;
  _num_bytes = 0;
  return result;
}

JniLocalRef<jintArray> IntArray::toJavaIntArray(JNIEnv *env) const {
  JniLocalRef<jintArray> result = env->NewIntArray((jsize)_num_bytes);
  JavaExceptionUtils::checkException(env);
  if (_num_bytes == 0 || _data == NULL) {
    return result;
  }
  env->SetIntArrayRegion(result, 0, (jsize)_num_bytes, (jint *)_data);
  return result.leak();
}

void IntArray::set(int *data, const size_t numBytes, bool copyData) {
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
    //_data = new int[_num_bytes];
    _data = (int*)malloc(numBytes*sizeof(int));
    memcpy(_data, data, numBytes*sizeof(int));
  } else {
    _data = data;
  }
  _num_bytes = numBytes;
}

void IntArray::set(JNIEnv *env, jintArray data) {
  if (_data != NULL) {
    delete(_data);
  }

  if (data != NULL) {
    _num_bytes = env->GetArrayLength(data);
    if (_num_bytes == 0) {
      _data = NULL;
    } else {
      //_data = new int[_num_bytes];
      _data = (int*)malloc(_num_bytes*sizeof(int));
      env->GetIntArrayRegion(data, 0, (jsize)_num_bytes, (jint *)_data);
    }
  }
}

} // namespace jni
} // namespace spotify
