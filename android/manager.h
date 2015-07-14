// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_ANDROID_MANAGER_H_
#define SPROCKET_ANDROID_MANAGER_H_

#include <jni.h>

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"

class SprocketWindow;

namespace blink {
class WebLayer;
}

// Creates an Android specific Sprocket view, which is our version of a Sprocket
// window. This view holds the controls and content views necessary to
// render a Sprocket window. Returns the java object representing the sprocket view
// object.
jobject CreateSprocketWindow(SprocketWindow* sprocket_window);

// Removes a previously created sprocket view.
void RemoveSprocketWindow(jobject sprocket_window);

// Registers the SprocketManager native methods.
bool RegisterSprocketManager(JNIEnv* env);

void SprocketWindowAttachLayer(blink::WebLayer* layer);
void SprocketWindowRemoveLayer(blink::WebLayer* layer);

#endif  // SPROCKET_ANDROID_WEB_CONTENTS_MANAGER_H_
