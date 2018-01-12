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

#ifndef __IntArray_h__
#define __IntArray_h__

#include "JniHelpersCommon.h"
#include "JniLocalRef.h"

namespace spotify {
namespace jni {

/**
 * @brief Class for holding Java byte[] data
 *
 * This class is used for passing raw data arrays through JNI. Internally
 * the data is stored as a void*.
 */
class EXPORT IntArray {
public:
  /**
   * @brief Create a new empty byte array
   */
  IntArray();

  /**
   * @brief Create a new byte array with data.
   * @param data Pointer to data
   * @param numBytes Size of data pointed to
   * @param copyData True if the data should be copied to this instance
   *
   * See the documentation for set() regarding the ownership of data stored
   * in ByteArray instances.
   */
  IntArray(int *data, const size_t numBytes, bool copyData);

  /**
   * @brief Create a new byte array with data from a Java byte[] object
   * @param env JNIEnv
   * @param data Java byte[] data
   */
  IntArray(JNIEnv *env, jintArray data);

  virtual ~IntArray();

  /**
   * @brief Get a pointer to the natively stored data
   */
  const int* get() const { return _data; }

  int getValByIndex(int iIndex) const { return _data[iIndex]; }

  /**
   * @brief Convert data to a Java byte[] array
   */
  JniLocalRef<jintArray> toJavaIntArray(JNIEnv *env) const;

  /**
   * @brief Return a pointer to the data stored by this instance
   *
   * When an instance of ByteArray is destroyed, it will attempt to free
   * the data stored internally. If this data is still needed elsewhere,
   * then you should call leak() or else it will be unavailable.
   */
  int *leak();

  /**
   * @brief Store data in this instance
   * @param data Pointer to data
   * @param numBytes Size of data pointed to
   * @param copyData True if the data should be copied to this instance
   *
   * If copyData is true, then this ByteArray instance owns that data and
   * the original data passed to this method can be freed without worry.
   * However, if copyData is false, then this ByteArray effectively just
   * points to that instance instead. In either case, after setting data
   * in a ByteArray, it effectively owns that data.
   *
   * When this ByteArray is destroyed, it will free the data stored in it,
   * regardless of how it has been set. This means that if copyData was
   * false and that data is still needed elsewhere, then a segfault will
   * probably occur when attempting to access that data after this object
   * has been destroyed. Thus, the leak() method can be used to remedy the
   * situation by removing the pointer to the data so the ByteArray will
   * not free it upon destruction.
   *
   * It is obviously more efficient to not copy the data, however this can
   * cause problems if your code does not respect the ownership behaviors
   * described above.
   */
  void set(int *data, const size_t numBytes, bool copyData);

  /**
   * @brief Store data in this instance from a Java byte[] array
   * @param env JNIenv
   * @param data Java byte[] array
   */
  void set(JNIEnv *env, jintArray data);

  /**
   * @brief Get the size of the data stored by this instance
   */
  const size_t size() const { return _num_bytes; }

private:
  int *_data;
  size_t _num_bytes;
};

} // namespace jni
} // namespace spotify

#endif // __ByteArray_h__
