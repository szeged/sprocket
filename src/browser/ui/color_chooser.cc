// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/color_chooser.h"

#include "content/public/browser/web_contents.h"

SprocketColorChooser::SprocketColorChooser(content::WebContents* web_contents)
    : web_contents_(web_contents) {
}

SprocketColorChooser::~SprocketColorChooser() {
}

void SprocketColorChooser::OnColorChosen(SkColor color) {
  web_contents_->DidChooseColorInColorChooser(color);
}

void SprocketColorChooser::OnColorChooserDialogClosed() {
  web_contents_->DidEndColorChooser();
}
