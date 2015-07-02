// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/tabbed_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "sprocket/browser/ui/tab.h"
#include "sprocket/browser/web_contents.h"
#include "ui/accessibility/ax_view_state.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font_list.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/layout_manager.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/scroll_view.h"

namespace {

const SkColor kTabBorderColor = SkColorSetRGB(0xC8, 0xC8, 0xC8);
const SkScalar kTabBorderThickness = 1.0f;

} // namespace

// static
const char TabbedPane::kViewClassName[] = "TabbedPane";

// The tab strip shown above the tab contents.
class TabStrip : public views::View {
 public:
  // Internal class name.
  static const char kViewClassName[];

  explicit TabStrip(TabbedPane* tabbed_pane);
  ~TabStrip() override;

  // Overridden from views::View:
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
  const char* GetClassName() const override;
  void OnPaint(gfx::Canvas* canvas) override;

 private:
  TabbedPane* tabbed_pane_;

  DISALLOW_COPY_AND_ASSIGN(TabStrip);
};

// static
const char TabStrip::kViewClassName[] = "TabStrip";

TabStrip::TabStrip(TabbedPane* tabbed_pane) : tabbed_pane_(tabbed_pane) {
}

TabStrip::~TabStrip() {
}

gfx::Size TabStrip::GetPreferredSize() const {
  gfx::Size size;
  for (int i = 0; i < child_count(); ++i) {
    const gfx::Size child_size = child_at(i)->GetPreferredSize();
    size.SetSize(size.width() + child_size.width(),
                 std::max(size.height(), child_size.height()));
  }
  return size;
}

void TabStrip::Layout() {
  const int kTabOffset = 9;
  int x = kTabOffset;  // Layout tabs with an offset to the tabstrip border.
  for (int i = 0; i < child_count(); ++i) {
    gfx::Size ps = child_at(i)->GetPreferredSize();
    child_at(i)->SetBounds(x, 0, ps.width(), ps.height());
    x = child_at(i)->bounds().right();
  }
}

const char* TabStrip::GetClassName() const {
  return kViewClassName;
}

void TabStrip::OnPaint(gfx::Canvas* canvas) {
  OnPaintBackground(canvas);

  // Draw the TabStrip border.
  SkPaint paint;
  paint.setColor(kTabBorderColor);
  paint.setStrokeWidth(kTabBorderThickness);
  SkScalar line_y = SkIntToScalar(height()) - (kTabBorderThickness / 2);
  SkScalar line_end = SkIntToScalar(width());
  int selected_tab_index = tabbed_pane_->selected_tab_index();
  if (selected_tab_index >= 0) {
    Tab* selected_tab = tabbed_pane_->GetTabAt(selected_tab_index);
    SkPath path;
    SkScalar tab_height =
        SkIntToScalar(selected_tab->height()) - kTabBorderThickness;
    SkScalar tab_width =
        SkIntToScalar(selected_tab->width()) - kTabBorderThickness;
    SkScalar tab_start = SkIntToScalar(selected_tab->GetMirroredX());
    path.moveTo(0, line_y);
    path.rLineTo(tab_start, 0);
    path.rLineTo(0, -tab_height);
    path.rLineTo(tab_width, 0);
    path.rLineTo(0, tab_height);
    path.lineTo(line_end, line_y);

    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(kTabBorderColor);
    paint.setStrokeWidth(kTabBorderThickness);
    canvas->DrawPath(path, paint);
  } else {
    canvas->sk_canvas()->drawLine(0, line_y, line_end, line_y, paint);
  }
}

TabbedPane::TabbedPane()
    : tab_strip_(new TabStrip(this)),
      contents_(new views::View()),
      tab_scroll_view_(new views::ScrollView),
      selected_tab_index_(-1) {
  SetFocusable(true);
  tab_scroll_view_->SetContents(tab_strip_);
  tab_scroll_view_->set_hide_horizontal_scrollbar(false);
  AddChildView(tab_scroll_view_);
  AddChildView(contents_);
  AddNewTabButton();
  InitAccelerators();
}

TabbedPane::~TabbedPane() {
  delete new_tab_button_;
  delete tab_strip_;
  delete tab_scroll_view_;
  delete contents_;
}

int TabbedPane::GetTabCount() const {
  return contents_->child_count();
}

Tab* TabbedPane::GetSelectedTab() {
  return selected_tab_index() < 0 ?
      NULL : GetTabAt(selected_tab_index());
}

void TabbedPane::AddTab(SprocketWebContents* sprocket_web_contents, views::WebView* contents) {
  AddTabAtIndex(GetTabCount(), sprocket_web_contents, contents);
}

void TabbedPane::AddTabAtIndex(int index,
                               SprocketWebContents* sprocket_web_contents,
                               views::WebView* contents) {
  DCHECK(index >= 0 && index <= GetTabCount());
  contents->SetVisible(false);

  tab_strip_->AddChildViewAt(new Tab(this, sprocket_web_contents, contents), index);
  contents_->AddChildViewAt(contents, index);
  if (selected_tab_index() < 0)
    SelectTabAt(index);

  PreferredSizeChanged();
}

