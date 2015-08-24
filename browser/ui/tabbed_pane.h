// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_TABBED_PANE_H_
#define SPROCKET_BROWSER_UI_TABBED_PANE_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/strings/string16.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/view.h"
#include "ui/views/controls/webview/webview.h"
#include "sprocket/browser/ui/tabbed_pane_listener.h"

class Tab;
class TabbedPaneListener;
class TabStrip;
class SprocketWebContents;

// TabbedPane is a view that shows tabs. When the user clicks on a tab, the
// associated view is displayed.
class TabbedPane : public views::View,
                   public views::ButtonListener {
 public:
  // Internal class name.
  static const char kViewClassName[];

  TabbedPane();
  ~TabbedPane() override;

  TabbedPaneListener* listener() const { return listener_; }
  void set_listener(TabbedPaneListener* listener) { listener_ = listener; }

  int selected_tab_index() const { return selected_tab_index_; }

  views::View* contents() const {return contents_; }

  // Returns the number of tabs.
  int GetTabCount() const;

  // Returns the contents of the selected tab or NULL if there is none.
  Tab* GetSelectedTab();

  // Adds a SprocketWebContents at the end of this TabbedPane. |contents| is the
  // view displayed when the tab is selected and is owned by the TabbedPane.
  void AddTab(SprocketWebContents* sprocket_web_contents, views::WebView* contents);

  // Adds a SprocketWebContents at |index|. |contents| is the view displayed
  // when the tab is selected and is owned by the TabbedPane. If the tabbed pane
  // is currently empty, the new tab is selected.
  void AddTabAtIndex(int index, SprocketWebContents* sprocket_web_contents, views::WebView* contents);

  // Get the Tab (the tabstrip view, not its content) at the valid |index|.
  Tab* GetTabAt(int index);

  // Selects the tab at |index|, which must be valid.
  void SelectTabAt(int index);

  // Selects |tab| (the tabstrip view, not its content) if it is valid.
  void SelectTab(Tab* tab);

  void CloseTab(Tab* tab);

  // Overridden from View:
  gfx::Size GetPreferredSize() const override;
  const char* GetClassName() const override;

  // Overridden from ButtonListener:
  void ButtonPressed(views::Button* sender, const ui::Event&) override;

 private:
   friend class TabStrip;

   void InitAccelerators();
   void AddNewTabButton();

  // Overridden from View:
   void Layout() override;
   bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
   void PreferredSizeChanged() override;

  // A listener notified when tab selection changes. Weak, not owned.
  TabbedPaneListener* listener_;

  // The tab strip and contents container. The child indices of these members
  // correspond to match each Tab with its respective content View.
  TabStrip* tab_strip_;
  views::View* contents_;
  views::ScrollView* tab_scroll_view_;
  views::LabelButton* new_tab_button_;

  // The selected tab index or -1 if invalid.
  int selected_tab_index_;

  DISALLOW_COPY_AND_ASSIGN(TabbedPane);
};

#endif  // SPROCKET_BROWSER_UI_TABBED_PANE_H_
