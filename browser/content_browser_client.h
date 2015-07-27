// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_CONTENT_BROWSER_CLIENT_H_
#define SPROCKET_BROWSER_CONTENT_BROWSER_CLIENT_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"

class SprocketBrowserContext;
class SprocketBrowserMainParts;

// Embedder API (or SPI) for participating in browser logic, to be implemented
// by the client of the content browser. The methods are assumed to be called
// on the UI thread unless otherwise specified.
class SprocketContentBrowserClient : public content::ContentBrowserClient {
 public:

  static SprocketContentBrowserClient* Get();

  SprocketContentBrowserClient();
  ~SprocketContentBrowserClient() override;

  // ContentBrowserClient overrides.

  // Allows the embedder to set any number of custom BrowserMainParts
  // implementations for the browser startup code.
  content::BrowserMainParts* CreateBrowserMainParts(
      const content::MainFunctionParams& parameters) override;

  // Creates the main net::URLRequestContextGetter. Should only be called once
  // per SprocketContentBrowserClient object.
  net::URLRequestContextGetter* CreateRequestContext(
      content::BrowserContext* browser_context,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;

  // Returns whether a specified URL is handled by the embedder's internal
  // protocol handlers.
  bool IsHandledURL(const GURL& url) override;

  // Returns the default filename used in downloads when we have no idea what
  // else we should do with the file.
  std::string GetDefaultDownloadName() override;

  content::DevToolsManagerDelegate* GetDevToolsManagerDelegate() override;

  // Populates |mappings| with all files that need to be mapped before launching
  // a child process.
#if defined(OS_ANDROID)
  void GetAdditionalMappedFilesForChildProcess(
      const base::CommandLine& command_line,
      int child_process_id,
      content::FileDescriptorInfo* mappings,
      std::map<int, base::MemoryMappedFile::Region>* regions) override;
#endif

  // Create and return a new quota permission context.
  content::QuotaPermissionContext* CreateQuotaPermissionContext() override;



  SprocketBrowserContext* BrowserContext();
  SprocketBrowserContext* OffTheRecordBrowserContext();

  SprocketBrowserMainParts* browser_main_parts() { return browser_main_parts_; }

 private:
  SprocketBrowserContext* SprocketBrowserContextForBrowserContext(
      content::BrowserContext* content_browser_context);

  SprocketBrowserMainParts* browser_main_parts_;
};

#endif  // SPROCKET_BROWSER_CONTENT_BROWSER_CLIENT_H_
