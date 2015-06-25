// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_UI_TAB_H
#define SPROCKET_BROWSER_UI_TAB_H

#include "ui/views/view.h"

#include "base/strings/string16.h"

class TabbedPane;
class SprocketWebContents;

namespace views {
class Label;
class LabelButton;
}

// The tab view shown in the tab strip.
class Tab : public views::View {
 public:
  // Internal class name.
  static const char kViewClassName[];
  static const char kEmptyTab[];

  Tab(TabbedPane* tabbed_pane,
      SprocketWebContents* sprocket_web_contents,
      views::View* contents);
  ~Tab() override;

  void SetTitle(const base::string16& title);

  views::View* contents() const { return contents_; }
  SprocketWebContents* sprocket_web_contents() const { return sprocket_web_contents_; }

  bool selected() const { return contents_->visible(); }
  void SetSelected(bool selected);

  // Overridden from views::View:
  bool OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseEntered(const ui::MouseEvent& event) override;
  void OnMouseExited(const ui::MouseEvent& event) override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
  const char* GetClassName() const override;

 private:
  enum TabState {
    TAB_INACTIVE,
    TAB_ACTIVE,
    TAB_HOVERED,
  };

  void SetState(TabState tab_state);

  TabbedPane* tabbed_pane_;
  views::LabelButton* close_button_;
  views::Label* title_;
  gfx::Size preferred_title_size_;
  TabState tab_state_;
  // The content view associated with this tab.
  views::View* contents_;

  SprocketWebContents* sprocket_web_contents_;

  DISALLOW_COPY_AND_ASSIGN(Tab);
};

#endif // SPROCKET_BROWSER_UI_TAB_H
