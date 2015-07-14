// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H
#define SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H

#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/ui/views_delegate_aura.h"
#include "sprocket/browser/ui/window.h"
#include "ui/aura/client/screen_position_client.h"
#include "ui/aura/env.h"
#include "ui/aura/window_tree_host.h"
#include "ui/gfx/screen.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#include "ui/views/widget/widget_delegate.h"
#include "sprocket/browser/ui/window_delegate_view_aura.h"
#include "sprocket/browser/web_contents.h"

namespace content {
struct NativeWebKeyboardEvent;
}

// Maintain the UI controls and web view for sprocket
class SprocketWindowDelegateView : public views::WidgetDelegateView {
 public:
  SprocketWindowDelegateView();

  ~SprocketWindowDelegateView() override;

  void SetWebContents(SprocketWebContents* sprocket_web_contents,
                      const gfx::Size& size);

  void SetWindowTitle(const base::string16& title) { title_ = title; }
  void HandleKeyboardEvent(const content::NativeWebKeyboardEvent& event);

 private:
  void InitSprocketWindow();
  void InitAccelerators();

  // Overridden from WidgetDelegateView
  bool CanResize()   const override;
  bool CanMaximize() const override;
  bool CanMinimize() const override;

  base::string16 GetWindowTitle() const override;

  void WindowClosing() override;

  views::View* GetContentsView() override;

  // Overridden from View
  gfx::Size GetMinimumSize() const override;

  void ViewHierarchyChanged( const ViewHierarchyChangedDetails& details) override;
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;

  // Hold a reference of Sprocket for deleting it when the window is closing
  SprocketWebContents* sprocket_web_contents_;

  // Window title
  base::string16 title_;

  // Contents view contains the web contents view
  views::View* contents_view_;
  views::WebView* web_view_;

  DISALLOW_COPY_AND_ASSIGN(SprocketWindowDelegateView);
};

#endif //SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H
