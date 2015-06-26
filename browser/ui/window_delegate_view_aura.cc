#include "sprocket/browser/ui/window_delegate_view_aura.h"
#include "url/gurl.h"

SprocketWindowDelegateView::SprocketWindowDelegateView()
  : toolbar_view_(new View),
    tabbed_pane_(new TabbedPane) {
}

SprocketWindowDelegateView::~SprocketWindowDelegateView() {
  delete web_view_;
  delete tabbed_pane_;
  delete url_entry_;
  delete back_button_;
  delete forward_button_;
  delete refresh_button_;
  delete stop_button_;
  delete toolbar_view_;
}

void SprocketWindowDelegateView::AddTab(SprocketWebContents* sprocket_web_contents,
                    const gfx::Size& size) {
  content::WebContents* web_contents = sprocket_web_contents->web_contents();
  gfx::Size prev_size;
  if (web_view_)
     prev_size.SetSize(web_view_->width(), web_view_->height());
  web_view_ = new views::WebView(web_contents->GetBrowserContext());
  web_view_->SetWebContents(web_contents);
  web_view_->SetPreferredSize(prev_size.IsEmpty() ? size : prev_size);
  web_contents->Focus();

  tabbed_pane_->SetListener((TabbedPaneListener *)this);
  tabbed_pane_->AddTab(sprocket_web_contents, web_view_);
  Layout();

  // Resize the widget, keeping the same origin.
  gfx::Rect bounds = GetWidget()->GetWindowBoundsInScreen();
  bounds.set_size(GetWidget()->GetRootView()->GetPreferredSize());
  GetWidget()->SetBounds(bounds);
}

Tab* SprocketWindowDelegateView::GetTabAt(int index) {
  return tabbed_pane_->GetTabAt(index);
}

void SprocketWindowDelegateView::SelectTabAt(int index) {
  tabbed_pane_->SelectTabAt(index);
}

void SprocketWindowDelegateView::SelectTab(Tab* tab) {
  tabbed_pane_->SelectTab(tab);
}

void SprocketWindowDelegateView::SetAddressBarURL(const GURL& url) {
  url_entry_->SetText(base::ASCIIToUTF16(url.spec()));
}

void SprocketWindowDelegateView::SetWindowTitle(const base::string16& title) {
  if (!title.length())
    title_ = base::UTF8ToUTF16(Tab::kEmptyTab);
  else
    title_ = title;
  tabbed_pane_->GetSelectedTab()->SetTitle(title_);
}

void SprocketWindowDelegateView::EnableUIControl(SprocketWindow::UIControl control, bool is_enabled) {
  if (control == SprocketWindow::BACK_BUTTON) {
    back_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
        : views::CustomButton::STATE_DISABLED);
  } else if (control == SprocketWindow::FORWARD_BUTTON) {
    forward_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
        : views::CustomButton::STATE_DISABLED);
  } else if (control == SprocketWindow::STOP_BUTTON) {
    stop_button_->SetState(is_enabled ? views::CustomButton::STATE_NORMAL
        : views::CustomButton::STATE_DISABLED);
  }
}

