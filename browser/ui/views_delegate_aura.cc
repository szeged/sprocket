// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/views_delegate_aura.h"

#include "content/public/browser/browser_thread.h"
#include "ui/aura/env.h"
#include "ui/views/linux_ui/linux_ui.h"
#include "ui/views/widget/desktop_aura/desktop_native_widget_aura.h"

SprocketViewsDelegateAura::SprocketViewsDelegateAura() {
}

SprocketViewsDelegateAura::~SprocketViewsDelegateAura() {
}

gfx::ImageSkia* SprocketViewsDelegateAura::GetDefaultWindowIcon() const {
  // TODO add default icon
  return nullptr;
}

void SprocketViewsDelegateAura::OnBeforeWidgetInit(
    views::Widget::InitParams* params,
    views::internal::NativeWidgetDelegate* delegate) {
  if (params->opacity == views::Widget::InitParams::INFER_OPACITY)
    params->opacity = views::Widget::InitParams::OPAQUE_WINDOW;

  if (!params->native_widget)
    params->native_widget = new views::DesktopNativeWidgetAura(delegate);
}

bool SprocketViewsDelegateAura::WindowManagerProvidesTitleBar(bool maximized) {
  // On Ubuntu Unity, the system always provides a title bar for maximized
  // windows.
  views::LinuxUI* ui = views::LinuxUI::instance();
  return maximized && ui && ui->UnityIsRunning();
}

ui::ContextFactory* SprocketViewsDelegateAura::GetContextFactory() {
  if (aura::Env::GetInstance())
    return aura::Env::GetInstance()->context_factory();
  return nullptr;
}

std::string SprocketViewsDelegateAura::GetApplicationName() {
  return "Sprocket";
}

scoped_refptr<base::TaskRunner>
SprocketViewsDelegateAura::GetBlockingPoolTaskRunner() {
  return content::BrowserThread::GetBlockingPool();
}
