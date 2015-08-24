// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H
#define SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H

#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/ui/context_menu_model.h"
#include "sprocket/browser/ui/views_delegate_aura.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/ui/tab.h"
#include "sprocket/browser/ui/tabbed_pane.h"
#include "sprocket/browser/ui/tabbed_pane_listener.h"
#include "sprocket/browser/ui/toolbar.h"
#include "ui/aura/client/screen_position_client.h"
#include "ui/aura/env.h"
#include "ui/aura/window_tree_host.h"
#include "ui/gfx/screen.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/window/client_view.h"

namespace content {
struct NativeWebKeyboardEvent;
}

class Tab;
class TabbedPane;
class TabbedPaneListener;
class Toolbar;

// Maintain the UI controls and web view for sprocket
class SprocketWindowDelegateView : public views::WidgetDelegateView,
                                   public TabbedPaneListener {
 public:
  SprocketWindowDelegateView();

  ~SprocketWindowDelegateView() override;

  void AddTab(SprocketWebContents* sprocket_web_contents, const gfx::Size& size);

  Tab* GetTabAt(int index);
  Tab* GetSelectedTab();
  // Update the state of UI controls
  void SelectTabAt(int index);
  void SelectTab(Tab* tab);

  void SetAddressBarURL(const GURL& url);

  void SetWindowTitle(const base::string16& title);

  void EnableUIControl(SprocketWindow::UIControl control, bool is_enabled);

  void ShowWebContentsContextMenu(const content::ContextMenuParams& params);

  void SetFullscreen(bool fullscreen);
  void HandleKeyboardEvent(const content::NativeWebKeyboardEvent& event);

  //TabbedPaneListener override
  void TabSelectedAt(int index) override;
  void LastTabClosed() override;
  void OpenNewEmptyTab() override;

 private:
  // Initialize the UI control contained in sprocket window
  void InitSprocketWindow();

  void InitAccelerators();

  // Overridden from WidgetDelegateView
  bool CanResize()   const override;
  bool CanMaximize() const override;
  bool CanMinimize() const override;

  base::string16 GetWindowTitle() const override;

  void WindowClosing() override;

  View* GetContentsView() override;

  // Overridden from View
  gfx::Size GetMinimumSize() const override;
  void Layout() override;

  void ViewHierarchyChanged( const ViewHierarchyChangedDetails& details) override;

  // Overridden from AcceleratorTarget:
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;

  views::View* grid_view_;

  // Window title
  base::string16 title_;

  // Toolbar view contains forward/backward/reload button and URL entry
  Toolbar* toolbar_;

  // Tabbed pane contains the web view
  TabbedPane* tabbed_pane_;

  scoped_ptr<SprocketContextMenuModel> context_menu_model_;
  scoped_ptr<views::MenuRunner> context_menu_runner_;

  DISALLOW_COPY_AND_ASSIGN(SprocketWindowDelegateView);
};
#endif //SPROCKET_BROWSER_UI_WINDOW_DELEGATE_VIEW_AURA_H
