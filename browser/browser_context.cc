// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/browser_context.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/nix/xdg_util.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"

SprocketBrowserContext::SprocketResourceContext::SprocketResourceContext()
    : getter_(NULL) {
}

SprocketBrowserContext::SprocketResourceContext::~SprocketResourceContext() {
}

net::HostResolver*
SprocketBrowserContext::SprocketResourceContext::GetHostResolver() {
  CHECK(getter_);
  return getter_->host_resolver();
}

net::URLRequestContext*
SprocketBrowserContext::SprocketResourceContext::GetRequestContext() {
  CHECK(getter_);
  return getter_->GetURLRequestContext();
}

SprocketBrowserContext::SprocketBrowserContext(bool off_the_record)
  : resource_context_(new SprocketResourceContext),
    ignore_certificate_errors_(false),
    off_the_record_(off_the_record) {
  InitWhileIOAllowed();
}

SprocketBrowserContext::~SprocketBrowserContext() {
  if (resource_context_) {
    content::BrowserThread::DeleteSoon(
      content::BrowserThread::IO, FROM_HERE, resource_context_.release());
  }
}

void SprocketBrowserContext::InitWhileIOAllowed() {
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  if (cmd_line->HasSwitch(switches::kIgnoreCertificateErrors))
    ignore_certificate_errors_ = true;

  scoped_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath config_dir(
    base::nix::GetXDGDirectory(env.get(),
                   base::nix::kXdgConfigHomeEnvVar,
                   base::nix::kDotConfigDir));
  path_ = config_dir.Append("sprocket");
  if (!base::PathExists(path_))
    base::CreateDirectory(path_);
}

base::FilePath SprocketBrowserContext::GetPath() const {
  return path_;
}

scoped_ptr<content::ZoomLevelDelegate>
SprocketBrowserContext::CreateZoomLevelDelegate(
  const base::FilePath& partition_path) {
    return scoped_ptr<content::ZoomLevelDelegate>();
}

bool SprocketBrowserContext::IsOffTheRecord() const {
  return off_the_record_;
}

net::URLRequestContextGetter*
SprocketBrowserContext::GetRequestContext() {
  return GetDefaultStoragePartition(this)->GetURLRequestContext();
}

net::URLRequestContextGetter*
SprocketBrowserContext::GetRequestContextForRenderProcess(
  int renderer_child_id) {
    return GetRequestContext();
}

net::URLRequestContextGetter*
SprocketBrowserContext::GetMediaRequestContext() {
  return GetRequestContext();
}

net::URLRequestContextGetter*
SprocketBrowserContext::GetMediaRequestContextForRenderProcess(
  int renderer_child_id) {
    return GetRequestContext();
}

net::URLRequestContextGetter*
SprocketBrowserContext::GetMediaRequestContextForStoragePartition(
  const base::FilePath& partition_path,
  bool in_memory) {
    return GetRequestContext();
}

content::ResourceContext*
SprocketBrowserContext::GetResourceContext() {
  return resource_context_.get();
}

content::DownloadManagerDelegate*
SprocketBrowserContext::GetDownloadManagerDelegate() {
  return NULL;
}

content::BrowserPluginGuestManager*
SprocketBrowserContext::GetGuestManager() {
  return NULL;
}

storage::SpecialStoragePolicy*
SprocketBrowserContext::GetSpecialStoragePolicy() {
  return NULL;
}

content::PushMessagingService*
SprocketBrowserContext::GetPushMessagingService() {
  return NULL;
}

content::SSLHostStateDelegate*
SprocketBrowserContext::GetSSLHostStateDelegate() {
  return NULL;
}

SprocketURLRequestContextGetter*
SprocketBrowserContext::CreateURLRequestContextGetter(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  return new SprocketURLRequestContextGetter(
      ignore_certificate_errors_,
      GetPath(),
      content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::IO),
      content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::FILE),
      protocol_handlers,
      request_interceptors.Pass());
}

net::URLRequestContextGetter*
SprocketBrowserContext::CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  url_request_getter_ = CreateURLRequestContextGetter(
    protocol_handlers, request_interceptors.Pass());
  resource_context_->set_url_request_context_getter(url_request_getter_.get());
  return url_request_getter_.get();
}
