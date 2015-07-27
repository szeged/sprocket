// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_BROWSER_DEVTOOLS_FRONTEND_H_
#define SPROCKET_BROWSER_DEVTOOLS_FRONTEND_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "net/url_request/url_fetcher_delegate.h"

namespace base {
class Value;
}

namespace content {
class RenderViewHost;
class WebContents;
}

class SprocketWebContents;

class SprocketDevToolsFrontend : public content::WebContentsObserver,
                                 public content::DevToolsFrontendHost::Delegate,
                                 public content::DevToolsAgentHostClient,
                                 public net::URLFetcherDelegate {
 public:
  static SprocketDevToolsFrontend* Show(content::WebContents* inspected_contents);

  void Activate();
  void Focus();
  void InspectElementAt(int x, int y);
  void Close();

  void DisconnectFromTarget();

  SprocketWebContents* frontend_contents() const { return frontend_contents_; }

  void CallClientFunction(const std::string& function_name,
                          const base::Value* arg1,
                          const base::Value* arg2,
                          const base::Value* arg3);

 protected:
  SprocketDevToolsFrontend(SprocketWebContents* frontend_contents,
                           content::WebContents* inspected_contents);
  ~SprocketDevToolsFrontend() override;

  // content::DevToolsAgentHostClient implementation.
  void AgentHostClosed(content::DevToolsAgentHost* agent_host, bool replaced) override;
  void DispatchProtocolMessage(content::DevToolsAgentHost* agent_host,
                               const std::string& message) override;
  base::DictionaryValue* preferences() { return &preferences_; }

 private:
  // WebContentsObserver overrides
  void RenderViewCreated(content::RenderViewHost* render_view_host) override;
  void DocumentAvailableInMainFrame() override;
  void WebContentsDestroyed() override;

  // content::DevToolsFrontendHost::Delegate implementation.
  void HandleMessageFromDevToolsFrontend(const std::string& message) override;
  void HandleMessageFromDevToolsFrontendToBackend(
      const std::string& message) override;

  // net::URLFetcherDelegate overrides.
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  void SendMessageAck(int request_id,
                      const base::Value* arg1);

  SprocketWebContents* frontend_contents_;
  content::WebContents* inspected_contents_;
  scoped_refptr<content::DevToolsAgentHost> agent_host_;
  scoped_ptr<content::DevToolsFrontendHost> frontend_host_;
  using PendingRequestsMap = std::map<const net::URLFetcher*, int>;
  PendingRequestsMap pending_requests_;
  base::DictionaryValue preferences_;
  base::WeakPtrFactory<SprocketDevToolsFrontend> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(SprocketDevToolsFrontend);
};

#endif  // SPROCKET_BROWSER_DEVTOOLS_FRONTEND_H_
