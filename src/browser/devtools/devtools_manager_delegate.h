// Copyright (c) 2016 University of Szeged.
// Copyright (c) 2016 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_
#define SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_

#include "components/devtools_http_handler/devtools_http_handler_delegate.h"
#include "content/public/browser/devtools_manager_delegate.h"


namespace devtools_http_handler {
class DevToolsHttpHandler;
}

class SprocketDevToolsHttpHandlerDelegate
      : public devtools_http_handler::DevToolsHttpHandlerDelegate {
 public:
  static std::string host();
  static int port();
  static GURL discoveryPageURL();

  explicit SprocketDevToolsHttpHandlerDelegate();
  ~SprocketDevToolsHttpHandlerDelegate() override;

  // devtools_http_handler::DevToolsHttpHandlerDelegate implementation.
  std::string GetDiscoveryPageHTML() override;
  std::string GetFrontendResource(const std::string& path) override;
  std::string GetPageThumbnailData(const GURL& url) override;
  content::DevToolsExternalAgentProxyDelegate*
    HandleWebSocketConnection(const std::string& path) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SprocketDevToolsHttpHandlerDelegate);
};

class SprocketDevToolsManagerDelegate : public content::DevToolsManagerDelegate {
 public:
  static devtools_http_handler::DevToolsHttpHandler* CreateHttpHandler();

  explicit SprocketDevToolsManagerDelegate();
  ~SprocketDevToolsManagerDelegate() override;

  // DevToolsManagerDelegate implementation.
  void Inspect(content::BrowserContext* browser_context,
               content::DevToolsAgentHost* agent_host) override {}
  void DevToolsAgentStateChanged(content::DevToolsAgentHost* agent_host,
                                 bool attached) override {}
  base::DictionaryValue* HandleCommand(content::DevToolsAgentHost* agent_host,
                                       base::DictionaryValue* command) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SprocketDevToolsManagerDelegate);
};

#endif  // SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_
