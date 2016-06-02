// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/toolbar.h"

#include "sprocket/browser/ui/window_delegate_view_aura.h"

Toolbar::Toolbar()
    : widget_delegate_(NULL),
      url_entry_(NULL),
      back_button_(NULL),
      forward_button_(NULL),
      refresh_stop_button_(NULL) {
}

Toolbar::~Toolbar() {
  delete url_entry_;
  delete back_button_;
  delete forward_button_;
  delete refresh_stop_button_;
}

void Toolbar::Init(SprocketWindowDelegateView* widget_delegate) {
  widget_delegate_ = widget_delegate;

  views::GridLayout* toolbar_layout = new views::GridLayout(this);
  SetLayoutManager(toolbar_layout);

  views::ColumnSet* toolbar_column_set =
      toolbar_layout->AddColumnSet(0);

  // Back button
  back_button_ = new views::LabelButton(this, base::ASCIIToUTF16("Back"));
  back_button_->SetStyle(views::Button::STYLE_BUTTON);
  gfx::Size back_button_size = back_button_->GetPreferredSize();
  toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                views::GridLayout::CENTER, 0,
                                views::GridLayout::FIXED,
                                back_button_size.width(),
                                back_button_size.width() / 2);
  // Forward button
  forward_button_ =
      new views::LabelButton(this, base::ASCIIToUTF16("Forward"));
  forward_button_->SetStyle(views::Button::STYLE_BUTTON);
  gfx::Size forward_button_size = forward_button_->GetPreferredSize();
  toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                views::GridLayout::CENTER, 0,
                                views::GridLayout::FIXED,
                                forward_button_size.width(),
                                forward_button_size.width() / 2);
  // Refresh/Stop button
  refresh_stop_button_ =
      new views::LabelButton(this, base::ASCIIToUTF16("Refresh"));
  refresh_stop_button_->SetStyle(views::Button::STYLE_BUTTON);
  gfx::Size refresh_stop_button_size =
      refresh_stop_button_->GetPreferredSize();
  toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                views::GridLayout::CENTER, 0,
                                views::GridLayout::FIXED,
                                refresh_stop_button_size.width(),
                                refresh_stop_button_size.width() / 2);

  toolbar_column_set->AddPaddingColumn(0, 2);
  // URL entry
  url_entry_ = new views::Textfield();
  url_entry_->set_controller(this);
  toolbar_column_set->AddColumn(views::GridLayout::FILL,
                                views::GridLayout::FILL, 1,
                                views::GridLayout::USE_PREF, 0, 0);
  toolbar_column_set->AddPaddingColumn(0, 2);

  // Fill up the first row
  toolbar_layout->StartRow(0, 0);
  toolbar_layout->AddView(back_button_);
  toolbar_layout->AddView(forward_button_);
  toolbar_layout->AddView(refresh_stop_button_);
  toolbar_layout->AddView(url_entry_);
}

bool Toolbar::HandleKeyEvent(views::Textfield* sender,
                             const ui::KeyEvent& key_event) {
  if (sender == url_entry_ && key_event.key_code() == ui::VKEY_RETURN) {
    std::string text = base::UTF16ToUTF8(url_entry_->text());
    GURL url(text);

    if (!url.has_scheme()) {
      url = GURL(std::string("http://") + std::string(text));
      url_entry_->SetText(base::ASCIIToUTF16(url.spec()));
    }

    SprocketWebContents* sprocket_web_contents =
        widget_delegate_->GetSelectedTab()->sprocket_web_contents();
    sprocket_web_contents->LoadURL(url);

    return true;
  }
  return false;
}

// Overridden from ButtonListener
void Toolbar::ButtonPressed(views::Button* sender, const ui::Event& event) {
  SprocketWebContents* sprocket_web_contents =
      widget_delegate_->GetSelectedTab()->sprocket_web_contents();

  if (sender == back_button_)
    sprocket_web_contents->GoBackOrForward(-1);
  else if (sender == forward_button_)
    sprocket_web_contents->GoBackOrForward(1);
  else if (sender == refresh_stop_button_ && sprocket_web_contents->IsLoading())
    sprocket_web_contents->Stop();
  else if (sender == refresh_stop_button_ && !sprocket_web_contents->IsLoading())
    sprocket_web_contents->Reload();
}

void Toolbar::EnableUIControl(SprocketWindow::UIControl control, bool is_enabled) {
  if (control == SprocketWindow::BACK_BUTTON) {
    back_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
        : views::CustomButton::STATE_DISABLED);
  } else if (control == SprocketWindow::FORWARD_BUTTON) {
    forward_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
        : views::CustomButton::STATE_DISABLED);
  } else if (control == SprocketWindow::REFRESH_STOP_BUTTON) {
    refresh_stop_button_->SetText(is_enabled ? base::ASCIIToUTF16("Stop")
        : base::ASCIIToUTF16("Refresh"));
  }
}

void Toolbar::SetAddressBarURL(const base::string16& new_text) {
  url_entry_->SetText(new_text);
}
