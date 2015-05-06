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

SprocketContentBrowserClient* g_browser_client;

SprocketContentBrowserClient* SprocketContentBrowserClient::Get() {
  return g_browser_client;
}

class SprocketQuotaPermissionContext : public content::QuotaPermissionContext {
public:
  void RequestQuotaPermission(
    const content::StorageQuotaParams& params,
    int render_process_id,
    const PermissionCallback& callback) override
  {
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

content::BrowserMainParts*
SprocketContentBrowserClient::CreateBrowserMainParts(
  const content::MainFunctionParams& parameters) {
  browser_main_parts_ = new SprocketBrowserMainParts(parameters);
  return browser_main_parts_;
}

net::URLRequestContextGetter*
SprocketContentBrowserClient::CreateRequestContext(
  content::BrowserContext* browser_context,
  content::ProtocolHandlerMap* protocol_handlers,
  content::URLRequestInterceptorScopedVector request_interceptors) {
    SprocketBrowserContext* sprocket_browser_context = SprocketBrowserContextForBrowserContext(browser_context);
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
    content::FileDescriptorInfo* mappings) {
  int flags = base::File::FLAG_OPEN | base::File::FLAG_READ;
  base::FilePath pak_file;
  bool r = PathService::Get(base::DIR_ANDROID_APP_DATA, &pak_file);
  CHECK(r);
  pak_file = pak_file.Append(FILE_PATH_LITERAL("paks"));
  pak_file = pak_file.Append(FILE_PATH_LITERAL("sprocket.pak"));

  base::File f(pak_file, flags);
  if (!f.IsValid()) {
    NOTREACHED() << "Failed to open file when creating renderer process: "
                 << "sprocket.pak";
  }

  mappings->Transfer(kSprocketPakDescriptor, base::ScopedFD(f.TakePlatformFile()));
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

SprocketBrowserContext*
SprocketContentBrowserClient::browser_context() {
  return browser_main_parts_->browser_context();
}

SprocketBrowserContext*
SprocketContentBrowserClient::off_the_record_browser_context() {
  return browser_main_parts_->off_the_record_browser_context();
}

SprocketBrowserContext*
SprocketContentBrowserClient::SprocketBrowserContextForBrowserContext(
    content::BrowserContext* content_browser_context) {
  if (content_browser_context == browser_context())
    return browser_context();
  DCHECK_EQ(content_browser_context, off_the_record_browser_context());
  return off_the_record_browser_context();
}
