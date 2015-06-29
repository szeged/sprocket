#include "sprocket/browser/ui/window_delegate_view_aura.h"
#include "url/gurl.h"

SprocketWindowDelegateView::SprocketWindowDelegateView()
    : toolbar_(new Toolbar),
      tabbed_pane_(new TabbedPane) {
}

SprocketWindowDelegateView::~SprocketWindowDelegateView() {
  delete tabbed_pane_;
  delete toolbar_;
}

void SprocketWindowDelegateView::AddTab(SprocketWebContents* sprocket_web_contents,
                                        const gfx::Size& size) {
  content::WebContents* web_contents = sprocket_web_contents->web_contents();
  gfx::Size prev_size;
  if (GetSelectedTab())
    prev_size = GetSelectedTab()->contents()->size();
  views::WebView* web_view = new views::WebView(web_contents->GetBrowserContext());
  web_view->SetWebContents(web_contents);
  web_view->SetPreferredSize(prev_size.IsEmpty() ? size : prev_size);
  web_contents->Focus();

  tabbed_pane_->set_listener((TabbedPaneListener *)this);
  tabbed_pane_->AddTab(sprocket_web_contents, web_view);
  Layout();

  // Resize the widget, keeping the same origin.
  gfx::Rect bounds = GetWidget()->GetWindowBoundsInScreen();
  bounds.set_size(GetWidget()->GetRootView()->GetPreferredSize());
  GetWidget()->SetBounds(bounds);
}

Tab* SprocketWindowDelegateView::GetTabAt(int index) {
  return tabbed_pane_->GetTabAt(index);
}

Tab* SprocketWindowDelegateView::GetSelectedTab() {
  return tabbed_pane_->GetSelectedTab();
}

void SprocketWindowDelegateView::SelectTabAt(int index) {
  tabbed_pane_->SelectTabAt(index);
}

void SprocketWindowDelegateView::SelectTab(Tab* tab) {
  tabbed_pane_->SelectTab(tab);
}

void SprocketWindowDelegateView::SetAddressBarURL(const GURL& url) {
  toolbar_->SetAddressBarURL(base::ASCIIToUTF16(url.spec()));
}

void SprocketWindowDelegateView::SetWindowTitle(const base::string16& title) {
  if (!title.length())
    title_ = base::UTF8ToUTF16(Tab::kEmptyTab);
  else
    title_ = title;
  tabbed_pane_->GetSelectedTab()->SetTitle(title_);
}

void SprocketWindowDelegateView::EnableUIControl(SprocketWindow::UIControl control, bool is_enabled) {
  toolbar_->EnableUIControl(control, is_enabled);
}

void SprocketWindowDelegateView::ShowWebContentsContextMenu(const content::ContextMenuParams& params) {
  gfx::Point screen_point(params.x, params.y);

  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->sprocket_web_contents();
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

  views::WebView* web_view = static_cast<views::WebView*>(GetSelectedTab()->contents());
  if (context_menu_runner_->RunMenuAt(web_view->GetWidget(),
                                      NULL,
                                      gfx::Rect(screen_point, gfx::Size()),
                                      views::MENU_ANCHOR_TOPRIGHT,
                                      ui::MENU_SOURCE_NONE) ==
      views::MenuRunner::MENU_DELETED) {
    return;
  }
}

void SprocketWindowDelegateView::TabSelectedAt(int index) {
  SprocketWebContents* sprocket_web_content = tabbed_pane_->GetSelectedTab()->sprocket_web_contents();
  SprocketWindow* sprocket_window = sprocket_web_content->window();

  sprocket_window->PlatformSetTitle(GetTabAt(index)->sprocket_web_contents()->web_contents()->GetTitle());
  sprocket_window->PlatformSetAddressBarURL(GetTabAt(index)->sprocket_web_contents()->web_contents()->GetVisibleURL());

  sprocket_window->PlatformEnableUIControl(SprocketWindow::BACK_BUTTON, sprocket_web_content->CanGoBack());
  sprocket_window->PlatformEnableUIControl(SprocketWindow::FORWARD_BUTTON, sprocket_web_content->CanGoForward());
  sprocket_window->PlatformEnableUIControl(SprocketWindow::REFRESH_STOP_BUTTON, sprocket_web_content->web_contents()->IsLoading());
}

void SprocketWindowDelegateView::LastTabClosed() {
  tabbed_pane_->GetSelectedTab()->sprocket_web_contents()->window()->Close();
}

void SprocketWindowDelegateView::OpenNewEmptyTab() {
  SprocketWebContents* sprocket_web_content = tabbed_pane_->GetSelectedTab()->sprocket_web_contents();
  content::WebContents* web_content = sprocket_web_content->web_contents();
  SprocketWindow* sprocket_window = sprocket_web_content->window();
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
    toolbar_->Init(this);
    layout->AddView(toolbar_);
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
      tabbed_pane_->GetSelectedTab()->sprocket_web_contents();
  delete sprocket_web_contents->window();
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
  if (details.is_add && details.child == this)
    InitSprocketWindow();
}

// Overridden from AcceleratorTarget:
bool SprocketWindowDelegateView::AcceleratorPressed(const ui::Accelerator& accelerator) {
  SprocketWebContents* sprocket_web_contents =
      tabbed_pane_->GetSelectedTab()->sprocket_web_contents();
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
