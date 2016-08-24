// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_BROWSER_CONTEXT_H_
#define SPROCKET_BROWSER_BROWSER_CONTEXT_H_

#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/resource_context.h"
#include "sprocket/browser/net/url_request_context_getter.h"

// This class holds the context needed for a browsing session.
// It lives on the UI thread. All these methods must only be called on the UI
// thread.
class SprocketBrowserContext : public content::BrowserContext {
 public:
  SprocketBrowserContext(bool off_the_record);
  ~SprocketBrowserContext() override;

  // BrowserContext implementation.

  // Returns the path of the directory where this context's data is stored.
  base::FilePath GetPath() const override;

  // Creates a delegate to initialize a HostZoomMap and persist its information.
  // This is called during creation of each StoragePartition.
  scoped_ptr<content::ZoomLevelDelegate> CreateZoomLevelDelegate(
      const base::FilePath& partition_path) override;

  // Return whether this context is incognito. Default is false.
  bool IsOffTheRecord() const override;

  // Returns the request context information associated with this context. Call
  // this only on the UI thread, since it can send notifications that should
  // happen on the UI thread.
  net::URLRequestContextGetter* GetRequestContext() override;

  // Returns the default request context for media resources associated with
  // this context.
  net::URLRequestContextGetter* GetMediaRequestContext() override;

  // Returns the request context for media resources associated with this
  // context and renderer process.
  net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
      int renderer_child_id) override;
  net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory) override;

  // Returns the resource context.
  content::ResourceContext* GetResourceContext() override;

  // Returns the DownloadManagerDelegate for this context. This will be called
  // once per context. The embedder owns the delegate and is responsible for
  // ensuring that it outlives DownloadManager. It's valid to return nullptr.
  content::DownloadManagerDelegate* GetDownloadManagerDelegate() override;

  // Returns the guest manager for this context.
  content::BrowserPluginGuestManager* GetGuestManager() override;

  // Returns a special storage policy implementation, or nullptr.
  storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;

  // Returns a push messaging service. The embedder owns the service, and is
  // responsible for ensuring that it outlives RenderProcessHost. It's valid to
  // return nullptr.
  content::PushMessagingService* GetPushMessagingService() override;

  // Returns the SSL host state decisions for this context. The context may
  // return nullptr, implementing the default exception storage strategy.
  content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;

  // Returns the PermissionManager associated with that context if any, nullptr
  // otherwise.
  content::PermissionManager* GetPermissionManager() override;

  // Returns the BackgroundSyncController associated with that context if any,
  // nullptr otherwise.
  content::BackgroundSyncController* GetBackgroundSyncController() override;

  // Creates the main net::URLRequestContextGetter. It's called only once.
  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;

  // Creates the net::URLRequestContextGetter for a StoragePartition. It's
  // called only once per partition_path.
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;

 protected:

  // ResourceContext contains the relevant context information required for
  // resource loading. It lives on the IO thread, although it is constructed on
  // the UI thread. It must be destructed on the IO thread.
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

  void set_url_request_context_getter(SprocketURLRequestContextGetter* getter) {
    url_request_getter_ = getter;
  }

  bool ignore_certificate_errors() const {
    return ignore_certificate_errors_;
  }

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
