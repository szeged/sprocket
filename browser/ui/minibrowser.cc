// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/browser/ui/minibrowser.h"

#include "base/auto_reset.h"
#include "base/message_loop/message_loop.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/renderer_preferences.h"
#include "minibrowser/browser/browser_main_parts.h"
#include "minibrowser/browser/content_browser_client.h"

const int kDefaultTestWindowWidthDip = 800;
const int kDefaultTestWindowHeightDip = 600;

std::vector<MiniBrowser*> MiniBrowser::windows_;
bool MiniBrowser::quit_message_loop_ = true;

MiniBrowser::MiniBrowser(content::WebContents* web_contents)
    : WebContentsObserver(web_contents),
      window_(NULL) {
  windows_.push_back(this);
}

MiniBrowser::~MiniBrowser() {
  PlatformCleanUp();

  for (size_t i = 0; i < windows_.size(); ++i) {
    if (windows_[i] == this) {
      windows_.erase(windows_.begin() + i);
      break;
    }
  }

  if (windows_.empty() && quit_message_loop_) {
    base::MessageLoop::current()->PostTask(FROM_HERE,
              base::MessageLoop::QuitClosure());
  }
}

// static
MiniBrowser* MiniBrowser::CreateMiniBrowser(content::WebContents* web_contents,
                const gfx::Size& initial_size) {
  MiniBrowser* minibrowser = new MiniBrowser(web_contents);
  minibrowser->PlatformCreateWindow(initial_size.width(), initial_size.height());

  minibrowser->web_contents_.reset(web_contents);
  web_contents->SetDelegate(minibrowser);

  minibrowser->PlatformSetContents();

  minibrowser->PlatformResizeSubViews();

  return minibrowser;
}

// static
void MiniBrowser::Initialize() {
  PlatformInitialize(GetMiniBrowserDefaultSize());
}

// static
gfx::Size MiniBrowser::AdjustWindowSize(const gfx::Size& initial_size) {
  if (!initial_size.IsEmpty())
    return initial_size;
  return GetMiniBrowserDefaultSize();
}

// static
MiniBrowser* MiniBrowser::CreateNewWindow(content::BrowserContext* browser_context,
                              const GURL& url,
                              content::SiteInstance* site_instance,
                              const gfx::Size& initial_size) {
  content::WebContents::CreateParams create_params(browser_context, site_instance);
  create_params.initial_size = AdjustWindowSize(initial_size);
  content::WebContents* web_contents = content::WebContents::Create(create_params);
  MiniBrowser* minibrowser = CreateMiniBrowser(web_contents, create_params.initial_size);
  if (!url.is_empty())
    minibrowser->LoadURL(url);
  return minibrowser;
}

void MiniBrowser::LoadURL(const GURL& url) {
  //LoadURLForFrame(url, std::string());
  content::NavigationController::LoadURLParams params(url);
  params.transition_type = ui::PageTransitionFromInt(
    ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
  web_contents_->GetController().LoadURLWithParams(params);
  web_contents_->Focus();
}

void MiniBrowser::AddNewContents(content::WebContents* source,
                           content::WebContents* new_contents,
                           WindowOpenDisposition disposition,
                           const gfx::Rect& initial_rect,
                           bool user_gesture,
                           bool* was_blocked) {
  CreateMiniBrowser(new_contents, AdjustWindowSize(initial_rect.size()));
}

bool MiniBrowser::CanGoBack() {
  return web_contents_->GetController().CanGoBack();
}

bool MiniBrowser::CanGoForward() {
  return web_contents_->GetController().CanGoForward();
}

void MiniBrowser::GoBackOrForward(int offset) {
  web_contents_->GetController().GoToOffset(offset);
  web_contents_->Focus();
}

void MiniBrowser::Reload() {
  web_contents_->GetController().Reload(false);
  web_contents_->Focus();
}

void MiniBrowser::Stop() {
  web_contents_->Stop();
  web_contents_->Focus();
}

void MiniBrowser::UpdateNavigationControls(bool to_different_document) {
  PlatformEnableUIControl(BACK_BUTTON, CanGoBack());
  PlatformEnableUIControl(FORWARD_BUTTON, CanGoForward());
  PlatformEnableUIControl(STOP_BUTTON,
      to_different_document && web_contents_->IsLoading());
}

gfx::NativeView MiniBrowser::GetContentView() {
  if (!web_contents_)
    return NULL;
  return web_contents_->GetNativeView();
}

content::WebContents* MiniBrowser::OpenURLFromTab(content::WebContents* source,
                                   const content::OpenURLParams& params) {
  // CURRENT_TAB is the only one we implement for now.
  if (params.disposition != CURRENT_TAB)
    return NULL;
  content::NavigationController::LoadURLParams load_url_params(params.url);
  load_url_params.source_site_instance = params.source_site_instance;
  load_url_params.referrer = params.referrer;
  load_url_params.frame_tree_node_id = params.frame_tree_node_id;
  load_url_params.transition_type = params.transition;
  load_url_params.extra_headers = params.extra_headers;
  load_url_params.should_replace_current_entry =
    params.should_replace_current_entry;

  load_url_params.is_renderer_initiated = params.is_renderer_initiated;
  if (params.transferred_global_request_id != content::GlobalRequestID()) {
    load_url_params.transferred_global_request_id =
      params.transferred_global_request_id;
  }

  source->GetController().LoadURLWithParams(load_url_params);
  return source;
}

void MiniBrowser::LoadingStateChanged(content::WebContents* source,
    bool to_different_document) {
  UpdateNavigationControls(to_different_document);
  PlatformSetIsLoading(source->IsLoading());
}

void MiniBrowser::CloseContents(content::WebContents* source) {
  Close();
}

bool MiniBrowser::CanOverscrollContent() const {
  return true;
}

void MiniBrowser::DidNavigateMainFramePostCommit(content::WebContents* web_contents) {
  PlatformSetAddressBarURL(web_contents->GetLastCommittedURL());
}

void MiniBrowser::ActivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Focus();
}

void MiniBrowser::DeactivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Blur();
}

bool MiniBrowser::HandleContextMenu(const content::ContextMenuParams& params) {
  return PlatformHandleContextMenu(params);
}

void MiniBrowser::WebContentsFocused(content::WebContents* contents) {
  PlatformWebContentsFocused(contents);
}

gfx::Size MiniBrowser::GetMiniBrowserDefaultSize() {
  static gfx::Size default_minibrowser_size;
  if (!default_minibrowser_size.IsEmpty())
    return default_minibrowser_size;

  default_minibrowser_size = gfx::Size(
    kDefaultTestWindowWidthDip, kDefaultTestWindowHeightDip);

  return default_minibrowser_size;
}

void MiniBrowser::TitleWasSet(content::NavigationEntry* entry, bool explicit_set) {
  if (entry)
    PlatformSetTitle(entry->GetTitle());
}
