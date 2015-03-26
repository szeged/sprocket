// Copyright (c) 2015 University of Szeged.
// Copyright (c) 2015 The Chromium Authors.
// All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sprocket/main_delegate.h"

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "content/public/browser/browser_main_runner.h"
#include "sprocket/browser/browser_main.h"
#include "sprocket/browser/content_browser_client.h"
#include "ui/base/resource/resource_bundle.h"


SprocketMainDelegate::SprocketMainDelegate() {
}

SprocketMainDelegate::~SprocketMainDelegate() {
}

bool SprocketMainDelegate::BasicStartupComplete(int* exit_code) {
  content::SetContentClient(&content_client_);
  return false;
}

void SprocketMainDelegate::PreSandboxStartup() {
  InitializeResourceBundle();
}

int SprocketMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {

  if (!process_type.empty())
      return -1;

    scoped_ptr<content::BrowserMainRunner> browser_runner_;
    browser_runner_.reset(content::BrowserMainRunner::Create());

    return BrowserMain(main_function_params, browser_runner_);
}

content::ContentBrowserClient*
SprocketMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new SprocketContentBrowserClient);
  return browser_client_.get();
}

// static
void SprocketMainDelegate::InitializeResourceBundle() {
  base::FilePath pak_file;
  base::FilePath pak_dir;

  PathService::Get(base::DIR_MODULE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("sprocket.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}
