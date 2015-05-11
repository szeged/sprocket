// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/jni_android.h"
#include "base/bind.h"
#include "content/public/app/content_jni_onload.h"
#include "content/public/app/content_main.h"
#include "content/public/browser/android/compositor.h"
#include "sprocket/android/jni_registrar.h"
#include "sprocket/app/main_delegate.h"

namespace {

bool RegisterJNI(JNIEnv* env) {
  return RegisterSprocketWebContentsJni(env);
}

bool Init() {
  content::Compositor::Initialize();
  content::SetContentMainDelegate(new SprocketMainDelegate());
  return true;
}

}  // namespace


// This is called by the VM when the shared library is first loaded.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  std::vector<base::android::RegisterCallback> register_callbacks;
  register_callbacks.push_back(base::Bind(&RegisterJNI));
  std::vector<base::android::InitCallback> init_callbacks;
  init_callbacks.push_back(base::Bind(&Init));
  if (!content::android::OnJNIOnLoadRegisterJNI(vm, register_callbacks) ||
      !content::android::OnJNIOnLoadInit(init_callbacks)) {
    return -1;
  }
  return JNI_VERSION_1_4;
}
