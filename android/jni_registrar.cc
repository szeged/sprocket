// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/android/jni_registrar.h"

#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "sprocket/android/web_contents_manager.h"
#include "sprocket/browser/ui/web_contents.h"

namespace {

static base::android::RegistrationMethod kSprocketWebContentsRegistrationMethods[] = {
    {"SprocketWebContents", SprocketWebContents::Register},
    {"SprocketManager", RegisterSprocketManager},
};

}  // namespace

bool RegisterSprocketWebContentsJni(JNIEnv* env) {
  return RegisterNativeMethods(env, kSprocketWebContentsRegistrationMethods,
                               arraysize(kSprocketWebContentsRegistrationMethods));
}
