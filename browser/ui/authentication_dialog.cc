// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/authentication_dialog.h"

#include "base/strings/utf_string_conversions.h"
#include "sprocket/browser/resource_dispatcher_host_delegate.h"

#if defined(USE_AURA)
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/box_layout.h"
#endif

SprocketAuthenticationDialog::SprocketAuthenticationDialog(
    SprocketResourceDispatcherHostLoginDelegate* delegate,
    std::string& realm,
    std::string& host)
    : delegate_(delegate) {
  message_ = base::ASCIIToUTF16("The server " + host + " requires a username and password. " +
             "The server says: " + realm + ".");
#if defined(USE_AURA)
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kVertical, 10, 10, 10));
  label_ = new views::Label;
  label_->SetMultiLine(true);
  label_->SetText(message_);
  AddChildView(label_);

  AddChildView(new views::Label(base::ASCIIToUTF16("Username:")));
  username_prompt_ = new views::Textfield;
  AddChildView(username_prompt_);
  AddChildView(new views::Label(base::ASCIIToUTF16("Password:")));
  password_prompt_ = new views::Textfield;
  password_prompt_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  AddChildView(password_prompt_);
#elif defined(OS_ANDROID)
  CreateJavaObject();
#endif
}

SprocketAuthenticationDialog::~SprocketAuthenticationDialog() {
}

#if defined(USE_AURA)

base::string16 SprocketAuthenticationDialog::GetWindowTitle() const {
  return base::ASCIIToUTF16("Authentication required");
}

bool SprocketAuthenticationDialog::Cancel() {
  delegate_->InputProvided(base::string16(), base::string16());
  return true;
}

bool SprocketAuthenticationDialog::Accept(bool window_closing) {
  if (window_closing)
    Cancel();
  else
    delegate_->InputProvided(username_prompt_->text(), password_prompt_->text());
  return true;
}

int SprocketAuthenticationDialog::GetDialogButtons() const {
  return ui::DIALOG_BUTTON_OK | ui::DIALOG_BUTTON_CANCEL;
}

base::string16 SprocketAuthenticationDialog::GetDialogButtonLabel(ui::DialogButton button) const {
  if (button == ui::DIALOG_BUTTON_OK)
    return base::ASCIIToUTF16("Login");
  else if (button == ui::DIALOG_BUTTON_CANCEL)
    return base::ASCIIToUTF16("Cancel");
  else
    return base::string16();
}

gfx::Size SprocketAuthenticationDialog::GetPreferredSize() const {
  const int width = 600;
  const int height = views::DialogDelegateView::GetPreferredSize().height()
                         + label_->GetHeightForWidth(width);
  return gfx::Size(width, height);
}

#endif
