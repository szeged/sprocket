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
#include "sprocket/browser/ui/authentication_dialog.h"
#include "sprocket/browser/ui/javascript_dialog.h"
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
}

void SprocketWindow::PlatformAddTab(SprocketWebContents* sprocket_web_contents) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_initFromNativeTabContents(
      env,
      java_object_.obj(),
      sprocket_web_contents->web_contents()->GetJavaWebContents().obj());
}

void SprocketWindow::PlatformSelectTabAt(int index) {
  NOTIMPLEMENTED() << ": " << index;
}

void SprocketWindow::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  JNIEnv* env = AttachCurrentThread();
  if (java_object_.is_null())
    return;
  Java_SprocketWindow_enableUiControl(env, java_object_.obj(), control, is_enabled);
}

void SprocketWindow::PlatformSetAddressBarURL(const GURL& url) {
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jstring> j_url = ConvertUTF8ToJavaString(env, url.spec());
  Java_SprocketWindow_onUpdateUrl(env, java_object_.obj(), j_url.obj());
}

void SprocketWindow::PlatformSetIsLoading(bool loading) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_setIsLoading(env, java_object_.obj(), loading);
}

void SprocketWindow::PlatformSetTitle(const base::string16& title) {
  NOTIMPLEMENTED() << ": " << title;
}

bool SprocketWindow::PlatformHandleContextMenu(const content::ContextMenuParams& params) {
  return false;
}

void SprocketWindow::PlatformLoadProgressChanged(double progress) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_onLoadProgressChanged(env, java_object_.obj(), progress);
}

void SprocketWindow::PlatformShowJavaScriptDialog(SprocketJavaScriptDialog* dialog) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_showJavaScriptDialog(env, java_object_.obj(), dialog->GetJavaObject().obj());
}

void SprocketWindow::PlatformShowColorChooserDialog(SprocketColorChooser* listener,
                                                    SkColor initial_color) {
  // TODO: Implement!
}

void SprocketWindow::PlatformShowAuthenticationDialog(SprocketAuthenticationDialog* dialog) {
  JNIEnv* env = AttachCurrentThread();
  Java_SprocketWindow_showAuthenticationDialog(env, java_object_.obj(), dialog->GetJavaObject().obj());
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

void CloseSprocketWindow(JNIEnv* env,
                         const JavaParamRef<jclass>& clazz,
                         jlong sprocket_windowPtr) {
  SprocketWindow* sprocket_window = reinterpret_cast<SprocketWindow*>(sprocket_windowPtr);
  sprocket_window->Close();
}
