// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/ui/web_contents.h"

#include "base/auto_reset.h"
#include "base/message_loop/message_loop.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/renderer_preferences.h"
#include "sprocket/browser/browser_main_parts.h"
#include "sprocket/browser/content_browser_client.h"

const int kDefaultTestWindowWidthDip = 800;
const int kDefaultTestWindowHeightDip = 600;

std::vector<SprocketWebContents*> SprocketWebContents::windows_;
bool SprocketWebContents::quit_message_loop_ = true;

SprocketWebContents::SprocketWebContents(content::WebContents* web_contents)
    : WebContentsObserver(web_contents),
      window_(NULL) {
  windows_.push_back(this);
}

SprocketWebContents::~SprocketWebContents() {
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
SprocketWebContents* SprocketWebContents::CreateSprocketWebContents(content::WebContents* web_contents,
                const gfx::Size& initial_size) {
  SprocketWebContents* sprocket_web_contents = new SprocketWebContents(web_contents);
  sprocket_web_contents->PlatformCreateWindow(initial_size.width(), initial_size.height());

  sprocket_web_contents->web_contents_.reset(web_contents);
  web_contents->SetDelegate(sprocket_web_contents);

  sprocket_web_contents->PlatformSetContents();

  sprocket_web_contents->PlatformResizeSubViews();

  return sprocket_web_contents;
}

// static
void SprocketWebContents::Initialize() {
  PlatformInitialize(GetSprocketWebContentsDefaultSize());
}

// static
gfx::Size SprocketWebContents::AdjustWindowSize(const gfx::Size& initial_size) {
  if (!initial_size.IsEmpty())
    return initial_size;
  return GetSprocketWebContentsDefaultSize();
}

// static
SprocketWebContents* SprocketWebContents::CreateNewWindow(content::BrowserContext* browser_context,
                              const GURL& url,
                              content::SiteInstance* site_instance,
                              const gfx::Size& initial_size) {
  content::WebContents::CreateParams create_params(browser_context, site_instance);
  create_params.initial_size = AdjustWindowSize(initial_size);
  content::WebContents* web_contents = content::WebContents::Create(create_params);
  SprocketWebContents* sprocket_web_contents = CreateSprocketWebContents(web_contents, create_params.initial_size);
  if (!url.is_empty())
    sprocket_web_contents->LoadURL(url);
  return sprocket_web_contents;
}

void SprocketWebContents::LoadURL(const GURL& url) {
  //LoadURLForFrame(url, std::string());
  content::NavigationController::LoadURLParams params(url);
  params.transition_type = ui::PageTransitionFromInt(
    ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
  web_contents_->GetController().LoadURLWithParams(params);
  web_contents_->Focus();
}

void SprocketWebContents::AddNewContents(content::WebContents* source,
                           content::WebContents* new_contents,
                           WindowOpenDisposition disposition,
                           const gfx::Rect& initial_rect,
                           bool user_gesture,
                           bool* was_blocked) {
  CreateSprocketWebContents(new_contents, AdjustWindowSize(initial_rect.size()));
}

bool SprocketWebContents::CanGoBack() {
  return web_contents_->GetController().CanGoBack();
}

bool SprocketWebContents::CanGoForward() {
  return web_contents_->GetController().CanGoForward();
}

void SprocketWebContents::GoBackOrForward(int offset) {
  web_contents_->GetController().GoToOffset(offset);
  web_contents_->Focus();
}

void SprocketWebContents::Reload() {
  web_contents_->GetController().Reload(false);
  web_contents_->Focus();
}

void SprocketWebContents::Stop() {
  web_contents_->Stop();
  web_contents_->Focus();
}

void SprocketWebContents::UpdateNavigationControls(bool to_different_document) {
  PlatformEnableUIControl(BACK_BUTTON, CanGoBack());
  PlatformEnableUIControl(FORWARD_BUTTON, CanGoForward());
  PlatformEnableUIControl(STOP_BUTTON,
      to_different_document && web_contents_->IsLoading());
}

gfx::NativeView SprocketWebContents::GetContentView() {
  if (!web_contents_)
    return NULL;
  return web_contents_->GetNativeView();
}

content::WebContents* SprocketWebContents::OpenURLFromTab(content::WebContents* source,
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

void SprocketWebContents::LoadingStateChanged(content::WebContents* source,
    bool to_different_document) {
  UpdateNavigationControls(to_different_document);
  PlatformSetIsLoading(source->IsLoading());
}

void SprocketWebContents::CloseContents(content::WebContents* source) {
  Close();
}

bool SprocketWebContents::CanOverscrollContent() const {
  return true;
}

void SprocketWebContents::DidNavigateMainFramePostCommit(content::WebContents* web_contents) {
  PlatformSetAddressBarURL(web_contents->GetLastCommittedURL());
}

void SprocketWebContents::ActivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Focus();
}

void SprocketWebContents::DeactivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Blur();
}

bool SprocketWebContents::HandleContextMenu(const content::ContextMenuParams& params) {
  return PlatformHandleContextMenu(params);
}

void SprocketWebContents::WebContentsFocused(content::WebContents* contents) {
  PlatformWebContentsFocused(contents);
}

gfx::Size SprocketWebContents::GetSprocketWebContentsDefaultSize() {
  static gfx::Size default_sprocket_size;
  if (!default_sprocket_size.IsEmpty())
    return default_sprocket_size;

  default_sprocket_size = gfx::Size(
    kDefaultTestWindowWidthDip, kDefaultTestWindowHeightDip);

  return default_sprocket_size;
}

void SprocketWebContents::TitleWasSet(content::NavigationEntry* entry, bool explicit_set) {
  if (entry)
    PlatformSetTitle(entry->GetTitle());
}