void SprocketWindowDelegateView::ShowWebContentsContextMenu(const content::ContextMenuParams& params) {
  gfx::Point screen_point(params.x, params.y);

  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->web_contents();
  aura::Window* web_contents_window =
      sprocket_web_contents->web_contents()->GetNativeView();
  aura::Window* root_window = web_contents_window->GetRootWindow();
  aura::client::ScreenPositionClient* screen_position_client =
      aura::client::GetScreenPositionClient(root_window);
  if (screen_position_client) {
      screen_position_client->ConvertPointToScreen(web_contents_window,
              &screen_point);
  }

  context_menu_model_.reset(new SprocketContextMenuModel(sprocket_web_contents, params));
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

void SprocketWindowDelegateView::TabSelectedAt(int index) {
  SprocketWebContents* sprocket_web_content = tabbed_pane_->GetSelectedTab()->web_contents();
  SprocketWindow* sprocket_window = sprocket_web_content->Window();

  sprocket_window->PlatformSetTitle(GetTabAt(index)->web_contents()->web_contents()->GetTitle());
  sprocket_window->PlatformSetAddressBarURL(GetTabAt(index)->web_contents()->web_contents()->GetVisibleURL());

  sprocket_window->PlatformEnableUIControl(SprocketWindow::BACK_BUTTON, sprocket_web_content->CanGoBack());
  sprocket_window->PlatformEnableUIControl(SprocketWindow::FORWARD_BUTTON, sprocket_web_content->CanGoForward());
  sprocket_window->PlatformEnableUIControl(SprocketWindow::STOP_BUTTON, sprocket_web_content->web_contents()->IsLoading());
}

void SprocketWindowDelegateView::LastTabClosed() {
  tabbed_pane_->GetSelectedTab()->web_contents()->Window()->Close();
}

void SprocketWindowDelegateView::OpenNewEmptyTab() {
  SprocketWebContents* sprocket_web_content = tabbed_pane_->GetSelectedTab()->web_contents();
  content::WebContents* web_content = sprocket_web_content->web_contents();
  SprocketWindow* sprocket_window = sprocket_web_content->Window();
  SprocketWebContents::CreateSprocketWebContents(
    sprocket_window,
    web_content->GetBrowserContext(),
    GURL(),
    web_content->GetPreferredSize());
  sprocket_window->PlatformSelectTabAt(tabbed_pane_->GetTabCount() - 1);
}

void SprocketWindowDelegateView::InitSprocketWindow() {
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

  // Add tabbed pane as the second row
  {
    layout->StartRow(1, 0);
    tabbed_pane_->SetLayoutManager(new views::FillLayout());
    layout->AddView(tabbed_pane_);
  }

  layout->AddPaddingRow(0, 5);
  InitAccelerators();
}

void SprocketWindowDelegateView::InitAccelerators() {
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
void SprocketWindowDelegateView::ContentsChanged(views::Textfield* sender,
                     const base::string16& new_contents) {

}

bool SprocketWindowDelegateView::HandleKeyEvent(views::Textfield* sender,
                    const ui::KeyEvent& key_event) {
 if (sender == url_entry_ && key_event.key_code() == ui::VKEY_RETURN) {
   std::string text = base::UTF16ToUTF8(url_entry_->text());
   GURL url(text);
   if (!url.has_scheme()) {
     url = GURL(std::string("http://") + std::string(text));
     url_entry_->SetText(base::ASCIIToUTF16(url.spec()));
   }

   SprocketWebContents* sprocket_web_contents =
       tabbed_pane_->GetSelectedTab()->web_contents();
   sprocket_web_contents->LoadURL(url);

   return true;
 }
 return false;
}

// Overridden from ButtonListener
void SprocketWindowDelegateView::ButtonPressed(views::Button* sender, const ui::Event& event) {

  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->web_contents();

  if (sender == back_button_)
    sprocket_web_contents->GoBackOrForward(-1);
  else if (sender == forward_button_)
    sprocket_web_contents->GoBackOrForward(1);
  else if (sender == refresh_button_)
    sprocket_web_contents->Reload();
  else if (sender == stop_button_)
    sprocket_web_contents->Stop();
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
  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->web_contents();
  delete sprocket_web_contents->Window();
}

views::View* SprocketWindowDelegateView::GetContentsView() {
  return this;
}

gfx::Size SprocketWindowDelegateView::GetMinimumSize() const {
  // We want to be able to make the window smaller than its initial
  // (preferred) size.
  return gfx::Size();
}

void SprocketWindowDelegateView::Layout() {
  views::View::Layout();
}

void SprocketWindowDelegateView::ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this) {
    InitSprocketWindow();
  }
}

// Overridden from AcceleratorTarget:
bool SprocketWindowDelegateView::AcceleratorPressed(const ui::Accelerator& accelerator) {
  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->web_contents();
  switch (accelerator.key_code()) {
  case ui::VKEY_F5:
    sprocket_web_contents->Reload();
    return true;
  case ui::VKEY_BROWSER_BACK:
    sprocket_web_contents->GoBackOrForward(-1);
    return true;
  case ui::VKEY_BROWSER_FORWARD:
    sprocket_web_contents->GoBackOrForward(1);
    return true;
  default:
    return views::WidgetDelegateView::AcceleratorPressed(accelerator);
  }
}
