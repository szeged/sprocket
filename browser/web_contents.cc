// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/web_contents.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "sprocket/browser/ui/window.h"
#include "ui/events/event.h"

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
  window->PlatformSetContents(this);
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

void SprocketWebContents::Close() {
  window_->PlatformCloseWindow();
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

void SprocketWebContents::NavigationStateChanged(content::WebContents* source,
                                                 content::InvalidateTypes changed_flags) {
  if (changed_flags & content::INVALIDATE_TYPE_TITLE)
    window_->PlatformSetTitle(source->GetTitle());
}

void SprocketWebContents::AddNewContents(content::WebContents* source,
                                         content::WebContents* new_contents,
                                         WindowOpenDisposition disposition,
                                         const gfx::Rect& initial_rect,
                                         bool user_gesture,
                                         bool* was_blocked) {
  SprocketWindow* window = SprocketWindow::CreateNewWindow(gfx::Size(initial_rect.size()));
  AdoptWebContents(window, new_contents);
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
#if defined(USE_AURA)
  if (enter_fullscreen || !window_->PlatformWasFullscreenForTab())
#endif
    window_->PlatformToggleFullscreenModeForTab(enter_fullscreen);
  if (is_fullscreen_ != enter_fullscreen) {
    is_fullscreen_ = enter_fullscreen;
    web_contents->GetRenderViewHost()->WasResized();
  }
}

bool SprocketWebContents::IsFullscreenForTabOrPending(const content::WebContents* web_contents) const {
  return is_fullscreen_;
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

void SprocketWebContents::ActivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Focus();
}

void SprocketWebContents::DeactivateContents(content::WebContents* contents) {
  contents->GetRenderViewHost()->Blur();
}

void SprocketWebContents::HandleKeyboardEvent(content::WebContents* source,
                                              const content::NativeWebKeyboardEvent& event) {
#if defined(USE_AURA)
  if (event.os_event->type() == ui::ET_KEY_PRESSED)
    window_->PlatformHandleKeyboardEvent(event);
#endif
}
