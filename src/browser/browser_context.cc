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
#include "content/public/browser/permission_manager.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "net/url_request/url_request_context.h"


SprocketBrowserContext::SprocketResourceContext::SprocketResourceContext()
    : getter_(NULL) {
}

SprocketBrowserContext::SprocketResourceContext::~SprocketResourceContext() {
}

net::HostResolver*
SprocketBrowserContext::SprocketResourceContext::GetHostResolver() {
  CHECK(getter_);
  return getter_->GetURLRequestContext()->host_resolver();
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

#if defined(OS_ANDROID)
  CHECK(PathService::Get(base::DIR_ANDROID_APP_DATA, &path_));
  path_ = path_.Append(FILE_PATH_LITERAL("sprocket"));
#else
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath config_dir(
      base::nix::GetXDGDirectory(env.get(),
                                 base::nix::kXdgConfigHomeEnvVar,
                                 base::nix::kDotConfigDir));
  path_ = config_dir.Append("sprocket");
#endif

  if (!base::PathExists(path_))
    base::CreateDirectory(path_);
  BrowserContext::Initialize(this, path_);
}

base::FilePath SprocketBrowserContext::GetPath() const {
  return path_;
}

std::unique_ptr<content::ZoomLevelDelegate>
SprocketBrowserContext::CreateZoomLevelDelegate(
    const base::FilePath& partition_path) {
  return std::unique_ptr<content::ZoomLevelDelegate>();
}

bool SprocketBrowserContext::IsOffTheRecord() const {
  return off_the_record_;
}

net::URLRequestContextGetter*
SprocketBrowserContext::CreateMediaRequestContext() {
  return url_request_getter_.get();
}

net::URLRequestContextGetter*
SprocketBrowserContext::CreateMediaRequestContextForStoragePartition(
    const base::FilePath& partition_path,
    bool in_memory) {
  return nullptr;
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

content::PermissionManager* SprocketBrowserContext::GetPermissionManager() {
  return NULL;
}

content::BackgroundSyncController* SprocketBrowserContext::GetBackgroundSyncController() {
  return NULL;
}

net::URLRequestContextGetter*
SprocketBrowserContext::CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) {
  if (!url_request_getter_.get())
    url_request_getter_ = new SprocketURLRequestContextGetter(
        ignore_certificate_errors_,
        GetPath(),
        protocol_handlers);
  resource_context_->set_url_request_context_getter(url_request_getter_.get());
  return url_request_getter_.get();
}

net::URLRequestContextGetter*
SprocketBrowserContext::CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) {
  if (!url_request_getter_.get())
    url_request_getter_ = new SprocketURLRequestContextGetter(
        ignore_certificate_errors_,
        GetPath(),
        protocol_handlers);
  return url_request_getter_.get();
}
