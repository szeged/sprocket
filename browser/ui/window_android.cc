// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/window.h"

#include "base/android/jni_string.h"
#include "content/public/browser/web_contents.h"
#include "jni/SprocketWindow_jni.h"
#include "sprocket/android/manager.h"
#include "sprocket/browser/web_contents.h"
#include <jni.h>

using base::android::AttachCurrentThread;
using base::android::ConvertUTF8ToJavaString;

// static
void SprocketWindow::PlatformInitialize() {
}

// static
void SprocketWindow::PlatformExit() {
}

void SprocketWindow::PlatformCleanUp() {
  JNIEnv* env = AttachCurrentThread();
  if (java_object_.is_null())
    return;
  Java_SprocketWindow_onNativeDestroyed(env, java_object_.obj());
}

void SprocketWindow::PlatformCreateWindow(int width, int height) {
  java_object_.Reset(AttachCurrentThread(), CreateSprocketWindow(this));
}

void SprocketWindow::PlatformCloseWindow() {
  RemoveSprocketWindow(java_object_.obj());
  delete sprocket_web_contents_;
}

void SprocketWindow::PlatformSetContents(SprocketWebContents* sprocket_web_contents) {
  sprocket_web_contents_ = sprocket_web_contents;
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_initFromNativeTabContents(
      env,
      java_object_.obj(),
      sprocket_web_contents->web_contents()->GetJavaWebContents().obj());
}

void SprocketWindow::PlatformSetTitle(const base::string16& title) {
  NOTIMPLEMENTED() << ": " << title;
}

void SprocketWindow::PlatformToggleFullscreenModeForTab(bool enter_fullscreen) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_toggleFullscreenModeForTab(
      env, java_object_.obj(), enter_fullscreen);
}

bool SprocketWindow::PlatformIsFullscreenForTabOrPending() const {
  JNIEnv* env = AttachCurrentThread();
  return Java_SprocketWindow_isFullscreenForTabOrPending(env, java_object_.obj());
}

void SprocketWindow::PlatformHandleKeyboardEvent(const content::NativeWebKeyboardEvent& event) {
}

// static
bool SprocketWindow::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

void CloseSprocketWindow(JNIEnv* env, jclass clazz, jlong sprocket_windowPtr) {
  SprocketWindow* sprocket_window = reinterpret_cast<SprocketWindow*>(sprocket_windowPtr);
  sprocket_window->Close();
}
