// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/android/web_contents_manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/bind.h"
#include "base/lazy_instance.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/ui/web_contents.h"
#include "sprocket/browser/browser_context.h"
#include "sprocket/browser/content_browser_client.h"
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

jobject CreateSprocketWebContentsView(SprocketWebContents* sprocket_web_contents) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_sprocket_manager = g_global_state.Get().j_sprocket_manager.obj();
  return Java_SprocketManager_createSprocketWebContents(
      env,
      j_sprocket_manager,
      reinterpret_cast<intptr_t>(sprocket_web_contents)).Release();
}

void RemoveSprocketWebContentsView(jobject sprocket_web_contents_view) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_sprocket_manager = g_global_state.Get().j_sprocket_manager.obj();
  Java_SprocketManager_removeSprocketWebContents(env, j_sprocket_manager, sprocket_web_contents_view);
}

// Register native methods
bool RegisterSprocketManager(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

static void Init(JNIEnv* env, jclass clazz, jobject obj) {
  g_global_state.Get().j_sprocket_manager.Reset(
      base::android::ScopedJavaLocalRef<jobject>(env, obj));
}

void LaunchSprocketWebContents(JNIEnv* env, jclass clazz, jstring jurl) {
  SprocketBrowserContext* browserContext =
      SprocketContentBrowserClient::Get()->browser_context();
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  SprocketWebContents::CreateNewWindow(browserContext,
                         url,
                         NULL,
                         gfx::Size());
}
