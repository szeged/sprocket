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

class SprocketWebContents;

namespace blink {
class WebLayer;
}

// Creates an Android specific sprocket view, which is our version of a sprocket
// window.  This view holds the controls and content views necessary to
// render a sprocket window.  Returns the java object representing the sprocket view.
// object.
jobject CreateSprocketWebContentsView(SprocketWebContents* sprocket_web_contents);

// Removes a previously created sprocket view.
void RemoveSprocketWebContentsView(jobject sprocket_web_contents_view);

// Registers the SprocketManager native methods.
bool RegisterSprocketManager(JNIEnv* env);

void SprocketWebContentsAttachLayer(blink::WebLayer* layer);
void SprocketWebContentsRemoveLayer(blink::WebLayer* layer);

#endif  // SPROCKET_ANDROID_WEB_CONTENTS_MANAGER_H_
