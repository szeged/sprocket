// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_
#define SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_

#if defined(USE_AURA)
#include "ui/views/window/dialog_delegate.h"

namespace views {
  class Label;
  class Textfield;
}
#endif

#include "url/gurl.h"

class SprocketResourceDispatcherHostLoginDelegate;

class SprocketAuthenticationDialog
#if defined(USE_AURA)
    : public views::DialogDelegateView
#endif
{
public:
  SprocketAuthenticationDialog(
      SprocketResourceDispatcherHostLoginDelegate* delegate,
      std::string realm,
      std::string host);
#if defined(USE_AURA)
  ~SprocketAuthenticationDialog() override;
#else
  ~SprocketAuthenticationDialog();
#endif

#if defined(USE_AURA)
  // Overridden from views::DialogDelegate:
  virtual base::string16 GetWindowTitle() const override;
  virtual bool Cancel() override;
  virtual bool Accept(bool window_closing) override;

  // Overridden from ui::DialogModel:
  virtual int GetDialogButtons() const override;
  virtual base::string16 GetDialogButtonLabel(ui::DialogButton button) const override;

  gfx::Size GetPreferredSize() const override;
#endif

private:

#if defined(USE_AURA)
  views::Label* label_;
  views::Textfield* username_prompt_;
  views::Textfield* password_prompt_;
#endif

  SprocketResourceDispatcherHostLoginDelegate* delegate_;

  DISALLOW_COPY_AND_ASSIGN(SprocketAuthenticationDialog);
};

#endif // SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_
