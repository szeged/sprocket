// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_ANDROID_JNI_REGISTRAR_H_
#define SPROCKET_ANDROID_JNI_REGISTRAR_H_

#include <jni.h>

// Register all JNI bindings necessary for sprocket.
bool RegisterSprocketJni(JNIEnv* env);

#endif  // SPROCKET_ANDROID_JNI_REGISTRAR_H_
