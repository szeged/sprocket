// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_BROWSER_BROWSER_CONTEXT_H_
#define MINI_BROWSER_BROWSER_BROWSER_CONTEXT_H_

#include "base/files/file_path.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/resource_context.h"
#include "minibrowser/browser/net/url_request_context_getter.h"

class MiniBrowserBrowserContext : public content::BrowserContext {
public:
  MiniBrowserBrowserContext(bool off_the_record);
  ~MiniBrowserBrowserContext() override;

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
  net::URLRequestContextGetter* CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors);

protected:
  // Contains URLRequestContextGetter required for resource loading.
  class MiniBrowserResourceContext : public content::ResourceContext {
  public:
    MiniBrowserResourceContext();
    ~MiniBrowserResourceContext() override;

    // ResourceContext implementation:
    net::HostResolver* GetHostResolver() override;
    net::URLRequestContext* GetRequestContext() override;

    void set_url_request_context_getter(MiniBrowserURLRequestContextGetter* getter) {
    getter_ = getter;
  }

  private:
    MiniBrowserURLRequestContextGetter* getter_;

    DISALLOW_COPY_AND_ASSIGN(MiniBrowserResourceContext);
  };

  MiniBrowserURLRequestContextGetter* url_request_context_getter() {
    return url_request_getter_.get();
  }

  // Used by MiniBrowserBrowserContext to initiate and set different types of
  // URLRequestContextGetter.
  virtual MiniBrowserURLRequestContextGetter* CreateURLRequestContextGetter(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors);
  void set_url_request_context_getter(MiniBrowserURLRequestContextGetter* getter) {
    url_request_getter_ = getter;
  }

  bool ignore_certificate_errors() const { return ignore_certificate_errors_; }

  scoped_ptr<MiniBrowserResourceContext> resource_context_;
  bool ignore_certificate_errors_;

private:
  void InitWhileIOAllowed();

  bool off_the_record_;
  base::FilePath path_;
  scoped_refptr<MiniBrowserURLRequestContextGetter> url_request_getter_;

  DISALLOW_COPY_AND_ASSIGN(MiniBrowserBrowserContext);
};


#endif // MINI_BROWSER_BROWSER_BROWSER_CONTEXT_H_