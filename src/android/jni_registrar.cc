// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/android/jni_registrar.h"

#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "sprocket/android/manager.h"
#include "sprocket/browser/web_contents.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/ui/javascript_dialog.h"
#include "sprocket/browser/ui/authentication_dialog.h"

namespace {

static base::android::RegistrationMethod kSprocketWindowRegistrationMethods[] = {
    {"SprocketWindow", SprocketWindow::Register},
    {"SprocketManager", RegisterSprocketManager},
    {"SprocketJavaScriptDialog", SprocketJavaScriptDialog::Register},
    {"SprocketAuthenticationDialog", SprocketAuthenticationDialog::Register},
};

}  // namespace

bool RegisterSprocketWindowJni(JNIEnv* env) {
  return RegisterNativeMethods(env, kSprocketWindowRegistrationMethods,
                               arraysize(kSprocketWindowRegistrationMethods));
}
