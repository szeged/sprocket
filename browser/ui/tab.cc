// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/tab.h"

#include "base/strings/utf_string_conversions.h"
#include "sprocket/browser/web_contents.h"
#include "sprocket/browser/ui/tabbed_pane.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/label.h"

namespace {

const SkColor kTabTitleColor_Inactive = SkColorSetRGB(0x64, 0x64, 0x64);
const SkColor kTabTitleColor_Active = SK_ColorBLACK;
const SkColor kTabTitleColor_Hovered = SK_ColorBLACK;

}  // namespace

// static
const char Tab::kViewClassName[] = "Tab";
const char Tab::kEmptyTab[] = "Empty tab";

Tab::Tab(TabbedPane* tabbed_pane, SprocketWebContents* sprocket_web_contents, views::View* contents)
    : tabbed_pane_(tabbed_pane),
      title_(new views::Label(base::UTF8ToUTF16(kEmptyTab),
                       ui::ResourceBundle::GetSharedInstance().GetFontList(
                           ui::ResourceBundle::BoldFont))),
      tab_state_(TAB_ACTIVE),
      contents_(contents),
      sprocket_web_contents_(sprocket_web_contents) {
  // Calculate this now while the font list is guaranteed to be bold.
  preferred_title_size_ = title_->GetPreferredSize();
  sprocket_web_contents_->SetTab(this);
  SetState(TAB_INACTIVE);
  AddChildView(title_);

  close_button_ = new views::LabelButton(tabbed_pane, base::UTF8ToUTF16("×"));
  AddChildView(close_button_);
}

Tab::~Tab() {
  delete sprocket_web_contents_;
  delete close_button_;
  delete title_;
}

void Tab::SetTitle(const base::string16& title) {
  title_->SetText(title);
}

void Tab::SetSelected(bool selected) {
  contents_->SetVisible(selected);
  SetState(selected ? TAB_ACTIVE : TAB_INACTIVE);
}

bool Tab::OnMousePressed(const ui::MouseEvent& event) {
  if (event.IsOnlyLeftMouseButton() &&
      GetLocalBounds().Contains(event.location()))
    tabbed_pane_->SelectTab(this);
  else if (event.IsOnlyMiddleMouseButton() &&
      GetLocalBounds().Contains(event.location()))
    tabbed_pane_->CloseTab(this);
  return true;
}

void Tab::OnMouseEntered(const ui::MouseEvent& event) {
  SetState(selected() ? TAB_ACTIVE : TAB_HOVERED);
}

void Tab::OnMouseExited(const ui::MouseEvent& event) {
  SetState(selected() ? TAB_ACTIVE : TAB_INACTIVE);
}

gfx::Size Tab::GetPreferredSize() const {
  return gfx::Size(130, 25);
}

void Tab::Layout() {
  gfx::Rect local_bounds = GetLocalBounds();

  gfx::Rect close_button_bounds(close_button_->GetPreferredSize());
  close_button_bounds.Inset(0, 1, 0, 5);
  close_button_bounds.set_x(local_bounds.width() - close_button_bounds.width());
  close_button_->SetBoundsRect(close_button_bounds);

  gfx::Rect title_bounds(local_bounds);
  title_bounds.Inset(3, 1, close_button_bounds.width(), 0);
  title_bounds.ClampToCenteredSize(preferred_title_size_);
  title_->SetBoundsRect(title_bounds);
}

const char* Tab::GetClassName() const {
  return kViewClassName;
}

void Tab::SetState(TabState tab_state) {
  if (tab_state == tab_state_)
    return;
  tab_state_ = tab_state;

  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  switch (tab_state) {
    case TAB_INACTIVE:
      title_->SetEnabledColor(kTabTitleColor_Inactive);
      title_->SetFontList(rb.GetFontList(ui::ResourceBundle::BaseFont));
      break;
    case TAB_ACTIVE:
      title_->SetEnabledColor(kTabTitleColor_Active);
      title_->SetFontList(rb.GetFontList(ui::ResourceBundle::BoldFont));
      break;
    case TAB_HOVERED:
      title_->SetEnabledColor(kTabTitleColor_Hovered);
      title_->SetFontList(rb.GetFontList(ui::ResourceBundle::BaseFont));
      break;
  }
  SchedulePaint();
}
