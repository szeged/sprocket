// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/browser/content_browser_client.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/quota_permission_context.h"
#include "content/public/common/content_descriptors.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "sprocket/browser/browser_context.h"
#include "sprocket/browser/browser_main_parts.h"
#include "net/url_request/url_request_context_getter.h"
#include "url/gurl.h"

#if !defined(USE_AURA)
#include "sprocket/browser/ui/web_contents_view_delegate.h"
#endif

#if defined(OS_ANDROID)
#include "base/android/path_utils.h"
#include "sprocket/android/descriptors.h"
#endif

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
#include "gin/v8_initializer.h"
#endif

SprocketContentBrowserClient* g_browser_client;

SprocketContentBrowserClient* SprocketContentBrowserClient::Get() {
  return g_browser_client;
}

class SprocketQuotaPermissionContext : public content::QuotaPermissionContext {
 public:
  void RequestQuotaPermission(
      const content::StorageQuotaParams& params,
      int render_process_id,
      const PermissionCallback& callback) override {
    callback.Run(QUOTA_PERMISSION_RESPONSE_DISALLOW);
  }

 private:
  ~SprocketQuotaPermissionContext() override {}
};


SprocketContentBrowserClient::SprocketContentBrowserClient()
    : browser_main_parts_(NULL) {
  DCHECK(!g_browser_client);
  g_browser_client = this;
}

SprocketContentBrowserClient::~SprocketContentBrowserClient() {
  g_browser_client = NULL;
}

content::BrowserMainParts* SprocketContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  browser_main_parts_ = new SprocketBrowserMainParts(parameters);
  return browser_main_parts_;
}

net::URLRequestContextGetter* SprocketContentBrowserClient::CreateRequestContext(
    content::BrowserContext* browser_context,
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  SprocketBrowserContext* sprocket_browser_context =
      SprocketBrowserContextForBrowserContext(browser_context);
  return sprocket_browser_context->CreateRequestContext(protocol_handlers);
}

bool SprocketContentBrowserClient::IsHandledURL(const GURL& url) {
  if (!url.is_valid()) {
    return false;
  }

  static const char* const kProtocolList[] = {
    url::kBlobScheme,
    url::kFileSystemScheme,
    url::kDataScheme,
    url::kFileScheme,
  };
  for (size_t i = 0; i < arraysize(kProtocolList); ++i) {
    if (url.scheme() == kProtocolList[i])
      return true;
  }
  return false;
}

std::string SprocketContentBrowserClient::GetDefaultDownloadName() {
  return "download";
}

#if defined(OS_ANDROID)
void SprocketContentBrowserClient::GetAdditionalMappedFilesForChildProcess(
    const base::CommandLine& command_line,
    int child_process_id,
    content::FileDescriptorInfo* mappings,
    std::map<int, base::MemoryMappedFile::Region>* regions) {
  mappings->Share(
      kSprocketPakDescriptor,
      base::GlobalDescriptors::GetInstance()->Get(kSprocketPakDescriptor));
  regions->insert(std::make_pair(
      kSprocketPakDescriptor,
      base::GlobalDescriptors::GetInstance()->GetRegion(kSprocketPakDescriptor)));
}
#endif

content::WebContentsViewDelegate* SprocketContentBrowserClient::GetWebContentsViewDelegate(
    content::WebContents* web_contents) {
#if !defined(USE_AURA)
  return CreateSprocketWebContentsViewDelegate(web_contents);
#else
  return NULL;
#endif
}

content::QuotaPermissionContext* SprocketContentBrowserClient::CreateQuotaPermissionContext() {
  // The browser crashes on quota permission requests if this returns NULL.
  return new SprocketQuotaPermissionContext;
}

void SprocketContentBrowserClient::ResourceDispatcherHostCreated() {
  resource_dispatcher_host_delegate_.reset(new SprocketResourceDispatcherHostDelegate);
  content::ResourceDispatcherHost::Get()->SetDelegate(resource_dispatcher_host_delegate_.get());
}

SprocketBrowserContext* SprocketContentBrowserClient::BrowserContext() {
  return browser_main_parts_->browser_context();
}

SprocketBrowserContext* SprocketContentBrowserClient::OffTheRecordBrowserContext() {
  return browser_main_parts_->off_the_record_browser_context();
}

SprocketBrowserContext* SprocketContentBrowserClient::SprocketBrowserContextForBrowserContext(
    content::BrowserContext* content_browser_context) {
  if (content_browser_context == BrowserContext())
    return BrowserContext();
  DCHECK_EQ(content_browser_context, OffTheRecordBrowserContext());
  return OffTheRecordBrowserContext();
}
