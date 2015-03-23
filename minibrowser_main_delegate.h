// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_BROWSER_MAIN_DELEGATE_H_
#define MINI_BROWSER_MAIN_DELEGATE_H_

#include "base/memory/scoped_ptr.h"
#include "content/public/app/content_main_delegate.h"
#include "minibrowser/common/content_client.h"

class MiniBrowserContentBrowserClient;

class MiniBrowserMainDelegate : public content::ContentMainDelegate {

public:
  MiniBrowserMainDelegate();
  ~MiniBrowserMainDelegate() override;

protected:
  // content::ContentMainDelegate implementation:
  bool BasicStartupComplete(int* exit_code) override;
  void PreSandboxStartup() override;
  int RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) override;
  content::ContentBrowserClient* CreateContentBrowserClient() override;

  static void InitializeResourceBundle();
private:
  scoped_ptr<MiniBrowserContentBrowserClient> browser_client_;
  MiniBrowserContentClient content_client_;

  DISALLOW_COPY_AND_ASSIGN(MiniBrowserMainDelegate);
};

#endif  // MINI_BROWSER_MAIN_DELEGATE_H_