// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minibrowser/minibrowser_main_delegate.h"

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "content/public/browser/browser_main_runner.h"
#include "minibrowser/browser/browser_main.h"
#include "minibrowser/browser/content_browser_client.h"
#include "ui/base/resource/resource_bundle.h"


MiniBrowserMainDelegate::MiniBrowserMainDelegate() {
}

MiniBrowserMainDelegate::~MiniBrowserMainDelegate() {
}

bool MiniBrowserMainDelegate::BasicStartupComplete(int* exit_code) {
  content::SetContentClient(&content_client_);
  return false;
}

void MiniBrowserMainDelegate::PreSandboxStartup() {
  InitializeResourceBundle();
}

int MiniBrowserMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {

  if (!process_type.empty())
      return -1;

    scoped_ptr<content::BrowserMainRunner> browser_runner_;
    browser_runner_.reset(content::BrowserMainRunner::Create());

    return BrowserMain(main_function_params, browser_runner_);
}

content::ContentBrowserClient*
MiniBrowserMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new MiniBrowserContentBrowserClient);
  return browser_client_.get();
}

// static
void MiniBrowserMainDelegate::InitializeResourceBundle() {
  base::FilePath pak_file;
  base::FilePath pak_dir;

  PathService::Get(base::DIR_MODULE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("minibrowser.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}