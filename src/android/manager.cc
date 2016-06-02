// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/android/manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/bind.h"
#include "base/lazy_instance.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/browser_context.h"
#include "sprocket/browser/content_browser_client.h"
#include "sprocket/browser/web_contents.h"
#include "sprocket/browser/ui/window.h"
#include "jni/SprocketManager_jni.h"
#include "url/gurl.h"

using base::android::ScopedJavaLocalRef;

namespace {

struct GlobalState {
  GlobalState() {}
  base::android::ScopedJavaGlobalRef<jobject> j_sprocket_manager;
};

base::LazyInstance<GlobalState> g_global_state = LAZY_INSTANCE_INITIALIZER;

}  // namespace

jobject CreateSprocketWindow(SprocketWindow* sprocket_window) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_sprocket_manager = g_global_state.Get().j_sprocket_manager.obj();
  return Java_SprocketManager_createSprocketWindow(
      env,
      j_sprocket_manager,
      reinterpret_cast<intptr_t>(sprocket_window)).Release();
}

void RemoveSprocketWindow(jobject sprocket_window) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_sprocket_manager = g_global_state.Get().j_sprocket_manager.obj();
  Java_SprocketManager_removeSprocketWindow(env, j_sprocket_manager, sprocket_window);
}

// Register native methods
bool RegisterSprocketManager(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

static void Init(JNIEnv* env,
                 const JavaParamRef<jclass>& clazz,
                 const JavaParamRef<jobject>& obj) {
  g_global_state.Get().j_sprocket_manager.Reset(obj);
}

void LaunchSprocketWindow(JNIEnv* env,
                 const JavaParamRef<jclass>& clazz,
                 const JavaParamRef<jstring>& jurl) {
  SprocketBrowserContext* browserContext =
      SprocketContentBrowserClient::Get()->BrowserContext();
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  SprocketWindow* window = SprocketWindow::CreateNewWindow(gfx::Size());
  SprocketWebContents::CreateSprocketWebContents(
      window,
      browserContext,
      url,
      gfx::Size());
}
