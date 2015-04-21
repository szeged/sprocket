// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/web_contents.h"

#include "base/command_line.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/context_factory.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/context_menu_params.h"
#include "sprocket/browser/ui/context_menu_model.h"
#include "ui/aura/window_tree_host.h"
#include "ui/aura/client/screen_position_client.h"
#include "ui/aura/env.h"
#include "ui/aura/window.h"
#include "ui/aura/window_event_dispatcher.h"
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/models/simple_menu_model.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/event.h"
#include "ui/gfx/screen.h"
#include "ui/views/background.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/button/menu_button_listener.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/test/desktop_test_views_delegate.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"

namespace {
// ViewDelegate implementation for aura sprocket
class SprocketViewsDelegateAura : public views::DesktopTestViewsDelegate {
public:
  SprocketViewsDelegateAura() : use_transparent_windows_(false) {
  }

  ~SprocketViewsDelegateAura() override {}

  void SetUseTransparentWindows(bool transparent) {
    use_transparent_windows_ = transparent;
  }

private:
  bool use_transparent_windows_;

  DISALLOW_COPY_AND_ASSIGN(SprocketViewsDelegateAura);
};

// Maintain the UI controls and web view for sprocket
class SprocketWindowDelegateView : public views::WidgetDelegateView,
                                      public views::TextfieldController,
                                      public views::ButtonListener {
public:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  SprocketWindowDelegateView(SprocketWebContents* sprocket_web_contents)
    : sprocket_web_contents_(sprocket_web_contents),
      toolbar_view_(new View),
      contents_view_(new View) {
  }
  ~SprocketWindowDelegateView() override {}

  // Update the state of UI controls
  void SetAddressBarURL(const GURL& url) {
    url_entry_->SetText(base::ASCIIToUTF16(url.spec()));
  }
  void SetWebContents(content::WebContents* web_contents, const gfx::Size& size) {
    contents_view_->SetLayoutManager(new views::FillLayout());
    web_view_ = new views::WebView(web_contents->GetBrowserContext());
    web_view_->SetWebContents(web_contents);
    web_view_->SetPreferredSize(size);
    web_contents->Focus();
    contents_view_->AddChildView(web_view_);
    Layout();

    // Resize the widget, keeping the same origin.
    gfx::Rect bounds = GetWidget()->GetWindowBoundsInScreen();
    bounds.set_size(GetWidget()->GetRootView()->GetPreferredSize());
    GetWidget()->SetBounds(bounds);
  }

  void SetWindowTitle(const base::string16& title) { title_ = title; }
  void EnableUIControl(UIControl control, bool is_enabled) {
    if (control == BACK_BUTTON) {
      back_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
          : views::CustomButton::STATE_DISABLED);
    } else if (control == FORWARD_BUTTON) {
      forward_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
          : views::CustomButton::STATE_DISABLED);
    } else if (control == STOP_BUTTON) {
      stop_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
          : views::CustomButton::STATE_DISABLED);
    }
  }

  void ShowWebViewContextMenu(const content::ContextMenuParams& params) {
    gfx::Point screen_point(params.x, params.y);

    // Convert from content coordinates to window coordinates.
    // This code copied from chrome_web_contents_view_delegate_views.cc
    aura::Window* web_contents_window =
        sprocket_web_contents_->web_contents()->GetNativeView();
    aura::Window* root_window = web_contents_window->GetRootWindow();
    aura::client::ScreenPositionClient* screen_position_client =
        aura::client::GetScreenPositionClient(root_window);
    if (screen_position_client) {
        screen_position_client->ConvertPointToScreen(web_contents_window,
                &screen_point);
    }

    context_menu_model_.reset(new SprocketContextMenuModel(sprocket_web_contents_, params));
    context_menu_runner_.reset(new views::MenuRunner(
        context_menu_model_.get(), views::MenuRunner::CONTEXT_MENU));

    if (context_menu_runner_->RunMenuAt(web_view_->GetWidget(),
                                        NULL,
                                        gfx::Rect(screen_point, gfx::Size()),
                                        views::MENU_ANCHOR_TOPRIGHT,
                                        ui::MENU_SOURCE_NONE) ==
        views::MenuRunner::MENU_DELETED) {
      return;
    }
  }

