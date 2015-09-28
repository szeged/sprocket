// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/javascript_dialog.h"

#include "base/android/jni_string.h"
#include "jni/SprocketJavaScriptDialog_jni.h"

using base::android::AttachCurrentThread;
using base::android::ConvertUTF16ToJavaString;
using base::android::ConvertJavaStringToUTF16;

void SprocketJavaScriptDialog::Cancel() {
  callback_.Run(false, default_prompt_text_);
}

void SprocketJavaScriptDialog::Accept() {
  callback_.Run(true, default_prompt_text_);
}

void SprocketJavaScriptDialog::Accept(const base::string16& prompt_text) {
  callback_.Run(true, prompt_text);
}

void SprocketJavaScriptDialog::CreateJavaObject() {
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jstring> j_message_text = ConvertUTF16ToJavaString(env, message_text_);
  ScopedJavaLocalRef<jstring> j_prompt_text = ConvertUTF16ToJavaString(env, default_prompt_text_);
  java_object_.Reset(
    env,
    Java_SprocketJavaScriptDialog_create(
      env,
      type_,
      j_message_text.obj(),
      j_prompt_text.obj(),
      reinterpret_cast<intptr_t>(this)).obj());
}

base::android::ScopedJavaLocalRef<jobject>
SprocketJavaScriptDialog::GetJavaObject() {
  return base::android::ScopedJavaLocalRef<jobject>(java_object_);
}

// static
bool SprocketJavaScriptDialog::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

void CancelDialog(JNIEnv* env, jclass clazz, jlong javaScriptDialogPtr) {
  SprocketJavaScriptDialog* javascript_dialog =
    reinterpret_cast<SprocketJavaScriptDialog*>(javaScriptDialogPtr);
  javascript_dialog->Cancel();
}

void AcceptDialog(JNIEnv* env, jclass clazz, jlong javaScriptDialogPtr) {
  SprocketJavaScriptDialog* javascript_dialog =
    reinterpret_cast<SprocketJavaScriptDialog*>(javaScriptDialogPtr);
  javascript_dialog->Accept();
}

void AcceptPromptDialog(JNIEnv* env, jclass clazz, jlong javaScriptDialogPtr, jstring jprompt_text) {
  SprocketJavaScriptDialog* javascript_dialog =
    reinterpret_cast<SprocketJavaScriptDialog*>(javaScriptDialogPtr);
  const base::string16 prompt_text = ConvertJavaStringToUTF16(env, jprompt_text);
  javascript_dialog->Accept(prompt_text);
}
