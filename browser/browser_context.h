// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_BROWSER_CONTEXT_H_
#define SPROCKET_BROWSER_BROWSER_CONTEXT_H_

#include "base/files/file_path.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/resource_context.h"
#include "sprocket/browser/net/url_request_context_getter.h"

class SprocketBrowserContext : public content::BrowserContext {
public:
  SprocketBrowserContext(bool off_the_record);
  ~SprocketBrowserContext() override;

  // BrowserContext implementation.
  base::FilePath GetPath() const override;
  scoped_ptr<content::ZoomLevelDelegate> CreateZoomLevelDelegate(
    const base::FilePath& partition_path) override;
  bool IsOffTheRecord() const override;
  net::URLRequestContextGetter* GetRequestContext() override;
  net::URLRequestContextGetter* GetRequestContextForRenderProcess(
    int renderer_child_id) override;
  net::URLRequestContextGetter* GetMediaRequestContext() override;
  net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
    int renderer_child_id) override;
  net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(
    const base::FilePath& partition_path,
    bool in_memory) override;
  content::ResourceContext* GetResourceContext() override;
  content::DownloadManagerDelegate* GetDownloadManagerDelegate() override;
  content::BrowserPluginGuestManager* GetGuestManager() override;
  storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;
  content::PushMessagingService* GetPushMessagingService() override;
  content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;
  content::PermissionManager* GetPermissionManager() override;

  net::URLRequestContextGetter* CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors);

protected:
  // Contains URLRequestContextGetter required for resource loading.
  class SprocketResourceContext : public content::ResourceContext {
  public:
    SprocketResourceContext();
    ~SprocketResourceContext() override;

    // ResourceContext implementation:
    net::HostResolver* GetHostResolver() override;
    net::URLRequestContext* GetRequestContext() override;

    void set_url_request_context_getter(SprocketURLRequestContextGetter* getter) {
    getter_ = getter;
  }

  private:
    SprocketURLRequestContextGetter* getter_;

    DISALLOW_COPY_AND_ASSIGN(SprocketResourceContext);
  };

  SprocketURLRequestContextGetter* url_request_context_getter() {
    return url_request_getter_.get();
  }

  // Used by SprocketBrowserContext to initiate and set different types of
  // URLRequestContextGetter.
  virtual SprocketURLRequestContextGetter* CreateURLRequestContextGetter(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors);
  void set_url_request_context_getter(SprocketURLRequestContextGetter* getter) {
    url_request_getter_ = getter;
  }

  bool ignore_certificate_errors() const { return ignore_certificate_errors_; }

  scoped_ptr<SprocketResourceContext> resource_context_;
  bool ignore_certificate_errors_;

private:
  void InitWhileIOAllowed();

  bool off_the_record_;
  base::FilePath path_;
  scoped_refptr<SprocketURLRequestContextGetter> url_request_getter_;

  DISALLOW_COPY_AND_ASSIGN(SprocketBrowserContext);
};

#endif // SPROCKET_BROWSER_BROWSER_CONTEXT_H_
