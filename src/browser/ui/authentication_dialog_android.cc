// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/authentication_dialog.h"

#include "sprocket/browser/resource_dispatcher_host_delegate.h"

#include "base/android/jni_string.h"
#include "jni/SprocketAuthenticationDialog_jni.h"

using base::android::AttachCurrentThread;
using base::android::ConvertUTF16ToJavaString;
using base::android::ConvertJavaStringToUTF16;

void SprocketAuthenticationDialog::Cancel() {
  delegate_->InputProvided(base::string16(), base::string16());
}

void SprocketAuthenticationDialog::Accept(const base::string16& username, const base::string16& password) {
  delegate_->InputProvided(username, password);
}

void SprocketAuthenticationDialog::CreateJavaObject() {
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jstring> j_message_ = ConvertUTF16ToJavaString(env, message_);
  java_object_.Reset(
    env,
    Java_SprocketAuthenticationDialog_create(
      env,
      j_message_.obj(),
      reinterpret_cast<intptr_t>(this)).obj());
}

base::android::ScopedJavaLocalRef<jobject> SprocketAuthenticationDialog::GetJavaObject() {
  return base::android::ScopedJavaLocalRef<jobject>(java_object_);
}

// static
bool SprocketAuthenticationDialog::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

void CancelAuthenticationDialog(JNIEnv* env, const JavaParamRef<jclass>& clazz, jlong authenticationDialogPtr) {
  SprocketAuthenticationDialog* authentication_dialog =
    reinterpret_cast<SprocketAuthenticationDialog*>(authenticationDialogPtr);
  authentication_dialog->Cancel();
}

void AcceptAuthenticationDialog(JNIEnv* env, const JavaParamRef<jclass>& clazz, jlong authenticationDialogPtr, const JavaParamRef<jstring>& jusername, const JavaParamRef<jstring>& jpassword) {
  SprocketAuthenticationDialog* authentication_dialog =
    reinterpret_cast<SprocketAuthenticationDialog*>(authenticationDialogPtr);
  const base::string16 username = ConvertJavaStringToUTF16(env, jusername);
  const base::string16 password = ConvertJavaStringToUTF16(env, jpassword);
  authentication_dialog->Accept(username, password);
}
