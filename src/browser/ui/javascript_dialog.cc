// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/javascript_dialog.h"

#if defined(USE_AURA)
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/box_layout.h"

using namespace views;
#endif

using namespace base;

SprocketJavaScriptDialog::SprocketJavaScriptDialog(
    content::JavaScriptMessageType type,
    const string16& message_text,
    const string16& default_prompt_text,
    const content::JavaScriptDialogManager::DialogClosedCallback& callback)
        : type_(type),
          message_text_(message_text),
          default_prompt_text_(default_prompt_text),
          callback_(callback) {
#if defined(USE_AURA)
  SetLayoutManager(new BoxLayout(BoxLayout::kVertical, 10, 10, 10));
  AddChildView(new Label(message_text));

  if (type == content::JAVASCRIPT_MESSAGE_TYPE_PROMPT) {
    prompt_ = new Textfield;
    prompt_->SetText(default_prompt_text);
    AddChildView(prompt_);
  }
#elif defined(OS_ANDROID)
  CreateJavaObject();
#endif
}

SprocketJavaScriptDialog::~SprocketJavaScriptDialog() {
}

#if defined(USE_AURA)

bool SprocketJavaScriptDialog::Cancel() {
  callback_.Run(false, default_prompt_text_);
  return true;
}

bool SprocketJavaScriptDialog::Accept() {
  if (type_ == content::JAVASCRIPT_MESSAGE_TYPE_PROMPT)
    callback_.Run(true, prompt_->text());
  else
    callback_.Run(true, default_prompt_text_);
  return true;
}

int SprocketJavaScriptDialog::GetDialogButtons() const {
  switch(type_) {
    case content::JAVASCRIPT_MESSAGE_TYPE_ALERT:
      return ui::DIALOG_BUTTON_OK;
      break;
    case content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM:
    case content::JAVASCRIPT_MESSAGE_TYPE_PROMPT:
      return ui::DIALOG_BUTTON_OK | ui::DIALOG_BUTTON_CANCEL;
      break;
    default:
      return ui::DIALOG_BUTTON_NONE;
  }
}

#endif
