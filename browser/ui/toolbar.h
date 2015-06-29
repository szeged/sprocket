// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_TOOLBAR_H_
#define SPROCKET_BROWSER_UI_TOOLBAR_H_

#include "base/strings/utf_string_conversions.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/ui/window_delegate_view_aura.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/layout/grid_layout.h"

#include "url/gurl.h"

class SprocketWindowDelegateView;

class Toolbar : public views::View,
                public views::TextfieldController,
                public views::ButtonListener {
 public:
  Toolbar();
  ~Toolbar() override;

  void Init(SprocketWindowDelegateView* widget_delegate);

  void SetAddressBarURL(const base::string16& new_text);
  void EnableUIControl(SprocketWindow::UIControl control, bool is_enabled);

 private:

  // Overridden from TextfieldController
  bool HandleKeyEvent(views::Textfield* sender, const ui::KeyEvent& key_event) override;

  // Overridden from ButtonListener
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  SprocketWindowDelegateView* widget_delegate_;
  views::Textfield* url_entry_;
  views::LabelButton* back_button_;
  views::LabelButton* forward_button_;
  views::LabelButton* refresh_stop_button_;

};

#endif // SPROCKET_BROWSER_UI_TOOLBAR_H_