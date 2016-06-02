// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/resource_dispatcher_host_delegate.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "net/url_request/url_request.h"
#include "sprocket/browser/ui/authentication_dialog.h"
#include "sprocket/browser/ui/window.h"
#include "sprocket/browser/web_contents.h"

SprocketResourceDispatcherHostLoginDelegate::SprocketResourceDispatcherHostLoginDelegate(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request)
    : url_(request->url()),
      realm_(auth_info->realm),
      host_(auth_info->challenger.ToString()),
      request_(request) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));

  // The IDs must be queried on IO thread
  content::ResourceRequestInfo::GetRenderFrameForRequest(
      request, &render_process_id_, &render_frame_id_);

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&SprocketResourceDispatcherHostLoginDelegate::TriggerDialog, this));
}

SprocketResourceDispatcherHostLoginDelegate::~SprocketResourceDispatcherHostLoginDelegate() {
}

void SprocketResourceDispatcherHostLoginDelegate::OnRequestCancelled() {
  request_ = 0;
}

void SprocketResourceDispatcherHostLoginDelegate::TriggerDialog() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  content::RenderViewHost* renderViewHost =
      content::RenderFrameHost::FromID(render_process_id_, render_frame_id_)->GetRenderViewHost();
  content::WebContents* web_contents = content::WebContents::FromRenderViewHost(renderViewHost);
  if (SprocketWebContents* sprocket_web_contents = SprocketWebContents::From(web_contents)) {
    SprocketWindow* window = sprocket_web_contents->window();
    window->PlatformShowAuthenticationDialog(
                new SprocketAuthenticationDialog(this, realm_, host_));
  }
}

void SprocketResourceDispatcherHostLoginDelegate::InputProvided(
    const base::string16& username,
    const base::string16& password) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&SprocketResourceDispatcherHostLoginDelegate::SetCredentials,
                  this,
                  username,
                  password));
}

void SprocketResourceDispatcherHostLoginDelegate::SetCredentials(
    const base::string16& username,
    const base::string16& password) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));

  if (username.length() + password.length() > 0)
    request_->SetAuth(net::AuthCredentials(username, password));
  else
    request_->CancelAuth();
  content::ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request_);

  request_ = 0;
}

content::ResourceDispatcherHostLoginDelegate*
SprocketResourceDispatcherHostDelegate::CreateLoginDelegate(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request) {
  return new SprocketResourceDispatcherHostLoginDelegate(auth_info, request);
}
