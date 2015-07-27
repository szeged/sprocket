// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_
#define SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "components/devtools_http_handler/devtools_http_handler_delegate.h"
#include "content/public/browser/devtools_manager_delegate.h"

namespace devtools_http_handler {
class DevToolsHttpHandler;
}

namespace content {
class BrowserContext;
}

class SprocketDevToolsManagerDelegate : public content::DevToolsManagerDelegate {
 public:
  static devtools_http_handler::DevToolsHttpHandler* CreateHttpHandler(
      content::BrowserContext* browser_context);

  explicit SprocketDevToolsManagerDelegate(content::BrowserContext* browser_context);
  ~SprocketDevToolsManagerDelegate() override;

  // DevToolsManagerDelegate implementation.
  void Inspect(content::BrowserContext* browser_context,
               content::DevToolsAgentHost* agent_host) override {}
  void DevToolsAgentStateChanged(content::DevToolsAgentHost* agent_host,
                                 bool attached) override {}
  base::DictionaryValue* HandleCommand(content::DevToolsAgentHost* agent_host,
                                       base::DictionaryValue* command) override;

 private:
  content::BrowserContext* browser_context_;

  DISALLOW_COPY_AND_ASSIGN(SprocketDevToolsManagerDelegate);
};

#endif  // SPROCKET_BROWSER_DEVTOOLS_MANAGER_DELEGATE_H_
