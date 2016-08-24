// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_
#define SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_

#include <string>
#include "base/strings/string16.h"
#include "build/build_config.h"

#if defined(USE_AURA)
#include "ui/views/window/dialog_delegate.h"

namespace views {
  class Label;
  class Textfield;
}
#elif defined(OS_ANDROID)
#include "base/android/scoped_java_ref.h"
#endif

class SprocketResourceDispatcherHostLoginDelegate;

class SprocketAuthenticationDialog
#if defined(USE_AURA)
    : public views::DialogDelegateView
#endif
{
public:
  SprocketAuthenticationDialog(
      SprocketResourceDispatcherHostLoginDelegate* delegate,
      std::string& realm,
      std::string& host);
#if defined(USE_AURA)
  ~SprocketAuthenticationDialog() override;
#else
  ~SprocketAuthenticationDialog();
#endif

#if defined(USE_AURA)
  // Overridden from views::DialogDelegate:
  base::string16 GetWindowTitle() const override;
  bool Cancel() override;
  bool Accept() override;

  // Overridden from ui::DialogModel:
  int GetDialogButtons() const override;
  base::string16 GetDialogButtonLabel(ui::DialogButton button) const override;

  gfx::Size GetPreferredSize() const override;
#elif defined(OS_ANDROID)
  void Cancel();
  void Accept(const base::string16& username, const base::string16& password);
  void CreateJavaObject();
  base::android::ScopedJavaLocalRef<jobject> GetJavaObject();
  static bool Register(JNIEnv* env);
#endif

private:

#if defined(USE_AURA)
  views::Label* label_;
  views::Textfield* username_prompt_;
  views::Textfield* password_prompt_;
#elif defined(OS_ANDROID)
  base::android::ScopedJavaGlobalRef<jobject> java_object_;
#endif

  SprocketResourceDispatcherHostLoginDelegate* delegate_;
  base::string16 message_;

  DISALLOW_COPY_AND_ASSIGN(SprocketAuthenticationDialog);
};

#endif // SPROCKET_BROWSER_UI_AUTHENTICATION_DIALOG_H_
