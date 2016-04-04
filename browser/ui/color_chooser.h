// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_COLOR_CHOOSER_H_
#define SPROCKET_BROWSER_UI_COLOR_CHOOSER_H_

#include "build/build_config.h"
#include "content/public/browser/color_chooser.h"
#include "ui/views/color_chooser/color_chooser_listener.h"

namespace content {
  class WebContents;
}

class SprocketColorChooser
    : public content::ColorChooser
    , public views::ColorChooserListener
{
public:
  SprocketColorChooser(content::WebContents* web_contents);
  ~SprocketColorChooser() override;

  // content::ColorChooser implementation

  // Ends connection with color chooser. Closes color chooser depending on the
  // platform.
  void End() override {}

  // Sets the selected color.
  void SetSelectedColor(SkColor color) override {}


  // views::ColorChooserListener implementation

  void OnColorChosen(SkColor color) override;
  void OnColorChooserDialogClosed() override;

private:
  content::WebContents* web_contents_;
};

#endif // SPROCKET_BROWSER_UI_COLOR_CHOOSER_H_