void TabbedPane::SelectTabAt(int index) {
  DCHECK(index >= 0 && index < GetTabCount());

  if (selected_tab_index() >= 0 && selected_tab_index() < GetTabCount())
    GetTabAt(selected_tab_index())->SetSelected(false);
  selected_tab_index_ = index;
  Tab* tab = GetTabAt(index);
  tab->SetSelected(true);
  tab_strip_->SchedulePaint();

  views::FocusManager* focus_manager = tab->contents()->GetFocusManager();
  if (focus_manager) {
    const views::View* focused_view = focus_manager->GetFocusedView();
    if (focused_view && contents_->Contains(focused_view) &&
        !tab->contents()->Contains(focused_view))
      focus_manager->SetFocusedView(tab->contents());
  }

  if (listener())
    listener()->TabSelectedAt(index);
}

void TabbedPane::SelectTab(Tab* tab) {
  SelectTabAt(tab_strip_->GetIndexOf(tab));
}


void TabbedPane::CloseTab(Tab* tab) {
  if (GetTabCount() > 1) {
    int selected_index = selected_tab_index();
    int delete_index = tab_strip_->GetIndexOf(tab);
    contents_->RemoveChildView(tab->contents());
    tab_strip_->RemoveChildView(tab);
    Layout();

    if (selected_index == delete_index && GetTabCount() > selected_index)
      SelectTabAt(selected_index);
    else if (selected_index >= delete_index)
      SelectTabAt(selected_index - 1);
    delete tab;
  } else {
    if (listener())
      listener()->LastTabClosed();
  }
}

gfx::Size TabbedPane::GetPreferredSize() const {
  gfx::Size size;

  if (selected_tab_index() >= 0 && selected_tab_index() < GetTabCount())
    size = contents_->child_at(selected_tab_index())->GetPreferredSize();

  size.Enlarge(0, tab_strip_->GetPreferredSize().height());
  return size;
}

Tab* TabbedPane::GetTabAt(int index) {
  return static_cast<Tab*>(tab_strip_->child_at(index));
}


void TabbedPane::InitAccelerators() {
  AddAccelerator(ui::Accelerator(ui::VKEY_TAB,
      ui::EF_CONTROL_DOWN | ui::EF_SHIFT_DOWN));
  AddAccelerator(ui::Accelerator(ui::VKEY_TAB, ui::EF_CONTROL_DOWN));
  AddAccelerator(ui::Accelerator(ui::VKEY_T, ui::EF_CONTROL_DOWN));
}

void TabbedPane::AddNewTabButton() {
  new_tab_button_ = new views::LabelButton(this, base::UTF8ToUTF16("+"));
  tab_strip_->AddChildViewAt(new_tab_button_, GetTabCount());

  PreferredSizeChanged();
}

void TabbedPane::Layout() {
  const gfx::Size size = tab_strip_->GetPreferredSize();
  const int tabstrip_width = std::max(parent()->width(), size.width()) +
      tab_scroll_view_->GetScrollBarWidth();
  tab_strip_->SetBounds(0, 0, tabstrip_width, size.height());
  const int scroll_view_height = width() < size.width() ?  size.height() +
      tab_scroll_view_->GetScrollBarWidth() : size.height();
  const int scroll_view_width = width() < size.width() ? parent()->width() :
      parent()->width()  + tab_scroll_view_->GetScrollBarWidth() ;
  tab_scroll_view_->SetBounds(0, 0, scroll_view_width, scroll_view_height);
  tab_scroll_view_->Layout();

  contents_->SetBounds(0, tab_scroll_view_->bounds().bottom(), width(),
                       std::max(0, height() - scroll_view_height));
  for (int i = 0; i < contents_->child_count(); ++i)
    contents_->child_at(i)->SetSize(contents_->size());
}

bool TabbedPane::AcceleratorPressed(const ui::Accelerator& accelerator) {
  // Handle Ctrl+Tab and Ctrl+Shift+Tab navigation of pages.
  if (accelerator.key_code() == ui::VKEY_TAB && accelerator.IsCtrlDown()) {
    const int tab_count = GetTabCount();
    if (tab_count <= 1)
      return false;
    const int increment = accelerator.IsShiftDown() ? -1 : 1;
    int next_tab_index = (selected_tab_index() + increment) % tab_count;
    // Wrap around.
    if (next_tab_index < 0)
      next_tab_index += tab_count;
    SelectTabAt(next_tab_index);
  } else if (accelerator.key_code() == ui::VKEY_T && accelerator.IsCtrlDown()) {
    if (listener())
      listener()->OpenNewEmptyTab();
  }
  return true;
}

void TabbedPane::PreferredSizeChanged() {
  views::View::PreferredSizeChanged();
  if (GetTabCount() > 0) {
    gfx::Size current_size = contents_->child_at(GetTabCount() - 1)->GetPreferredSize();
    for (int i = 0; i < contents_->child_count(); ++i) {
      views::WebView* child_web_view = static_cast<views::WebView*>(contents_->child_at(i));
      child_web_view->SetPreferredSize(current_size);
    }
  }
}

const char* TabbedPane::GetClassName() const {
  return kViewClassName;
}

void TabbedPane::ButtonPressed(views::Button* sender, const ui::Event&) {
  if (sender == new_tab_button_ && listener())
    listener()->OpenNewEmptyTab();
  else if (sender != new_tab_button_)
    CloseTab(static_cast<Tab*>(sender->parent()));
}
