// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_BROWSER_BROWSER_MAIN_PARTS_H_
#define MINI_BROWSER_BROWSER_BROWSER_MAIN_PARTS_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_main_parts.h"
#include "content/public/common/main_function_params.h"
#include "minibrowser/browser/browser_context.h"

class MiniBrowserBrowserMainParts : public content::BrowserMainParts {
public:
  explicit MiniBrowserBrowserMainParts(
    const content::MainFunctionParams& parameters);
  ~MiniBrowserBrowserMainParts() override;

  MiniBrowserBrowserContext* browser_context() {
    return browser_context_.get();
  }
  MiniBrowserBrowserContext* off_the_record_browser_context() {
    return off_the_record_browser_context_.get();
  }

  // content::BrowserMainParts overrides.
  void PreEarlyInitialization() override;
  void PreMainMessageLoopStart() override;
  void PostMainMessageLoopStart() override;
  void PreMainMessageLoopRun() override;
  bool MainMessageLoopRun(int* result_code) override;
  void PostMainMessageLoopRun() override;

protected:
  virtual void InitializeBrowserContexts();
  virtual void InitializeMessageLoopContext();

  void set_browser_context(MiniBrowserBrowserContext* context) {
    browser_context_.reset(context);
  }
  void set_off_the_record_browser_context(MiniBrowserBrowserContext* context) {
    off_the_record_browser_context_.reset(context);
  }

private:
  scoped_ptr<MiniBrowserBrowserContext> browser_context_;
  scoped_ptr<MiniBrowserBrowserContext> off_the_record_browser_context_;

  const content::MainFunctionParams parameters_;
  bool run_message_loop_;

  DISALLOW_COPY_AND_ASSIGN(MiniBrowserBrowserMainParts);
};

#endif // MINI_BROWSER_BROWSER_BROWSER_MAIN_PARTS_H_