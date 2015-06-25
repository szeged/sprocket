// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_VIEWS_DELEGATE_AURA_H_
#define SPROCKET_BROWSER_UI_VIEWS_DELEGATE_AURA_H_

#include "ui/views/views_delegate.h"

class SprocketViewsDelegateAura : public views::ViewsDelegate {
 public:
  SprocketViewsDelegateAura();
  ~SprocketViewsDelegateAura() override;

  gfx::ImageSkia* GetDefaultWindowIcon() const override;

  void OnBeforeWidgetInit(
      views::Widget::InitParams* params,
      views::internal::NativeWidgetDelegate* delegate) override;

  bool WindowManagerProvidesTitleBar(bool maximized) override;

  ui::ContextFactory* GetContextFactory() override;
  std::string GetApplicationName() override;

  scoped_refptr<base::TaskRunner> GetBlockingPoolTaskRunner() override;

  DISALLOW_COPY_AND_ASSIGN(SprocketViewsDelegateAura);
};

#endif // SPROCKET_BROWSER_UI_VIEWS_DELEGATE_AURA_H_
