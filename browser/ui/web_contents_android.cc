// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/web_contents.h"

#include <jni.h>

#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "sprocket/android/manager.h"
#include "jni/SprocketWebContents_jni.h"

using base::android::AttachCurrentThread;
using base::android::ConvertUTF8ToJavaString;

void SprocketWebContents::PlatformInitialize(const gfx::Size& default_window_size) {
}

void SprocketWebContents::PlatformExit() {
}

void SprocketWebContents::PlatformCleanUp() {
  JNIEnv* env = AttachCurrentThread();
  if (java_object_.is_null())
    return;
  Java_SprocketWebContents_onNativeDestroyed(env, java_object_.obj());
}

void SprocketWebContents::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  JNIEnv* env = AttachCurrentThread();
  if (java_object_.is_null())
    return;
  Java_SprocketWebContents_enableUiControl(env, java_object_.obj(), control, is_enabled);
}

void SprocketWebContents::PlatformSetAddressBarURL(const GURL& url) {
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jstring> j_url = ConvertUTF8ToJavaString(env, url.spec());
  Java_SprocketWebContents_onUpdateUrl(env, java_object_.obj(), j_url.obj());
}

void SprocketWebContents::PlatformSetIsLoading(bool loading) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWebContents_setIsLoading(env, java_object_.obj(), loading);
}

void SprocketWebContents::PlatformCreateWindow(int width, int height) {
  java_object_.Reset(AttachCurrentThread(), CreateSprocketWebContentsView(this));
}

void SprocketWebContents::PlatformSetContents() {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWebContents_initFromNativeTabContents(
      env, java_object_.obj(), web_contents()->GetJavaWebContents().obj());
}

void SprocketWebContents::PlatformResizeSubViews() {
  // Not needed; subviews are bound.
}

void SprocketWebContents::PlatformSetTitle(const base::string16& title) {
  NOTIMPLEMENTED() << ": " << title;
}

void SprocketWebContents::LoadProgressChanged(content::WebContents* source, double progress) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWebContents_onLoadProgressChanged(env, java_object_.obj(), progress);
}

void SprocketWebContents::PlatformToggleFullscreenModeForTab(content::WebContents* web_contents,
                                               bool enter_fullscreen) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWebContents_toggleFullscreenModeForTab(
      env, java_object_.obj(), enter_fullscreen);
}

bool SprocketWebContents::PlatformIsFullscreenForTabOrPending(
    const content::WebContents* web_contents) const {
  JNIEnv* env = AttachCurrentThread();
  return Java_SprocketWebContents_isFullscreenForTabOrPending(env, java_object_.obj());
}

bool SprocketWebContents::PlatformHandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}

void SprocketWebContents::Close() {
  RemoveSprocketWebContentsView(java_object_.obj());
  delete this;
}

// static
bool SprocketWebContents::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

// static
void CloseSprocketWebContents(JNIEnv* env, jclass clazz, jlong sprocket_web_contentsPtr) {
  SprocketWebContents* sprocket_web_contents = reinterpret_cast<SprocketWebContents*>(sprocket_web_contentsPtr);
  sprocket_web_contents->Close();
}