private:
  // Initialize the UI control contained in sprocket window
  void InitSprocketWindow() {
    set_background(views::Background::CreateStandardPanelBackground());

    views::GridLayout* layout = new views::GridLayout(this);
    SetLayoutManager(layout);

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 2);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 1,
                          views::GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 2);

    layout->AddPaddingRow(0, 2);

    // Add toolbar buttons and URL text field
    {
      layout->StartRow(0, 0);
      views::GridLayout* toolbar_layout = new views::GridLayout(toolbar_view_);
      toolbar_view_->SetLayoutManager(toolbar_layout);

      views::ColumnSet* toolbar_column_set =
          toolbar_layout->AddColumnSet(0);
      // Back button
      back_button_ = new views::LabelButton(this, base::ASCIIToUTF16("Back"));
      back_button_->SetStyle(views::Button::STYLE_BUTTON);
      gfx::Size back_button_size = back_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                    views::GridLayout::CENTER, 0,
                                    views::GridLayout::FIXED,
                                    back_button_size.width(),
                                    back_button_size.width() / 2);
      // Forward button
      forward_button_ =
          new views::LabelButton(this, base::ASCIIToUTF16("Forward"));
      forward_button_->SetStyle(views::Button::STYLE_BUTTON);
      gfx::Size forward_button_size = forward_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                    views::GridLayout::CENTER, 0,
                                    views::GridLayout::FIXED,
                                    forward_button_size.width(),
                                    forward_button_size.width() / 2);
      // Refresh button
      refresh_button_ =
          new views::LabelButton(this, base::ASCIIToUTF16("Refresh"));
      refresh_button_->SetStyle(views::Button::STYLE_BUTTON);
      gfx::Size refresh_button_size = refresh_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                    views::GridLayout::CENTER, 0,
                                    views::GridLayout::FIXED,
                                    refresh_button_size.width(),
                                    refresh_button_size.width() / 2);
      // Stop button
      stop_button_ = new views::LabelButton(this, base::ASCIIToUTF16("Stop"));
      stop_button_->SetStyle(views::Button::STYLE_BUTTON);
      gfx::Size stop_button_size = stop_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(views::GridLayout::CENTER,
                                    views::GridLayout::CENTER, 0,
                                    views::GridLayout::FIXED,
                                    stop_button_size.width(),
                                    stop_button_size.width() / 2);
      toolbar_column_set->AddPaddingColumn(0, 2);
      // URL entry
      url_entry_ = new views::Textfield();
      url_entry_->set_controller(this);
      toolbar_column_set->AddColumn(views::GridLayout::FILL,
                                    views::GridLayout::FILL, 1,
                                    views::GridLayout::USE_PREF, 0, 0);
      toolbar_column_set->AddPaddingColumn(0, 2);

      // Fill up the first row
      toolbar_layout->StartRow(0, 0);
      toolbar_layout->AddView(back_button_);
      toolbar_layout->AddView(forward_button_);
      toolbar_layout->AddView(refresh_button_);
      toolbar_layout->AddView(stop_button_);
      toolbar_layout->AddView(url_entry_);

      layout->AddView(toolbar_view_);
    }

    layout->AddPaddingRow(0, 5);

    // Add web contents view as the second row
    {
      layout->StartRow(1, 0);
      layout->AddView(contents_view_);
    }

    layout->AddPaddingRow(0, 5);

    InitAccelerators();
  }
  void InitAccelerators() {
    static const ui::KeyboardCode keys[] = { ui::VKEY_F5,
                                             ui::VKEY_BROWSER_BACK,
                                             ui::VKEY_BROWSER_FORWARD };
    for (size_t i = 0; i < arraysize(keys); ++i) {
      GetFocusManager()->RegisterAccelerator(
        ui::Accelerator(keys[i], ui::EF_NONE),
        ui::AcceleratorManager::kNormalPriority,
        this);
    }
  }
  // Overridden from TextfieldController
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override {}
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override {
   if (sender == url_entry_ && key_event.key_code() == ui::VKEY_RETURN) {
     std::string text = base::UTF16ToUTF8(url_entry_->text());
     GURL url(text);
     if (!url.has_scheme()) {
       url = GURL(std::string("http://") + std::string(text));
       url_entry_->SetText(base::ASCIIToUTF16(url.spec()));
     }
     sprocket_web_contents_->LoadURL(url);
     return true;
   }
   return false;
  }

  // Overridden from ButtonListener
  void ButtonPressed(views::Button* sender, const ui::Event& event) override {
    if (sender == back_button_)
      sprocket_web_contents_->GoBackOrForward(-1);
    else if (sender == forward_button_)
      sprocket_web_contents_->GoBackOrForward(1);
    else if (sender == refresh_button_)
      sprocket_web_contents_->Reload();
    else if (sender == stop_button_)
      sprocket_web_contents_->Stop();
  }

  // Overridden from WidgetDelegateView
  bool CanResize() const override { return true; }
  bool CanMaximize() const override { return true; }
  bool CanMinimize() const override { return true; }
  base::string16 GetWindowTitle() const override { return title_; }
  void WindowClosing() override {
    if (sprocket_web_contents_) {
      delete sprocket_web_contents_;
      sprocket_web_contents_ = NULL;
    }
  }
  View* GetContentsView() override { return this; }

  // Overridden from View
  gfx::Size GetMinimumSize() const override {
    // We want to be able to make the window smaller than its initial
    // (preferred) size.
    return gfx::Size();
  }
  void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) override {
    if (details.is_add && details.child == this) {
      InitSprocketWindow();
    }
  }

  // Overridden from AcceleratorTarget:
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override {
    switch (accelerator.key_code()) {
    case ui::VKEY_F5:
      sprocket_web_contents_->Reload();
      return true;
    case ui::VKEY_BROWSER_BACK:
      sprocket_web_contents_->GoBackOrForward(-1);
      return true;
    case ui::VKEY_BROWSER_FORWARD:
      sprocket_web_contents_->GoBackOrForward(1);
      return true;
    default:
      return views::WidgetDelegateView::AcceleratorPressed(accelerator);
    }
  }

  // Hold a reference of Sprocket for deleting it when the window is closing
  SprocketWebContents* sprocket_web_contents_;

  // Window title
  base::string16 title_;

  // Toolbar view contains forward/backward/reload button and URL entry
  View* toolbar_view_;
  views::LabelButton* back_button_;
  views::LabelButton* forward_button_;
  views::LabelButton* refresh_button_;
  views::LabelButton* stop_button_;
  views::Textfield* url_entry_;
  scoped_ptr<SprocketContextMenuModel> context_menu_model_;
  scoped_ptr<views::MenuRunner> context_menu_runner_;

  // Contents view contains the web contents view
  View* contents_view_;
  views::WebView* web_view_;

  DISALLOW_COPY_AND_ASSIGN(SprocketWindowDelegateView);
};

}  // namespace

