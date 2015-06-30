// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_JAVASCRIPT_DIALOG_H_
#define SPROCKET_BROWSER_UI_JAVASCRIPT_DIALOG_H_

#if defined(USE_AURA)
#include "ui/views/window/dialog_delegate.h"

namespace views {
class Textfield;
}
#endif

#include "sprocket/browser/javascript_dialog_manager.h"

class SprocketJavaScriptDialog
#if defined(USE_AURA)
    : public views::DialogDelegateView
#endif
{
 public:
  SprocketJavaScriptDialog(
      content::JavaScriptMessageType type,
      const base::string16& message_text,
      const base::string16& default_prompt_text,
      const content::JavaScriptDialogManager::DialogClosedCallback& callback);
  ~SprocketJavaScriptDialog();

#if defined(USE_AURA)
  // Overridden from views::DialogDelegate:
  bool Cancel() override;
  bool Accept(bool window_closing) override;

  // Overridden from ui::DialogModel:
  int GetDialogButtons() const override;
#endif

 private:
  content::JavaScriptMessageType type_;
  base::string16 message_text_;
  base::string16 default_prompt_text_;
  content::JavaScriptDialogManager::DialogClosedCallback callback_;

#if defined(USE_AURA)
  views::Textfield* prompt_;
#endif

  DISALLOW_COPY_AND_ASSIGN(SprocketJavaScriptDialog);
};

#endif  // SPROCKET_BROWSER_UI_JAVASCRIPT_DIALOG_H_
