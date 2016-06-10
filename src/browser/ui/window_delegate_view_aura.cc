// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/window_delegate_view_aura.h"
#include "ui/content_accelerators/accelerator_util.h"
#include "url/gurl.h"

SprocketWindowDelegateView::SprocketWindowDelegateView()
    : sprocket_web_contents_(0),
      contents_view_(new View) {
}

SprocketWindowDelegateView::~SprocketWindowDelegateView() {
  delete web_view_;
}

void SprocketWindowDelegateView::SetWebContents(SprocketWebContents* sprocket_web_contents,
    const gfx::Size& size) {
  sprocket_web_contents_ = sprocket_web_contents;

  contents_view_->SetLayoutManager(new views::FillLayout());

  content::WebContents* web_contents = sprocket_web_contents->web_contents();
  web_view_ = new views::WebView(web_contents->GetBrowserContext());
  web_view_->SetWebContents(web_contents);
  web_view_->SetPreferredSize(size);
  web_contents->Focus();

  InitAccelerators();

  contents_view_->AddChildView(web_view_);
  Layout();

  // Resize the widget, keeping the same origin.
  gfx::Rect bounds = GetWidget()->GetWindowBoundsInScreen();
  bounds.set_size(GetWidget()->GetRootView()->GetPreferredSize());
  GetWidget()->SetBounds(bounds);
}

void SprocketWindowDelegateView::InitSprocketWindow() {
  set_background(views::Background::CreateStandardPanelBackground());

  views::FillLayout* layout = new views::FillLayout();
  SetLayoutManager(layout);

  AddChildView(contents_view_);
}

void SprocketWindowDelegateView::InitAccelerators() {
  GetFocusManager()->RegisterAccelerator(ui::Accelerator(ui::VKEY_F11, ui::EF_NONE),
      ui::AcceleratorManager::kNormalPriority, this);
}

bool SprocketWindowDelegateView::CanResize() const {
  return true;
}

bool SprocketWindowDelegateView::CanMaximize() const {
  return true;
}

bool SprocketWindowDelegateView::CanMinimize() const {
  return true;
}

base::string16 SprocketWindowDelegateView::GetWindowTitle() const {
  return title_;
}

void SprocketWindowDelegateView::WindowClosing() {
  delete sprocket_web_contents_->window();
}

views::View* SprocketWindowDelegateView::GetContentsView() {
  return this;
}

gfx::Size SprocketWindowDelegateView::GetMinimumSize() const {
  // We want to be able to make the window smaller than its initial
  // (preferred) size.
  return gfx::Size();
}

void SprocketWindowDelegateView::ViewHierarchyChanged(
    const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this)
    InitSprocketWindow();
}

void SprocketWindowDelegateView::HandleKeyboardEvent(const content::NativeWebKeyboardEvent& event) {
  AcceleratorPressed(ui::GetAcceleratorFromNativeWebKeyboardEvent(event));
}

bool SprocketWindowDelegateView::AcceleratorPressed(const ui::Accelerator& accelerator) {
  switch (accelerator.key_code()) {
  case ui::VKEY_F11:
    sprocket_web_contents_->window()->PlatformToggleFullscreenModeForTab(
        !sprocket_web_contents_->window()->PlatformIsFullscreenForTabOrPending());
    return true;
  default:
    return views::WidgetDelegateView::AcceleratorPressed(accelerator);
  }
}
