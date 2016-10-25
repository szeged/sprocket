// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SPROCKET_APP_MAIN_DELEGATE_H_
#define SPROCKET_APP_MAIN_DELEGATE_H_

#include "base/memory/ptr_util.h"
#include "content/public/app/content_main_delegate.h"
#include "sprocket/common/content_client.h"

class SprocketContentBrowserClient;
class SprocketContentRendererClient;

#if defined(OS_ANDROID)
namespace content {
class BrowserMainRunner;
}
#endif

// The embedder has a chance to customize startup using the ContentMainDelegate interface.
class SprocketMainDelegate : public content::ContentMainDelegate {

 public:
  SprocketMainDelegate();
  ~SprocketMainDelegate() override;

 protected:
  // content::ContentMainDelegate implementation:

  // Tells the embedder that the absolute basic startup has been done, i.e.
  // it's now safe to create singletons and check the command line. Return true
  // if the process should exit afterwards, and if so, |exit_code| should be
  // set. This is the place for embedder to do the things that must happen at
  // the start. Most of its startup code should be in the methods below.
  bool BasicStartupComplete(int* exit_code) override;

  // Asks the embedder to start a process. Return -1 for the default behavior.
  int RunProcess(
      const std::string& process_type,
      const content::MainFunctionParams& main_function_params) override;

  // Called once per relevant process type to allow the embedder to customize
  // content. If an embedder wants the default (empty) implementation, don't
  // override this.
  content::ContentBrowserClient* CreateContentBrowserClient() override;
  // content::ContentPluginClient* CreateContentPluginClient() override;
  content::ContentRendererClient* CreateContentRendererClient() override;
  // content::ContentUtilityClient* CreateContentUtilityClient() override;

  static void InitializeResourceBundle();
 private:
  // The embedder API for participating in browser logic.
  std::unique_ptr<SprocketContentBrowserClient> browser_client_;
  // The embedder API for participating in renderer logic.
  std::unique_ptr<SprocketContentRendererClient> renderer_client_;

  SprocketContentClient content_client_;

#if defined(OS_ANDROID)
  std::unique_ptr<content::BrowserMainRunner> browser_runner_;
#endif

  DISALLOW_COPY_AND_ASSIGN(SprocketMainDelegate);
};

#endif  // SPROCKET_APP_MAIN_DELEGATE_H_
