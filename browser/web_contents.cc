// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/web_contents.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/ui/window.h"
#if defined(USE_AURA)
#include "sprocket/browser/ui/tab.h"
#endif

// static
SprocketWebContents* SprocketWebContents::CreateSprocketWebContents(
    SprocketWindow* window,
    content::BrowserContext* browser_context,
    const GURL& url,
    const gfx::Size& initial_size) {
  content::WebContents::CreateParams create_params(browser_context, NULL);
  const gfx::Size& size = SprocketWindow::AdjustWindowSize(initial_size);
  create_params.initial_size = size;
  content::WebContents* web_contents = content::WebContents::Create(create_params);

  SprocketWebContents* sprocket_web_contents = new SprocketWebContents(window, web_contents);
  if (!url.is_empty())
    sprocket_web_contents->LoadURL(url);

  return sprocket_web_contents;
}

// static
SprocketWebContents* SprocketWebContents::AdoptWebContents(
    SprocketWindow* window,
    content::WebContents* web_contents) {
  return new SprocketWebContents(window, web_contents);
}

SprocketWebContents::SprocketWebContents(SprocketWindow* window,
                                         content::WebContents* web_contents)
    : window_(window),
    is_fullscreen_(false) {
  web_contents_.reset(web_contents);
  window->PlatformAddTab(this);
  web_contents->SetDelegate(this);
}

SprocketWebContents::~SprocketWebContents() {
}

void SprocketWebContents::LoadURL(const GURL& url) {
  content::NavigationController::LoadURLParams params(url);
  params.transition_type = ui::PageTransitionFromInt(
      ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
  web_contents_->GetController().LoadURLWithParams(params);
  web_contents_->Focus();
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

bool SprocketWebContents::IsLoading() {
  return web_contents_->IsLoading();
}

void SprocketWebContents::UpdateNavigationControls(bool to_different_document) {
#if defined(USE_AURA)
  if (tab_->selected())
#endif
  {
    window_->PlatformEnableUIControl(SprocketWindow::BACK_BUTTON, CanGoBack());
    window_->PlatformEnableUIControl(SprocketWindow::FORWARD_BUTTON, CanGoForward());
    window_->PlatformEnableUIControl(SprocketWindow::REFRESH_STOP_BUTTON,
        to_different_document && web_contents_->IsLoading());
  }
}

void SprocketWebContents::Close() {
  window_->PlatformCloseWindow();
}

content::WebContents* SprocketWebContents::OpenURLFromTab(content::WebContents* source,
                                   const content::OpenURLParams& params) {
  if (params.disposition == CURRENT_TAB) {
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
  } else if (params.disposition == NEW_BACKGROUND_TAB) {
    SprocketWebContents::CreateSprocketWebContents(
        window_,
        source->GetBrowserContext(),
        params.url,
        source->GetPreferredSize());
  } else if (params.disposition == NEW_FOREGROUND_TAB) {
#if defined(USE_AURA)
    SprocketWebContents* sprocket_web_contents =
#endif
    SprocketWebContents::CreateSprocketWebContents(
        window_,
        source->GetBrowserContext(),
        params.url,
        source->GetPreferredSize());
#if defined(USE_AURA)
    window_->PlatformSelectTab(sprocket_web_contents->tab());
#endif
  }

  return NULL;
}

void SprocketWebContents::NavigationStateChanged(content::WebContents* source,
                                                 content::InvalidateTypes changed_flags) {
#if defined(USE_AURA)
  if (tab_->selected() && changed_flags & content::INVALIDATE_TYPE_TITLE)
    window_->PlatformSetTitle(source->GetTitle());
  else
    tab_->SetTitle(source->GetTitle());
#elif defined(OS_ANDROID)
  if (changed_flags & content::INVALIDATE_TYPE_TITLE)
    window_->PlatformSetTitle(source->GetTitle());
#endif
}

void SprocketWebContents::AddNewContents(content::WebContents* source,
                                         content::WebContents* new_contents,
                                         WindowOpenDisposition disposition,
                                         const gfx::Rect& initial_rect,
                                         bool user_gesture,
                                         bool* was_blocked) {
  if (disposition == NEW_FOREGROUND_TAB) {
#if defined(USE_AURA)
    SprocketWebContents* adopted_web_contents =
#endif
        AdoptWebContents(window_, new_contents);
#if defined(USE_AURA)
    window_->PlatformSelectTab(adopted_web_contents->tab());
#endif
  } else if  (disposition == NEW_BACKGROUND_TAB) {
    AdoptWebContents(window_, new_contents);
  } else {
    SprocketWindow* window = SprocketWindow::CreateNewWindow(gfx::Size(initial_rect.size()));
    AdoptWebContents(window, new_contents);
  }
}

void SprocketWebContents::LoadingStateChanged(content::WebContents* source,
                                              bool to_different_document) {
  UpdateNavigationControls(to_different_document);
  window_->PlatformSetIsLoading(source->IsLoading());
}

void SprocketWebContents::LoadProgressChanged(content::WebContents* source, double progress) {
  window_->PlatformLoadProgressChanged(progress);
}

void SprocketWebContents::EnterFullscreenModeForTab(content::WebContents* web_contents,
                                      const GURL& origin) {
  ToggleFullscreenModeForTab(web_contents, true);
}

void SprocketWebContents::ExitFullscreenModeForTab(content::WebContents* web_contents) {
  ToggleFullscreenModeForTab(web_contents, false);
}

void SprocketWebContents::ToggleFullscreenModeForTab(content::WebContents* web_contents,
                                       bool enter_fullscreen) {
#if defined(OS_ANDROID)
  window_->PlatformToggleFullscreenModeForTab(web_contents, enter_fullscreen);
#endif
  if (is_fullscreen_ != enter_fullscreen) {
    is_fullscreen_ = enter_fullscreen;
    web_contents->GetRenderViewHost()->WasResized();
  }
}

bool SprocketWebContents::IsFullscreenForTabOrPending(const content::WebContents* web_contents) const {
#if defined(OS_ANDROID)
  return window_->PlatformIsFullscreenForTabOrPending(web_contents);
#else
  return is_fullscreen_;
#endif
}

void SprocketWebContents::CloseContents(content::WebContents* source) {
  Close();
}

bool SprocketWebContents::CanOverscrollContent() const {
#if defined(USE_AURA)
  return true;
#else
  return false;
#endif
}

void SprocketWebContents::DidNavigateMainFramePostCommit(content::WebContents* web_contents) {
#if defined(USE_AURA)
  if (tab_->selected())
#endif
    window_->PlatformSetAddressBarURL(web_contents->GetLastCommittedURL());
}

void SprocketWebContents::ActivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Focus();
}

void SprocketWebContents::DeactivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Blur();
}

bool SprocketWebContents::HandleContextMenu(const content::ContextMenuParams& params) {
  return window_->PlatformHandleContextMenu(params);
}