views::ViewsDelegate* SprocketWebContents::views_delegate_ = NULL;

// static
void SprocketWebContents::PlatformInitialize(const gfx::Size& default_window_size) {
  gfx::Screen::SetScreenInstance(
      gfx::SCREEN_TYPE_NATIVE, views::CreateDesktopScreen());
  views_delegate_ = new SprocketViewsDelegateAura();
}

// static
void SprocketWebContents::PlatformExit() {
  delete views_delegate_;
  views_delegate_ = NULL;
  aura::Env::DeleteInstance();
}

void SprocketWebContents::PlatformCleanUp() {
}

void SprocketWebContents::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  SprocketWindowDelegateView* delegate_view =
    static_cast<SprocketWindowDelegateView*>(window_widget_->widget_delegate());
  if (control == BACK_BUTTON) {
    delegate_view->EnableUIControl(SprocketWindowDelegateView::BACK_BUTTON,
        is_enabled);
  } else if (control == FORWARD_BUTTON) {
    delegate_view->EnableUIControl(SprocketWindowDelegateView::FORWARD_BUTTON,
        is_enabled);
  } else if (control == STOP_BUTTON) {
    delegate_view->EnableUIControl(SprocketWindowDelegateView::STOP_BUTTON,
        is_enabled);
  }
}

void SprocketWebContents::PlatformSetAddressBarURL(const GURL& url) {
  SprocketWindowDelegateView* delegate_view =
    static_cast<SprocketWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->SetAddressBarURL(url);
}

void SprocketWebContents::PlatformSetIsLoading(bool loading) {
}

void SprocketWebContents::PlatformCreateWindow(int width, int height) {
  window_widget_ = new views::Widget;
  views::Widget::InitParams params;
  params.bounds = gfx::Rect(0, 0, width, height);
  params.delegate = new SprocketWindowDelegateView(this);
  window_widget_->Init(params);

  content_size_ = gfx::Size(width, height);

  window_ = window_widget_->GetNativeWindow();
  // Call ShowRootWindow on RootWindow created by WMTestHelper without
  // which XWindow owned by RootWindow doesn't get mapped.
  window_->GetHost()->Show();
  window_widget_->Show();
}

void SprocketWebContents::PlatformSetContents() {
  views::WidgetDelegate* widget_delegate = window_widget_->widget_delegate();
  SprocketWindowDelegateView* delegate_view =
      static_cast<SprocketWindowDelegateView*>(widget_delegate);
  delegate_view->SetWebContents(web_contents_.get(), content_size_);
}

void SprocketWebContents::PlatformResizeSubViews() {
}

void SprocketWebContents::Close() {
  window_widget_->CloseNow();
}

void SprocketWebContents::PlatformSetTitle(const base::string16& title) {
  SprocketWindowDelegateView* delegate_view =
    static_cast<SprocketWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->SetWindowTitle(title);
  window_widget_->UpdateWindowTitle();
}

bool SprocketWebContents::PlatformHandleContextMenu(
    const content::ContextMenuParams& params) {
  SprocketWindowDelegateView* delegate_view =
    static_cast<SprocketWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->ShowWebViewContextMenu(params);
  return true;
}
