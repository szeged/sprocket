// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_BROWSER_CONTENT_BROWSER_CLIENT_H_
#define MINI_BROWSER_BROWSER_CONTENT_BROWSER_CLIENT_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"

class MiniBrowserBrowserContext;
class MiniBrowserBrowserMainParts;

class MiniBrowserContentBrowserClient : public content::ContentBrowserClient {
public:

  MiniBrowserContentBrowserClient();
  ~MiniBrowserContentBrowserClient() override;

  // ContentBrowserClient overrides.
  content::BrowserMainParts* CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) override;
  net::URLRequestContextGetter* CreateRequestContext(
    content::BrowserContext* browser_context,
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) override;
  bool IsHandledURL(const GURL& url) override;
  std::string GetDefaultDownloadName() override;
  void GetAdditionalMappedFilesForChildProcess(
    const base::CommandLine& command_line,
    int child_process_id,
    content::FileDescriptorInfo* mappings) override;

  MiniBrowserBrowserContext* browser_context();
  MiniBrowserBrowserContext* off_the_record_browser_context();

  MiniBrowserBrowserMainParts* browser_main_parts() {
    return browser_main_parts_;
  }

private:
  MiniBrowserBrowserContext* MiniBrowserBrowserContextForBrowserContext(
    content::BrowserContext* content_browser_context);

  base::ScopedFD v8_natives_fd_;
  base::ScopedFD v8_snapshot_fd_;

  MiniBrowserBrowserMainParts* browser_main_parts_;
};

#endif  // MINI_BROWSER_BROWSER_CONTENT_BROWSER_CLIENT_H_